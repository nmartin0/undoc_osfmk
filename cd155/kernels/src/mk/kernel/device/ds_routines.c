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
 * $Log: ds_routines.c,v $
 * Revision 1.28  1995/04/01  00:56:00  cfleck
 *  Reviewer: jerrie, terry
 *  Risk: low
 *  Benefit or PTS #: 10757
 *  Testing: rw test, hippi write test, IPI3 eats, PFS sat
 *  Module(s): vm/vm_map.c, (files with backed out first attempt:
 *         device/io_req.h, i860paragon/model_dep.c, device/ds_routines,
 *         vm/vm_kern.c)
 *  Description: removing SGIO VM cleanup code because a safer and easier
 * 		fix was put in vm_map.c..
 *
 * Revision 1.27  1995/03/21  23:01:30  cfleck
 *  Reviewer: jerrie
 *  Risk: low
 *  Benefit or PTS #: 10757
 *  Testing: IPI3 rw test
 *  Module(s): device/ds_routines.c, i860paragon/model_dep.c
 *  Description: added bootmagic for sgio vm cleanup.. Turned off by default
 *               until all the kinks are worked out.
 *
 * Revision 1.26  1995/03/15  01:20:05  cfleck
 *  Reviewer: jerrie, arlin, andyp
 *  Risk: Medium
 *  Benefit or PTS #: 10757
 *  Testing: arlin's  hippi write test, IPI3, PFS test
 *  Module(s): device/io_req.h, device/ds_routines.c, vm/vm_kern.c, vm/vm_map.c
 *  Description: Added code to cleanup after sg io.
 *
 * Revision 1.25  1995/03/02  23:44:28  arlin
 *   device_read_alloc_sg() incorrectly
 *   adjusts io_count of request when
 *   device does not return block size.
 *
 *   Reviewer: Jerrie Coffman, Bernie Keany
 *   Risk: low, affects sg devices only
 *   Benefit or PTS #: 12411
 *   Testing: HiPPI EATs: Raw, TCP/IP, and IPI-3.
 *     Also developed special applications to test
 *     new MPC and CONT modes.
 *   Module(s): device_read_alloc_sg()
 *
 * Revision 1.24  1995/03/02  13:04:57  cfleck
 *  Reviewer: jerrie
 *  Risk: medium
 *  Benefit or PTS #: 12580, 12583, 12520
 *  Testing: arlin's hippi test, pfs SAT, pfs EATS, rw IPI3 test
 *  Module(s): kernel/device/ds_routines.c
 *  Description: vm copy page lists were becoming corrupted for scatter/gather io
 *               devices because of a bug induced when fixing 10308.
 *
 * Revision 1.23  1995/02/09  22:50:34  cfleck
 *  Reviewer: jerrie, andyp
 *  Risk: medium
 *  Benefit or PTS #: 10308
 *  Description:
 * 	This bug appears when there is a page list continuation that started
 * 	on a non-page aligned address.  The data gets corrupted because
 * 	the device is given an io_count that is a non-multiple of block size.
 * 	(The device fills out the block with NULLS).  Inorder to fix this
 * 	bug, the first io request's io count must be made a multiple of the
 * 	device block size.  For this to happen properly, the continuation
 * 	arguments must be adjusted based upon the "adjustment" count and the
 * 	continuation address map entry must be adjusted to include the last
 * 	page of the initial request (since you are cutting off the request
 * 	somewhere in the last page to get the proper io count).
 *
 *  Testing: PFS SAT, PFS eats, PFS SAT modified to use different offsets
 *           into the first page..
 *  Module(s): device/ds_routines.c , vm/vm_map.c vm/vm_map.h
 *
 * Revision 1.22  1994/11/18  20:31:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.21  1994/10/04  15:57:06  jerrie
 *  Reviewer: Arlin Davis
 *  Risk: Low. Only affects scatter/gather to block devices.
 *  Benefit or PTS #: PTS 10477 IPI-3 Raw I/O hangs or returns I/O error if
 * 	data length is not a multiple of the device block size.
 *  Testing: Used two of Evaluation's tests and created one of my own to
 * 	walk through various combinations of block sizes and offsets.
 * 	Ran Evaluation's HiPPI and IPI-3 EATs on a kernel with the
 * 	fixes in place.
 *  Module(s): vm/vm_kern.c, device/ds_routines.c, ipi/ipi_disk.c
 * 	ipi/ipi_labels.c, ipi/ipi-3.c
 *
 * Revision 1.20  1994/07/12  19:17:48  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.19  1994/07/07  16:56:21  stans
 *   Function 'ds_no_senders()'
 *     Now that the last device port leak has been fixed, a NMS (No-More-Senders)
 *     notification is correctly delivered. Problem is that OSF never completed
 *     the NMS handler routine beyond a debug printf(). The user command 'halt'
 *     forced the death of a task holding the last send right for a device
 *     port. The MK handled the NMS by calling ds_no_senders() which output an
 *     ugly debug printf() "ds_no_senders called! device_port=0x%x count=%d\n".
 *     ds_no_senders() now verifies the device is NOT closing (normal NMS) and
 *     then calls ds_device_close() on behalf of the terminating task.
 *
 *  Reviewer: self
 *  Risk: low
 *  Benefit or PTS #:10133
 *  Testing: developer
 *
 * Revision 1.18  1994/06/29  23:15:26  stans
 *  In function 'ds_open_done()', remove call to convert_device_to_port()
 *  as it adds an extra send-right + reference; leaks the device port on
 *  device close.
 *
 *  Reviewer: self
 *  Risk: low
 *  Benefit or PTS #: plug port leak.
 *  Testing: developer
 *
 * Revision 1.17  1994/01/26  18:17:22  stans
 *  Removed "export_paging" as it's no longer used.
 *  Reviewer: self
 *  Risk: low
 *  Benefit or PTS #: cleanup/size-reduction
 *  Testing: none
 *
 * Revision 1.16.2.5  1994/07/07  00:37:43  stans
 *   Routine 'ds_no_senders()'
 * 	Remove debug printf() and if the device is not being closed then
 * 	call device_close() on behalf of the task which died without calling
 * 	device_close().  'halt' command triggers the no-more-senders
 * 	notification.
 *
 * Revision 1.16.2.4  1994/06/29  23:27:13  andyp
 * Make the I/O done thread fixpri 0 by default.  (To fix a priority
 * inversion seen on an L35 system).
 *
 * Revision 1.16.2.3  1994/06/25  01:12:40  stans
 *   ds_device_open()
 * 	Plugged device port leak.
 *
 * Revision 1.16.2.2  1994/05/18  22:34:48  stans
 * NORMA_IPC==0 & old iPSC860 debug removed
 *
 * Revision 1.16.2.1  1994/02/16  00:21:34  andyp
 * Updates from the mainline.
 *
 * Revision 1.17  1994/01/26  18:17:22  stans
 *  Removed "export_paging" as it's no longer used.
 *  Reviewer: self
 *  Risk: low
 *  Benefit or PTS #: cleanup/size-reduction
 *  Testing: none
 *
 * Revision 1.16  1993/12/21  18:55:23  dleslie
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: remove (with #if 0) assignments of vars to themselves so
 * 	lint won't see fatal errors; this hack was probably put in in the
 * 	first place to shut lint up
 *  Testing: built, ran lint successfully
 *  Module(s): ds_routines.c
 *
 * Revision 1.15  1993/12/03  00:58:04  terry
 * backing out rev 1.14 of this file.  This had a severe side affect.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.13  1993/09/28  17:55:12  andyp
 * Update for the R1.2 release.
 *
 *
 *	Allow the iodone thread to have an optional fixed priority
 *	scheduling policy, with a specified priority level.  This
 *	guarantees that the iodone thread won't be disadvantaged
 *	when processing interrupt completions.
 *	[andyp@ssd.intel.com, alanl@osf.org]
 *
 * Revision 1.12  1993/07/08  16:31:22  rkl
 * Fixes panic when I/O request is greater than a page list and iodone()
 * is called from the strategy routine (i.e. write to passthrough device).
 * Fix supplied by David Black
 *
 * Revision 1.11  1993/06/30  22:22:12  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.10  1993/06/11  00:04:29  arlin
 * add ds_master_notify to support deadname notification
 * on master device port
 *
 * Revision 1.9  1993/06/09  01:27:06  terry
 * source sync with OSF
 *
 * Revision 1.8  1993/06/03  23:31:01  richardg
 * Removed the 2k kludge and localized it in scsi driver.
 *
 * Revision 1.7  1993/05/27  22:27:34  arlin
 * scatter/gather i/o for Hippi Driver
 *
 * Revision 1.6  1993/05/24  22:28:39  stans
 *    Device forwarding has finally gone away!!
 *
 * Revision 1.5  1993/05/05  17:56:55  richardg
 * Added support for 2k DEV_BSIZE
 *
 * Revision 2.26.2.9  92/09/15  18:16:27  jeffreyh
 * 	Device forward changes. 
 * 	[92/09/15            jeffreyh]
 * 
 * Revision 2.26.2.8  92/09/15  17:14:34  jeffreyh
 * 	Dev forward changes from Intel
 * 	[92/09/10            jeffreyh]
 * 
 * 22-Jun-92  Alan Langerman (alanl) at Open Software Foundation
 *    Add assertions verifying copy object is of page list flavor.
 *
 * Revision 2.26.2.7  92/05/28  18:16:43  jeffreyh
 * 	Change arguments to remote_device to pass new type field.
 * 
 * Revision 2.26.2.6  92/03/28  10:04:41  jeffreyh
 * 	Calculate minimum transfer size in device_write_get and
 * 	pass it to kmem_io_map_copyout.  This makes large block
 * 	sizes work with tapes.
 * 	[92/03/20  14:09:52  dlb]
 * 
 * Revision 2.26.2.5  92/03/03  16:13:49  jeffreyh
 * 	Increase DEVICE_IO_MAP_SIZE to 2 megs
 * 	[92/03/03  13:53:25  jeffreyh]
 * 
 * 	Change panic message to match previous change.
 * 	[92/02/21  10:11:55  dlb]
 * 
 * 	Use page lists for device reads going to default pager.
 * 	[92/02/20  15:16:19  dlb]
 * 
 * 	Temporary change to return data read from devices as a page
 * 	list instead of an entry list.  The keep_wired logic has
 * 	to be updated to convert the default pager to this mechanism
 * 	when making this change permanent.
 * 	[92/02/19  17:36:50  dlb]
 * 	[David L. Black 92/02/22  17:03:11  dlb@osf.org]
 * 		Check protection argument to device_map.
 * 
 * Revision 2.26.2.4  92/02/18  18:39:30  jeffreyh
 * 	Increased DEVICE_IO_MAP_SIZE to 1 Meg
 * 	[91/12/06            bernadat]
 * 
 * Revision 2.26.2.3  92/01/21  21:49:45  jsb
 * 	Added ds_notify and ds_no_senders routines in preparation for using
 * 	no-senders notifications to close unreferenced devices.
 * 	[92/01/21  18:15:38  jsb]
 * 
 * Revision 2.26.2.2  92/01/09  18:43:29  jsb
 * 	Use remote_device() instead of norma_get_special_port().
 * 	[92/01/04  18:15:21  jsb]
 * 
 * Revision 2.26.2.1  92/01/03  16:34:41  jsb
 * 	Corrected log.
 * 	[91/12/24  13:51:07  jsb]
 * 
 * Revision 2.26  91/12/10  13:25:33  jsb
 * 	Merged in 2.25.1.1 from dlb.
 * 	[91/12/10  11:16:31  jsb]
 * 
 * Revision 2.25.1.1  91/12/05  10:55:51  dlb
 * 	4-Dec-91 David L. Black (dlb) at Open Software Foundation
 * 	Change ds_read_done to call vm_map_copyin_page_list directly
 * 	if destination of reply is remote.
 * 
 * Revision 2.25  91/11/14  16:52:41  rpd
 * 	Replaced master_device_port_at_node call with
 *	call to norma_get_special_port.
 * 	[91/11/00  00:00:00  jsb]
 * 
 * Revision 2.24  91/10/09  16:05:37  af
 * 	Fixed device_write_get to check kmem_io_map_copyout return code.
 * 	Enabled wait_for_space in device_io_map.
 * 	[91/09/17            rpd]
 * 
 * Revision 2.23  91/09/12  16:37:22  bohman
 * 	Changed device_write_inband() to not require a reply port.
 * 	Fixed device_write_get() to allow inband calls.  In this case,
 * 	an io_inband buffer is allocated and the data is copied into it.
 * 	Fixed device_write_dealloc() to correctly deallocate io_inband
 * 	buffers.
 * 	Fixed ds_read_done() to free io_inband buffers only if one was
 * 	actually allocated.
 * 	[91/09/11  17:06:50  bohman]
 * 
 * Revision 2.22  91/08/28  11:11:16  jsb
 * 	From rpd: increased DEVICE_IO_MAP_SIZE; documented why
 * 	device_write_get cannot be used for in-band data.
 * 	[91/08/22  15:28:19  jsb]
 * 
 * 	In device_write_get, always set the wait parameter to something;
 * 	by default, it's FALSE.
 * 	[91/08/16  14:19:31  jsb]
 * 
 * 	Support synchronous wait by writers when vm continuations are present.
 * 	Optimize device_write_dealloc.  Fix MP bug in iowait/iodone.
 * 	Convert from bsize to dev_info entry in device op vector.
 * 	[91/08/12  17:27:15  dlb]
 * 
 * 	Page lists working reliably: delete old code.
 * 	[91/08/06  17:16:09  dlb]
 * 
 * 	Clean up and add continuation support for device_write page lists.
 * 	[91/08/05  17:30:38  dlb]
 * 
 * 	First version of support for vm page lists in device_write.
 * 	Still needs cleanup and continuation support.  Old code left
 * 	under #ifdef 0.  
 * 	[91/07/31  14:42:24  dlb]
 * 
 * Revision 2.21  91/08/24  11:55:43  af
 * 	Spls definitions.
 * 	[91/08/02  02:44:45  af]
 * 
 * Revision 2.20  91/08/03  18:17:33  jsb
 * 	Device_write_get doesn't need to do anything for loaned ior's.
 * 	[91/08/02  12:13:15  jsb]
 * 
 * 	Create the right flavor of copy object in ds_read_done.
 * 	Replace NORMA_BOOT conditionals with NORMA_DEVICE.
 * 	Free loaned ior's directly in iodone().
 * 	[91/07/27  22:45:09  jsb]
 * 
 * Revision 2.19  91/06/25  10:26:57  rpd
 * 	Changed mach_port_t to ipc_port_t where appropriate.
 * 	Removed device_reply_search and device_reply_terminate.
 * 	[91/05/28            rpd]
 * 
 * Revision 2.18  91/06/17  15:43:58  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  09:58:59  jsb]
 * 
 * Revision 2.17  91/05/18  14:29:52  rpd
 * 	Added vm/memory_object.h.
 * 	[91/03/22            rpd]
 * 
 * Revision 2.16  91/05/14  15:47:34  mrt
 * 	Correcting copyright
 * 
 * Revision 2.15  91/03/16  14:43:02  rpd
 * 	Updated for new kmem_alloc interface.
 * 	[91/03/03            rpd]
 * 	Added io_done_thread_continue.
 * 	[91/02/13            rpd]
 * 	Removed thread_swappable.
 * 	[91/01/18            rpd]
 * 
 * Revision 2.14  91/02/05  17:09:25  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:28:57  mrt]
 * 
 * Revision 2.13  91/01/08  15:09:38  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.12  90/12/14  10:59:39  jsb
 * 	Moved mechanism for mapping global to local device names
 * 	to the machine-dependent function dev_forward_name.
 * 	[90/12/14  09:37:18  jsb]
 * 
 * 	Added device request forwarding to support inter-node device access.
 * 	[90/12/14  08:30:53  jsb]
 * 
 * Revision 2.11  90/10/25  14:44:32  rwd
 * 	Let ds_device_write proceed w/o a valid reply port.  This is used
 * 	by the unix server ether_output routine.
 * 	[90/10/22            rwd]
 * 	Fixed ds_write_done to use ds_device_write_inband_reply
 * 	when appropriate.
 * 	[90/10/18            rpd]
 * 	Check for invalid reply ports.
 * 	[90/10/17            rwd]
 * 
 * Revision 2.10  90/09/09  14:31:27  rpd
 * 	Use decl_simple_lock_data.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.9  90/06/02  14:48:00  rpd
 * 	Cleaned up check for default pager in ds_read_done.
 * 	[90/04/29            rpd]
 * 
 * 	Fixed ds_read_done to leave memory wired if the read reply
 * 	is being sent to the default pager.
 * 	[90/04/05            rpd]
 * 	Converted to new IPC.  Purged MACH_XP_FPD.
 * 	[90/03/26  21:55:28  rpd]
 * 
 * Revision 2.8  90/02/22  20:02:12  dbg
 * 	Use vm_map_copy routines.
 * 	[90/01/25            dbg]
 * 
 * Revision 2.7  90/01/11  11:42:01  dbg
 * 	De-lint.
 * 	[89/12/06            dbg]
 * 
 * Revision 2.6  89/11/29  14:08:54  af
 * 	iodone() should set the IO_DONE flag.
 * 	[89/11/03  16:58:16  af]
 * 
 * Revision 2.5  89/11/14  10:28:19  dbg
 * 	Make read and write handle zero-length transfers correctly (used
 * 	to implement select).
 * 	[89/10/27            dbg]
 * 
 * Revision 2.4  89/09/08  11:24:17  dbg
 * 	Converted to run in kernel context.
 * 	Add list of wired pages to tail of IOR allocated for write.
 * 	Reorganized file: moved open/close to beginning, map to end.
 * 	[89/08/23            dbg]
 * 
 * Revision 2.3  89/08/31  16:18:46  rwd
 * 	Added ds_read_inband and support
 * 	[89/08/15            rwd]
 * 
 * Revision 2.2  89/08/05  16:06:39  rwd
 * 	Added ds_write_inband for use by tty and ds_device_map_device.
 * 	[89/07/17            rwd]
 * 
 * 12-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Added device_reply_terminate.
 *
 *  3-Mar-89  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */
