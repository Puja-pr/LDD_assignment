#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static int print_numbers(void *data) {
    int i;
    for(i=1; i<=10; i++) {
        pr_info("%s: kthread (%d) running %d.\n", THIS_MODULE->name, current->pid, i);
        msleep(1000);
    }
    return 0;
}

static int __init desd_init(void) {
    struct task_struct *task;
    pr_info("%s: desd_init() called.\n", THIS_MODULE->name);
    task = kthread_run(print_numbers, NULL, "numthread");
    pr_info("%s: new kernel thread created %d\n", THIS_MODULE->name, task->pid);
    return 0;
}

static void __exit desd_exit(void) {
    pr_info("%s: desd_exit() called.\n", THIS_MODULE->name);
}

module_init(desd_init);
module_exit(desd_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Kernel thread demo");
MODULE_AUTHOR("Adesh bhawar <adesh@gmail.com>");


