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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: boot_config.c,v $
 * Revision 1.43  1995/05/04  21:41:00  stans
 *  Undo the incorrect thread_reduction disable fix. Real fix is in
 *  "uxkern/ux_server_loop.c".
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:8993
 *  Testing: developer tests
 *
 * Revision 1.42  1995/04/27  17:18:00  stans
 *  Disable thread reduction code as it's not ready for prime-time. Default
 *  UX_OLD_THREADS == 1 which disables thread reduction (phase I) code.
 *
 *  Reviewer:stans
 *  Risk:low
 *  Benefit or PTS #: 8993
 *  Testing:
 *   R1.3 WW15 sats & munops
 *  Module(s):
 *   uxkern/ux_server_loop.c
 *
 * Revision 1.41  1995/03/28  22:22:54  stans
 *  Support ux server thread reduction bootmagic 'UX_OLD_THREADS'
 *  UX_OLD_THREADS=1 enables old R1.2 style ux server threads mgmt.
 *  UX_OLD_THREADS=0 enable new style thread mgmt; Aggressively recycle threads
 *  back into the Cthread library.
 *
 *  Reviewer:jlitvin
 *  Risk:high
 *  Benefit or PTS #: 8993
 *  Testing:
 *         WW10 sats: watson, a5, si160 & testjig
 *         EATS on a5
 *         munops on a5
 *         Developer tests:
 *                 pathkiller XPS150(1024 nodes), watson, a5, si160 and testjig
 *                 PFS i/o perf tests: a5, testjig, XPS150.
 *                   PFS read & write performance sees a slight increase with
 *                   reduced threads.
 *
 * Revision 1.40  1995/03/14  01:25:00  yazz
 *  Reviewer: John Litvin, Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 12961
 *  Testing: EATs controlc
 *  Module(s): server/uxkern/boot_config.c
 * When parsing boolean bootmagic strings, accept 0 and 1 as false & true,
 * instead of calling them an error.
 *
 * Revision 1.39  1995/03/08  22:22:52  jlitvin
 * OK, I fixed the ramdisk build, but broke the LITE server.  Thanks to
 * Stan for getting everything working this time.
 *
 * Revision 1.38  1995/03/08  20:58:42  jlitvin
 * FULLSERVER is defined for the ramdisk, so we need to be more
 * restrictive when we refer to the new I/O max bootmagic
 * 'FS_MAX_DEVICE_REQUEST'.
 *
 * Revision 1.37  1995/03/07  22:56:30  stans
 *  Support device I/O max bootmagic 'FS_MAX_DEVICE_REQUEST'
 *
 *  Reviewer:rlg
 *  Risk:low
 *  Benefit or PTS #:11397
 *  Testing:WW09 sats
 *  Module(s):
 * 	uxkern/boot_config.c
 * 	ufs/ufs_vnops.c
 *
 * Revision 1.36  1995/03/02  21:37:08  stans
 *  Lint picking
 *
 *  Reviewer:lenb & suri
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW07 sats
 *
 * Revision 1.35  1995/02/18  01:40:34  yazz
 *  Reviewer: John Litvin
 *  Risk: Med
 *  Benefit or PTS #: 12240, including emul console logging cleanup
 *  Testing: EATs controlc, sched
 *  Module(s):
 * 	svr/emulator/bsd_user_side.c
 * 	svr/emulator/emul_chkpnt.c
 * 	svr/emulator/emul_init.c
 * 	svr/emulator/emul_mapped.c
 * 	svr/emulator/fsvr_user_side.c
 * 	svr/server/bsd/kern_sig.c
 * 	svr/server/bsd/mach_signal.c
 * 	svr/server/bsd/subr_prf.c
 * 	svr/server/tnc/dvp_vpops.c
 * 	svr/server/uxkern/boot_config.c
 * 	svr/server/uxkern/bsd_server_side.c
 * 	svr/server/uxkern/credentials.c
 * 	svr/server/uxkern/rpm_clock.c
 *
 * General cleanup of emulator console logging.  Added bootnode_printf()
 * routine to server.  Added server bootmagic variable ENABLE_RPM_TIMESTAMP
 * so printf() and bootnode_printf() messages are timestamped with the
 * 56-bit RPM global clock value.  This enables very fine timings to be
 * observable in console log output.
 *
 * Revision 1.34  1995/02/01  22:15:51  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.33  1994/12/09  00:43:17  suri
 *  Reviewer: jlitvin
 *  Risk: L
 *  Benefit or PTS #: 11454
 *  Testing: Specific testcase
 *  Module(s): uxkern/boot_config.c, conf/param.c
 *  Description: Added a new bootmagic parameter called BOOT_MAXUSERS which
 *    can be used to increase MAXUSERS and therefore other parameters that
 *    depend on MAXUSERS such as nproc and nfile. The default values will be
 *    same as the current settings which can be overridden by using the new
 *    tuneable parameter at the boot time.
 *
 * Revision 1.32  1994/11/18  20:46:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.31  1994/10/25  21:00:44  hobbes
 *  Benefit or PTS #:11347
 *  Testing: TCP/IP EATS and developer tests to verify improved ftp performance
 *  Module(s): uxkern/boot_config.c netinet/tcp_subr.c
 *
 * Revision 1.30  1994/09/06  20:35:57  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: #70732 H-1
 *  Testing: controlc EATS, with new bootmagic CHECK_FOR_RPMFAIL true & false
 *
 *  Module(s): server/uxkern/rpm_clock.c, .../boot_config.c
 *
 * Define new boolean bootmagic variable CHECK_FOR_RPMFAIL.
 *
 * Revision 1.29  1994/08/31  22:47:31  mtm
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
 * Revision 1.24.2.6  1994/08/23  17:56:49  dbm
 * Added missing ifdef PFS statements around the pfs_async_dflt logic.
 * This was causing the ramdisk build to break.
 *  Reviewer: rlg
 *  Risk:Low
 *  Benefit or PTS #: 10701
 *  Testing: Ramdisk build.
 *  Module(s):
 * 	server/uxkern/boot_config.c
 * 	emulator/emul_init.c
 *
 * Revision 1.24.2.5  1994/08/23  00:42:37  yazz
 *  Author of fix: Paul Cantrell (paul@locus.com)
 *  Reviewer: Bob Yasi, Mike Leibensperger, Dave Minturn
 *  Risk: Lo
 *  Benefit or PTS #: 10647 C-0
 *  Testing: Specific testcase
 *  Module(s): server/uxkern/boot_config.c, server/uxkern/rpm_clock.c
 * RPM failure error messages now contain more info for RPM diagnosis, and
 * appear on the boot node regardless of the failing node.  New bootmagic
 * REBOOT_ON_RPMFAIL defaults to false; only if set true will the system reboot
 * when an RPM failure occurs.
 *
 * Revision 1.24.2.4  1994/08/19  22:19:15  dbm
 * Added support for a new bootmagic, PFS_ASYNC_DFLT, this allows setting
 * the default PFS I/O mode to M_ASYNC.
 *
 *  Reviewer:Bob Godley
 *  Risk:M
 *  Benefit or PTS #:10569
 *  Testing: Specific test cases. PFS EATS (With and without bootmagic set)
 *  Module(s):
 *
 *     (server)
 * 	uxkern/boot_config.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr.defs
 *     (emulator)
 * 	emul_init.c
 * 	fsvr_user_side.c
 * 	pfs2_user_side.c
 * 	pfs_iomode.c
 * 	pfs_tokenmgt.c
 * 	pfs_iomode.h
 * 	pfs_fdt.h
 *     (libnx)
 * 	_pfs_setio.c
 * 	_setiomode.c
 *
 * Revision 1.24.2.3  1994/08/13  01:51:54  paul
 * Reviewer: mjl jlitvin
 * Risk:M
 * Benefit or PTS #: 8836
 * Testing: tested on Plymouth across diag station reboots
 * Module(s):
 *     sys/time.h
 *     uxkern/rpm_clock.c
 *     uxkern/boot_config.c
 *
 * Changes to fix bug 8836 9521 RTI - Handle an RPM which stops counting
 *     This change halts a system when the RPM stops counting either due
 *     to a hardware failure, or a reboot of the diagnostic station. It
 *     also includes a boot magic variable to override use of the RPM.
 *     Check out "DONT_USE_RPM_GLOBAL_CLOCK" in this file.
 *
 * Revision 1.24.2.2  1994/08/12  14:56:43  flb
 *  Reviewer: cfj
 *  Risk: medium
 *  Benefit or PTS #: 10173
 *  Testing: boot big bootmagic
 *  Module(s): boot_config.c
 *  Description:  Included the changes that aloocate/deallocate
 * 			   memory for boot magic string storage that
 * 			   was left out of previous version.
 *
 * Revision 1.24.2.1  1994/08/10  17:36:19  flb
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS # 10173
 *  Testing: System Boot with large MAGIC.MASTER
 *  Module(s): uxkern/boot_config.c,uxkern/server_init.c,user/etc/load_level/osf1_dep.c
 *
 * Revision 1.24  1994/07/27  14:16:15  johannes
 * introduced new boot magic PARACORE_MAX_PROCESSES
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Low
 *  Benefit or PTS #: limit the number of processes to dump core
 *  Testing: developer
 *  Module(s): server/uxkern/boot_config.c
 * 	    server/paracore/dvp_pvpcore.c
 *
 * Revision 1.23  1994/06/28  23:18:01  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.22  1994/06/02  22:34:50  chrisp
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
 * Revision 1.21  1994/05/20  18:14:08  lenb
 * for cfj, BOOT_PAGE0_ACCESS=1
 * to workaround problem with applications using page0
 *
 * Revision 1.20  1994/01/12  17:47:33  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.19  1993/12/21  23:16:56  cfj
 * Merge R1.2 changed into main-stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.18  1993/12/10  22:36:26  nina
 * Fixed bugs that prevented Paragons from being used
 * as NFS clients if the boot node is not a network
 * server node.  See #6831, #6719, #7421, #7422, #7423
 * #7424 and #7426.  Added entries to the configTable[]
 * to support the CLEARINGHOUSE bootmagic variable and
 * to allow allocation of buffer cache entries on network
 * server nodes that don't have physical file systems.
 *
 *
 *  Reviewer:bolsen, dbm
 *  Risk:Medium
 *  Benefit or PTS #:7423, 7426
 *  Testing:Lachman NFS main suite, various configurations
 *  Module(s):./server/uxkern/boot_config.c
 *
 * Revision 1.17.2.1  1993/12/21  21:33:43  cfj
 * Added BOGUS_VNODE_PROXY_PANIC bootmagic which if set will panic the
 * server when the "bogus vnode_proxy" condition is detected.  Also puts
 * the printf inside #if MACH_ASSERT.  This has been added for debug
 * purposes only.  It does not FIX any bugs bug may allow us to debug
 * this problem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):server/uxkern/boot_config.c
 *            server/uxkern/fsvr_msg.c
 *
 * Revision 1.17  1993/10/11  20:57:04  cfj
 * Remove RB_WIRE_SERVER bootmagic since that was intended as an experiment
 * and should not have been left in.
 *
 * Revision 1.16  1993/10/04  19:10:42  stans
 *    Be a little less verbose when 'Overriding previous value of' a bootenv
 *    variable; Routine parse_boot_config(). Eliminate output of BOOT_NODE_LIST
 *    being reset do to failed nodes. "-v" server cmd line will enable the
 *    verbose message.
 *
 * Revision 1.15  1993/10/01  16:21:20  stefan
 * Call sll_check_bm_fork_remote_timeout when parsing FORK_REMOTE_TIMEOUT.
 *
 * Revision 1.14  1993/09/24  14:28:16  cfj
 * Initialize num_buffers_bmagic to 10 so that the ramdisk will work
 * even if you do not have NUM_BUFFERS=10 in the bootmagic.
 *
 * Revision 1.13  1993/07/20  21:44:23  nandy
 * Changed the rpc_waittime from long to mach_msg_timeout_t.
 *
 * Revision 1.12  1993/07/16  19:34:19  rkl
 * Added HIPPI_BUF_CACHE_SIZE bootmagic variable to support HiPPI network
 * buffer cache implementation.
 *
 * Revision 1.11  1993/07/14  18:39:26  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:58:01  cfj
 * Adding new code from vendor
 *
 * Revision 1.10  1993/06/01  23:28:58  nandy
 * Added bootmagic RPC_WAITTIME to specify RPC timmeout values for TNC calls.
 *
 * Revision 1.9  1993/06/01  16:12:47  stefan
 * Renamed sll/sll.h to sll/sll_types.h.
 *
 * Revision 1.8  1993/05/18  17:40:00  cfj
 * Make CUBE_IO_NODE_LIST bootmagic under #ifdef i860.
 *
 * Revision 1.7  1993/05/18  16:58:35  cfj
 * Properly split the RAMDISK bootmagic code between boot_config.c and server_init.c
 *
 * Revision 1.6  1993/05/17  21:58:49  nandy
 * Changed the name of bootmagic BOOT_RAID5_NODE_LIST to BOOT_DISK_NODE_LIST
 *
 * Revision 1.5  1993/05/13  09:11:10  stefan
 * Integrated static load leveling support.
 *
 * Revision 1.4  1993/05/10  18:04:42  cfj
 * Added CUBE_IO_NODE_LIST back into the bootmagic.
 *
 * Revision 1.3  1993/05/10  15:20:59  cfj
 * Fixup boot magic definitions for NX.
 *
 * Revision 1.2  1993/05/06  19:24:25  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:50:27  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.7  94/02/03  11:02:15  dnoveck
 *      Changes for per-node buffer-cache block size: add new variable
 *      BCACHE_MAXBSIZE to control it.
 *
 * Revision 2.8  93/06/02  12:31:53  yazz
 * For Sys V IPC under TNC allow new bootmagic string "SVIPC_NODE" to
 * specify which node will be the svipc clearinghouse node.
 * 
 * Revision 2.7  93/05/05  16:37:39  bolsen
 * [Bug 239] added code to support Hypercube I/O nodes (i386).
 * 
 * Revision 2.6  93/04/29  23:01:51  klh
 * Add extern node_t for TNC compilation
 *
 * Revision 2.5  93/04/12  16:43:21  durriya
 * 	build an array of nodes that may have paging files (kernel & vnode)
 * 	based on the boot config variables. This list is used for getting paging
 * 	file info for the entire domain.
 * 	[93/04/12            durriya]
 * 
 * Revision 2.4  93/04/08  11:37:41  loverso
 * 	Fix function callout for non-vector types to pass args in same
 * 	order.  Added WarnOldPagingConfig for a test against EXPORT_PAGING.
 * 	(loverso)
 * 
 * Revision 2.3  93/02/04  11:48:58  durriya
 * 	Add backwards compatible paging configuration under conditional
 * 	compilation.  This is unsupported.
 * 
 * Revision 2.2  93/01/14  13:10:11  loverso
 * 	Server boot config parsing logic. (loverso)
 * 
 */
