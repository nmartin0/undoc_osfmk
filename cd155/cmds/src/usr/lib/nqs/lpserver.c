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
 
/*++ lpserver.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/lpserver.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS dumb line printer server.
 *
 *
 *	Character set definition author:
 *	--------------------------------
 *	Bill Shannon
 *	December 21, 1978.
 *
 *	Server Author:
 *	--------------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	September 3, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:52:30 $ $State: Exp $)
 * $Log: lpserver.c,v $
 * Revision 1.3  1994/11/19  02:52:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:24:21  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:20  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:01:59  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)lpserver.c	1.2 (lpserver.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <pwd.h>
#include <signal.h>
#include <errno.h>
#if	SGI | SYS52 | UTS | UNICOS | OSF	/* Lpserver doesn't work on UNICOS */
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termio.h>
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sgtty.h>
#include <fcntl.h>			/* added for SUN */
#include <termios.h>                    /* for SUN */
#else
BAD SYSTEM TYPE
#endif
#endif
#include "nqs.h"			/* Types and definitions */
#include "requestcc.h"			/* Request completion codes */
#include "nqsdirs.h"			/* For nqslib library functions */

#if UTS
#define B19200	14
#endif

/*
 *
 *	External variables and functions.
 */
extern int errno;			/* System call error number */
extern char *sys_errlist [];		/* System error names */
extern int atoi ();			/* ASCII to integer conversion */
extern void bufstdout();		/* Block buffer stdout */
extern void bufstderr();		/* Block buffer stderr */
extern void closepwdb();		/* Close account/password database */
extern struct passwd *fetchpwuid ();	/* Fetch password entry for user-id */
extern char *fmtmidname();		/* Format name for machine-id */
extern char *fmttime();			/* Return pointer to formatted */
					/* time string */
extern FILE *fopendata();		/* Open an NQS data file stream */
extern int gethostname();		/* get name of local host */
extern int readreq();			/* Read NQS req request header */
                       /*Intergraph*/
extern int wait();                    /* wait for child if a model */
extern char *getenv();                        /* to get DEBUG in env */
extern void pack6name();              /* To generate filename */

extern void serexit();			/* Server exit procedure */
extern long time();


/*
 *
 *	Variables global within and local to this module.
 */
int pagelength;				/* Printer page length */
short formfeeds;			/* BOOLEAN printer device understands */
					/* the formfeed character: '\f' */
short interlock;			/* Signal interlock (see donewline(),*/
					/* doformfeed(), and restore()). */
unsigned lines;				/* Number of lines written to stdout */
					/* on current page */
unsigned pages;				/* Number of pages printed */
short queued_signal;			/* BOOLEAN TRUE if a signal has been */
					/* queued (see donewline(), */
					/* doformfeed(), and restore()). */
short signal_received;			/* Signal that was queued if */
					/* queued_signal is TRUE.  See */
					/* restore(). */
#if	SGI | SYS52 | UTS | UNICOS | OSF
static struct termio save;		/* Original printer characteristics */
#else
#if	BSD42 | BSD43 | ULTRIX
static struct sgttyb save;		/* Original printer characteristics */
#else
BAD SYSTEM TYPE
#endif
#endif


 /*
  * Intergraph global variables
  */
       int translate;
       int no_preprocess;
       int noreturn;
       static int Debug;
	int ret;

	char *model_interface, *ptr;
/*
 *	Defaults and limits.
 */
#define	DEFAULT_PAGEWIDTH	132	/* Default page-width is 132 columns */
#define	DEFAULT_PAGELENGTH	66	/* Default page-length is 66 lines */
#define	MIN_PAGEWIDTH		64	/* Minimum page-width in characters */
#define	MAX_PAGEWIDTH		132	/* Maximum page-width in characters */
#define	MIN_PAGELENGTH		51	/* Minimum page-length in lines */
#define	MAX_PAGELENGTH		67	/* Maximum page-length in lines */
#define	MAX_OVERSTRIKE		20	/* Maximum number of overstrikes */
					/* per column of a given output line */


/*
 *
 *	Banner character set definition.
 */

#define	BANNER_CHAR	'#'	/* Character to be used to display banner */
				/* characters */
#define	CHEIGHT	9		/* character matrix height */
#define	CWIDTH	7		/* character matrix width */
#define	CDEPTH	3		/* number of lines to drop desenders */
#define	CBEFORE	1		/* space before character */
#define	CAFTER	1		/* space after character */
#define	CABOVE	1		/* space above character */
#define	CBELOW	1		/* space below character */

