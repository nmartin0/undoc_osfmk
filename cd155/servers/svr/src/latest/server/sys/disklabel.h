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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: disklabel.h,v $
 * Revision 1.7  1994/11/18  20:40:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/10/20  19:22:19  yazz
 *  Reviewer: jlitvin
 *  Risk: Low
 *  Benefit or PTS #: 10932
 *  Testing: By Inspection.
 *  Module(s): server/sys/disklabel.h
 *
 *  Added disklabel type for IPI-3 devices.
 *
 * Revision 1.5  1994/06/28  22:52:18  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.4  1993/07/14  18:24:06  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:32:58  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:18:32  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:41:03  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.2  1992/11/13  21:07:54  cfj
 * Up MAXPARTITIONS to 16.
 *
 * Revision 1.1.2.1  1992/11/05  22:39:18  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:00  cfj
 * Bump major revision number.
 *
 * Revision 2.4  94/02/03  11:01:26  dnoveck
 *      Make this file's disk label definition match the kernel's.
 *
 * Revision 2.3  93/10/20  15:26:50  dnoveck
 *      DEV_BSIZE elimination: Add warning about deplication between
 *      this header and the kernel header file device/disk_status.h.
 *      Clean up and rearrange.  Add DIOCGPINFO ioctl for server's
 *      internal use.
 *
 * Revision 2.2  1991/08/31  14:03:18  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:42:12  sp
 *  Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  14:49:42  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:08:04  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:38:03  gm]
 * 
 * 	Posix tty support.
 * 	[90/02/06  11:55:50  morris]
 * 
 * Revision 1.5  90/08/24  12:25:47  devrcs
 * 	HP/Apollo M68K
 * 	[90/08/13  17:36:55  mcg]
 * 
 * Revision 1.4  90/02/16  16:52:07  devrcs
 * 	Merged Robert Coren's and Rich Morris's changes
 * 	[90/02/15  14:03:30  ers]
 * 
 * Revision 1.3  90/01/02  20:16:28  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:33:28  gm
 * 	Added #if MACH.  Changed arguments to _IOR and _IOW.
 * 	[89/12/25            noemi]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1987, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)disklabel.h	7.10 (Berkeley) 6/27/88
 */

/****************************************************************************
*                                                                           *
*   *********************************************************************   *
*   *                                                                   *   *
*   *   WARNING: This file duplicates most of the definitions in        *   *
*   *            the kernel header file device/disk_status.h.  Until    *   *
*   *            someone fixes this, please keep them in sync.          *   *
*   *                                                                   *   *
*   *********************************************************************   *
*                                                                           *
****************************************************************************/


#ifndef	_DISKLABEL_
#define _DISKLABEL_
/*
 * Disk description table, see disktab(5)
 */
#define	DISKTAB		"/etc/disktab"

/*
 * Each disk has a label which includes information about the hardware
 * disk geometry, filesystem partitions, and drive specific information.
 * The label is in block 0 or 1, possibly offset from the beginning
 * to leave room for a bootstrap, etc.
 */

/*
 * The stuff below is totally a crock. To start, it assumes that the label is in the same
 * location on all disks which is bogus, and that the position of the label is invariant
 * with sector size which is also bogus. For instance, here at Apollo, we must have it
 * at either 1kbytes from the beginning of the disk or at 12k from the beginning regardless
 * of the sector size. So I'm just gonna change offset and hold the sector constant.
 */
#ifdef __hp_osf
#define LABELSECTOR	1			/* sector containing label - compatibility baggage, ya know */
#define LABELOFFSET	0			/* offset of label in sector - ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
#else   /* __hp_osf */
#define LABELSECTOR	0			/* sector containing label */
#define LABELOFFSET	64			/* offset of label in sector */
#endif  /* __hp_osf */

#define DISKMAGIC	((u_long) 0x82564557)	/* The disk magic number */
#ifndef MAXPARTITIONS
#define	MAXPARTITIONS	16
#endif


#ifndef LOCORE
struct disklabel {
	u_long	d_magic;		/* the magic number */
	short	d_type;			/* drive type */
	short	d_subtype;		/* controller/d_type specific */
	char	d_typename[16];		/* type name, e.g. "eagle" */
	/*
	 * d_packname contains the pack identifier and is returned when
	 * the disklabel is read off the disk or in-core copy.
	 * d_boot0 and d_boot1 are the (optional) names of the
	 * primary (block 0) and secondary (block 1-15) bootstraps
	 * as found in /usr/mdec.  These are returned when using
	 * getdiskbyname(3) to retrieve the values from /etc/disktab.
	 */
#if defined(KERNEL) || defined(STANDALONE)
	char	d_packname[16];			/* pack identifier */
#else
	union {
		char	un_d_packname[16];	/* pack identifier */
		struct {
			char *un_d_boot0;	/* primary bootstrap name */
			char *un_d_boot1;	/* secondary bootstrap name */
		} un_b;
	} d_un;
#define d_packname	d_un.un_d_packname
#define d_boot0		d_un.un_b.un_d_boot0
#define d_boot1		d_un.un_b.un_d_boot1
#endif	/* ! KERNEL or STANDALONE */
			/* disk geometry: */
	u_long	d_secsize;		/* # of bytes per sector */
	u_long	d_nsectors;		/* # of data sectors per track */
	u_long	d_ntracks;		/* # of tracks per cylinder */
	u_long	d_ncylinders;		/* # of data cylinders per unit */
	u_long	d_secpercyl;		/* # of data sectors per cylinder */
	u_long	d_secperunit;		/* # of data sectors per unit */
	/*
	 * Spares (bad sector replacements) below
	 * are not counted in d_nsectors or d_secpercyl.
	 * Spare sectors are assumed to be physical sectors
	 * which occupy space at the end of each track and/or cylinder.
	 */
	u_short	d_sparespertrack;	/* # of spare sectors per track */
	u_short	d_sparespercyl;		/* # of spare sectors per cylinder */
	/*
	 * Alternate cylinders include maintenance, replacement,
	 * configuration description areas, etc.
	 */
	u_long	d_acylinders;		/* # of alt. cylinders per unit */

