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
 
/******************************************************************************
* Copyright (c) 1990 San Diego Supercomputer Center.
* All rights reserved.  The SDSC software License Agreement
* specifies the terms and conditions for redistribution.
*
* File:	macd.c
*
* Abstract:	This file contains macd's initialization routines, request-
*		dispatcher routine and main loops.
*
******************************************************************************/

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/mode.h>
#include <syslog.h>
#include <nx/nxacct.h>
#include <pwd.h>
#include "conf.h"
#include "macd.h"
#include "db.h"
#include "mac.h"
#include "smd.h"
#include "appacct.h"
#include "filename.h"
#include "defines.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

/*
  Different values of status which are returned to the requesting process
*/

#define OK (0)
#define ERROR (-1)

/*
  Some other defines and stuff which are really important
*/

#define CLINULL ((struct cpulim_info *) NULL)
#define PERCENT (10000)
#define DEFAULT_MASK (022)
#define die(X,Y) { fprintf(stderr, X, Y); exit(ERROR); }

extern time_t time();

/*
  Global variables in the MACD
*/

struct macsconf *conf;
struct app_ref macd_ref;
struct acct_ent *top_link;
struct defer_upd *defer_top = NULL;
int n_queue;
struct queue *qcharge;
int sm_conn = DOWN;
struct tm tlastlog;
time_t tlastsync = 0;
int upd_hold = 0;
int _debug_;
int sm_sock=-1;

/*
 * qtabInit ()
 *
 * Abstract:	Initialize qcharge array with DATAPATH/nqstable
 *
 * Arguments:	None
 *
 * Return value: 0 -	successful
 *		-1 -	error
 */

qtabInit()
{
    int i, errflg=0;
    FILE *fd;
    extern int n_queue;
    extern struct queue *qcharge;
    extern void _sendmail();
    extern char *timstr();
    extern struct macsconf *conf;
    extern char *calloc();
    extern char *strcpy();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr, "Enter qtabInit()\n");
	(void) fflush (stderr);
    }

    if ((fd = fopen (MACS_NQSTABLE_FILE, "r")) == NULL) {
	n_queue = 1;
	errflg = 1;
    }
    else if (fscanf (fd, "%d", &n_queue) != 1) {
	n_queue = 1;
	errflg = 1;
    }
    if (n_queue > MAX_QUEUE) {
	n_queue = 1;
	errflg = 1;
    }

    if ((qcharge = (struct queue *)calloc (n_queue, sizeof (struct queue)))
	 == NULL) return (-1);
    if (errflg) {
	(void) strcpy (qcharge[0].name, "UNKNOWN");
	qcharge[0].rate = conf->def_qrate;
	(void) _sendmail (DEF_NQSTABLE, NULL);
	return (0);
    }

    (void) printf ("MACD : %s - %d NQS queues:\n", timstr(0), n_queue);
    (void) fflush (stdout);
    for (i=0; i<n_queue; i++) {
	if (fscanf (fd, "%s %f", qcharge[i].name, &(qcharge[i].rate)) != 2) {
	    (void) printf ("\tInvalid format in %s, abort\n", MACS_NQSTABLE_FILE);
	    (void) fflush (stdout);
	    (void) _sendmail (ERR_NQSTABLE, NULL);
	    return (-1);
	}
	if (strlen (qcharge[i].name) > 23) {
	    qcharge[i].name[23] = '\0';
	    (void) printf ("\tQueue name = %s.., too long, abort\n", 
		qcharge[i].name);
	    (void) fflush (stdout);
	    (void) _sendmail (QNL_NQSTABLE, NULL);
	    return (-1);
	}
	(void) printf ("\t%d\t%s\t%f\n", i+1, qcharge[i].name, qcharge[i].rate);
	(void) fflush (stdout);
    }

    return (0);
}

/*
 * usage()
 *
 * Abstract:	displays the valid command line options
 *
 * Arguments:	name -	program name
 *
 * Return value: None
 */

static void usage(name)
char *name;

{
   (void) printf("usage: %s [-debug] [-nosmd] [-acctonly] [-daily] [-weekly] [-monthly]\n", name);
   exit(0);
}

/*****************************************************************************
*
* paraboot(log_msg)
*
* Abstract:	check if system rebooted since last time MACD went down,
*		if it did, write paraboot entry into the log file and
*		update the lastboot.log file.
*
* Arguments:	log_msg - buffer for message
*
* Return value: None
*
******************************************************************************/

