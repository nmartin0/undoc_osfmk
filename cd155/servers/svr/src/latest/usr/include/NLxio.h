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
 * $Log: NLxio.h,v $
 * Revision 1.3  1994/11/18  21:09:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:13:09  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:56:35  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:14:44  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:17:18  nandy
 * Initial revision
 *
 * Revision 1.4  92/04/13  14:34:22  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.4  90/10/07  20:23:33  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:39:39  gm]
 * 
 * Revision 1.3  90/04/27  23:05:44  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/16  13:47:01  tom]
 * 
 * Revision 1.2  90/03/13  21:20:59  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:05:50  tom]
 * 
 * $EndLog$
 */
/* @(#)NLxio.h	1.4  com/inc,3.1,9013 12/20/89 13:22:55 */
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Library Support
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
#ifndef _NLXIO_H_
#define _NLXIO_H_

/*
 *
 * FUNCTION:	This file is designed to be included by NLxio.c
 * and any programs using _NLxin/_NLxout
 */

struct NLxtbl {
	char source[16];
	char target[16];
	char chrs[256];
};

#ifdef   _NO_PROTO
int NLxout();
int NLxin();
#else  /*_NO_PROTO */
int NLxin(char *,char *,int );
int NLxout(char *,char *,int );
#endif /*_NO_PROTO */

extern struct NLxtbl *_NLxitbl;
extern struct NLxtbl *_NLxotbl;

#define _NLxin(c)	(_NLxitbl->chrs[c])
#define _NLxout(c)	(_NLxotbl->chrs[c])

#endif /* _NLXIO_H_ */
