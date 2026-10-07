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
 
/*++ qsub.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qsub/qsub.c,v $
 *
 * DESCRIPTION:
 *
 *	Submit a batch request to the NQS system for execution.
 *	This program MUST be run as a setuid program.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY:($Revision: 1.5 $ $Date: 1994/11/19 01:36:12 $ $State: Exp $)
 * $Log: qsub.c,v $
 * Revision 1.5  1994/11/19  01:36:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/09/07  20:24:36  shala
 * Fixed bug #6360 by jkearns.
 *
 * Revision 1.3.18.1  1993/09/07  18:13:24  shala
 * Fixed bug #6360. Fixed by jkearns.
 *
 * Revision 1.3  1992/12/17  21:16:16  mwan
 * T6 update 2
 *
 * Revision 1.2  1992/10/09  22:30:36  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:29:42  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:59:10  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:08:39  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "%W% (qsub.c OSF/1 NQS2.0 GJK) %G%";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>		/* Standard I/O */
#include <ctype.h>		/* Typedefs */
#include <signal.h>		/* Signal names */
#include "nqs.h"		/* NQS definitions and sys/type.h */
#include NETDB			/* Network database header file; */
				/* Proto/Makefile will tell us */
				/* where to find netdb.h */
#include "nqsdirs.h"		/* For nqslib library functions */
#include "informcc.h"		/* NQS information completion bits/masks */
#include "mkreqcc.h"		/* mkreq.c (mkctrl) completion codes */
#if	UNICOS
#include "scpnqs.h"		/* UNICOS SCP interface structure */
#endif

#ifdef SDSC
#include "transactcc.h"
#include <grp.h>
#include <ctype.h>
#include <pwd.h>
#include "buddy.h"
#ifdef NXACCT
#include <nx/nxacct.h>
#endif
#endif


/*
 *	The #defines of FLAG_ are used to detect multiple specifications
 *	of a given argument of option flag.
 *	These bits refer to the variables cmdflags and scriptflags.
 */
#define	FLAG_A		000000000001L	/* "-a" seen */
#ifdef SDSC
/* since all 32 bits are used, we are using the flag that used to be -p */
#define	FLAG_C		000100000000L	/* "-c" account */
#endif
#define	FLAG_E		000000000002L	/* "-e" */
#define	FLAG_EO		000000000004L	/* "-eo" */
#define	FLAG_KE		000000000010L	/* "-ke" */
#define	FLAG_KO		000000000020L	/* "-ko" */
#define	FLAG_L6		000000000040L	/* "-l6" */
#define FLAG_LC		000000000100L	/* "-lc" */
#define	FLAG_LD		000000000200L	/* "-ld" */
#define	FLAG_LFP	000000000400L	/* "-lf" */
#define	FLAG_LFR	000000001000L	/* "-lF" */
#define	FLAG_LMP	000000002000L	/* "-lm" */
#define	FLAG_LMR	000000004000L	/* "-lM" */
#define	FLAG_LN		000000010000L	/* "-ln" */
#define	FLAG_LP		000000020000L	/* "-lP" */
#define	FLAG_LQP	000000040000L	/* "-lq" */
#define	FLAG_LQR	000000100000L	/* "-lQ" */
#define FLAG_LS		000000200000L	/* "-ls" */
#define	FLAG_LTP	000000400000L	/* "-lt" */
#define	FLAG_LTR	000001000000L	/* "-lT" */
#define FLAG_LVP	000002000000L	/* "-lv" */
#define FLAG_LVR	000004000000L	/* "-lV" */
#define	FLAG_LW		000010000000L	/* "-lw" */
#define	FLAG_MU		000020000000L	/* "-mu" */
#define	FLAG_O		000040000000L	/* "-o" */
#define	FLAG_P		000100000000L	/* "-p" */
#define	FLAG_Q		000200000000L	/* "-q" */
#define	FLAG_R		000400000000L	/* "-r" */
#define	FLAG_RE		001000000000L	/* "-re" */
#define	FLAG_RO		002000000000L	/* "-ro" */
#define	FLAG_S		004000000000L	/* "-s" */
#define	FLAG_X		010000000000L	/* "-x" */
#ifdef SDSC
#define	FLAG_LPR	020000000000L	/* "-lw" */
#endif

/*
 *	The #defines of FLAG2_ are used to detect conflicts,
 *	and to know when to store the current directory.
 *	These bits refer to the variables cmdflags2 and scriptflags2.
 */
#define	FLAG2_EM	00000000001L	/* "-e" with machine name */
					/* specification */
#define	FLAG2_OM	00000000002L	/* "-o" with machine name */
					/* specification */

#define	MAXLINEARGS	63	/* Maximum number of line arguments scanned */
				/* on a shell script line */
#define	COPYBUFSIZE	4096	/* Size of the buffer used to produce the */
				/* spooled version of the shell script file */

/*
 *	Get/Set output file access mode macros.
 */
#define	GETACCESS(a)	(a & ~OMD_M_KEEP)
#define	SETACCESS(a,b)	(a = (a & OMD_M_KEEP) | b) /* Clobber all but M_KEEP */

/*
 *	UNIX external definitions:
 */
extern int errno;		/* System call error # */
extern char *sys_errlist [];	/* Error text list */

/*
 *	External functions:
 */

extern void analyzetcm();	/* Analyze and display transaction */
				/* diagnostics */
extern int atoi();		/* Ascii to integer conversion */
extern char *destacct();	/* Get local account relative to machine */
extern char *destpath();	/* Get local batch path relative to machine */
extern char *getenv();		/* Get pointer to environment var value */
extern int gethostname();	/* Get name of local host */
extern int isdecstr();		/* Boolean; Is a string of decimal digits */
extern long lseek();		/* System call returns non-int */
extern int machacct();		/* Get machine-id for account specification */
extern int machpath();		/* Get machine-id for batch pathname */
extern char *malloc();		/* Allocate from heap space */
extern int mkctrl();		/* Make a control file */
extern int mkdata();		/* Make a data file */
extern long quereq();		/* Queue an NQS request */
extern int scancpulim();	/* Scan a cpu time limit */
extern int scanquolim();	/* Scan a disk or memory quota limit */
extern int scnftime();		/* Scan a date/time specification defaulting */
				/* unspecified values to non-past current or */
				/* next temporally closest value */
extern char *strcat();		/* Concatenate strings */
#if	BSD42 | BSD43 | ULTRIX
extern char *index();		/* Locate a character within a string */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern char *strchr();		/* Locate a character within a string */
#else
BAD SYSTEM TYPE
#endif
#endif
extern int strcmp();		/* String comparison */
extern char *strcpy();		/* String copy */
extern int strlen();		/* Length of string */
extern int strncmp();		/* String comparison limited by length */
extern char *strncpy();		/* String copy n characters */
#if	BSD42 | BSD43 | ULTRIX
extern char *rindex();		/* Locate a character from end of string */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern char *strrchr();		/* Locate a character from end of string */
#else
BAD SYSTEM TYPE
#endif
#endif
extern int writereq();		/* Write control file request header */
extern void zapreq();		/* Unlink all files associated with a */
				/* request from the NQS new request */
				/* staging directory */
#ifdef SDSC
extern int read_glb_param ();
#ifdef NXACCT
extern gid_t nx_getacctid ();
extern struct nxacct *nx_getanam ();
#else
extern struct passwd *getpwuid ();
extern struct group *getgrnam ();
extern struct group *getgrgid ();
#endif
extern int my_open ();
extern int my_runok ();
extern struct inp inp;
extern struct glb glb;
#endif

/*
 *	Internal functions not returning a value. "exit" means exit gracefully.
 */
void bad_syntax();		/* Display message, exit */
void bad_value();		/* Display message, exit */
#ifdef SDSC
void bad_account();		/* Display message, exit */
#endif
void cleanup();			/* Exit */
void enverror();		/* Display message, exit */
void exceeds();			/* Display message, exit */
void flag_conflict();		/* For (-e,-eo), (-e,-ke), (-o,-ko) */
				/* display message, exit */
void insmemstage();		/* Display message, exit */
void invalidflag();		/* Display message.  Might exit */
void notqueued();		/* Display message, exit */
void no_value();		/* Display message, exit */
void scanargs();		/* Scan arguments on shell script line */
void scnametoolong();		/* Display message, exit */
void seekhelp();		/* Display message, exit */
void showline();		/* Show what line we are on */
void spoolerror();		/* Display message, exit */
void twice();			/* Display message, exit */

/*
 *	Variables that are global within this module.
 */
long cmdflags;			/* Command line argument seen flags */
long cmdflags2;			/* Command line argument seen flags */
FILE *scriptfile;		/* Shell script file */
struct rawreq rawreq;		/* Raw request structure */
char scriptbuffer [COPYBUFSIZE];/* Shell script copy buffer */
int scriptbytes;		/* Number of bytes read into script buffer */
short scripteof;		/* EOF flag for script file */
long scriptflags;		/* Script file argument seen flags */
long scriptflags2;		/* Script file argument seen flags */
int scriptline;			/* Shell script line number */
int scriptleft;			/* Number of bytes left in script file copy */
				/* buffer */
char *scriptname = NULL;	/* Name of shell script file */
char *workdir;			/* Current working directory */
char *istafile[MAX_INSTAPERREQ];/* Stage-in file names */
char *ostafile[MAX_OUSTAPERREQ];/* Stage-out file names */
short silent;			/* Request shall be queued silently boolean */
short export;			/* "-x" flag seen boolean */
int scpflag = 0;		/* UNICOS SCP flag */
#if	UNICOS
struct	qsub_scp scpsub;	/* UNICOS structure returned to SCP */
#endif

/*** main
 *
 *
 *	Submit a batch request.
 */
