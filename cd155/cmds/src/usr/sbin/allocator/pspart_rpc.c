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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/pspart_rpc.c,v 1.12 1994/12/19 16:02:38 johannes Exp $
 *
 */

/* History:
 *	$Log: pspart_rpc.c,v $
 * Revision 1.12  1994/12/19  16:02:38  johannes
 * fill_in_app_info(): Set the active field of the nx_pspart structure to
 *                     DUMPING_CORE if the application is dumping core
 *                     (indicated by appl_core).
 *
 *  Reviewer: Scott Hahn
 *  Risk: High (several components involved)
 *  Benefit or PTS #: 11577
 *  Testing: developer testing, special testing by Simon Tsang,
 *           corefile EAT, sched EAT, rmcmd EAT, controlc EAT
 *  Module(s): svr/server/paracore/dvp_pvpcore.c
 *             svr/server/nx/nx.c, nx.defs
 *             usr/sbin/allocator/alloc.defs, misc_rpcs.c, init_appl.c,
 *                                allocator.c, pspart_rpc.c
 *             usr/include/nx/schedule.h
 *             usr/include/allocsys.h
 *             usr/bin/pspart
 *
 * Revision 1.11  1994/11/19  03:04:23  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/01/07  20:18:18  carbajal
 *  * Initialize and update the total time for active partitions.
 *  *  Reviewer: cameron
 *  *  Risk: Low
 *  *  Benefit or PTS #: 7489
 *  *  Testing: Bug report
 *  *  Module(s): pspart_rpc.c, schedule.c
 *  *
 *
 * Revision 1.8.2.2  1994/01/06  22:19:27  carbajal
 * Initialize and update the total time for active partitions.
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7489
 *  Testing: Bug report
 *  Module(s): pspart_rpc.c, schedule.c
 *
 * Revision 1.8.2.1  1993/12/29  17:46:29  carbajal
 * Grab uid and gid out of partition and application structures to
 * pass pack to nx_pspart() call.
 *  Reviewer: John Litvin
 *  Risk: Low
 *  Benefit or PTS #: 7425
 *  Testing: Bug report
 *  Module(s): pspart_rpc.c
 *
 * Revision 1.8  1993/12/01  19:59:24  carbajal
 * Changed APPREQ_T to nx_pspart_t
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.7  1993/12/01  01:38:40  carbajal
 * Pass back active partition information
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.6  1993/11/18  20:23:59  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of the export tree
 *  Testing: built on Suns and 486
 *  Module(s): scripts.mk standard.mk
 *
 * Revision 1.5  1993/11/17  02:54:57  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.4  1993/10/27  02:02:14  carbajal
 * include allocutils.h
 *
 * Revision 1.3  1993/07/20  18:11:30  carbajal
 * Changed MIG_BITMAP_T to char *
 *
 * Revision 1.2  1993/07/13  22:21:38  carbajal
 * Added alloc.h
 *
 * Revision 1.1  1993/05/25  23:53:36  carbajal
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
#include <allocsys.h>
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

/* Forward references */
void get_app_info();
void fill_in_app_info();
void fill_in_part_info();

/* defined in server_loop.c */
extern unsigned long    interval;
extern int pspart_debug;


/*	Allocator entry point for the pspart command 
 *
 *	Parameters:
 *		serv_port
 *		inode	=	inode of partition
 *		uid	=	user id of calling process
 *		gid	=	group id of calling process
 *		error	=	pointer to an integer
 *				this is set to the error code
 *				if an error happens
 *
 *	Returns:
 *		MACH_KERN_SUCCESS
*/

kern_return_t
pspart(serv_port,inode,uid,gid,app_ptr,applistCnt,error)
mach_port_t	serv_port;
ino_t		inode;		/* inode of partition */
uid_t		uid;		/* user id of caller */
gid_t		gid;		/* group id of caller */
char		***app_ptr;	/* application info list */
int		*applistCnt;	/* number of apps in list */
int		*error;		/* used to return error code */
{
        PART_T   	*part;         /* Partition to change */
	nx_pspart_t 	*app_list;	/* application info list */
	kern_return_t	retcode;

	/* The flow here is to look up the partition
	 * using the inode. We then need to count
	 * the number of applications within a layer,
	 * allocate space for the application info list, and
	 * then fill in the list.
	*/

	*error = 0;
	*applistCnt = 0;

	/* make sure we have access to the partition */ 
	part = validate_allocator_access(inode,inode,uid,gid,READ,error);

	/* see if we got a partition 
	 */
	if (part != (PART_T *)0){
		/* We have access to the partition so now count the 
	 	* number of applications within the partition
		*/
		*applistCnt = count_obj_in_part(part);
		/* See if there is anything else to do */
		if (*applistCnt > 0){
			/* There are applications in this partition
		 	* so now allocate space for the app_list
			*/
			retcode = vm_allocate(mach_task_self(), (vm_address_t *) &app_list,
				(*applistCnt * sizeof(nx_pspart_t)),1);

			if (retcode != KERN_SUCCESS){
				/* out of memory! */
				*error = EPALLOCERR;	/* Allocator internal error */
			}
			else{
				/* yes, so now we need to traverse the layers
				*/
				get_app_info(part,app_list,*applistCnt);
			}
			*app_ptr = (char **) app_list;
		} /* *applistCnt > 0 */
	} /* part != NULL */

	*applistCnt = sizeof(nx_pspart_t) * *applistCnt;

	return 0;
}

