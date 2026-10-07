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
 * Copyright (c) 1993, Intel Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: load_leveld.c,v $
 * Revision 1.19  1994/11/19  03:07:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.18  1994/11/18  20:53:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.17  1994/05/27  13:02:55  stefan
 * Moved definition of NO_TIMEOUT here (cosmetic).
 * Also now the paging behavior of a node is included in the calculation of the
 * load value.
 * Also, we now use rforkmulti() instead of rfork() in a loop to fork the load
 * leveler to all other service nodes.
 *
 *  Reviewer: yazz
 *  Risk: low to medium
 *  Benefit or PTS #:
 *  Testing: developer testing
 *  Module(s): load_leveld.c
 *
 * Revision 1.16  1994/02/16  16:31:15  stefan
 * Merged version 1.15.2.1 into main trunk.
 *
 * Revision 1.15.2.1  1994/02/16  14:40:45  stefan
 *  Reviewer: bolsen (Locus)
 *  Risk: low to medium
 *  Benefit or PTS #: 7899, 8028, 8039
 *  Testing: developer testing
 *  Module(s): svr/user/etc/load_level/load_level_types.h
 *             svr/user/etc/load_level/load_leveld.c
 *             svr/user/etc/load_level/loadlevel
 *             svr/user/etc/load_level/osf1_dep.c
 *             svr/user/etc/load_level/sll_load_level_types.h
 *             svr/user/etc/load_level/sll_load_leveld.c
 *             svr/user/etc/load_level/parameters
 *
 * For PTS #7899 I have added 2 new parameters to the load leveler daemon:
 * root_fs_node_target   boolean that specifies if root_fs_node should be
 *                       used as a target node for load leveling.
 *                       Default: 0
 * root_fs_node_source   boolean that specifies if root_fs_node should be
 *                       used as source node for load leveling.
 *                       Default: 1
 *
 * For PTS #8028 the default behaviour has been changed so that process
 * migration is disabled by default and can be switched on using the command
 * line switch -d.
 *
 * For PTS #8039 a check has been implemented if a load_leveld is already
 * running on a node and if this is the case the new load leveler exits with
 * an error message.
 * Also, I have fixed a problem where load_leveld dumped core in migrate() when
 * the startup node is not included in the nodes_to_use parameter.
 * Instead of migrating to another node load_leveld now exits on the startup
 * node after rforking it's children.
 * In addition, now it is checked if get_tnc_port() returns MACH_PORT_DEAD and
 * in this case it is assumed that the corresponding peer is simply a little
 * late.
 *
 * Revision 1.15  1993/11/19  18:17:23  bolsen
 *  Reviewer: Mike Barnett and Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: 5389 - parameter file lines with > 79 chars
 * 		   5390 - incorrect size for shared memory segment
 *  Testing: LCC Load_leveld tests
 *  Module(s): user/etc/load_level/load_leveld.c
 * 	    user/etc/load_level/onnode.c
 *
 * Revision 1.14  1993/09/30  18:08:35  stefan
 * Made the load leveler multi-threaded
 *
 * Revision 1.13  1993/07/14  18:51:02  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.12  1993/05/20  16:04:58  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 1.1.1.9  1993/07/01  21:17:07  cfj
 * Adding new code from vendor
 *
 * Revision 1.11  1993/05/13  09:18:41  stefan
 * Integrated static load leveling support.
 *
 * Revision 1.10  1993/05/06  19:29:01  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.9  1993/04/28  13:55:11  stefan
 * Fixed bug #4471: The load leveling daemon now syslogs only a warning and
 * exits with error code 0 if there is only 1 service node.
 *
 * Also if the load leveling daemon is started by another user than root,
 * errno is set to EPERM before callinf error_fatal in order to get a meaningful
 * error message.
 *
 * I also fixed a bug which caused that when an attempt to migrate a process
 * failed (this can only happen when the process doesn't exist any longer) it
 * was attempted to migrate the same process to a number of other nodes.
 * Changing a continue to a break did the job.
 *
 * Revision 1.8  1993/02/18  12:20:06  stefan
 * Merged Locus 02-17-93 code drop.
 *
 * Revision 1.7  1993/01/22  19:31:30  stefan
 * Merged Locus 01-20-93 code drop.
 *
 * Revision 1.6  1992/12/18  20:37:06  stefan
 * Modified the way how a random node is choosen in send_info_to_other_node to
 * make the code a little bit more efficient on systems which are sparsely
 * populated with nodes:
 *   - Introduced a new data structure used_nodes which is an array that
 *     contains only the used node numbers.
 *   - used_nodes is initialized in init_load_leveler().
 *   - send_info_to_other_node() uses a random number as the index into
 *     used_nodes. If by chance the local mode number is choosen then the next
 *     used node is selected.
 * Another benefit of used_nodes is that it is now used to rfork() the load
 * leveling daemon to the set of used nodes.
 *
 * Revision 1.5  1992/12/14  14:28:52  stefan
 * Made lots of modifications for performance tuning and bug fixing:
 *   - Elimination of duplicates in load vector is done in O(n) now instead
 *     od O(n^2).
 *   - Invalid elements in load vector are now marked by a node number (int)
 *     of -1 instead of a load meassure (double) of -1. Integer comparison is
 *     still cheaper than floating point.
 *   - node_info_ptr is now replaced by node_in_use, which is only an array
 *     which is indexed by the node number. Hence we have complexity of O(1)
 *     instead of O(n).
 *   - node_self() is now cached in local_node_num.
 *   - Now do an error exit if there are not at least 2 nodes.
 *   - random_num is now correctly decremented by underload_ratio not
 *     underload.
 *   - underload_ratio is now calculated on the fly. This saves half of the
 *     calculations of underload_ratio in the average case.
 *   - send_info_to_other_node() now chooses a used node (simplified the code a
 *     little bit).
 *   - Changed permissions for shared memory segment to 0644. This fixed a bug
 *     where only root could create and access the shared memory segment.
 *   - Changed the way how the error message is prepended by the program name.
 *     Introduced error_init for this reason.
 *   - Call error_init() at the beginning.
 *   - use migrate() to migrate myself to another node. This avoids a possible
 *     race condition where the process continues to run on the old node after
 *     he has sent the SIGMIGRATE to himself. To do that the variable migrate
 *     had to be renamed to enable_migrate.
 *   - implemented the function daemonize() to make the load leveler a real
 *     daemon. This function does all the necessary stuff including creating
 *     a new process group for the load leveling daemons.
 *   - changed the way where the key files for the shared memory keys are
 *     created. They are now created in a own directory
 *     (/etc/load_level/shm_keys). The -K flag now must have a path to a
 *     directory as an argument. This was necessary to be able to start
 *     multiple load levelers in different partitions. Now a different
 *     directory can be used for the keys of each partition. Maybe the -K
 *     parameter should be changed to -k and should be documented then.
 *
 * Revision 1.4  1992/12/11  03:08:44  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  23:01:45  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:36:13  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:49:45  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.10  1992/10/12  17:22:23  stefan
 * added support for migration logs.
 *
 * fixed bug where lines in parameters file which begin with tabs or blanks
 * are skipped.
 *
 * fixed bug which caused a comment in the parameters file only to be ignored
 * if it was preceeded by whitespace.
 *
 * fixed a bug which caused that a command name in cmds was not null terminated.
 *
 * fixed a bug which caused lines in migrate_commands which begin with #
 * to be not ignored.
 *
 * renamed num_procs_to_migrate to local_node_overload.
 *
 * local_node_overload and load_vec_copy[i].underload are now also decremented
 * by per_process_avg_load.
 *
 * Revision 1.1.1.7  1993/05/03  17:57:48  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.22  93/08/03  10:48:08  bolsen
 * Fixed the following bugs:
 * [Bug 303/5389]  parameter file lines with > 79 chars fail
 * [Bug 304/5390]  use structure size for shared memory segment OR zero
 * 
 * Revision 3.21  93/05/04  12:21:52  bolsen
 * [Bug 235] load_leveld error message misleading (from Intel).
 * [Bug 236] continue should be a break (from Intel).
 * [SPE 0019] errno should be valid when calling error_fatal() (from Intel).
 * 
 * Revision 3.20  93/04/23  16:19:09  bolsen
 * [SPE 0007] reference common header file for shared memory structures,
 * 	variables and NEW macros (ie. common to load_leveld and onnode
 * 	(fast & fastnode)).
 * 
 * Revision 3.19  93/02/03  10:01:29  mbarnett
 * Fixed comment in history log.
 * 
 * Revision 3.18  93/02/03  09:38:10  mbarnett
 * I modified the code so that the owner of the created shared memory has write
 * permissions.  As a result of this change, I also added bullet proofing so 
 * that no user other than root will be able to invoke the load leveler daemon.
 * This resolves SPE #0012.
 *
 * Revision 3.17  93/01/20  14:01:35  yazz
 * Corrected computation of proj_id parameter used in ftok() call, so
 * that node numbers of the form X*128 - 1 (like 127, 255) will work.
 * 
 * Revision 3.16  92/12/30  16:18:06  mbarnett
 * The code was changed so that scanning for duplicate node information in the
 * load vector within the function re_dispatch_local_procs is no longer 
 * necessary.  This resolves SPE #0008.  Also, changes were made so that the 
 * random_node number in send_info_to_other_node() is now selected in a much 
 * quicker and more efficient manner.  This resolves SPE #0017.
 * 
 * Revision 3.15  92/12/28  15:27:44  mbarnett
 * Modified load_leveld.c code so that the key files used to generate 
 * the ipc keys for shared memory are now created in a private 
 * directory(/etc/load_level/shm_keys).  This change resolves SPE #0010.  Also,
 * the load_leveld.c code was changed to cache node_self(SPE #0014).  Finally,
 * the load_leveld.c code was changed so that all error messages willl include 
 * the program name(SPE #0013).
 * 
 * Revision 3.14  92/12/22  12:55:59  mbarnett
 * The variable random_num in the function "re_dispatch_local_procs" is now
 * decremented by underload_ratio.  This fixes bug #0132.  Also, the load 
 * leveler daemon code(load_leveld.c) has been modified to become a process
 * group leader at initialization time.
 * 
 * Revision 3.13  92/12/02  14:29:41  mbarnett
 * Modified load leveler daemon code to generate special unique shared memory 
 * keys for the load leveler VSTNC test.
 * 
 * Revision 3.12  92/11/25  12:26:14  mbarnett
 * Modified key generation code so that fewer key files are used in order
 * to produce unique interprocess communication keys for shared memory 
 * segments for each node.
 * 
 * Revision 3.11  92/11/12  15:11:58  mbarnett
 * Put in fixes for bugs #0096 and #0099.  First, I modified the mode flag
 * which is passed in as the third parameter to the shmget function.  The
 * changed permissions mode will now allow access of the shared memory 
 * segment by users other than root.  Also, the pathname of the file which
 * is passed in as the first parameter to the "ftok" function is now appended
 * with the local node number in order to cause a unique key to be generated
 * in when more than 256 nodes exist.
 * 
 * Revision 3.10  92/10/23  15:34:59  mbarnett
 * I changed the variable name "num_procs_to_migrate" to 
 * "local_node_overload"(#0077).  I also made changes so that lines in the
 * "parameters" file that begin with blanks or tabs are not automatically 
 * skipped(#0080).  Also, comment lines in the "migrate_commands" file are
 * now allowed(#0081).  I also made changes so that commands names as read in
 * from the "migrate_commands" file will always be null terminated(#0082). Also,
 * comments in the "parameters" file no longer need to be preceeded by blanks or
 * tabs(#0083).  In addition, I made changes to the load leveler daemon so that
 * it would cache locally its own pid(#0084).  Also, commands listed in the
 * "migrate_commands" file can now contain embedded whitespace and/or "=" 
 * characters(#0085).  Finally, I increased the precision of the timeout 
 * parameters(variables) "send_timeout" and "re_dispatch_timeout" from seconds
 * to microseconds(#0086).
 * 
 * Revision 3.9  92/10/08  15:46:22  mbarnett
 * I made changes to accomodate the new configurable parameter 
 * "per_process_avg_load".  Also, the variable num_underloaded_nodes
 * is now decremented each time a node changes from being underloaded
 * to not being underloaded.  This fixes bug #62 and thus prevents the 
 * load leveler from executing an infinite loop for certain settings 
 * of the "min_underload" parameter.
 * 
 * Revision 3.8  92/07/10  16:19:49  mbarnett
 * Modified comment line.
 * 
 * Revision 3.7  92/06/24  14:26:21  mbarnett
 * Changed printf call to call error_nonfatal routine instead.  Also, fixed 
 * parameters in call to getopt function.
 * 
 * Revision 3.6  92/06/15  13:03:34  mbarnett
 * Made changes to use shared memory segment for the load vector.  Also, removed
 * the following fuctions: send_sig_to_pgroup, re_read_config_data. In addition,
 * the following functions were added: send_sigkill_to_pgroup,
 * send_sigusr2_to_pgroup, got_to_sleep, get_nodes_from_parameters,
 * modify_node_info.
 * 
 * Revision 3.3  92/05/13  09:17:40  mbarnett
 * Previous check-in was incomplete.
 * 
 * Revision 3.2  92/04/02  13:23:24  mbarnett
 * Added code to check for errors returned from get_tnc_port & set_tnc_port.
 * 
 * Revision 3.1  92/03/31  11:04:39  mbarnett
 * Replaced calls to signal and sigsetmask with calls to sigaction & 
 * sigprocmask.
 * Also, added calls to newsignal routines sigemptyset and sigaddset.
 * 
 */

/*****************************************************************************
**  PROGRAM: load_leveld
** 
**  DESCRIPTION: This program is the load_leveler daemon which is responsible
**               for exchanging load information between the nodes, 
**               determining which nodes are overloaded and underloaded, and 
**               re-dispatching processes from overloaded nodes to underloaded 
**               nodes.  The load leveler daemon runs on each node and each 
**               daemon running on any one particular service node only 
**               maintains load information about a subset of other service 
**               nodes.  Also, each load leveler daemon only re-dispatches 
**               local processes to a subset of other service nodes.
**
*****************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include "load_level_com.h"
#include "load_level_types.h"

#ifdef SLL
#include "sll_load_level_types.h"
#endif /* SLL */

#ifdef MIGRATELOG
/* string containing the name of the command found by
   get_next_migration_process() */
char migrated_command[CMD_LEN];

/* int which indicates if we want to do migration logs */
int domigratelog = 0;
#endif /* MIGRATELOG */

#define sig_block_mask sigmask(SIGUSR1)|sigmask(SIGUSR2)
#define sig_unblock_mask 0


/* pointer to load info structure which contains the vector structure which
   in turn contains the load information for a number of nodes */
struct load_info *load_info_ptr;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* maximum number of commands in commands_file */
#define MAX_CMDS 100 

/* mode value to set for shared memory segment so that other users other
   than root can access the shared memory segment */
#define READ_BY_OTHERS 0644

/*
 * Priority value of the load leveler.
 */
#define LLDPRIO	(-10)

/*
 * Define eternity for timeouts.
 */
#define NO_TIMEOUT	(10.0 * 24.0 * 60.0 * 60.0 * 1000000.0)

/* variable containing the frequency in microseconds with which the load 
information is sent to other nodes */
double send_timeout = 5000000.0;

/* variable containing the frequency in microseconds with which the node 
overload algorithm and re-dispatch algorithm are invoked */
double re_dispatch_timeout = 7000000.0;

/* number of load vector elements, which determines how much remote 
   nodes' load information is contained by the local node. */
unsigned num_lvec_elements;

/* variable indicating whether the migrate_commands file contains an inclusion
   or exclusion list of commands for re-dispatching */
int inclusion_list = 0;
 
/* variable containing the minimum underload amount for which a node is to
   be considered underloaded */
double min_underload = 1.0;

/* variable containing the minimum underload amount for which a node is to
   be considered overloaded */
double min_overload = 1.0;

/* a process will only be migrated if the cpu runtime of the process is at
   least the number of seconds specified in this variable */
double min_cputime = 1.0;

/* the average load generated by a single process */
double per_process_avg_load = 1.0;

/* array filled with the values of the weight factors as specified in the
   parameters file */
long parm_data[3] = {50, 25, 25};

/* variable indicating if processes should be migrated to ROOT_FS_NODE */
int root_fs_node_target = 0;

/* variable indicating if processes should be migrated from ROOT_FS_NODE */
int root_fs_node_source = 1;

/* variable containing the amount of load generated by a single page-in
   per second */
double pagein_load = 0.05;

/* variable containing the amount of load generated by a single page-out
   per second */
double pageout_load = 0.10;

/* variable containing the preferred sampling interval for paging
   statistics */
double pgstat_pref_interval = 10000000.0;

/* variable containing the maximum sampling interval for paging
   statistics */
double pgstat_max_interval = 30000000.0;

/* pointer to an array containing the node numbers in the system */
int *node_num_ptr;

/* array containing the commands extracted from the migrate_commands file */
char cmds[MAX_CMDS][CMD_LEN];

/* number of service nodes in the system */
int num_nodes;

/* pointer to the string containing the pathname of the parameters file */
char *parm_file = { "/etc/load_level/parameters" };

/* pointer to the string containing the pathname to a directory in which
   the key files for shared memory are created */
extern char *key_dir = { KEY_DIR };

/* pointer to the string containing the pathname of the migrate commands 
   file */
char *commands_file = { "/etc/load_level/migrate_commands" };

/* variable containing undocumented parameter used for testing purposes */
int iparm = -1;

/* names of parameters as they should appear in the parameters file */
static char *weight_str[] = { "first_weight_factor",
                              "second_weight_factor",
                              "third_weight_factor"};

/* array containing tunable weight factors which allow one to assign special
   importance to specific interval load averages */
double weight_factor[3]; 

/* pid of the local load leveler daemon process */
pid_t mypid;

/* number of nodes which are used by this load leveler */
int num_nodes_in_use;

/* number of slots */
int num_slots;

/* variable to cache local node number */
int local_node_num;

/* pointer to array which indicates if a node number is a duplicate */
char *duplicate;

/* pointer to array which indicates if a node is in use */
char *node_in_use;

/* pointer to array which holds the numbers of the nodes in use */
int *used_nodes;

/* node number of ROOT_FS_NODE */
int root_fs_node;

/*
 * array of page-fault statistics.
 */
struct pg_statistics	pg_stat[MAX_PG_STATS];

/*
 * index of newest entry in pg_stat.
 */
int	pg_stat_index = 0;


/* external functions */
extern void error_fatal(char *);
extern void error_nonfatal();
extern void init_com();
extern void get_node_array();
extern void get_load_average(double *);
extern int send_load_info(int);
extern void get_nodes_from_parameters(FILE *, char *, int);
extern void modify_node_port_map(int *, int, int);
extern int check_local_pid();
extern void initialize_migration_processes();
extern int get_next_migration_process();
extern void error_init();
extern void start_rx_info_thread();
extern void lock_load_vec();
extern void unlock_load_vec();
extern int get_root_fs_node();
#ifndef SLL
extern void wait_for_timeout(double *, double *, double *);
#else /* SLL */
extern void wait_for_timeout(double *, double *, double *, double *);
#endif /* SLL */

/* local functions */
void init_load_leveler();
void read_in_parameters();
void read_in_commands_list();
char *get_data_from_str(char *, int);
int send_sigkill_to_pgroup();
int send_sigusr2_to_pgroup();
int go_to_sleep();
void compute_local_load_measure();
void re_dispatch_local_procs();
void send_info_to_other_node();
int on_commands_list(char *);
void receive_load_vector(int [], double[]);
void get_nodes_from_parameters(FILE *, char *, int);
void modify_node_info(int *, int, int);
void daemonize();

main(int argc,
     char *argv[])
{

    int i, temp_node, retval;
    int shmid;
    long ran;
    double remain_send_time, remain_re_dispatch_time, elapsed_time;
    char c;
    extern char *optarg;
    int errflg = 0;
    int enable_migrate = FALSE;
    struct sigaction actterm;
    struct sigaction actusr1;
    struct sigaction actusr2;
    FILE *filep;
    struct shmid_ds stat_buf;
    int j;
    int fork_count;
    int *fork_nodes;
    int *fork_errnos;
    int *fork_pids;
#ifdef SLL
    double	remain_fast_node_time;
#endif /* SLL */

    /* become a real daemon */
    daemonize();

    /* initialize error logging and verify that the load leveler is being
       invoked by root */
    error_init(argv[0]);
    if ( getuid() != 0 ) {
	errno = EPERM;
	error_fatal("only root may execute this program");
    }

#ifndef SLL
#ifndef MIGRATELOG
    while ((c = getopt(argc, argv, "c:p:dI:K:")) != EOF) {
    	switch (c) {
#else /* MIGRATELOG */
    while ((c = getopt(argc, argv, "c:p:dI:K:L")) != EOF) {
	switch (c) {
		case 'L':
			domigratelog = 1;
			break;
#endif /* MIGRATELOG */
#else /* SLL */
#ifndef MIGRATELOG
    while ((c = getopt(argc, argv, "c:p:dsI:K:")) != EOF) {
	switch (c) {
                case 's':
			static_load_leveling = 1;
			break;
#else /* MIGRATELOG */
    while ((c = getopt(argc, argv, "c:p:dsI:K:L")) != EOF) {
	switch (c) {
		case 'L':
			domigratelog = 1;
			break;
                case 's':
			static_load_leveling = 1;
			break;
#endif /* MIGRATELOG */
#endif /* SLL */
    		case 'c':
    			commands_file = optarg; 
    			break;
    		case 'p':
    			parm_file = optarg;
    			break;
    		case 'd':
    			enable_migrate = TRUE;
    			break;
    		case 'I':
    			iparm = (int)strtoul(optarg, (char **)0, 10);
    			break;
                case 'K':
                        key_dir = optarg;
                        break;
    		default:
    			errflg++;
    			break;
    	}
    }
    if (errflg)
    	error_nonfatal("load leveler argument error");

    /* read in configuration information and initialize data structures */
    init_load_leveler();

    /* load leveling only makes sense with at least 2 nodes */
    if ( num_nodes_in_use < 2 ) {
        error_nonfatal("(warning) only one node is available, exiting\n");
        exit(0);
    }

    /* set the local timeout values to the values read in from the 
       parameters file */
    remain_send_time = send_timeout;
    remain_re_dispatch_time = re_dispatch_timeout;
#ifdef SLL
    remain_fast_node_time = fast_node_timeout;
#endif /* SLL */

    /*
     * Allocate the various arrays for rforkmulti().
     */
    fork_nodes = (int *) calloc((size_t) num_nodes_in_use - 1, sizeof(int));
    fork_errnos = (int *) calloc((size_t) num_nodes_in_use - 1, sizeof(int));
    fork_pids = (int *) calloc((size_t) num_nodes_in_use - 1, sizeof(int));
    if ( fork_nodes == NULL || fork_errnos == NULL || fork_pids == NULL )  {
        /* OOPS - no memory */
        error_fatal("calloc failed");
    }

    /*
     * Initialize fork_pids with all other nodes that are defined 
     * within the set of nodes to use.
     */
    j = 0;
    for ( i = 0; i < num_nodes_in_use; i++ ) {
        if (*(used_nodes + i) != local_node_num) {
	    *(fork_nodes + j++) = *(used_nodes + i);
        }
    }
    fork_count = j;

    /*
     * Use rforkmulti to start the other daemons.
     */
    retval = rforkmulti(&fork_count, fork_nodes, fork_errnos, fork_pids);
    if ( retval < 0 ) {
	error_fatal("load leveler rforkmulti fatal error");
    }

    /*
     * Free the various arrays for rforkmulti().
     */
    free(fork_nodes);
    free(fork_errnos);
    free(fork_pids);

    /* recache local node number */
    local_node_num = node_self();

    if ( *(node_in_use + local_node_num) != 1 ) {
	/*
	 * The local node is not included in the node list - exit.
	 * (Only possible for the father process)
	 */
	exit(0);
    }


    /* store locally the pid of this load_leveld process */
    mypid = getpid();
    /* setup to handle receiving a signal from a user which will cause the
       load leveler daemon to send a SIGKILL signal to the process group */
    sigaction(SIGTERM, (struct sigaction *)0, &actterm);
    actterm.sa_handler = (void (*)(int))send_sigkill_to_pgroup;
    sigaction(SIGTERM, &actterm, (struct sigaction *)0);

    /* setup to handle receiving a SIGUSR1 signal from a user */
    sigaction(SIGUSR1, (struct sigaction *)0, &actusr1);
    actusr1.sa_handler = (void (*)(int))send_sigusr2_to_pgroup;
    sigaction(SIGUSR1, &actusr1, (struct sigaction *)0);

    /* setup to handle receiving a SIGUSR2 signal from a load leveler daemon
       which will cause the local load leveler daemon to sleep for 10 
       minutes */
    sigaction(SIGUSR2, (struct sigaction *)0, &actusr2);
    actusr2.sa_handler = (void (*)(int))go_to_sleep;
    sigaction(SIGUSR2, &actusr2, (struct sigaction *)0);

    /* seed the random number generator so that all load leveler daemons 
       running on different nodes won't choose the same sequence of random
       numbers */
    srand(mypid);

    /* set up shared memory and initialize the load vector */
    GET_KEY_FILE(key_file, key_dir);
    filep = fopen(key_file, "w+");
    if (filep == NULL) {
	error_nonfatal("unable to open key_file '%s'\n", key_file);
	error_fatal("unable to open key file");
    }
    retval = fclose(filep);
    if (retval != 0) {
	error_nonfatal("unable to close key_file '%s'\n", key_file);
    	error_fatal("unable to close key file"); 
    }
    GET_KEY(key, key_file);
    if (key == (key_t)-1)
    	error_fatal("unable to get key for shared memory segment");
    shmid = shmget(key, sizeof(struct load_info), IPC_CREAT | READ_BY_OTHERS);
    if (shmid == -1) 
    	error_fatal("shmget failed");
    load_info_ptr = (struct load_info*)shmat(shmid, 0, 0);
    if (load_info_ptr == (struct load_info*)-1)
    	error_fatal("shared memory attach operation failed");

    /* insert the size of the load vector(number of elements) into the
       load info structure */
    load_info_ptr->num_elements = num_lvec_elements;

    /* insert local node number into first element of load vector, and
       initialize the rest of the load vector */
    load_info_ptr->load_vector[0].node = local_node_num;
    load_info_ptr->load_vector[0].lm = -1;
    for (i = 1; i < MAX_NUM_LVEC_ELEMENTS; i++) {
        load_info_ptr->load_vector[i].node = -1;
        load_info_ptr->load_vector[i].lm = -1;
    }

    /* setup the load leveler daemon so that it will be able to communicate
       with remote load leveler daemons */
    init_com();

    /* check if we should migrate processes from ROOT_FS_NODE */
    if ( local_node_num == root_fs_node && ! root_fs_node_source ) {
	enable_migrate = 0;
    }

    if ( ! enable_migrate ) {
	/* there is no need to wake up if we don't do migration */
	re_dispatch_timeout = NO_TIMEOUT;
	remain_re_dispatch_time = NO_TIMEOUT;
    }

#ifdef SLL
    /*
     * Check if we should do SLL from ROOT_FS_NODE.
     */
    if ( local_node_num == root_fs_node && ! root_fs_node_source ) {
	static_load_leveling = 0;
    }

    if ( static_load_leveling ) {
	/*
	 * Enable remote process creation in the OS.
	 */
	if ( enable_remote_process_creation() == -1 ) {
	    /*
	     * We did not succeed - it is assumed that remote process creation
	     * is disabled by a boot magic.
	     */
	     error_nonfatal("unable to support static load leveling\n");
	     static_load_leveling = 0;
	}
    }

    if ( ! static_load_leveling ) {
	/*
	 * There is no need to wake up if we don't do static load leveling.
	 */
	fast_node_timeout = NO_TIMEOUT;
	remain_fast_node_time = NO_TIMEOUT;
    }
#endif /* SLL */

	/*
	 * Increase our priority for better catching up with messages.
	 */
	if ( setpriority(PRIO_PROCESS, 0, LLDPRIO)  == -1 ) {
		error_nonfatal("setpriority failed\n");
	}

	/*
	 * Start message receiver thread
	 */
	start_rx_info_thread();

    /* loop until an exit condition is reached */
    for (; ;) {
#ifndef SLL
    	wait_for_timeout(&remain_send_time, &remain_re_dispatch_time, 
				&elapsed_time);
#else /* SLL */
	wait_for_timeout(&remain_send_time, &remain_re_dispatch_time, 
				&remain_fast_node_time, &elapsed_time);
	remain_fast_node_time -= elapsed_time;
#endif /*SLL */
    	remain_send_time -= elapsed_time;
    	remain_re_dispatch_time -= elapsed_time;
#ifndef SLL
    	if ((remain_send_time <= 0) || (remain_re_dispatch_time <= 0))
#else /* SLL */
	if ( (remain_send_time <= 0) || (remain_re_dispatch_time <= 0)
		|| (remain_fast_node_time <= 0) )
#endif /* SLL */

    		/* compute the local node's load measure */
    		compute_local_load_measure();
    	if (remain_send_time <= 0) {
        
    		/* set send_timeout value back to initial setting */
    		remain_send_time = send_timeout;
    
    		/* it's time to send load information to another node */
    		send_info_to_other_node();
    
    		/* Change send time value back to initial setting */
    		remain_send_time = send_timeout;
    	}
    	if (remain_re_dispatch_time <= 0) {
    		if (enable_migrate)

    			/* migrate local processes to underloaded nodes if 
    			   local node is overloaded */
    			re_dispatch_local_procs();
    
    		/* set remain_re_dispatch time value back to initial setting */ 
    		remain_re_dispatch_time = re_dispatch_timeout;
    	}
#ifdef SLL
	if ( remain_fast_node_time <= 0 ) {
		if ( static_load_leveling ) {
			/*
			 * Determine most lightly loaded node and hand it
			 * to the server.
			 */
			fast_node();
    		}

		/*
		 * Set remain_fast_node_time value back to initial setting
		 */ 
		remain_fast_node_time = fast_node_timeout;
	}
#endif /* SLL */
    }
}

/******************************************************************
**  FUNCTION: init_load_leveler
** 
**  DESCRIPTION: This function will read in the configuration information
**               and allocate memory for the significant data structures.
**
**  INPUTS:  none
**
**  OUTPUTS: none
**
******************************************************************/
void 
init_load_leveler()
{
    unsigned i, j;

    /* cache local node number */
    local_node_num = node_self();

    /* get the number of nodes as well as all the node numbers */
    get_node_array();

    /* get the number of ROOT_FS_NODE */
    root_fs_node = get_root_fs_node();

    /* initialize duplicate array */
    duplicate = malloc((size_t) num_slots);
    if ( duplicate == NULL ) 
        /* OOPS - no memory */
        error_fatal("malloc failed");

    /* initialize the node_in_use array */
    node_in_use = malloc((size_t) num_slots);
    if ( node_in_use == NULL ) 
        /* OOPS - no memory */
        error_fatal("malloc failed");
    bzero(node_in_use, num_slots);

    /* mark all available nodes as used */
    for ( i = 0; i < num_nodes; i++ ) {
        *(node_in_use + *(node_num_ptr + i)) = 1;
    }

    /* read in the parameters file data */
    read_in_parameters();

    /* read in the commands list from the migrate_commands file */
    read_in_commands_list();

    /* count how many nodes are in use */
    num_nodes_in_use = 0;
    for (i =0 ; i < num_slots; i++) {
        if (*(node_in_use + i) == 1)
            num_nodes_in_use++;
    }

   /* initialize used_nodes array */
    used_nodes = (int *) calloc((size_t) num_nodes_in_use, sizeof(int));
    if (used_nodes == NULL) 
        /* OOPS - no memory */
        error_fatal("calloc failed");

    for (i = 0, j = 0; i < num_slots; i++)  {
        if (*(node_in_use + i) == 1) 
            *(used_nodes + j++) = i;
    }

    /*
     * Initialize page-fault statistics array.
     */
    for ( i = 0; i < MAX_PG_STATS; i++ ) {
	pg_stat[i].page_outs = 0;
	pg_stat[i].page_ins = 0;
	pg_stat[i].time_stamp = 0.0;
    }
}

/******************************************************************
**  FUNCTION: read_in_parameters
** 
**  DESCRIPTION: This function will read in the configurable/tunable
**               parameters from the parameters file.
**
**  INPUTS:  none
**
**  OUTPUTS: none
**
******************************************************************/
void 
read_in_parameters()
{
    FILE *fp;
    long weight_sum;
    char sbuf[BUFSIZ];
    char *str;
    char *str_ptr;
    char *parm_name_ptr;
    char *weight_str_ptr;
    int i, j, cnt, found, value, start;
    char *value_ptr;
    double value_float;
    
    /* Initialize variables  */
    weight_sum = 0;
    num_lvec_elements = num_nodes/3;
    if (num_lvec_elements < 2)
    	num_lvec_elements = 2;
    if (num_lvec_elements > MAX_NUM_LVEC_ELEMENTS)
    	num_lvec_elements = MAX_NUM_LVEC_ELEMENTS;

    /* get the data from the parameters file */
    fp = fopen(parm_file, "r");
    if (fp == NULL) {
    	error_nonfatal("The parameters file '%s' does not exist.\n", parm_file);
    	error_fatal("non-existent parameters file");
    }
    start = 0;
    cnt = 1;
    str_ptr = str = sbuf;
    while (fgets(str_ptr, BUFSIZ, fp) != NULL) {
	i = strlen(str_ptr);
	/* check for line larger than buffer size */
	if (str_ptr[i-1] != '\n' && str_ptr[i-1] != EOF && i >= BUFSIZ - 1) {
	    if (str == sbuf) {
		str = malloc(BUFSIZ * ++cnt);
		if (str == NULL) {
		    error_nonfatal("Can't read parameter, malloc() failed\n");
		    /* reset i, cnt and eat the rest of the line */
		    i = 0;
		    cnt = 1;
		    str_ptr = str = sbuf;
		    while ((str[i] = fgetc(fp)) != EOF && str[i] != '\n');
		    if (str[i] == EOF)
		   	break;
		}
		else {
		    strcpy(str, sbuf);
		    str_ptr = str + i;
		}
	    }
	    else if (start && i < BUFSIZ * cnt - 1)
		str_ptr += i;
	    else if ((str_ptr = realloc(str, BUFSIZ * ++cnt)) == NULL) {
		error_nonfatal("Can't read parameter, realloc() failed\n");
		/* reset i, cnt and eat the rest of the line */
		i = 0;
		cnt = 1;
		str_ptr = str = sbuf;
		while ((str[i] = fgetc(fp)) != EOF && str[i] != '\n');
		if (str[i] == EOF)
		    break;
	    }
	    else {
		i = strlen(str_ptr);
		str = str_ptr;
		str_ptr += i;
	    }
	    continue;
	}

	if(str[0] != '#' && str[0] != '\n') {
	    parm_name_ptr = get_data_from_str(str, 0);
    	    i = strlen(parm_name_ptr) + parm_name_ptr - str;
    	    found = FALSE;
    	    for (j = 0; j < 3 && !found; j++) {
		weight_str_ptr = weight_str[j];
		if (!strcasecmp(parm_name_ptr, weight_str_ptr)) {
		    found = TRUE;
		    value_ptr = get_data_from_str(str, i+1);
		    if (isdigit(*value_ptr)) {
		        value = (int)strtoul(value_ptr, (char **)0, 10);
			parm_data[j] = value;
		    }
		    else
			error_nonfatal("errno = %d: no expected numeric value "
				"was found for parameters %s\n",
				errno, parm_name_ptr);
		}
	    }
	    if (found) {
		if (cnt > 1)
		   start = 1;
		else
		   start = 0;
		str_ptr = str;
		str[0] = '\0';
		continue;
	    }

            /* check whether the string read in corresponds to one
    	       of the remaining valid parameter names */
    	    if (!strcasecmp(parm_name_ptr, "send_timeout")) {
     	        value_ptr = get_data_from_str(str, i+1);
    		if (isdigit(*value_ptr)) {
		    value_float = atof(value_ptr);
    		    send_timeout = value_float * 1000000.0;
    		}
    		else
    		    error_nonfatal("invalid send_timeout value\n");
    	    }
    	    else if (!strcasecmp(parm_name_ptr, "re_dispatch_timeout")) {
    		value_ptr = get_data_from_str(str, i+1);
    		if (isdigit(*value_ptr)) {
		    value_float = atof(value_ptr);
    		    re_dispatch_timeout = value_float * 1000000.0;
    		}
    		else
    		    error_nonfatal("invalid re_dispatch_timeout value\n");
    	    }
    	    else if (!strcasecmp(parm_name_ptr, "number_vector_elements")){
    		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr)) {
		    value = (int)strtoul(value_ptr, (char **)0, 10);
		    num_lvec_elements = value;
		    if (num_lvec_elements > MAX_NUM_LVEC_ELEMENTS) {
			num_lvec_elements = MAX_NUM_LVEC_ELEMENTS;
			error_nonfatal("the number of load vector elements "
					"specified( =%d) is too big\n",
					num_lvec_elements);
		    }
		}
		else
		    error_nonfatal("errno = %d: no expected numeric value was "
				"found for parameters %s\n", 
				errno, parm_name_ptr);
	    }
	    else if (!strcasecmp(parm_name_ptr, "inclusion_list")) {
    		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr))
		    value = (int)strtoul(value_ptr, (char **)0, 10);
		else
		    error_nonfatal("errno = %d: no expected numeric value was "
				"found for parameters %s\n",
				errno, parm_name_ptr);
		if ((value == 0) || (value == 1))
		    inclusion_list = value;
		else
		    error_nonfatal("bad value(= %d) for inclusion_list "
				"parameter %s\n", value);
	    }
	    else if (!strcasecmp(parm_name_ptr, "minimum_underload")) {
		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr)) {
		    value_float = atof(value_ptr);
		    min_underload = value_float;
		}
		else
		    error_nonfatal("invalid minimum_underload value\n");
	    }
	    else if (!strcasecmp(parm_name_ptr, "minimum_overload")) {
		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr)) {
		    value_float = atof(value_ptr);
		    min_overload = value_float;
		}
		else
		    error_nonfatal("invalid minimum_overload value\n");
	    }
	    else if (!strcasecmp(parm_name_ptr, "minimum_cputime")) {
		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr)) {
		    value_float = atof(value_ptr);
		    min_cputime = value_float;
		}
		else
		    error_nonfatal("invalid minimum_cputime value\n");
	    }
	    else if (!strcasecmp(parm_name_ptr, "per_process_avg_load")) {
		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr)) {
		    value_float = atof(value_ptr);
		    per_process_avg_load = value_float;
		}
		else
		    error_nonfatal("invalid per_process_avg_load value\n");
	    }
	    else if (!strcasecmp(parm_name_ptr, "root_fs_node_target")) {
    		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr))
		    value = (int)strtoul(value_ptr, (char **)0, 10);
		else
		    error_nonfatal("errno = %d: no expected numeric value was "
				"found for parameters %s\n",
				errno, parm_name_ptr);
		if ((value == 0) || (value == 1))
		    root_fs_node_target = value;
		else
		    error_nonfatal("bad value(= %d) for root_fs_node_target "
				"parameter %s\n", value);
	    }
	    else if (!strcasecmp(parm_name_ptr, "root_fs_node_source")) {
    		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr))
		    value = (int)strtoul(value_ptr, (char **)0, 10);
		else
		    error_nonfatal("errno = %d: no expected numeric value was "
				"found for parameters %s\n",
				errno, parm_name_ptr);
		if ((value == 0) || (value == 1))
		    root_fs_node_source = value;
		else
		    error_nonfatal("bad value(= %d) for root_fs_node_source "
				"parameter %s\n", value);
	    }
	    else if (!strcasecmp(parm_name_ptr, "pagein_load")) {
		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr)) {
		    value_float = atof(value_ptr);
		    if ( value_float < 0.0 ) {
			error_nonfatal("invalid pagein_load value\n");
		    }
		    else {
			pagein_load = value_float;
		    }
		}
		else {
		    error_nonfatal("invalid pagein_load value\n");
		}
	    }
	    else if (!strcasecmp(parm_name_ptr, "pageout_load")) {
		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr)) {
		    value_float = atof(value_ptr);
		    if ( value_float < 0.0 ) {
			error_nonfatal("invalid pageout_load value\n");
		    }
		    else {
			pageout_load = value_float;
		    }
		}
		else {
		    error_nonfatal("invalid pageout_load value\n");
		}
	    }
	    else if (!strcasecmp(parm_name_ptr, "pgstat_pref_interval")) {
		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr)) {
		    value_float = atof(value_ptr);
		    if ( value_float < 0.0 ) {
			error_nonfatal("invalid pgstat_pref_interval value\n");
		    }
		    else {
			pgstat_pref_interval = value_float * 1000000.0;
		    }
		}
		else {
		    error_nonfatal("invalid pgstat_pref_interval value\n");
		}
	    }
	    else if (!strcasecmp(parm_name_ptr, "pgstat_max_interval")) {
		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr)) {
		    value_float = atof(value_ptr);
		    if ( value_float < 0.0 ) {
			error_nonfatal("invalid pgstat_max_interval value\n");
		    }
		    else {
			pgstat_max_interval = value_float * 1000000.0;
		    }
		}
		else {
		    error_nonfatal("invalid pgstat_max_interval value\n");
		}
	    }
	    else if (!strcasecmp(parm_name_ptr, "nodes_to_use"))
		get_nodes_from_parameters(fp, str, i+1);
