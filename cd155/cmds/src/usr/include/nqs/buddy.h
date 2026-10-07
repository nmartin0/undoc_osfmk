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
 
/*++ buddy.h
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/buddy.h,v $
 *
 * DESCRIPTION:
 *
 *	This is the header file for the Modified Buddy System
 *
 *
 *	Developer:
 *	----------
 *
 *	Michael Wan of San Deigo Supercomputer Center.
 *
 *
 *
 */
/*
 * HISTORY
 * $Log: buddy.h,v $
 * Revision 1.15  1995/03/21  22:07:55  kremenek
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
 * Revision 1.14  1995/03/17  18:36:18  kremenek
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
 * Revision 1.13  1994/11/19  02:43:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.12  1994/08/31  20:23:06  bradf
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
 * Revision 1.10.2.1  1994/08/05  20:29:05  kremenek
 *  Reviewer: George Kremenek SDSC
 *  Risk: Low
 *  Benefit or PTS #: 6589  9491 9950 10355
 *  Testing: EATS
 *  Module(s):
 *
 * Revision 1.11  1994/08/05  16:57:02  mwan
 *  Reviewer: George Kremenek SD
 *  Risk: Low
 *  Benefit or PTS #: 6589 10355 10355
 *  Testing: ETS
 *  Module(s):
 *
 * Revision 1.8.2.2  1994/03/23  17:55:37  mwan
 * Fixed pts 8599,8576,8600,8601
 *
 *  Reviewer: kremenek
 *  Risk: L
 *  Benefit or PTS #: 8599,8576,8600,8601
 *  Testing:
 *  Module(s): usr/lib/nqs/macs_rootp.c,
 * 	    usr/include/nqs/buddy.h, usr/lib/nqs/macs_lib.c
 * 	    usr/lib/nqs/nqs_spawn.c usr/lib/nqs/macs_sched.c
 * 	    usr/lib/nqs/macs_job.c
 *
 * Revision 1.8.2.1  1994/01/26  19:58:42  mwan
 * Certain error situations cause NQS to empty its queues.
 *
 *  Reviewer: Jkearns
 *  Risk: L
 *  Benefit or PTS #: 7838, 7858
 *  Testing:
 *  Module(s): usr/include/nqs/requestcc.h, usr/include/nqs/buddy.h,
 *                    usr/ccs/lib/rcmmsgs.c,
 *                    usr/lib/nqs/nqs_reqser.c, usr/lib/nqs/nqs_reqexi.c,
 *                    usr/lib/nqs/nqs_reqcom.c, usr/lib/nqs/nqs_spawn.c
 * 		   usr/lib/nqs/macs_rootp.c
 *
 * Revision 1.5  1993/02/18  22:15:16  mwan
 * Before 2/26/92 frozen date - Change def part priority to handle dedicated jobs.
 *
 * Revision 1.4  1992/12/16  23:23:23  mwan
 * T6 update 2
 *
 * Revision 1.3  1992/10/26  19:42:31  mwan
 * T6 update 1
 *
 * Revision 1.2  1992/10/09  22:17:28  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 *
 */

#ifdef SDSC
#define in_file stdin           /* input job file */

#define DEBUG   1
#define TEST    1		/* It is only a test */


#define N_KEYWD       30        /* number of input in the sched_param file */
#define INSIZE        512       /* input buffer size */
#define SEC_P_DAY     86400     /* number of seconds per day */
#define NAME_LEN      24        /* max length of name */
#define TZ_LEN        2*NAME_LEN /* max length of name */
#define PART_NAME_LEN 128
#define MAX_JOB       500	/* max number of job_req structure */
#define MAX_BLOCK     1500	/* max number of block structure */
#define MAX_BUD       4		/* max number of buddies per division */
#define DAYS_PER_W    7		/* number of days per week */
#define MKPART_RETRY  2		/* No to time to retry for mkpart */
#define MKPART_SLEEPT 60	/* sleep seconds before retrying again */
#define MACS_RETRY    2		/* No to time to retry for comm with MACS */
#define MACS_SLEEPT   60	/* sleep seconds before retrying again */
#define TSCHED_TSLOT  1800	/* tennis court time slot in sec */
#define MAX_SLOT_CNT  336	/* max number of tsched slot, 7 x 24 x 2 */
#define MAX_ACTUSER   100	/* max number of active users */

/* add_job () or add_blk () flags. */

