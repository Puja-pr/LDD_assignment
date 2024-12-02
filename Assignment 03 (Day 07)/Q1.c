#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kfifo.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/wait.h>

#define DEVCNT 4
#define MAX 32

typedef struct devinfo {
    struct kfifo mybuf;
    dev_t devn;
    struct cdev cdev;
    int id;
    wait_queue_head_t wr_wq;
    wait_queue_head_t rd_wq;
} devinfo_t;

// Device information array
static devinfo_t devices[DEVCNT];
// Device number
dev_t devno;
static int major = 250;
// Device class
static struct class *pclass;

static int pchar_open(struct inode *pinode, struct file *pfile) {
    devinfo_t *dev = container_of(pinode->i_cdev, devinfo_t, cdev);
    pfile->private_data = dev;
    pr_info("%s: pchar_open() called for pchar%d\n", THIS_MODULE->name, dev->id);
    return 0;
}

static int pchar_close(struct inode *pinode, struct file *pfile) {
    devinfo_t *dev = (devinfo_t *)pfile->private_data;
    pr_info("%s: pchar_close() called for pchar%d\n", THIS_MODULE->name, dev->id);
    return 0;
}

static ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t bufsize, loff_t *f_pos) {
    int nbytes, ret;
    devinfo_t *dev = (devinfo_t *)pfile->private_data;
    pr_info("%s: pchar_write() called for pchar%d\n", THIS_MODULE->name, dev->id);

    ret = wait_event_interruptible(dev->wr_wq, !kfifo_is_full(&dev->mybuf));
    if (ret != 0) {
        pr_err("%s: pchar_write() blocked for pchar%d, to up by giving a signal\n", THIS_MODULE->name, dev->id);
        return ret;  
    }

    ret = kfifo_from_user(&dev->mybuf, ubuf, bufsize, &nbytes);
    if (ret < 0) {
        pr_err("%s: pchar_write() failed for pchar%d\n", THIS_MODULE->name, dev->id); 
        return ret;
    }

    pr_err("%s: pchar_write() wrote %d bytes to pchar%d\n", THIS_MODULE->name, nbytes, dev->id); 
    if (nbytes > 0) {
        wake_up_interruptible(&dev->rd_wq);
        pr_info("%s: pchar_write woke up process for pchar%d\n", THIS_MODULE->name, dev->id);
    }
    return nbytes;
}

static ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t bufsize, loff_t *f_pos) {
    int nbytes, ret;
    devinfo_t *dev = (devinfo_t *)pfile->private_data;
    pr_info("%s: pchar_read() called for pchar%d\n", THIS_MODULE->name, dev->id); 

    ret = wait_event_interruptible(dev->rd_wq, !kfifo_is_empty(&dev->mybuf));
    if (ret != 0) {
        pr_err("%s: pchar_read() blocked for pchar%d, to up by giving a signal\n", THIS_MODULE->name, dev->id);
        return ret;  
    }

    ret = kfifo_to_user(&dev->mybuf, ubuf, bufsize, &nbytes);
    if (ret < 0) {
        pr_err("%s: pchar_read() failed for pchar%d\n", THIS_MODULE->name, dev->id); 
        return ret;
    }

    pr_err("%s: pchar_read() read %d bytes from pchar%d\n", THIS_MODULE->name, nbytes, dev->id); 
    if (nbytes > 0) {
        wake_up_interruptible(&dev->wr_wq);
        pr_info("%s: pchar_read woke up process for pchar%d\n", THIS_MODULE->name, dev->id);
    }
    return nbytes;
}

static struct file_operations f_ops = {
    .owner = THIS_MODULE,
    .open = pchar_open,
    .release = pchar_close,
    .write = pchar_write,
    .read = pchar_read,
};

