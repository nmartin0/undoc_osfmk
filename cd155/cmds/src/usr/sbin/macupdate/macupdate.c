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
* File: macupdate.c
*
* Abstract: 	This file contains the routines of MACS
*		database update utility.
*
******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include "db.h"
#include "macd.h"
#include "mac.h"
#include "filename.h"
#include <nx/nxacct.h>

#define	PERCENT100 10000
#define SET	0
#define ADD	1
#define WEIGHT	2

extern char *malloc();
void dumprecord();

FILE *log, *fd;
int err_count=0;

/*
 * main()
 *
 * Abstract:    This utility allows system administrator update/reset
 *              account allocations/usages in MACD database.  Although
 *		macadmin/macalloc can change many account attributes in
 *		MACD database, the MACUPDATE is the only utility can
 *		change/reset the usage field in an account.
 *		MACUPDATE sends C_UPD_HOLD message to MACD before updates
 *		the MACD database and sends C_UPD_RESUME message to MACD
 *		when it's done with the update.
 *		This utility require update data input from a file, one
 *		account per line, each line must in the format:
 *		acct_name alloc_weight add_alloc usage_remain
 *		thus new_usage = old_usage * usage_remain
 *		and new_allocation = remaining_allocation * alloc_weight
 *					+ addition_alloc + new_usage
 *		where alloc_weight normally is between 0 - 100 as percentage
 *		and usage_remain normally is either 0 or 1 
 *
 * command:	macupdate formatted_input_file
 */

