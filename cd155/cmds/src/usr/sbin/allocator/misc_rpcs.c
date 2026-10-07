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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/misc_rpcs.c,v 1.26 1995/03/07 00:57:37 carolr Exp $
 *
 */

/* History:
 *	$Log: misc_rpcs.c,v $
 * Revision 1.26  1995/03/07  00:57:37  carolr
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
 * Revision 1.25  1994/12/19  15:55:41  johannes
 * New function nx_appl_dumping_core() sets the new field appl_core in the
 * application structure to APPL_DUMPING_CORE.
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
 * Revision 1.24  1994/11/19  03:04:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.23  1994/07/06  17:33:03  mag
 * Correct -rlx on SPS partition
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 9954
 *  Testing: developer
 *  Module(s): misc_rpcs.c allocutils.c allocutils.h mkpart_rpc.c
 *
 * Revision 1.22  1994/06/29  17:17:44  johannes
 * new function get_nx_app_part() to get the complete information
 * needed for allocinfo file
 *
 *  Initial check-in of parallel core dumping
 *  Reviewer: stefan, jlitvin
 *  Risk: Medium
 *  Benefit or PTS #: OS support for Postmortem Debugging
 *  Testing: developer tests
 *  Module(s):
 * 	svr/server/conf/MASTER
 * 	svr/server/conf/MASTER.i860
 * 	svr/server/conf/files.i860
 * 	svr/server/paracore/core_types.h
 * 	svr/server/paracore/allocinfo.c
 * 	svr/server/paracore/core.c
 * 	svr/server/paracore/dump.c
 * 	svr/server/paracore/dvp_pvpcore.c
 * 	svr/server/sys/allocinfo.h
 * 	svr/server/sys/core.h
 * 	svr/server/sys/user.h
 * 	svr/server/nx/nx.defs
 * 	svr/server/nx/nx.c
 * 	svr/server/bsd/kern_exit.c
 * 	svr/server/bsd/kern_fork.c
 * 	svr/server/bsd/kern_sig.c
 * 	svr/server/tnc/dpvproc.h
 * 	svr/server/tnc/dvp_init.c
 * 	svr/server/tnc/dvp_pvpops.c
 * 	svr/server/tnc/pvp.ops
 * 	svr/server/uxkern/fsvr_msg.c
 * 	cmds_libs/src/usr/sbin/allocator/alloc.defs
 * 	cmds_libs/src/usr/sbin/allocator/misc_rpcs.c
 * 	cmds_libs/src/usr/sbin/allocator/Makefile
 * 	cmds_libs/src/usr/include/README.locate
 * 	cmds_libs/src/usr/include/sys/Makefile
 *
 * Revision 1.21  1994/06/13  22:46:33  sdh
 * Changed debug messages to go through debug print routine.
 *
 *  Reviewer: mag
 *  Risk: low
 *  Benefit or PTS #:
 *  Testing: EATS
 *  Module(s):
 *         cmds_libs/src/usr/sbin/allocator/tiles.c
 *         cmds_libs/src/usr/sbin/allocator/allocator.c
 *         cmds_libs/src/usr/sbin/allocator/allocutils.c
 *         cmds_libs/src/usr/sbin/allocator/conflict.c
 *         cmds_libs/src/usr/sbin/allocator/init_appl.c
 *         cmds_libs/src/usr/sbin/allocator/misc_rpcs.c
 *         cmds_libs/src/usr/sbin/allocator/mkpart_rpc.c
 *         cmds_libs/src/usr/sbin/allocator/rmpart_rpc.c
 *         cmds_libs/src/usr/sbin/allocator/schedule.c
 *         cmds_libs/src/usr/sbin/allocator/server_loop.c
 *         cmds_libs/src/usr/sbin/allocator/smd.c
 *         cmds_libs/src/usr/sbin/allocator/tiles.c
 *
 * Added set_all_children() and clear_all_children() to set and clear the
 * gang_ancestor variable for each child in the case of a chpart that changes
 * the partition from sps to rq or vice versa.
 *
 *  Reviewer: carbajal
 *  Risk: low
 *  Benefit or PTS #: 8464
 *  Testing: EATS, manual testing
 *  Module(s):
 *         cmds_libs/src/usr/sbin/allocator/misc_rpcs.c
 *
 * Revision 1.20  1994/06/02  18:13:45  mag
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
 * Revision 1.19  1994/01/12  18:34:08  carbajal
 *  6393 PARAGON      OPEN      M        carbajal  davidl              R1.1 T12.1
 *       MESH UTILS   24-SEP-93 **       25-AUG-93 25-AUG-93
 *       chpart -g requires write permission; ownership should suffice
 * Benefit: Proper functionality
 * Risk: Low
 * Eng: Carbajal Reviewer:
 * Main Issue: when changing group ids the allocator was erroneously requiring
 * write permission on the partition. All you need to do is be the owner.
 *
 * 6392 PARAGON      OPEN      M        carbajal  davidl              R1.1 T12.1
 *       MESH UTILS   24-SEP-93 **       25-AUG-93 25-AUG-93
 *       chpart -g <samegroup> always fails with "Invalid group id"
 * Benefit: Can use chpart to set group id of a partition
 * Risk: Low
 * Eng: Carbajal Reviewer:
 * Main Issue: The check for a valid group was being done by the allocator. The
 * call getgroup() uses the calling processes gid to get the group list. Well
 * the allocator gid is not the same as the calling processes group id. The
 * change was to make this call from the user process before calling the
 * allocator.
 *
 * Revision 1.15.2.3  1994/01/12  18:05:39  carbajal
 *  6392 PARAGON      OPEN      M        carbajal  davidl              R1.1 T12.1
 *       MESH UTILS   24-SEP-93 **       25-AUG-93 25-AUG-93
 *       chpart -g <samegroup> always fails with "Invalid group id"
 * Benefit: Can use chpart to set group id of a partition
 * Risk: Low
 * Eng: Carbajal Reviewer:
 * Main Issue: The check for a valid group was being done by the allocator. The
 * call getgroup() uses the calling processes gid to get the group list. Well
 * the allocator gid is not the same as the calling processes group id. The
 * change was to make this call from the user process before calling the
 * allocator.
 *
 *  7711 PARAGON      OPEN      M        carbajal  gregt               R1.2 WW50
 *       MESH UTILS   06-JAN-94 **       06-JAN-94 06-JAN-94
 *       Inconsistent definitions of what is a "free" node in doc and
 *       implementation.
 * Benefit: Easy to understand and matches documentation
 * Risk: Low
 * Eng: Carbajal Reviewer:
 *
 * Main Issue: In computing the free space both the allocation and scheduling
 * layers were considered. While this is the right thing to do for most of the
 * R1.1 user model, it was deemed too confusing under R1.2. Instead free nodes
 * will be just those nodes that do not have applications on them.
 *
 * PARARAGON      OPEN      M        carbajal  davidl              R1.1 T12.1
 *       MESH UTILS   24-SEP-93 **       25-AUG-93 25-AUG-93
 *       chpart -g requires write permission; ownership should suffice
 * Benefit: Proper functionality
 * Risk: Low
 * Eng: Carbajal Reviewer:
 * Main Issue: when changing group ids the allocator was erroneously requiring
 * write permission on the partition. All you need to do is be the owner.
 *
 * Revision 1.15.2.2  1993/12/29  17:44:15  carbajal
 * Change some type castings, compute the enclosing rectangle for
 * nx_part_attr() call, set the real rectangle.
 *  Reviewer: John Litvin
 *  Risk: Low
 *  Benefit or PTS #: 7301,7302
 *  Testing: Bug report
 *  Module(s): misc_rpcs.c
 *
 * Revision 1.15.2.1  1993/12/20  21:52:09  carbajal
 * Be careful to vm_dealloc() memory properly. Use macros FREE and
 * MALLOC to make sure all memory allocated is properly deallocated.
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7257
 *  Testing: SATs, EATs, MUNOPS, bug test
 *  Module(s): misc_rpcs.c, mkpart_rpc.c, schedule.c, rmpart_rpc.c
 *
 * Revision 1.15  1993/12/01  01:34:46  carbajal
 * Use new error codes, EEXCEEDCONF, ESCHEDCONF. Use NX_ALL_SPACE etc
 * in getting free nodes
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.14  1993/11/22  17:53:29  carbajal
 * Allow specification of EPL on SPS partitions
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: As per R1.2 Usage Model
 *  Testing: EATs
 *  Module(s):
 *
 * Revision 1.13  1993/11/18  20:23:55  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of the export tree
 *  Testing: built on Suns and 486
 *  Module(s): scripts.mk standard.mk
 *
 * Revision 1.12  1993/11/17  06:55:11  carbajal
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: More R1.2 User Model Changes
 *  Testing:
 *  Module(s):
 *
 * Revision 1.11  1993/11/17  04:33:31  carbajal
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Corrected part_attributes() call
 *  Testing:
 *  Module(s):
 *
 * Revision 1.10  1993/11/17  02:54:31  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.9  1993/10/27  01:59:47  carbajal
 * Added get_app_node_list()
 *
 * Revision 1.8  1993/08/28  01:17:14  carbajal
 * Added register_daemon_allocator() for PTS #6141
 *
 * Revision 1.7  1993/08/23  17:18:32  carbajal
 * Save partinfo.flag values in chpart() before calling
 * build_partinfo(). PTS #6130.
 *
 * Revision 1.6  1993/07/29  20:42:43  carbajal
 * In a chpart -o request if the uid is not root then return
 * EACESS instead of EPACCESS
 *
 * Revision 1.5  1993/07/20  18:10:53  carbajal
 * Added double dlock();
 *
 * Revision 1.4  1993/07/18  19:47:54  carbajal
 * New chpart() system call support
 *
 * Revision 1.3  1993/07/13  22:18:32  carbajal
 * Added new routines for allocator system call support
 *
 * Revision 1.2  1993/07/07  22:04:40  carbajal
 * Changed error code from EACCES to EPINVALSCHED when the user has
 * specified an rq for a std scheduled partition.
 *
 * Revision 1.1  1993/05/25  23:53:25  carbajal
 * New module with RPC specific code pulled out of allocator.c
 *
*/
#include <sys/dir.h>
#include <signal.h>
#include <stdio.h>
#include <mach/mach.h>
#include <mach_error.h>
#include <mach/mig_errors.h>
#include <mach/message.h>
#include <mach/norma_special_ports.h>
#include <mach/mach_host.h>
#include <errno.h>
#include <assert.h>
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
#include <nx/alloc_types.h>