/* CMU_ENDHIST */
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
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	3/89
 */

#include <mach_assert.h>
#include <norma_device.h>
#include <norma_ipc.h>

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/mig_errors.h>
#include <mach/port.h>
#include <mach/vm_param.h>
#include <mach/notify.h>
#include <machine/machparam.h>		/* spl definitions */

#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>

#include <kern/ast.h>
#include <kern/counters.h>
#include <kern/queue.h>
#include <kern/zalloc.h>
#include <kern/thread.h>
#include <kern/task.h>
#include <kern/sched_prim.h>

#include <vm/memory_object.h>
#include <vm/pmap.h>
#include <vm/vm_map.h>
#include <vm/vm_kern.h>

#include <device/device_types.h>
#include <device/dev_hdr.h>
#include <device/conf.h>
#include <device/io_req.h>
#include <device/ds_routines.h>
#include <device/net_status.h>
#include <device/device_port.h>
#include <device/device_reply.h>

#include <machine/machparam.h>

#if	PARAGON860 || iPSC386 || iPSC860
/*
 * mk node address where root filesystem device is. See autoconf.c for
 * initialization.
 */
int	root_dev_node;
int	root_fs_node;
#endif	PARAGON860 || iPSC386 || iPSC860


#if	MACH_ASSERT
void
db_show_device(device_t dev)
{
	if ( dev == DEVICE_NULL )
		return;
	db_printf("device 0x%x\n",dev);
	db_printf("  ref_count %d state %d flags 0x%x open_count %d\n",
		dev->ref_count,
		dev->state,
		dev->flag,
		dev->open_count);
	db_printf("  io_in_progress %d io_wait %s port 0x%x dev_num %d\n",
		dev->io_in_progress,
		(dev->io_wait ? "True" : "False"),
		dev->port,
		dev->dev_number);
}
#endif	/* MACH_ASSERT */


