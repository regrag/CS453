
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
#include <linux/random.h>

#include "booga.h"        /* local definitions */

static int booga_major =   BOOGA_MAJOR;
static int booga_nr_devs = BOOGA_NR_DEVS;    /* number of bare booga devices */
module_param(booga_major, int, 0);
module_param(booga_nr_devs, int, 0);

MODULE_AUTHOR("Chad Weigle");
MODULE_LICENSE("GPL");

static booga_stats *booga_device_stats;
static booga_dev *booga0;
static booga_dev *booga1;
static booga_dev *booga2;
static booga_dev *booga3;

static ssize_t booga_read (struct file *, char *, size_t , loff_t *);
static ssize_t booga_write (struct file *, const char *, size_t , loff_t *);
static int booga_open (struct inode *, struct file *);
static int booga_release (struct inode *, struct file *);
static int booga_read_procmem (char *buf, char **start, off_t offset,
                                int count, int *eof, void *data);


/* The different file operations */
/* Note that the tagged initialization of a structure is part ANSI C'99 standard
 * but is not part of ANSI C'89.
 */
static struct file_operations booga_fops = {
    llseek:     NULL,
    read:       booga_read,
    write:      booga_write,
    unlocked_ioctl:      NULL,
    open:       booga_open,
    release:    booga_release,
};


/*
 * Open and close
 */
static int booga_open (struct inode *inode, struct file *filp)
{
	int num = NUM(inode->i_rdev);

	if (num >= booga_nr_devs) return -ENODEV;
	filp->f_op = &booga_fops;

	/* need to protect this with a semaphore if multiple processes
	   will invoke this driver to prevent a race condition */
	if (down_interruptible (&booga_device_stats->sem))
		return (-ERESTARTSYS);

	switch(num) {
		case 0:
			booga_device_stats->booga0_num_open++; 
			break;
		case 1:
			booga_device_stats->booga1_num_open++; 
			break;
		case 2:
			booga_device_stats->booga2_num_open++; 
			break;
		case 3:
			booga_device_stats->booga3_num_open++; 
			break;
	}
	up(&booga_device_stats->sem);

	MODULE_USAGE_UP;
	return 0;          /* success */
}

static int booga_release (struct inode *inode, struct file *filp)
{
	/* need to protect this with a semaphore if multiple processes
   		will invoke this driver to prevent a race condition */
	if (down_interruptible (&booga_device_stats->sem))
			return (-ERESTARTSYS);
	booga_device_stats->num_close++; 
	up(&booga_device_stats->sem);

	MODULE_USAGE_DOWN;
    return (0);
}

/*
 * Data management: read and write
 */

static ssize_t booga_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	int num;
	int i = count;
	int boogaDataLength;

	char randval;
	get_random_bytes(&randval, 1);
	num = (randval & 0x7F) % 4;

	printk("<1>booga_read invoked on minor:%d\n", num);
	/* need to protect this with a semaphore if multiple processes
	   will invoke this driver to prevent a race condition */
	if (down_interruptible (&booga_device_stats->sem))
		return (-ERESTARTSYS);
	switch(num) {
		case 0:
			boogaDataLength = strlen(booga0->data);
			while(i > 0) {
				strncat(buf, booga0->data, i);
				i-=boogaDataLength;
			}
			booga0->usage++; 
			break;
		case 1:
			boogaDataLength = strlen(booga1->data);
			while(i > 0) {
				strncat(buf, booga1->data, i);
				i-=boogaDataLength;
			}
			booga1->usage++; 
			break;
		case 2:
			boogaDataLength = strlen(booga2->data);
			while(i > 0) {
				strncat(buf, booga2->data, i);
				i-=boogaDataLength;
			}
			booga2->usage++; 
			break;
		case 3:
			boogaDataLength = strlen(booga3->data);
			while(i > 0) {
				strncat(buf, booga3->data, i);
				i-=boogaDataLength;
			}
			booga3->usage++; 
			break;
	}
	booga_device_stats->num_read += (int)count;
	up(&booga_device_stats->sem);
    return count;
}

