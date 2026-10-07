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
#define	ARPA_INET_BUGFIX	1	/* commands.c */
				/* include <arpa/inet.h> to resolve lint */
#define	BETTER_OFFER		1	/* telnet.c */
				/* new code to offer initial options */
#define	CARRIAGE_RETURN_FIXES	1	/* telnet.c */
				/* simplify and correct CR handling code */
#define	CHECK_BROADCAST_ADDR	1	/* commands.c */
				/* check for both 0 and -1 bcast addrs */
#define	CLEARER_NETTRACE	1	/* utilities.c */
				/* clearer nettrace messages */
#define	CLEARER_PRINTFS		1	/* commands.c */
				/* clearer command messages */
#define	DOUBLE_ESCAPE		1	/* commands.c/externs.h/telnet.c */
				/* support for sending 1 escape for 2 typed */
#if	CMUCS
#define	DOUBLE_ESCAPE_DEFAULT	1	/* default for DOUBLE_ESCAPE */
#else	/* CMUCS */
#define	DOUBLE_ESCAPE_DEFAULT	0	/* default for DOUBLE_ESCAPE */
#endif	/* CMUCS */
#define	EINTR_BUGFIX		1	/* commands.c */
				/* handle EINTR during connect() syscall */
#define	MINLEN_BUGFIX		1	/* utilities.c */
				/* fixed bug in TTYPE option length code */
#define	OFFER_OPTIONS		1	/* commands.c/externs.h/telnet.c */
				/* support toggle offer of initial options */
#define	OFFICIAL_TTYPE		1	/* sys_bsd.c */
				/* support sending "official" ttype first */
#define	OPTION_LFLOW		1	/* most modules */
				/* support LFLOW option */
#define	OPTION_NAWS		1	/* most modules */
				/* support NAWS option */
#if	EE_UWHERE
#define	OPTION_SNDLOC		1	/* most modules */
				/* support SNDLOC (ECE only) option */
#endif	/* EE_UWHERE */
#define	OPTION_TSPEED		1	/* most modules */
				/* support TSPEED option */
#if	CMUCS
#define	OPTION_TTYLOC		1	/* most modules */
				/* support TTYLOC (CMUCS only) option */
#endif	/* CMUCS */
#define	OPTION_XDISPLOC		1	/* most modules */
				/* support XDISTLOC (new) option */
#define	OWE_OPTIONS		1	/* commands.c/externs.h/telnet.c */
				/* defer command options until ready */
#define	PENDING_OPTIONS		1	/* most modules */
				/* support remembering pending options */
#define	SHORT_PROMPT		1	/* main.c */
				/* use tail of argv[0] for prompt */
#define	SHOW_INET_ADDR		1	/* commands.c */
				/* show internet address being used */
#define	SUBOPTION_SUPPORT	1	/* sys_bsd.c/telnet.c */
				/* general support for suboptions */
#define	TOGGLE_BUGFIX		1	/* commands.c */
				/* printf bugfix in toggle() command */
#define	USE_VARARGS		1	/* commands.c */
				/* changed call() to use varargs */
