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
/* @(#)NLctype.h	1.12  com/inc,3.1,9013 2/27/90 22:07:32 */
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 3 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 */
#ifndef _NLCTYPE_H_
#define _NLCTYPE_H_

#include <NLchar.h>
#include <ctype.h>

#ifdef _KJI
#include <jctype.h>    
#endif  /* _KJI */


/*  Ctype definitions for use with setlocale (loc_t->lc_ctype) table.
 *  (Note:  NCisshift definition is in NLchar.h.)
 */

#ifndef lint
#ifndef _KJI
#define	_NCtoupper(c)	(((_locp->lc_chrtbl)->lc_caseconv+1)[c]) 
						/* assume lowercase c */
#define	_NCtolower(c)	(((_locp->lc_chrtbl)->lc_caseconv+1)[c]) 
						/* assume uppercase c */
#else  /* _KJI */
#define	_NCtoupper(c)	(isascii(c) ? _toupper(c) : _tojupper(c))
#define	_NCtolower(c)	(isascii(c) ? _tolower(c) : _tojlower(c))
#define _atojis(c)	(_atojistab[(c) - 0x20])
#define _jistoa(c)	(_jistoatab[((c)>>8) - 0x81][((c)&0xff) - 0x40])
#endif   /* _KJI */
#endif   /* lint */

/* The following macros implement character "flattening". This feature
 * may not be portable to future releases of OSF/1.
 */

/*  Macros with no old equivalents.
 */
#ifndef _KJI
extern char _NLflattab[];
extern unsigned _NLflattsize;
#endif   /* _KJI */

#ifndef lint
#ifndef _KJI
#define	NCflatchr(c)	(((unsigned)(c) < 0x80) ? \
				(c) : ((unsigned)(c) - 0x80 < _NLflattsize) ? \
				_NLflattab[c - 0x80] : '?')
#endif /* _KJI */
#endif   /* lint */

/*  Information-preserving escape sequence definitions:
 */
#ifdef _KJI
#define NLESCMAX	7	/* Maximum length of a sequence */
/* if the range of mnemonic escape sequences changes in NLesctab.c, MINESCVAL
 * and MAXESCVAL should be revised
 */
#define MINESCVAL	0xa1	/* Minimun NLchar value mapped in NLesctab */
#define MAXESCVAL	0xdf	/* Maximum NLchar value mapped in NLesctab */
#else
#define NLESCMAX	5	/* Maximum length of a sequence */
#endif   /* _KJI */

/*  Map from code point to escape sequence.
 */
#ifndef _KJI
extern char _NLesctab[][2];
extern unsigned _NLesctsize;
extern unsigned _NLunesctsize;
#endif   /* _KJI */

/*  Map from escape sequence to code point.
 */
extern struct NLescdata {
	unsigned char *key;
	NLchar value;
} _NLunesctab[];

#ifndef lint
/*  Translate single NLchar at nlc to char escape string at c.
 */
#ifdef _KJI
#define hextoa(c)       (((c) < 10) ? ('0' + (c)) : ('a' + ((c) - 10)))
#define atohex(c)       (((c) <= '9') ? ((c) - '0') : (((c) - 'a') + 10))
/*  Translate single NLchar at nlc to char escape string at c.
 */
#define NCeschex(nlc, c)    ((c)[0] = '\\', \
			    (c)[1] = '<', \
                            (c)[2] = hextoa ((*(nlc) >> 12) & 0xf), \
                            (c)[3] = hextoa ((*(nlc) >> 8) & 0xf), \
                            (c)[4] = hextoa ((*(nlc) >> 4) & 0xf), \
                            (c)[5] = hextoa (*(nlc) & 0xf), \
			    (c)[6] = '>')

/*  Translate hex escape string at c to single NLchar at nlc.
 */
#define NCuneschex(c, nlc)  ((nlc)[0] = (((atohex((c)[2])) & 0xf) << 12) | \
                                        (((atohex((c)[3])) & 0xf) << 8) | \
                                        (((atohex((c)[4])) & 0xf) << 4) | \
                                        ((atohex((c)[5])) & 0xf))
                                         
#define ishexesc(c)	    (((((c)[0] >= 'a' && (c)[0] <= 'f') || \
					((c)[0] >= '0' && (c)[0] <= '9')) && \
				(((c)[1] >= 'a' && (c)[1] <= 'f') || \
					((c)[1] >= '0' && (c)[1] <= '9')) && \
				(((c)[2] >= 'a' && (c)[2] <= 'f') || \
					((c)[2] >= '0' && (c)[2] <= '9')) && \
				(((c)[3] >= 'a' && (c)[3] <= 'f') || \
					((c)[3] >= '0' && (c)[3] <= '9'))) ? 1 : -1)
#endif   /* _KJI */
#endif   /* lint */

#endif	/* _NLCTYPE_H_ */
