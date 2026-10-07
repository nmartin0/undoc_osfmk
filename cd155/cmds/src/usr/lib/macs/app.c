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
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * File:	app.c
 *
 * Abstract:	This file contains all SMD im-coming message handlers
 */

#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <malloc.h>
#include "macd.h"
#include "db.h"
#include "conf.h"
#include "appacct.h"
#include "smd.h"
#include "filename.h"

/******************************************************************************
*
* appStart() 
*
* Abstract:	This routine processes APP_START data from SMD
*		It searches the application tree for a pgid match,
*		a match will be treated as if the application has
*		ended.  Then a new application for this pgid is
*		added into the application tree.
*
* Arguments:	sm_data_size -	size of the data
*		sm_data -	address of the data
*
* Return value:	None
*
******************************************************************************/

void appStart(sm_data_size, sm_data)
int sm_data_size;
struct smd_resp *sm_data;

{
    int rval;
    long idle_time;
    extern struct app_ref macd_ref;
    extern struct macsconf *conf;
    extern int appNew();
    extern void appGone();
    extern int find_pg();
    extern char *timstr();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr,
            "Enter appStart(sm_data_size=%d, sm_data=%d)\n",
            sm_data_size, sm_data);
	(void) fflush (stderr);
    }

    /*
     * check the data size
     * should be struct app_info and followed by 0 or more struct app_info
     */

    if (sm_data_size < sizeof(struct smd_resp)) {
	(void) printf("WARNING  : %s - Invalid APP_START message-size=%d from SMD, message discarded\n",
		timstr(0), sm_data_size);
	(void) fflush (stdout);
	(void) free (sm_data);
	return;
    }
if (_debug_) (void) dump_smd ("appStart", sm_data);

    /*
     * check the application size
     */
 
    if (sm_data->value.app_size > conf->total_nodes) {
        (void) printf("WARNING  : %s - Invalid app_size=%d in APP_START message from SMD, message discarded\n",
                timstr(0), sm_data->value.app_size);
	(void) fflush (stdout);
        (void) free (sm_data);
        return;
    }

    /*
     * Change time stamp to MACD's local time.  Temp. solution for
     * time difference amoung different cpu's
     */
    sm_data->value.event_time = time(0);

    /*
     * process APP_START data
     */

    if (find_pg (sm_data->q_id.pgid) == 0) {
        (void) printf("WARNING  : %s - APP_START pgid=%d, already exists\n",
                 timstr(0), sm_data->q_id.pgid);
	(void) fflush (stdout);
	(void) appGone (0, 0);
    }

    if (set_ref (sm_data) == 0) {
        (void) printf("WARNING  : %s - APP_START pgid=%d, already exists\n",
                 timstr(0), sm_data->q_id.pgid);
	(void) fflush (stdout);
        (void) appGone (0, 0);
    }

    idle_time = 0;
    if (macd_ref.part_ptr && !macd_ref.part_ptr->npg) {
	idle_time = sm_data->value.event_time - macd_ref.part_ptr->last_update;
        if (idle_time > sm_data->value.event_time - macd_ref.part_ptr->start_time)
	    (void) printf("WARNING  : %s - APP_START bad idle time %d, sm_data->value.event_time %d, macd_ref.part_ptr->last_update %d\n",
		 timstr(0), idle_time, sm_data->value.event_time, macd_ref.part_ptr->last_update);
    }
    if (idle_time < 0) idle_time = 0;

    if (appNew (sm_data) != 0) {
         (void) printf("WARNING  : %s - Fail adding APP_START pgid=%d in application list\n",
                 timstr(0), sm_data->q_id.pgid);
	(void) fflush (stdout);
         (void) free (sm_data);
         return;
    }

    if (conf->macdmode != ACCTONLY && (rval = upd_db (0, 0, idle_time)) < 0) {
        (void) printf("WARNING  : %s - %s, Fail in checking account balance\n",
            timstr(0), rval==-1 ? "NULL ref. pointer to application entry" :
            rval==-2 ? "Fail saving defered usage data" :
            "Invalid account-id or User-id");
	(void) fflush (stdout);
    }

    (void) free (sm_data);
}


/******************************************************************************
*
* appNew() 
*
* Abstract:	This routine add an application information into the
*		application tree.  It assumes the macd_ref is set to
*		point the add-points in the application tree.
*
* Arguments:	sm_data_size -	size of the data
*		sm_data -	address of the data
*
* Return value:	0	successful
*		else	error
*
******************************************************************************/

int appNew (sm_data)
struct smd_resp *sm_data;
{
    extern struct app_ref macd_ref;
    extern struct acct_ent *top_link;
    extern struct macsconf *conf;
    extern char * calloc();

    /* new account, add new account entry */
    if (macd_ref.acct_ptr == NULL) {
	macd_ref.acct_ptr = (struct acct_ent *) calloc (1, sizeof (struct acct_ent));
	if (macd_ref.acct_ptr == NULL) {
	    (void) printf("WARNING  : %s - Memory allocation failure in appNew() for acct_ent\n",
		timstr(0));
	    (void) fclose (stdout);
	    exit(1);
	}
	macd_ref.acct_ptr->acct_id = sm_data->q_id.acct_id;
	if (macd_ref.prev_acct != NULL && macd_ref.prev_acct->next == NULL)
	    macd_ref.prev_acct->next = macd_ref.acct_ptr;
	else {
	    if (top_link != NULL) macd_ref.acct_ptr->next = top_link;
	    top_link = macd_ref.acct_ptr;
	    macd_ref.prev_acct = NULL;
	}
    }
	
