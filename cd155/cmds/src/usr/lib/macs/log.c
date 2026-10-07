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
 *              Copyright (c) 1990 San Diego Supercomputer Center.
 *              All rights reserved.  The SDSC software License Agreement
 *              specifies the terms and conditions for redistribution.
 *
 * File:        log.c
 *
 * Abstract:    This file contains routines manipulate log files
 *****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include "conf.h"
#include "macd.h"
#include "appacct.h"
#include "mac.h"
#include "smd.h"
#include <errno.h>
#include "filename.h"

extern time_t time();

/*****************************************************************************
*
* lastlog()
*
* Abstract:	This routine check logfile entries, return the last logfile 
* 		name string and the switching time of the last logfile
*
* Arguments:	lasttime	- address to store the switching time
*
* Return value: last logfile name or NULL for error
*
*****************************************************************************/

char * lastlog(lasttime)
time_t *lasttime;
{
	FILE *pstr;
	int scan, fname_int;
	char fname[256], *lastlogname=NULL;
	char *cmd, dummy[256];
	extern char *malloc();
	extern int errno, _debug_;

	if (_debug_) {
	    (void) fprintf (stderr, "Start lastlog()\n");
	    (void) fflush (stderr);
	}
        cmd = (char *) malloc (strlen(MACD_LOG_PATH)+12);
        (void) sprintf (cmd, "/bin/ls -1 %s",MACD_LOG_PATH);
 
        if ((pstr = popen(cmd, "r")) == (FILE *) NULL) {
            (void) fprintf(stderr, "Lastlog: Unable to open pipe /bin/ls -1\n");
            return(NULL);
        }

        while (fgets (fname, 256, pstr) != NULL) {
                if ((scan = sscanf(fname,"macd%d",&fname_int)) != 1) {
			if (_debug_) {
                        	(void) fprintf (stderr, 
					"Lastlog: Read in invalid file name %s\n", fname);
                        	(void) fflush (stderr);
			}
                        continue;
                }

		if ((lastlogname = 
			malloc (strlen (MACD_LOG_PATH) + 13)) == NULL) {
			if (_debug_) {
			    (void) fprintf (stderr, "Lastlog: malloc failed\n");
			    (void) fflush (stderr);
			}
			return (NULL);
		}
		(void) sprintf (lastlogname, "%s/macd%8d", MACD_LOG_PATH, fname_int);
	}
	(void) pclose(pstr);
	
	*lasttime = 0;
	if (lastlogname == NULL) {
		if (_debug_) {
		    (void) fprintf (stderr, 
			"Lastlog: No valid logfile found in %s\n",
			MACD_LOG_PATH);
		    (void) fflush (stderr);
		}
		return (NULL);
	}
	if (_debug_) {
	    (void) fprintf (stderr, "Lastlog: Last logfile name is %s\n",
		lastlogname);
	    (void) fflush (stderr);
	}

	if ((pstr = fopen (lastlogname, "r")) == NULL) { 
		if (_debug_) {
		    (void) fprintf (stderr, 
			"Lastlog: Fail in opening last logfile %s, errno=%d\n",
			lastlogname, errno);
		    (void) fflush (stderr);
		}
		return (NULL);
	}

	/* get the first time stamp in the file */
	while (fgets (fname, 256, pstr) != NULL)
		if ((scan = sscanf (fname, "%s : (%d)", dummy, lasttime)) == 2) {
			if (_debug_) {
			    (void) fprintf (stderr, 
				"Lastlog: Return lasttime=%ld\n", *lasttime);
			    (void) fflush (stderr);
			}
			return (lastlogname);
		}

	if (_debug_) {
                (void) fprintf (stderr, 
			"Rastlog: Failed in getting time stamp in file %s\n",
			lastlogname);
                (void) fflush (stderr);
        }
	return(NULL);		
}


/*****************************************************************************
*
* newlog()
*
* Abstract:	This routine opens up a logfile for MACD and
* 		associates that log file with stdout.
*
* Arguments:	log_tm	- contain time info. for naming the file
*		now	- current time
*
* Return value: None
*
*****************************************************************************/