#include <fullserver.h>
#include <sys/param.h>
#include <sys/types.h>
#include <kern/lock.h>
#include <uxkern/import_mach.h>

#ifdef  OSF1_ADFS
#include <norma_ipc.h>
#include <remote_proc.h>
#endif

#ifdef SLL
#include <sys/unix_defs.h>
#include <sll/sll_types.h>
#endif /* SLL */


/* Always on for now */
#define WarnOldPagingConfig


/* Bootstrap debug messages, syscall tracing, etc: */
extern int	print_debug_messages;
#define print_debug(x) \
	if (print_debug_messages) printf x; else 0


#ifdef  NX
#define MAX_BOOT_OPTIONS_SIZE   KERNEL_BOOTMAGIC_MAX
#else   /* NX */
#define MAX_BOOT_OPTIONS_SIZE  	KERNEL_BOOT_INFO_MAX
#endif  /* NX */

extern char	boot_options_buf[];

#define MAX_EMUL_NAME_SIZE      256
extern char    emul_name_buf[];

extern int		bcache_maxbsize;
extern int		ipi3_bcache_maxbsize;
#ifdef PFS
extern int		pfs_async_dflt;
#endif

extern mach_port_t	privileged_host_port;

extern node_t	this_node;		/* Our node id.  0 if !NORMA_IPC */
extern node_t	root_fs_node;		/* Node id of root fileserver */
extern node_t	root_device_node;	/* Node id of root device */
extern node_t	debug_node;		/* Node id of debug node */
extern node_t	debug_root_device_node;	/* Node id of debug node root dev */
#if     TNC
extern node_t	netserver_node;		/* Node id of networking device */
extern node_t	svipc_node;		/* Node id of System V IPC server */
extern node_t	clearinghouse_node;	/* Node id of clearinghouse database*/
extern boolean_t tnc_rforkmulti_binary_tree; /* Use binary tree (default) */
#if defined(i860)
extern boolean_t rpm_avoid_using_global_clock; /* avoid using RPM hw clock */
extern boolean_t rpm_reboot_on_fail; 	/* avoid reboot if bad RPM clock */
extern boolean_t rpm_check_for_fail; 	/* if false, don't complain about RPM */
extern boolean_t enable_rpm_timestamp;	/* add <<56-bit>> rpmtime to printfs */
#endif
#endif  /* TNC */

