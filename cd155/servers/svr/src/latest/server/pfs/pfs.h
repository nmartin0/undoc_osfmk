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
 * $Log: pfs.h,v $
 * Revision 1.6  1994/11/18  20:38:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/06/14  17:42:42  brad
 * Added R1.3 support for getting, setting, and temporarily mapping
 * per-file PFS stripe attributes via new F_GETSATTR and F_SETSATTR
 * fcntl() requests.  Also added an F_GETFULLSATTR request for use by ls
 * in getting full (including stripe file pathnames) stripe attributes.
 * Also modified default attributes given a file when it is created: the
 * start stripe directory is now a random member of the stripe group,
 * rather than always the first member, so small files consume disk space
 * evenly across the stripe group.
 *
 *  Reviewer: rlg (in progress)
 *  Risk: Med
 *  Benefit or PTS #: 7593, 9686
 *  Testing: Developer tests, PFS EATs on 64 nodes
 *  Module(s): server/{sys/fcntl.h,pfs/pfs.h}
 *             emulator/{pfs_fdt.h,fsvr_user_side.c,pfs2_user_side.c}
 *
 * Revision 1.4  1993/08/31  23:19:55  brad
 * Simple clarification of a comment ... no code changes.
 *
 * Revision 1.3  1993/04/03  03:07:00  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.6  1993/03/20  23:06:18  brad
 * Made PFS_MNAMELEN dependent on MNT_MNAMELEN.
 *
 * Revision 1.1.2.1.2.5  1993/03/11  00:54:27  dbm
 * Added estatfs structure.
 *
 * Revision 1.1.2.1.2.4  1993/02/23  04:34:36  brad
 * Changed field prefix in pfsmntinfo so that it's different from prefix
 * in statpfs struct.
 *
 * Revision 1.1.2.1.2.3  1993/01/12  00:08:45  dbm
 * Added definition of pfsmntinfo to support the getpfsinfo() function.
 *
 * Revision 1.1.2.1.2.2  1992/12/18  18:15:07  brad
 * Added p_magic to statpfs structure, for PFS file header validation.
 *
 * Revision 1.2  1992/11/30  22:33:41  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:10:59  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.1  1992/11/10  15:21:55  cfj
 * Put into NX branch.
 *
 * Revision 1.1  1992/11/05  22:34:59  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 1.1  1992/10/22  15:24:07  dbm
 * Added for PFS functionality.
 *
 *
 */
#ifndef _SYS_PFS_H_
#define _SYS_PFS_H_

/*
 * Parallel File System definitions.
 */


#ifndef	_SIZE_SET
#define	_SIZE_SET

/*
 * Symbolic constants for the lsize() and esize() functions.
 */
#define SIZE_SET 0      /* set file size to MAX(cur size,"offset") */
#define SIZE_CUR 1      /* set file size to MAX(cur size,cur offset+"offset" */
#define SIZE_END 2      /* set file size to MAX(cur size,cur size+"offset" */
#endif

/*
 * Value to sanity check a PFS file header.
 */
#define PFS_MAGIC	0x045911


/*
 * Generic pathname structure.
 */
typedef struct	pathname {
	uint_t	namelen;		/* length of pathname in name */
	char	name[4];		/* placeholder for pathname and      */
					/* a NULL terminator, 4-byte aligned */
} pathname_t;


/*
 * Full PFS stripe attributes structure.
 */
struct statpfs {
	uint_t		p_reclen;	/* total length of this record      */
	long		p_magic;	/* PFS magic number used by open()  */
	size_t		p_sunitsize;	/* size of each stripe unit         */
	uint_t		p_sfactor;	/* stripe factor (= # stripedirs)   */
	uint_t		p_start_sdir;	/* index of starting stripedir      */
	uint_t		p_reserved;	/* reserved for future use          */
	pathname_t	p_sdirs;	/* variable length array of         */
 					/* stripedir pathnames              */
};


/*
 * PFS stripe attributes under user control via fcntl().  A subset of the
 * statpfs structure above.
 */
struct sattr {
	size_t	s_sunitsize;	/* size of each stripe unit                 */
	uint_t	s_sfactor;	/* stripe factor (= # stripedirs)           */
	uint_t	s_start_sdir;	/* index of starting stripe directory       */
};


/*
 * Macros useful for manipulating the array of stripedirs (p_sdirs) in the
 * statpfs structure.
 *
 * The PATHSIZE macro calculates the record length of the given pathname_t
 * structure.  This length includes enough space for a terminating NULL byte
 * (p->namelen+1), and is rounded up to a 4 byte boundary (pathnames must be
 * word-aligned in order to avoid a SIGBUS "hardware alignment" error).
 *
 * Usage: n += PATHSIZE(sdirp);
 *
 * Where sdirp is of type (pathname_t *) and has been initialized properly;
 * e.g., sdirp = &statp->p_sdirs.
 */
#define	PATHSIZE(p)	((sizeof((p)->namelen) + (p)->namelen+1 + 3) & ~3)

/*
 * The NEXTPATH macro accepts a pathname_t pointer and returns a pointer to
 * the next path in the array of p_sdirs.
 *
 * Usage: sdirp = NEXTPATH(sdirp);
 *
 * Where sdirp is of type (pathname_t *) and has been initialized properly;
 * e.g., sdirp = &statp->p_sdirs.
 */
#define NEXTPATH(p)	((pathname_t *)((char *)(p) + PATHSIZE((p))))


/*
 * Extended statfs structure.
 *   **** WARNING:  This structure must match the statfs structure except
 *                  for the three esize_t entries.
 */
#define MNT_MNAMELEN	90	/* This value must be the same as defined
				   by MNAMELEN in sys/mount.h */
typedef quad mnt_fsid_t;	/* Ditto */

struct estatfs {
	short		f_type;		/* type of filesystem (see below) */
	short		f_flags;	/* copy of mount flags */
	long		f_fsize;	/* fundamental filesystem block size */
	long		f_bsize;	/* optimal transfer block size */
	esize_t		f_blocks;	/* total data blocks in file system */
	esize_t		f_bfree;	/* free blocks in fs */
	esize_t		f_bavail;	/* free blocks avail to non-su */
	long		f_files;	/* total file nodes in file system */
	long		f_ffree;	/* free file nodes in fs */
	mnt_fsid_t	f_fsid;		/* file system id */
	long		f_spare[9];	/* spare for later */
	char		f_mntonname[MNT_MNAMELEN];   /* mounted on directory */
	char		f_mntfromname[MNT_MNAMELEN]; /* mounted filesystem */
};


/*
 * PFS mount information for getpfsinfo().   The size of m_mntonname is
 * rounded up to a 4 byte boundary from MNAMELEN (in mount.h) so that each 
 * structure in a list of pfsmntinfo structures is word-aligned (to avoid a
 * SIGBUS "hardware alignment" error trying to access the next pathname).
 */
#define	PFS_MNAMELEN	((MNT_MNAMELEN + 3) & ~3)

struct pfsmntinfo {
	char		m_mntonname[PFS_MNAMELEN];/* file system mount point */
	struct statpfs  m_statpfs;	/* PFS stripe attributes */
};

#endif /* _SYS_PFS_H_ */
