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
 
/*++ qpr.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qpr/qpr.c,v $
 *
 * DESCRIPTION:
 *
 *	Submit a print req to the NQS system.
 *	This program MUST be run as a setuid program.
 *
 *	Option flags are:
 *
 *		-a print-after-date-time	:Print after this time/date
 *		-f form-name			:Print forms to use
 *              -d environment variable         :Define an environment var.
 *              -e export file                  :Export file to server
 *              -l log message                  :Log message (accounting)
 *              -o options                      :Input type dependent options
 *              -t type                         :Input type
 *              -x                              :export environment variables
 *		-mb				:Send mail upon beginning
 *		-me				:Send mail upon ending 
 *		-mr				:Send mail on restart
 *		-ms				:Send mail on shutdown abort
 *		-mt				:Send mail upon transport
 *		-mu mail-address		:Send mail to this user
 *		-n #-of-copies			:Number of copies to print
 *		-p intra-queue-priority#	:Req priority within queue
 *		-q queue-name			:Name of print queue
 *		-r request-name			:Specify request-name
 *		-z				:Submit request silently
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	November 12, 1985.
 *
 *	Numerous additions by Intergraph 
 *		-d, -e, -l, -o, -t, -x.
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * Revision 1.6  89/05/04  00:53:27  mar
 * added -l option to log a message to accounting.
 *
 * Revision 1.5  89/04/18  02:24:56  mar
 * Should not close printfd when datafile is from stdin.
 * Closing it causes nqsdaemon to die because of an inconsistent message packet.
 *
 * Revision 1.4  89/03/24  11:01:08  mar
 * Close printfd after each file is spooled.
 * Causes File Table Overflow-ENFILE
 *
 * Revision 1.3  89/03/22  00:00:30  mar
 * add support for symbolic links (-s option)
 *
 * Revision 1.2  89/02/28  17:25:36  mar
 * add new qpr options to support devserver
 * -d define environment variable
 * -e define export file
 * -o user options to pass to server
 * -t input type
 * -x export the rest of the environment to the server
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 01:35:56 $ $State: Exp $)
 * $Log: qpr.c,v $
 * Revision 1.3  1994/11/19  01:35:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:30:23  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:29:01  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:59:06  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:11:18  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)qpr.c	1.2 (qpr.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <signal.h>		/* Signal names */
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>			/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#include <fcntl.h>			/* for SUN */
#else
BAD SYSTEM TYPE
#endif
#endif
#include "nqs.h"		/* NQS types, definitions, and sys/type.h */
#include "nqsdirs.h"		/* For nqslib library functions */
#include "informcc.h"		/* NQS information completion bits/masks */
#include "mkreqcc.h"		/* mkreq.c (mkctrl) completion codes */
#include <sys/stat.h>		/* Stat file */
#include <ctype.h>		

/*
 *	The #defines of FLAG_ are used to detect multiple specifications
 *	of a given argument of option flag.
 */
#define	FLAG_A	     000001	/* "-a" seen */
#define	FLAG_F	     000002	/* "-f" seen */
#define	FLAG_MU	     000004	/* "-mu" seen */
#define	FLAG_N	     000010	/* "-n" seen */
#define	FLAG_P	     000020	/* "-p" seen */
#define	FLAG_Q	     000040	/* "-q" seen */
#define	FLAG_R	     000100	/* "-r" seen */
#define FLAG_T             000200     /* "-t" seen */
#define FLAG_O             000400     /* "-o" seen */
#define FLAG_L             001000     /* "-l" seen */
 

/*
 *	Define the largest number than can fit in an unsigned long in 32-bits.
 */
#define	MAX_U32	   4294967295L	/* 2^32-1 */

/*
 *	UNIX external definitions:
 */
extern int errno;		/* UNIX system call error# */
extern char *sys_errlist [];	/* Error text list */

/*
 *	External functions:
 */

extern void analyzetcm();	/* Analyze and display transaction */
				/* diagnostics */
extern int atoi();		/* Ascii to integer conversion */
extern int gethostname();	/* get name of local host */
extern char *destacct();	/* Return pointer to destination account */
				/* relative to destination machine */
extern int isdecstr();		/* Boolean is a string of decimal digits */
extern int machacct();		/* Determine machine-id of machine */
				/* in a mail account specification. */
extern int mklink();          /* Make a symbolic link to data file */
extern char *strchr();                /* Locate a character within a string */
extern int mkctrl();		/* Make a control file */
extern int mkdata();		/* Make a data file */
extern long quereq();		/* Queue an NQS req */
extern int scnftime();		/* Scan a date/time specification defaulting */
				/* unspecified values to non-past current or */
				/* next temporally closest value */
extern char *strcpy();		/* String copy */
extern int strlen();		/* Compute string length */
extern char *strncpy();		/* Anchored string copy */
#if	BSD42 | BSD43 | ULTRIX
extern char *rindex();		/* Find character from end of string */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern char *strrchr();		/* Find character from end of string */
#else
BAD SYSTEM TYPE
#endif
#endif
extern int writereq();		/* Write control file request hdr */
extern void zapreq();		/* Unlink all files associated with */
				/* the request from the NQS new */
				/* request staging directory */

/*
 *	Internal functions not returning a value.
 */
void cleanup();			/* Cleanup function */
void enverror();              /* Environment error and exit */
void invalidflag();		/* Invalid flag option */
void notqueued();		/* Req not queued message and exit() */
void openerror();		/* Print file open error */
void seekhelp();		/* Seek help from system staff */
				/* message and notqueued(). */
void spoolerror();		/* Display error spooling print files & exit */

 /*
  * Global variables
  */
 char *inputtype;
 char *logmessage;
 char *optionlist;
 short export;
 short slink;
 char **xnames;
 char **xfiles;
 char **next_xname;
 char **next_xfile;
 char **envars;
 char **next_envar;

 
/*** main
 *
 *
 *	Submit a print request.
 */
