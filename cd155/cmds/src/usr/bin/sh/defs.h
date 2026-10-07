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
 * OSF/1 Release 1.0.2
 */
/*****
static char rcsid[] = "@(#)$RCSfile: defs.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:00 $";
*****/
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

#include <sys/types.h>

/* error exits from various parts of shell */
#define 	ERROR		1
#define 	SYNBAD		2
#define 	SIGFAIL 	2000
#define	 	SIGFLG		0200

/* command tree */
#define 	FPRS		0x0100
#define 	FINT		0x0200
#define 	FAMP		0x0400
#define 	FPIN		0x0800
#define 	FPOU		0x1000
#define 	FPCL		0x2000
#define 	FCMD		0x4000
#define 	COMMSK		0x00F0
#define		CNTMSK		0x000F

#define 	TCOM		0x0000
#define 	TPAR		0x0010
#define 	TFIL		0x0020
#define 	TLST		0x0030
#define 	TIF			0x0040
#define 	TWH			0x0050
#define 	TUN			0x0060
#define 	TSW			0x0070
#define 	TAND		0x0080
#define 	TORF		0x0090
#define 	TFORK		0x00A0
#define 	TFOR		0x00B0
#define		TFND		0x00C0

/* execute table */
#define 	SYSSET		1
#define 	SYSCD		2
#define 	SYSEXEC		3
#define 	SYSNEWGRP 	4
#define 	SYSTRAP		5
#define 	SYSEXIT		6
#define 	SYSSHFT 	7
#define 	SYSWAIT		8
#define 	SYSCONT 	9
#define 	SYSBREAK	10
#define 	SYSEVAL 	11
#define 	SYSDOT		12
#define 	SYSRDONLY 	13
#define 	SYSTIMES 	14
#define 	SYSXPORT	15
#define 	SYSNULL 	16
#define 	SYSREAD 	17
#define		SYSTST		18
#define 	SYSLOGIN	19	
#define 	SYSUMASK 	20
#define 	SYSULIMIT 	21
#define 	SYSECHO		22
#define		SYSHASH		23
#define		SYSPWD		24
#define 	SYSRETURN	25
#define		SYSUNS		26
#define		SYSMEM		27
#define		SYSTYPE  	28
#define		SYSINLIB  	29
#define		SYSRMLIB  	30
	

/*io nodes*/
#define 	USERIO		10
#define 	IOUFD		15
#define 	IODOC		16
#define 	IOPUT		32
#define 	IOAPP		64
#define 	IOMOV		128
#define 	IORDW		256
#define		IOSTRIP		512
#define 	INPIPE		0
#define 	OTPIPE		1

/* arg list terminator */
#define 	ENDARGS		0

#include	"mac.h"
#include	"mode.h"
#include	"name.h"
#include	<signal.h>
#include        <stdio.h>


/* used for input and output of shell */
#define         INIO            (OPEN_MAX - 1)


/*	error catching */
extern int 		errno;

/* result type declarations */

#define 	alloc 		malloc
#ifdef	_SHRLIB
#define		free		alloc_free
#endif

extern uchar_t				*alloc();
extern uchar_t				*make();
extern uchar_t				*movstr();
extern uchar_t				*movstrn();
extern struct trenod	*cmd();
extern struct trenod	*makefork();
extern struct namnod	*lookup();
extern struct namnod	*findnam();
extern struct dolnod	*useargs();
extern float			expr();
extern uchar_t				*catpath();
extern uchar_t				*getpath();
extern uchar_t				*nextpath();
extern uchar_t				**scan();
extern uchar_t				*mactrim();
extern uchar_t				*macro();
extern int				exname();
extern int				printnam();
extern int				printro();
extern int				printexp();
extern uchar_t				**setenv();
extern uchar_t                             *NLSndecode();
extern uchar_t                             *scanset();