#ifdef SLL
	    else if (!strcasecmp(parm_name_ptr, "fast_node_timeout")) {
		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr)) {
		    value_float = atof(value_ptr);
		    if ( value_float < 0.0 ) {
			error_nonfatal("invalid fast_node_timeout value\n");
		    }
		    else {
			fast_node_timeout = value_float * 1000000.0;
		    }
		}
		else {
		    error_nonfatal("invalid fast_node_timeout value\n");
		}
	    }
	    else if (!strcasecmp(parm_name_ptr, "static_min_load_delta")) {
		value_ptr = get_data_from_str(str, i+1);
		if (isdigit(*value_ptr)) {
		    value_float = atof(value_ptr);
		    if ( value_float < 0.0 ) {
			error_nonfatal("invalid static_min_load_delta "
					"value\n");
		    }
		    else {
			static_min_load_delta = value_float;
		    }
		}
		else {
		    error_nonfatal( "invalid static_min_load_delta value\n");
		}
	    }
#endif /* SLL */
	    else
		/* the parameter name in the parameters file is invalid */
		error_nonfatal("%s: parameter skipped due to invalid "
				"parameter name\n", parm_name_ptr);

	}

	/* reset str pointers */
	if (cnt > 1)
	    start = 1;
	else
	    start = 0;

	str_ptr = str;
	str[0] = '\0';

    } /* while */
    fclose(fp);

    /* convert the values in data[] to floating point values and store the
       value in the weight factor array. */
    weight_sum = parm_data[0] + parm_data[1] + parm_data[2];
    for (i = 0; i < 3; i++) {
	weight_factor[i] = parm_data[i];
	weight_factor[i] /= weight_sum;
    }
}