#include <allocsys.h>

double	dclock();
/* Forward reference */
int rename_partition(char *from,char *to);

extern PART_T	*root;
extern locktype alloc_lock;
extern int sched_debug,signal_debug,num_gang_parts;
extern HASH_TBL_T       appl_tbl[];
extern HASH_TBL_T      part_tbl[];
extern alloc_config_t	alloc_config;
extern BITMAP_T		*inverted_root;
extern BITMAP_T        *unusable_node_map;


extern int build_partinfo(ino_t inode, PARTREQ_T *partinfo);
extern smd_init(mach_port_t server_port, u_char *socket_addr, int socket_len, int *ret_code);
extern macs_init(mach_port_t server_port, u_char *socket_addr, int socket_len, int *ret_code);

void clear_all_children(PART_T *part);
void set_all_children(PART_T *part);


/* Entry point into allocator for chpart command 
 *
 *	PARAMETERS:
 *		serv_port
 *		partinfo= partition request record
 *			  containing items to change
 *			  the flags field will be set
 *			  to indicate what items to change
 *		uid	= user id
 *		gid	= group id
 *		error   = pointer to error condition returned by allocator
 *
 *	RETURNS:
 *		always returns MACH_KERN_SUCCESS so you need
 *		to check error.
 *
*/

kern_return_t
chpart(serv_port, buffer, buf_len, uid, gid, error)
mach_port_t     serv_port;      /* Server port */
char		*buffer;
int		buf_len;
uid_t           uid;            /* UID of calling process */
gid_t           gid;            /* GID of calling process */
int             *error;         /* Errno */
{
        PART_T   	*part;         /* Partition to change */
	int		req_access;	/* Needed permission to partition */
	char		*pathname,*new_pathname;
	PARTREQ_T	partinfo;
	int		flags;


	if (unmarshall_chpart_rpc(buf_len,buffer,&pathname,&new_pathname,&partinfo) != 0)
		goto chpart_error;

	flags = partinfo.flags;
	CSECT_ENTER

	/* if we are changing the mode then we must be the owner of the
	 * partition
	*/
	if (partinfo.flags == MODFLG)
		req_access = OWNER;
	else
	if ( (partinfo.flags & OWNERFLG) && (partinfo.gid != -1) ) {
		/* They want to change the owner, the need to be
		 * the owner of the partition
		*/
		req_access = OWNER;
	}
	else
		req_access = WRITE;
	if ( (partinfo.flags & NMFLG) != 0)
		req_access |= CHECK_AGAINST_PARENT;

	*error = 0;

	/* make sure we have access to the partition */ 
	if ( (part = 
		validate_allocator_access(partinfo.parent_inode,partinfo.inode,
				uid,gid,req_access,error)) == 
				(PART_T *)0)
		goto chpart_error;

	if ( partinfo.flags & NMFLG){
		/* Make sure it is ok to rename */
		if ( get_inode(new_pathname) > 0){
			*error = EPPARTEXIST;
			goto chpart_error;
		}
	}

	/* If we are changing the owner then we must be root */
	if ( (partinfo.flags & OWNERFLG) && (partinfo.uid != -1) ) {
			/* we need to be root for this one */
			if (uid != 0){
				*error = EACCES;
				goto chpart_error;
			}
	}

	/* Make sure that we don't change the rollin quantum of 
	 * a standard scheduled partition
	*/
	if ( (part->sched  == UNIX) && 
		( ((partinfo.flags & RQFLG) == RQFLG) ||
		  ((partinfo.flags & SPSFLG) == SPSFLG) )) {
		*error = EPINVALSCHED;
		goto chpart_error;
	}

	if ( (part->sched == SPACE_SHARE) )
		/* We are a space shared partition, see if
		 * they want to change it to a GANG partition.
		*/
		if (partinfo.flags & RQFLG){
			/* yes, see if we would exceed the allocator
			 * configuration file for number of gang 
			 * scheduled partitions
			*/
			if ( (alloc_config.num_of_gang_parts > 0) &&
				(num_gang_parts == 
					alloc_config.num_of_gang_parts) ){
				*error = EEXCEEDCONF;
				goto chpart_error;
			}
		}

	/* Check for a valid RQ */
	if ( (partinfo.flags & RQFLG) == RQFLG){
		if (alloc_config.min_rq > 0)
			/* The config file has set a minimum */
			if ( (partinfo.rq != 0))
				if (partinfo.rq < alloc_config.min_rq){
					/* we are going below the min */
					*error = ESCHEDCONF; 
					goto chpart_error;
				}
	}

	/* At this point the values passed are ok so we can
	 * go ahead and update the partition record 
	*/

	/* See if we need to change from gang schedule to space 
	 * share
	*/
	if ( ((partinfo.flags & SPSFLG) == SPSFLG) &&
		(part->sched == GANG) ) {
		/* we want to go to space sharing mode, we can only
		 * do this if there are no overlapping sub partitions
		 * and no applications running.
		*/
		if (part->child_alloc_lyr != (LAYER_T *)0 &&
			part->child_alloc_lyr->next != (LAYER_T *)0){
				/* XXXX */
				*error = EPINVALSCHED; 
				goto chpart_error;
		}
		/* If there are any applications running in this partition
		 * then don`t let them change the characteristics
		*/
		else
		if (part->child_sched_lyr != (LAYER_T *)0){
			/* XXX */
			*error = EPINVALSCHED; 
			goto chpart_error;
		}
	}

	if ( ((partinfo.flags & SPSFLG) == SPSFLG)){
		part->rollin_quantum = 0;
		if (part->sched == GANG)
			num_gang_parts--;
		part->sched = SPACE_SHARE;
		/*
		 * Pick up gang_ancester from parent. This way if any
		 * partitions above this are gang scheduled then this
		 * is set, otherwise, it now gets cleared.
		 */
		part->gang_ancestor = part->parent_alloc_lyr->part->gang_ancestor;
		/*
		 * We now need to update the gang ancestor field of
		 * each of the children. We only need to do this if
		 * the current field is being cleared.
		 */
		if (!part->gang_ancestor) {
		    /*
		     * Clear children
		     */
		    debug_allocation(5, "Clearing gang_ancestor of children of 0x%x\n", part);
		    clear_all_children(part);
		}
	    }
	

	/* see if we need to change the rollin quantum */
	if ( (partinfo.flags & RQFLG) == RQFLG){
		/* We are changing from a space sharing partition to
		 * a gang scheduling partition 
		*/
		if (part->sched == SPACE_SHARE){
			part->sched = GANG;
			if (!part->gang_ancestor) {
			    /*
			     * Set the gang_ancestor field to true in this
			     * partition and all the children.
			     */
			    set_all_children(part);
			} else {
			    /*
			     * Since gang_ancestor was already set, we'll
			     * assume that all the children are already set
			     * correctly.
			     */
			    debug_allocation(5,"gang ancestor is already true.\n");
			}
			num_gang_parts++;
		}
		part->rollin_quantum = partinfo.rq;
	}

	/* see if we need to change the effective priority level */
	if ( (partinfo.flags & EPLFLG) == EPLFLG){
		part->max_priority = partinfo.maxpri;
	}

	/* see if we need to change the access mode */
	if ( (partinfo.flags & MODFLG) == MODFLG){
		part->protection = partinfo.access;
	}

	/* see if we need to change the owner */
	if ( (partinfo.flags & OWNERFLG) == OWNERFLG){
		if (partinfo.uid != -1)
			part->owner = partinfo.uid;
		if (partinfo.gid != -1)
			part->group = partinfo.gid;
	}

	if (partinfo.flags & (SPSFLG | OWNERFLG | MODFLG | EPLFLG | RQFLG)){ 
		/* at this point partinfo.flags is zeroed */
		build_partinfo(part->inode,&partinfo);
		if ( write_partinfo(pathname,&partinfo,part->bitmap,part->lp) == FALSE)
				if (rmdir(pathname) != 0){
					*error = errno;
					goto chpart_error;
				}
	}

	if (flags & NMFLG)
		if ( (*error = rename_partition(pathname,new_pathname)) != 0)
			goto chpart_error;

	if ( (flags & EPLFLG) == EPLFLG){
		/* if we change the epl then we need to readjust 
		 * the priorities of the layers above us
		*/
		debug_sched(5,"changing partition EPL part->inode %d EPL = %d\n",part->inode,part->max_priority);
		set_cur_part_pri(part);
		/* Record the effect of changed the priority filter EPL */
		perk_player_pri(part->parent_sched_lyr);
	}

chpart_error:
	CSECT_EXIT
        if (pathname != NULL)
                FREE((void *)pathname);
        vm_deallocate(mach_task_self(),(vm_address_t ) buffer,
                (vm_size_t) buf_len);

        return 0;

}