#define LAST          0		/* add to the end of the queue */
#define TOP           1		/* add to the top of the queue */
#define TIME_PRI      2		/* decreasing order of req_time */
#define SIZE          3		/* decreasing order of size */
#define BEST          4		/* smallest that is >= the input value */
#define OVER          5		/* smallest that is >= the input value */
#define UNDER         6		/* largest that is <= the input value */
#define PRI           7		/* decreasing order of priority */
#define ST_TIME       8		/* increasing order of start_time */

/* Prime and Non-prime defination */

#define MAX_PERIOD    2		/* number of time periods */
#define PRIME_M	      0		/* Prime mode */
#define NPRIME_M      1		/* Non-prime mode */

/* Layer, set and group */

#define MAX_LAYER     3		/* number of layers */
#define BASE_LAYER    0		/* The base layer */
#define TS_LAYER      1         /* Timeshare layer */
#define TC_LAYER      2         /* Tennis court layer */

#define MAX_SET       64	/* number of node sets */
#define MAX_GROUP     64	/* number of node groups */
#define MAX_RPART     5		/* number of root partition */

#define MAJOR         0		/* the major partition */
#define MINOR         1		/* The minor partition */
#define COMBO         2		/* The combo partition */

/* Search type */

#define LARGE		0	/* large type */
#define SMALL		1	/* small type */

#define SZ_FACTOR	4	/* the determining factor for classifying
				 * whether a request is of large or small
				 * type. If the request is >= 1/SZ_FACTOR
				 * of the the first child of root, it is
				 * classified as large. Otherwise, it is
				 * small.
				 */

#define FRAG_FACTOR	4	/* fragmentation factor for determining
				 * the initial search order. This initial
				 * search will start with the largest buddy
				 * the is <= the (request size*FRAG_FACTOR).
				 */

#define PART_PFIX	"NQS_"	/* NQS job partition name prefix. i.e., all
				 * partitions created by NQS will have the
				 * form "NQS_nnnnn", where nnnnn is the NQS
				 * request number.
				 */

/* value for macs_flag */

#define MACS_OFF        0       /* do not talk to macs */
#define MACS_ON         1       /* Macs is on. Talk to it */
#define MACS_ON_NOKILL  2       /* Macs is on. Allow jobs to run even if NQS
                                 * cannot talk to it */

/* value for pn_overrun */

#define NO_OVERRUN	1	/* do not overrun to Non-prime */
#define OK_OVERRUN	0	/* overrun to prime time */

/* definition for checking soft and hard user_limit */

#define CHK_SOFT_ULIM	0
#define CHK_HARD_ULIM	1

/* default input parameters */
 
#define DF_TIME_ZONE     "PST8PDT"
#define DF_PRIME_START   8              /* 8 am */
#define DF_PRIME_END     18             /* 6 pm */
#define DF_PRIME_SET     0              /* prime set */
#define DF_NPRIME_SET    0              /* nprime set */
#define DF_TIMESHARE     0              /* no time share*/
#define DF_TIMESH_MINOR  0              /* no time share of the minor part */
#define DF_TIMESH_PRI    0              /* min pri to trigger timeshare */ 
#define DF_BLOCK_TS_PRI  -1             /* min pri for timesh blocking */ 
#define DF_BLOCK_PRI     -1             /* min pri for blocking base layer */ 
#define DF_TSCHED_PRI    64             /* min pri for tennis court sched */ 
					/* 64 => no tsched since max_p = 63 */
#define DF_CHK_RUNLIMIT  0              /* do not chk NQS runlimit */ 
#define DF_AGE_FACTOR    0.0            /* pri aging factor */ 
#define DF_GRACE_TIME    10             /* 10 sec grace period */
#define DF_ROLLIN_QUAN   600		/* 600 sec */
#define DF_CUR_OPEN_NAME  "\0"		/* no currently active open partition */
#define DF_OPEN_NP_NAME  "\0"		/* no OPEN_NP partition */
#define DF_OPEN_P_NAME  "\0"		/* no OPEN_P partition */
#define DF_DO_WALL 0
#define DF_MACS_FLAG 0                  /* do not talk to macs */
#define DF_TIME_FLAG 0			/* MAJOR partition */
#define DF_NP_OVERRUN	OK_OVERRUN	/* allow overrun */
#define DF_USELOGIN 0                   /* do not use .login, .cshrc */
#define DF_NOSCHED 0                    /* schedule jobs for running */
#define DF_SCR2PRIM "\0"                /* do not use script from non_prime to prime */
#define DF_SCR2NONPRIM "\0"             /* do not use script from prime to non_prime */
#define DF_WALL_CLOCK 0			/* use SMD for per_request CPU time limit */

/* partition attribute */

#define PART_MOD      0744
#define USER_PART_MOD 0544
#define NPRIME_RQ     10    /* 10 sec - rollin quantum during non-prime */

