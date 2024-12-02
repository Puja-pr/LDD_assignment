#include <linux/module.h>
#include <linux/kernel.h>

static int __init desd_init(void) {
    printk(KERN_INFO "%s: desd_init() called.\n", THIS_MODULE->name);
    return 0;
}

module_init(desd_init);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello Kernel Module Split into Multiple source files");
MODULE_AUTHOR("Adesh bhawar <adesh@gmail.com>>");
