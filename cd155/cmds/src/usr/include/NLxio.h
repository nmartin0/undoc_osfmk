/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
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
#ifdef	_PGI_
	unsigned char chrs[256];
#else
	char chrs[256];
#endif
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
