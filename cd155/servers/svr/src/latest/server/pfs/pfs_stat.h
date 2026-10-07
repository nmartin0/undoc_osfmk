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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992 Intel Corporation.
 */

/*
 * HISTORY
 * $Log: pfs_stat.h,v $
 * Revision 1.3  1994/11/18  20:38:23  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/11/15  23:41:42  dbm
 *  Reviewer: None
 *  Risk:Low
 *  Benefit or PTS #:7140, added mode field to pfs_stat structure so that
 * 	                the type of file can be determined when setting
 * 			the PFS I/O mode.
 *  Testing: Verified with test case.
 *  Module(s): pfs_stat.h
 *
 * Revision 1.1  1992/11/25  22:23:34  brad
 * Initial revision
 *
 *
 */

#ifndef _PFS_PFS_STAT_H_
#define _PFS_PFS_STAT_H_

/*
 * Private PFS stat structure that contains the real device number, rather
 * than the "mount id" returned in the OSF/1 AD stat structure which is
 * not persistent across mounts of the same disk partition.
 */
struct pfs_stat {
	node_t		pst_node;	/* # of node on which device lives */
	dev_t		pst_dev;	/* # of device on which file lives */
	ino_t		pst_ino;	/* # of file (inode number on UFS) */
	off_t		pst_size;	/* file size in bytes */
	mode_t		pst_mode;	/* File mode (type). */
};

#endif _PFS_PFS_STAT_H_
