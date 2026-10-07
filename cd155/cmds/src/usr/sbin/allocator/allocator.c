/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
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
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/allocator.c,v 1.71 1995/03/07 00:53:47 carolr Exp $
 *
 */

/* History:
 *	$Log: allocator.c,v $
 * Revision 1.71  1995/03/07  00:53:47  carolr
 * Description:
 *          moved the call to free_schedule_lists() from schedule.c to allocator.c
 *          so it will be outside the main loop and only freed once.  All references
 *          to the rollin_list/rollout_list and ri_list/ro_list are based upon the
 *          indices. These indices are cleared (set to zero) upon entry to the
 *          schedule().
 *
 *          added checks to ensure the item about to be free'd (misc_rpcs.c)
 *          was not null before calling free_bitmap()
 *
 *  Reviewer: sdh
 *  Risk: low
 *  Benefit or PTS #: 11531
 *  Testing:
 *    EATS: controlc, rmcall, rmcmd, sched (sans the mandatory 12 node test)
 *    manual testing
 *
 *  Module(s):
 *      .../src/usr/sbin/allocator/allocator.c
 *      .../src/usr/sbin/allocator/schedule.c
 *      .../src/usr/sbin/allocator/misc_rpcs.c
 *
 * Revision 1.70  1994/12/19  15:59:49  johannes
 * dump_appl_tbl(): Dump the new field appl_core of the application structure.
 *
 *  Reviewer: Scott Hahn
 *  Risk: High (several components involved)
 *  Benefit or PTS #: 11577
 *  Testing: developer testing, special testing by Simon Tsang,
 *           corefile EAT, sched EAT, rmcmd EAT, controlc EAT
 *  Module(s): svr/server/paracore/dvp_pvpcore.c
 *             svr/server/nx/nx.c, nx.defs
 *             usr/sbin/allocator/alloc.defs, misc_rpcs.c, init_appl.c,
 *                                allocator.c, pspart.c
 *             usr/include/nx/schedule.h
 *             usr/include/allocsys.h
 *             usr/bin/pspart
 *
 * Revision 1.69  1994/11/19  03:04:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.68  1994/10/14  23:23:44  sdh
 * Set the do_MACS variable if USE_MACS is in the config
 * file and fix the logical to physical mapping debug code.
 *
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 10467
 *  Testing:
 * 	EATS: controlc, rmcall, rmcmd
 * 	manual tests
 *  Module(s):
 * 	allocator.c
 *
 * Revision 1.67  1994/07/06  17:41:00  sdh
 * Changed thew parsing get_config_rq() so it passes over the
 * text portion of the string and finds the rq value.
 *
 *  Reviewer: jkearns
 *  Risk: low
 *  Benefit or PTS #: 9408
 *  Testing: manual
 *  Module(s):
 * 	allocator.c
 *
 * Corrected some spelling in log message.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: 7737
 *  Testing: manual
 *  Module(s):
 * 	allocator.c
 *
 * Revision 1.66  1994/06/13  16:41:12  sdh
 * Changed debug messages to go through debug print routine.
 *
 *  Reviewer: mag
 *  Risk: Low:
 *  Benefit or PTS #:
 *  Testing: EATS
 *  Module(s):
 * 	cmds_libs/src/usr/sbin/allocator/tiles.c
 * 	cmds_libs/src/usr/sbin/allocator/allocator.c
 * 	cmds_libs/src/usr/sbin/allocator/allocutils.c
 * 	cmds_libs/src/usr/sbin/allocator/conflict.c
 * 	cmds_libs/src/usr/sbin/allocator/init_appl.c
 * 	cmds_libs/src/usr/sbin/allocator/misc_rpcs.c
 * 	cmds_libs/src/usr/sbin/allocator/mkpart_rpc.c
 * 	cmds_libs/src/usr/sbin/allocator/rmpart_rpc.c
 * 	cmds_libs/src/usr/sbin/allocator/schedule.c
 * 	cmds_libs/src/usr/sbin/allocator/server_loop.c
 * 	cmds_libs/src/usr/sbin/allocator/smd.c
 * 	cmds_libs/src/usr/sbin/allocator/tiles.c
 *
 * Revision 1.65  1994/06/03  22:34:10  sdh
 * Added a call to ignore SIGPIPE. Now when we write to a socket
 * that has been closed on the other side, we will get and EPIPE
 * instead of a SIGPIPE signal.
 *
 * Also add a routine to dump the application table and partition table
 * when the allocator receives a SIGINFO signal.
 *
 *  Reviewer:
 *  Risk: low
 *  Benefit or PTS #: 9488
 *  Testing: manual tests, EATS: nqs, sched
 *  Module(s):
 * 	cmds_libs/src/usr/sbin/allocator/allocator.c
 *
 * Revision 1.64  1994/06/01  20:45:19  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): Makefile, alloc.defs, alloc_types.defs, allocator.c init_appl.c
 * 	    misc_rpcs.c, mkpart_rpc.c, schedule.c, tiles.c, tiles.h,
 * 	    attributes.c (new), attributes.h (new)
 *  Related: libnx, server, emulator, bootmesh, mkpart, showpart, lspart
 *
 * Revision 1.63  1994/02/09  00:14:11  carbajal
 *   7268 PARAGON      OPEN      M        carbajal  simont              R1.2 WW47
 *        MESH UTILS   27-JAN-94 **       26-JAN-94 29-NOV-93
 *        allocator does NOT handle comment in the config file correctly
 *   Reviewer: Cameron
 *   Risk: Low
 *   Benefit or PTS #: 7268
 *   Testing: resource mgmt EATs
 *   Module(s): allocator.c
 *
 *  Issue: I was using fscanf() to process the config file, so it was failing to
 *  recognize a comment line that began with a # followed by some number of
 *  spaces followed by the comment. The change was to use fgets instead.
 *
 * Revision 1.61.2.3  1994/02/09  00:11:31  carbajal
 *  7268 PARAGON      OPEN      M        carbajal  simont              R1.2 WW47
 *       MESH UTILS   27-JAN-94 **       26-JAN-94 29-NOV-93
 *       allocator does NOT handle comment in the config file correctly
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: 7268
 *  Testing: resource mgmt EATs
 *  Module(s): allocator.c
 * Issue: I was using fscanf() to process the config file, so it was failing to
 * recognize a comment line that began with a # followed by some number of spaces
 * followed by the comment. The change was to use fgets instead.
 *
 * Revision 1.61.2.2  1994/01/12  18:18:50  carbajal
 *  7268 PARAGON      OPEN      M        carbajal  simont              R1.2 WW47
 *       MESH UTILS   29-NOV-93 **       29-NOV-93 29-NOV-93
 *       allocator does NOT handle comment in the config file correctly
 * Benefit: Proper functionality
 * Risk: Low
 * Eng: carbajal   Reviewers: litvin
 *
 * Revision 1.61.2.1  1993/12/20  21:50:23  carbajal
 * Configuration file updates
 * Memory leaks
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7257
 *  Testing: EATs, SATs, MUNOPs, bug test
 *  Module(s): allocator.c
 *
 * Revision 1.61  1993/12/01  01:31:22  carbajal
 * Dump allocator config file to stdout if there is an error
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.60  1993/11/22  17:56:55  carbajal
 * Change config file parameters to all caps before processing. Allow
 * the use of comments.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.59  1993/11/18  20:23:45  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of the export tree
 *  Testing: built on Suns and 486
 *  Module(s): scripts.mk standard.mk
 *
 * Revision 1.58  1993/11/17  06:54:54  carbajal
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: More R1.2 User Model Changes
 *  Testing:
 *  Module(s):
 *
 * Revision 1.57  1993/11/17  02:53:24  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.56  1993/10/27  01:56:06  carbajal
 * Split up module by putting generic utils into allocutils.c
 *
 * Revision 1.55  1993/08/28  01:00:27  carbajal
 * Added -MACS switch for PTS #6141
 *
 * Revision 1.54  1993/07/13  22:13:07  carbajal
 * Setup_allocator_port() is new
 *
 * Revision 1.52  1993/05/25  23:56:24  carbajal
 * Re-org'd pulled alot of code out of here and into various other smaller files.
 * Added support for scheduling and allocation layers
 *
 * Revision 1.50  1993/05/06  18:31:55  carbajal
 * Changed part_remove() to build a list of inodes to remove. A recursive
 * remove is now done depth first with permissions being checked along the
 * way.
 * In init_part() check the rollin quantum of the partition, if it is below
 * MIN_ALLOWED_RQ then changed it to MIN_ALLOWED_RQ and update the partition
 * file.
 *
 * Revision 1.49  1993/04/16  00:14:54  carbajal
 * Fixed pspart so that it will work off the boot node.
 * Problem was that the size of the passed back data structure was
 * not correct
 *
 * Revision 1.48  1993/04/14  18:40:17  carbajal
 * If using -tile then in do_mkpart() call internal_error1 on error
 *
 * Revision 1.47  1993/04/13  01:09:37  carbajal
 * More debug statements
 * -tile option
 * Increased value of LOCKED_TOO_LONG
 *
 * Revision 1.46  1993/04/05  06:23:56  carbajal
 * Corrected sequence of events when removing a partition
 *
 * Revision 1.45  1993/04/02  23:27:59  carbajal
 * Upped lock time out value
 * Set partition priority to NO_PRI upon initialization
 * pspart changes to time
 *
 * Revision 1.44  1993/03/31  21:15:26  carbajal
 * Don't add the interval in when fill in pspart info
 *
 * Revision 1.43  1993/03/29  23:48:17  carbajal
 * Added debugging statements
 *
 * Revision 1.42  1993/03/25  02:45:13  carbajal
 * Cleaned up compiler warnings
 * Cleaned up code that handles readjusting layer priorities
 * server_loop.c: now calls schedule() before responding to server RPC
 *
 * Revision 1.40  1993/03/09  18:19:34  carbajal
 * When removing a partition make sure we readjust the active
 * layer structure correctly and perk the priorities of the
 * parent partition back up.
 * When using rmpart -r make sure we look at the number of applications
 * in the subpartitions also.
 *
 * Revision 1.39  1993/03/01  18:52:45  carbajal
 * In part_sz_allocate() the p_ndlistcnt was set to
 * the wrong value. This would cause false allocator
 * internal error messages from mkpart.
 *
 * Revision 1.38  1993/02/24  21:58:58  carbajal
 * When changing the EPL of a partition we need to call
 * search_layers to re-adjust the layer priorities.
 *
 * Revision 1.37  1993/02/19  19:46:41  carbajal
 * Pass priority in to init_appl
 *
 * Revision 1.36  1993/02/18  03:15:22  carbajal
 * Fixes relating to PTS# 4083, #4124. These are priority
 * related bugs
 *
 * Revision 1.35  1993/01/28  19:48:32  carbajal
 * Removed multiply defined LP_MAP_T stuff. This is no longer
 * in allocator.h but is now in msmsg_appl.h
 *
 * Revision 1.34  1993/01/28  17:22:24  carbajal
 * Just mode debug statements
 *
 * Revision 1.33  1993/01/28  03:31:58  carbajal
 * init_appl call notify_smd
 * cleaned up compiler warnings
 * increased max message size for MIG to 4k
 * If select_nodes fails return any allocated resources found in the bitmap back to the
 * cloned layer
 *
 * Revision 1.32  1993/01/19  20:07:47  carbajal
 * In init_appl call notify_smd()
 *
 * Revision 1.31  1993/01/18  20:04:33  carbajal
 * allocator.c: put DFLT_PRI in defines.h
 * 	     call init_scheduling
 * Pass back EANOEXIST in set_nx_pri and remove_nx_appl if process group is not there.
 * use debug macros instead of ifdefs
 * put in many asserts
 *
 * Revision 1.27  1993/01/05  22:38:06  shala
 * Support nx_pri.
 *
 * Revision 1.26  1992/12/24  03:19:43  carbajal
 * init_appl: set application priority to max level of
 * partition. Set appl->appl_stat = APPL_NOT_STARTED
 *
 * Revision 1.25  1992/12/18  17:04:33  carbajal
 * Removed some ifdefed code
 * init_appl: set layer priorities
 * nx_remove_appl: new call
 * nx_set_pri: new call
 * in main read in bad nodes file
 * defined init_app_cnt
 * Added bad/missing node support - New error code EPBXRS
 *
 * Revision 1.23  1992/11/03  00:08:17  carbajal
 * Added new MIG calls into allocator lock_part
 * and release_lock. These are BIG locks that
 * serialize access to modifying partitions.
 * Currently mkpart,rmpart and chpart all make
 * these calls.
 *
 * Revision 1.22  1992/10/23  01:36:47  rkl
 * Made sure that all uid and gid were uid_t and gid_t respectively.
 *
 * Revision 1.21  1992/10/22  21:19:06  carbajal
 * Send back elapsed time in pspart call
 *
 * Revision 1.20  1992/10/22  01:11:56  carbajal
 * Changes to part_remove:
 * 	don't allow root to be the path on a remove
 * 	this will return an access error.
 * 	call validate_allocator_access to check for permission.
 *
 * Changes to validate_allocator_access:
 * 	pass in access request
 * 	check to see if we need owner permission
 * 	check to see if we need to check access against parent
 *
 * Changes to chpart:
 * 	If we are changing the name then just check access.
 * 	If we are changing the mode only (-mod) check to
 * 	see if we are the owner.
 *
 * Changes to pspart:
 * 	Check for READ permission instead of WRITE
 *
 * Revision 1.19  1992/10/12  17:06:03  carbajal
 * opendir_r and readdir_r have been moved into
 * utils.c in libnx.a. There are ifdefed out
 * of here.
 *
 * Revision 1.18  1992/10/12  16:56:58  carbajal
 * Added support for rmpart -r
 * Cleaned up implementation of rmpart -f
 *
 * Revision 1.17  1992/10/08  23:45:15  rkl
 * Removed alloc.h include which is not needed in prep for making allocator
 * server/user libraries.
 *
 * Revision 1.16  1992/10/08  22:39:49  rkl
 * Added support for SMD and made scheduling/no-scheduling a command line option.
 *
 * Revision 1.15  1992/10/08  19:16:48  carbajal
 * added pspart support and chpart support
 *
 * Revision 1.12.1.2  1992/10/06  00:53:47  carbajal
 * added support for pspart command
 *
 * Revision 1.12.1.1  1992/09/22  00:13:31  carbajal
 * Added support for chpart. There is only on entry point for
 * chpart now.
 *
*/

