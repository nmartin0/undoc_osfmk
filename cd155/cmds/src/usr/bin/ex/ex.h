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
/* @(#)$RCSfile: ex.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:23:44 $ */
/*
 * COMPONENT_NAME: (CMDEDIT) ex.h
 *
 * FUNCTION: none
 *
 * ORIGINS: 3, 10, 13, 26, 27
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
 * ex.h	1.9  com/cmd/edit/vi,3.1,9013 2/21/90 17:51:20
 * 
 * Copyright (c) 1981 Regents of the University of California 
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#ifdef V6
#include <retrofit.h>
#endif

/*
 * Ex version 3 (see exact version in ex_cmds.c, search for /Version/)
 *
 * Mark Horton, U.C. Berkeley
 * Bill Joy, U.C. Berkeley
 * November 1979
 *
 * This file contains most of the declarations common to a large number
 * of routines.  The file ex_vis.h contains declarations
 * which are used only inside the screen editor.
 * The file ex_tune.h contains parameters which can be diddled per installation.
 *
 * The declarations relating to the argument list, regular expressions,
 * the temporary file data structure used by the editor
 * and the data describing terminals are each fairly substantial and
 * are kept in the files ex_{argv,re,temp,tty}.h which
 * we #include separately.
 *
 * If you are going to dig into ex, you should look at the outline of the
 * distribution of the code into files at the beginning of ex.c and ex_v.c.
 * Code which is similar to that of ed is lightly or undocumented in spots
 * (e.g. the regular expression code).  Newer code (e.g. open and visual)
 * is much more carefully documented, and still rough in spots.
 *
 * Please forward bug reports to
 *
 *      Computer Systems Research Group
 *      Computer Science Division, EECS
 *      EVANS HALL
 *      U.C. Berkeley 94720
 *      (415) 642-7780 (project office)
 *
 * or to 4bsd-bugs@Berkeley on the ARPA-net or ucbvax!4bsd-bugs on UUCP.
 * We would particularly like to hear of additional terminal descriptions
 * you add to the terminfo data base.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <NLctype.h>
#define is_cntrl(c)     ((c) < ' ' || (c) == 0x7f)
#define is_print(c)     ((c) >= ' ' && (c) != 0x7f)

#if defined(KJI)
/* indicate if c displays as a double-wide character */
#define is_dblwid(c)    ((c) > 0x400)
#endif

#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/stat.h>

#include <NLchar.h>

#define BUFFERSIZ (2048)
#ifndef var
#define var     extern
#endif
/*
 *      The following little dance copes with the new USG tty handling.
 *      This stuff has the advantage of considerable flexibility, and
 *      the disadvantage of being incompatible with anything else.
 *      The presence of the symbol USG will indicate the new code:
 *      in this case, we define CBREAK (because we can simulate it exactly),
 *      but we won't actually use it, so we set it to a value that will
 *      probably blow the compilation if we goof up.
 */
/*
 *	Note to future maintainers of this file:
 *	vi does not, and never has, used curses. It uses the underlying
 *	database (terminfo or termcap) but not the high-level routines.
 *	This is primarily historical, but changing it will be time 
 *	consuming, and not necessarily of any benefit. DO NOT change this
 *	file to #include <curses.h>, directly or indirectly, unless you
 *	rip out the high-level routines and actually replace them with
 *	the routines from curses. The only reason this note exists is
 *	that some genius already tried this, and made a complete hash
 *	of this file and most of the others due to name conflicts. It
 *	also breaks "makeoptions". Horton & Joy were no fools, so don't
 *	second-guess them lightly.
 */
#ifdef USG
#include <termio.h>
typedef struct termio SGTTY;
#else
#include <sgtty.h>
typedef struct sgttyb SGTTY;
#endif

/* By defining SINGLE, we avoid dereferencing NULL pointers if
 * there is a problem initializing the terminfo information. This
 * is basically a hack, to deal with the sloppy port from termcap
 * to terminfo.
 */
#define SINGLE
#include <term.h>
#undef SINGLE

#if defined(USG) && !defined(CBREAK)
#define CBREAK xxxxx
#endif

extern  int errno;

typedef int     line;

#include "ex_tune.h"
#include "ex_vars.h"
/*
 * Options in the editor are referred to usually by "value(name)" where
 * name is all uppercase, i.e. "value(PROMPT)".  This is actually a macro
 * which expands to a fixed field in a static structure and so generates
 * very little code.  The offsets for the option names in the structure
 * are generated automagically from the structure initializing them in
 * ex_data.c... see the shell script "makeoptions".
 */
struct  option {
        char    *oname;
        char    *oabbrev;
        short   otype;          /* Types -- see below */
        short   odefault;       /* Default value */
        short   ovalue;         /* Current value */
        char    *osvalue;
};

