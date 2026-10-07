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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: server_init.c,v $
 * Revision 1.37  1995/02/11  00:02:17  stans
 *  'lint' picking with typedefs for a clean compile.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW05 sats
 *
 * Revision 1.36  1995/02/01  22:28:43  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.35  1994/11/18  20:49:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.34  1994/10/25  18:55:10  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo, but could expose other code overwriting text segment
 *  Benefit or PTS #: 11380
 *  Testing: memsnoop test, EATs sched, controlc, rmcall, os_interfaces
 *  Module(s): server/uxkern/server_init.c
 *
 * Call vm_protect() to protect the server's text segment against overwriting.
 *
 * Revision 1.33  1994/08/31  22:47:51  mtm
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
 * Revision 1.31.2.1  1994/08/10  17:44:24  flb
 * Reviewer: cfj
 * Risk: low
 * Benefit or PTS # 10173
 * Testing: System Boot with large MAGIC.MASTER
 * Module(s): uxkern/boot_config.c,uxkern/server_init.c,user/etc/load_level/osf1_dep.c
 *
 * Revision 1.31  1994/07/11  20:17:23  dbm
 * Fixed BOOT_IPI3_NODE_LIST bootmagic logic to work correctly. Removed
 * checks from this module.
 *  Reviewer: None.
 *  Risk:Low
 *  Benefit or PTS #:10105
 *  Testing: Specific test case, configured IPI3 node.
 *  Module(s):
 *         uxkern/server_init.c
 *         vfs/vfs_bio.c
 *         ufs/ufs_vfsops.c
 *
 * Revision 1.30  1994/06/28  23:20:23  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.29  1994/04/28  19:17:51  chrisp
 * Changes for introduction of tncgen and support of i386 builds.
 *
 *  Reviewer: dleslie, cfj
 *  Risk: M
 *  Benefit or PTS #: 9188
 *  Testing: Builds and tested on i386 platform.
 *  Module(s): server_init.c syscall_subr.c ux_server_loop.c
 *
 * Revision 1.28  1994/01/12  17:46:04  jlitvin
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
 * Revision 1.27  1994/01/04  15:36:40  cfj
 * Merge the page 0 protection change from the R1_2 branch.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.26  1993/12/10  22:32:14  nina
 * Fixed bugs that prevented Paragons from being used
 * as NFS clients if the boot node is not a network
 * server node.  See #6831, #6719, #7421, #7422, #7423
 * #7424 and #7426.  If a network server node that had
 * no physical file systems attached was used as the client
 * for an NFS mount, the system would hang doing NFS
 * reads/writes waiting for a buffer to free up. This
 * was due to the fact that NFS uses buffer cache entries
 * allocated for the physical file systems on a given node.
 * In NX servers, no memory was set aside for buffer cache
 * entries if no physical file systems were detected in
 * startup().  startup() was modified to set aside memory
 * for buffers if a) there are physical file systems attached
 * to the node or b) the node has network interface hardware.
 *
 *
 *  Reviewer:bolsen, dbm
 *  Risk:Medium
 *  Benefit or PTS #:#7426
 *  Testing:Lachman NFS main suite, various configuratins
 *  Module(s):./server/uxkern/server_init.c
 *
 * Revision 1.25  1993/12/03  20:28:59  paul
 * Various fixes to RPM support, plus support for global setting of Timezone
 *
 * Removed declaration of debug_rpmoffset
 *
 *  Reviewer: John Litvin (jlitvin@ssd.intel.com) Brent Olsen (bolsen@locus.com)
 *  Risk: Moderate
 *  Benefit or PTS #: :Fixes bug #s 3503 5303 6029 7299
 *  Testing: functionality checked on olympus.sd.locus.com w/RPM support
 *  Module(s): server/uxkern/server_init.c
 *
 * Revision 1.24.2.3  1994/01/04  15:31:08  cfj
 * Change the protections on the server's page 0 from VM_PROT_READ to
 * VM_PROT_NONE so that a fault is generated if the server ever attempts to
 * dereference an address in its own page 0.  This is a debugging tools to
 * attempt to catch some of the catch_exception_raise panics.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.24.2.2  1993/12/21  22:30:15  dbm
 *  Reviewer: Nina Lepack and B Olsen (Locus)  Dave Minturn
 *  Risk: Medium
 *  Benefit or PTS #:7426
 *  Testing: Tested under 1.2 for specific test cases.  See log for
 *  	  revision 1.26 for specific description.  This is the
 * 	  1.2 checkin for bug fix in 1.26.
 *  Module(s):
 * 		server_init.c
 *
 * Revision 1.24  1993/10/29  11:57:10  paul
 * Add support for setting and using the RPM distributed time-of-day clock.
 *
 * Revision 1.23  1993/10/21  23:37:33  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.22  1993/10/11  20:57:16  cfj
 * Remove RB_WIRE_SERVER bootmagic since that was intended as an experiment
 * and should not have been left in.
 *
 * Revision 1.21  1993/07/28  21:22:35  nandy
 * Changed rpc_waittime to 24hrs.
 *
 * Revision 1.20  1993/07/20  21:43:40  nandy
 * Changed the tupe of rpc_waittime from long to mach_msg_timeout_t.
 *
 * Revision 1.19  1993/07/19  22:59:54  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.18  1993/07/14  18:44:06  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.9  1993/07/01  21:05:51  cfj
 * Adding new code from vendor
 *
 * Revision 1.17  1993/06/01  23:28:42  nandy
 * Added bootmagic RPC_WAITTIME to specify RPC timmeout values for TNC calls.
 * RPC_WAITTIME=-1 will turn off the timeout capability.
 *
 * Revision 1.16  1993/05/18  16:58:54  cfj
 * Properly split the RAMDISK bootmagic code between boot_config.c and server_init.c
 *
 * Revision 1.15  1993/05/08  00:11:37  cfj
 * Merge hell.
 *
 * Revision 1.14  1993/05/06  19:32:50  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.7  1993/05/03  17:53:34  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.28  93/09/17  10:48:33  chrisp
 * [SPE 0030] Generic Spanning Trees: Include dpvproc_struct.h rather than
 * 	dpvproc.h -- so that make clean works.
 * 
 * Revision 2.24  94/02/03  11:02:19  dnoveck
 *      Changes for per-node buffer-cache block size: call bio_startup
 *      early on to set up size, clear up the sizing logic.
 *
 * Revision 2.27  93/07/13  16:13:10  slively
 * 
 * 	Revision 2.23  93/07/01  13:49:44  loverso
 * 	Warn about missing values of CUBE_IO_NODE_LIST.
 * 
 * Revision 2.26  93/06/25  11:26:50  slively
 * Backout the LITE server changes.  Remove #if UFS and #if RAMDISK.
 * 
 * Revision 2.25  93/06/22  20:08:38  slively
 * Support for LITE server. #if UFS and #if RAMDISK sections.
 * 
 * Revision 2.24  93/06/02  12:37:38  yazz
 * For Sys V IPC under TNC declare scalar holding the svipc node number
 * and cause it to default to the root filesystem node in the absence
 * of a specific bootmagic string.
 * 
 * Revision 2.23  93/04/29  14:06:59  klh
 * 	Revision 2.20  93/04/08  11:42:50  loverso
 * 		Handle "-h" boot option for standalone servers.
 * 		[1992/09/23  12:42:52  barbou]
 * 
 * 		Handle "-W" boot option to turn off wired threads. (loverso)
 * 
 * 	Revision 2.19  93/03/22  23:58:30  condict
 * 		Moved call to ux_server_init down past the processing of command line
 * 		args.  Otherwise, the "-w" arg (wired threads) is processed too late.
 * 
 * 	Revision 2.18  93/02/26  16:37:22  durriya
 * 		ignore SIGWINCH signal - don't want the server to panic if you resize
 * 		the xterm in which it has been started (durriya)
 * 
 * 	Revision 2.17  93/02/02  15:23:57  rabii
 * 		[92/12/29]  13:08:34  chrisp
 * 		For TNC, remove call to tnc_startup().
 * 		Remove static alocation of vproc, pvproc, cfree, file and vnode tables -
 * 			these are all now dynamically allocated using zalloc().
 * 		Ensure that quota structures are never allocated - they're simply not
 * 			required.
 * 
 * 	Revision 2.16  93/01/14  13:10:22  loverso
 * 		Moved boot config code to uxkern/boot_config.c.
 * 
 * Revision 2.22  93/03/22  21:20:38  yazz
 * OSF lock changes exposed an argument parsing bug: Do not init
 * server thread processing until after command line arguments are
 * processed.  The "-w" flag forces wired threads and so args must
 * be processed before threads are created.
 * 
 * Revision 2.21  92/12/29  13:08:34  chrisp
 * For TNC, remove call to tnc_startup().
 * Remove static alocation of vproc, pvproc, cfree, file and vnode tables -
 * 	these are all now dynamically allocated using zalloc().
 * Ensure that quota structures are never allocated - they're simply not
 * 	required.
 * 
 * Revision 2.20  92/12/08  10:56:14  chrisp
 * Change NODE_RANGES from 20 to 4K and define MAX_BOOT_OPTIONS_SIZE
 * 	as KERNEL_BOOT_INFO_MAX, the maximum size available from the mk.
 * 
 * Revision 2.19  92/12/02  11:13:48  chrisp
 * Allocate the vproc hash table dynamically.
 * For TNC, call tnc_startup() before allocating system tables so that
 * 	the number of vprocs (nvproc) can be determined dynamically.
 * 
 * Revision 2.18  92/11/23  16:05:59  klh
 * 	Revision 2.14  92/11/12  22:22:30  loverso
 * 		Fix declaration of mach_init_sleep().
 * 
 * 	Revision 2.13  92/11/11  17:26:06  condict
 * 		Remove unnecessary and mysterious calls to ux_thread_wire/unwire.
 * 		[92/11/11            condict]
 * 
 * Revision 2.17  92/11/04  11:24:39  chrisp
 * For TNC, don't catch FPU exceptions in the server.
 * 
 * Revision 2.16  92/10/08  11:34:14  roman
 * Add procedure declaration for a clean compile.
 * 
 * Revision 2.15  92/10/06  12:23:03  roman
 * Fix RCS comments.
 * 
 * Revision 2.14  92/10/05  14:02:08  klh
 * 	Revision 2.10  92/09/29  16:54:45  rabii
 * 		Rationalized method of computing buffers; servers always 
 *		fully populate the buffer pool, if MAPPED_FILES we use 
 *		fewer.  (dwm; #404).
 * 		Also, catch_signals should set handlers for 32 signals, not 31.
 * 
 * 	Revision 2.9  92/09/11  09:29:52  rabii
 * 		Modified boot config parser to handle cases when a table is to
 *		be made from the "<..>" type input (rabii)
 * 
 * 		Added new boot config option ACCEPT_PROCS to tell non PM 
 *		servers if they should register to rproc (rabii)
 * 
 * Revision 2.13  92/09/29  08:24:31  roman
 * Change type of node_array to be node_t.
 * 
 * Revision 2.11  92/08/06  16:43:29  roman
 * Fixes to allow builds without REMOTE_PROC.
 * 
 * Revision 2.10  92/08/06  13:36:37  klh
 * 	Revision 2.7  92/07/28  19:46:36  rabii
 * 		Revision 3.17  92/04/08  20:45:02  barbou
 * 		Fix for bug #105: request dead name notification for task 
 * 		ports. (loverso)
 * 
 * 		Added initialization for new rproc linked list and remove extra
 * 		boot variables (rabii)
 * 
 * 	Revision 2.6  92/07/16  09:56:45  rabii
 * 		Added fix to routine printversions to correctly initialize 
 * 		old_version to FALSE as requested by intel. 
 * 
 * Revision 2.9  92/07/10  09:14:27  chrisp
 * For MAP_UAREA, initialize p_shared_mutex in each proc structure.
 * 
 * Revision 2.8  92/07/07  13:07:45  klh
 * 	Revision 2.5  92/06/30  22:48:10  loverso
 * 		Modified emul_shared_init to use remote_vnode_pager_get when 
 * 		import_paging is turned "on". (rabii)
 * 
 * Revision 2.7  92/06/18  12:12:09  roman
 * Change uses of isspace() to account for fact that isspace() returns
 * 	true for 'n'.
 * 
 * Revision 2.6  92/06/10  16:44:32  roman
 * Get rid of TNC kludge to server_init cause by v0.89 makefile integration
 * error that caused MAP_UAREA build problems. The problem was fixed in the
 * makefiles for v0.9.
 * 
 * Revision 2.5  92/06/05  15:42:41  roman
 * Temporary kludge added to get TNC to compile until sys/server_build.h
 * 	is correctly generated.
 * 
 * Revision 2.4  92/05/26  18:20:42  pjg
 * 	Fix parameter to call to vm_map early in main()
 * 
 * Revision 2.3  92/05/26  13:54:12  pjg
 * 	added proper declaration for mach_host_self (rabii)
 * 
 * Revision 2.2  92/05/24  19:52:33  pjg
 * 	Adapted to OSF/1 AD.
 * 
 * Revision 3.3  92/03/24  21:04:16  barbou
 * Fix for bug #117: protect the page 0 of the server.
 * 
 * Revision 3.2  92/03/23  18:04:36  condict
 * Move lvprobe initialization call here from init_main.c.
 * 
 * Revision 3.1  92/03/20  15:08:52  jose
 * Added code to extract the norma_mk_version (barbou).
 * Fixed amount of memory printed
 * on the buffer cache size boot message.
 * 
 * Revision 3.0  92/03/13  15:18:05  condict
 * New main program and low-level initialization of the server.  Corresponds
 * to locore.s and some machine-dependent functions in the integrated kernel.
 * 
 * $EndLog$
 */
/*
 * Main function and initialization code for the OSF/1 server.  Contains
 * that which differs substantially from the integrated OSF/1 kernel
 * bootstrap code.  This file is roughly equivalent to locore.s, but also
 * contains the following functions from the indicated integrated kernel
 * source files:
 *
 *	vm_set_page_size()	vm/vm_resident.c
 *	startup()		i386/machdep.c
 *	size_memory()		i386/i386_init.c
 *
 * The contents of these functions should be kept equivalent to that of the
 * corresponding integrated kernel function.
 *
 */
#include <ufs.h>
#include <map_uarea.h>
#include <mach_afs.h>

#include <sys/types.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#ifdef  TNC
#include <tnc/dpvproc.h>
#else   /* !TNC */
#include <vproc/bpvproc.h>
#endif  /* !TNC */
#include <sys/vnode.h>
#include <sys/specdev.h>
#include <sys/mount.h>
#include <kern/parallel.h>
#include <kern/lock.h>
#include <ufs/inode.h>
#include <ufs/fs.h>
#include <ufs/ufsmount.h>
#include <nfs/nfsnode.h>
#if     MACH_AFS
#include <afsint/afsint.h>
#include <afs/lock.h>
#include <afs/afs.h>
#endif  /* MACH_AFS */
#include <sys/buf.h>
#include <sys/systm.h>
#include <sys/reboot.h>
#include <sys/file.h>

#include <uxkern/vm_param.h>
#include <uxkern/import_mach.h>

#ifdef  OSF1_ADFS
#include <uxkern/sthread.h>
#include <norma_ipc.h>
#if NORMA_IPC
#include <mach/norma_special_ports.h>
#endif	/* NORMA_IPC */
#endif	/* OSF1_ADFS */

#if	MAP_UAREA
#include <mach/default_pager_object.h>
#endif	/* MAP_UAREA */

#include "lv.h"
#if 	NLV > 0
extern void lvprobe();		/* Logical Volume Manager initializer */
#endif	/* NLV > 0 */

/*
 *	For arptab:
 */
#include <net/net_globals.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/if_ether.h>

/* Should all server C-threads be wired to a Mach kernel thread? */
extern boolean_t wired_threads;

/* Bootstrap debug messages, syscall tracing, etc: */
extern int	print_debug_messages;
extern int	syscalltrace;
#define print_debug(x) \
	if (print_debug_messages) printf x; else 0

/* Which Mach micro-kernel is running now ? */
int	running_mk_version = -1;	/* standard MK version */
int	running_norma_mk_version = -1;	/* NORMA MK version */

extern dev_t	rootdev;	/* device of the root */
extern char	*rootdev_name;	/* name of the root device */

vm_size_t	page_mask;
int		page_shift;
void		vm_set_page_size();
void		print_versions();

mach_port_t	privileged_host_port;
mach_port_t	host_port;
mach_port_t	device_server_port;
mach_port_t	default_processor_set;
mach_port_t	default_processor_set_name;
mach_port_t	default_pager_port = MACH_PORT_NULL;
mach_port_t	sleep_port = MACH_PORT_NULL;   /* used to implement sleep */

#if	MAP_UAREA
mach_port_t	shared_memory_port = MACH_PORT_NULL;
int		shared_pages_nb = 0, ro_pages_nb = 0, rw_pages_nb = 0;
int		io_pages_nb = IO_PAGES_NB;/* nb of pages for shared_readwrite */
#include "sys/server_build.h"
char		*server_build = SERVER_BUILD;
#endif	/* MAP_UAREA */

extern void	ux_server_init(), mach_init_sleep();
extern mach_port_t mach_host_self();

void		setup_main(), OSF1_main();	/* forward */

#ifdef	SECOND_SERVER
int	second_server = 0;	/* Run under another server ? */
void	catch_signals();	/* forward */
#endif	/* SECOND_SERVER */

#ifdef  RAMDISK
extern int     num_buffers_bmagic;      /* NumBuffers from bootmagic    */
#endif	/* RAMDISK */

struct condition kill_main = CONDITION_INITIALIZER;
struct mutex kill_lock = MUTEX_INITIALIZER;

extern dev_t	parse_root_device();


#ifdef  NX
#define MAX_BOOT_OPTIONS_SIZE   KERNEL_BOOTMAGIC_MAX
#else   /* NX */
#define MAX_BOOT_OPTIONS_SIZE   KERNEL_BOOT_INFO_MAX
#endif  /* NX */

char	boot_options_buf[MAX_BOOT_OPTIONS_SIZE];

#define MAX_EMUL_NAME_SIZE      256
char	emul_name_buf[MAX_EMUL_NAME_SIZE];

extern char	*emulator_name;
extern char	*init_program_name;
extern char	**init_program_args;

#ifdef  OSF1_ADFS
mach_port_t	nameserver_port = MACH_PORT_NULL;
mach_port_t	fileserver_port = MACH_PORT_NULL;
mach_port_t	root_ns_port = MACH_PORT_NULL;
mach_port_t	root_fs_port = MACH_PORT_NULL;
mach_port_t	root_vnode_port = MACH_PORT_NULL;

node_t	this_node = 0;			/* Our node id.  0 if !NORMA_IPC */
node_t	root_fs_node = 0;		/* Node id of root fileserver */
node_t	root_device_node = 0;		/* Node id of root device */
#ifdef NX
node_t  allocator_node = 0;   /* Allocator port */
node_t  boot_first_node = 0;   /* Only for paragon */
int     debug_sethostname = 0; /* Debug sethostname. */

extern  node_t fs_node_array[][2];        /* a series of ranges     */
extern	int    fs_node_array_entries;     /* number of valid ranges */

extern node_t  enet_node_array[][2];		/* nodes with ethernet hdw */
extern int     enet_node_array_entries;      	/* # of nodes with ethernet */
extern node_t  hippi_node_array[][2];		/* nodes with HIPPI hdw */
extern int     hippi_node_array_entries;     	/* # of node with HIPPI */
extern node_t  ipi3_node_array[][2];		/* nodes with IPI3 hdw */
extern node_t  ipi3_node_array_entries;		/* # of nodes with IPI3 */

mach_msg_timeout_t	rpc_waittime = 0x5265c00;		/* Timeout for TNC RPCs */

#endif /* NX */
node_t         ipi3_node=0;			/* Non zero if ipi3_node */
node_t	debug_node = -1;		/* Node id of debug node */
node_t	debug_root_device_node = -1;	/* Node id of debug node root dev */

#ifdef	TNC
node_t	svipc_node = -1;	/* Node id where Sys V IPC's are handled */
#endif  /* TNC */

#if     NORMA_IPC
boolean_t	import_paging = FALSE;
boolean_t	export_paging = FALSE;
node_t		pager_node = -1;	/* remote pager node */
					/* (default and vnode) */
/* Boot config parser */
extern void	boot_config_init(void);
extern int	boot_config_parse_vars(char *);
#endif  /* NORMA_IPC */

#endif  /* OSF1_ADFS */

#ifdef	SECOND_SERVER
void
sig_handler(sig, code, scp)
	int sig;
	int code;
	char *scp;	/* sigcontext */
{
	printf("** SERVER RECEIVED SIGNAL %d. Calling Debugger...\n", sig);
	Debugger("killed");
	second_sigreturn(scp);
}

void
catch_signals()
{
	struct sigvec sv;
	int sig;

	sv.sv_handler = sig_handler;
	sv.sv_mask = -1;		/* mask all signals */
	sv.sv_flags = 0;
	
	/* assume 32 signals ... */
	for (sig = 1; sig <= 32; sig++) {
#ifdef	TNC
		if (sig == SIGFPE)
			continue;
#endif	/* TNC */
                /* ignore resizing of window in which svr is started */
                if (sig == SIGWINCH)
                  continue;

		second_sigvec(sig, &sv, (struct sigvec *)0);
	}
}
#endif	/* SECOND_SERVER */

/*
 * Declare these as initialized data so we can patch them.
 */
int	nbuf = 0;	/* N.B.:  NBUF is ignored! */
#ifdef  BUFPAGES
int     bufpages = BUFPAGES;
#else	/* BUFPAGES */
int     bufpages = 0;
#endif	/* BUFPAGES */
int     show_space = 0;


vm_size_t	mem_size;
int		vm_page_free_count;

/*
 * Determine size of block-buffer cache, print server/micro-kernel version,
 * and allocate memory for most tables.  This is intended to be the same
 * as the machine-dependent startup function in <machine>/machdep.c.
 */
startup()
{
	register vm_offset_t	firstaddr, v;
	vm_offset_t		alloc_addr;
	vm_size_t		size;
	kern_return_t		rc;
	extern char		etext;		/* an ABS symbol from loader */
#ifdef NX
        int                     i, found_fs = 0;
        node_t                  node;
#endif /* NX */

	print_debug((
	"--- startup() -----------------------------------------------\n"));

	print_versions();
#define MEG	(1024*1024)

	printf("physical memory = %d.%d%d megabytes\n",
		mem_size/MEG,
		((mem_size%MEG)*10)/MEG,
		((mem_size%(MEG/10))*100)/MEG);

	/*
	 * Protect the server's own text segment against writing.
	 */
#if defined(i860)
#define SERVER_TEXT_START	0x10000		/* 64K */
#else 
#define SERVER_TEXT_START	vm_page_size
#endif

	rc = vm_protect(mach_task_self(), (vm_address_t)SERVER_TEXT_START,
			(vm_size_t)(((char *)&etext) - SERVER_TEXT_START),
			FALSE, VM_PROT_READ|VM_PROT_EXECUTE);

	ASSERT(rc == KERN_SUCCESS);
#undef SERVER_TEXT_START

#define	valloc(name, type, num) \
	    (name) = (type *)v; v = (vm_offset_t)((name)+(num))
#define	valloclim(name, type, num, lim) \
	    (name) = (type *)v; v = (vm_offset_t)((lim) = ((name)+(num)))

	/*
	 *	We make two passes over the table allocation code.
	 *	The first pass merely calculates the size needed
	 *	for the various data structures.  The second pass
	 *	really allocates memory and then sets the actual
	 *	addresses.  The code must not change any of the
	 *	allocated sizes between the two passes.
	 */
	firstaddr = 0;
	for (;;) {
	    v = firstaddr;	    
	    vn_maxprivate = max(sizeof(struct inode),
				sizeof(struct nfsnode));
#if     MACH_AFS
	    vn_maxprivate = max(vn_maxprivate,
				sizeof(struct vcache) - sizeof(struct vnode));
#endif	/* MACH_AFS */
	    /* vnode table now zalloc'd - see vfsinit() */
	    /* file table now zalloc'd - see file_table_init() */
	    valloclim(proc, struct proc, nproc, procNPROC);
	    /* cblocks now zalloc'd - see cinit() */
	    valloc(namecache, struct namecache, nchsize);
	    nchsz = rndup(nchsz, MINNCHSZ);
	    valloc(nchash, struct nchash, nchsz);
#if	UFS
	    inohsz = rndup(inohsz, MININOHSZ);
	    valloc(ihead, struct ihead, inohsz);
	    bufhsz = rndup(bufhsz, MINBUFHSZ);
	    valloc(bufhash, struct bufhd, bufhsz);
	    spechsz = rndup(spechsz, MINSPECHSZ);
	    valloc(speclisth, struct spechash, spechsz);
	    valloc(mounttab, struct ufsmount, nmount);
#endif	/* UFS */
	    vprochsz = rndup(nvproc/4, VPROC_NHASH);
	    valloc(vproc_hash, struct vproc *, vprochsz);
	/*
	 *	Use 5% of memory for buffers, unless
	 *	>= 8Mb and not MAPPED_FILES, then 10%.
	 *
	 *	N.B.:  NBUF is completely ignored; For servers, we
	 *	deliberately fully allocate all buffers, since doing
	 *	otherwise is a pointless exercise in multi-level VM.
	 */
	{
		bio_startup();

		if (!bufpages) {
			bufpages = atop(mem_size/20);
#if	!MAPPED_FILES
			if (mem_size >= (8*MEG))
				bufpages <<= 1;
#endif
		}
#ifndef RAMDISK
		if ((nbuf = bufpages * vm_page_size / bcache_maxbsize) < 16)
			nbuf = 16;
#else   /* RAMDISK */
                /*
                 * If we're building a RAMDISK server, then see if the
                 * variable 'num_buffers_bmagic' has been defined. If so,
                 * then force nbuf to this value. Otherwise, still force
                 * nbuf to a fairly low value.
                 */
		nbuf=num_buffers_bmagic;
#endif /* RAMDISK */
		bufpages =
		  (nbuf * bcache_maxbsize + vm_page_size - 1) / vm_page_size;
        }

#ifdef NX

        if( this_node == boot_first_node) {
                        valloc(buf, struct buf, nbuf);
                        found_fs = 1;
        }
        for(i = 0; i < fs_node_array_entries; i++) {
                for ( node = fs_node_array[i][0]; node <= fs_node_array[i][1]; node++) {
                        if(( this_node == node )  && ( found_fs == 0  )) {
                                valloc(buf, struct buf, nbuf);
                                found_fs = 1;
                                break;
                        }
                }
                if( found_fs ) {
                        break;
                }
        }

	/*
	 * If there are no physical file systems
	 * attached to this node, check to see if
	 * it is a network server node.  If it is,
	 * it might be used to "talk" to a NFS server 
	 * because it has network interface hardware.
	 * We must set aside space for the buffer cache
	 * before any reads/writes are directed to any
	 * NFS server. If we don't do this, we will hang
	 * forever waiting for somebody who doesn't
	 * exist to do a brelse().
	 */

	/* check for ethernet interfaces first */
        if ( !(found_fs )) {
		for(i = 0; i < enet_node_array_entries; i++) {
			for (node = enet_node_array[i][0];
			     node <= enet_node_array[i][1];
			     node++) {
				if( this_node == node ) {
					valloc(buf, struct buf, nbuf);
					found_fs = TRUE; 
					break;
				}
			} /* inner for loop */

			if(found_fs) 
	                        break;

		} /* outer for loop */
        } 

	/* if we don't have ethernet, check for HIPPI */
        if ( !(found_fs )) {
		for(i = 0; i < hippi_node_array_entries; i++) {
			for (node = hippi_node_array[i][0];
			     node <= hippi_node_array[i][1];
			     node++) {
				if( this_node == node ) {
					valloc(buf, struct buf, nbuf);
					found_fs = TRUE; 
					break;
				}
			} /* inner for loop */

			if(found_fs) 
	                        break;

		} /* outer for loop */
        } 

	/*
	 * If no  physical file systems and no network interface
	 * hardware, we won't be doing any file system I/O, so
	 * don't configure memory for the buffer cache.
	 */
        if ( !(found_fs )) {
                bufpages = 0;
                nbuf = 0;
                valloc(buf, struct buf, 0);
        }

        found_fs = 0;
#else	/* !NX */
        valloc(buf, struct buf, nbuf);
#endif /* NX */

#if	QUOTA && 0
	    valloclim(quota, struct quota, nquota, quotaNQUOTA);
	    valloclim(dquot, struct dquot, ndquot, dquotNDQUOT);
#endif	/* QUOTA && 0 */
	    {
		extern struct arptab *arptab;
		extern int arptab_size, arptab_bsiz, arptab_nb;

		arptab_size = arptab_bsiz * arptab_nb;
		valloc(arptab, struct arptab, arptab_size);
	    }

	    if (firstaddr == 0) {
		/*
		 *	Size has been calculated; allocate memory.
		 */
		size = (vm_size_t)(v - firstaddr);
		if (vm_allocate(mach_task_self(),
				&alloc_addr,
				size,
				TRUE)
			!= KERN_SUCCESS)
		    panic("startup: no room for tables");
#if	0
		(void) vm_pageable(mach_task_self(),
				   alloc_addr,
				   size,
				   VM_PROT_READ|VM_PROT_WRITE);
#endif
		firstaddr = alloc_addr;
	    }
	    else {
		/*
		 *	Memory has been allocated.  Make sure that
		 *	table size has not changed.
		 */
		if ((vm_size_t)(v - firstaddr) != size)
		    panic("startup: table size inconsistent");
		break;
	    }
	}
	{
		register int    nbytes;

		nbytes = ptoa(vm_page_free_count);
		printf("available memory = %d.%d%d megabytes\n",
			nbytes/MEG,
			((nbytes%MEG)*10)/MEG,
			((nbytes%(MEG/10))*100)/MEG);
		nbytes = ptoa(bufpages);
		printf("using %d buffers containing up to %d.%d%d megabytes of memory\n",
			nbuf,
			nbytes/MEG,
			((nbytes%MEG)*10)/MEG,
			((nbytes%(MEG/10))*100)/MEG);
	}
}


void
get_config_info()
{
	mach_port_t	bootstrap_port;
	mach_port_t	reply_port;
	kern_return_t	result;
#ifdef	SECOND_SERVER
	mach_port_type_t port_type;
#endif	/* SECOND_SERVER */

	struct imsg {
		mach_msg_header_t	hdr;
		mach_msg_type_t	port_desc_1;
		mach_port_t		port_1;
		mach_msg_type_t	port_desc_2;
		mach_port_t		port_2;
	} imsg;

	/*
	 * Get our bootstrap port
	 */
	result = task_get_bootstrap_port(mach_task_self(), &bootstrap_port);
	if (result != KERN_SUCCESS)
		panic("get bootstrap port %d", result);

	/*
	 * Allocate a reply port
	 */
	reply_port = mach_reply_port();
	if (reply_port == MACH_PORT_NULL)
		panic("allocate reply port");

	/*
	 * Send a message to it, asking for the host and device ports
	 */
	imsg.hdr.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND,
					    MACH_MSG_TYPE_MAKE_SEND_ONCE);
	imsg.hdr.msgh_size = 0;
	imsg.hdr.msgh_remote_port = bootstrap_port;
	imsg.hdr.msgh_local_port = reply_port;
	imsg.hdr.msgh_kind = MACH_MSGH_KIND_NORMAL;
	imsg.hdr.msgh_id = 999999;

	result = mach_msg(&imsg.hdr, MACH_SEND_MSG|MACH_RCV_MSG,
			  sizeof imsg.hdr, sizeof imsg, reply_port,
			  MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (result != MACH_MSG_SUCCESS)
		panic("mach_msg");

	privileged_host_port = imsg.port_1;
	device_server_port = imsg.port_2;

#ifdef	SECOND_SERVER
	if ((mach_port_type(mach_task_self(), privileged_host_port, 
			    &port_type) == KERN_INVALID_NAME) ||
	    (mach_port_type(mach_task_self(), device_server_port,
			    &port_type) == KERN_INVALID_NAME)) {
		second_server = 1;	/* try as a second server */
		printf("The server is not running standalone. PID = %d.\n",
		       second_getpid());
	}
	if (second_server) {
		if ((privileged_host_port = second_task_by_pid(-1)) == MACH_PORT_NULL)
			panic("failed to get privileged host port");	
		if ((device_server_port = second_task_by_pid(-2)) == MACH_PORT_NULL)
			panic("failed to get device server port");
	}
#endif	/* SECOND_SERVER */

	/*
	 * Get a port to talk to the world and initialize printfs and panics.
	 * NOTE: Printfs are not possible until after this point:
	 */
	{
		decl_simple_lock_data(extern,printf_lock)
		console_init();
		simple_lock_init(&printf_lock);
		panic_init();
	}

	print_debug((
	"--- get_config_info() ---------------------------------------\n"));

	host_port = mach_host_self();

	/*
	 * Lookup our default processor-set name/control ports.
	 */

	(void) processor_set_default(mach_host_self(),
				     &default_processor_set_name);
	(void) host_processor_set_priv(privileged_host_port,
				       default_processor_set_name,
				       &default_processor_set);

	/*
	 * Store the default emulator name in the emul_name_buf.
	 */
	strcpy(emul_name_buf, emulator_name);
	emulator_name = &emul_name_buf[0];
}

void
parse_command_args(argc, argv)
	int     argc;
	char    **argv;
{
	/*
	 * Arg 0 is program name
	 */
	argv++, argc--;

	/*
	 * Process flag arguments:
	 */
	while (argc > 0 && argv[0][0] == '-') {
	    register char *cp = argv[0];
	    register char c;

	    while ((c = *++cp) != '\0') {
		switch (c) {
		    case 'a':
			boothowto |= RB_ASKNAME;
			break;
		    case 's':
			boothowto |= RB_SINGLE;
			break;
		    case 'd':
			boothowto |= RB_KDB;
			break;
		    case 'n':
			boothowto |= RB_INITNAME;
			break;
		    case 'h':
#ifdef	SECOND_SERVER
			if (second_server) {
				printf("Suspended and ready to continue.\n");
				task_suspend(mach_task_self());
				break;
			}
#endif	/* SECOND_SERVER */
			printf("inline call to debugger\n");
			Debugger();
			break;
		    case 'x':
			break;		/* no flags */
/*
 *	The remaining flags won't make it through the Mach boot.
 */
#ifdef	SECOND_SERVER
		    case '2':	 
			if (second_server != 1) {
				second_server = 1;
				printf("Warning: `-2' flag forced on a standalone server.\n");
			}
			break;
#endif	/* SECOND_SERVER */
		    case 'e':
			/* Allow non-default emulator file name: */
			emulator_name = argv[1];
			/* Skip over the file name argument: */
			argv++, argc--;
			break;
		    case 'i':
			/* Allow non-default init program file name: */
			init_program_name = argv[1];
			/* Skip over the file name argument: */
			argv++, argc--;
			break;
		    case 'v':
			/* Turn on syscall tracing: */
			syscalltrace = -1;
			print_debug_messages = 1;
			break;
		    case 'w':
			/* Turn on wired server threads: */
			wired_threads = 1;
			break;
		    case 'W':
			/* Turn off wired server threads: */
			wired_threads = 0;
			break;
		    case 'V':
			/* Set boot config variable */
			(void)boot_config_parse_vars(argv[1]);
			argv++, argc--;
			break;
		    default:
			printf("Warning: invalid flag `-%c'\n", c);
			break;
		}
	    }
	    argv++, argc--;
	}
	/*
	 * First non-flag arg is required and should be root dev name:
	 */
	if (argc == 0) {
		/* Report error: */
		rootdev_name = "<none specified>";
		rootdev = (int)(-1);
	} else {
		rootdev_name = argv[0];
		rootdev = parse_root_device(argv[0]);
	}

	/*
	 * Any remaining args are to be passed to the init program:
	 */
	argv++, argc--;
	if (argc > 0) {
		init_program_args = argv;
	}
}


/*
 * Check Mach kernel version and warn if it is incompatible.  Print
 * Mach kernel version and server version.
 */
void
print_versions()
{
	char		kernel_version[KERNEL_VERSION_MAX];
	char 		*cp;
	int		i;
	boolean_t	old_version = FALSE;
	extern int	version_need_mk, version_need_norma_mk;
	mach_port_t	host_port = mach_host_self();

	print_debug((
	"--- print_versions() ----------------------------------\n"));
	if (host_kernel_version(host_port,&kernel_version[0]) != KERN_SUCCESS) {
		printf("%s %s\n",
		       "Warning: print_versions:",
		       "can't get micro-kernel version.");
		printf("%s\n", version);
	} else {
		printf("%s", kernel_version);	/* MK version */
		printf("%s\n",version);		/* server version */
		for (cp = &kernel_version[0]; 
		     cp < &kernel_version[KERNEL_VERSION_MAX] && *cp; 
		     cp++) {
			if (!strncmp(cp, "VERSION(MK", 10)) {
				cp += 10;    /* strlen("VERSION(MK") */
				running_mk_version = 0;
				break;
			}
			if (!strncmp(cp, "VERSION(NORMA_MK", 16)) {
				cp += 16;    /* strlen("VERSION(NORMA_MK") */
				running_norma_mk_version = 0;
				break;
			}
		}
		for (i = 0; 
		     cp < &kernel_version[KERNEL_VERSION_MAX] && *cp;
		     cp++) {
			if (*cp < '0' || *cp > '9')
				break;
			i *= 10;
			i += *cp - '0';
		}
		if (running_mk_version != -1) {
			running_mk_version = i;
			if (running_mk_version < version_need_mk)
				old_version = TRUE;
		} else if (running_norma_mk_version != -1) {
			running_norma_mk_version = i;
			if (running_norma_mk_version < version_need_norma_mk)
				old_version = TRUE;
		}
		if (old_version)
			printf("\a** %s %s MK%d or NORMA_MK%d.\a\n",
			       "WARNING: this server won't run",
			       "correctly on kernels prior to",
			       version_need_mk, version_need_norma_mk);
		if (running_mk_version == -1 && 
		    running_norma_mk_version == -1) {
			printf("%s %s\n",
			       "WARNING: print_versions:",
			       "can't identify micro-kernel version.");
		}
	}
}


#if	MAP_UAREA
/*
 * Set up memory for sharing with emulator:
 */
void
emul_shared_init()
{
	kern_return_t	result;
	int		cproc = 0;
	vm_address_t	shared_address = 0;
	register struct proc *p;
	node_t		node;
        int 		max_retry = 75;   /* try 75 times for server contact */
        int 		print_count = 10; /* print a message every 10 retries */
        int 		sleep_secs = 10;  /* sleep 10 secs between each retry */
	int		i;

	/*
	 * Compute the number of pages shared between the server and the 
	 * emulator according to the size of the structures.
	 */
	ro_pages_nb = (((vm_offset_t)sizeof(struct ushared_ro)) +
		       (vm_page_size -1)) / vm_page_size;
	rw_pages_nb = (((vm_offset_t)sizeof(struct ushared_rw)) +
		       (vm_page_size -1)) / vm_page_size;
	shared_pages_nb = ro_pages_nb + rw_pages_nb + io_pages_nb;

	if (!import_paging) {
		result = default_pager_object_create(default_pager_port,
					&shared_memory_port,
					shared_pages_nb*vm_page_size*nproc);
	} else {
		node = pager_node;
		for (i=0; i<max_retry; i++) {
			result = remote_vnode_pager_get(FALSE, 
					shared_pages_nb*vm_page_size*nproc,
					&shared_memory_port,
					&node);
			if (result == KERN_SUCCESS) {
				break;
			}
                	if (!(i % print_count)) {
				printf("emul_shared_init: %s %d\n", 
					"Waiting for pager on node", node);
			}
			mach_init_sleep(sleep_secs);
		}
	}

	if (result != KERN_SUCCESS) {
		printf("emul_shared_init: default_pager_port %x, shared_memory_port %x shared_pages_nb %d result %x\n", default_pager_port, &shared_memory_port, shared_pages_nb * vm_page_size * nproc,  result);
		
		panic("getting shared_memory port");
	}
	result = vm_map(mach_task_self(),
			&shared_address, shared_pages_nb*nproc*vm_page_size,
			0, 1, shared_memory_port, 0, 0,
			VM_PROT_READ|VM_PROT_WRITE, VM_PROT_ALL,
			VM_INHERIT_NONE);
	if (result != KERN_SUCCESS)
		panic("OSF1_main:vm_map shared_memory %d\n",result);

	for(p = proc; p != procNPROC; p++,cproc++) {
	    p->p_shared_off = shared_pages_nb*vm_page_size*cproc;
	    p->p_readwrite = (char *)shared_address;
	    p->p_shared_rw = (struct ushared_rw *)
		    (shared_address + io_pages_nb*vm_page_size);
	    p->p_shared_ro = (struct ushared_ro *)
		    (shared_address + (io_pages_nb+rw_pages_nb)*vm_page_size);
	    shared_address += shared_pages_nb*vm_page_size;
	    mutex_init(&p->p_shared_mutex);
	}
}
#endif MAP_UAREA


/*
 *	vm_set_page_size:
 *
 *	Sets the page size, perhaps based upon the memory
 *	size.  Must be called before any use of page-size
 *	dependent functions.
 *
 *	Sets page_shift and page_mask from vm_page_size.
 */
void
vm_set_page_size()
{
	page_mask = vm_page_size - 1;

	if ((page_mask & vm_page_size) != 0)
		panic("vm_set_page_size: page size not a power of two");

	for (page_shift = 0; ; page_shift++)
		if ((1 << page_shift) == vm_page_size)
			break;
}


/*
 * Figure out size of physical memory and set vm page size stuff.  This
 * is from i386/i386_init.c in the integrated kernel.
 */
void
size_memory()
{
	kern_return_t		kr;
	int			host_buff[HOST_BASIC_INFO_COUNT];
	mach_msg_type_number_t	host_buff_size = HOST_BASIC_INFO_COUNT;
	host_t			mach_host_self();
	vm_statistics_data_t	vm_stat;

	if ((kr = host_info(mach_host_self(), HOST_BASIC_INFO,
				host_buff, &host_buff_size)) != KERN_SUCCESS)
		panic("size_memory: host_info failed");

	mem_size = ((struct host_basic_info *)host_buff)->memory_size;

	if ((kr = vm_statistics(mach_task_self(), &vm_stat)) != KERN_SUCCESS)
		panic("size_memory: vm_statistics failed");

	vm_page_free_count = vm_stat.free_count;

	vm_set_page_size();
}		 	

/*
 * This function implements a sleep/timeout facility that can be used in 
 * mach_init.c where sleep() cannot be called
 */
void
mach_init_sleep(secs)
	int	secs;
{
        kern_return_t           kr;
	mach_msg_header_t 	msg_header;
        mach_msg_return_t       mr;

        if (sleep_port == NULL) {
                if ((kr = mach_port_allocate(mach_task_self(), 
                                             MACH_PORT_RIGHT_RECEIVE,
                                             &sleep_port)
                     != KERN_SUCCESS))
                  panic("Unable to allocate port for timeout facility\n");
        }

        msg_header.msgh_size = sizeof(msg_header);
        msg_header.msgh_local_port = sleep_port;

        mr = mach_msg(&msg_header, MACH_RCV_MSG|MACH_RCV_TIMEOUT,0,
                      msg_header.msgh_size, sleep_port, secs * 1000,
                      MACH_PORT_NULL);

        if (mr != MACH_RCV_TIMED_OUT) {
                /* We MUST timeout - there is not other way we want to
                 * return from the above mach_msg call 
                 */
                panic("Unable to implement timeout using mach_msg\n");
        }
}

int
string_to_args(string, argv, max)
	char	*string;
	char	*argv[];
        int      max;
{
	char	*sp;
	int	argc = 0;

	*argv++ = NULL;		/* program name is always skipped */
	argc = 1;
	sp = string;

	/* get rid of blanks */
	while (sp != NULL && *sp == ' ') {
		sp++;
	}
	while (sp != NULL && *sp != '\0') {
		argc++;
		*argv++ = sp;
		if (argc >= max) return(argc);
		/* Get to the end of current string */
		while (*sp != ' ' && *sp != '\0') {
			sp++;
		}
		/* NULL terminate the argv pointer */
		*sp++ = '\0';
		/* get rid of blanks */
		while (*sp == ' ') {
			sp++;
		}
	}
	return(argc);
}


/*
 * Server main program.  Unlike in OSF/1 integrated kernel, execution begins
 * with the "main" function.  This function is the analog of locore.s in the
 * integrated kernel.  The main function of the integrated kernel has been
 * renamed to OSF1_main and is still found in bsd/init_main.c.
 */
main(argc, argv)
{
	kern_return_t		rc;
	boolean_t		page_0_was_allocated;
	extern vm_prot_t	page_0_protection;

	/*
	 * Protect our page 0, so that all null pointers references
	 * will fail. We may have to undo the existing protection...
	 */
        if (vm_protect(mach_task_self(), 0, vm_page_size, TRUE,
                       VM_PROT_NONE) != KERN_SUCCESS ||
            vm_protect(mach_task_self(), 0, vm_page_size, FALSE,
                       VM_PROT_NONE) != KERN_SUCCESS) {
                rc = vm_deallocate(mach_task_self(), 0, vm_page_size);
                page_0_was_allocated = (rc == KERN_SUCCESS);
                rc = vm_map(mach_task_self(), 0, vm_page_size,
                            0, FALSE, MEMORY_OBJECT_NULL, 0, FALSE,
                            VM_PROT_NONE, VM_PROT_NONE, VM_INHERIT_NONE);
        } else {
                rc = KERN_SUCCESS;
                page_0_was_allocated = FALSE;
        }

	/*
	 * Get initial ports and arguments. (NOTE: printf's will not work
	 * before this point)
	 */
	get_config_info();

	print_debug((
	"--- main(after get_config_info) ----------------------------\n"));

	if (rc != KERN_SUCCESS)
		printf("\aWARNING: failed to reserve page 0. vm_map() returned 0x%x\n", rc);
	if (page_0_was_allocated) {
		printf("\aWARNING: existing page 0 deallocated.\n");
	}
	/*
	 * page_0_protection might have been changed in get_config_info
	 */
	if (page_0_protection != VM_PROT_NONE) {
		(void) vm_protect(mach_task_self(), 0, vm_page_size, FALSE,
				  page_0_protection);
	}

	/*
	 * Initialize mem size and vm global values.
	 */
	size_memory();

#if	NORMA_IPC
	/*
	 * Get this node's node number.  Must be done before call
	 * to boot_config_init().
	 */
	if ((rc = norma_node_self(mach_task_self(), &this_node))
            != KERN_SUCCESS)
		panic("Can't get this node number; rc=0x%x", rc);
#endif
   	/* 
	 * Get the boot-time configuration inforamtion from the kernel 
	 * and parse it, we need to do this now, since it will setup
	 * command line args.
         */
        boot_config_init();

	/*
	 * Parse and setup command line info based on boot magic
	 */
	{
#define         MAX_NUM_ARGS  100


		char *boot_argv_buf[MAX_NUM_ARGS];
		char **boot_argvp = boot_argv_buf;
		int boot_argc;

		boot_argc = string_to_args(boot_options_buf, boot_argvp, MAX_NUM_ARGS);
		parse_command_args(boot_argc, boot_argv_buf);
	}

	/*
	 * Parse and setup command line info based on command line args
	 */
	parse_command_args(argc, argv);

	/*
	 * Initialize module for handling server threads.  Must be
	 * done very early, because even our wired threads are managed
	 * by this module, but cannot be done until after parsing of
	 * command line args, since "-w" turns on wired threads:
	 */
	ux_server_init();

#if	NORMA_IPC
	/*
	 * Enforce bootconfig rules
	 */
	if (import_paging && pager_node == this_node) {
		printf("Node %d: Panic: Can't import paging from self\n",
			this_node);
		panic("Invalid bootconfig options");
	}

#if     __i860__
{
        extern size_t node_array_entries, cube_io_node_array_entries;
        extern          int _i860_cpu_type();
        boolean_t       is_cube = (_i860_cpu_type() == 1);

        if (node_array_entries == 0)
		printf("WARNING: no boot config variable BOOT_NODE_LIST: assuming all nodes valid\n");

        if (is_cube && cube_io_node_array_entries == 0)
		printf("WARNING: no boot config variable CUBE_IO_NODE_LIST: assuming all nodes valid\n");
}
#endif	/* __i860__ */

#ifdef TNC
	/*
	 * If no System V IPC node specified, default to root filesystem node.
 	 */
	if (svipc_node == -1) {
		svipc_node = root_fs_node;
	}
#endif /* TNC */

	/*
	 * Handle config variables for debugging.
	 */
	if (this_node == debug_node) {
		/*
		 * A debug node acts as its own root fs node.
		 */
		printf("Node %d: Acting as debug node (using own root fs)\n", 
			this_node);
		if (import_paging) {
			printf("Node %d: Debug node may not import_paging",
				this_node);
			panic("Invalid bootconfig options");
		}
		if (debug_root_device_node == -1) {
			printf("Node %d: Must specify DEBUG_ROOT_DEVICE_NODE variable when using DEBUG_NODE",
				this_node);
			panic("Invalid bootconfig options");
		}
		root_fs_node = debug_node;
		root_device_node = debug_root_device_node;
	}
#endif	/* NORMA_IPC */

#ifdef	SECOND_SERVER
	/*
	 * Catch signals if second server.
	 */
	if (second_server)
		catch_signals();
#endif	/* SECOND_SERVER */

	/*
	 * Setup mappable time
	 */
	init_mapped_time();

	/* OSF/1 IK bug: this has to be done before netinit() is called.
	 * Doing it in global_lock_initialization is too late:
	 */
	TIME_LOCK_INIT();

#ifdef	MP_SLAVE_AS_CLK
	/*
	 * Set up hi-resolution clock access, if it exists
	 */
	init_hi_res_clock();
#endif	/* MP_SLAVE_AS_CLK */

	zone_init();

	/*
	 * Initialize SPL emulator
	 */
	spl_init();

#if GPROF
	/*
	 * Initializations for the Server profiling function.
	 */
	kmstartup();
#endif

	/*
	 * Start device reply server
	 */
	dev_utils_init();
	device_reply_hdlr();

        /*
         * Initialize notification port.
         */
        ux_notify_init();

	(void)setup_main();

	task_to_proc_init();

#if	NLV > 0
	lvprobe(0);	/* Logical Volume Manager initialization */
#endif	/* NLV > 0 */

	/*
	 * Start up a ux_server_loop thread to do the rest of the
	 * initialization:
	 */
	(void) ux_create_thread(OSF1_main);

	/*
	 * This should never return from this condition wait
	 */
	mutex_lock(&kill_lock);
	condition_wait(&kill_main,&kill_lock);
}