kern_return_t
lock_part(serv_port,lockval,error)
mach_port_t	serv_port;
int		*lockval;	/* TRUE if we have the lock FALSE otherwise */
int		*error;
{
	unsigned long	now;

#ifdef DEBUG
printf("lock part\n");
#endif DEBUG
	if (alloc_lock.locked == TRUE){
		/* allocator is locked. Check the elapsed time */
		now = GET_TIME_IN_MILLISECONDS();
#ifdef DEBUG
printf("lock held for %d\n",now - alloc_lock.last);
#endif DEBUG
		if ( (now - alloc_lock.last) > LOCKED_TOO_LONG){
			/* we have been locked too long. Go ahead
			 * and give them the lock
			*/
			SET_LOCK(alloc_lock);	
			*lockval = TRUE;
		}
		else
			*lockval = FALSE;
	}
	else{
#ifdef DEBUG
printf("got the lock\n");
#endif DEBUG
		SET_LOCK(alloc_lock);
		*lockval = TRUE;
	}

	*error = 0;
	return(KERN_SUCCESS);

}

kern_return_t
unlock_part(serv_port,error)
mach_port_t	serv_port;
int		*error;
{
#ifdef DEBUG
printf("unlock part\n");
#endif DEBUG
	INIT_LOCK(alloc_lock);
	*error = 0;
	return(KERN_SUCCESS);
}