main (argc, argv, envp)
int argc;
char *argv[];
char *envp[];				/* Environment */
{
	char **procarg();		/* Process an argument */

	static char *reserved [] = {	/* Environment variable names */
		"QSUB_HOME",		/* Reserved by NQS */
		"QSUB_HOST",
		"QSUB_REQID",
		"QSUB_REQNAME",
#if	SGI | SYS52 | UNICOS | UTS | OSF
		"QSUB_LOGNAME",
#else
#if	BSD42 | BSD43 | ULTRIX
		"QSUB_USER",
#else
BAD SYSTEM TYPE
#endif
#endif
		"QSUB_MAIL",
		"QSUB_PATH",
		"QSUB_SHELL",
		"QSUB_TZ",
		"QSUB_WORKDIR",
		NULL
	};

	static char *intercept [] = {	/* More environment variables which */
		"HOME",			/* user does not have the right */
		"LOGNAME",		/* to pass with -x */
		"MAIL",			
		"PATH",
		"SHELL",
		"TZ",
		"USER",
		NULL
	};

	FILE *ctrlfile;			/* Control file */
	FILE *datafile;
	char *scriptargs [MAXLINEARGS+1];/* Script file default arguments */
	char linebuf [MAX_REQPATH+2];	/* Shell script line */
	char path [MAX_REQPATH+1];	/* Scratch pathname generation area */
	short linesize;			/* Number of bytes in script line */
	register char *cp;		/* Character scanning */
	register int fromfd;		/* From file-descriptor (script file) */
	register int tofd;		/* To file-descriptor (spooled script)*/
	long reqseq;			/* Sequence # assigned to request */
	long transaction_code;		/* Transaction completion code */
	char hostname [MAX_MACHINENAME+1];/* Name of local host */
	mid_t local_mid;		/* Local host machine-id */
	int i;				/* Work var */
#ifdef SDSC
#ifndef NXACCT
	struct passwd *passwd;
#endif
        char *bp;
        int c;
#ifdef NXACCT
        struct nxacct *nxacct;
#else
        struct group *grp;
#endif
	int resd_open;
#endif

	signal (SIGHUP, cleanup);	/* Upon receipt of any of these */
	signal (SIGQUIT, cleanup);	/* signals, delete all files assoc. */
	signal (SIGINT, cleanup);	/* with the request and exit in a */
	signal (SIGTERM, cleanup);	/* graceful and dignified manner */
	/*
	 *
	 *  Do 500 hundred things along with creating the control file
	 *  for the request.  When done, we will have a control file, our
	 *  current working directory will be the NQS new request
	 *  directory, we will have lost our setuid() privileges,
	 *  and the request structure we specified will have been
	 *  initialized to a reasonable default state.
	 */
#ifdef SDSC
	/* Read the sched_param file */

	if (read_glb_param () != 0) {
		perror ("reading the sched_param file");
		MACS_FLAG = MACS_OFF;
	}

	/* open a privileged port before we lost the privilege. */

	if (MACS_FLAG != MACS_OFF) {
	    if ((resd_open = my_open ()) < 0) {
		perror ("connecting to MACS");
		if (MACS_FLAG == MACS_ON) {
		    printf ("Job not queued\n");
		    exit (1);
		}
	    }
	}
#endif
	switch (mkctrl (&rawreq, RTYPE_BATCH, &ctrlfile, &workdir,
			&local_mid)) {
	case MKREQ_SUCCESS:
		break;
	case MKREQ_NOCREATE:
		fprintf (stderr, "Unable to create control file.\n");
		seekhelp();		/* Exit */
	case MKREQ_NOMID:
		fprintf (stderr, "Unable to determine local machine-id.\n");
		seekhelp();		/* Exit */
	case MKREQ_BADREQTYPE:
		fprintf (stderr, "Bad request type.\n");
		seekhelp();		/* Exit */
	case MKREQ_NOUSERNAME:
		fprintf (stderr,
			 "Unable to determine username from user-id.\n");
		seekhelp();		/* Exit */
	case MKREQ_NOCWD:
		fprintf (stderr,
			 "Unable to determine current working directory.\n");
		seekhelp();		/* Exit */
	case MKREQ_CWDNEWLINE:
		/*
		 *  Wow!  The current working directory contains a newline
		 *  character which causes all kinds of consternation since
		 *  the newline character is used as the delimiter for all
		 *  entities in the varying portion of a NQS control file
		 *  following the fixed portion.
		 */
		fprintf (stderr, "Current working directory path ");
		fprintf (stderr, "contains a newline character.\n");
		notqueued();		/* Exit */
	case MKREQ_NOCHDIRNEW:
		fprintf (stderr,
			 "Unable to chdir() to new request directory.\n");
		seekhelp();		/* Exit */
	case MKREQ_NOLOCALDAE:
		fprintf (stderr,
			 "Unable to get a pipe to the local daemon.\n");
		seekhelp();		/* Exit */
	case MKREQ_NOSETUGID:
		fprintf (stderr, "Unable to set [uid,gid].\n");
		seekhelp();		/* Exit */
	case MKREQ_NOPARMFILE:
		fprintf (stderr, "Unable to open NQS parameters file.\n");
		seekhelp();		/* Exit */
	}
	/*
	 *  The control file was successfully created.
	 *  Create the data file which shall hold the spooled version
	 *  of the shell script file.
	 */
	if (mkdata (&datafile) != 0) {
		/*
		 *  We were unable to create the data file to hold the
		 *  spooled shell script.
		 */
		fprintf (stderr, "Unable to create spooled script file.\n");
		seekhelp();		/* Exit */
	}
	/*
	 *  The data file was successfully created.  We must now scan
	 *  the command line arguments.  When all command line arguments
 	 *  have been successfully scanned, the shell script file must
	 *  then be spooled.
	 */
	rawreq.ndatafiles = 1;		/* One data file for req */
	cmdflags = 0;			/* No command line flags seen yet */
	scriptflags = 0;		/* No script file flags seen yet */
	silent = 0;			/* -z not seen */
	export = 0;			/* -x not seen */
	argv++;				/* Reference first argument */
	while (*argv != NULL) {
		argv = procarg (argv, 1);	/* Scan arguments */
	}
	/*
	 *  We have reached the end of the command line arguments.
	 *  We must now spool the shell script file.
	 *
	 *  When spooling a shell script file, it is necessary to scan
	 *  for flags identical to the command line flags.   These
	 *  optional, syntactically identical flags are used to
	 *  determine default characteristics for the request when the
	 *  command line flags have omitted the definition of a
	 *  particular request characteristic.
	 *
	 *  These flags if present, must appear inside a shell script
	 *  comment, and must be preceded by the special character
	 *  sequence:  "@$" or "QSUB".
	 *
	 *  The algorithm used to scan for default command line arguments
	 *  is as follows:
	 *
	 *	1.  Read the first line of the shell script file.
	 *
	 *	2.  If the line contains only space or tab characters,
	 *	    or the first non-whitespace character in the line is
	 *	    ":", then goto step 7.
	 *
	 *	3.  If the first non-whitespace character of the current
	 *	    line is not a '#' character, then goto step 8.
	 *
	 *	4.  If the second non-whitespace character in the current
	 *	    line is NOT the '@' character, or the character immed-
	 *	    iately following the second non-whitespace character
	 *	    in the current line is NOT a '$' character, then goto
	 *	    step 7.
	 *
	 *				OR
         *
	 *	    If the second non-whitespace character in the current
	 *	    line is NOT the 'Q' character, or the string immed-
	 *	    iately following the second non-whitespace character
	 *	    in the current line is NOT "SUB", then goto step 7.
         *
         *
	 *	5.  If no '-' is present as the first non-whitespace 
         *          character following the '@$' or 'QSUB' sequence, then
         *          goto step 8.
	 *
	 *	6.  Process one or more embedded option flags, stopping
	 *	    the parsing process upon reaching the end of the
	 *	    line, or upon reaching the first unquoted '#'
	 *	    character.
	 *
	 *	7.  Read the next line.  Goto step 2.
	 *
 	 *	8.  End.  No more embedded options will be recognized.
	 */
	if (scriptname == NULL) {	/* No specific script name */
		scriptfile = stdin;
		scriptname = "STDIN";
	}
	else {
		/*
		 *  A script file has been specified.
		 */
		if (scriptname [0] != '/') {
			/*
			 *  The pathname is relative.  Expand to absolute
			 *  pathname since we had to chdir() to the new
			 *  request directory.
			 */
			if (strlen (workdir) + 1 +
			    strlen (scriptname) > MAX_REQPATH) {
				scnametoolong(); /* Script name too long */
			}
			sprintf (path, "%s/%s", workdir, scriptname);
		}
		else {
			if (strlen (scriptname) > MAX_REQPATH) {
				scnametoolong(); /* Script name too long */
			}
			strcpy (path, scriptname);
		}
		if ((scriptfile = fopen (path, "r")) == NULL) {
			fprintf (stderr, "Unable to open script file: %s.\n",
				scriptname);
			fprintf (stderr, "Reason: %s.\n", sys_errlist [errno]);
			notqueued();	/* Exit */
		}
	}
	scriptbytes = 0;		/* No bytes in script file buffer */
	scripteof = 0;			/* No EOF on script file yet */
	scriptline = 0;			/* On line 0 of the shell script file */
	scriptleft = 0;			/* No bytes left in script file buffer*/
	linesize = getscript (linebuf);	/* Read a line from the script file */
	while (linesize >= 0) {		/* While there might be flags */
					/* in the script file... */
		/*
		 *  Echo the script file line to the spooled version of
		 *  the script file.
		 */
		if (fputs (linebuf, datafile) == EOF ||
		    fputc ('\n', datafile) == EOF) {
			/*
			 *  Display message and exit.
			 */
			spoolerror();
		}
		cp = linebuf;
		while (isspace (*cp)) cp++;
		if (*cp == '#') {
			/*
			 *  This line is a comment line.  Scan the line
			 *  for possible embedded default argument(s).
			 */
			do {
				do {
				    cp++;
				} while (*cp != '\0' && *cp != '@'
							 && *cp != 'Q');
				if (*cp == '@' && cp [1] == '$') {
				    /*
				     *  We have embedded default argument(s),
				     *  or the end of the embedded default
				     *  argument set has been reached.
				     */
				    cp += 2;	/* Scan past '$' */
				    while (isspace(*cp))
					cp++;
				    if (*cp == '-') {
					/*
					 *  We have embedded default
					 *  argument(s) to parse.
					 */
					scanargs (cp, scriptargs);
					argv = scriptargs;
					do {
					    argv = procarg (argv, 0);
					} while (*argv != NULL);
				    }
				    else {
					/*
					 * The search for default command line
					 * arguments is to be ended.
					 */
					linesize = -1;
				    }
				}
				else if (*cp == 'Q' && cp [1] == 'S' &&
					  cp [2] == 'U' && cp [3] == 'B') {
				    /*
				     *  We have embedded default argument(s),
				     *  or the end of the embedded default
				     *  argument set has been reached.
				     */
				    cp += 4;	/* Scan past 'B' */
				    while (isspace(*cp))
					cp++;
				    if (*cp == '-') {
					/*
					 *  We have embedded default
					 *  argument(s) to parse.
					 */
					scanargs (cp, scriptargs);
					argv = scriptargs;
					do {
					    argv = procarg (argv, 0);
					} while (*argv != NULL);
				    }
				    else {
					/*
					 * The search for default command line
					 * arguments is to be ended.
					 */
					linesize = -1;
				    }
				}
			} while (linesize >= 0 && *cp);
		}
		else if (*cp != '\0' &&		/* Blank lines are skipped */
			 *cp != ':') {		/* Null commands are skipped */
			/*
			 *  There are no more default command line
			 *  arguments to be processed.
			 */
			linesize = -1;		/* Set to exit loop */
		}
		if (linesize >= 0) {
			/*
			 * It is possible that more default command line
			 * arguments exist.
			 * Get the next line of the shell script file.
			 */
			linesize = getscript (linebuf);
		}
	}
	fflush (datafile);			/* Purge spool output buffer */
	if (ferror (datafile)) spoolerror();	/* Exit */
	/*
	 *  Now, finish spooling the rest of the shell script file.
	 */
	fromfd = fileno (scriptfile);
	tofd = fileno (datafile);
	if (scriptleft) {
		/*
		 * Write-out bytes remaining in buffer that are not going
		 * to be scanned for the appropriate flags, etc.
		 */
		cp = scriptbuffer + scriptbytes - scriptleft;
		if (write (tofd, cp, scriptleft) != scriptleft) {
			/*
			 *  Display message and exit.
			 */
			spoolerror();
		}
	}
	if (!scripteof) {
		/*
		 *  There are more bytes to spool to the shell script file.
		 */
		while ((scriptbytes = read (fromfd, scriptbuffer,
					  COPYBUFSIZE)) != 0) {
			if (scriptbytes < 0 ||
			    write (tofd, scriptbuffer,
				   scriptbytes)!=scriptbytes) {
				/*
				 *  Display message and exit.
				 */
				spoolerror();
			}
		}
	}
	close (fromfd);		/* Close source version of script file */
	close (tofd);		/* Close spooled version of script file */
	/*
	 *  We have completed the arduous process of scanning the command
	 *  line and default shell script arguments.  The shell script file
	 *  has also been completely spooled.
	 */
	if (rawreq.quename [0] == '\0') {
		/*
		 *  No queue specified, and no default batch queue
		 *  is declared for the local system.
		 */
		fprintf (stderr, "No request queue specified, and no ");
		fprintf (stderr, "local default has been defined.\n");
		notqueued();		/* Exit */
	}
	/*
	 *  If no standard error file has been specified, AND
	 *  the standard error mode is not OMD_EO, and OMD_M_KEEP
	 *  is not set, then it is necessary to store the current working
	 *  directory in the stderr name slot and to also make sure that
	 *  the manufactured default name will fit.
	 */
	if (rawreq.v.bat.stderr_name [0] == '\0' &&
	    GETACCESS(rawreq.v.bat.stderr_acc) != OMD_EO &&
	    (rawreq.v.bat.stderr_acc & OMD_M_KEEP) == 0) {
		/*
		 *  No stderr pathname was specified.
		 */
		if (strlen (workdir) + 15 > MAX_REQPATH) {
			/*
			 *  Unable to form default standard error file
			 *  name because the pathname would be too long.  Exit.
			 */
			exceeds ("Default stderr pathname", MAX_REQPATH, 1);
		}
		/*
		 *  Copy in the current working directory
		 *  FOLLOWED BY A SLASH!!!!!
		 */
		strcpy (rawreq.v.bat.stderr_name, workdir);
		strcat (rawreq.v.bat.stderr_name, "/");
	}
	else if (rawreq.v.bat.stderr_name [0] != '\0' &&
		 rawreq.v.bat.stderr_name [0] != '/' &&
		(rawreq.v.bat.stderr_acc & OMD_M_KEEP) == 0) {
		/*
		 *  A filename has been specified for the standard
		 *  error of the request.  It may be necessary
		 *  to prefix this name with the current working
		 *  directory.
		 */
		i = 0;				/* Clear prepend flag */
		if (cmdflags & FLAG_E) {
			/*
			 *  -e was specified on the command line.
			 */
			if ((cmdflags2 & FLAG2_EM) == 0) {
				/*
				 *  The current working directory must be
				 *  prepended to the specified standard
				 *  error file name.
				 */
				i = 1;
			}
		}
		else {
			/*
			 *  -e was specified in the script file.
			 */
			if ((scriptflags2 & FLAG2_EM) == 0) {
				/*
				 *  The current working directory must be
				 *  prepended to the specified standard
				 *  error file name.
				 */
				i = 1;
			}
		}
		if (i) {
			/*
			 *  The current working directory path
			 *  must be prepended to the specified
			 *  relative file name.
			 */
			if (strlen (workdir) + 1 +
			    strlen (rawreq.v.bat.stderr_name) > MAX_REQPATH) {
				/*
				 *  File name would be too long. Exit.
				 */
				exceeds ("Stderr pathname", MAX_REQPATH, 1);
			}
			sprintf (path, "%s/%s", workdir,
				 rawreq.v.bat.stderr_name);
			strcpy (rawreq.v.bat.stderr_name, path);
		}
	}
	/*
	 *  If no standard output file has been specified, AND OMD_M_KEEP
	 *  is not set, then it is necessary to store the current working
	 *  directory in the stdout name slot, and to also make sure that
	 *  the manufactured default name will fit.
	 */
	if (rawreq.v.bat.stdout_name [0] == '\0' &&
	   (rawreq.v.bat.stdout_acc & OMD_M_KEEP) == 0) {
		/*
		 *  No stdout pathname was specified.
		 */
		if (strlen (workdir) + 15 > MAX_REQPATH) {
			/*
			 * Unable to form default standard output file name
			 * because the pathname would be too long. Exit.
			 */
			exceeds ("Default stdout pathname", MAX_REQPATH, 1);
		}
		/*
		 *  Copy in the current working directory
		 *  FOLLOWED BY A SLASH!!!!!
		 */
		strcpy (rawreq.v.bat.stdout_name, workdir);
		strcat (rawreq.v.bat.stdout_name, "/");
	}
	else if (rawreq.v.bat.stdout_name [0] != '\0' &&
		 rawreq.v.bat.stdout_name [0] != '/' &&
		(rawreq.v.bat.stdout_acc & OMD_M_KEEP) == 0) {
		/*
		 *  A filename has been specified for the standard
		 *  output file of the request.  It may be necessary
		 *  to prefix this name with the current working
		 *  directory.
		 */
		i = 0;				/* Clear prepend flag */
		if (cmdflags & FLAG_O) {
			/*
			 *  -o on the command line.
			 */
			if ((cmdflags2 & FLAG2_OM) == 0) {
				/*
				 *  The current working directory must be
				 *  prepended to the specified standard
				 *  output file name.
				 */
				i = 1;
			}
		}
		else {
			/*
			 *  -o in the script file.
			 */
			if ((scriptflags2 & FLAG2_OM) == 0) {
				/*
				 *  The current working directory must be
				 *  prepended to the specified standard
				 *  output file name.
				 */
				i = 1;
			}
		}
		if (i) {
			/*
			 * The current working directory path must be
			 * prepended to the specified relative file name.
			 */
			if (strlen (workdir) + 1 +
			    strlen (rawreq.v.bat.stdout_name) > MAX_REQPATH) {
				/*
				 *  File name would be too long. Exit.
				 */
				exceeds ("Stdout pathname", MAX_REQPATH, 1);
			}
			sprintf (path, "%s/%s", workdir,
				 rawreq.v.bat.stdout_name);
			strcpy (rawreq.v.bat.stdout_name, path);
		}
	}
	/*
	 *  Try to check for both stderr and stdout specified as
	 *  same name.  Our test here unfortunately does not test
	 *  ALL cases.
	 */
	if (GETACCESS(rawreq.v.bat.stderr_acc) != OMD_EO &&
	    rawreq.v.bat.stderr_mid == rawreq.v.bat.stdout_mid &&
	  ((rawreq.v.bat.stderr_acc & OMD_M_KEEP) ==
	   (rawreq.v.bat.stdout_acc & OMD_M_KEEP)) &&
	    rawreq.v.bat.stderr_name [0] != '\0') {
		/*
		 *  See if the names match.
		 */
		if (!strcmp (rawreq.v.bat.stdout_name,
			     rawreq.v.bat.stderr_name) &&
		    rawreq.v.bat.stdout_name
			[strlen (rawreq.v.bat.stdout_name) - 1] != '/' &&
		    rawreq.v.bat.stderr_name
			[strlen (rawreq.v.bat.stderr_name) - 1] != '/') {
			/*
			 *  Same destination specified for both stderr
			 *  and stdout.
			 */
			fprintf (stderr, "Stdout and stderr file ");
			fprintf (stderr,
				 "specifications refer to same file.\n");
			fprintf (stderr, "Use -eo if this is intended.\n");
			notqueued();			/* Exit */
		}
	}
	if (rawreq.reqname [0] == '\0') {
		/*
		 *  No reqname was assigned.  We default to the name of
		 *  the shell script file, prepended with a 'R' if
		 *  necessary to enforce the requirement of all reqnames
		 *  beginning with a non-digit.
		 */
#if	BSD42 | BSD43 | ULTRIX
		if ((cp = rindex (scriptname, '/')) == (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
		if ((cp = strrchr (scriptname, '/')) == (char *) 0) {
#else
BAD SYSTEM TYPE
#endif
#endif
			cp = scriptname;
		}
		else cp++;
		if (*cp >= '0' && *cp <= '9') {
			rawreq.reqname [0] = 'R';
			strncpy (rawreq.reqname+1, cp, MAX_REQNAME-1);
		}
		else strncpy (rawreq.reqname, cp, MAX_REQNAME);
		rawreq.reqname [MAX_REQNAME] = '\0';
	}
	/*
	 *  Write the request file header.
	 */
	if (writereq (fileno (ctrlfile), &rawreq) != 0) {
		/*
		 *  Error writing out request control file header.
		 */
		fprintf (stderr, "Unable to write control file header.\n");
		fprintf (stderr, "Reason: %s.\n", sys_errlist [errno]);
		seekhelp();		/* Exit */
	}
	/*
	 *  Write-out stage-in file/hierarchy names into the control file.
	 */
	fseek (ctrlfile, lseek (fileno (ctrlfile), 0L, 1), 0);
	for (i = 0; i < rawreq.v.bat.instacount; i++) {
		fprintf (ctrlfile, "I%s\n", istafile [i]);
		if (ferror (ctrlfile)) {
			fprintf (stderr, "Unable to write stage-in event ");
			fprintf (stderr, "specification to control file.\n");
			seekhelp();	/* Exit */
		}
	}
	/*
	 *  Write-out stage-out file/hierarchy names into the control file.
	 */
	for (i = 0; i < rawreq.v.bat.oustacount; i++) {
		fprintf (ctrlfile, "O%s\n", ostafile [i]);
		if (ferror (ctrlfile)) {
			fprintf (stderr, "Unable to write stage-out event ");
			fprintf (stderr, "specification to control file.\n");
			seekhelp();	/* Exit */
		}
	}
	/*
	 *  Save the current working directory.
	 */
	fprintf (ctrlfile, "D%s\n", workdir);	/* Save working directory */
	if (ferror (ctrlfile)) {
		fprintf (stderr,
			 "Unable to store current working directory ");
		fprintf (stderr, "in control file.\n");
		fprintf (stderr, "Reason: %s.\n", sys_errlist [errno]);
		seekhelp();		/* Exit */
	}
	if ((cp = getenv ("HOME")) != NULL) {
		fprintf (ctrlfile, "E%s=%s\n", "QSUB_HOME", cp);
	}
	if ((cp = getenv ("SHELL")) != NULL) {
		fprintf (ctrlfile, "E%s=%s\n", "QSUB_SHELL", cp);
	}
	if ((cp = getenv ("PATH")) != NULL) {
		fprintf (ctrlfile, "E%s=%s\n", "QSUB_PATH", cp);
	}
#if	SGI | SYS52 | UTS | UNICOS | OSF
	if ((cp = getenv ("LOGNAME")) != NULL) {
		fprintf (ctrlfile, "E%s=%s\n", "QSUB_LOGNAME", cp);
	}
#else
#if	BSD42 | BSD43 | ULTRIX
	if ((cp = getenv ("USER")) != NULL) {
		fprintf (ctrlfile, "E%s=%s\n", "QSUB_USER", cp);
	}
#else
BAD SYSTEM TYPE
#endif
#endif
	if ((cp = getenv ("MAIL")) != NULL) {
		fprintf (ctrlfile, "E%s=%s\n", "QSUB_MAIL", cp);
	}
	if ((cp = getenv ("TZ")) != NULL) {
		fprintf (ctrlfile, "E%s=%s\n", "QSUB_TZ", cp);
	}
#ifdef SDSC
	
	/* check the acount here. account = rawreq.cube_account, 
	 * uid = rawreq.orig_uid */ 

	if (rawreq.cube_account == 0) { 
	    if ((cp = getenv ("ACCOUNT")) == NULL) { 
		/* account not specified. Get default account */ 
#ifdef NXACCT
		rawreq.cube_account = (int) nx_getacctid ();
#else
		if ((passwd = getpwuid (rawreq.orig_uid)) == NULL) 
		    rawreq.cube_account = 0; 
		rawreq.cube_account = passwd->pw_gid; 
#endif
		fprintf (ctrlfile, "E%s=%d\n", "ACCOUNT", rawreq.cube_account);
	    } else { 

		/* Now, check cube_account */ 

		/* is cube_account a integer ? */ 

		bp = cp; 
		while ((c = (int) *bp) != '\0') { 
		    if (isdigit (c) == 0) 
			break; 
		    bp++; 
		}
        	if (c != '\0') {    /* not a digit */
#ifdef NXACCT
            	    if ((nxacct = nx_getanam (cp)) == NULL)
                	bad_account ();
        	    rawreq.cube_account = nxacct->acct_id;
#else
            	    if ((grp = getgrnam (cp)) == NULL)
                	bad_account ();
        	    rawreq.cube_account = grp->gr_gid;
#endif
        	} else {	/* it is a digit */
		    if (*cp != '\0') {
        	        rawreq.cube_account = atoi (cp);
		    }
		}
		if (!export)
		    fprintf (ctrlfile, 
		    "E%s=%d\n", "ACCOUNT", rawreq.cube_account);
	    }
	}
#ifdef DEBUG
	printf ("Account = %d\n", rawreq.cube_account);
	fflush (stdout);
#endif
	/* We should check with resd here to see if the account is 
	 * valid.
	 */

	if (MACS_FLAG != MACS_OFF) {
	  if (resd_open >= 0) { 
	    if (my_runok (rawreq.cube_account, rawreq.orig_uid,
	    rawreq.v.bat.prncpus) < 0)
	        bad_account ();
	  }
	} 
#endif
	if (export) {				/* -x was specified */
		/*
		 *  Export more environment variables than just the basic set.
		 */
		while (*envp != NULL) {
#if	BSD42 | BSD43 | ULTRIX
			if ((cp = index (*envp, '\n')) != (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
			if ((cp = strchr (*envp, '\n')) != (char *) 0) {
#else
BAD SYSTEM TYPE
#endif
#endif
				*cp = '\0';	/* Trailing newlines removed */
			}
			if (!collision (*envp, reserved) &&
			    !collision (*envp, intercept)) {
				/*
				 *  Save the environment variable away.
				 */
				fprintf (ctrlfile, "E%s\n", *envp);
				if (ferror (ctrlfile)) enverror();  /* Exit */
			}
			envp++;			/* Get next environment var */
		}
	}
	fflush (ctrlfile);
	if (ferror (ctrlfile)) enverror();	/* Exit */
	/*
	 *  Now, queue the request (the control file must still be open
	 *  so that we can find out the req sequence number assigned
	 *  to this req).
	 */
	signal (SIGHUP, SIG_IGN);	/* Do NOT let a signal cause an exit */
	signal (SIGINT, SIG_IGN);	/* past this point! */
	signal (SIGQUIT, SIG_IGN);
	signal (SIGTERM, SIG_IGN);
	if ((reqseq = quereq (&transaction_code)) >= 0) {
#if	UNICOS
		if (scpflag) {
			scpsub.qsubpid = getpid();
			scpsub.seqno = reqseq;
			strcpy (scpsub.jobname, rawreq.reqname);
			write (scpflag, (char *) &scpsub, sizeof (scpsub));
		} else
#endif
		if (!silent) {
			if (gethostname (hostname, MAX_MACHINENAME) == -1) {
				hostname [0] = '\0';
			}
			else hostname [MAX_MACHINENAME] = '\0';
					/* Make sure of trailing '\0' */
			printf ("Request %1ld.%s submitted to queue: %s.\n",
				reqseq, hostname, rawreq.quename);
			if (transaction_code & XCI_INFORM_MASK) {
				/*
				 *  Display quota binding messages.
				 */
				analyzetcm (transaction_code, stderr, "");
			}
		}
		exit (0);		/* Request successfully queued */
	}
	/*
	 *  The request was not successfully queued.
	 */
#ifdef SDSC
	/* This is a hack due to difficulty creating new error code in NQS */

	if (transaction_code == TCML_INSQUESPA && rawreq.start_time > 0) {
	    printf ("Problem getting the requested time slot.\n");
	    printf ("There are probably insufficient free nodes, or\n");
	    printf ("the specified time slot is not at 30 min boundary, or\n");
	}
#endif
	analyzetcm (transaction_code, stderr, "");
	exit (1);			/* Request not queued */
}


/*** bad_syntax
 *
 *
 *	void bad_syntax():
 *	Display message and exit.
 */
static void bad_syntax (flag, cmdline)
char *flag;				/* Which flag */
int cmdline;				/* Boolean */
{
	fprintf (stderr, "Invalid syntax following -%s flag.\n", flag);
	if (!cmdline) showline();
	notqueued();			/* Exit */
}


/*** bad_value
 *
 *
 *	void bad_value ():
 *	Display message and exit.
 */
static void bad_value (flag, cmdline)
char *flag;				/* Which flag */
int cmdline;				/* Boolean */
{
	fprintf (stderr, "Invalid value following -%s flag.\n", flag);
	if (!cmdline) showline();
	notqueued();			/* Exit */
}

#ifdef SDSC
/*** bad_account
 *
 *
 *	void bad_account ():
 *	Display message and exit.
 */
static void bad_account ()
{
	fprintf (stderr, "Invalid account or account out of allocation.\n");
	notqueued();			/* Exit */
}
#endif


/*** cleanup
 *
 *
 *	void cleanup():
 *
 *	Upon receipt of certain signals, we are not to complete the
 *	task of submitting a request to the NQS system for execution.
 */
static void cleanup (sig)
int sig;			/* Signal received */
{
	signal (sig, SIG_IGN);	/* Ignore multiple signals */
	zapreq();		/* Unlink all files in the NQS new request */
				/* staging directory before exiting. */
	exit (1);		/* Request not submitted. */
}


/*** collision
 *
 *
 *	int collision():
 *
 *	Returns non-zero, if the character string specified by the first
 *	argument terminating with an "=" character matches any of the
 *	character strings specified in the collision set.  Otherwise,
 *	zero is returned.
 */
static int collision (target, collideset)
char *target;				/* The target string */
char *collideset[];			/* Set of collision strings */
{
	register int length;
	register char *cp;

#if	BSD42 | BSD43 | ULTRIX
	if ((cp = index (target, '=')) == (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	if ((cp = strchr (target, '=')) == (char *) 0) {
#else
BAD SYSTEM TYPE
#endif
#endif
		length = strlen (target);
	}
	else length = cp - target;
	while (*collideset != NULL) {
		if (strncmp (target, *collideset++, length) == 0) {
			return (1);	/* collision */
		}
	}
	return (0);			/* No collision */
}

/*** docpu
 *
 *
 *	char **docpu():
 *	Finish processing an argument.
 *	Returns a pointer (char**) to the next argument to be processed.
 */
static char **
docpu(argv, argument, cmdline, string, flag, seenbit, enfbit, where, maxonly)
char *argv[];				/* Ptr to remaining args */
char *argument;				/* First of remaining args */
int cmdline;				/* Boolean */
char *string;				/* For error msgs */
char *flag;				/* For error msgs */
long seenbit;				/* To mark "seen" in bit mask */
long enfbit;				/* To mark "enforce" in bit mask */
struct cpulimit *where;			/* To put into rawreq */
int maxonly;				/* Do not accept warning limit */
{
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    if (*argv == NULL) {
		/*
		 *  Value after flag is missing. Exit.
		 */
		no_value (string, flag, cmdline);
	    }
	    if (cmdline) {
		if (cmdflags & seenbit) {
		    /*
		     *  This flag was seen before on the command line. Exit.
		     */
		    twice (flag, 1);
		}
	        cmdflags |= seenbit;	/* Set seen flag */
	    }
	    else {
		if (scriptflags & seenbit) {
		    /*
		     *  This flag was seen before in the script. Exit.
		     */
		    twice (flag, 0);
		}
		scriptflags |= seenbit;	/* Set seen flag */
		if (cmdflags & seenbit) return (++argv); /* Already seen */
	    }
	    if (**argv == 'u') {
		rawreq.v.bat.explicit |= enfbit;
		rawreq.v.bat.infinite |= enfbit;
	    }
	    else {
		switch (scancpulim (*argv, where, maxonly)) {
		case 0:
		    rawreq.v.bat.explicit |= enfbit;
		    rawreq.v.bat.infinite &= ~enfbit;
		    break;
		case -1:
		case -3:			/* Missing cpu limit */
		    bad_syntax (flag, cmdline);	/* Exit. */
		case -2:			/* Overflow/semantics */
		case -4:			/* Warning > maximum */
		    bad_value (flag, cmdline);	/* Exit */
		}
	    }
	    return (++argv);		/* Success */
}

#ifdef SDSC
/*** doncpus
 *
 *
 *	char **doncpus():
 *	Finish processing an argument.
 *	Returns a pointer (char**) to the next argument to be processed.
 */
static char **
doncpus(argv, argument, cmdline, string, flag, seenbit, enfbit, where)
char *argv[];				/* Ptr to remaining args */
char *argument;				/* First of remaining args */
int cmdline;				/* Boolean */
char *string;				/* For error msgs */
char *flag;				/* For error msgs */
long seenbit;				/* To mark "seen" in bit mask */
long enfbit;				/* To mark "enforce" in bit mask */
short *where;				/* To put into rawreq */
{
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    if (*argv == NULL) {
		/*
		 *  Value after flag is missing. Exit.
		 */
		no_value (string, flag, cmdline);
	    }
	    if (cmdline) {
		if (cmdflags & seenbit) {
		    /*
		     *  This flag was seen before on the command line. Exit.
		     */
		    twice (flag, 1);
		}
	        cmdflags |= seenbit;	/* Set seen flag */
	    }
	    else {
		if (scriptflags & seenbit) {
		    /*
		     *  This flag was seen before in the script. Exit.
		     */
		    twice (flag, 0);
		}
		scriptflags |= seenbit;	/* Set seen flag */
		if (cmdflags & seenbit) return (++argv); /* Already seen */
	    }
	    *where = atoi (*argv);
	    if (*where > MAX_NCPUS || *where < MIN_NCPUS) {
		/*
		 *  Request ncpus value out of bounds
		 */
		fprintf (stderr, "-lP ncpus value out of bounds: [");
		fprintf (stderr, "%1d..%1d].\n", MIN_NCPUS, MAX_NCPUS);
		if (!cmdline) showline();
		notqueued();	/* Exit */
	    }
	    rawreq.v.bat.explicit |= enfbit;
	    rawreq.v.bat.infinite &= ~enfbit;
	    return (++argv);		/* Success */
}
#endif

/*** donice
 *
 *
 *	char **donice():
 *	Finish processing an argument.
 *	Returns a pointer (char**) to the next argument to be processed.
 */
static char **
donice(argv, argument, cmdline, string, flag, seenbit, enfbit, where)
char *argv[];				/* Ptr to remaining args */
char *argument;				/* First of remaining args */
int cmdline;				/* Boolean */
char *string;				/* For error msgs */
char *flag;				/* For error msgs */
long seenbit;				/* To mark "seen" in bit mask */
long enfbit;				/* To mark "enforce" in bit mask */
short *where;				/* To put into rawreq */
{
	    int negnice;
	    int i;
	    
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    if (*argv == NULL) {
		/*
		 *  Missing nice value. Exit.
		 */
		no_value (string, flag, cmdline);
	    }
	    if (cmdline) {
		if (cmdflags & seenbit) {
		    /*
		     *  We've seen this flag before on the command line. Exit.
		     */
		    twice (flag, 1);
		}
	        cmdflags |= seenbit;	/* Set seen flag */
	    }
	    else {
		if (scriptflags & seenbit) {
		    /*
		     *  We've seen this flag before in the script file. Exit.
		     */
		    twice (flag, 0);
		}
		scriptflags |= seenbit;	/* Set seen flag */
	        if (cmdflags & seenbit) return (++argv); /* Already seen */
	    }
	    argument = *argv;		/* Get value string */
	    negnice = 0;
	    if (*argument == '-') {
		negnice = 1;
		argument++;
	    }
	    i = strlen (argument);	/* Get length of digit string */
	    if (isdecstr (argument, i)) {
		/*
		 *  Be careful about overflow.
		 */
		while (*argument=='0'){	/* Scan leading zeroes. */
		    argument++;		/* Next character */
		    i--;		/* Less digits */
		}
		if (*argument == '\0') {	/* All zeros. */
		    argument--;		/* Backup */
		    i++;		/* One digit */
		}
		if (negnice == 0 && i <= 4)
		    *where = atoi (argument);
		if (negnice == 1 && i <= 4)
		    *where = -( atoi (argument));
		if (i > 4 || *where > MAX_REQNICE || *where < MIN_REQNICE) {
		    /*
		     *  Request nice value out of bounds
		     */
		    fprintf (stderr, "-ln nice value out of bounds: [");
		    fprintf (stderr, "%1d..%1d].\n", MIN_REQNICE, MAX_REQNICE);
		    if (!cmdline) showline();
		    notqueued();	/* Exit */
		}
	    }
	    else {
		/*
		 *  Nice value was not a decimal digit string.
		 */
		fprintf (stderr, "-ln nice value ");
		fprintf (stderr, "is not a decimal digit string.\n");
		if (!cmdline) showline();
		notqueued();		/* Exit */
	    }
	    rawreq.v.bat.explicit |= enfbit;
	    rawreq.v.bat.infinite &= ~enfbit;
	    return (++argv);
}

/*** doquota
 *
 *
 *	char **doquota():
 *	Finish processing an argument.
 *	Returns a pointer (char**) to the next argument to be processed.
 */
static char **
doquota(argv, argument, cmdline, string, flag, seenbit, enfbit, where, maxonly)
char *argv[];				/* Ptr to remaining args */
char *argument;				/* First of remaining args */
int cmdline;				/* Boolean */
char *string;				/* For error msgs */
char *flag;				/* For error msgs */
long seenbit;				/* To mark "seen" in bit mask */
long enfbit;				/* To mark "enforce" in bit mask */
struct quotalimit *where;		/* To put into rawreq */
int maxonly;				/* Do not accept warning limit */
{
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    if (*argv == NULL) {
		/*
		 *  Value after flag is missing. Exit.
		 */
		no_value (string, flag, cmdline);
	    }
	    if (cmdline) {
		if (cmdflags & seenbit) {
		    /*
		     *  This flag was seen before on the command line. Exit.
		     */
		    twice (flag, 1);
		}
	        cmdflags |= seenbit;	/* Set seen flag */
	    }
	    else {
		if (scriptflags & seenbit) {
		    /*
		     *  This flag was seen before in the script. Exit.
		     */
		    twice (flag, 0);
		}
		scriptflags |= seenbit;	/* Set seen flag */
		if (cmdflags & seenbit) return (++argv); /* Already seen */
	    }
	    if (**argv == 'u') {
		rawreq.v.bat.explicit |= enfbit;
		rawreq.v.bat.infinite |= enfbit;
	    }
	    else {
		switch (scanquolim (*argv, where, maxonly)) {
		case 0:
		    rawreq.v.bat.explicit |= enfbit;
		    rawreq.v.bat.infinite &= ~enfbit;
		    break;
		case -1:
		case -3:			/* Missing quota limit */
		    bad_syntax (flag, cmdline);	/* Exit. */
		case -2:			/* Overflow/underflow */
		case -4:			/* Warning > maximum */
		    bad_value (flag, cmdline);	/* Exit */
		}
	    }
	    return (++argv);		/* Success */
}


/*** enverror
 *
 *
 *	void enverror():
 *	Display message and exit.
 */
static void enverror()
{
	fprintf (stderr,
		 "Unable to write environment variable to control file.\n");
	fprintf (stderr, "Reason: %s.\n", sys_errlist [errno]);
	seekhelp();			/* Exit */
}


/*** exceeds
 *
 *
 *	void exceeds():
 *	Display message and exit.
 */
static void exceeds (string, chars, cmdline)
char *string;				/* What was exceeded */
int chars;				/* Legal max characters */
int cmdline;				/* Boolean */
{
	fprintf (stderr, "%s exceeds %d characters.\n", string, chars);
	if (!cmdline) showline();
	notqueued();			/* Exit */
}


/*** flag_conflict
 *
 *
 *	void flag_conflict():
 *	Display message and exit.
 */
static void flag_conflict (flag1, flag2, cmdline)
char *flag1;				/* First flag */
char *flag2;				/* Second flag */
int cmdline;				/* Boolean */
{
	fprintf (stderr, "Conflict between -%s and -%s flags.\n", flag1, flag2);
	if (!cmdline) showline();
	notqueued();			/* Exit */
}


/*** getscript
 *
 *
 *	int getscript (buffer)
 *
 *	Read the next line of the shell script file into the specified
 *	buffer.  This function traps all line-too-long errors and shell
 *	script file read errors.
 *
 *	Returns:
 *		The number of characters in the next line as read from
 *		the shell script file.  The newline character delimiting
 *		the line is NOT returned as part of the string, but is
 *		instead replaced with a '\0' character.
 *
 *		Upon reaching the end of the file, -1 is returned.
 */
static int getscript (buffer)
register char buffer [MAX_REQPATH+2];	/* Line buffer */
{
	register int charcount;		/* Number of characters copied to */
					/* line buffer so far. */
	register char *ch;		/* Ptr to next buffer character */
	register int fd;		/* File descriptor for script file */

	if (scripteof) return (-1);	/* End of file */
	charcount = 0;			/* Number of bytes placed in buffer */
	scriptline++;			/* Update shell script line number */
	ch = scriptbuffer + scriptbytes - scriptleft;
	fd = fileno (scriptfile);
	for (;;) {
		if (scriptleft-- == 0) {
			/*
			 *  No more bytes left in the shell script buffer.
			 */
			if ((scriptbytes = read (fd, scriptbuffer,
						 COPYBUFSIZE)) == -1) {
				spoolerror();	/* Display message and exit */
			}			/* errno has error code */
			scriptleft = scriptbytes;
			if (scriptbytes == 0) {
				/*
				 *  Zero bytes were read (meaning we reached
				 *  the end of the file).
				 */
				*buffer = '\0';	/* Append null */
				scripteof = 1;	/* Set EOF flag */
				if (charcount) return (charcount);
				return (-1);	/* No chars at all */
			}
			ch = scriptbuffer;	/* Next character */
			scriptleft--;		/* # remaining in buffer */
		}
		if (*ch == '\n') {
			/*
			 *  A newline character marking the end of a shell
			 *  script line has been located.
			 */
			*buffer = '\0';		/* Null terminate buffer */
			return (charcount);	/* # of bytes in buf */
		}
		if (++charcount >= MAX_REQPATH+2) {
			/*
			 *  This line of the shell script file is too long
			 *  for NQS to handle.  Exit.
			 */
			exceeds ("Line length", MAX_REQPATH + 2, 0);
		}
		*buffer++ = *ch++;		/* Store the character */
	}
}
		

/*** insmemstage
 *
 *
 *	void insmemstage():
 *	Display message and exit.
 */
static void insmemstage()
{
	fprintf (stderr, "Insufficient heap space to save staging event.\n");
	notqueued();			/* Exit */
}


/*** invalidflag
 *
 *
 *	void invalidflag():
 *	Display message.  Exit if called while processing command line.
 */
static void invalidflag (cmdline, argument)
int cmdline;
char *argument;
{
	fprintf (stderr, "Invalid argument flag: %s.\n", argument);
	if (!cmdline) showline();
	notqueued();			/* Exit */
}


/*** notqueued
 *
 *
 *	void notqueued():
 *
 *	Unlink all files associated with the request,
 *	show a message, and exit(1).
 */
static void notqueued()
{
	zapreq();			/* Unlink any request files */
	fprintf (stderr, "Request not queued.\n");
	exit (1);
}


/*** no_value
 *
 *
 *	void no_value():
 *
 *	Display a message and exit(1).
 */
static void no_value (string, flag, cmdline)
char *string;
char *flag;
int cmdline;
{
	fprintf (stderr, "%s value missing after -%s flag.\n", string, flag);
	if (!cmdline) showline();
	notqueued ();			/* Exit */
	
}


/*** procarg
 *
 *
 *	char **procarg():
 *	Process an argument.
 *
 *	Returns:
 *		The updated argument pointer to point at the next
 *		argument to be processed.
 */
static char **procarg (argv, cmdline)
register char *argv[];			/* Ptr to remaining arguments */
int cmdline;				/* Non-zero if processing command */
					/* line arguments */
{
    register char *argument;		/* Ptr to argument text */
    register int i;			/* Index and length var */
    register char *cp;			/* Character pointer */
#ifdef SDSC
        char *bp;
        int c;
#ifdef NXACCT
        struct nxacct *nxacct;
#else
        struct group *grp;
#endif
#endif
    /*
     *  The following declaration will need to be added when file
     *  staging is supported in NQS.
     *
     *	    register char *newmem;	(* Ptr to new mem by malloc() *)
     */

    argument = *argv++;			/* Get argument */
    if (*argument != '-') {
	/*
	 *  The argument does not begin with a '-'.
	 */
	if (cmdline) {
		/*
		 * We are parsing command line arguments. This argument
		 * is the name of the shell script to be executed.
		 */
		if (scriptname != NULL) {
		    /*
		     * A shell script was already specified.
		     */
		    fprintf (stderr, "Multiple shell scripts specified.\n");
		    notqueued();		/* Exit */
		}
		scriptname = argument;	/* Save shell script name */
	}
	else {
		/*
		 *  We are parsing embedded default flags within
		 *  the shell script file.  All embedded flags
		 *  must be introduced with a '-' character.
		 */
		fprintf (stderr, "A '-' was expected to precede the ");
		fprintf (stderr, "embedded default flag: %s.\n", argument);
		showline();
		notqueued();
	}
	return (argv);			/* Return updated argv */
    }
    /*
     *  Otherwise, the flag discovered has a preceding "-".
     */
    switch (argument [1]) {
#if	UNICOS
    case '*':
	scpflag = atoi (&argument [2]);
	return (argv);			/* Return updated argv */
#endif
    case 'a':				/* Run request after time */
	if (argument [2] != '\0') {
	    invalidflag (cmdline, argument);
	    return (argv);
	}
	if (*argv == NULL) {
	    /*
	     *  Missing start-after time value. Exit.
	     */
	    no_value ("Start-time", "a", cmdline);
	}
	if (cmdline) {
	    if (cmdflags & FLAG_A) {
		/*
		 *  A start-after time was previously specified. Exit.
		 */
		twice ("a", 1);
	    }
	    cmdflags |= FLAG_A;		/* Set seen flag */
	}
	else {
	    if (scriptflags & FLAG_A) {
		/*
		 *  Multiple start-after times specified in script file. Exit.
		 */
		twice ("a", 0);
	    }
	    scriptflags |= FLAG_A;	/* Set seen flag */
	    if (cmdflags & FLAG_A) return (++argv);	/* Already seen */
	}
	switch (scnftime (*argv, &rawreq.start_time)) {
	case 0:
		break;
	case -1:
	    bad_syntax ("a", cmdline); /* Exit */
	case -2:
	case -3:
	    bad_value ("a", cmdline);	/* Exit */
	}
	return (++argv);		/* Success */
#ifdef SDSC
    case 'c':                           /* account */
        if (argument [2] != '\0') {
            invalidflag (cmdline, argument);
            return (argv);
        }
        if (*argv == NULL) {
            /*
             *  Missing account name. Exit.
             */
            no_value ("Cube_account", "c", cmdline);
        }
        if (strlen (*argv) > MAX_ACCNAME) {
            /*
             *  Account name is too long.  Exit.
             */
            exceeds ("-c cube_account name", MAX_ACCNAME, cmdline);
        }
        if (cmdline) {
            if (cmdflags & FLAG_C) {
                /*
                 *  A cube_account was previously specified. Exit.
                 */
                twice ("c", 1);
            }
            cmdflags |= FLAG_C;         /* Set seen flag */
        }
        else {
           if (scriptflags & FLAG_C) {
                /*
                 *  Multiple cube_account specified in script file. Exit.
                 */
                twice ("c", 0);
            }
            scriptflags |= FLAG_C;      /* Set seen flag */
            if (cmdflags & FLAG_C) return (++argv); /* Already seen */
        }
        /* Now, check cube_account */

        /* is cube_account a integer ? */

        bp = *argv;
        while ((c = (int) *bp) != '\0') {
            if (isdigit (c) == 0)
                break;
            bp++;
        }
#ifdef NXACCT
        if (c != '\0') {    /* not a digit */
            if ((nxacct = nx_getanam (*argv)) == NULL)
                bad_account ();
            rawreq.cube_account = nxacct->acct_id;
        } else {	/* it is a digit */
            rawreq.cube_account = atoi (*argv);
	}
#else
        if (c != '\0') {    /* not a digit */
            if ((grp = getgrnam (*argv)) == NULL)
                bad_account ();
            rawreq.cube_account = grp->gr_gid;
        } else {	/* it is a digit */
            rawreq.cube_account = atoi (*argv);
            if ((grp = getgrgid (rawreq.cube_account)) == NULL)
                bad_account ();
	}
#endif
        return (++argv);
#endif
    case 'e':				/* -e or -eo */
	if (argument [2] == 'o') {
	    /*
	     *  -eo has been specified.
	     */
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    if (cmdline) {
		if (cmdflags & FLAG_E) flag_conflict ("e", "eo", 1);
		cmdflags |= FLAG_EO;
	    }
	    else {
		if (scriptflags & FLAG_E) flag_conflict ("e", "eo", 0);
		scriptflags |= FLAG_EO;
		if (cmdflags & FLAG_E || cmdflags & FLAG_EO) {
		    return (argv);
		}
	    }
	    SETACCESS(rawreq.v.bat.stderr_acc, OMD_EO);
	    return (argv);		/* Success */
	}
	if (argument [2] != '\0') {
	    invalidflag (cmdline, argument);
	    return (argv);
	}
	if (*argv == NULL) {
	    /*
	     *  Missing stderr file name. Exit.
	     */
	    no_value ("Stderr file name", "e", cmdline);
	}
	if (cmdline) {
	    if (cmdflags & FLAG_E) {
		/*
		 *  A stderr file flag was previously specified. Exit.
		 */
		twice ("e", 1);
	    }
	    if (cmdflags & FLAG_EO) flag_conflict ("e", "eo", 1);
	    cmdflags |= FLAG_E;		/* Set seen flag */
	}
	else {
	    if (scriptflags & FLAG_E) {
		/*
		 *  Multiple stderr file flags specified in script file. Exit.
		 */
		twice ("e", 0);
	    }
	    if (scriptflags & FLAG_EO) flag_conflict ("e", "eo", 0);
	    scriptflags |= FLAG_E;	/* Set seen flag */
	    if (cmdflags & FLAG_E) return (++argv);	/* Already seen */
	}
	argument = *argv;		/* Get value string */
	switch (machpath (argument, &rawreq.v.bat.stderr_mid)) {
	case 1:				/* Explicit machine-id */
	    if (cmdline) {
		if (cmdflags & FLAG_KE) flag_conflict ("e", "ke", 1);
		cmdflags2 |= FLAG2_EM;
	    }
	    else {
		if (scriptflags & FLAG_KE) flag_conflict ("e", "ke", 0);
		scriptflags2 |= FLAG2_EM;
	    }
	    break;
	case 0:				/* Implicit machine-id */
	    break;
	case -1:			/* Null machine-name */
	    fprintf (stderr, "Null -e stderr machine name.\n");
	    if (!cmdline) showline();
	    notqueued();		/* Exit */
	case -2:
	    fprintf (stderr, "-e stderr machine name is not ");
	    fprintf (stderr, "known to local host.\n");
	    if (!cmdline) showline();
	    notqueued();		/* Exit */
	case -3:
	case -4:
	    fprintf (stderr, "Unable to determine machine-id for -e stderr ");
	    fprintf (stderr, "machine name.\n");
	    if (!cmdline) showline();
	    seekhelp();			/* Seek staff support */
	}
	cp = destpath (argument);	/* get pathname relative */
					/* to any machine-name */
	i = strlen (cp);
	if (i > MAX_REQPATH) {
	    /*
	     *  Show Stderr pathname too long error message and exit(1).
	     */
	    exceeds ("-e stderr pathname", MAX_REQPATH, cmdline);
	}
	if (i == 0 || cp [i-1] == '/') {
	    /*
	     *  Show invalid name given for Stderr file message and exit(1).
	     */
	    bad_value ("e", cmdline);
	}
	if (scpflag) return (++argv);
	strcpy (rawreq.v.bat.stderr_name, cp);
	return (++argv);
    case 'k':				/* -ke or -ko */
	if (argument [2] == 'e') {
	    /*
	     *  -ke: keep stderr on execution machine
	     */
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    if (cmdline) {
		if (cmdflags2 & FLAG2_EM) flag_conflict ("e", "ke", 1);
		cmdflags |= FLAG_KE;
	    }
	    else {
		if (scriptflags2 & FLAG2_EM) flag_conflict ("e", "ke", 0);
		scriptflags |= FLAG_KE;
		if ((cmdflags & FLAG_KE) || (cmdflags2 & FLAG2_EM)) {
		    return (argv);
		}
	    }
	    if (scpflag) return (argv);
	    rawreq.v.bat.stderr_acc |= OMD_M_KEEP;
	    return (argv);		/* Success */
	}
	else if (argument [2] == 'o') {
	    /*
	     *  -ko: keep stdout on execution machine
	     */
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    if (cmdline) {
		if (cmdflags2 & FLAG2_OM) flag_conflict ("o", "ko", 1);
		cmdflags |= FLAG_KO;
	    }
	    else {
		if (scriptflags2 & FLAG2_OM) flag_conflict ("o", "ko", 0);
		scriptflags |= FLAG_KO;
		if ((cmdflags & FLAG_KO) || (cmdflags2 & FLAG2_OM)) {
		    return (argv);
		}
	    }
	    if (scpflag) return (argv);
	    rawreq.v.bat.stdout_acc |= OMD_M_KEEP;
	    return (argv);		/* Success */
	}
	/*
	 *  Bad flag argument.
	 */
	invalidflag (cmdline, argument);
	return (argv);
    case 'l':		/* -lc, -ld, -lf, -lF, -lm -lM, -ln, -ls, */
			/* -lt, -lT, -lv, -lV, and -lw are meaningful */
#ifdef SDSC
			/* -lP - per_req NCPU, is also meaningful */
#endif
	switch (argument [2]) {
	/*
	 * Core file size limit
	 */
	case 'c':
	    return (doquota (argv, argument, cmdline, "Core file size limit",
		"lc", FLAG_LC, LIM_PPCORE, &rawreq.v.bat.ppcoresize, 1));
	/*
	 * Data segment size limit
	 */
	case 'd':
	    return (doquota (argv, argument, cmdline, "Data segment size limit",
		"ld", FLAG_LD, LIM_PPDATA, &rawreq.v.bat.ppdatasize, 0));
	/*
	 * Per-process permanent file size limit
	 */
	case 'f':
	    return (doquota (argv, argument, cmdline,
		"Per-process permanent file size limit",
		"lf", FLAG_LFP, LIM_PPPFILE, &rawreq.v.bat.pppfilesize, 0));
	/*
	 * Per-request permanent file size limit
	 */
	case 'F':
	    return (doquota (argv, argument, cmdline,
		"Per-request permanent file size limit",
		"lF", FLAG_LFR, LIM_PRPFILE, &rawreq.v.bat.prpfilespace, 0));
	/*
	 * Per-process memory size limit
	 */
	case 'm':
	    return (doquota (argv, argument, cmdline,
		"Per-process memory size limit",
		"lm", FLAG_LMP, LIM_PPMEM, &rawreq.v.bat.ppmemsize, 0));
	/*
	 * Per-request memory size limit
	 */
	case 'M':
	    return (doquota (argv, argument, cmdline,
		"Per-request memory size limit",
		"lM", FLAG_LMR, LIM_PRMEM, &rawreq.v.bat.prmemsize, 0));
	/*
	 * Nice value
	 */
	case 'n':
	    return (donice (argv, argument, cmdline, "Nice value",
		"ln", FLAG_LN, LIM_PPNICE, &rawreq.v.bat.ppnice));
	/*
	 * NCPUS value
	 */
	case 'P':
	    return (doncpus (argv, argument, cmdline, "NCPUS value",
		"lP", FLAG_LPR, LIM_PRNCPUS, &rawreq.v.bat.prncpus));
	/*
	 * Stack segment size limi
	 */
	case 's':
	    return (doquota (argv, argument, cmdline,
		"Stack segment size limit",
		"ls", FLAG_LS, LIM_PPSTACK, &rawreq.v.bat.ppstacksize, 1));
	/*
	 * Per-process cpu time limit
	 */
	case 't':
	    return (docpu (argv, argument, cmdline,
		"Per-process cpu time limit",
		"lt", FLAG_LTP, LIM_PPCPUT, &rawreq.v.bat.ppcputime, 0));
	/*
	 * Per-request cpu time limit
	 */
	case 'T':
	    return (docpu (argv, argument, cmdline,
		"Per-request cpu time limit",
		"lT", FLAG_LTR, LIM_PRCPUT, &rawreq.v.bat.prcputime, 0));
	/*
	 * Per-process temporary file size limit
	 */
	case 'v':
	    return (doquota (argv, argument, cmdline,
		"Per-process temporary file size limit",
		"lv", FLAG_LVP, LIM_PPTFILE, &rawreq.v.bat.pptfilesize, 0));
	/*
	 * Per-request temporary file size limit
	 */
	case 'V':
	    return (doquota (argv, argument, cmdline,
		"Per-request temporary file size limit",
		"lV", FLAG_LVR, LIM_PRTFILE, &rawreq.v.bat.prtfilespace, 0));
	/*
	 * Working set size limit
	 */
	case 'w':
	    return (doquota (argv, argument, cmdline,
		"Working set size limit",
		"lw", FLAG_LW, LIM_PPWORK, &rawreq.v.bat.ppworkset, 1));
	default:
	    invalidflag (cmdline, argument);
	    return (argv);
	}				/* end of switch (argument[2]) */
    case 'm':				/* -mb, -me, -mr, -ms, -mt, and */
	switch (argument [2]) {		/* -mu are meaningful */
	case 'b':	    
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    rawreq.flags |= RQF_BEGINMAIL;
	    return (argv);		/* Mail on request begin; success */
	case 'e':
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    rawreq.flags |= RQF_ENDMAIL;
	    return (argv);		/* Mail on request end; success */
	case 'r':
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    rawreq.flags |= RQF_RESTARTMAIL;
	    return (argv);		/* Mail on request restart; success */
	case 't':
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    rawreq.flags |= RQF_TRANSMAIL;
	    return (argv);		/* Mail on request transport; success */
	case 'u':
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    if (*argv == NULL) {
		/*
		 *  Missing mail account name. Exit.
		 */
		no_value ("Mail account", "mu", cmdline);
	    }
	    if (cmdline) {
		if (cmdflags & FLAG_MU) {
		    /*
		     *  A username mail flag was previously specified. Exit.
		     */
		    twice ("mu", 1);
		}
	        cmdflags |= FLAG_MU;	/* Set seen flag */
	    }
	    else {
		if (scriptflags & FLAG_MU) {
		    /*
		     * Multiple username mail specification given
		     * in the script file. Exit.
		     */
		    twice ("mu", 0);
		}
		scriptflags |= FLAG_MU;	/* Set seen flag */
	        if (cmdflags & FLAG_MU) return (++argv); /* Already seen */
	    }
	    argument = *argv;		/* Get value string */
	    switch (machacct (argument, &rawreq.mail_mid)) {
	    case 0:			/* Successfully got mid */
		break;
	    case -1:			/* Null machine-name */
		fprintf (stderr, "Null -mu mail destination machine-name.\n");
		if (!cmdline) showline();
		notqueued();		/* Exit */
	    case -2:
		fprintf (stderr, "-mu mail destination machine-name is ");
		fprintf (stderr, "unknown to local host.\n");
		if (!cmdline) showline();
		notqueued();		/* Exit */
	    case -3:
	    case -4:
		fprintf (stderr, "Unable to determine machine-id of -mu ");
		fprintf (stderr, "mail destination.\n");
		if (!cmdline) showline();
		seekhelp();		/* Seek staff support */
	    }
	    cp = destacct (argument); /* Get account-name relative to machine */
	    if (strlen (cp) > MAX_ACCOUNTNAME) {
		/*
		 *  Show account-name too long error message.  Exit.
		 */
		exceeds (" -mu account-name", MAX_ACCOUNTNAME, cmdline);
	    }
	    strcpy (rawreq.mail_name, cp);	/* Save account-name */
	    return (++argv);
	default:
	    invalidflag (cmdline, argument); /* -m?, ? != {b,e,u} */
	    return (argv);
	}			/* end of switch (argument[2]) */
    case 'n':			/* -nr and -nc are meaningful */
	if (argument [2] == 'r') {
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    rawreq.flags &= ~RQF_RESTARTABLE;
	    return (argv);		/* Not restartable; return success */
	}
	if (argument [2] == 'c') {
            if (argument [3] != '\0') {
                invalidflag (cmdline, argument);
                return (argv);
            }
            rawreq.flags &= ~RQF_RECOVERABLE;
            return (argv);              /* Not recoverable; return success */
        }
	invalidflag (cmdline, argument);
	return (argv);
    case 'o':				/* Output file specification */
	if (argument [2] != '\0') {
	    invalidflag (cmdline, argument);
	    return (argv);
	}
	if (*argv == NULL) {
	    /*
	     *  Missing stdout file name. Exit.
	     */
	    no_value ("Stdout file name", "o", cmdline);
	}
	if (cmdline) {
	    if (cmdflags & FLAG_O) {
		/*
		 *  A stdout file flag was previously specified. Exit.
		 */
		twice ("o", 1);
	    }
	    cmdflags |= FLAG_O;		/* Set seen flag */
	}
	else {
	    if (scriptflags & FLAG_O) {
		/*
		 *  Multiple stdout file flags specified in script file. Exit.
		 */
		twice ("o", 0);
	    }
	    scriptflags |= FLAG_O;	/* Set seen flag */
	    if (cmdflags & FLAG_O) return (++argv); /* Already seen */
	}
	argument = *argv;		/* Get value string */
	switch (machpath (argument, &rawreq.v.bat.stdout_mid)) {
	case 1:				/* Explicit machine-name */
	    if (cmdline) {
		if (cmdflags & FLAG_KO) flag_conflict ("o", "ko", 1);
		cmdflags2 |= FLAG2_OM;
	    }
	    else {
		if (scriptflags & FLAG_KO) flag_conflict ("o", "ko", 0);
		scriptflags2 |= FLAG2_OM;
	    }
	    break;
	case 0:				/* Implicit machine-name */
	    break;
	case -1:			/* Null machine-name */
	    fprintf (stderr, "Null -o stdout machine name.\n");
	    if (!cmdline) showline();
	    notqueued();		/* Exit */
	case -2:
	    fprintf (stderr, "-o stdout machine name is not known to ");
	    fprintf (stderr, "the local host.\n");
	    if (!cmdline) showline();
	    notqueued();		/* Exit */
	case -3:
	case -4:
	    fprintf (stderr, "Unable to determine machine-id for -o stdout ");
	    fprintf (stderr, "machine name.\n");
	    if (!cmdline) showline();
	    seekhelp();			/* Seek staff support */
	}
	cp = destpath (argument);	/* get pathname relative */
					/* to any machine-name */
	i = strlen (cp);
	if (i > MAX_REQPATH) {
	    /*
	     *  Show Stdout pathname too long error message and exit(1).
	     */
	    exceeds ("-o stdout pathname", MAX_REQPATH, cmdline);
	}
	if (i == 0 || cp [i-1] == '/') {
	    /*
	     *  Show invalid name given for Stdout file message and exit(1).
	     */
	    bad_value("o", cmdline); 	/* Exit */
	}
	if (scpflag) return (++argv);
	strcpy (rawreq.v.bat.stdout_name, cp);
	return (++argv);
    case 'p':				/* Intra-queue request priority */
#ifndef SDSC
	if (argument [2] != '\0') {
	    invalidflag (cmdline, argument);
	    return (argv);
	}
	if (*argv == NULL) {
	    /*
	     *  Missing request priority value. Exit.
	     */
	    no_value ("Request priority", "p", cmdline);
	}
	if (cmdline) {
	    if (cmdflags & FLAG_P) {
		/*
		 *  A priority flag was previously specified. Exit.
		 */
		twice ("p", 1);
	    }
	    cmdflags |= FLAG_P;		/* Set seen flag */
	}
	else {
	    if (scriptflags & FLAG_P) {
		/*
		 *  Multiple priority flags specified in script file. Exit.
		 */
		twice ("p", 0);
	    }
	    scriptflags |= FLAG_P;	/* Set seen flag */
	    if (cmdflags & FLAG_P) return (++argv); /* Already seen */
	}
	argument = *argv;		/* Get value string */
	i = strlen (argument);		/* Get length */
	if (isdecstr (argument, i)) {
	    /*
	     *  Be careful about overflow.
	     */
	    while (*argument == '0') {	/* Scan leading zeroes. */
		argument++;		/* Next character */
		i--;			/* Less digits */
	    }
	    if (*argument == '\0') {	/* The whole thing is "0" */
		argument--;		/* Backup */
		i++;			/* One digit */
	    }
	    if (i <= 4) rawreq.rpriority = atoi (argument);
	    if (i > 4 || rawreq.rpriority > MAX_RPRIORITY) {
		/*
		 *  Request priority exceeds maximum.
		 */
		fprintf (stderr, "-p priority exceeds limit: %1d.\n",
			 MAX_RPRIORITY);
		if (!cmdline) showline();
		notqueued();		/* Exit */
	    }
	}
	else {
	    /*
	     *  Request priority was not a decimal digit string.
	     */
	    fprintf (stderr, "-p priority is not a decimal digit string.\n");
	    if (!cmdline) showline();
	    notqueued();		/* Exit */
	}
#endif
	return (++argv);
    case 'q':				/* Queue name to submit req to */
	if (argument [2] != '\0') {
	    invalidflag (cmdline, argument);
	    return (argv);
	}
	if (*argv == NULL) {
	    /*
	     *  Missing queue name. Exit.
	     */
	    no_value ("Queue name", "q", cmdline);
	}
	if (strlen (*argv) > MAX_QUEUENAME) {
	    /*
	     *  Queue name is too long.  Exit.
	     */
	    exceeds ("-q queue name", MAX_QUEUENAME, cmdline);
	}
	if (cmdline) {
	    if (cmdflags & FLAG_Q) {
		/*
		 *  A queue was previously specified. Exit.
		 */
		twice ("q", 1);
	    }
	    cmdflags |= FLAG_Q;		/* Set seen flag */
	}
	else {
	    if (scriptflags & FLAG_Q) {
		/*
		 *  Multiple queues specified in script file. Exit.
		 */
		twice ("q", 0);
	    }
	    scriptflags |= FLAG_Q;	/* Set seen flag */
	    if (cmdflags & FLAG_Q) return (++argv); /* Already seen */
	}
	strcpy (rawreq.quename, *argv);	/* Save queue name */
	return (++argv);
    case 'r':				/* Request name or possible */
					/* -ro -re flag */
	if (argument [2] == '\0') {
	    /*
	     *  -r flag.
	     */
	    if (*argv == NULL) {
		/*
		 *  Missing request name value. Exit.
		 */
		no_value ("Request name", "r", cmdline);
	    }
	    if (cmdline) {
		if (cmdflags & FLAG_R) {
		    /*
		     *  A request name flag was previously specified. Exit.
		     */
		    twice ("r", 1);
		}
	        cmdflags |= FLAG_R;	/* Set seen flag */
	    }
	    else {
		if (scriptflags & FLAG_R) {
		    /*
		     * Multiple request name flags specified in script file.
		     * Exit.
		     */
		    twice ("r", 0);
		}
		scriptflags |= FLAG_R;	/* Set seen flag */
	        if (cmdflags & FLAG_R) return (++argv); /* Already seen */
	    }
	    argument = *argv;		/* Reqname */
	    if (*argument >= '0' && *argument <= '9') {
		/*
		 *  Request names cannot start with a digit.  Correct 
		 *  this by prepending a 'R' to the request-name.
		 */
		rawreq.reqname [0] = 'R';
		strncpy (rawreq.reqname+1, argument, MAX_REQNAME-1);
	    }
	    else strncpy (rawreq.reqname, argument, MAX_REQNAME);
	    rawreq.reqname [MAX_REQNAME] = '\0';	/* Null terminate */
	    return (++argv);
	}
	if (argument [2] == 'e') {
	    /*
	     *  -re
	     */
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    if (cmdline) {
		cmdflags |= FLAG_RE;
	    }
	    else {
		scriptflags |= FLAG_RE;
		if (cmdflags & FLAG_RE) {
		    return (argv);
		}
	    }
	    SETACCESS(rawreq.v.bat.stderr_acc, OMD_NOSPOOL);
	    return (argv);		/* Success */
	}
	if (argument [2] == 'o') {
	    /*
	     *  -ro
	     */
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    if (cmdline) {
		cmdflags |= FLAG_RO;
	    }
	    else {
		scriptflags |= FLAG_RO;
		if (cmdflags & FLAG_RO) {
		    return (argv);
		}
	    }
	    SETACCESS(rawreq.v.bat.stdout_acc, OMD_NOSPOOL);
	    return (argv);		/* Success */
	}
	if (argument [2] == 's') {
	    /*
	     * -rs flag, to make job restartable
	     */
            if (argument [3] != '\0') {
                invalidflag (cmdline, argument);
                return (argv);
            }
            rawreq.flags |= RQF_RESTARTABLE;
            return (argv);              /* Not recoverable; return success */
        }
	/*
	 *  Bad flag argument.
	 */
	invalidflag (cmdline, argument);
	return (argv);
    case 's':
	if (argument [2] == 'i') {	/* Possible stage-in event */
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    fprintf (stderr, "WARNING:  Unimplemented \"-si\" flag RESERVED ");
	    fprintf (stderr, "for future use.\n");
/*
/*
/*	    if (*argv == NULL) {
/*		/*
/*		 *  Missing stage-in event specification. Exit.
/*		 */
/*		no_value ("Stage-in event specification", "si", cmdline);
/*	    }
/*	    if (rawreq.v.bat.instacount >= MAX_INSTAPERREQ) {
/*		fprintf (stderr, "Number of stage-in events exceeds the ");
/*		fprintf (stderr, "maximum of %1d as\n supported by NQS.\n",
/*			 MAX_INSTAPERREQ); 
/*		notqueued();		/* Exit */
/*	    }
/*	    argument = *argv;		/* Get value string */
/*	    switch (machpath (argument, &rawreq.v.bat.instamid
/*		    [rawreq.v.bat.instacount])) {
/*	    case 1:			/* Explicit pathname/file hierarchy */
/*		break;
/*	    case 0:			/* Implicit pathname/file hierarchy */
/*		break;
/*	    case -1:			/* Null machine-name */
/*		fprintf (stderr, "Null -si stage-in event machine-name.\n");
/*		if (!cmdline) showline();
/*		notqueued();		/* Exit */
/*	    case -2:
/*		fprintf (stderr, "-si stage-in event machine-name is ");
/*		fprintf (stderr, "not known to local host.\n");
/*		if (!cmdline) showline();
/*		notqueued();		/* Exit */
/*	    case -3:
/*	    case -4:
/*		fprintf (stderr, "Unable to determine machine-id of -si ");
/*		fprintf (stderr, "stage-in event machine-name.\n");
/*		if (!cmdline) showline();
/*		seekhelp();		/* Seek staff support */
/*	    }
/*	    cp = destpath (argument);	/* get pathname/file hierarchy */
/*					/* relative to any machine-name */
/*	    i = strlen (cp);
/*	    if (cp [0] != '/') i += strlen (workdir);
/*	    if (i > MAX_REQPATH) {
/*		/*
/*		 * Pathname too long.  Exit.
/*		 */
/*		exceeds ("-si stage-in-pathname", MAX_REQPATH, cmdline);
/*	    }
/*	    if ((newmem = malloc (i+1)) == NULL) insmemstage();
/*	    istafile [rawreq.v.bat.instacount] = newmem;
/*	    *newmem = '\0';		/* Null length */
/*	    if (cp [0] != '/') {
/*		strcpy (istafile [rawreq.v.bat.instacount], workdir);
/*		strcat (istafile [rawreq.v.bat.instacount], "/");
/*	    }
/*	    strcat (istafile [rawreq.v.bat.instacount], cp);
/*	    rawreq.v.bat.instacount++;	/* One more stage-in event */
/*	    return (++argv);		/* Success */
/*
 *
 */
	}
	else if (argument [2] == 'o') {	/* Possible stage-out event */
	    if (argument [3] != '\0') {
		invalidflag (cmdline, argument);
		return (argv);
	    }
	    fprintf (stderr, "WARNING:  Unimplemented \"-so\" flag RESERVED ");
	    fprintf (stderr, "for future use.\n");
/*
/*
/*	    if (*argv == NULL) {
/*		/*
/*		 *  Missing stage-out event specification. Exit.
/*		 */
/*		no_value ("Stage-out event specification", "so", cmdline);
/*	    }
/*	    if (rawreq.v.bat.oustacount >= MAX_OUSTAPERREQ) {
/*		fprintf (stderr, "Number of stage-out events exceeds the ");
/*		fprintf (stderr, "maximum of %1d as\n supported by NQS.\n",
/*			 MAX_OUSTAPERREQ); 
/*		notqueued();		/* Exit */
/*	    }
/*	    argument = *argv;		/* Get value string */
/*	    switch (machpath (argument, &rawreq.v.bat.oustamid
/*		    [rawreq.v.bat.oustacount].mid)) {
/*	    case 1:			/* Explicit pathname/file hierarchy */
/*		break;
/*	    case 0:			/* Implicit pathname/file hierarchy */
/*		break;
/*	    case -1:			/* Null machine-name */
/*		fprintf (stderr, "Null -so stage-out event machine-name.\n");
/*		if (!cmdline) showline();
/*		notqueued();		/* Exit */
/*	    case -2:
/*		fprintf (stderr, "-so stage-out event machine-name is ");
/*		fprintf (stderr, "not known to local host.\n");
/*		if (!cmdline) showline();
/*		notqueued();		/* Exit */
/*	    case -3:
/*	    case -4:
/*		fprintf (stderr, "Unable to determine machine-id of -so ");
/*		fprintf (stderr, "stage-out event machine-name.\n");
/*		if (!cmdline) showline();
/*		seekhelp();		/* Seek staff support */
/*	    }
/*	    cp = destpath (argument);	/* get pathname/file hierarchy */
/*					/* relative to any machine-name */
/*	    i = strlen (cp);
/*	    if (cp [0] != '/') i += strlen (workdir);
/*	    if (i > MAX_REQPATH) {
/*		/*
/*		 * Pathname too long.  Exit.
/*		 */
/*		exceeds ("-so stage-out-pathname", MAX_REQPATH, cmdline);
/*	    }
/*	    if ((newmem = malloc (i+1)) == NULL) insmemstage();
/*	    ostafile [rawreq.v.bat.oustacount] = newmem;
/*	    *newmem = '\0';		/* Null length */
/*	    if (cp [0] != '/') {
/*		strcpy (ostafile [rawreq.v.bat.oustacount], workdir);
/*		strcat (ostafile [rawreq.v.bat.oustacount], "/");
/*	    }
/*	    strcat (ostafile [rawreq.v.bat.oustacount], cp);
/*	    rawreq.v.bat.oustacount++;	/* One more stage-out event */
/*	    return (++argv);		/* Success */
/*
 *
 */
	}
	else if (argument [2] != '\0') {
	    invalidflag (cmdline, argument);
	    return (argv);
	}
	/*
	 *  -s <shell-pathname>
	 */
	argument = *argv;
	if (argument == NULL) {
	    /*
	     *  Missing shell pathname. Exit.
	     */
	    no_value ("Shell pathname", "s", cmdline);
	}
	if (*argument != '/') {
	    /*
	     *  Pathname must be absolute.
	     */
	    fprintf (stderr, "-s shell pathname must be absolute, starting ");
	    fprintf (stderr, "with \na \"/\" character (e.g. /bin/sh).\n");
	    if (!cmdline) showline();
	    notqueued();		/* Exit */
	}
	if (strlen (argument) > MAX_SHELLNAME) {
	    /*
	     *  Pathname too long. Exit.
	     */
	    exceeds ("-s shell-pathname", MAX_SHELLNAME, cmdline);
	}
	if (cmdline) {
	    if (cmdflags & FLAG_S) {
		/*
		 *  A shell specification flag was previously specified. Exit.
		 */
		twice ("s", 1);
	    }
	    cmdflags |= FLAG_S;		/* Set seen flag */
	}
	else {
	    if (scriptflags & FLAG_S) {
		/*
		 *  Multiple shell specifications given in script file. Exit.
		 */
		twice ("s", 0);
	    }
	    scriptflags |= FLAG_S;	/* Set seen flag */
	    if (cmdflags & FLAG_S) return (++argv); /* Already seen */
	}
	strcpy (rawreq.v.bat.shell_name, argument);
	return (++argv);		/* Save shell pathname */
    case 'x':				/* Possible -x flag */
	if (argument [2] == '\0') {	/* -x */
	    export = 1;			/* Set export mode */
	    return (argv);		/* Success */
	}
	/*
	 *  Bad flag argument.
	 */
	invalidflag (cmdline, argument);
	return (argv);
    case 'z':				/* Possible -z flag */
	if (argument [2] == '\0') {	/* -z */
	    silent = 1;			/* Set silent mode */
	    return (argv);		/* Success */
	}
	/*
	 *  Bad flag argument.
	 */
	invalidflag (cmdline, argument);
	return (argv);
    default:				/* Invalid argument flag */
	invalidflag (cmdline, argument);
	return (argv);
    }
}


/*** scanargs
 *
 *
 *	void scanargs():
 *	Scan arguments on shell script file line.
 */
static void scanargs (scan, scriptargs)
register char *scan;			/* Scan position in script line */
char *scriptargs [MAXLINEARGS+1];	/* Argument pointer array */
{
	register arg;
	register char ch;		/* Scan character */
	register char *copy;		/* As parsing continues, the */
					/* script line is rewritten, */
					/* removing '\' characters as*/
					/* necessary */

	arg = 0;			/* Argument 0 */
	copy = scan;			/* Set copy pointer */
	while (isspace (*scan)) scan++;	/* Scan leading whitespace */
	ch = *scan;			/* ch contains the scan char */
	while (ch != '\0' && ch != '#' && arg < MAXLINEARGS) {
	    /*
	     *  Loop until all arguments on the line have been
	     *  processed, a comment has been found, or a maximum
	     *  of MAXLINEARGS have been processed.
	     */
	    scriptargs [arg++] = copy;	/* Save pointer to argument */
	    do {
		/*
		 *  Scan the current argument following the quoting and
		 *  escaping conventions of the Bourne shell (sh), with
		 *  the exceptions that an escaped newline does not allow
		 *  for line continuation, and mismatched quotes are not
		 *  complained about.
		 *
		 *  The reader will note that the shell script line
		 *  handed to this function has had the trailing '\n'
		 *  character removed....
		 */
		switch (ch) {
		case '\\':		/* We have encountered an escaped */
					/* character */
		    ch = *++scan;	/* Scan the escaped character */
		    if (ch != '\0') {
			*copy++ = ch;	/* Copy the escaped character */
			ch = *++scan;	/* Scan the next character */
		    }
		    break;
		case '\'':		/* Entering single quotes */
					/* '\' is NOT recognized as escape */
					/* inside '...' */
		    ch = *++scan;	/* Scan the next character */
		    while (ch != '\'' && ch != '\0') {
					/* Scan until (') encountered */
			*copy++ = ch;	/* Copy the quoted character */
			ch = *++scan;	/* Scan the next character */
		    }
		    if (ch == '\'') {
			ch = *++scan;	/* Scan the next character */
		    }
		    /*
		     *  Now, trim any TRAILING whitespace that appears in
		     *  the argument.  We do this so that things like:
		     *
		     *	   -'s ' /bin/csh
		     *
		     *  are seen as:
		     *
		     *	   -'s' /bin/csh
		     *
		     *  in the argument processing function:  procarg().
		     */
		    while (isspace (*--copy))
			;
		    copy++;
		    break;
		case '"':		/* Entering double quotes */
					/* '\' is recognized as escape */
					/* inside "..." */
		    ch = *++scan;	/* Scan the next character */
		    while (ch != '"' &&	ch != '\0') {
					/* Scan until non-escaped (") */
			if (ch=='\\') {	/* Escape char has been scanned */
			    ch= *++scan;/* Scan the escaped character */
			    if (ch != '\0') {
				*copy++ = ch;
				ch = *++scan;
					/* Copy the escaped character */
			    }		/* and scan the next character */
			}
			else {
			    *copy++ = ch;
			    ch = *++scan;
					/* Copy the current character */
			}		/* and scan the next character */
		    }
		    if (ch == '"') {
			ch = *++scan;	/* Scan the next character */
		    }
		    /*
		     *  Now, trim any TRAILING whitespace that appears in
		     *  the argument.  We do this so that things like:
		     *
		     *	   -"s " /bin/csh
		     *
		     *  are seen as:
		     *
		     *	   -"s" /bin/csh
		     *
		     *  in the argument processing function:  procarg().
		     */
		    while (isspace (*--copy))
			;
		    copy++;
		    break;
		default:
		    *copy++ = ch;	/* Simply copy the character */
		    ch = *++scan;	/* Scan the next character */
		}
	    } while (ch != '\0' && !isspace (ch));
	    /*
	     *  We have reached the end of the present argument.
	     */
	    while (isspace (*scan)) {	/* Scan trailing whitespace */
		scan++;
	    }
	    ch = *scan;			/* Load ch with 1st nonspace char */
	    *copy++ = '\0';		/* Null terminate argument */
	}
	if (*scan != '\0' && *scan != '#') {
	    /*
	     *  Too many arguments in shell script file line.
	     */
	    fprintf (stderr, "Error: too many arguments.\n");
	    showline();
	    notqueued();
	}
	scriptargs [arg] = NULL;	/* Null terminate argument list */
}


/*** scnametoolong
 *
 *	void scnametoolong():
 *	Show message and exit(1).
 */
static void scnametoolong()
{
	fprintf (stderr, "Script file pathname too long.\n");
	notqueued();			/* Exit */
}


/*** seekhelp
 *
 *	void seekhelp():
 *	Show message and exit(1).
 */
static void seekhelp()
{
	fprintf (stderr, "Seek help from system support personnel.\n");
	notqueued();			/* Exit */
}


/*** showline
 *
 *
 *	void showline():
 *	Show what line of the script we are on.
 */
static void showline()
{
	fprintf (stderr, "  Script file line %1u.\n", scriptline);
}


/*** spoolerror
 *
 *
 *	void spoolerror():
 *	Display message and exit.
 */
static void spoolerror()
{
	fprintf (stderr, "Error creating spooled version of script file.\n");
	fprintf (stderr, "Reason: %1s.\n", sys_errlist [errno]);
	seekhelp();			/* Exit */
}


/*** twice
 *
 *
 *	void twice ():
 *	Display message and exit.
 */
static void twice (flag, cmdline)
char *flag;				/* Which flag */
int cmdline;				/* Boolean */
{
	fprintf (stderr, "-%s appears more than once ", flag);
	if (cmdline) {
		fprintf (stderr, "on command line.\n");
	}
	else {
		fprintf (stderr, "in script file.\n");
		showline();
	}
	notqueued();			/* Exit */
}