/* partition priority */

#define LOW_PART_PRI     1
#define AVE_PART_PRI     2
#define HIGH_PART_PRI    3
#define PREEMPT_PART_PRI 4

#define PARAM_FILE    "/usr/spool/nqs/conf/sched_param"
#define PART_ROOTPATH "/etc/nx"
#define DF_PART_ROOTPATH "/etc/nx/compute/"
#define PARTINFO      ".partinfo"
#define NXACCOUNTS    "/etc/nxaccounts"

/* some smail definations */

#define CMD_LEN       200       /* mail command length */
#define MAILPROG      "/usr/bin/mailx"
#define ERRLOG        "/dev/null"

/* SCHED_FLAG defination */

#define SCHED         1         /* do scheduling */
#define NO_SCHED      0         /* don't do scheduling */

/* block and block queue structure */

struct block {			/* block structure */
	long free_nodes;	/* no. of free nodes in this block, or
				 * no. of free nodes in the descendants  */
	long x;			/* The x location of the upper left corner */
	long y;			/* The y location of the upper left corner */
	long width;		/* The width (x-axis) of the block */
	long height;		/* The height (y-axis) of the block */
	long no_combo;		/* if set, this block will not be combined */
				/* with its peer. */
	long child_cnt;		/* No. of immediate descendants. If the block
				 * has not been split, set to zero */
	struct block *child[MAX_BUD]; /* pointers to immediate decendants */
	struct block *parent;
	struct block *prev;	/* backward pointer of the double link que */
	struct block *next;	/* forward pointer */
	struct node_set *node_set; /* the root set of this block */
	int layer_inx;		/* the layer index */
};

struct blk_que {		/* block queue header */
	struct block *top;	/* pointer to the top of the block queue */
	struct block *last;	/* pointer to the end of the block queue */
};

/* Job request and job queue structure */

struct job_que {		/* job queue header */
	long req_nodes;		/* number of nodes if wait_que */
	struct job_req *top;    /* pointer to the top of the job_req queue */
	struct job_req *last;   /* pointer to the end of the job_req queue */
};

struct actuser {
	int uid;
	int node_used;
};

struct job_req {		 /* job request header */
	long req_nodes;		 /* number of node requested */
	long req_time;		 /* request time in sec */
	float prior;		 /* request priority */
	long start_time;	 /* time at which the job should be started */
	int jobname;
	int run_flag;		 /* 1->running in NQS, 0->not running */
        int uid; 		 /* uid */
	int acct_id;		 /* accounting id */
	int orig_mid;		 /* NQS mid */
	int orig_seqno;		 /* NQS seqno */
	int part_id;		/* partition id */
	int period_inx;		/* the period index */
	int grp_inx;		/* node_group index */
	struct request *nqs_req; /* nqs request pointer */
        char username[NAME_LEN]; /* login user name */
        char part_name[PART_NAME_LEN]; /* partition name */
        char que_name[NAME_LEN]; /* name of queue */
	struct blk_que blk_que;  /* list of blocks allocated to this job */
	struct job_req *prev;	 /* backward pointer of the double link que */
	struct job_req *next;	 /* forward pointer */
};

/* Node set structure */

struct node_set {
	int rootp_inx;			/* The root partition index */
	int size;			/* size of this set */
	struct block *root_blk[MAX_LAYER];	/* the root block */
	long def_order[MAX_BUD];     	/* default search order */
	char nodef[NAME_LEN];		/* the nodef */
	int blocked[MAX_LAYER];    	/* 0 = base layer  not blocked, */
					/* 1 = blocked */
};

/* Layer structure */

struct layer {
	struct job_que inuse_que[MAX_PERIOD + 1];    /* inuse queue */
	struct job_que wait_que;     /* list of job in queue */
};

/* Node group structure */

struct node_grp {
	int num_set;		        /* number of sets */
	int set_inx[MAX_SET];		/* node sets members of this group */
	int time_flag[MAX_SET];		/* 0 = major, 1 = minor */
	int free_nodes[MAX_LAYER][MAX_PERIOD];	/* free_nodes for each */
						/* layer and time_group */
	int size[MAX_PERIOD];			/* size for each */
						/* layer and time_group */
};

/* Root partition structure */

struct root_part {
	char part_name[PART_NAME_LEN];	/* parent partition name */
	int *phynode;			/* pointer to the phynode array */
};

/* some global variables */