/*	Allocator entry point for server notification of a process group leader
 *	exiting.
 *
 *	Parameters:
 *		serv_port
 *		pgroup		integer indicating process group leader id
 *		error		pointer to integer for return error code
 *
 *	Returns:
 *		KERN_SUCCESS
*/
kern_return_t
remove_nx_appl(serv_port,pgroup,error)
mach_port_t	serv_port;
pid_t		pgroup;
int		*error;
{
	APPL_T	*app;

	*error = 0;

	debug_sched(3, "remove_nx_appl() %d\n\n",pgroup);
	if ( (app = HASH_LOOKUP_APPL(pgroup)) == (APPL_T *) 0)
		*error = EANOEXIST; /* Application does not exist for process group */
	else{
		debug_sched(5,"call remove_application\n");
		remove_application(app);
	}

	return(KERN_SUCCESS);
}

#ifdef PARACORE
/*	Allocator entry point for server notification of a application,
 *	identified by the process group leader, dumping core.
 *
 *	Parameters:
 *		serv_port
 *		pgroup		integer indicating process group leader id
 *		error		pointer to integer for return error code
 *
 *	Returns:
 *		KERN_SUCCESS
*/
kern_return_t
nx_appl_dumping_core(serv_port,pgroup,error)
mach_port_t	serv_port;
pid_t		pgroup;
int		*error;
{
	APPL_T	*app;

	*error = 0;

	debug_sched(3, "nx_appl_dumping_core() %d\n\n",pgroup);
	if ( (app = HASH_LOOKUP_APPL(pgroup)) == (APPL_T *) 0)
		*error = EANOEXIST; /* Application does not exist for process group */
	else{
		debug_sched(5,"set appl_core: APPL_DUMPING_CORE\n");
		app->appl_core = APPL_DUMPING_CORE;
	}

	return(KERN_SUCCESS);
}
#endif /* PARACORE */