void newlog (now, log_tm)
time_t now;
struct tm *log_tm;
{
    char filename[256], logname[256];
    FILE *fd;
    time_t lasttime = 0;
    char *lastlogf;
    extern struct tm tlastlog;
    extern char *timstr();
    extern time_t tlastsync;
    extern int _debug_;

if (_debug_) (void) fprintf (stderr, "Enter newlog(%d,%d)\n", now, log_tm);

    /*
     * compose the new log file name, if this is a normal logfile-switching
     * rename the file contain last status info to the new log file name
     */
    (void) sprintf(filename, "%s/macd%04d%02d%02d", MACD_LOG_PATH, 
	log_tm->tm_year+1900, log_tm->tm_mon+1, log_tm->tm_mday);
    if (tlastsync) {
	if (_debug_) (void) fprintf (stderr,
	    "%s - newlog(), tlastsync=%d\n", timstr(0), tlastsync);
	/*
	 * get the time and logfile name of the last log
	 */
	if ((lastlogf = lastlog(&lasttime)) != NULL) {
	    if (_debug_) (void) fprintf (stderr, "lastlog(%d) return %s\n",
		lasttime, lastlogf);
	    /*
	     * append the current status log entries if any
	     * in to the last macdlog file
	     */
	    (void) logjobs (1, lastlogf);
	}
	else if (_debug_) (void) fprintf (stderr,
	    "%s - newlog(), %s is accessible\n", timstr(0), filename);
	(void) logjobs (1, filename);
    }
    else if (_debug_) (void) fprintf (stderr,
	"%s - newlog(), tlastsync=%d\n", timstr(0), tlastsync);

    /* open or create the log file and link to stdout */
    (void) fflush(stdout);
    (void) freopen(filename, "a", stdout);
    tlastlog = *log_tm;
}


/*****************************************************************************
*
* startLogfile()								*
* Abstract:	This routine opens up a logfile for MACD and
* 		associates that log file with stdout.  It's
*		only called by MACD at start-up time
*
* Arguments:	None
*
* Return value: None
*
*****************************************************************************/

void startLogfile()