static char rcsid[] = "$Id: allocator.c,v 1.71 1995/03/07 00:53:47 carolr Exp $";

/*
 * allocator.c	- Don Cameron	3/92
 *
 * This file contains the MIG server routines exported by the allocator server.
 *
 */

#include <sys/dir.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <mach/mach.h>
#include <mach_error.h>
#include <mach/mig_errors.h>
#include <mach/message.h>
#include <mach/norma_special_ports.h>
#include <mach/mach_host.h>
#include <sys/errno.h>
#include <assert.h>
#include <syslog.h>
#include <mcmsg/mcmsg_appl.h>
#include <nx/defines.h>
#include <nx/bitmap.h>
#include <nx/hash.h>
#include <nx/schedule.h>
#include <nx/allocator.h>
#include <nx/smd.h>
#include <nx/utils.h>
#include <nx/writepart.h>
#include "debug.h"
#include "macros.h"
#include "conflict.h"
#include "tiles.h"
#include "allocutils.h"


extern	APPL_T	*new_appl_ptr;

pid_t	new_appl;

/*
 *  Global flag/pgroup for new application.
 */
BITMAP_T	*unusable_node_map;
BITMAP_T	*inverted_root,*bad_bitmap;

/*
 * Forward references to internal routines.
 */
static int init_part(char *path);
void allocServ_run();
void init_signals();
int process_config_file(char *file_name,alloc_config_t *config);
void dump_config();
void dump_part_tbl();
void dump_appl_tbl();

