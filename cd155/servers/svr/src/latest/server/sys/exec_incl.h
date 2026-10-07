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
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log: exec_incl.h,v $
 * Revision 1.4  1994/11/18  20:40:23  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:24:32  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:33:26  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:41:53  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:33  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:17  cfj
 * Bump major revision number.
 *
 * Revision 2.5  1991/11/13  14:44:01  rabii
 * 	[Mon Oct 21 10:08:47 PDT 1991] cfj@ssd.intel.com
 * 	     Added i860 ifdefs.
 *
 *
 * Revision 2.3  91/10/14  13:01:33  sjs
 * 	ision 3.3  91/09/13  12:51:25  sp
 * 	lude uxkern/vm_param.h to find PAGE_SIZE
 * 
 * Revision 2.2  91/08/31  14:04:29  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/26  15:54:22  jose
 * Adapted from 1.0.2 to OSF/1s environment
 * 
 * Revision 1.4.5.2  91/06/19  08:43:29  lwa
 * 	Add exec_privs struct, for holding credentials and privileges
 * 	across exec.
 * 	[91/06/17  08:17:42  lwa]
 * 
 * Revision 1.4  90/10/07  14:50:08  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:38:44  gm]
 * 
 * Revision 1.3  90/08/24  12:26:01  devrcs
 * 	Cleaned up SECTALIGN definitions for i386
 * 	[90/08/17  18:56:36  kevins]
 * 
 * 	HP/Apollo M68K
 * 	[90/08/13  17:37:08  mcg]
 * 
 * Revision 1.2  90/06/22  20:52:50  devrcs
 * 	nags merge
 * 	[90/06/12  21:38:07  gmf]
 * 
 * 	Included OSF/Mach-O stuff plus kalloc.h.
 * 	[90/06/08  12:12:52  melanie]
 * 
 * Revision 1.2  90/04/27  18:51:17  devrcs
 * 	Initial Revision.
 * 	[90/04/18  10:01:11  kwallace]
 * 
 * $EndLog$
 */
/*
 * This file contains common definitions and include files used by
 *  both kern_exec.c and ldr_exec.c
 */

#include <cputypes.h>
#include <bsd_a_out.h>
#include <sysv_coff.h>
#include <osf_mach_o.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/map.h>
#include <sys/user.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/mount.h>
#include <sys/ucred.h>
#include <sys/buf.h>
#include <sys/vnode.h>
#ifndef	OSF1_SERVER
#include <sys/vm.h>
#endif	/* OSF1_SERVER */
#include <sys/file.h>
#include <sys/uio.h>
#include <sys/acct.h>
#include <sys/exec.h>

#include <machine/reg.h>
#if	defined(vax) || defined(i386)
#include <machine/psl.h>
#endif

#if	SYSV_COFF
/*
 *	Fileheaders for coff files.
 */
#ifdef __hp_osf
/* On HP, get coff header files copied from usr/include/<machine> source rather than
   those in the kernel/sysv source.  This is probably what needs to be done
   for the other platforms as well (right now, mips has 2 versions of each
   header - in usr/include/<machine> and in kernel/sysv).  The makefile at
   kernel/include/Makefile does the copying of these (and other) headers. */
#include <scnhdr.h>
#include <aouthdr.h>
#include <filehdr.h>
#else
#include <sysV/scnhdr.h>
#include <sysV/aouthdr.h>
#include <sysV/filehdr.h>
#endif /* hp_osf */
#endif

#if	OSF_MACH_O
/*
 *	header files for processing OSF/mach-o.
 */
#include <mach_o_header.h>
#include <mach_o_format.h>
#include <kern/kalloc.h>
#endif

#ifdef	mips
#include <sys/ptrace.h>
#endif


/*
 *  Force all namei() calls to permit remote names since this module has
 *  been updated.
 */
#if	MACH_RFS
#undef	namei
#define namei	rnamei
#endif

#ifdef	ibmrt
#include <ca/debug.h>
#endif

#ifdef	sun4
#include <sun4/asm_linkage.h>
#endif

#include <sys/signal.h>
#ifndef	OSF1_SERVER
#include <kern/task.h>
#include <kern/thread.h>

#include <mach/vm_param.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <mach/memory_object.h>
#include <vm/vm_kern.h>
#include <vm/vm_user.h>
#else	/* OSF1_SERVER */
#include <uxkern/vm_param.h>
#endif	/* OSF1_SERVER */
#include <builtin/inode_pager.h>
#include <kern/zalloc.h>

#include <sys/signal.h>
#include <machine/vmparam.h>
#include <kern/parallel.h>
#include <uxkern/import_mach.h>



#if	BSD_A_OUT
/*
 * All of these silly loader_page_size's should be moved into a machine
 * dependent directory, for obvious reasons.  -BB
 */
#ifdef	vax
#define LOADER_PAGE_SIZE	(1024)
#endif
#if	defined(ibmrt) || defined(balance)
#define LOADER_PAGE_SIZE	(2048)
#endif
#ifdef	sun
#define LOADER_PAGE_SIZE	(8192)
#endif
#ifdef	__hp_osf
/* warning, this may be changed to 4096 at a later date */
#define LOADER_PAGE_SIZE	(8192)
#endif
#ifdef	i386
#define	LOADER_PAGE_SIZE	(4096)
#endif
#ifdef	i860
#define	LOADER_PAGE_SIZE	(4096)
#endif
#endif	/* BSD_A_OUT */

#if	SYSV_COFF
/*
 *	Corresponding definitions are in coff_getxfile because the
 *	section size (SECTALIGN, corresponds to LOADER_PAGE_SIZE)
 *	must be obtained from the file header for some architectures.
 */

#ifdef	LOADER_PAGE_SIZE
#define SECTALIGN		LOADER_PAGE_SIZE
#else
#define LOADER_PAGE_SIZE	SECTALIGN
#endif
#endif

#define loader_round_page(x)	((vm_offset_t)((((vm_offset_t)(x)) \
						+ LOADER_PAGE_SIZE - 1) \
					& ~(LOADER_PAGE_SIZE-1)))
#define loader_trunc_page(x)	((vm_offset_t)(((vm_offset_t)(x)) \
					& ~(LOADER_PAGE_SIZE-1)))


/*
 *	A corrupted fileheader can cause getxfile to decide to bail
 *	out without setting up the address space correctly.  It is
 *	essential in this case that control never get back to the
 *	user.  The following error code is used by getxfile to tell
 *	execve that the process must be killed.
 */

#define EGETXFILE	126

/* The exec_privs structure contains the credentials (and privileges,
 * in a secure system) from the original program being exec'ed.
 * These privileges are preserved here across the call to getxfile()
 * or exec_load_loader(), and then established for the process when
 * the getxfile() is complete.
 */

struct exec_privs {
	struct ucred	*creds;		/* credentials */
	boolean_t	is_priv;	/* true iff file had privileges */

#if	SEC_BASE
	struct	vsecattr vsattr;	/* file's security attributes */
#endif	/* SEC_BASE */
};
