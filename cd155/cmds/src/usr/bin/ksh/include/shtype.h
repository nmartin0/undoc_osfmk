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
/* static char rcsid[] = "@(#)$RCSfile: shtype.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:06 $"; */
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 
 *                  1988, 1989   AT&T
 *      All Rights Reserved

 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE 
 *      CODE OF AT&T.
 *      The copyright notice above does not 
 *      evidence any actual or intended
 *      publication of such source code.

 */
/*
 *	UNIX shell
 *
 *	S. R. Bourne
 *	AT&T Bell Laboratories
 *
 */

#include	"sh_config.h"

/* table 1 */
#define QUOTE	(~0177)
#define T_PSC	01	/* Special pattern characters, insert escape */
#define T_MET	02
#define	T_SPC	04
#define T_DIP	010
#define T_EXP	020	/* characters which require expansion or substitution */
#define T_EOR	040
#define T_QOT	0100
#define T_ESC	0200

/* table 2 */
#define T_ALP	01	/* alpha, but not upper or lower */
#define T_DEF	02
#define T_PAT	04	/* special chars when preceding () */
#define	T_DIG	010	/* digit */
#define T_UPC	020	/* uppercase only */
#define T_SHN	040	/* legal parameter characters */
#define	T_LPC	0100	/* lowercase only */
#define T_SET	0200

/* for single chars */
#define _TAB	(T_SPC)
#define _SPC	(T_SPC)
#define _ALP	(T_ALP)
#define _UPC	(T_UPC)
#define _LPC	(T_LPC)
#define _DIG	(T_DIG)
#define _EOF	(T_EOR)
#define _EOR	(T_EOR)
#define _BAR	(T_DIP|T_PSC)
#define _BRA	(T_MET|T_DIP|T_EXP|T_PSC)
#define _KET	(T_MET|T_PSC)
#define _AMP	(T_DIP|T_PSC)
#define _SEM	(T_DIP)
#define _LT	(T_DIP)
#define _GT	(T_DIP)
#define _LQU	(T_QOT|T_ESC|T_EXP)
#define _QU1	T_EXP
#define _AST1	T_EXP
#define _BSL	(T_ESC|T_PSC)
#define _DQU	(T_QOT)
#define _DOL1	(T_ESC|T_EXP)

#define _CKT	T_DEF
#define _AST	(T_PAT|T_SHN)
#define _EQ	(T_DEF)
#define _MIN	(T_DEF|T_SHN)
#define _PCS	(T_DEF|T_SHN|T_SET|T_PAT)
#define _NUM	(T_DEF|T_SHN|T_SET)
#define _DOL2	(T_SHN|T_PAT)
#define _PLS	(T_DEF|T_SET|T_PAT)
#define _AT	(T_SHN|T_PAT)
#define _QU	(T_DEF|T_SHN|T_PAT)
#define _LPAR	T_SHN
#define _SS2	T_ALP
#define _SS3	T_ALP

/* abbreviations for tests */
#define _IDCH	(T_UPC|T_LPC|T_DIG|T_ALP)
#define _META	(T_SPC|T_DIP|T_MET|T_EOR)

extern const unsigned char	_ctype1[];

/* these args are not call by value !!!! */
#if (defined(NLS) || defined(KJI))

#include <NLctype.h>

#undef isspace
#undef  isdigit
#undef islower
#undef isupper
#undef isalnum
#undef isalpha
#undef dolchar

#ifdef NLS

#define	isblank(c)	((c&~0377)==0 && (_ctype1[c]&(T_SPC)))
#define	isspace(c)	((c&~0377)==0 && (_ctype1[c]&(T_SPC|T_EOR)))
#define ismeta(c)	((c&~0377)==0 && (_ctype1[c]&(_META)))
#define isqmeta(c)	((c&~0377)==0 && (_ctype1[c]&(_META|T_QOT)))
#define NLSletter(c) 	((c) > 0177 || NCisalpha(c) || (c)=='_')
#define NLSalphanum(c)	((c) > 0177 || NCisalnum(c) || (c)=='_')

#else /* KJI */

#define	isblank(c)	(isjspace(c) || ((c&~0377)==0 && (_ctype1[c]&(T_SPC))))
#define	isspace(c)	(isjspace(c) || ((c&~0377)==0 && (_ctype1[c]&(T_SPC|T_EOR))))
#define ismeta(c)	(((c&~0377)==0 && (_ctype1[c]&(_META))) || isjspace(c))
#define isqmeta(c)	(((c&~0377)==0 && (_ctype1[c]&(_META|T_QOT))) || isjspace(c))
#define NLSletter(c) ((c)=='_' || NCisalpha(c) || isjkanji(c) || isjkata(c) || isjhira(c))
#define NLSalphanum(c)	(NLSletter(c) || NCisdigit(c))

