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
 * HISTORY
 * $Log: utsname.h,v $
 * Revision 1.4  1994/11/18  20:42:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:30:54  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:40:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:45:37  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:43:10  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:42:18  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:16:44  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:47:29  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:56:38  devrcs
 * 	made SYS_NMLN invisible in _POSIX_SOURCE
 * 	[90/10/03  18:58:35  rabin]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:49:02  gm]
 * 
 * Revision 1.3  90/04/27  19:28:22  devrcs
 * 	Integrated with AIX header.
 * 	[90/04/20  10:57:29  bet]
 * 
 * Revision 1.2  90/02/23  00:33:17  devrcs
 * 	Initial version of header file for operating system identification.
 * 	[90/02/13  19:55:18  bet]
 * 
 * $EndLog$
 */
/* @(#)utsname.h	1.8  com/inc/sys,3.1,8943 4/5/89 09:01:38 */

#ifndef	_SYS_UTSNAME_H_
#define	_SYS_UTSNAME_H_
#include <standards.h>

/*
 * POSIX requires that certain values be included in utsname.h.  It also
 * requires that when _POSIX_SOURCE is defined only those standard
 * specific values are present.  This header includes all the POSIX
 * required entries.
 */
#ifdef _POSIX_SOURCE

#define  _SYS_NMLN           32     /* Important: do not change this value ! */

struct utsname {
	char    sysname[_SYS_NMLN];
	char    nodename[_SYS_NMLN];
	char    release[_SYS_NMLN];
	char    version[_SYS_NMLN];
	char    machine[_SYS_NMLN];
};

#ifndef _KERNEL
#ifdef _NO_PROTO	
extern int uname();
#else
extern int uname(struct utsname *);
#endif /* _NO_PROTO */
#endif /* _KERNEL */

#endif /* _POSIX_SOURCE */

#ifdef _OSF_SOURCE

#define SYS_NMLN	_SYS_NMLN

extern struct utsname utsname;

#endif /* _OSF_SOURCE */

#endif /* _SYS_UTSNAME_H_ */
