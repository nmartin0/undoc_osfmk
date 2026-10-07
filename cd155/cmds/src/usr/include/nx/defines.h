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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/defines.h,v 1.26 1994/11/19 02:44:40 mtm Exp $
 *
 */

/* History
 * $Log: defines.h,v $
 * Revision 1.26  1994/11/19  02:44:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.25  1993/08/28  00:54:42  carbajal
 * Added DAEMON_ID's for PTS #6141
 *
 * Revision 1.24  1993/07/18  19:43:58  carbajal
 * added NO_ID_SPECIFIED
 *
 * Revision 1.23  1993/07/14  01:40:04  carbajal
 * Added constants for allocator syscall support
 *
 * Revision 1.22  1993/07/07  23:00:14  carbajal
 * Removed MAXPART, MAXNODES, MAXCOLS
 *
 * Revision 1.21  1993/05/26  00:11:42  carbajal
 * More cleanup of history log
 *
 * Revision 1.20  1993/05/26  00:00:08  carbajal
 * Cleaned up comments in history log
 *
 * Revision 1.19  1993/05/25  23:59:25  carbajal
 * Moved
 * define SIZE    1       
 * define RECT    2       
 * define EXACT   3       
 * from allocator.c to here
 *
 * Revision 1.18  1993/05/06  18:18:48  carbajal
 * Added MIN_ALLOWED_RQ
 *
 * Revision 1.17  1993/04/02  23:25:17  carbajal
 * Added NO_PRI constant to indicate a null priority value
 *
 * Revision 1.16  1993/03/31  01:35:08  carbajal
 * Added definition for STD_PRI priority value for STD scheduled partitions
 *
 * Revision 1.15  1993/01/19  19:42:51  carbajal
 * Added define for DFLT_PRI
 *
 * Revision 1.14  1992/12/18  02:19:29  carbajal
 * no changes
 *
 * Revision 1.13  1992/12/11  02:08:03  carbajal
 * Missing post afs crash - rechecked in.
 *
 *
 * Revision 1.12  1992/11/30  21:45:33  carbajal
 * Added constant NX_DFLT_SIZE
 *
 * Revision 1.11  1992/11/07  01:21:41  carbajal
 * Moved NX_DFLT_PART from allocator.h to here
 *
 * Revision 1.10  1992/11/03  00:23:03  carbajal
 * Added constant for ".partinfo" string
 *
 * Revision 1.9  1992/10/22  01:17:40  carbajal
 * Added OWNER as an access code
 * Added CHECK_AGAINST_OWNER as a access modifier
 * Added IAM_ROOT for checking access
 *
 * Revision 1.8  1992/10/19  17:34:37  carbajal
 * Changed RECTSET from 0xff to 0x7fff to avoid any
 * possible conflict between using node 255 in a rectangular
 * specification.
 *
*/

/*
 *  Global variables and defines for cube partitioning
 */
#ifndef _DEFINES_H
#define _DEFINES_H

/*
 * Values used to indicate how the nodes are specified.
 */
#define SIZE    1       /* Only number of nodes is specified */
#define RECT    2       /* Nodes are specified as row x col rectangle */
#define EXACT   3       /* Exact set of nodes is specified in node list */

#define SIZENOTSET  -2
#define SIZEANY     -1

#define RECTCOLSNOTSET  -2
#define RECTROWSNOTSET  -2
#define BITCOLSNOTSET   -2
#define BITROWSNOTSET   -2
#define NDLISTNOTSET    -2

#define NAMELEN      256 
#define USAGE_EXIT     1  
#define USAGE_ERROR    -1  
#define SIZEANY       -1
#define RECTSET       0x7fff

/* Partition access permissions */
#define READ          1
#define WRITE         2
#define EXEC          3
#define OWNER	      4			/* indicates that we must own the 
					 * partition in order to operate on it
					*/
#define CHECK_AGAINST_PARENT	0x8000  /* indicates that we need to verify the
					 * access mode against the parent partition
					*/	

#define IAM_ROOT	0		  /* Root user id */

#define SPECIAL		{ '\007', '\0' }  /* nx_loadve inherit ptype flag */
#define SPECIAL2	{ '\006', '\0' }  /* exec() inherit ptype flag */


/*
 * Maximum and minimum priorities
 */
#define	MAXPRI	10
#define	MINPRI	0
#define DFLT_PRI	5
/* Priority for STD scheduled partitions */
#define STD_PRI	2*MAXPRI
/* No priority value for empty partitions */
#define NO_PRI	-1
 
/* Inherit values for partition attributes */
/* Inherit priority from parent */
#define NO_EPL_SPECIFIED        -1
/* Inherent scheduling from parent */
#define NO_SCHED_SPECIFIED      -1
/* Inherit RQ from parent */
#define NO_RQ_SPECIFIED         2 * MAXRQ
/* Inherit access from parent */
#define NO_ACCESS_SPECIFIED     -1
/* Inherit number of nodes from parent */
#define NO_NODES_SPECIFIED     -1
/* Inherit value for uid and gid */
#define NO_ID_SPECIFIED		-1


/*
 * Maximum rollin quantum: 24 hours in milliseconds.
 */
#define	MAXRQ		86400000	/* 24*60*60*1000 */
#define MINRQ		0

#ifdef UP_MIN_RQ
#define MIN_ALLOWED_RQ	(5 * 60 * 1000) /* 5 minutes in milliseconds */
#else
#define MIN_ALLOWED_RQ	100		/* 100 ms */
#endif UP_MIN_RQ

/* These are used by the internal mkpart sys calls to flag what action
 * needs to be taken
*/
#define MKPART_ND	0
#define MKPART_SZ	1


/* Daemon ids */
#define	SMD_DAEMON_ID	0
#define MACS_DAEMON_ID	1


#ifdef	DBGMALLOC
void *__dbgmalloc;
#define MALLOC(c)	(__dbgmalloc = malloc((c)), \
			 printf("m: 0x%x\t%d\n", __dbgmalloc, (c)), \
			 __dbgmalloc)

#define FREE(p)		(printf("f: 0x%x\n", (p)), free((p)))
#else
#define	MALLOC(c)	malloc(c)
#define	FREE(p)		free(p)
#endif	DBGMALLOC

/*
 * Signal masking macros to protect critical sections
 */
#include <signal.h>
static sigset_t	csect_new;	/* new signal mask */			
static sigset_t	csect_old;	/* old signal mask */			

#define	CSECT_ENTER	sigemptyset(&csect_new);			\
			sigaddset(&csect_new, SIGINT);			\
			sigaddset(&csect_new, SIGHUP);			\
			sigprocmask(SIG_SETMASK, &csect_new, &csect_old);

#define	CSECT_EXIT	sigprocmask(SIG_SETMASK, &csect_old, (void *) 0);

#define ROOT_PART_PATH	"/etc/nx"
#define	PARTINFO_STR	"/.partinfo"
/* Environment string for the default partition */
#define NX_DFLT_PART	"NX_DFLT_PART"
#define NX_DFLT_SIZE	"NX_DFLT_SIZE"

#endif	/*_DEFINES_H */