#define ONOFF   0
#define NUMERIC 1
#define STRING  2               /* SHELL or DIRECTORY */
#define OTERM   3
#if defined(KJI)
#define ONECOL  4               /* need char that is 1 column wide on display */
#define WRAP    5               /* wraptype - word, col, rigid */
#endif
#define	CLOSEP	6		/* closing punctuation */
#define value(a)        options[a].ovalue
#define svalue(a)       options[a].osvalue
extern   struct option options[NOPTS + 1];
#ifdef TRACE
#       include <stdio.h>
        var     FILE    *trace;
        var     short    trubble;
        var     short    techoin;
        var     char    tracbuf[BUFFERSIZ];
#endif
#define SBUFSIZ 256     /* small buffer size */
#define NCSIZE(x)       (sizeof((x)) / sizeof(NLchar))
/*
 * For improved performance, vi optimizes the updating of variuos terminals
 * that act differently for spaces, tabs and blanks.
 * See lengthy comment in ex_vput.c before vgotab().
 *
 * To make NLS vi and Kanji vi more compatible, the QUOTE bit has
 * been eradicated.
 *
 * We need a bunch of out-of-band values to represent what used to be the
 * QUOTE bit.  QUOTE_CTRL_D (^D) is used only in ex_vops2.c, QUOTE_BSP (^H)
 * gets passed through the putchar() routines to flush1(), QUOTE_NL (^J) and
 * QUOTE_CR (^M) are handled within the putchar() routines.  QUOTE_NUL and
 * QUOTE_SP are the only ones used by the pseudo-curses code.
 *
 * QUOTE_BULLET indicates a PARTIALCHAR indicator in the last display column.
 * The PARTIALCHAR feature replaces an is_dblwid(c) in the last
 * display column with a continuation indicator.  The double-wide character
 * is displayed on the next row.
 *
 * PRINTWID and is_dblwid must indicate that QUOTE_BULLET and QUOTE_SP
 * display in 1 column.
 */
#define QUOTE_CTRL_D    0x300
#define QUOTE_BSP       0x301
#define QUOTE_NL        0x302
#define QUOTE_CR        0x303
#define QUOTE_NUL       0x304
#define QUOTE_SP        0x305
#if defined(KJI)
#define QUOTE_KJI       0x306
#define QUOTE_BULLET    0x307
#endif
#define ESCAPE  '\033'
#define Ctrl(c) ((c) & 037)
#define ctlof(c)        ((c) ^ 0x40)

/*
 * Miscellaneous random variables used in more than one place
 */
var     NLchar  NCemptystr[1];  /* an NLchar null string */
char    origfile[FNSIZE];	/* holds name of originally edited file */
var     short    aiflag;         /* Append/change/insert with autoindent */
var     short    anymarks;       /* We have used '[a-z] */
var     int     chng;           /* Warn "No write" */
var     char    *Command;
var     short   defwind;        /* -w# change default window size */
var     int     dirtcnt;        /* When >= MAXDIRT, should sync temporary */
#ifdef SIGTSTP
var     short    dosusp;         /* Do SIGTSTP in visual when ^Z typed */
#endif
var     short    edited;         /* Current file is [Edited] */
var     line    *endcore;       /* pointer to (just beyond) end of allocated memory */
extern  short    endline;        /* Last cmd mode command ended with \n */
var     line    *fendcore;      /* First address in line pointer space */
var     char    file[FNSIZE];   /* Working file name */
var     NLchar  genbuf[LBSIZE]; /* Working buffer when manipulating linebuf */
var     short    hush;           /* Command line option - was given, hush up! */
var     NLchar  *globp;         /* (Untyped) input string to command mode */
var     short    holdcm;         /* Don't cursor address */
var     short    inappend;       /* in ex command append mode */
var     short    inglobal;       /* Inside g//... or v//... */
var     NLchar  *initev;        /* Initial : escape for visual */
var     short    inopen;         /* Inside open or visual */
var     NLchar  *input;         /* Current position in cmd line input buffer */
var     short    intty;          /* Input is a tty */
var     short   io;             /* General i/o unit (auto-closed on error!) */
extern  int     lastc;          /* Last character ret'd from cmd input */
var     short    laste;          /* Last command was an "e" (or "rec") */
var     NLchar  lastmac;        /* Last macro called for ** */
var     NLchar  lasttag[TAGSIZE];       /* Last argument to a tag command */
var     NLchar  *linebp;        /* Used in substituting in \n */
var     NLchar  linebuf[LBSIZE];        /* The primary line buffer */
var     short    listf;          /* Command should run in list mode */
var     NLchar  *loc1;          /* Where re began to match (in linebuf) */
var     NLchar  *loc2;          /* First char after re match (") */
var     line    names['z'-'a'+2];       /* Mark registers a-z,' */
var     int     notecnt;        /* Count for notify (to visual from cmd) */
var     short    numberf;        /* Command should run in number mode */
var     char    obuf[BUFFERSIZ];   /* Buffer for tty output */
var     short   oprompt;        /* Saved during source */
var     short   ospeed;         /* Output speed (from gtty) */
var     int     otchng;         /* Backup tchng to find changes in macros */
var     int     peekc;          /* Peek ahead character (cmd mode input) */
var     NLchar  *pkill[2];      /* Trim for put with ragged (LISP) delete */
var     short    pfast;          /* Have stty -nl'ed to go faster */
var     int     pid;            /* Process id of child */
var     int     ppid;           /* Process id of parent (e.g. main ex proc) */
var     jmp_buf resetlab;       /* For error throws to top level (cmd mode) */
var     int     rpid;           /* Pid returned from wait() */
var     short    ruptible;       /* Interruptible is normal state */
var     short    seenprompt;     /* 1 if have gotten user input */
var     short    shudclob;       /* Have a prompt to clobber (e.g. on ^D) */
var     int     status;         /* Status returned from wait() */
var     int     tchng;          /* If nonzero, then [Modified] */
extern  short   tfile;          /* Temporary file unit */
var     short    vcatch;         /* Want to catch an error (open/visual) */
var     jmp_buf vreslab;        /* For error throws to a visual catch */
var     short    writing;        /* 1 if in middle of a file write */
var     int     xchng;          /* Suppresses multiple "No writes" in !cmd */
var	long	bsize;		/* Block size for disk i/o */