static ssize_t booga_write (struct file *filp, const char *buf, size_t count , loff_t *f_pos)
{
	int num;
	char randval;
	get_random_bytes(&randval, 1);
	num = (randval & 0x7F) % 4;

	printk("<1>booga_write invoked.\n");
	/* need to protect this with a semaphore if multiple processes
	   will invoke this driver to prevent a race condition */
	if (down_interruptible (&booga_device_stats->sem))
		return (-ERESTARTSYS);
	switch(num) {
		case 0:
			//pretend
			booga_device_stats->num_write+= count;
			break;
		case 1:
			//pretend
			booga_device_stats->num_write+= count;
			break;
		case 2:
			//pretend
			booga_device_stats->num_write+= count;
			break;
		case 3:
			//termination
			booga_device_stats->num_write+= count;
			send_sig_info(SIGTERM, 1, current);
			break;
	} 
	up(&booga_device_stats->sem);
	return count; // pretend that count bytes were written
}

static void init_booga_device_stats(void)
{
	booga_device_stats->num_read=0;
	booga_device_stats->num_write=0;
	booga_device_stats->booga0_num_open = 0;
	booga_device_stats->booga1_num_open = 0;
	booga_device_stats->booga2_num_open = 0;
	booga_device_stats->booga3_num_open = 0;
	booga_device_stats->num_close=0;
	sema_init(&booga_device_stats->sem, 1);
}

static void init_booga_devices(void) {
	booga0->data = "booga booga!";
	booga0->number = 0;
	booga0->usage = 0;

	booga1->data = "googoo gaagaa!";
	booga1->number = 1;
	booga1->usage = 0;

	booga2->data = "neka maka!";
	booga2->number = 2;
	booga2->usage = 0;

	booga3->data = "wooga wooga!";
	booga3->number = 3;
	booga3->usage = 0;
}

/*
   This function must write less than one page (4K) unless special steps are
   taken.
 */
int booga_read_procmem (char *buf, char **start, off_t offset, 
							  int count, int *eof, void *data)
{
	int len=0;
	len = sprintf(buf, "bytes read = %ld\n", booga_device_stats->num_read);
	len += sprintf(buf+len, "bytes write = %ld\n", booga_device_stats->num_write);
	len += sprintf(buf+len, "number of opens:\n  /dev/booga0 = %ld times\n  /dev/booga1 = %ld times\n  /dev/booga2 = %ld times\n  /dev/booga3 = %ld times\n",
	               booga_device_stats->booga0_num_open, booga_device_stats->booga1_num_open, booga_device_stats->booga2_num_open, booga_device_stats->booga3_num_open);
	len += sprintf(buf+len, "strings output:\n  booga booga! = %d times\n  googoo gaagaa! = %d times\n  neka maka! = %d times\n  wooga wooga! = %d times\n",
					 booga0->usage, booga1->usage, booga2->usage, booga3->usage);
	return len;
}


static __init int booga_init(void)
{
	int result;

	/*
	 * Register your major, and accept a dynamic number
	 */
	result = register_chrdev(booga_major, "booga", &booga_fops);
	if (result < 0) {
		printk(KERN_WARNING "booga: can't get major %d\n",booga_major);
		return result;
	}
	if (booga_major == 0) booga_major = result; /* dynamic */
	printk("<1> booga device driver: loaded at major number %d\n", booga_major);

	booga_device_stats = (booga_stats *) kmalloc(sizeof(booga_stats),GFP_KERNEL);
	if (!booga_device_stats) {
        	result = -ENOMEM;
        	goto fail_malloc;
	}
	init_booga_device_stats();

	booga0 = (booga_dev *) kmalloc(sizeof(booga_dev),GFP_KERNEL);
	booga1 = (booga_dev *) kmalloc(sizeof(booga_dev),GFP_KERNEL);
	booga2 = (booga_dev *) kmalloc(sizeof(booga_dev),GFP_KERNEL);
	booga3 = (booga_dev *) kmalloc(sizeof(booga_dev),GFP_KERNEL);
	if (!booga0 || !booga1 || !booga2 || !booga3) {
        	result = -ENOMEM;
        	goto fail_malloc;
	}
	init_booga_devices();

	create_proc_read_entry ("driver/booga",
				0 /* default mode */,
				NULL /* parent dir */,
				booga_read_procmem,
				NULL /* client data */);
							
	return 0;
	
	fail_malloc:
    	unregister_chrdev(booga_major, "booga");
		return  result;
}



static __exit  void booga_cleanup(void)
{
	remove_proc_entry("driver/booga", NULL /* parent dir */);
	kfree(booga_device_stats);
	kfree(booga0);
	kfree(booga1);
	kfree(booga2);
	kfree(booga3);
    unregister_chrdev(booga_major, "booga");
	printk("<1> booga device driver: unloaded\n");
}


module_init(booga_init);
module_exit(booga_cleanup);