/*
 * Allocator server port
 */
static mach_port_t ServerPort;
extern boolean_t alloc_server();	/* Defined in the mig created file */
extern void init_scheduling();		/* Defined in schedule.c */

/*
 * Root partition.
 */
PART_T	*root = (PART_T *) 0;
static char     root_path[NAMELEN] = "/etc/nx";

#define attribute_file	"/etc/nx/nodeinfo"

#define alloc_config_file "/etc/nx/allocator.config"

/*
 * Partition and application hash tables
 */
HASH_TBL_T		part_tbl[HASH_TBL_LENGTH];
HASH_TBL_T		appl_tbl[HASH_TBL_LENGTH];

/* Allocator configuration 
 *
*/
alloc_config_t		alloc_config;
int			num_gang_parts;	/* Number of gang scheduled partitions
					 * in the machine
					*/


#ifdef iPSC860_COMPAT
/*
 * Hypercube mesh mapping
 */
LP_MAP_T	mesh_map;
#endif iPSC860_COMPAT

static	int	do_scheduling;

locktype alloc_lock;
long init_app_cnt;
int	conflict_debug,sched_debug,allocation_debug,attrib_debug,
	signal_debug,compare_debug,tile,pspart_debug;
int	do_MACS;


typedef ino_t	*inode_list_t;

/* defined in server_loop.c */
extern unsigned long	interval;
extern void remove_application(APPL_T *app);
extern void perk_remove_active_partition(PART_T *part);
extern void set_layer_pri(LAYER_T *layer);
extern void set_cur_part_pri(PART_T *part);
extern void perk_player_pri(LAYER_T *p_layer);

void
setup_allocator_port(char *port_name,mach_port_t *ServerPort)
{
 	kern_return_t	retcode;
	mach_port_t	name_server_port;
	mach_port_t	privileged_host_port;
	/*
	 * Allocate the server port
	*/
	if (( retcode = mach_port_allocate(mach_task_self(),
		MACH_PORT_RIGHT_RECEIVE, ServerPort)) != KERN_SUCCESS)
	{
		debug_print(DEBUG_OTHER, 0, "unable to allocate port \n");
		exit (2);
	}

	debug_allocation(5,"ALLOCATOR : PORT ALLOCATED \n");

	if (mach_port_insert_right(mach_task_self(), *ServerPort,
		*ServerPort, MACH_MSG_TYPE_MAKE_SEND) != KERN_SUCCESS)
			debug_print(DEBUG_OTHER, 0, "can't create send right\n");

	privileged_host_port = task_by_pid(-1);

	if ((retcode = norma_get_nameserver_port(privileged_host_port,
		nx_allocator_node(),
		&name_server_port)) != KERN_SUCCESS) {
		debug_print(DEBUG_OTHER, 0,"Unable to get nameserver port\n");
		exit(1);
	}
	debug_allocation(5,"ALLOCATOR : NAMESERVER PORT ALLOCATED\n");

	/*
	 * Register the server port with the Name server
	 */
	retcode = netname_check_in(name_server_port, port_name,
				   MACH_PORT_NULL, *ServerPort);

	if(retcode != KERN_SUCCESS) {
		debug_print(DEBUG_OTHER, 0,"Port checkin failed\n");
 		exit (2);
	}

	debug_allocation(5,"ALLOCATOR : PORT CHECKED IN\n");

}

