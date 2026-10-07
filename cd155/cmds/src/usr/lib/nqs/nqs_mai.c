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
 
/*++ nqs_mai.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_mai.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the two functions:
 *
 *		mai_send(),	and
 *		mai_outfiles()
 *
 *	which respectively send an NQS mail message (for request completion
 *	events), and diagnose the need to send mail concerning the disposition
 *	of output file events (for batch requests).
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	May 2, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:52:59 $ $State: Exp $)
 * $Log: nqs_mai.c,v $
 * Revision 1.4  1994/11/19  02:52:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/10/07  00:46:45  kremenek
 *  Reviewer:  davidl doyle
 *  Risk: Low
 *  Benefit or PTS #: 5706
 *  Testing: EATS
 *  Module(s): cmds_libs/src/usr/lib/nqs/netdaemon.c, nqs_abort.c,
 *             nqs_boot.c, nqs_mai.c, nqs_main.c
 *
 * Revision 1.2  1992/10/09  22:25:39  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:11  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:06:41  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_mai.c	50.2 (nqs_mai.c OSF/1 NQS2.0 GJK) 9/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "nqs.h"			/* NQS types and defns */
#include "nqsmail.h"			/* NQS mail definitions */
#include "requestcc.h"			/* NQS request completion codes */
#include "nqsxvars.h"			/* Global vars */
#define	MAX_SUBJECT_SUFFIX	50	/* Maximum length of subject suffix */

#ifdef SDSC
#include <syslog.h>
#endif

/*
 *	External functions:
 */

extern void analyzercm();		/* Analyze and display request */
					/* completion diagnostics */
extern int errno;			/* System call error codes */
extern struct passwd *fetchpwuid ();	/* Fetch password entry by user-id */
extern char *fmtmidname();		/* Format name for machine-id */
extern char *fmttime();			/* Return formatted time string */
extern char *namstdlog();		/* Return formatted time string */
extern char *nmap_get_nam();		/* Get name for machine-id */
extern char *strcat();			/* String concatenate */
extern int strlen();			/* String length */
extern time_t time();			/* Get GMT time */


/*** mai_send
 *
 *
 *	int mai_send():
 *
 *	Send mail regarding a specific NQS request and WAIT for
 *	the mail process to exit.
 *
 *	WARNING:
 *		Nqs_mail() must NEVER be called at any time
 *		by the NQS daemon after the rebuild operation
 *		is complete, since this mail procedure waits
 *		for the spawned mail process to exit, which
 *		could result in the waiting for an exited
 *		request shepherd process, rather than the
 *		mail process.
 *
 *	Returns:
 *		0: if mail sent (does not guarantee delivery);
 *	       -1: if unsuccessful (diagnostic text is written
 *		   to the logfile).
 */
int mai_send (rawreq, requestlog, suffixcode)
register struct rawreq *rawreq;		/* Raw request structure for req */
register struct requestlog *requestlog;	/* Mail request log structure */
					/* (Can be NIL if suffixcode is */
					/*  one of MSX_BEGINNING or */
					/*  MSX_RESTARTING) */
