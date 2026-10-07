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
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: syscall_subr.h,v $
 * Revision 1.9  1995/04/08  00:19:41  yazz
 *  Authors of fix: Ray Shapouri and Bob Yasi
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 12851
 *  Testing: EATs controlc, sched, os_interfaces MUNOPS SAT runs
 *  Module(s):
 * 	server/uxkern/fsvr_msg.c
 * 	server/uxkern/fsvr_server_side.c
 * 	server/uxkern/syscall_subr.c
 * 	server/uxkern/syscall_subr.h
 * Added new routines vm_allocate_strict() and vm_deallocate_strict().  They
 * do what their corresponding Mach OS calls do, but panic on any failure.
 *
 * Revision 1.8  1995/02/01  22:30:28  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.7  1994/11/18  20:49:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/06/22  18:29:51  jlitvin
 * SERVER_DEALLOC() macro doesn't deallocate OOL memory after most system
 * call errors.  Remove this macro and always call vm_deallocate().
 * Thanks to Bob Yasi of Locus for finding these potential memory leaks.
 *
 *  Reviewer: cfj & yazz
 *  Risk: low
 *  Benefit or PTS #: 9619
 *  Testing: it builds and boots
 *  Module(s): uxkern/{syscall_subr.h,bsd_server_side.c,fsvr_msg.c,
 * 		fsvr_server_side.c}
 *
 * Revision 1.5  1994/06/18  00:04:49  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.4  1993/07/14  18:44:43  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:06:30  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:33:32  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:53:54  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.15  93/06/02  12:39:21  yazz
 * For Sys V IPC under TNC define macros that convert svipc portnames to
 * svipc structure pointers and vice versa.
 * 
 * 	Revision 2.15  93/03/05  17:46:52  loverso
 * 		Add an END_VPROCSERVER variant (_NOSIG) for use by SYS_fork to
 * 		avoid signal checks in the "middle" of the syscall.
 *
 * 	Revision 2.14  93/01/06  10:37:20  loverso
 * 		Fix notice.
 *
 * Revision 2.13  92/07/30  16:27:33  chrisp
 * end_vprocserver_op() now takes the target vproc port as its first parameter.
 * 
 * Revision 2.12  92/05/27  20:08:38  pjg
 * 	Include mach_ldebug.h (needed in parallel.h).
 * 
 * Revision 2.11  92/05/26  16:43:25  pjg
 * 	Take the master lock if compiling for uni-processor (NCPUS == 1) in
 * 	{START,END}_VPROCSERVER.
 * 
 * Revision 2.10  92/05/24  14:03:12  pjg
 * 	Revision 3.6  92/03/23  18:04:41  condict
 * 	Always take master lock if compiling for uni-processor (NCPUS == 1).
 * 
 * Revision 2.9  92/03/09  13:55:08  durriya
 * 	Revision 3.5  92/02/28  18:00:11  barbou
 * 	Fix for bug #97: memory leak...
 * 
 * Revision 2.8  92/03/01  18:36:53  pjg
 * 	Only deallocate ports and memory received in messages in case of
 * 	success, acording to the new ux_server_loop.
 * 	Got rid of unused macros and simplified the existing ones.
 * 
 * Revision 2.7  92/01/17  17:25:52  roy
 * 	Remove mpport macros (loverso).
 * 
 * Revision 2.6  92/01/05  20:13:13  roy
 * 	91/11/27  04:09:59  ses
 * 	Added MACRO_BEGIN and MACRO_END to some multi-statement macro 
 * 	definitions.
 * 
 * 	91/11/12  21:46:04  noemi
 * 	Moved back PORT_TO_VNODE_LOOKUP and VNODE_LOOKUP_DONE.  Changes to
 * 	check that vnodes and file structures are in the range of allocated
 * 	vnodes and file structures, respectively.
 * 
 * 	1991/10/14  21:10:36  noemi
 * 	Added PORT_TO_FILE_LOOKUP, FILE_TO_PORT_LOOKUP, and FILE_LOOKUP_DONE 
 * 	macros.  Removed port_to_file_lookup and file_to_port_lookup macros.  
 * 	Also removed FILESERVER_OP and VNODESERVER_OP macros.  
 * 	Added SERVER_DEALLOC, ENTER_PORT, REMOVE_PORT and LOOKUP_PORT macros.
 * 
 * Revision 2.5  91/11/13  15:51:26  rabii
 *    Removed "register" from declaration of uthread and error to make
 *    GreenHills compiler happy
 * 
 * Revision 2.4  91/10/04  15:28:59  chrisp
 * Add many *_FILESERVER_*(), *_VNODESERVER_*(), and *_VPROCSERVER_*()
 * macros to correspond to the new *_SERVER_*() macros from the
 * previous OSF Grenoble submission.
 *
 * Revision 2.3  91/09/17  09:10:02  sjs
 * integrate Locus changes	roman
 * Add new macros START_FILESERVER(), START_VPROCSERVER() and 
 * START_VNODESERVER() and the corresponding END_*SERVER() and
 * END_*SERVER_DEALLOC(). These macros are used rather than 
 * START_SERVER(), END_SERVER(), and END_SERVER_DEALLOC()
 * when the destination of an message from the emulator to the server 
 * is a file port, vproc port or vnode port rather than a proc port.
 * 
 * Revision 2.2  91/08/31  14:28:22  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/08/27  15:39:46  barbou
 * New macros for END_SERVER_DEALLOC() (for the UX26 upgrade).
 * 
 * Revision 3.3  91/06/25  17:14:15  condict
 * Moved sys header files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.2  91/05/07  15:57:37  condict
 * Define new versions of the START/END_SERVER macros that determine
 * whether the syscall is serial or parallel by looking in sysent table.
 * 
 * Revision 3.1  91/03/08  16:06:42  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.0  91/01/17  12:06:20  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.3  90/06/02  15:28:33  rpd
 * 	Revised for new reply msg technology.
 * 	[90/04/27            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  20:23:33  rpd]
 * 
 * Revision 2.2  89/10/17  11:27:22  rwd
 * 	Added interrupt return parameter to all calls.
 * 	[89/09/21            dbg]
 * 
 * $EndLog$
 */

