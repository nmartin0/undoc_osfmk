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
 * File:        job_misc.c
 *
 * Abstract:    This file contains miscellaneous routines called by various
 *              SMD in-comming-message handlers
 *****************************************************************************/
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include "appacct.h"
#include "mac.h"
#include "conf.h"
#include "filename.h"


/*
 * set_job ()
 * Abstract:    This routine set pointers in macd_ref according to
 *              the data received from NQS, thus they will point to the
 *              relevant entries in the application tree.  It starts
 *              from the top of the tree, match account id, user id and
 *              partition id, downward the tree link one level
 *              at a time and stop when no-match, then the pointer and
 *              the pointers in the level below will be set to NULL.
 *
 * Arguments:   nqs_data -      in-coming data from NQS
 *
 * Return value:        0 -     successful
 *                      -1 -    error
 */

int set_job (nqs_data)
struct nqs_job_info *nqs_data;
{

    int i;
    extern struct app_ref macd_ref;
    extern struct acct_ent *top_link;
    extern char *timstr();
    extern void bzero();
    extern int _debug_;
    extern void dump_ref();

    if (_debug_) {
	(void) fprintf (stderr, "Enter set_job (nqs_data=%d)\n", nqs_data);
        (void) fflush (stderr);
    }

    if (nqs_data == NULL) {
        (void) printf("WARNING  : %s - NULL reference pointer in set_job() for nqs_data\n",
            timstr(0));
        (void) fclose (stdout);
        exit(1);
    }
    (void) bzero ((char *)&macd_ref, sizeof (struct app_ref));
    if (top_link == NULL) return (-1);

    macd_ref.acct_ptr = top_link;

    if (macd_ref.acct_ptr->acct_id != nqs_data->acct_id) {
	if (_debug_) i=0;
	for (macd_ref.acct_ptr = top_link, macd_ref.prev_acct = NULL;
	    macd_ref.acct_ptr != NULL; 
	    macd_ref.prev_acct = macd_ref.acct_ptr,
	    macd_ref.acct_ptr = macd_ref.acct_ptr->next) {
	    if (macd_ref.acct_ptr->acct_id == nqs_data->acct_id) {
		macd_ref.user_ptr = macd_ref.acct_ptr->user_list;
		macd_ref.prev_user = NULL;
		break;
	    }
	    if (_debug_ && i++>100) {
		(void) dump_ref ("find_part acct loop");
		return (-1);
	    }
	}
	if (macd_ref.acct_ptr == NULL) {
	    (void) bzero ((char *)&macd_ref, sizeof (struct app_ref));
	    return (-1);
	}
    }

    if (macd_ref.user_ptr == NULL) 
	macd_ref.user_ptr = macd_ref.acct_ptr->user_list;

    if (macd_ref.user_ptr == NULL) {
	macd_ref.prev_user = NULL;
	macd_ref.part_ptr = NULL;
	macd_ref.prev_part = NULL;
	macd_ref.pg_ptr = NULL;
	macd_ref.prev_pg = NULL;
        return (-1);
    }

    if (macd_ref.user_ptr->uid != nqs_data->uid) {
	if (_debug_) i=0;
	for (macd_ref.user_ptr = macd_ref.acct_ptr->user_list,
	    macd_ref.prev_user = NULL;
	    macd_ref.user_ptr != NULL;
	    macd_ref.prev_user = macd_ref.user_ptr,
	    macd_ref.user_ptr = macd_ref.user_ptr->next) {
	    if (macd_ref.user_ptr->uid == nqs_data->uid) {
		macd_ref.part_ptr = macd_ref.user_ptr->part_list;
                macd_ref.prev_part = NULL;
		break;
	    }
	    if (_debug_ && i++>100) {
		(void) dump_ref ("find_part user loop");
		return (-1);
	    }
	}
	if (macd_ref.user_ptr == NULL) {
	    macd_ref.prev_user = NULL;
	    macd_ref.part_ptr = NULL;
	    macd_ref.prev_part = NULL;
	    macd_ref.pg_ptr = NULL;
	    macd_ref.prev_pg = NULL;
            return (-1);
	}
    }

