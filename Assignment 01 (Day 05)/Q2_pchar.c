#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define MAX 32
static char mybuf[MAX];
static dev_t devno;
static int major = 250;
static struct class *pclass;
static struct cdev pchar_cdev;

static int pchar_open(struct inode *pinode, struct file *pfile){
    printk(KERN_INFO "%s: pchar_open() called\n", THIS_MODULE->name);
    return 0;
}

static int pchar_close(struct inode *pinode, struct file *pfile){
    printk(KERN_INFO "%s: pchar_close() called\n", THIS_MODULE->name);
    return 0;
}

static ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t bufsize, loff_t *pf_pos){
    int max_bytes, bytes_to_write, nbytes;
    printk(KERN_INFO "%s: pchar_write() called\n", THIS_MODULE->name);
    max_bytes = MAX - *pf_pos;
    bytes_to_write = (max_bytes < bufsize) ? max_bytes : bufsize;
    if(bytes_to_write <= 0){
        printk(KERN_ERR "%s: pchar_write() - mybuf is full.\n", THIS_MODULE->name);
        return -ENOSPC;
    }
    nbytes = bytes_to_write - copy_from_user(mybuf + *pf_pos, ubuf, bytes_to_write);
    *pf_pos = *pf_pos + nbytes;
    printk(KERN_INFO "%s: pchar_write %d bytes to mybuf\n", THIS_MODULE->name, nbytes);
    return nbytes;
}

static ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t bufsize, loff_t *pf_pos){
    int max_bytes, bytes_to_read, nbytes;
    printk(KERN_INFO "%s: pchar_read() called\n", THIS_MODULE->name);
    max_bytes = MAX - *pf_pos;
    bytes_to_read = (max_bytes < bufsize) ? max_bytes : bufsize;
    if(bytes_to_read <= 0){
        printk(KERN_INFO "%s: pchar_read() - Empty buffer.\n", THIS_MODULE->name);
        return 0;
    }
    nbytes = bytes_to_read - copy_to_user(ubuf, mybuf + *pf_pos, bytes_to_read);
    *pf_pos = *pf_pos + nbytes;
    printk(KERN_INFO "%s: pchar_read %d bytes from mybuf\n", THIS_MODULE->name, nbytes);
    return nbytes;
}

static loff_t pchar_lseek(struct file *pfile, loff_t offset, int origin) {
    loff_t newpos = 0;
    printk(KERN_INFO "%s: pchar_lseek() called.\n", THIS_MODULE->name);
    switch(origin){
        case SEEK_SET:
            newpos = 0+offset;
            break;
        case SEEK_END:
            newpos = MAX+offset;
            break;
        case SEEK_CUR:
            newpos = pfile->f_pos+offset;
            break;
    }
    if(newpos < 0)
        newpos = 0;
    if(newpos > MAX)
        newpos = MAX;
    pfile->f_pos = newpos;
    printk(KERN_INFO "%s: pchar_lseek() newpos = %lld.\n", THIS_MODULE->name, newpos);
    return newpos;
}

static struct file_operations pchar_fops = {
.owner = THIS_MODULE,
.open = pchar_open,
.read = pchar_read,
.write = pchar_write,
.release = pchar_close,
.llseek = pchar_lseek
};


static int __init pchar_init(void){
    int ret;
    struct device *pdevice;
    printk(KERN_INFO "%s: pchar_init() called\n", THIS_MODULE->name);

    // allocate device no.
    devno = MKDEV(major, 0);
    ret = alloc_chrdev_region(&devno, 0, 1, "pchar");
    if(ret!=0){
        printk(KERN_ERR "%s: alloc_chrdec_region() failed\n", THIS_MODULE->name);
        return ret;
    }   
    major = MAJOR(devno);
    printk(KERN_INFO "%s: alloc_chrdec_region() device no: %d\n", THIS_MODULE->name, major);

    // create device class
    pclass = class_create("pchar_class");
    if(IS_ERR(pclass)){
        printk(KERN_ERR "%s: class_create() failed\n", THIS_MODULE->name);
        unregister_chrdev_region(devno, 1);
        return -1;
    }
    printk(KERN_INFO "%s: class_create() pchar_class created \n", THIS_MODULE->name);

    // create device file
    pdevice = device_create(pclass, NULL, devno, NULL, "pchar");
    if(IS_ERR(pdevice)){
        printk(KERN_ERR "%s: device_create() failed\n", THIS_MODULE->name);
        class_destroy(pclass);
        unregister_chrdev_region(devno, 1);
        return -1;
    }
    printk(KERN_INFO "%s: device_create() pchar_class created \n", THIS_MODULE->name);

    // // initialize cdev object and add it in kernel
    pchar_cdev.owner = THIS_MODULE;
    cdev_init(&pchar_cdev, &pchar_fops);
    ret = cdev_add(&pchar_cdev, devno, 1);
    if(ret != 0){
        printk(KERN_ERR "%s: cdev_add() failed\n", THIS_MODULE->name);
        device_destroy(pclass, devno);
        class_destroy(pclass);
        unregister_chrdev_region(devno, 1);
        return -1;
    }
    printk(KERN_INFO "%s: cdev_add() created \n", THIS_MODULE->name);

    return 0;
}

static void __exit pchar_exit(void){
    printk(KERN_INFO "%s: pchar_exit() called\n", THIS_MODULE->name);
    // destroy cdev
    cdev_del(&pchar_cdev);
    printk(KERN_INFO "%s: cdev_add released\n", THIS_MODULE->name);
    // destroy device file
    device_destroy(pclass, devno);
    printk(KERN_INFO "%s: device_create() pdevice released\n", THIS_MODULE->name);
    // destroy device class
    class_destroy(pclass);
    printk(KERN_INFO "%s: class_create() pchar_class released\n", THIS_MODULE->name);
    //release device no.
    unregister_chrdev_region(devno, 1);
    printk(KERN_INFO "%s: unregister_chrdev_region() released device no: %d\n", THIS_MODULE->name, major);
} 

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HELLO MODULE");
MODULE_AUTHOR("HARDIK <hardik@gmail.com>");