    /* new user, add a user entry */
    if (macd_ref.user_ptr == NULL) {
	macd_ref.user_ptr = (struct user_ent *) calloc (1, sizeof (struct user_ent));
	if (macd_ref.user_ptr == NULL) {
	    (void) printf("WARNING  : %s - Memory allocation failure in appNew() for user_ent\n",
		timstr(0));
	    (void) fclose (stdout);
	    exit(1);
	}
	macd_ref.user_ptr->uid = sm_data->q_id.uid;
	macd_ref.acct_ptr->nuser++;
	if (macd_ref.prev_user != NULL && macd_ref.prev_user->next == NULL)
	    macd_ref.prev_user->next = macd_ref.user_ptr;
	else {
	    if (macd_ref.acct_ptr->user_list != NULL) 
		macd_ref.user_ptr->next = macd_ref.acct_ptr->user_list;
	    macd_ref.acct_ptr->user_list = macd_ref.user_ptr;
	    macd_ref.prev_user = NULL;
	}
    }
	
    /* new partition, add a new partition entry */
    if (macd_ref.part_ptr == NULL) {
	macd_ref.part_ptr = (struct part_ent *) calloc (1, sizeof (struct part_ent));
	if (macd_ref.part_ptr == NULL) {
	    (void) printf("WARNING  : %s - Memory allocation failure in appNew() for part_ent\n",
		timstr(0));
	    (void) fclose (stdout);
	    exit(1);
	}
	macd_ref.part_ptr->part_id = sm_data->q_id.part_id;
	macd_ref.part_ptr->part_size = sm_data->value.app_size;
	macd_ref.part_ptr->charge_rate = conf->def_qrate;
	macd_ref.user_ptr->npart++;
	macd_ref.part_ptr->start_time = 
	    macd_ref.part_ptr->last_update = sm_data->value.event_time;
	if (macd_ref.prev_part != NULL && macd_ref.prev_part->next == NULL)
	    macd_ref.prev_part->next = macd_ref.part_ptr;
	else {
	    if (macd_ref.user_ptr->part_list != NULL) 
		macd_ref.part_ptr->next = macd_ref.user_ptr->part_list;
	    macd_ref.user_ptr->part_list = macd_ref.part_ptr;
	    macd_ref.prev_part = NULL;
	}
    }

    /* new application, add process group entry */
    if (macd_ref.pg_ptr == NULL) {
	macd_ref.pg_ptr = (struct pg_ent *) calloc (1, sizeof (struct pg_ent));
	if (macd_ref.pg_ptr == NULL) {
	    (void) printf("WARNING  : %s - Memory allocation failure in appNew() for pg_ent\n",
		timstr(0));
	    (void) fclose (stdout);
	    exit(1);
	}

	macd_ref.pg_ptr->pgid = sm_data->q_id.pgid;
        if (!macd_ref.part_ptr->npg &&
	    sm_data->value.event_time > macd_ref.part_ptr->last_update) {
	    macd_ref.part_ptr->idle_time +=
		sm_data->value.event_time - macd_ref.part_ptr->last_update;
    	    if (macd_ref.part_ptr->idle_time > 
		sm_data->value.event_time - macd_ref.part_ptr->start_time)
        	(void) printf("WARNING  : %s - APP_NEW bad idle time %d, sm_data->value.event_time %d, macd_ref.part_ptr->last_update %d\n",
                 timstr(0), macd_ref.part_ptr->idle_time, 
		sm_data->value.event_time, macd_ref.part_ptr->last_update);
	}
	if (macd_ref.part_ptr->last_update < sm_data->value.event_time)
	    macd_ref.part_ptr->last_update = sm_data->value.event_time;
        macd_ref.part_ptr->npg++;
	macd_ref.pg_ptr->app_size = sm_data->value.app_size;
	macd_ref.pg_ptr->start_time = sm_data->value.event_time;
	macd_ref.pg_ptr->last_update = sm_data->value.event_time;
	macd_ref.acct_ptr->nnodes += sm_data->value.app_size;
	macd_ref.acct_ptr->node_rate += 
		sm_data->value.app_size * macd_ref.part_ptr->charge_rate;
	macd_ref.user_ptr->nnodes += sm_data->value.app_size;
	macd_ref.user_ptr->node_rate += 
		sm_data->value.app_size * macd_ref.part_ptr->charge_rate;

	if (macd_ref.prev_pg != NULL && macd_ref.prev_pg->next == NULL)
	    macd_ref.prev_pg->next = macd_ref.pg_ptr;
	else {
	    if (macd_ref.part_ptr->pg_list != NULL) 
		macd_ref.pg_ptr->next = macd_ref.part_ptr->pg_list;
	    macd_ref.part_ptr->pg_list = macd_ref.pg_ptr;
	    macd_ref.prev_pg = NULL;
	}

    }

    /*
     * make log entry for application start
     */
    (void) printf("BEGINAPP : %s\n", timstr(sm_data->value.event_time));
    (void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d Rate=%f\n",
    	macd_ref.acct_ptr->acct_id,
    	macd_ref.user_ptr->uid, 
    	macd_ref.part_ptr->part_id,
    	macd_ref.pg_ptr->pgid, 
    	macd_ref.pg_ptr->app_size, 
    	macd_ref.pg_ptr->cpu_time,
	macd_ref.part_ptr->charge_rate);
    (void) fflush(stdout);
    return (0);
}


/******************************************************************************
*
* appEnd() 
*
* Abstract:	This routine processes APP_END data from SMD
*		It searches the application tree for a pgid match,
*		if no match is found then a new application for this 
*		pgid is added into the application tree.  Then it
*		updates resource usage in MACD database, log the
*		application-end event and remove the application from
*		the application tree.
*
* Arguments:	sm_data_size -	size of the data
*		sm_data -	address of the data
*
* Return value:	None
*
******************************************************************************/

void appEnd(sm_data_size, sm_data)
int sm_data_size;
struct smd_resp *sm_data;

