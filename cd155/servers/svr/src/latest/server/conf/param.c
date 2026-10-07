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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: param.c,v $
 * Revision 1.10  1994/12/08  23:25:33  suri
 *  Reviewer: jlitvin
 *  Risk: L
 *  Benefit or PTS #: 11454
 *  Testing: Specific testcase
 *  Module(s): uxkern/boot_config.c, conf/param.c
 *  Description: Added a new bootmagic parameter called BOOT_MAXUSERS which
 *  can be used to increase MAXUSERS and therefore other parameters that
 *  depend on MAXUSERS such as nproc and nfile. The default values will be
 *  same as the current settings which can be overridden by using the new
 *  tuneable parameter at the boot time.
 *
 * Revision 1.9  1994/11/18  20:29:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1993/07/19  22:59:11  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.7  1993/07/14  17:54:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:13:03  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:15:11  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:28:07  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:05:42  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/01/09  00:03:55  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.4  1992/12/22  17:11:42  cfj
 * Changed sysname to be "Paragon OSF/1".
 * 
 * Revision 1.1.2.1.2.1  1992/12/16  05:59:51  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:56:06  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:19:38  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:18:45  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.9  93/07/13  16:00:56  slively
 *      Revision 2.7  93/06/29  16:16:35  rabii
 *      Lite server changes (rabii)
 *
 * Revision 2.8  93/06/25  11:23:12  slively
 * Backout the LITE server changes, lite_node.h and #if LITE_NODE.
 * 
 * Revision 2.7  93/06/22  20:00:53  slively
 * Support for LITE server, #if LITE_NODE section.
 * 
 * Revision 2.6  92/12/02  11:08:03  chrisp
 * Add vproc hash table size defaulting to VPROC_NHASH.
 *
 * Revision 2.5  91/12/16  14:50:41  roy
 * 	91/10/17  15:45:04  condict
 * 	Convert completely to the OSF/1 Rel. 1.0.2 version, 
 * 	except for minor diffs.
 * 
 * Revision 2.4  91/10/04  14:56:21  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  16:39:55  rabii
 * 	Merge of V2.0 and Locus (locus check-in by roman)
 * 	Added initialization of number of vprocs in system.
 * 
 * Revision 2.2  91/08/31  13:31:16  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/07/29  16:30:03  barbou
 * Avoid conflict with the vers.c copyright.
 * 
 * Revision 3.2  91/04/12  13:59:00  condict
 * Update to OSF/1 version by adding missing items.
 * 
 * Revision 3.1  91/03/08  16:03:07  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.0  91/01/17  12:07:03  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 1.10.4.2  91/06/20  06:41:29  dwm
 * 	fixed timeout panic (bug #2416).
 * 	[91/06/20  06:40:06  dwm]
 * 
 * Revision 1.10  90/10/07  13:24:01  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:53:04  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:09:42  gm]
 * 
 * 	Really increase nvnode. 2 vnodes per mount point. (for fs + covered
 * 	directory) + 1 vnode for each ufs mount point, for the block device
 * 	shadow vnode. Why don't these come out of a zone?
 * 	[90/09/17  22:02:43  jeffc]
 * 
 * Revision 1.9  90/09/23  15:44:48  devrcs
 * 	Add NMOUNT to NVODE calculation.
 * 	[90/09/13  09:31:56  brezak]
 * 
 * Revision 1.8  90/09/13  11:43:33  devrcs
 * 	Use CLOCKS_PER_SEC instead of HZ. This is defined in
 * 	<machine/machlimits.h>.
 * 	[90/08/21  10:27:00  brezak]
 * 
 * Revision 1.7  90/08/09  13:16:44  devrcs
 * 	Copyright[] moved to vers.c
 * 	[90/08/03  17:43:04  tmt]
 * 
 * 	Run Pmaxen at HZ=256 again. This now works with new clock
 * 	correction code from DEC.
 * 
 * 	Revision Digital modification 90/07/27  15:15:50
 * 	     Al Deloray,Joe Comuzzi/DEC
 * 	Initialize fixtick (used in kern_clock.c) if HZ doesn't divide
 * 	     1000000 evenly
 * 
 * 	Revision DECdts modifications 90/07/24  14:14:31  Joe Comuzzi/DEC
 * 	Added global state variable maxdriftrecip which contains the
 * 	the reciprocal of the maximum drift rate for this processor.
 * 	[90/07/31  16:35:06  brezak]
 * 
 * Revision 1.6  90/06/22  20:09:21  devrcs
 * 	nags merge
 * 	[90/06/12  21:21:56  gmf]
 * 
 * Revision 1.5  90/02/23  00:22:30  devrcs
 * 	Allocate utsname structure with operating system id info.
 * 	[90/02/16  14:50:09  bet]
 * 
 * Revision 1.4  90/02/05  15:48:25  robert
 * 	Integrated 4.4BSD file system changes as of 1/5/90 [noemi]
 * 
 * Revision 1.3  90/01/03  11:52:40  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:28:30  gm]
 * 
 * Revision 1.2  89/12/26  09:26:22  gm
 * 	Change nfsnode to nnfsnode (oops).
 * 	Fixed quota.h include and added nnfsnode.
 * 	Include inode.h from ufs directory.
 * 	[89/12/25            gmf]
 * 
 * Revision 6.1  89/07/26  16:30:58  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.11  89/08/28  22:32:28  af
 * 	Changed definition of HZ to 64 for Pmaxen.  This provides
 * 	no rounding errors at all for tick. Needed, along with companion
 * 	fix to adjtime(), to get NTP run smoothly.
 * 	[89/08/26            af]
 * 
 * Revision 2.10  89/07/11  13:29:52  jjc
 * 	[89/07/06  11:59:38  jjc]
 * 
 * 	Defined HZ to be 100 for SPARC.
 * 	[89/03/30            jjc]
 * 
 * Revision 2.9  89/05/30  10:37:09  rvb
 * 	The clock on the Pmax is odd.  Made it run at 8ms, so that both
 * 	hz and tick do not suffer from rounding errors.
 * 	[89/04/16            af]
 * 
 * Revision 2.8  89/03/09  19:54:22  rpd
 * 	More cleanup.
 * 
 * Revision 2.7  89/02/25  17:41:56  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.6  88/11/23  16:36:15  rpd
 * 	Removed tickadj sun conditional.  (From bww.)
 * 	[88/11/23  15:21:23  rpd]
 * 
 * Revision 2.5  88/10/11  10:08:50  rpd
 * 	Changed includes to the new style.
 * 	Added cfreelist, cfreecount declarations.
 * 	Made cfree initialized, so ld won't try
 * 	to pull in cfree() from libc on RTs.
 * 	[88/10/06  07:49:13  rpd]
 * 
 * Revision 2.4  88/08/22  21:28:54  mja
 * 	Raise `tickadj' on the SUN3 to the same ridiculously high
 * 	number as under SunOS.
 * 	[88/08/22  13:30:11  mja]
 * 
 * 	Initialize `tickadj' based on HZ [bww].
 * 	[88/08/04  13:31:13  mja]
 * 
 * Revision 0.0  88/04/03            mwyoung
 * 	MACH: Remove unused variable declarations.
 * 	[88/04/03            mwyoung]
 * 
 * Revision 0.0  87/06/26            bolosky
 * 	Made QUOTA a #if-type option.
 * 	[87/06/26            bolosky]
 * 
 * Revision 0.0  87/04/07            dlb
 * 	MULTIMAX: 10 HZ clocks.
 * 	[87/04/07            dlb]
 * 
 * Revision 0.0  86/12/17            dbg
 * 	Removed uses of cmap.h, text.h for MACH.
 * 	[86/12/17            dbg]
 * 
 * Revision 0.0  86/11/01            bolosky
 * 	romp: defined HZ to be 64, because this responds better to the
 * 	1024 hz hardware clock on the RT.  This avoids serious
 * 	round-off error introduced with HZ = 100 which makes the clock
 * 	run really fast.
 * 	[86/11/01            bolosky]
 * 
 * Revision 0.0  86/09/27            mja
 * 	Changed to include "confdep.h"
 * 	[86/09/27            mja]
 * 
 * Revision 0.0  86/08/21            jjc
 * 	Use 50 hz for Sun instead of 100 hz.
 * 	[86/08/21            jjc]
 * 
 * Revision 0.0  86/07/10            mach
 * 	Changed NINODE calculation to *2 from *4.
 * 	[86/07/10            mach]
 * 
 * Revision 0.0  86/06/30            rfr
 * 	Restored size of NINODE to original NINODE*4.
 * 	[86/06/30            rfr]
 * 
 * Revision 0.0  86/06/30            rfr
 * 	Increased NINODE by 64 to cover inodes in use by the
 * 	vm_object_cache.
 * 	[86/06/30            rfr]
 * 
 * Revision 0.0  86/06/26            rfr
 * 	Restore calculation of NINODE now that the inode pager no longer
 * 	depends on the inode table to keep track of its paging inodes.
 * 	[86/06/26            rfr]
 * 
 * Revision 0.0  86/03/20            avie
 * 	If compiling a system with new VM, then increase NINODE by a
 * 	factor of 4 (since the new VM code pages out to inodes).
 * 	[86/03/20            avie]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1980, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)param.c	7.1 (Berkeley) 6/5/86
 */