/******************************************************************
**  FUNCTION: read_in_commands_list
** 
**  DESCRIPTION: This function will read in the commands list from 
**               migrate commands file.
**
**  INPUTS:  none
**
**  OUTPUTS: none
**
******************************************************************/
void 
read_in_commands_list() 
{
    FILE *fp;
    struct stat file_stat;
    char str[80];
    char *cmd_begin;
    int i, j;
 
    i = 0;
    if ((fp = fopen(commands_file, "r")) == NULL) 
    	error_fatal("migrate_commands file does not exist"); 
    while ((fgets(str, 80, fp) != NULL) && (i < (MAX_CMDS - 1))) {
	j = 0;
    	if ((str[0] != '\n') && (str[0] != '#')) {
		while (isspace(str[j]))
			j++;
		cmd_begin = &str[j];
		while ((str[j] != '\0') && (str[j] != '\n') &&
    		 (j < (CMD_LEN - 1))) 
			j++;
		str[j] = '\0';
    		strncpy(cmds[i], cmd_begin, CMD_LEN);
		cmds[i] [CMD_LEN - 1] = '\0';
		i++;
	}
    }
    cmds[i][0] = '\0';
    fclose(fp);
}

/******************************************************************
**  FUNCTION: get_data_from_str
** 
**  DESCRIPTION: Given a string and where to start in the string, this
**               function will return the first word in the string. 
**               
**
**  INPUTS:  str - string from which to extract a word
**           from - position in the string(str) from which the next word
**                  will be extracted
**
**  OUTPUTS: string containing the contents of the first word 
**           beginning at the specified position in the original
**           input string
**
******************************************************************/
char *
get_data_from_str(
    char 	*str,
    int 	from)
{
    char *ret;

    str += from;
    while ((isspace(*str)) || (*str == '='))
    	str++;
    ret = str;
    while ((!isspace(*str)) && (*str != '\0') && (*str != '=') &&
	    (*str != '#'))
    	str++;
    *str = '\0';
    return ret;
}

