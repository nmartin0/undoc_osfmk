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
 
#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

#define DEF_QRATE	1.0
#define ALL_PART -1

#define CLEAR_PG	1		/* clear match flag for pg_list */
#define CLEAR_PART	2		/* clear match flag for part_list */
#define CHECK_MATCH	1		/* check match flag */

#define REMAIN_LOW	60	/* low balance in seconds per node */
#define USER_LOW	1	/* low balance in user account */

#define MAX_QUEUE	64

struct queue {
	char name[24];
	float rate;
};

struct defer_upd {
	int acct_id;
	int uid;
	long rollin;
	double charge;
	struct defer_upd *next;
};

struct pg_ent {			/* application */
	int	pgid;		/* process group id */
	int	app_size;	/* number of nodes used */
	long	cpu_time;	/* accumulated roll-in time * nodes */
	time_t	start_time;	/* application start time */
	time_t	last_update;	/* time stamp of last info from SM */
	int	match;		/* flag for matching */
	struct pg_ent *next;	/* pointer to next application */
};

struct part_ent {		/* partition */
	int	part_id;	/* partition id */
	time_t	start_time;	/* job start time */
	int	part_size;	/* number of nodes requested by user */
	int	node_type;
	float	charge_rate;	/* queue charge rate, default 1 */
	long	cpu_time;	/* used node-time */
	long	under_used;	/* partially idled node-time */
	long	idle_time;	/* whole partition idle time */
	long	last_update;	/* time stamp of last info from SM or NQS */
	char	queue_name[16]; /* nqs queue name */
	int	match;		/* flag for matching */
	struct pg_ent *pg_list; /* pointer to a list of pg_ent */
	int	npg;		/* number of pg_ent in list */
	struct part_ent *next;	/* pointer to next partition */
};

struct user_ent {		/* user account */
	int	uid;		/* user id, defined in /etc/passwd */
	int	npart;		/* number of part_ent in list */
	int	nnodes;		/* number of nodes used currently */
	float	node_rate;	/* number of active nodes * charge rate */
	long	cpu_time;	/* cpu_time used by current active apps. */
	struct part_ent *part_list;/* pointer to a list of partitions */
	struct user_ent *next;	/* pointer to next user account */
	char	req_id[ 16 ];   /* SMD alarm request ID */
};

struct acct_ent {		/* account */
	int 	acct_id;	/* account id, defined in /etc/account */
	int	nuser;		/* number of user_ent in list */
	int	nnodes;		/* number of nodes used currently */
	float	node_rate;	/* number of active nodes * charge rate */
	long	cpu_time;	/* cpu_time used by current active apps. */
	struct user_ent *user_list;/* pointer to a list of users */
	struct acct_ent *next;	/* pointer to next user account */
	char	req_id[ 16 ];   /* SMD alarm request ID */
};

struct app_ref {		/* ref. pointers to relevant data structures */
	struct acct_ent *acct_ptr;/* current acct_ent */
	struct acct_ent *prev_acct;/* acct_ent ahead of the current in list */
	struct user_ent *user_ptr;/* current user_ent */
	struct user_ent *prev_user;/* user_ent ahead of the current in list */
	struct part_ent *part_ptr;/* current part_ent */
	struct part_ent *prev_part;/* part_ent ahead of the current in list */
	struct pg_ent *pg_ptr;	/* current pg_ent */
	struct pg_ent *prev_pg; /* pg_ent ahead of the current in list */
};

#define USER_ALARM	0
#define ACCT_ALARM	1