int suffixcode;				/* Subject suffix code */
{
	static char no_mailsent[]= "I$No mail sent.\n";
	static char lostbycrash[]= "information lost due to a system crash.\n";
	static char *suffixtab []= {
		"aborted",		/* MSX_ABORTED */
		"aborted for NQS shutdown",
					/* MSX_ABORTSHUTDN */
		"beginning",		/* MSX_BEGINNING */
		"deleted",		/* MSX_DELETED */
		"delivered",		/* MSX_DELIVERED */
		"ended",		/* MSX_ENDED */
		"failed",		/* MSX_FAILED */
		"requeued",		/* MSX_REQUEUED */
		"restarting",		/* MSX_RESTARTING */
#ifdef SDSC
                "time exceeded",        /* MSX_TIMEOUT */
		"bad tennis court schedule",	/* MSX_BADTSCHED */
#endif
		"status"		/* Bad MSX value maps to here */
	};
	static char stderr_dest[]  = "  Destination: -e %s:%s\n";
	static char stderr_dispos[]= "\n  Stderr return disposition ";
	static char stderr_status[]= "\n  Stderr file staging event status:\n";
	static char stdout_dest[]  = "  Destination: -o %s:%s\n";
	static char stdout_dispos[]= "\n  Stdout return disposition ";
	static char stdout_status[]= "\n  Stdout file staging event status:\n";

	char maildest [MAX_ACCOUNTNAME+1+MAX_MACHINENAME+1];
	char subject [19 + MAX_MACHINENAME + MAX_SUBJECT_SUFFIX + 2];
				/* Subject line for mail */
				/* 13 chars for "NQS request: "; */
				/* 5  chars for request-id #; */
				/* 1  char for "." between  */
				/*    request-id# and the machine */
				/*    name; */
				/* MAX_MACHINENAME chars for */
				/*    machine name; */
				/* MAX_SUBJECT_SUFFIX chars for */
				/*    the subject suffix; */
				/* 1  char for trailing "."; */
				/*    and */
				/* 1  delimiting null char. */
	int i;			/* Work var */
	char *machinename;	/* Machine name */
	FILE *mailfile;		/* Mail file */
	uid_t mail_account;	/* Account used to send mail */
	int p[2];		/* Read/write pipe descriptors */
	int pid;		/* Process-id of child */
	struct passwd *pw_entry;/* Password file entry for NQS mail account */
	time_t timeofday;	/* Time of day */
	char *argv [3];		/* Arguments to shell */
	char *envp [5];		/* Environment variables for process that */
				/* will be used to send NQS mail */
	char home [64];		/* HOME=........................ */
#if	SGI | SYS52 | UTS | UNICOS | OSF
	char logname [30];	/* LOGNAME=..................... */
/*
 *	char mail [30];	*/	/* MAIL=/usr/mail/.............. */
/**/
#else
#if	BSD42 | BSD43 | ULTRIX
/*
 *	char mail [35]; */	/* MAIL=/usr/spool/mail/........ */
/**/
	char user [30];		/* USER=........................ */
#else
BAD SYSTEM TYPE
#endif
#endif


	/*
	 *  Build subject line.
	 */
	if (suffixcode < 0 || suffixcode > MSX_MAXMSX) {
		suffixcode = MSX_MAXMSX+1;
	}
	sprintf (subject, "NQS request:  %1ld.%s %s.",
		(long) rawreq->orig_seqno, fmtmidname (rawreq->orig_mid),
		suffixtab [suffixcode]);
	/*
	 *  Determine mail machine address.
	 */
	if (Locmid == rawreq->mail_mid) {
		/*
		 *  We are going to be sending mail to the same
		 *  machine.
		 */
		sprintf (maildest, "%s", rawreq->mail_name);
	}
	else {
		/*
		 *  Get the name of the machine to which ALL mail should be
		 *  sent concerning the NQS request, where the mail machine
		 *  DIFFERS from the local machine.
		 */
		machinename = nmap_get_nam (rawreq->mail_mid);
					/* Get hostname as viewed from */
					/* local machine */
		if (machinename == (char *) 0) {
			/*
			 *  Cannot determine host name for mid.
			 */
			printf ("E$Unable to get hostname for mid: %1ld\n",
				(long) rawreq->mail_mid);
			printf ("E$in mai_send().\n");
			printf (no_mailsent);
			fflush (stdout);
			return (-1);
		}
		sprintf (maildest, "%s@%s", rawreq->mail_name, machinename);
	}
	if (pipe (p) == -1) return (-1);
	fflush (stdout);	/* Purge any diagnostic messages prior to */
	fflush (stderr);	/* flush */
	if ((pid = fork()) == 0) {
		/*
		 *  We are child process.
		 *
		 *  Simulate enough of a login under the NQS mail
		 *  account, to cause the mail program to operate
		 *  in the necessary fashion.
		 */
		mail_account = Mail_uid;
		if ((pw_entry = fetchpwuid ((int) mail_account))
			== (struct passwd *) 0) {
			printf ("E$Non-existent mail account ");
			printf ("encountered by mai_send().\n");
			printf ("I$Using root account to send mail.\n");
			fflush (stdout);
			mail_account = 0;	/* Use root account */
			if ((pw_entry = fetchpwuid ((int) mail_account))
				== (struct passwd *) 0) {
				printf ("E$No password entry for root ");
				printf ("account in mai_send().\n");
				printf (no_mailsent);
				fflush (stdout);
				_exit (1);
			}
		}
		/*
		 *  Change directory to the home directory of the
		 *  NQS mail account (or default mail account).
		 */
		if (chdir (pw_entry->pw_dir) == -1) {
			/*
			 *  We were unable to chdir() to the home
			 *  directory of the NQS mail account.
			 */
			if (mail_account == 0) {
				printf ("E$Unable to chdir to root ");
			}
			else printf ("E$Unable to chdir() to NQS mail ");
			printf ("account home\n");
			printf ("E$directory in mai_send().\n");
			printf (no_mailsent);
			fflush (stdout);
			_exit (1);
		}
		sprintf (home, "HOME=%s", pw_entry->pw_dir);
		envp [0] = home;	/* Record the home directory */
		envp [1] = "SHELL=/bin/sh";
					/* Default shell */
#if		SGI | SYS52 | UTS | UNICOS | OSF
		envp [2] = "PATH=:/bin:/usr/bin";
		sprintf (logname, "LOGNAME=%s", pw_entry->pw_name);
		envp [3] = logname;
		envp [4] = (char *) 0;	/* No more environment variables */
#else
#if		BSD42 | BSD43 | ULTRIX
		envp [2] = "PATH=:/usr/ucb:/bin:/usr/bin";
		sprintf (user, "USER=%s", pw_entry->pw_name);
		envp [3] = user;
		envp [4] = (char *) 0;	/* No more environment variables */
#else
BAD SYSTEM TYPE
#endif
#endif
		/*
		 *  Set our user-id to the account used to send
		 *  NQS mail.
		 */
#if UNICOS & NEWPWD
		if (acctid (0,pw_entry->pw_acid[0]) == -1) {
			printf ("Invalid account ID\n");
			fflush (stdout);
			_exit (1);	
		}
#endif
		if (setuid ((int) mail_account) == -1) {
			/*
			 *  Unable to set-uid to the NQS mail account.
			 *  No mail will be sent.
			 */
			printf ("E$Unable to setuid() in mai_send().\n");
			printf (no_mailsent);
			fflush (stdout);
			_exit (1);;
		}
		close (p[1]);			/* Close writing descr */
		close (0);			/* Close stdin */
		if (fcntl (p[0], F_DUPFD, 0) == -1) _exit (1);
		/*
		 *  Note that we leave stdout and stderr open.
		 *  If mail decides to be vocal, then the logfile
		 *  will get some bad messages.
		 *
		 *  We leave stdout and stderr open anyway, because
		 *  some versions of mail(1) wrap themselves up into
		 *  a marvelous infinite loop if stdout or stderr
		 *  is closed.  Sigh....
		 */
#if	BSD42 | BSD43 | ULTRIX
		i = getdtablesize();		/* #of file descrs per proc */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
		i = _NFILE;			/* #of file descrs per proc */
#else
BAD SYSTEM TYPE
#endif
#endif
		while (--i >= 3) close (i);	/* Close all other files */
		argv [0] = "mail";
		argv [1] = maildest;
		argv [2] = (char *) 0;
#ifdef SDSC
                /* Execve() mail program */
                if((-1) == execve ("/bin/mail", argv,envp))
                {
                        (void)syslog(LOG_WARNING,
                        "NQS /bin/mail failed %m");
                }
#else
                execve ("/bin/mail", argv,      /* Execve() mail program */
                        envp);
#endif

		_exit (1);
	}
	else if (pid == -1) return (-1);	/* Fork failed */
	close (p[0]);				/* Close read descr */
	if ((mailfile = fdopen (p[1], "w")) == (FILE *) 0) {
		/*
		 *  The mail process was successfully created,
		 *  but we were unable to create a buffered
		 *  mailfile FILE stream.
		 *
		 *  Wait for the mail program to exit.
		 */
		close (p[1]);			/* Close write descr */
		while (wait ((int *)0) == -1 && errno == EINTR)
			;
		return (-1);			/* Failure */
	}
	fprintf (mailfile, "Subject:  %s\n\n", subject);
	fprintf (mailfile, "Request name:   %s\n", rawreq->reqname);
	fprintf (mailfile, "Request owner:  %s\n", rawreq->username);
	time (&timeofday);
	fprintf (mailfile, "Mail sent at:   %s\n\n", fmttime (&timeofday));
	if (requestlog != (struct requestlog *) 0) {
		/*
		 *  We are sending non-trivial mail.
		 */
		if (requestlog->reqrcmknown) {
			analyzercm (requestlog->svm.rcm, mailfile, "");
		}
		else {
			fputs ("Request completion", mailfile);
			fputs (lostbycrash, mailfile);
		}
		if (requestlog->svm.mssg [0] != '\0') {
			/*
			 *  A server message is also present.
			 */
			fprintf (mailfile, "\n%s\n", requestlog->svm.mssg);
		}
		if (rawreq->type == RTYPE_BATCH &&
		    mai_outfiles (rawreq, requestlog)) {
			/*
			 *  We are sending mail about a batch request, and
			 *  output return transaction results should be
			 *  discussed.
			 *  
			 *  Note that for the moment, the general
			 *  implementation of file staging is not
			 *  complete....
			 */
			if (rawreq->v.bat.stdout_acc & OMD_SPOOL) {
			    /*
			     *  It was necessary to return the stdout file
			     *  produced by the batch request.
			     *
			     *  Remember:	Any stdout return transaction
			     *			is file output staging event
			     *			30.
			     */
			    if (requestlog->outrcmknown & (1L << 30)) {
				/*
				 *  The results of the stdout return
				 *  transaction are known.
				 */
				if (requestlog->outrcm [30] != RCM_STAGEOUT) {
				    /*
				     *  The return of the stdout file was
				     *  not entirely successful.
				     */
				    fputs (stdout_status, mailfile);
				    fprintf (mailfile, stdout_dest,
					     fmtmidname (
						rawreq->v.bat.stdout_mid
					     ),
					     rawreq->v.bat.stdout_name);
				    analyzercm (requestlog->outrcm [30],
						mailfile, "    ");
				}
			    }
			    else {
				fputs (stdout_dispos, mailfile);
				fputs (lostbycrash, mailfile);
			    }
			}
			if ((rawreq->v.bat.stderr_acc & OMD_SPOOL) &&
			   !(rawreq->v.bat.stderr_acc & OMD_EO)) {
			    /*
			     *  It was necessary to return the stderr file
			     *  produced by the batch request.
			     *
			     *  Remember:	Any stderr return transaction
			     *			is file output staging event
			     *			31.
			     */
			    if (requestlog->outrcmknown & (1L << 31)) {
				/*
				 *  The results of the stderr return
				 *  transaction are known.
				 */
				if (requestlog->outrcm [31] != RCM_STAGEOUT) {
				    /*
				     *  The return of the stderr file was
				     *  not entirely successful.
				     */
				    fputs (stderr_status, mailfile);
				    fprintf (mailfile, stderr_dest,
					     fmtmidname (
						rawreq->v.bat.stderr_mid
					     ),
					     rawreq->v.bat.stderr_name);
				    analyzercm (requestlog->outrcm [31],
						mailfile, "    ");
				}
			    }
			    else {
				fputs (stderr_dispos, mailfile);
				fputs (lostbycrash, mailfile);
			    }
			}
		}
	}

	/* Intergraph change Bill Mar	12/08/89 TAC */
	if (rawreq->type == RTYPE_DEVICE) {
		char * logfile;
		struct stat *sbuf;
		int ret;
		int fd;

		logfile = namstdlog(rawreq->orig_seqno, rawreq->orig_mid);
		if ((stat(logfile,sbuf) != -1) &&
		    (sbuf->st_size != 0) &&
		    ((fd=open(logfile, O_RDONLY)) != -1)) {
			fprintf(mailfile, 
				"\nThe job produced the following errors:\n\n");
			fflush(mailfile);
			filecopyentire(fd,fileno(mailfile));
		}
	}
	
	fclose (mailfile);			/* Close pipe */
	/*
	 *  The mail process was successfully created.
	 *  Wait for the mail program to exit.
	 */
	while (wait ((int *)0) == -1 && errno == EINTR)
		;
	return (0);				/* Success */
}


