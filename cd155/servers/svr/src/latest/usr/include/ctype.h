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
 * $Log: ctype.h,v $
 * Revision 1.3  1994/11/18  21:09:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:13:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:56:51  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:05  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:17:47  nandy
 * Initial revision
 *
 * Revision 2.6  92/04/13  14:34:46  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.6  90/10/07  20:26:28  devrcs
 * 	fix name-space pollution problems reported by VSX in POSIX-ONLY
 * 	[90/10/03  16:47:54  rabin]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  20:43:57  gm]
 * 
 * Revision 2.5  90/08/24  13:52:29  devrcs
 * 	Add a comment describing how the character classification defines are
 * 	intimately tied to those duplicated in ctab.c
 * 	[90/08/21  14:55:26  stevem]
 * 
 * Revision 2.4  90/04/27  23:06:15  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/16  13:47:23  tom]
 * 
 * Revision 2.3  90/03/13  21:21:33  mbrown
 * 	change _H_CTYPE to _CTYPE_H_ for consistancy
 * 	[90/02/15  14:48:01  tom]
 * 
 * 	AIX merge first cut.
 * 	[90/02/12  17:48:32  tom]
 * 
 * $EndLog$
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
