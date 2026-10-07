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
 * $Log: cpio.h,v $
 * Revision 1.3  1994/11/18  21:09:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:13:23  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:56:46  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:14:58  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:17:35  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:33:58  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:26:15  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:43:38  gm]
 * 
 * Revision 1.2  90/07/27  10:40:09  devrcs
 * 	New from AIX/POSIX
 * 	[90/07/17  23:42:03  glenn]
 * 
 * $EndLog$
 */
/* @(#)$RCSfile: cpio.h,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/18 21:09:12 $ */

/*
 * COMPONENT_NAME: cpio.h
 *                                                                    
 * ORIGIN: IBM
 *
 * Copyright International Business Machines Corp. 1990
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 *
 * cpio.h	1.2  com/inc,3.1,9021 3/29/90 15:01:16
 */                                                                   
#ifndef _CPIO_H_
#define _CPIO_H_

#define	C_IRUSR		0000400
#define	C_IWUSR		0000200
#define	C_IXUSR		0000100
#define	C_IRGRP		0000040
#define	C_IWGRP		0000020
#define	C_IXGRP		0000010
#define	C_IROTH		0000004
#define	C_IWOTH		0000002
#define	C_IXOTH		0000001
#define	C_ISUID		0004000
#define	C_ISGID		0002000
#define	C_ISVTX		0001000
#define	C_ISDIR		0040000
#define	C_ISFIFO	0010000
#define	C_ISREG		0100000
#define	C_ISBLK		0060000
#define	C_ISCHR		0020000
#define	C_ISCTG		0110000
#define	C_ISLNK		0120000
#define	C_ISSOCK	0140000

#define	MAGIC		"070707"

#endif  /* _CPIO_H_ */
