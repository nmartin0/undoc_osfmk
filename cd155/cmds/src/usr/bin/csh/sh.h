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
/***
static char rcsid[] = "@(#)$RCSfile: sh.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:46 $";
***/
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS:
 *
 * ORIGINS: 10,26,27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * (Copyright statements and/or associated legends of other
 * companies whose code appears in any part of this module must
 * be copied here.)
 */
/*
 * C shell
 *
 * Bill Joy, UC Berkeley
 * October, 1978; May 1980
 *
 * Jim Kulp, IIASA, Laxenburg Austria
 * April, 1980
 */

#ifdef KJI
#include <NLchar.h>
#endif

#ifdef	_BSD
#include <sgtty.h>
#include <termios.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <dirent.h>
#include <sys/param.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/times.h>
#include "local.h"
#include "port.h"


#include "csh_msg.h"
extern nl_catd catd;
extern int FSHMSG;
#define MSGSTR(num,str) catgets(catd,MS_CSH,num,str)   

#define	isdir(d)	((d.st_mode & S_IFMT) == S_IFDIR)


typedef	char	bool;

#define	eq(a, b)	(strcmp(a, b) == 0)

#ifndef NULL
#define NULL	(void *)0
#endif

/*
 * Global flags
 */

/* added from CMU csh for command-line expansion & editing */
#ifdef CMDEDIT
#ifndef FILEC
#define FILEC
#endif
bool    cmdedit;                /* Use cmu_tenex routine for input */
#endif

bool	chkstop;		/* Warned of stopped jobs... allow exit */
bool	didcch;			/* Have closed unused fd's for child */
bool	didfds;			/* Have setup i/o fd's for child */
bool	doneinp;		/* EOF indicator after reset from readc */
bool	exiterr;		/* Exit if error or non-zero exit status */
bool	child;			/* Child shell ... errors cause exit */
bool	haderr;			/* Reset was because of an error */
bool	intty;			/* Input is a tty */
bool	intact;			/* We are interactive... therefore prompt */
bool	justpr;			/* Just print because of :p hist mod */
bool	loginsh;		/* We are a loginsh -> .login/.logout */
bool	neednote;		/* Need to pnotify() */
bool	noexec;			/* Don't execute, just syntax check */
bool	pjobs;			/* want to print jobs if interrupted */
bool	setintr;		/* Set interrupts on/off -> Wait intr... */
bool	timflg;			/* Time the next waited for command */
#ifdef FILEC
bool  filec;                /* doing filename expansion */
#endif

/*
 * Global i/o info
 */
uchar_t	*arginp;		/* Argument input for sh -c and internal `xx` */
int	onelflg;		/* 2 -> need line for -t, 1 -> exit on read */
uchar_t	*file;			/* Name of shell file for $0 */

uchar_t	*err;			/* Error message from scanner/parser */
extern int	errno;		/* Error from C library routines */
uchar_t	*shtemp;		/* Temp name for << shell files in /tmp */
struct timeval	time0;		/* Time at which the shell started */

#undef putchar
#define setpgrp(g,p)	setpgid(g,p)
struct rusage ru0;

/*
 * Miscellany
 */
uchar_t	*doldol;		/* Character pid for $$ */
uid_t	uid;			/* Invokers uid */
time_t	chktim;			/* Time mail last checked */
pid_t	shpgrp;			/* Pgrp of shell */
pid_t	tpgrp;			/* Terminal process group */
/* If tpgrp is -1, leave tty alone! */
pid_t	opgrp;			/* Initial pgrp and tty pgrp */
int     oldisc;                 /* Initial line discipline or -1 */
struct	tms shtimes;		/* shell and child times for process timing */
#ifdef CMDEDIT
uchar_t    *ShellTypeAheadToTenex; /* Read-ahead buffer for cmu_tenex () */
#endif


/*
 * These are declared here because they want to be
 * initialized in sh.init.c (to allow them to be made readonly)
 */

extern struct   mesg {
        uchar_t    *iname;         /* name from /usr/include */
        uchar_t    *pname;         /* print name */
} mesg[];

struct	biltins {
	uchar_t	*bname;
	int	(*bfunct)();
	short	minargs, maxargs;
};
extern struct biltins bfunc[];

#define	INF	1000