/* Count the number of applications within a partition
*/
int
count_obj_in_part(p)
PART_T	*p;
{
	LAYER_T		*layer;
	CONSUMER_T	*co;
	int		count;

	count = 0;

	layer = p->child_sched_lyr;

	while(layer != (LAYER_T *)0){
		/* we have a valid layer,
		 * get the consumer object
		 */
		co = layer->consumer;
		while(co != (CONSUMER_T *)0){
			/* we have a valid consumer object
			*/
			count++;
			co = co->next;
		}
		layer = layer->next;
	}

	return(count);
}

/* Traverse the layers filling in the application info
 * structure
*/
void
get_app_info(p,app_info,appCnt)
PART_T		*p;			/* Partition requested */
nx_pspart_t	*app_info;		/* application info list */
int		appCnt;			/* Number of apps in partition */
{
	LAYER_T		*layer;
	CONSUMER_T	*co;
	int		found_so_far;	/* applications filled in so far */
	nx_pspart_t	*ptr;

	found_so_far=0;
	layer = p->child_sched_lyr;
	ptr = app_info;

	while(layer != (LAYER_T *)0){
		/* we have a valid layer,
		 * get the consumer object
		 */
		co = layer->consumer;
		while(co != (CONSUMER_T *)0){
			/* we have a valid consumer object
			*/
			if (co->type == APPL){
				/* this consumber object is
				 * an application
				*/
				fill_in_app_info(p,(APPL_T *)co,ptr);
				/* make sure we don't overflow our count
				 * since applications are dynamic
				*/
				found_so_far++;
				ptr++;
				if (found_so_far > appCnt)
					/* we are in trouble!
					 * return with what we have so
					 * far
					*/
					return;
			}
			else
			if (co->type == PART){
				/* this is an active partition */
				fill_in_part_info(p,(PART_T *)co,ptr);
				found_so_far++;
				ptr++;
				if (found_so_far > appCnt)
					/* we are in trouble!
					 * return with what we have so
					 * far
					*/
					return;
			}

			co = co->next;
		}
		layer = layer->next;
	}

	return;
}

void
fill_in_app_info(p,co,app_info)
PART_T		*p;			/* partition */
APPL_T		*co;			/* application info for allocator */
nx_pspart_t	*app_info;		/* application info list */
{
	app_info->object_type	= NX_APPLICATION;
	app_info->object_id 	= co->pgroup;
	app_info->size   	= co->size;
	app_info->uid		= co->uid;
	app_info->gid		= co->gid;
	app_info->priority	= co->priority;
	app_info->rollin_q	= p->rollin_quantum;
	app_info->active	= (co->appl_stat == APPL_GANG_START);
#ifdef PARACORE
	if (co->appl_core == APPL_DUMPING_CORE)
		app_info->active |= DUMPING_CORE;
#endif /* PARACORE */
	if (app_info->active){
		app_info->elapsed	= co->elapsed + interval;
		app_info->rolled_in	= co->rolled_in + interval;
	}
	else{
		app_info->elapsed       = co->elapsed;
		app_info->rolled_in	= co->rolled_in;
	}
	app_info->time_started 	= co->start_time;

	if (pspart_debug){
		printf("fill in app\n");
		printf("pgroup=%d size=%d pri=%d rolled_in=%d rollin_q=%d elapsed=%d active=%d\n",
			app_info->object_id,app_info->size,
			app_info->priority,app_info->rolled_in,app_info->rollin_q,
			app_info->elapsed,app_info->active);
	}
}

void
fill_in_part_info(p,co,app_info)
PART_T		*p;			/* partition */
PART_T		*co;			/* partition info for allocator */
nx_pspart_t	*app_info;		/* application info list */
{
	app_info->object_type	= NX_PARTITION;
	app_info->object_id 	= co->inode;
	app_info->size   	= co->nodes;
	app_info->uid		= co->owner;
	app_info->gid		= co->group;
	app_info->priority	= co->cur_priority;
	app_info->rollin_q	= p->rollin_quantum;
	app_info->active	= (co->status == PART_ROLLED_IN); 
	if (app_info->active){
		app_info->elapsed	= co->elapsed + interval;
		app_info->rolled_in	= co->rolled_in + interval;
	}
	else{
		app_info->elapsed       = co->elapsed;
		app_info->rolled_in	= co->rolled_in;
	}

	app_info->time_started 	= co->start_time;

	if (pspart_debug){
		printf("fill in part\n");
		printf("type %d inode=%d size=%d pri=%d rolled_in=%d rollin_q=%d elapsed=%d active=%d\n",
			app_info->object_type,app_info->object_id,
			app_info->size,
			app_info->priority,app_info->rolled_in,app_info->rollin_q,
			app_info->elapsed,app_info->active);
	}

}


