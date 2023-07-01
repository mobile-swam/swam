/*
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Due to this file being licensed under the GPL there is controversy over
 *	whether this permits you to write a module that #includes this file
 *	without placing your module under the GPL.  Please consult a lawyer for
 *	advice before doing this.
 *
 */

#ifdef CONFIG_MMU
extern unsigned long unmap_unit_sz;
extern unsigned long sysctl_unmap_unit_sz;
#else
#define sysctl_unmap_unit_sz	0UL
#endif

#ifdef CONFIG_MMU
extern int unmap_unit_sz_handler(struct ctl_table *table, int write,
				 void __user *buffer, size_t *lenp, loff_t *ppos);
#endif