/*
 * Macros
 */
#define CP(a, b)        (void) NCstrcpy(a, b)
                        /*
                         * FIXUNDO: do we want to mung undo vars?
                         * Usually yes unless in a macro or global.
                         */
#define FIXUNDO         (inopen >= 0 && (inopen || !inglobal))
#define ckaw()          {if (chng && value(AUTOWRITE) && !value(READONLY)) wop(0);}
#ifdef lint
#define NLsgetc(s)      (*(s)++)
#define NLsputc(s, c)   (*(s)++ = (c))
#else
/* The NLS equivalent of RHS *s++.  Argument must be an lvalue. */
/* NLsgetc gets 1 or 2 bytes from (char *) s to form the (NLchar) c */
#define NLsgetc(s)      (_NCis2((s)[0], (s)[1]) ? (s)+=2, _NCd2((s)[-2], \
                         (s)[-1]) : (*(s)++ & 0xff))
/* The NLS equivalent of LHS *s++.  Argument must be an lvalue. */
/* NLsputc puts 1 or 2 bytes into (char *) s depending on the (NLchar) c */
#if defined(KJI)
#define NLsputc(s, c)   ((c) <= 0xff ? (*(s)++ = (c)) : \
                         (*(s)++ = (c)>>8, *(s)++ = (c)))
#else
#define NLsputc(s, c)   ((c) <= 0xff ? (*(s)++ = (c)) : \
                         (*(s)++ = 0x21 - ((c)>>7), *(s)++ = (c)|0x80))
#endif
#endif
#define copy(to,from,n) (void)memcpy((char *)(to), (char *)(from), n) 
#define eq(a, b)        ((a) && (b) && strcmp(a, b) == 0)
#define NCeq(a, b)      ((a) && (b) && wcscmp(a, b) == 0)
#define getexit(a)      copy(a, resetlab, sizeof (jmp_buf))
#define lastchar()      lastc
#define outchar(c)      (*Outchar)(c)
#define pastwh()        (ignore(skipwh()))
#define pline(no)       (*Pline)(no)
#define reset()         longjmp(resetlab,1)
#define resexit(a)      copy(resetlab, a, sizeof (jmp_buf))
#define setexit()       setjmp(resetlab)
#define setlastchar(c)  lastc = c
#define ungetchar(c)    peekc = c

			/* macro must set vcatch after setjmp */
#define CATCH           if (setjmp(vreslab) == 0 && (vcatch=1)) {
#define ONERR           } else { vcatch = 0;
#define ENDCATCH        } vcatch = 0;

/*
 * Environment like memory
 */