#include <quota.h>
#include <confdep.h>
#include <fullserver.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/socket.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/vnode.h>
#include <ufs/quota.h>
#include <sys/file.h>
#include <sys/callout.h>
#include <sys/clist.h>
#include <sys/mbuf.h>
#include <sys/kernel.h>
#include <sys/utsname.h>

/*
 * System parameter formulae.
 *
 * This file is copied into each directory where we compile
 * the kernel; it should be modified there to suit local taste
 * if necessary.
 *
 * Compiled with -DTIMEZONE=x -DDST=x -DMAXUSERS=xx
 */

/*
 * The CLOCKS_PER_SEC (Posix) constant is defined per machine in
 * <machine/machlimits.h>. But if it isn't default to 100.
 */

#if !defined(CLOCKS_PER_SEC)
#define CLOCKS_PER_SEC 100
#endif

int	hz = CLOCKS_PER_SEC;
int	tick = 1000000 / CLOCKS_PER_SEC;
                                        /* This is the amount of time which
					   must be added once every hz tick
					   (i.e. once per second!) */
int	fixtick = 1000000 - ((1000000/CLOCKS_PER_SEC) * CLOCKS_PER_SEC);
int     maxdriftrecip = 1000;		/* This is a default which represents
					   a ridiculously poor clock which
					   drifts at a rate of 1 part in 1000,
					   most cpu clocks are good to between
					   1 part in 10000 (maxdriftrecip =
					   10000) and 2 parts in 100000
					   (maxdriftrecip = 50000). This value
					   should be established in some (clock)
					   hardware dependent module, e.g.
					   mc146818clock.c. for the PMAX */