{
    int rval;
    long rollin;
    extern struct app_ref macd_ref;
    extern struct macsconf *conf;
    extern int appNew();
    extern void appGone();
    extern char *timstr();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr,
            "Enter appEnd(sm_data_size=%d, sm_data=%d)\n",
            sm_data_size, sm_data);
	(void) fflush (stderr);
    }

    /*
     * check the data size
     * should be struct app_info and followed by 0 or more struct app_info
     */

    if (sm_data_size < sizeof(struct smd_resp)) {
	(void) printf("WARNING  : %s - Invalid APP_END message size=%d from SMD, message discarded\n",
		timstr(0), sm_data_size);
	(void) fflush (stdout);
	(void) free (sm_data);
	return;
    }

    /*
     * check the application size
     */
 
    if (sm_data->value.app_size > conf->total_nodes) {
        (void) printf("WARNING  : %s - Invalid app_size=%d in APP_END message from SMD, message discarded\n",
                timstr(0), sm_data->value.app_size);
	(void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
		sm_data->q_id.acct_id,
    		sm_data->q_id.uid, 
    		sm_data->q_id.part_id,
    		sm_data->q_id.pgid, 
    		sm_data->value.app_size, 
    		sm_data->value.cpu_time); 
	(void) fflush (stdout);
        (void) free (sm_data);
        return;
    }
if (_debug_) (void) dump_smd ("appEnd", sm_data);

    /*
     * Change time stamp to MACD's local time.  Temp. solution for
     * time difference amoung different cpu's
     */
    sm_data->value.event_time = time(0);

    /*
     * process APP_END data
     */

    if (set_ref (sm_data) != 0) {
        (void) printf("WARNING  : %s - No matching pgid=%d to APP_END\n",
                 timstr(0), sm_data->q_id.pgid);
	(void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
		sm_data->q_id.acct_id,
    		sm_data->q_id.uid, 
    		sm_data->q_id.part_id,
    		sm_data->q_id.pgid, 
    		sm_data->value.app_size, 
    		sm_data->value.cpu_time); 
	(void) fflush (stdout);
/*
	if (appNew (sm_data) != 0) {
	    (void) printf("WARNING  : %s - Fail adding pgid=%d in application list\n",
                 timstr(0), sm_data->q_id.pgid);
	    (void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
		sm_data->q_id.acct_id,
    		sm_data->q_id.uid, 
    		sm_data->q_id.part_id,
    		sm_data->q_id.pgid, 
    		sm_data->value.app_size, 
    		sm_data->value.cpu_time); 
	    (void) fflush (stdout);
    	    (void) free (sm_data);
	    return;
        }
	if ((rollin = sm_data->value.cpu_time) < 0) {
	    (void) printf("WARNING  : %s - Invalid cpu_time=%d received from SMD, rollin set to 0\n",
                 timstr(0), sm_data->value.cpu_time);
	    (void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
		sm_data->q_id.acct_id,
    		sm_data->q_id.uid, 
    		sm_data->q_id.part_id,
    		sm_data->q_id.pgid, 
    		sm_data->value.app_size, 
    		sm_data->value.cpu_time); 
	    (void) fflush (stdout);
	    rollin = sm_data->value.cpu_time = 0;
        }
*/
	return;
    } else {
        /* application was found by set_ref() */

        /* calculate maximum possible total CPU time 
           (total runtime * # of nodes) */
        time_t max_cpu_time = (sm_data->value.event_time - 
			       macd_ref.pg_ptr->start_time) * 
    	                      sm_data->value.app_size;

        if (sm_data->value.cpu_time > max_cpu_time) {
    	/* SMD says app has accumulated more time than it
    	   physically could -- reduce value to maximum */
            (void) printf("WARNING  : %s - Too large cpu_time=%d received from SMD; reduced to maximum (%d)\n",
                          timstr(0), sm_data->value.cpu_time, max_cpu_time);
            (void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
    	    sm_data->q_id.acct_id,
    	    sm_data->q_id.uid, 
    	    sm_data->q_id.part_id,
    	    sm_data->q_id.pgid, 
    	    sm_data->value.app_size, 
    	    sm_data->value.cpu_time); 
            (void) fflush (stdout);
	    sm_data->value.cpu_time = max_cpu_time;
        }

	/* calculate rollin value for upd_db() and check validity */
        if ((rollin = sm_data->value.cpu_time - macd_ref.pg_ptr->cpu_time) < 0) {
	    /* SMD says app has accumulated less time than we think it 
	       has; update macd_ref.pg_ptr->cpu_time, but don't charge 
	       user anything */
            rollin = 0;
            (void) printf("WARNING  : %s - Invalid cpu_time=%d received from SMD, prev_accum=%d, rollin set to 0\n",
                     timstr(0), sm_data->value.cpu_time, macd_ref.pg_ptr->cpu_time);
	    (void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
		 sm_data->q_id.acct_id,
    		 sm_data->q_id.uid, 
    		 sm_data->q_id.part_id,
    		 sm_data->q_id.pgid, 
    		 sm_data->value.app_size, 
    		 sm_data->value.cpu_time); 
	    (void) fflush (stdout);
	}
    }

    /*
     * update active apps. accumulators
     */
    macd_ref.user_ptr->cpu_time += rollin;
    macd_ref.acct_ptr->cpu_time += rollin;
    macd_ref.part_ptr->cpu_time += rollin;

    /*
     * update and timestamp the matching partition entry and pg entry
     */
    macd_ref.pg_ptr->cpu_time = sm_data->value.cpu_time;
    macd_ref.pg_ptr->last_update = sm_data->value.event_time;
    macd_ref.part_ptr->last_update = sm_data->value.event_time;

    /*
     *  update usage and charge in macd database
     */
    if (conf->macdmode != ACCTONLY && rollin &&
	(rval = upd_db (rollin, 0, 0)) < 0) {
	(void) printf("WARNING  : %s - %s, usage-update failed\n",
            timstr(0), rval==-1 ? "NULL ref. pointer to application entry" : 
	    rval==-2 ? "Fail saving defered usage data" : 
	    "Invalid account-id or User-id");
	(void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
		sm_data->q_id.acct_id,
    		sm_data->q_id.uid, 
    		sm_data->q_id.part_id,
    		sm_data->q_id.pgid, 
    		sm_data->value.app_size, 
    		sm_data->value.cpu_time); 
	(void) fflush(stdout);
    }
    
    /*
     * release entries related to the application
     */
    (void) appGone (0, sm_data->value.event_time);

    (void) free (sm_data);
}


