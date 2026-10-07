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
 * Data structures used by the SDSC Cpu Quota System Database (iPSC)
 *
 * Copyright (c) 1990 San Diego Supercomputer Center
 *
 */

#ifndef DB_H
#define DB_H

/* filesystem entry */
struct cpulim_info {
#ifdef SDSC
	unsigned	id:24;		/* uid or agid */
	unsigned	agid:24;	/* extra id for agid if this is uid */
#else
	uid_t		id;		/* uid or agid */
	gid_t		agid;		/* extra id for agid if this is uid */
#endif
	unsigned	ref_cnt:8;	/* number of users in account (agid) */
	unsigned	shutoff:1;	/* unused bit to round to 1 word */
	unsigned	weight:1;	/* do nice value weighting of cpu time */
	unsigned	inhibit:1;	/* account has exceeded cputime */
	unsigned 	modify:1;	/* user may modify his allocations */
	unsigned	transfer:1;	/* may transfer time to another agid */
	unsigned 	use:1;		/* user may use time */
	unsigned	empty:1;	/* record is unused (hashing) */
	unsigned	uid_pred:1;	/* is this a uid? */ /*(hashing)end 1st word */
	int		next;		/* used for collision (hashing) */
	unsigned 	percent:16;	/* percentage of time allocated */
	unsigned	killjobs:1;	/* if set kill jobs - dont try chkpnt */
	unsigned	lockjobs:1;	/* dont allow setting of killjobs bit*/
	unsigned	unlimit:1;	/* unlimit allocation bit */
	unsigned	unused2:13;	/* spare bits left in structure */
	int		maxnodes;	/* maximum number of nodes user can allocate */
	double		authorized;	/* total time authorized */
	double		used_time;	/* total time used */
	double		sbu_time;	/* discounted time via nice factor used */
	time_t		non_cpu_time;	/* non cpu charges */
	time_t 		timestamp;	/* time last updated */
};
/* available functions */
int db_findgood();
int db_init();
int db_writeall();
int db_writeuid();
int db_writeagid();
struct cpulim_info *db_getuid();
struct cpulim_info *db_getagid();
struct cpulim_info *db_getnextuid();
int db_insuid();
int db_insagid();
int db_deluid();
int db_delagid();
int rehash();

/* Version and release number of database (stored at beginning of
   database file).  Note that this is NOT necessarily the
   version/release number of MACS. It should be incremented whenever
   the database format changes.  */

#define VERSION 1
#define RELEASE 3

#endif