#if     NORMA_IPC
extern boolean_t	import_paging;
extern boolean_t	export_paging;
extern node_t		pager_node;	/* remote pager node */
					/* (default and vnode) */

/* Device table */
extern boolean_t	dev_tab_fun(node_t *, node_t *, node_t);

extern long	strtol(const char *, char **, int);
extern ulong	strtoul(const char *, char **, int);
extern char *	strtok_r(char *, const char *, char **);
extern char *	index(char *, const char);
extern char *	strpbrk(const char *, const char *);
extern size_t	strspn(const char *, const char *);

/* Forward */
void		boot_config_init(void);
int		boot_config_parse_vars(char *);

#ifdef WarnOldPagingConfig
boolean_t	importPagingWarn(void *, void *, node_t);
#endif

boolean_t	maxusers_init_func(void *, long *, node_t);

boolean_t	complainUnknown = FALSE;

#if	BUG_COMPAT
extern	boolean_t	allow_page_0_access;
#endif

#ifdef  RAMDISK
#define RAMDISK_NUM_BUFFERS     10;              /* Use this for ramdisk   */
int     num_buffers_bmagic = RAMDISK_NUM_BUFFERS; /* NumBuffers for ramdisk */
#endif

boolean_t bogus_vnode_proxy_panic = FALSE;

#if RFC_1323
extern	long	hippi_buf_cache_size;
extern	u_long	tcp_space_size;
#endif

extern	vm_prot_t	page_0_protection;

extern	long	maxusers;
extern	int	nproc;
extern	int	nvnode;
extern	int	nfile;
extern	int	ncallout;
extern	int	select_max_elements;
extern	int	nchsize;
extern	int	nclist;
#if	QUOTA 
extern	int 	nquota;
extern	int	ndquot;
#endif

#if	REMOTE_PROC
int     accept_procs = FALSE;
#endif

#endif	/* NORMA_IPC */


#if 	NORMA_IPC

/*
 * List of nodes in configuration
 */
#define NODE_RANGES	4096

node_t	node_array[NODE_RANGES][2];	/* a series of ranges; XXX: huge */
size_t	node_array_entries;		/* number of valid ranges */
ulong	incarnation;			/* not really used */

#ifdef i860

node_t  cube_io_node_array[NODE_RANGES][2];   /* a series of ranges */
int     cube_io_node_array_entries = 0;	      /* number of valid ranges */
#endif /* i860 */

#ifdef NX
node_t  fs_node_array[NODE_RANGES][2];  /* a series of ranges */
int     fs_node_array_entries=0;	/* number of valid ranges */
node_t  enet_node_array[NODE_RANGES][2];  	/* nodes with ethernet hdw */
int     enet_node_array_entries=0;        	/* # of nodes with ethernet */
node_t  hippi_node_array[NODE_RANGES][2];  	/* nodes with HIPPI hdw */
int     hippi_node_array_entries=0;        	/* # of node with HIPPI */
node_t  ipi3_node_array[NODE_RANGES][2];  	/* nodes with IPI3 hdw */
int     ipi3_node_array_entries=0;        	/* # of node with IPI3 */

extern node_t  allocator_node;     /* Allocator port */
extern node_t  boot_first_node;    /* Only for paragon */
extern int     debug_sethostname;  /* Debug sethostname. */
extern boolean_t      nx_check_rpc_waittime(node_t *, long *);
extern mach_msg_timeout_t    rpc_waittime;     /* Timeout value for TNC calls */
#endif /* NX */

#ifdef PARACORE
extern long paracore_max_processes;
#endif /* PARACORE */

#if FULLSERVER && !defined(RAMDISK)
extern long fs_max_device_request;	/* see server/ufs/ufs_vnops.c */
#endif

extern long old_threads;		/* see uxkern/ux_server_loop.c */

/* 
 * pager_node_list is dynamically allocated one-dimensional array used to keep
 * track of nodes that have a 'pager' - either vnode or kernel. These are the
 * nodes that will be contacted for paging information.
 * pager_node_list_ent keeps track of the  # of valid entries in pager_node_list
 * max_pager_node_list_ent is the max size of pager_node_list array
 */
node_t  *pager_node_list;
size_t  pager_node_list_ent = 0;
size_t  max_pager_node_list_ent = 0;

