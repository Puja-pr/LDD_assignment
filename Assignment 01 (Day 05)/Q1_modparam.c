#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

static char* user = "DESD";
module_param(user, charp, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
static int count = 3;
module_param_named(cnt, count, int, 0644);

static int __init desd_init(void) {
    int i;
    printk(KERN_INFO "%s: desd_init() called.\n", THIS_MODULE->name);
    for(i=1; i<=count; i++)
        printk(KERN_INFO "%s: Hi, %s.\n", THIS_MODULE->name, user);
    return 0;
}

static void __exit desd_exit(void) {
    int i;
    printk(KERN_INFO "%s: desd_exit() called.\n", THIS_MODULE->name);
    for(i=1; i<=count; i++)
        printk(KERN_INFO "%s: Bye, %s.\n", THIS_MODULE->name, user);
}

module_init(desd_init);
module_exit(desd_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module Parameter");
MODULE_AUTHOR("Adesh bhawar <adesh@gmail.com>>");

