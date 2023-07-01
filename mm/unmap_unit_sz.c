/*
 * Memory Unmap Operation Unit Interface
 * (C) Geunsik Lim, April 2011
 */

#include <linux/init.h>
#include <linux/mm.h>
#include <linux/unmap_unit_sz.h>
#include <linux/sysctl.h>

/* amount of vm to unmap from userspace access by both non-preemptive mode
 * and preemptive mode
 */
unsigned long unmap_unit_sz;

/*
 * Memory unmap operation unit of vm to release allocated memory size from
 * userspace using  mmap system call
 */
#if defined(CONFIG_FAST_SWAPOUT_MUNMAP)
unsigned long sysctl_unmap_unit_sz = CONFIG_FAST_SWAPOUT_MUNMAP;
#endif

/*
 * Update unmap_unit_sz that changed with /proc/sys/vm/unmap_unit_sz
 * tunable value.
 */
static void update_unmap_unit_sz(void)
{
	unmap_unit_sz = sysctl_unmap_unit_sz;
}

/*
 * sysctl handler which just sets sysctl_unmap_unit_sz = the new value
 * and then calls update_unmap_unit_sz()
 */
int unmap_unit_sz_handler(struct ctl_table *table, int write,
			  void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;

	ret = proc_doulongvec_minmax(table, write, buffer, lenp, ppos);

	update_unmap_unit_sz();

	return ret;
}

static int __init init_unmap_unit_sz(void)
{
	update_unmap_unit_sz();

	return 0;
}
pure_initcall(init_unmap_unit_sz);