/******************************************************************************
*
* appGone ()
*
* Abstract:	This routine logs the application-end event, remove the
*		application entry from the application tree.  It assumes
*		the macd_ref is set to point the remove-points in the tree.
* 		It will release empty partition and empty user links 
* 		if the part_done=1 or the application is under an interactive
*		partition 
*
* Arguments:	part_done -	release empty partition/user/account branch
*
* Return value:	0	successful
*		else	error
*
******************************************************************************/

void appGone (part_done, term_time)
int part_done;
long term_time;
{
    FILE *lastlog;
    extern struct acct_ent *top_link;
    extern struct app_ref macd_ref;

    if (_debug_ && macd_ref.pg_ptr == NULL) {
        (void) fprintf(stderr, "Enter appGone() and macd_ref.pg_ptr == NULL\n");
	(void) fflush (stderr);
    }
    if (macd_ref.pg_ptr != NULL) {
        /*
         * make log entry for application end
         */
        (void) printf("ENDAPP   : %s\n", timstr(term_time));
        (void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d Rate=%f\n",
        	macd_ref.acct_ptr->acct_id,
        	macd_ref.user_ptr->uid, 
        	macd_ref.part_ptr->part_id,
        	macd_ref.pg_ptr->pgid, 
        	macd_ref.pg_ptr->app_size, 
        	macd_ref.pg_ptr->cpu_time,
		macd_ref.part_ptr->charge_rate);
        (void) fflush(stdout);

	/*
	 * update active apps. accumulators
	 */
	macd_ref.user_ptr->nnodes -= macd_ref.pg_ptr->app_size;
	macd_ref.user_ptr->node_rate -= 
	    macd_ref.pg_ptr->app_size * macd_ref.part_ptr->charge_rate;
	macd_ref.acct_ptr->nnodes -= macd_ref.pg_ptr->app_size;
	macd_ref.acct_ptr->node_rate -= 
	    macd_ref.pg_ptr->app_size * macd_ref.part_ptr->charge_rate;
	macd_ref.user_ptr->cpu_time -= macd_ref.pg_ptr->cpu_time;
	macd_ref.acct_ptr->cpu_time -= macd_ref.pg_ptr->cpu_time;
        
        /*
         * free the pg_ent
         */
        if (macd_ref.prev_pg == NULL)
    	    macd_ref.part_ptr->pg_list = macd_ref.pg_ptr->next;
        else macd_ref.prev_pg->next = macd_ref.pg_ptr->next;
        (void) free (macd_ref.pg_ptr);
	macd_ref.pg_ptr = NULL;
        if (macd_ref.part_ptr->npg > 0) macd_ref.part_ptr->npg--;
    }
    
    if (macd_ref.part_ptr != NULL) {
	/*
	 * free the partition entry if there is no more application 
	 * running under it and if we have already got job-end message 
	 * from nqs or if this is an interactive partition
	 * (normally we would receive all app-end from SMD before get
	 * job-end from NQS, but it's possible we could get an app-end
	 * after job-end.
	 */
	if (macd_ref.part_ptr->npg <= 0 && (part_done || 
	    macd_ref.part_ptr->queue_name == NULL ||
            strlen(macd_ref.part_ptr->queue_name) <= 0 ||
	    strcmp(macd_ref.part_ptr->queue_name,"UNKNOWN") == 0)) {
	    if (macd_ref.prev_part == NULL)
		    macd_ref.user_ptr->part_list = macd_ref.part_ptr->next;
	    else macd_ref.prev_part->next = macd_ref.part_ptr->next;
	    (void) free (macd_ref.part_ptr);
	    macd_ref.part_ptr = NULL;
	    macd_ref.user_ptr->npart--;
	}
	else return;
    }
    
    if (macd_ref.user_ptr != NULL) {
	if (macd_ref.user_ptr->npart <= 0) {
	    /*
	     * remove an user entry if there is no more
	     * partitions under it
	     */
	    if (macd_ref.user_ptr->req_id[0] != '\0') 
		(void) smAlarmCancel(USER_ALARM, &macd_ref);
	    if (macd_ref.prev_user == NULL)
		macd_ref.acct_ptr->user_list = macd_ref.user_ptr->next;
	    else macd_ref.prev_user->next = macd_ref.user_ptr->next;
	    (void) free (macd_ref.user_ptr);
	    macd_ref.user_ptr = NULL;
	    macd_ref.acct_ptr->nuser--;
	}
	else return;
    }

    if (macd_ref.acct_ptr == NULL) return;
    if (macd_ref.acct_ptr->nuser > 0) return;

    /*
     * remove an account entry if there is no more
     * user under it
     */
    if (macd_ref.acct_ptr->req_id[0] != '\0') 
	(void) smAlarmCancel(ACCT_ALARM, &macd_ref);
    if (macd_ref.prev_acct == NULL)
	top_link = macd_ref.acct_ptr->next;
    else macd_ref.prev_acct->next = macd_ref.acct_ptr->next;
    (void) free (macd_ref.acct_ptr);
    macd_ref.acct_ptr = NULL;
    return;
}


/******************************************************************************
*
* appStatu() 
*
* Abstract:	This routine processes APP_STATUS data from SMD.
*		The data received is one data structure per application
*		for all currently active applications and is processed 
*		one at a time.
*		For every application, this routine searches the 
*		application tree for a pgid match.
*		if no match is found then a new application for this 
*		pgid is added into the application tree and the new entry
*		is marked with match-flag set.  
*		It updates resource usage in MACD database for every 
*		application in status message and log the status info
*		into LOGPATH/last.log file.
*		When all the status data is processed, this routine then
*		traverse the application tree and looking for application
*		entries with match-flag not set.  These application entries
*		then are processed as if APP_END events have been received
*		on them.
*
* Arguments:	sm_data_size -	size of the data
*		sm_data -	address of the data
*
* Return value:	None
*
******************************************************************************/