			/* hardware characteristics: */
	/*
	 * d_interleave, d_trackskew and d_cylskew describe perturbations
	 * in the media format used to compensate for a slow controller.
	 * Interleave is physical sector interleave, set up by the formatter
	 * or controller when formatting.  When interleaving is in use,
	 * logically adjacent sectors are not physically contiguous,
	 * but instead are separated by some number of sectors.
	 * It is specified as the ratio of physical sectors traversed
	 * per logical sector.  Thus an interleave of 1:1 implies contiguous
	 * layout, while 2:1 implies that logical sector 0 is separated
	 * by one sector from logical sector 1.
	 * d_trackskew is the offset of sector 0 on track N
	 * relative to sector 0 on track N-1 on the same cylinder.
	 * Finally, d_cylskew is the offset of sector 0 on cylinder N
	 * relative to sector 0 on cylinder N-1.
	 */
	u_short	d_rpm;			/* rotational speed */
	u_short	d_interleave;		/* hardware sector interleave */
	u_short	d_trackskew;		/* sector 0 skew, per track */
	u_short	d_cylskew;		/* sector 0 skew, per cylinder */
	u_long	d_headswitch;		/* head switch time, usec */
	u_long	d_trkseek;		/* track-to-track seek, usec */
	u_long	d_flags;		/* generic flags */
#define NDDATA 5
	u_long	d_drivedata[NDDATA];	/* drive-type specific information */
#define NSPARE 5
	u_long	d_spare[NSPARE];	/* reserved for future use */
	u_long	d_magic2;		/* the magic number (again) */
	u_short	d_checksum;		/* xor of data incl. partitions */

			/* filesystem and partition information: */
	u_short	d_npartitions;		/* number of partitions in following */
	u_long	d_bbsize;		/* size of boot area at sn0, bytes */
	u_long	d_sbsize;		/* max size of fs superblock, bytes */
	struct	partition {		/* the partition table */
		u_long	p_size;		/* number of sectors in partition */
		u_long	p_offset;	/* starting sector */
		u_long	p_fsize;	/* filesystem basic fragment size */
		u_char	p_fstype;	/* filesystem type, see below */
		u_char	p_frag;		/* filesystem fragments per block */
		u_short	p_cpg;		/* filesystem cylinders per group */
	} d_partitions[MAXPARTITIONS];	/* actually may be more */
};
#else LOCORE
	/*
	 * offsets for asm boot files.
	 */
	.set	d_secsize,40
	.set	d_nsectors,44
	.set	d_ntracks,48
	.set	d_ncylinders,52
	.set	d_secpercyl,56
	.set	d_secperunit,60
	.set	d_end_,276		/* size of disk label */
#endif LOCORE

/* d_type values: */
#define	DTYPE_SMD		1		/* SMD, XSMD; VAX hp/up */
#define	DTYPE_MSCP		2		/* MSCP */
#define	DTYPE_DEC		3		/* other DEC (rk, rl) */
#define	DTYPE_SCSI		4		/* SCSI */
#define	DTYPE_ESDI		5		/* ESDI interface */
#define	DTYPE_ST506		6		/* ST506 etc. */
#define	DTYPE_IPI		7		/* HiPPI IPI-3 */
#define	DTYPE_FLOPPY		10		/* floppy */

#ifdef DKTYPENAMES
static char *dktypenames[] = {
	"unknown",
	"SMD",
	"MSCP",
	"old DEC",
	"SCSI",
	"ESDI",
	"type 6",
	"IPI",
	"type 8",
	"type 9",
	"floppy",
	0
};
#define DKMAXTYPES	(sizeof(dktypenames) / sizeof(dktypenames[0]) - 1)
#endif

/*
 * Filesystem type and version.
 * Used to interpret other filesystem-specific
 * per-partition information.
 */
#define	FS_UNUSED	0		/* unused */
#define	FS_SWAP		1		/* swap */
#define	FS_V6		2		/* Sixth Edition */
#define	FS_V7		3		/* Seventh Edition */
#define	FS_SYSV		4		/* System V */
#define	FS_V71K		5		/* V7 with 1K blocks (4.1, 2.9) */
#define	FS_V8		6		/* Eighth Edition, 4K blocks */
#define	FS_BSDFFS	7		/* 4.2BSD fast file system */