/*
 * Server boot config variable parsing.
 *
 * The boot configuration is gotten from the micro-kernel, and is
 * constrained by the machine-dependent boot configuration scheme.
 * By the time the configuration is available to servers, each entry
 * consists of "name=value" and has been NL-terminated.  Any further
 * syntax (as described below) is enforced by the server.  In some
 * cases (i.e., tolerance of embedded white space), restrictions may
 * be enforced by machine-dependent boot or micro-kernel code.
 *
 * (i.e., on a i386 with "nd", it is OK to have embedded white space;
 *  but a cube or paragon won't let it pass)
 *
 * Configuration variables are of the form:
 *	NAME=value
 * Where NAME is usually in caps, and is separated from its value by an "=".
 * White space is tolerated.
 *
 * Server config variables come in two forms.
 * First, there are simple types:
 *	void, boolean, long, unsigned long, char, string
 *
 * "Bool" requires a value of {1|"true"|"on"} or {0|"false"|"off"},
 *	and is case independent.
 * "Void" means "no parsing is done".
 * "Node" is syntactically the same as unsigned long (ULong).
 * "Range" defines multiple nodes, as in:
 *	(node)		n ::= #
 *	(range)		r ::= { n | n..n | r,r }
 *
 * Thus:
 *	(simpletype)	s ::= { void, bool, long, ulong, char, str, n, r }
 *
 * Then there are composite types.  Only one is defined:
 *	vector (aka attribute/value).
 *
 * A vector has two parts, the selector and the value, each from the
 * simpletype list.  The selector is matched against to decide which
 * value to use.  The notation for vectors is:
 *	(vector)	v ::= { sval | <ssel>sval | v:v }
 *
 * EX: ssel is Range and sval is node:
 * 	The string "<1..4>34:<5>0" indicates that the value for nodes 1
 *	through 4 is "node 34", whereas node 5 gets the value "node 0".
 *	A lone sval, as in "<1..4>34:9", is used as a default value, matching
 *	anything that hasn't already had a match.
 *
 * A variable is defined by "struct configVar".
 *
 *	+ If the value is not a vector, then selType is Void.
 *
 *	+ The value field, if not NULL, will be filled in with the decoded
 *	  value of type valtype.  The value will be:
 * 	  	for simple types except Range: the value
 *	  	for Range: a node list (node_t(*)[2])
 *	  	for vectors: the value for my_node if func is NULL
 *	  (ALERT: vector code is Range-centric; see func for other mappings)
 *
 *	+ If the thisNode field is not NULL, it will be filled in with a
 *	  TRUE/FALSE if my_node is present in the Node or Range given
 * 	  (for Node and Range types).
 *
 *	+ An escape function can be specified to do additional work on
 *	  the value or values of a variable.  It will be called for the value,
 *	  or for vectors, for each selector/value pair.  It is declared:
 *
 *		boolean_t (*func)(void *sel, void *val, node_t matchNode);
 *
 *	  It will be passed pointers to the decoded selector/value,
 *	  and the node for which this is being decoded.  If there is no
 *	  selector (because we are decoding a simpletype or because this
 *	  is the default value for a vector), then it is passed as NULL.
 *	  It should return TRUE if the value field should be given this value.
 *	  This return value will be store in the thisNode field, if it is
 *	  non-NULL.
 *
 *	  By using an escape function, vectors can support any simpletype
 *	  as the selector and value.  However, it is tricky to understand what
 *	  is really meant by a combination of Range,Range!
 *
 *	  NOTE: It is possible for duplicate selector values to be passed
 *		to the escape function:
 *			<0..10>1;<5>2
 *
 *	+ The size field, if not 0, indicates the size of the buffer
 *	  supplied for the value.  (Only for Range and String).
 *
 *	+ The outsize field, if not NULL, will be filled in with
 *	  the size of the string or range stored, or 1 for other types.
 *
 *	+ The rawValue field is a hint, and it's contents are not to
 *	  used/trusted.  (it can only be tested against NULL).
 */

typedef enum {
	Void, Bool, Long, ULong, Char, String, Node, Range
} cfType;

struct configVar {
        char		*name, *rawValue;
        cfType		valType, selType;
        size_t		size, *outsize;	/* only used for String, Range */
        boolean_t	*thisNode;
        void		*value;
	boolean_t	(*func)(void *sel, void *val, node_t matchNode);
};
#define noFunc ((boolean_t(*)(void *, void*, node_t))NULL)

union	simpletype {
	ulong	ul;
	long	l;
	char	c;			/* XXX: alignment */
	boolean_t b;
	node_t	n;
	char	*s;
};

/*
 * Textual convention:
 *	{ "ROOT_FS_NODE", NULL,		name, value
 *		Node, Void,		value type, selector type
 *		0, NULL,		maxsize, out size (pointer)
 *		NULL,			thisNode,
 *		&root_fs_node,		value,
 *		noFunc },		function
 */
struct configVar configTable[] = {
	{ "ROOT_FS_NODE", NULL,
		Node, Void,
		0, NULL,
		NULL, &root_fs_node, noFunc },
	{ "ROOT_DEVICE_NODE", NULL,
		Node, Void,
		0, NULL,
		NULL, &root_device_node, noFunc},
#ifdef OldPagingConfiguration
	{ "IMPORT_PAGING", NULL,
		Bool, Range,
		0, NULL,
		NULL, &import_paging, noFunc},
	{ "EXPORT_PAGING", NULL,
		Bool, Range,
		0, NULL,
		NULL, &export_paging, noFunc},
	{ "PAGER_NODE", NULL,
		Node, Range,
		0, NULL,
		NULL, &pager_node, noFunc},
#else
#ifdef WarnOldPagingConfig
	{ "IMPORT_PAGING", NULL,
		Void, Void,
		0, NULL,
		NULL, NULL, importPagingWarn},
#endif
	{ "EXPORT_PAGING", NULL,
		Range, Void,
		0, NULL,
		&export_paging, NULL, noFunc},
	{ "PAGER_NODE", NULL,
		Node, Range,
		0, NULL,
		&import_paging, &pager_node, noFunc},
#endif
	{ "BOOT_EMULATOR_NAME",	NULL,
		String, Void,
		MAX_EMUL_NAME_SIZE, NULL,
		NULL, emul_name_buf, noFunc},
	{ "BOOT_OPTIONS", NULL,
		String, Void,
		MAX_BOOT_OPTIONS_SIZE, NULL,		/* 4K! */
		NULL, boot_options_buf, noFunc},
	{ "BOOT_NODE_LIST", NULL,
		Range, Void,
		NODE_RANGES, &node_array_entries,
		NULL, node_array, noFunc},
#ifdef	i860
	{"CUBE_IO_NODE_LIST", NULL,
		Range, Void,
		NODE_RANGES, &cube_io_node_array_entries,
		NULL, cube_io_node_array, noFunc},
#endif	/* i860 */
	{ "DEV_TAB", NULL,
		Node, Node,
		0, NULL,
		NULL, NULL, dev_tab_fun},
#if REMOTE_PROC
#ifdef OldPagingConfiguration
	{ "ACCEPT_PROCS", NULL,
		Bool, Range,
		0, NULL,
		NULL, &accept_procs, noFunc},
#else
	{ "ACCEPT_PROCS", NULL,
		Range, Void,
		0, NULL,
		&accept_procs, NULL, noFunc},
#endif
#endif
#ifdef TNC
	{ "NETSERVER", NULL,
		Node, Void,
		0, NULL,
		NULL, &netserver_node, noFunc},

	{ "SVIPC_NODE", NULL,
		Node, Void,
		0, NULL,
		NULL, &svipc_node, noFunc},

	{ "CLEARINGHOUSE", NULL,
		Node, Void,
		0, NULL,
		NULL, &clearinghouse_node, noFunc},

