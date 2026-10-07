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
 * File:        app_misc.c
 *
 * Abstract:	This file contains miscellaneous routines called by various
 *		SMD in-comming-message handlers
 *****************************************************************************/
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "conf.h"
#include "db.h"
#include "macd.h"
#include "appacct.h"
#include "smd.h"


/*
 * upd_db ()
 *
 * Abstract:	This routine updates CPU node-time usages for an
 *		user account then checks the remaining usable time.  
 *		If the remaining usable time drop below 
 *	(last_sync_time + conf->sync_interval - current_time) * nodes
 * 		send APP_ALARM request to SMD for notification when
 *		the account run-out the remaining CPU node-time.
 * 		If the allocation run out, an e-mail will be sent 
 *		to both the administrator and the user.  MACS will 
 *		send termination signals to all process groups under 
 *		this user account only if KILL is defined in the 
 *		configuration file or the kill-bit is set in the account
 *		entry in MACD database.  It assumes macd_ref is set to 
 *		points in the application tree to the user and account
 *		entries of which the CPU node-time are to be updated.
 *
 * Arguments:	rollin - incremental cpu node-time used
 *		under_used -	CPU node-time of the idle nodes in a
 *				partition where there is at least one
 *				node active.
 *		idle_time -	wall-clock node-time of the idle nodes
 *				when the whole partition is idle
 *
 * Return value:	0 -	successful
 *			-1 -	invalid macd_ref pointers
 *			-3 -	fail in database access
 */

