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
 *
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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/schedule.c,v 1.50 1995/03/07 01:03:51 carolr Exp $
 *
 * History:
 * $Log: schedule.c,v $
 * Revision 1.50  1995/03/07  01:03:51  carolr
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
 *    EATS: controlc, rmcall, rmcmd, sched
 *    manual testing
 *
 *  Module(s):
 *      .../src/usr/sbin/allocator/allocator.c
 *      .../src/usr/sbin/allocator/schedule.c
 *      .../src/usr/sbin/allocator/misc_rpcs.c
 *
 * Revision 1.49  1994/11/19  03:04:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.48  1994/06/13  16:58:13  sdh
 * Changed debug messages to go through debug print routine.
 *
 *  Reviewer: mag
 *  Risk: low
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
 * Changed code so a new application starting in a partition that is
 * already running an application, does not pick up the interval time.
 *
 *  Reviewer: carbajal
 *  Risk: low
 *  Benefit or PTS #: 7624
 *  Testing: EATS, manual testing
 *  Module(s):
 *         cmds_libs/src/usr/sbin/allocator/schedule.c
 *
 * Revision 1.47  1994/06/02  18:14:13  mag
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
 * Revision 1.46  1994/04/27  23:09:24  sdh
 *  Merge with R1.2
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.45  1994/03/02  19:59:55  carbajal
 * 7998 PARAGON      OPEN      H        carbajal  simont              R1.2 WW05
 *       MESH UTILS   03-FEB-94 1        31-JAN-94 31-JAN-94
 *       overlapping job occasionally roll out & other job roll in before its
 *       RQ expires
 *
 * Mandatory?:  No (but should be, this was what was causing the sched EATs to
 *                 fail)
 *   Reviewer(s): Cameron
 *   Risk:       Low
 *   Benefit:    Make scheduling EATs pass consistently (PTS #7998)
 *   Module(s):  schedule.c (in allocator source)
 *   Testing:    resource mgmt EATs, Scheduling Eats, manual round robin testing
 *
 * Revision 1.40.2.5  1994/04/27  00:11:56  sdh
 *  Added check_for_reschedule() and added a call to it in rollin code.
 *  Reviewer: John Carbajal
 *  Risk: Medium
 *  Benefit or PTS #: 8687
 *  Testing: EATS: sched, controlc, rmcall
 *  Module(s):
 * 	schedule.c
 *
 * Revision 1.40.2.4  1994/03/02  19:58:13  carbajal
 *  8287 PARAGON      OPEN      H        carbajal  pinglee             R1.2 WW08
 *       MESH UTILS   25-FEB-94 **       25-FEB-94 25-FEB-94
 *       Overlapping app in one part. or overlapping active part. wasn't gang
 *       scheduled.
 *
 *   Mandatory?:   No but should be
 *   Reviewer(s): Cameron
 *   Risk:        Low
 *   Benefit:     Gang scheduling is broken in WW08 as a result of a bug fix
 *   Module(s):   schedule.c (allocator)
 *   Testing:     Sched eats, bug report test case, developer
 *
 * Revision 1.40.2.3  1994/02/16  19:55:59  carbajal
 * 7998 PARAGON      OPEN      H        carbajal  simont              R1.2 WW05
 *       MESH UTILS   03-FEB-94 1        31-JAN-94 31-JAN-94
 *       overlapping job occasionally roll out & other job roll in before its
 *       RQ expires
 *
 * Mandatory?:  No (but should be, this was what was causing the sched EATs to
 * 		fail)
 *   Reviewer(s): Cameron
 *   Risk:       Low
 *   Benefit:    Make scheduling EATs pass consistently (PTS #7998)
 *   Module(s):  schedule.c (in allocator source)
 *   Testing:    resource mgmt EATs, Scheduling Eats, manual round robin testing
 *
 * Revision 1.40.2.2  1994/01/06  22:19:44  carbajal
 * Initialize and update the total time for active partitions.
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7489
 *  Testing: Bug report
 *  Module(s): pspart_rpc.c, schedule.c
 *
 * Revision 1.40.2.1  1993/12/20  21:56:45  carbajal
 *   Be careful to vm_dealloc() memory properly. Use macros FREE and
 *   MALLOC to make sure all memory allocated is properly deallocated.
 *    Reviewer: cameron
 *    Risk: Low
 *    Benefit or PTS #: 7257
 *    Testing: SATs, EATs, MUNOPS, bug test
 *    Module(s): misc_rpcs.c, mkpart_rpc.c, schedule.c, rmpart_rpc.c
 *
 * Revision 1.41  1993/12/10  20:40:04  carbajal
 * Religiously use macros FREE and MALLOC to help debug vm leaks
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: Works towards solving 7257
 *  Testing: Bug test
 *  Module(s):
 *
 * Revision 1.40  1993/12/01  01:39:52  carbajal
 * Initialize elapsed and start_time to 0 when a partition is deactivated.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.39  1993/11/18  20:24:02  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of the export tree
 *  Testing: built on Suns and 486
 *  Module(s): scripts.mk standard.mk
 *
 * Revision 1.38  1993/11/17  02:55:26  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.37  1993/10/27  02:03:28  carbajal
 * Added IFDEFed signal profiling code
 *
 * Revision 1.36  1993/08/31  01:49:45  carbajal
 * Increment application time in remove_appl(). PTS #5999
 *
 * Revision 1.35  1993/08/23  17:21:16  carbajal
 * Use part->sched_list for round robin layer selection. PTS #6095
 *
 * Revision 1.34  1993/07/29  00:27:48  carbajal
 * When removing an application use remove_consumer_from_tile()
 *
 * Revision 1.33  1993/07/15  22:21:05  carbajal
 * Cleanup conflict list when a partition has no more scheduling
 * layers.
 *
 * Revision 1.32  1993/05/27  00:18:00  carbajal
 * Call migrate_objects_to_tile before capturing the active layer pointer in
 * rollin()
 *
 * Revision 1.31  1993/05/25  23:56:57  carbajal
 * Added support for scheduling layers, and migrating objects between layers
 *
 * Revision 1.30  1993/04/13  01:12:04  carbajal
 * More debugging
 * INCR_TIME placement
 *
 * Revision 1.29  1993/04/05  06:24:20  carbajal
 * Use NO_PRI for null priority value
 *
 * Revision 1.28  1993/04/02  23:28:44  carbajal
 * Use NO_PRI to indicate null priority values
 * Fixed STD scheduled partitions
 *
 * Revision 1.27  1993/03/31  21:16:23  carbajal
 * reset the rollin_quantum in set_cur_pri if the partition
 * priority is zero.
 * Enabled std scheduled partitions
 *
 * Revision 1.26  1993/03/29  23:48:38  carbajal
 * Make sure we reset rolled_in in the right spots
 *
 * Revision 1.25  1993/03/25  02:45:19  carbajal
 * Cleaned up compiler warnings
 * Cleaned up code that handles readjusting layer priorities
 * server_loop.c: now calls schedule() before responding to server RPC
 *
 * Revision 1.24  1993/02/27  20:41:55  carbajal
 * Fixed a precendence problem in rollin() that would cause an
 * application to be stopped but never started.
 *
 * Revision 1.23  1993/02/18  03:18:55  carbajal
 * Corrected extern declaration from HASH_TBL_T *appl_tbl to
 * HASH_TBL_T appl_tbl[].
 * In remove an nx application clean up the chain of active layers
 * make sure we reset the priorities along the way.
 * Fixed PTS#4083, enforcement of EPL across layers
 * Fixed PTS#4124, partitions getting stuck at high priority if you run
 * a high priority application in them.
 *
 * Revision 1.21  1993/01/28  21:39:07  shala
 * Fixed multiply defined LP_MAP_T .
 *
 * Revision 1.20  1993/01/28  19:48:38  carbajal
 * Removed multiply defined LP_MAP_T stuff. This is no longer
 * in allocator.h but is now in msmsg_appl.h
 *
 * Revision 1.19  1993/01/28  17:23:08  carbajal
 * Set parent layer priority as we percolate up
 *
 * Revision 1.18  1993/01/28  03:33:01  carbajal
 * cleaned up compiler warnings
 * cleaned up some asserts
 * removed kill(pgroup,0) check to see if pgroup still exists
 * added priority percolation
 *
 * Revision 1.17  1993/01/19  05:05:16  carbajal
 * Fixed two bugs: Inequality test was wrong in set_part_pri
 * Using wrong index into rollin list
 *
 * Revision 1.16  1993/01/18  20:05:07  carbajal
 * schedule.c: use debug macros instead of ifdefs
 * put in many asserts
 * Fixed sorting of rollin/rollout lists
 * Check to see if application visited is the new aplication
 * Only rollout new application if we know for sure that
 * it is not to be scheduled now.
 *
 * Revision 1.15  1992/12/24  03:19:59  carbajal
 * Make a list of all applications that will be rolled
 * in and out. Compare the rollin list to the roll out
 * list and remove duplicate entries.
 * When checking priorities of layers first see if there
 * are applications in the layer.
 * Set appl->appl_stat in gang_start and gang_stop.
 * Don't register an application for gang_stop if
 * appl->appl_stat == APPL_GANG_STOP.
 *
 * Revision 1.14  1992/12/18  17:06:10  carbajal
 * Use priorities when scheduling
 * Made routines gang_start, gang_stop
 * Extern int init_app_cnt
 *
 * Revision 1.13  1992/11/05  01:51:33  carbajal
 * Fixed and ifdef DEBUG bug that caused a core dump because appl
 * was not initialized prior to trying to printf it.
 *
 *
 */

/*
 * schedule.c	- Don Cameron	3/92
 *
 * This file contains the scheduling routines used by the allocator server.
 *
 */
#include <sys/types.h>
#include <sys/resource.h>
#include <signal.h>
#include <stdio.h>
#if OSF
#include <malloc.h>
#else
#include <libc.h>
#endif
#include <assert.h>
#include <mcmsg/mcmsg_appl.h>
#include <nx/bitmap.h>
#include <nx/hash.h>
#include <nx/schedule.h>
#include <nx/defines.h>
#include <nx/smd.h>
#include "debug.h"
#include "macros.h"
#include "conflict.h"
#include "tiles.h"
 
#define MAX_RQ_LIST	10000

#ifdef DO_SIG_PROF
/* Global for timing tests */
typedef struct {
        int     cnt;
        double  elapsed;
} profile_t;

profile_t       kill_time;

void
init_profile()
{
        kill_time.cnt = 0;
        kill_time.elapsed = 0.0;
}

void
dump_profile()
{
        if (kill_time.cnt == 0)
                return;
        printf("cnt %d total elapsed %f\n",kill_time.cnt,kill_time.elapsed);
        printf("time per schedule event %f\n",kill_time.elapsed/kill_time.cnt);
}
#endif DO_SIG_PROF

/*
 * Global variable indicating the latest application started
 */
APPL_T	*new_appl_ptr;


/*
 * Application table allocator.c
 */
extern PART_T  *root;
extern HASH_TBL_T	appl_tbl[]; 
extern long		init_app_cnt;
extern int sched_debug,allocation_debug,signal_debug,compare_debug;
extern unsigned long interval;
/*
 * Forward declarations
 */
void remove_application(APPL_T *appl);
LAYER_T *find_highest_pri(PART_T *part);
void gang_start(APPL_T *appl);
void gang_stop(APPL_T *appl);
void register_gang_start(APPL_T *appl);
void register_gang_stop(APPL_T *appl);
void scheduler(unsigned long interval, PART_T *part);
void rollin(int do_start,PART_T *p);
void perk_remove_active_partition(PART_T *part);

void set_layer_pri(LAYER_T *layer);
void set_cur_part_pri(PART_T *part);
void perk_player_pri(LAYER_T *p_layer);
void rollin_std(PART_T *p);
LAYER_T *
check_for_reschedule(PART_T *part, int *reschedule);


/* Scheduling lists */
static APPL_T	**rollin_list;		/* List for rollin register */
static APPL_T	**rollout_list;		/* List for rollout register */
static APPL_T	**ro_list;		/* actual items to rollout */
static APPL_T	**ri_list;		/* actual items to rollin */
static int	ro_size,ri_size,ro_cnt,ri_cnt;


/* Setup to do gang scheduling by creating the rollin/rollout registers 
*/
void
init_scheduling()
{
	/* XXXX there are no checks to see if this size is exceeded XXXX*/
	ro_size = MAX_RQ_LIST;
	rollout_list = (APPL_T **) MALLOC(ro_size * sizeof(APPL_T *));
	ri_size = MAX_RQ_LIST;
	rollin_list = (APPL_T **) MALLOC(ri_size * sizeof(APPL_T *));
	if (rollin_list == (APPL_T **) 0){
		perror("allocator error ");
		exit(1);
	}
	if (rollout_list == (APPL_T **) 0){
		perror("allocator error ");
		exit(1);
	}

	/* allocate space for actual rollin/rollout lists */
	ro_list = (APPL_T **) MALLOC(ro_size * sizeof(APPL_T *));
	ri_list = (APPL_T **) MALLOC(ri_size * sizeof(APPL_T *));
	if (ri_list == (APPL_T **) 0){
		perror("allocator error ");
		exit(1);
	}
	if (ro_list == (APPL_T **) 0){
		perror("allocator error ");
		exit(1);
	}
}

/* Free the scheduling registers 
*/
void
free_schedule_lists()
{
	FREE( (void *) rollout_list);
	FREE( (void *) rollin_list);
	FREE( (void *) ri_list);
	FREE( (void *) ro_list);
}


/* Compare pid's. This is used by bsearch() and qsort()
*/
int
pid_compare(ap1,ap2)
APPL_T	**ap1,**ap2;
{
	debug_compare(5,"pid_compare %d %d\n",(*ap1)->pgroup,(*ap2)->pgroup);
	return((*ap1)->pgroup - (*ap2)->pgroup);
}


void
schedule(interval,part)
unsigned long	interval;
PART_T		*part;
{
	register int	i;
	int		ro_cidx,ri_cidx;		/* indices into scedule lists */
	APPL_T		**ptr,**new_ptr1,**new_ptr2;

	/* indexes for actual lists */
	ro_cidx = 0;
	ri_cidx = 0;

	/* count of items registered by register_gang_stop/start */
	ro_cnt = 0;
	ri_cnt = 0;

	/* build our rollin/rollout lists */
	scheduler(interval,part);
	
	/* sort the rollout list to be used with the binary search */
	qsort((char *)rollout_list,ro_cnt,sizeof(APPL_T **),pid_compare);
	/* sort the rollin list to be used with the binary search */
	qsort((char *)rollin_list,ri_cnt,sizeof(APPL_T **),pid_compare);

	if (sched_debug){
		debug_sched(5,"Items registered are:\n");
		for(i = 0; i < ri_cnt; i++){
			debug_sched(5,"ri -> %d\n",rollin_list[i]->pgroup);
		}
		for(i = 0; i < ro_cnt; i++){
			debug_sched(5,"ro -> %d\n",rollout_list[i]->pgroup);
		}
	}

	/* Check every item in the rollin list against what is in the rollout 
	 * list. Any duplicates will not be entered into our actual rollin list 
	*/
	for(i = 0; i < ri_cnt; i++){
		ptr = (APPL_T **) bsearch((char *)(&rollin_list[i]),
			(char *)rollout_list,ro_cnt,
                	sizeof(APPL_T **),pid_compare);
		if (ptr == (APPL_T **) 0){
			/* item in the rollin_list does not match anything in 
			 * the rollout_list so add this item to the actual
			 * list we will use to rollin
			*/
			ri_list[ri_cidx++] = rollin_list[i];
		}
	}

	/* Check every item in the rollout list against what is in the rollin 
	 * list. Any duplicates will not be entered into our actual rollout list 
	*/
	for(i = 0; i < ro_cnt; i++){
		ptr = (APPL_T **) bsearch((char *)(&rollout_list[i]),
			(char *)rollin_list,ri_cnt,
                	sizeof(APPL_T **),pid_compare);
		if (ptr == (APPL_T **) 0){
			/* item in the rollout_list does not match anything in 
			 * the rollin_list so add this item to the actual
			 * list we will use to rollout
			*/
			ro_list[ro_cidx++] = rollout_list[i];
		}
	}

	/* if new_appl_ptr is not null then we need to 
	 * stop this application
	*/
	if (new_appl_ptr != (APPL_T *)0){
		debug_sched(5,"New application does not show up in any lists\n");
		ro_list[ro_cidx++] = new_appl_ptr;
		new_appl_ptr = (APPL_T *)0;
	}

	if (sched_debug){
		debug_sched(5,"Actual items to rollin/rollout\n");
		for(i = 0; i < ri_cidx; i++){
			if ( ri_list[i] != (APPL_T *)0)
				debug_sched(5,"ri -> %d\n",ri_list[i]->pgroup);
		}
		for(i = 0; i < ro_cidx; i++){
			if ( ro_list[i] != (APPL_T *)0)
				debug_sched(5,"ro -> %d\n",ro_list[i]->pgroup);
		}
	}

	/* First rollout the applications */
	for(i = 0; i < ro_cidx; i++)
		if (ro_list[i] != (APPL_T *)0)
			gang_stop(ro_list[i]);

	/* Now rollin the new applications */
	for(i = 0; i < ri_cidx; i++)
		if (ri_list[i] != (APPL_T *)0)
			gang_start(ri_list[i]);

		/* don't call it here, call at end of allocServ_run() in allocator.c */
		/* calling it here frees the list before we are done using it.... */
        /* free_schedule_lists();  */

}


/*
 * scheduler() - Increment rollin time of applications and partitions and
 *              determine what needs to be rolled out and in.
 *	
 *	Parameters:
 *		unsigned long interval	Millseconds since last scheduled
 *		PART_T *part		Partition to schedule
*/
void
scheduler(unsigned long interval, PART_T *part)
{

	PART_T	*part_ptr;
	LAYER_T	*layer;
	int	reschedule;

	assert((part != (PART_T *)0));

	if (part->status == PART_ROLLED_IN){
		/* only increment the amount of time rolled in for a partition
		 * that is active 
		*/
		debug_sched(5,"scheduler():INCR_TIME partition part->inode %d\n",part->inode);
		INCR_TIME(interval, part->rolled_in);
	}

	if (sched_debug){
		debug_sched(5,"part->inode=%d part->rolled_in=%d part->rollin_quantum=%d part->sched=%d\n",
       			part->inode, part->rolled_in, part->rollin_quantum, part->sched);
		debug_sched(5,"status=%d intvl= %d\n",
       			part->sched, part->status,interval);
		if (part->active_lyr != (LAYER_T *)0)
			debug_sched(5,"active layer 0x%x\n",part->active_lyr);
	}

	layer = check_for_reschedule(part,&reschedule);

	/* if our partition is gang scheduled and */
	if ( (part->sched != UNIX) &&                 
		/* if the rolling quantum has expired */
	    ( ((part->rollin_quantum != 0) && (part->rolled_in > part->rollin_quantum)) ||
		/* or we are not the highest priority layer */
	    reschedule) )
	{
		/*
		 * Our quantum has expired, rollout active layer and rollin
		 * next layer.
		 */
		if (sched_debug){
	    		if ( (part->rollin_quantum != 0) && 
				(part->rolled_in > part->rollin_quantum)
			   )
				debug_sched(5,"quantum expired !!!\n");
			else
				debug_sched(5,"not highest priority\n");
		}
		rollout(interval, part);
		select_next_layer(part);
		rollin(FALSE,part);
	}
	else {
		CONSUMER_T	*consumer;

		/*
		 * Our quantum has not expired, so we check to see if any of
		 * our subpartitions' quanta have.
		 */
		debug_sched(5,"quantum not expired part->inode %d\n", part->inode);
restart:


		if (part->active_lyr == (LAYER_T *) 0) {
			if (part->child_sched_lyr == (LAYER_T *) 0)
				consumer = (CONSUMER_T *)0;
			else
				consumer = part->child_sched_lyr->consumer;
		}
		else {
			consumer = part->active_lyr->consumer;
		}

		while (consumer !=  (CONSUMER_T *) 0) {
			assert( (consumer->type == PART) || (consumer->type == APPL) );
			if (consumer->type == PART) {
				PART_T	*partition;
				/* Use this opportunity to update the partition's
				 * rolled in time and the elapsed time
				*/
				partition = (PART_T *)consumer;
				debug_sched(5,"scheduler():INCR_TIME partition\n");		
				INCR_TIME(interval, partition->elapsed);
				/*
				 * Consumer is a partition, recursively apply
				 * schedule().
				 */
				scheduler(interval, (PART_T *) consumer);
			} else {
			    APPL_T *appl;
			    
			    /*
			     * Consumer is an application.
			     */
			    appl = (APPL_T *) consumer;
			    
			    /*
			     * Check to see if this application is
			     * newly started. If it was, then it hasn't
			     * yet been started and we must do it here.
			     */
			    if ( (new_appl_ptr != (APPL_T *)0) && (appl == new_appl_ptr)){
				debug_sched(5,"new application to remain active\n");
				debug_sched(5,"scheduler():New appl %d %d\n",appl->pgroup,appl->elapsed);
				
				new_appl_ptr = (APPL_T *)0;
				appl->appl_stat = APPL_GANG_START;
			    } else {
				/*
				 * Take this
				 * opportunity to update the rollin time
				 * of the application. This is done by
				 * rollout() if our quantum has expired.
				 */
				debug_sched(5,"scheduler():INCR_TIME appl %d %d\n",appl->pgroup,interval);
				INCR_TIME(interval, appl->elapsed);
				INCR_TIME(interval, appl->rolled_in);
			    }
			    
			}
                        consumer = consumer->next;
		    }
		
		/* See if our layer is full */
		if (part->active_lyr != (LAYER_T *)0){
			if (part->active_lyr->num_free != 0){
				/* the currently scheduled layer is only partially full
			 	* see if we can fill it
				*/
				migrate_objects_to_tile(TRUE,&(part->active_lyr),&(part->child_sched_lyr));
			}
		}
	}
}

/*
 * rollout() - Rollout layer of partitions and applications after incrementing
 *             rollin time.
*/
void
rollout(interval, p)
unsigned long	interval;	/* Milliseconds since last scheduled */
PART_T		*p;		/* Partition containing layer to rollout */
{
	CONSUMER_T	*consumer;	/* partition or application */
	
	assert((p != (PART_T *)0));
	debug_sched(5,"rollout part->inode %d\n",p->inode);

	if (p->active_lyr == (LAYER_T *) 0) {
		consumer = (CONSUMER_T *) 0;
	}
	else {
		consumer = p->active_lyr->consumer;
	}

	p->status = PART_ROLLED_OUT;
	
	while (consumer !=  (CONSUMER_T *) 0) {
		assert( (consumer->type == PART) || (consumer->type == APPL) );
		if (consumer->type == PART) {
			PART_T *part;

			/*
			 * Consumer is a partition, increment rollin time
			 * and rollout its active layer.
			 */
			part = (PART_T *) consumer;
			if (part->status == PART_ROLLED_IN){
				debug_sched(5,"rollout():INCR_TIME partition inode %d\n",part->inode);	
				INCR_TIME(interval, part->rolled_in);
			}
			INCR_TIME(interval, part->elapsed);
			rollout(interval, part);
			consumer = consumer->next;
		}
		else {
			APPL_T *appl;

			/*
			 * Consumer is an application, increment rollin time
			 * and send it a signal to stop.
			*/
			appl = (APPL_T *) consumer;
			/* do this now in case consumer (appl) disappears */
			consumer = consumer->next;
			debug_sched(5,"rollout():INCR_TIME appl %d intvl %d\n",appl->pgroup,interval);
			INCR_TIME(interval, appl->elapsed);
			appl->rolled_in = 0;
			register_gang_stop(appl);
			if ( (new_appl_ptr != (APPL_T *)0) && (new_appl_ptr == appl)){
				new_appl_ptr = (APPL_T *)0;
				debug_sched(5,"new application will be rolled out\n");
			}
		}
	}
}

/*
 * rollin() - Rollin layer of partitions and applications.
 *
 *	Parameters:
 *		do_start	== 0 if we are to call register_gang_start()
 *				== 1 if we are to call gang_start()
 *		p		== pointer to the partition to rollin
 *
 *	NOTES:
 *		This code is shared between scheduler() and migrate_objects_to_tile().
 *		The do_start flag is used to indicate where we are coming from, do_start == 0
 *		if we are being called from scheduler().
 *
 */
void
rollin(int do_start,PART_T *p)
{
	CONSUMER_T	*consumer;	/* partition or application */
	int		reschedule;
	

	assert((p != (PART_T *)0));
	debug_sched(5,"rollin part->inode %d\n",p->inode);

	if (p->sched == UNIX){
		rollin_std(p);
		return;
	}

	/* see if we can change the partition status */
	if (p->cur_priority > NO_PRI)
		/* This partition has a positive priority value, this
		 * implies that there are applications to schedule in it,
		 * or in it's subpartitions
		*/
		p->status = PART_ROLLED_IN;
	

	/* Only reset the rollin quantum if our quantum is not zero and
	 * we have exceeded our rollin quantum. We will be called to rollin
	 * a partition if we are rollin the parent partition. The 2 could
	 * have different rollin quanta.
	*/
	if ( (p->rollin_quantum != 0) && (p->rolled_in > p->rollin_quantum) ){
		debug_sched(5,"quantum reset\n");
		p->rolled_in = 0;
	}


restart:
	/* Make sure the tile is full */
	migrate_objects_to_tile(FALSE,&(p->active_lyr),&(p->child_sched_lyr));

	if (p->active_lyr == (LAYER_T *) 0) {
		consumer = (CONSUMER_T *) 0;
	}
	else {
		consumer = p->active_lyr->consumer;
	}

	while (consumer !=  (CONSUMER_T *) 0) {
		assert( (consumer->type == PART) || (consumer->type == APPL) );
		if ((consumer->type == PART) &&		    /* Partition */
		    (((PART_T *) consumer)->sched != UNIX)) /* Gang scheduled */
		{
			PART_T *part;
			LAYER_T *layer;

			/*
			 * Consumer is a gang scheduled partition, rollin
			 * its active layer.
			 */
			part = (PART_T *) consumer;
			debug_sched(5,"rollin gang partition inode %d\n",part->inode);

			layer = check_for_reschedule(part,&reschedule);

			debug_sched(5,"rollin reschedule %d\n",reschedule);
			
			/* See if our quantum has expired, if so then
			 * we need to select the next layer to schedule.
			 * We can get to rollin() because we are rolling
			 * in our parent. Our particular quantum may have
			 * not been exceeded when we were rolled out.
			*/
			if ( ( (part->rollin_quantum != 0) && 
				(part->rolled_in > part->rollin_quantum)) ||
				  reschedule )
			{
				/*
				 * Quantum exceeded, get next layer.
				 */
				if ( (part->rollin_quantum != 0) && 
					(part->rolled_in > part->rollin_quantum) ) 
					debug_sched(5,"rollin quantum expired!\n");
				else
					debug_sched(5,"rollin not highest priority\n");
				
				select_next_layer(part);
			}
			rollin(do_start,part);
			consumer = consumer->next;
		}
		else if (consumer->type == APPL) {
			APPL_T *appl;
			/*
			 * Consumer is an application.
			 */
			debug_sched(5,"rollin application\n");
			appl = (APPL_T *) consumer;
			/* do this now in case consumer (appl) disappears */
			consumer = consumer->next;
			/* See if we have a new application */
			if (new_appl_ptr != (APPL_T *)0){
				/* Yes!, is this the new one? */
				if (new_appl_ptr != appl)
					/* No so register it */
					if (!do_start)
						register_gang_start(appl);
					else
						gang_start(appl);
				else{
					/* Yes, the application is already
					 * started and it is ok to let it run
					*/
					debug_sched(5,"New application will remain active\n");
					new_appl_ptr = (APPL_T *)0;
					appl->appl_stat = APPL_GANG_START;
				}
			}
			else
				if (!do_start)
					register_gang_start(appl);
				else
					gang_start(appl);
		}
		else
		if ((consumer->type == PART) &&             /* Partition */
			(((PART_T *) consumer)->sched == UNIX)){
			/* Std scheduled */
			debug_sched(5,"rollin std partition\n");
			rollin(FALSE,(PART_T *)consumer);
			consumer = consumer->next;
		}
	}
}

/*
 * rollin_std() - Rollin layer of partitions and applications.
 */
void
rollin_std(p)
PART_T	*p;	/* Partition containing layer to rollin */
{
	CONSUMER_T	*consumer;	/* partition or application */
	LAYER_T		*l;
	APPL_T		*appl;

	assert((p != (PART_T *)0));
	debug_sched(5,"rollin_std part->inode %d\n",p->inode);

	/* see if we can change the partition status */
	if (p->cur_priority > NO_PRI)
		/* This partition has a positive priority value, this
		 * implies that there are applications to schedule in it,
		 * or in it's subpartitions
		*/
		p->status = PART_ROLLED_IN;

	for(l = p->child_sched_lyr; l != (LAYER_T *)0; l = l->next){
		for(consumer = l->consumer; consumer != (CONSUMER_T *)0;
			consumer = consumer->next){

			if (consumer->type == APPL){
				/*
			 	* Consumer is an application.
			 	*/
				debug_sched(5,"rollin_std application\n");
				appl = (APPL_T *) consumer;
				/* See if we have a new application */
				if (new_appl_ptr != (APPL_T *)0){
					/* Yes!, is this the new one? */
					if (new_appl_ptr == appl){
						/* Yes, the application is already
					 	* started and it is ok to let it run
						*/
						debug_sched(5,"New application will remain active\n");
						new_appl_ptr = (APPL_T *)0;
						appl->appl_stat = APPL_GANG_START;
					}
				}
			}
			else{
				/* should not get here */
			}
		}
	}
}

/*
 * select_next_layer() - Select next layer that should be rolled in and update
 *                       part->active_lyr.
 */
void
select_next_layer(part)
PART_T	*part;		/* Partition containing layers */
{
	LAYER_T	*layer,run_this;

	debug_sched(5,"select next layer part->inode %d\n",part->inode);

	assert((part != (PART_T *)0));

	if (part->sched_list == (LAYER_T *) 0) {
		/*
		 * No child layers, nothing to do. Set active_lyr to be safe.
		 */
		part->active_lyr = (LAYER_T *) 0;
		return;
	}

	layer = find_highest_pri(part);
	part->active_lyr = layer;
}

/*	Find the highest priority layer, starting with the layer
 *	that is next in the scheduling list. This routine will
 *	perform a round robin selection of the schedule list
 *	when looking for the highest priority layer.
 *
 *	Each partition contains a linked list of scheduling layers 
 *	denoted by sched_list. When a partition has a rollin quantum > 0
 *	the scheduler needs to perform a round robin selection among
 *	all layers with the same priority. 
 *
 *	Parameters:
 *		part	pointer to partition that owns the layers
 *	Returns:
 *		pointer to layer with highest priority.
 *
*/
LAYER_T *
find_highest_pri(PART_T *part)
{
	LAYER_T	*save,*p;
	int	max_pri;

	assert((part != (PART_T *)0));

	debug_sched(5,"find highest pri layer for part->inode %d \n",part->inode);
	max_pri = NO_PRI;

	if (part->sched_list == (LAYER_T *)0)
		return((LAYER_T *)0);

	save = p = (LAYER_T *)0;

	/* 2 cases to deal with here
	 * 
	 * Case 1: There is no active layer
 	 *		Start at the beginning of the schedule list
	 *
	 * Case 2: There is an active layer
	 *		Start at the next item on the 
	 *		schedule list
	 *
	 *
	 */

	/* Case 1 */ 
	if (part->active_lyr == (LAYER_T *)0){
		/* start at the beginning of the schedule list */
		p = part->sched_list;
		debug_sched(5,"No active layer starting from 0x%x\n",p);
		while (p != (LAYER_T *)0){
			if (p->priority > max_pri){
				/* this is the max, so record it */
				debug_sched(5,"layer 0x%x has pri %d\n",p,p->priority);
				save = p;
				max_pri = p->priority;
			} 
			/* Move on to next layer */
			p = p->sched_next;
		}
	}
	else{
		/* Case 2, there is an active layer */
		/* Start looking for a max starting at the
		 * next item in the active layers schedule
		 * list
		*/
		p = part->active_lyr->sched_next;
		debug_sched(5,"Active layer, starting from 0x%x\n",p);
		do{
			if (p == (LAYER_T *)0)
				/* if we are at the end of the 
				 * list, then start from the 
				 * beginning to handle the 
				 * wrap around
				*/
				p = part->sched_list;
			if (p->priority > max_pri){
				/* this is the max, so record it */
				debug_sched(5,"layer 0x%x has pri %d\n",p,p->priority);
				save = p;
				max_pri = p->priority;
			} 
			/* Move on to next layer */
			p = p->sched_next;
		/* Repeat loop until we come back to where we started from
		 */
		} while (p != part->active_lyr->sched_next);
	}

	/* Make sure the layer we found is valid */
	if ( (save != (LAYER_T *)0) && (save->priority == NO_PRI) )
		save = (LAYER_T *)0;

	debug_sched(5,"max_pri %d in layer 0x%x\n",save->priority,save);

	return(save);
}

/*
 * remove_application() -  Remove application from layer and hash table
 *                         following termination of application.
 *
 *	Parameters:
 *		appl	pointer to application data structure to remove
 */
void
remove_application(APPL_T *appl)
{
	LAYER_T	*p_layer;	/* Parent layer of this layer */
	LAYER_T	*layer;		/* Layer containing application */
	PART_T	*p;		/* partition which contains layer (and appl) */
	LAYER_T *layer_list;

	assert((appl != (APPL_T *)0));

	init_app_cnt--;

	/* Update the application times before calling SMD */
	debug_sched(5,"INCR_TIME appl %d intvl %d \n",appl->pgroup,interval);
	INCR_TIME(interval, appl->elapsed);
	INCR_TIME(interval, appl->rolled_in);

	/*
	 *  Notify the Scheduler Monitor Daemon of the applications demise.
	 */
	notify_smd(appl, APP_END);

	/*
	 * Return nodes used by application to the layer
	*/
	layer = appl->parent_lyr;
	assert((layer != (LAYER_T *)0));

	debug_sched(5,"remove_application: appl->pgroup = %d appl->size %d layer 0x%x num free %d\n", appl->pgroup,appl->size,layer,layer->num_free);
	debug_sched(5,"elapsed %d rolled_in %u\n",appl->elapsed, appl->rolled_in);

	p = layer->part;

	remove_consumer_from_tile(FALSE,SCHED_TILE,(CONSUMER_T *)appl,&(appl->parent_lyr),
				appl->bitmap,appl->size,
				&(p->child_sched_lyr));


	/* if the list of scheduling layers is now empty for the partition
	 * then we need to deactivate the partition
	*/
	if (p->child_sched_lyr == (LAYER_T *)0)
		perk_remove_active_partition(p);

	/*
	 * Remove application from hash table
	 */
	HASH_REMOVE_APPL(appl->pgroup);

	/* Remove application from all the conflict lists that it appears in */
	remove_from_all_conflict_lists((CONSUMER_T *)appl);

	/*
	 * Free memory used by application
	*/
	FREE((void *) appl->bitmap);
	FREE((void *) appl->lp);
	FREE((void *) appl);

}

/* Recursively remove a partition from its parent partition's scheduling layers 
 *
 *	Parameters:
 *		part	pointer to partition to remove	
*/
void
perk_remove_active_partition(PART_T *part)
{
	LAYER_T	*layer;
	PART_T  *parent_part;

	assert(part != (PART_T *)0);

	part->elapsed = 0;
	part->start_time = 0;
	debug_sched(5,"perk_remove_active_partition part->inode %d\n",part->inode);

	set_cur_part_pri(part);

	/* Remove application from all the conflict lists that it appears in */
	remove_from_all_conflict_lists((CONSUMER_T *)part);
	/* Reinitialize the partition's conflict list */
	init_conflict_list(&(part->conflict));

	/* find parent partition */
	if (part->parent_alloc_lyr)
		parent_part = part->parent_alloc_lyr->part;
	else
		return;

	/* See if we have a parent partition */
	if (parent_part != (PART_T *) 0){
		/* we have a parent, remove the partition from the layer in which
		 * it resides
		 */
		remove_consumer_from_tile(FALSE,SCHED_TILE,(CONSUMER_T *)part,&(part->parent_sched_lyr),
							part->bitmap,part->slots,
							&(parent_part->child_sched_lyr));

		/* if our parent no longer contains any scheduling layers then
		 * remove our parent from it's parent's scheduling layers
		*/
		if (parent_part->child_sched_lyr == (LAYER_T *)0)
			perk_remove_active_partition(parent_part);
		else
		if (part->parent_sched_lyr != (LAYER_T *)0)
			/* the layer in which our partition resided is not empty, we
			 * need to readjust the layer priorities and percolate the 
			 * results up
			*/
			perk_player_pri(part->parent_sched_lyr);
		else{
			/* our parent scheduling layer is empty and our parent still has
			 * schedulable objects. Recompute the parent's partition priority and
			 * pass those results back up
			*/
			set_cur_part_pri(parent_part);
			/* re-adjust the priority of scheduling layer that our parent resides
			 * in
			*/
			perk_player_pri(parent_part->parent_sched_lyr);
		}
	}
}

#ifdef JMC
/* Percolate the results of removing the active layer back up through
 * the list of active layers
*/
void
perk_remove_active(PART_T *part)
{
	LAYER_T	*layer;

	assert(part != (PART_T *)0);

	/* Root partition is the only partition whose parent layer
	 * should be null
	*/
	if (part->parent_sched_lyr != (LAYER_T *)0){
		/* There is a parent layer */
		layer = part->parent_sched_lyr;
		debug_sched(5,"perk_remove_active inode %d from layer 0x%x\n",part->inode,layer);
		/* see if this is the active layer */
		if ( (layer->part->active_lyr != (LAYER_T *)0) &&
			(layer == layer->part->active_lyr) ){
			/* This is the active layer, check to see if
			 * this is the only sublayer in the active layer
			*/
			debug_sched(5,"parent layer is also active\n");
			/* does this layer contain any more applications ? */
			if (has_appl(layer) == 0){
				debug_sched(5,"no applications in layer\n");
				/* reset the layer priority */
				layer->part->active_lyr->priority = NO_PRI;
				/* reset current priority for the partition */
				set_cur_part_pri(layer->part);
				/* reset the active layer pointer */
				layer->part->active_lyr = (LAYER_T *)0;	
				/* keep walking back up the chain */
				perk_remove_active(layer->part);
			}
			else{
				debug_sched(5,"layer has applications\n");
				/* reset this layer`s priority */
				/* percolate the results back up */
				perk_player_pri(layer);
			}
		}
	}
}
#endif JMC


/*
 * find_smallest_rollin_quantum() -  Return smallest rollin quantum.
 *
 *	Parameters:
 *		part	pointer to partition to search
 *		rq	smallest known rollin quantum
 *
 *	Returns:
 *		smallest rollin quantum
*/
unsigned long
find_smallest_rollin_quantum(part, rq)
PART_T		*part;	/* partition to search */
unsigned long	rq;	/* smallest known rollin quantum */
{
	CONSUMER_T	*consumer;	/* partition or application */
	
	assert((part != (PART_T *)0));
	if (part->active_lyr == (LAYER_T *) 0) {
		consumer = (CONSUMER_T *) 0;
	}
	else {
		consumer = part->active_lyr->consumer;
	}

	/*
	 * Check each partition in active layer.
	 */
	while (consumer !=  (CONSUMER_T *) 0) {
		if (consumer->type == PART) {

			/*
			 * Consumer is a partition recursively check
			 * rollin quantum.
			 */
			rq = find_smallest_rollin_quantum((PART_T *) consumer,
			                                  rq);
		}
		consumer = consumer->next;
	}

	/*
	 * Return smaller of what is left of our quantum, or the smallest of
         * what is left of the quantum of any of our subpartitions.
	 */
	if (part->rollin_quantum == 0) {
		return rq;
	}
	else {
		return MIN(rq, part->rollin_quantum - part->rolled_in);
	}
}

#ifdef JMC
/*
 * has_appl() -  Return 1 if layer has an application.
 */
static int
has_appl(layer)
LAYER_T		*layer;	/* Layer to search */
{
	CONSUMER_T	*consumer;	/* partition or application */

	assert(layer != (LAYER_T *)0);
	consumer = layer->consumer;
	if (consumer == (CONSUMER_T *) 0) {
		return 0;
	}

	/*
	 * Check each consumer.
	 */
	while (consumer !=  (CONSUMER_T *) 0) {
		if (consumer->type == APPL) {
			return 1;
		}
		else {
			/*
			 * Consumer is a partition recursively check
			 * each layer.
			 */
			PART_T	*part;
			LAYER_T	*l;

			part = (PART_T *) consumer;	
			l = part->child_sched_lyr;
			while (l != (LAYER_T *) 0) {
				if (has_appl(l)) {
					return 1;
				}
				l = l->next;
			}
		}
		
		consumer = consumer->next;
	}

	return 0;
}
#endif JMC

/*
 * set_layer_pri() - 
 *	set the priority of a layer based on the objects contained in it. The layer will
 *	assume the priority of the highest priority object.
 *
 *	Parameters:
 *		layer	pointer of layer to set priority in
 */
void 
set_layer_pri(LAYER_T *layer)
{
	CONSUMER_T	*consumer;	/* partition or application */
	APPL_T		*app;
	PART_T		*part;		/* Our partition */
	LAYER_T		*l;
	int		max_pri;	/* Maximum priority found so far */

	assert((layer != (LAYER_T *)0));

	debug_sched(5,"set layer pri in part->inode %d layer 0x%x\n",layer->part->inode,layer);
	/* Reset priority */
	layer->priority = NO_PRI;
	max_pri = NO_PRI;
	consumer = layer->consumer;

	/*
	 * Check each consumer.
	 */
	while (consumer !=  (CONSUMER_T *) 0) {
		if (consumer->type == APPL) {
			debug_sched(5,"consumer is an application \n");
			app = (APPL_T *) consumer;
			/* Application has a higher priority than we
			 *  have currently seen so far
			*/
			max_pri = MAX(app->priority,max_pri);
		}
		else {
			/* Check the partition priority against what we
			 * have seen so far 
			*/
			part =  (PART_T *)consumer;
			debug_sched(5,"consumer is a partition \n");
			max_pri = MAX(part->cur_priority,max_pri);
		}
		
		consumer = consumer->next;
	}

	/* Set the layer to the value we found */
	layer->priority = max_pri;

	debug_sched(5,"layer priority set to %d\n",max_pri);

}

/* Percolate the priority up through the parent layers
 *
 * 1. set p_layer->priority based on the objects in the layer
 * 2. set the partition priority that contains this layer
 * 3. if there is a parent layer then make a recursive call to this routine
 *
 *	Parameters:
 *		p_layer	= pointer to parent layer
*/ 
void
perk_player_pri(LAYER_T *p_layer)
{
	if (p_layer == (LAYER_T *)0)
		return;

	assert(p_layer->part != (PART_T *)0);

	debug_sched(5,"perk_player_pri layer 0x%x\n",p_layer);

	set_layer_pri(p_layer);

	debug_sched(5,"perk_player_pri: set p_layer %d\n",p_layer->priority);

	/* If our layer priority is not the same as the partition priority then
	 * recalc the partition priority and percolate the results
	*/
	if (p_layer->priority != p_layer->part->cur_priority){
		set_cur_part_pri(p_layer->part);
		if (p_layer->part->parent_sched_lyr != (LAYER_T *)0)
			/* keep percolating the priority back up */
			perk_player_pri(p_layer->part->parent_sched_lyr);
	}
}

/* Find the highest priority object in the partition. The partition's
 * current priority value will be the result then of taking the max
 * and filtering it through the EPL
 *
 *	Parameters:
 *		part	pointer to partition to set priority for
*/ 
void
set_cur_part_pri(PART_T *part)
{
	LAYER_T	*l;
	int	max;

	assert( (part != (PART_T *)0) );

	max = NO_PRI;

	debug_sched(5,"set_cur_part_pri for part->inode %d\n",part->inode);

	/* Reset the partition priority */
	part->cur_priority = NO_PRI;

	/* Scan all layers belonging to this partition, record the 
	 * maximum priority we see
	*/
	for(l = part->child_sched_lyr; l != (LAYER_T *)0; l = l->next)
		max = MAX(l->priority,max);		

	/* Pass the max priority seen so far through the partition's EPL 
	 * filter
	*/
	if (part->max_priority > max)
		/* No need to filter */
		part->cur_priority = max;
	else
	if (max == STD_PRI)
		/* We are dealing with a standard scheduled partition
		 * which has infinite priority
		*/ 
		part->cur_priority = max;
	else
		/* Need to filter */
		part->cur_priority = part->max_priority;

	if (part->cur_priority == NO_PRI){
		part->rolled_in = 0;
		part->status = PART_ROLLED_OUT;
	}

	debug_sched(5,"part->cur_priority to %d\n",part->cur_priority);

}

void
/* Register an application to be started */
register_gang_start(APPL_T *appl)
{
	assert((appl != (APPL_T *)0));
	assert(ri_cnt < MAX_RQ_LIST);
	rollin_list[ri_cnt++] = appl;
	debug_sched(5,"reg gstart %d %d\n",ri_cnt,rollin_list[ri_cnt-1]->pgroup);
}

/* Register an application to be stopped */
void
register_gang_stop(APPL_T *appl)
{
	assert((appl != (APPL_T *)0));
	assert(ro_cnt < MAX_RQ_LIST);
	/* XXX This is a hack */
	if (appl->appl_stat == APPL_GANG_START){
		rollout_list[ro_cnt++] = appl;
	debug_sched(5,"reg gstop %d %d\n",ro_cnt,rollout_list[ro_cnt-1]->pgroup);
	}
}

/*	Gang start and application 
*/
void
gang_start(APPL_T *appl)
{
	int	stat;
	double  start,stop;

	assert((appl != (APPL_T *)0));

	debug_signal(5,"gang_start-> %d\n",appl->pgroup);

#ifdef DO_SIG_PROF
	start = dclock();
        stat = kill(-appl->pgroup,SIGGSTART);
        stop = dclock();
        kill_time.elapsed += (stop - start);
        kill_time.cnt++;
#else 
        stat = kill(-appl->pgroup,SIGGSTART);
#endif DO_SIG_PROF
	
	debug_signal(5,"%d started\n",appl->pgroup);

	if (stat == -1){
		debug_print(DEBUG_OTHER, 0, "gang_start error on pid = %d\n",appl->pgroup);
		perror("error:");
		remove_application(appl);
	}
	else{
		notify_smd(appl, APP_ROLLIN);
		appl->appl_stat = APPL_GANG_START;
	}
}

/* Gang stop and application
*/
void
gang_stop(APPL_T *appl)
{
	int	stat;
	double  start,stop;

	assert((appl != (APPL_T *)0));
	debug_signal(5,"gang_stop-> %d\n",appl->pgroup);

#ifdef DO_SIG_PROF
        start = dclock();
        stat = kill(-appl->pgroup,SIGGSTOP);
        stop = dclock();
        kill_time.elapsed += (stop - start);
        kill_time.cnt++;
#else
        stat = kill(-appl->pgroup,SIGGSTOP);
#endif DO_SIG_PROF

	debug_signal(5,"%d stopped\n",appl->pgroup);

	if (stat == -1){
		debug_print(DEBUG_OTHER, 0, "gang_stop error on pid = %d\n",appl->pgroup);
		perror("error:");
		remove_application(appl);
	}
	else{
		notify_smd(appl, APP_ROLLOUT);
		appl->appl_stat = APPL_GANG_STOP;
	}
}

/*  Check to see if we need to call the scheduler
 *
 *	Parameters:
 *		part	pointer to partition to search
 *		reschedule	pointer to integer 
 *				FALSE if we do not need to call the scheduler
 *
 *	Returns:
 *		pointer to layer with highest priority
*/
LAYER_T *
check_for_reschedule(PART_T *part, int *reschedule)
{
	LAYER_T *layer;

	layer = (LAYER_T *)0;
	*reschedule = FALSE;

	/* scan layers to see if there is something of higher priority */
	layer = find_highest_pri(part);
	if ( part->active_lyr != (LAYER_T *)0 && layer != (LAYER_T *)0 )
		/* compare our priority to the active layer's */
		*reschedule = (layer->priority > part->active_lyr->priority);
	else
	/* there is no active layer */
	if (layer != (LAYER_T *)0)
		/* we found a layer that has a priority and is not the active layer */
		*reschedule = TRUE;
	else
		*reschedule = FALSE;

#if 0

	if (!*reschedule){
		/* See if our layer is full */
		if (part->active_lyr != (LAYER_T *)0)
			/* check the active layer first */
			*reschedule = (part->active_lyr->num_free != 0);
		else 
		if (layer != (LAYER_T *)0)
			/* no active layer, so check the layer we found */
			*reschedule = (layer->num_free != 0);
		debug_sched(5,"reschedule check layer not full %d\n",*reschedule);
	}
	else
		debug_sched(5,"reschedule %d\n",*reschedule);
#endif
	return(layer);
}