main(argc, argv)
int	argc;
char	**argv;
{
	int		i;
	char		*path;

	inverted_root = (BITMAP_T *)0;
	bad_bitmap = (BITMAP_T *)0;

	/* Setup the time zone for the localtime call */
	tzset();

	openlog("allocator", LOG_CONS, LOG_DAEMON);

#ifdef DO_SIG_PROF
	init_profile();
#endif DO_SIG_PROF

	init_scheduling();
	/* Pass all output to stdout */
	dup2(2,1);

	path = (char *) malloc(NAMELEN);
	if (path == NULL){
		errno = ENOMEM;
		perror("allocator");
		exit(1);
	}
	path[0] = '\0';
	path = (char *)strcat(path,root_path);
	if (path == NULL){
		errno = ENOMEM;
		perror("allocator");
		exit(1);
	}
	
	init_app_cnt = 0;
	do_scheduling = 1;
	sched_debug = 0;
	conflict_debug = 0;
	allocation_debug = 0;
	signal_debug = 0;
	compare_debug = 0;
	pspart_debug = 0;
	attrib_debug = 0;
	tile = 0;
	do_MACS = 0;

	for (i = 1; i < argc; i++){
		if (strcmp("-nosched", argv[i])== 0)
			do_scheduling = 0;
		else
		if (strcmp("-debug", argv[i])== 0){
			allocation_debug = 1;
			sched_debug = 1;
			conflict_debug = 1;
			attrib_debug = 1;
		}
		else
		if (strcmp("-allocation_debug", argv[i])== 0)
			allocation_debug = 1;
		else
		if (strcmp("-sched_debug", argv[i])== 0)
			sched_debug = 1;
		else
		if (strcmp("-conflict_debug", argv[i])== 0)
			conflict_debug = 1;
		else
		if (strcmp("-signal_debug", argv[i])== 0)
			signal_debug = 1;
		else
		if (strcmp("-compare_debug", argv[i])== 0)
			compare_debug = 1;
		else
		if (strcmp("-pspart_debug", argv[i])== 0)
			pspart_debug = 1;
		else
		if (strcmp("-tile", argv[i])== 0)
			tile = 1;
		else
		if (strcmp("-MACS", argv[i])== 0)
			do_MACS = 1;
		else
		if (strcmp("-attrib_debug", argv[1]) == 0)
			attrib_debug = 1;
	}

	init_signals();
	hash_tbl_init(part_tbl);
	hash_tbl_init(appl_tbl);
	num_gang_parts = 0;
	if (process_config_file(alloc_config_file,&alloc_config) == -1){
		perror("invalid allocator config file");
		exit(1);
	}
	if (allocation_debug)
		dump_config(alloc_config);
	debug_print(DEBUG_OTHER, 0, "Allocator started\n");
	syslog(LOG_INFO, "Allocator started");

	setup_allocator_port("ALLOCATOR",&ServerPort);

	/*
	 * Read .partinfo files to set up initial partitions.
	 */
	if (init_part(path) == -1) {
		perror("Allocator");
 		exit (2);
	}

	debug_allocation(5,"init_part done\n");

#ifdef iPSC860_COMPAT
if (is_ipsc860()) {
	debug_allocation(5,"map hypercube to mesh\n");
	/*
	 * Initialize a mapping table for the hypercube mapped mesh.
	 */
	mesh_map = (LP_MAP_T) MALLOC(root->slots * sizeof(LP_MAP_ENTRY_T));
	if (mesh_map == (LP_MAP_T) 0) {
		perror("Allocator");
 		exit (2);
	}
	old_gray_mesh(root->bitmap->rows, root->bitmap->cols,
	              root->slots, mesh_map);
	debug_allocation(5,"mesh mapped\n");
}
#endif iPSC860_COMPAT

	ReadAttributes(attribute_file);

	/*
	 * MIG dispatch loop
	 */
	allocServ_run();
	debug_print(DEBUG_OTHER, 0, "ALLOCATOR SERVER EXITING !\n");
	syslog(LOG_CRIT, "ALLOCATOR SERVER EXITING !");
	exit(3);
}

#define	MAXMSGWORDS	4096	/* Maximum message size */

typedef struct {
	mach_msg_header_t	head;
	mach_msg_type_t	retcodetype;
	kern_return_t	return_code;
	int		body[MAXMSGWORDS];
 } DUMMY_MSG_T;


void allocServ_run()
{
  typedef struct DumMsg
    {
      mach_msg_header_t head;
      mach_msg_type_t   retcodetype;
      kern_return_t     retcode;
      char              space[1024];
    } DumMsg;

  kern_return_t   retcode;

#ifdef DEBUG
printf("ALLOCATOR: WAITING \n");
fflush(stdout);
#endif

if (do_scheduling)
  retcode = server_loop(alloc_server, sizeof(DUMMY_MSG_T), ServerPort, root);
else {
  retcode = mach_msg_server(alloc_server, sizeof(DUMMY_MSG_T), ServerPort);
  printf("retcode:%d 0x%08x\n", retcode, retcode);
}

  free_schedule_lists();

}

/* Read in the allocator configuration file */

/* Allocator configuration file format
 *
 *      SPACE_SHARE=<0,1>       1 enables the option
 *      NUM_GANG_PARTS=n        n is an integer
 *                              -1 means no limit
 *      DEGREE_OF_OVERLAP=n     n in an integer
 *                              -1 means no limit
 *      MIN_RQ_ALLOWED=s        s is a string, valid constructs
 *                              match the mkpart, chpart
 *                              NULL implies no other limit outside
 *                              of those defined by mkpart, chpart
 *      REJECT_PLK=<0,1>        1 will cause jobs run with -plk in a
 *                              gang scheduled partition hierarchy to
 *                              be rejected
 *
*/
void
get_config_int(s,integer)
char *s;
int  *integer;
{
        char    *ptr;

        ptr = (char *)strpbrk(s,"=");

        /* did we find an "=" */
        if (ptr != NULL){
                ptr++;
                *integer = atoi(ptr);
        }
}

void
get_config_rq(s,rq)
char    *s;
unsigned long   *rq;
{
        char    *ptr, *equal_sign;
	
	/* Pass over the MIN_RQ_ALLOWED text */
        equal_sign = (char *)strpbrk(s,"=");
        /* did we find an "=" */
        if (equal_sign == NULL){
                return;
        }

	/* Change the time indicator back to lower case because
	 * the parsing routine will not accept it
	*/
        ptr = (char *)strpbrk(equal_sign,"H");
	if (ptr != NULL)
		*ptr = 'h';
        ptr = (char *)strpbrk(equal_sign,"M");
	if (ptr != NULL)
		*ptr = 'm';
        ptr = (char *)strpbrk(equal_sign,"S");
	if (ptr != NULL)
		*ptr = 's';
	
	ptr = ++equal_sign;
	(void)parse_rq(ptr,rq);
}