int upd_db (rollin, under_used, idle_time) 
long 	rollin;
long	under_used;
long	idle_time;
{
    struct cpulim_info *acct_db_ptr, *user_db_ptr;
    double charge; 
    long wctime;
    extern struct app_ref macd_ref;
    extern struct acct_ent *top_link;
    extern struct macsconf *conf;
    extern time_t tlastsync;
    extern void _sendmail();
    extern int _debug_;
    extern int upd_hold, upd_hold_list();

    if (_debug_) {
	(void) fprintf (stderr,
            "Enter upd_db (rollin=%d, under_used=%d, idle_time=%d)\n",
	    rollin, under_used, idle_time);
	(void) fflush (stderr);
    }

    if (rollin < 0) rollin = 0;
    if (under_used < 0) under_used = 0;
    if (idle_time < 0) idle_time = 0;

    if (macd_ref.acct_ptr == NULL || macd_ref.user_ptr == NULL ||
	macd_ref.part_ptr == NULL)
	return (-1);
    if ((acct_db_ptr = db_getagid (macd_ref.acct_ptr->acct_id)) == NULL ||
        (user_db_ptr = db_getuid (macd_ref.user_ptr->uid,
        macd_ref.acct_ptr->acct_id)) == NULL) return (-3);

    if (rollin || idle_time) {
      if (rollin) charge = (double)rollin * macd_ref.part_ptr->charge_rate;
      else if (idle_time) charge = (double) (idle_time * 
	macd_ref.part_ptr->part_size) * conf->idle_rate;
      else charge = 0.0;

      /* safe guard against over-charge */
      if (charge > (double) (conf->sync_interval * 60 * 
	macd_ref.part_ptr->part_size)) {
	double max = (double) (conf->sync_interval * 60 * 
		macd_ref.part_ptr->part_size);
	if (rollin) (void) printf(
	    "WARNING  : %s - UPD_DB error, charge=%f rollin=%d rate=%f max=%f\n",
		timstr(0), charge, rollin, macd_ref.part_ptr->charge_rate, max);
	else (void) printf(
	    "WARNING  : %s - UPD_DB error, charge=%f idle=%d rate=%f max=%f\n",
		timstr(0), charge, idle_time, conf->idle_rate, max);
	/* charge = max; */
	(void) fflush (stdout);
      }

      if (upd_hold) {
	if (upd_hold_list (macd_ref.acct_ptr->acct_id,
	    macd_ref.user_ptr->uid, rollin, charge) != 0) return (-2);
	return (0);
      }

      if (_debug_) {
	(void) fprintf (stderr, "old_acct_sbu=%f, old_user_sbu=%f\n", 
		acct_db_ptr->sbu_time, user_db_ptr->sbu_time);
	(void) fflush (stderr);
      }
      acct_db_ptr->used_time += (double)rollin;
      acct_db_ptr->sbu_time += charge;
      user_db_ptr->used_time += (double)rollin;
      user_db_ptr->sbu_time += charge;
      if (_debug_) {
	(void) fprintf (stderr, "new_acct_sbu=%f, new_user_sbu=%f\n", 
		acct_db_ptr->sbu_time, user_db_ptr->sbu_time);
	(void) fflush (stderr);
      }
    }

#ifdef SDSC
    if (macd_ref.user_ptr->uid == 0) return(0);
#endif

    /*
     * Check the remaining usable time.  
     */

    /* wall clock time before next status request */
    wctime = tlastsync + conf->sync_interval*60 - time(0);

    if (!(int)acct_db_ptr->unlimit) {
        if (acct_db_ptr->sbu_time >= acct_db_ptr->authorized) {
            if (conf->enforce & ACCTKILL && (int)acct_db_ptr->killjobs) {
                acct_db_ptr->inhibit = TRUE;
    	        (void) appKill(ACCT_STOP);
    	        return (0);
            }
            if (!(int)acct_db_ptr->inhibit) {
                acct_db_ptr->inhibit = TRUE;
    	        (void) _sendmail (ACCTOVER, &macd_ref);
    	    }
        }
        /* skip the account already have alarm request to SMD */
        else if (macd_ref.acct_ptr->req_id[0] == '\0' && 
	    macd_ref.acct_ptr->node_rate && macd_ref.acct_ptr->nnodes &&
            wctime * macd_ref.acct_ptr->node_rate >
    	    acct_db_ptr->authorized - acct_db_ptr->sbu_time) 
    	    (void) smAlarmReq (ACCT_ALARM,
                (long) ((acct_db_ptr->authorized - acct_db_ptr->sbu_time)
    		/ macd_ref.acct_ptr->node_rate * macd_ref.acct_ptr->nnodes
		+ macd_ref.acct_ptr->cpu_time));
    }

    if (!(int)user_db_ptr->use) {
	(void) appKill(USER_STOP);
	(void) _sendmail (USERNOUSE, &macd_ref);
    }
    else if (!(int)user_db_ptr->unlimit) {
        if (user_db_ptr->sbu_time >= user_db_ptr->authorized) {
            if (conf->enforce & USERKILL && (int)acct_db_ptr->killjobs) 
		(void) appKill(USER_STOP);
            else if (!(int)user_db_ptr->inhibit) (void) _sendmail (USEROVER, &macd_ref);
            user_db_ptr->inhibit = TRUE;
    	    return (0);
        }
        if (macd_ref.user_ptr->req_id[0] == '\0' &&
	    macd_ref.user_ptr->node_rate && macd_ref.user_ptr->nnodes &&
            wctime * macd_ref.user_ptr->node_rate >
	    user_db_ptr->authorized - user_db_ptr->sbu_time) 
		(void) smAlarmReq (USER_ALARM,
		(long) ((user_db_ptr->authorized - user_db_ptr->sbu_time) /
		macd_ref.user_ptr->node_rate * macd_ref.user_ptr->nnodes
		+ macd_ref.user_ptr->cpu_time));
    }
    return (0);

}

/*
 * set_ref ()
 *
 * Abstract:	This routine set pointers in macd_ref according to
 *		the data received from SMD, thus they will point to the
 *		relevant entries in the application tree.  It starts
 *		from the top of the tree, match account id, user id,
 *		partition id and pgid, downward the tree link one level
 *		at a time and stop when no-match, then the pointer and
 *		the pointers in the level below will be set to NULL.
 *
 * Arguments:	sm_data -	in-coming data from SMD
 *
 * Return value:	0 -	successful
 *			-1 -	error
 */

