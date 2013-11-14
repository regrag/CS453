
#ifndef __BOOGA_H
#define __BOOGA_H
/*
 * booga.h -- definitions for the char module
 *
 */

/* version dependency between 2.4  and 2.6 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
#define MODULE_USAGE_UP {MOD_INC_USE_COUNT;}
#else
#define MODULE_USAGE_UP {try_module_get(THIS_MODULE);}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
#define MODULE_USAGE_DOWN {MOD_DEC_USE_COUNT;}
#else
#define MODULE_USAGE_DOWN {module_put(THIS_MODULE);}
#endif


#ifndef BOOGA_MAJOR
#define BOOGA_MAJOR 0   /* dynamic major by default */
#endif

#ifndef BOOGA_NR_DEVS
#define BOOGA_NR_DEVS 4    /* booga0 through booga3 */
#endif
/*
 * Split minors in two parts
 */
#define TYPE(dev)   (MINOR(dev) >> 4)  /* high nibble */
#define NUM(dev)    (MINOR(dev) & 0xf) /* low  nibble */

/*
 * The different configurable parameters
 */

struct booga_stats {
	long int booga0_num_open;
	long int booga1_num_open;
	long int booga2_num_open;
	long int booga3_num_open;
	long int num_read; 
	long int num_write; 
	long int num_close; 
	struct semaphore sem;
};
typedef struct booga_stats booga_stats;

struct booga_dev {
	char *data;
	unsigned int number; /* minor number */
	unsigned int usage; /* keep track of device usage */
};
typedef struct booga_dev booga_dev;

/*extern booga_stats booga_Device_Stats;*/

#endif /* __booga_H */