static short chset[96][CHEIGHT] = 
{

#define c_______ 0
#define c______1 01
#define c_____1_ 02
#define c____1__ 04
#define c____11_ 06
#define c___1___ 010
#define c___1__1 011
#define c___1_1_ 012
#define c___11__ 014
#define c__1____ 020
#define c__1__1_ 022
#define c__1_1__ 024
#define c__11___ 030
#define c__111__ 034
#define c__111_1 035
#define c__1111_ 036
#define c__11111 037
#define c_1_____ 040
#define c_1____1 041
#define c_1___1_ 042
#define c_1__1__ 044
#define c_1_1___ 050
#define c_1_1__1 051
#define c_1_1_1_ 052
#define c_11____ 060
#define c_11_11_ 066
#define c_111___ 070
#define c_111__1 071
#define c_111_1_ 072
#define c_1111__ 074
#define c_1111_1 075
#define c_11111_ 076
#define c_111111 077
#define c1______ 0100
#define c1_____1 0101
#define c1____1_ 0102
#define c1____11 0103
#define c1___1__ 0104
#define c1___1_1 0105
#define c1___11_ 0106
#define c1__1___ 0110
#define c1__1__1 0111
#define c1__11_1 0115
#define c1__1111 0117
#define c1_1____ 0120
#define c1_1___1 0121
#define c1_1_1_1 0125
#define c1_1_11_ 0126
#define c1_111__ 0134
#define c1_1111_ 0136
#define c11____1 0141
#define c11___1_ 0142
#define c11___11 0143
#define c11_1___ 0150
#define c11_1__1 0151
#define c111_11_ 0166
#define c1111___ 0170
#define c11111__ 0174
#define c111111_ 0176
#define c1111111 0177

	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,			/*   */

	c__11___,
	c__11___,
	c__11___,
	c__11___,
	c__11___,
	c_______,
	c_______,
	c__11___,
	c__11___,			/* ! */

	c_1__1__,
	c_1__1__,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,			/* " */

	c_______,
	c__1_1__,
	c__1_1__,
	c1111111,
	c__1_1__,
	c1111111,
	c__1_1__,
	c__1_1__,
	c_______,			/* # */

	c___1___,
	c_11111_,
	c1__1__1,
	c1__1___,
	c_11111_,
	c___1__1,
	c1__1__1,
	c_11111_,
	c___1___,			/* $ */

	c_1_____,
	c1_1___1,
	c_1___1_,
	c____1__,
	c___1___,
	c__1____,
	c_1___1_,
	c1___1_1,
	c_____1_,			/* % */

	c_11____,
	c1__1___,
	c1___1__,
	c_1_1___,
	c__1____,
	c_1_1__1,
	c1___11_,
	c1___11_,
	c_111__1,			/* & */

	c___11__,
	c___11__,
	c___1___,
	c__1____,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,			/* ' */

	c____1__,
	c___1___,
	c__1____,
	c__1____,
	c__1____,
	c__1____,
	c__1____,
	c___1___,
	c____1__,			/* ( */

	c__1____,
	c___1___,
	c____1__,
	c____1__,
	c____1__,
	c____1__,
	c____1__,
	c___1___,
	c__1____,			/* ) */

	c_______,
	c___1___,
	c1__1__1,
	c_1_1_1_,
	c__111__,
	c_1_1_1_,
	c1__1__1,
	c___1___,
	c_______,			/* * */

	c_______,
	c___1___,
	c___1___,
	c___1___,
	c1111111,
	c___1___,
	c___1___,
	c___1___,
	c_______,			/* + */

	c_______,
	c_______,
	c_______,
	c_______,
	c__11___,
	c__11___,
	c__1____,
	c_1_____,
	c_______,			/* , */

	c_______,
	c_______,
	c_______,
	c_______,
	c1111111,
	c_______,
	c_______,
	c_______,
	c_______,			/* - */

	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c__11___,
	c__11___,			/* . */

	c_______,
	c______1,
	c_____1_,
	c____1__,
	c___1___,
	c__1____,
	c_1_____,
	c1______,
	c_______,			/* / */

	c_11111_,
	c1_____1,
	c1____11,
	c1___1_1,
	c1__1__1,
	c1_1___1,
	c11____1,
	c1_____1,
	c_11111_,			/* 0 */

	c___1___,
	c__11___,
	c_1_1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c_11111_,			/* 1 */

	c_11111_,
	c1_____1,
	c______1,
	c_____1_,
	c__111__,
	c_1_____,
	c1______,
	c1______,
	c1111111,			/* 2 */

	c_11111_,
	c1_____1,
	c______1,
	c______1,
	c__1111_,
	c______1,
	c______1,
	c1_____1,
	c_11111_,			/* 3 */

	c_____1_,
	c____11_,
	c___1_1_,
	c__1__1_,
	c_1___1_,
	c1____1_,
	c1111111,
	c_____1_,
	c_____1_,			/* 4 */

	c1111111,
	c1______,
	c1______,
	c11111__,
	c_____1_,
	c______1,
	c______1,
	c1____1_,
	c_1111__,			/* 5 */

	c__1111_,
	c_1_____,
	c1______,
	c1______,
	c1_1111_,
	c11____1,
	c1_____1,
	c1_____1,
	c_11111_,			/* 6 */

	c1111111,
	c1_____1,
	c_____1_,
	c____1__,
	c___1___,
	c__1____,
	c__1____,
	c__1____,
	c__1____,			/* 7 */

	c_11111_,
	c1_____1,
	c1_____1,
	c1_____1,
	c_11111_,
	c1_____1,
	c1_____1,
	c1_____1,
	c_11111_,			/* 8 */

	c_11111_,
	c1_____1,
	c1_____1,
	c1_____1,
	c_111111,
	c______1,
	c______1,
	c1_____1,
	c_1111__,			/* 9 */

	c_______,
	c_______,
	c_______,
	c__11___,
	c__11___,
	c_______,
	c_______,
	c__11___,
	c__11___,			/* : */


	c__11___,
	c__11___,
	c_______,
	c_______,
	c__11___,
	c__11___,
	c__1____,
	c_1_____,
	c_______,			/* ; */

	c____1__,
	c___1___,
	c__1____,
	c_1_____,
	c1______,
	c_1_____,
	c__1____,
	c___1___,
	c____1__,			/* < */

	c_______,
	c_______,
	c_______,
	c1111111,
	c_______,
	c1111111,
	c_______,
	c_______,
	c_______,			/* = */

	c__1____,
	c___1___,
	c____1__,
	c_____1_,
	c______1,
	c_____1_,
	c____1__,
	c___1___,
	c__1____,			/* > */

	c__1111_,
	c_1____1,
	c_1____1,
	c______1,
	c____11_,
	c___1___,
	c___1___,
	c_______,
	c___1___,			/* ? */

	c__1111_,
	c_1____1,
	c1__11_1,
	c1_1_1_1,
	c1_1_1_1,
	c1_1111_,
	c1______,
	c_1____1,
	c__1111_,			/* @ */

	c__111__,
	c_1___1_,
	c1_____1,
	c1_____1,
	c1111111,
	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,			/* A */

	c111111_,
	c_1____1,
	c_1____1,
	c_1____1,
	c_11111_,
	c_1____1,
	c_1____1,
	c_1____1,
	c111111_,			/* B */

	c__1111_,
	c_1____1,
	c1______,
	c1______,
	c1______,
	c1______,
	c1______,
	c_1____1,
	c__1111_,			/* C */

	c11111__,
	c_1___1_,
	c_1____1,
	c_1____1,
	c_1____1,
	c_1____1,
	c_1____1,
	c_1___1_,
	c11111__,			/* D */

	c1111111,
	c1______,
	c1______,
	c1______,
	c111111_,
	c1______,
	c1______,
	c1______,
	c1111111,			/* E */

	c1111111,
	c1______,
	c1______,
	c1______,
	c111111_,
	c1______,
	c1______,
	c1______,
	c1______,			/* F */

	c__1111_,
	c_1____1,
	c1______,
	c1______,
	c1______,
	c1__1111,
	c1_____1,
	c_1____1,
	c__1111_,			/* G */

	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,
	c1111111,
	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,			/* H */

	c_11111_,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c_11111_,			/* I */

	c__11111,
	c____1__,
	c____1__,
	c____1__,
	c____1__,
	c____1__,
	c____1__,
	c1___1__,
	c_111___,			/* J */

	c1_____1,
	c1____1_,
	c1___1__,
	c1__1___,
	c1_1____,
	c11_1___,
	c1___1__,
	c1____1_,
	c1_____1,			/* K */

	c1______,
	c1______,
	c1______,
	c1______,
	c1______,
	c1______,
	c1______,
	c1______,
	c1111111,			/* L */

	c1_____1,
	c11___11,
	c1_1_1_1,
	c1__1__1,
	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,			/* M */

	c1_____1,
	c11____1,
	c1_1___1,
	c1__1__1,
	c1___1_1,
	c1____11,
	c1_____1,
	c1_____1,
	c1_____1,			/* N */

	c__111__,
	c_1___1_,
	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,
	c_1___1_,
	c__111__,			/* O */

	c111111_,
	c1_____1,
	c1_____1,
	c1_____1,
	c111111_,
	c1______,
	c1______,
	c1______,
	c1______,			/* P */

	c__111__,
	c_1___1_,
	c1_____1,
	c1_____1,
	c1_____1,
	c1__1__1,
	c1___1_1,
	c_1___1_,
	c__111_1,			/* Q */

	c111111_,
	c1_____1,
	c1_____1,
	c1_____1,
	c111111_,
	c1__1___,
	c1___1__,
	c1____1_,
	c1_____1,			/* R */

	c_11111_,
	c1_____1,
	c1______,
	c1______,
	c_11111_,
	c______1,
	c______1,
	c1_____1,
	c_11111_,			/* S */

	c1111111,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,			/* T */

	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,
	c_11111_,			/* U */

	c1_____1,
	c1_____1,
	c1_____1,
	c_1___1_,
	c_1___1_,
	c__1_1__,
	c__1_1__,
	c___1___,
	c___1___,			/* V */

	c1_____1,
	c1_____1,
	c1_____1,
	c1_____1,
	c1__1__1,
	c1__1__1,
	c1_1_1_1,
	c11___11,
	c1_____1,			/* W */

	c1_____1,
	c1_____1,
	c_1___1_,
	c__1_1__,
	c___1___,
	c__1_1__,
	c_1___1_,
	c1_____1,
	c1_____1,			/* X */

	c1_____1,
	c1_____1,
	c_1___1_,
	c__1_1__,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,			/* Y */

	c1111111,
	c______1,
	c_____1_,
	c____1__,
	c___1___,
	c__1____,
	c_1_____,
	c1______,
	c1111111,			/* Z */

	c_1111__,
	c_1_____,
	c_1_____,
	c_1_____,
	c_1_____,
	c_1_____,
	c_1_____,
	c_1_____,
	c_1111__,			/* [ */

	c_______,
	c1______,
	c_1_____,
	c__1____,
	c___1___,
	c____1__,
	c_____1_,
	c______1,
	c_______,			/* \ */

	c__1111_,
	c_____1_,
	c_____1_,
	c_____1_,
	c_____1_,
	c_____1_,
	c_____1_,
	c_____1_,
	c__1111_,			/* ] */

	c___1___,
	c__1_1__,
	c_1___1_,
	c1_____1,
	c_______,
	c_______,
	c_______,
	c_______,			/* ^ */

	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,			/* _ */
	c1111111,

	c__11___,
	c__11___,
	c___1___,
	c____1__,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,			/* ` */

	c_______,
	c_______,
	c_______,
	c_1111__,
	c_____1_,
	c_11111_,
	c1_____1,
	c1____11,
	c_1111_1,			/* a */

	c1______,
	c1______,
	c1______,
	c1_111__,
	c11___1_,
	c1_____1,
	c1_____1,
	c11___1_,
	c1_111__,			/* b */

	c_______,
	c_______,
	c_______,
	c_1111__,
	c1____1_,
	c1______,
	c1______,
	c1____1_,
	c_1111__,			/* c */

	c_____1_,
	c_____1_,
	c_____1_,
	c_111_1_,
	c1___11_,
	c1____1_,
	c1____1_,
	c1___11_,
	c_111_1_,			/* d */

	c_______,
	c_______,
	c_______,
	c_1111__,
	c1____1_,
	c111111_,
	c1______,
	c1____1_,
	c_1111__,			/* e */

	c___11__,
	c__1__1_,
	c__1____,
	c__1____,
	c11111__,
	c__1____,
	c__1____,
	c__1____,
	c__1____,			/* f */

	c_111_1_,
	c1___11_,
	c1____1_,
	c1____1_,
	c1___11_,
	c_111_1_,
	c_____1_,
	c1____1_,
	c_1111__,			/* g */

	c1______,
	c1______,
	c1______,
	c1_111__,
	c11___1_,
	c1____1_,
	c1____1_,
	c1____1_,
	c1____1_,			/* h */

	c_______,
	c___1___,
	c_______,
	c__11___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c__111__,			/* i */

	c____11_,
	c_____1_,
	c_____1_,
	c_____1_,
	c_____1_,
	c_____1_,
	c_____1_,
	c_1___1_,
	c__111__,			/* j */

	c1______,
	c1______,
	c1______,
	c1___1__,
	c1__1___,
	c1_1____,
	c11_1___,
	c1___1__,
	c1____1_,			/* k */

	c__11___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c__111__,			/* l */

	c_______,
	c_______,
	c_______,
	c1_1_11_,
	c11_1__1,
	c1__1__1,
	c1__1__1,
	c1__1__1,
	c1__1__1,			/* m */

	c_______,
	c_______,
	c_______,
	c1_111__,
	c11___1_,
	c1____1_,
	c1____1_,
	c1____1_,
	c1____1_,			/* n */

	c_______,
	c_______,
	c_______,
	c_1111__,
	c1____1_,
	c1____1_,
	c1____1_,
	c1____1_,
	c_1111__,			/* o */

	c1_111__,
	c11___1_,
	c1____1_,
	c1____1_,
	c11___1_,
	c1_111__,
	c1______,
	c1______,
	c1______,			/* p */

	c_111_1_,
	c1___11_,
	c1____1_,
	c1____1_,
	c1___11_,
	c_111_1_,
	c_____1_,
	c_____1_,
	c_____1_,			/* q */

	c_______,
	c_______,
	c_______,
	c1_111__,
	c11___1_,
	c1______,
	c1______,
	c1______,
	c1______,			/* r */

	c_______,
	c_______,
	c_______,
	c_1111__,
	c1____1_,
	c_11____,
	c___11__,
	c1____1_,
	c_1111__,			/* s */

	c_______,
	c__1____,
	c__1____,
	c11111__,
	c__1____,
	c__1____,
	c__1____,
	c__1__1_,
	c___11__,			/* t */

	c_______,
	c_______,
	c_______,
	c1____1_,
	c1____1_,
	c1____1_,
	c1____1_,
	c1___11_,
	c_111_1_,			/* u */

	c_______,
	c_______,
	c_______,
	c1_____1,
	c1_____1,
	c1_____1,
	c_1___1_,
	c__1_1__,
	c___1___,			/* v */

	c_______,
	c_______,
	c_______,
	c1_____1,
	c1__1__1,
	c1__1__1,
	c1__1__1,
	c1__1__1,
	c_11_11_,			/* w */

	c_______,
	c_______,
	c_______,
	c1____1_,
	c_1__1__,
	c__11___,
	c__11___,
	c_1__1__,
	c1____1_,			/* x */

	c1____1_,
	c1____1_,
	c1____1_,
	c1____1_,
	c1___11_,
	c_111_1_,
	c_____1_,
	c1____1_,
	c_1111__,			/* y */

	c_______,
	c_______,
	c_______,
	c111111_,
	c____1__,
	c___1___,
	c__1____,
	c_1_____,
	c111111_,			/* z */

	c___11__,
	c__1____,
	c__1____,
	c__1____,
	c_1_____,
	c__1____,
	c__1____,
	c__1____,
	c___11__,			/* { */

	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,
	c___1___,			/* | */

	c__11___,
	c____1__,
	c____1__,
	c____1__,
	c_____1_,
	c____1__,
	c____1__,
	c____1__,
	c__11___,			/* } */

	c_11____,
	c1__1__1,
	c____11_,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,
	c_______,			/* ~ */

	c_1__1__,
	c1__1__1,
	c__1__1_,
	c_1__1__,
	c1__1__1,
	c__1__1_,
	c_1__1__,
	c1__1__1,
	c__1__1_,			/* rub-out */
};