#define 	attrib(n,f)		(n->namflg |= f)
#define		round(a,b)		((((ulong)(a)+(b))-1)&~((b)-1))
#define 	closepipe(x)	(close(x[INPIPE]), close(x[OTPIPE]))
#define 	eq(a,b)			(cf(a,b)==0)
#define 	max(a,b)		((a)>(b)?(a):(b))
#define 	assert(x)		;

#define         NLSisencoded(s)         ((s == NULL)? 0 : *(s)==FNLS)
#define         NLSskiphdr(s)           (NLSisencoded(s)?(s)++:(s))
# ifdef KJI
#define         NLSneedesc(c)           ((c)>127||NLSfontshift(c)||(c)==FNLS)
#define		NLSenclen(c)		(((c) == FSH0) ? 2 \
					: (((c) == FSH20) || ((c) == FSH21) ?  \
					3 : 1))
# else
#define         NLSneedesc(c)           ((c)>127||(c)==FSH0||(c)==FNLS)
# endif


#include "sh_msg.h"
extern nl_catd catd;
#define MSGSTR(num,str) catgets(catd,MS_SH,num,str)

/* temp files and io */
extern int				output;
extern int				ioset;
extern struct ionod		*iotemp;	/* files to be deleted sometime */
extern struct ionod		*fiotemp;	/* function files to be deleted sometime */
extern struct ionod		*iopend;	/* documents waiting to be read at NL */
extern struct fdsave	fdmap[];


/* substitution */
extern int				dolc;
extern uchar_t				**dolv;
extern struct dolnod	*argfor;
extern struct argnod	*gchain;

/* stak stuff */
#include		"stak.h"

/* string constants */
extern uchar_t				atline[];
extern uchar_t				readmsg[];
extern uchar_t				colon[];
extern uchar_t				minus[];
extern uchar_t				nullstr[];
extern uchar_t				sptbnl[];
extern uchar_t				unexpected[];
extern uchar_t				endoffile[];
extern uchar_t				synmsg[];

/* name tree and words */
extern struct sysnod	reserved[];
extern int				no_reserved;
extern struct sysnod	commands[];
extern int				no_commands;

extern int				wdval;
extern int				wdnum;
extern int				fndef;
extern int				nohash;
extern struct argnod	*wdarg;
extern int				wdset;
extern BOOL				reserv;

/* prompting */
extern uchar_t				shstdprompt[];
extern uchar_t				shsupprompt[];
extern uchar_t				profile[];
extern uchar_t				sysprofile[];

/* built in names */
extern struct namnod	fngnod;
extern struct namnod	cdpnod;
extern struct namnod	ifsnod;
extern struct namnod	homenod;
extern struct namnod	mailnod;
extern struct namnod	pathnod;
extern struct namnod	ps1nod;
extern struct namnod	ps2nod;
extern struct namnod	mchknod;
extern struct namnod	acctnod;
extern struct namnod	mailpnod;
extern struct namnod    mailmnod;
extern struct namnod    timenod;
extern struct namnod    nlspathnod;
extern struct namnod    langnod;

/* special names */
extern uchar_t				flagadr[];
extern uchar_t				*pcsadr;
extern uchar_t				*pidadr;
extern uchar_t				*cmdadr;

extern uchar_t				shdefpath[];
extern uchar_t				sudefpath[];

/* names always present */
extern uchar_t				mailname[];
extern uchar_t				homename[];
extern uchar_t				pathname[];
extern uchar_t				cdpname[];
extern uchar_t				ifsname[];
extern uchar_t				ps1name[];
extern uchar_t				ps2name[];
extern uchar_t				shellname[];
extern uchar_t				mchkname[];
extern uchar_t				acctname[];
extern uchar_t				mailpname[];
extern uchar_t                             mailmname[];
extern uchar_t                             timename[];
extern uchar_t                             lang[];
extern uchar_t                             nlspath[];
extern uchar_t                             ctype[];
extern uchar_t                             collate[];
extern uchar_t                             monetary[];
extern uchar_t                             lctime[];
extern uchar_t                             numeric[];
extern uchar_t                             messages[];

/* transput */
extern uchar_t				tmpout[];
extern uchar_t				*tempname;
extern unsigned int				serial;

