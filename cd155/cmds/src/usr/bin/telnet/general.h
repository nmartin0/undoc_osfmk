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
/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)general.h	1.5 (Berkeley) 6/28/90
 */

/*
 * Some general definitions.
 */


#define	numberof(x)	(sizeof x/sizeof x[0])
#define	highestof(x)	(numberof(x)-1)

#if	defined(unix)
#define	ClearElement(x)		bzero((char *)&x, sizeof x)
#define	ClearArray(x)		bzero((char *)x, sizeof x)
#else	/* defined(unix) */
#define	ClearElement(x)		memset((char *)&x, 0, sizeof x)
#define	ClearArray(x)		memset((char *)x, 0, sizeof x)
#endif	/* defined(unix) */

#if	defined(unix)		/* Define BSD equivalent mem* functions */
#define	memcpy(dest,src,n)	bcopy(src,dest,n)
#define	memmove(dest,src,n)	bcopy(src,dest,n)
#define	memset(s,c,n)		if (c == 0) { \
				    bzero(s,n); \
				} else { \
				    register char *src = s; \
				    register int count = n; \
					\
				    while (count--) { \
					*src++ = c; \
				    } \
				}
#define	memcmp(s1,s2,n)		bcmp(s1,s2,n)
#endif	/* defined(unix) */