var     char    altfile[FNSIZE];        /* Alternate file name */
extern  char    direct[ONMSZ];          /* Temp file goes here */
extern  char    shell[ONMSZ];           /* Copied to be settable */
extern	 char	ttytype[ONMSZ];		/* A long and pretty name */
/* buffer to exec shell commands */
var     char    uxb[(UXBSIZE + 2) * sizeof (NLchar)];
/* nc_uxb for . repeat command because vglobp is read in getbr() */
var     NLchar  nc_uxb[(UXBSIZE + 2)];
/*
 * The editor data structure for accessing the current file consists
 * of an incore array of pointers into the temporary file tfile.
 * Each pointer is 15 bits (the low bit is used by global) and is
 * padded with zeroes to make an index into the temp file where the
 * actual text of the line is stored.
 *
 * To effect undo, copies of affected lines are saved after the last
 * line considered to be in the buffer, between dol and unddol.
 * During an open or visual, which uses the command mode undo between
 * dol and unddol, a copy of the entire, pre-command buffer state
 * is saved between unddol and truedol.
 */
var     line    *addr1;                 /* First addressed line in a command */
var     line    *addr2;                 /* Second addressed line */
var     line    *dol;                   /* Last line in buffer */
var     line    *dot;                   /* Current line */
var     line    *one;                   /* First line */
var     line    *truedol;               /* End of all lines, including saves */
var     line    *unddol;                /* End of undo saved lines */
var     line    *zero;                  /* Points to empty slot before one */

/*
 * Undo information
 *
 * For most commands we save lines changed by salting them away between
 * dol and unddol before they are changed (i.e. we save the descriptors
 * into the temp file tfile which is never garbage collected).  The
 * lines put here go back after unddel, and to complete the undo
 * we delete the lines [undap1,undap2).
 *
 * Undoing a move is much easier and we treat this as a special case.
 * Similarly undoing a "put" is a special case for although there
 * are lines saved between dol and unddol we don't stick these back
 * into the buffer.
 */
var     short   undkind;
var     line    *unddel;        /* Saved deleted lines go after here */
var     line    *undap1;        /* Beginning of new lines */
var     line    *undap2;        /* New lines end before undap2 */
var     line    *undadot;       /* If we saved all lines, dot reverts here */

#define UNDCHANGE       0
#define UNDMOVE         1
#define UNDALL          2
#define UNDNONE         3
#define UNDPUT          4
#ifdef CRYPT
/*
 * Various miscellaneous flags and buffers needed by the encryption routines.
 */
#define KSIZE   9       /* key size for encryption */
#define KEYPROMPT       "Key: "
var     int     xflag;          /* True if we are in encryption mode */
var     int     xtflag;         /* True if the temp file is being encrypted */
var     int     kflag;          /* True if the key has been accepted */
var     char    perm[768];
var     char    tperm[768];
var     char    *key;
var     char    crbuf[CRSIZE];
char    *getpass();
#endif

/*
 * Function type definitions
 */
#define NOSTR   (NLchar *) 0
#define NOLINE  (line *) 0

extern  int     (*Outchar)();
extern  int     (*Pline)();
extern  int     (*Putchar)();
var     void     (*oldhup)();
int     (*setlist())();
int     (*setnumb())();
line    *address();
NLchar  *genindent();
char    *getenv();
line    *getmark();
char	*longname();
char    *malloc();
/* char    *memcpy(); removed for ansi compliance */
char    *mesg();
NLchar  *place();
line    *setin();
char    *strcat();
char    *strend();              /* part of vi source */
NLchar  *NCstrcat();
NLchar  *NCstrcpy();
NLchar  *NCstrcpy2();           /* part of vi sources */
NLchar  *NCstrend();            /* part of vi sources */
char    *tailpath();
char    *tgetstr();
char    *tgoto();
char    *tparm();
char    *ttyname();
line    *vback();
NLchar  *vfindcol();
NLchar  *vgetline();
NLchar  *vpastwh();
NLchar  *vskipwh();
int     put();
int     putreg();
int     YANKreg();
int     delete();
int     filter();
int     getfile();
int     getsub();
int     gettty();
int     join();
int     listchar();
off_t   lseek();
int     normline();
int     numbline();
var     void     (*oldquit)();
void    onhup();
void    onintr();
void	oncore();
#ifdef SIGTSTP
void    onsusp();
#endif
int     putch();
int     shift();
int     termchar();
int     vfilter();
#ifdef CBREAK
void    vintr();
#endif
int     vputch();
int     vshftop();
int     yank();
unsigned alarm();
unsigned sleep();
void    error();
void    exit();
void    merror();
void    serror();
void    smerror();
void    syserror();
void    tail();
void    tail2of();
void    tailprim();
#       define  ignore(a)       a
#       define  ignorf(a)       a
#ifdef MSG
#include "ex_msg.h"
var     nl_catd ex_catd;
#define msg(id,ds)      catgets(ex_catd, 1, id, ds)
#else
#define msg(id,ds)      ds
#endif

#ifdef OSF_MMAP
var int pagesize;	/* For mmap() increment */
#endif