#define		TMPNAM 		7

extern struct fileblk	*standin;

#define 	input		(standin->fdes)
#define 	eof			(standin->sh_feof)

extern unsigned int				peekc;
extern unsigned int				peekn;
extern uchar_t				*comdiv;
extern uchar_t				devnull[];
extern unsigned int                     fshift;   /* readc() state */

/* flags */
#define		noexec		01
#define		sysflg		01
#define		intflg		02
#define		prompt		04
#define		setflg		010
#define		errflg		020
#define		ttyflg		040
#define		forked		0100
#define		oneflg		0200
#define		rshflg		0400
#define		waiting		01000
#define		stdflg		02000
#define		STDFLG		's'
#define		execpr		04000
#define		readpr		010000
#define		keyflg		020000
#define		hashflg		040000
#define		nofngflg	0200000
#define		exportflg	0400000
#ifdef NLSDEBUG
#define         debugflg        01000000
#endif

extern long				flags;
extern int				rwait;	/* flags read waiting */

/* error exits from various parts of shell */
#include	<setjmp.h>
extern jmp_buf			subshell;
extern jmp_buf			errshell;

/* fault handling */
#include	"brkincr.h"

extern unsigned			brkincr;
#define 	MINTRAP		0

#define 	TRAPSET		2
#define 	SIGSET		4
#define 	SIGMOD		8
#define 	SIGCAUGHT	16

extern void				fault(int);
extern BOOL				trapnote;
extern BOOL                             mailalarm;
extern uchar_t				*trapcom[];
extern BOOL				trapflg[];

/* name tree and words */
extern uchar_t				**environ;
extern uchar_t				numbuf[];
extern uchar_t				export[];
extern uchar_t				duperr[];
extern uchar_t				readonly[];

/* execflgs */
extern int				exitval;
extern int				retval;
extern BOOL				execbrk;
extern int				loopcnt;
extern int				breakcnt;
extern int				funcnt;

/* messages */
extern uchar_t				cd_args[];
extern uchar_t				mailmsg[];
extern uchar_t				coredump[];
extern uchar_t				badopt[];
extern uchar_t				badhash[];
extern uchar_t				badparam[];
extern uchar_t				unset[];
extern uchar_t				badsub[];
extern uchar_t				nospace[];
extern uchar_t				nostack[];
extern uchar_t				notfound[];
extern uchar_t				badtrap[];
extern uchar_t				baddir[];
extern uchar_t				badshift[];
extern uchar_t				restricted[];
extern uchar_t				execpmsg[];
extern uchar_t				notid[];
extern uchar_t 				badulimit[];
extern uchar_t				wtfailed[];
extern uchar_t				badcreate[];
extern uchar_t				nofork[];
extern uchar_t				noswap[];
extern uchar_t				piperr[];
extern uchar_t				badopen[];
extern uchar_t				badnum[];
extern uchar_t				arglist[];
extern uchar_t				txtbsy[];
extern uchar_t				toobig[];
extern uchar_t				badexec[];
extern uchar_t				badfile[];
extern uchar_t				badreturn[];
extern uchar_t				badexport[];
extern uchar_t				badunset[];
extern uchar_t				nohome[];
extern uchar_t				badperm[];
extern uchar_t				badinlib[];
extern uchar_t				badrmlib[];
extern uchar_t				no_args[];


/*	'builtin' error messages	*/

extern uchar_t				btest[];
extern uchar_t				badop[];

/*	fork constant	*/

#define 	FORKLIM 	32

#include	"shctype.h"

extern int				wasintr;	/* used to tell if break or delete is hit
				   					 *  while executing a wait
									 */
extern int				eflag;


/*
 * Find out if it is time to go away.
 * `trapnote' is set to SIGSET when fault is seen and
 * no trap has been set.
 */

#define		sigchk()	if (trapnote & SIGSET)	\
							exitsh(exitval ? exitval : SIGFAIL)

#define 	exitset()	retval = exitval