static void paraboot(log_msg)
char *log_msg;
{
    FILE *fd, *lfd;
    char buf[80], lbuf[80];

    if ((fd = fopen (BOOTMESH_LOG, "r")) <= NULL) {
	(void) sprintf (log_msg, "Fail open %s", BOOTMESH_LOG);
	(void) syslog (LOG_ERR, log_msg);
	(void) printf("WARNING  : %s - %s, may result in missing PARABOOT here\n", 
		timstr(0), log_msg);
        (void) fclose (fd);
	return;
    }
    while (fgets (lbuf, 80, fd) != NULL) 
	if (strncmp (lbuf, "Bootmesh", 8) == 0) strncpy (buf, lbuf, 80);
    if (strncmp (buf, "Bootmesh", 8) != 0) {
	(void) sprintf (log_msg, "Fail reading system-boot record from %s", 
		BOOTMESH_LOG);
        (void) syslog (LOG_ERR, log_msg);
        (void) printf("WARNING  : %s - %s, may result in missing PARABOOT here\n", 
		timstr(0), log_msg);
        (void) fclose (fd);
	return;
    }

    if ((lfd = fopen (LASTBOOT_LOG, "r")) <= NULL) {
        (void) sprintf (log_msg, "Fail open %s", LASTBOOT_LOG);
        (void) syslog (LOG_ERR, log_msg);
	(void) printf("WARNING  : %s - %s, may result in missing PARABOOT here\n", 
		timstr(0), log_msg);
    }
    else if (fgets (lbuf, 80, lfd) == NULL) {
	(void) sprintf (log_msg, "Fail reading %s", LASTBOOT_LOG);
        (void) syslog (LOG_ERR, log_msg);
        (void) printf("WARNING  : %s - %s, may result in missing PARABOOT here\n", 
		timstr(0), log_msg);
    }
    else if (strncmp (lbuf, "Bootmesh", 8) != 0) {
	(void) sprintf (log_msg, "%s is corrupted", LASTBOOT_LOG);
        (void) syslog (LOG_ERR, log_msg);
        (void) printf("WARNING  : %s - %s, may result in missing PARABOOT here\n", 
		timstr(0), log_msg);
    }
    else if (strcmp (buf, lbuf) != 0) 
	(void) printf("PARABOOT : %s - System Up After Reboot\n", timstr(0));
    else return;

    if (lfd) (void) fclose (lfd);
    if ((lfd = fopen (LASTBOOT_LOG_TMP, "w")) > NULL) {
	if (fputs (buf, lfd) != EOF) {
	    (void) fclose (lfd);
	    (void) unlink (LASTBOOT_LOG);
	    if (rename (LASTBOOT_LOG_TMP, LASTBOOT_LOG) == 0) return;
	    (void) printf("WARNING  : %s - Fail renaming %s to %s\n",
                    timstr(0), LASTBOOT_LOG_TMP, LASTBOOT_LOG);
	}
    }
    (void) printf("WARNING  : %s - Fail to update %s with %s\n",
	timstr(0), LASTBOOT_LOG, buf);
    (void) fflush (stdout);
    return;
}


/*****************************************************************************
*
* options(argc, argv)								*
* Abstract:	parses, checks command line options and set global variables.
*
* Arguments:	argc -	argument count
*		argv -	argument list
*
* Return value: None
*
******************************************************************************/

static void options(argc, argv)
int argc;
char ** argv;

{
   int i;

   for (i = 1; i < argc; i++)
      if (!strcmp(argv[i], "-debug"))
         _debug_ = TRUE;
      else if (!strcmp(argv[i], "-nosmd"))
	 _debug_ = NO_SMD;
      else if (!strcmp(argv[i], "-acctonly"))
         conf->macdmode = ACCTONLY;
      else if (!strcmp(argv[i], "-daily"))
         conf->switchlog = LOGDAILY;
      else if (!strcmp(argv[i], "-weekly"))
         conf->switchlog = LOGWEEKLY;
      else if (!strcmp(argv[i], "-monthly"))
         conf->switchlog = LOGMONTHLY;
      else
         (void) usage(argv[0]);
}


/*****************************************************************************
*
* processRequest()
*
* Abstract:	This routine is the request-dispatcher.  Depending on
*		the request type in the in-coming message header, 
*		basically, a message from SMD or NQS will be passed to
*		a handler and other message will be processed by this
*		routine itself.
*
* Arguments:	request -	in-coming message
*
* Return value: None
*
******************************************************************************/

#define CHECKSIZE(SIZE)       					\
   if (request->size != SIZE) {					\
      (void) portReply(request->socket, ERROR, EINVAL, 0, NULL);\
      break;							\
   }

static void processRequest(request)
struct msg_in *request;

{
   int status, firsttime;
   struct cpulim_info *cli, *ucli, *acli, account;
   struct cpu_ctrl *cc;
#ifndef INTELv1r1
   struct job_charge *job_rep;
#endif
   struct si_info *si;
   extern char *timstr();
   extern void *memset();
#ifndef INTELv1r1
   extern struct job_charge *jobEnd();
#endif
   
   /*
     switch on the type of data request read in from the sockets
   */

