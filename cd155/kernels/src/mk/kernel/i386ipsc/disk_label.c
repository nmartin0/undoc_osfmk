/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * Copyright 1992 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: disk_label.c,v $
 * Revision 1.5  1994/11/18  20:37:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:28:50  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:27:21  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:27:50  dleslie
 * First R1_0 release
 *
 * Revision 2.1.2.1  92/03/28  10:07:09  jeffreyh
 * 	New file for generic scsi driver.
 * 	[92/03/17            jeffreyh]
 * 
 *
 */

/*
 * Set up a default disk label
 * Called if the drive does not have one
 */

#include <mach/std_types.h>
#include <scsi/compat_30.h>
#include <scsi/scsi_defs.h>

/*
 * Maxtor 760 Specs
 */
#define NSECPERTRACK_760	54
#define NCYL_760		1632
#define NHEADS_760		15
#define NTRACKPERCYL_760	NHEADS_760
#define NSECPERCYL_760		(NSECPERTRACK_760 * NTRACKPERCYL_760)
#define NSEC_760		(NSECPERCYL_760 * NCYL_760)

#if	OSF_PARTITION

/*
 * revised, hard coded values better suited to OSF1/AD - andyp
 *
 *	                /-------------------------------------------------\
 *	                |                        g                        |
 *	/---+---+-------+---------------------------------------+---------+
 *	| a | f |   b   |                    e                  |    d    |
 *	+---+---+-------+---------------------------------------+---------+
 *	|                               c                                 |
 *	\-----------------------------------------------------------------/
 *
 */

/*
 * Maxtor 760 using Andy's OSF partitioning
 */
#define PART0_SIZE_760	(  96 * NSECPERCYL_760)		/*  37.97 MB (/) */
#define PART1_SIZE_760	( 164 * NSECPERCYL_760)		/*  64.86 MB (page) */
#define PART2_SIZE_760	((NCYL_760-18)*NSECPERCYL_760)	/* 638.34 MB (all) */
#define PART3_SIZE_760	( 380 * NSECPERCYL_760)		/* 150.29 MB (/var) */
#define PART4_SIZE_760	( 878 * NSECPERCYL_760)		/* 347.26 MB (/usr) */
#define PART5_SIZE_760	(  96 * NSECPERCYL_760)		/*  37.97 MB (2nd /) */
#define PART6_SIZE_760	(1258 * NSECPERCYL_760)		/* 497.55 MB (d + e) */

#if 0
/*
 * *my* drive is holding at least 16 cylinders in it's back pocket...
 * the following partitions were computed from the spec sheet without
 * knowing about the 16+ cylinders reserved by the drive.  This is okay
 * because I never managed to put anything out in d or g, and never
 * tried to use c for a filesystem.
 */
#define PART2_SIZE_760	(NCYL_760*NSECPERCYL_760)/* 645.47 MB (entire disk) */
#define PART3_SIZE_760	( 398 * NSECPERCYL_760)	/* 157.41 MB (/var) */
#define PART6_SIZE_760	(1276 * NSECPERCYL_760)	/* 504.67 MB (d + e) */
#endif

#define	PART0_START_760	(   0 * NSECPERCYL_760)	/* a */
#define PART1_START_760	( 192 * NSECPERCYL_760)	/* b */
#define PART2_START_760	(   0 * NSECPERCYL_760)	/* c */
#define PART3_START_760	(1234 * NSECPERCYL_760)	/* d */
#define PART4_START_760	( 356 * NSECPERCYL_760)	/* e */
#define PART5_START_760	(  96 * NSECPERCYL_760)	/* f */
#define PART6_START_760	( 356 * NSECPERCYL_760)	/* g */

#else	OSF_PARTITION

#if	OSF_LOCAL
/*
 * Hard coded values for partition info, according to the
 * way the OSF cube is set up.  This will go away with the
 * new SCSI stuff in the next release.
 */
#define PART0_START_760	0
#define PART0_SIZE_760	52250

#define PART1_START_760	(PART0_SIZE_760 + PART0_START_760)
#define PART1_SIZE_760	83830