/*** main
 *
 *
 *	main():
 *
 *	Invocation:
 *
 *		lpserver [ -c ]			:Print control chars literally
 *			 [ -f ]			:Printer understands \f
 *			 [ -i indent ]		:Indent n columns
 *			 [ -l page-length ]	:Define page length
 *			 [ -w page-width ]	:Define page width
 *			 printer-name		:Name of printer
 *
 *
 *	This process is exec'd by the shepherd process (child of the
 *	NQS daemon) with the following file descriptors open as described:
 *
 *	File descriptor 0: Control file (O_RDWR).
 *	File descriptor 1: The print device file (O_RDWR if possible, otherwise
 *			   O_RDONLY).
 *	File descriptor 2: Stderr writes to the NQS log process.
 *	File descriptor 3: Write file descriptor for serexit() back
 *			   to the shepherd process.
 *	File descriptors [4.._NFILE] are closed.
 *
 *	The current working directory of this process is the NQS
 *	root directory.
 *
 *	All signal actions are set to SIG_DFL.
 *
 *	The environment of the process contains the environment string:
 *
 *		DEBUG=nnn
 *
 *	where nnn specifies (in integer ASCII decimal), the debug
 *	level in effect when this process was forked from NQS.
 */
main (argc, argv)
int argc;
char *argv [];
{
	void banner();			/* Write banner */
	void donewline();		/* Write an interlocked newline */
	void doformfeed();		/* Write an interlocked formfeed */
	void nqs();			/* Write NQS flag page header/footer */
	void restore();			/* Restore device state function */

	register char *cp;		/* Pointer to character */
	register short max_overstrike;	/* Maximum number of overstrike lines */
					/* for the current line */
	register short i;		/* Iteration and index var */
	register short ch;		/* Print character */
	short v;			/* Index var */
	char output [MAX_OVERSTRIKE+1][MAX_PAGEWIDTH];
					/* Output buffer */
	short overstrike[MAX_PAGEWIDTH];/* Number of overstrike chars per col */
	short length [MAX_OVERSTRIKE+1];/* Length of given overstrike line */
#if	SGI | SYS52 | UTS | UNICOS | OSF
	struct termio tbuf;		/* Used for printer ioctl() */
#else
#if	BSD42 | BSD43 | ULTRIX
	struct sgttyb tbuf;		/* Used for printer ioctl() */
	int bsdmask;
#else
BAD SYSTEM TYPE
#endif
#endif
	struct rawreq rawreq;		/* Raw request control file header */
	time_t timeval;			/* Current time */
	struct passwd *pw_entry;	/* Password file entry */
	FILE *printfile;		/* Print file stream */
	char message [50];		/* Successful completion message buf */
	char localhost [256];		/* Name of local host */
	char *device;			/* Pointer to name of printer device */
	short indent;			/* Printer #of cols to indent */
	short pagewidth;		/* Printer page width */
	short ctrlchars;		/* BOOLEAN Control chars are to be */
					/* printed literally */
	short prfileno;			/* Print file number */
	int copy;			/* Counter for -n flag */
        /*
        * Intergraph definitions
        */
       char *tmpfile;
       char path [MAX_PATHNAME+1];     /* Path name for data file */
       int fildes[2], namedpipe;
       int forked, status;
       unsigned short baud, bits, parity, usenamedpipe;



	/*
	 *  Make sure that output to the NQS log process is fully
	 *  buffered, since many processes write to the NQS log
	 *  process.  This is NOT optional.  This is REQUIRED.
	 */
	bufstderr();
	/*
	 *  On some UNIX implementations, stdio functions alter errno to
	 *  ENOTTY when they first do something where the file descriptor
	 *  corresponding to the stream happens to be a pipe (which is
	 *  the case here).
	 *
	 *  The fflush() calls are added to get this behavior out of the
	 *  way, since bugs have occurred in the past when a server ran
	 *  into difficultly, and printed out an errno value in situations
	 *  where the diagnostic printf() displaying errno occurred AFTER
	 *  the first stdio function call invoked.
	 */
	fflush (stderr);		/* Stderr is a pipe */
	/*
	 *  Block buffer stdout, for maximum efficiency.
	 */
	bufstdout();			/* Block buffer to the device */
	lines = 0;			/* No lines written yet */
	pages = 0;			/* No pages written yet */
	interlock = 0;			/* No signal interlock at the */
					/* moment (see donewline(), */
					/* doformfeed(), and restore()). */
	queued_signal = 0;		/* No signal is presently queued */
					/* (see donewline(), doformfeed(), */
					/* and restore()). */
	if ((ptr = getenv("DEBUG")) == (char *) 0) {
		Debug = 0;
	}
	else
		Debug = atoi(ptr);
	/*
	 *  Read the entire request header for the batch request.
	 */
	time (&timeval);		/* Get current time */
	if (readreq (fileno (stdin), &rawreq) == -1) {
		if (errno != 0) serexit (RCM_BADCDTFIL, sys_errlist [errno]);
		serexit (RCM_BADCDTFIL, (char *) 0);
	}
	/*
	 *  Get name of local host.
	 */
	gethostname (localhost, 255);
	localhost [255] = '\0';		/* Null terminate */
	/*
	 *	Determine the account name for the request owner user-id.
	 */
	if ((pw_entry = fetchpwuid ((int) getuid())) == (struct passwd *) 0) {
		serexit (RCM_NOACCAUTH, (char *) 0);
						/* Need password entry to run */
	}
	closepwdb();				/* Close account/password */
						/* database */
	/*
	 *  Process any server arguments.
	 */
	ctrlchars = 0;				/* Translate ctrl chars to ' '*/
	formfeeds = 0;				/* We assume that the printer */
						/* device does NOT understand */
						/* formfeed characters */
               /* Intergraph added options */
       noreturn = 0;                           /* assume cr-lf for model */
       baud = B9600;                           /* assume 9600 baud */
       bits = CS7;                             /* assume 7 bits */
       parity = PARODD;                        /* assume odd parity */
       translate = 1;                          /* assume use of ioctl */
       usenamedpipe = 0;                       /* assume no named pipe */
       no_preprocess = 0;                      /* pre-process output/banner*/
 
	indent = 0;				/* Default is no indent */
	pagewidth = DEFAULT_PAGEWIDTH;		/* Default page width */
	pagelength = DEFAULT_PAGELENGTH;	/* Default page length */
	device = "Printer";			/* Default device name */
	while (*++argv != (char *) 0) {
		if (**argv != '-') {
			/*
			 *  We have a printer name.
			 */
			device = *argv;
		}
		else switch (*((*argv)+1)) {
               case '7':
                       bits = CS7;
                       break;
               case '8':
                       bits = CS8;
                       break;
               case 'b':
                       if (*++argv == (char *) 0) {
                               /*
                                *  Missing argument value.
                                */
                               fprintf(stderr,"E$lpserver: -b argument missing\n");
                               serexit (RCM_BADSRVARG,
                                        "Missing value for -b argument.");
                       }
                       switch (atoi(*argv)) {
                               case 50         : baud = B50; break;
                               case 75         : baud = B75; break;
                               case 110        : baud = B110; break;
                               case 134        : baud = B134; break;
                               case 150        : baud = B150; break;
                               case 200        : baud = B200; break;
                               case 300        : baud = B300; break;
                               case 600        : baud = B600; break;
                               case 1200       : baud = B1200; break;
                               case 1800       : baud = B1800; break;
                               case 2400       : baud = B2400; break;
                               case 4800       : baud = B4800; break;
                               case 9600       : baud = B9600; break;
                               case 19200      : baud = B19200; break;
                               default         :
                                       /*
                                        *  Missing argument value.
                                        */
                                       fprintf(stderr,"E$lpserver: -b out of range\n");
                                       serexit (RCM_BADSRVARG,
                                        "Out of range value for -b argument.");
                                       break;
                       }
                       break;
 
		case 'c':
			ctrlchars = 1;		/* Ctrl chars are to be */
			break;			/* printed literally */
		case 'e':
			parity = 0;
			break;
		case 'f':
			formfeeds = 1;		/* Print device understands */
			break;			/* form feed character */
		case 'i':
			if (*++argv == (char *) 0) {
				/*
				 *  Missing argument value.
				 */
                       		fprintf(stderr,"E$lpserver: -i argument missing\n");

				serexit (RCM_BADSRVARG,
					 "Missing value for -i argument.");
			}
			indent = atoi (*argv);
			if (indent < 0 ||
			    indent >= MIN_PAGEWIDTH) {
				/*
				 *  Catch out of range values.
				 */
                               fprintf(stderr,"E$lpserver: -i out of range\n");
				serexit (RCM_BADSRVARG,
					 "Out of range value for -i argument.");
			}
			break;
		case 'l':
			if (*++argv == (char *) 0) {
				/*
				 *  Missing argument value.
				 */
                               fprintf(stderr,"E$lpserver: -l argument missing\n");

				serexit (RCM_BADSRVARG,
					 "Missing value for -l argument.");
			}
			pagelength = atoi (*argv);
			if (pagelength < MIN_PAGELENGTH ||
			    pagelength > MAX_PAGELENGTH) {
				/*
				 *  Catch out of range values.
				 */
                               fprintf(stderr,"E$lpserver: -l out of range\n"); 
				serexit (RCM_BADSRVARG,
					 "Out of range value for -l argument.");
			}
			break;
               case 'm':       /* Intergraph added option */
                       if (*++argv == (char *) 0) {
                               fprintf(stderr,"E$lpserver: -m argument missing\n");
                               serexit(RCM_BADSRVARG,
                                       "Missing value for -m argument.");
                       }
                       model_interface = *argv;
                       break;
               case 'n':
                       no_preprocess = 1;
                       break;
               case 'o':
                       parity = PARODD;
                       break;
               case 'p':
                       usenamedpipe = 1;
                       break;
               case 'r':       /* Intergraph added option */
                       noreturn = 1;

		case 'w':
			if (*++argv == (char *) 0) {
				/*
				 *  Missing argument value.
				 */
                               fprintf(stderr,"E$lpserver: -l out of range\n"); 
				serexit (RCM_BADSRVARG,
					 "Missing value for -w argument.");
			}
			pagewidth = atoi (*argv);
			if (pagewidth < MIN_PAGEWIDTH ||
			    pagewidth > MAX_PAGEWIDTH) {
				/*
				 *  Catch out of range values.
				 */
				serexit (RCM_BADSRVARG,
					 "Out of range value for -w argument.");
			}
			break;
               case 'x':
                       translate=0;
                       break;
 
		default:
                               fprintf(stderr,"E$lpserver: invalid argument\n");

			serexit (RCM_BADSRVARG,
				 "Invalid argument to lpserver.");
			break;
		}
	}
	/*
	 *  Get/save original device state.
	 */
#if	SGI | SYS52 | UTS | UNICOS | OSF
if (translate)
	ioctl (fileno (stdout), TCGETA, &save);
#else
#if	BSD42 | BSD43 | ULTRIX
	ioctl (fileno (stdout), TIOCGETP , &save);
#else
BAD SYSTEM TYPE
#endif
#endif

       if (no_preprocess) {    /* generate pathname of file to use */
               prfileno = 1;
               pack6name(path, Nqs_data,
                       (int) (rawreq.orig_seqno % MAX_OUTPSUBDIRS),
                       (char *) 0, (long) rawreq.orig_seqno, 5,
                       (long) rawreq.orig_mid, 6, prfileno-1, 3);
       }
       /*
        * Intergraph added
        * if an interface is being used, pipe and fork the interface
        */
       if (model_interface) {
               if (usenamedpipe) {
                       tmpfile=tempnam("/usr/tmp","NQS");
                       if ( mknod(tmpfile,0010666) == -1)
                               serexit(RCM_ENOSPCRUN, (char *) 0);
                       if (Debug>2)
                               fprintf(stderr,"D$lpserver: tmpfile created\n");               }
               else
                       if (pipe(fildes) == -1)
                               serexit(RCM_ENFILERUN, (char *) 0);
               if ((forked = fork()) == 0) {
                       close(0);
                       close(3);
                       if (usenamedpipe | no_preprocess) {
                               if (Debug>2) {
                                       fprintf(stderr,"D$lpserver: Pipe exec'ed\n");
                                       fflush(stderr);
                               }
                               if (no_preprocess)
                                       execlp("/bin/ksh","-ksh",
                                               model_interface,path,0);
                               else
                                       execlp(model_interface,
                                               model_interface,tmpfile,0);
                               serexit(RCM_SEREXEFAI, (char *) 0);
                       }
                       else {
                               fcntl(fildes[0],F_DUPFD,0);
                               close(fildes[0]);
                               close(fildes[1]);
                               execlp(model_interface,model_interface,0);
                       }
                       serexit(RCM_SEREXEFAI, (char *) 0);
               }
               else if (forked == -1)
                       serexit(RCM_INSUFFMEM, (char *) 0);
               if (usenamedpipe && !no_preprocess) {
                       if ((namedpipe = open (tmpfile, O_WRONLY)) == -1)
                               serexit(RCM_ENFILERUN, (char *) 0);
                       close(1);
                       fcntl(namedpipe,F_DUPFD,1);
                       close(namedpipe);
               }
               else {
                       close(1);
                       fcntl(fildes[1],F_DUPFD,1);
                       close(fildes[1]);
                       close(fildes[0]);
               }
       }


	/*
	 *  Set up proper signal handling to restore device state, and wait
	 *  for output to drain upon receipt of a terminating signal.
	 */
 if (!no_preprocess) {

	signal (SIGHUP, restore);
	signal (SIGINT, restore);
	signal (SIGQUIT, restore);
	signal (SIGILL, restore);
	signal (SIGTRAP, restore);
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	signal (SIGIOT, restore);
	signal (SIGEMT, restore);
#else
#if	UNICOS
	signal (SIGHWE, restore);
#else
BAD SYSTEM TYPE
#endif
#endif
	signal (SIGFPE, restore);
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	signal (SIGBUS, restore);
	signal (SIGSEGV, restore);
#else
#if	UNICOS
	signal (SIGPRE, restore);
	signal (SIGORE, restore);
#else
BAD SYSTEM TYPE
#endif
#endif
	signal (SIGSYS, restore);
	signal (SIGPIPE, restore);
	signal (SIGALRM, restore);
	signal (SIGTERM, restore);
#if	SGI | SYS52 | UTS | UNICOS | OSF
	signal (SIGUSR1, restore);
	signal (SIGUSR2, restore);
#else
#if	BSD42 | BSD43 | ULTRIX
	signal (SIGURG, restore);
	signal (SIGSTOP, restore);
	signal (SIGTSTP, restore);
	signal (SIGCONT, restore);
	signal (SIGCHLD, restore);
	signal (SIGTTIN, restore);
	signal (SIGTTOU, restore);
	signal (SIGIO, restore);
	signal (SIGXCPU, restore);
	signal (SIGXFSZ, restore);
	signal (SIGVTALRM, restore);
	signal (SIGPROF, restore);
#else
BAD SYSTEM TYPE
#endif
#endif
 }	/* no_preprocess */
	/*
	 *  Set new device state.
	 */
	tbuf = save;
#if	SGI | SYS52 | UTS | UNICOS | OSF
	tbuf.c_iflag = IGNPAR | ISTRIP | BRKINT | IXON;
	tbuf.c_oflag = OPOST | ONLCR | TAB3 | FF1;
	tbuf.c_cflag = baud | bits | CREAD | PARENB;
	tbuf.c_lflag = ISIG;
if (translate)
	ioctl (fileno (stdout), TCSETA, &tbuf);
#else
#if	BSD42 | BSD43 | ULTRIX
	/*
	 * The following code needs work.
	 */
	ioctl (fileno (stdout), TIOCEXCL, (char *) 0);
	tbuf.sg_flags = CRMOD | CBREAK | XTABS | FF1;
	tbuf.sg_ispeed = B9600;
	tbuf.sg_ospeed = B9600;
	ioctl (fileno (stdout), TIOCSETP, &tbuf);
	bsdmask = 0;
	ioctl (fileno (stdout), TIOCLBIC, &bsdmask);
	bsdmask = LDECCTQ;
	ioctl (fileno (stdout), TIOCLBIS, &bsdmask);
#else
BAD SYSTEM TYPE
#endif
#endif

if (!no_preprocess) {
	/*
	 *  Print the flag page.
	 */
	donewline();				/* Interlocked newline */
	nqs (pagewidth);			/* Write header */
	for (i=1; i <= 2; i++) {
		donewline();			/* Interlocked newline */
	}
	banner (rawreq.reqname, pagewidth);	/* Show reqname */
	banner (rawreq.username, pagewidth);	/* Show real owner */
	/*
	 *  Skip appropriate number of lines based on page length.
	 */
	i = pagelength - 13 - lines;
	while (i--) {
		donewline();			/* Interlocked newline */
	}
	printf ("Local owner: %s", pw_entry->pw_name);
	if (pw_entry->pw_comment [0] != '\0') {
		printf ("[%s]", pw_entry->pw_comment);
	}
	donewline();				/* Interlocked newline */
	printf ("Printed at: %s", fmttime (&timeval));
	donewline();				/* Interlocked newline */
	printf ("Submitted at: %s", fmttime (&rawreq.create_time));
	donewline();				/* Interlocked newline */
	printf ("Request-id: %1d.%s", rawreq.orig_seqno,
		fmtmidname (rawreq.orig_mid));
	donewline();				/* Interlocked newline */
	printf ("Host: %s", localhost);
	donewline();				/* Interlocked newline */
	printf ("Device: %s", device);		/* NQS name for device */
	donewline();				/* Interlocked newline */
	for (i=1; i <= 2; i++) {
		donewline();			/* Interlocked newline */
	}
	nqs (pagewidth);			/* Write footer */
	doformfeed();				/* Go to top of new page */
	/*
	 *  Prepare to print the file(s).
	 */
	max_overstrike = MAX_OVERSTRIKE+1;
	while (max_overstrike--) {		/* Clear ALL overstrike lines */
		cp = &output [max_overstrike][0];
		i = MAX_PAGEWIDTH;
		while (i--) *cp++ = ' ';
		length [max_overstrike] = 0;	/* Nothing in this line */
	}
	/*
	 *  Loop to print copies according to the -n flag
	 */
	for (copy = 1; copy <= rawreq.v.dev.copies; copy++ ) {
	    /*
	     *  Loop to print all files associated with this print-request.
	     */
	    for (prfileno = 1; prfileno <= rawreq.ndatafiles; prfileno++) {
		/*
		 *  Open the next print file for the print-request.
		 */
		if ((printfile = fopendata (rawreq.orig_seqno, rawreq.orig_mid,
					    prfileno)) == (FILE *) 0) {
			serexit (RCM_BADCDTFIL,
				 "Unable to open print file.");
		}
		/*
		 *  Now print the file.
		 */
		ch = getc (printfile);		/* Get first character of */
						/* the print file */
		/*
		 *  While not EOF do....
		 */
		while (ch != EOF) {
			i = MAX_PAGEWIDTH;
			while (i--) {
				/*
				 *  No output or overstrikes in any column
				 *  just yet.
				 */
				overstrike [i] = 0;
			}
			max_overstrike = 0;	/* No output yet */
			i = indent;		/* Current column */
			while (ch != '\n' && ch != '\f' && ch != EOF) {
				if (ch == '\b') {
					if (--i < indent) i = indent;
				}	
				else if (ch == '\r') i = indent;
				else if (ch == '\t') {
					i = ((i-indent) | 07) + indent + 1;
				}
				else if (ch == ' ' || i >= pagewidth ||
					 !ctrlchars && ch < ' ') {
					i++;	/* One more column, but */
				}		/* no output */
				else {
					/*
					 *  The character is printable, and
					 *  within the page width of the
					 *  printer.
					 */
					v = overstrike [i];
					if (v < MAX_OVERSTRIKE+1) {
						/*
						 *  There is sufficient
						 *  overstrike space for the
						 *  character in the current
						 *  column.
						 */
						output [v][i] = ch;
						if (++overstrike [i]
							> max_overstrike) {
							max_overstrike++;
						}
						if (++i > length [v]) {
							length [v] = i;
						}
					}
					else i++;	/* One more column */
				}
				ch = getc (printfile);	/* Get next print */
			}				/* file char */
			/*
			 *  Print-out this line of the output file with the
			 *  computed overstrikes.
			 */
			while (max_overstrike-- > 0) {
				cp = &output [max_overstrike][0];
				i = length [max_overstrike];
				/*
				 *  Reset overstrike line length for the next
				 *  line of the print file.
				 */
				length [max_overstrike] = 0;
				/*
				 *  Print-out this overstrike line.
				 */
				while (i--) {
					putchar (*cp);
					*cp++ = ' ';	/* Replace with space*/
				}
				if (!noreturn)
				putchar ('\r');	/* Carriage return for next */
						/* overstrike */
			}
			if (ch != '\f') {
				donewline();	/* Interlocked newline */
			}
			if (ch != EOF) {
				if (ch == '\f') doformfeed(); /* interlocked */
				ch = getc (printfile);	/* Get next */
			}				/* printfile char */
		}
		if (lines) {
			/*
			 *  Finish on a page boundary.
			 */
			doformfeed();	/* Interlocked formfeed */
		}
		fclose (printfile);	/* Close the current print file */
	    				/* in preparation for the next one */
	    }				/* end of inner for */
	}				/* end of outer for */
       if (usenamedpipe) {
               if (Debug>2)
                       fprintf(stderr,"D$lpserver: unlink tmpfile\n");
               if (unlink(tmpfile) < 0) {
                       fprintf(stderr,"E$lpserver: '%s' not unlinked\n",tmpfile);
               }
       }
 
	fclose (stdout);		/* Flush output and close device */
	fclose (stderr);		/* Flush any stderr output and close */
 }	/* end of preprocess */
	/*
	 *  Return printer to original state (waiting for output to drain).
	 */
#if	SGI | SYS52 | UTS | UNICOS | OSF
if (translate)
	ioctl (fileno (stdout), TCSETAW, &save);
#else
#if	BSD42 | BSD43 | ULTRIX
	ioctl (fileno (stdout), TIOCSETP , &save);
#else
BAD SYSTEM TYPE
#endif
#endif
       errno=0;
       if (forked)
               while((ret=wait (&status)) == -1 &&
                       (errno == EINTR || errno == ENOTTY));
 
	/*
	 *  Format message with number of pages printed and exit.
	 */
       if (status != 0)
               if ((status & 0xFF) == 0)
                       sprintf(message, "Interface: _exit value was: %1d.",
                               (status >> 8) & 0xFF);
               else if ((status & 0xFF00) == 0)
                       sprintf(message,
                               "Interface: Aborting signal was %1d.",
                               status & 0x7F);
       else if (no_preprocess)
               sprintf (message, "1 job completed.");
       else
	sprintf (message, "%1d pages printed.", pages);
	serexit (RCM_EXITED, message);
}