void appStatu(sm_data_size, sm_data)
int sm_data_size;
struct smd_resp *sm_data;

{
    int rval, remain;
    long rollin;
    time_t event_time;
    struct smd_resp *sm_app;
    extern struct app_ref macd_ref;
    extern struct macsconf *conf;
    extern char *timstr();
    extern int find_pg(), appNew();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr,
            "Enter appStatu(sm_data_size=%d, sm_data=%d)\n",
            sm_data_size, sm_data);
	(void) fflush (stderr);
    }

    /*
     * check the data size and return status
     */

    if (sm_data_size < sizeof(struct smd_resp)) {
	(void) printf("WARNING  : %s - Invalid SMD_STATUS_RESP message-size=%d from SMD, message discarded\n",
		timstr(0), sm_data_size);
	(void) fflush (stdout);
	(void) unlink (MACD_LASTLOG_FNAME);
	(void) free (sm_data);
	return;
    }
    if (_debug_) (void) dump_smd ("app_status", sm_data);
    /*
     * Change time stamp to MACD's local time.  Temp. solution for
     * time difference amoung different cpu's
     */
    event_time = time(0);

    if (sm_data->value.status != SMD_OK) {
	if (sm_data->value.status == APP_NOT_FOUND) {
		(void) appAllGone(0, 0, event_time);
		if (_debug_) {
		    (void) fprintf (stderr, 
			"APP_NOT_FOUND returned from SMD\n");
		    (void) fflush (stderr);
    		}
		(void) logjobs(0, NULL);
	}
	else {
		(void) printf("WARNING  : %s - Invalid status=%d in SMD_STATUS_RESP message from SMD, message discarded\n",
                timstr(0), sm_data_size);
		(void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
			sm_data->q_id.acct_id,
			sm_data->q_id.uid, 
			sm_data->q_id.part_id,
			sm_data->q_id.pgid, 
			sm_data->value.app_size, 
			sm_data->value.cpu_time); 
		(void) fflush (stdout);
		(void) unlink (MACD_LASTLOG_FNAME);
	}
        (void) free (sm_data);
        return;
    }
    
    /*
     * process SMD_STATUS_RESP data
     */
    (void) clr_match ();
    for (sm_app = (struct smd_resp *)sm_data, 
	remain = sm_data_size/sizeof(struct smd_resp);
	remain > 0; remain--, sm_app++) {
	if (_debug_) dump_smd ("appStatus", sm_app);
	sm_app->value.event_time = event_time;

        if (set_ref (sm_app) != 0) {
            (void) printf("WARNING  : %s - No matching pgid=%d to APP_STATUS\n",
                timstr(0), sm_app->q_id.pgid);
	    (void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
		sm_data->q_id.acct_id,
    		sm_data->q_id.uid, 
    		sm_data->q_id.part_id,
    		sm_data->q_id.pgid, 
    		sm_data->value.app_size, 
    		sm_data->value.cpu_time); 
		(void) fflush (stdout);
    	    if (appNew (sm_app) != 0) {
    	        (void) printf("WARNING  : %s - Fail adding pgid=%d in application list\n",
                     timstr(0), sm_app->q_id.pgid);
	        (void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
		    sm_data->q_id.acct_id,
    		    sm_data->q_id.uid, 
    		    sm_data->q_id.part_id,
    		    sm_data->q_id.pgid, 
    		    sm_data->value.app_size, 
    		    sm_data->value.cpu_time); 
		(void) fflush (stdout);
    	        continue;
            }
            if ((rollin = sm_app->value.cpu_time) < 0) {
                (void) printf("WARNING  : %s - Invalid cpu_time=%d received from SMD, rollin set to 0\n",
                 timstr(0), sm_app->value.cpu_time);
	        (void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
		    sm_data->q_id.acct_id,
    		    sm_data->q_id.uid, 
    		    sm_data->q_id.part_id,
    		    sm_data->q_id.pgid, 
    		    sm_data->value.app_size, 
    		    sm_data->value.cpu_time); 
		(void) fflush (stdout);
            }
	    else {
        	/*
        	 * update active apps. accumulators
        	 */
        	macd_ref.user_ptr->cpu_time += rollin;
        	macd_ref.acct_ptr->cpu_time += rollin;
        	macd_ref.part_ptr->cpu_time += rollin;
	    }
            rollin = 0;
        } else {
	    /* application was found by set_ref() */

            /* calculate maximum possible total CPU time 
	       (total runtime * # of nodes) */
	    time_t max_cpu_time = (event_time - macd_ref.pg_ptr->start_time) * 
		                  sm_data->value.app_size;

	    if (sm_app->value.cpu_time > max_cpu_time) {
		/* SMD says app has accumulated more time than it
		   physically could -- reduce value to maximum */
                (void) printf("WARNING  : %s - Too large cpu_time=%d received from SMD; reduced to maximum (%d)\n",
                     timstr(0), sm_app->value.cpu_time, max_cpu_time);
	        (void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
		     sm_data->q_id.acct_id,
    		     sm_data->q_id.uid, 
    		     sm_data->q_id.part_id,
    		     sm_data->q_id.pgid, 
    		     sm_data->value.app_size, 
    		     sm_data->value.cpu_time); 
	        (void) fflush (stdout);
	        sm_app->value.cpu_time = max_cpu_time;
            }

	    /* calculate rollin value for upd_db() and check validity */
            if ((rollin = sm_app->value.cpu_time - macd_ref.pg_ptr->cpu_time) < 0) {
		/* SMD says app has accumulated less time than we think it 
		   has; update macd_ref.pg_ptr->cpu_time, but don't charge 
		   user anything */
                rollin = 0;
                (void) printf("WARNING  : %s - Invalid cpu_time=%d received from SMD, prev_accum=%d, rollin set to 0\n",
                     timstr(0), sm_app->value.cpu_time, macd_ref.pg_ptr->cpu_time);
	        (void) printf("           Acct=%d User=%d Part=%d Pgid=%d Size=%d Time=%d\n",
		     sm_data->q_id.acct_id,
    		     sm_data->q_id.uid, 
    		     sm_data->q_id.part_id,
    		     sm_data->q_id.pgid, 
    		     sm_data->value.app_size, 
    		     sm_data->value.cpu_time); 
	        (void) fflush (stdout);
	    }

            /*
             * update active apps. accumulators
             */
            macd_ref.user_ptr->cpu_time += rollin;
            macd_ref.acct_ptr->cpu_time += rollin;
            macd_ref.part_ptr->cpu_time += rollin;
	}
	macd_ref.pg_ptr->match = 1;
    
        /*
         * update and timestamp the matching partition entry and pg entry
         */
        macd_ref.pg_ptr->cpu_time = sm_app->value.cpu_time;
        macd_ref.pg_ptr->last_update = sm_app->value.event_time;
        macd_ref.part_ptr->last_update = sm_app->value.event_time;
    
        /*
         *  update usage and charge in macd database
         */
        if (conf->macdmode != ACCTONLY && (rval = upd_db (rollin, 0, 0)) < 0) {
    	    (void) printf("WARNING  : %s - %s, usage-update failed\n",
                timstr(0), rval==-1 ? "NULL ref. pointer to application entry" :
	        rval==-2 ? "Fail saving defered usage data" : 
	        "Invalid account-id or User-id");
    	    (void) printf("           Acct=%d User=%d Part=%d Pgid=%d Node=%d Time=%d\n",
    		sm_app->q_id.acct_id,
        	sm_app->q_id.uid, 
        	sm_app->q_id.part_id,
        	sm_app->q_id.pgid, 
        	sm_app->value.app_size, 
        	sm_app->value.cpu_time); 
    	    (void) fflush(stdout);
        }
    }

    /*
     * remove no-mensioned pg_ent from linked list
     */

    (void) appAllGone (0, CHECK_MATCH, event_time);

    /*
     * log all existing jobs
     */ 
    (void) logjobs (0, NULL);

    (void) free (sm_data);
}