	{ "TNC_RFORKMULTI_BINARY_TREE", NULL,
		Bool, Range,
		0, NULL,
		NULL, &tnc_rforkmulti_binary_tree, noFunc},
#if defined(i860)
	{ "DONT_USE_RPM_GLOBAL_CLOCK", NULL,
		Bool, Range,
		0, NULL,
		NULL, &rpm_avoid_using_global_clock, noFunc},
	{ "REBOOT_ON_RPMFAIL", NULL,
		Bool, Range,
		0, NULL,
		NULL, &rpm_reboot_on_fail, noFunc},
	{ "CHECK_FOR_RPMFAIL", NULL,
		Bool, Range,
		0, NULL,
		NULL, &rpm_check_for_fail, noFunc},
	{ "ENABLE_RPM_TIMESTAMP", NULL,
		Bool, Range,
		0, NULL,
		NULL, &enable_rpm_timestamp, noFunc},
#endif
#endif
#ifdef  NX
        {"ALLOCATOR_NODE", NULL,
	        Node, Void,
	        0, NULL,
	        NULL,
	       &allocator_node, noFunc},

        {"BOOT_FIRST_NODE", NULL,
	        Node, Void,
	        0, NULL,
	        NULL,
	       &boot_first_node, noFunc},

        {"BOOT_DISK_NODE_LIST", NULL,
                Range, Void,
                NODE_RANGES, &fs_node_array_entries,
	        NULL, fs_node_array, noFunc},

        {"RB_DEBUG_SETHOSTNAME", NULL,
                Bool, Void, 
	        0, NULL,
	        NULL, &debug_sethostname, noFunc},

	{ "RPC_WAITTIME", NULL,
		Node, Void,
		0, NULL,
		NULL, &rpc_waittime, nx_check_rpc_waittime},

        {"BOOT_ENET_NODE_LIST", NULL,
                Range, Void,
                NODE_RANGES, &enet_node_array_entries,
	        NULL, enet_node_array, noFunc},

        {"BOOT_HIPPI_NODE_LIST", NULL,
                Range, Void,
                NODE_RANGES, &hippi_node_array_entries,
	        NULL, hippi_node_array, noFunc},

        {"BOOT_IPI3_NODE_LIST", NULL,
                Range, Void,
                NODE_RANGES, &ipi3_node_array_entries,
	        NULL, ipi3_node_array, noFunc},
#endif /* NX */
#ifdef SLL
        {BM_ENABLE_FORK_REMOTE, NULL,
                Bool, Void, 
	        0, NULL,
	        NULL, &fork_remote, noFunc},
        {BM_FORK_REMOTE_TIMEOUT, NULL,
                ULong, Void, 
	        0, NULL,
	        NULL, &fork_remote_timeout, sll_check_bm_fork_remote_timeout},
        {BM_FORK_REMOTE_SHM, NULL,
                Bool, Void, 
	        0, NULL,
	        NULL, &fork_remote_shm, noFunc},
#endif /* SLL */
#ifdef RAMDISK
        {"NUM_BUFFERS", NULL,
	        ULong, Void,
	        0, NULL,
                NULL, &num_buffers_bmagic, noFunc},
#endif
#if BUG_COMPAT
	{ "ALLOW_PAGE_0_ACCESS", NULL,
		Bool, Void,
		0, NULL,
		NULL, &allow_page_0_access, noFunc},
#endif
 	{ "DEBUG_NODE", NULL,
		Node, Void,
		0, NULL,
		NULL, &debug_node, noFunc},
 	{ "DEBUG_ROOT_DEVICE_NODE", NULL,
		Node, Void,
		0, NULL,
		NULL, &debug_root_device_node, noFunc},
	{ "COMPLAIN_UNKNOWN", NULL,
		Bool, Void,
		0, NULL,
		NULL, &complainUnknown, noFunc},
	{ "INCARNATION", NULL,
		ULong, Void,
		0, NULL,
		NULL, &incarnation, noFunc},
        {"BOGUS_VNODE_PROXY_PANIC", NULL,
                Bool, Void, 
	        0, NULL,
	        NULL, &bogus_vnode_proxy_panic, noFunc},
#if RFC_1323
	{ "HIPPI_BUF_CACHE_SIZE", NULL,
		Long, Void,
		0, NULL,
		NULL, &hippi_buf_cache_size, noFunc},
	{ "TCP_SPACE_SIZE", NULL,
		ULong, Void,
		0, NULL,
		NULL, &tcp_space_size, noFunc},
#endif
	{ "BCACHE_MAXBSIZE", NULL,	/* Generic maximum buffer cache size */
		Long, Void,
 		0, NULL,
		NULL, &bcache_maxbsize, noFunc }, 

	{ "IPI3_BCACHE_MAXBSIZE", NULL,	/* IPI-3 devices maximum buffer cache */
		Long, Void,		/* size */
 		0, NULL,
		NULL, &ipi3_bcache_maxbsize, noFunc },

	{ "BOOT_PAGE0_ACCESS", NULL,
		ULong, Void,
		0, NULL,
		NULL, &page_0_protection, noFunc},
		
	{ "BOOT_MAXUSERS", NULL,
		Long, Void,
		0, NULL,
		NULL, &maxusers, maxusers_init_func },
		
#ifdef PFS
	{ "PFS_ASYNC_DFLT", NULL,
		ULong, Void,
		0, NULL,
		NULL, &pfs_async_dflt, noFunc},
#endif

#ifdef PARACORE
	 /* max number of processes to dump core */
	{ "PARACORE_MAX_PROCESSES", NULL,
		Long, Void,
 		0, NULL,
		NULL, &paracore_max_processes, noFunc },
#endif /* PARACORE */

#if FULLSERVER && !defined(RAMDISK)
	{ "FS_MAX_DEVICE_REQUEST", NULL,
		ULong, Void,
		(512*1024), NULL,
		NULL, &fs_max_device_request, noFunc},
#endif

        { "UX_OLD_THREADS", NULL,
                ULong, Void,
                0, NULL,
                NULL, &old_threads, noFunc},
	0
};

/* 
 * forward declarations - functions used to build pager_node_list
 */
static boolean_t remove_pager_node(node_t *, node_t *, node_t);
static boolean_t add_pager_node(node_t *, node_t *, node_t);
static void build_pager_node_list();
void compact_pager_node_list();

static node_t dummy1;        /* parsing logic needs this in PagerNodeTable */
static boolean_t dummy2;     /* parsing logic needs this in ExportPagingTable */

/*
 * PagerNodeTable and ExportPagingtable are used to parse PAGER_NODE and 
 * EXPORT_PAGING for building pager_node_list. 
 * The reason this is done in seperate tables is because the pager_node_list
 * needs to be built in the following order - 
 *    first build a list of al nodes in the domain (BOOT_NODE_LIST), 
 *    then remove those nodes that import paging (i.e those that have 
 *         PAGER_NODE set) 
 *    and finally add back those nodes that export paging 
 */
struct configVar PagerNodeTable[] = {
	{ "PAGER_NODE", NULL,
		Node, Range,
		0, NULL,
		NULL, &dummy1, remove_pager_node},
        NULL
};

struct configVar ExportPagingTable[] = {
	{ "EXPORT_PAGING", NULL,
		Range, Void,
		0, NULL,
		&dummy2, NULL, add_pager_node},
        NULL
};