   switch(request->command) {

      /*
        C_OFF:  Shut the MACD off
      */

      case C_OFF:
         if (conf->macdmode != ACCTONLY) status = db_writeall();
         (void) portReply(request->socket, status, errno, 0, NULL);
         if (!_debug_) {
	    (void) syslog (LOG_INFO, "MACD received shutdown request, exiting");
	 }
         (void) portClose();
         (void) printf("MACDDOWN : %s - MACD received shutdown request, exiting\n", timstr(0));
         (void) fflush(stdout);
         exit(0);
         break;

      /*
        C_BOOT:  Log the system reboot
      */

      case C_BOOT:
         (void) portReply(request->socket, OK, OK, 0, NULL);
         (void) printf("PARABOOT : %s - System Up After Reboot\n", timstr(0));
         (void) fflush(stdout);
         break;

      /*
        C_ON_COUNT:  Flush all data and turn off accounting
      */

      case C_ON_COUNT:	/* not implemented yet */
         (void) portReply(request->socket, ERROR, EINVAL, 0, NULL);
         break;

      /*
        C_SYNC:  Flush all data to the database
      */

      case C_SYNC:
	 if (upd_hold) {
	    (void) portReply(request->socket, ERROR, EROFS, 0, NULL);
	    break;
	 }
         if (conf->macdmode != ACCTONLY) status = db_writeall();
         (void) portReply(request->socket, status, errno, 0, NULL);
         if (status < 0)
            (void) printf("MACDSYNC : %s - Error in flushing database errno %d\n", 
		timstr(0), errno);
         else (void) printf("MACDSYNC : %s - Flushed database\n", timstr(0));
         (void) fflush(stdout);
         break;

      /*
        case C_OPEN:  Just access the MACD
      */

      case C_OPEN:
         (void) portReply(request->socket, OK, OK, 0, NULL);
         break;

      /*
        case C_CLOSE:  Close connection to the MACD
      */

      case C_CLOSE:
         (void) portReply(request->socket, OK, OK, 0, NULL);
         (void) portCloseConnection(request->socket);
         break;

      /*
        case C_VALID:  Returns true if user/account_id combination 
	may use time
      */

      case C_VALID:
         CHECKSIZE(sizeof(struct cpu_ctrl))
         cc = (struct cpu_ctrl *) request->data;
	if (_debug_) {
	    (void) fprintf (stderr,
		"case C_VALID:  cc->info.id=%d, cc->agid=%d\n", 
		    cc->info.id, cc->agid);
	    (void) fflush (stderr);
	}
        if (conf->macdmode == ACCTONLY) {
	    struct passwd *pwent;
	    struct nxacct *nxaent;
	    char **t;
	    int okflg;

	    if (cc->info.id == 0) {
		(void) portReply(request->socket, TRUE, 0, 0, NULL);
		break;
	    }
	    if ((pwent = getpwuid (cc->info.id)) != NULL) {
		if ((nxaent = nx_getaid(cc->agid)) != NULL) {
		    if (_debug_) {
			(void) fprintf (stderr,
			    "getpwuid(%d) and nx_getaid(%d) both NOT NULL\n",
			    cc->info.id, cc->agid);
			(void) fflush (stderr);
		    }
		    for (okflg=0, t = nxaent->acct_mem; *t; ++t) {
			if ((**t == '!') && 
			    (strcmp(pwent->pw_name, ((*t)+1)) == 0))
			    break;
			if ((strcmp(pwent->pw_name, *t) == 0) 
			    || (strcmp(*t, "*") == 0)) {
			    if (_debug_) {
			        (void) fprintf (stderr,
			            "permission granted\n");
			        (void) fflush (stderr);
		            }
			    okflg = 1;
			    (void) portReply(request->socket, TRUE, 0, 0, NULL);
			    break;
		        }
		    }
		    if (okflg) break;
		}
	    }
            (void) portReply(request->socket, FALSE, INVALID_ACCT, 0, NULL);
	 }
         else if ((acli = db_getagid(cc->agid)) == CLINULL && cc->info.id != 0)
            (void) portReply(request->socket, FALSE, INVALID_ACCT, 0, NULL);
         else if ((cli = db_getuid(cc->info.id, cc->agid)) == CLINULL
		&& cc->info.id != 0)
            (void) portReply(request->socket, FALSE, INVALID_ACCT, 0, NULL);
	 else if (acli == CLINULL || cli == CLINULL)
	    (void) portReply(request->socket, TRUE, 0, 0, NULL);
         else if (!cli->use)
               (void) portReply(request->socket, FALSE, NOT_ALLOWED, 0, NULL);
         else if (conf->enforce != 0 && acli->inhibit)
               (void) portReply(request->socket, FALSE, OVER_ALLOC, 0, NULL);
         else if (conf->enforce != 0 && cli->inhibit)
               (void) portReply(request->socket, FALSE, OVER_ALLOC, 0, NULL);
         else if (cli->maxnodes > 0 && 
		((struct cpu_ctrl *) request->data)->info.maxnodes > 
		cli->maxnodes)
               (void) portReply(request->socket, FALSE, OVER_NODES, 0, NULL);
         else if (cli->maxnodes < 0 && 
		(((struct cpu_ctrl *) request->data)->info.maxnodes > 0) &&
		(((struct cpu_ctrl *) request->data)->info.maxnodes >
		conf->total_nodes))
               (void) portReply(request->socket, FALSE, OVER_NODES, 0, NULL);
         else (void) portReply(request->socket, TRUE, OK, 0, NULL);
         break;

      /*
        C_GETLIMIT:  Return CPU limit information for the specified agid and uid
        C_GETACCT:   Return CPU time limit information for an accounting group
      */

      case C_GETLIMIT:
      case C_GETACCT:
	 if (conf->macdmode == ACCTONLY) {
	    (void) portReply(request->socket, ERROR, EACCTONLY, 0, NULL);
	    break;
	 }
         CHECKSIZE(sizeof(struct cpu_ctrl))
         cc = (struct cpu_ctrl *) request->data;
         if (request->command == C_GETLIMIT)
            cli = db_getuid(cc->info.id, cc->agid);
         else
            cli = db_getagid(cc->agid);
         if (cli == CLINULL)
            (void) portReply(request->socket, ERROR, ENOENT, 0, NULL);
         else {
            cc->info.id         = cli->id;
            cc->info.weight     = cli->weight;
            cc->info.inhibit    = cli->inhibit;
            cc->info.modify     = cli->modify;
            cc->info.transfer   = cli->transfer;
            cc->info.use        = cli->use;
            cc->info.percent    = cli->percent;
            cc->info.maxnodes   = cli->maxnodes;
            cc->info.killjobs   = cli->killjobs;
            cc->info.lockjobs   = cli->lockjobs;
            cc->info.authorized = cli->authorized;
            cc->info.used_time  = cli->used_time;
            cc->info.sbu_time   = cli->sbu_time;
            cc->info.unlimit    = cli->unlimit;
            cc->info.timestamp  = cli->timestamp;
            (void) portReply(request->socket, OK, OK, 
			sizeof(struct cpu_ctrl), (char *) cc);
         }
         break;

      /*
        C_CREACCT:  Creates CPU limit data for the accounting group specified
      */

      case C_CREACCT:
	 if (conf->macdmode == ACCTONLY) {
	    (void) portReply(request->socket, ERROR, EACCTONLY, 0, NULL);
	    break;
	 }
	 if (upd_hold) {
	    (void) portReply(request->socket, ERROR, EROFS, 0, NULL);
	    break;
	 }
         CHECKSIZE(sizeof(struct cpu_ctrl))
         cc = (struct cpu_ctrl *) request->data;
         if (db_getagid(cc->info.id) != CLINULL)
            (void) portReply(request->socket, ERROR, EEXIST, 0, NULL);
         else {
            (void *) memset((char* ) &account, 0, sizeof(struct cpulim_info));
            account.agid       = cc->info.id;
            account.id         = cc->info.id;
            account.lockjobs   = cc->info.lockjobs;
            account.killjobs   = cc->info.killjobs;
            account.weight     = cc->info.weight;
            account.authorized = max(cc->info.authorized, 0.0);
            account.unlimit    = cc->info.unlimit;
            account.maxnodes   = cc->info.maxnodes;
            account.timestamp  = time(0);
            if (db_insagid(account.agid, &account) < 0)
               portReply(request->socket, ERROR, errno, 0, NULL);
            else
               portReply(request->socket, OK, OK, 0, NULL);
         }
         break;

      /*
        C_DELACCT:  Delete CPU limit data for the specified accounting group
      */

      case C_DELACCT:
	 if (conf->macdmode == ACCTONLY) {
	    (void) portReply(request->socket, ERROR, EACCTONLY, 0, NULL);
	    break;
	 }
	 if (upd_hold) {
	    (void) portReply(request->socket, ERROR, EROFS, 0, NULL);
	    break;
	 }
         CHECKSIZE(sizeof(struct cpu_ctrl))
         cc = (struct cpu_ctrl *) request->data;
         if ((cli = db_getagid(cc->info.id)) == CLINULL)
            (void) portReply(request->socket, ERROR, ENOENT, 0, NULL);
         else if (cli->ref_cnt != 0)
            (void) portReply(request->socket, ERROR, EBUSY, 0, NULL);
         else if (db_delagid(cc->info.id) < 0)
            (void) portReply(request->socket, ERROR, errno, 0, NULL);
         else
            (void) portReply(request->socket, OK, OK, 0, NULL);
         break;

      /*
        C_DELLIMIT:  Delete CPU limit data for the specified user
      */

      case C_DELLIMIT:
	 if (conf->macdmode == ACCTONLY) {
	    (void) portReply(request->socket, ERROR, EACCTONLY, 0, NULL);
	    break;
	 }
	 if (upd_hold) {
	    (void) portReply(request->socket, ERROR, EROFS, 0, NULL);
	    break;
	 }
         CHECKSIZE(sizeof(struct cpu_ctrl))
         cc = (struct cpu_ctrl *) request->data;
         if (((acli = db_getagid(cc->agid)) == CLINULL) ||
             ((ucli = db_getuid(cc->info.id, cc->agid)) == CLINULL))
            (void) portReply(request->socket, ERROR, ENOENT, 0, NULL);
         else if (db_deluid(cc->info.id, cc->agid) < 0)
            (void) portReply(request->socket, ERROR, errno, 0, NULL);
         else {
            (acli->ref_cnt)--;
            (void) portReply(request->socket, OK, OK, 0, NULL);
         }
         break;

      /*
        C_SETLIMIT:  Changes CPU limit for user id in accounting group agid
      */

      case C_SETLIMIT:
	 if (conf->macdmode == ACCTONLY) {
	    (void) portReply(request->socket, ERROR, EACCTONLY, 0, NULL);
	    break;
	 }
	 if (upd_hold) {
	    (void) portReply(request->socket, ERROR, EROFS, 0, NULL);
	    break;
	 }
         CHECKSIZE(sizeof(struct cpu_ctrl))
         cc = (struct cpu_ctrl *) request->data;
         if ((acli = db_getagid(cc->agid)) == CLINULL)
            (void) portReply(request->socket, ERROR, ENOENT, 0, NULL);
         else {
            firsttime = FALSE;
            if ((ucli = db_getuid(cc->info.id, cc->agid)) == CLINULL) {
               firsttime = TRUE;
               ucli = &account;
               (void *) memset((char* ) ucli, 0, sizeof(struct cpulim_info));
               ucli->id = cc->info.id;
               ucli->agid = cc->agid;
            }
            ucli->modify     = cc->info.modify;
            ucli->transfer   = cc->info.transfer;
            ucli->use        = cc->info.use;
            ucli->percent    = cc->info.percent;
            ucli->percent    = min(ucli->percent, PERCENT);
            ucli->percent    = max(ucli->percent, 0);
            ucli->maxnodes   = cc->info.maxnodes;
            ucli->timestamp  = time(0);
            if (acli->unlimit) ucli->authorized = cc->info.authorized;
	    else {
		if (cc->info.authorized > acli->authorized || 
		    cc->info.authorized < 0.0)
		    ucli->authorized = 
		        (ucli->percent * acli->authorized)/PERCENT;
		else ucli->authorized = cc->info.authorized;
	    }
            if (cc->info.unlimit) {
                ucli->authorized == 0.0;
            	ucli->unlimit = TRUE;
	    }
            else ucli->unlimit = FALSE;
            if (((ucli->sbu_time < ucli->authorized)  &&
                 (ucli->sbu_time < acli->authorized)) || 
		(acli->unlimit && ucli->percent != 0) || 
		(acli->unlimit && ucli->unlimit))
               ucli->inhibit = ucli->shutoff = FALSE;
            else
               ucli->inhibit = TRUE;
            if (firsttime) {
               if (db_insuid(ucli->id, ucli->agid, ucli) < 0)
                  (void) portReply(request->socket, ERROR, errno, 0, NULL);
               else
                  (acli->ref_cnt)++;
               if (ucli != CLINULL) (void) free((char *) ucli);
            }
            (void) db_writeuid(cc->info.id, cc->agid);
            (void) portReply(request->socket, OK, OK, 0, NULL);
         }
         break;

      /*
        C_SETACCT:  Changes CPU limit data for the accounting group agid
      */

      case C_SETACCT:
	 if (conf->macdmode == ACCTONLY) {
	    (void) portReply(request->socket, ERROR, EACCTONLY, 0, NULL);
	    break;
	 }
	 if (upd_hold) {
	    (void) portReply(request->socket, ERROR, EROFS, 0, NULL);
	    break;
	 }
         CHECKSIZE(sizeof(struct cpu_ctrl))
         cc = (struct cpu_ctrl *) request->data;
         if ((acli = db_getagid(cc->info.id)) == CLINULL)
            (void) portReply(request->socket, ERROR, ENOENT, 0, NULL);
         else {
            if (cc->info.unlimit) acli->inhibit = FALSE;
	    else if (acli->unlimit || 
		(cc->info.authorized != acli->authorized)) {
               if (cc->info.authorized > acli->sbu_time)
                  acli->inhibit = FALSE;
	       else
                  acli->inhibit = TRUE;
	    }
	    else /* unchange */;
            acli->authorized = max(cc->info.authorized, 0.0);
            acli->maxnodes   = cc->info.maxnodes;
            if ((acli->lockjobs = cc->info.lockjobs) == FALSE)
               acli->killjobs = cc->info.killjobs;
            acli->weight   = cc->info.weight;
            acli->unlimit  = cc->info.unlimit;
            acli->timestamp = time(0);
            (void) db_writeagid(cc->info.id);

	    /*
		recalculate cpu limit for each user in the accounting group
	    */
            while ((ucli = db_getnextuid(cc->info.id)) != CLINULL) {
	       if (acli->unlimit) { 
		   if (ucli->percent != 0) {
			ucli->unlimit = TRUE;
			ucli->inhibit = FALSE;
	           }
	       }
	       else {
		   ucli->unlimit = FALSE;
                   ucli->authorized = (ucli->percent * acli->authorized)/PERCENT;
                   if ((ucli->sbu_time < ucli->authorized)  &&
                       (ucli->sbu_time < acli->authorized))
                       ucli->inhibit = FALSE;
                   else ucli->inhibit = TRUE;
	       }
               ucli->timestamp = time(0);
               (void) db_writeuid(ucli->id, ucli->agid);
            }
            (void) portReply(request->socket, OK, OK, 0, NULL);
         }
         break;

      /*
        NQS_JOB_START:  job start information from nqs
      */

      case NQS_JOB_START:
         (void) jobStart(request->size, request->data);
         break;

      /*
        NQS_JOB_END:  job end information from nqs
      */

      case NQS_JOB_END:
#ifndef INTELv1r1
         if ((job_rep = jobEnd(request->size, request->data)) > NULL) {
	    (void) portReply(request->socket, OK, OK, 
		sizeof (struct job_charge), job_rep);
            if (_debug_) {
                (void) fprintf (stderr, "job_charge send to NQS:\n");
                (void) fprintf (stderr,
                    "cpu=%d, rate=%f, under=%d, rate=%f, idle=%d, rate=%f\n",
                        job_rep->cpu_time, job_rep->cpu_rate,
                        job_rep->under_used, job_rep->under_rate,
                        job_rep->idle_time, job_rep->idle_rate);
            }
	    (void) free (job_rep);
	 }
	 else (void) portReply(request->socket, FALSE, errno, 0, NULL);
#else
         (void) jobEnd(request->size, request->data);
#endif
         break;

      /*
        SMD_STATUS_RESP:  Cube information about current users
      */

      case SMD_STATUS_RESP:
         (void) appStatu(request->size, request->data);
         break;

      /*
        SMD_APP_ALARM_IND:  Cube information about current users
      */

      case SMD_APP_ALARM_IND:
         (void) appAlarm(request->size, request->data);
         break;

      /*
        SMD_APP_START_IND:  A application just started
      */

      case SMD_APP_START_IND:
         (void) appStart(request->size, request->data);
         break;

      /*
        SMD_APP_END_IND:  A application just ended
      */

      case SMD_APP_END_IND:
         (void) appEnd(request->size, request->data);
         break;

      /*
        SCHED_DOWN:  interruption/down-time log
      */

      case SCHED_DOWN:
	 si = (struct si_info *)request->data;
	 (void) printf("INTRRUPT : %s - Scheduled=%d Type=%s %s\n", 
		timstr(0), si->flag, stype[si->type], si->comment);
         (void) fflush(stdout);
         break;

      /*
        SI_OFF:  End of scheduled service interruption

      case SI_OFF:
         (void) printf("INTRRUPT : %s - Scheduled=0\n", timstr(0));
         (void) fflush(stdout);
         break;
      */

      /*
	C_UPD_HOLD:  Hold the update to database
      */

      case C_UPD_HOLD:
	 if (conf->macdmode == ACCTONLY)
		(void) portReply(request->socket, -1, EACCTONLY, 0, NULL);
	 else if (upd_hold) 
		(void) portReply(request->socket, -1, EEXIST, 0, NULL);
	 else {
	    status = db_writeall();
            (void) portReply(request->socket, status, errno, 0, NULL);
	    upd_hold = 1;
            (void) printf("UPDHOLD  : %s - Start defering usage update until MACUPDATE complete\n", timstr(0));
            (void) fflush(stdout);
	 }
         break;


      /*
        C_UPD_RESUME:  Hold the update to database
      */
 
      case C_UPD_RESUME:
         if (conf->macdmode == ACCTONLY || !upd_hold)
                (void) portReply(request->socket, -1, EACCTONLY, 0, NULL);
         else {
            status = upd_resume();
            (void) portReply(request->socket, status, errno, 0, NULL);
            upd_hold = 0;
            (void) printf("UPDRESUM : %s -  MACUPDATE complete.  Resume usage update\n", timstr(0));
            (void) fflush(stdout);
         }
         break;

      /*
        C_ISMACWATCH:  Determine if MACS is in "macwatch" mode
      */

      case C_ISMACWATCH:
	 if (conf->macdmode == ACCTONLY) {
	    (void) portReply(request->socket, ERROR, EACCTONLY, 0, NULL);
	 } else {
            (void) portReply(request->socket, OK, OK, 0, NULL);
	 }
	 break;

      /*
        default:  Bad message sent - reply with an error message
      */

      default:
         (void) portReply(request->socket, ERROR, EINVAL, 0, NULL);
         break;
   }
   if (request->data != NULL) (void) free (request->data);
   request->size == 0;
   request->data == NULL;
}