io_return_t
ds_device_open(open_port, reply_port, reply_port_type,
	       mode, name, device_p)
	ipc_port_t	open_port;
	ipc_port_t	reply_port;
	mach_msg_type_name_t reply_port_type;
	dev_mode_t	mode;
	char *		name;
	device_t	*device_p;	/* out */
{
	register device_t	device;
	register kern_return_t	result;
	register io_req_t	ior;
	char			namebuf[64];
	ipc_port_t		notify;
	static int		mynode= -1; /* whoami ? */


	/*
	 * Open must be called on the master device port.
	 */
	if (open_port != master_device_port)
	    return (D_INVALID_OPERATION);

	/*
	 * There must be a reply port.
	 */
	if (!IP_VALID(reply_port)) {
	    printf("ds_* invalid reply port\n");
	    Debugger("ds_* reply_port");
	    return (MIG_NO_REPLY);	/* no sense in doing anything */
	}

#if	NORMA_DEVICE
	if ( mynode == -1 )
		mynode = node_self();
	/*
	 * Translate device name if needed:
	 * With the server sending to the correct MK, device forwarding becomes
	 * an issue only when booting; trying to read pagingfile & server
	 * from the boot file system.
	 */
	/*
	 * Map global device name to <node> + local device name.
	 */
	if (name[0] != '<') {
		extern char *dev_forward_name();

		name = dev_forward_name(name, namebuf, sizeof(namebuf));
	}

	/*
	 * Look for explicit node specifier, e.g., <2>sd0a.
	 * If found, then forward request to correct device server.
	 * If not found, then remove '<n>' and process locally.
	 *
	 * XXX should handle send-right reply_port as well as send-once XXX
	 */
	if (name[0] == '<') {
		char *n;
		int node = 0;

		for (n = &name[1]; *n != '>'; n++) {
			if (*n >= '0' && *n <= '9') {
				node = 10 * node + (*n - '0');
			} else {
				return (D_NO_SUCH_DEVICE);
			}
		}
		if (node == mynode) {
			name = &n[1];	/* skip trailing '>' */
		} else {
			forward_device_open_send(remote_device(node,
						      MACH_MSG_TYPE_PORT_SEND),
						 reply_port, mode, name);
			return (MIG_NO_REPLY);
		}
	}
#endif	NORMA_DEVICE

	/*
	 * Find the local device.
	 * If not DEVICE_NULL then device has a reference.
	 */
	device = device_lookup(name);
	if (device == DEVICE_NULL) {
	    return (D_NO_SUCH_DEVICE);
	}

	/*
	 * If the device is being opened or closed,
	 * wait for that operation to finish.
	 */
	device_lock(device);
	while (device->state == DEV_STATE_OPENING ||
		device->state == DEV_STATE_CLOSING) {
	    device->io_wait = TRUE;
	    thread_sleep((int)device, simple_lock_addr(device->lock), TRUE);
	    device_lock(device);
	}

	/*
	 * If the device is already open, increment the open count
	 * and return.
	 */
	if (device->state == DEV_STATE_OPEN) {

	    if (device->flag & D_EXCL_OPEN) {
		/*
		 * Cannot open a second time.
		 */
		device_unlock(device);
		device_deallocate(device);
		return (D_ALREADY_OPEN);
	    }

	    device->open_count++;
	    device_unlock(device);
	    *device_p = device;
	    return (D_SUCCESS);
	    /*
	     * Return deallocates device reference while acquiring
	     * port.
	     */
	}

	/*
	 * Allocate the device port and register the device before
	 * opening it.
	 */
	device->state = DEV_STATE_OPENING;
	device_unlock(device);

	/*
	 * Allocate port, keeping a reference for it.
	 */
	device->port = ipc_port_alloc_kernel();
	if (device->port == IP_NULL) {
	    device_lock(device);
	    device->state = DEV_STATE_INIT;
	    device->port = IP_NULL;
	    if (device->io_wait) {
		device->io_wait = FALSE;
		thread_wakeup((int)device);
	    }
	    device_unlock(device);
	    device_deallocate(device);
	    return (KERN_RESOURCE_SHORTAGE);
	}

	dev_port_enter(device);	/* adds device reference for IPC port */

	/*
	 * Request no-senders notifications on device port.
	 */
	notify = ipc_port_make_sonce(device->port);
	ip_lock(device->port);
	ipc_port_nsrequest(device->port, 1, notify, &notify);
	assert(notify == IP_NULL);

	/*
	 * Open the device.
	 */
	io_req_alloc(ior, 0);

	ior->io_device	= device;
	ior->io_unit	= device->dev_number;
	ior->io_op	= IO_OPEN | IO_CALL;
	ior->io_mode	= mode;
	ior->io_error	= 0;
	ior->io_done	= ds_open_done;
	ior->io_reply_port = reply_port;
	ior->io_reply_port_type = reply_port_type;

	result = (*device->dev_ops->d_open)(device->dev_number, (int)mode, ior);
	if (result == D_IO_QUEUED)
	    return (MIG_NO_REPLY);

	/*
	 * Return result via ds_open_done.
	 */
	ior->io_error = result;
	(void) ds_open_done(ior);

	io_req_free(ior);

	return (MIG_NO_REPLY);	/* reply already sent */
}

