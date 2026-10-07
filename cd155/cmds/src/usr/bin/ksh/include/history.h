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
/* @(#)history.h	1.7  com/cmd/ksh/sh,3.1,9013 11/6/89 19:36:11 */
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
 *	Header File for history mechanism
 *	written by David Korn
 *
 */



#ifndef IOBSIZE
#   define IOBSIZE	1024
#endif
#define FC_CHAR		'!'
#define HIS_DFLT	128		/* default size of history list */
#define HISMAX		(sizeof(int)*IOBSIZE)
#define HISBIG		(0100000-1024)	/* 1K less than maximum short */
#define HISLINE		16		/* estimate of average sized history line */
#define MAXLINE		258		/* longest history line permitted */

#define H_UNDO		0201		/* invalidate previous command */
#define H_CMDNO		0202		/* next 3 bytes give command number */
#define H_VERSION	1		/* history file format version no. */

struct history
{
	struct fileblk	*fixfp;		/* file descriptor for history file */
	int 		fixfd;		/* file number for history file */
	char		*fixname;	/* name of history file */
	off_t		fixcnt;		/* offset into history file */
	int		fixind;		/* current command number index */
	int		fixmax;		/* number of accessible history lines */
	int		fixflush;	/* set if flushed outside of hflush() */
	off_t		fixcmds[1];	/* byte offset for recent commands */
};

typedef struct
{
	short his_command;
	short his_line;
} histloc;

extern struct history	*hist_ptr;

#ifndef KSHELL
#	if (defined(NLS) || defined(KJI))
		extern char *NLgetenv();
#	else
		extern char *getenv();
#	endif
    extern void	p_flush();
    extern void	p_setout();
    extern void	p_char();
    extern void	p_str();
#	if (defined(NLS) || defined(KJI))
#	   define nam_strval(s)	NLgetenv("s")
#	else
#	   define nam_strval(s)	getenv("s")
#	endif
#   define NIL		((char*)0)
#   define sh_fail	ed_failed
#   define sh_copy	ed_movstr
#endif	/* KSHELL */

/* the following are readonly */
extern const char	hist_fname[];
extern const char	e_history[];

/* these are the history interface routines */
#ifdef _NO_PROTO
    extern int		hist_open();
    extern void 	hist_cancel();
    extern void 	hist_close();
    extern int		hist_copy();
    extern void 	hist_eof();
    extern histloc	hist_find();
    extern void 	hist_flush();
    extern void 	hist_list();
    extern int		hist_match();
    extern off_t	hist_position();
    extern void 	hist_subst();
    extern char 	*hist_word();
#   ifdef ESH
	extern histloc	hist_locate();
#   endif	/* ESH */
#else /* ~ _NO_PROTO */
    extern int		hist_open(void);
    extern void 	hist_cancel(void);
    extern void 	hist_close(void);
    extern int		hist_copy(char*,int,int);
    extern void 	hist_eof(void);
    extern histloc	hist_find(char*,int,int,int);
    extern void 	hist_flush(void);
    extern void 	hist_list(off_t,int,char*);
    extern int		hist_match(off_t,char*,int);
    extern off_t	hist_position(int);
    extern void 	hist_subst(const char*,int,char*);
    extern char 	*hist_word(char*,int);
#   ifdef ESH
	extern histloc	hist_locate(int,int,int);
#   endif	/* ESH */
#endif /* _NO_PROTO */
