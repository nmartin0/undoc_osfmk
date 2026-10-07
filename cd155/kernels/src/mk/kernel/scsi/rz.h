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
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * HISTORY
 * $Log: rz.h,v $
 * Revision 1.11  1995/03/14  23:48:17  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.10  1994/11/18  20:59:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/08/31  21:25:49  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.7.2.1  1994/08/04  16:08:07  richardg
 *  Reviewer: Jerry Coffman
 *  Risk: Medium
 *  Benefit or PTS #: 10171
 *  Testing: ran Tensor's tapewrt program simultaneously against both LUNs.
 *  Module(s): RZLUN_MASK
 *
 * Revision 1.7  1994/06/28  23:48:12  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.6  1993/06/30  22:53:28  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.5  1993/05/12  18:42:34  jerrie
 * Fix for bug 5099.
 * Added tzlun() macro.  For tape devices, use the lower three bits of
 * the minor number (three of the partition bits for disks, previously
 * unused for tape) to indicate the tape drive logical unit number.
 *
 * Revision 1.4  1993/04/27  20:47:38  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:53:04  dleslie
 * First R1_0 release
 *
 * Revision 2.7.4.2  92/04/30  12:00:16  bernadat
 * 	Used defines for controller, slave and partition fields
 * 	in dev number since they may be used elsewhere.
 * 	Changes from TRUNK:
 * 		Fabricate extra partition info to deal with the
 * 		first alternate partition range and "PARITITON_ABSOLUTE".
 * 		[92/04/01            rvb]
 * 
 * Revision 2.7.4.1  92/02/18  19:19:45  jeffreyh
 * 	Increase number of disk partitions to 16 to be compatible with 2.5
 * 	[92/02/08            bernadat]
 * 
 * Revision 2.7  91/06/19  11:56:57  rvb
 * 	File moved here from mips/PMAX since it is now "MI" code, also
 * 	used by Vax3100 and soon -- the omron luna88k.
 * 	[91/06/04            rvb]
 * 
 * 	A couple of macros were not fully parenthesized, which screwed
 * 	up the second scsi bus on Vaxen.  [This was the only bug in the
 * 	multi-bus code, amazing].
 * 	[91/05/30            af]
 * 
 * Revision 2.6  91/05/14  17:26:19  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/05/13  06:04:20  af
 * 	Redefined naive macro names to avoid conflicts.
 * 	[91/05/12  16:08:55  af]
 * 
 * Revision 2.4  91/02/05  17:43:42  mrt
 * 	Added author notices
 * 	[91/02/04  11:16:33  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:15:29  mrt]
 * 
 * Revision 2.3  90/12/05  23:33:55  af
 * 
 * 
 * Revision 2.1.1.1  90/11/01  03:43:37  af
 * 	Created.
 * 	[90/10/21            af]
 */
/*
 *	File: rz.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Mapping between U*x-like indexing and controller+slave
 *	Each controller handles at most 8 slaves, few controllers.
 */

#define RZCONTROLLER_SHIFT	7
#define RZSLAVE_SHIFT		4

#define RZCONTROLLER_MASK	0x1
#define	RZSLAVE_MASK		0x7
#define RZPARTITION_MASK	0xf
#define RZLUN_MASK		0x7

#define DEV_MASK	((RZCONTROLLER_MASK << RZCONTROLLER_SHIFT) | \
			 (RZSLAVE_MASK	    << RZSLAVE_SHIFT)	   | \
			 (RZPARTITION_MASK))

/* Define rzpassthru to be the 'p' partition */
#define	rzpassthru(dev)		(((dev)&RZPARTITION_MASK)==0xf)

#define	rzcontroller(dev)	(((dev)>>RZCONTROLLER_SHIFT)&RZCONTROLLER_MASK)
#define	rzslave(dev)		(((dev)>>RZSLAVE_SHIFT)&RZSLAVE_MASK)
#define	rzlun(dev)		((dev)&RZLUN_MASK)
#define	rzpartition(dev)	((PARTITION_TYPE(dev)==0xf)?MAXPARTITIONS:((dev)&RZPARTITION_MASK))

#define PARTITION_TYPE(dev)	(((dev)>>24)&0xf)
#define PARTITION_ABSOLUTE	(0xf<<24)

#define	RZDISK_MAX_SECTOR	65536

#define	RZ_DEFAULT_BSIZE	2048	/* DFLT_512 Was 512 */

#ifdef RUN1.1
#define RECS_PER_SEC(tgt)	1
#else
#define RECS_PER_SEC(tgt)	\
	((tgt)->dev_info.disk.l.d_secsize/(tgt)->block_size)
#endif
#ifdef	MACH_KERNEL
#else	/*MACH_KERNEL*/
#define tape_unit(dev)		((((dev)&0xe0)>>3)|((dev)&0x3))
#define	TAPE_UNIT(dev)		((dev)&(~0xff))|(tape_unit((dev))<<3)
#define	TAPE_REWINDS(dev)	(((dev)&0x1c)==0)||(((dev)&0x1c)==8)
#endif	/*MACH_KERNEL*/