/*** nqs
 *
 *
 *	void nqs():
 *	Display flag page header/footer to stdout.
 */
static void nqs (width)
int width;				/* Width of line printer */
{
	static char text[] = "Network Queueing System*";

	register char *cp;
	register int i;
	register int j;

	i = 4;
	while (i--) {
		j = 0;
		cp = text;
		while (j++ < width) {
			if (*cp == '\0') cp = text;
			putchar (*cp++);
		}
		donewline();		/* Interlocked newline */
	}
}


/*** banner
 *
 *
 *	void banner():
 *
 *	Display the specified character string as a horizontal banner
 *	to stdout.
 */
static void banner (string, width)
char *string;				/* String to be printed */
int width;				/* Width of line printer */
{
	register short i;		/* Counter */
	register short column;		/* Column [0..n] */
	register short ch;		/* Banner character */
	register short bits;		/* Bitmap */
	register short j;		/* Counter */
	register char *cp;		/* Used to walk banner string */

	/*
	 *  Skip lines above banner.
	 */
	i = CABOVE;
	while (i--) {
		donewline();			/* Interlocked newline */
	}
	/*
	 *  Display the string in banner form.
	 */
	for (i=0; i < CHEIGHT + CDEPTH; i++) {
		/*
		 *  Loop to generate the i-th line of the character string
		 *  in the form of a banner.
		 */
		column = 0;
		cp = string;
		while (ch = *cp++) {
			if ((column += CBEFORE + CWIDTH + CAFTER) >= width) {
				/*
				 *  We must truncate the banner string.
				 *  Not all of the characters of the banner
				 *  will fit on the line.
				 */
				break;
			}
			ch %= 128;		/* Force ch in [0..127] */
			if (ch < ' ') ch = ' ';	/* Non-printable -> space */
			/*
			 *  Display the proper number of spaces before the
			 *  current banner character.
			 */
			j = CBEFORE;
			while (j--) {
				putchar (' ');
			}
			/*
			 *  The characters: 'g', 'j', 'p', 'q',
			 *  'y', ',', and ';' are special because
			 *  they have descenders....
			 */
			switch (ch) {
			case 'g':
			case 'j':
			case 'p':
			case 'q':
			case 'y':
			case ',':
			case ';':
				if (i < CDEPTH) bits = 0;
				else bits = chset [ch-' '][i - CDEPTH];
				break;
			default:
				/* All other characters */
				if (i >= CHEIGHT) bits = 0;
				else bits = chset [ch-' '][i];
				break;
			}
			/*
			 *  Display the proper "scan line" of the current
			 *  character.
			 */
			j = CWIDTH;
			while (j--) {
				if ((bits <<= 1) & (1 << CWIDTH)) {
					putchar (BANNER_CHAR);
				}
				else putchar (' ');
			}
			/*
			 *  Display the proper number of spaces after the
			 *  current banner character.
			 */
			j = CAFTER;
			while (j--) {
				putchar (' ');
			}
		}
		donewline();			/* Interlocked newline */
	}
	/*
	 *  Skip lines below banner.
	 */
	i = CBELOW;
	while (i--) {
		donewline();			/* Interlocked newline */
	}
}