/******************************************************************************
*
* clrmatch ()
*
* Abstract:	This routine clears match flag in all application entries
*		in the tree.
*
* Arguments:	None
*
* Return value:	None
*
******************************************************************************/

clr_match ()
{
    extern struct acct_ent *top_link;
    extern struct app_ref macd_ref;

    for (macd_ref.acct_ptr = top_link, macd_ref.prev_acct = NULL;
	macd_ref.acct_ptr != NULL; 
	macd_ref.prev_acct = macd_ref.acct_ptr,
	macd_ref.acct_ptr = macd_ref.acct_ptr->next)
	for (macd_ref.user_ptr = macd_ref.acct_ptr->user_list,
	    macd_ref.prev_user = NULL;
	    macd_ref.user_ptr != NULL;
	    macd_ref.prev_user = macd_ref.user_ptr,
	    macd_ref.user_ptr = macd_ref.user_ptr->next)
	    for (macd_ref.part_ptr = macd_ref.user_ptr->part_list,
		macd_ref.prev_part = NULL;
		macd_ref.part_ptr != NULL;
		macd_ref.prev_part = macd_ref.part_ptr,
		macd_ref.part_ptr = macd_ref.part_ptr->next) {
		macd_ref.part_ptr->match = 0;
		for (macd_ref.pg_ptr = macd_ref.part_ptr->pg_list,
		    macd_ref.prev_pg = NULL;
		    macd_ref.pg_ptr != NULL;
		    macd_ref.prev_pg = macd_ref.pg_ptr,
		    macd_ref.pg_ptr = macd_ref.pg_ptr->next)
		    macd_ref.pg_ptr->match = 0;
	    }
}


/******************************************************************************
*
* appAllGone ()
*
* Abstract:	If check_match=0, this routine logs APP_END event for 
*		all applications entries in the appliation tree and 
*		removes those entries.  If check_match=1, then above
*		messioned is done on all application entries without
*		match flag set. 
* 		It will release empty partitions and empty user links 
* 		if the part_done=1 or the application is under 
*		interactive partition 
*
* Arguments:	part_done -	release empty partition/user/account branch
*		check_match -	see above
*		event_time -	time stamp for last_update
*
* Return value:	None
*
******************************************************************************/

appAllGone (part_done, check_match, event_time)
int part_done;
int check_match;
time_t event_time;
{
    extern struct acct_ent *top_link;
    extern struct app_ref macd_ref;

    for (macd_ref.acct_ptr = top_link, macd_ref.prev_acct = NULL;
	macd_ref.acct_ptr != NULL; 
	macd_ref.prev_acct = macd_ref.acct_ptr,
	macd_ref.acct_ptr = macd_ref.acct_ptr->next)
	for (macd_ref.user_ptr = macd_ref.acct_ptr->user_list,
	    macd_ref.prev_user = NULL;
	    macd_ref.user_ptr != NULL;
	    macd_ref.prev_user = macd_ref.user_ptr,
	    macd_ref.user_ptr = macd_ref.user_ptr->next)
	    for (macd_ref.part_ptr = macd_ref.user_ptr->part_list,
		macd_ref.prev_part = NULL;
		macd_ref.part_ptr != NULL;
		macd_ref.prev_part = macd_ref.part_ptr,
		macd_ref.part_ptr = macd_ref.part_ptr->next) {
		if (macd_ref.part_ptr->npg == 0) {
		    if (event_time > macd_ref.part_ptr->last_update) {
			macd_ref.part_ptr->idle_time += event_time -
			macd_ref.part_ptr->last_update;
            		if (macd_ref.part_ptr->idle_time >
                	    event_time - macd_ref.part_ptr->start_time)
                	    (void) printf("WARNING  : %s - APP_STATUS bad idle time %d, event_time %d, macd_ref.part_ptr->last_update %d\n",
                 		timstr(0), macd_ref.part_ptr->idle_time, 
                		event_time, macd_ref.part_ptr->last_update);

		    }
		}
		else for (macd_ref.pg_ptr = macd_ref.part_ptr->pg_list,
		    macd_ref.prev_pg = NULL;
		    macd_ref.pg_ptr != NULL;
		    macd_ref.prev_pg = macd_ref.pg_ptr,
		    macd_ref.pg_ptr = macd_ref.pg_ptr->next) {
		    if (check_match && macd_ref.pg_ptr->match) continue;
		    (void) appGone (part_done, event_time);
		}
		macd_ref.part_ptr->last_update = event_time;
	    }
}

