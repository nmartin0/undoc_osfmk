/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * 23-Jul-92  David Bernard (bernard) at Open Software Foundation
 *	added device info support for scatter/gather list i/o.
 *
 * $Log: conf.h,v $
 * Revision 1.6  1994/11/18  20:30:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  22:21:38  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/05/27  22:25:56  arlin
 *  scatter/gather i/o for Hippi Driver
 *
 * Revision 1.3  1993/04/27  20:19:54  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:19:33  dleslie
 * First R1_0 release
 *
 * Revision 2.4  91/08/28  11:11:12  jsb
 * 	Change block size entry to general device info entry.
 * 	[91/08/12  17:24:37  dlb]
 * 
 * 	Add block size entry.  Only needed for block_io devices.
 * 	[91/08/05  17:28:42  dlb]
 * 
 * Revision 2.3  91/05/14  15:39:57  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/02/05  17:08:10  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:26:55  mrt]
 * 
 * Revision 2.1  89/08/03  15:26:07  rwd
 * Created.
 * 
 * 12-May-89  David Golub (dbg) at Carnegie-Mellon University
 *	Added indirect devices.
 *
 * 12-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Added port_death routine.
 *
 * 24-Aug-88  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	8/88
 */

#ifndef	_CONF_
#define	_CONF_

/*
 * Operations list for major device types.
 */
struct dev_ops {
	char *    	d_name;		/* name for major device */
	int		(*d_open)();	/* open device */
	int		(*d_close)();	/* close device */
	int		(*d_read)();	/* read */
	int		(*d_write)();	/* write */
	int		(*d_getstat)();	/* get status/control */
	int		(*d_setstat)();	/* set status/control */
	int		(*d_mmap)();	/* map memory */
	int		(*d_async_in)();/* asynchronous input setup */
	int		(*d_reset)();	/* reset device */
	int		(*d_port_death)();
					/* clean up reply ports */
	int		d_subdev;	/* number of sub-devices per
					   unit */
	int		(*d_dev_info)(); /* driver info for kernel */
};
typedef struct dev_ops *dev_ops_t;

/*
 * Routines for null entries.
 */
extern int	nulldev();		/* no operation - OK */
extern int	nodev();		/* no operation - error */

/*
 * Flavor constants for d_dev_info routine
 */
#define D_INFO_BLOCK_SIZE	1
#define D_INFO_SGLIST_IO	2
/*
 * Head of list of attached devices
 */
extern struct dev_ops	dev_name_list[];
extern int		dev_name_count;

/*
 * Macro to search device list
 */
#define	dev_search(dp)	\
	for (dp = dev_name_list; \
	     dp < &dev_name_list[dev_name_count]; \
	     dp++)

/*
 * Indirection vectors for certain devices.
 */
struct dev_indirect {
	char *		d_name;		/* name for device */
	dev_ops_t	d_ops;		/* operations (major device) */
	int		d_unit;		/* and unit number */
};
typedef struct dev_indirect	*dev_indirect_t;

/*
 * List of indirect devices.
 */
extern struct dev_indirect	dev_indirect_list[];
extern int			dev_indirect_count;

/*
 * Macro to search indirect list
 */
#define	dev_indirect_search(di) \
	for (di = dev_indirect_list; \
	     di < &dev_indirect_list[dev_indirect_count]; \
	     di++)

/*
 * Exported routine to set indirection.
 */
extern void	dev_set_indirect();

#endif	_CONF_
