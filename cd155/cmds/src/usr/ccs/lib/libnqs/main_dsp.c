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
 * HISTORY
 * $Log: main_dsp.c,v $
 * Revision 1.11  1994/12/08  00:33:28  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 6073 8229
 *  Testing: developer testing
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnqs/main_dsp.c
 *
 * Revision 1.10  1994/11/19  02:27:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/04/06  22:12:40  mwan
 * Update R1_3 to R1_2 WW15
 *
 *  Reviewer: kremenek
 *  Risk: M
 *  Benefit or PTS #: 7738,8087,8346, 8325,8599,8576,8600,8601,8088,8597,8876,8886
 *  Testing:
 *  Module(s): fetchpwdb.c listq.c listr.c main_dsp.c
 *
 * Revision 1.8  1993/11/02  01:23:58  mwan
 * 1.2 mods
 *
 * Revision 1.7  1993/09/07  20:27:37  shala
 * Fixed bug #6360 by jkearns.
 *
 * Revision 1.6.8.1  1993/09/07  18:16:19  shala
 * Fixed bug #6360 by jkearns.
 *
 * Revision 1.6  1993/07/13  17:57:44  mwan
 * T11 - fixed PTS 5012
 *
 * Revision 1.5  1993/05/19  17:48:32  mwan
 * Before T10 frozen. Fixed several PTS
 *
 * Revision 1.3  1992/10/26  18:38:53  mwan
 * T6 update 1
 *
 * Revision 1.2  1992/10/09  20:17:11  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)main_dsp.c	50.2 (main_dsp.c OSF/1 NQS2.0 GJK) 9/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include "nqs.h"
#include <string.h>

#if	UNICOS
#include <fcntl.h>			/* File control */
#include <nlist.h>
#include <sys/param.h>
#include <sys/jtab.h>
#else
#if	SGI | SYS52 | UTS | OSF | OSF
#include <fcntl.h>			/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#include <pwd.h>
#include "nqsdirs.h"			/* NQS files and directories */

#define DSP_ERR1	"-g, -u, -U, -A are mutually exclusive"
#define DSP_ERR2	" or one has been used previously in the command."
#define DSP_ERR3	"multiple uses of "
#define DSP_ERR4	"\tadministrator option '-U -u -A -G' was"
#define DSP_ERR5	"requested by an invalid NQS manager"


/*
 *	External functions:
 */
extern void bufstdout();		/* Buffer stdout */
extern void closepwdb();		/* Close the password file */
extern int daepres();			/* Determine if the local NQS */
					/* daemon is present */
extern struct passwd *fetchpwnam();	/* NQS version of getpwnam() */
extern uid_t getuid();			/* Get real user-id */
extern struct confd *opendb();		/* Open an NQS database file */
extern int dsp_q();			/* Show all queues */
extern int shoqbyname();		/* Show a queue given by name */

/*
 *	Global variables:
 */
char *Qstat_prefix = "Qstat";
int scpflag = 0;
#if	UNICOS
struct	jtab	jtab[NJOB];	/* job table */
#endif

/*** main_dsp
 *
 * set up everything for the displays
 * open needed databases, set up special
 * needs, ect.
 */
