#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/kernel.h>

static int __init proc_init(void) {
    struct task_struct *trav;

    pr_info("%s: proc_init() called.\n", THIS_MODULE->name);

    list_for_each_entry(trav, &current->tasks, tasks) {
        pr_info("%s: pid=%d, cmd=%s\n", THIS_MODULE->name, trav->pid, trav->comm);
    }

    return 0;
}

static void __exit proc_exit(void) {
    pr_info("%s: proc_exit() called.\n", THIS_MODULE->name);
    pr_info("%s: current process pid=%d, cmd=%s\n", THIS_MODULE->name, current->pid, current->comm);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel module demo to display process list starting from the current process");
MODULE_AUTHOR("Adesh bhawar <adesh@gmail.com>>");