/*	Allocator entry point for setting an application's priority 
 *
 *	Parameters:
 *		serv_port
 *		pgroup		integer indicating process group leader id
 *		pri		integer indicating priority
 *		error		pointer to an integer for return error code
 *
 *	Returns:
 *		KERN_SUCCESS
*/
kern_return_t
set_nx_pri(serv_port,pgroup,pri,error)
mach_port_t	serv_port;
int		pgroup;
int		pri;
int		*error;
{

	APPL_T	*app;
	LAYER_T *layer;

	*error = 0;
	debug_sched(5,"set_nx_pri() %d %d\n",pgroup,pri);

	if ( (app = HASH_LOOKUP_APPL(pgroup)) == (APPL_T *) 0)
		*error = EANOEXIST; /* Application does not exist for process group */
	else{
		app->priority = pri;
		debug_sched(5,"app->parent_lyr->priority %d pri %d\n",app->parent_lyr->priority, pri);
		layer = app->parent_lyr;	
		perk_player_pri(layer);
	}

	return(KERN_SUCCESS);
}

/* 	Get partition attributes
 *
 *	Entry point for Server to Allocator RPC
 *
*/
kern_return_t
get_part_attributes(serv_port,inode,uid,gid,partreq,retcode)
mach_port_t	serv_port;
ino_t		inode;
uid_t		uid;
gid_t		gid;
PARTREQ_T	*partreq;
int		*retcode;
{
	PART_T	*part,*parent_part;
	LAYER_T *parent_layer;
        int     encl_col,encl_row,encl_w,encl_h;


	/* make sure we have access to the partition */ 
	if ( (part = validate_allocator_access(inode,inode,uid,gid,READ,retcode)) 
		!= (PART_T *)0){
		partreq->uid	= part->owner;
		partreq->gid	= part->group;
		partreq->access	= part->protection;
		partreq->sched	= part->sched;
		partreq->rq	= part->rollin_quantum;
		partreq->maxpri	= part->max_priority;
		partreq->inode	= inode;
		/* See if we have a parent */
		partreq->parent_inode = 0;
		if (part->parent_alloc_lyr != (LAYER_T *)0){
			/* we have a parent layer */
			parent_layer = part->parent_alloc_lyr;
			if (parent_layer->part != (PART_T *)0){
				/* Our parent layer has a partition */
				parent_part  = parent_layer->part;
				partreq->parent_inode = parent_part->inode;
			}
		}
		partreq->slots	= part->slots;
		partreq->nodes	= part->nodes;
                /* YECH YECH YECH
                 * These values are overloaded and should be
                 * added into the PARTREQ_T structure in their
                 * own right. But for now they represent the
                 * enclosing rectangular values
                */
                if (bitmap_encl_rect(part->bitmap,
					&encl_col,&encl_row,
					&encl_w,&encl_h)){
			partreq->flags     = encl_w;
			partreq->free      = encl_h;
                }
                partreq->rect.cols = part->cols;
                partreq->rect.rows = part->rows;
		*retcode = 0;
	}
	else
		*retcode = EPACCES;

done:
	return(KERN_SUCCESS);
}

/* 	Get partition node list
 *
 *	Entry point for Server to Allocator RPC
 *
*/
kern_return_t
get_part_node_list(serv_port,inode,uid,gid,nd_list,ndlistCnt,retcode)
mach_port_t	serv_port;
ino_t		inode;
uid_t		uid;
gid_t		gid;
LP_MAP_T	*nd_list;
int		*ndlistCnt;
int		*retcode;
{
	PART_T	*parent_part,*part;
	int	i;

	*nd_list = (LP_MAP_T )0;
	*ndlistCnt = 0;

	/* make sure we have access to the partition */ 
	if ( (part = validate_allocator_access(inode,inode,uid,gid,READ,retcode)) 
		!= (PART_T *)0){

		/* Now we allocate our node list to be passed back
		*/
		*retcode = (int) vm_allocate(mach_task_self(),
		  (vm_address_t*) nd_list,
		  (vm_size_t) (part->slots * sizeof(LP_MAP_ENTRY_T)),
		  (boolean_t) 1);

		if ( *retcode != KERN_SUCCESS){
			/* We don't have any memory so quit */
			*retcode = ENOMEM;
			goto done;
		}

		*ndlistCnt = part->slots;

		bcopy(part->lp,*nd_list,part->slots * sizeof(LP_MAP_ENTRY_T));

		*retcode = 0;
	}
	else
		*retcode = EPACCES;

done:
	return(KERN_SUCCESS);
}

