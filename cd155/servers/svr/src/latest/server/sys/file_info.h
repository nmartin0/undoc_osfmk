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
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: file_info.h,v $
 * Revision 1.4  1994/11/18  20:40:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:24:49  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:33:40  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:42:02  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:39  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:27  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:06:10  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.0  91/01/17  12:08:51  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  90/05/29  20:24:27  rwd
 * 	Created from sys/ushared.h
 * 	[90/04/30            rwd]
 * 
 *
 */

#ifndef	_FILE_INFO_
#define	_FILE_INFO_

#include <sys/shared_lock.h>

typedef	struct file_info {
	    shared_lock_t
			lock;
	    int		open:1,
			read:1,
			write:1,
			append:1,
			mapped:1,
	    		control:1,
	    		inuse:1,
			wants:1,
			dirty:1;
	    int		offset;
	    struct map_info {
		int	address;
		int	size;
		int	offset;
		int	inode_size;
	    } map_info;
	} file_info_t;
#endif	_FILE_INFO_
