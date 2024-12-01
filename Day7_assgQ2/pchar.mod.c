#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0x89940875, "mutex_lock_interruptible" },
	{ 0x30a80826, "__kfifo_from_user" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x4578f528, "__kfifo_to_user" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x38e073, "class_create" },
	{ 0x23a131f9, "device_create" },
	{ 0x37a0cba, "kfree" },
	{ 0x5c658724, "cdev_init" },
	{ 0xa2dc7695, "cdev_add" },
	{ 0x139f2189, "__kfifo_alloc" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0xb35af14c, "param_ops_int" },
	{ 0x122c3a7e, "_printk" },
	{ 0xdb760f52, "__kfifo_free" },
	{ 0xd58dd2b1, "cdev_del" },
	{ 0x2f8479f2, "device_destroy" },
	{ 0x9901e5e3, "class_destroy" },
	{ 0xf079b8f9, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "2A9C9EAC4C134C304E8E910");
