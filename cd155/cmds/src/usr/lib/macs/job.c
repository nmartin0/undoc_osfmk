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
 * File:        job.c
 *
 * Abstract:	This file contains NQS in-coming-message handler routines
 *****************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <time.h>
#include "conf.h"
#include "mac.h"
#include "appacct.h"
#include "filename.h"

/******************************************************************************
*
* jobStart() 
*
* Abstract:	This routine called by macd to process NQS_JOB_START 
*		data from NQS shepherd daemon.  It will first try to
*		find a matching entry in the application tree, if a
*		match is found, log a warning message, since it could
*		happen if MACD receives application-start message from
*		SMD before receives job-start message from NQS.  Then
*		a partition entry will be created or updated in the
*		application tree.
*
* Arguments:	nqs_data_size -	data size
*		nqs_data -	partition information for a NQS job
*
* Return value: None
*
******************************************************************************/

void jobStart(nqs_data_size, nqs_data)
int nqs_data_size;
char *nqs_data;
{
    struct nqs_job_info *job_start_ptr;
    extern struct app_ref macd_ref;
    extern void partGone();
    extern char *calloc(), *timstr();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr,
            "Enter jobStart(nqs_data_size=%d, nqs_data=%d)\n",
            nqs_data_size, nqs_data);
        (void) fflush (stderr);
    }

    /*
     * check the data size
     * should be struct nqs_job_info
     */

    if (nqs_data_size != sizeof(struct nqs_job_info)) {
	(void) printf("WARNING  : %s - Invalid JOB_START message-size=%d from NQS, message discarded\n",
		timstr(0), nqs_data_size);
        (void) fflush (stdout);
	(void) free (nqs_data);
	return;
    }

    /*
     * process JOB_START data
     */

    job_start_ptr = (struct nqs_job_info *)nqs_data;
if (_debug_) (void) dump_job ("jobStart", job_start_ptr);

    /*
     * Change time stamp to MACD's local time.  Temp. solution for
     * time difference amoung different cpu's
     * Clear out the cpu/under/idle time fields
     */
    job_start_ptr->event_time = time(0);
    job_start_ptr->cpu_time = 0;
    job_start_ptr->part_idle = 0;
    job_start_ptr->part_active = 0;

    /*
     * If the partition exist and the usage is non-zero, 
     * must be an old one, should be closed
     */
    if (find_part (job_start_ptr) == 0) { 
	if (macd_ref.part_ptr->cpu_time > 0 || macd_ref.part_ptr->idle_time > 0
	    || macd_ref.user_ptr->uid != job_start_ptr->uid) {
	    struct nqs_job_info *old_part = 
		(struct nqs_job_info *) calloc (1, sizeof (struct nqs_job_info));
	    old_part->acct_id = macd_ref.acct_ptr->acct_id;
	    old_part->uid = macd_ref.user_ptr->uid;
	    old_part->part_id = job_start_ptr->part_id;
	    old_part->cpu_time = macd_ref.part_ptr->cpu_time;
	    old_part->part_idle = macd_ref.part_ptr->idle_time;
	    old_part->part_size = macd_ref.part_ptr->part_size;
	    old_part->event_time = job_start_ptr->event_time;
	    old_part->submit_time = job_start_ptr->submit_time;
	    old_part->requested_time = job_start_ptr->requested_time;
	    (void) bcopy (macd_ref.part_ptr->queue_name, old_part->queue_name,  16);
            (void) printf("WARNING  : %s - JOB_START part_id=%d already exist, close out\n",
                 timstr(0), job_start_ptr->part_id);
            (void) fflush (stdout);
	    (void) partGone(old_part);
	    (void) free (old_part);
	}
    } 
    else if (set_job (job_start_ptr) == 0) {
        (void) printf("WARNING  : %s - JOB_START part_id=%d, still exists!\n",
                 timstr(0), job_start_ptr->part_id);
        (void) fflush (stdout);
    }
if (_debug_) (void) dump_ref ("jobStart after set_job()");

    if (jobNew (job_start_ptr) != 0) {
         (void) printf("WARNING  : %s - Fail adding JOB_START part_id=%d in application list\n",
                 timstr(0), job_start_ptr->part_id);
        (void) fflush (stdout);
    }
if (_debug_) (void) dump_ref ("jobStart just out jobNew ()");

}


/*
 * jobNew (job_info_ptr)
 *
 * Abstract:	This routine adds an entry for a new NQS job (part_ent)
 *		to the application tree.  It assumes the pointers in
 *		macd_ref are set to point to all relevant entries of
 *		the nqs job in the tree.  If the partition entry already
 *		exists, it will log a warning message, but updates the
 *		entry instead of add.  This can happen if MACD receives
 *		application-start message from SMD before receives job-start
 *		message from NQS.
 *
 * Arguments:	job_info_ptr -	nqs job information
 *
 * Return value: 0 -	successful
 *		-1 -	error
 */