int set_ref (sm_data)
struct smd_resp *sm_data;
{
    int i;
    extern struct app_ref macd_ref;
    extern struct acct_ent *top_link;
    extern void bzero();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr, "Enter set_ref (sm_data=%d)\n", sm_data);
	(void) fflush (stderr);
    }

    /* match account id */
    (void) bzero ((char *)&macd_ref, sizeof (struct app_ref));
    if (top_link == NULL) return (-1);

    if (sm_data->q_id.acct_id == QUALIFY_ANY) return (-1);

    macd_ref.acct_ptr = top_link;

    if (macd_ref.acct_ptr->acct_id != sm_data->q_id.acct_id) {
	if (_debug_) i=0;
	for (macd_ref.acct_ptr = top_link, macd_ref.prev_acct = NULL;
	    macd_ref.acct_ptr != NULL; 
	    macd_ref.prev_acct = macd_ref.acct_ptr,
	    macd_ref.acct_ptr = macd_ref.acct_ptr->next) {
	    if (macd_ref.acct_ptr->acct_id == sm_data->q_id.acct_id) {
		macd_ref.user_ptr = macd_ref.acct_ptr->user_list;
		macd_ref.prev_user = NULL;
		break;
	    }
	    if (_debug_ && i++>100) {
		(void) dump_ref ("set_ref acct loop");
		return (-1);
	    }
	}
	if (macd_ref.acct_ptr == NULL) {
	    (void) bzero ((char *)&macd_ref, sizeof (struct app_ref));
	    return (-1);
	}
    }

    /* match user id */
    if (macd_ref.user_ptr == NULL) 
	macd_ref.user_ptr = macd_ref.acct_ptr->user_list;

    if (macd_ref.user_ptr == NULL || sm_data->q_id.uid == QUALIFY_ANY) {
	macd_ref.user_ptr = NULL;
	macd_ref.prev_user = NULL;
	macd_ref.part_ptr = NULL;
	macd_ref.prev_part = NULL;
	macd_ref.pg_ptr = NULL;
	macd_ref.prev_pg = NULL;
	if (sm_data->q_id.uid == QUALIFY_ANY) return (0);
	return (-1);
    }

    if (macd_ref.user_ptr->uid != sm_data->q_id.uid) {
	if (_debug_) i=0;
	for (macd_ref.user_ptr = macd_ref.acct_ptr->user_list,
	    macd_ref.prev_user = NULL;
	    macd_ref.user_ptr != NULL;
	    macd_ref.prev_user = macd_ref.user_ptr,
	    macd_ref.user_ptr = macd_ref.user_ptr->next) {
	    if (macd_ref.user_ptr->uid == sm_data->q_id.uid) {
		macd_ref.part_ptr = macd_ref.user_ptr->part_list;
                macd_ref.prev_part = NULL;
		break;
	    }
	    if (_debug_ && i++>100) {
		(void) dump_ref ("set_ref user loop");
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

    /* match partition id */
    if (macd_ref.part_ptr == NULL)
	macd_ref.part_ptr = macd_ref.user_ptr->part_list;

    if (macd_ref.part_ptr == NULL || sm_data->q_id.part_id == QUALIFY_ANY) {
	macd_ref.part_ptr = NULL;
	macd_ref.prev_part = NULL;
	macd_ref.pg_ptr = NULL;
	macd_ref.prev_pg = NULL;
	if (sm_data->q_id.part_id == QUALIFY_ANY) return (0);
	return (-1);
    }

    if (macd_ref.part_ptr->part_id != sm_data->q_id.part_id) {
	if (_debug_) i=0;
	for (macd_ref.part_ptr = macd_ref.user_ptr->part_list,
	    macd_ref.prev_part = NULL;
	    macd_ref.part_ptr != NULL;
	    macd_ref.prev_part = macd_ref.part_ptr,
	    macd_ref.part_ptr = macd_ref.part_ptr->next) {
	    if (macd_ref.part_ptr->part_id == sm_data->q_id.part_id) {
                macd_ref.pg_ptr = macd_ref.part_ptr->pg_list;
                macd_ref.prev_pg = NULL;
		break;
	    }
	    if (_debug_ && i++>100) {
		(void) dump_ref ("set_ref part loop");
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

    /* match process group id */
    if (macd_ref.pg_ptr == NULL) macd_ref.pg_ptr = macd_ref.part_ptr->pg_list;

    if (macd_ref.pg_ptr == NULL || sm_data->q_id.pgid == QUALIFY_ANY) {
	macd_ref.pg_ptr = macd_ref.prev_pg = NULL;
        if (sm_data->q_id.pgid == QUALIFY_ANY) return (0);
	return (-1);
    }

    if (macd_ref.pg_ptr->pgid != sm_data->q_id.pgid)
	if (_debug_) i=0;
	for (macd_ref.pg_ptr = macd_ref.part_ptr->pg_list,
	    macd_ref.prev_pg = NULL;
	    macd_ref.pg_ptr != NULL;
	    macd_ref.prev_pg = macd_ref.pg_ptr,
	    macd_ref.pg_ptr = macd_ref.pg_ptr->next) {
	    if (macd_ref.pg_ptr->pgid == sm_data->q_id.pgid) return (0);
	    if (_debug_ && i++>100) {
		(void) dump_ref ("set_ref pg loop");
		return (-1);
	    }
	}
    macd_ref.prev_pg = NULL;
    return (-1);
}


/*
 * find_pg ()
 *
 * Abstract:	This routine searches the whole application tree for
 *		a matching pgid.  If a match is found, the macd_ref
 *		is set to point to all relevant entries for the
 *		application; if no match, macd_ref is set to NULLs.
 *
 * Arguments:	pgid -	process group id for an application
 *
 * Return value:	0 -	found
 *			-1 -	not found
 */

int find_pg (pgid)
int pgid;
{
    int i,j,k,l;
    extern struct app_ref macd_ref;
    extern struct acct_ent *top_link;
    extern int _debug_;
    extern void bzero();

    if (_debug_) {
	(void) fprintf (stderr, "Enter find_pg (pgid=%d)\n", pgid);
	(void) fflush (stderr);
    }

    for (i=0,macd_ref.acct_ptr = top_link, macd_ref.prev_acct = NULL;
	macd_ref.acct_ptr != NULL; 
	macd_ref.prev_acct = macd_ref.acct_ptr,
	macd_ref.acct_ptr = macd_ref.acct_ptr->next, i++) {
	if (_debug_ && i==50) {
	    (void) fprintf (stderr, "find_pg (pgid=%d) in account loop 50 times\n", pgid);
	    (void) fflush (stderr);
	    i=0;
        }
	for (j=0,macd_ref.user_ptr = macd_ref.acct_ptr->user_list,
	    macd_ref.prev_user = NULL;
	    macd_ref.user_ptr != NULL;
	    macd_ref.prev_user = macd_ref.user_ptr,
	    macd_ref.user_ptr = macd_ref.user_ptr->next,j++) {
	    if (_debug_ && j==50) {
	        (void) fprintf (stderr, "find_pg (pgid=%d) in user loop 50 times\n", pgid);
	        (void) fflush (stderr);
	        j=0;
            }
	    for (k=0,macd_ref.part_ptr = macd_ref.user_ptr->part_list,
		macd_ref.prev_part = NULL;
		macd_ref.part_ptr != NULL;
		macd_ref.prev_part = macd_ref.part_ptr,
		macd_ref.part_ptr = macd_ref.part_ptr->next,k++) {
	        if (_debug_ && k==50) {
	            (void) fprintf (stderr, "find_pg (pgid=%d) in part loop 50 times\n", pgid);
	            (void) fflush (stderr);
	            k=0;
                }
		for (l=0,macd_ref.pg_ptr = macd_ref.part_ptr->pg_list,
		    macd_ref.prev_pg = NULL;
		    macd_ref.pg_ptr != NULL;
		    macd_ref.prev_pg = macd_ref.pg_ptr,
		    macd_ref.pg_ptr = macd_ref.pg_ptr->next,l++) {
	            if (_debug_ && l==50) {
	                (void) fprintf (stderr, "find_pg (pgid=%d) in pg loop 50 times\n", pgid);
	                (void) fflush (stderr);
	                l=0;
                    }
		    if (macd_ref.pg_ptr->pgid == pgid) return (0);
		}
	    }
	}
    }
    (void) bzero ((char *)&macd_ref, sizeof (struct app_ref));
    return (-1);
}


/*
 * ll_find_pg ()
 *
 * Abstract:	This routine searches the an application link-list
 *		(pg_list) of a partition entry in the application
 *		tree for matching pgid.
 *
 * Arguments:	pgid -	process group id for an application
 *
 * Return value:	>NULL -	valid pg_end pointer, if found 
 *			NULL -	not found
 */

struct pg_ent *ll_find_pg (pg_ptr, pgid)
struct pg_ent *pg_ptr;
int pgid;
{
    struct pg_ent *found;
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr,
            "Enter ll_find_pg (pg_ptr=%d, pgid=%d)\n", pg_ptr, pgid);
	(void) fflush (stderr);
    }

    for (found = pg_ptr; found != NULL; found = found->next)
	if (found->pgid == pgid) return (found);
    return (NULL);
}