main_dsp (argc,argv,flags,selector,tname,hname)
int 	argc;
char 	*argv[];
long	flags;
char 	* selector;
char	* tname;
char	* hname;
{
	extern char	*optarg;
	extern int 	optind;

	struct confd *queuefile;	/* Queue description file */
	struct confd *qcomplexfile;	/* Queue complex definition file */
	struct confd *qmapfile;		/* Qmap description file */
	struct confd *pipeqfile;	/* Pipeqfile description file */
	char *argument;
	short daepresent;		/* Boolean non-zero if the local */
					/* NQS daemon is present and running */
	char whom[10];			/* Whom we are interested in */
	struct passwd *whompw;		/* Password entry of "whom" */
	uid_t whomuid;			/* Uid of whom we are interested in */
	mid_t	Local_mid, itsmid;	/* CERN Boissat */
	int	i;

	/* step 1. change to root directory */
	if (chdir (Nqs_root) == -1) {
		fprintf (stderr, "%s(FATAL): Unable to chdir() to the NQS ",
			 Qstat_prefix);
		fprintf (stderr, "root directory.\n");
		return (-1);
	}

	/* step 2. opend queue, complex, map, and pipeto databases */
	if ((queuefile = opendb (Nqs_queues, O_RDONLY)) == NULL) {
		fprintf (stderr, "%s(FATAL): Unable to open the NQS queue ",
			 Qstat_prefix);
		fprintf (stderr, "definition database file.\n");
		return (-1);
	}

        if ((qcomplexfile = opendb (Nqs_qcomplex, O_RDONLY)) == NULL) {
                fprintf (stderr, "QSTAT:(FATAL): Unable to open the NQS queue "
                         );
                fprintf (stderr, "complex definition database file.\n");
                exit (2);
        }
        if ((qmapfile = opendb (Nqs_qmaps, O_RDONLY)) == NULL) {
                fprintf (stderr, "QSTAT:(FATAL): Unable to open the NQS queue "
                         );
                fprintf (stderr, "complex definition database file.\n");
                exit (2);
        }
        if ((pipeqfile = opendb (Nqs_pipeto, O_RDONLY)) == NULL) {
                fprintf (stderr, "QSTAT:(FATAL): Unable to open the NQS queue "
                         );
                fprintf (stderr, "complex definition database file.\n");
                exit (2);
        }



	/* get user id */
	whomuid = getuid();		/* Assume asking about self */
	whompw=getpwuid(whomuid);
	strcpy(whom,whompw->pw_name);
	strcat(whom,"\0");
	closepwdb();		/* Close the password file */

#if UNICOS
	/* step 6.  Read job table from kernel memory.  */
	read_j_table();
#endif
	/* step 7.  Now, buffer stdout for speed.  */
	bufstdout();


	/* step 8. move to correct arg */
	for(i=0; i<argc; i++)
		argv++;

	/* step 9. if HOST then send to information to host */
	if (hname != NULL) {
		if (get_host_id(  hname, &itsmid) != SUCCESS) {
			printf("host not available. \n");
			exit (0);
		}
		do {
			call_host(whomuid, flags, itsmid, *argv);
			if (*argv != NULL)
				argv++;
		} while (*argv != NULL);
		return(0);
	}


	/* step 10. if requested adminstrator flag validate adminstrator 
	 *	if not requeusting adminstrator option then set
	 *	ourself as the selector for requests to be seen
	 */
#ifdef SDSC
	if (selector != NULL && !(flags & SHO_TSCHED)) {
#else
	if (selector != NULL) {
#endif
		if (localmid (&Local_mid) != 0) {
			printf("Unable to get machine-id of local host \n");
			exit(2);
		}
		if (nqspriv(whomuid, Local_mid, Local_mid) == 0) {	/* CERN Boissat */
			printf("%s %s \n",DSP_ERR4, DSP_ERR5);
			return(-2);
		}
	}
#ifdef SDSC
	else if (!(flags & SHO_TSCHED)) 
#else
	else
#endif
		selector = whom;



	/* step 11. print requests */
	do {
#ifdef SDSC
		seekdb (qcomplexfile, 0);	/* rewound to zero */
#endif
		list(queuefile, qcomplexfile, *argv, flags, selector,qmapfile,
			pipeqfile);
		if (*argv != NULL)
			argv++;
	} while (*argv != NULL);


	/* step 12.  Flush any output buffers and exit.  */
	fflush (stdout);
	fflush (stderr);
	closedb(qcomplexfile);
	closedb(queuefile);
	closedb(qmapfile);
	closedb(pipeqfile);
	return (0);
}


/*** read_j_table 
 * only for UNICOS needs
 */
read_j_table()
{
#if UNICOS
                register int fd;
                struct nlist nl[2];

                nl[0].n_name = "jtab";
                nl[1].n_name = "";
                fd = open("/dev/kmem", 0);
                if (fd < 0 ||
                    nlist( "/unicos", nl ) < 0 ||
                    lseek( fd, nl[0].n_value, 0 ) < 0 ||
                    read( fd, (char*)jtab, sizeof(jtab) ) != sizeof(jtab)) {
                        fprintf(stderr, "Unable to read job table.\n");
                }
                close(fd);
#endif
}


/*** set_sflag
 *
 * show specific queues that are in specific state
 */
set_sflag(optarg)
char * optarg;
{
	int i;
	long flags=0; /* CERN Boissat */

	for (i=0; (*optarg != NULL); i++, optarg++) {
		switch(*optarg) {
		case 'h':
			flags |= SHO_RS_HOLD;
			break;
		case 'r':
			flags |= SHO_RS_RUN;
			break;
		case 'q':
			flags |= SHO_RS_QUEUED;
			break;
		case 't':
			flags |= SHO_RS_EXIT;
			break;
		default:
			printf("error \n");
		}	/* end switch */
	}		/* end for    */
	return(flags);
}


/***parse_args
 *
 * parse args for commands
 */
long 
parse_args(argc, argv, string,selector,tname,hname)
  int  *argc;
  char *argv[];
  char * string;
  char ** selector;
  char ** tname;
  char ** hname;
{
	extern 		char	*optarg;
	extern 		int 	optind;
	register	int	c;
	register	int	i;
	register	long	flags=0;
	register	int	state=0;
#ifdef SDSC
        char *bp;
        int inp;
#endif

	while ((c=getopt(*argc, argv, string)) !=-1) {
		switch (c) {
#if UNICOS
		case '*':
			scpflag = strtol(++argument, &argument, 10);
			argument--;
			break;
#endif
		case 'b':		/* batch q */
			flags |= SHO_BATCH;
			break;
		case 'p':		/* pipe q */
			flags |= SHO_PIPE;
			break;
		case 'd':		/* device q */
			flags |= SHO_DEVICE;
			break;
		case 'f':		/* full display */
		case 'l':		/* long display */
			flags |= SHO_FULL;
			break;
		case 'L':		/* rescources */
			flags |= SHO_LIMITS;
			break;
		case 'n':		/* no header/trailer */
			flags |= SHO_NOHDR;
			break;
		case 'x':		/* extended by vendor */
			flags |= SHO_EXTEND;
			break;
		case 'h':		/* extended by vendor */
			if (flags & SHO_HOST){
				printf(" %s -h \n",DSP_ERR3);
				exit(2);
			}
			flags |= SHO_HOST;
			*hname = optarg;
			break;
		case 's':		/* extended by vendor */
			state++;
			flags |= set_sflag(optarg);
			break;
		case 'T':		/*target uname in conjunction w/ host*/
			if (flags & SHO_TARGET){
				printf(" %s -T \n",DSP_ERR3);
				exit(2);
			}
			flags |= SHO_TARGET;
			*tname = optarg;
			break;
#ifdef SDSC
		case 't':		/* list tennis court time slot */
                	bp = optarg;
                	while ((inp = (int) *bp) != '\0') {
                    	    if (isdigit (inp) == 0) {
				printf ("-t value out of range, must be 0-6\n");
				exit (2);
			    }
                    	    bp++;
                	}
			if (optarg != NULL) {
			    if (atoi (optarg) < 0 || atoi (optarg) > 6) {
				printf ("-t value out of range, must be 0-6\n");
				exit (2);
			    }
			}
			flags |= SHO_TSCHED;
			*selector = optarg;
			break;
                case 'v':   /* verbose output (132 characters per line) */
                        flags |= SHO_VERBOSE;
                        break;
#endif
		case 'A':		/* account list */
			if ((flags & SHO_GROUP) || (flags & SHO_ACCT)
			|| (flags & SHO_USER) || (flags & SHO_R_ALLUID)) {
				printf(" %s \n", DSP_ERR1);
				printf(" %s \n", DSP_ERR2);
				exit(2);
			}
			flags |= SHO_ACCT;
			*selector = optarg;
			break;
		case 'g':		/* group list */
			if ((flags & SHO_GROUP) || (flags & SHO_ACCT)
			|| (flags & SHO_USER) || (flags & SHO_R_ALLUID)) {
				printf(" %s \n", DSP_ERR1);
				printf(" %s \n", DSP_ERR2);
				exit(2);
			}
			flags |= SHO_GROUP;
			*selector = optarg;
			break;
		case 'u':		/* user list */
			if ((flags & SHO_GROUP) || (flags & SHO_ACCT)
			|| (flags & SHO_USER) || (flags & SHO_R_ALLUID)) {
				printf(" %s \n", DSP_ERR1);
				printf(" %s \n", DSP_ERR2);
				exit(2);
			}
			flags |= SHO_USER;
			*selector = optarg;
			break;
		case 'a':		/* show all */
		case 'U':		/* Unrestricted display */
			if ((flags & SHO_GROUP) || (flags & SHO_ACCT)
			|| (flags & SHO_USER)) {
				printf(" %s \n", DSP_ERR1);
				printf(" %s \n", DSP_ERR2);
				exit(2);
			}
			flags |= SHO_R_ALLUID;
			break;
		case 'Q':		/* queue list */
			flags |= (SHO_BATCH | SHO_QUE);
			break;
		case '?':
			fprintf (stderr, "Invalid option flag ");
			fprintf (stderr, "specified.\n");
#ifdef SDSC
			if (0 == strncmp(argv[strlen(argv[0])-5], "qstat",5)){
			  fprintf (stderr, "Command format is:\n\n");
			  fprintf (stderr, "qstat [-a] [-b] [-v] [-p] [-f] [-l] [-n] [-t] [-U] [-s]\n");
			  fprintf (stderr, " [-h host-name ] [-u user-name ] [ queue-name[@host-name]]\n");
			}
			else {
                          fprintf (stderr, "Command format is:\n\n");
                          fprintf (stderr, "    %s %s",argv[0],string);
                          fprintf (stderr, "[ <list-name(s)> ]\n\n");
			}
#else
			fprintf (stderr, "Command format is:\n\n");
			fprintf (stderr, "    %s %s",argv[0],string);
			fprintf (stderr, "[ <list-name(s)> ]\n\n");
#endif
			exit (7);
		}
	}

	/* step 5. move argv up to where we stopped parsing */
	*argc=optind;

	/* if no specific type is mentioned then show all types */

	/* Well, on second thought, the default case will be to show */
	/* requests, and that will be taken care of by qstat.c. TS   */
        /*
	 * if ( (! (flags & SHO_BATCH)) && (! (flags & SHO_PIPE)) && 
	 *      (! (flags & SHO_DEVICE)) )
	 * {
	 *      flags |= ( SHO_BATCH | SHO_PIPE | SHO_DEVICE );
	 * }
	 */

#ifdef SDSC
	/* No other options can be used with the -t option */

	if (flags & SHO_TSCHED) {
	    if (flags != SHO_TSCHED) {
		printf ("No other option can be used with the -t option\n");
		exit (2);
	    }
	}
#endif

	/* step 6. no special state so show all */
	if (state == 0)
		flags |= (SHO_RS_RUN | SHO_RS_EXIT | SHO_RS_HOLD |
			  SHO_RS_QUEUED);

	return(flags);
}
