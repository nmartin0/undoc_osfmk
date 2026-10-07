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
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)clock.h	1.6  com/inc/sys,3.1,8943 3/6/89 14:05:36 */
/* clock.h	5.2 87/01/09 18:20:40 */
/*
 * OSF/1 Release 1.0
 */

#ifndef _SYS_CLOCK_H_
#define _SYS_CLOCK_H_
 
#include        <sys/low.h>

#define	SECHR	(60*60)	/* seconds/hr */
#define	SECDAY	(24*SECHR)	/* seconds/day */
#define	SECYR	(365*SECDAY)	/* seconds/common year */
/*
 * if file system date is FSDATE days old
 * the user is warned that the date may need to be reset
 */
#define FSDATE  15

#define CLLEV	4
#define CLPSB PSB4
#define CL_EN	0x80000000	/* enable timer */
#define CL_DIS	0x40000000	/* disable timer */
#define CL_SLVL	0x20000000	/* set intr. level */
#define CL_SNTV	0x10000000	/* set interval */
#define CL_ST	0x08000000	/* set current time */
#define CL_EN60	0x01000000	/* turn on locore 60 hz timer */
#define CL_START (CL_EN|CL_SLVL|CL_SNTV|CL_EN60)
#define MAXCLKINTV (CLK_TCK/10)	/* never slower than 10 times a second */

extern int	clkintv;	/* default timer interval */
extern int	nxtclkintv;	/* next value for timer interval */
extern int	deltint;	/* elasped time between timer interrupts */
extern int	clklevel;	/* clock interrupt level */
extern time_t	deltime;	/* aiws lowcore time fudge factor */
extern char	clkints;	/* run with clock interrupts */
#endif	/* _SYS_CLOCK_H_ */
