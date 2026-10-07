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
 * mac.h
 *
 *   Interface into library call for MACS 
 *	mac_list()
 *	mac_modify()
 *	mac_transfer()
 *	mac_create()
 *	mac_delacct()
 *	mac_deluser()
 *	mac_update()
 *	mac_balance()
 *	mac_runok()
 * 	mac_send()
 */

#ifndef MAC_H
#define MAC_H

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

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
#define SCHED_DOWN	((CPUCTRL<<8)|0x1A)
#define SI_OFF		((CPUCTRL<<8)|0x1B)
#define C_UPD_HOLD	((CPUCTRL<<8)|0x1C)
#define C_UPD_RESUME	((CPUCTRL<<8)|0x1D)
#define C_BOOT		((CPUCTRL<<8)|0x1E)
#define C_ISMACWATCH	((CPUCTRL<<8)|0x1F)
#define MAX_REQNUM	((CPUCTRL<<8)|0x1F)


struct	cpu_lblk	{
#ifdef SDSC
 	unsigned	id:24;		/* uid or agid */
#else
 	uid_t		id;		/* uid or agid */
#endif
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
	int		maxnodes;	/* maximum number of allowed nodes */
	double		authorized;	/* total time authorized */
	double		used_time;	/* total time used */
	double		sbu_time;	/* total time used (weighted)*/
	time_t 		timestamp;	/* time last updated */
} ;

struct	cpu_ctrl	{
#ifdef SDSC
	unsigned	uid:24;		/* requestors real uid */
	unsigned	agid:24;	/* requestors agid */
#else
	uid_t		uid;		/* requestors real uid */
	gid_t		agid;		/* requestors agid */
#endif
	struct cpu_lblk info;		/* data block to be used by command */
	short seq;			/* a seq number (rffu) */
};

/* error code for return request C_VALID */
#define	INVALID_ACCT	-1
#define NOT_ALLOWED	-2
#define	OVER_ALLOC	-3
#define	OVER_NODES	-4

/* special error code for return requests */
#define EACCTONLY	-10

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
#ifndef INTELv1r1
        long    submit_time;    /* job submision time in seconds */
        long    requested_time; /* job requested execution time */
#endif
};

#ifndef INTELv1r1
struct job_charge {
	long	cpu_time;
	float	cpu_rate;
	long	under_used;
	float	under_rate;
	long	idle_time;
	float	idle_rate;
};
#endif


/*
 * definition for si_info.flag
 */

#define SI_NONE		0
#define SI_START	1
#define SI_END		2

/* definition for type of interrupts
 * 0 is the default
 */
#define MAX_ITYPE	9
#define CRASH		0		
#define HANG		1
#define MAINT		2
#define SOFT		3
#define HARD		4
#define TEST		5
#define EXTERN		6
#define OTHER		7
#define UNKNOWN         8

#define UNKNOWN_STRING "unknown"

static char *stype[MAX_ITYPE] =
        { "crash", "hangs", "maint", "softw", "hardw", "testi", "exter", "other", UNKNOWN_STRING };

struct si_info {
	int	flag;		/* scheduled interrupt on, off, none */
	int	type;		/* type of interrupt */
	char	comment[64];	/* comments */
};

#endif
