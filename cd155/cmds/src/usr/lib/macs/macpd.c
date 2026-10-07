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
 
/*****************************************************************************
 *              Copyright (c) 1990 San Diego Supercomputer Center.
 *              All rights reserved.  The SDSC software License Agreement
 *              specifies the terms and conditions for redistribution.
 *
 * File:	macpd.c
 *
 *
 * Abstract:	This is the persistent daemon for main MACS
 *		daemon (macd).  It forks a child process to run macd and
 *		watches it all the time. If the macd dies for any reason
 *		it will fork and restart macd.  It logs everything to the
 * 		log file (macpd.log).  In case the MACD dies very often
 *		that is if the interval between the starts drop below
 *		a predefined value, it will double the previous waiting
 *		time before every restart of the MACD and it also will
 *		send an email message to inform the system administrator.
 *
 *****************************************************************************/

#if !defined(lint)
static char     sccs_id[] = "@(#)macpd.c	1.8 (MACPD GJK) 4/13/92";
#endif

#include <sys/param.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <syslog.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <utime.h>
#include <strings.h>
#include "conf.h"
#include "macpd.h"
#include "filename.h"
#include "macd.h"
#include "mac.h"
#include "db.h"

FILE           *LogFile;
char            LogFilePath[132];
struct macsconf *conf;

/*****************************************************************************
*
*
*  SignalHandler
*
*
*****************************************************************************/
static void
SignalHandler(recv_sig)
   int             recv_sig;
{
   extern char    *timstr();

   (void) signal(recv_sig, SignalHandler);
#ifdef SDSC
   if (recv_sig == SIGALRM) {
      if (utime (HEART_BEAT_FNAME, (struct utimbuf *)NULL) != 0) {
	 (void) fprintf(LogFile,
            "%s   MACPD failed to touch %s\n",timstr(0),HEART_BEAT_FNAME);
      }
      (void) alarm (60);
   } else   
#endif
   {
   (void) fprintf(LogFile, "%s   Macpd received signal %d\n", 
	timstr(0), recv_sig);
   (void) fflush(LogFile);
   }
   if (recv_sig == SIGKILL) {	/* or more ? */
      KillChildProcess();
      (void) syslog (LOG_ERR, "received SIGKILL, exiting");
      (void) fprintf(LogFile, "%s   Macpd exiting\n", timstr(0));
      (void) fflush(LogFile);
      exit(0);
   }
}