/******************************************************************
**  FUNCTION: send_sigkill_to_pgroup
** 
**  DESCRIPTION: This function will send a SIGKILL signal to the load 
**               leveler process group.  
**
**  INPUTS:  none
**
**  OUTPUTS: none
**
******************************************************************/
send_sigkill_to_pgroup()
{
    int ret;
    pid_t pgrp;

    /* get the process group id */
    pgrp = getpgrp();

    /* send SIGKILL signal to the process group */
    if (ret = kill(-pgrp, SIGKILL) != 0)

    	/* sending the signal to the process group failed */
    	error_fatal(
    	"couldn't send signal SIGKILL to load_leveld process group");
}

/******************************************************************
**  FUNCTION: send_sigusr2_to_pgroup
** 
**  DESCRIPTION: This function will send a SIGUSR2 signal to the load 
**               leveler process group.  
**
**  INPUTS:  none
**
**  OUTPUTS: none
**
******************************************************************/
send_sigusr2_to_pgroup()
{
    int ret;
    pid_t pgrp;

    /* get the process group id */
    pgrp = getpgrp();

    /* send SIGUSR2 signal to the process group */
    if (ret = kill(-pgrp, SIGUSR2) != 0)

    	/* sending the signal to the process group failed */
    	error_fatal(
    	"couldn't send signal SIGUSR2 to load_leveld process group");
}

