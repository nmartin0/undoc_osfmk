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
 
/* Copyright 1992, Intel Corporation */
/*
 * All rights reserved.
 */
/*
 * HISTORY
 * $Log: alloc_types.h,v $
 * Revision 1.14  1994/11/19  02:44:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/10/14  23:10:57  sdh
 * Added nx_acctid in place of one of the reserved ints in
 * nx_app_info_t.
 *
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 11097
 *  Testing:
 * 	EATS: controlc, rmcall, rmcmd
 * 	manual tests
 *  Module(s):
 * 	init_appl.c (allocator)
 * 	nx_initve.c (libnx)
 * 	macs.c (allocator)
 * 	alloc_types.h
 *
 * Revision 1.12  1994/06/01  20:27:40  mag
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
 * Revision 1.11  1993/12/01  01:26:20  carbajal
 * changed time_start from struct tm to time_t
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.10  1993/11/22  17:44:35  carbajal
 * Protect double declared types with #ifdefs
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Cleaner compiles
 *  Testing:
 *  Module(s):
 *
 * Revision 1.9  1993/11/17  02:48:14  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.8  1993/10/27  01:39:00  carbajal
 * Added NX_INIT_T structure
 *
 * Revision 1.7  1993/07/13  21:07:38  carbajal
 * Added string typedef
 *
 * Revision 1.6  1993/05/06  18:26:38  carbajal
 * Added inode_list typedef
 *
 * Revision 1.5  1993/01/28  19:47:11  carbajal
 * Removed multiply defined LP_MAP_T and LP_MAP_ENTRY. These
 * are defined in mcmsg_appl.h
 *
 * Revision 1.4  1992/10/08  22:49:40  carbajal
 * Add flags to PARTREQ_T for chpart call
 *
 * Revision 1.3  1992/10/08  22:42:51  rkl
 * Added smd_iface_t for SMD interface.
 *
 * Revision 1.2  1992/10/06  00:18:27  cameron
 * Support for empty slots in root partition.
 *
 * Revision 1.1  92/07/30  16:27:39  shala
 * Initial revision
 * 
 * Revision 1.1  92/03/31  17:16:50  cfj
 * Initial revision
 *
 *
*/

#ifndef _ALLOC_TYPES_H_
#define _ALLOC_TYPES_H_

#include <sys/types.h>
#include <sys/time.h>

#ifndef _RECT_T_H_
#define _RECT_T_H_
typedef struct {
        int rows;
        int cols;
} RECT_T;
#endif _RECT_T_H_


#ifndef _PARTREQ_T_H_
#define _PARTREQ_T_H_
typedef struct {
        uid_t           uid;            /* User Id */
        uid_t           gid;            /* Group Id */
        int             access;         /* Access Permissions */
        int             sched;          /* Unix scheduling vs Gang Scheduling */
        unsigned long   rq;             /* Rollin Quantum */
        int             maxpri;         /* Maximum Priority */
        ino_t           inode;          /* Inode number of the partition */
        ino_t           parent_inode;   /* Inode number of parent partition */
        RECT_T          rect;           /* Rectangle to allocate */
/* 
 * Note: The two fields below should be the same in all cases except 
 *       the root partition. For the root partition we number across 
 *       empty slots.    
 */
	int		slots;		/* Number of slots in the partition */
        int		nodes;		/* Number of nodes in the partition */
	int		flags;		/* flags for chpart call */
	int		free;		/* Number of free nodes */
	int		relaxed;	/* Relaxed allocation requested */
} PARTREQ_T;
#endif


#ifndef _COLUMN_MAP_T_H_
#define _COLUMN_MAP_T_H_
typedef unsigned long	COLUMN_MAP_T;	/* bitmap of one column */
#endif

#ifndef _BITMAP_T_H_
#define _BITMAP_T_H_
typedef struct {
        int             rows;           /* Number of rows */
        int             cols;           /* Number of columns */
        int             row_offset;     /* Starting row of bitmap */
        COLUMN_MAP_T    colmap[1];      /* Variable length array of column
                                           bitmaps */
} BITMAP_T;
#endif

typedef char *MIG_BITMAP_T;
typedef	u_char	*smd_iface_t;
typedef ino_t	*inode_list_t;
typedef char	*string;

/* 
 * Structure passed to allocator during nx_init() call
 *
*/

typedef struct {
	ino_t		inode;		/* Inode of partition */
	int		size;		/* Number of nodes  */
	int		priority;	/* Priority of job */
	unsigned long	rows;		/* Number of rows */
	unsigned long	cols;		/* Number of columns */
	unsigned long	start_node;	/* Starting node */		
	int		plk; 	 	/* ==1 if -plk was specified on command line
					 * == 0 otherwise
					*/
	int		relaxed;	/* Relaxed allocation requested */
        int             macs_account;   /* MACS account to use for request */
        int             reserved2[8];   /* reserved */
} NX_INIT_T;

#ifndef _NX_APPINFO_T
#define _NX_APPINFO_T

typedef struct {
        int             size;           /* Number of nodes in application */
        int             nrows;          /* X dimension of application, 1 if
                                         * nodes are not contiguous
                                        */
        int             ncols;          /* Y dimension of application,
                                         * set to size
                                         * if nodes are not contiguous
                                        */
        int             priority;       /* Priority of application */
        unsigned long   rolled_in;      /* Milliseconds this appl rolled in */
        unsigned long   elapsed;        /* Milliseconds this appl rolled in */
        uid_t           uid;            /* UID of user running application */
        gid_t           nx_acctid;      /* NX account id (MACS)
                                         * of application */
        time_t          start_time;     /* Time stamp of when
                                         * application started
                                        */
} nx_app_info_t;

#endif _NX_APPINFO_T


#endif  _ALLOC_TYPES_H_