int jobNew (job_info_ptr)
struct nqs_job_info *job_info_ptr;
{
    int i;
    extern struct app_ref macd_ref;
    extern struct acct_ent *top_link;
    extern int n_queue;
    extern struct queue *qcharge;
    extern struct macsconf *conf;
    extern char *calloc();
    extern char *strcpy();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr, "Enter jobNew(job_info_ptr=%d)\n",job_info_ptr);
        (void) fflush (stderr);
    }

    if (macd_ref.acct_ptr == NULL) {
	macd_ref.acct_ptr = (struct acct_ent *) calloc (1, sizeof (struct acct_ent));
	if (macd_ref.acct_ptr == NULL) {
            (void) printf("WARNING  : %s - Memory allocation failure in jobNew() for acct_ent\n",
                timstr(0));
            (void) fclose (stdout);
            exit(1);
        }
	macd_ref.acct_ptr->acct_id = job_info_ptr->acct_id;
	if (macd_ref.prev_acct != NULL && macd_ref.prev_acct->next == NULL)
	    macd_ref.prev_acct->next = macd_ref.acct_ptr;
	else {
	    if (top_link != NULL) macd_ref.acct_ptr->next = top_link;
	    top_link = macd_ref.acct_ptr;
	    macd_ref.prev_acct = NULL;
	}
    }
	
    if (macd_ref.user_ptr == NULL) {
	macd_ref.user_ptr = (struct user_ent *) calloc (1, sizeof (struct user_ent));
	if (macd_ref.user_ptr == NULL) {
            (void) printf("WARNING  : %s - Memory allocation failure in jobNew() for user_ent\n",
                timstr(0));
            (void) fclose (stdout);
            exit(1);
	}
	macd_ref.user_ptr->uid = job_info_ptr->uid;
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
	
    if (macd_ref.part_ptr == NULL) {
	macd_ref.part_ptr = (struct part_ent *) calloc (1, sizeof (struct part_ent));
	if (macd_ref.part_ptr == NULL) {
            (void) printf("WARNING  : %s - Memory allocation failure in jobNew() for part_ent\n",
                timstr(0));
            (void) fclose (stdout);
            exit(1);
	}

	macd_ref.part_ptr->part_id = job_info_ptr->part_id;
        macd_ref.part_ptr->start_time = macd_ref.part_ptr->last_update 
		= job_info_ptr->event_time;
        macd_ref.user_ptr->npart++;

	if (macd_ref.prev_part != NULL && macd_ref.prev_part->next == NULL)
	    macd_ref.prev_part->next = macd_ref.part_ptr;
	else {
	    if (macd_ref.user_ptr->part_list != NULL) 
		macd_ref.part_ptr->next = macd_ref.user_ptr->part_list;
	    macd_ref.user_ptr->part_list = macd_ref.part_ptr;
	    macd_ref.prev_part = NULL;
	}

    }

    /* 
     * allow update partition information, in case NQS send JOB_START
     * after SMD send APP_START
     */
    if (macd_ref.part_ptr->part_id == job_info_ptr->part_id &&
	macd_ref.acct_ptr->acct_id == job_info_ptr->acct_id &&
	macd_ref.user_ptr->uid == job_info_ptr->uid) {
        macd_ref.part_ptr->part_size = job_info_ptr->part_size;
	if (macd_ref.part_ptr->start_time < job_info_ptr->event_time) {
	    macd_ref.part_ptr->idle_time += job_info_ptr->event_time -
		macd_ref.part_ptr->start_time;
	    if (macd_ref.part_ptr->idle_time > job_info_ptr->event_time - 
		macd_ref.part_ptr->start_time)
        	(void) printf("WARNING  : %s - APP_START bad idle time %d, sm_data->value.event_time %d, macd_ref.part_ptr->last_update %d\n",
                 timstr(0), macd_ref.part_ptr->idle_time, 
		job_info_ptr->event_time, macd_ref.part_ptr->start_time);
	}
        macd_ref.part_ptr->start_time = job_info_ptr->event_time;
	if (macd_ref.part_ptr->last_update < job_info_ptr->event_time)
	    macd_ref.part_ptr->last_update = job_info_ptr->event_time;
	(void) strncpy (macd_ref.part_ptr->queue_name,
		job_info_ptr->queue_name, 16);
	macd_ref.part_ptr->queue_name[15] = '\0';
	for (i=0; i<n_queue; i++)
	    if (strcmp (job_info_ptr->queue_name, qcharge[i].name) == 0) {
		macd_ref.part_ptr->charge_rate =  qcharge[i].rate;
		break;
	    }
	if (i>=n_queue) {
	    macd_ref.part_ptr->charge_rate = conf->def_qrate;
	}
    }
    else return (-1);

    /*
     * make log entry for job start
     */
    if (job_info_ptr->queue_name == NULL || 
	strlen(job_info_ptr->queue_name) <= 0)
	(void) strcpy (job_info_ptr->queue_name, "UNKNOWN");
    (void) printf("STARTJOB : %s\n", timstr(job_info_ptr->event_time));
    (void) printf("           Acct=%d User=%d Part=%d Size=%d Type=%d Queue=%s\n",
    	job_info_ptr->acct_id,
    	job_info_ptr->uid, 
    	job_info_ptr->part_id,
    	job_info_ptr->part_size,
/*
    	job_info_ptr->node_type,
*/
	0,
	job_info_ptr->queue_name); 
#ifndef INTELv1r1
    (void) printf("           Subm=%d Reqst=%d\n",
	job_info_ptr->submit_time, job_info_ptr->requested_time);
#endif
    (void) fflush(stdout);
    return (0);
}