boolean_t
ds_open_done(ior)
	register io_req_t	ior;
{
	kern_return_t		result;
	register device_t	device;

	device = ior->io_device;
	result = ior->io_error;

	if (result != D_SUCCESS) {
	    /*
	     * Open failed.  Deallocate port and device.
	     */
	    dev_port_remove(device);
	    ipc_port_dealloc_kernel(device->port);
	    device->port = IP_NULL;

	    device_lock(device);
	    device->state = DEV_STATE_INIT;
	    if (device->io_wait) {
		device->io_wait = FALSE;
		thread_wakeup((int)device);
	    }
	    device_unlock(device);

	    device_deallocate(device);
	    device = DEVICE_NULL;
	}
	else {
	    /*
	     * Open succeeded.
	     */
	    device_lock(device);
	    device->state = DEV_STATE_OPEN;
	    device->open_count = 1;
	    if (device->io_wait) {
		device->io_wait = FALSE;
		thread_wakeup((int)device);
	    }
	    device_unlock(device);

	    /* donate device reference to get port */
	}

	/*
	 * Must explicitly convert device to port, since
	 * device_reply interface is built as 'user' side
	 * (thus cannot get translation).
	 */
	if (IP_VALID(ior->io_reply_port)) {
		ipc_port_t dev_port = IP_NULL;

		/*
		 * perform the interesting aspects of convert_device_to_port()
		 * without adding an extra-send right/reference. The
		 * 'device->port', by virture of the device_reply.defs
		 * definition of ds_device_open_reply() 'mach_port_make_send_t'
		 * will generate the send right and associated reference.
		 *	see
		 *		mach_msg_send_from_kernel()
		 *		ipc_object_copyin_from_kernel()
		 */
		if ( device != DEVICE_NULL ) {
			device_lock(device);
			if ( device->state == DEV_STATE_OPEN )
				dev_port = device->port;
			device_unlock(device);
			/* lose reference from device_enter() */
			device_deallocate(device);
		}

		(void) ds_device_open_reply(ior->io_reply_port,
					    ior->io_reply_port_type,
					    result,
					    dev_port);
	} else
		device_deallocate(device);

	return (TRUE);
}

io_return_t
ds_device_close(device)
	register device_t	device;
{
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	device_lock(device);

	/*
	 * If device will remain open, do nothing.
	 */
	if (--device->open_count > 0) {
	    device_unlock(device);
	    return (D_SUCCESS);
	}

	/*
	 * If device is being closed, do nothing.
	 */
	if (device->state == DEV_STATE_CLOSING) {
	    device_unlock(device);
	    return (D_SUCCESS);
	}

	/*
	 * Mark device as closing, to prevent new IO.
	 * Outstanding IO will still be in progress.
	 */
	device->state = DEV_STATE_CLOSING;
	device_unlock(device);

	/*
	 * ? wait for IO to end ?
	 *   only if device wants to
	 */

	/*
	 * Remove the device-port association.
	 */
	dev_port_remove(device);
	ipc_port_dealloc_kernel(device->port);

	/*
	 * Close the device
	 */
	(*device->dev_ops->d_close)(device->dev_number);

	/*
	 * Finally mark it closed.  If someone else is trying
	 * to open it, the open can now proceed.
	 */
	device_lock(device);
	device->state = DEV_STATE_INIT;
	if (device->io_wait) {
	    device->io_wait = FALSE;
	    thread_wakeup((int)device);
	}

	device_unlock(device);

	return (D_SUCCESS);
}

/*
 * Write to a device.
 */

io_return_t
ds_device_write_common(device, reply_port, reply_port_type, mode, recnum,
		data, data_count, sync, bytes_written)
	register device_t	device;
	ipc_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	dev_mode_t		mode;
	recnum_t		recnum;
	io_buf_ptr_t		data;
	unsigned int		data_count;
	boolean_t		sync;		/* caller will wait sync */
	int			*bytes_written;	/* out */
{
	register io_req_t	ior;
	register io_return_t	result;

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/*
	 * XXX Need logic to reject ridiculously big requests.
	 */

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * Package the write request for the device driver
	 */
	io_req_alloc(ior, data_count);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= (sync ? (IO_WRITE | IO_SYNC) : (IO_WRITE | IO_CALL));
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_data		= data;
	ior->io_count		= data_count;
	ior->io_total		= data_count;
	ior->io_alloc_size	= 0;
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_write_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type	= reply_port_type;
	ior->io_copy		= VM_MAP_COPY_NULL;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

	if (data == 0)
	    panic("ds_device_write: no data");

	/*
	 * And do the write ...
	 *
	 * device_write_dealloc returns false if there's more
	 * to do; it has updated the ior appropriately and expects
	 * its caller to reinvoke it on the device.
	 */

	do {

		result = (*device->dev_ops->d_write)(device->dev_number, ior);

		/*
		 * If the IO was queued, delay reply until it is finished.
		 */
		if (result == D_IO_QUEUED) {
			assert(!sync);
			return (MIG_NO_REPLY);
		}
		/*
		 * Discard the local mapping of the data.
		 */

	} while (!device_write_dealloc(ior));

	/*
	 * For Synchronous IO, return status.
	 */
        if(ior->io_error) /* check if there was an error set rag */
			result = ior->io_error;

	/*
	 * Return the number of bytes actually written.
	 */
	*bytes_written = ior->io_total - ior->io_residual;

	/*
	 * Remove the extra reference.
	 */
	device_deallocate(device);

	io_req_free(ior);
	return (result);
}

/*
 * Write to a device.
 */
io_return_t
ds_device_write(device, reply_port, reply_port_type, mode, recnum,
		data, data_count, bytes_written)
	register device_t	device;
	ipc_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	dev_mode_t		mode;
	recnum_t		recnum;
	io_buf_ptr_t		data;
	unsigned int		data_count;
	int			*bytes_written;	/* out */
{
	return(ds_device_write_common(device, reply_port, reply_port_type,
			       mode, recnum, data, data_count, FALSE, bytes_written));
}

/*
 * Write to a device, but memory is in message.
 */
io_return_t
ds_device_write_inband(device, reply_port, reply_port_type, mode, recnum,
		       data, data_count, bytes_written)
	register device_t	device;
	ipc_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	dev_mode_t		mode;
	recnum_t		recnum;
	io_buf_ptr_inband_t	data;
	unsigned int		data_count;
	int			*bytes_written; /* out */
{
	register io_req_t	ior;
	register io_return_t	result;


	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * Package the write request for the device driver.
	 */
	io_req_alloc(ior, 0);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= IO_WRITE | IO_CALL | IO_INBAND;
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_data		= data;
	ior->io_count		= data_count;
	ior->io_total		= data_count;
	ior->io_alloc_size	= 0;
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_write_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type = reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

	if (data == 0)
	    panic("ds_device_write: no data");

	/*
	 * And do the write.
	 */
	result = (*device->dev_ops->d_write)(device->dev_number, ior);

	/*
	 * If the IO was queued, delay reply until it is finished.
	 */
	if (result == D_IO_QUEUED)
	    return (MIG_NO_REPLY);

	/*
	 * Return the number of bytes actually written.
	 */
	*bytes_written = ior->io_total - ior->io_residual;

	/*
	 * Remove the extra reference.
	 */
	device_deallocate(device);

	io_req_free(ior);
	return (result);
}

/*
 * Wire down incoming memory to give to device in scatter/gather list form.
 */
kern_return_t
device_write_get_sg(ior, wait)
	register io_req_t	ior;
	boolean_t		*wait;
{
	vm_map_copy_t		io_copy;
        struct io_sglist        *sgp;
	register kern_return_t	result;
	kern_return_t           kr;
	int			bsize;
	vm_size_t		min_size;

	/*
	 * By default, caller does not have to wait.
	 */
	*wait = FALSE;


	/*
	 * Nothing to do if no data.
	 */
	if (ior->io_count == 0)
	    return (KERN_SUCCESS);

	/*
	 * Loaned iors already have valid data.
	 */
	if (ior->io_op & IO_LOANED)
	    return (KERN_SUCCESS);

	/*
	 *	Figure out how much data to move this time.  If the device
	 *	won't return a block size, then we have to do the whole
	 *	request in one shot (ditto if this is a block fragment),
	 *	otherwise, move at least one block's worth.
	 */
	result = (*ior->io_device->dev_ops->d_dev_info)(
					ior->io_device->dev_number,
					D_INFO_BLOCK_SIZE,
					&bsize);

	if (result != KERN_SUCCESS)
		min_size  = (vm_size_t) ior->io_count;
	else if (ior->io_count % (vm_size_t) bsize) { 
		min_size  = (vm_size_t) ior->io_count;
		min_size += (vm_size_t) (bsize - ior->io_count % bsize);
	} 
	else 
		min_size  = (vm_size_t) ior->io_count;

	/*
	 *	Collect the pages from this page list into a scatter/gather list.
	 *	io_data will point to the scatter/gather list.
	 *	io_alloc_size of the total length of the transfer.
	 *      ARD - make sure message is VM_MAP_COPY_PAGE_LIST type.
	 */
	io_copy = (vm_map_copy_t) ior->io_data;

#if	NORMA_IPC || NORMA2
	if (io_copy->type != VM_MAP_COPY_PAGE_LIST) {
		assert(io_copy->type == VM_MAP_COPY_OBJECT);
		kr = vm_map_object_to_page_list(&io_copy);
		assert(kr == KERN_SUCCESS);
		ior->io_data = (io_buf_ptr_t) io_copy;
	}
#endif	/* NORMA_IPC || NORMA2 */
	assert(io_copy->type == VM_MAP_COPY_PAGE_LIST);

	

        result = kmem_io_page_list_to_sglist(io_copy, &sgp,
					     &ior->io_alloc_size, min_size);
        if (result != KERN_SUCCESS) {
                return (result);
        }

        ior->io_sgp = sgp;

	if (ior->io_count > sgp->iosg_hdr.length) {

		/*
		 *	Operation has to be split.  Reset io_count for how
		 *	much we can do this time.
		 */
		assert(vm_map_copy_has_cont(io_copy));
		assert(ior->io_count == io_copy->size);
		ior->io_count = sgp->iosg_hdr.length;

		/*
		 *	Caller must wait synchronously.
		 */
		ior->io_op &= ~IO_CALL;
		*wait = TRUE;		
	}
	ior->io_copy = io_copy;			/* vm_map_copy to discard */
	return (KERN_SUCCESS);
}