#define PART2_START_760	(PART1_SIZE_760 + PART1_START_760)
#define PART2_SIZE_760	103680		/* 53 MB */

#define PART3_START_760	(PART2_SIZE_760 + PART2_START_760)
#define PART3_SIZE_760	103680		/* 53 MB */

#define PART4_START_760	(PART3_SIZE_760 + PART3_START_760)
#define PART4_SIZE_760	207360		/* 106 MB */

#define PART5_START_760	(PART4_SIZE_760 + PART4_START_760)
#define PART5_SIZE_760	207360		/* 106 MB */

#define PART6_START_760	0
#define PART6_SIZE_760	0

#else	OSF_LOCAL

/*
 * Maxtor 760 using Intel partitioning
 */
#define PART0_SIZE_760	64800		/* 31.6 MB */
#define PART1_SIZE_760	103680		/* 53 MB */
#define PART2_SIZE_760	103680		/* 53 MB */
#define PART3_SIZE_760	103680		/* 53 MB */
#define PART4_SIZE_760	207360		/* 106 MB */
#define PART5_SIZE_760	207360		/* 106 MB */
#define PART6_SIZE_760	0

#define PART0_START_760	0
#define PART1_START_760	(PART0_SIZE_760 + PART0_START_760)
#define PART2_START_760	(PART1_SIZE_760 + PART1_START_760)
#define PART3_START_760	(PART2_SIZE_760 + PART2_START_760)
#define PART4_START_760	(PART3_SIZE_760 + PART3_START_760)
#define PART5_START_760	(PART4_SIZE_760 + PART4_START_760)
#define PART6_START_760	0

#endif OSF_LOCAL
#endif	OSF_PARTITION

/*
 * Label used if the disk does not have one
 */
struct disklabel ipsc_default_label =
{
        /*
         * label for Maxtor 760
         */
	DISKMAGIC, DTYPE_SCSI, 0, "SCSI", "",
	DEV_BSIZE,				/* bytes per sector */
	NSECPERTRACK_760,			/* sectors per track */
	NTRACKPERCYL_760,			/* tracks per cylinder */
	NCYL_760,				/* cylinders per unit */
	NSECPERCYL_760,				/* sectors per cylinder */
	NSEC_760,				/* sectors per unit */
	0, 0, 0,
	3600, 1, 1, 1, 0, 2500, 0,
	{0,}, {0,},
	DISKMAGIC, 0,

	7, 8192, 8192,
	{{ PART0_SIZE_760, PART0_START_760, 1024, FS_BSDFFS, 8, 3 },
	 { PART1_SIZE_760, PART1_START_760, 1024, FS_BSDFFS, 8, 3 },
	 { PART2_SIZE_760, PART2_START_760, 1024, FS_BSDFFS, 8, 3 },
	 { PART3_SIZE_760, PART3_START_760, 1024, FS_BSDFFS, 8, 3 },
	 { PART4_SIZE_760, PART4_START_760, 1024, FS_BSDFFS, 8, 3 },
	 { PART5_SIZE_760, PART5_START_760, 1024, FS_BSDFFS, 8, 3 },
	 { PART6_SIZE_760, PART6_START_760, 1024, FS_BSDFFS, 8, 3 }}
};

/*
 * our vendor label is really *no* label
 */
boolean_t
rz_ipsc_label( tgt, label, ior)
    target_info_t		*tgt;
    struct disklabel	*label;
    io_req_t		ior;
{
	if (strncmp(tgt->tgt_name, "MAXTOR XT-8760S", 15) == 0) {
		/*
		 * default to compiled-in Maxtor 760 partitioning
		 */
		printf("rz%d: using default label, %d actual sectors.\n",
			tgt->unit_no,
			label->d_secperunit);
		*label = ipsc_default_label;
		return TRUE;
	}

	printf("rz%d: unlabeled %s, %d total sectors.\n",
		tgt->unit_no,
		tgt->tgt_name,
		label->d_secperunit);

	return FALSE;
}