/******************************************************************
**  FUNCTION: go_to_sleep
** 
**  DESCRIPTION: This function will cause this load leveler daemon process
**               to sleep for 10 minutes.
**
**  INPUTS:  none
**
**  OUTPUTS: none
**
******************************************************************/
go_to_sleep()
{

    sleep(10*60);
}

/******************************************************************
**  FUNCTION: compute_local_load_measure
** 
**  DESCRIPTION: This function will compute the load measure for
**               the local node.
**
**  INPUTS:  none
**
**  OUTPUTS: none
**
******************************************************************/
void 
compute_local_load_measure()
{
    int i;
    double load_average[3];
    int		found_index;
    int		j;
    int		pips;
    int		pops;
    double	tdiff;
    double	pg_load;
    double	total_load;

    get_load_average(load_average);

    /* calculate the load measure for the local node */
    total_load = 0.0;
    for (i = 0; i < 3; i++) {
    	total_load += load_average[i] * weight_factor[i];
    } 

    /*
     * Look for an entry in the page-fault statistics array with an age
     * between pgstat_pref_interval and pgstat_max_interval (if available).
     */
    found_index = -1;
    j = pg_stat_index;
    for ( i = 0; i < MAX_PG_STATS - 1; i++ ) {
	if ( --j < 0 ) {
	    /*
	     * Wrap-around.
	     */
	    j = MAX_PG_STATS - 1;
	}

	if ( pg_stat[j].time_stamp <
		pg_stat[pg_stat_index].time_stamp - pgstat_max_interval ) {
	    /*
	     * Too old.
	     */
	    break;
	}

	/*
	 * Remember the current index.
	 */
	found_index = j;

	if ( pg_stat[j].time_stamp <=
		pg_stat[pg_stat_index].time_stamp - pgstat_pref_interval ) {
	    /*
	     * Old enough;
	     */
	    break;
	}
    }

    /*
     * Calculate the paging load of the node.
     */
    pg_load = 0.0;

    if ( found_index != -1 ) {
	tdiff = (pg_stat[pg_stat_index].time_stamp -
			pg_stat[found_index].time_stamp) / 1000000.0;

	if ( tdiff > 0.0 ) {
	    pips = (pg_stat[pg_stat_index].page_ins -
			pg_stat[found_index].page_ins) / tdiff;

	    pops = (pg_stat[pg_stat_index].page_outs -
			pg_stat[found_index].page_outs) / tdiff;

	    pg_load = (double) pips * pagein_load +
			(double) pops * pageout_load;
	}
   }

    /*
     * Add the paging load to the CPU load.
     */
    total_load += pg_load;

    /*
     * Update the load vector.
     * No locking necessary, as the 0th element is only accessed by the main
     * thread.
     */
    load_info_ptr->load_vector[0].lm = total_load;
}
    
