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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
/***
static char rcsid[] = "@(#)$RCSfile: mac.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:11 $";
***/
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
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

#undef TRUE
#define TRUE	(-1)
#define FALSE	0
#define LOBYTE	0377
#define STRIP   0177
#define QUOTE	0200

#define EOF	0
#define NL	'\n'
#define SP	' '
#define LQ	'`'
#define RQ	'\''
#define MINUS	'-'
#define COLON	':'
#define TAB	'\t'
#ifndef KJI
/*
 * The values of FSH0 and FNLS are arbitrary, in theory, but trouble does
 * in fact occur if either one appears in a string.  The current workaround
 * is to use ^? and ^V, which are the default INTR and QUIT characters, so
 * that the user is unlikely to encounter the problem.  The correct fix is
 * probably to use arrays of NLchar instead of "encoded strings"; this would
 * make both of these constants obsolete.
 */
#define FSH0    '\177'
#define FNLS    '\026'
#else	/* KJI */
#define FSH0	'\35'
#define FSH20	'\36'
#define FSH21	'\37'
#define FNLS    '\026'
#endif	/* KJI */

#define MAX(a,b)	((a)>(b)?(a):(b))

#define blank()		prc(SP)
#define	tab()		prc(TAB)
#define newline()	prc(NL)