/******************************************************************************
*
* jobEnd() 
*
* Abstract:	This routine called by macd to process NQS_JOB_END 
*		data from NQS shepherd daemon.  It first tries to
*		find the partition entry (which matches on all three
*		levels - account id, user id, partition id) for the 
*		NQS job in the application tree.  If the three-level-
*		match does not exist, it will then search the whole
*		application tree and try to find a match of partition
*		id only.  The reason for doing the later is that the
*		NQS daemon may not have information about account id
*		and user id under some circumstances when sending a
*		JOB_END messages.  But if both search failed, then
*		it will log a warning message, add a partition entry
*		for the job in the application tree.  This routine
*		updates CPU node-time, under-used and idle-time for
*		the account/user in MACD database, log the JOB_END
*		event and remove the partition entry from the tree.
*		
*
* Arguments:	nqs_data_size -	data size
*		nqs_data -	partition information for a NQS job
*
* Return value: None
*
******************************************************************************/

#ifndef INTELv1r1
struct job_charge *jobEnd(nqs_data_size, nqs_data)
#else
void jobEnd(nqs_data_size, nqs_data)
#endif
int nqs_data_size;
struct nqs_job_info *nqs_data;
{
    int rval;
    struct job_charge *charge_ptr;
    extern struct app_ref macd_ref;
    extern struct macsconf *conf;
    extern void partGone();
    extern char *timstr();
    extern char *malloc();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr, "Enter jobEnd(nqs_data_size=%d, nqs_data=%d)\n",
	    nqs_data_size, nqs_data);
        (void) fflush (stderr);
    }

    /*
     * check the data size
     * should be struct job_info and followed by 0 or more struct job_info
     */

    if (nqs_data_size != sizeof(struct nqs_job_info)) {
	(void) printf("WARNING  : %s - Invalid JOB_END message-size=%d from NQS, message discarded\n",
		timstr(0), nqs_data_size);
	(void) fflush (stdout);
	(void) free (nqs_data);
#ifndef INTELv1r1
	return (NULL);
#else
	return;
#endif
    }