/*****************************************************************************
*
*
*   main
*
*
*****************************************************************************/
main(argc, argv)
   int             argc;
   char           *argv[];
{
   int		i, fd, ttyfd, wait_status;
   int		ret;
   char		*db_findgood_err;
   struct stat	filestat;
   char         srvr_string[132];
   char         log_msg[512];
   int          pid_tmp, total_died = 0, email_admin = 0, nterms;
   time_t       prev_time = 0, curr_time = 0;
   unsigned     sleep_time = SLEEP_TIME, time_diff = TIME_DIFF, uiDidntSleepYet;
   extern char  *timstr();
   extern int   ttywrite(), openlog(), syslog();

   (void) openlog("macpd", LOG_CONS|LOG_NOWAIT, LOG_DAEMON);

   /* check critical files, if problem, write to terminal or 
      send email to root */
   if (access(MACS_CONF_FILE, F_OK) == 0) {
	if (stat(MACS_CONF_FILE, &filestat) == 0) {
	    if (filestat.st_size <= 0) {
                (void) sprintf (log_msg, 
                    "%s is 0 length, MACS stopped", MACS_CONF_FILE);
                (void) fprintf (stderr, "%s\n", log_msg);
		(void) fflush(stderr);
		(void) syslog (LOG_ERR, log_msg);
		(void) mailroot (MACSCONF_0);
		exit(1);
	    }
	   else if ((conf = readconf()) == NULL) {
	      switch (errno) {
		case ENOENT: (void) sprintf(log_msg,
	        		"Fail in opening %s, MACS cannot start", 
				MACS_CONF_FILE);
			     break;
		case ENOMEM: (void) sprintf(log_msg,
	        		"Fail in malloc() in readconf(), MACS cannot start");
			     break;
		case EINVAL: (void) sprintf(log_msg,
	        		"Invalid enforcement parameter in %s, MACS cannot start", 
				MACS_CONF_FILE);
			     break;
		default: (void) sprintf(log_msg,
	        	"Unknown error returned from readconf(), MACS cannot start");
	      }
	      (void) fprintf (stderr, "%s\n", log_msg);
	      (void) fflush(stderr);
	      (void) mailroot (MACSCONF_ERR);
	      (void) syslog (LOG_ERR, log_msg);
	      exit(1);
	   }
	}
	else {
            (void) sprintf (log_msg, 
                "Failed in getting %s status, MACS stopped", MACS_CONF_FILE);
            (void) fprintf (stderr, "%s\n", log_msg);
	    (void) fflush(stderr);
	    (void) syslog (LOG_ERR, log_msg);
	    (void) mailroot (MACSCONF_STAT);
	    exit(1);
	}
   }
   else {
        (void) sprintf (log_msg, 
            "%s does not exist, MACS stopped", MACS_CONF_FILE);
        (void) fprintf (stderr, "%s\n", log_msg);
	(void) fflush(stderr);
	(void) syslog (LOG_ERR, log_msg);
	(void) mailroot (MACSCONF_NONE);
	exit(1);
   }

   if (gethostname (log_msg, 256) < 0 ||
	gethostbyname(log_msg) == NULL || gethostbyname("localhost") == NULL) {
        (void) syslog(LOG_ERR, 
	"Error in getting local host name, please check /etc/hosts.  MACS stopped");
	(void) fprintf(stderr, "Error in getting local host name, please check /etc/hosts\n");
	(void) fprintf(stderr, "MACS stopped\n");
	(void) fflush(stderr);
	exit(1);
   }
   if (access(NXACCOUNT_FNAME, F_OK) == 0) {
	if (stat(NXACCOUNT_FNAME, &filestat) == 0) {
	    if (filestat.st_size <= 0) {
		(void) sprintf (log_msg, 
		    "%s is 0 length, MACS stopped", NXACCOUNT_FNAME);
		(void) fprintf (stderr, "%s\n", log_msg);
		(void) fflush(stderr);
                (void) syslog (LOG_ERR, log_msg);
		(void) mailroot (NXACCT_0);
		exit(1);
	    }
	}
	else {
            (void) sprintf (log_msg, 
                "Failed in getting %s status, MACS stopped", NXACCOUNT_FNAME);
            (void) fprintf (stderr, "%s\n", log_msg);
	    (void) fflush(stderr);
            (void) syslog (LOG_ERR, log_msg);
	    (void) mailroot (NXACCT_STAT);
	    exit(1);
	}
   }
/*
   else {
	(void) sprintf (log_msg, 
	    "%s does not exist, MACS stopped\n", NXACCOUNT_FNAME);
	(void) ttywrite("root", log_msg);
	(void) mailroot (NXACCT_NONE);
	exit(1);
   }
*/

   /*
    * Check nqstable file.  No nqstable file is ok.  But if the file exist,
    * it cannot be empty, unreadable or ill-formatted.
    */
   if (access(MACS_NQSTABLE_FILE, F_OK) == 0) {
	FILE *qfd;
	int n_queue;
	char buf[128], name[64];
	double rate;
	if ((qfd = fopen (MACS_NQSTABLE_FILE, "r")) <= NULL) { 
            (void) sprintf (log_msg,
                "%s is unreadable, MACS cannot start", MACS_NQSTABLE_FILE);
            (void) fprintf (stderr, "%s\n", log_msg);
	    (void) fflush(stderr);
	    (void) syslog (LOG_ERR, log_msg);
	    (void) mailroot (NRD_NQSTABLE);
	    exit(1);
	}
	if (fgets(buf, 128, qfd) <= NULL) {
            (void) sprintf (log_msg,
                "%s is empty, MACS cannot start", MACS_NQSTABLE_FILE);
            (void) fprintf (stderr, "%s\n", log_msg);
	    (void) fflush(stderr);
	    (void) syslog (LOG_ERR, log_msg);
	    (void) mailroot (NUL_NQSTABLE);
	    exit(1);
	}
	if (sscanf (buf, "%d", &n_queue) != 1) {
            (void) sprintf (log_msg,
                "Format error in %s, first line should be an integer.  MACS cannot start", 
                MACS_NQSTABLE_FILE);
            (void) fprintf (stderr, "%s\n", log_msg);
	    (void) fflush(stderr);
	    (void) syslog (LOG_ERR, log_msg);
            (void) mailroot (CNT_NQSTABLE);
            exit(1);
        }
	for (i=0; fgets (buf, 128, qfd) > NULL; i++) {
	    if (sscanf (buf, "%s %f", name, &rate) != 2) {
                (void) sprintf (log_msg, 
                    "Format error in %s line %d.  MACS cannot start", 
            	    MACS_NQSTABLE_FILE, i+2);
                (void) fprintf (stderr, "%s\n", log_msg);
	        (void) fflush(stderr);
		(void) syslog (LOG_ERR, log_msg);
                (void) mailroot (FMT_NQSTABLE);
                exit(1);
            }
	}
	if (i != n_queue) {
	    (void) sprintf (log_msg,
                "Queue count stated differs from queues defined in %s.  MACS cannot start",

		MACS_NQSTABLE_FILE);
	    (void) fprintf (stderr, "%s\n", log_msg);
	    (void) fflush(stderr);
	    (void) syslog (LOG_ERR, log_msg);
	    (void) mailroot (DIF_NQSTABLE);
	    exit(1);
	}
   }

   if (conf->macdmode != ACCTONLY) {
	/*
	  call db_findgood() to get a valid MACD_DATABASE_FILE
	 */

	db_findgood_err = malloc(2048);
	ret = db_findgood(db_findgood_err);
	switch(ret) {
	case 0: /* no errors */
		break;

	case 1: /* database recovered from backup */
	        (void) fprintf (stderr, "Note: MACS database recovered from backup\n%s",
			db_findgood_err);
		(void) fflush(stderr);
	        (void) ttywrite(conf->operator, "Note: MACS database recovered from backup\n");
	        (void) syslog (LOG_NOTICE, "MACS database recovered from backup\n");
	        (void) syslog (LOG_NOTICE, db_findgood_err);
	        (void) mailroot (MACDDB_RECOVERED);
	        free(db_findgood_err);
		break;

	case -1: /* no usable database found */
	        (void) fprintf (stderr, "No %s file found; cannot start MACD.\n%s",
			MACD_DATABASE_FILE, db_findgood_err);
		(void) fflush(stderr);
	        (void) ttywrite(conf->operator, "No MACS database found; cannot start MACD.\n");
	        (void) syslog (LOG_ERR, "No MACS database found; cannot start MACD.\n");
	        (void) syslog (LOG_ERR, db_findgood_err);
	        (void) mailroot (MACDDB_NONE);
	        free(db_findgood_err);
		exit(1);

	default: /* unrecognized return code */
	        (void) fprintf (stderr, "Unexpected return code from db_findgood, cannot start MACD\n%s",
			MACD_DATABASE_FILE, db_findgood_err);
		(void) fflush(stderr);
	        (void) ttywrite(conf->operator, "Unexpected return code from db_findgood, cannot start MACD\n");
	        (void) syslog (LOG_ERR, "Unexpected return code from db_findgood, cannot start MACD\n");
	        (void) syslog (LOG_ERR, db_findgood_err);
	        (void) mailroot (MACDDB_NONE);
	        free(db_findgood_err);
		exit(1);
	}
	free(db_findgood_err);
   }

   /* validate command-line arguments */
   for (i = 1; i < argc; i++)
      if (!strcmp(argv[i], "-debug"))
	 continue;
      else if (!strcmp(argv[i], "-nosmd"))
	 continue;
      else if (!strcmp(argv[i], "-acctonly"))
	 conf->macdmode = ACCTONLY;
      else if (!strcmp(argv[i], "-daily"))
	 conf->switchlog = LOGDAILY;
      else if (!strcmp(argv[i], "-weekly"))
	 conf->switchlog = LOGWEEKLY;
      else if (!strcmp(argv[i], "-monthly"))
	 conf->switchlog = LOGMONTHLY;
      else {
	 (void) fprintf(stderr, 
	    "Usage: macpd [-debug] [-nosmd] [-acctonly] [-daily] [-weekly] [-monthly]\n");
	 exit(0);
      }

   /* show configuration parameters
   (void) printf("\nMACS Configuration Parameters\n");
   (void) printf("\nConfiguration File:   %s\n\n", MACS_CONF_FILE);
   (void) printf("\tmaximum compute-partition size:   %d\n", conf->total_nodes);
   (void) printf("\tmail program:   %s\n", conf->mailer ? conf->mailer : "NULL");
   (void) printf("\tadministrator:   %s\n", conf->admin ? conf->admin : "NULL");
   (void) printf("\toperator:   %s\n", conf->operator ? conf->operator : "NULL");
   (void) printf("\tswitch logfile:   %s\n", conf->switchlog ?
		 ((conf->switchlog - 1) ? "monthly" : "weekly") : "daily");
   (void) printf("\tmacd mode:   %s\n", conf->macdmode ?
		 "control resource usage" : "accounting-only");
   (void) printf("\tenforcement level:  ");
   if (conf->enforce) {
      if (conf->enforce & ACCTKILL && conf->enforce & USERKILL)
	 (void) printf("Both account and user levels\n");
      else if (conf->enforce & ACCTKILL)
	 (void) printf("Account level only\n");
      else if (conf->enforce & USERKILL)
	 (void) printf("User level only\n");
      else
	 (void) printf("INVALID\n");
   } else
      (void) printf("None\n");
   (void) printf("\tcommand-line options:\n");
   for (i = 1; i < argc; i++)
      (void) printf("\t\t\t%s\n", argv[i]);
 */
   errno = 0;

   pid_tmp = fork();
   if (pid_tmp) {
      if (errno) {
	 (void) perror("macpd: ");
      } else {
      }
      exit(0);
   }
   (void) sprintf(srvr_string, "%s", MACS_MACD_FNAME);
   (void) sprintf(LogFilePath, "%s/macpd.log", MACS_LOG_PATH);

   for (fd = 3; fd < NOFILE; fd++)
      (void) close(fd);
 
   fd = open("/dev/null", 1);
   (void) openlog("macpd", LOG_CONS|LOG_NOWAIT, LOG_DAEMON);
   LogFile = fopen(LogFilePath, "a");

   if (!LogFile) {
      (void) sprintf(log_msg,
         "Couldn't open logfile %s.  MACS cannot start", LogFilePath);
      (void) printf("Macpd: %s\n", log_msg);
      (void) printf("Macpd: Exiting\n");
      (void) syslog (LOG_ERR, log_msg);
      (void) mailroot (MACPDLOG_ERR);
      exit(1);
   }

   /*
    * change our process group to disassociate ourselves from the terminal
    * (System V, but it doesn't hurt for BSD)
    */

#if !defined(IPSC)
   /*
    * BSD style systems
    */
   (void) setpgrp(0, getpid());
#else
   /*
    * System V style systems
    * 
    */
   (void) setpgrp();
#endif

   (void) close(0);		/* close stdin */
   (void) close(1);		/* close stdout */

   if ((ttyfd = open("/dev/tty", O_RDWR)) >= 0) {
      (void) close(ttyfd);
   }
   (void) signal(1, SignalHandler);
   (void) signal(2, SignalHandler);
   (void) signal(3, SignalHandler);
   (void) signal(4, SignalHandler);
   (void) signal(5, SignalHandler);
   (void) signal(6, SignalHandler);
   (void) signal(7, SignalHandler);
   (void) signal(8, SignalHandler);
   (void) signal(9, SignalHandler);
   (void) signal(10, SignalHandler);
   (void) signal(11, SignalHandler);
   (void) signal(12, SignalHandler);
   (void) signal(13, SignalHandler);
   (void) signal(14, SignalHandler);
   (void) signal(15, SignalHandler);
   (void) signal(16, SignalHandler);
   (void) signal(17, SignalHandler);
   (void) signal(18, SignalHandler);
   (void) signal(19, SignalHandler);
   (void) signal(20, SignalHandler);
   (void) signal(21, SignalHandler);
   (void) signal(22, SignalHandler);
   (void) signal(23, SignalHandler);
   (void) signal(24, SignalHandler);
   (void) signal(25, SignalHandler);
   (void) signal(26, SignalHandler);
   (void) signal(27, SignalHandler);
   (void) signal(28, SignalHandler);
   (void) signal(29, SignalHandler);
   (void) signal(30, SignalHandler);
   (void) signal(31, SignalHandler);
   (void) signal(32, SignalHandler);


   for (;;) {

      pid[SRVR_PID] = StartChildProcess(srvr_string, argc, argv, fd, 1);

      if (pid[SRVR_PID] < 0) {
	 (void) fprintf(LogFile,
		"%s   StartChildProcess(srvr_string, argc, argv, fd,1) < 0, srvr_string=%s\n",
		timstr(0), srvr_string);
	 (void) fflush(LogFile);
      } else {
	 if (pid[SRVR_PID] == 0) {
	    (void) fprintf(LogFile,
		"%s   StartChildProcess(srvr_string, argc, argv, fd,1) == 00, srvr_string=%s\n",
		timstr(0), srvr_string);
	    (void) fflush(LogFile);
	 }
      }

#ifdef SDSC
      (void) alarm (60);
#endif
      while (wait(&wait_status) == -1 && errno == EINTR) {
	 (void) fprintf(LogFile,
	    "%s   MACPD wait received interrupt, wait_status=%d\n",
		timstr(0), wait_status);
	 (void) fflush(LogFile);
      }
      (void) fprintf(LogFile,
	"%s   MACD (pid=%d) is terminated with signal %d, wait_status=%d\n",
	      timstr(0), pid[SRVR_PID], WTERMSIG(wait_status), wait_status);
      (void) fflush(LogFile);
      total_died++;
      if ((int) fmod(total_died, 5) == 0) {
	 /* time to warn admin and operator about a potential problem */
         (void) syslog (LOG_ERR, "Failed to start MACD");
	 if (conf->operator && strcmp(conf->operator, "")) {
	    nterms = ttywrite(conf->operator, "Cannot start MACD\n");
	    if (nterms <= 0)
	       (void) fprintf(LogFile,
		 "%s   Fail in writing to operator's console\n", timstr(0));
	    else
	       (void) fprintf(LogFile,
		    "%s   Send message to operator's console\n", timstr(0));
	 }
	 if (!email_admin && conf->mailer != NULL) {
	    FILE           *mail;
	    char            buf[BUFSIZ];
	    (void) sprintf(buf, "%s %s", conf->mailer, conf->admin);
	    if ((mail = popen(buf, "w")) != (FILE *) NULL) {
	       (void) setbuf(mail, buf);
	       (void) fprintf(mail, "To admin:\n");
	       (void) fprintf(mail, "MACD die too often!\n");
	       (void) fprintf(mail, "More information can be find in %s\n",
			      LogFilePath);
	       (void) fprintf(mail, "\n\nGood Luck!\n\nFrom macpd\n");
	       (void) pclose(mail);
	       email_admin = 1;
	       (void) sprintf(log_msg, "Email to %s about macd die",
			      conf->admin);
	    } else
	       (void) sprintf(log_msg, "Fail open pipe to mail to %s about macd die",
			      conf->admin);
	    (void) fprintf(LogFile, "%s   %s\n", timstr(0), log_msg);
	    (void) fflush(LogFile);
	 }
      }
      (void) fprintf(LogFile,
		     "%s   MACS daemon died now. It died %d times so far.\n",
		     timstr(0), total_died);
      (void) fflush(LogFile);
      curr_time = time((time_t) NULL);

      if (curr_time - prev_time < time_diff) {
	 time_diff = time_diff << 1;
	 sleep_time = sleep_time << 1;
	 if (sleep_time > MAX_SLEEP_TIME) {
	    sleep_time = MAX_SLEEP_TIME;	/* use maximum value for
						 * retry */
	    time_diff = MAX_SLEEP_TIME + TIME_DIFF;
	 }
	 (void) fprintf(LogFile,
			"%s   Forking new macd too often! New sleep_time=%d, diff_time = %d\n",
			timstr(0), sleep_time, time_diff);
	 (void) fprintf(LogFile, "%s   MACD died %d times so far.\n",
			timstr(0), total_died);
	 (void) fflush(LogFile);
      } else {
	 email_admin = 0;
	 time_diff = TIME_DIFF;
	 sleep_time = SLEEP_TIME;	/* set to default */
	 (void) fprintf(LogFile, "%s   New sleep_time=%d, diff_time=%d \n",
			timstr(0), sleep_time, time_diff);
	 (void) fflush(LogFile);
      }

      uiDidntSleepYet = sleep(sleep_time);
      while (uiDidntSleepYet > 0) {
	 /* sleep didn't finished because of signal */
	 uiDidntSleepYet = uiDidntSleepYet - 1;
	 /* minus 1 second to awoid sleep for ever in the infinite loop */
	 uiDidntSleepYet = sleep(uiDidntSleepYet);
	 /* was it because of signal ? (finished uiDidntSleepYet == 0) */

      }
      prev_time = curr_time;
   }
}