/******************************************************************
**  FUNCTION: re_dispatch_local_procs
** 
**  DESCRIPTION: This function will determine which of the nodes(whose
**               load measures are known by the local node) are overloaded
**               or underloaded and then conditionally migrate certain local
**               processes to underloaded nodes provided that the local
**               node is overloaded.
**
**  INPUTS:  none
**
**  OUTPUTS: none
**
******************************************************************/
void 
re_dispatch_local_procs()
{

    struct underload_info
    {
    	int node;
    	double lm;
    	double underload;
    };

    struct underload_info load_vec_copy[MAX_NUM_LVEC_ELEMENTS];
    int i, j, ret, num_underloaded_nodes, num_nodes_with_info, new_pid; 
    double total_lm, dlm, total_underload, random_num;
    double local_node_overload;
    double underload_ratio;

    /*
     * Lock the load vector.
     */
     lock_load_vec();

    /* initialize underload structure */
    for (i=0; i < num_lvec_elements; i++) {
    	load_vec_copy[i].node = load_info_ptr->load_vector[i].node;
    	load_vec_copy[i].lm = load_info_ptr->load_vector[i].lm;
    	load_vec_copy[i].underload = 0.0;
    }

    /*
     * Unlock the load vector.
     */
     unlock_load_vec();

    /* zero out duplicates field */
    bzero(duplicate, num_slots);

    /* eliminate duplicates */
    for ( i = 0; i < num_lvec_elements; i++ ) {
        if ( load_vec_copy[i].node != -1 ) {
            if ( *(duplicate + load_vec_copy[i].node) == 1 ) {

                /* this is a duplicate - mark as invalid */
                load_vec_copy[i].node = -1;
                load_vec_copy[i].lm = -1;
            }
            else 
                *(duplicate + load_vec_copy[i].node) = 1;
        }
    }

    /* calculate the total load measures for all nodes specified in the
       load vector */
    total_lm = 0.0;
    num_nodes_with_info = 0;
    for (i = 0; i < num_lvec_elements; i++) {
    	if (load_vec_copy[i].node != -1) {
	    /*
	     * Check if we should consider ROOT_FS_NODE.
	     */
	    if ( load_vec_copy[i].node == root_fs_node &&
			! root_fs_node_target ) {
		/* mark as invalid */
		load_vec_copy[i].node = -1;
		continue;
	    }

    	    total_lm += load_vec_copy[i].lm;
    	    num_nodes_with_info++; 
    	}
    }

    /* compute the desired load measure */
    dlm = total_lm/(double)num_nodes_with_info;
 
    /*
     * No locking necessary, as the 0th element is only accessed by the main
     * thread.
     */

    /* if the local node is overloaded, determine the underloaded nodes and
       re-dispatch the necessary local processes */
    local_node_overload = (load_info_ptr->load_vector[0].lm - dlm);
    if (local_node_overload < min_overload) 
    	return;

    /* determine the underloaded amounts for each node in the load 
       vector */
    total_underload = 0.0;
    num_underloaded_nodes = 0;
    for (i = 0; i < num_lvec_elements; i++) {
    	if (load_vec_copy[i].node != -1) {
    		load_vec_copy[i].underload = dlm - load_vec_copy[i].lm; 
    		if (load_vec_copy[i].underload < min_underload)
    			load_vec_copy[i].underload = 0;
    		if (load_vec_copy[i].underload > 0) {
    			total_underload += load_vec_copy[i].underload;
    			num_underloaded_nodes++;
    		}
    	}
    }
    initialize_migration_processes();

    /* migrate a local process as long as an underloaded site exists,
       the local node is overloaded, and a local process matches/doesn't
       match an entry in the inclusion/exclusion list */
    while ((local_node_overload >= min_overload) && 
     (num_underloaded_nodes > 0)) {
    	new_pid = get_next_migration_process();
    	if (new_pid == -1)
    		break;
        random_num = rand();
        random_num /= RAND_MAX;
        for (i = 0; (i < num_lvec_elements); i++) {

                /* calculate underload ratio of this node which is used in
                   the random placing of the process to be migrated */
                underload_ratio = load_vec_copy[i].underload / total_underload;

                if ((load_vec_copy[i].node == -1) ||
                 (underload_ratio <= 0))
                        continue;
                if (underload_ratio < random_num) {
                        random_num -= underload_ratio;
                        continue;
                }

    		/* migrate the process */
    		ret = kill3(new_pid, SIGMIGRATE, load_vec_copy[i].node);
#ifdef MIGRATELOG
                if ( domigratelog == 1 ) {
                    error_nonfatal("%s (%d) migrates: %d -> %d\n",
                                   migrated_command, new_pid,
                                   local_node_num, load_vec_copy[i].node);
                }
#endif /* MIGRATELOG */
    		if (ret == -1) {
    			error_nonfatal(
    			"process %d failed to migrate to node %d\n",
    			new_pid, load_vec_copy[i].node);
    			break;
    		}
    		local_node_overload -= per_process_avg_load;
    		load_vec_copy[i].underload -= per_process_avg_load;
    		if (load_vec_copy[i].underload < min_underload) {
    			load_vec_copy[i].underload = 0.0;
			num_underloaded_nodes--;
		}
    		total_underload -= per_process_avg_load;
    		break;
    	}
    } /* while */
}

