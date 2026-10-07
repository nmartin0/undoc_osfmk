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
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 * $Log: host_info.h,v $
 * Revision 1.7  1994/11/18  20:53:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/08/31  21:24:50  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.4.20.1  1994/08/10  16:26:06  flb
 *  Reviewer: terry
 *  Risk: low
 *  Benefit or PTS #:9976
 *  Testing: System Boot with large MAGIC.MASTER
 *  Module(s): ipsc/bootenv.c, mach/host_info.h, i860/ctrap.c, kern/machine.c
 *
 * Revision 1.4  1993/06/30  22:46:48  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:38:03  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:39:06  dleslie
 * First R1_0 release
 *
 * Revision 2.4.2.1  92/01/09  18:44:17  jsb
 * 	From durriya@ri.osf.org: defined kernel_boot_info_t.
 * 	[92/01/08  15:01:53  jsb]
 * 
 * Revision 2.4  91/05/14  16:51:48  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:31:58  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:17:13  mrt]
 * 
 * Revision 2.2  90/06/02  14:57:58  rpd
 * 	Added HOST_LOAD_INFO and related definitions.
 * 	[90/04/27            rpd]
 * 	Created for new host/processor technology.
 * 	[90/03/26  23:50:51  rpd]
 * 
 * 	Cleanup changes.
 * 	[89/08/02            dlb]
 * 	Add sched_info flavor to return minimum times for use by
 * 	external schedulers.
 * 	[89/06/08            dlb]
 * 	Added kernel_version type definitions.
 * 	[88/12/02            dlb]
 * 
 * Revision 2.4  89/10/15  02:05:31  rpd
 * 	Minor cleanups.
 * 
 * Revision 2.3  89/10/11  17:32:15  dlb
 * 	Include mach/machine/vm_types.h instead of mach/vm_param.h
 * 	[89/10/11            dlb]
 * 
 * Revision 2.2  89/10/11  14:36:55  dlb
 * 	Add sched_info flavor to return minimum times for use by
 * 	external schedulers.
 * 	[89/06/08            dlb]
 * 
 * 	Added kernel_version type definitions.
 * 	[88/12/02            dlb]
 * 
 * 30-Nov-88  David Black (dlb) at Carnegie-Mellon University
 *	Created.  2 flavors so far: basic info,  slot numbers.
 *
 */

/*
 *	File:	mach/host_info.h
 *
 *	Definitions for host_info call.
 */

#ifndef	_MACH_HOST_INFO_H_
#define	_MACH_HOST_INFO_H_

#include <mach/machine.h>
#include <mach/machine/vm_types.h>

/*
 *	Generic information structure to allow for expansion.
 */
typedef int	*host_info_t;		/* varying array of int. */

#define	HOST_INFO_MAX	(1024)		/* max array size */
typedef int	host_info_data_t[HOST_INFO_MAX];

#define KERNEL_VERSION_MAX (512)
typedef char	kernel_version_t[KERNEL_VERSION_MAX];

#define KERNEL_BOOT_INFO_MAX (4096)
typedef char	kernel_boot_info_t[KERNEL_BOOT_INFO_MAX];

#define	KERNEL_BOOTMAGIC_MAX	(8192)

/*
 *	Currently defined information.
 */
#define HOST_BASIC_INFO		1	/* basic info */
#define HOST_PROCESSOR_SLOTS	2	/* processor slot numbers */
#define HOST_SCHED_INFO		3	/* scheduling info */
#define	HOST_LOAD_INFO		4	/* avenrun/mach_factor info */

struct host_basic_info {
	int		max_cpus;	/* max number of cpus possible */
	int		avail_cpus;	/* number of cpus now available */
	vm_size_t	memory_size;	/* size of memory in bytes */
	cpu_type_t	cpu_type;	/* cpu type */
	cpu_subtype_t	cpu_subtype;	/* cpu subtype */
};

typedef	struct host_basic_info	host_basic_info_data_t;
typedef struct host_basic_info	*host_basic_info_t;
#define HOST_BASIC_INFO_COUNT \
		(sizeof(host_basic_info_data_t)/sizeof(int))

struct host_sched_info {
	int		min_timeout;	/* minimum timeout in milliseconds */
	int		min_quantum;	/* minimum quantum in milliseconds */
};

typedef	struct host_sched_info	host_sched_info_data_t;
typedef struct host_sched_info	*host_sched_info_t;
#define HOST_SCHED_INFO_COUNT \
		(sizeof(host_sched_info_data_t)/sizeof(int))

struct host_load_info {
	long		avenrun[3];	/* scaled by LOAD_SCALE */
	long		mach_factor[3];	/* scaled by LOAD_SCALE */
};

typedef struct host_load_info	host_load_info_data_t;
typedef struct host_load_info	*host_load_info_t;
#define	HOST_LOAD_INFO_COUNT \
		(sizeof(host_load_info_data_t)/sizeof(int))

#endif	_MACH_HOST_INFO_H_
