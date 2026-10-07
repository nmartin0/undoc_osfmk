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
 * COMPONENT_NAME: CMDTTY tty control commands
 *
 * FUNCTIONS: header file for tset
 *
 * ORIGINS: 26, 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 */
/* @(#)tset-posix.h	1.1  com/cmd/tty/tset,3.1,9021 2/20/90 18:08:29 */

/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

/*
**  SYSTEM DEPENDENT TERMINAL DELAY TABLES
**
**	Evans Hall VAX
**
**	This file maintains the correspondence between the delays
**	defined in /etc/termcap and the delay algorithms on a
**	particular system.  For each type of delay, the bits used
**	for that delay must be specified (in XXbits) and a table
**	must be defined giving correspondences between delays and
**	algorithms.  Algorithms which are not fixed delays (such
**	as dependent on current column or line number) must be
**	cludged in some way at this time.
*/

/*
**  Carriage Return delays
*/

int	CRbits = CRDLY;
struct delay	CRdelay[] = {
    0,	CR0,
    80,	CR1,
    100, CR2,
    150, CR3,
    -1,
};

/*
**  TaB delays
*/

int	TBbits = TABDLY;
struct delay	TBdelay[] = {
    0,	TAB0,
    11,	TAB1,				/* special M37 delay */
    100, TAB2,
    -1,
};

/*
**  Back Space delays
*/

int	BSbits = BSDLY;
struct delay	BSdelay[] = {
    0,	BS0,
    50, BS1,
    -1,
};

/*
**  Form Feed delays
*/

int	FFbits = FFDLY;
struct delay	FFdelay[] = {
    0,	FF0,
    2000, FF1,
    -1,
};

/*
**  New Line delays
*/

int	NLbits = NLDLY;
struct delay	NLdelay[] = {
    0,	NL0,
    100, NL1,
    -1,
};

/*
**  Verticle tab delays
*/

int	VTbits = VTDLY;
struct delay	VTdelay[] = {
    0,	VT0,
    2000, VT1
    -1,
};
