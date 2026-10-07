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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/allocator.h,v 1.19 1994/11/19 02:44:36 mtm Exp $
 *
 */

/* History
 * $Log: allocator.h,v $
 * Revision 1.19  1994/11/19  02:44:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.18  1994/06/21  23:57:18  sdh
 *  Removed an extra open comment that caused lint errors
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.17  1994/06/01  20:27:51  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): nx.h, fnx.h, allocsys.h, nx/alloc_types.h, nx/allocator.h
 * 	    nx/bitmap.h, nx/hash.h, nx/partprint.h, nx/partutils.h,
 * 	    nx/schedule.h
 *  Related: server, emulator, allocator, bootmesh, mkpart, showpart, lspart
 *
 * Revision 1.16  1994/01/25  21:56:32  carbajal
 *  7886 PARAGON      OPEN      H        carbajal  gregt               R1.2 WW03
 *       MESH UTILS   20-JAN-94 1        19-JAN-94 19-JAN-94
 *       nx_pspart() constants NX_PARTITION, NX_APPLICATION not defined.
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: 7886
 *  Testing: rmcall, rmcmd, sched EATs
 *  Module(s): allocsys.h, fnx.h, allocator.h
 *
 * Revision 1.15.2.1  1994/01/25  18:20:41  carbajal
 *  7886 PARAGON      OPEN      H        carbajal  gregt               R1.2 WW03
 *       MESH UTILS   20-JAN-94 1        19-JAN-94 19-JAN-94
 *       nx_pspart() constants NX_PARTITION, NX_APPLICATION not defined.
 *
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: 7886
 *  Testing: rmcall, rmcmd, sched EATs
 *  Module(s): allocator.h, allocsys.h, fnx.h
 *
 * Revision 1.15  1993/12/01  19:58:45  carbajal
 * Ifdefed out APPREQ_T this is now nx_pspart_t in allocsys.h
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.14  1993/12/01  01:26:09  carbajal
 * changed time_start from struct tm to time_t
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.13  1993/11/22  17:44:23  carbajal
 * Protect double declared types with #ifdefs
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Cleaner compiles
 *  Testing:
 *  Module(s):
 *
 * Revision 1.12  1993/11/17  02:48:26  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.11  1993/01/28  19:47:10  carbajal
 * Removed multiply defined LP_MAP_T and LP_MAP_ENTRY. These
 * are defined in mcmsg_appl.h
 *
 * Revision 1.10  1993/01/18  19:35:14  joel
 * ifdef around conflicting type declarations in usr/ccs/lib/libnx/nx_initve.c
 *
 * Revision 1.9  1992/12/18  02:17:16  carbajal
 * path type structures
 *
 * Revision 1.8  1992/11/07  01:21:23  carbajal
 * Moved NX_DFLT_PART from here to defines.h
 *
 * Revision 1.7  1992/10/22  01:16:24  carbajal
 * Added NMFLG for chpart call
 *
 * Revision 1.6  1992/10/13  00:38:54  carbajal
 * Added a const NX_DFLT_PART to define the environment
 * variable that hold the default partition.
 *
 * Revision 1.5  1992/10/08  19:32:09  carbajal
 * Added a flags var to PARTREQ_T for chpart
 * Added a new struct APPREQ_T for pspart
 *
*/

/*
 * allocator.h	- Don Cameron	3/92
 *
 * This file contains the exported interface to the allocator server.
 */
#ifndef	_ALLOCATOR_H
#define	_ALLOCATOR_H
#include <sys/types.h>
#include <sys/time.h>

/*
 * Values below used in "sched" field of partition to specify scheduling policy. */
#define SPACE_SHARE	2	/* Space shared partition */
#define	GANG		1	/* Gang Scheduled */
#define UNIX		0	/* Unix scheduled */

/* Flag settings for indicating exactly what chpart wants to do */
#define	RQFLG		1	/* Set rollin quantum */
#define	EPLFLG		2	/* Set effective priority level */
#define	MODFLG		4	/* Set access mode */
#define	OWNERFLG	8	/* Set owner */
#define NMFLG		16	/* Change the partition name */
#define SPSFLG  	32

#ifndef _RECT_T_H_
#define _RECT_T_H_
/*
 * Structure describing rectangle to allocate.
 */
typedef struct {
	int rows;
	int cols;
} RECT_T;
#endif 

#ifndef _PARTREQ_T_H_
#define _PARTREQ_T_H_
/*
 * Structure passed to allocator when allocating partitions.
 */
typedef struct {
	uid_t 		uid;		/* User Id */
	gid_t 		gid;		/* Group Id */
	int		access;		/* Access Permissions */
	int		sched;		/* Unix scheduling vs Gang Scheduling */
	unsigned long	rq; 		/* Rollin Quantum */
	int		maxpri;		/* Maximum Priority */
	ino_t		inode;		/* Inode number of the partition */
	ino_t		parent_inode;	/* Inode number of parent partition */
	RECT_T		rect;		/* Rectangle to allocate */
/*
 * Note: The two fields below should be the same in all cases except
 *       the root partition. For the root partition we number across
 *       empty slots.
 */
        int		slots;		/* Number of slots in the partition */
        int		nodes;		/* Number of nodes in the partition */
	int		flags;		/* flags for chpart call */
	int		free;		/* number of unused nodes in the
					 * partition
					*/
	int		relaxed;	/* relaxed allocator requested */
} PARTREQ_T;
#endif 


#if 0
/*
 * Structure passed to allocator when running pspart
*/
typedef struct {
	int		object_type;	/* object type, 
					 * NX_APPLICATION
					 * NX_PARTITION
					*/
	int		object_id;	/* process group id, or partition id */ 
	uid_t		uid;		/* user id */
	gid_t		gid;		/* group id */
	int		size;		/* number of nodes used */
	int		priority;	/* priority of process */
	unsigned long	rolled_in;	/* time rolled in so far */
	unsigned long	rollin_q;	/* rollin quantum for partition */
	unsigned long	elapsed;	/* elapsed time */
	int		active;		/* 1 == active
					 * 0 == not active
					*/
	time_t  	time_started;	/* Wall clock time started */
} APPREQ_T;
#endif


/* structure to support the building of partition pathnames */

typedef struct pathstruct {
        int     maxspace;               /* keep track of how much TOTAL
                                         * space we have in space
                                        */
        char    *space;                 /* our path string */
} PATHTYPE;;

/* Allocator configuration structure
 *
 *
*/

typedef struct {
	/* Space share the whole machine */
	int	space_share;	

	/* Max number of gang scheduled partition allowed in the
	 * machine. If set to -1 then there is no limit. A standard
	 * scheduled partition is not included in this count.
	*/

	int	num_of_gang_parts;	

	/* Max number of layers allowed per partition. If set to 
	 * -1 then there is no limit.
	*/
	int	degree_of_overlap;	

	/* The minimum allowable rollin quantum that is > 0. */

	unsigned long	min_rq;

	/* Allow the use of -plk if there is a gang scheduled partition
	 * in your parent hierarchy. 
 	 *  If set to 1 then -plk is ok, otherwise -plk will be 
	 * rejected.
	*/
	int	reject_plk;		

	/* Use MACS if == 1, otherwise ignore MACS */
	int	use_macs;

} alloc_config_t;

/*
 * Exported functions.
 */

/*
 * Exported globals
 */

#endif	/* !_ALLOCATOR_H */