/* for an easier time with the kernel debugger */
#define static

static char genErrorMsg[] =
	"Node %d: Warning: %sconfig variable %s: %s%s\n";
static char errorInValue[] = "Node %d: Warning: config variable %s: %s: at value \"%s\"\n";

#define GenericError(node, what, var, s1, s2) \
		printf(genErrorMsg, node, what, var, s1, s2)
#define ValueError(node, var, msg, val) \
		printf(errorInValue, node, var, msg, val)

static int
decode_rangetype(
	char			*vp,
	struct configVar	*var,
	node_t			matchNode,
	struct configVar	*vecval)
{
	char	*errmsg;
	int	count = 0, r = 0, errors = 0;
	node_t	(*save)[2], scan[2][2];
	char	*start;

	save = (node_t(*)[2])var->value;
	for (; vp && *vp; r=1-r, count++) {
		start = vp;
		while (isspace(*vp))
			vp++;
		if (!isdigit(*vp)) {
#ifdef WarnOldPagingConfig
			if (*vp == '<') {
				errmsg = "contains <..>; suspect old paging configuration";
			} else
#endif
			errmsg = "Not a node number";
			goto error;
		}
		scan[r][0] = strtoul(vp, &vp, 0);

		/*
		 * This next check is dubious, at best.  But, there is code
		 * that expects a range to never have a duplicate value,
		 * and this check guarantees it.  Note that this check
		 * does not prevent a vector from having a duplicate value,
		 * to witt:
		 *	<1..10>3;<5>2
		 */
		if (count && scan[1-r][1] >= scan[r][0]) {
			errmsg = "Range list not in increasing order";
			goto error;
		}

		while (isspace(*vp))
			vp++;
		if (*vp == ',' || !*vp) {
			scan[r][1] = scan[r][0];
			if (var->thisNode && matchNode == scan[r][0])
				*var->thisNode = TRUE;
		} else {
			if (*vp != '.' || *(vp+1) != '.') {
				errmsg = "Syntax error (expected .. or ,)";
				goto error;
			}
			vp += 2;
			while (isspace(*vp))
				vp++;
			if (!isdigit(*vp)) {
				errmsg = "Not a node number";
				goto error;
			}
			scan[r][1] = strtoul(vp, &vp, 0);
			if (scan[r][0] > scan[r][1]) {
				errmsg = "Not an increasing .. in range";
				goto error;
			}
			if (var->thisNode &&
			    matchNode >= scan[r][0] && matchNode <= scan[r][1])
				*var->thisNode = TRUE;
			while (isspace(*vp))
				vp++;
			if (*vp != ',' && *vp) {
				errmsg = "Syntax error (expected , after ..)";
				goto error;
			}
		}
		if (var->func != noFunc) {
			node_t	n;
			boolean_t	fr;

			for (n = scan[r][0]; n <= scan[r][1]; n++) {
				if (vecval)
					fr = (*var->func)((void *)&n,
							  vecval->value,
							  matchNode);
				else
					fr = (*var->func)(NULL,
							  (void *)&n,
							  matchNode);
				/* allow func to override */
				if (var->thisNode)
					*var->thisNode = fr;
			}
		}
		if (*vp)		/* skip "," if there was one */
			vp++;
		if (save) {
			if (count == var->size) {
				/* out of space, do nothing */
			} else if (count > var->size) {
				/* overflow */
				ValueError(matchNode,
					var->name,
					"Overflow max size of range",
					start);
				errors++;
				save = (node_t(*)[2])NULL;
			} else {
				(*save)[0] = scan[r][0];
				(*save)[1] = scan[r][1];
				save++;
			}
		}
	}

	if (var->outsize != NULL)
		*var->outsize = count > var->size ? var->size : count;

	return errors;

error:
	ValueError(matchNode, var->name, errmsg, start);
	return ++errors;
}


static int
decode_simpletype(
	char			*vp,
	struct configVar	*var,
	node_t			matchNode,
	struct configVar	*vecval)
{
	char	*errmsg;
	boolean_t	store = TRUE;
	union	simpletype v;

	if (var->outsize != NULL)
		*var->outsize = 1;		/* default */

	while (isspace(*vp))
		vp++;

	switch (var->valType) {
	case ULong:
		if (!isdigit(*vp)) {
			errmsg = "Not an unsigned number";
			goto error;
		}
		v.ul = (ulong)strtoul(vp, NULL, 0);
		break;
	case Long:
		if (!isdigit(*vp) && *vp != '-') {
			errmsg = "Not a number";
			goto error;
		}
		v.l = (long)strtol(vp, NULL, 0);
		break;
	case Bool:
		/*
		 * Uses cheap tests because I'd rather not import strncasecmp.
		 * And I consider this even uglier:
		 *	(!strcmp(*vp, "true") || !strcmp(*vp, "TRUE"))
		 */
#ifdef AllowAnyBoolean
		/*
                 * Accept: 1|true|on or 0|false|off, any case
                 */
                if (index("oO", *vp))		/* skip o of on/off */
                        vp++;
                if (index("1tTnN", *vp))	/* 1, true, on */
                        v.b = TRUE;
                else if (index("0fF", *vp))	/* 0, false, off */
                        v.b = FALSE;
#else
		/*
		 * Accept: "true" or "false", any case (and allow abreviations)
		 */
		if (index("tT1", *vp))
			v.b = TRUE;
		else if (index("fF0", *vp))
			v.b = FALSE;
#endif
		else {
			errmsg = "Not a boolean value";
			goto error;
		}
		break;
	case Node:
		if (!isdigit(*vp)) {
			errmsg = "Not a node number";
			goto error;
		}
		v.n = (node_t)strtoul(vp, NULL, 0);
		if (var->thisNode && v.n == matchNode)
			*var->thisNode = TRUE;
		break;
	case Char:
		v.c = *vp;
		break;
	case String:
		v.s = vp;
		break;
	case Range: 
		return decode_rangetype(vp, var, matchNode, vecval);
	case Void:
		break;
	default:
		errmsg = "Unknown data type";
		goto error;
	}

	if (var->func != noFunc) {
		if (vecval)
			store = (*var->func)((void *)&v,
					     vecval->value,
					     matchNode);
		else
			store = (*var->func)(NULL,
					     (void *)&v,
					     matchNode);
		/* allow func to override */
		if (var->thisNode)
			*var->thisNode = store;
	}

	if (store && var->value) {
		switch (var->valType) {
		case ULong:
			*(ulong *)var->value = v.ul;
			break;
		case Long:
			*(long *)var->value = v.l;
			break;
		case Bool:
			*(boolean_t *)var->value = v.b;
			break;
		case Node:
			*(node_t *)var->value = v.n;
			break;
		case Char:
			*(char *)var->value = v.c;
			break;
		case String: {
			strncpy((char *)var->value, v.s, var->size);
			((char *)var->value)[var->size] = '\0';
			if (var->outsize)
				*var->outsize = strlen((char *)var->value);
			break;
		   }
		case Void:
			break;
		}
	}
	return 0;

error:
	ValueError(matchNode, var->name, errmsg, vp);
	return 1;
} 


/*
 * Sadly, there are a few hacks that only support Range and Node selectors,
 * and don't support Range or String for values.
 */
