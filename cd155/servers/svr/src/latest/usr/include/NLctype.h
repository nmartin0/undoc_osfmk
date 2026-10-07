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
 * $Log: NLctype.h,v $
 * Revision 1.3  1994/11/18  21:09:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:13:01  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:56:27  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:14:36  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:17:12  nandy
 * Initial revision
 *
 * Revision 1.7  92/04/13  14:34:12  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.7  90/10/07  20:23:21  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:39:22  gm]
 * 
 * Revision 1.6  90/09/23  16:42:44  devrcs
 * 	Removed duplicate character classification defines.  They must be
 * 	in ctype.h, which is included in this file.
 * 	[90/09/12  16:49:54  glenn]
 * 
 * Revision 1.5  90/09/13  12:31:50  devrcs
 * 	upgrade to AIX gold for NLesctab
 * 	[90/08/27  10:04:05  mbrown]
 * 
 * Revision 1.4  90/05/13  20:43:17  devrcs
 * 	added _CHAR defines again
 * 	[90/05/09  18:55:56  mbrown]
 * 
 * Revision 1.3  90/04/27  23:05:36  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/16  13:46:31  tom]
 * 
 * Revision 1.2  90/03/13  21:20:53  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:05:15  tom]
 * 
 * $EndLog$
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