struct srch {
	uchar_t	*s_name;
	short	s_value;
};
extern struct srch srchn[];

/*
 * To be able to redirect i/o for builtins easily, the shell moves the i/o
 * descriptors it uses away from 0,1,2.
 * Ideally these should be in units which are closed across exec's
 * (this saves work) but for version 6, this is not usually possible.
 * The desired initial values for these descriptors are defined in
 * sh.local.h.
 */
short	SHIN;			/* Current shell input (script) */
short	SHOUT;			/* Shell output */
short	SHDIAG;			/* Diagnostic output... shell errs go here */
short	OLDSTD;			/* Old standard input (def for cmds) */

/*
 * Error control
 *
 * Errors in scanning and parsing set up an error message to be printed
 * at the end and complete.  Other errors always cause a reset.
 * Because of source commands and .cshrc we need nested error catches.
 */

jmp_buf	reslab;

#define	setexit()	setjmp(reslab)
#define	reset()		longjmp(reslab, 0)
	/* Should use structure assignment here */
#define	getexit(a)	copy((char *)(a), (char *)reslab, sizeof reslab)
#define	resexit(a)	copy((char *)reslab, ((char *)(a)), sizeof reslab)

uchar_t	*gointr;		/* Label for an onintr transfer */
void	(*parintr)(int);		/* Parents interrupt catch */
void	(*parterm)(int);		/* Parents terminate catch */

/*
 * Lexical definitions.
 *
 * All lexical space is allocated dynamically.
 * The eighth bit of characters is used to prevent recognition,
 * and eventually stripped.
 */
#define	QUOTE 	0200		/* Eighth char bit used internally for 'ing */
#define	TRIM	0177		/* Mask to strip quote bit */

/*
 * Each level of input has a buffered input structure.
 * There are one or more blocks of buffered input for each level,
 * exactly one if the input is seekable and tell is available.
 * In other cases, the shell buffers enough blocks to keep all loops
 * in the buffer.
 */
struct	Bin {
	off_t	Bfseekp;		/* Seek pointer */
	off_t	Bfbobp;			/* Seekp of beginning of buffers */
	off_t	Bfeobp;			/* Seekp of end of buffers */
	short	Bfblocks;		/* Number of buffer blocks */
	uchar_t	**Bfbuf;		/* The array of buffer blocks */
} B;

#define	fseekp	B.Bfseekp
#define	fbobp	B.Bfbobp
#define	feobp	B.Bfeobp
#define	fblocks	B.Bfblocks
#define	fbuf	B.Bfbuf

off_t	btell();

/*
 * The shell finds commands in loops by reseeking the input
 * For whiles, in particular, it reseeks to the beginning of the
 * line the while was on; hence the while placement restrictions.
 */
off_t	lineloc;

#ifdef	TELL
off_t	tell();
bool	cantell;			/* Is current source tellable ? */
#endif

/*
 * Input lines are parsed into doubly linked circular
 * lists of words of the following form.
 */
struct	wordent {
	uchar_t	*word;
	struct	wordent *prev;
	struct	wordent *next;
};

/*
 * During word building, both in the initial lexical phase and
 * when expanding $ variable substitutions, expansion by `!' and `$'
 * must be inhibited when reading ahead in routines which are themselves
 * processing `!' and `$' expansion or after characters such as `\' or in
 * quotations.  The following flags are passed to the getC routines
 * telling them which of these substitutions are appropriate for the
 * next character to be returned.
 */
#define	DODOL	1
#define	DOEXCL	2
#define	DOALL	DODOL|DOEXCL

/*
 * Labuf implements a general buffer for lookahead during lexical operations.
 * Text which is to be placed in the input stream can be stuck here.
 * We stick parsed ahead $ constructs during initial input,
 * process id's from `$$', and modified variable values (from qualifiers
 * during expansion in sh.dol.c) here.
 */
uchar_t	labuf[BUFR_SIZ];

uchar_t	*lap;

/*
 * Parser structure
 *
 * Each command is parsed to a tree of command structures and
 * flags are set bottom up during this process, to be propagated down
 * as needed during the semantics/exeuction pass (sh.sem.c).
 */