    if (macd_ref.part_ptr == NULL)
	macd_ref.part_ptr = macd_ref.user_ptr->part_list;

    if (macd_ref.part_ptr == NULL) {
	macd_ref.prev_part = NULL;
	macd_ref.pg_ptr = NULL;
	macd_ref.prev_pg = NULL;
	return (-1);
    }

    if (macd_ref.part_ptr->part_id != nqs_data->part_id) {
	if (_debug_) i=0;
	for (macd_ref.part_ptr = macd_ref.user_ptr->part_list,
	    macd_ref.prev_part = NULL;
	    macd_ref.part_ptr != NULL;
	    macd_ref.prev_part = macd_ref.part_ptr,
	    macd_ref.part_ptr = macd_ref.part_ptr->next) {
	    if (macd_ref.part_ptr->part_id == nqs_data->part_id) {
                macd_ref.pg_ptr = macd_ref.part_ptr->pg_list;
                macd_ref.prev_pg = NULL;
		break;
	    }
	    if (_debug_ && i++>100) {
		(void) dump_ref ("find_part part loop");
		return (-1);
	    }
	}
	if (macd_ref.part_ptr == NULL) {
	    macd_ref.prev_part = NULL;
	    macd_ref.pg_ptr = NULL;
	    macd_ref.prev_pg = NULL;
	    return (-1);
	}
    }

    return (0);
}


/*
 * logjobs (switch_log, filename)
 *
 * Abstract:    This routine makes log entries for all existing jobs
 *		(partitions) in the application tree
 *
 * Arguments:   switch_log - 0 or 1 for switching log file operation
 *		filename - when switch_log=1 log into this file
 *
 * Return value:        None
 */