{
    struct tm nowtm;
    time_t now, lasttime = 0;
    char *lastlogf;
    char cmd[256];
    FILE *fd;
    struct stat logstat;
    extern char *timstr();
    extern struct macsconf *conf;
    extern struct tm tlastlog;
    extern int _debug_;

if (_debug_) (void) fprintf (stderr, "Enter startLogfile()\n");

    now = time(0);
    nowtm = *(localtime(&now));
    if (_debug_) (void) fprintf (stderr, 
	"nowtm = %d %d %d %d %d %d %d %d %d\n",
	nowtm.tm_sec, nowtm.tm_min, nowtm.tm_hour, nowtm.tm_mday,
	nowtm.tm_mon, nowtm.tm_year, nowtm.tm_wday, nowtm.tm_yday,
	nowtm.tm_isdst);

    /*
     * get the time and logfile name of the last log
     */
    if ((lastlogf = lastlog(&lasttime)) != NULL) {
	if (_debug_) (void) fprintf (stderr, "lastlog(%d) return %s\n",
	    lasttime, lastlogf);
	/*
	 * append the last status log entries if any
	 * in to the last macdlog file
	 */
	if (access(MACD_LASTLOG_FNAME, F_OK) == 0 && 
	    stat (MACD_LASTLOG_FNAME, &logstat) == 0 && logstat.st_size > 0) {
	    (void) sprintf (cmd, "cat %s %s > %s", 
	        lastlogf, MACD_LASTLOG_FNAME, MACD_TEMPLOG_FNAME);
	    (void) system (cmd); 
	    (void) unlink (lastlogf);
	    if (rename (MACD_TEMPLOG_FNAME, lastlogf) != 0) {
	        (void) printf("WARNING  : %s - Fail renaming %s to %s\n",
	            timstr(0), MACD_TEMPLOG_FNAME, lastlogf);
		(void) fflush (stdout);
	    }
	}
	else if (access(MACD_TEMPLOG_FNAME, F_OK) == 0 && 
	    stat (MACD_TEMPLOG_FNAME, &logstat) == 0 && logstat.st_size > 0) {
	    (void) sprintf (cmd, "cat %s %s > %s", 
	        lastlogf, MACD_TEMPLOG_FNAME, MACD_LASTLOG_FNAME);
	    (void) system (cmd); 
	    (void) unlink (lastlogf);
	    if (rename (MACD_LASTLOG_FNAME, lastlogf) != 0) {
	        (void) printf("WARNING  : %s - Fail renaming %s to %s\n",
	            timstr(0), MACD_LASTLOG_FNAME, lastlogf);
                (void) fflush(stdout);
	    }
	}

	/*
	 * log the last heart-beat time as MACDDOWN time
	 */
	if (stat (HEART_BEAT_FNAME, &logstat) != 0) {
	    if (errno == ENOENT) {
	        if (creat (HEART_BEAT_FNAME, S_IRUSR) < 0) {
		    (void) fprintf (stderr, "WARNING  : %s - cannot create %s\n",
		        timstr(0), HEART_BEAT_FNAME);
		    (void) fflush (stderr);
	        }
	    }
	    else {
	        (void) fprintf (stderr,
		    "MACDERR: %s - cannot stat() %s file to record last down time\n",
		    timstr(0), HEART_BEAT_FNAME);
	        (void) fflush(stderr);
            }
        }
	else {
	    if ((fd = fopen (lastlogf, "a")) != NULL) {
		(void) fprintf(fd, "MACDDOWN : %s - MACS down\n", 
		    timstr(logstat.st_mtime)); 
                (void) fflush(fd);
	    }
	    else {
		(void) fprintf (stderr, 
		    "MACDERR: %s - cannot open %s to record last heart-beat %s\n", 
		    timstr(0), lastlogf, timstr(logstat.st_mtime));
		(void) fflush(stderr);
	    }
	}

	/*
	 * if it's a new time period switch to a new logfile
	 */
	tlastlog = *(localtime(&lasttime));
    if (_debug_) (void) fprintf (stderr, 
	"tlastlog = %d %d %d %d %d %d %d %d %d\n",
	tlastlog.tm_sec, tlastlog.tm_min, tlastlog.tm_hour, tlastlog.tm_mday,
	tlastlog.tm_mon, tlastlog.tm_year, tlastlog.tm_wday, tlastlog.tm_yday,
	tlastlog.tm_isdst);
	if (conf->switchlog == LOGMONTHLY) { 
	    if (nowtm.tm_mon != tlastlog.tm_mon) {
		(void) newlog(now, &nowtm);
		(void) free (lastlogf);
		return;
	    }
	}
        else if (conf->switchlog == LOGWEEKLY) { 
	    if (nowtm.tm_wday < tlastlog.tm_wday) {
		(void) newlog(now, &nowtm);
		(void) free (lastlogf);
		return;
	    }
	    else if (nowtm.tm_wday == tlastlog.tm_wday) {
		if (nowtm.tm_yday != tlastlog.tm_yday) {
		    (void) newlog(now, &nowtm);
		    (void) free (lastlogf);
		    return;
		}
	    }
	    else {
		if ((now - lasttime) > 7*24*3600) {
		    (void) newlog(now, &nowtm);
		    (void) free (lastlogf);
		    return;
		}
	    }
	}
	else {	/* conf->switchlog == LOGDAILY */
	    if (_debug_) fprintf (stderr, 
		"switch daily, nowtm.tm_yday=%d tlastlog.tm_yday=%d\n", 
		nowtm.tm_yday, tlastlog.tm_yday);
	    if (nowtm.tm_yday != tlastlog.tm_yday) {
		(void) newlog(now, &nowtm);
		(void) free (lastlogf);
		return;
	    }
	}
    }
    else {	/* no last log file */
	(void) newlog(now, &nowtm);
	(void) free (lastlogf);
        return;
    }

    /*
     * should use last logfile
     */
    if (access(lastlogf, F_OK) != 0) {
	/*
	 * switch to new log file anyway
	 */
	(void) newlog(now, &nowtm);
	(void) free (lastlogf);
	return;
    }

    (void) fflush(stdout);
    (void) freopen(lastlogf, "a", stdout);
    (void) free (lastlogf);
}


/*****************************************************************************
* 
* _sendmail(reason, pass_ref) 
* 
* Function _sendmail() sends mail to the user specified in ref for the 
* specified accounting reason. 
* 
*****************************************************************************/

