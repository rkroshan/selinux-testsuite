#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init setestsuite_module_init(void)
{
	int result = 0;

	pr_info("INIT - setestsuite_module\n");
	result = request_module_nowait("dummy-module");
	pr_info("request_module() returned: %d\n", result);
	return result;
}

static void __exit setestsuite_module_exit(void)
{
	pr_info("EXIT - setestsuite_module\n");
}

module_init(setestsuite_module_init);
module_exit(setestsuite_module_exit);
MODULE_LICENSE("GPL");
