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
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: osf1_dep.c,v $
 * Revision 1.23  1995/02/01  23:51:48  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.22  1994/11/19  03:07:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.21  1994/11/18  20:53:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.20  1994/08/31  22:48:20  mtm
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
 * Revision 1.17.4.1  1994/08/10  17:50:59  flb
 * Reviewer: cfj
 * Risk: low
 * Benefit or PTS # 10173
 * Testing: System Boot with large MAGIC.MASTER
 * Module(s): uxkern/boot_config.c,uxkern/server_init.c,user/etc/load_level/osf1_dep.c
 *
 * Revision 1.17  1994/05/27  13:06:46  stefan
 * Now the paging behavior of a node is included in the calculation of the load
 * value.
 * Also, cthreads are wired to mach threads.
 *
 *  Reviewer: yazz
 *  Risk: low to medium
 *  Benefit or PTS #:
 *  Testing: developer testing
 *  Module(s): osf1_dep.c
 *
 * Revision 1.16  1994/02/16  16:35:43  stefan
 * Merged version 1.15.2.1 into main trunk.
 *
 * Revision 1.15.2.1  1994/02/16  14:42:59  stefan
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
 * Revision 1.15  1993/11/19  18:58:21  bolsen
 *  Reviewer: Mike Barnett and Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: 5393 - load_leveld workaround for MIG problem
 * 		   5396 - load_leveld should use mach_error_string()
 * 		   7207 - VSTNC load_leveld test1 fails
 *  Testing: LCC Load_leveld tests
 *  Module(s): user/etc/load_level/osf1_dep.c
 *
 * Revision 1.14  1993/10/04  14:12:16  stefan
 * Added a #endif that I forgot last time (didn't hurt as it is in a comment,
 * but having it in there makes the code more obvious).
 *
 * Revision 1.13  1993/09/30  18:09:51  stefan
 * Made the load leveler multi-threaded
 *
 * Revision 1.12  1993/07/23  17:46:41  stefan
 * Replaced use of itimer() by gettimeofday() because of a  bug
 * in itimer().
 * As this also simplifies the code, it should be left this way even if itimer
 * is fixed.
 *
 * Revision 1.11  1993/07/14  18:51:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.10  1993/06/22  16:54:38  stefan
 * Implemented temporary workaround for the alignment bug in mig.
 *
 * Revision 1.1.1.5  1993/07/01  21:17:43  cfj
 * Adding new code from vendor
 *
 * Revision 1.9  1993/05/13  09:19:07  stefan
 * Integrated static load leveling support.
 *
 * Revision 1.8  1993/05/06  19:29:26  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.7  1993/01/22  19:33:05  stefan
 * Merged Locus 01-20-93 code drop.
 *
 * Revision 1.6  1992/12/18  20:05:30  stefan
 * Adjusted a comment to fit the code.
 *
 * Revision 1.5  1992/12/18  16:23:14  stefan
 * Changed in function send_load_info() the way how errors from get_tnc_port()
 * are handled: if error is ENOENT the error is silently ignored because it is
 * assumed that this is only a result of the peer beeing a little bit delayed
 * in it's startup phase. Otherwise the error is considered fatal (as all
 * errors from get_tnc_port() were in the original code from LOCUS).
 * This modification eliminates lots of warning messages during the startup on
 * large machines.
 * As a side effect the delay in function init_com() could be decreased from
 * 5 seconds to 1 second.
 *
 * Revision 1.4  1992/12/14  14:41:25  stefan
 * Made lots of modifications for performance tuning and bug fixing:
 *   - node_port_map now is only an array which is indexed by the node number.
 *     Hence we have complexity of O(1) instead of O(n).
 *   - node_info_ptr is now replaced by node_in_use, which is only an array
 *     which is indexed by the node number. Hence we have complexity of O(1)
 *     instead of O(n).
 *   - error_fatal() and error_nonfatal() now include the program name in the
 *     error message.
 *   - Changed the way how the error message is prepended by the program name.
 *     Introduced error_init for this reason.
 *   - node_self() is now cached in local_node_num.
 *   - Now only call error_nonfatal if get_tnc_port fails as this call might
 *     only fail because of an insufficient startup dalay.
 *   - Fixed a bug in rx_info_from_other_node where the local variable elapsed
 *     time instead of the output parameter elapsed_time_ptr was updated.
 *     Also the variable current_time is now initialized.
 *
 * Revision 1.3  1992/11/30  23:01:53  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.5  1992/11/10  19:22:57  cfj
 * Fix up some merge conflicts.
 *
 * Revision 1.1.2.4  1992/11/10  17:34:19  cfj
 * Added missing #endif.
 *
 * Revision 1.1.2.3  1992/11/10  17:27:15  cfj
 * Resolve a missed conflict.
 *
 * Revision 1.1.2.2  1992/11/06  20:36:22  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:49:54  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.6  1992/10/23  16:47:16  cfj
 * Get rid of an annoying "non-prototype declaration in scope" compile
 * warning which sometimes blows up the compiler.
 *
 * Revision 3.5  1992/10/12  17:32:42  stefan
 * added support for migration logs.
 *
 * implemented work around for syslog() bug.
 *
 * fixed bug where same process was migrated several times, becaus index in
 * process table was not incremented.
 *
 * Revision 1.1.1.3  1993/05/03  17:58:03  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.11  93/08/03  10:55:07  bolsen
 * Fixed the following bugs:
 * [Bug 306/5393]  load_leveld workaround for MIG problem
 * [Bug 307/5396]  use mach_error_string() call in Mach message errors
 * [Bug 310] VSTNC load leveler test fails
 * [Bug 325/5905]  bug in itimer kills load_leveld, use gettimeofday
 * 
 * Revision 3.10  93/04/23  16:20:30  bolsen
 * [SPE 0007] reference common header file for shared memory structures,
 * 	variables and NEW macros (ie. common to load_leveld and onnode
 * 	(fast & fastnode)).
 * 
 * Revision 3.9  93/03/23  11:07:58  bolsen
 * Bug #184 fixed error_nonfatal(), use stdarg (varargs) routines.
 * 
 * Revision 3.8  92/12/30  16:32:33  mbarnett
 * The array node_in_use has replaced node_info_ptr and node_port_map has been
 * changed from a structure to an array.  These changes reduce complexity from
 * O(n) to O(1).  With the use of these arrays, scanning for duplicate nodes
 * in the load vector is eliminated.  These changes resolve SPE #0008.  Also,
 * the errno ENOENT returned from calls to get_tnc_port in the send_load_info 
 * function are now silently ignored.  This resolves SPE #0016.  Finally, the
 * output elpased_time parameter in the function rx_info_from_other_node is
 * now properly updated.  Previously, only the local elapsed_time variable was
 * updated when a message was received.  This fixes bug #0137.
 * 
 * Revision 3.7  92/12/28  15:32:04  mbarnett
 * Modified the osf1_dep.c code to cache node_self locally(SPE #0014).  Also,
 * the osf1_dep.c code was changed so that error messages will contain the
 * program name and the messages will appear in the "/usr/adm/syslog/daemon.log"
 * file(SPE #0013).
 * 
 * Revision 3.6  92/10/28  16:06:19  roman
 * Have the load-leveller use the table_node() system call rather than
 * 	the table() system call. The table() system call now
 * 	returns cluster-wide information.
 * 
 * Revision 3.5  92/10/23  15:39:49  mbarnett
 * I made changes to accomodate the increase precision in the "send_timeout" and
 * "re_dispatch_timeout" variables(#0086).  Also, changes were made to cache the
 * pid of the local load leveler daemon(#0084).  Finally, changes were made to
 * fix the race condition bug where a migratable process was sometimes migrated
 * more than once(#0078).
 * 
 * Revision 3.4  92/10/08  16:01:44  mbarnett
 * The variable time_in_micros is now calculated correctly such that the
 * calculated runtime does not become negative as a result of an integer
 * overflow.  This change fixes bug #61.
 * 
 * Revision 3.3  92/07/10  16:21:18  mbarnett
 * Effectively removed explicit calls to printf with calls to syslog in order
 * to log errors.
 * 
 * Revision 3.2  92/06/15  13:08:00  mbarnett
 * Removed the "osdep" prefix for all function names and removed the following
 * functions: osdep_free_allocated_mem, osdep_start_timer, osdep_check_timer,
 * osdep_disseminate_local_load_vector, osdep_get_proc_info.  Also, the 
 * following functions were added: get_next_migration_process, 
 * traversed_entire_proc_table, get_elapsed_time.
 * 
 * Revision 3.1  92/05/13  12:26:16  mbarnett
 * Made change to log a message but not exit if the load array size passed in by
 * mig does not correspond to the load vector size read in from the parameters 
 * file by the load leveler.
 * 
 * Revision 3.0  92/05/13  08:22:54  mbarnett
 * initial checkin
 * 
 *
 */
/*****************************************************************************
**  PROGRAM: osf1_dep
**
**  DESCRIPTION: This module contains all routines in the load leveler daemon
**               which are dependent on the OSF1 system.
**
*****************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <stdarg.h>
#include <sys/syslog.h>
#include <sys/table.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <mach.h>
#include <mig_errors.h>
#include <cthreads.h>
#include "load_level_com.h"
#include "load_level_types.h"
        
/* maximum size of the boot environment */
#define	BOOTMAGIC_MAX	(1024 * 16)

#ifdef MIGRATELOG
/* string containing the name of the command found by
   get_next_migration_process() */
extern char migrated_command[];
#endif /* MIGRATELOG */

/* structure of the load message header */
union load_msg
{
    mach_msg_header_t hdr;
    mig_reply_header_t reply_msg;
    char space[8192];
} msg_buf1, msg_buf2, msg_buf_rx1, msg_buf_rx2;

/* pointer to an array which maps node numbers to port numbers */
mach_port_t *node_port_map;

/* the local port for receiving messages */
mach_port_t mynode_port = MACH_PORT_NULL;

/* variable indicating whether a shared memory segment has been attached */
int shared_memory_is_attached = FALSE;

/* variable which contains a starting random index into the proc table */
int proc_index_start;

/* variable which contains current index into the proc table */
int proc_index_current;

/* variable containing the number of local processes */

int number_of_procs;

/* variable containing the value of the timer the last time the timer was
   checked */
double old_time;

/* variable containing the current value of the timer */
double current_time;

/* parameter value sent to system calls "set_tnc_port" and
   "get_tnc_port" which specifies that the port is to be used by the
   load leveler daemon */
#define LOAD_LEVEL_ID 0

/*
 * port used for timeouts.
 */
mach_port_t	timeout_port = MACH_PORT_NULL;

/*
 * spin lock to protect the load array
 */
spin_lock_t	load_vec_lock;

/* external functions */
extern mach_msg_return_t tx_load_message(mach_port_t, int[],
                                         mach_msg_type_number_t, double[],
                                         mach_msg_type_number_t);
extern void load_level_server(mach_msg_header_t *, mach_msg_header_t *);
extern void receive_load_vector(int[], double[]);
extern on_commands_list(char *);

/* local functions */
void error_fatal(char *);
void error_nonfatal(char *,...);
void init_com();
void get_node_array();
void get_load_average(double *);
int send_load_info(int);
void initialize_migration_processes();
int get_next_migration_process();
int traversed_entire_proc_table();
void get_elapsed_time(double *);
void error_init(char *);
void start_rx_info_thread();
void rx_info_thread(any_t);
void lock_load_vec();
void unlock_load_vec();
int get_root_fs_node();
char *getbootenv();
#ifndef SLL
void wait_for_timeout(double *, double *, double *);
#else /* SLL */
void wait_for_timeout(double *, double *, double *, double *);
#endif /* SLL */


/******************************************************************
**  FUNCTION: init_com
**
**  DESCRIPTION: This function will set up the load leveler daemon so that
**               it will be able to send/receive messages from/to remote
**               load leveler daemons.
**
**  INPUTS:  none
**
**  OUTPUTS: none
**
******************************************************************/
void
init_com()
{

    int retval;
    kern_return_t ret;
    struct timeval      time_now;
    mach_port_t		dummy_port;

#ifdef __i860__
    /*
     * Temporary work around a bug in mig that causes even double values
     * to be only aligned on 4-byte boundaries which results in an exception
     * on an i860.
     *
     * Allow the trap handler to emulate non-aligned accesses.
     *
     * This should be removed as soon as mig is fixed.
     */
     asm("trap r0,r18,r0");
#endif /* __i860__ */

    /* get current time */
    if ( gettimeofday(&time_now, NULL) == -1 ) {
	error_fatal("couldn't get current time");
    }
    current_time = (double)(1000000.0 * (double)time_now.tv_sec) +
			(double)(time_now.tv_usec);
    old_time = current_time;

    /* allocate a port for receiving messages containing load information
       from other nodes */
    ret = mach_port_allocate(mach_task_self(),
                             MACH_PORT_RIGHT_RECEIVE,
                             &mynode_port);

    if (ret != KERN_SUCCESS)
        error_fatal("mach_port_allocate failure");

    /* check if there is already another load_leveld running */
    if ( get_tnc_port(LOAD_LEVEL_ID, local_node_num, &dummy_port) != -1 &&
        dummy_port != MACH_PORT_DEAD ) {
        /* don't start two load_leveld on the same node */
        error_nonfatal("cannot run more than one load_leveld on each node");
	exit(1);
    }

    /* register the port with the server */
    set_tnc_port(LOAD_LEVEL_ID, MACH_PORT_NULL);
    if (iparm = -1)
    	retval = set_tnc_port(LOAD_LEVEL_ID, mynode_port);
    else
    	retval = set_tnc_port(iparm, mynode_port);
    if (retval == -1)
        error_fatal("call to set_tnc_port failed");

    /* sleep for one second to allow the other load leveler
       daemons time to register their ports with the servers */
    sleep(1);

    /*
     * Create a dummy port used for timeouts.
     */
    ret = mach_port_allocate(mach_task_self(),
				MACH_PORT_RIGHT_RECEIVE, 
				&timeout_port);

    if ( ret != KERN_SUCCESS ) {
		error_fatal("mach_port_allocate failure");
    }

    /*
     * Initialize load_vec_lock.
     */
     spin_lock_init(&load_vec_lock);
}


/******************************************************************
**  FUNCTION: get_node_array
**
**  DESCRIPTION: This function will get the number of nodes as well
**               as all the node numbers
**
**  INPUTS:  none
**
**  OUTPUTS: node
**
******************************************************************/
void
get_node_array()
{
    int i;
    
    if ((num_nodes = table(TBL_NODEINFO, 0, (char *)0, 32767, 0)) == -1)
        error_fatal("cannot obtain the number of service nodes");
    node_num_ptr = (int *)calloc((size_t)num_nodes, sizeof(int));

    /* get node numbers */
    if (table(TBL_NODEINFO, 0, (char *)node_num_ptr, num_nodes,
              sizeof(long)) == -1)
    error_fatal("cannot obtain the service node numbers");

    /* get number of slots - this only differs from the number of nodes
       if there is are no partitions and there are empty/defect slots */
    if (table(TBL_NODEINFO, num_nodes - 1, (char *) &num_slots, 1,
              sizeof(long)) == -1)
        error_fatal("cannot obtain number of slots in system");

    /* we got maximum node number - increment to get number of slots */
    num_slots++;

    /* initialize the node_port_map structure */
    node_port_map = (mach_port_t *) calloc((size_t) num_slots,
                         sizeof (mach_port_t));
    for (i = 0; i < num_slots; i++) {
       *(node_port_map + i) = MACH_PORT_NULL;
    }
}


/******************************************************************
**  FUNCTION: get_load_average
**
**  DESCRIPTION: This function will get the interval load averages
**               for the local node.
**
**  INPUTS:  load_avg_ptr - pointer to an array which will be 
**                          filled with the interval load averages
**
**  OUTPUTS: none
**
******************************************************************/
void
get_load_average(
    double *load_avg_ptr)
{
    struct tbl_loadavg load;
    union avenrun {
	long l[3];
	double d[3];
    } *load_avg;
    int i;
    struct vm_statistics	vm_stat;
    kern_return_t		ret;


    /* get the 5 second, 30 second, and 1 minute load averages for
       the local node */
    table_node(local_node_num, TBL_LOADAVG, 0, (char *)&load, 1, sizeof(load));
    load_avg = (union avenrun *)&load.tl_avenrun;

    /* calculate the load measure for the local node */
    if (load.tl_lscale != 0) {
        for (i = 0; i < 3; i++) {
                *(load_avg_ptr + i) = 
    		 (((double)load_avg->l[i])/load.tl_lscale);
        }
    }
    else {
        for (i = 0; i < 3; i++) {
                *(load_avg_ptr + i) = (load_avg->d[i]);
        }
    }


    /*
     * Get vm statistics from kernel.
     */
    ret = vm_statistics(mach_task_self(), (vm_statistics_data_t *) &vm_stat);
    if ( ret != KERN_SUCCESS ) {
    	error_nonfatal("could not get vm_statistics: %s (%d)",
			mach_error_string(ret), ret);
    }

    /*
     * Create new entry in page-fault array.
     */
    if ( ++pg_stat_index == MAX_PG_STATS ) {
	/*
	 * Wrap-around.
	 */
	pg_stat_index = 0;
    }
    pg_stat[pg_stat_index].page_ins = vm_stat.pageins;
    pg_stat[pg_stat_index].page_outs = vm_stat.pageouts;
    pg_stat[pg_stat_index].time_stamp = current_time;

}


/******************************************************************
**  FUNCTION: rx_load_message
** 
**  DESCRIPTION: This function is called by MIG generated code.  It 
**               will pass on the received load information to the 
**               os independent code.
**
**  INPUTS: nodes - array containing node numbers for the received load 
**                  information
**          node_array_size - specifies the number of elements in 
**                            the nodes array
**
**          loads - array containing the load measures for the nodes 
**                  specified in the nodes array
**
**          loads_array_size - specifies the number of elements in
**                             the loads array
**
**  OUTPUTS: none
**
******************************************************************/
void 
rx_load_message(
    mach_port_t	rx_port,
    int		nodes[],
    int		nodes_array_size,
    double	loads[],
    int		loads_array_size)
{

    /* Check to make sure that the size of the arrays are what they should 
       be. The sizes may not correspond as a result of a user invoking the
       load_leveld_restart utility to cause load_leveld to re-load the
       configuration parameters on the fly. If the sizes do not correspond,
       log a message but don't grab the received info. */
    if (nodes_array_size != (num_lvec_elements/2)) {
    	error_nonfatal("the received nodes array is not sized properly");
	return;
    }
    if (loads_array_size != (num_lvec_elements/2))  {
    	error_nonfatal("the received loads array is not sized properly");
	return;
    }

    /* pass on the received load information to the os independent code for
       processing */
    receive_load_vector(nodes, loads);
}


/******************************************************************
**  FUNCTION: send_load_info
** 
**  DESCRIPTION: This function will send the local load vector to 
**               randomly selected node.
**
**  INPUTS:  
**
**  INPUTS:  ran_node - randomly selected node number
**
**  OUTPUTS: ret - 0 if load vector has been sent, 1 if load vector has
**                 not been sent
**
******************************************************************/
int
send_load_info(
    int ran_node)
{
    int node_array[MAX_NUM_LVEC_ELEMENTS/2];
    mach_msg_type_number_t node_array_sz;
    double load_array[MAX_NUM_LVEC_ELEMENTS/2];
    mach_msg_type_number_t load_array_sz;
    mach_port_t tx_port;
    int i, ret;
    mach_msg_return_t tx_msg_ret;

    /* fill the arrays(node_array & load_array) with half of the local node's
       load vector */

    /*
     * Lock the load vector.
     */
     lock_load_vec();

    for (i = 0; i < num_lvec_elements/2; i++) {
    	node_array[i] = load_info_ptr->load_vector[i].node;
    	load_array[i] = load_info_ptr->load_vector[i].lm;
    }

    /*
     * Unlock the load vector.
     */
     unlock_load_vec();

    node_array_sz = num_lvec_elements/2;
    load_array_sz = num_lvec_elements/2;

    /* scan the node_port_map structure to see whether a port has already
       been obtained for the randomly chosen node */
    if ( *(node_in_use + ran_node) == 1 ) 
        tx_port = *(node_port_map + ran_node);
    else 
        tx_port = MACH_PORT_DEAD;

    if (tx_port == MACH_PORT_NULL) {

        /* get a port associated with the generated random node
           number */
        ret = get_tnc_port(LOAD_LEVEL_ID, ran_node, &tx_port);
        if (ret == -1) {
            if (errno == ENOENT) 

                /*
                 * Let's assume the peer in only a little bit late in its
                 * start up.
                 */
                return(0);
            else 

                /*
                 * This is fatal.
                 */
                error_fatal("call to get_tnc_port failed");
        }

	if ( tx_port == MACH_PORT_DEAD ) {
		/*
		 * Let's assume the peer in only a little bit late in its
		 * start up.
		 */
		return (0);
	}

        /* update the node port map structure */
        *(node_port_map + ran_node) = tx_port;
    }
    if ((tx_port != MACH_PORT_NULL) && (tx_port != MACH_PORT_DEAD)) {

    	/* call the MIG generated routine which will send the mach message */
    	tx_msg_ret = tx_load_message(tx_port, node_array, node_array_sz,
       	                             load_array, load_array_sz);
    	if (tx_msg_ret != MACH_MSG_SUCCESS)
    		error_nonfatal("Mach message not queued: %s (%d)\n",
				mach_error_string(tx_msg_ret), tx_msg_ret);
    	return (0);
    }
    else
    	return (1);
} 


/******************************************************************
**  FUNCTION: initialize_migration_processes
**
**  DESCRIPTION: This function will determine the number of local processes
**               and randomly chose one of them.
**
**  INPUTS:  none
**
**  OUTPUTS: none
**
******************************************************************/
void
initialize_migration_processes()
{
    number_of_procs = table_node(local_node_num, TBL_PROCINFO, 0, NULL, 32767, 
				 0);
    if (number_of_procs == -1) {
    	proc_index_start = -1;
    }
    else {
    	proc_index_start = rand();
    	proc_index_start = proc_index_start%number_of_procs;
    	proc_index_current = proc_index_start;
    }
}


/***************************************************************************
**  FUNCTION: get_next_migration_process
** 
**  DESCRIPTION: This function will get the next local process that
**               is a candidate for migration.
**
**  INPUTS:  none
**
**  OUTPUTS: m_pid - pid of a process which is a candidate for migration
**
***************************************************************************/
int
get_next_migration_process()
{

    int m_pid;
    struct tbl_procinfo pi;
    task_t task;
    task_thread_times_info_data_t thread_i;
    int count;
    kern_return_t ret;
    double time_in_micros;

    if (proc_index_start == -1)
    	return -1;
    do {
    	(void)table_node(local_node_num, TBL_PROCINFO, proc_index_current, 
		    (char *)&pi, 1, sizeof(pi));

    	/* We're only interested in the current active user processes
    	   such that the process's controlling tty must have a proc group 
    	   id equal to the proc group id of the process.  Also, special bullet 
    	   proofing is added to make sure that we don't include this 
    	   process(load leveler daemon). */
    	if (pi.pi_status != PI_ACTIVE || pi.pi_pgrp == 0 || 
    	 pi.pi_pid == mypid || pi.pi_ttyd == -1) 
    		continue;
    	/* determine whether the process has accumulated enough processor
    	   time to be eligible for migration */
    	task = task_by_pid(pi.pi_pid);
    	count = TASK_THREAD_TIMES_INFO_COUNT;
    	ret = task_info(task, TASK_THREAD_TIMES_INFO,
     			(task_info_t) &thread_i,
			(mach_msg_type_number_t *) &count);
    	if (ret != KERN_SUCCESS) 
    		continue;

    	/* check the user run time for the live threads of the task and compare
           it against the minimum cpu time specified in the parameters file */
    	time_in_micros = (double)(1000000.0 * 
	 (double) thread_i.user_time.seconds) +
     	 (double)(thread_i.user_time.microseconds);
    	if (time_in_micros < (min_cputime * 1000000.0)) 
    		continue;

    	/* The process is still eligible for migration. Hence, check whether
    	   the migrate_commands file is an inclusion/exclusion list and match
    	   the short command name of the process with the commands listed in
    	   list. */
    	if (inclusion_list) {

    		/* the migrate_commands file is an inclusion list and thus
    		   we're only interested in those processes whose short command 
    		   name corresponds to commands listed in the migrate_commands 
    		   file */
    		if (!on_commands_list(pi.pi_comm)) 
    			continue;
    		m_pid = pi.pi_pid;
        }
        else {

                /* the migrate_commands file is an exclusion list and thus
                   we're only interested in those processes whose short 
    		   command name does not correspond to commands listed in the
                   migrate_commands file */
                if (on_commands_list(pi.pi_comm)) 
    			continue;
    		m_pid = pi.pi_pid;
        }
	if (traversed_entire_proc_table())
		proc_index_start = -1;

#ifdef MIGRATELOG
        strncpy(migrated_command,  pi.pi_comm, CMD_LEN);
        migrated_command[CMD_LEN - 1] = '\0';
#endif /* MIGRATELOG */

    	return (m_pid);
    } while (!traversed_entire_proc_table());

    /* this point is reached if the entire proc table has been scanned and
       no eligible process has been found */
    return -1;
}


/******************************************************************
**  FUNCTION: traversed_entire_proc_table
**
**  DESCRIPTION: This function will increment the current proc index
**               and check whether the entire proc table has been traversed.
**
**  INPUTS:  none
**
**  OUTPUTS: TRUE if entire proc table has been traversed
**           FALSE if entire proc table has not yet been traversed
**
******************************************************************/
int
traversed_entire_proc_table()
{
    proc_index_current++;
    if (proc_index_current >= number_of_procs) 
    	proc_index_current = 0;
    if (proc_index_current == proc_index_start) 
    	return TRUE;
    else
    	return FALSE;
}


/******************************************************************
**  FUNCTION: get_elapsed_time
**
**  DESCRIPTION: This function will get the time elapsed since the timer was
**               last checked.
**
**  INPUTS:  none
**
**  OUTPUTS: time_which_has_elapsed_ptr - time elapsed since the timer was
**                                        last checked
**
******************************************************************/
void
get_elapsed_time(
    double	*time_which_has_elapsed_ptr)
{
    struct timeval  time_now;

    if ( gettimeofday(&time_now, NULL) == -1 ) {
        error_fatal("couldn't get current time");
    }

    current_time = (double)(1000000.0 * (double)time_now.tv_sec) +
                   (double)(time_now.tv_usec);
    *time_which_has_elapsed_ptr = current_time - old_time ;

    /* reset old_time */
    old_time = current_time;
}


/******************************************************************
**  FUNCTION: error_init
**
**  DESCRIPTION: This function initializes error logging.
**
**  INPUTS:  str - string to be prepended to every error message
**
**  OUTPUTS: none
**
******************************************************************/
void
error_init(
    char        *str)
{
    /* initialize syslog() */
    openlog(str, LOG_CONS, LOG_DAEMON);
}


/******************************************************************
**  FUNCTION: error_fatal
**
**  DESCRIPTION: This function writes the specified message to the system
**               log and then checks whether memory was allocated for any
**               load leveler data items.  Finally the process is terminated.
**
**  INPUTS:  str - string to output to the system log
**
**  OUTPUTS: none
**
******************************************************************/
void
error_fatal(
    char        *str)
{
    int ret;

    syslog(LOG_ERR, "%s: %m\n", str);
    set_tnc_port(LOAD_LEVEL_ID, MACH_PORT_NULL);
    exit(1);
}


/******************************************************************
**  FUNCTION: error_nonfatal
**
**  DESCRIPTION: This function writes the specified message to the system
**               log. 
**
**  INPUTS:  variable number of arguments which are either integer
**           or string types
**
**  OUTPUTS: none
**
******************************************************************/
void
error_nonfatal(
	char *fmt,
	...)
{
    va_list ap;
    char abuf[BUFSIZ];

    va_start(ap, fmt);
    vsprintf(abuf, fmt, ap);
    syslog(LOG_INFO, abuf);
    va_end(args)
}


/******************************************************************
**  FUNCTION:
**		start_rx_info_thread
**
**  DESCRIPTION:
**		This function starts the thread that handles the
**		receiving of the load information.
**
**  INPUTS:
**		none
**
**  OUTPUTS:
**		none
**
******************************************************************/
void start_rx_info_thread()
{

	/*
	 * Wire the main thread.
	 */
	cthread_wire();

	/*
	 * Create the C thread.
	 */
	cthread_detach( cthread_fork((any_t (*) ()) rx_info_thread, (any_t) 0));
}


/******************************************************************
**  FUNCTION:
**		rx_info_thread
**
**  DESCRIPTION:
**		This function implements the thread that handles the
**		receiving of the load information.
**
**  INPUTS:
**		not used
**
**  OUTPUTS:
**		none
**
******************************************************************/
void rx_info_thread(arg)
any_t	arg;
{
	mach_msg_return_t	msg_ret;

	/*
	 * Wire the receiving thread.
	 */
	cthread_wire();

	/*
	 * Loop forever, processing incoming load messages.
	 */
	for ( ;; ) {
		msg_ret = mach_msg(&msg_buf_rx1.hdr, MACH_RCV_MSG, 0, 
					sizeof msg_buf_rx1, mynode_port, 0, 
					MACH_PORT_NULL);

		if (msg_ret != MACH_MSG_SUCCESS) {
    			error_nonfatal("Mach message not received: %s (%d)\n",
					mach_error_string(msg_ret), msg_ret);
			continue;
		}

		load_level_server(&msg_buf_rx1.hdr, &msg_buf_rx2.reply_msg.Head);
		/* no reply required */
	}
}


/******************************************************************
**  FUNCTION:
**	wait_for_timeout()
** 
**  DESCRIPTION:
**	This function waits till a timeout occurs.
**
**  INPUTS:
**	 remain_send_time_ptr :		pointer to variable containing the
**					time in microseconds left before
**					sending load info to another node.
**
**	remain_re_dispatch_time_ptr :	pointer to variable containing the
**					time in microseconds left before 
**					before invoking re-dispatch 
**					algorithm.
#ifdef SLL
**	remain_fast_node_time_ptr :	pointer to variable containing the
**					time in microseconds left before 
**					before invoking fast_node.
#endif /*SLL*/ /*
**
**  OUTPUTS:
**	elapsed_time :			time elapsed since the interval
**					timer was last checked.
**
******************************************************************/
void
wait_for_timeout(
    double	*remain_send_time_ptr,
    double	*remain_re_dispatch_time_ptr,
#ifdef SLL
    double	*remain_fast_node_time_ptr,
#endif /* SLL */
    double	*elapsed_time_ptr)
{
    int ret;
    mach_msg_return_t msg_ret;
    mach_msg_timeout_t msg_timeout;
#ifdef SLL
    double	min_timeout;
#endif /* SLL */


#ifndef SLL
    /* choose the minimum of the rem_send_time and rem_re_dispatch_time
       for the receive timeout value */
    if (*remain_send_time_ptr < *remain_re_dispatch_time_ptr) 
    	msg_timeout = (mach_msg_timeout_t)((*remain_send_time_ptr) / 1000.0);
    else
    	msg_timeout =
	 (mach_msg_timeout_t)((*remain_re_dispatch_time_ptr) / 1000.0);
#else /* SLL */
    /*
     * Choose the minimum of all remaining times for the timeout value
     */
    min_timeout = *remain_send_time_ptr;
    if ( *remain_re_dispatch_time_ptr < min_timeout ) {
	min_timeout = *remain_re_dispatch_time_ptr;
    }
    if ( *remain_fast_node_time_ptr < min_timeout ) {
	min_timeout = *remain_fast_node_time_ptr;
    }

    msg_timeout = (mach_msg_timeout_t)(min_timeout / 1000.0);
#endif /* SLL */

    /*
     *	I am also too lazy at the moment to implement anything
     *	other than a timeout on a dummy port.
     *  Later, when the itimer() stuff is working  usleep() could be used
     *  instead.
     */
    msg_ret = mach_msg(&msg_buf1.hdr, MACH_RCV_MSG | MACH_RCV_TIMEOUT, 0, 
    		       sizeof msg_buf1, timeout_port, msg_timeout, 
    		       MACH_PORT_NULL);

    get_elapsed_time(elapsed_time_ptr);
}


/******************************************************************
**  FUNCTION:
**	lock_load_vec()
** 
**  DESCRIPTION:
**	This function locks the load vector
**
**  INPUTS:
**	 none 
**
**  OUTPUTS:
**	 none 
**
******************************************************************/
void	lock_load_vec()
{
	/*
	 * Get the lock.
	 */
	spin_lock(&load_vec_lock);
}


/******************************************************************
**  FUNCTION:
**	unlock_load_vec()
** 
**  DESCRIPTION:
**	This function unlocks the load vector
**
**  INPUTS:
**	 none 
**
**  OUTPUTS:
**	 none 
**
******************************************************************/
void	unlock_load_vec()
{
	/*
	 * Release the lock.
	 */
	spin_unlock(&load_vec_lock);
}

/******************************************************************
**  FUNCTION: get_root_fs_node
**
**  DESCRIPTION: This function will return the node number
**               of ROOT_FS_NODE
**
**  INPUTS:  none
**
**  OUTPUTS: node number of ROOT_FS_NODE
**
******************************************************************/
int	 get_root_fs_node()
{
	int	i;
	int	num_phys_nodes;
	int	*phys_node_num_ptr;
	int	phys_root_fs_node;
	char	*bootvalue;


	/*
	 * First get bootmagic ROOT_FS_NODE.
	 */
	bootvalue = getbootenv("ROOT_FS_NODE");
	if ( bootvalue == NULL ) {
		error_nonfatal("cannot get bootmagic ROOT_FS_NODE\n");
		return(-1);
	}

	/*
	 * Convert to node number.
	 */
	if ( isdigit(*bootvalue) ) {
		phys_root_fs_node = atoi(bootvalue);
	}
	else {
		error_nonfatal("invalid value for ROOT_FS_NODE\n");
		return(-1);
	}

	/*
	 * Get number of physical nodes in service partition.
	 */
    	num_phys_nodes = table(TBL_PHYSNODEINFO, 0, (char *)0, 32767, 0);
	if ( num_phys_nodes == -1 ) {
		error_nonfatal("cannot obtain the number of physical service nodes\n");
		return(-1);
	}

	/*
	 * Allocate memory for the list of physical node numbers.
	 */
	phys_node_num_ptr = (int *)calloc((size_t)num_phys_nodes, sizeof(int));
	if ( phys_node_num_ptr == NULL ) {
		error_nonfatal("out of memory\n");
		return(-1);
	}

	/*
	 * Get the list of physical node numbers.
	 */
	if (table(TBL_PHYSNODEINFO, 0, (char *)phys_node_num_ptr,
			num_phys_nodes, sizeof(int)) == -1) {
		error_nonfatal("cannot obtain list of physical service node"
				" numbers\n");
		return(-1);
	}

	/*
	 * Look for phys_root_fs_node in the list of physical node numbers.
	 */
	for ( i = 0; i < num_phys_nodes; i++ ) {
		if ( *(phys_node_num_ptr + i) == phys_root_fs_node ) {
			break;
		}
	}

	/*
	 * Free the list of physical node numbers.
	 */
	free(phys_node_num_ptr);

	/*
	 * Check if we were sucessful.
	 */
	if ( i >= num_phys_nodes || i >= num_nodes ) {
		error_nonfatal("cannot obtain logical node number for"
				"node %d\n", phys_root_fs_node);
		return(-1);
	}

	/*
	 * We are done.
	 */
	return(*(node_num_ptr + i));
}


#ifdef	NX
int frank_magic(where)
    char    *where;
{
    asm("trap r0,r25,r0");
}


int trap_host_get_boot_info( priv_host, boot_info )
    host_t              priv_host;
    char		*boot_info;
{
	int 		    boot_stuff_len;

	boot_stuff_len = frank_magic(boot_info);

	if (boot_stuff_len > 0)
		return (KERN_SUCCESS);
}

#define host_get_boot_info	trap_host_get_boot_info
#endif	/* NX */

/******************************************************************
**  FUNCTION: getbootenv
**
**  DESCRIPTION: This function returns the value of a boot magic.
**
**  INPUTS:  name of boot magic
**
**  OUTPUTS: pointer to value of boot magic
**
******************************************************************/
char	*getbootenv(name)
char	*name;
{
	static char	*bootmagic = NULL;
	char		*tmp_bootmagic;
	kern_return_t	kr;
	mach_port_t	privileged_host_port;
        int		len;
        char		*src;
        char		*dest;
        char		*str;
	int		i;


	if ( bootmagic == NULL ) {
		/*
		 * Not initialized yet.
		 */
		bootmagic = (char *) malloc(BOOTMAGIC_MAX);
		if ( bootmagic == NULL ) {
			/*
			 * Oops - out of memory.
			 */
			return (NULL);
		}

		tmp_bootmagic = (char *) malloc(BOOTMAGIC_MAX);
		if ( tmp_bootmagic == NULL ) {
			/*
			 * Oops - out of memory.
			 */
			return (NULL);
		}

		/*
		 * Get the boot magics from the kernel.
		 */
		privileged_host_port = task_by_pid(-1);
		kr = host_get_boot_info(privileged_host_port, tmp_bootmagic);
		if ( kr != KERN_SUCCESS ) {
			/*
			 * Oops - didn't get boot magics from kernel.
			 */
	    		return (NULL);
		}

		/*
		 * Convert to null terminated format.
		 */
		len = strlen(tmp_bootmagic);

		/*
		 * Copy source to bootmagicination, converting
		 * newlines to NULLs.
		 */
		src = tmp_bootmagic;
		dest = bootmagic;
		for ( i = 0; i < len; i++ ) {
			*dest++ = *src++;
			if ( *(src - 1) == '\n' ) {
				*(dest - 1) = NULL;
			}
		}

		/*
		 * Add terminating NULL.
		 */
		*dest = NULL;

		/*
		 * Free temporary buffer.
		 */
		free(tmp_bootmagic);
	}

	/*
	 * Search for the boot magic we have been asked for.
	 */
	len = strlen(name);
	for ( str = bootmagic; *str; str += strlen(str) + 1 ) {
		if ( strncmp(str, name, len) == 0 && str[len] == '=' ) {
			return (&str[len + 1]);
		}
	}

	/*
	 * Not found.
	 */
	return (NULL);
}