void make_str_all_caps(char *s)
{
	int	i,len;

	len = strlen(s);

	for (i = 0; i < len; i++)
		if (s[i] >= 'a' && s[i] <= 'z')
			s[i] -= 'a' - 'A';
		else
		if (s[i] == '\n')
			s[i] = '\0';
}

int
assign_config_var(s,config)
char    *s;
alloc_config_t  *config;
{

	if (s == NULL) 
		return(0);
	/* Skip over comment lines */
	if ( (s[0] == '#') || (s[0] == '/') || 
		(s[0] == '\n') || 
		(s[0] == '*') )
		return(0);
	make_str_all_caps(s);
        if (strstr(s,"SPACE_SHARE") != NULL)
                get_config_int(s,&(config->space_share));
        else
        if (strstr(s,"NUM_GANG_PARTS") != NULL)
                get_config_int(s,&(config->num_of_gang_parts));
        else
        if (strstr(s,"DEGREE_OF_OVERLAP") != NULL)
                get_config_int(s,&(config->degree_of_overlap));
        else
        if (strstr(s,"MIN_RQ_ALLOWED") != NULL)
                get_config_rq(s,&(config->min_rq));
        else
        if (strstr(s,"REJECT_PLK") != NULL)
                get_config_int(s,&(config->reject_plk));
	else
        if (strstr(s,"USE_MACS") != NULL) {
		get_config_int(s,&(config->use_macs));
		do_MACS = config->use_macs;
	}else{
		debug_print(DEBUG_OTHER, 0, "Unrecognized string in allocator configuration\n");
		debug_print(DEBUG_OTHER, 0,"%s\n",s);
		syslog(LOG_WARNING, "Unrecognized string in allocator configuration");
		syslog(LOG_WARNING, "%s\n",s);
		return(-1);
	}
	return(0);
	
}

void
dump_config(config)
alloc_config_t  config;
{
        printf("config.space_share %d\n",config.space_share);
        printf("config.num_of_gang_parts %d\n",config.num_of_gang_parts);
        printf("config.degree_of_overlap %d\n",config.degree_of_overlap);
        printf("config.min_rq %d\n",config.min_rq);
        printf("config.reject_plk %d\n",config.reject_plk);
}

int
verify_config(config)
alloc_config_t  config;
{
        if (config.space_share > 1 || config.space_share < 0)
                return(-1);
        else
	/* The minimum rq can be 0 <= min_rq <= MAXRQ */
        if ( (config.min_rq < 0) || (config.min_rq > MAXRQ) )
                return(-1);
        else
        if (config.reject_plk > 1 || config.reject_plk < 0)
                return(-1);
	else
	if (config.num_of_gang_parts < 0)
		return(-1);
	else
	if (config.degree_of_overlap < 0)
		return(-1);
	return(0);
}

void
init_config(config)
alloc_config_t	*config;
{
	config->use_macs = 0;
	config->space_share = 0;
	config->num_of_gang_parts = 0;
	config->degree_of_overlap = 0;
	config->min_rq = 0;
	config->reject_plk = 0;
}

int
process_config_file(file_name,config)
char            *file_name;
alloc_config_t  *config;
{
        FILE    *fp;
        char    s[80];

	init_config(config);
        if ( (fp = fopen(file_name,"r")) == NULL)
                return(0);

        while ( fgets(&s[0],sizeof(s),fp) != NULL){
                if (assign_config_var(s,config) == -1)
			return(-1);
        }

        fclose(fp);

	if (config->space_share){
		/* space share turns all gang 
	  	 * scheduling parameters off 
		*/
		config->num_of_gang_parts = 0;
		config->degree_of_overlap = 0;
		config->min_rq = 0;
		config->reject_plk = 0;
	}

	if (verify_config(*config) == -1){
		dump_config(*config);
		return(-1);
	}
	else
		return(0);
}

/*
 * init_part()
 *
 * Initialize a set of partitions by reading the directory hierarchy which
 * contains the .partinfo files describing the partition characteristics.
 */
static int
init_part(path)
char	*path;		/* Pathname of directory corresponding to partition */
{
	DIR		dir;		/* Directory structure */
	struct dirent	dir_entry;	/* Directory entry */
	PARTREQ_T	pr;		/* Partition info */
	BITMAP_T	*bitmap;	/* Bitmap pointer */
	LP_MAP_T	node_list;	/* Ptr to logical to phys node map */
        int             save_len;       /* Length of pathname before recursive
                                           call, so that we can truncate the
                                           path for traversal of sibling */
	int		status;
	int		new_size;

/*
 * Get partition information from filesystem.
 */
	debug_allocation(5,"Getting partition information from: %s\n", path);

	if (get_part_info(path,&pr,&bitmap,&node_list) == -1) {
		debug_allocation(3, "Couldn't read partition information from file %s/.partinfo\n", path);
		/* See if we have least established a root partition */
		if ( root == (PART_T *)0){
			/* Now root partition, we can't start the allocator */
			debug_print(DEBUG_OTHER, 0,"There is no root partition! The allocator can't startup\n");
			debug_print(DEBUG_OTHER, 0,"You need to rerun initpart.\n");
			syslog(LOG_CRIT, "There is no root partition! The allocator can't startup");
			syslog(LOG_CRIT, "You need to rerun initpart.");
			errno = EPALLOCERR;
			return -1;
		}
		else{
			debug_print(DEBUG_OTHER, 0,"Removing partition files and any subpartitions of %s\n",path);
			syslog(LOG_WARNING,"Removing partition files and any subpartitions of %s",path);
			delete_partitions(path);
			return 0;
		}
	}

	/* Check the rq in the partition file, 
	 * if it is below alloc_config.min_rq
	 * then change it to the minimum and update the file
	*/
	if (alloc_config.min_rq > (unsigned)0){
		if (pr.sched == GANG && pr.rq != 0 && 
			pr.rq < alloc_config.min_rq){
			debug_print(DEBUG_OTHER, 0, "Changing RQ for partition %s from %dms to %dms\n",path,pr.rq,alloc_config.min_rq);
			syslog(LOG_WARNING, "Changing RQ for partition %s from %dms to %dms",path,pr.rq,alloc_config.min_rq);
			pr.rq = alloc_config.min_rq;
			if (write_partinfo(path,&pr,bitmap,node_list) == 0){
				debug_print(DEBUG_OTHER, 0, "error updating partition file %s\n", path);
				perror("allocator:init_part");
				syslog(LOG_ERR, "error updating partition file %s:%m", path);
			}
		}
	}

/*
 * Insert partition in internal tables.
 */
	new_size = pr.nodes;
	pr.relaxed = 0;
        do_mkpart(TRUE,EXACT, node_list, &new_size, bitmap, pr,
	  (BITMAP_T*) 0, &status);
        if (status != ESUCCESS) {
                errno = status;
                return -1;
        }

/*
 * Set up subpartitions.
 */ 

	opendir_r(path, &dir);
	while (readdir_r(&dir, &dir_entry) != -1) {
		save_len = strlen(path);
		strcat(path, "/");
		strncat(path, dir_entry.d_name, NAMELEN - strlen(path));
		if ((isdir(path)) &&
                    (strcmp(".", dir_entry.d_name) != 0)         &&
		    (strcmp("..", dir_entry.d_name) != 0))
		{
			if (init_part(path) == -1) {
				return -1;
			}
		}

		/* truncate path to original length */
		path[save_len] = '\0';
	} 
	closedir(&dir);
	return 0;
}

