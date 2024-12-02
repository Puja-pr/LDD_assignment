#include <linux/module.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init desd_init(void){
    struct module *trav;
    pr_info("%s: desd_init() called.\n", THIS_MODULE->name);
    list_for_each_entry(trav, &THIS_MODULE->list, list){
        pr_info("Module name: %s\n", trav->name);
    }
    return 0;
}

static void __exit desd_exit(void){
    pr_info("%s: desd_exit() called.\n", THIS_MODULE->name);
}

module_init(desd_init);
module_exit(desd_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel module demo of Kernel Linked List");
MODULE_AUTHOR("Adesh bhawar <adesh@gmail.com>");