main(argc,argv) 
int argc;
char *argv[];
{
	int agid,add_alloc,alloc_weight,rval,done,verbose=0;
        char buf[256];
	char *logfile, *in_fname;
	extern char *timstr();

	/* 
	  Verify that MACS is in "macwatch" mode 
	*/
	is_macwatch();

        /*
          Check the command line
        */
	if (argc != 2 && argc != 3) {
	    (void) fprintf (stderr, "\nUsage:\tmacupdate [-v] input_file_name\n");
	    exit (-1);
	}
	else if (strcmp (argv[1], "-v") == 0) {
		if (argc != 3) {
	    		(void) fprintf (stderr, 
				"\nUsage:\tmacupdate [-v] input_file_name\n");
	    		exit (-1);
		}
		verbose = 1;
		in_fname = argv[2];
	}
	else in_fname = argv[1];

	/*
	  Get the name of the log file
	*/

        (void) printf ("\nStart MACD database update\n\nPrepare input file and log file ...... ");
	logfile = malloc(strlen(MACUPD_LOG_PATH)+15);
        (void) sprintf (logfile, "%s/macupdate.log", MACUPD_LOG_PATH);
	log = fopen(logfile, "w");
	if ((fd = fopen (in_fname, "r")) == NULL) {
	    (void) fprintf (stderr, "\nFailed to open the input file %s, abort\n",
		in_fname);
	    exit (-1);
	}


	/*
          Make sure the user knows what he is doing
	*/

        (void) printf ("Done!\n\nThis utility will ask MACD to stop its internal database-update,");
        (void) printf ("\nand to start updating the database with information provided"); 
	(void) printf ("\nin the input file to this utility.");

        switch (get_yes_no("\nContinue (y/n)? ")) {
        case EOF:       
        case 0: if (log) (void) fclose (log);
		(void) fclose (fd);        
		exit(0);                /* EOF/no */
        case 1:         break;                  /* yes */
        }


        /*
          Save maclist -U results before the update
        */
 
	if (log) (void) fprintf (log, "%s %s start macupdate\n",
		timstr(0), getlogin());
        if (verbose) (void) printf ("\nSave maclist -U results before the update ...... ");
	(void) sprintf (buf, "maclist -U > %s/before.update",
			MACUPD_LOG_PATH);
	if ((rval = system (buf)) != 0) {
		(void) fprintf (stderr, "Failed.\nCannot continue - abort\n");
        	if (log) (void) fclose (log);
		(void) fclose (fd);        
		exit (-1);
	}
        if (log) (void) fprintf (log, 
		"%s Saved maclist -U results in %s/before.update\n",
		timstr(0), MACUPD_LOG_PATH);
		

        /*
          Back-up MACD database file
        */
 
        if (verbose) (void) printf ("Done!\nBack-up MACD database file ...... ");
	(void) sprintf (buf, "cp %s %s.bak",
			MACD_DATABASE_FILE, MACD_DATABASE_FILE);
	if ((rval = system (buf)) != 0) {
		(void) fprintf (stderr, "Failed.\nCannot continue - abort\n");
        	if (log) (void) fclose (log);
		(void) fclose (fd);
		exit (-1);
	}
        if (log) (void) fprintf (log, "%s Backed-up %s to %s.bak\n",
		timstr(0), MACD_DATABASE_FILE, MACD_DATABASE_FILE);


	/*
	  Send C_UPD_HOLD to MACD
	*/

	if (verbose) (void) printf ("\nSend C_UPD_HOLD message to MACD ......\n");
        if (mac_msg (C_UPD_HOLD, NULL) != 0) {
                (void) fprintf (stderr, "Failed!\nExit\n");
        	if (log) (void) fclose (log);
		(void) fclose (fd);
                exit(-1);
        }
        if (log) (void) fprintf (log, 
		"%s Sent C_UPD_HOLD message to MACD ok\n", timstr(0));


        /*
          Update MACD database
        */

	if (verbose) (void) printf ("\nDone!\n\n NOTICE!!!\nIF FOR ANY REASON THIS UTILITY EXITS BEFORE COMPLETION, PLEASE DO");
	if (verbose) (void) printf ("\n\n\t# %s start\n\nIf the database file %s is corrupted",
		MACS_START_SCRIPT, MACD_DATABASE_FILE);
	if (verbose) (void) printf ("\ncopy %s.bak into %s, then do\n\n\t# %s start\n\n",
		MACD_DATABASE_FILE, MACD_DATABASE_FILE, MACS_START_SCRIPT);
	if (verbose) (void) printf ("\nUpdate MACD database ......\n");
	if(db_init(DB_OVERFLOW) == (-1)) {
		(void) fprintf(stderr, "Error initializing database\n");
        	if (log) (void) fclose (log);
		(void) fclose (fd);
		exit(1);
	}
        if (log) (void) fprintf (log, "%s Initialized database ok\n", timstr(0));
	if (done = update(verbose)) {
	    if (log) (void) fprintf (log, 
		"%s - %d records updated in database\n", timstr(0), done);
	    if(db_writeall()==(-1)) {
		(void) fprintf(stderr, "Error writing database\n");
        	if (log) (void) fclose (log);
		(void) fclose (fd);
		exit(1);
	    }
            if (log) (void) fprintf (log, 
		"%s Write updated-database to disk ok\n", timstr(0));
	}
        else if (log) {
		(void) fprintf (log, "%s No update made to the database\n", timstr(0));
	}

        /*
          Send C_UPD_RESUME to MACD
        */
 
        if (verbose) (void) printf ("Done!\nSend C_UPD_RESUME message to MACD ......\n");
        if (mac_msg (C_UPD_RESUME, NULL) != 0) {
                (void) fprintf (stderr, "Failed!\nExit\n");
        	if (log) (void) fclose (log);
		(void) fclose (fd);
                exit(-1);
        }
        if (log) (void) fprintf (log, 
		"%s Sent C_UPD_RESUME message to MACD ok\n", timstr(0));


        /*
          Save maclist -U results after the update
        */
 
	if (done) {
            if (verbose) (void) printf ("\nSave maclist -U results after the update ...... ");
	    (void) sprintf (buf, "maclist -U > %s/after.update", MACUPD_LOG_PATH);
	    if ((rval = system (buf)) != 0) {
		(void) fprintf (stderr, "Failed.\nCannot continue - abort\n");
        	if (log) (void) fclose (log);
		(void) fclose (fd);
		exit (-1);
	    }
            if (log) (void) fprintf (log, 
		"%s Saved maclist -U results in %s/after.update\n",
		timstr(0), MACUPD_LOG_PATH);
	}
		
	if (log) (void) fprintf (log, 
		"%s macupdate completed with %d success and %d error\n",
		timstr(0), done, err_count);
	if (verbose) (void) printf (
		"Done!\nMACD database update completed with %d success and %d error\n",
		done, err_count);

        if (log) (void) fclose (log);
	(void) fclose (fd);
	exit(err_count);
}