static int __init pchar_multidev_init(void) {
    int ret, i;
    dev_t devnum;
    struct device * pdevice;
    pr_info("%s: pchar_multidev_init module called\n", THIS_MODULE->name);
    
    // Allocate device number
    ret = alloc_chrdev_region(&devno, 0, DEVCNT, "pchar");
    if (ret != 0) {
        pr_err("%s: alloc_chrdev_region() failed\n", THIS_MODULE->name);
        goto alloc_chardev_region_failed;
    }
    
    major = MAJOR(devno);
    pr_info("%s: alloc_chrdev_region() allocated device number: %d\n", THIS_MODULE->name, major);
    
    // Create device class
    pclass = class_create("pchar_class");
    if (IS_ERR(pclass)) {
        pr_err("%s: class_create() failed\n", THIS_MODULE->name); 
        goto class_create_failed;
    }  
    
    pr_info("%s: class_create() created device class\n", THIS_MODULE->name);
    
    // Create device files for multiple device instances
    for (i = 0; i < DEVCNT; i++) {
        devnum = MKDEV(major, i);
        pdevice = device_create(pclass, NULL, devnum, NULL, "pchar%d", i);
        if (IS_ERR(pdevice)) {
            pr_err("%s: device_create() failed pchar%d\n", THIS_MODULE->name, i);
            ret = -1;
            goto device_create_failed;
        }  
        pr_info("%s: device_create() created device file pchar%d\n", THIS_MODULE->name, i);
    }
    
    // Initialize cdev object for multiple device instances and load into kernel
    for (i = 0; i < DEVCNT; i++) {   
        devnum = MKDEV(major, i);
        devices[i].cdev.owner = THIS_MODULE;
        cdev_init(&devices[i].cdev, &f_ops);
        ret = cdev_add(&devices[i].cdev, devnum, 1);
        if (ret != 0) {
            pr_err("%s: cdev_add() failed pchar%d\n", THIS_MODULE->name, i);
            goto cdev_add_failed;
        } 
        pr_info("%s: cdev_add() added device file pchar%d\n", THIS_MODULE->name, i);
    }
    
    // Allocate kfifo
    for (i = 0; i < DEVCNT; i++) {
        devices[i].id = i;
        devices[i].devn = MKDEV(major, i);
        ret = kfifo_alloc(&devices[i].mybuf, MAX, GFP_KERNEL);
        if (ret != 0) {
            pr_err("%s: kfifo_alloc() failed\n", THIS_MODULE->name);
            goto kfifo_alloc_failed;
        }
        pr_info("%s: kfifo_alloc() created buffer pchar%d\n", THIS_MODULE->name, i);
    }
    
    // Initialize waiting queue
    for (i = 0; i < DEVCNT; i++) {
        init_waitqueue_head(&devices[i].wr_wq);
        init_waitqueue_head(&devices[i].rd_wq);
        pr_info("%s: init_waitqueue_head() initialized wr_wq for pchar%d\n", THIS_MODULE->name, i);
    }
    
    return 0;

kfifo_alloc_failed:
    for (i = i - 1; i >= 0; i--) {
        kfifo_free(&devices[i].mybuf);
    }
    i = DEVCNT;

cdev_add_failed:
    for (i = i - 1; i >= 0; i--) {
        cdev_del(&devices[i].cdev);
    }
    i = DEVCNT;

device_create_failed:
    for (i = i - 1; i >= 0; i--) {
        devnum = MKDEV(major, i);
        device_destroy(pclass, devnum);
    }

class_create_failed:
    unregister_chrdev_region(devno, DEVCNT);
    ret = -1;

alloc_chardev_region_failed:
    return ret; 
}

static void __exit pchar_multidev_exit(void) {
    int i;
    pr_info("%s: pchar_multidev_exit module called\n", THIS_MODULE->name);
    
    // Wake up all processes
    for (i = 0; i < DEVCNT; i++) {
        wake_up_interruptible_all(&devices[i].wr_wq);
        wake_up_interruptible_all(&devices[i].rd_wq);
    }
    
    // Free kfifo
    for (i = 0; i < DEVCNT; i++) {
        kfifo_free(&devices[i].mybuf);
        pr _info("%s: kfifo_free() destroyed buffer pchar%d\n", THIS_MODULE->name, i);
    }
    
    // Delete cdev object for multiple device instances and unload from kernel
    for (i = 0; i < DEVCNT; i++) {
        cdev_del(&devices[i].cdev);
        pr_info("%s: cdev_del() deleted device object pchar%d\n", THIS_MODULE->name, i);
    }
    
    // Destroy device files 
    for (i = 0; i < DEVCNT; i++) {
        devno = MKDEV(major, i);
        device_destroy(pclass, devno);
        pr_info("%s: device_destroy() destroyed device file pchar%d\n", THIS_MODULE->name, i);
    }
    
    // Destroy device class
    class_destroy(pclass);
    pr_info("%s: class_destroy() destroyed device class\n", THIS_MODULE->name);
    
    // Unallocate device number
    unregister_chrdev_region(devno, DEVCNT);
    pr_info("%s: unregister_chrdev_region() unallocated device number: %d\n", THIS_MODULE->name, major);
}

module_init(pchar_multidev_init);
module_exit(pchar_multidev_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Day 07 Assignment");
MODULE_AUTHOR("Hardik Chotalia <hardikc@gmail.com>");
