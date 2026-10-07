/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
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
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: proc_to_task.h,v $
 * Revision 1.6  1994/11/18  20:49:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/09/29  23:00:09  yazz
 *  Author of fix: Chris Peak
 *  Reviewer: Bob Yasi, Nandini Ajmani
 *  Risk: Medium
 *  Benefit or PTS #: 9346 (plus 10485, 6266, 7916, 9265, 8194)
 *  Testing: control-c EAT case 3 (20 iterations), other EATs os_interfaces,
 * 		xtrnl, controlc, rmcall, fileio, message
 *  Module(s):
 * 	server/sys/vproc.h
 * 	server/uxkern/proc_to_task.h
 * 	server/uxkern/proc_to_task.c
 * 	server/tnc/dvp_vpops.c
 * 	server/tnc/rvp_subr.c
 *
 * Abolish the renaming of vproc ports for send rights on remote nodes.
 * Assign vproc port name at vproc alloc time and clear it at dealloc time.
 * Make vproc_to_port() lookup macro return the contents of the new vproc
 * port name field in the vproc structure.
 *
 * Revision 1.4  1993/07/14  18:43:49  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:05:16  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:32:14  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:53:22  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.4  1991/10/04  15:24:10  chrisp
 * Add Locus copyright.
 *
 * Revision 2.3  91/09/17  09:14:15  sjs
 * integrate Locus changes	roman
 * Add translation for vprocs to ports and vice versa.
 * 
 * Revision 2.2  91/08/31  14:27:27  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.0  91/01/17  12:06:15  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.3  90/06/02  15:28:18  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  20:22:33  rpd]
 * 
 * Revision 2.2  89/12/08  20:16:50  rwd
 * 	Changed port_to_proc_lookup into a macro.
 * 	[89/11/01            rwd]
 * 
 */
/*
 * Task->process and request_port -> process routines.
 */
extern void	task_to_proc_init();
extern mach_port_t task_to_proc_enter();	/* task_t, struct proc * */
extern struct proc *
		task_to_proc_lookup();	/* task_t */
extern void	task_to_proc_remove();	/* task_t */

#define port_to_proc_lookup(port) ((struct proc *)port)
#define proc_to_port_lookup(proc) ((mach_port_t)proc)

#include <sys/vproc.h>
#define port_to_vproc_lookup(port) ((struct vproc *)port)
#define vproc_to_port_lookup(vproc) ((vproc)->vp_port_name)