if (_debug_) (void) dump_job ("jobEnd", nqs_data);

    /*
     * Change time stamp to MACD's local time.  Temp. solution for
     * time difference amoung different cpu's
     * Clear the cpu/under/idle fields
     */
    nqs_data->event_time = time(0);
    nqs_data->cpu_time = 0;
    nqs_data->part_idle = 0;
    nqs_data->part_active = 0;

    /*
     * process JOB_END data
     */

    if (find_part (nqs_data) != 0) {
        (void) printf("WARNING  : %s - JOB_END part_id=%d uid=%d, no match in the application list\n",
                 timstr(0), nqs_data->part_id, nqs_data->uid);
	(void) fflush (stdout);
    }
    else {
	long /* rollin=0, under_used=0, */ idle_time=0;

if (_debug_) (void) dump_ref ("jobEnd after set_job()");
        (void) printf("MACDINFO : %s - JOB_END acct_id=%d\n",timstr(0), nqs_data->acct_id);
	nqs_data->acct_id = macd_ref.acct_ptr->acct_id;
	if (!macd_ref.part_ptr->npg) {
	    if (nqs_data->event_time > macd_ref.part_ptr->last_update) {
	        idle_time = nqs_data->event_time - macd_ref.part_ptr->last_update;
              if (idle_time > nqs_data->event_time -
                macd_ref.part_ptr->start_time)
                (void) printf("WARNING  : %s - JOBNEW bad idle time %d, nqs_data->event_time %d, macd_ref.part_ptr->last_update %d\n",
                 timstr(0), idle_time,
                nqs_data->event_time, macd_ref.part_ptr->start_time);
	    }
	}
	else {
            (void) printf(
                "WARNING  : %s - JOBDONE message received from NQS on part=%d before all ENDAPP messages received from SMD\n",
                 timstr(0), macd_ref.part_ptr->part_id);
	    (void) fflush (stdout);
	    if (macd_ref.part_ptr->pg_list)
		for (macd_ref.pg_ptr = macd_ref.part_ptr->pg_list, 
		macd_ref.prev_pg = NULL;
		macd_ref.pg_ptr != NULL; 
		macd_ref.pg_ptr = macd_ref.pg_ptr->next)
		(void) appGone (0, 0);
	}
/*
	if ((rollin = nqs_data->cpu_time - macd_ref.part_ptr->cpu_time) < 0) {;
            if (_debug_) (void) printf(
		"WARNING  : %s - Invalid cputime=%d received from NQS, prev_accum=%d, rollin set to 0\n",
                 timstr(0), nqs_data->cpu_time, macd_ref.part_ptr->cpu_time);
	    rollin = 0;
        }
*/
	nqs_data->cpu_time = macd_ref.part_ptr->cpu_time;
	nqs_data->part_idle = macd_ref.part_ptr->idle_time + idle_time;
        if (nqs_data->part_idle > nqs_data->event_time -
                macd_ref.part_ptr->start_time)
                (void) printf("WARNING  : %s - JOBEND bad part_idle %d, idle_time %d, macd_ref.part_ptr->idle_time %d\n",
                 timstr(0), nqs_data->part_idle,
                idle_time, macd_ref.part_ptr->idle_time);
#ifndef INTELv1r1
	charge_ptr = (struct job_charge *)malloc (sizeof (struct job_charge));
	if (charge_ptr > NULL) {
	    charge_ptr->cpu_time = macd_ref.part_ptr->cpu_time;
	    charge_ptr->cpu_rate = macd_ref.part_ptr->charge_rate;
	    charge_ptr->under_used = macd_ref.part_ptr->under_used;
	    charge_ptr->under_rate = conf->uunt_rate;
	    charge_ptr->idle_time = 
		macd_ref.part_ptr->idle_time * macd_ref.part_ptr->part_size;
	    charge_ptr->idle_rate = conf->idle_rate;
	    if (_debug_) {
		(void) fprintf (stderr, "Job_charge send to NQS:\n");
		(void) fprintf (stderr, 
		    "cpu=%d, rate=%f, under=%d, rate=%f, idle=%d, rate=%f\n",
			charge_ptr->cpu_time, charge_ptr->cpu_rate,
			charge_ptr->under_used, charge_ptr->under_rate,
			charge_ptr->idle_time, charge_ptr->idle_rate);
		(void) fflush (stderr);
	    }
	}
	else {
            (void) printf("WARNING  : %s - Memory allocation failure in jobEnd() for job_charge\n",
                timstr(0));
            (void) fclose (stdout);
            exit(1);
	}
#endif
/*
	under_used = nqs_data->part_active - nqs_data->cpu_time
			- macd_ref.part_ptr->under_used;
	idle_time = nqs_data->part_idle - macd_ref.part_ptr->idle_time;
*/
	if (conf->macdmode != ACCTONLY /* && (rollin || under_used || idle_time) */
		&& (rval = upd_db (0, 0, idle_time)) < 0) {
	    (void) printf("WARNING  : %s - %s, usage-update failed\n",
	        timstr(0), rval==-1 ? "NULL ref. pointer to application entry" :
	        rval==-2 ? "Fail saving defered usage data" : 
		"Invalid account-id or user-id");
	    (void) printf("           Acct=%d User=%d Part=%d Size=%d Type=%d\n",
	            nqs_data->acct_id,
	            nqs_data->uid,
	            nqs_data->part_id,
	            nqs_data->part_size,
/*
	            nqs_data->node_type);
*/
		    0);
	    (void) fflush(stdout);
	}
    }
if (_debug_) (void) dump_ref ("jobEnd after upd_db()");

    /* log end of job, release part_ent */
    (void) partGone(nqs_data);

    (void) free (nqs_data);
#ifndef INTELv1r1
    return (charge_ptr);
#else
    return;
#endif

}


