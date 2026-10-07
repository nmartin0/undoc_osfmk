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
static char rcsid[] = "@(#)$RCSfile: defs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:00 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
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
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

#include		<sys/limits.h>
#include		<sys/types.h>
#include 		<setjmp.h>
#include		"mode.h"
#include		"name.h"

/* temp files and io */

int				output = 2;
int				ioset;
struct ionod	*iotemp;	/* files to be deleted sometime */
struct ionod	*fiotemp;	/* function files to be deleted sometime */
struct ionod	*iopend;	/* documents waiting to be read at NL */
struct fdsave	fdmap[OPEN_MAX];

/* substitution */
int				dolc;
uchar_t			**dolv;
struct dolnod	*argfor;
struct argnod	*gchain;


/* name tree and words */
int				wdval;
int				wdnum;
int				fndef;
int				nohash;
struct argnod	*wdarg;
int				wdset;
BOOL			reserv;

/* special names */
uchar_t			*pcsadr;
uchar_t			*pidadr;
uchar_t			*cmdadr;

/* transput */ 
uchar_t 			*tempname;
unsigned int 			serial; 
unsigned int 			peekc;
unsigned int 			peekn;
uchar_t 			*comdiv;

long			flags;
int				rwait;	/* flags read waiting */

/* error exits from various parts of shell */
jmp_buf			subshell;
jmp_buf			errshell;

/* fault handling */
BOOL			trapnote;
BOOL                    mailalarm;

/* execflgs */
int				exitval;
int				retval;
BOOL			execbrk;
int				loopcnt;
int				breakcnt;
int 			funcnt;

int				wasintr;	/* used to tell if break or delete is hit
				   			   while executing a wait
							*/

int				eflag;

/* The following stuff is from stak.h	*/

uchar_t 			*stakbas;
uchar_t			*staktop;
uchar_t			*stakbot;
uchar_t			*stakbsy;
uchar_t 			*brkend;
