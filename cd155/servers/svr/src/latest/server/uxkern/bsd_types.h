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
 * Mach Operating System
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: bsd_types.h,v $
 * Revision 1.9  1995/02/01  22:17:39  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.8  1994/11/18  20:47:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/02  22:35:11  chrisp
 * RPCs bsd_rforkmulti and rforkmulti_long parameters change:  file
 * ports added; child task and thread ports eliminated.  Boolean boot
 * magic variable TNC_RFORKMULTI_BINARY_TREE added to determine the
 * spanning tree scheme rforkmulti is to use; it defaults to true i.e.
 * binary tree.  Parameter added for RPC fsvr_file_ref to specify the
 * required change in reference count. Previously, this operation
 * incremented counts by 1 only; now n can be specify and note that
 * n can be negative.
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): boot_config.c bsd_1.defs bsd_types.defs bsd_types.h fsvr.defs
 *             fsvr_server_side.c
 *
 * Revision 1.6  1993/07/14  18:39:56  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  20:59:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:25:27  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/03  03:11:18  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 * Revision 1.1.1.2  1993/05/03  17:50:56  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.3  1992/11/30  22:53:08  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2.2.1  1992/11/25  23:16:28  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.2  1992/11/06  20:34:13  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:42:05  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.15  92/11/02  12:27:51  roman
 * Add types necessary for version of rforkmulti() using out-of-line memory.
 * 
 * Revision 2.14  92/10/28  15:59:57  roman
 * Add types for TNC-specific system calls.
 * 
 * Revision 2.17  93/06/16  15:28:05  klh
 * 	Revision 2.14  93/06/02  17:13:05  rabii
 * 		Added definition for acct_t (rabii)
 * 
 * Revision 2.16  93/04/24  18:48:40  klh
 * 	Revision 2.13  93/03/30  16:09:02  roy
 * 		Remove fast_path_io definitions.
 * 		[93/02/18            roy]
 *
 * Revision 2.15  92/11/02  12:27:51  roman
 * Add types necessary for version of rforkmulti() using out-of-line memory.
 * 
 * Revision 2.14  92/10/28  15:59:57  roman
 * Add types for TNC-specific system calls.
 * 
 * Revision 2.13  92/10/01  10:36:23  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 2.12  92/06/10  10:11:51  klh
 * 	Revision 2.11  92/06/08  18:26:18  pjg
 * 		Add FAST_PATH_IO definitions (pjg).
 * 
 * Revision 2.11  92/04/14  10:45:35  roman
 * Add types necessary for TNC rforkmulti() system call.
 * 
 * Revision 2.10  92/04/06  19:10:19  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.9  92/04/05  17:10:46  pjg
 * 	Define devstat_t and stat_t (durriya).
 * 	Got rid of statb_t (pjg).
 * 
 * Revision 2.8  92/03/01  18:35:49  pjg
 * 	Remove transaction_id_t definition.  It is now in sthread.h, which
 * 	is included by sys/user.h and thus gets everywhere.  (loverso)
 * 
 * Revision 2.7  92/01/17  17:27:10  roy
 * 	Added transaction_id_t (loverso).
 * 
 * Revision 2.6  91/11/22  15:20:49  rabii
 * 	Added declaration of rusage_dev_t (used by remote pvproc process reap 
 * 	operation) (klh)
 * 
 * Revision 2.5  91/10/14  13:23:50  sjs
 * 	91/10/04  16:43:01  sp
 * 	Add support for auxiliary vector
 * 
 * Revision 2.4  91/10/04  15:18:21  chrisp
 * Add in Locus copyright.
 * 
 * Revision 2.3  91/09/17  09:19:18  sjs
 * integrate Locus changes	roman
 * Declarations added for credentials types.
 * 
 * Revision 2.2  91/08/31  14:24:46  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/05/30  17:39:01  jose
 * Added new fields to statb struct.
 * 
 * Revision 3.1  91/05/15  17:58:18  barbou
 * Definition of type "sigaction_t" used by osf1_sigaction()/
 * 
 * Revision 3.0  91/01/17  12:05:50  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.4  90/06/02  15:27:07  rpd
 * 	Fixed type of rlimit_t.
 * 	[90/03/26  20:09:53  rpd]
 * 
 * Revision 2.3  90/05/29  20:24:59  rwd
 * 	Added cfname_t to return command processor name and args from
 * 	exec.
 * 	[90/03/22            dbg]
 * 
 * Revision 2.2  90/05/21  14:01:30  dbg
 * 	Fix type for rlimit_t so it is copied as a by-reference
 * 	parameter.
 * 	[90/04/12            dbg]
 * 
 * 	Added cfname_t to return command processor name and args from
 * 	exec.
 * 	[90/03/22            dbg]
 * 
 * $EndLog$
 */

#ifndef	_UXKERN_BSD_TYPES_H_
#define	_UXKERN_BSD_TYPES_H_

/*
 * Types for BSD kernel interface.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <sys/acct.h>
#ifdef	PFS
#include <pfs/pfs_stat.h>
#endif

#include <uxkern/bsd_types_gen.h>

typedef	char		*char_array;
typedef char		small_char_array[SMALL_ARRAY_LIMIT];
typedef	char		path_name_t[PATH_LENGTH];
typedef struct timeval	timeval_t;
typedef	struct timeval	timeval_2_t[2];
typedef	struct timeval	timeval_3_t[3];

typedef struct stat     stat_t;
typedef	struct rusage	rusage_t;
typedef	char		sockarg_t[128];
typedef	vm_address_t	entry_array[16];
typedef	int		gidset_t[NGROUPS];
typedef	struct rlimit	rlimit_t[1];
typedef	struct sigvec	sigvec_t;
typedef struct sigaction sigaction_t;
typedef	struct sigstack	sigstack_t;
typedef struct timezone	timezone_t;
typedef	struct itimerval itimerval_t;
typedef	char		hostname_t[MAXHOSTNAMELEN];
typedef	char		domainname_t[MAXDOMNAMELEN];
typedef	char		cfname_t[64];
typedef struct cred { int cred_int[CRED_SIZE]; } cred_t;
typedef struct rlimit	rlimit_array_t[RLIM_NLIMITS];
typedef	struct rusage_dev rusage_dev_t[1];
typedef struct devstat devstat_t;
typedef struct acct acct_t;
#ifdef	PFS
typedef struct pfs_stat	pfs_stat_t;
#endif

#include <sys/auxv.h>

#ifdef	TNC

/* types used for rforkmulti() system call */
typedef	int		multi_int_array_t[MAX_MULTI_LIST_SIZE];
typedef	pid_t		multi_pid_array_t[MAX_MULTI_LIST_SIZE];
typedef	node_t		multi_node_array_t[MAX_MULTI_LIST_SIZE];
typedef	mach_port_t	multi_name_array_t[MAX_MULTI_LIST_SIZE];
typedef	mach_port_t	multi_port_array_t[MAX_MULTI_LIST_SIZE];
typedef	mach_port_t	multi_vproc_array_t[MAX_MULTI_LIST_SIZE];

typedef	int		*long_multi_int_array_t;
typedef	pid_t		*long_multi_pid_array_t;
typedef	node_t		*long_multi_node_array_t;
typedef	mach_port_t	*long_multi_name_array_t;
typedef	mach_port_t	*long_multi_port_array_t;
typedef	mach_port_t	*long_multi_vproc_array_t;

#endif	/* TNC */

#endif	_UXKERN_BSD_TYPES_H_
