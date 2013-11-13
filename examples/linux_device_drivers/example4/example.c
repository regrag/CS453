/* 
   This example shows how to use a semaphore to avoid race conditions
   in updating a global data structure inside a driver.
*/

/*
 * example.c -- the bare example char module
 *
 *********/
#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/version.h> /* printk() */
#include <linux/init.h>  /*  for module_init and module_cleanup */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/proc_fs.h>  /* size_t */
#include "example.h"        /* local definitions */

static int example_major =   EXAMPLE_MAJOR;
static int example_nr_devs = EXAMPLE_NR_DEVS;    /* number of bare example devices */
module_param(example_major, int, 0);
module_param(example_nr_devs, int, 0);

MODULE_AUTHOR("Amit Jain");
MODULE_LICENSE("GPL");

static example_stats *example_device_stats;

static ssize_t example_read (struct file *, char *, size_t , loff_t *);
static ssize_t example_write (struct file *, const char *, size_t , loff_t *);
static int example_open (struct inode *, struct file *);
static int example_release (struct inode *, struct file *);
static int example_read_procmem (char *buf, char **start, off_t offset,
                                int count, int *eof, void *data);


/* The different file operations */
/* Note that the tagged initialization of a structure is part ANSI C'99 standard
 * but is not part of ANSI C'89.
 */
static struct file_operations example_fops = {
    llseek:     NULL,
    read:       example_read,
    write:      example_write,
    unlocked_ioctl:      NULL,
    open:       example_open,
    release:    example_release,
};


/*
 * Open and close
 */
static int example_open (struct inode *inode, struct file *filp)
{
	int num = NUM(inode->i_rdev);

	if (num >= example_nr_devs) return -ENODEV;

	filp->f_op = &example_fops;

	/* need to protect this with a semaphore if multiple processes
	   will invoke this driver to prevent a race condition */
	if (down_interruptible (&example_device_stats->sem))
		return (-ERESTARTSYS);
	example_device_stats->num_open++; 
	up(&example_device_stats->sem);

	MODULE_USAGE_UP;
	return 0;          /* success */
}

static int example_release (struct inode *inode, struct file *filp)
{
	/* need to protect this with a semaphore if multiple processes
   		will invoke this driver to prevent a race condition */
	if (down_interruptible (&example_device_stats->sem))
			return (-ERESTARTSYS);
	example_device_stats->num_close++; 
	up(&example_device_stats->sem);

	MODULE_USAGE_DOWN;
    return (0);
}

/*
 * Data management: read and write
 */

static ssize_t example_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	printk("<1>example_read invoked.\n");
	/* need to protect this with a semaphore if multiple processes
	   will invoke this driver to prevent a race condition */
	if (down_interruptible (&example_device_stats->sem))
		return (-ERESTARTSYS);
	example_device_stats->num_read++; 
	up(&example_device_stats->sem);
    return 0;
}

static ssize_t example_write (struct file *filp, const char *buf, size_t count , loff_t *f_pos)
{
	printk("<1>example_write invoked.\n");
	/* need to protect this with a semaphore if multiple processes
	   will invoke this driver to prevent a race condition */
	if (down_interruptible (&example_device_stats->sem))
		return (-ERESTARTSYS);
	example_device_stats->num_write++; 
	up(&example_device_stats->sem);
	return count; // pretend that count bytes were written
}

static void init_example_device_stats(void)
{
	example_device_stats->num_read=0;
	example_device_stats->num_write=0;
	example_device_stats->num_open=0;
	example_device_stats->num_close=0;
	sema_init(&example_device_stats->sem, 1);
}


/*
   This function must write less than one page (4K) unless special steps are
   taken.
 */
int example_read_procmem (char *buf, char **start, off_t offset, 
							  int count, int *eof, void *data)
{
	int len=0;
	len = sprintf(buf, "open = %ld times\n", example_device_stats->num_open);
	len += sprintf(buf+len, "close = %ld times\n", example_device_stats->num_close);
	len += sprintf(buf+len, "read = %ld times\n", example_device_stats->num_read);
	len += sprintf(buf+len, "write = %ld times\n", example_device_stats->num_write);
	return len;
}


static __init int example_init(void)
{
	int result;

	/*
	 * Register your major, and accept a dynamic number
	 */
	result = register_chrdev(example_major, "example", &example_fops);
	if (result < 0) {
		printk(KERN_WARNING "example: can't get major %d\n",example_major);
		return result;
	}
	if (example_major == 0) example_major = result; /* dynamic */
	printk("<1> example device driver version 4: loaded at major number %d\n", example_major);

	example_device_stats = (example_stats *) kmalloc(sizeof(example_stats),GFP_KERNEL);
	if (!example_device_stats) {
        	result = -ENOMEM;
        	goto fail_malloc;
	}
	init_example_device_stats();

	create_proc_read_entry ("driver/example",
				0 /* default mode */,
				NULL /* parent dir */,
				example_read_procmem,
				NULL /* client data */);
							
	return 0;
	
	fail_malloc:
    	unregister_chrdev(example_major, "example");
		return  result;
}



static __exit  void example_cleanup(void)
{
	remove_proc_entry("driver/example", NULL /* parent dir */);
	kfree(example_device_stats);
    unregister_chrdev(example_major, "example");
	printk("<1> example device driver version 4: unloaded\n");
}


module_init(example_init);
module_exit(example_cleanup);

/* vim: set ts=4: */
