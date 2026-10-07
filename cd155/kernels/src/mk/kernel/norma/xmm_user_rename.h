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
 * SSD HISTORY
 * $Log: xmm_user_rename.h,v $
 * Revision 1.6  1994/11/18  20:57:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/22  02:22:14  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:53:02  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:47:11  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:52:46  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_user_rename.h,v
 * Revision 1.1  1992/09/30  02:34:17  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.3.2.1  92/06/24  18:03:45  jeffreyh
 * 	Add create, data_initialize.
 * 	[92/06/08            dlb]
 * 
 * Revision 2.3  91/08/28  11:16:32  jsb
 * 	Added supply_completed, data_return, change_completed.
 * 	[91/08/16  10:47:04  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:51  jsb
 * 	First checkin.
 * 	[91/06/17  11:08:51  jsb]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 */
/*
 *	File:	norma/xmm_user_rename.h
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Renames to interpose on memory_object.defs user side.
 */

#include <norma_vm.h>

#if	NORMA_VM
#define	memory_object_init		k_memory_object_init
#define	memory_object_create		k_memory_object_create
#define	memory_object_terminate		k_memory_object_terminate
#define	memory_object_data_request	k_memory_object_data_request
#define	memory_object_data_unlock	k_memory_object_data_unlock
#define	memory_object_data_write	k_memory_object_data_write
#define memory_object_copy		k_memory_object_copy
#define	memory_object_lock_completed	k_memory_object_lock_completed
#define	memory_object_supply_completed	k_memory_object_supply_completed
#define	memory_object_data_return	k_memory_object_data_return
#define	memory_object_change_completed	k_memory_object_change_completed
#define	memory_object_data_initialize	k_memory_object_data_initialize
#endif	NORMA_VM
