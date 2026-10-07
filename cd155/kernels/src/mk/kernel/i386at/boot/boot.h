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
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * boot.h,v
 * Revision 1.1.4.2  1993/02/12  17:35:39  dleslie
 * Adding new code from vendor
 *
 * Revision 1.1  1992/09/30  02:26:33  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.1.2.1  92/04/30  11:53:01  bernadat
 * 	Use mk inludes only
 * 	Copied from main line
 * 	[92/03/19            bernadat]
 * 
 * Revision 2.2  92/04/04  11:35:03  rpd
 * 	Fabricated from 3.0 bootstrap.  But too many things are global.
 * 	[92/03/30            mg32]
 * 
 */
/* CMU_ENDHIST */
/*
 * Mach Operating System
 * Copyright (c) 1992, 1991 Carnegie Mellon University
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

#if	!defined(KERNEL)
typedef int lock_data_t;
#endif	/* !defined(KERNEL) */

#include <boot_ufs/file_io.h>

extern char *devs[], *name, *iodest;
extern struct fs *fs;
extern struct file inode;
extern int unit, part, maj, boff, poff, bnum, cnt;
extern char *names[];
extern int currname;

#define is_space(c)	((c) == ' ' || (c) == '\t')
#define server_delimiter ':'
#define BOOT_LINE_LENGTH 160
