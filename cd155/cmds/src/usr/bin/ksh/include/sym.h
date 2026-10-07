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
/* static char rcsid[] = "@(#)$RCSfile: sym.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:08 $"; */
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
 *	S. R. Bourne
 *	Rewritten by David Korn
 */


/* symbols for parsing */
#ifdef  KSH_88D
#define NOTSYM  '!'
#endif /* KSH_88D */
#define DOSYM	0405
#define FISYM	0420
#define EFSYM	0422
#define ELSYM	0421
#define INSYM	0412
#define BRSYM	0406
#define KTSYM	0450
#define THSYM	0444
#define ODSYM	0441
#define ESSYM	0442
#define IFSYM	0436
#define FORSYM	0435
#define WHSYM	0433
#define UNSYM	0427
#define CASYM	0417
#define PROCSYM	0460
#define SELSYM	0470
#define TIMSYM	0474
#define BTSTSYM (SYMREP|'[')
#define ETSTSYM (SYMREP|']')
#define TESTUNOP	0466
#define TESTBINOP	0467

#define SYMREP	04000
#define ECSYM	(SYMREP|';')
#define ANDFSYM	(SYMREP|'&')
#define ORFSYM	(SYMREP|'|')
#define APPSYM	(SYMREP|'>')
#define DOCSYM	(SYMREP|'<')
#define SYMALT1	01000
#define SYMALT2	010000
#ifdef  KSH_88D
#define ECASYM  (SYMALT1|'&')
#endif /* KSH_88D */
#define COOPSYM	(SYMALT1|'|')
#define IPROC	(SYMALT1|'(')
#define OPROC	(SYMALT2|'(')
#define EOFSYM	02000
#define SYMFLG	0400

/* arg to `sh_parse' */
#define NLFLG	1
#define MTFLG	2
#ifdef  KSH_88D
#define TOPFLG  4
#endif /* KSH_88D */

/* odd chars */
#undef ESCAPE
#define DQUOTE	'"'
#define SQUOTE	'`'
#define DOLLAR	'$'
#define LBRACE	'{'
#define RBRACE	'}'
#define LPAREN	'('
#define RPAREN	')'
#define ESCAPE	'\\'
#define	COMCHAR	'#'		/* comment delimiter */
#define ENDOF	0
#define LITERAL '\''            /* single quote */


/* wdset flags */
/*  KEYFLAG defined in defs.h,  the others must have a different value */
#define IN_CASE		1
#define CAN_ALIAS	2
#define IN_TEST		4
#define TEST_OP1	8
#define TEST_OP2	16

extern SYSTAB		tab_reserved;
extern SYSTAB		tab_options;
extern SYSTAB		tab_attributes;
extern const char	e_let[];
extern const char	e_unexpected[];
extern const char	e_unmatched[];
#ifdef DEVFD
    extern const char	e_devfd[];
#endif	/* DEVFD */