int logjobs (switch_log, filename)
int switch_log;
char *filename;
{
    FILE *temp_fd=NULL;
    extern struct app_ref macd_ref;
    extern struct acct_ent *top_link;
    extern struct macsconf *conf;
    extern char *timstr();
    extern char *strcpy();
    extern void bzero();
    extern int errno;
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr, "Enter logjobs ()\n");
        (void) fflush (stderr);
    }

    /*
     * nothing to log, delete the previous logs if any
     */
    if (top_link == NULL) {
	if (switch_log) return(0);
	(void) unlink (MACD_LASTLOG_FNAME);
	(void) unlink (MACD_TEMPLOG_FNAME);
	return(0);
    }

    /*
     * prepare status log file
     */ 
    if (!switch_log) (void) unlink (MACD_TEMPLOG_FNAME);
    if (switch_log && (temp_fd = fopen (filename, "a")) == NULL) {
	(void) printf("WARNING  : %s - Fail opening %s file, errno=%d\n",
	    timstr(0), filename, errno);
    }
    else if (!switch_log && (temp_fd = fopen (MACD_TEMPLOG_FNAME, "a")) == NULL) {
        (void) printf("WARNING  : %s - Fail opening %s file, errno=%d\n",
            timstr(0), MACD_TEMPLOG_FNAME, errno);
    }
    else
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
		    if (macd_ref.part_ptr->queue_name == NULL || 
			strlen(macd_ref.part_ptr->queue_name) <= 0)
			(void) strcpy (macd_ref.part_ptr->queue_name, "UNKNOWN");
		    if (strcmp (macd_ref.part_ptr->queue_name, "UNKNOWN") == 0)
			for (macd_ref.pg_ptr  = macd_ref.part_ptr->pg_list,
			    macd_ref.prev_pg = NULL;
			    macd_ref.pg_ptr != NULL;
			    macd_ref.prev_pg = macd_ref.pg_ptr,
			    macd_ref.pg_ptr = macd_ref.pg_ptr->next) {
                            if ((time_t) macd_ref.pg_ptr->last_update > time(0)) {
			        (void) fprintf(temp_fd,
	                            "WARNING  : %s - Following OLDJOB has invalid date (%d)\n", 
				    timstr(0), macd_ref.pg_ptr->last_update);
                            }
			    (void) fprintf(temp_fd,
				"OLDJOB   : %s\n", timstr(macd_ref.pg_ptr->last_update));
			    (void) fprintf(temp_fd,
				"           Acct=%d User=%d Part=%d Size=%d Type=0 Queue=%s Pgid=%d\n",
				macd_ref.acct_ptr->acct_id,
				macd_ref.user_ptr->uid,
				macd_ref.part_ptr->part_id,
				macd_ref.pg_ptr->app_size,
				"UNKNOWN", 
				macd_ref.pg_ptr->pgid);
			    (void) fprintf(temp_fd,
				"           Time=%d Rate=%f Under=%d Rate=%f Idle=%d Rate=%f\n",
				macd_ref.pg_ptr->cpu_time,
				conf->def_qrate,
				0,
				conf->uunt_rate,
				0,
			 	conf->idle_rate);
			}
		    else {
			(void) fprintf(temp_fd,
			    "OLDJOB   : %s\n", timstr(macd_ref.part_ptr->last_update));
			(void) fprintf(temp_fd,
			    "           Acct=%d User=%d Part=%d Size=%d Type=0 Queue=%s Pgid=%d\n",
			    macd_ref.acct_ptr->acct_id,
			    macd_ref.user_ptr->uid,
			    macd_ref.part_ptr->part_id,
			    macd_ref.part_ptr->part_size,
			    macd_ref.part_ptr->queue_name, 0);
			(void) fprintf(temp_fd,
			    "           Time=%d Rate=%f Under=%d Rate=%f Idle=%d Rate=%f\n",
			    macd_ref.part_ptr->cpu_time,
			    macd_ref.part_ptr->charge_rate,
			    macd_ref.part_ptr->under_used,
			    conf->uunt_rate,
			    macd_ref.part_ptr->idle_time * macd_ref.part_ptr->part_size,
			    conf->idle_rate);
		    }
		    (void) fflush(temp_fd);
		}

    (void) bzero ((char *)&macd_ref, sizeof (struct app_ref));

    if (temp_fd != NULL) {
        (void) fclose (temp_fd);
        if (!switch_log) {
	    if (rename (MACD_TEMPLOG_FNAME, MACD_LASTLOG_FNAME) != 0) {
        	(void) printf("WARNING  : %s - Fail renaming %s to %s\n", 
		    timstr(0), MACD_TEMPLOG_FNAME, MACD_LASTLOG_FNAME);
            }
            else if (_debug_) {
                (void) fprintf (stderr, "rename (%s, %s)\n", 
		    MACD_TEMPLOG_FNAME, MACD_LASTLOG_FNAME);
                (void) fflush (stderr);
            }
        }
    }
    else if (_debug_) {
        if (switch_log) (void) fprintf (stderr, "temp_fd %s is NULL\n", filename);
        else (void) fprintf (stderr, "temp_fd %s is NULL\n", MACD_TEMPLOG_FNAME);
        (void) fflush (stderr);
    }

    return(0);
}


/*
 * find_part ()
 *
 * Abstract:    This routine searches the whole application tree for
 *              a matching partition id.  If a match is found, the 
 *              macd_ref is set to point to all relevant entries for
 *              the application; if no match, macd_ref is set to NULLs.
 *
 * Arguments:   nqs_data - pointer to job entry from NQS 
 *
 * Return value:        0 -     found
 *                      -1 -    not found
 */

int find_part (nqs_data)
struct nqs_job_info *nqs_data;
{

    extern struct app_ref macd_ref;
    extern struct acct_ent *top_link;
    extern void bzero();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr, "Enter find_part (part_id=%d)\n", nqs_data->part_id);
        (void) fflush (stderr);
    }

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
		macd_ref.part_ptr = macd_ref.part_ptr->next)
		    if (macd_ref.part_ptr->part_id == nqs_data->part_id) {
			if (macd_ref.user_ptr->uid == nqs_data->uid) return (0);
		    }
    (void) bzero ((char *)&macd_ref, sizeof (struct app_ref));
    return (-1);
}
