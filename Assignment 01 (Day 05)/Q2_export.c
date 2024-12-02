#include <linux/module.h>
#include <linux/kernel.h>

int exported_var=101;

void exported_fn(void) {
    printk(KERN_INFO "%s: exported_fn() called.\n", THIS_MODULE->name);
    exported_var++;
}

static int __init desd_init(void) {
    printk(KERN_INFO "%s: desd_init() called.\n", THIS_MODULE->name);
    return 0;
}

static void __exit desd_exit(void) {
    printk(KERN_INFO "%s: desd_exit() called.\n", THIS_MODULE->name);
}

module_init(desd_init);
module_exit(desd_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module Export");
MODULE_AUTHOR("Adesh bhawar <adesh@gmail.com>>");

EXPORT_SYMBOL(exported_fn);
EXPORT_SYMBOL_GPL(exported_var);