#ifndef _UXKERN_SYSCALL_SUBR_H_
#define _UXKERN_SYSCALL_SUBR_H_


#include <cpus.h>
#ifdef  _KERNEL
#include <mach_ldebug.h>
#endif
/*
 * Glue to make MiG procedure interfaces look like old UX system calls.
 */
#include <sys/systm.h>
#include <kern/parallel.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/errno.h>
#include <sys/file.h>
#ifdef  _KERNEL
#include <kern/assert.h>
#endif

/*
 * Declare these void routines.  They are identical to their corresponding
 * Mach OS calls, except that they panic on any failure.
 */
extern void vm_allocate_strict(
	mach_port_t,
	vm_address_t*,
	vm_size_t,
	boolean_t);

extern void vm_deallocate_strict(
	mach_port_t,
	vm_address_t,
	vm_size_t);

/*
 * The following macros are used to go from file pointers to Mach
 * ports and vice versa.
 */
#define FILE_TO_PORT_LOOKUP(fp,port)				\
	port = ((mach_port_t)fp)

#define PORT_TO_FILE_LOOKUP(port,fp)				\
MACRO_BEGIN							\
	fp = ((struct file *)port);				\
	if (fp < file || fp > fileNFILE)			\
		fp = (struct file *)0;				\
	else if ((fp)->f_magic == F_MAGIC)			\
		FP_REF(fp);					\
	else							\
		fp = (struct file *)0;				\
MACRO_END

#define	FILE_LOOKUP_DONE(fp)					\
	FP_UNREF(fp)

/*
 * The following macros are used to go from vnode pointers to Mach
 * ports and vice versa.
 */
#define PORT_TO_VNODE_LOOKUP(port,vp)				\
MACRO_BEGIN							\
	vp = ((struct vnode *)port);				\
	if (vp < vnode || vp > vnodeNVNODE)			\
		vp = (struct vnode *)0;				\
	else if ((vp)->v_magic == V_MAGIC)			\
		VREF(vp);					\
	else							\
		vp = (struct vnode *)0;				\
MACRO_END

#define VNODE_LOOKUP_DONE(vp)					\
MACRO_BEGIN							\
	ASSERT(vp);						\
	vrele(vp);						\
MACRO_END

/*
 * The following are macros used for all system calls directed
 * to the "proc" port.
 */
#define	START_SERVER(syscode, nargs)				\
	uthread_t uth = &u;					\
	int	error;						\
	struct proc *procp;					\
	int	arg[nargs];					\
	int serial = !sysent[syscode].sy_parallel;		\
								\
	error = start_server_op(proc_port, syscode);		\
	if (error)						\
	    return (error);					\
	procp = port_to_proc_lookup(proc_port);			\
	uth->uu_reply_msg = 0;					\
	if (NCPUS == 1 || serial) {				\
	    unix_master();					\
        }

#define	END_SERVER(dummy)					\
	if (NCPUS == 1 || serial)				\
	    unix_release();					\
	return (end_server_op(error, interrupt));

/*
 * The following macros are used for all system calls directed at
 * a "file" port.
 */
#define	START_VPROCSERVER(syscode, nargs)			\
	uthread_t uth = &u;					\
	int	error;						\
	struct vproc *vprocp;					\
	int	arg[nargs];					\
	int serial = !sysent[syscode].sy_parallel;		\
								\
	error = start_vprocserver_op(vproc_port, syscode);	\
	if (error)						\
	    return (error);					\
	vprocp = port_to_vproc_lookup(vproc_port);		\
	uth->uu_reply_msg = 0;					\
	if (NCPUS == 1 || serial) {				\
	    unix_master();					\
	}

#define	END_VPROCSERVER(dummy)					\
	if (NCPUS == 1 || serial)				\
	    unix_release();					\
	return (end_vprocserver_op(vproc_port, error, interrupt, FALSE));

#define	END_VPROCSERVER_NOSIG(dummy)				\
	if (NCPUS == 1 || serial)				\
	    unix_release();					\
	return (end_vprocserver_op(vproc_port, error, interrupt, TRUE));

#ifdef	TNC
/*
 * The following macros are used to convert from SVIPC (System V IPC)
 * pointers to Mach ports and vice versa.
 */
#define SVIPC_TO_PORT_LOOKUP(svp,port)				\
	port = ((mach_port_t)svp)

#define PORT_TO_SVIPC_LOOKUP(port,svp)				\
MACRO_BEGIN							\
	svp = ((struct svipc *)port);				\
	if ((svp)->sv_magic != SV_MAGIC)			\
		svp = (struct svipc *)0;			\
MACRO_END

#endif	/* TNC */

#endif /* _UXKERN_SYSCALL_SUBR_H_ */
