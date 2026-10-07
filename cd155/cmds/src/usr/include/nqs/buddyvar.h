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
 
/* buddyvar.h
 *
 * The buddy allocation system's global variable defination file.
 */
/*
 * HISTORY
 * $Log: buddyvar.h,v $
 * Revision 1.10  1995/03/21  22:08:11  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 10507
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/include/nqs/buddy.h
 * 	cmds_libs/src/usr/include/nqs/buddyvar.h
 * 	cmds_libs/src/usr/lib/nqs/macs_lib.c
 * 	cmds_libs/src/usr/lib/nqs/nqs_spawn.c
 * 	cmds_libs/src/usr/lib/nqs/smd_msg.c
 *
 * Revision 1.9  1995/03/17  18:36:20  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 9765
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/include/nqs/buddy.h
 *    cmds_libs/src/usr/include/nqs/buddyvar.h
 *    cmds_libs/src/usr/lib/nqs/macs_lib.c
 *    cmds_libs/src/usr/lib/nqs/nqs_bsc.c
 *    cmds_libs/src/usr/lib/nqs/nqs_vtimer.c
 *    cmds_libs/src/usr/lib/nqs/macs_sched.c
 *    cmds_libs/src/usr/lib/nqs/macs_rootp.c
 *
 * Revision 1.8  1994/11/19  02:43:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/08/31  20:23:10  bradf
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.5.6.1  1994/08/05  20:30:41  kremenek
 *  Reviewer: George Kremenek SDSC
 *  Risk: Low
 *  Benefit or PTS #: 6589 9491 9950 10355
 *  Testing: EATS passed
 *  Module(s):
 *
 * Revision 1.6  1994/08/05  16:57:06  mwan
 *  Reviewer: George Kremenek SD
 *  Risk: Low
 *  Benefit or PTS #: 6589 10355 10355
 *  Testing: ETS
 *  Module(s):
 *
 * Revision 1.5  1993/11/02  00:05:47  mwan
 * R1.2 mods.
 *
 * Revision 1.3  1993/07/13  18:18:49  mwan
 * T11 - fixed PTS 5012
 *
 * Revision 1.2  1992/10/09  22:17:32  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 *
 */

#include "buddy.h"

/* Block structure */

struct block block[MAX_BLOCK];		/* block structure */
struct blk_que free_blk_que;		/* free block queue */

/* Job Request structure */

struct job_req job_req[MAX_JOB];	/* job request structure */
struct job_que free_job_que;		/* free job request queue */
struct job_que timesh_que;

/* Layer and root part structure */

struct layer layer[MAX_LAYER];
struct root_part rpart[MAX_RPART];
struct node_set node_set[MAX_SET];
struct node_grp node_grp[MAX_GROUP];

/* global variables */

struct glb glb;

/* default search order */

int def_order[MAX_BUD] = { 0, 1, 3, 2 };

char key_wd[N_KEYWD][NAME_LEN] = {
        "time_zone",            /* 0 time zone */
        "mesh_w",               /* 1 number of column nodes */
        "mesh_h",               /* 2 number of row nodes */
        "prime_start",          /* 3 starting hr of prime period */
        "prime_end",            /* 4 ending hr of prime period */
                                /* during prime, batch during non-prime */
        "prime_list",           /* 5 list of sets for prime time */
        "nprime_list",          /* 6 list of sets for non-prime time */
        "node_set",             /* 7 set definition */
        "node_group",           /* 8 node group definition */
        "cur_open_name",        /* 9 name of currently active open part */
        "open_np_name",         /* 10 name of OPEN_NP part */
        "open_p_name",          /* 11 name of OPEN_P part */
        "timeshare",            /* 12 timeshare ? 0 = no, 1 = yes */
        "timesh_minor",         /* 13 timeshare of the minor? 0 = no, 1 = yes */
        "timesh_pri",           /* 14 */
        "block_ts_pri",         /* 15 */
        "tsched_pri",           /* 16 min static pri for tsched */
        "chk_runlimit",         /* 17 */
        "age_factor",           /* 18 */
        "grace_time",           /* 19 */
        "rollin_quan",          /* 20 */
        "do_wall",              /* 21 */
        "block_pri",            /* 22 */
        "macs_flag",            /* 23 */
        "np_overrun",           /* 24 */
        "use_login",            /* 25  use .login, .cshrc? 0 = no, 1 = yes */
        "nosched",              /* 26  start scheduling jobs? 0 = yes, 1 = no */
        "prime_script", 	/* 27  pathname of an executable file
				(typically a shell script) which is run
				at the 	beginning of prime time. */
        "nprime_script",	/* 28  pathname of an executable file
				(typically a shell script) which is run
				at the 	beginning of nonprime time. */
	"wallclock_limits"      /* 29 = 0 (default) per_request CPU time 
				limits is interpreted in node-hours time,
				= 1 time limits is interpreted in 
				wall-clock time. */
};

/* sched_param file input */

struct inp inp;