struct glb {
	long job_in_que;
	long max_x;		/* number of nodes in x-direction */
	long max_y;		/* number of nodes in y-direction */
        long cur_time24;         /* current time in 24 hr. clock */
        long p_start;           /* start of PRIME_M */
        long p_end;             /* end of PRIME_M */
        long cur_time;          /* current time in sec since 1970 */
	int  cur_mode;		/* current mode, PRIME or NON_PRIME */
        int sched_flag;         /* SCHED OR NO_SCHED */
        long macs_flag;         /* 0 = macs_sched not busy, 1 = busy */
	int nxaccounts;		/* 1 = nxaccount file exists, 0 = not */
	int *work_lognode;
	int param_read;		/* 0 = have not read the sched_param file */
				/* 1 = read */
	int rpart_cnt;		/* number of root partitions */
	int num_set;		/* number of sets */
	int set_inx[MAX_SET];	/* set array */
	int num_group;		/* number of groups */
	int grp_inx[MAX_GROUP];	/* group array */
	int num_actuser;
	struct actuser actuser[MAX_ACTUSER];
};

/* struct for input param */
 
struct inp {
        char time_zone[TZ_LEN];          /* time zone */
	long mesh_w;			 /* no. of columns nodes */
	long mesh_h;			 /* no. of rows nodes */
        long prime_start[DAYS_PER_W];    /* starting hr of interactive period */
					 /* 1 val for each day starting SUN */
        long prime_end[DAYS_PER_W];      /* ending hr of interactive period */
	char cur_open_name[PART_NAME_LEN]; /* name of the currently active */
					   /* partition */
        char open_np_name[PART_NAME_LEN]; /* name of open_np part, */
        char open_p_name[PART_NAME_LEN];  /* name of open_p part, */
	int timeshare;		/* timeshare ? 0 = no, 1 = yes */
	int timesh_minor;	/* timeshare minor part ? 0 = no, 1 = yes */
        int timesh_pri;         /* min priority to trigger timesharing */
        int block_ts_pri;       /* min priority for timesh blocking */
        int block_pri;          /* min priority for blocking */
        int tsched_pri;         /* min priority for tennis court sched */
	int chk_runlimit;	/* 1->check NQS runlimit, 0->no check */
	float age_factor;	/* time coefficient for priority */
        int grace_time;         /* grace period in second. When a job's
                                 * time is up, a SIGUSR2 signal is sent
                                 * , then, after grace_time seconds, a
                                 * SIGKILL signal will be sent.
                                 * SIGKILL signal will be sent.
                                 */
	int rollin_quan;	/* part rollin quantum */
	int do_wall;
	int macs_flag;
	int np_overrun;		/* overrun Non-prime period ? 0 = yes, 1 = no */
        int uselogin;           /* use .login, .cshrc ? 0 = no, 1 = yes */
        int nosched;            /* schedule jobs to run ? 0 = yes, 1 = no */
        char sToPrimeScriptName[MAX_PATHNAME+1];  /* name of the script performed from non_prime to prime period */
        char sToNonPrimeScriptName[MAX_PATHNAME+1];  /* name of the script performed from non_prime to prime period */
	int iWallClock;         /* default = 0 uses SMD time, = 1 uses wall clock time */
};

/* input parameters */
 
#define TIME_ZONE     inp.time_zone
#define MESH_W        inp.mesh_w
#define MESH_H        inp.mesh_h
#define PRIME_START   inp.prime_start
#define PRIME_END     inp.prime_end
#define CUR_OPEN_NAME inp.cur_open_name
#define OPEN_NP_NAME  inp.open_np_name
#define OPEN_P_NAME   inp.open_p_name
#define TIMESHARE     inp.timeshare
#define TIMESH_MINOR  inp.timesh_minor
#define TIMESH_PRI    inp.timesh_pri
#define BLOCK_TS_PRI  inp.block_ts_pri
#define BLOCK_PRI     inp.block_pri
#define TSCHED_PRI    inp.tsched_pri
#define CHK_RUNLIMIT  inp.chk_runlimit
#define AGE_FACTOR    inp.age_factor
#define GRACE_TIME    inp.grace_time
#define ROLLIN_QUAN   inp.rollin_quan
#define DO_WALL	      inp.do_wall
#define MACS_FLAG     inp.macs_flag
#define NP_OVERRUN    inp.np_overrun
#define USELOGIN      inp.uselogin
#define NOSCHED       inp.nosched
#define SCR2PRIM      inp.sToPrimeScriptName
#define SCR2NONPRIM   inp.sToNonPrimeScriptName
#define WALL_CLOCK    inp.iWallClock

/* schedule struct */

struct schedule {
	long node_cnt[MAX_PERIOD + 1]; /* for now, major, minor and combo */
	long mode;
};
#endif