#endif /* NLS */


#define qotchar(c)	((c&~0377)==0 && (_ctype1[c]&(T_QOT)))
#define eolchar(c)	((c&~0377)==0 && (_ctype1[c]&(T_EOR)))
#define dipchar(c)	((c&~0377)==0 && (_ctype1[c]&(T_DIP)))
#define addescape(c)	((c&~0377)==0 && (_ctype1[c]&(T_PSC)))
#define subchar(c)	((c&~0377)==0 && (_ctype1[c]&(T_SUB|T_QOT)))
#define escchar(c)	((c&~0377)==0 && (_ctype1[c]&(T_ESC)))
#ifdef  KSH_88D
#define expchar(c)      ((c&~0377)==0 && (_ctype1[c]&(T_EXP|_META|T_QOT)))
#else
#define expchar(c)      ((c&~0377)==0 && (_ctype1[c]&(T_EXP|T_SPC)))
#endif /* KSH_88D */
#define isexp(c)	((c&~0377)==0 && (_ctype1[c]&T_EXP))
#define iochar(c)	((c&~0377)==0 && ((c)=='<'||(c)=='>'))

extern const unsigned char	_ctype2[];

#define	isdigit(c)	((c&~0377)==0 && (_ctype2[c]&(T_DIG)))
#define dolchar(c)	((c&~0377)==0 && (_ctype2[c]&(_IDCH|T_SHN)))
#define patchar(c)	((c&~0377)==0 && (_ctype2[c]&(T_PAT)))
#define defchar(c)	((c&~0377)==0 && (_ctype2[c]&(T_DEF)))
#define setchar(c)	((c&~0377)==0 && (_ctype2[c]&(T_SET)))
#define	isalpha(c)	((c&~0377)==0 && (_ctype2[c]&(T_UPC|T_LPC|T_ALP)))
#define isalnum(c)	((c&~0377)==0 && (_ctype2[c]&(_IDCH)))
#define isupper(c)	((c&~0377)==0 && (_ctype2[c]&(T_UPC)))
#define islower(c)	((c&~0377)==0 && (_ctype2[c]&(T_LPC)))
#define astchar(c)	((c&~0377)==0 && ((c)=='*'||(c)=='@'))

#else

#define	isblank(c)	(_ctype1[c]&(T_SPC))
#define	isspace(c)	(_ctype1[c]&(T_SPC|T_EOR))
#define ismeta(c)	(_ctype1[c]&(_META))
#define isqmeta(c)	(_ctype1[c]&(_META|T_QOT))
#define qotchar(c)	(_ctype1[c]&(T_QOT))
#define eolchar(c)	(_ctype1[c]&(T_EOR))
#define dipchar(c)	(_ctype1[c]&(T_DIP))
#define addescape(c)	(_ctype1[c]&(T_PSC))
#define escchar(c)	(_ctype1[c]&(T_ESC))
#ifdef  KSH_88D
#define expchar(c)      (_ctype1[c]&(T_EXP|_META|T_QOT))
#else
#define expchar(c)      (_ctype1[c]&(T_EXP|T_SPC))
#endif
#define isexp(c)	(_ctype1[c]&T_EXP)
#define iochar(c)	((c)=='<'||(c)=='>')

extern const unsigned char	_ctype2[];

#define	isprint(c)	(((c)&0340) && ((c)!=0177))
#define	isdigit(c)	(_ctype2[c]&(T_DIG))
#define dolchar(c)	(_ctype2[c]&(_IDCH|T_SHN))
#define patchar(c)	(_ctype2[c]&(T_PAT))
#define defchar(c)	(_ctype2[c]&(T_DEF))
#define setchar(c)	(_ctype2[c]&(T_SET))
#define	isalpha(c)	(_ctype2[c]&(T_UPC|T_LPC|T_ALP))
#define isalnum(c)	(_ctype2[c]&(_IDCH))
#define isupper(c)	(_ctype2[c]&(T_UPC))
#define islower(c)	(_ctype2[c]&(T_LPC))
#define astchar(c)	((c)=='*'||(c)=='@')
#define toupper(c)	((c) + 'A' - 'a')
#define tolower(c)	((c) + 'a' - 'A')
#endif  (NLS | KJI)

