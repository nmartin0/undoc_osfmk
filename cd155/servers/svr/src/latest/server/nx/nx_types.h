/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * HISTORY
 * $Log: nx_types.h,v $
 * Revision 1.12  1994/11/18  20:37:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/06/29  09:32:09  johannes
 * changed type of 'start_time' in 'nx_app_info'
 * from 'struct tm' to 'time_t' for user and server also
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 10007
 *  Testing: calling libnx function nx_app_attr()
 *  Module(s):	svr/server/nx/nx_types.h
 * 		svr/server/nx/nx_types.defs
 * 		cmds_libs/src/usr/include/allocsys.h
 *
 * Revision 1.10  1994/06/29  09:27:47  johannes
 * undo last commit done without log message
 *
 * Revision 1.9  1994/06/29  09:11:56  johannes
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.8  1994/06/01  20:35:33  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): server/sys/errno.h, server/sys/version.h server/nx/nx.c
 * 	    server/nx/nx.defs server/nx/nx_svr.defs server/nx/nx_types.defs,
 * 	    server/nx/nx_types.h, server/nx/nx_create_attr.c (new)
 * 	    emulator/i860/emul_machdep.c
 *  Related: libnx, allocator, bootmesh, mkpart, showpart, lspart
 *
 * Revision 1.7  1994/03/14  02:02:49  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.6  1993/11/17  02:30:56  carbajal
 * R1.2 User Model Support
 *  Reviewer: cfj
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.5  1993/10/27  01:27:51  carbajal
 * Added NX_INIT_T structure
 *
 * Revision 1.4  1993/07/18  19:41:20  carbajal
 * chpart system call stuff
 *
 * Revision 1.3  1993/07/14  21:13:13  carbajal
 * Updated to match the allocator definitions
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

#ifndef _NX_TYPES_H_
#define _NX_TYPES_H_

#include <sys/types.h>

typedef struct {
        int rows;
        int cols;
} RECT_T;


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
	int		relaxed;	/* relaxed allocation requested */
} PARTREQ_T;

typedef unsigned long	COLUMN_MAP_T;	/* bitmap of one column */

typedef struct {
        int             rows;           /* Number of rows */
        int             cols;           /* Number of columns */
        int             row_offset;     /* Starting row of bitmap */
        COLUMN_MAP_T    colmap[1];      /* Variable length array of column
                                           bitmaps */
} BITMAP_T;

typedef char *MIG_BITMAP_T;
#ifdef CHKPNT
typedef char line_t[4096];
#endif /* CHKPNT */
typedef	u_char	*smd_iface_t;
typedef ino_t	*inode_list_t;
typedef char	*string;

/*
 * Structure passed to allocator during nx_init() call
 *
*/

typedef struct {
	ino_t		inode;
        int             size;           /* Number of nodes  */
        int             priority;       /* Priority of job */
        unsigned long   rows;           /* Number of rows */
        unsigned long   cols;           /* Number of columns */
        unsigned long   start_node;     /* Starting node */
#ifdef	CHKPNT
        unsigned int    ck_interval;	/*application's checkpoint interval */
        unsigned int    no_restart;	/* whether to restart application */
        char    	*cmd_line;	/* Command line used to initiate application */
        unsigned int cmd_line_count;	/* Size of the command line string */
#endif	/* CHKPNT */
        int             unused;		/* */
	int		reserved[10];	/* reserved */
} NX_INIT_T;

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


#endif  _NX_TYPES_H_

