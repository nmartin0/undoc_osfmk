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
 * File:        upd.c
 *
 * Abstract:    This file contains routines handling update hold_list
 *		and update resume request
 */

#include <stdio.h>
#include "db.h"
#include "macd.h"
#include "appacct.h"

/******************************************************************************
*
* upd_resume()
*
* Abstract:     This routine is invoked upon a C_UPD_RESUME request.
*		First, it reads the database from disk to in-core memory.
*               then it adds the accumulated rollins and charges into
*		database and writes the resulting database onto disk.
*
* Arguments:    None
*
* Return value: 0	successful
*		-1	error
*
******************************************************************************/
 
upd_resume ()
{
        struct defer_upd *ptr, *frptr;
	struct cpulim_info *acct_db_ptr, *user_db_ptr;
        extern struct defer_upd *defer_top;
	extern struct cpulim_info *db;
	extern char *timstr();
	extern char *malloc();
	extern int db_writeall(), _debug_;

	if (_debug_) {
	    (void) fprintf (stderr, "Enter upd_resume()\n");
	    (void) fflush (stderr);
	}

	/*
	 * Get new database
	 */

	(void) free (db);
	if (db_init(DB_OVERFLOW) != 0) return (-1);
	if (rehash(DB_REHASH) != 0) return (-1);
 
	if (defer_top == NULL) {
		if (_debug_) {
		    (void) fprintf (stderr,
			"Upd_resume():  No deferred usage to be updated\n");
		    (void) fflush (stderr);
		}
		return (0);
	}

        /*
         * update MACD database
         */
 
        for (ptr=defer_top, defer_top=NULL; ptr!=NULL; 
		frptr=ptr, ptr=ptr->next, free (frptr)) {
		if ((acct_db_ptr = db_getagid (ptr->acct_id)) == NULL ||
        		(user_db_ptr = db_getuid (ptr->uid, ptr->acct_id)) 
			== NULL) {
			if (_debug_) {
			    (void) fprintf (stderr,
				"Upd_resume():  No records for acct %d user %d in new database\n",
				ptr->acct_id, ptr->uid);
			    (void) fflush (stderr);
			}
			continue;
		}
                acct_db_ptr->used_time += ptr->rollin;
                acct_db_ptr->sbu_time += ptr->charge;
                user_db_ptr->used_time += ptr->rollin;
                user_db_ptr->sbu_time += ptr->charge;
		if (_debug_) {
		    (void) fprintf (stderr,
			"Upd_resume():  Add rollin=%d charge=%f for acct %d user %d in new database\n",
			ptr->rollin, ptr->charge, ptr->acct_id, ptr->uid);
		    (void) fflush (stderr);
		}
	} 

	/*
	 * flush database to disk
	 */

	if (db_writeall()< 0)
               (void) printf("MACDSYNC : %s - Error in flushing database after UPD_RESUME\n",
                timstr(0));
        else (void) printf("MACDSYNC : %s - Flushed database after UPD_RESUME\n", timstr(0));
	if (_debug_) {
	    (void) fprintf (stderr, "Exit upd_resume()\n");
	    (void) fflush (stderr);
	}
	return (0);
}

/******************************************************************************
*
* upd_hold_list()
*
* Abstract:     During the time of upd_hold=1, upd_db() will call this
*		routine to save the usage information in the hold_list
*		instead to the in-core database.
*		The hold_list is a linked list.
*
* Arguments:    acct_id -	account id
*		uid -	user id
*		rollin -	rollin CPU node-time
*		charge -	calculated charge by upd_db()
*
* Return value: 0       successful
*               -1      error
*
******************************************************************************/

upd_hold_list (acct_id, uid, rollin, charge)
int acct_id, uid;
long rollin; 
double charge;
{
	struct defer_upd *ptr;
	extern struct defer_upd *defer_top;
	extern int _debug_;

	if (_debug_) {
	    (void) fprintf (stderr, 
		"Enter upd_hold_list(acct_id%d, uid=%d, rollin=%d, charge=%f)\n",
		acct_id, uid, rollin, charge);
	    (void) fflush (stderr);
	}

	/*
	 * if an entry exist for the acct_id/uid, increment amounts
	 */

	for (ptr=defer_top; ptr!=NULL; ptr=ptr->next) {
		if (ptr->acct_id == acct_id && ptr->uid == uid) {
			ptr->rollin += rollin;
			ptr->charge += charge;
			if (_debug_) {
			    (void) fprintf (stderr,
				"Upd_hold_list(): Add rollin=%d charge=%f for acct %d user %d\nin an existing entry in the hold_list\n",
				rollin, charge, acct_id, uid);
			    (void) fflush (stderr);
			}
			return (0);
		}
	}

	/*
	 * need a new entry for the acct_id/uid
	 */

	if ((ptr = (struct defer_upd *)malloc (sizeof(struct defer_upd)) ) == NULL) {
		return (-1);
	}
	ptr->acct_id = acct_id;
	ptr->uid = uid;
        ptr->rollin += rollin;
        ptr->charge += charge;
	ptr->next = defer_top;
	defer_top = ptr;
	if (_debug_) {
	    (void) fprintf (stderr,
		"Upd_hold_list(): Add a new entry in the hold_list for acct %d user %d rollin=%d charge=%f\n",
		acct_id, uid, rollin, charge);
	    (void) fprintf (stderr, "Exit upd_hold_list()\n");
	    (void) fflush (stderr);
	}
	return (0);
}