/*** donewline
 *
 *
 *	void donewline():
 *
 *	Write a newline '\n' to the line printer device being extremely
 *	careful with signal handling so that the number of lines and
 *	pages written to the device are properly updated, even when a
 *	signal arrives asynchronously at an inconvenient time.
 */
static void donewline()
{
	void restore();			/* Signal catcher, and device state */
					/* restore function. */

	interlock++;			/* Set for restore() signal handler */
	 if (!noreturn)                  /* Intergraph added option */
               putchar('\r');          /* assume model printer wants cr-lf */
 
	putchar ('\n');
	lines++;			/* Another line */
	if (lines == pagelength) {
		pages++;		/* Another complete page */
		lines = 0;		/* Starting at top of page */
	}
	interlock--;			/* Release signal interlock. */
	if (queued_signal) restore (signal_received);
}


/*** doformfeed
 *
 *
 *	void doformfeed():
 *
 *	Write a formfeed '\f' to the line printer device being extremely
 *	careful with signal handling so that the number of lines and
 *	pages written to the device are properly updated, even when a
 *	signal arrives asynchronously at an inopportune time.
 */
static void doformfeed()
{
	void restore();			/* Signal catcher, and device state */
					/* restore function. */

	interlock++;			/* Set for restore() signal handler */
	if (formfeeds) putchar ('\f');
	else {
		while (lines < pagelength) {
			putchar ('\n');
			lines++;	/* Another complete line */
		}
	}
	lines = 0;			/* We are now at the top of the page */
	pages++;			/* Another complete page */
	interlock--;			/* Release signal interlock */
	if (queued_signal) restore (signal_received);
}


