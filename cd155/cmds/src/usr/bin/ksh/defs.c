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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: defs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:11 $";
#endif
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 27
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
 * Ksh - AT&T Bell Laboratories
 * Written by David Korn
 * This file defines all the  read/write shell global variables
 */

#include	"defs.h"
#include	"jobs.h"
#include	"sym.h"
#include	"history.h"
#include	"edit.h"
#include	"timeout.h"


struct sh_scoped	st;
struct sh_static	sh;
pid_t			line_pid;	/*  PTM 12563 and 13362 */

#ifdef VSH
    struct	edit	editb;
#else
#   ifdef ESH
	struct	edit	editb;
#   endif /* ESH */
#endif	/* VSH */

struct history	*hist_ptr;
struct jobs	job;
int		sh_lastbase = 10; 
time_t		sh_mailchk = 600;
#ifdef TIMEOUT
    long		sh_timeout = TIMEOUT;
#else
    long		sh_timeout = 0;
#endif /* TIMEOUT */
char		io_tmpname[] = "/tmp/shxxxxxx.aaa";

#ifdef 	NOBUF
    char	_sibuf[IOBSIZE+1];
    char	_sobuf[IOBSIZE+1];
#endif	/* NOBUF */

struct fileblk io_stdin = { _sibuf, _sibuf, _sibuf, 0, IOREAD, 0, F_ISFILE};
struct fileblk io_stdout = { _sobuf, _sobuf, _sobuf+IOBSIZE, 0, IOWRT,2};
struct fileblk *io_ftable[NFILE];

#ifdef MULTIBYTE
/*
 * These are default values.  They can be changed with CSWIDTH
 */

char int_charsize[] =
{
	1, CCS1_IN_SIZE, CCS2_IN_SIZE, CCS3_IN_SIZE,	/* input sizes */
	1, CCS1_OUT_SIZE, CCS2_OUT_SIZE, CCS3_OUT_SIZE	/* output widths */
};
#endif /* MULTIBYTE */
#if !(defined(NLS) || defined(KJI) || defined(MULTIBYTE))
char int_charsize[] =
{
	1, 0, 0, 0,	/* input sizes */
	1, 0, 0, 0	/* output widths */
};
#endif /* !NLS && !KJI && !MULTIBYTE */