void _sendmail(reason, pass_ref)
int reason;
struct app_ref *pass_ref;

{
    FILE *mail;
    char buf[BUFSIZ], cmd[256];
    int pass_err=0;
    struct user_ent *user_ptr;
    struct passwd *pwent;
    extern int _debug_;
    extern struct macsconf *conf;
    extern struct passwd *getpwuid();
    extern char *timstr();

    /*
      Open up a pipe to the mail command and set up the buffering
    */

    if (_debug_) (void) fprintf (stderr, "Enter _sendmail(reason=%d, pass_ref=%d)\n",
	reason, pass_ref);
    if (reason >= SMD_DISCONN)
	(void) sprintf(cmd, "%s %s", conf->mailer, conf->admin);
    else if (pass_ref) {
	if (reason == ACCTOVER) {
	    if (pass_ref->acct_ptr) {
	        char *ptr;
	        (void) sprintf(cmd, "%s %s", conf->mailer, conf->admin);
	        for (user_ptr=pass_ref->acct_ptr->user_list, 
		    ptr=cmd+strlen(cmd); user_ptr!=NULL && strlen(cmd)<240; 
		    ptr=cmd+strlen(cmd), user_ptr=user_ptr->next) {
		    if (_debug_) (void) fprintf (stderr, "user_ptr->uid=%d\n", user_ptr->uid);
		    if (pwent = getpwuid(user_ptr->uid))
			(void) sprintf (ptr, " %s", pwent->pw_name);
		    else if (_debug_) 
			(void) fprintf (stderr, "getpwuid(%d)=NULL\n",user_ptr->uid);
		}
	    }
	    else {
		if (_debug_) (void) fprintf (stderr, "pass_ref->acct_ptr is NULL\n");
		(void) sprintf(cmd, "%s %s", conf->mailer, conf->admin);
		(void) printf("MACDMAIL : %s - _sendmail passed in NULL pass_ref->acct_ptr for ACCTOVER\n",
		    timstr(0)); 
	        (void) fflush (stdout);
	        pass_err = 1;
	    }
	}
	else if (pass_ref->user_ptr && (pwent=getpwuid(pass_ref->user_ptr->uid))) {
	    if (_debug_) (void) fprintf (stderr, "pass_ref->user_ptr->uid=%d\n",
		pass_ref->user_ptr->uid);
	    (void) sprintf(cmd, "%s %s %s", conf->mailer, conf->admin, 
	        pwent->pw_name);
	}
	else {
	    (void) sprintf(cmd, "%s %s", conf->mailer, conf->admin);
	    if (pass_ref->user_ptr == NULL) {
	        if (_debug_) (void) fprintf (stderr, "pass_ref->user_ptr=NULL\n");
		(void) printf("MACDMAIL : %s - _sendmail passed in NULL pass_ref->user_ptr\n",
		    timstr(0)); 
	        (void) fflush (stdout);
	    }
	    else {
		if (_debug_) (void) fprintf (stderr, "Fail to getpwuid(%d)\n",
			pass_ref->user_ptr->uid);
		(void) printf("MACDMAIL : %s - Fail to getpwuid(%d), no mail send to uid=%d\n",
		    timstr(0), pass_ref->user_ptr->uid, pass_ref->user_ptr->uid);
	    }
	    (void) fflush (stdout);
	    pass_err = 2;
	}
    }
    else (void) sprintf(cmd, "%s %s", conf->mailer, conf->admin);

    if ((mail = popen(cmd, "w")) == (FILE *) NULL)
      {
	if (_debug_ || reason >= SMD_DISCONN)
	(void) printf("MACDMAIL : %s - Failed popen(), errno=%d.  Unable to %s\n", 
	    timstr(0), errno, cmd);
	(void) fflush (stdout);
	return;
      }
    setbuf(mail, buf); 

    /*
      Mail the user about what we are going to do
    */