/*****************************************************************************
*
* sig_handler()
*
* Abstract:	catches signals, exits on some after save the MACD database
*		and ignore the rest.  It logs the signal received and the 
*		action taken.
*
* Arguments:	sig -	signal number
*
* Return value: None
*
******************************************************************************/

static void sig_handler(sig)
int sig;

{
/*
     if (sig == SIGHUP || sig == SIGQUIT || sig == SIGTERM) {
        if (conf->macdmode != ACCTONLY) {
	   if (db_writeall() < 0)
               (void) printf("MACDSYNC : %s - Error in flushing database errno %d\n", 
		timstr(0), errno);
           else
               (void) printf("MACDSYNC : %s - Flushed database\n", timstr(0));
	}
        (void) printf("MACDDOWN : %s - MACD exiting (signal %d)\n", 
		timstr(0), sig);
        (void) fflush(stdout);
        (void) portClose();
        exit(0);
     }
*/
    (void) printf ("MACDINFO : %s - Signal=%d is received and ignored\n",
           timstr(0), sig);
     (void) fflush(stdout);
  }

/*****************************************************************************
*
* main()
*
* Abstract:	This is the main routine of MACS daemon.
*		It initializes global variables and MACD database, opens
*		connections to SMD and MACD_PORT, gets messages from the
* 		sockets, and processes the requests.  This daemon is to be
* 		started by macpd.  Before macd can get the valid log file path
* 		it will log event to macpd log file which is the last argument
*		in argv
*
* Arguments:	argc -	argument count
*		argv -	argument list
*
* Return value: None
*
*****************************************************************************/