/*
 * Wire down incoming memory to give to device.
 */
kern_return_t
device_write_get(ior, wait)
	register io_req_t	ior;
	boolean_t		*wait;
{
	vm_map_copy_t		io_copy;
	vm_offset_t		new_addr;
	register kern_return_t	result;
	kern_return_t		kr;
	int			bsize;
	vm_size_t		min_size;
	extern kern_return_t   vm_map_copyin_page_list_cont();

	/*
	 * By default, caller does not have to wait.
	 */
	*wait = FALSE;

	/*
	 * Nothing to do if no data.
	 */
	if (ior->io_count == 0)
	    return (KERN_SUCCESS);

	/*
	 * Loaned iors already have valid data.
	 */
	if (ior->io_op & IO_LOANED)
	    return (KERN_SUCCESS);

	/*
	 * Inband case.
	 */
	if (ior->io_op & IO_INBAND) {
	    assert(ior->io_count <= sizeof (io_buf_ptr_inband_t));
	    new_addr = zalloc(io_inband_zone);
	    bcopy(ior->io_data, new_addr, ior->io_count);
	    ior->io_data = (io_buf_ptr_t)new_addr;
	    ior->io_alloc_size = sizeof (io_buf_ptr_inband_t);

	    return (KERN_SUCCESS);
	}
	else {
		boolean_t sgio = FALSE;
		/*
		 *	Figure out if this device can to scatter/gather I/O.
		 */
		result = (*ior->io_device->dev_ops->d_dev_info)(
					ior->io_device->dev_number,
					D_INFO_SGLIST_IO,
					&sgio);

		if ((result == KERN_SUCCESS) && sgio) {
			/*
			 * Yes, note it and do the setup.
			 */
			ior->io_op |= IO_SGLIST;
			return (device_write_get_sg(ior, &wait));
		}
        }

	/*
	 *	Figure out how much data to move this time.  If the device
	 *	won't return a block size, then we have to do the whole
	 *	request in one shot (ditto if this is a block fragment),
	 *	otherwise, move at least one block's worth.
	 */
	result = (*ior->io_device->dev_ops->d_dev_info)(
					ior->io_device->dev_number,
					D_INFO_BLOCK_SIZE,
					&bsize);

	if (result != KERN_SUCCESS || ior->io_count < (vm_size_t) bsize)
		min_size = (vm_size_t) ior->io_count;
	else
		min_size = (vm_size_t) bsize;

	/*
	 *	Map the pages from this page list into memory.
	 *	io_data records location of data.
	 *	io_alloc_size is the vm size of the region to deallocate.
	 */
	io_copy = (vm_map_copy_t) ior->io_data;
#if	NORMA_IPC || NORMA2
	if (io_copy->type != VM_MAP_COPY_PAGE_LIST) {
		assert(io_copy->type == VM_MAP_COPY_OBJECT);
		kr = vm_map_object_to_page_list(&io_copy);
		assert(kr == KERN_SUCCESS);
		ior->io_data = (io_buf_ptr_t) io_copy;
	}
#endif	/* NORMA_IPC || NORMA2 */
	assert(io_copy->type == VM_MAP_COPY_PAGE_LIST);
	result = kmem_io_map_copyout(device_io_map, &ior->io_data, &new_addr,
				     &ior->io_alloc_size, io_copy, min_size);
	if (result != KERN_SUCCESS)
	    return (result);

	if ((ior->io_data + ior->io_count) > 
	    (((char *)new_addr) + ior->io_alloc_size)) {

		/*
		 *	Operation has to be split.  Reset io_count for how
		 *	much we can do this time.
		 */
		assert(vm_map_copy_has_cont(io_copy));
		assert(ior->io_count == io_copy->size);
		ior->io_count = ior->io_alloc_size -
			(ior->io_data - ((char *)new_addr));

		
		/* There is a case when you have a request that starts
		   on a non-page aligned address and has a continuation
		   where the io_count can turn out to be a non-multiple of
		   a block size.
		   This will cause the data to be corrupted because the
		   SCSI device pads NULLS to fillout the request.
		   Inorder to fix this problem, the io_count must be
		   truncated to be a multiple of a block size.
		   The continuation arguments and the address map entries
		   must be adjusted to compensate for this change.

		 */

#define trunc_bsize(x)	(x & ~(bsize-1))
#define IO_COUNT_ADJUSTED 0x10

		/* I'm assuming the bsize is good */

		assert(bsize);
		assert(io_copy->cpy_cont == vm_map_copyin_page_list_cont);

 
		if (ior->io_count % bsize)
		  {

		    int temp_count,adjust_count;
		    vm_map_copyin_args_t cont_args;

		    /* save the current io_count */

		    temp_count = ior->io_count;      

		    ior->io_count=trunc_bsize(ior->io_count);

		    /* how much is left over */

		    adjust_count=temp_count-ior->io_count;

		    cont_args=(vm_map_copyin_args_t) io_copy->cpy_cont_args;

		    if(vm_map_adjust_entries(cont_args->map, io_copy->offset,
					cont_args->src_addr) != KERN_SUCCESS)
		      {
			panic("vm_map_adjust_entries failed");
		      }

		    /* fix up continuation args */

		    cont_args->src_addr -= adjust_count;
		    cont_args->src_len += adjust_count;

		    /* need to set a flag to enable deallocation
		       know that there was an adjustment made 
		       This is needed because the cpy_npages
		       field includes the last page and
		       we don't want in marked NOT Busy after it
		       is placed in the next page list 
		       We will utilize an UN-used bit in the 
		       continuation args protection field to 
		       accomplish this...
		       
		    */

		    cont_args->prot |= IO_COUNT_ADJUSTED;

		  }

		/*
		 *	Caller must wait synchronously.
		 */
		ior->io_op &= ~IO_CALL;
		*wait = TRUE;		
	}

	ior->io_copy = io_copy;			/* vm_map_copy to discard */
	return (KERN_SUCCESS);
}

/*
 * Clean up memory allocated for IO.
 */