    if (pass_err) {
	if (_debug_) (void) fprintf (stderr, "pass_err=%d\n", pass_err);
	if (pass_err == 1)
	    (void) fprintf(mail, "MACDMAIL : %s\n_sendmail passed in NULL pass_ref->acct_ptr for ACCTOVER\n",
		timstr(0));
	else {
            if (pass_ref->user_ptr == NULL)
                (void) fprintf(mail, "MACDMAIL : %s\n_sendmail passed in NULL pass_ref->user_ptr\n",
		    timstr(0));
            else (void) fprintf(mail, "MACDMAIL : %s\nFail to getpwuid(%d), no mail send to uid=%d\n",
                timstr(0), pass_ref->user_ptr->uid, pass_ref->user_ptr->uid);
	}
    }
    else if (reason == SMD_DISCONN) {
	if (_debug_) (void) fprintf (stderr, "reason == SMD_DISCONN\n");
	(void) fprintf (mail, 
	"MACD failed in reconnecting to \n");
	(void) fprintf (mail, 
	"Scheduler Monitor Deamon.  Please check MACD log files\n");
	(void) fprintf (mail,
	  "under %s for more information\n", MACS_LOG_PATH);
    }
    else if (reason == DEF_NQSTABLE) {
	if (_debug_) (void) fprintf (stderr, "reason == DEF_NQSTABLE\n");
	(void) fprintf (mail, 
	"MACD failed in getting queue charges from %s.\n", MACS_NQSTABLE_FILE);
	(void) fprintf (mail, 
	"It's running with the default nqstable which contains\n");
	(void) fprintf (mail,
	  "only the interactive queue.\n");
    }
    else if (reason == ERR_NQSTABLE) {
	if (_debug_) (void) fprintf (stderr, "reason == ERR_NQSTABLE\n");
	(void) fprintf (mail, 
	"MACD aborted due to format error in %s.\n", MACS_NQSTABLE_FILE);
    }
    else if (reason == ERR_MACHEART) {
	if (_debug_) (void) fprintf (stderr, "reason == ERR_MACHEART\n");
	(void) fprintf (mail, "MACD failed in touching the file %s\n", 
		HEART_BEAT_FNAME);
    }
    else if (reason == QNL_NQSTABLE) {
	if (_debug_) (void) fprintf (stderr, "reason == QNL_NQSTABLE\n");
	(void) fprintf (mail, 
	"MACD aborted due to queue name too long (>32 characters) in %s.\n", 
	MACS_NQSTABLE_FILE);
    }
    else if (reason == DB_INIT) {
	if (_debug_) (void) fprintf (stderr, "reason == DB_INIT\n");
	(void) fprintf (mail,
        "MACD failed in initializing database, please check file %s\n",MACD_DATABASE_FILE);
    }
    else if (reason == ACCTOVER) {
	if (_debug_) (void) fprintf (stderr, "reason == ACCTOVER\n");
	(void) fprintf (mail, 
	"You have exceeded the allocation under account %d\n",
	pass_ref->acct_ptr->acct_id);
    }
    else if (reason == ACCT_STOP) {
	if (_debug_) (void) fprintf (stderr, "reason == ACCT_STOP\n");
	(void) fprintf (mail, 
	"You have exceeded the allocation under account %d\n",
	pass_ref->acct_ptr->acct_id);
	(void) fprintf (mail, "Your processes have been terminated...\n");
    }
    else if (reason == USEROVER) {
	if (_debug_) (void) fprintf (stderr, "reason == USEROVER\n");
	(void) fprintf (mail, 
	"You have exceeded the allocation under account %d uid %d\n",
	pass_ref->acct_ptr->acct_id, pass_ref->user_ptr->uid);
    }
    else if (reason == USERNOUSE) {
	if (_debug_) (void) fprintf (stderr, "reason == USERNOUSE\n");
	(void) fprintf (mail, 
	"Your account (account_id %d uid %d) has been disabled\n",
	pass_ref->acct_ptr->acct_id, pass_ref->user_ptr->uid);
	(void) fprintf (mail, "Your processes have been terminated...\n");
    }
    else if (reason == USER_STOP) {
	if (_debug_) (void) fprintf (stderr, "reason == USER_STOP\n");
	(void) fprintf (mail, 
	    "You have exceeded the allocation under account %d uid %d\n",
	    pass_ref->acct_ptr->acct_id, pass_ref->user_ptr->uid);
	(void) fprintf (mail, "Your processes have been terminated...\n");
    }
    else if (reason == ERR_MACLOG) {
	if (_debug_) (void) fprintf (stderr, "reason == ERR_MACLOG\n");
	(void) fprintf (mail, 
	    "Write to MACS logfile in directory %s failed!  (File system full?)\n",
	    MACD_LOG_PATH);
    }
    