/******************************************************************************
*
* appAlarm() 
*
* Abstract:	This routine processes APP_ALARM data from SMD
*		It searches the application tree for account/user/
*		request-id match, if no match is found, log error;
*		if a match is found, updates resource usage in MACD 
*		database and see if it's allocation over-run.  If
*		it is, then take the appropriate action according
*		to the policies set by the system and in the database.
*
* Arguments:	sm_data_size -	size of the data
*		sm_data -	address of the data
*
* Return value:	None
*
******************************************************************************/

void appAlarm(sm_data_size, sm_data)
int sm_data_size;
struct smd_resp *sm_data;

{
    struct cpulim_info *acct_db_ptr, *user_db_ptr;
    extern struct app_ref macd_ref;
    extern struct macsconf *conf;
    extern void _sendmail();
    extern char *timstr();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr, 
	    "Enter appAlarm(sm_data_size=%d, sm_data=%d)\n",
	    sm_data_size, sm_data);
	(void) fflush (stderr);
    }
    /*
     * should not get alarm reply if running under ACCTONLY
     */

    if (conf->macdmode == ACCTONLY) {
	(void) printf("WARNING  : %s - Under ACCTONLY received SMD_APP_ALARM_IND from SMD\n",
		timstr(0));
	(void) fflush (stdout);
	(void) free (sm_data);
	return;
    }

    /*
     * check the data size and return status
     */

    if (sm_data_size != sizeof(struct smd_resp)) {
	(void) printf("WARNING  : %s - Invalid SMD_APP_ALARM_IND message-size=%d from SMD, message discarded\n",
		timstr(0), sm_data_size);
	(void) fflush (stdout);
	(void) free (sm_data);
	return;
    }

    if (_debug_) (void) dump_smd ("appAlarm", sm_data);

    if (sm_data->value.status != SMD_OK) {
	(void) printf("WARNING  : %s - Invalid status=%d in SMD_APP_ALARM_IND message from SMD, message discarded\n",
                timstr(0), sm_data->value.status);
	(void) fflush (stdout);
        (void) free (sm_data);
        return;
    }
    
    if (sm_data->q_id.acct_id <= QUALIFY_ANY) {
	(void) printf("WARNING  : %s - Invalid acctid=%d in SMD_APP_ALARM_IND message from SMD, message discarded\n",
                timstr(0), sm_data->q_id.acct_id);
	(void) fflush (stdout);
        (void) free (sm_data);
        return;
    }
    
    if ((int)sm_data->q_id.uid < QUALIFY_ANY) {
	(void) printf("WARNING  : %s - Invalid uid=%d in SMD_APP_ALARM_IND message from SMD, message discarded\n",
                timstr(0), sm_data->q_id.uid);
	(void) fflush (stdout);
        (void) free (sm_data);
        return;
    }
    
    if ((int)sm_data->q_id.uid == 0) {
	(void) printf("WARNING  : %s - Invalid uid=%d in SMD_APP_ALARM_IND message from SMD, message discarded\n",
                timstr(0), sm_data->q_id.uid);
	(void) fflush (stdout);
        (void) free (sm_data);
        return;
    }
    
    if (sm_data->q_id.part_id < QUALIFY_ANY) {
	(void) printf("WARNING  : %s - Invalid partid=%d in SMD_APP_ALARM_IND message from SMD, message discarded\n",
                timstr(0), sm_data->q_id.part_id);
	(void) fflush (stdout);
        (void) free (sm_data);
        return;
    }
    
    if (sm_data->q_id.pgid < QUALIFY_ANY) {
	(void) printf("WARNING  : %s - Invalid pgid=%d in SMD_APP_ALARM_IND message from SMD, message discarded\n",
                timstr(0), sm_data->q_id.pgid);
	(void) fflush (stdout);
        (void) free (sm_data);
        return;
    }
    
    /*
     * process SMD_APP_ALARM_IND data
     */
    if (set_ref (sm_data) != 0) {
        (void) printf("WARNING  : %s - SMD_APP_ALARM_IND acct=%d uid=%d, no match in the application list\n",
                timstr(0), sm_data->q_id.acct_id, sm_data->q_id.uid);
	(void) fflush (stdout);
        (void) free (sm_data);
        return;
    }

    if ((acct_db_ptr = db_getagid (macd_ref.acct_ptr->acct_id)) == NULL) {
	(void) printf("WARNING  : %s - Fail getting db_entry for acct=%d\n",
            timstr(0), sm_data->q_id.acct_id);
	(void) fflush (stdout);
        (void) free (sm_data);
        return;
    }

    /* alarm for account allocation overrun */
    if (macd_ref.user_ptr == NULL) {
	if (strcmp (sm_data->q_id.req_id, macd_ref.acct_ptr->req_id) != 0) {
            (void) printf("WARNING  : %s - SMD_APP_ALARM_IND acct=%d req_id=%s, no match in the application list\n",
                timstr(0), sm_data->q_id.acct_id, sm_data->q_id.req_id);
	    (void) fflush (stdout);
            (void) free (sm_data);
            return;
        }

	/* kill jobs? or just send email */
        if (!(int)acct_db_ptr->unlimit) {
            if (conf->enforce & ACCTKILL && (int)acct_db_ptr->killjobs) {
                (void) appKill(ACCT_STOP);
    	        acct_db_ptr->inhibit = TRUE;
    	    }
    	    else if (!(int)acct_db_ptr->inhibit) {
    	        (void) _sendmail (ACCTOVER, &macd_ref);
    	        acct_db_ptr->inhibit = TRUE;
    	    }
        }
    
	macd_ref.acct_ptr->req_id[0] = '\0';
        (void) free (sm_data);
        return;
    }

    /* alarm for user account allocation overrun */
    if (strcmp (sm_data->q_id.req_id, macd_ref.user_ptr->req_id) != 0) {
        (void) printf("WARNING  : %s - SMD_APP_ALARM_IND acct=%d uid=%d req_id=%s, no match in the application list\n",
            timstr(0), sm_data->q_id.acct_id, sm_data->q_id.uid,
		sm_data->q_id.req_id);
	(void) fflush (stdout);
        (void) free (sm_data);
        return;
    }
    if ((user_db_ptr = db_getuid (macd_ref.user_ptr->uid, macd_ref.acct_ptr->acct_id)) == NULL) {
        (void) printf("WARNING  : %s - Fail getting db_entry for user=%d, acct=%d\n",
            timstr(0), sm_data->q_id.uid, sm_data->q_id.acct_id);
	(void) fflush (stdout);
        (void) free (sm_data);
        return;
    }
    if (!(int)user_db_ptr->unlimit) {
        if ((conf->enforce & USERKILL && (int)acct_db_ptr->killjobs) 
		|| !(int)user_db_ptr->use) {
            (void) appKill(USER_STOP);
	    user_db_ptr->inhibit = TRUE;
	}
	else if (!(int)user_db_ptr->inhibit) {
	    (void) _sendmail (USEROVER, &macd_ref);
	    user_db_ptr->inhibit = TRUE;
	}
    }

    macd_ref.user_ptr->req_id[0] = '\0';
    (void) free (sm_data);
    return;
}