struct	command {
	short	t_dtyp;				/* Type of node */
	short	t_dflg;				/* Flags, e.g. FAND|... */
	union {
		uchar_t	*T_dlef;		/* Input redirect word */
		struct	command *T_dcar;	/* Left part of list/pipe */
	} L;
	union {
		uchar_t	*T_drit;		/* Output redirect word */
		struct	command *T_dcdr;	/* Right part of list/pipe */
	} R;
#define	t_dlef	L.T_dlef
#define	t_dcar	L.T_dcar
#define	t_drit	R.T_drit
#define	t_dcdr	R.T_dcdr
	uchar_t	**t_dcom;			/* Command/argument vector */
	struct	command *t_dspr;		/* Pointer to ()'d subtree */
	short	t_nice;
};

#define	TCOM	1		/* t_dcom <t_dlef >t_drit	*/
#define	TPAR	2		/* ( t_dspr ) <t_dlef >t_drit	*/
#define	TFIL	3		/* t_dlef | t_drit		*/
#define	TLST	4		/* t_dlef ; t_drit		*/
#define	TOR	5		/* t_dlef || t_drit		*/
#define	TAND	6		/* t_dlef && t_drit		*/

#define	FSAVE	(FNICE|FTIME|FNOHUP)	/* save these when re-doing */

#define	FAND	(1<<0)		/* executes in background	*/
#define	FCAT	(1<<1)		/* output is redirected >>	*/
#define	FPIN	(1<<2)		/* input is a pipe		*/
#define	FPOU	(1<<3)		/* output is a pipe		*/
#define	FPAR	(1<<4)		/* don't fork, last ()ized cmd	*/
#define	FINT	(1<<5)		/* should be immune from intr's */
/* spare */
#define	FDIAG	(1<<7)		/* redirect unit 2 with unit 1	*/
#define	FANY	(1<<8)		/* output was !			*/
#define	FHERE	(1<<9)		/* input redirection is <<	*/
#define	FREDO	(1<<10)		/* reexec aft if, repeat,...	*/
#define	FNICE	(1<<11)		/* t_nice is meaningful */
#define	FNOHUP	(1<<12)		/* nohup this command */
#define	FTIME	(1<<13)		/* time this command */

/*
 * The keywords for the parser
 */
#define	ZBREAK		0
#define	ZBRKSW		1
#define	ZCASE		2
#define	ZDEFAULT 	3
#define	ZELSE		4
#define	ZEND		5
#define	ZENDIF		6
#define	ZENDSW		7
#define	ZEXIT		8
#define	ZFOREACH	9
#define	ZGOTO		10
#define	ZIF		11
#define	ZLABEL		12
#define	ZLET		13
#define	ZSET		14
#define	ZSWITCH		15
#define	ZTEST		16
#define	ZTHEN		17
#define	ZWHILE		18

/*
 * Structure defining the existing while/foreach loops at this
 * source level.  Loops are implemented by seeking back in the
 * input.  For foreach (fe), the word list is attached here.
 */
struct	whyle {
	off_t	w_start;		/* Point to restart loop */
	off_t	w_end;			/* End of loop (0 if unknown) */
	uchar_t	**w_fe, **w_fe0;	/* Current/initial wordlist for fe */
	uchar_t	*w_fename;		/* Name for fe */
	struct	whyle *w_next;		/* Next (more outer) loop */
} *whyles;

/*
 * Variable structure
 *
 * Lists of aliases and variables are sorted alphabetically by name
 */
struct	varent {
	uchar_t	**vec;		/* Array of words which is the value */
	uchar_t	*name;		/* Name of variable/alias */
	struct	varent *link;
} shvhed, aliases;

/*
 * The following are for interfacing redo substitution in
 * aliases to the lexical routines.
 */
struct	wordent *alhistp;		/* Argument list (first) */
struct	wordent *alhistt;		/* Node after last in arg list */
uchar_t	**alvec;			/* The (remnants of) alias vector */

/*
 * Filename/command name expansion variables
 */
short	gflag;				/* After tglob -> is globbing needed? */

/*
 * A reasonable limit on number of arguments would seem to be
 * the maximum number of characters in an arg list / 6.
 */
#define	GAVSIZ	NCARGS / 6

/*
 * Variables for filename expansion
 */
uchar_t	**gargv;			/* Pointer to the (stack) arglist */
short	gargc;				/* Number args in gargv */
short	gnleft;