/*
 * partGone ()
 *
 * Abstract:	This routine log JOBDONE event, delete the part_ent.
 * 		It will release empty account or user links 
 *
 * Arguments:	nqs_data -	job information from NQS
 *
 * Return value: None
 */

void partGone (nqs_data)
struct nqs_job_info *nqs_data;
{
    int i;
    float qrate;
    FILE *lastlog;
    extern int n_queue;
    extern struct queue *qcharge;
    extern struct macsconf *conf;
    extern struct acct_ent *top_link;
    extern struct app_ref macd_ref;
    extern char *strcpy();
    extern int errno;
    extern int _debug_;

    if (macd_ref.part_ptr == NULL) {
        for (i=0; i<n_queue; i++)
            if (strcmp (nqs_data->queue_name, qcharge[i].name) == 0) {
                qrate =  qcharge[i].rate;
                break;
            }
        if (i>=n_queue) {
            qrate = conf->def_qrate;
	}
    }
    else qrate = macd_ref.part_ptr->charge_rate;

    if (macd_ref.acct_ptr != NULL) 
	nqs_data->acct_id = macd_ref.acct_ptr->acct_id;

    /*
     * make log entry for job end
     */
    if (nqs_data->queue_name == NULL || strlen(nqs_data->queue_name) <= 0)
	(void) strcpy (nqs_data->queue_name, "UNKNOWN");
    (void) printf("JOBDONE  : %s\n", timstr(nqs_data->event_time));
    (void) printf("           Acct=%d User=%d Part=%d Size=%d Type=%d Queue=%s\n",
    	nqs_data->acct_id,
        nqs_data->uid, 
       	nqs_data->part_id,
       	nqs_data->part_size, 
/*
       	nqs_data->node_type,
*/
	0,
	nqs_data->queue_name);
	(void) fflush (stdout);
    if ((nqs_data->part_idle * nqs_data->part_size) < 0) nqs_data->part_idle = 0;
    if (nqs_data->cpu_time < 0) nqs_data->cpu_time = 0;
#ifndef INTELv1r1
    if (nqs_data->submit_time < 0) nqs_data->submit_time = 0;
    if (nqs_data->requested_time < 0) nqs_data->requested_time = 0;
#endif
    (void) printf("           Time=%d Rate=%f Under=%d Rate=%f Idle=%d Rate=%f\n",
       	nqs_data->cpu_time,
	qrate,
	0,
/*
	nqs_data->part_active - nqs_data->cpu_time,
*/
	conf->uunt_rate,
	nqs_data->part_idle * nqs_data->part_size,
	conf->idle_rate); 
#ifndef INTELv1r1
    (void) printf("           Subm=%d Reqst=%d\n",
	nqs_data->submit_time, nqs_data->requested_time);
#endif
    (void) fflush(stdout);
        
    if (macd_ref.part_ptr != NULL) {
        /*
         * free the part_ent
         */
        if (macd_ref.prev_part == NULL)
    	    macd_ref.user_ptr->part_list = macd_ref.part_ptr->next;
        else macd_ref.prev_part->next = macd_ref.part_ptr->next;
        (void) free (macd_ref.part_ptr);
	macd_ref.part_ptr = NULL;
        if (macd_ref.user_ptr->npart > 0) macd_ref.user_ptr->npart--;
    }
if (_debug_) (void) dump_ref ("partGone after release partition entry");
    
    if (macd_ref.user_ptr != NULL) {
	if (macd_ref.user_ptr->npart <= 0) {
	    /*
	     * remove an user entry if there is no more
	     * partitions under it
	     */
	    if (macd_ref.prev_user == NULL)
		macd_ref.acct_ptr->user_list = macd_ref.user_ptr->next;
	    else macd_ref.prev_user->next = macd_ref.user_ptr->next;
	    (void) free (macd_ref.user_ptr);
	    macd_ref.user_ptr = NULL;
	    if (macd_ref.acct_ptr->nuser > 0) macd_ref.acct_ptr->nuser--;
	}
	else return;
    }
if (_debug_) (void) dump_ref ("partGone after release user entry");

    if (macd_ref.acct_ptr == NULL) return;
    if (macd_ref.acct_ptr->nuser > 0) return;

    /*
     * remove an account entry if there is no more
     * user under it
     */
    if (macd_ref.prev_acct == NULL)
	top_link = macd_ref.acct_ptr->next;
    else macd_ref.prev_acct->next = macd_ref.acct_ptr->next;
    (void) free (macd_ref.acct_ptr);
    macd_ref.acct_ptr = NULL;
if (_debug_) (void) dump_ref ("partGone after release account entry");
    return;
}