#ifdef	DKTYPENAMES
static char *fstypenames[] = {
	"unused",
	"swap",
	"Version 6",
	"Version 7",
	"System V",
	"4.1BSD",
	"Eighth Edition",
	"4.2BSD",
	0
};
#define FSMAXTYPES	(sizeof(fstypenames) / sizeof(fstypenames[0]) - 1)
#endif

/*
 * flags shared by various drives:
 */
#define		D_REMOVABLE	0x01		/* removable media */
#define		D_ECC		0x02		/* supports ECC */
#define		D_BADSECT	0x04		/* supports bad sector forw. */
#define		D_RAMDISK	0x08		/* disk emulator */
#define		D_CHAIN		0x10		/* can do back-back transfers */

/*
 * Drive data for SMD.
 */
#define	d_smdflags	d_drivedata[0]
#define		D_SSE		0x1		/* supports skip sectoring */
#define	d_mindist	d_drivedata[1]
#define	d_maxdist	d_drivedata[2]
#define	d_sdist		d_drivedata[3]

/*
 * Drive data for ST506.
 */
#define d_precompcyl	d_drivedata[0]
#define d_gap3		d_drivedata[1]		/* used only when formatting */

#ifndef LOCORE
/*
 * Structure used to perform a format
 * or other raw operation, returning data
 * and/or register values.
 * Register identification and format
 * are device- and driver-dependent.
 */
struct format_op {
	char	*df_buf;
	int	df_count;		/* value-result */
	daddr_t	df_startblk;
	int	df_reg[8];		/* result */
};

/*
 * Structure which used to be used internally to retrieve
 * information about a partition on a disk.
 */
struct partinfo {
	struct disklabel *disklab;
	struct partition *part;
};

/*
 * These are really generic scsi structure for some ioctl's below.
 * They are in this header file because they were defined in
 * device/disk_status.h and tracking that monstrosity is the least
 * bad of the available options.
 */
/*
 * Structure used to perform SCSI pass-through commands
 */
struct scsi_io {
#define SCSI_CDB_LENGTH 12
        unsigned char   cdb[SCSI_CDB_LENGTH];
        unsigned long   cmd_count;
#define SCSI_NONE       (0 << 0)
#define MEM_TO_SCSI     (1 << 0)
#define SCSI_TO_MEM     (1 << 1)
        unsigned long   direction;
#define SCSI_GOOD                       0x00
#define SCSI_CHECK_CONDITION            0x01
#define SCSI_CONDITION_MET              0x02
#define SCSI_BUSY                       0x03
#define SCSI_INTERMEDIATE               0x04
#define SCSI_INTERMEDIATE_CONDITION_MET 0x05
#define SCSI_RESERVATION_CONFLICT       0x06
#define SCSI_COMMAND_TERMINATED         0x07
#define SCSI_QUEUE_FULL                 0x08
        unsigned long   status;
        unsigned long   buf_len;
#define SCSI_BUF_MAX    (2 * 1024)
        unsigned char   buf[SCSI_BUF_MAX];
};

/*
 * Structure used to get SCSI device physical information
 */
struct scsi_phys {
        unsigned long   slot;
        unsigned char   controller;
        unsigned char   target_id;
        unsigned char   lun;
};


/*
 * Disk-specific ioctls and some scsi-generic ones forced on us by
 * disk_status.h.
 */

/*
 * Disk-specific ioctls.
 */
		/* get and set disklabel; DIOCGPART obsolete */
#define DIOCGDINFO	_IOR('d', 101, struct disklabel)/* get */
#define DIOCSDINFO	_IOW('d', 102, struct disklabel)/* set */
#define DIOCWDINFO	_IOW('d', 103, struct disklabel)/* set, update disk */
#define DIOCGPART	_IOW('d', 104, struct partinfo)	/* get partition */

/* do format operation, read or write */
#define DIOCRFORMAT	_IOWR('d', 105, struct format_op)
#define DIOCWFORMAT	_IOWR('d', 106, struct format_op)

#define DIOCSSTEP	_IOW('d', 107, int)	/* set step rate */
#define DIOCSRETRIES	_IOW('d', 108, int)	/* set # of retries */
#define DIOCWLABEL	_IOW('d', 109, int)	/* write en/disable label */

#define DIOCSBAD	_IOW('d', 110, struct dkbad)	/* set kernel dkbad */
/* SCSI pass-through command */
#define DIOCSCSI        _IOWR('d', 111, struct scsi_io)
/* Get SCSI device physical information */
#define DIOCGPHYS       _IOR('d', 112, struct scsi_phys)
/* Just get partition information.  Used interally in unix server. */
#define DIOCGPINFO      _IOR('d', 113, struct partition)
/* Used to get the mach_record size in DISK_GRANULE units. */
#define DIOMRINFO	_IOR('d', 114, int)   
#endif LOCORE

#if !defined(KERNEL) && !defined(LOCORE)
struct	disklabel *getdiskbyname();
#endif
#endif	_DISKLABEL_