/*****************************************************************************
*
*
*   StartChildProcess
*
*
*****************************************************************************/
int
StartChildProcess(Path, argc, argv, OutputFD, Duplicate_it)
   char           *Path;
   int             argc;
   char           *argv[];
   int             OutputFD;
   int             Duplicate_it;
{
   int             ForkPID, i;
   char          **gargv;
   char            log_msg[256], cmd_line[256];
   extern char    *timstr();
   extern char    *strcpy(), *strcat();

   if ((gargv = (char **) malloc((argc + 2) * sizeof(char *))) == NULL) {
      (void) fprintf(stderr, "macpd():  Fail in memory allocation for gargv\n");
      exit(1);
   }
   for (i = 1; i < argc + 2; i++) {
      gargv[i] = (char *) NULL;
   }

   gargv[0] = Path;

   (void) strcpy(cmd_line, "Starting  : '");
   (void) strcat(cmd_line, gargv[0]);

   for (i = 1; i < argc; i++) {
      gargv[i] = argv[i];
      (void) sprintf(log_msg, " %s", gargv[i]);
      (void) strcat(cmd_line, log_msg);
   }

   (void) strcat(cmd_line, "'");
   (void) fprintf(LogFile, "%s   %s\n", timstr(0), cmd_line);
   (void) fflush(LogFile);
   switch (ForkPID = fork()) {
   case -1:

      return (-1);

   case 0:			/* We are the child  */

      /*
       * we set our process group equal to our PID, either implicitly (System
       * V) or explicitly (BSD)
       */
#if !defined(IPSC)
      /*
       * BSD style systems
       */
      (void) setpgrp(0, getpid());
#else
      /*
       * System V style systems
       * 
       */
      (void) setpgrp();
#endif

      if (Duplicate_it) {
	 (void) dup2(OutputFD, 1);
	 (void) dup2(OutputFD, 2);
      }
      (void) close(OutputFD);

      (void) execv(gargv[0], gargv);
      _exit(1);
      break;

   default:			/* we are the parent  */

      break;
   }

   return (ForkPID);
}

