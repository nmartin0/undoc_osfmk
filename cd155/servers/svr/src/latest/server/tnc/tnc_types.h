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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: tnc_types.h,v $
 * Revision 1.13  1995/02/01  21:58:15  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.12  1994/11/18  20:44:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/06/02  22:30:23  chrisp
 * In dpvpop_reap(), perform PVPOP_RMV_PGRP_LIST() for zombie child
 * only if child's pgrp leader is not its parent; return child pgid.
 * In dvpop_wait(), analyze pgid returned for zombie child and call
 * PVPOP_RMV_PGRP_LIST() if this is the parent pid.
 *
 * Support added for waitmulti() - viz: elder reporting and reap multi
 * operation (refer to rtask_cli_vproc.c).
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): dpvproc.h dvp_pvpops.c dvp_vpops.c pvp.ops pvps.ops rtask.h
 * 	    rtask_cli_vproc.c rtask_server.c rtask_svr_vproc.c
 * 	    spanning_tree.c tnc_async.defs tnc_server_side.c
 * 	    tnc_types.defs tnc_types.h tnc_types_gen.c
 *
 * Revision 1.10  1994/04/04  17:33:03  paul
 *  Reviewer:John Litvin
 *  Risk: M
 *  Benefit or PTS #:7046
 *  Testing: test program run on Plymouth several thousand times...
 *  Module(s):
 *         server/tnc/dvp_vpsops.c
 *         server/tnc/dpvproc_struct.h
 *         server/tnc/tnc_types.h
 *         server/tnc/tnc_types.defs
 *         server/tnc/rvp_pvpsops.c
 *         server/tnc/dvp_pvpsops.c
 *         server/bsd/kern_xxx.c
 *         server/tnc/rvp_pvpsops_server.c
 * Use spanning trees for setting global hostname, hostid, domain name,
 *     timezone, rpm offset
 *
 * Revision 1.9  1993/10/21  23:37:07  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.8  1993/07/14  18:35:14  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.7  1993/07/01  20:49:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/06  19:26:43  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.5  1993/05/03  17:47:43  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6  1993/04/03  03:09:47  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.3.2.1  1992/12/16  06:03:23  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.5  1992/12/11  03:02:55  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.4  1992/11/30  22:49:05  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.3  1992/11/09  18:01:08  cfj
 * Conflict resolution of 11/05/92 bug fix drop from Locus.
 *
 * Revision 1.1.2.2  1992/11/06  20:32:27  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:47:12  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.15  93/09/16  09:32:28  chrisp
 * [SPE 0030] Generic Spanning Trees: a message handle is a mach (reply) port.
 * 
 * Revision 3.14  93/06/19  15:41:10  yazz
 * [ ad1.04 merge ]
 * 	Replace MAX_MMAP_REGIONS with TNC_MAX_MMAP_REGIONS.
 * 
 * Revision 3.13  92/12/01  11:45:51  chrisp
 * Add type definition for rval_t. Used only for RPCs with NO_REPLY.
 * 
 * Revision 3.12  92/11/05  15:35:37  klh
 * Include user.h explicitly 
 * 
 * Revision 3.11  92/11/04  11:21:59  chrisp
 * [Bug #91] Remove dependency upon the generated header tnc_types_gen.h
 * 	by quoting the constants used to generate it rather than the
 * 	generated constants (which only MIG really needs).
 * 
 * Revision 3.10  92/11/02  12:25:43  roman
 * Get rid of type buffer_t; it's really a char_array.
 * Add types needed for out-of-line data versions of migrate and rexecve.
 * 
 * Revision 3.9  92/10/28  15:55:46  roman
 * Add new types needed for table() pvpsops.
 * 
 * Revision 3.8  92/10/01  10:34:54  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 3.7  92/09/29  08:22:33  roman
 * Change type for pid_array_t for cleaner compilations.
 * 
 * Revision 3.6  92/08/08  01:49:08  jdh
 * both send and receive right port relocation arrays now -- jdh
 * 
 * Revision 3.5  92/06/11  16:10:47  mjl
 * Added new types blob_t and reloc_port_array_t used in pipe relocation.
 * 
 * Revision 3.4  92/03/27  11:34:56  roman
 * Add new type for array of credentials cache nodes.
 * 
 * Revision 3.3  92/02/14  09:02:49  roman
 * Change lists of vprocs and pids to use new symbolic constant (used for
 * parent-child-sibling lists, pgrp lists, and session lists).
 * 
 * Revision 3.2  92/01/07  18:28:14  roman
 * Add types needed to support mmap structures in tnc.defs.
 * 
 * Revision 3.1  91/12/19  11:14:57  chrisp
 * Add type for array of stati (status values) for migrate and rexec.
 * 
 * Revision 3.0  91/12/13  09:16:52  roman
 * Initial submission. The 'C' file equivalent to tnc/tnc_types.defs.
 * 
 */

#ifndef	_TNC_TNC_TYPES_H_
#define	_TNC_TNC_TYPES_H_

/*
 * Types for TNC server to server interface.
 */

#include <sys/types.h>
#include <sys/procset.h>
#include <sys/user.h>
#include <tnc/rtask.h>
#include <tnc/reloc.h>
#include <uxkern/bsd_types.h>

typedef pid_t		pid_array_t[MAX_VPROC_LIST];
typedef mach_port_t	vproc_array_t[MAX_VPROC_LIST];
typedef int		stat_array_t[MAX_VPROC_LIST];

typedef pid_t		*long_pid_array_t;
typedef mach_port_t	*long_vproc_array_t;
typedef int		*long_stat_array_t;
typedef struct rstat	*long_reap_stat_array_t;

typedef mach_port_t	mach_port_make_send_t;
typedef mach_port_t	mach_port_move_send_t;
typedef mach_port_t	mach_port_copy_send_t;
typedef mach_port_t	mach_port_name_t;

typedef char		*command_t;
typedef struct procset	procset_t[1];
typedef	struct rf_data	rf_data_t[1];
typedef	struct mi_data	mi_data_t[1];
typedef	struct re_data	re_data_t[1];
typedef char		command_name_t[MAXCOMLEN + 1];
typedef char		logname_t[MAXLOGNAME];
typedef struct mmap_struct mmap_struct_t;
typedef mmap_struct_t	mmap_struct_array_t[TNC_MAX_MMAP_REGIONS];
typedef mach_port_t	mmap_port_array_t[TNC_MAX_MMAP_REGIONS];

typedef node_t		cred_cache_array_t[CREDENTIALS_CACHE_SIZE];
typedef node_t		*long_cred_cache_array_t;
typedef node_t		*long_node_array_t;

typedef char		blob_t[MAX_BLOB_SIZE];
typedef mach_port_t	rright_array_t[RELOC_PORT_ARRAY_SIZE];
typedef mach_port_t	sright_array_t[RELOC_PORT_ARRAY_SIZE];

typedef int		*rval_t;

typedef mach_port_t	msg_handle_t;

#endif	_TNC_TNC_TYPES_H_