static int
decode_vector(
	char			*vp,
	struct configVar	*var,
	node_t			matchNode)
{
	struct configVar cfsel, cfval;
	int	errors = 0;
	char	*next, *sel, *val;
	boolean_t	match, hadmatch = FALSE;
	union simpletype	xval;

	/* variable description for decoding selector */
	bzero(&cfsel, sizeof(cfsel));
	cfsel.name = var->name;
	cfsel.valType = var->selType;
	cfsel.thisNode = &match;
	cfsel.func = var->func;
	cfsel.value = (void *)NULL;	/* not used; done via func */

	/*
	 * variable description for value
	 * Don't include function or thisNode, and subvert value
	 */
	cfval = *var;
	cfval.func = noFunc;
	cfval.thisNode = (void *)NULL;
	/* use a temporary location; won't work for Range or for String */
	cfval.value = (void *)&xval;

	next = vp;
	while (next && *next != '\0') {
		size_t len;
		match = FALSE;

		sel = next;
		next = index(next, ':');
		if (next)
			*next++ = '\0';

		while (isspace(*sel))
			sel++;
		if (*sel == '<') {
			sel++;
			/* make sure we don't have <<> */
			len = strspn(sel, "0123456789,.");
			if (!len && sel[len] != '>') {
				/* print unbalanced <> */
				errors++;
				break;
			}
			val = sel + len;
			*val++ = '\0';		/* clobber > */
			while (isspace(*val))
				val++;
		} else {
			val = sel;
			sel = NULL;
			match = TRUE;
		}

		/*
		 * First decode the value into a temporary location,
		 * as we might want to send it with a function call.
		 */
		if (decode_simpletype(val, &cfval, matchNode, NULL)) {
			errors++;
			break;
		}
		/*
		 * Now, decode the selector, allowing function calls through.
		 * This will fill in as to if we've a match.
		 */
		if (sel) {
			if (decode_simpletype(sel, &cfsel, matchNode, &cfval)) {
				errors++;
				break;
			}
		} else {
			/* we must do function call for default values */
			if (var->func != noFunc) {
				/* match = */
				match = (*var->func)((void *)NULL,
						     cfval.value,
						     matchNode);
				/* allow func to override */
				if (var->thisNode)
					*var->thisNode = match;
			}
		}
		if (match) {
			/* If we've a real match, then remember it */
			if (sel)
				hadmatch = TRUE;
			/*
			 * Only store new value if
			 *	this is a real match,
			 * or	(this is a default value) we've to yet have
			 *      a real match
			 */
			if ((sel || !hadmatch) && var->value)
				*(union simpletype *)var->value =
					*(union simpletype *)cfval.value;
			if (var->thisNode)
				*var->thisNode = match;
		}
	}
	return errors;
}

int
parse_boot_config(
	struct configVar	*table,
	node_t			matchNode,
	char			*bootstring)
{
	struct configVar *c;
	char	*nextline, *var, *value;
	int	errors = 0;

	nextline = bootstring;
	while (nextline && *nextline != '\0') {
		var = nextline;
		nextline = index(nextline, '\n');
		if (nextline)
			*nextline++ = '\0';	/* clobber nl */
		value = index(var, '=');
		if (!value) {
			/* Don't complain about empty lines */
			while (isspace(*var))
				var++;
			if (*var != '\0') {
				GenericError(matchNode, "No assignment in ",
						var, "ignored", "");
				errors++;
			}
			continue;
		} else
			*value++ = '\0';

		for (c = table; c && c->name; c++) {
			if (strcmp(var, c->name) == 0)
				break;
		}

		if (!c || !c->name) {
			if (complainUnknown) {
				GenericError(matchNode, "Unknown ",
						var, "ignored", "");
				errors++;
			}
			continue;
		}

		if (c->rawValue) {
			if ( print_debug_messages ) {
				GenericError(matchNode,
					"Overriding previous value of ", var,
					"new value: ", value);
			}
		} else {
			/* first time initialization */
			if (c->thisNode)
				*c->thisNode = FALSE;
		}
		c->rawValue = value;

		switch (c->selType) {
		case Void:
			/* Handle non-vector simple types */
			errors += decode_simpletype(value, c, matchNode, NULL);
			break;

		/* We have a vector */

		case Node:
		case Range:
			if (c->valType == Range || c->valType == String) {
				/* not currently possible */
				GenericError(matchNode, "", var,
						"No support for Range or String with selector: ignored", "");
				errors ++;
			} else
				errors += decode_vector(value, c, matchNode);
			break;

		/* support not tested for other types! */

		default:
			GenericError(matchNode, "", var,
				"No support for selector type: ignored", "");
			errors++;
			break;
		}
	}

	return errors;
}


#ifdef  NX

int bootmagic_trap(where)
	char    *where;
{
	asm("trap r0,r25,r0");
}


int trap_host_get_boot_info( priv_host, boot_info )
	host_t    priv_host;
	char      *boot_info;
{
	int        boot_stuff_len;

	boot_stuff_len = bootmagic_trap(boot_info);

	if (boot_stuff_len > 0)
		return (KERN_SUCCESS);
	else
		return (KERN_FAILURE);
}

#define host_get_boot_info      trap_host_get_boot_info

#endif  /* NX */


int
boot_config_parse_vars(
	char	*bootstring)
{
	return parse_boot_config(configTable, this_node, bootstring);
}

void
boot_config_init(void)
{
	kernel_boot_info_t  *mkbootinfo;	
	kern_return_t       kr;
	int                 errors;


	kr = vm_allocate(mach_task_self(),
			 (vm_address_t *)&mkbootinfo,
			 (vm_size_t)(sizeof(kernel_boot_info_t)),
			 TRUE);
	if (kr != KERN_SUCCESS) {
	    panic("boot_config_init: Unable to allocate memory for configuration information. kr =", kr);
	}

	kr = host_get_boot_info(privileged_host_port, mkbootinfo);
	if (kr != KERN_SUCCESS) {
		printf("Node %d: Alert: Unable to get boot configuration: host_get_boot_info returned %x\n",
			this_node, kr);
		goto out;
	}

	print_debug(("Boot config info from the kernel: %s\n", mkbootinfo));

	errors = boot_config_parse_vars( (char *)mkbootinfo);
	if (errors > 0)
		printf("Node %d: Warning: boot config contained %d error%s\n",
			this_node, errors, errors==1 ? "" : "s");
        if (this_node == root_fs_node) {
                /* 
                 * We build pager_ndoe_list only on the node which can 
                 * run the table() call for TBL_PGINFO
                 */
                /* Build a list of nodes that may have paging files - 
                 * pager_node_list.  Nodes in this list are contacted for 
                 * obtaining paging info.
                 * pager_node_list = (all nodes) - (nodes importing paging)
                 *                    + (nodes epxorting paging)
                 *
                 * build_pager_node_list first adds all nodes in the domain 
                 * to this list.
                 * Subsequently, we parse the boot-config info looking 
                 * for the PAGER_node boot variable. The function 
                 * remove_pager_node is called by the parsing logic to remove 
                 * nodes that import paging
                 * Finally, we need to add back the nodes that export their
                 * pager. This is done by searching for EXPORT_PAGING and
                 * calling add_pager_node.
                 */

                build_pager_node_list();

                /*
                 * need to get the bootinfo each time because 
                 * parse_boot_config trashes the string!
                 */
                kr = host_get_boot_info(privileged_host_port, mkbootinfo);
                if (kr != KERN_SUCCESS) {
                        printf("Node %d: Alert:Unable to get boot configuration",
                               this_node);
                        printf(": host_get_boot_info returned %x\n", kr);
                        goto out;
                }
                
                errors = parse_boot_config(PagerNodeTable, this_node, 
                                           (char *)mkbootinfo);
                if (errors > 0) {
                        printf("Node : %d: Warning: parsing boot config ",
                               this_node);
                        printf("for paging information contained %d error%s\n", 
                               errors, errors == 1 ? "" : "s");
                
                }

                /*
                 * need to get the bootinfo each time because 
                 * parse_boot_config trashes the string!
                 */
                kr = host_get_boot_info(privileged_host_port, mkbootinfo);
                if (kr != KERN_SUCCESS) {
                        printf("Node %d: Alert:Unable to get boot configuration",
                               this_node);
                        printf(": host_get_boot_info returned %x\n", kr);
                        goto out;
                }
                
                errors = parse_boot_config(ExportPagingTable, this_node, 
                                          (char *)mkbootinfo);
                if (errors > 0) {
                        printf("Node : %d: Warning: parsing boot config ",
                               this_node);
                        printf("for paging information contained %d error%s\n", 
                               errors, errors == 1 ? "" : "s");
                
                }
                compact_pager_node_list();
        }
    out:
	(void) vm_deallocate(mach_task_self(),
			     (vm_address_t)mkbootinfo,
			     (vm_size_t)(sizeof(kernel_boot_info_t)));
}

