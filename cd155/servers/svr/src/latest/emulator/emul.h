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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/* 
 * HISTORY
 * $Log: emul.h,v $
 * Revision 1.9  1995/02/01  21:21:41  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.8  1994/11/18  20:22:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/07/14  17:30:33  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:22:05  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/18  21:23:59  cfj
 * Turn DEBUG off by default.
 *
 * Revision 1.5  1993/05/06  20:14:36  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:17:17  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.12  93/06/16  15:10:40  klh
 * 	Revision 1.13  93/05/16  20:58:37  loverso
 * 		Add isc interface for locally blocked. (loverso)
 * 
 * Revision 1.11  93/06/02  09:40:14  yazz
 * Declare global emulator data for Sys V IPC under TNC.
 * 
 * Revision 1.10  93/04/24  18:36:24  klh
 * 	Revision 1.12  93/01/06  10:36:35  loverso
 * 		Fix notice.
 *
 * Revision 1.4  1993/04/03  03:17:25  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  05:56:43  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.2  1992/11/30  22:08:15  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.3  1992/12/11  02:51:17  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.1.2.1  1992/11/05  22:15:30  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 1.9  92/11/23  15:51:02  klh
 * 	Revision 1.11  92/11/05  17:14:40  roy
 * 		Added rlimit_fsize extern.
 * 		[92/10/26            roy]
 * 
 * 	Revision 1.10  92/11/03  10:46:04  loverso
 * 		Include prototype for emul_vm_map().
 * 
 * Revision 1.8  92/10/05  15:21:05  klh
 * 	Revision 1.9  92/09/11  09:25:29  rabii
 * 		Fix typo.
 * 
 * 	Revision 1.8  92/08/26  12:09:08  loverso
 * 		Add new emul_blocking/emul_unblocking macros.
 * 		Add emul_thread definitions. (loverso)
 * 
 * Revision 1.7  92/06/05  17:48:36  klh
 * 	Revision 1.7  92/05/18  12:24:10  roy
 * 		Revision 1.5.1.1  92/04/22  09:47:18  roy
 * 		Moved mapped_files code to fdt.h.
 * 		[92/04/06            roy]
 * 
 * Revision 1.6  92/03/28  10:43:07  chrisp
 * Added #define for "migrate notification" debugging.
 * 
 * Revision 1.5  92/03/15  14:28:53  roy
 * 	Remove #include <machine/vmparam.h> from this file.
 * 
 * Revision 1.4  92/01/03  09:27:36  roy
 * 	Remove ADFS_HACK.
 * 
 * Revision 1.3  92/01/02  18:37:14  roy
 * 	91/09/23  20:29:38  noemi
 * 	OSF1/ADFS update.  Added local_rootdir_port.
 * 
 * Revision 1.2  91/12/13  10:04:47  roy
 * 	91/11/19  10:15:38  roy
 * 	Added fmode, modified, min_va, and max_va fields to fdte.
 * 
 * 	91/11/12  10:15:54  roy
 * 	Added MAPPED_FILES support.
 * 
 * 	91/10/29  17:49:59  roy
 * 	Overhaul releated to new file descriptor table mechanism.
 *
 * Revision 1.1  91/09/17  12:10:34  sjs
 * Initial revision
 * 
 * $EndLog$
 */
#ifndef	_EMUL_USER_H_
#define	_EMUL_USER_H_

#include <sys/param.h>
#include <sys/unix_defs.h>

/*
 * Macros for printing debug messages on the console.
 */
/*#define	DEBUG 1 /**/

#if	DEBUG
extern int	e_emulator_error();
#if	__STDC__ == 1
#define	EPRINT(__msg)	e_emulator_error ##__msg
#define EASSERT(__ex)	((##__ex) ? 0 : emul_panic(# __ex))
#else
#define	EPRINT(msg)	e_emulator_error/**/__msg/**/
#define EASSERT(__ex)	((__ex) ? 0 : emul_panic("__ex"))
#endif

#else	

#define	EPRINT(a)
#define	EASSERT(ex)
#endif

/*
 * debugging - modifiy 'edebug' flags to control debugging printf's
 */
#define EDEBUG_NONE     0x0
#define EDEBUG_MF       0x1
#define EDEBUG_TNC_UIPC	0x2

/*
 * Current process ID (initially -1, saved on first call to getpid)
 */
extern pid_t current_pid;

/*
 * Server port for non-file-related system calls.
 */
extern mach_port_t our_bsd_server_port;

/*
 * Server port for vproc system calls.
 */
extern mach_port_t vproc_port;

/*
 * Credentials port for file-related system calls.
 */
extern mach_port_t credentials_port;

/*
 * Ports for current root directory and current working directory
 */
extern mach_port_t rootdir_port;
extern mach_port_t currentdir_port;

#ifdef	TNC
/*
 * System V IPC port for svipc calls under TNC.
 */
extern mach_port_t our_svipc_port;
#endif	/* TNC */

#ifdef	MAPPED_FILES
/*
 * Max. file size specified by rlimit.
 */
extern unsigned int rlimit_fsize;	 
#endif

/*
 * "safe" blocking macros for non-fileserver messages
 * (really just an isc interface)
 */
#define emul_blocking() \
	isc_register(MACH_PORT_DEAD, (transaction_id_t *)NULL)
#define emul_unblocking() \
	isc_deregister((int *)NULL)

/*
 * operations interruptible by a thread_abort()
 */
#define isc_register_local(t)		isc_register(MACH_PORT_NULL, t)
#define isc_deregister_local(i)		isc_deregister(i)

typedef unsigned long	emul_thread_id;
emul_thread_id		emul_thread_id_self(), emul_thread_id_of();

kern_return_t		emul_thread_get_state(), emul_thread_set_state();

kern_return_t	emul_vm_map(
	mach_port_t	target_task,
	vm_address_t	*address,
	vm_size_t	size,
	vm_address_t	mask,
	boolean_t	anywhere,
	mach_port_t	memory_object,
	vm_offset_t	offset,
	boolean_t	copy,
	vm_prot_t	cur_protection,
	vm_prot_t	max_protection,
	vm_inherit_t	inheritance);


#endif	_EMUL_USER_H_
