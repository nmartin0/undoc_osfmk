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
 * $Log: locale.h,v $
 * Revision 1.3  1994/11/18  21:09:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:14:54  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:52  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:16:21  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:19:01  nandy
 * Initial revision
 *
 * Revision 1.8  92/04/13  14:36:16  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.8  90/10/31  15:15:37  devrcs
 * 	Put #include <ctype.h> back under _OSF_SOURCE.
 * 	[90/10/10            gm]
 * 
 * 	Remove #include <ctype.h>
 * 	[90/10/05  07:54:26  rabin]
 * 
 * Revision 1.7  90/10/07  20:29:14  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:48:02  gm]
 * 
 * Revision 1.6  90/09/23  16:43:19  devrcs
 * 	protected reentrant function declarations
 * 	[90/09/10  13:40:31  rabin]
 * 
 * Revision 1.5  90/09/13  12:32:13  devrcs
 * 	Correct the setlocale_r prototype
 * 	[90/08/29  12:03:57  sp]
 * 
 * 	fix _NO_PROTO
 * 	[90/08/28  15:08:54  mbrown]
 * 
 * Revision 1.4  90/08/09  14:26:25  devrcs
 * 	Changes for reentrant C library
 * 	[90/08/02  09:56:20  sp]
 * 
 * Revision 1.3  90/04/27  23:06:34  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/17  12:36:31  tom]
 * 
 * Revision 1.2  90/03/13  21:22:34  mbrown
 * 	fix for GCC to lconv struct
 * 	[90/02/25  14:33:19  mbrown]
 * 
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:22:12  tom]
 * 
 * $EndLog$
 */
/* @(#)locale.h 1.15  com/inc,3.1,9013 2/27/90 22:08:43 */
/*
 * COMPONENT_NAME: locale.h
 *                                                                    
 * Copyright International Business Machines Corp. 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */                                                                   

#ifndef _LOCALE_H_
#define _LOCALE_H_
#include <standards.h>

/*
 *      The ANSI, POSIX, and, XOPEN standards require that certain values be
 *	in locale.h.  They also require that if _ANSI_C_SOURCE is defined then 
 *	ONLY these values are present.
 *
 *      This header includes all the ANSI, POSIX, and XOPEN required entries.
 *	The requirements for all three standards are identical, therefore;
 *	only the _ANSI_C_SOURCE flag need be defined.
 *
 */

#ifdef _ANSI_C_SOURCE

typedef struct lconv {
   char *decimal_point;		/* decimal point character		*/
   char *thousands_sep;		/* thousands separator		 	*/
   char *grouping;		/* digit grouping		 	*/
   char *int_curr_symbol;	/* international currency symbol	*/
   char *currency_symbol;	/* national currency symbol		*/
   char *mon_decimal_point;	/* currency decimal point		*/
   char *mon_thousands_sep;	/* currency thousands separator		*/
   char *mon_grouping;		/* currency digits grouping		*/
   char *positive_sign;		/* currency plus sign			*/
   char *negative_sign;		/* currency minus sign		 	*/
   char int_frac_digits;	/* internat currency fractional digits	*/
   char frac_digits;		/* currency fractional digits		*/
   char p_cs_precedes;		/* currency plus location		*/
   char p_sep_by_space;		/* currency plus space ind.		*/
   char n_cs_precedes;		/* currency minus location		*/
   char n_sep_by_space;		/* currency minus space ind.		*/
   char p_sign_posn;		/* currency plus position		*/
   char n_sign_posn;		/* currency minus position		*/
} lconv;

#ifndef   _NO_PROTO

extern char *setlocale(int , const char *);
extern struct lconv *localeconv(void);

/* _POSIX_REENTRANT_FUNCTIONS */
#if defined(_THREAD_SAFE) || defined(_REENTRANT)
extern int setlocale_r(int , const char *, char *);
#endif

#else  /*_NO_PROTO */

extern char *setlocale();
extern struct lconv *localeconv();

/* _POSIX_REENTRANT_FUNCTIONS */
#if defined(_THREAD_SAFE) || defined(_REENTRANT)
extern int setlocale_r();
#endif

#endif /*_NO_PROTO */

#define LC_ALL		0xFFFF	/* name of locale's category name 	*/	
#define LC_COLLATE	0x0000	/* locale's collating sequence		*/
#define LC_CTYPE	0x0001	/* locale's ctype handling		*/
#define LC_MONETARY	0x0002	/* locale's monetary handling		*/
#define LC_NUMERIC	0x0003	/* locale's decimal handling		*/
#define LC_TIME		0x0004	/* locale's time handling		*/
#define LC_MESSAGES	0x0005	/* locale's messages handling		*/

#ifndef NULL
#define NULL	0
#endif

#endif /* _ANSI_C_SOURCE */

#ifdef	_OSF_SOURCE

#include <ctype.h>

#endif

#endif /* _LOCALE_H_ */