void
catch_sigint()
{
	debug_sched(5,"caught sigint\n");
	if (allocation_debug || sched_debug)
		do_remove_using_sched_layer(root);

#ifdef DO_SIG_PROF
	dump_profile();
#endif DO_SIG_PROF

	exit(0);
}

void
catch_sigbus()
{
	printf("caught SIGBUS\n");
	exit(0);
}

void 
catch_sigsegv()
{
	printf("caught SIGSEGV\n");
	exit(0);
}

void 
catch_sigusr1()
{
	debug_print(DEBUG_OTHER, 0, "Toggling scheduling debug\n");
	if (sched_debug == 1)
		sched_debug = 0;
	else
		sched_debug = 1;
}

void 
catch_sigusr2()
{
	debug_print(DEBUG_OTHER, 0, "Toggling allocation debug\n");
	if (allocation_debug == 1)
		allocation_debug = 0;
	else
		allocation_debug = 1;
}
void
catch_siginfo()
{
    FILE *dfile;
    char dumpfile[]="/etc/nx/allocator_table.dump";

    dfile = freopen(dumpfile, "a+", stderr);
    if ( dfile == NULL) {
	/* syslog(LOG_WARNING, "can't open dump file %s: %m", debugfile); */
	perror("Can't open dump file"); 
	return;
    } else {
	(void)setvbuf(dfile, NULL, _IOLBF, BUFSIZ);
	(void)fcntl(fileno(dfile), F_SETFL, FAPPEND);
    }

    (void)dump_part_tbl(dfile);
    (void)dump_appl_tbl(dfile);

    (void) fclose(dfile);
}

void
init_signals()
{
	struct sigaction	new_sigaction;
	struct sigaction	old_sigaction;

	if (sched_debug || allocation_debug || conflict_debug){
	new_sigaction.sa_handler = catch_sigint;
	if (sigaction(SIGINT, &new_sigaction, &old_sigaction) == -1){
		perror("allocator");
		exit(1);
	}
	}

	new_sigaction.sa_handler = catch_sigbus;
	if (sigaction(SIGBUS, &new_sigaction, &old_sigaction) == -1){
		perror("allocator");
		exit(1);
	}

	new_sigaction.sa_handler = catch_sigsegv;
	if (sigaction(SIGSEGV, &new_sigaction, &old_sigaction) == -1){
		perror("allocator");
		exit(1);
	}

	new_sigaction.sa_handler = catch_sigusr1;
	if (sigaction(SIGUSR1, &new_sigaction, &old_sigaction) == -1){
		perror("allocator");
		exit(1);
	}

	new_sigaction.sa_handler = catch_sigusr2;
	if (sigaction(SIGUSR2, &new_sigaction, &old_sigaction) == -1){
		perror("allocator");
		exit(1);
	}

        new_sigaction.sa_handler = catch_siginfo;
        if (sigaction(SIGINFO, &new_sigaction, &old_sigaction) == -1){
                perror("allocator");
                exit(1);
	    }


	/* Ignore SIGPIPE, SIGHUP,SIGQUIT,SIGTERM,SIGTSTP,SIGCHLD,
	 * SIGUSR1,SIGUSR2,SIGMIGRATE
	*/
        new_sigaction.sa_handler = SIG_IGN;
        if (sigaction(SIGPIPE, &new_sigaction, &old_sigaction) == -1){
                perror("allocator");
                exit(1);
        }

	new_sigaction.sa_handler = SIG_IGN;
	if (sigaction(SIGHUP, &new_sigaction, &old_sigaction) == -1){
		perror("allocator");
		exit(1);
	}
	new_sigaction.sa_handler = SIG_IGN;
	if (sigaction(SIGQUIT, &new_sigaction, &old_sigaction) == -1){
		perror("allocator");
		exit(1);
	}
	new_sigaction.sa_handler = SIG_IGN;
	if (sigaction(SIGTERM, &new_sigaction, &old_sigaction) == -1){
		perror("allocator");
		exit(1);
	}
	new_sigaction.sa_handler = SIG_IGN;
	if (sigaction(SIGTSTP, &new_sigaction, &old_sigaction) == -1){
		perror("allocator");
		exit(1);
	}

	new_sigaction.sa_handler = SIG_IGN;
	if (sigaction(SIGMIGRATE, &new_sigaction, &old_sigaction) == -1){
		perror("allocator");
		exit(1);
	}
}

#ifdef	iPSC860_COMPAT
#define	EVEN(n)	(!(n & 1))
/*
 * old_gray_mesh() 
 *
 * Map a mesh onto the hypercube.
 */
static int
old_gray_mesh(rows, cols, count, node_list)
int		rows;		/* rows in mesh: must be power of 2 */
int		cols;		/* cols in mesh: must be power of 2 */
int		count;		/* nodes in node list */
LP_MAP_T	node_list;	/* nodes to map */
{
	int i, j, node_index, gray_index;

	node_index = 0;
	gray_index = 0;
	for (i = 0; i < rows; i++) {
		if (EVEN(i)) {
			for (j = 0; j < cols; j++) {
				node_index = (i * cols) + j;
				node_list[node_index] = gray(gray_index++);
			}
		}
		else { /* ODD */
			for (j = cols - 1; j >= 0; j--) {
				node_index = (i * cols) + j;
				node_list[node_index] = gray(gray_index++);
			}
		}
	}
}