#if 1
/* 	get application node list 
 *	Entry point for Server to Allocator RPC
 *
*/
kern_return_t
get_app_node_list(serv_port,pgroup,uid,gid,nd_list,ndlistCnt,retcode)
mach_port_t	serv_port;
uid_t		pgroup;
uid_t		uid;
gid_t		gid;
LP_MAP_T	*nd_list;
int		*ndlistCnt;
int		*retcode;
{
	int	i;
	APPL_T	*appl;
	PART_T	*part;

	*ndlistCnt = 0;
	*nd_list = (LP_MAP_T)0;

	if ( (appl = HASH_LOOKUP_APPL(pgroup)) == (APPL_T *) 0) {
		*retcode = EANOEXIST; /* Application does not exists 
				     * for process group 
				    */
		goto done;
	}

	part = appl->parent_lyr->part;
	/* make sure we have access to the partition */ 
	if ( (part = validate_allocator_access(
		part->inode,part->inode,uid,gid,READ,retcode)) 
		!= (PART_T *)0){

		/* Now we allocate our node list to be passed back
		*/
		*retcode = (int) vm_allocate(mach_task_self(),
		  (vm_address_t*) nd_list,
		  (vm_size_t) (appl->size * sizeof(LP_MAP_ENTRY_T)),
		  (boolean_t) 1);

		if ( *retcode != KERN_SUCCESS){
			/* We don't have any memory so quit */
			*retcode = ENOMEM;
			goto done;
		}
		*ndlistCnt = appl->size;

		bcopy(appl->lp,*nd_list,(*ndlistCnt) * sizeof(LP_MAP_ENTRY_T));

		*retcode = 0;
	}
	else
		*retcode = EPACCES;

done:
	return(KERN_SUCCESS);
}

/* 	get application information
 *	Entry point for Server to Allocator RPC
 *
*/
kern_return_t
get_nx_app_info(serv_port,pgroup,uid,gid,app_attr,retcode)
mach_port_t	serv_port;
pid_t		pgroup;
uid_t		uid;
gid_t		gid;
nx_app_info_t	*app_attr;
int		*retcode;
{
	int	i;
	APPL_T	*appl;
	PART_T	*part;

	if ( (appl = HASH_LOOKUP_APPL(pgroup)) == (APPL_T *) 0) {
		*retcode = EANOEXIST; /* Application does not exists 
				     * for process group 
				    */
		goto done;
	}

	part = appl->parent_lyr->part;
	/* make sure we have access to the partition */ 
	if ( (part = validate_allocator_access(
		part->inode,part->inode,uid,gid,READ,retcode)) 
		!= (PART_T *)0){

		app_attr->size = appl->size;
		app_attr->nrows = appl->nrows;
		app_attr->ncols = appl->ncols;
		app_attr->priority = appl->priority;
		app_attr->rolled_in = appl->rolled_in;
		app_attr->elapsed = appl->elapsed;
		app_attr->uid	= appl->uid;
		app_attr->nx_acctid = appl->nx_acctid;
		app_attr->start_time = appl->start_time;
		*retcode = 0;
	}
	else
		*retcode = EPACCES;

done:
	return(KERN_SUCCESS);
}

#ifdef PARACORE
/* 	get partition in which application runs
 *	Entry point for Server to Allocator RPC
 *
*/
kern_return_t
get_nx_app_part(serv_port,pgroup,uid,gid,app_attr,partreq,retcode)
mach_port_t	serv_port;
pid_t		pgroup;
uid_t		uid;
gid_t		gid;
nx_app_info_t	*app_attr;
PARTREQ_T	*partreq;
int		*retcode;
{
	int	i;
	APPL_T	*appl;
	PART_T	*part,*parent_part;
	LAYER_T *parent_layer;
        int     encl_col,encl_row,encl_w,encl_h;

	if ( (appl = HASH_LOOKUP_APPL(pgroup)) == (APPL_T *) 0) {
		*retcode = EANOEXIST; /* Application does not exists 
				     * for process group 
				    */
		goto done;
	}

	part = appl->parent_lyr->part;
	/* make sure we have access to the partition */ 
	if ( (part = validate_allocator_access(
		part->inode,part->inode,uid,gid,READ,retcode)) 
		!= (PART_T *)0){
		
		/* following copied from get_nx_app_info() */
		app_attr->size = appl->size;
		app_attr->nrows = appl->nrows;
		app_attr->ncols = appl->ncols;
		app_attr->priority = appl->priority;
		app_attr->rolled_in = appl->rolled_in;
		app_attr->elapsed = appl->elapsed;
		app_attr->uid	= appl->uid;
		app_attr->nx_acctid = appl->nx_acctid;
		app_attr->start_time = appl->start_time;
		
		/* following copied from get_part_attributes() */
		partreq->uid	= part->owner;
		partreq->gid	= part->group;
		partreq->access	= part->protection;
		partreq->sched	= part->sched;
		partreq->rq	= part->rollin_quantum;
		partreq->maxpri	= part->max_priority;
		partreq->inode	= part->inode;
		/* See if we have a parent */
		partreq->parent_inode = 0;
		if (part->parent_alloc_lyr != (LAYER_T *)0){
			/* we have a parent layer */
			parent_layer = part->parent_alloc_lyr;
			if (parent_layer->part != (PART_T *)0){
				/* Our parent layer has a partition */
				parent_part  = parent_layer->part;
				partreq->parent_inode = parent_part->inode;
			}
		}
		partreq->slots	= part->slots;
		partreq->nodes	= part->nodes;
                /* YECH YECH YECH
                 * These values are overloaded and should be
                 * added into the PARTREQ_T structure in their
                 * own right. But for now they represent the
                 * enclosing rectangular values
                */
                if (bitmap_encl_rect(part->bitmap,
					&encl_col,&encl_row,
					&encl_w,&encl_h)){
			partreq->flags     = encl_w;
			partreq->free      = encl_h;
                }
                partreq->rect.cols = part->cols;
                partreq->rect.rows = part->rows;

		*retcode = 0;
	}
	else
		*retcode = EPACCES;

done:
	return(KERN_SUCCESS);
}
#endif /* PARACORE */