/*** restore
 *
 *
 *	void restore():
 *	Restore original device state, waiting for output to drain.
 */
static void restore (sig)
int sig;				/* Signal received */
{
	if (queued_signal++) {
		/*
		 *  A signal has already been queued for delivery.
		 */
		return;
	}
	if (interlock) {
		/*
		 *  This server is trying to output a newline or formfeed
		 *  character and must not be interrupted.
		 */
		signal_received = sig;	/* Remember the signal that we */
					/* received */
		return;			/* We will be invoked by the */
					/* interrupted procedure when */
					/* updates are complete. */
	}
	if (!no_preprocess) {
		donewline();			/* Write an interlocked newline */
		printf ("Print req aborted by signal %1d.", sig);
		donewline();			/* Write an interlocked newline */
		if (lines && lines != pagelength) {
			doformfeed();		/* End on a page boundary */
		}				/* (interlocked) */
	}
	fclose (stdout);		/* Flush output and close device */
	fclose (stderr);		/* Flush any stderr output and close */
	/*
	 *  Return printer to original state (waiting for output to drain).
	 */
#if	SGI | SYS52 | UTS | UNICOS | OSF
if (translate)
	ioctl (fileno (stdout), TCSETAW, &save);
#else
#if	BSD42 | BSD43 | ULTRIX
	ioctl (fileno (stdout), TIOCSETP , &save);
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  Now that we've put things right with the device, commit
	 *  seppuku with the original signal so that the server shepherd
	 *  process discerns the correct cause of our demise.
	 */
	signal (sig, SIG_DFL);
	kill (getpid(), sig);
}