/*****************************************************************************
*
*
*   KillChildProcess
*
*
*****************************************************************************/
int
KillChildProcess()
{
   extern char    *timstr();

   (void) fprintf(LogFile, "%s   Killing the macd server\n", timstr(0));
   (void) fflush(LogFile);

   if (pid[SRVR_PID]) {

      /*
       * first try to shutdown macd gracefully but may hang macpd if macd
       * hang (void) fprintf(LogFile, "%s   sent C_OFF to shutdown macd\n",
       * timstr(0)); if (c_open() < 0 || cpuctl (C_OFF, NULL) < 0) { (void)
       * fprintf(LogFile, "%s   ERROR:  Unable to send C_OFF to MACD\n"); }
       * (void) c_close();
       */

      (void) fprintf(LogFile, "%s   Sent signal SIGTERM to SRVR_PID pg=%d\n",
		     timstr(0), pid[SRVR_PID]);
      (void) fflush(LogFile);

#ifdef IPSC
      if (0 != kill(pid[SRVR_PID], SIGTERM)) {
	 (void) fprintf(LogFile, "%s   kill(%d,%d) failed, errno=%d\n",
			timstr(0), pid[SRVR_PID], SIGTERM, errno);
	 (void) fflush(LogFile);
      }
#else
      if (0 != killpg(pid[SRVR_PID], SIGTERM)) {
	 (void) fprintf(LogFile, "%s   killpg(%d,%d) failed, errno=%d\n",
			timstr(0), pid[SRVR_PID], SIGTERM, errno);
	 (void) fflush(LogFile);
      }
#endif
   }
   return (0);
}