    /*
      Leave with a friendly message - who likes their job killed?
    */

    (void) fprintf(mail, "\nSincerely,\nYour friendly MACD\n");
    (void) pclose(mail);

    (void) printf("MACDMAIL : %s - %s\n", timstr(0), cmd);
    (void) fflush (stdout);
    if (_debug_) (void) fprintf (stderr, "exit _sendmail\n");
    return;
}


/*
     print out the contents of various data structures macd uses
     mainly for debugging purposes
*/


void dump_ref (s)
char *s;
{

     extern struct app_ref macd_ref;

     (void) fprintf (stderr, "%s: dump macd_ref:\n", s);
     if (macd_ref.acct_ptr == NULL) (void) fprintf (stderr,
	"macd_ref.acct_ptr = NULL\n");
     else {
	(void) fprintf (stderr,
	  "Acct=%d nuser=%d nnodes=%d node_rate=%f time=%d ",
	  macd_ref.acct_ptr->acct_id,
	  macd_ref.acct_ptr->nuser,
	  macd_ref.acct_ptr->nnodes,
	  macd_ref.acct_ptr->node_rate,
	  macd_ref.acct_ptr->cpu_time);
	if (macd_ref.acct_ptr->user_list == NULL)
		(void) fprintf (stderr, "user_list=NULL ");
	else(void) fprintf (stderr, "user_list=%d ",
	  	macd_ref.acct_ptr->user_list);
	if (macd_ref.acct_ptr->next == NULL)
		(void) fprintf (stderr, "next_acct=NULL ");
	else (void) fprintf (stderr, "next_acct=%d ",
	  	macd_ref.acct_ptr->next);
	if (macd_ref.acct_ptr->req_id[0] == '\0')
		(void) fprintf (stderr, "req_id=NULL\n");
	else (void) fprintf (stderr, "req_id=%s\n",
	  	macd_ref.acct_ptr->req_id);
     }
     (void) fprintf (stderr, "macd_ref.prev_acct = %d\n", macd_ref.prev_acct);

     if (macd_ref.user_ptr == NULL) (void) fprintf (stderr,
	"macd_ref.user_ptr = NULL\n");
     else {
	(void) fprintf (stderr, 
	"Uid=%d npart=%d nnodes=%d node_rate=%f time=%d",
	  macd_ref.user_ptr->uid,
	  macd_ref.user_ptr->npart,
	  macd_ref.user_ptr->nnodes,
	  macd_ref.user_ptr->node_rate,
	  macd_ref.user_ptr->cpu_time);
	if (macd_ref.user_ptr->part_list == NULL)
		(void) fprintf (stderr, " part_list=NULL");
	else (void) fprintf (stderr, " part_list=%d",
	  	macd_ref.user_ptr->part_list);
	if (macd_ref.user_ptr->next == NULL)
		(void) fprintf (stderr, " next_user=NULL");
	else (void) fprintf (stderr, " next_user=%d", macd_ref.user_ptr->next);
 	if (macd_ref.user_ptr->req_id[0] == '\0')
		(void) fprintf (stderr, " req_id=NULL\n");
	else (void) fprintf (stderr, " req_id=%s\n", macd_ref.user_ptr->req_id);
     }
     (void) fprintf (stderr, "macd_ref.prev_user = %d\n", macd_ref.prev_user);

     if (macd_ref.part_ptr == NULL) (void) fprintf (stderr,
	"macd_ref.part_ptr = NULL\n");
     else {
	(void) fprintf (stderr, 
	"Part_id=%d start=%d size=%d npg=%d rate=%f time=%d under=%d idle=%d last=%d match=%d queue=%s",
	  macd_ref.part_ptr->part_id,
	  macd_ref.part_ptr->start_time,
	  macd_ref.part_ptr->part_size,
	  macd_ref.part_ptr->npg,
	  macd_ref.part_ptr->charge_rate,
	  macd_ref.part_ptr->cpu_time,
	  macd_ref.part_ptr->under_used,
	  macd_ref.part_ptr->idle_time,
	  macd_ref.part_ptr->last_update,
	  macd_ref.part_ptr->match,
	  macd_ref.part_ptr->queue_name);
	if (macd_ref.part_ptr->pg_list == NULL)
		(void) fprintf (stderr, " pg_list=NULL");
	else (void) fprintf (stderr, " pg_list=%d", macd_ref.part_ptr->pg_list);
	if (macd_ref.part_ptr->next == NULL) 
		(void) fprintf (stderr, " next_part=NULL\n");
	else (void) fprintf (stderr, " next_part=%d\n", macd_ref.part_ptr->next);
     }
     (void) fprintf (stderr, "macd_ref.prev_part = %d\n", macd_ref.prev_part);

     if (macd_ref.pg_ptr == NULL) (void) fprintf (stderr,
	"macd_ref.pg_ptr = NULL\n");
     else {
	(void) fprintf (stderr, 
	"Pgid=%d size=%d time=%d start=%d last=%d match=%d ",
	  macd_ref.pg_ptr->pgid,
	  macd_ref.pg_ptr->app_size,
	  macd_ref.pg_ptr->cpu_time,
	  macd_ref.pg_ptr->start_time,
	  macd_ref.pg_ptr->last_update,
	  macd_ref.pg_ptr->match);
	  if (macd_ref.pg_ptr->next == NULL) 
		(void) fprintf (stderr, "next_pg=NULL\n");
	else (void) fprintf (stderr, "next_pg=%d\n", macd_ref.pg_ptr->next);
     }
     (void) fprintf (stderr, "macd_ref.prev_pg = %d\n", macd_ref.prev_pg);

     (void) fflush (stderr);
     return;
}