/*
 * is_ipsc860() 
 *
 * Returns TRUE if we are running on the iPSC/860.
 */
int
is_ipsc860()
{
	struct processor_basic_info	pinfo;	/* Processor info structure */
	mach_msg_type_number_t	size;		/* Size (in bytes) of pinfo */
	mach_port_t		host;		/* Host port */
	static int		called = 0;	/* Have we been called yet */
	static int		ipsc860;	/* Are we an ipsc860 */
	mach_msg_type_number_t	pcount;		/* Number of processors */
	mach_port_t		processor;	/* Processor port */
	processor_array_t	plist;		/* List of processor ports */
	kern_return_t		kstat;		/* Mach kernel call status */
	mach_port_t		privileged_host_port;

	if (!called) {
		called = 1;
		privileged_host_port = task_by_pid(-1);
		kstat = host_processors(privileged_host_port, &plist, &pcount);
		processor = plist[0];
		size = PROCESSOR_BASIC_INFO_COUNT;
		kstat = processor_info(processor, PROCESSOR_BASIC_INFO, &host,
		               (processor_info_t) &pinfo, &size);
		if (pinfo.cpu_subtype == CPU_SUBTYPE_iPSC860) {
			ipsc860 = TRUE;
#ifdef	DEBUG
printf("is_ipsc860() returns TRUE\n");
#endif	DEBUG
		}
		else {
			ipsc860 = FALSE;
#ifdef	DEBUG
printf("is_ipsc860() returns FALSE\n");
#endif	DEBUG
		}
	}
	return ipsc860;
} 
#endif	iPSC860_COMPAT

void
dump_part_tbl(dfile)
    FILE *dfile;
{
    int i;
    int j;
    int k;
    BUCKET_T *bucket;
    PART_T *part;
    LAYER_T *layer;
    
    fprintf(dfile,"**** Dumping Partitiion Table @ %lu ****\n", time(0));
    for (i = 0; i < HASH_TBL_LENGTH; i++) {
        for (bucket = part_tbl[i]; bucket; bucket = bucket->next) {
            for (j = 0; j < BUCKET_SIZE; j++ ) {
                if ((part = bucket->slot[j].p) != (void *)0) {
                    fprintf(dfile,"Hash bucket %d  Slot %d\n", i, j);
                    fprintf(dfile,"\tPARTITION: 0x%lx\n", part);
                    fprintf(dfile,"\tprev (CONSUMER_T *): 0x%lx\n",part->prev);
                    fprintf(dfile,"\tnext (CONSUMER_T *): 0x%lx\n",part->next);
                    fprintf(dfile,"\ttype: %ld\n",part->type);
                    fprintf(dfile,"\talloc_prev (PART_T *): 0x%lx\n",part->alloc_prev);
                    fprintf(dfile,"\talloc_next (PART_T *): 0x%lx\n",part->alloc_next);
                    /**                fprintf(dfile,"\tconflict: %lx\n",part->conflict); **/
                    fprintf(dfile,"\tparent_alloc_lyr (LAYER_T *): 0x%lx\n",part->parent_alloc_lyr);
                    fprintf(dfile,"\tparent_sched_lyr (LAYER_T): 0x%lx\n",part->parent_sched_lyr);
                    fprintf(dfile,"\tbitmap (BITMAP_T): %lx\n",part->bitmap);
		    fprintf(dfile,"\t\tbitmap->rows: %u\n", part->bitmap->rows);
		    fprintf(dfile,"\t\tbitmap->cols: %u\n", part->bitmap->cols);
		    fprintf(dfile,"\t\tbitmap->row_offset: %u\n", part->bitmap->row_offset);
		    for (k=0;k<part->bitmap->cols;k++) {
			fprintf(dfile,"\t\tbitmap->colmap[%d]: 0x%lx\n", k, part->bitmap->colmap[k]);
		    }
		    fprintf(dfile,"\tLogical to Physical node mapping\n");
                    for (k=0;k<part->slots;k++) {
                        fprintf(dfile, "\t\tlogical node %d = physical node %d\n", k, part->lp[k]);
		    }
                    fprintf(dfile,"\tslots: %d\n",part->slots);
                    fprintf(dfile,"\tnodes: %d\n",part->nodes);
                    fprintf(dfile,"\trows: %d\n",part->rows);
                    fprintf(dfile,"\tcols: %d\n",part->cols);
                    fprintf(dfile,"\tcur_priority: %d\n",part->cur_priority);
                    fprintf(dfile,"\tmax_priority: %d\n",part->max_priority);
                    fprintf(dfile,"\tinode: %ld\n",part->inode);
                    switch(part->sched) {
                    case UNIX:
                        fprintf(dfile,"\tsched: UNIX\n");
                        break;
                    case GANG:
                        fprintf(dfile,"\tsched: GANG\n");
                        break;
                    case SPACE_SHARE:
                        fprintf(dfile,"\tsched: SPACE_SHARE\n");
                        break;
                    default:
                        fprintf(dfile,"\tsched: %d\n",part->sched);
                        break;
                    };
                    fprintf(dfile,"\trollin_quantum: %u\n",part->rollin_quantum);
                    fprintf(dfile,"\trolled_in: %u\n",part->rolled_in);
                    fprintf(dfile,"\telapsed: %u\n",part->elapsed);
                    fprintf(dfile,"\towner: %d\n",part->owner);
                    fprintf(dfile,"\tgroup: %d\n",part->group);
                    fprintf(dfile,"\tprotection: %o\n",part->protection);
                    fprintf(dfile,"\tactive_lyr (LAYER_T *): 0x%lx\n",part->active_lyr);
                    if (part->child_sched_lyr == NULL) {
                        fprintf(dfile,"\tchild_sched_lyr (LAYER_T *): NULL\n");
                    } else {
                        fprintf(dfile,"\tchild_sched_lyr (LAYER_T *):\n");
                        for (layer = part->child_sched_lyr; layer; layer= layer->next) {
                            fprintf(dfile,"\t\tlayer: 0x%lx\n", layer);
                            fprintf(dfile,"\t\tprev (LAYER_T *): 0x%lx\n", layer->prev);
                            fprintf(dfile,"\t\tnext (LAYER_T *): 0x%lx\n", layer->next);
                            fprintf(dfile,"\t\tpart (PART_T *): 0x%lx\n", layer->part);
                            fprintf(dfile,"\t\tbitmap (bitmap *): 0x%lx\n", layer->bitmap);
                            fprintf(dfile,"\t\tpriority: %d\n", layer->priority);
                            fprintf(dfile,"\t\tnum_free: %d\n", layer->num_free);
                            fprintf(dfile,"\t\tconsumer (CONSUMER_T *): 0x%lx\n", layer->consumer);
                            fprintf(dfile,"\t\tsched_prev (LAYER_T *): 0x%lx\n", layer->sched_prev);
                            fprintf(dfile,"\t\tsched_next (LAYER_T *): 0x%lx\n\n", layer->sched_next);
                        }
                    }
                    if (part->child_alloc_lyr == NULL) {
                        fprintf(dfile,"\tchild_alloc_lyr (LAYER_T *): NULL\n");
                    } else {
                        fprintf(dfile,"\tchild_alloc_lyr (LAYER_T *):\n");
                        for (layer = part->child_alloc_lyr; layer; layer= layer->next) {
                            fprintf(dfile,"\t\tlayer: 0x%lx\n", layer);
                            fprintf(dfile,"\t\tprev (LAYER_T *): 0x%lx\n", layer->prev);
                            fprintf(dfile,"\t\tnext (LAYER_T *): 0x%lx\n", layer->next);
                            fprintf(dfile,"\t\tpart (PART_T *): 0x%lx\n", layer->part);
                            fprintf(dfile,"\t\tbitmap (bitmap *): 0x%lx\n", layer->bitmap);
                            fprintf(dfile,"\t\tpriority: %d\n", layer->priority);
                            fprintf(dfile,"\t\tnum_free: %d\n", layer->num_free);
                            fprintf(dfile,"\t\tconsumer (CONSUMER_T *): 0x%lx\n", layer->consumer);
                            fprintf(dfile,"\t\tsched_prev (LAYER_T *): 0x%lx\n", layer->sched_prev);
                            fprintf(dfile,"\t\tsched_next (LAYER_T *): 0x%lx\n\n", layer->sched_next);
                        }
                    }

                    fprintf(dfile,"\tsched_list (LAYER_T *): 0x%lx\n",part->sched_list);
                    fprintf(dfile,"\tstart_time: 0x%lx\n",part->start_time);
                    fprintf(dfile,"\tstatus: %d\n",part->status);
                    fprintf(dfile,"\tmax_num_layers: %d\n",part->max_num_layers);
                    fprintf(dfile,"\tgang_ancestor: %d\n",part->gang_ancestor);
                }
            }
        }
    }
}

