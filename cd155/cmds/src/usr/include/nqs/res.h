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
 * res.h
 *
 *   Interface into library call for resource management system
 *	res_list()
 *	res_modify()
 *	res_transfer()
 *	res_create()
 *	res_delacct()
 *	res_deluser()
 *	res_update()
 *	res_balance()
 *	res_runok()
 * 	res_send()
 */
/*
 * HISTORY
 * $Log: res.h,v $
 * Revision 1.4  1994/11/19  02:44:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  00:06:36  mwan
 * R1.2 mods.
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 *
 */

#define TRUE   1
#define FALSE  0
#define RESD_PORT (2001)        /* port address for the resource daemon */

#define	CPUCTRL		'q'

#define	C_OPEN		((CPUCTRL<<8)|0x1)
#define	C_CLOSE		((CPUCTRL<<8)|0x2)
#define	C_GETLIMIT	((CPUCTRL<<8)|0x3)
#define	C_GETACCT	((CPUCTRL<<8)|0x4)
#define	C_VALID		((CPUCTRL<<8)|0x5)
/* must be euid == 0 for following */
#define	C_OFF		((CPUCTRL<<8)|0x10)
#define	C_ON_COUNT	((CPUCTRL<<8)|0x11)
#define	C_SYNC		((CPUCTRL<<8)|0x12)
#define	C_SETACCT	((CPUCTRL<<8)|0x13)
#define	C_SETLIMIT	((CPUCTRL<<8)|0x14)
#define	C_CREACCT	((CPUCTRL<<8)|0x15)
#define	C_DELLIMIT	((CPUCTRL<<8)|0x16)
#define	C_DELACCT	((CPUCTRL<<8)|0x17)
#define	NQS_JOB_START	((CPUCTRL<<8)|0x18)
#define	NQS_JOB_END	((CPUCTRL<<8)|0x19)
#define SI_ON		((CPUCTRL<<8)|0x1A)
#define SI_OFF		((CPUCTRL<<8)|0x1B)
#define MAX_REQNUM	((CPUCTRL<<8)|0x1B)


struct	cpu_lblk	{
 	uid_t           id;		/* uid or agid */
	unsigned	weight:1;	/* weight time by number of nodes allocated */
	unsigned	inhibit:1;	/* account has exceeded cputime */
	unsigned	modify:1;	/* user may modify his allocations */
	unsigned	transfer:1;	/* user may transfer time to another agid*/
	unsigned	use:1;		/* user may NOT use time */
	unsigned	unlimit:1;	/* account with unlimited allocation */
	unsigned	killjobs:1;	/* if set kill jobs - dont try chkpnt */
	unsigned	lockjobs:1;	/* dont allow setting of killjobs bit*/
	unsigned	unused2:16;	/* some more unused space in structure */
	unsigned	percent:16;	/* percentage of time allocated to user */
	unsigned	maxnodes;	/* maximum number of allowed nodes */
	double		authorized;	/* total time authorized */
	double		used_time;	/* total time used */
	double		sbu_time;	/* total time used (weighted)*/
	time_t 		timestamp;	/* time last updated */
} ;

struct	cpu_ctrl	{
        	uid_t    uid;   /* requestors real uid */
        	gid_t   agid;   /* requestors agid */
		struct cpu_lblk info;	/* data block to be used by command */
		short seq;		/* a seq number (rffu) */
};

/* error code for return request C_VALID */
#define	INVALID_ACCT	-1
#define NOT_ALLOWED	-2
#define	OVER_ALLOC	-3
#define	OVER_NODES	-4

/* bit mask for permissions */
#define MODIFY_MASK     (1<<2)
#define TRANSFER_MASK   (1<<1)
#define USE_MASK        1


struct nqs_job_info {
	int	uid;
	int	acct_id;
	int	part_id;
	int	part_size;
	int	node_type;
	time_t	event_time;	/* depend on types: NQS_JOB_START, ... */
	long	cpu_time;	/* seconds * number-0f-nodes */
	long	part_active;	/* 1 of more nodes in use in partition */
	long	part_idle;	/* 0 nodes in use in partition */
	char queue_name[24];
        long    submit_time;    /* job submision time in seconds */
        long    requested_time; /* job requested execution time */
};

struct job_charge {
        long    cpu_time;
        float   cpu_rate;
        long    under_used;
        float   under_rate;
        long    idle_time;
        float   idle_rate;
};


extern int res_list();
extern int res_modify();
extern int res_transfer();
extern int res_create();
extern int res_delacct();
extern int res_deluser();
extern int res_update();
extern int res_balance();
extern int res_runok();
extern int res_send();
