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
 * $Log: tar.h,v $
 * Revision 1.3  1994/11/18  21:11:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:00:17  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:19:07  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:21:39  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:39:49  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:37:51  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:00:58  gm]
 * 
 * Revision 1.2  90/03/13  21:23:49  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:38:09  tom]
 * 
 * $EndLog$
 */
/*
 * COMPONENT_NAME: tar.h
 *                                                                    
 * ORIGIN: IBM
 *
 * Copyright International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */                                                                   


#ifndef _TAR_H_
#define _TAR_H_
#include <standards.h>

/*
 * POSIX required that certain values be included in tar.h.  It also requires
 * that if _POSIX_SOURCE is defined then only those standard specific values
 * be present.  This header includes all the POSIX required entries.
 */

#ifdef _POSIX_SOURCE

#define TMAGIC		"ustar"		/* ustar and a null */
#define TMAGLEN		6		  
#define TVERSION	"00"		/* 00 and no null */
#define TVERSLEN	2

/* Values used in typeflag field */
#define REGTYPE		'0'		/* regular file */
#define AREGTYPE	'\0'		/* regular file */
#define LNKTYPE		'1'		/* line         */
#define SYMTYPE		'2'		/* reserved     */
#define CHRTYPE		'3'		/* character special */
#define BLKTYPE 	'4'		/* block special */
#define DIRTYPE		'5'		/* directory */
#define FIFOTYPE	'6'		/* FIFO special */
#define CONTTYPE	'7'		/* reserved */

/* Bits used in the mode field - values in octal */
#define TSUID		04000		/* set UID on execution */
#define TSGID		02000		/* set GID on execution */
#define TSVTX		01000		/* reserved 	   	*/
#define TUREAD		00400		/* read by owner	*/
#define TUWRITE		00200		/* write by owner	*/
#define TUEXEC		00100		/* execute/search by owner */
#define TGREAD		00040		/* read by group 	*/
#define TGWRITE		00020		/* write by group 	*/
#define TGEXEC		00010		/* execute/search by group */
#define TOREAD		00004		/* read by other 	*/
#define TOWRITE		00002		/* write by other 	*/
#define	TOEXEC		00001		/* execute/search by other */

#endif /* _POSIX_SOURCE */
#endif /* _TAR_H_ */