void dump_job (s, nqs_data)
char *s;
struct nqs_job_info *nqs_data;
{
     (void) fprintf (stderr, "%s: dump nqs_data:\n", s);
     if (nqs_data == NULL) {
	(void) fprintf (stderr, "nqs_data=NULL\n");
	return;
     }
#ifndef INTELv1r1
     (void) fprintf (stderr, "uid=%d acct=%d part=%d size=%d type=%d time=%d cpu=%d active=%d idle=%d queue=%s submit=%d request=%d\n",
#else
     (void) fprintf (stderr, "uid=%d acct=%d part=%d size=%d type=%d time=%d cpu=%d active=%d idle=%d queue=%s\n",
#endif
	nqs_data->uid,
	nqs_data->acct_id,
	nqs_data->part_id,
	nqs_data->part_size,
/*
	nqs_data->node_type,
*/
	0,
	nqs_data->event_time,
	nqs_data->cpu_time,
	nqs_data->part_active,
	nqs_data->part_idle,
#ifndef INTELv1r1
	nqs_data->queue_name,
	nqs_data->submit_time,
	nqs_data->requested_time);
#else
	nqs_data->queue_name);
#endif
     (void) fflush (stderr);
}


void dump_smd (s, smd_data)
char *s;
struct smd_resp *smd_data;
{
     (void) fprintf (stderr, "%s: dump smd_data:\n", s);
     if (smd_data == NULL) {
	(void) fprintf (stderr, "smd_data=NULL\n");
	return;
     }
     if (smd_data->q_id.uid<0) smd_data->q_id.uid *= (-1);
     (void) fprintf (stderr, "acct=%d uid=%d part=%d pgid=%d req_id=%16s\n",
	smd_data->q_id.acct_id,
	smd_data->q_id.uid,
	smd_data->q_id.part_id,
	smd_data->q_id.pgid,
	smd_data->q_id.req_id);
     (void) fprintf (stderr, "status=%d global=%f event=%d cpu=%d appsize=%d priority=%d partsize=%d\n",
	smd_data->value.status,
	smd_data->value.global_time,
	smd_data->value.event_time,
	smd_data->value.cpu_time,
	smd_data->value.app_size,
	smd_data->value.app_priority,
	smd_data->value.part_size);
     (void) fflush (stderr);
}