/*** mai_outfiles
 *
 *
 *	int mai_outfiles():
 *
 *	Return non-zero if there is output file information to be
 *	analyzed.
 */
int mai_outfiles (rawreq, requestlog)
register struct rawreq *rawreq;			/* Raw request */
register struct requestlog *requestlog;		/* Request log */
{
	if (rawreq->type != RTYPE_BATCH) {
		/*
		 *  Only batch requests have any kind of output file
		 *  information.
		 */
		return (0);
	}
	/*
	 *  The request is a batch request.
	 */
	switch (requestlog->svm.rcm) {
	case RCM_ABORTED:
	case RCM_EXITED:
	case RCM_MIDUNKNOWN:
	case RCM_NOACCAUTH:
	case RCM_NORESTART:
	case RCM_SHEXEF2BIG:
	case RCM_SHUTDNABORT:
	case RCM_SSHBRKPNT:
	case RCM_SSHEXEFAI:
	case RCM_UNABLETOEXE:
	case RCM_UNCRESTDERR:
	case RCM_UNCRESTDOUT:
	case RCM_USHBRKPNT:
	case RCM_USHEXEFAI:
		if (rawreq->v.bat.stdout_acc & OMD_SPOOL) {
			/*
			 *  It was necessary to return the stdout file
			 *  produced by the batch request.
			 *
			 *  Remember:	Any stdout return transaction
			 *		is file output staging event 31.
			 */
			if (requestlog->outrcmknown & (1L << 30)) {
				/*
				 *  The results of the stdout return
				 *  transaction are known.
				 */
				if (requestlog->outrcm [30] != RCM_STAGEOUT) {
					/*
					 *  Something went wrong with the
					 *  return of the stdout file.
					 */
					return (1);
				}
			}
			else {
				/*
				 *  Information concerning the return
				 *  of the stdout file was lost.
				 */
				return (1);
			}
		}
		if ((rawreq->v.bat.stderr_acc & OMD_SPOOL) &&
		   !(rawreq->v.bat.stderr_acc & OMD_EO)) {
			/*
			 *  It was necessary to return the stderr file
			 *  produced by the batch request.
			 *
			 *  Remember:	Any stderr return transaction
			 *		is file output staging event 31.
			 */
			if (requestlog->outrcmknown & (1L << 31)) {
				/*
				 *  The results of the stderr return
				 *  transaction are known.
				 */
				if (requestlog->outrcm [31] != RCM_STAGEOUT) {
					/*
					 *  Do not say anything unless
					 *  something went wrong.
					 */
					return (1);
				}
			}
			else {
				/*
				 *  Information concerning the return
				 *  of the stderr file was lost.
				 */
				return (1);
			}
		}
		break;
	}
	return (0);			/* No mail was sent */
}