boolean_t
device_write_dealloc(ior)
	register io_req_t	ior;
{
	vm_map_copy_t	new_copy = VM_MAP_COPY_NULL;
	register
	vm_map_copy_t	io_copy;
	kern_return_t	result;
	vm_offset_t	size_to_do;
	int		bsize;	
	extern kern_return_t   vm_map_copyin_page_list_cont(),
	                       kmem_io_sgpage_list_cleanup();

	if (ior->io_alloc_size == 0)
	    return (TRUE);

	/*
	 * Inband case.
	 */
	if (ior->io_op & IO_INBAND) {
	    zfree(io_inband_zone, ior->io_data);

	    return (TRUE);
	}
	
	if ((io_copy = ior->io_copy) == VM_MAP_COPY_NULL)
	    return (TRUE);
	assert(io_copy->type == VM_MAP_COPY_PAGE_LIST);

        if (ior->io_op & IO_SGLIST) {
                io_sglist_free(ior->io_sgp);
        } else {
	/*
	 *	To prevent a possible deadlock with the default pager,
	 *	we have to release space in the device_io_map before
	 *	we allocate any memory.  (Which vm_map_copy_invoke_cont
	 *	might do.)  See the discussion in ds_init.
	 */

	kmem_io_map_deallocate(device_io_map,
		     	 trunc_page(ior->io_data),
			(vm_size_t) ior->io_alloc_size);
        }

	if (vm_map_copy_has_cont(io_copy)) {

		/*
		 *	Remember how much is left, then 
		 *	invoke or abort the continuation.
		 */
		size_to_do = io_copy->size - ior->io_count;
		if (ior->io_error == 0) {

		    int io_adjusted_flag=0;
		    vm_map_copyin_args_t cont_args;

		     /* do this only if it is a copyin_page_list continuation */

		     if(io_copy->cpy_cont == vm_map_copyin_page_list_cont) {
                     		   	
		        cont_args=(vm_map_copyin_args_t) 
			              io_copy->cpy_cont_args;

		        /* has this continuation adjusted to make it
			   a multiple of a block size ?
			   If so, we need to fix up the cpy_npages 
			   so it does not clear the busy status of the last 
			   page of the previous io operation following the
			   the call to continuation
			 */

			   
		        if(cont_args->prot & IO_COUNT_ADJUSTED)
			  {
			    /* restore original protection in cont_args */
			      cont_args->prot &= (~IO_COUNT_ADJUSTED);
			      
			    /* set adjust flag to let us know to fix
			       the cpy_npages on return from continuation */

			      io_adjusted_flag++;
			    }
			} 
		  
			vm_map_copy_invoke_cont(io_copy, &new_copy, &result);

		        /* The busy bit for the last page was
			   cleared before the continuation.
			   It was set for the next page list.
			   Leave it that way by not including it
			   in the vm_map_copy_discard..
			 */
			 
		        if(io_adjusted_flag)
			  {
			    io_copy->cpy_npages -= 1;
			  }
		}
		else {
			vm_map_copy_abort_cont(io_copy);
			result = KERN_FAILURE;
		}

		if (result == KERN_SUCCESS && new_copy != VM_MAP_COPY_NULL) {
			register int	res;

			/*
			 *	We have a new continuation, reset the ior to
			 *	represent the remainder of the request.  Must
			 *	adjust the recnum because drivers assume
			 *	that the residual is zero.
			 */
			ior->io_op &= ~IO_DONE;
			if ((ior->io_op & IO_SYNC) == 0)
				ior->io_op |= IO_CALL;

			res = (*ior->io_device->dev_ops->d_dev_info)(
					ior->io_device->dev_number,
					D_INFO_BLOCK_SIZE,
					&bsize);

			if (res != D_SUCCESS)
				panic("device_write_dealloc: No block size");
			
			ior->io_recnum += ior->io_count/bsize;
			ior->io_count = new_copy->size;
		}
		else {

			/*
			 *	No continuation.  Add amount we didn't get
			 *	to into residual.
			 */
			ior->io_residual += size_to_do;
		}
	}

	/*
	 *	Clean up the state for the IO that just completed.
	 */
        vm_map_copy_discard(ior->io_copy);
	ior->io_copy = VM_MAP_COPY_NULL;
	ior->io_data = (char *) new_copy;

	/*
	 *	Return FALSE if there's more IO to do.
	 */

	return(new_copy == VM_MAP_COPY_NULL);
}

/*
 * Send write completion message to client, and discard the data.
 */
boolean_t
ds_write_done(ior)
	register io_req_t	ior;
{
	/*
	 *	device_write_dealloc discards the data that has been
	 *	written, but may decide that there is more to write.
	 */
	while (!device_write_dealloc(ior)) {
		register io_return_t	result;
		register device_t	device;

		/*
		 *     More IO to do -- invoke it.
		 */
		device = ior->io_device;
		result = (*device->dev_ops->d_write)(device->dev_number, ior);

		/*
		 * If the IO was queued, return FALSE -- not done yet.
		 */
		if (result == D_IO_QUEUED)
		    return (FALSE);
	}

	/*
	 *	Now the write is really complete.  Send reply.
	 */

	if (!(ior->io_op & IO_SYNC) && IP_VALID(ior->io_reply_port)) {
	    (void) (*((ior->io_op & IO_INBAND) ?
		      ds_device_write_reply_inband :
		      ds_device_write_reply))(ior->io_reply_port,
					      ior->io_reply_port_type,
					      ior->io_error,
					      (int) (ior->io_total -
						     ior->io_residual));
	}
	device_deallocate(ior->io_device);
	
	return (TRUE);
}

/*
 * Read from a device.
 */
io_return_t
ds_device_read_common(device, reply_port, reply_port_type, mode, recnum,
	       bytes_wanted, sync, data, data_count)
	register device_t	device;
	ipc_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	dev_mode_t		mode;
	recnum_t		recnum;
	int			bytes_wanted;
	boolean_t		sync;
	io_buf_ptr_t		*data;		/* out */
	unsigned int		*data_count;	/* out */
{
	register io_req_t	ior;
	register io_return_t	result;

#if 0
#ifdef lint
	*data = *data;
	*data_count = *data_count;
#endif lint
#endif /* 0 */

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * There must be a reply port unless the call is synchronous.
	 */
	if (!sync && !IP_VALID(reply_port)) {
	    printf("ds_* invalid reply port\n");
	    Debugger("ds_* reply_port");
	    return (MIG_NO_REPLY);	/* no sense in doing anything */
	}

	/*
	 * Package the read request for the device driver
	 */
	io_req_alloc(ior, 0);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= (sync ? (IO_READ | IO_SYNC) : (IO_READ | IO_CALL));
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_data		= 0;		/* driver must allocate data */
	ior->io_count		= bytes_wanted;
	ior->io_alloc_size	= 0;		/* no data allocated yet */
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_read_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type	= reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

	/*
	 * And do the read.
	 */
	result = (*device->dev_ops->d_read)(device->dev_number, ior);

	/*
	 * If the IO was queued, delay reply until it is finished.
	 */
	if (result == D_IO_QUEUED) {
		assert(!sync);
		return (MIG_NO_REPLY);
	}

	/*
	 * Return result via ds_read_done.
	 * If there was a problem queueing the IO, report it.
	 * Otherwise, return status of IO.
	 */
	if (result != D_SUCCESS)
	ior->io_error = result;
	else
		result = ior->io_error;

	(void) ds_read_done(ior);
	*data = ior->io_data;
	*data_count = ior->io_total;
	io_req_free(ior);

	return (sync ? result : MIG_NO_REPLY);	/* reply has already been sent. */
}

/*
 * Read from a device with overwrite.
 */
io_return_t
ds_device_read_overwrite_common(device, reply_port, reply_port_type, mode, 
			        recnum, bytes_wanted, sync, data, data_count)
	register device_t	device;
	ipc_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	dev_mode_t		mode;
	recnum_t		recnum;
	int			bytes_wanted;
	boolean_t		sync;
	vm_address_t		data;	
	unsigned int		*data_count;	/* out */
{
	register io_req_t	ior;
	register io_return_t	result;

#if 0
#ifdef lint
	*data = *data;
	*data_count = *data_count;
#endif lint
#endif /* 0 */

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * There must be a reply port.
	 */
	if (!sync && !IP_VALID(reply_port)) {
	    printf("ds_* invalid reply port\n");
	    Debugger("ds_* reply_port");
	    return (MIG_NO_REPLY);	/* no sense in doing anything */
	}

	/*
	 * Package the read request for the device driver
	 */
	io_req_alloc(ior, 0);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= (sync ? (IO_READ | IO_SYNC | IO_OVERWRITE) : 
					  (IO_READ | IO_CALL | IO_OVERWRITE));
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_uaddr		= data;		/* user address to overwrite */
	ior->io_map 		= current_thread()->task->map;
	ior->io_count		= bytes_wanted;
	ior->io_alloc_size	= 0;		/* no data allocated yet */
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_read_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type	= reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

	/*
	 * And do the read.
	 */
	result = (*device->dev_ops->d_read)(device->dev_number, ior);

	/*
	 * If the IO was queued, delay reply until it is finished.
	 */
	if (result == D_IO_QUEUED) {
		assert(!sync);
		return (MIG_NO_REPLY);
	}

	/*
	 * Return result via ds_read_done.
	 * If there was a problem queueing the IO, report it.
	 * Otherwise, return status of IO.
	 */
	if (result != D_SUCCESS)
		ior->io_error = result;
	else
		result = ior->io_error;

	(void) ds_read_done(ior);
	*data_count = ior->io_total;
	io_req_free(ior);

	return (sync ? result : MIG_NO_REPLY);	/* reply has already been sent. */
}

/*
 * Read from a device.
 */
io_return_t
ds_device_read(device, reply_port, reply_port_type, mode, recnum,
	       bytes_wanted, data, data_count)
	register device_t	device;
	ipc_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	dev_mode_t		mode;
	recnum_t		recnum;
	int			bytes_wanted;
	io_buf_ptr_t		*data;		/* out */
	unsigned int		*data_count;	/* out */
{
	return(ds_device_read_common(device, reply_port, reply_port_type,
			      mode, recnum, bytes_wanted, FALSE, data, data_count));
}

/*
 * Read from a device, but return the data 'inband.'
 */