void main(argc, argv)
int argc;
char **argv;

{
   int pid;
   char log_msg[128];
   struct msg_in request;
   FILE *fd0, *fd1, *fd2=NULL;
   struct sockaddr_in macd_port;
   struct hostent *he;
   extern u_short htons();
   extern int openlog(), syslog();

   fd2 = freopen (MACD_START_LOG, "a", stdout);
   if (fd2) {
      int i;
      for (i=1; i<argc; i++) if (argv[i][0]=='-' && argv[i][1]=='d') break;
      if (i>=argc) /* dup2 (1,2) */ freopen (MACD_ERR_LOG, "a", stderr);
   }
   (void) openlog("macd", LOG_CONS|LOG_NOWAIT, LOG_DAEMON);

   /*
     Get the configuration parameters
   conf = (struct macsconf *)malloc (sizeof (struct macsconf));
   */
   if ((conf = readconf()) == NULL) {
      if (!_debug_) {
        (void) sprintf (log_msg, 
	  "errno=%d returned from reading configuration file.  Exit", errno);
        (void) syslog (LOG_ERR, log_msg);
      }
      die("ERROR:  Error in reading configuration file - errno=%d", errno)
    }
    (void) printf ("MACD : %s - Configuration Parameters:\n", timstr(0));
    (void) printf ("\tNODES\t%d\n", conf->total_nodes);
    (void) printf ("\tMAILER\t%s\n", conf->mailer);
    (void) printf ("\tADMIN\t%s\n", conf->admin);
    (void) printf ("\tOPERATOR\t%s\n", conf->operator);
    (void) printf ("\tSWITCHLOG\t%s\n", conf->switchlog ? 
    (conf->switchlog==LOGWEEKLY? "weekly" : "monthly") : "daily");
    (void) printf ("\tMACDMODE\t%s\n", conf->macdmode ? 
        "resource-control" : "accounting-only");
    (void) printf ("\tENFORCE\t%s\n", 
        conf->enforce ? (conf->enforce<USERKILL ?
    (conf->enforce==USERKILL ? "userkill" : "acctkill") : 
        "userkill,acctkill") : "nokill");
    (void) printf ("\tDEF_QRATE\t%f\n", conf->def_qrate);
    (void) printf ("\tUNDERUSE\t%f\n", conf->uunt_rate);
    (void) printf ("\tIDLERATE\t%f\n", conf->idle_rate);
    (void) printf ("\tSYNC_INTERVAL\t%d\n", conf->sync_interval);
    (void) fflush (stdout);
    if (qtabInit() != 0) {
      if (!_debug_) {
        (void) sprintf (log_msg, 
         "errno=%d returned from initializing queue charge table.  Exit", errno);
        (void) syslog (LOG_ERR, log_msg);
      }
      die("ERROR:  Error in initialize queue charge table - errno=%d\n", errno)
    }

   /*
     Take care of any options given on the command line
	Open all the std file descriptors, so it won't be
	opened as sockets later
   */
   (void) options(argc, argv);
   fd2 = freopen ("/dev/null", "w", stdout);
  /* if (!_debug_) fd1 = freopen ("/dev/null", "w", stderr); */
   fd0 = freopen ("/dev/null", "r", stdin);

   /*
     Disassociate us from the parent and set up permissions

   if ((pid = fork()) < 0) {
      if (!_debug_) {
        (void) sprintf (log_msg, 
         "errno=%d returned from forking off child process.  Exit", errno);
        (void) syslog (LOG_ERR, log_msg);
      }
      die("ERROR:  Error in forking off child process - errno=%d", errno)
   }
   if (pid != 0) exit(0);
   if (setuid(0) < 0) {
      if (!_debug_) {
        (void) sprintf (log_msg, 
         "errno=%d returned from setting uid to root.  Exit", errno);
        (void) syslog (LOG_ERR, log_msg);
      }
      die("ERROR:  Error in getting root user permissions - errno=%d\n", errno)
   }
   if (setgid(0) < 0) {
      if (!_debug_) {
        (void) sprintf (log_msg, 
         "errno=%d returned from setting gid to 0.  Exit", errno);
        (void) syslog (LOG_ERR, log_msg);
      }
      die("ERROR:  Error in getting root group permissions - errno=%d\n", errno)
   }
   if (setpgrp() < 0) {
      if (!_debug_) {
        (void) sprintf (log_msg, 
         "errno=%d returned from setting process group id.  Exit", errno);
        (void) syslog (LOG_ERR, log_msg);
      }
      die("ERROR:  Error in setting process group - errno=%d", errno)
   }
   (void) umask(DEFAULT_MASK);
   */
   (void) signal(SIGINT,  sig_handler);
   (void) signal(SIGIOT,  sig_handler);
   (void) signal(SIGPIPE,  sig_handler);
   (void) signal(SIGUSR1,  sig_handler);
   (void) signal(SIGUSR2,  sig_handler);
   (void) signal(SIGHUP,  sig_handler);
   (void) signal(SIGQUIT, sig_handler);
   (void) signal(SIGTERM, sig_handler);

   /*
     Connect to SM_PORT, issue status request to scheduler monitor
   (void) smStatuReq ();
   */

   /*
     Open up the main server communications socket
     Open up the database and rehash it just for fun
     Initialize all appsacct data structures
   */

   if (portOpen() < 0) {
      if (!_debug_) {
        (void) sprintf (log_msg, 
         "errno=%d returned from opening socket.  Exit", errno);
        (void) syslog (LOG_ERR, log_msg);
      }
      die("ERROR:  Error in opening up socket - errno=%d\n", errno)
   }
   if (conf->macdmode != ACCTONLY) {
	int temp;
      if ((temp = db_init(DB_OVERFLOW)) < 0) {
         if (!_debug_) {
            (void) sprintf (log_msg, 
              "errno=%d returned from initializing MACD database.  Exit", errno);
            (void) syslog (LOG_ERR, log_msg);
	 }
         (void) _sendmail (DB_INIT, NULL);
         (void) fprintf (stderr, "%s: Error in opening up database - %d\n",
                timstr(0), temp);
         exit(ERROR);
      }
      if (rehash(DB_REHASH)) {
         if (!_debug_) {
            (void) sprintf (log_msg, 
               "errno=%d returned from rehashing MACD database.  Exit", errno);
            (void) syslog (LOG_ERR, log_msg);
	 }
         die("ERROR:  Error in rehashing database - errno=%d\n", errno)
      }
   }

   /*
     Close stdin and stderr and redirect stdout to a logfile
   */

   (void) close(fileno(stdin));
   /* if (!_debug_) (void) close(fileno(stderr)); */
   (void) startLogfile();

   /*
     Register with the allocator
   */
   bzero ((char *) &macd_port, sizeof (struct sockaddr_in));
   macd_port.sin_family = AF_INET;
   macd_port.sin_port = htons(MACD_PORT);
   if ((he = gethostbyname("localhost")) == (struct hostent *) NULL) {
      if (!_debug_) {
        (void) sprintf (log_msg, 
          "errno=%d returned from getting local host entry.  Exit", errno);
        (void) syslog (LOG_ERR, log_msg);
      }
      die("ERROR: Error getting local host entry - errno=%d\n",errno);
   }
   bcopy(he->h_addr, &macd_port.sin_addr.s_addr, he->h_length);
   if (nx_register_daemon(MACS_DAEMON_ID, &macd_port, sizeof (macd_port)) != 0){
        perror("MACD allocator registration");
        exit(1);
   }
   if (_debug_) (void) fprintf (stderr, "MACD registered with allocator\n");

   /* Ready */
   (void) paraboot(log_msg);
   (void) printf("MACDUP   : %s - MACD starting\n", timstr(0));
   (void) fflush(stdout);

   request.socket = -1;
   request.command = -1;
   request.size = 0;
   request.data = NULL;

   /*
     Wait until we get a data request from the server port and process it
   */

   while (TRUE) { 
      (void) portRequest(&request);
      (void) processRequest(&request);
   }
}
