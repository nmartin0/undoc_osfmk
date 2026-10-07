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
/* @(#)national.h	1.6  com/cmd/ksh/sh,3.1,9013 9/10/89 07:07:31 */
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

/* @(#)national.h	1.1 */

/*
 *  national.h -  definitions for international character sets
 *
 *   David Korn
 *   AT&T Bell Laboratories
 *   Room 5D-112
 *   Murray Hill, N. J. 07974
 *   Tel. x7975
 *
 */

/*
 * This data must be defined for each country in defs.c
 */

#ifndef HIGHBIT
# define HIGHBIT		0x80
#endif /* HIGHBIT */

#ifndef ESS_MAXCHAR	/* allow multiple includes */

/*
 *  This section may change from country to country
 */

#define ESS_MAXCHAR	2	/* Maximum number of non-escape bytes
				   for any and all character sets */
#define	CCS1_IN_SIZE	2
#define	CCS1_OUT_SIZE	2
#define	CCS2_IN_SIZE	1
#define	CCS2_OUT_SIZE	1
#define	CCS3_IN_SIZE	2
#define	CCS3_OUT_SIZE	2

/*
 * This part is generic
 */

#define MARKER		0x100	/* Must be invalid character */
#define ESS2		0x8e	/* Escape to char set 2 */
#define ESS3		0x8f	/* Escape to char set 3 */
#define ESS_SETMASK	(3<<(7*ESS_MAXCHAR))	/* character set bits */

#define	echarset(c)	((c)==ESS3?3:((c)==ESS2)?2:((c)>>7)&1)
#define icharset(i)	((i)>>(7*ESS_MAXCHAR)&3)

#define in_csize(s)	int_charsize[s]
#define out_csize(s)	int_charsize[s+4]

extern char int_charsize[8];

#endif /* ESS_MAXCHAR */