io_return_t
ds_device_read_inband(device, reply_port, reply_port_type, mode, recnum,
		      bytes_wanted, data, data_count)
	register device_t	device;
	ipc_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	dev_mode_t		mode;
	recnum_t		recnum;
	int			bytes_wanted;
	char			*data;		/* pointer to OUT array */
	unsigned int		*data_count;	/* out */
{
	register io_req_t	ior;
	register io_return_t	result;

#if 0
#ifdef lint
	*data = *data;
	*data_count = *data_count;
#endif lint
#endif /* 0 */

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * There must be a reply port.
	 */
	if (!IP_VALID(reply_port)) {
	    printf("ds_* invalid reply port\n");
	    Debugger("ds_* reply_port");
	    return (MIG_NO_REPLY);	/* no sense in doing anything */
	}

	/*
	 * Package the read for the device driver
	 */
	io_req_alloc(ior, 0);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= IO_READ | IO_CALL | IO_INBAND;
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_data		= 0;		/* driver must allocate data */
	ior->io_count		= 
	    ((bytes_wanted < sizeof(io_buf_ptr_inband_t)) ?
		bytes_wanted : sizeof(io_buf_ptr_inband_t));
	ior->io_alloc_size	= 0;		/* no data allocated yet */
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_read_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type	= reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

	/*
	 * Do the read.
	 */
	result = (*device->dev_ops->d_read)(device->dev_number, ior);

	/*
	 * If the io was queued, delay reply until it is finished.
	 */
	if (result == D_IO_QUEUED)
	    return (MIG_NO_REPLY);

	/*
	 * Return result, via ds_read_done.
	 */
	ior->io_error = result;
	(void) ds_read_done(ior);
	io_req_free(ior);

	return (MIG_NO_REPLY);	/* reply has already been sent. */
}

/*
 * Allocate wired-down memory for device read with scatter/gather lists.
 */
kern_return_t device_read_alloc_sg(ior, size)
	register io_req_t	ior;
	register vm_size_t	size;
{
	vm_offset_t		addr;
	kern_return_t		kr;
        int                     nentries, ii;
        struct io_sglist        *sgp;
	vm_size_t	        xfer_length;

	/*
	 * If io_device field is not present we assume the allocate size
	 * is correct.  Otherwise, call d_dev_info to check the device
	 * block size and possibly adjust the allocate size.
	 */
	if (ior->io_device) {
		int		bsize;
		vm_size_t	min_size;

		/*
		 * Figure out how much data to move this time.  If the device
		 * won't return a block size, then we have to do the whole
		 * request in one shot (ditto if this is a block fragment),
		 * otherwise, move at least one block's worth.
		 */
		kr = (*ior->io_device->dev_ops->d_dev_info)(
						ior->io_device->dev_number,
						D_INFO_BLOCK_SIZE,
						&bsize);

		if (kr == KERN_SUCCESS) {
			if (ior->io_count % (vm_size_t) bsize) { 
				min_size  = (vm_size_t) ior->io_count;
				min_size += (vm_size_t) (bsize - ior->io_count % bsize);
			} 
			else {
				min_size  = (vm_size_t) ior->io_count;
			}
			min_size = round_page(min_size);
			if (size < min_size)
				size = min_size;
		}
	}

        xfer_length = size;
	size = round_page(size);
        nentries = size/PAGE_SIZE;
        io_sglist_alloc(sgp, nentries);

        kr = kmem_alloc(kernel_map, &addr, size);
        if (kr != KERN_SUCCESS) {
                io_sglist_free(sgp);
                return (kr);
        }

	ior->io_data = (io_buf_ptr_t) addr;
        ior->io_alloc_size = size;

        /*
         * Fill in the scatter/gather list.
         */
        for (ii = 0; ii < nentries; ii++) {
                sgp->iosg_list[ii].iosge_length = PAGE_SIZE;
                sgp->iosg_list[ii].iosge_phys =
                        pmap_extract(vm_map_pmap(kernel_map), addr);
                addr += PAGE_SIZE;
        }
        sgp->iosg_list[nentries-1].iosge_length -= (size - xfer_length);
        sgp->iosg_hdr.length = xfer_length;
        ior->io_sgp = sgp;
	ior->io_op |= IO_SGLIST;
	return(KERN_SUCCESS);
}

/*
 * Allocate wired-down memory for device read.
 */
kern_return_t device_read_alloc(ior, size)
	register io_req_t	ior;
	register vm_size_t	size;
{
	vm_offset_t		addr;
	kern_return_t		kr;
	boolean_t               sgio = FALSE;

	/*
	 * Nothing to do if no data.
	 */
	if (ior->io_count == 0)
	    return (KERN_SUCCESS);

	if (ior->io_op & IO_INBAND) {
	    ior->io_data = (io_buf_ptr_t) zalloc(io_inband_zone);
	    ior->io_alloc_size = sizeof(io_buf_ptr_inband_t);
	} else {
		/*
		 *	Figure out if this device can do scatter/gather I/O.
		 */
		kr = (*ior->io_device->dev_ops->d_dev_info)(
					ior->io_device->dev_number,
					D_INFO_SGLIST_IO,
					&sgio);

		if ((kr == KERN_SUCCESS) && sgio) {
			kr = device_read_alloc_sg(ior, size);
			if (kr != KERN_SUCCESS)
				return (kr);
		} else {
	    size = round_page(size);
	    kr = kmem_alloc(kernel_map, &addr, size);
	    if (kr != KERN_SUCCESS)
		return (kr);

	    ior->io_data = (io_buf_ptr_t) addr;
	    ior->io_alloc_size = size;
	}
	}

	return (KERN_SUCCESS);
}

boolean_t ds_read_done(ior)
	io_req_t	ior;
{
	vm_offset_t		start_data, end_data;
	vm_offset_t		start_sent, end_sent;
	register vm_size_t	size_read;

	if (ior->io_error)
	    size_read = 0;
	else
	    size_read = ior->io_count - ior->io_residual;

	start_data  = (vm_offset_t)ior->io_data;
	end_data    = start_data + size_read;

	start_sent  = (ior->io_op & IO_INBAND) ? start_data :
						trunc_page(start_data);
	end_sent    = (ior->io_op & IO_INBAND) ? 
		start_data + ior->io_alloc_size : round_page(end_data);

	/*
	 * Zero memory that the device did not fill.
	 */
	if (start_sent < start_data)
	    bzero((char *)start_sent, start_data - start_sent);
	if (end_sent > end_data)
	    bzero((char *)end_data, end_sent - end_data);


	/*
	 * Touch the data being returned, to mark it dirty.
	 * If the pages were filled by DMA, the pmap module
	 * may think that they are clean.
	 */
	{
	    register vm_offset_t	touch;
	    register int		c;

	    for (touch = start_sent; touch < end_sent; touch += PAGE_SIZE) {
		c = *(char *)touch;
		*(char *)touch = c;
	    }
	}

	/*
	 * Send the data to the reply port - this
	 * unwires and deallocates it.
	 */
	if (ior->io_op & IO_INBAND) {
		assert((ior->io_op & IO_SYNC) == 0);
		(void)ds_device_read_reply_inband(ior->io_reply_port,
					      ior->io_reply_port_type,
					      ior->io_error,
					      (char *) start_data,
					      size_read);
	}
	else {
		vm_map_copy_t copy;
		kern_return_t kr;

		kr = vm_map_copyin_page_list(kernel_map, start_data,
					     size_read, TRUE, TRUE,
					     &copy, FALSE, VM_PROT_READ,
					     FALSE);

		if (kr != KERN_SUCCESS)
			ior->io_error = kr;
		else if (ior->io_op & IO_OVERWRITE) {
			kr = vm_map_copy_overwrite_page_list(ior->io_map, ior->io_uaddr, copy, FALSE);
			if (kr != KERN_SUCCESS) {
				vm_map_copy_discard(copy);
		    		ior->io_error = kr;
				ior->io_total = 0;
			}
			else if (ior->io_op & IO_SYNC)
				ior->io_total = size_read;
			else {
	        		(void)ds_device_read_reply_overwrite(
					   ior->io_reply_port,
				           ior->io_reply_port_type,
				           ior->io_error,
				           size_read);
			}
		}
		else if (ior->io_op & IO_SYNC) {
			kr = vm_map_copyout_page_list(current_map(), &ior->io_data, copy);
			if (kr != KERN_SUCCESS) {
				ior->io_error = kr;
				ior->io_total = 0;
				vm_map_copy_discard(copy);
			}
			else
			ior->io_total = size_read;
		}
		else {
			(void)ds_device_read_reply(ior->io_reply_port,
						   ior->io_reply_port_type,
						   ior->io_error,
						   (char *) copy,
						   size_read);
		}
	}

	/*
	 * Free any memory that was allocated but not sent.
	 */
	if (ior->io_count != 0) {
	    if (ior->io_op & IO_INBAND) {
		if (ior->io_alloc_size > 0)
		    zfree(io_inband_zone, (vm_offset_t)ior->io_data);
	    } else {
		register vm_offset_t	end_alloc;

		end_alloc = start_sent + round_page(ior->io_alloc_size);
		if (end_alloc > end_sent)
				(void) vm_deallocate(kernel_map, end_sent,
					 end_alloc - end_sent);
                if (ior->io_op & IO_SGLIST) {
                        io_sglist_free(ior->io_sgp);
	    	}
	    }
	}

	device_deallocate(ior->io_device);

	return (TRUE);
}