/******************************************************************************
*
* appKill ()
*
* Abstract:	This routine sends kill signal to all process groups under
* 		the indicated account or user account depend on the input
*		argument.  It assumes macd_ref are set to point the
*		corresponding account/user entrie(s) in the application tree.
*
* Arguments:	acct_kill -	1	kill all processes under the account
*				0	kill all processes under the user 
*					account
*
* Return value:	0	successful
*		-1	error
*
******************************************************************************/

appKill (acct_kill)
int acct_kill;
{
    struct app_ref local_ref;
    extern struct app_ref macd_ref;
    extern void _sendmail();
    extern int _debug_;
    extern int errno;

    if (_debug_) {
	(void) fprintf (stderr, "Enter appKill (acct_kill=%d)\n", acct_kill);
	(void) fflush (stderr);
    }

    (void) bcopy (&macd_ref, &local_ref, sizeof (struct app_ref));
    if (local_ref.acct_ptr == NULL) return (-1);

    if (acct_kill) {
	local_ref.user_ptr = local_ref.acct_ptr->user_list;
	local_ref.prev_user = NULL;
	if (local_ref.acct_ptr->req_id[0] != '\0') 
	    (void) smAlarmCancel(ACCT_ALARM, &local_ref);
    }
    else if (local_ref.user_ptr == NULL) return (-1);

    for (; local_ref.user_ptr != NULL;
	local_ref.prev_user = local_ref.user_ptr,
	local_ref.user_ptr = local_ref.user_ptr->next) {
	if (local_ref.user_ptr->req_id[0] != '\0') 
	    (void) smAlarmCancel(USER_ALARM, &local_ref);
	for (local_ref.part_ptr = local_ref.user_ptr->part_list,
	    local_ref.prev_part = NULL;
	    local_ref.part_ptr != NULL;
	    local_ref.prev_part = local_ref.part_ptr,
	    local_ref.part_ptr = local_ref.part_ptr->next)
	    for (local_ref.pg_ptr = local_ref.part_ptr->pg_list,
		local_ref.prev_pg = NULL;
		local_ref.pg_ptr != NULL;
		local_ref.prev_pg = local_ref.pg_ptr,
		local_ref.pg_ptr = local_ref.pg_ptr->next)
		if (killpg (local_ref.pg_ptr->pgid, SIGKILL) != 0) {
		    if (errno == ESRCH) 
			(void) printf("MACDINFO : %s - killpg(): pgid=%d is already gone (acct=%d uid=%d part=%d)\n",
		        timstr(0), local_ref.pg_ptr->pgid, 
			local_ref.acct_ptr->acct_id, 
			local_ref.user_ptr->uid, 
			local_ref.part_ptr->part_id);
		    else (void) printf("WARNING  : %s - Failed to terminate pgid=%d acct_id=%d uid=%d part=%d errno=%d\n",
		        timstr(0), local_ref.pg_ptr->pgid,
		        local_ref.acct_ptr->acct_id, local_ref.user_ptr->uid, 
			local_ref.part_ptr->part_id, errno);
		}
		else (void) printf("MACDINFO : %s - killpg(): Terminated pgid=%d acct_id=%d uid=%d part=%d\n",
                        timstr(0), local_ref.pg_ptr->pgid,
                        local_ref.acct_ptr->acct_id, local_ref.user_ptr->uid, 
                        local_ref.part_ptr->part_id);

	/*
	 * only kill processes under one user account
	 */
        if (!acct_kill) {
            (void) _sendmail (USER_STOP, &local_ref);
	    break;
	}

        (void) _sendmail (ACCT_STOP, &local_ref);
    }
    return (0);
}
