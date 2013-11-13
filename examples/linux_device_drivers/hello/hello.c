
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
/* only needed for kernels prior to 2.6.19 */
/*#include <linux/config.h>*/

/*MODULE_LICENSE("Proprietary");*/
MODULE_LICENSE("GPL");

static int __init hello_init(void)      { printk("<1>Hello, world\n"); return 0; }
static void __exit hello_cleanup(void)  { printk("<1>Goodbye cruel world\n"); }

module_init(hello_init);
module_exit(hello_cleanup);