#if	CLOCKS_PER_SEC > 100
int	tickadj = 1;			/* can adjust CLOCKS_PER_SEC usecs per second */
#else
int	tickadj = 100 / CLOCKS_PER_SEC;		/* can adjust 100 usecs per second */
#endif
struct	timezone tz = { TIMEZONE, DST };

struct utsname utsname = {
	"Paragon OSF/1",
	"",
	"",
	"",
	MACHINE
};
		
#define NPROC (20 + 8 * MAXUSERS)
#define NVNODE ((((NPROC + 16 + MAXUSERS) + 32)*2) + (NMOUNT+((8*NMOUNT)*2)))
int	maxusers = MAXUSERS;
int	nproc = NPROC;

#if     FULLSERVER
int	nvproc = NPROC*2;
int	nvnode = NVNODE;
int	nclist = 60 + 12 * MAXUSERS;
#else   /* FULLSERVER */
int     nvproc = 0;
int     nvnode = 0;
int     nclist = 0;
#endif  /* FULLSERVER */

int	nfile = 16 * (NPROC + 16 + MAXUSERS) / 10 + 32;
int	ncallout = 16 + NPROC;
int	create_fastlinks = 1;
int	select_max_elements = 1024 + NPROC * 4;
int	select_chunk_elements = 256;
int	path_num_max = 64;
int	ucred_max = 128;
int	nmount_max = 8 * NMOUNT;
int	nmount = NMOUNT;
int	nchsize = NVNODE * 11 / 10;
int	inohsz = 512;
int	spechsz = 64;
int	nchsz = 128;
int	bufhsz = 512;
int	vprochsz = VPROC_NHASH;
int	numcpus = 1;
#ifdef	multimax
int	nmbclusters;
#else
int     nmbclusters = NMBCLUSTERS;
#endif
#if	QUOTA
int	nquota = (MAXUSERS * 9) / 7 + 3;
int	ndquot = NVNODE + (MAXUSERS * NMOUNT) / 4;
#endif

/*
 * These are initialized at bootstrap time
 * to values dependent on memory size
 */

int	nbuf;


/*
 * These have to be allocated somewhere; allocating
 * them here forces loader errors if this file is omitted
 * (if they've been externed everywhere else; hah!).
 */
struct	proc *proc, *procNPROC;
struct	vnode *vnode, *vnodeNVNODE;
struct	file *file, *fileNFILE;
struct 	callout *callout;
struct	cblock *cfree = 0;
struct	cblock *cfreelist = 0;
int	cfreecount = 0;
struct	buf *buf;
char	*buffers;
struct	namecache *namecache;
#if	QUOTA
struct	quota *quota, *quotaNQUOTA;
struct	dquot *dquot, *dquotNDQUOT;
#endif