main (argc, argv,envp)
int argc;
char **argv;
char **envp;
{
	char **files;			/* List of files to be printed */
	char **next_file;		/* Ptr to next available slot */
					/* in the print file list */
	register char *argument;	/* Ptr to argument text */
	struct stat stat_buf;		/* fstat() buffer */
	long cmdflags;			/* Flags and option mask */
	FILE *ctrlfile;			/* Control file */
	FILE *datafile;			/* Data file */
	int printfd;			/* Print file file-descriptor */
	struct rawreq rawreq;		/* Raw request structure */
	short silent;			/* Req shall be queued silently */
					/* boolean var */
	char *workdir;			/* Current working directory */
	char *cp;			/* Character pointer */
	char path [MAX_REQPATH+1];	/* Scratch pathname generation area */
        char path2 [MAX_PATHNAME+1];    /* Another pathname generation area */
	char buffer [BUFSIZ];		/* File copy buffer */
	long reqseq;			/* Sequence number assigned to req */
	long transaction_code;		/* Transaction completion code */
	char hostname [MAX_MACHINENAME+1];/* Name of local host */
	mid_t local_mid;		/* Local host machine-id */
	int i;				/* Work var */
	short ch;			/* A character from stdin */
	unsigned long stdinbytes;	/* Number of bytes from stdin */
        short last_printfile;           /* last print file before xfiles */

	signal (SIGHUP, cleanup);	/* Upon the receipt of any of these */
	signal (SIGQUIT, cleanup);	/* signals, delete all files assoc. */
	signal (SIGINT, cleanup);	/* with the request and exit in a */
	signal (SIGTERM, cleanup);	/* graceful and dignified manner */

	/*
	 *  Do 500 hundred things along with creating the control file
	 *  for the request.  When done, we will have a control file, our
	 *  current working directory will be the NQS new request
	 *  directory, we will have lost our setuid() privileges,
	 *  and the request structure we specified will have been
	 *  initialized to a reasonable default state.
	 */

	switch (mkctrl (&rawreq, RTYPE_DEVICE, &ctrlfile, &workdir,
			&local_mid)) {
	case MKREQ_SUCCESS:
		break;
	case MKREQ_NOCREATE:
		fprintf (stderr, "Unable to create the print request ");
		fprintf (stderr, "control file.\n");
		seekhelp();		/* Exit */
	case MKREQ_NOMID:
		fprintf (stderr, "Unable to determine the machine-id of the ");
		fprintf (stderr, "local host.\n");
		seekhelp();		/* Exit */
	case MKREQ_BADREQTYPE:
		fprintf (stderr, "Bad request type.\n");
		seekhelp();		/* Exit */
	case MKREQ_NOUSERNAME:
		fprintf (stderr,
			 "Unable to determine username from user-id.\n");
		seekhelp();		/* Exit */
	case MKREQ_NOCWD:
		fprintf (stderr, "Unable to determine the current working ");
		fprintf (stderr, "directory.\n");
		notqueued();		/* Exit */
	case MKREQ_CWDNEWLINE:		/* Not a problem for print reqs */
		break;
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
	default:
		fprintf (stderr, "Bad mkctrl() return value.\n");
		seekhelp();		/* Exit */
	}
	/*
	 *  The control file was successfully created.
	 */
	cmdflags = 0;			/* No flags seen yet */
	silent = 0;			/* -z not seen */
	files = argv;			/* Will become ptr to files to print */
	next_file = argv;		/* Used for building list of print */
					/* files */
       xnames = (char **) calloc(argc,sizeof(char *));
       next_xname = xnames;
       xfiles = (char **) calloc(argc,sizeof(char *));
       next_xfile = xfiles;
       envars = (char **) calloc(argc,sizeof(char *));
       next_envar = envars;
       export = 0;
       slink = 0;
       inputtype = "default";
       logmessage = "";
       optionlist = "";

 
	argv++;				/* Reference first argument */
	while (*argv != NULL) {
		argument = *argv++;		/* Get argument */
		if (*argument != '-') {
			/*
			 *  This argument is the name of a file to be
			 *  printed.
			 */
			*next_file++ = argument;/* Save ptr to filename */
			continue;		/* Loop */
		}
		/*
		 *  Otherwise, the flag discovered is an argument flag
		 *  with a preceding "-".
		 */
		switch (argument [1]) {
		case 'a':			/* Run request after time */
			if (argument [2] != '\0') {
				invalidflag (argument);	/* Exit */
			}
			if (cmdflags & FLAG_A) {
				/*
				 *  A start-after time was previously
				 *  specified.
				 */
				fprintf (stderr,
					 "Multiple request start-after ");
				fprintf (stderr,
					 "time specifications: \"-a\".\n");
				notqueued();	/* Exit */
			}
			cmdflags |= FLAG_A;	/* Set seen flag */
			if (*argv == NULL) {
				/*
				 *  Missing start-after time value.
				 */
				fprintf (stderr,
					 "No start-time value given for ");
				fprintf (stderr, "argument: \"-a\".\n");
				notqueued();	/* Exit */
			}
			switch (scnftime (*argv, &rawreq.start_time)) {
			case -1:
				fprintf (stderr, "Invalid date/time syntax ");
				fprintf (stderr, "for argument: \"-a\".\n");
				notqueued();	/* Exit */
			case -2:		/* Bad date/time syntax */
			case -3:		/* Missing date/time spec */
				fprintf (stderr, "Invalid date/time ");
				fprintf (stderr, "specified for ");
				fprintf (stderr, "argument: \"-a\".\n");
				notqueued();	/* Exit */
			}
			argv++;			/* Scan past date/time */
			break;			/* Loop */
               case 'd':                       /* Define Environment Var */
                       if (argument [2] != '\0') {
                               invalidflag (argument); /* Exit */
                       }
                       if (*argv == NULL) {
                               /*
                                *  Missing environment variable specification
                                */
                               fprintf (stderr,
                                   "No environment variable given for ");
                               fprintf (stderr, "argument: \"-d\".\n");
                               notqueued();    /* Exit */
                       }
                       if (strlen (*argv) > MAX_REQPATH) {
                               /*
                                *  Environment variable is too long.
                                */
                               fprintf (stderr,
                                   "Specified environment variable for ");
                               fprintf (stderr, "argument: \"-d\" exceeds\n");
                               fprintf (stderr,
                                        "the maximum length of %1d.\n",
                                         MAX_REQPATH);
                               notqueued();
                       }
                       addevar (*argv);        /* Save the env variable */
                       ++argv;                 /* Scan past env variable */
                       break;                  /* Loop */
               case 'e':                       /* Export file */
                       if (argument [2] != '\0') {
                               invalidflag (argument); /* Exit */
                       }
                       if (*argv == NULL) {
                               /*
                                *  Missing export specification
                                */
                               fprintf (stderr,
                                   "No export file specification given for ");
                               fprintf (stderr, "argument: \"-e\".\n");
                               notqueued();    /* Exit */
                       }
                       if (strlen (*argv) > MAX_REQPATH) {
                               /*
                                *  Input type is too long.
                                */
                               fprintf (stderr,
                                   "Specified export specification for ");
                               fprintf (stderr, "argument: \"-e\" exceeds\n");
                               fprintf (stderr,
                                        "the maximum length of %1d.\n",
                                         MAX_REQPATH);
                               notqueued();
                       }
                       addxfile (*argv);       /* Save the export file */
                       ++argv;                 /* Scan past form name */
                       break;                  /* Loop */
 
		case 'f':			/* Forms to use */
			if (argument [2] != '\0') {
				invalidflag (argument); /* Exit */
			}
			if (cmdflags & FLAG_F) {
				/*
				 *  A print-form was previously specified.
				 */
				fprintf (stderr, "Multiple print form ");
				fprintf (stderr, "specifications: \"-f\".\n");
				notqueued();	/* Exit */
			}
			cmdflags |= FLAG_F;	/* Set seen flag */
			if (*argv == NULL) {
				/*
				 *  Missing form name.
				 */
				fprintf (stderr, "No form name given for ");
				fprintf (stderr, "argument: \"-f\".\n");
				notqueued();	/* Exit */
			}
			if (strlen (*argv) > MAX_FORMNAME) {
				/*
				 *  Form name is too long.
				 */
				fprintf (stderr, "Specified form name for ");
				fprintf (stderr, "argument: \"-f\" exceeds\n");
				fprintf (stderr,
					 "the maximum length of %1d.\n",
					  MAX_FORMNAME);
				notqueued();
			}
			strcpy (rawreq.v.dev.forms, *argv);/* Save form name */
			++argv;			/* Scan past form name */
			break;			/* Loop */
               case 'l':
                       if (argument [2] != '\0') {
                               invalidflag (argument); /* Exit */
                       }
                       if (cmdflags & FLAG_L) {
                               /*
                                *  An input type was previously specified.
                                */
                               fprintf (stderr, "Multiple input type ");
                               fprintf (stderr, "specifications: \"-l\".\n");
                               notqueued();    /* Exit */
                       }                                                      
                       cmdflags |= FLAG_L;     /* Set seen flag */
                       if (*argv == NULL) {
                               /*                                  
                                *  Missing log message.
                                */
                               fprintf (stderr, "No log message given for ");
                               fprintf (stderr, "argument: \"-t\".\n");
                               notqueued();    /* Exit */
                       }                                                
                       if (strlen (*argv) > MAX_REQPATH) {
                               /*
                                *  Input type is too long. 
                                */
                               fprintf (stderr, "Specified log message for ");
                               fprintf (stderr, "argument: \"-l\" exceeds\n");
                               fprintf (stderr,
                                        "the maximum length of %1d.\n",
                                                
                                          MAX_REQPATH);
                               notqueued();
                        }                                                
                       if (strlen (*argv) > MAX_REQPATH) {
                               /*
                                *  Input type is too long. 
                                */
                               fprintf (stderr, "Specified log message for ");
                               fprintf (stderr, "argument: \"-l\" exceeds\n");
                               fprintf (stderr,
                                        "the maximum length of %1d.\n",
                                                
                                          MAX_REQPATH);
                               notqueued();
                                                       

                       }
                       logmessage = *argv;     /* Save the log message */
                       ++argv;                 /* Scan past log message */
                       break;                  /* Loop */
 
		case 'm':
			switch (argument [2]) {
			case 'b':		/* Begin mail flag */
				if (argument [3] != '\0') {
					invalidflag (argument);	/* Exit */
				}
				rawreq.flags |= RQF_BEGINMAIL;
				break;
			case 'e':		/* End mail flag */
				if (argument [3] != '\0') {
					invalidflag (argument);	/* Exit */
				}
				rawreq.flags |= RQF_ENDMAIL;
				break;
			case 'r':		/* Restart mail flag */
				if (argument [3] != '\0') {
					invalidflag (argument);	/* Exit */
				}
				rawreq.flags |= RQF_RESTARTMAIL;
				break;
			case 't':		/* Transport mail flag */
				if (argument [3] != '\0') {
					invalidflag (argument);	/* Exit */
				}
				rawreq.flags |= RQF_TRANSMAIL;
				break;
			case 'u':		/* Mail destination */
				if (argument [3] != '\0') {
					invalidflag (argument);	/* Exit */
				}
				if (cmdflags & FLAG_MU) {
					/*
					 *  A username mail flag was previously
					 *  specified.
					 */
					fprintf (stderr, "Multiple mail ");
					fprintf (stderr,
						 "specifications: \"-mu\".\n");
					notqueued();	/* Exit */
				}
				cmdflags |= FLAG_MU;	/* Set seen flag */
				if (*argv == NULL) {
					/*
					 *  Missing mail account name.
					 */
					fprintf (stderr,
						 "No mail account given for ");
					fprintf (stderr,
						 "argument: \"-mu\".\n");
					notqueued();	/* Exit */
				}
				argument = *argv;	/* Get value string */
				switch (machacct (argument, &rawreq.mail_mid)) {
				case 0:		/* Successfully got mid */
					break;
				case -1:	/* Null machine-name */
					fprintf (stderr,
						 "Null mail destination ");
					fprintf (stderr, "machine-name for ");
					fprintf (stderr,
						 "argument: \"-mu\".\n");
					notqueued();	/* Exit */
				case -2:
					fprintf (stderr,
						 "Mail destination machine-");
					fprintf (stderr,
						 "name for argument: ");
					fprintf (stderr,
						 "\"-mu\" is unknown to\n");
					fprintf (stderr, "local host.\n");
					notqueued();	/* Exit */
				case -3:
				case -4:
					fprintf (stderr,
						 "Unable to determine ");
					fprintf (stderr,
						 "machine-id of mail ");
					fprintf (stderr,
						 "destination for argument: ");
					fprintf (stderr, "\"-mu\".\n");
					seekhelp();	/* Seek staff support */
					notqueued();	/* Exit */
				}
				cp = destacct (argument);/* Get account-name */
						/* relative to machine */
				if (strlen (cp) > MAX_ACCOUNTNAME) {
					/*
					 *  Show account-name too long 
					 *  error message.
					 */
					fprintf (stderr,
						 "Account-name specified ");
					fprintf (stderr,
						 "for \"-mu\" flag exceeds ");
					fprintf (stderr,
						 "the maximum length ");
					fprintf (stderr, "length supported\n");
					fprintf (stderr,
						 "by NQS of %1d characters.\n",
						  MAX_ACCOUNTNAME);
					notqueued();	/* Exit */
				}
				strcpy (rawreq.mail_name, cp);
						/* Save mail account-name */
				++argv;
				break;
			default:
				invalidflag (argument);	/* Exit */
			}
			break;
		case 'n':			/* Number of copies */
			if (argument [2] != '\0') {
				invalidflag (argument);	/* Exit */
			}
			if (cmdflags & FLAG_N) {
				/*
				 *  A number of copies flag was previously
				 *  specified.
				 */
				fprintf (stderr,
					 "Multiple \"-n\" specifications.\n");
				notqueued();	/* Exit */
			}
			cmdflags |= FLAG_N;	/* Set seen flag */
			if (*argv == NULL) {
				/*
				 *  Missing number of copies specification.
				 */
				fprintf (stderr,
					 "No number of copies value given ");
				fprintf (stderr, "for argument: \"-n\".\n");
				notqueued();	/* Exit */
			}
			argument = *argv;	/* Get value string */
			i = strlen (argument);	/* Get length */
			if (isdecstr (argument, i)) {
				/*
				 *  Be careful about overflow.
				 */
				while (*argument == '0') {
					/* Scan leading zeroes. */
					argument++;	/* Next character */
					i--;		/* Less digits */
				}
				if (*argument == '\0') {
					/* The whole thing is "0" */
					argument--;	/* Backup */
					i++;		/* One digit */
				}
				if (i <= 4) rawreq.v.dev.copies=atoi (argument);
				if (i > 4 || rawreq.v.dev.copies > MAX_COPIES) {
					/*
					 *  Too many copies specified.
					 */
					fprintf (stderr,
						"Too many copies specified.\n");
					notqueued();	/* Exit */
				}
			}
			else {
				/*
				 *  Number of copies value was not a
				 *  decimal digit string.
				 */
				fprintf (stderr,
					 "Specified \"-n\" copies value ");
				fprintf (stderr,
					 "is not a decimal digit string.\n");
				notqueued();		/* Exit */
			}
			++argv;		/* Scan past copies spec */
			break;		/* Loop */
               case 'o':               /* Input type dependent options */
                       if (argument [2] != '\0') {
                               invalidflag (argument); /* Exit */
                       }
                       if (cmdflags & FLAG_O) {                   
                               /*
                                *  An options list was previously specified.
                                */
                               fprintf (stderr, "Multiple option list ");
                               fprintf (stderr, "specifications: \"-o\".\n");
                               notqueued();    /* Exit */                 

                       }
                       cmdflags |= FLAG_O;     /* Set seen flag */
                       if (*argv == NULL) {
                               /*
                                *  Missing input type name.
                                */
                               fprintf (stderr, "No option list given for ");
                               fprintf (stderr, "argument: \"-o\".\n");
                               notqueued();    /* Exit */
                       }
                       if (strlen (*argv) > MAX_REQPATH) {
                               /*
                                *  Option list is too long.
                                */
                               fprintf (stderr, "Specified option list for ");
                               fprintf (stderr, "argument: \"-o\" exceeds\n");
                               fprintf (stderr,                                
                                        "the maximum length of %1d.\n",

                                                                         
                                          MAX_REQPATH);
                               notqueued();
                       }                               
                       optionlist = *argv;     /* Save the input type */
                       ++argv;                 /* Scan past form name */
                       break;                  /* Loop */                


		case 'p':		/* Intra-queue request priority */
			if (argument [2] != '\0') {
				invalidflag (argument);	/* Exit */
			}
			if (cmdflags & FLAG_P) {
				/*
				 *  A priority flag was previously specified.
				 */
				fprintf (stderr, "Multiple request priority ");
				fprintf (stderr, "specifications: \"-p\".\n");
				notqueued();	/* Exit */
			}
			cmdflags |= FLAG_P;		/* Set seen flag */
			if (*argv == NULL) {
				/*
				 *  Missing request priority value.
				 */
				fprintf (stderr,
					 "No request priority value given ");
				fprintf (stderr, "for argument: \"-p\".\n");
				notqueued();		/* Exit */
			}
			argument = *argv;		/* Get value string */
			i = strlen (argument);		/* Get length */
			if (isdecstr (argument, i)) {
				/*
				 *  Be careful about overflow.
				 */
				while (*argument == '0') {
					/* Scan leading zeroes. */
					argument++;	/* Next character */
					i--;		/* Less digits */
				}
				if (*argument == '\0') {
					/* The whole thing is "0" */
					argument--;	/* Backup */
					i++;		/* One digit */
				}
				if (i <= 4) rawreq.rpriority=atoi (argument);
				if (i > 4 || rawreq.rpriority > MAX_RPRIORITY) {
					/*
					 *  Request priority exceeds maximum.
					 */
					fprintf (stderr,
						 "Specified request priority ");
					fprintf (stderr,
						 "exceeds limit:  %1d.\n",
						  MAX_RPRIORITY);
					notqueued();	/* Exit */
				}
			}
			else {
				/*
				 *  Request priority was not a 
				 *  decimal digit string.
				 */
				fprintf (stderr,
					 "Specified request priority value ");
				fprintf (stderr,
					 "is not a decimal digit string.\n");
				notqueued();	/* Exit */
			}
			++argv;		/* Scan past priority */
			break;		/* Loop */
		case 'q':		/* Queue name to submit req to */
			if (argument [2] != '\0') {
				invalidflag (argument);	/* Exit */
			}
			if (cmdflags & FLAG_Q) {
				/*
				 *  A queue was previously specified.
				 */
				fprintf (stderr, "Multiple queue ");
				fprintf (stderr, "specifications: \"-q\".\n");
				notqueued();	/* Exit */
			}
			cmdflags |= FLAG_Q;	/* Set seen flag */
			if (*argv == NULL) {
				/*
				 *  Missing queue name.
				 */
				fprintf (stderr, "No queue name given for ");
				fprintf (stderr, "argument: \"-q\".\n");
				notqueued();	/* Exit */
			}
			if (strlen (*argv) > MAX_QUEUENAME) {
				/*
				 *  Queue name is too long.
				 */
				fprintf (stderr, "Specified queue name for ");
				fprintf (stderr, "argument: \"-q\"\n");
				fprintf (stderr,
					 "exceeds the maximum length of %1d.\n",
					  MAX_QUEUENAME);
				notqueued();
			}
			strcpy (rawreq.quename, *argv);	/* Save queue name */
			++argv;			/* Scan past queue name */
			break;			/* Loop */
		case 'r':			/* Request name specified */
			if (argument [2] != '\0') {
				invalidflag (argument);	/* Exit */
			}
			if (cmdflags & FLAG_R) {
				/*
				 *  A request name flag was previously
				 *  specified.
				 */
				fprintf (stderr, "Multiple request name ");
				fprintf (stderr, "specifications: \"-r\".\n");
				notqueued();	/* Exit */
			}
			cmdflags |= FLAG_R;	/* Set seen flag */
			if (*argv == NULL) {
				/*
				 *  Missing request name value.
				 */
				fprintf (stderr,
					 "No request name value given for ");
				fprintf (stderr, "argument: \"-r\".\n");
				notqueued();	/* Exit */
			}
			argument = *argv;	/* Request name */
			i = strlen (argument);	/* Length of name */
			if (i > MAX_REQNAME) {
				/*
				 *  The request name must be truncated.
				 */
				i = MAX_REQNAME;
			}
			if (*argument >= '0' && *argument <= '9') {
				/*
				 *  Request names cannot start with a
				 *  digit.  Correct this by prepending
				 *  a 'R' to the request name truncating
				 *  again if necessary.
				 */
				if (i == MAX_REQNAME) i--;
				rawreq.reqname [0] = 'R';
				strncpy (rawreq.reqname+1, argument, i++);
				fprintf (stderr,
					 "Warning:  Specified request name ");
				fprintf (stderr,
					 "begins with a digit which is ");
				fprintf (stderr, "illegal.\n");
				fprintf (stderr,
					"An \"R\" has been prepended to the ");
				fprintf (stderr, "request name:  %s.\n",
					rawreq.reqname);
			}
			else strncpy (rawreq.reqname, argument, i);
			rawreq.reqname [i] = '\0';	/* Null terminate */
			++argv;			/* Scan past request name */
			break;			/* Loop */
               case 's':
#if UTS
			fprintf(stderr,"Symbolic links are not supported \n");
#endif
                       if (argument [2] != '\0') {     /* -x */
                               invalidflag (argument); /* Exit */
                       }
                       slink = 1;      /* Set symbolic link mode */
                       break;          /* Loop */
               case 't':
                       if (argument [2] != '\0') {
                               invalidflag (argument); /* Exit */
                       }
                       if (cmdflags & FLAG_T) {                   
                               /*
                                *  An input type was previously specified.
                                */
                               fprintf (stderr, "Multiple input type ");
                               fprintf (stderr, "specifications: \"-t\".\n");
                               notqueued();    /* Exit */
                       }
                       cmdflags |= FLAG_T;     /* Set seen flag */
                       if (*argv == NULL) {
                               /*
                                *  Missing input type name.
                                */
                               fprintf (stderr, "No input type given for ");
                               fprintf (stderr, "argument: \"-t\".\n");
                               notqueued();    /* Exit */                    
                       }
                       if (strlen (*argv) > MAX_FORMNAME) {
                               /*
                                *  Input type is too long.
                                */
                               fprintf (stderr, "Specified input type for ");
                               fprintf (stderr, "argument: \"-t\" exceeds\n");
                               fprintf (stderr,
                                        "the maximum length of %1d.\n",        
                                         MAX_FORMNAME);
                               notqueued();
                       }                                
                       inputtype = *argv;      /* Save the input type */
                       ++argv;                 /* Scan past form name */
                       break;                  /* Loop */
               case 'x':                       /* Possible -x flag */
                       if (argument [2] != '\0') {     /* -x */
                               invalidflag (argument); /* Exit */     
                       }
                       export = 1;     /* Set export mode */
			break;		/* loop */
		case 'z':			/* Possible -z flag */
			if (argument [2] != '\0') {	/* -z */
				invalidflag (argument);	/* Exit */
			}
			silent = 1;	/* Set silent mode */
			break;		/* Loop */
		default:			/* Invalid argument flag */
			invalidflag (argument);	/* Exit */
		}				/* end switch (argument [1]) */
	}
	/*
	 *  Null terminate the print file list.  The variable:
	 *
	 *	files
	 *
	 *  now points to an array of pointers to character strings
	 *  of which each character string is the name of a file to
	 *  be printed.
	 */
	*next_file = NULL;
       *next_xname = NULL;
       *next_xfile = NULL;
       *next_envar = NULL;
 
	/*
	 *  We have completed the arduous process of scanning the command
	 *  line arguments.
	 */
	if (rawreq.quename [0] == '\0') {
		/*
		 *  No queue specified, and no default print queue
		 *  is declared for the local system.
		 */
		fprintf (stderr, "No request queue specified, and no ");
		fprintf (stderr, "local default has been defined.\n");
		notqueued();		/* Exit */
	}
	/*
	 *  If no reqname has been assigned, then we must assign a default.
	 */
	if (rawreq.reqname [0] == '\0') {
		/*
		 *  No reqname was assigned.  We default to the name of
		 *  the first print file, prepended with a 'R' if
		 *  necessary to enforce the requirement of all reqnames
		 *  beginning with a non-digit. When there is no 
		 *  print file, we default to "STDIN".
		 */
		if (*files == NULL) {
			strncpy (rawreq.reqname, "STDIN", MAX_REQNAME);
		}
		else {
#if	BSD42 | BSD43 | ULTRIX
			if ((cp = rindex (*files, '/')) == (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
			if ((cp = strrchr (*files, '/')) == (char *) 0) {
#else
BAD SYSTEM TYPE
#endif
#endif
				cp = *files;
			}
			else cp++;
			if (*cp >= '0' && *cp <= '9') {
				rawreq.reqname [0] = 'R';
				strncpy (rawreq.reqname+1, cp, MAX_REQNAME-1);
			}
			else strncpy (rawreq.reqname, cp, MAX_REQNAME);
		}
		rawreq.reqname [MAX_REQNAME] = '\0';
	}
	if (*files == NULL) {
	    /*
	     *  No print files have been specified; look to stdin.
	     */
	    fstat (0, &stat_buf);
	    switch (stat_buf.st_mode & 0170000) {
	    case 0010000:
			break;			/* Pipe */
	    case 0020000:
			break;			/* Character device */
	    case 0040000:
			fprintf (stderr, "Stdin is a directory.\n");
			notqueued();		/* exit */
	    case 0060000:
			fprintf (stderr, "Stdin is a block device.\n");
			notqueued();		/* exit */
	    default:
			break;			/* Ordinary file */
	    }
	    if (mkdata (&datafile) != 0) {
		/*
		 * We were unable to create the data file
		 * to hold the contents of stdin.
		 */
	        fprintf (stderr, "Unable to create spooled print file ");
		fprintf (stderr, "for request.\n");
		seekhelp ();			/* exit */
	    }
	    rawreq.ndatafiles = 1;
	    /*
	     *  Spool the contents of stdin to the data file.
	     */
	    stdinbytes = 0;
	    while ((ch = getchar ()) != EOF && stdinbytes != MAX_U32) {
		stdinbytes++;
		putc (ch, datafile);
	    }
	    if (ch != EOF) {
		/*
		 *  Too much input!
		 */
		fprintf (stderr, "Maximum printfile size exceeded.\n");
		notqueued ();			/* Exit */
	    }
	    if (stdinbytes == 0) {
		fprintf (stderr, "Input contains zero bytes.\n");
		notqueued ();			/* exit */
	    }
	    rawreq.v.dev.size = stdinbytes;
	    fclose (datafile);
	}
	else {
	    /*
	     *  Loop to spool specified print file(s).
	     */
	    do {
		if (*files [0] != '/') {
			/*
			 *  The pathname is relative.  Expand to absolute
			 *  pathname since we had to chdir() to the new
			 *  request directory.
			 */
			if (strlen (workdir) + 1 +
			    strlen (*files) > MAX_REQPATH) {
				fprintf (stderr,
					 "Absolute pathname of print file:\n");
				fprintf (stderr, "%s\nis too long.\n", *files);
				notqueued();
			}
			sprintf (path, "%s/%s", workdir, *files);
			printfd = open (path, O_RDONLY | O_NDELAY);
		}
		else {
			strcpy(path,*files);
			printfd = open (*files, O_RDONLY | O_NDELAY);
		}
		if (printfd == -1) {
			/*
			 *  Unable to open specified print file.
			 */
			openerror (*files);
			continue;		/* Loop to try the next file */
		}
		/*
		 *  We successfully opened the file.
		 *  Make sure that it is an ordinary file.
		 */
		fstat (printfd, &stat_buf);
		switch (stat_buf.st_mode & 0170000) {
		case 0010000:
			fprintf (stderr, "%s is a pipe.\n", *files);
			close (printfd);	/* Close the file */
			continue;		/* Loop to get next file */
		case 0020000:
			fprintf (stderr,
				 "%s is a character device.\n", *files);
			close (printfd);	/* Close the file */
			continue;		/* Loop to get next file */
		case 0040000:
			fprintf (stderr, "%s is a directory.\n", *files);
			close (printfd);	/* Close the file */
			continue;		/* Loop to get next file */
		case 0060000:
			fprintf (stderr, "%s is a block device.\n", *files);
			close (printfd);	/* Close the file */
			continue;		/* Loop to get next file */
		default:
			break;			/* Ordinary file */
		}
		if (stat_buf.st_size == 0) {
			fprintf (stderr, "%s is empty.\n", *files);
			close (printfd);
			continue;
		}
		/*
		 *  Check to see that the total number of bytes in the
		 *  print request does not exceed the number representable
		 *  in an unsigned long 32-bit integer.  The check we make
		 *  depends upon the underlying implementation of unsigned
		 *  long integers being the traditional twos-complement, or
		 *  less common ones-complement representation, and the fact
		 *  that UNIX file sizes cannot presently exceed the size
		 *  of a signed-long integer.
		 */
		if (stat_buf.st_size + rawreq.v.dev.size < rawreq.v.dev.size) {
			/*
			 *  Overflow would occur.  We have a ludicrously
			 *  large print request!
			 */
			fprintf (stderr,
				 "Maximum print request size exceeded.\n");
			notqueued();		/* Exit */
		}

#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UNICOS | OSF
               if (slink) {
                       if (mklink (path) != 0) {
                               /*
                                *  We were unable to create the symbolic
                                *  link.
                                */
                               fprintf (stderr,
                                   "Unable to create spooled print file ");
                               fprintf (stderr, "for request.\n");
                               seekhelp();             /* Exit */
                       }
                       rawreq.ndatafiles++;
                       rawreq.v.dev.size += stat_buf.st_size; /* Add size */
               }
               else {
#endif
                       /*
                        *  The print file is an ordinary file.
                        *  Create a data file to hold the spooled version of
                        *  the next print file.
                        */
                       if (mkdata (&datafile) != 0) {
                               /*
                                *  We were unable to create the data
                                *  file to hold the next print file.
                                */
                               fprintf (stderr,
                                   "Unable to create spooled print file ");
                               fprintf (stderr, "for request.\n");
                               seekhelp();             /* Exit */
                       }
                       /*
                        *  The data file was successfully created.

                        */
                       rawreq.ndatafiles++;    /* One more data file */
                       /*
                        *  Spool the contents of the print
                        *  file to the data file.
                        */
                       while ((i = read (printfd, buffer, BUFSIZ)) > 0) {
                               if (write (fileno (datafile), buffer, i) != i){
                                       spoolerror (*files);
                               }
                               rawreq.v.dev.size += i; /* Update size */
                                                       /* of print file */
                       }
                       if (i == -1) spoolerror (*files);                   
                       fclose (datafile);      /* This data file is */
                                               /* complete */
                       close (printfd);        /* close the spool file too */
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UNICOS | OSF
               }
#endif
               /*
                *  We are now done spooling this print file.
                *  See if there is another file.
                */
           } while (*++files != NULL);           
                                       /* Done spooling specified files */

           if (rawreq.v.dev.size == 0) {
               fprintf (stderr, "Input contains zero bytes.\n");
               notqueued();
           }
       }
       last_printfile = rawreq.ndatafiles;     /* last print file */
       if (*xfiles) {
           /*
            *  Loop to spool specified export file(s).
            */  
           do {
               if (*xfiles [0] != '/') {
                       /*
                        *  The pathname is relative.  Expand to absolute
                        *  pathname since we had to chdir() to the new
                        *  request directory.
                        */
                       if (strlen (workdir) + 1 +
                           strlen (*xfiles) > MAX_REQPATH) {
                               fprintf (stderr,
                                       "Absolute pathname of export file:\n");
                               fprintf (stderr, "%s\nis too long.\n", *xfiles);
                               notqueued();
                       }
                       sprintf (path, "%s/%s", workdir, *xfiles);
                       printfd = open (path, O_RDONLY | O_NDELAY);
               }
               else printfd = open (*xfiles, O_RDONLY | O_NDELAY);
               if (printfd == -1) {
                       /*
                        *  Unable to open specified export file.
                        */
                       openerror (*xfiles);
                       notqueued();    /* In this case, to not queue it */
               }
               /*
                *  We successfully opened the file.
                *  Make sure that it is an ordinary file.
                */
               fstat (printfd, &stat_buf);
               switch (stat_buf.st_mode & 0170000) {
               case 0010000:
                       fprintf (stderr, "%s is a pipe.\n", *xfiles);
                       close (printfd);        /* Close the file */
                       continue;               /* Loop to get next file */

               case 0020000:
                       fprintf (stderr,
                                "%s is a character device.\n", *xfiles);
                       close (printfd);        /* Close the file */
                       continue;               /* Loop to get next file */
               case 0040000:
                       fprintf (stderr, "%s is a directory.\n", *xfiles);
                       close (printfd);        /* Close the file */
                       continue;               /* Loop to get next file */
               case 0060000:
                       fprintf (stderr, "%s is a block device.\n", *xfiles);
                       close (printfd);        /* Close the file */
                       continue;               /* Loop to get next file */
               default:
                       break;                  /* Ordinary file */
               }
               if (stat_buf.st_size == 0) {
                       fprintf (stderr, "%s is empty.\n", *xfiles);
                       close (printfd);
                       continue;
               }
               /*
                *  Check to see that the total number of bytes in the
                *  print request does not exceed the number representable
                *  in an unsigned long 32-bit integer.  The check we make
                *  depends upon the underlying implementation of unsigned
                *  long integers being the traditional twos-complement, or
                *  less common ones-complement representation, and the fact
                *  that UNIX file sizes cannot presently exceed the size
                *  of a signed-long integer.
                */
               if (stat_buf.st_size + rawreq.v.dev.size < rawreq.v.dev.size) {
                       /*
                        *  Overflow would occur.  We have a ludicrously
                        *  large print request!
                        */
                       fprintf (stderr,
                                "Maximum print request size exceeded.\n");
                       notqueued();            /* Exit */
               }
 
		/*
		 *  The print file is an ordinary file.
		 *  Create a data file to hold the spooled version of
		 *  the next print file.
		 */
		if (mkdata (&datafile) != 0) {
			/*
			 *  We were unable to create the data file to hold
			 *  the next exprot file.
			 */
			fprintf (stderr,
				 "Unable to create spooled export file ");
			fprintf (stderr, "for request.\n");
			seekhelp();		/* Exit */
		}
		/*
		 *  The data file was successfully created.
		 */
		rawreq.ndatafiles++;		/* One more data file */
		/*
		 *  Spool the contents of the print file to the data file.
		 */
		while ((i = read (printfd, buffer, BUFSIZ)) > 0) {
			if (write (fileno (datafile), buffer, i) != i) {
				spoolerror (*xfiles);
			}
			rawreq.v.dev.size += i;	/* Update size of print file */
		}
		if (i == -1) spoolerror (*xfiles);
		fclose (datafile);		/* This data file is */
						/* complete */
		close(printfd);
		/*
		 *  We are now done spooling this print file.
		 *  See if there is another file.
		 */
	    } while (*++xfiles != NULL);
	   	 			/* Done spooling specified xfiles */
	    if (rawreq.v.dev.size == 0) {
		fprintf (stderr, "Input contains zero bytes.\n");
		notqueued();
	    }
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
        *  Write-out input type to control file
        */
       fseek (ctrlfile, lseek (fileno (ctrlfile), 0L, 1), 0);
       fprintf (ctrlfile, "T%s\n",inputtype);
       if (ferror (ctrlfile)) {
               fprintf (stderr, "Unable to write control file header.\n");
               fprintf (stderr, "Reason: %s.\n", sys_errlist [errno]);
               seekhelp();             /* Exit */
       }
       /*
        *  Write-out option list if any to the control file
        */
       if (optionlist) {
               fprintf (ctrlfile, "P%s\n",optionlist);
               if (ferror (ctrlfile)) {
                   fprintf (stderr, "Unable to write control file header.\n");
                   fprintf (stderr, "Reason: %s.\n", sys_errlist [errno]);
                   seekhelp();         /* Exit */
               }
       }
       /*
        *  Export environment variables if any
        */
       if (export) {
               while (*envp != NULL) {
                       if ((cp = strchr (*envp, '\n')) != (char *) 0) {
                               *cp = '\0';     /* Trailing newlines removed */
                       }
                       fprintf (ctrlfile, "E%s\n", *envp);
                       if (ferror (ctrlfile)) enverror();  /* Exit */
                       envp++;                 /* Get next environment var */
               }
       }
       while (*envars != NULL) {
               if ((cp = strchr (*envars, '\n')) != (char *) 0) {
                       *cp = '\0';     /* Trailing newlines removed */
               }
               fprintf (ctrlfile, "E%s\n", *envars++);
               if (ferror (ctrlfile)) {
                   fprintf (stderr, "Unable to write control file header.\n");
                   fprintf (stderr, "Reason: %s.\n", sys_errlist [errno]);
                   seekhelp();         /* Exit */
               }
       }
       while (*xnames != NULL) {
               fprintf (ctrlfile, "X%d=%s\n", ++last_printfile,*xnames++);
               if (ferror (ctrlfile)) {
                   fprintf (stderr, "Unable to write control file header.\n");
                   fprintf (stderr, "Reason: %s.\n", sys_errlist [errno]);
                   seekhelp();         /* Exit */
               }
       }
       /*
        *  Write-out log message to control file
        */
       fprintf (ctrlfile, "L%s\n",logmessage);
       if (ferror (ctrlfile)) {
               fprintf (stderr, "Unable to write control file header.\n");
               fprintf (stderr, "Reason: %s.\n", sys_errlist [errno]);
               seekhelp();             /* Exit */
       }
       fflush (ctrlfile);
       if (ferror (ctrlfile)) enverror();      /* Exit */

       /*
 
	 *  Now, queue the req (the control file must still be open
	 *  so that we can find out the req sequence number assigned
	 *  to this req).
	 */
	signal (SIGHUP, SIG_IGN);	/* Do NOT let ourselves exit */
	signal (SIGINT, SIG_IGN);	/* past this point! */
	signal (SIGQUIT, SIG_IGN);
	signal (SIGTERM, SIG_IGN);
	if ((reqseq = quereq (&transaction_code)) >= 0) {
		if (!silent) {
			if (gethostname (hostname, MAX_MACHINENAME) == -1) {
				hostname [0] = '\0';
			}
			else hostname [MAX_MACHINENAME] = '\0';
					/* Make sure of trailing '\0' */
			printf ("Request %1ld.%s submitted to queue: %s.\n",
				reqseq, hostname, rawreq.quename);
			if (transaction_code & XCI_INFORM_MASK) {
				analyzetcm (transaction_code, stderr, "");
			}
		}
		exit (0);		/* Request successfully queued */
	}
	/*
	 *  The request was not successfully queued.  Why?
	 */
	analyzetcm (transaction_code, stderr, "");
	exit (1);			/* Request not queued */
}


/*** cleanup
 *
 *
 *	void cleanup():
 *
 *	Upon the receipt of certain signals, we are to not complete
 *	the task of submitting a req to the NQS system for execution.
 */
static void cleanup (sig)
int sig;			/* Signal received */
{
	signal (sig, SIG_IGN);	/* Ignore multiple signals */
	zapreq();		/* Unlink all files in the NQS new request */
				/* staging directory before exiting. */
	exit (1);		/* Req not submitted. */
}


/*** invalidflag
 *
 *
 *	void invalidflag():
 *	Display invalid flag option message.
 */
static void invalidflag (argument)
char *argument;
{
	fprintf (stderr, "Invalid argument flag ");
	fprintf (stderr, "specified: %s.\n", argument);
	notqueued();
}


/*** notqueued
 *
 *
 *	void notqueued():
 *
 *	Unlink all files associated with the request and show
 *	the "Request not queued.\n" message and exit(1).
 */
static void notqueued()
{
	zapreq();			/* Unlink any request files */
	fprintf (stderr, "Request not queued.\n");
	exit (1);
}


/*** openerror
 *
 *
 *	void openerror():
 *	Display "Error opening print file".
 */
static void openerror (pathname)
char *pathname;				/* Name of print file */
{
	fprintf (stderr, "Error opening print file: %s.\n", pathname);
	fprintf (stderr, "Reason: %s.\n", sys_errlist [errno]);
	fprintf (stderr, "        %s not spooled.\n", pathname);
}


/*** seekhelp
 *
 *	void seekhelp():
 *	Show "Seek help from system support personnel.\n" and
 *	     "Request not queued.\n" messages and exit(1).
 */
static void seekhelp()
{
	fprintf (stderr, "Seek help from system support personnel.\n");
	notqueued();			/* Req not queued */
}


/*** spoolerror
 *
 *
 *	void spoolerror():
 *	Display "Error spooling print file" and exit.
 */
static void spoolerror (pathname)
char *pathname;				/* Name of print file */
{
	fprintf (stderr, "Error spooling print file: %s.\n", pathname);
	fprintf (stderr, "Reason: %s.\n", sys_errlist [errno]);
	seekhelp();			/* Exit */
}


 /*** enverror
  *
  *
  *    void enverror():
  *    Display message and exit.
  */
 static void enverror()
 {
       fprintf (stderr,
                "Unable to write environment variable to control file.\n");
       fprintf (stderr, "Reason: %s.\n", sys_errlist [errno]);
       seekhelp();                     /* Exit */
 }                                                             


 /*** addxfile
  *
  *
  *    int addxfile():
  *    Add export file to the list
  */
 int addxfile(cp)                  
 char *cp;
 {
       while (isspace(*cp))
               cp++;                   /* skip white space */
       if (*cp == '=') {               /* we have "=..." */
               fprintf(stderr, "No export tag (tag=filename) specified ");
               fprintf(stderr, "for argument: \"-e\".\n");
               notqueued();    /* Exit */
       }                                                   
       *next_xname++ = cp;             /* get the export tag name */

       while (*cp && *cp != '=') {     /* find the filename */
               if (isspace(*cp))
                       *cp++ = '\0';
               else
                       cp++;
       }
       if (*cp == '\0') {              /* we have "x" */
               fprintf(stderr, "Invalid export specification ");
               fprintf(stderr, "(tag=filename) for argument: \"-e\".\n");
               notqueued();    /* Exit */
       }
       *cp++ = '\0';                   /* terminate the export tag name */
       while (isspace(*cp))            /* skip white space */
               cp++;
       if (*cp == '\0') {              /* we have "x=" */
               fprintf(stderr, "No filename specified ");
               fprintf(stderr, "for argument: \"-e\".\n");
               notqueued();    /* Exit */
       }
       *next_xfile++ = cp;             /* get the file name */
 }


 /*** addevar
  *
  *
  *    int addevar():
  *    Add environment variable to list
  */
 int addevar(cp)
 char *cp;
 {
       while (isspace(*cp))
               cp++;                   /* skip white space */
       if (*cp == '=' || *cp == '\0' || strchr(cp,'=') == NULL ) {
               fprintf(stderr, "No name (name=value) specified ");
               fprintf(stderr, "for argument: \"-d\".\n");
               notqueued();    /* Exit */
       }
       *next_envar++ = cp;             /* get the export tag name */
  }



 