kern_return_t
get_part_free_nodes(serv_port,inode,uid,gid,bitmap,bitmapCnt,retcode)
mach_port_t	serv_port;
ino_t		inode;
uid_t		uid;
gid_t		gid;
BITMAP_T 	**bitmap;
int		*bitmapCnt;
int		*retcode;
{
	PART_T	*part;

        *bitmapCnt = 0;
        *bitmap = (BITMAP_T *)0;

	/* make sure we have access to the partition */ 
	if ( (part = validate_allocator_access(inode,inode,
			uid,gid,READ,retcode)) != (PART_T *)0){
		*retcode = (int) vm_allocate(mach_task_self(),
		  (vm_address_t*) bitmap,
		  (vm_size_t) bitmap_size(part->bitmap), (boolean_t) 1);

		if ( *retcode != KERN_SUCCESS){
			/* We don't have any memory so quit */
			*retcode = ENOMEM;
			goto done;
		}

		*bitmapCnt 	= bitmap_size(part->bitmap);
		(*bitmap)->rows = part->bitmap->rows;
		(*bitmap)->cols = part->bitmap->cols;
		(*bitmap)->row_offset = part->bitmap->row_offset;

		build_free_bitmap(part, *bitmap);
		*retcode = 0;
	}
	else
		*retcode = EPACCES;

done:
	return(KERN_SUCCESS);
}


build_free_bitmap(part, bitmap)
PART_T*		part;
BITMAP_T*	bitmap;
{
	int		i;
	BITMAP_T	*free_image,*alloc_image,*sched_image;
	int		map_type;

	map_type = NX_APP_SPACE;

	alloc_image = (BITMAP_T *)0;
	sched_image = (BITMAP_T *)0;
	if (part->sched != UNIX){
		/* Standard scheduled partitions are always free */
		if (map_type == NX_ALL_SPACE || 
			map_type == NX_PART_SPACE)
			alloc_image = 
				overlay_layers(part->child_alloc_lyr);
		if (map_type == NX_ALL_SPACE || 
			map_type == NX_APP_SPACE)
			sched_image = 
				overlay_layers(part->child_sched_lyr);
	}

	if (alloc_image == (BITMAP_T *)0 &&
		sched_image == (BITMAP_T *)0)
		free_image = bitmap_clone(part->bitmap);
	else
	if (alloc_image != (BITMAP_T *)0 &&
		sched_image != (BITMAP_T *)0)
		free_image  = AND_bitmaps(alloc_image,sched_image);
	else
	if (alloc_image != (BITMAP_T *)0)
		free_image = alloc_image;
	else
	if (sched_image != (BITMAP_T *)0)
		free_image = sched_image;

	/* Now take out bad nodes and missing nodes */
	bitmap_allocate_space(unusable_node_map,free_image);

	for (i = 0; i < part->bitmap->cols; i++)
		bitmap->colmap[i] = free_image->colmap[i];

	if (alloc_image != (BITMAP_T *)0)
	    free_bitmap(alloc_image);

	if (sched_image != (BITMAP_T *)0)
	    free_bitmap(sched_image);

	if (free_image != alloc_image && free_image != sched_image)
	    free_bitmap(free_image);
}

#endif /* 0 */

/* 	Get partition bitmap structure
 *
 *	Entry point for Server to Allocator RPC
 *
*/
kern_return_t
get_part_bitmap(serv_port,inode,uid,gid,bitmap,bitmapCnt,retcode)
mach_port_t	serv_port;
ino_t		inode;
uid_t		uid;
gid_t		gid;
BITMAP_T	**bitmap;
int		*bitmapCnt;
int		*retcode;
{
	PART_T		*part;
	int		i;
	BITMAP_T	*invert;

	*bitmap = (BITMAP_T *)0;
	*bitmapCnt = 0;

	/* make sure we have access to the partition */ 
	if ( (part = validate_allocator_access(inode,inode,uid,gid,READ,retcode)) 
		!= (PART_T *)0){

		*retcode = (int) vm_allocate(mach_task_self(),
		  (vm_address_t*) bitmap,
		  (vm_size_t) bitmap_size(part->bitmap),
		  (boolean_t) 1);

		if ( *retcode != KERN_SUCCESS){
			/* We don't have any memory so quit */
			*retcode = ENOMEM;
			goto done;
		}


		*bitmapCnt 	= bitmap_size(part->bitmap);
		(*bitmap)->rows = part->bitmap->rows;
		(*bitmap)->cols = part->bitmap->cols;
		(*bitmap)->row_offset = part->bitmap->row_offset;

		/* if we are asking for root, then send the root bitmap
		 * minus the inverted root bitmap
		*/
		if (part == root){
			invert = invert_bitmap(inverted_root);
			for (i = 0; i < invert->cols; i++)
				(*bitmap)->colmap[i] = invert->colmap[i];
			free_bitmap(invert);
		}
		else
			for (i = 0; i < part->bitmap->cols; i++)
				(*bitmap)->colmap[i] = part->bitmap->colmap[i];

		*retcode = 0;
	}
	else
		*retcode = EPACCES;

done:
	return(KERN_SUCCESS);
}

#if 0

