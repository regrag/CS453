
#ifndef __EXAMPLE_H
#define __EXAMPLE_H
/*
 * example.h -- definitions for the char module
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


#ifndef EXAMPLE_MAJOR
#define EXAMPLE_MAJOR 0   /* dynamic major by default */
#endif

#ifndef EXAMPLE_NR_DEVS
#define EXAMPLE_NR_DEVS 4    /* example0 through example3 */
#endif
/*
 * Split minors in two parts
 */
#define TYPE(dev)   (MINOR(dev) >> 4)  /* high nibble */
#define NUM(dev)    (MINOR(dev) & 0xf) /* low  nibble */

/*
 * The different configurable parameters
 */

struct example_stats {
	long int num_open;
	long int num_read; 
	long int num_write; 
	long int num_close; 
};
typedef struct example_stats example_stats;

extern example_stats Example_Device_Stats;

#endif /* __EXAMPLE_H */