/*
 * Variables for command expansion.
 */
uchar_t	**pargv;			/* Pointer to the argv list space */
uchar_t	*pargs;				/* Pointer to start current word */
short	pargc;				/* Count of arguments in pargv */
short	pnleft;				/* Number of chars left in pargs */
uchar_t	*pargcp;			/* Current index into pargs */

/*
 * Max size for env variable 
 */
#ifdef KJI
#define MAX_ENV_NAMLEN	80
#else
#define MAX_ENV_NAMLEN	40
#endif

/* Max login name for ~foo use */
#define MAX_LOG_NAMLEN	MAX_ENV_NAMLEN

/*
 * History list
 *
 * Each history list entry contains an embedded wordlist
 * from the scanner, a number for the event, and a reference count
 * to aid in discarding old entries.
 *
 * Essentially "invisible" entries are put on the history list
 * when history substitution includes modifiers, and thrown away
 * at the next discarding since their event numbers are very negative.
 */
struct	Hist {
	struct	wordent Hlex;
	int	Hnum;
	int	Href;
	struct	Hist *Hnext;
} Histlist;

struct	wordent	paraml;			/* Current lexical word list */
int	eventno;			/* Next events number */
int	lastev;				/* Last event reference (default) */

#ifdef KJI
extern  int    HIST;                   /* history invocation character */
extern  int    HISTSUB;                /* auto-substitute character */
#else
extern  uchar_t    HIST;                   /* history invocation character */
extern  uchar_t    HISTSUB;                /* auto-substitute character */
#endif

uchar_t	*Dfix1();
struct	varent *adrof(), *adrof1();
uchar_t	**blkcat();
uchar_t	**blkcpy();
uchar_t	**blkend();
uchar_t	**blkspl();
uchar_t	*calloc();
uchar_t	*cname();
uchar_t	**copyblk();
uchar_t	**dobackp();
uchar_t	*domod();
struct	wordent *dosub();
uchar_t	*exp3();
uchar_t	*exp3a();
uchar_t	*exp4();
uchar_t	*exp5();
uchar_t	*exp6();
struct	Hist *enthist();
struct	Hist *findev();
struct	wordent *freenod();
uchar_t	*getenv();
uchar_t	*getinx();
struct	varent *getvx();
struct	passwd *getpwnam();
struct	wordent *gethent();
struct	wordent *getsub();
uchar_t	*getwd();
uchar_t	*globone();
struct	biltins *isbfunc();
uchar_t	**glob();
uchar_t	*operate();
void	pintr();
void	pchild();
uchar_t	*putn();
uchar_t	**saveblk();
uchar_t	*savestr();
char	*strcat();
char	*strcpy();
uchar_t	*strend();
uchar_t	*strings();
uchar_t	*strip();
uchar_t	*strspl();
uchar_t	*subword();
struct	command *syntax();
struct	command *syn0();
struct	command *syn1();
struct	command *syn1a();
struct	command *syn1b();
struct	command *syn2();
struct	command *syn3();
unsigned int	tglob();
int	trim();
uchar_t	*value(), *value1();
uchar_t	*xhome();
uchar_t	*xname();
uchar_t	*xset();

#define	NOSTR	((uchar_t *) 0)

/*
 * setname is a macro to save space (see sh.err.c)
 */
uchar_t	*bname;
#define	setname(a)	bname = (a);

uchar_t	**evalvec;
uchar_t	*evalp;

#define NLQUOTE '\001'
#define ALIASCHR '\002'
#define ALIASSTR "\002"
#ifdef NLS
#define NLDEBUG
/* NLQUOTE is an arbitrary escape byte, but NLQUOTE & QUOTE must be zero 
 * ALIASCHR and ALIASSTR are arbitrary (0200 can't be used for NLS since its
 * a valid character
 */
extern bool    NLdebug;
#else

# ifdef KJI
#define PUTCH(p,c)	{if (c < 0xff) *p++ = c; else {*p++ = _NCtop(c); \
			 *p++= _NCbot(c);}}

#define PUTSTR(a,b)	{*a = *b++; if (NCisshift(*a)) *(++a) = *b++; a++;}
extern bool    NLdebug;
# endif

#endif
