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

