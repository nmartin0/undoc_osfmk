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
/* @(#)ctype.h	1.21  com/inc,3.1,9013 2/27/90 22:08:15 */
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
#ifndef _CTYPE_H_
#define _CTYPE_H_

#include <standards.h>

/*
 *
 *      The ANSI and XOPEN standards require that certain values be in ctype.h.
 *      They also require that if _ANSI_C_SOURCE or _XOPEN_SOURCE is defined
 *      then ONLY those standard specific values are present. This header
 *      includes all the ANSI and XOPEN required entries.
 *
 */

#ifdef   _ANSI_C_SOURCE

#ifdef _NO_PROTO

extern int isalpha();
extern int isalnum();
extern int isdigit();
extern int iscntrl();
extern int ispunct();
extern int isprint();
extern int isspace();
extern int isgraph();
extern int isxdigit();
extern int islower();
extern int isupper();
extern int toupper();
extern int tolower();

#else /* _NO_PROTO */

extern int isalpha(int);
extern int isalnum(int);
extern int iscntrl(int);
extern int isdigit(int);
extern int isgraph(int);
extern int isprint(int);
extern int ispunct(int);
extern int isspace(int);
extern int isxdigit(int);
extern int isupper(int);
extern int islower(int);
extern int tolower(int);
extern int toupper(int);

#endif /* _NO_PROTO */

#endif /*_ANSI_C_SOURCE */

#ifdef _XOPEN_SOURCE

#include <NLctype.h>

/*
 *  NOTE!!!  All of these character classifications are duplicated in
 *	     usr/bin/ctab.c and they must always match EXACTLY.  Change
 *	     them here and the identical change(s) must be made to
 *	     ctab.c!
 */
#define	_U	   01
#define	_L	   02
#define	_N	   04
#define	_S	  010
#define	_P	  020
#define	_C	  040
#define	_B	 0100
#define	_X	 0200
#define _A       0400
#define _G      01000

#define _ctype  (_locp->lc_chrtbl)->lc_ctype

#ifdef _NO_PROTO
extern int isascii();
extern int toascii();

#else	/* _NO_PROTO */

extern int isascii(int);
extern int toascii(int);
#endif /* _NO_PROTO */

/* To ensure correct funtionality outside ASCII, the following macros
 * now  use the same code as the corresponding functions.
 */
#define	_toupper(c)	( ((_locp->lc_chrtbl)->lc_caseconv+1)[c] )
#define	_tolower(c)	( ((_locp->lc_chrtbl)->lc_caseconv+1)[c] )

#endif /* _XOPEN_SOURCE */
 
#endif /* _CTYPE_H_ */
