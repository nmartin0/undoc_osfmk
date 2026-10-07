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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: version.h,v $
 * Revision 1.8  1994/11/18  20:42:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/01  20:32:55  mag
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
 * Revision 1.6  1993/10/27  01:14:46  carbajal
 * Changed NX_APP_VERSION to 2 to record changes to _nx_init().
 *
 * Revision 1.5  1993/10/04  18:48:35  cfj
 * Add NX_APP_VERSION.
 *
 * Revision 1.4  1993/07/14  18:31:06  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:40:18  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:21:07  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:45:44  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:43:16  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:42:27  cfj
 * Bump major revision number.
 * Revision 1.1.1.1  1993/05/03  17:43:57  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  14:16:58  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:47:36  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:56:52  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:09:32  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:49:21  gm]
 * 
 * Revision 1.3  90/01/02  20:16:15  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/10/26  08:07:38  gm
 * 	MACH X115 Update
 * 
 * Revision 6.1  89/07/26  14:26:43  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.11  89/10/11  23:00:16  dlb
 * 	Bump include version to 26 for new 0-31 priorities.
 * 	[89/10/11            dlb]
 * 
 * Revision 2.10  89/03/09  22:10:19  rpd
 * 	More cleanup.
 * 
 * Revision 2.9  89/02/25  17:58:26  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.8  89/01/20  18:30:49  jsb
 * 	I guess I'd better not #undef _XXX_INCLUDE_VERSION.
 * 
 * Revision 2.7  89/01/20  17:22:54  jsb
 * 	Change method of allocation of include version numbers to reflect the
 * 	fact that different configurations (vide VICE, MACH_VFS) of the same
 * 	version of a kernel are incompatible.
 * 
 * Revision 2.6  88/08/24  02:53:04  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:28:01  mwyoung]
 * 
 * Revision 0.0  87/10/13            mja
 * 	Update MACH_VM to #11 and non-Mach to #10.
 * 	[ V5.1(XF18) ]
 * 	[87/10/13            mja]
 * 
 * Revision 0.0  87/04/21            mja
 * 	Bumped MACH_VM include version to 5 for SUN debugger related
 * 	fixes to the proc and pcb structures.
 * 	[ V5.1(F10) ]
 * 	[87/04/21            mja]
 * 
 * $EndLog$
 */

#ifndef	_SYS_VERSION_H_
#define _SYS_VERSION_H_

/*
 *	Each kernel has a major and minor version number.  Changes in
 *	the major number in general indicate a change in exported features.
 *	Changes in minor number usually correspond to internal-only
 *	changes that the user need not be aware of (in general).  These
 *	values are stored at boot time in the machine_info strucuture and
 *	can be obtained by user programs with the host_info kernel call.
 *	This mechanism is intended to be the formal way for Mach programs
 *	to provide for backward compatibility in future releases.
 *
 *	[ This needs to be reconciled somehow with the major/minor version
 *	  number stuffed into the version string - mja, 5/8/87 ]
 *
 *	Following is an informal history of the numbers:
 *
 *	25-March-87  Avadis Tevanian, Jr.
 *		Created version numbering scheme.  Started with major 1,
 *		minor 0.
 */

#define KERNEL_MAJOR_VERSION	1
#define KERNEL_MINOR_VERSION	0

/* 
 *  Version number of the kernel include files.
 *
 *  This number must be changed whenever an incompatible change is made to one
 *  or more of our include files which are used by application programs that
 *  delve into kernel memory.  The number should normally be simply incremented
 *  but may actually be changed in any manner so long as it differs from the
 *  numbers previously assigned to any other versions with which the current
 *  version is incompatible.  It is used at boot time to determine which
 *  versions of the system programs to install.
 *
 *  Structures that are used by system programs include
 *	user, uthread, utask, proc, file 
 *  Note that the symbol _INCLUDE_VERSION must be set to this in the symbol
 *  table.  On the VAX for example, this is done in locore.s.
 */

#define INCLUDE_VERSION	26			/* add one to me */

/*
 *  Bump this number when ever an incompatible change has been made
 *  such that NX applications need to be relinked or recompiled.
 */
#define NX_APP_VERSION   3
#endif	_SYS_VERSION_H_
