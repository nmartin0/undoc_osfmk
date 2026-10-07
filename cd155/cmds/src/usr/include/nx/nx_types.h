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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/nx_types.h,v 1.3 1994/11/19 02:44:48 mtm Exp $
 *
 */


/* Copyright 1992, Intel Corporation */
/*
 * All rights reserved.
 */
/*
 * HISTORY
 * $Log: nx_types.h,v $
 * Revision 1.3  1994/11/19  02:44:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/05/19  09:59:22  shala
 * Added header
 *
 * Revision 1.1  92/05/13  07:58:47  shala
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
typedef int LP_MAP_ENTRY_T;
typedef LP_MAP_ENTRY_T * LP_MAP_T ;

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
        short           size;           /* Size of the partition */
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

#endif  _NX_TYPES_H_

