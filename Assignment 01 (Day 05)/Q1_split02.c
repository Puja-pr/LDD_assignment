#include <linux/module.h>
#include <linux/kernel.h>

static void __exit desd_exit(void) {
    printk(KERN_INFO "%s: desd_exit() called.\n", THIS_MODULE->name);
}

module_exit(desd_exit);
