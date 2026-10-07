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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/schedule.h,v 1.22 1994/12/19 16:05:58 johannes Exp $
 *
 */


/*
 * schedule.h	- Don Cameron	3/92
 *
 * This file contains the exported interface to the scheduling module of
 * the allocator server.
 */
#ifndef	_schedule_h
#define	_schedule_h

#include <sys/types.h>
#include <sys/time.h>
#if 0
#include <sys/stdtypes.h>
#endif
#include <nx/hash.h>
#include <nx/bitmap.h>
#include <nx/allocator.h>
 
/*
 * Define pseudo gang scheduling signals
 */
#ifdef SIGNAL_BUG
#define	SIGGSTOP	0
#define	SIGGSTART	0
#else
#define	SIGGSTOP	SIGUSR2
#define	SIGGSTART	SIGCONT
#endif

/*
 * The values below are placed in the "type" field to identify a structure
 * as being either an application or a partition.
 */
#define	APPL		601
#define PART		602


/* Status of loaded applications */
#define	APPL_NOT_STARTED	0
#define APPL_GANG_START		1
#define APPL_GANG_STOP		2

#ifdef PARACORE
#define	APPL_NOT_DUMPING_CORE	0
#define APPL_DUMPING_CORE	1
#endif /* PARACORE */

#define PART_ROLLED_OUT		0
#define PART_ROLLED_IN		1

/* This are used to key some general purpose routines to what kind of tile
 * they are operating on
*/
#define ALLOC_TILE		0
#define SCHED_TILE		1		
/*
 * Structures defined below have recursive references so we will put the
 * typedefs first.
 */
typedef struct consumer		CONSUMER_T;
typedef struct layer		LAYER_T;
typedef struct appl		APPL_T;
typedef struct part		PART_T;
typedef struct conflict_list_s	conflict_list_t;

#define CONFLICT_ALLOC_QUANTUM	5

struct conflict_list_s { 
        CONSUMER_T 	**list;
	int		list_size;
	int		n_list;
	int		free_index;
}; 

/*
 * Generic description of consumer structure which could be either application
 * or partition. first two fields match generic linked list structure and
 * third is used to differentiate APPL_T from PART_T.
 */
struct consumer { 
        struct consumer	*prev;
        struct consumer	*next; 
        int		type; 
}; 

/*
 * Structure that describes a layer
 */
struct layer { 
        struct layer	*prev;
        struct layer	*next;
        PART_T		*part;		/* Partition in which layer resides */
        BITMAP_T	*bitmap;	/* Bitmap of nodes free in layer */
        int             priority;	/* Highest priority consumer */
        int             num_free;	/* Number of nodes free in layer */
        CONSUMER_T	*consumer;	/* List of partitions and applications
					   in layer */
	struct layer	*sched_prev;	/* Pointer to previous layer to schedule, used
					 * for round robin scheduling.
					*/
	struct layer	*sched_next;	/* Pointer to next layer to schedule, used
					 * for round robin scheduling 
					*/
}; 

/*
 * Structure that describes an application
 */
struct appl {
	CONSUMER_T	*prev;
	CONSUMER_T	*next;
	int		type;
	conflict_list_t	conflict;	/* conflict list */

	LAYER_T		*parent_lyr;	/* Scheduling layer which contains 
					 * application 
					*/
	BITMAP_T	*bitmap;	/* Bitmap of nodes used by appl */
	LP_MAP_T	lp;		/* Logical to physical node map */
	int		size;		/* Number of nodes in application */
	int		nrows;		/* X dimension of application, 1 if
					 * nodes are not contiguous
					*/
	int		ncols;		/* Y dimension of application, set to size
					 * if nodes are not contiguous
					*/
	int		priority;	/* Priority of application */
	unsigned long	rolled_in;	/* Milliseconds this appl rolled in */
	unsigned long	elapsed;	/* Milliseconds this appl rolled in */
	pid_t		pgroup;		/* Process group of application */
	uid_t		uid;		/* UID of user running application */
	gid_t		gid;		/* GID of user running application */
	gid_t		nx_acctid;	/* NX account id (MACS) of application */
	int		appl_stat;	/* Indicates the appl is started */
	time_t		start_time;	/* Time stamp of when 
					 * application started 
					*/
#ifdef PARACORE
	int		appl_core;	/* Indicates the appl is dumping core*/
#endif /* PARACORE */
};

/*
 * Structure that describes a partition
 */
struct part {
	CONSUMER_T	*prev;
	CONSUMER_T	*next;
	int		type;
	PART_T		*alloc_prev;	/* previous element in allocation layer 
					 * consumer list
					*/
	PART_T		*alloc_next;	/* next element in allocation layer 
					 * consumer list
					*/
	conflict_list_t	conflict;	/* conflict list */
	LAYER_T		*parent_alloc_lyr;/* Allocation layer which contains 
					   * partition 
					  */
	LAYER_T		*parent_sched_lyr;/* Scheduling layer which contains 
					 * partition 
					*/
	BITMAP_T	*bitmap;	/* Bitmap of nodes used by partition */
	LP_MAP_T	lp;		/* Logical to physical node map */
/*
 * Note: The two fields below should be the same in all cases except
 *       the root partition. For the root partition we number across
 *       empty slots.
 */
	int		slots;		/* Number of slots in partition */
	int		nodes;		/* Number of nodes in partition */
	int		rows;		/* Number of rows if partition is a rectangle
					 * -1 if it is not
					*/
	int		cols;		/* Number of cols if partition is a rectangle
					 * -1 is it is not
					*/
	int		cur_priority;	/* Highest priority object in part */
	int		max_priority;	/* Maximum exported priority */
	ino_t		inode;		/* Inode which uniquely IDs partition */
	int		sched;		/* Scheduling policy: GANG or UNIX */
	unsigned long	rollin_quantum;	/* Rollin quantum in milliseconds */
	unsigned long	rolled_in;	/* Milliseconds this part rolled in */
	unsigned long	elapsed;	/* Milliseconds this partition has been active */
	uid_t		owner;		/* Owner of partition */
	gid_t		group;		/* Group of owner of partition */
	int		protection;	/* Chmod-style protection of part */
	LAYER_T		*active_lyr;	/* Layer which is currently active */
	LAYER_T		*child_sched_lyr;/* List of scheduling layers of 
					 * this partition 
					*/
	LAYER_T		*child_alloc_lyr;/* List of allocation layers of this 
					  * partition 
					 */
	LAYER_T		*sched_list;	/* List of scheduling layers used for round robin
					 * selection
					*/
	time_t		start_time;
	int		status;		 /* PART_ROLLED_IN or PART_ROLLED_OUT */
	int		max_num_layers;	/* Maximum number of layers, from config file */
	int		gang_ancestor;  /* 1 if one of our ancestors is gang scheduled,
					 * 0 otherwise
					*/
};

/*
 * Exported functions.
 */
void rollin();
void rollout();
void schedule();
void select_next_layer();
unsigned long find_smallest_rollin_quantum();

/*
 * Exported global variables
 */

/*
 * Pgroup of the newly started application.
 */
extern pid_t	new_appl;

#endif	/* !_schedule_h */