io_return_t
ds_device_set_status(device, flavor, status, status_count)
	register device_t	device;
	int			flavor;
	dev_status_t		status;
	unsigned int		status_count;
{
	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	return ((*device->dev_ops->d_setstat)(device->dev_number,
					      flavor,
					      status,
					      status_count));
}

io_return_t
ds_device_get_status(device, flavor, status, status_count)
	register device_t	device;
	int			flavor;
	dev_status_t		status;		/* pointer to OUT array */
	unsigned int		*status_count;	/* out */
{
	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	return ((*device->dev_ops->d_getstat)(device->dev_number,
					      flavor,
					      status,
					      status_count));
}

io_return_t
ds_device_set_filter(device, receive_port, priority, filter, filter_count)
	register device_t	device;
	ipc_port_t		receive_port;
	int			priority;
	filter_t		filter[];	/* pointer to IN array */
	unsigned int		filter_count;
{
	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * Request is absurd if no receive port is specified.
	 */
	if (!IP_VALID(receive_port))
	    return (D_INVALID_OPERATION);

	return ((*device->dev_ops->d_async_in)(device->dev_number,
					       receive_port,
					       priority,
					       filter,
					       filter_count));
}

io_return_t
ds_device_map(device, protection, offset, size, pager, unmap)
	register device_t	device;
	vm_prot_t		protection;
	vm_offset_t		offset;
	vm_size_t		size;
	ipc_port_t		*pager;	/* out */
	boolean_t		unmap;	/* ? */
{
	if (protection & ~VM_PROT_ALL)
		return (KERN_INVALID_ARGUMENT);
	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	return (device_pager_setup(device, protection, offset, size,
				    pager));
}

/*
 * Handle the No-More_Senders notification generated from a device port destroy.
 * Since there are no longer any tasks which hold a send right to this device
 * port a NMS notification has been generated. We check the device state and
 * if it's closing then this becomes a no-op, otherwise the task holding the
 * send right has died without performing a device_close() call. We clean house
 * here by calling ds_device_close() on the task's behalf.
 */

ds_no_senders(notification)
	mach_no_senders_notification_t *notification;
{
	device_t		device;
	ipc_port_t		dev_port;
	extern device_t		dev_port_lookup();

	dev_port = (ipc_port_t) notification->not_header.msgh_remote_port;

	/*
	 * convert a port to it's device structure.
	 */
	if ( (device = dev_port_lookup( dev_port )) != DEVICE_NULL ) {
		io_return_t	rc;
		short		state;

		/*
		 * release reference created by previous dev_port_lookup()
		 */
		device_deallocate(device);

		assert( device->port == dev_port );

		/*
		 * If the device is already being closed (NMS generated by
		 * calling ds_device_close()), then do nothing as this
		 * NMS notification is extra baggage at this juncture.
		 */
		device_lock(device);
		state = device->state;
		device_unlock(device);
		if ( state == DEV_STATE_CLOSING )
			return;

		/*
		 * The task which owned the last send right has died without
		 * doing a ds_device_close(), do it now.
		 */
		if ( (rc=ds_device_close(device)) != D_SUCCESS )
			printf("ds_no_senders() ds_device_close(%x) rc %d\n",
				device,rc);
	}
}

boolean_t
ds_notify(msg)
	mach_msg_header_t *msg;
{
	switch (msg->msgh_id) {
		case MACH_NOTIFY_NO_SENDERS:
		ds_no_senders((mach_no_senders_notification_t *) msg);
		return TRUE;
		
		default:
		printf("ds_notify: strange notification %d\n", msg->msgh_id);
		return FALSE;
	}
}

boolean_t
ds_master_notify(msg)
	mach_msg_header_t *msg;
{
	extern ipc_port_t master_device_port;
	ipc_port_t port = (ipc_port_t) msg->msgh_remote_port;

        if ((port == master_device_port) && (msg->msgh_id & MACH_NOTIFY_DEAD_NAME)) {
		return TRUE;
	}
	else {
		printf("ds_master_notify: strange notification %d\n", msg->msgh_id);
		return FALSE;
	}
}

queue_head_t		io_done_list;
decl_simple_lock_data(,	io_done_list_lock)

#define	splio	splsched	/* XXX must block ALL io devices */

void iodone(ior)
	register io_req_t	ior;
{
	register int	s;

	/*
	 * If this ior was loaned to us, return it directly.
	 */
	if (ior->io_op & IO_LOANED) {
		(*ior->io_done)(ior);
		return;
	}
	/*
	 * If !IO_CALL, some thread is waiting for this.  Must lock
	 * structure to interlock correctly with iowait().  Else can
	 * toss on queue for io_done thread to call completion.
	 */
	s = splio();
	if ((ior->io_op & IO_CALL) == 0) {
	    ior_lock(ior);
	    ior->io_op |= IO_DONE;
	    ior->io_op &= ~IO_WANTED;
	    ior_unlock(ior);
	    thread_wakeup((int)ior);
	} else {
	    ior->io_op |= IO_DONE;
	    simple_lock(&io_done_list_lock);
	    enqueue_tail(&io_done_list, (queue_entry_t)ior);
	    thread_wakeup((int)&io_done_list);
	    simple_unlock(&io_done_list_lock);
	}
	splx(s);
}


void io_done_thread_continue()
{
	for (;;) {
	    register int	s;
	    register io_req_t	ior;

	    s = splio();
	    simple_lock(&io_done_list_lock);
	    while ((ior = (io_req_t)dequeue_head(&io_done_list)) != 0) {
		simple_unlock(&io_done_list_lock);
		(void) splx(s);

		if ((*ior->io_done)(ior)) {
		    /*
		     * IO done - free io_req_elt
		     */
		    io_req_free(ior);
		}
		/* else routine has re-queued it somewhere */

		s = splio();
		simple_lock(&io_done_list_lock);
	    }

	    assert_wait((int) &io_done_list, FALSE);
	    simple_unlock(&io_done_list_lock);
	    (void) splx(s);
	    counter(c_io_done_thread_block++);
	    thread_block(io_done_thread_continue);
	}
}

int	iodone_thread_fixpri = 0;

void io_done_thread()
{
	/*
	 * Set thread privileges and highest priority.
	 */
	current_thread()->vm_privilege = TRUE;
	stack_privilege(current_thread());

	if (iodone_thread_fixpri == -1) {
		thread_set_own_priority(0);
	} else {
		thread_set_own_priority(iodone_thread_fixpri);
		(void) thread_policy(current_thread(), POLICY_FIXEDPRI, 1);
	}

	io_done_thread_continue();
	/*NOTREACHED*/
}

#define	DEVICE_IO_MAP_SIZE	(2 * 1024 * 1024)

void ds_init()
{
	vm_offset_t	device_io_min, device_io_max;

	queue_init(&io_done_list);
	simple_lock_init(&io_done_list_lock);

	device_io_map = kmem_suballoc(kernel_map,
				      &device_io_min,
				      &device_io_max,
				      DEVICE_IO_MAP_SIZE,
				      FALSE);
	/*
	 *	If the kernel receives many device_write requests, the
	 *	device_io_map might run out of space.  To prevent
	 *	device_write_get from failing in this case, we enable
	 *	wait_for_space on the map.  This causes kmem_io_map_copyout
	 *	to block until there is sufficient space.
	 *	(XXX Large writes may be starved by small writes.)
	 *
	 *	There is a potential deadlock problem with this solution,
	 *	if a device_write from the default pager has to wait
	 *	for the completion of a device_write which needs to wait
	 *	for memory allocation.  Hence, once device_write_get
	 *	allocates space in device_io_map, no blocking memory
	 *	allocations should happen until device_write_dealloc
	 *	frees the space.  (XXX A large write might starve
	 *	a small write from the default pager.)
	 */
	device_io_map->wait_for_space = TRUE;

	io_inband_zone = zinit(sizeof(io_buf_ptr_inband_t),
			    1000 * sizeof(io_buf_ptr_inband_t),
			    10 * sizeof(io_buf_ptr_inband_t),
			    FALSE,
			    "io inband read buffers");
}

iowait(ior)
io_req_t ior;
{
    int s;

    s = splio();
    ior_lock(ior);
    while ((ior->io_op&IO_DONE)==0) {
	assert_wait((int)ior, FALSE);
	ior_unlock(ior);
	thread_block((void (*)()) 0);
        ior_lock(ior);
    }
    ior_unlock(ior);
    splx(s);
}