/******************************************************************
**  FUNCTION: receive_load_vector
** 
**  DESCRIPTION: This function is called by os dependent code.  It 
**               will process the load information which has been 
**               read received by the local node.
**
**  INPUTS: nodes_array - array containing node numbers for the received load 
**                        information
**
**          loads_array - array containing the load measures for the nodes 
**                        specified in the nodes array
**
**  OUTPUTS: none
**
******************************************************************/
void 
receive_load_vector(
    int		nodes_array[],
    double	loads_array[])
{

    int i;

    /*
     * Lock the load vector.
     */
     lock_load_vec();
    
    /* shuffle the existing elements of the load vector before shuffling
       in the received load info */

    for (i = (num_lvec_elements/2 - 1); i >= 1; i--) {
    	load_info_ptr->load_vector[2*i].node = 
    	 load_info_ptr->load_vector[i].node;
    	load_info_ptr->load_vector[2*i].lm = 
    	 load_info_ptr->load_vector[i].lm;
    }

    /* now, shuffle in the received load info */
    for (i = 0; i <= ((num_lvec_elements + 1)/2 - 1); i++) {
    	load_info_ptr->load_vector[2*i + 1].node = nodes_array[i]; 
    	load_info_ptr->load_vector[2*i + 1].lm = loads_array[i];
    }

    /*
     * Unlock the load vector.
     */
     unlock_load_vec();
}