/*
 * Purpose : Build an initial list of nodes that may have paging files. 
 *           Nodes in this list are contacted for obtaining paging info.
 *           To start with, all nodes in the domain are added to this list
 *           Subsequently, as the PAGER_NODE config variable is parsed
 *           nodes that import paging are removed from this list.
 *           After that, nodes that EXPORT_PAGING are added to the list.
 */
void
build_pager_node_list()
{
        int i;

        for (i=0; i < node_array_entries; i++) {
                max_pager_node_list_ent += node_array[i][1] - 
                                           node_array[i][0] + 1;
        }
        pager_node_list = (node_t *)malloc(max_pager_node_list_ent * 
                                           sizeof(node_t));
        pager_node_list_ent = 0;

        for (i=0; i < node_array_entries; i++) {
                pager_node_list[pager_node_list_ent++] = node_array[i][0];

                while (pager_node_list[pager_node_list_ent - 1] < 
                       node_array[i][1]) {
                        pager_node_list[pager_node_list_ent] = 
                          pager_node_list[pager_node_list_ent - 1] + 1;
                        pager_node_list_ent++;
                }
        }
}


/*
 * Purpose : remove a node from the list of nodes that may have paging files
 *           This function will be called by the boot config parsing code
 *           when it parses the "PAGER_NODE" config variable in the
 *           PagerNodetable
 * Args : nodep - pointer to the node that imports paging
 *        misc1, misc2 - of no consequence in this routine. 
 *
 * We basically want to remove all nodes that import paging from the list 
 * of nodes that may have paging files 
 */
boolean_t
remove_pager_node(nodep, misc1, misc2)
        node_t   *nodep;
        node_t   *misc1;
        node_t   misc2;
{
        int i;

        for (i=0; i < pager_node_list_ent; i++) {
                if (pager_node_list[i] == *nodep) {
                        pager_node_list[i] = NONODE;
                        break;
                }
        } 
        return(FALSE);
}

/*
 * Purpose : add a node to the list of nodes that may have paging files
 *           This function will be called by the boot config parsing code
 *           when it parses the "EXPORT_PAGING" config variable in the
 *           table ExportPagingTable
 * Args : nodep - pointer to the node that imports paging
 *        misc1, misc2 - of no consequence in this routine. 
 *
 * We basically want to add nodes that are exporting their  pager to other
 * nodes to the list of nodes that may have paging files 
 */
boolean_t
add_pager_node(misc1,nodep,misc2)
        node_t   *misc1;
        node_t   *nodep;
        node_t   misc2;
{
        int i;
        int free_slot = -1;
        
        for (i = 0; i < pager_node_list_ent; i++) {
                if (pager_node_list[i] == NONODE) {
                        free_slot = i;
                } else if (pager_node_list[i] == *nodep) {
                        /* node is already in the list */
                        return(FALSE);
                }
        } 
        if (free_slot >= 0) {
                pager_node_list[free_slot] = *nodep;
                return(FALSE);
        }
        if (pager_node_list_ent >= max_pager_node_list_ent) {
                printf("Warning:Array of nodes that provide paging info is ");
                printf("too small - \n      some nodes may not be contacted");
                printf(" for paging info\n");
                return(FALSE);
        }
        pager_node_list[pager_node_list_ent++] = *nodep;
        return(FALSE);
}

/*
 * This function compacts the pager_node_list array. When 
 * remove_pager_node is called some entries in the list may get set
 * to -1. This functions compacts the list to have valid node numbers from
 * 0 to pager_node_list_ent
 */
void
compact_pager_node_list()
{
        int cur = 0;
        int ahead = 0;
        
        /* 
         * The algorithm used is -
         * 0..cur                        contains valid node numbers
         * cur..ahead                    contains invalid node nos.
         * ahead..pager_node_list_ent      has'nt been scanned yet
         */
        while (cur < pager_node_list_ent) {
                if (pager_node_list[cur] == NONODE) {
                        /* 
                         * skip ahead to the next valid entry, 
                         * starting from where we previously ended 
                         * the look-ahead search
                         */
                        ahead = max(cur, ahead);
                        while ((++ahead < pager_node_list_ent) && 
                               (pager_node_list[ahead] == NONODE));
                        if (ahead == pager_node_list_ent) {
                                /* we are done - break out of the while loop */
                                break ; 
                        }
                        pager_node_list[cur] = pager_node_list[ahead];
                        pager_node_list[ahead] = NONODE;
                }
                cur++;
        }
        pager_node_list_ent = cur;
}

#ifdef WarnOldPagingConfig
/*ARGSUSED*/
boolean_t
importPagingWarn(void *s, void *v, node_t n)
{
	printf("Node %d: Warning: IMPORT_PAGING is obsolete; suspect old paging configuration\n",
		this_node);
	return FALSE;
}
#endif
#endif	/* NORMA_IPC */

boolean_t
maxusers_init_func(void *s, long *maxusers_addr, node_t n)
{

	maxusers = *maxusers_addr;

	if (maxusers > 0) {
	    nproc = 20 + 8 * maxusers;
	    nvnode = (((nproc+16+maxusers)+32)*2) + (NMOUNT + ((8*NMOUNT)*2));
	    nfile = 16 * (nproc + 16 + maxusers)/10 + 32;
	    ncallout = 16 + nproc;
	    select_max_elements = 1024 + nproc * 4;
	    nchsize = nvnode * 11 / 10;
#if	FULLSERVER
	    nclist = 60 + 12 * maxusers;
#else	/* FULLSERVER */
	    nclist = 0;
#endif	/* FULLSERVER */
#if     QUOTA
	    nquota = (maxusers * 9) / 7 + 3;
	    ndquot = nvnode + (maxusers * NMOUNT) / 4;
#endif
	}
	return(FALSE);
}

#ifdef NX
void
reset_boot_magic_var(string, stringlen)
char *string;
int   stringlen;
{
int errors;

    errors = boot_config_parse_vars(string);
}
#endif /* NX */