/* 	Get partition free space
 *
 *	Entry point for Server to Allocator RPC
 *
*/
kern_return_t
get_part_freespace(serv_port,inode,uid,gid,bitmap,bitmapCnt,retcode)
mach_port_t	serv_port;
ino_t		inode;
uid_t		uid;
gid_t		gid;
BITMAP_T	**bitmap;
int		*bitmapCnt;
int		*retcode;
{
	PART_T	*part;
	int	i;
	BITMAP_T *free_map,*overlayed_alloc,*overlayed_sched;

	/* make sure we have access to the partition */ 
	if ( (part = validate_allocator_access(inode,inode,
			uid,gid,READ,retcode)) 
		!= (PART_T *)0){

		overlayed_alloc = overlay_layers(part->child_alloc_lyr);
		overlayed_sched = overlay_layers(part->child_sched_lyr);
		/* If we have subpartitions allocated and applications
		 * scheduled then just AND the allocation and scheduling
		 * maps
		*/
		if (overlayed_alloc != (BITMAP_T *)0 &&
			overlayed_sched != (BITMAP_T *)0 )
			free_map = AND_bitmaps(overlayed_alloc,overlayed_sched);
		else
		/* Otherwise one of the maps was empty, so figure out which
		 * one
		*/
		if (overlayed_alloc == (BITMAP_T *)0 &&
			overlayed_sched == (BITMAP_T *)0)
			/* Both maps are empty so partition is free */
			free_map = bitmap_clone(part->bitmap);
		else
		if (overlayed_alloc != (BITMAP_T *)0)
			/* the allocation map was not empty so copy it */
			free_map = bitmap_clone(overlayed_alloc);
		else
			/* the scheduling map was not empty so copy it */
			free_map = bitmap_clone(overlayed_sched);
		
		*retcode = vm_allocate(mach_task_self(),(vm_address_t ) bitmap,
				bitmap_size(part->bitmap),1);

		if ( *retcode != KERN_SUCCESS){
			/* We don't have any memory so quit */
			*retcode = ENOMEM;
			goto done;
		}

		*bitmapCnt 	= bitmap_size(part->bitmap);
		(*bitmap)->rows = part->bitmap->rows;
		(*bitmap)->cols = part->bitmap->cols;
		(*bitmap)->row_offset = part->bitmap->row_offset;
		for (i = 0; i < part->bitmap->cols; i++)
			(*bitmap)->colmap[i] = free_map->colmap[i];

		free_bitmap(free_map);
		free_bitmap(overlayed_alloc);
		free_bitmap(overlayed_sched);
		*retcode = 0;
	}
	else
		*retcode = EPACCES;

done:
	return(KERN_SUCCESS);
}
#endif /* 0 */

/***************** Register a daemon with the allocator **************
 *
 *
 *
*/

register_daemon_allocator(server_port, daemon_id, socket_addr, socket_len, ret_code)
mach_port_t	server_port;
int		daemon_id;
u_char		*socket_addr;
int		socket_len;
int		*ret_code;
{
	switch (daemon_id){
	case SMD_DAEMON_ID:
		smd_init(server_port,socket_addr,socket_len,ret_code);
		break;
	case MACS_DAEMON_ID:
		macs_init(server_port,socket_addr,socket_len,ret_code);
		break;
	default:
		*ret_code = 0;
	}
	return(KERN_SUCCESS);
}


/********************************  rename_partition ******************
 *
 *	Calling Sequence:
 *		rename_partition(from,to)
 *
 *	Description:
 *		rename the partition from to the partition to
 *
 *	Parameters:
 *		from	char pointer to the from string
 *		to	char pointer to the to string
 *
 *	Returns:
 *		0 if all is ok
 *		error code otherwise
*/
int 
rename_partition(char *from,char *to)
{
	int	ok;
	int	error;
	

	error = 0;

	if (rename(from,to) != 0){
		/* remap some of the errnos */
		switch(errno){
		case ENOTEMPTY:
		case EEXIST:
			error = EPPARTEXIST;
			break;
		case EACCES:
			error = EPACCES;
			break;
		default:
			error = errno;
			break; 
		}
	}

	return(error);
}
/*
 *
 *      Calling Sequence:
 *              clear_all_children(part)
 *
 *      Description:
 *              Clear the gang_ancestor field of each child partition struct.
 *              This routine is recursive in order to walk the lists. The
 *              data structures are set up such that a partition structure (PART_T)
 *              points to a layer structure (LAYER_T) which then points to the
 *              child partition structure (CONSUMER_T - in this case a PART_T).
 *
 *      Parameters:
 *              part    partition struct (PART_T)
 *
 *      Returns:
 *              voidp
 */
void
clear_all_children(PART_T *part)
{
    LAYER_T *layer;
    PART_T *child_part;
    
    for ( ; part; part = part->alloc_next) {
        part->gang_ancestor = FALSE;
        for (layer = part->child_alloc_lyr; layer; layer = layer->next) {
            if (((child_part = layer->consumer) != (PART_T*) 0) &&
	      (child_part->type == PART)) {
                clear_all_children(child_part); /* recursive call */
            }
        }
    }
}

/*
 *
 *      Calling Sequence:
 *              set_all_children(part)
 *
 *      Description:
 *              Set the gang_ancestor field of each child partition struct.
 *              This routine is recursive in order to walk the lists. The
 *              data structures are set up such that a partition structure (PART_T)
 *              points to a layer structure (LAYER_T) which then points to the
 *              child partition structure (CONSUMER_T - in this case a PART_T).
 *
 *      Parameters:
 *              part    partition struct (PART_T)
 *
 *      Returns:
 *              void
 */
void
set_all_children(PART_T *part)
{
    LAYER_T *layer;
    PART_T *child_part;
    
    for ( ; part; part = part->alloc_next) {
        part->gang_ancestor = TRUE;
        for (layer = part->child_alloc_lyr; layer; layer = layer->next) {
            if (((child_part = layer->consumer) != (PART_T*) 0) &&
	      (child_part->type == PART)) {
                set_all_children(child_part); /* recursive call */
            }
        }
	
    }
}