/******************************************************************
**  FUNCTION: send_info_to_other_node
** 
**  DESCRIPTION: This function will package up half of the local
**               load vector and transmit the info to a randomly
**               selected node.
**
**  INPUTS:  none
**
**  OUTPUTS: none
**
******************************************************************/
void 
send_info_to_other_node()
{
    int random_node, random_index, error;

    do {

    	/* choose a random node number other than the local node number */
        random_index = rand();
        random_index %= num_nodes_in_use;
	random_node = *(used_nodes + random_index);
        if (random_node == local_node_num) {

            /*
             * Got local node - simply take next one.
             */
            random_index++;
            random_index %= num_nodes_in_use;
            random_node = *(used_nodes + random_index);
        }

        error = send_load_info(random_node);
    } while (error);
} 

/*****************************************************************************
**  FUNCTION: on_commands_list
** 
**  DESCRIPTION: This function determines whether a command name passed
**               as an input parameter exists in the migrate_commands file.
**
**  INPUTS:  cmd - command name
**
**  OUTPUTS: "TRUE(= 1)" if command was found in the migrate_commands file
**
**           "FALSE(=0)" if command was not found in the migrate_commands file
**
*****************************************************************************/
int
on_commands_list(
    char *cmd)
{
    int found, i;

    found = FALSE;
    i = 0;
    while (cmds[i][0] != '\0') {
    	if (strcmp(cmds[i++], cmd) == 0) {
    		found = TRUE;
    		break;
    	}
    }
    return (found);
}

/******************************************************************
**  FUNCTION: get_nodes_from_parameters
**
**  DESCRIPTION: This function will get the node numbers specified in
**               the parameters file.
**
**  INPUTS:  fptr - pointer to the parameters file containing the useable
**                  node numbers.
**
**           str - pointer to the line of the parameters file containing the
**                 node numbers.
**           from - pointer to the position in the line where to begin the
**                  search for the node numbers
**           
**
**  OUTPUTS: none
**
******************************************************************/
void
get_nodes_from_parameters(
    FILE *fptr,
    char *str,
    int from)
{
    char *str_begin;
    char saved_char;
    int i, not_finished, value1 = -1, value2 = -1, no_nodes_found, range_found;
    int found_separator;

    /* re-initialize node_in_use array to accomodate the possibility that
       only a subset of all service nodes is specified in the parameters
       file */
    bzero(node_in_use, num_slots);

    range_found = FALSE;
    no_nodes_found = TRUE;
    not_finished = TRUE;
    found_separator = TRUE;
    str += from;
    while ((isspace(*str)) || (*str == '=') ) {
	/* ignore blanks, equal signs, tabs */
    	str++;
    }
    while (not_finished) {
    	str_begin = str;
    	while (isdigit(*str))
    		str++;
    	saved_char = *str;
    	*str = '\0';
    	if (isdigit(*str_begin) && found_separator) {
    		if (range_found) {
    			value2 = (int)strtoul(str_begin, (char **)0, 10);
    			range_found = FALSE;
    		}
    		else {
    			value1 = (int)strtoul(str_begin, (char **)0, 10);
    			value2 = value1;
    		}
    	}
    	switch (saved_char) {
    		case '-':
    			range_found = TRUE;
    			str++;
			found_separator = TRUE;
    			break;

    		case '\\':
    			if (fgets(str, 80, fptr) == NULL) {
    				modify_node_info(&no_nodes_found, value1,
    							   value2);
    				not_finished = FALSE;
			}
    			break;

    		case '#':
    		case '\n':
    		case '\0':
    			modify_node_info(&no_nodes_found, value1,
    						   value2);
    			not_finished = FALSE;
    			break;

    		case ',':
    			modify_node_info(&no_nodes_found, value1,
    						   value2);
			value1 = -1;
    			str++;
			found_separator = TRUE;
    			break;

		case ' ':
		case '\t':
    			str++;
			found_separator = TRUE;
    			break;
			
    		default:
			value1 = -1;
    			str++;
			found_separator = FALSE;
    			break;
    	} /* switch */
    } /* while */
    if (no_nodes_found) {
    	
    	/* Either no nodes were found in the parameters file or else the
    	   specified nodes did not match any nodes in the node_info
    	   structure. Hence by default, all service nodes can be used. */
    	for (i = 0; i < num_nodes; i++) {
            *(node_in_use + *(node_num_ptr + i)) = 1;
    	}
    }
}

/******************************************************************
**  FUNCTION: modify_node_info
**
**  DESCRIPTION: This function will set the element of the
**               node_in_use array if the passed in node number(s)
**               corresponds to the existing node number(s).
**
**  INPUTS:  no_nodes_found_ptr - pointer to a variable indicating whether
**                                a match exists between the node numbers
**                                specified in the parameters file and the
**                                node numbers contained in the node_port_map
**                                structure.
**
**           node_num1 - node number from the parameters file
**
**           node_num2 - possible second node number from the parameters
**                       file.  If this node number is not equal to 
**                       node_num2, then the node number pair consisting of
**                       node_num1 and node_num2 form a range of node
**                       numbers.
**
**  OUTPUTS: none
**
******************************************************************/
void
modify_node_info(
    int *no_nodes_found_ptr,
    int node_num1,
    int node_num2)

{
    int i, j;

    /* return to caller if no node numbers were specified in the parameters 
       file */
    if (node_num1 == -1)
    	return;

    /* if the passed in node number(s) match node numbers in
       the  node_num_ptr array, mark the node(s) as useable */

    for ( j = node_num1; j <= node_num2; j++ ) {
        for ( i = 0; i < num_nodes; i++ ) {
            if ( *(node_num_ptr + i) == j ) {
                *(node_in_use + *(node_num_ptr + i)) = 1;
                *no_nodes_found_ptr = FALSE;
                break;
            }
        }
    }
}

/******************************************************************
**  FUNCTION: daemonize
**
**  DESCRIPTION: This function makes the caller a real daemon.
**
**
**  INPUTS:  none
**
**  OUTPUTS: none
**
******************************************************************/
void
daemonize()
{
    struct sigaction    signal_action;
    pid_t               childpid;
    int                 fd;


    /* ignore terminal stop signals */
    signal_action.sa_handler = SIG_IGN;
    sigaction(SIGTTOU, &signal_action, (struct sigaction *)0);
    sigaction(SIGTTIN, &signal_action, (struct sigaction *)0);
    sigaction(SIGTSTP, &signal_action, (struct sigaction *)0);

    /* fork to go into the background */
    childpid = fork();
    if ( childpid == -1 ) {
        /* error functions not initialized yet - use perror */
        perror("load_leveld: fork failed");
        exit(1);
    }

    if ( childpid > 0 ) {
        /* parent - exit */
        exit(0);
    }

    /* create new process group */
    setpgid(0, 0);

    /* disassociate from controlling tty */
    if ((fd = open("/dev/tty",O_RDWR)) >= 0)
    {
        ioctl(fd,TIOCNOTTY, (char*) NULL);
        close(fd);
    }

    /* close all open file descriptors */
    for ( fd = 0; fd < NOFILE; fd++ ) {
        close(fd);
    }

    errno=0;

    umask(022);
}