void
dump_appl_tbl(dfile)
    FILE *dfile;
{
    int i;
    int j;
    int k;
    BUCKET_T *bucket;
    APPL_T *appl;

    fprintf(dfile,"**** Dumping Application Table @ %lu ****\n", time(0));
    for (i = 0; i < HASH_TBL_LENGTH; i++) {
        for (bucket = appl_tbl[i]; bucket; bucket = bucket->next) {
            for (j = 0; j < BUCKET_SIZE; j++ ) {
                if ((appl = bucket->slot[j].p) != (void *)0) {
                    fprintf(dfile,"Hash bucket %d  Slot %d\n", i, j);
                    fprintf(dfile,"\tAPPLICATION: 0x%lx\n", appl);
                    fprintf(dfile,"\tprev (CONSUMER_T *): 0x%lx\n",appl->prev);
                    fprintf(dfile,"\tnext (CONSUMER_T *): 0x%lx\n",appl->next);
                    fprintf(dfile,"\ttype: %ld\n",appl->type);
                    /**                fprintf(dfile,"\tconflict: %lx\n",appl->conflict); **/
                    fprintf(dfile,"\tparent_lyr (LAYER_T *): 0x%lx\n",appl->parent_lyr);
                    fprintf(dfile,"\tbitmap (BITMAP_T): %lx\n",appl->bitmap);
                    fprintf(dfile,"\t\tbitmap->rows: %u\n", appl->bitmap->rows);
                    fprintf(dfile,"\t\tbitmap->cols: %u\n", appl->bitmap->cols);
                    fprintf(dfile,"\t\tbitmap->row_offset: %u\n", appl->bitmap->row_offset);
                    for (k=0;k<appl->bitmap->cols;k++) {
                        fprintf(dfile,"\t\tbitmap->colmap[%d]: 0x%lx\n", k, appl->bitmap->colmap[k]);
                    }
                    fprintf(dfile,"\tLogical to Physical node mapping\n");
		    for (k=0;k<appl->size;k++) {
			fprintf(dfile, "\t\tlogical node %d = physical node %d\n", k, appl->lp[k]);
		    }
                    fprintf(dfile,"\tsize: %d\n",appl->size);
                    fprintf(dfile,"\tnrows: %d\n",appl->nrows);
                    fprintf(dfile,"\tncols: %d\n",appl->ncols);
                    fprintf(dfile,"\tpriority: %d\n",appl->priority);
                    fprintf(dfile,"\trolled_in: %u\n",appl->rolled_in);
                    fprintf(dfile,"\telapsed: %u\n",appl->elapsed);
                    fprintf(dfile,"\tpgroup: %d\n",appl->pgroup);
                    fprintf(dfile,"\tuid: %d\n",appl->uid);
                    fprintf(dfile,"\tgid: %d\n",appl->gid);
                    fprintf(dfile,"\tnx_acctid: %d\n",appl->nx_acctid);
                    switch(appl->appl_stat) {
                    case APPL_NOT_STARTED:
                        fprintf(dfile,"\tappl_stat: APPL_NOT_STARTED\n");
                        break;
                    case APPL_GANG_START:
                        fprintf(dfile,"\tappl_stat: APPL_GANG_START\n");
                        break;
                    case APPL_GANG_STOP:
                        fprintf(dfile,"\tappl_stat: APPL_GANG_STOP\n");
                        break;
                    default:
                        fprintf(dfile,"\tappl_stat: %d\n",appl->appl_stat);
                        break;
                    };
                    fprintf(dfile,"\tstart_time: 0x%lx\n",appl->start_time);
#ifdef PARACORE
                    switch(appl->appl_core) {
                    case APPL_NOT_DUMPING_CORE:
                        fprintf(dfile,"\tappl_core: APPL_NOT_DUMPING_CORE\n");
                        break;
                    case APPL_DUMPING_CORE:
                        fprintf(dfile,"\tappl_core: APPL_DUMPING_CORE\n");
                        break;
                    default:
                        fprintf(dfile,"\tappl_core: %d\n",appl->appl_core);
                        break;
                    };
#endif /* PARACORE */
                }
            }
        }
    }
}