int update(verbose)
int verbose;
{
	char line[256], account[64], dumchars[64];
	struct nxacct *nxstuff;
	int agid,usage_remain,add_alloc,alloc_weight,rval,done=0;
	struct cpulim_info *tempcpu;
	struct cpulim_info newcpu;
	double old_used,old_alloc;
	extern char *fgets();
	extern int err_count;

	/*
	 * read a line at a time from stdin and process it
	 */
	while(fgets (line, 256, fd) != NULL) {

	    /*
	     * skip a ill-formatted line
	     */
	    if ((rval = sscanf(line, "%s %d %d %d %s",
		account,&alloc_weight,&add_alloc,&usage_remain,dumchars)) != 4) {
		(void) fprintf (stderr, "\nExpect 4 but matched %d\n", rval);
		(void) fprintf (stderr,
		    "\nInvalid format line:\n%s\nNot processed\n", line);
		(void) fflush(stderr);
		err_count++;
		continue;
	    }

	    nxstuff = nx_getanam(account);
	    if (nxstuff == 0) {
                (void) fprintf(stderr,
                    "\nNo account record for %s.  Skipped line:\n%s\n",
                    account,line);
                (void) fflush(stderr);
		err_count++;
                continue;
            }
	    agid = nxstuff->acct_id;

	    /*
	     * get the database record for the account_id
	     */
	    tempcpu=db_getagid(agid);

            /*
             * No database record for the account_id, Skip it
             */
	    if(tempcpu==NULL) {	
		(void) fprintf(stderr,
		    "\nNo database record for agid %d.  Skipped line:\n%s\n",
		    agid,line);
		(void) fflush(stderr);
		err_count++;
		continue;
	    }

            /*
             * invalid weight-factor for carrying allocation balance
             */
	    if (alloc_weight < 0 || alloc_weight > 100) {
		(void) fprintf(stderr,
		    "\nInvalid weight-factor %d for carrying allocation balance.  Skipped line:\n%s\n",
		    alloc_weight,line);
		(void) fflush(stderr);
		err_count++;
		continue;
	    }

            /*
             * invalid weight-factor for carrying usage balance
             */
            if (usage_remain != 0 && usage_remain != 1) {
                (void) fprintf(stderr,
                    "\nInvalid weight-factor %d for carrying usage balance.  Skipped line:\n%s\n",
                    usage_remain,line);
                (void) fflush(stderr);
		err_count++;
                continue;
            }

            /*
             * calculate the new allocation and usage and update the
	     * the database record
             */
	    if (verbose) (void) printf ("Update account %s\n", account);
	    (void) fflush(stderr);
	    (void) dumprecord(tempcpu,"Old acct-entry");
	    add_alloc *= 60.0;
	    old_alloc = tempcpu->authorized;
	    old_used = tempcpu->sbu_time;
	    if (tempcpu->unlimit) tempcpu->authorized = 0.0;
	    else {
		tempcpu->authorized = (old_alloc - tempcpu->sbu_time) 
		    * alloc_weight/100. + add_alloc + tempcpu->sbu_time
		    * usage_remain;
		if (tempcpu->authorized < 0) tempcpu->authorized = 0.0;
	    }
	    if (!usage_remain) tempcpu->sbu_time = tempcpu->used_time = 0;
	    tempcpu->timestamp = time(0);
	    if (!tempcpu->unlimit && (tempcpu->sbu_time > tempcpu->authorized))
		tempcpu->inhibit = 1;
	    else tempcpu->inhibit = 0;

	    (void) dumprecord(tempcpu,"New acct-entry");

	    /*
	     * If the allocation or the usage has changed, recalculate 
	     * users allocations or usages under this account
	     */
	    if (old_alloc != tempcpu->authorized || !usage_remain) {
		struct cpulim_info *u_ptr;
		while ((u_ptr = db_getnextuid(agid)) != NULL) {
		    (void) dumprecord(u_ptr,"Old user-entry");
		    if (old_alloc != tempcpu->authorized) {
			u_ptr->unlimit = 0;
			u_ptr->authorized = 
			    (u_ptr->percent * tempcpu->authorized)/PERCENT100;
		    }
		    if (!usage_remain) {
			u_ptr->sbu_time = 0;
			u_ptr->used_time = 0;
		    }
		    if (u_ptr->sbu_time < u_ptr->authorized)
			u_ptr->inhibit = 0;
		    else u_ptr->inhibit = 1;
		    u_ptr->timestamp = time(0);
		    (void) dumprecord(u_ptr,"New user-entry");
		}
	    }
	    done++;
	}
	return (done);
}


void dumprecord(ptr,s)
struct	cpulim_info	*ptr;
char	*s;
{
	(void) fprintf(log,"%s\t%d:%d:%d:%d:%d:%d:%d:%d:%d:%f:%f:%f:%d\n",s,
	ptr->id,ptr->agid,ptr->shutoff,ptr->weight,ptr->inhibit,ptr->modify,
	ptr->transfer,ptr->use,ptr->percent,ptr->authorized,
	ptr->used_time,ptr->sbu_time,ptr->timestamp);
	(void) fflush(log);
}
