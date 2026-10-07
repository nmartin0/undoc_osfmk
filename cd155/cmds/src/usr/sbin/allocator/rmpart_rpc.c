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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/rmpart_rpc.c,v 1.14 1994/11/19 03:04:24 mtm Exp $
 *
 */

/* History:
 *	$Log: rmpart_rpc.c,v $
 * Revision 1.14  1994/11/19  03:04:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/06/13  16:54:21  sdh
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
 * Revision 1.12  1993/12/20  23:10:35  carbajal
 *   Be careful to vm_dealloc() memory properly. Use macros FREE and
 *   MALLOC to make sure all memory allocated is properly deallocated.
 *    Reviewer: cameron
 *    Risk: Low
 *    Benefit or PTS #: 7257
 *    Testing: SATs, EATs, MUNOPS, bug test
 *    Module(s): misc_rpcs.c, mkpart_rpc.c, schedule.c, rmpart_rpc.c
 *
 * Revision 1.10.2.1  1993/12/20  21:58:08  carbajal
 *   Be careful to vm_dealloc() memory properly. Use macros FREE and
 *   MALLOC to make sure all memory allocated is properly deallocated.
 *    Reviewer: cameron
 *    Risk: Low
 *    Benefit or PTS #: 7257
 *    Testing: SATs, EATs, MUNOPS, bug test
 *    Module(s): misc_rpcs.c, mkpart_rpc.c, schedule.c, rmpart_rpc.c
 *
 * Revision 1.11  1993/12/10  20:38:44  carbajal
 * Call vm_dealloc on any out of line data from MIG
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: Works towards fixing 7257
 *  Testing: mkpart_leak and developer testing
 * 	(this does not completely solve the problem)
 *  Module(s):
 *
 * Revision 1.10  1993/11/18  20:24:00  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of the export tree
 *  Testing: built on Suns and 486
 *  Module(s): scripts.mk standard.mk
 *
 * Revision 1.9  1993/11/17  02:55:08  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.8  1993/10/27  02:02:46  carbajal
 * Include allocutils.h
 *
 * Revision 1.7  1993/08/31  01:52:26  carbajal
 * Fixed PTS #6335.
 *
 * Revision 1.6  1993/08/23  17:20:31  carbajal
 * Fixed PTS 5677 by removing a call to validate_allocator_access()
 * in build_part_list().
 *
 * Revision 1.5  1993/07/29  19:19:08  carbajal
 * Changed *nsubparts++ to *nsubparts += 1 in count_consumers_in_partition()
 *
 * Revision 1.4  1993/07/29  00:30:04  carbajal
 * Use linked list pointers alloc_prev and alloc_next for removing
 * a partition from an allocation layer
 *
 * Revision 1.3  1993/07/13  22:22:43  carbajal
 * Support for allocator system calls
 *
 * Revision 1.2  1993/07/07  22:55:35  carbajal
 * When removing the scheduling layer, make sure the data associated
 * with the layers are freed.
 *
 * Revision 1.1  1993/05/25  23:54:46  carbajal
 * New module dealing with rmpart RPC code was pulled out of allocator.c
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

extern HASH_TBL_T              part_tbl[];
extern HASH_TBL_T              appl_tbl[];
extern int	num_gang_parts,signal_debug,sched_debug,allocation_debug;

extern PART_T  *root;
typedef ino_t   *inode_list_t;


/* Forward declarations */
int count_consumers_of_partition(PART_T *part,int *nsubparts, int *napps);
void do_remove_using_alloc_layer(PART_T *part);
void do_remove_using_sched_layer(PART_T *part);
int
remove_part_using_list(char *partname,inode_list_t list,int n_list,int *error);
int
remove_partition(char *pathname,int *error);
int
inode_compare(ino_t *in1,ino_t *in2);
int
is_inode_on_list(char *pathname,inode_list_t list,int n_list);

/* Count the total number of objects in each sublayer of the
 * partition part
*/
void
count_objects_in_sublayers(int matchtype,PART_T *part,int *napps)
{
	LAYER_T	*l;
	CONSUMER_T *c;
	PART_T	   *p;
	int	count;
	char	*matchstring;

	if (matchtype == PART){
		/* if we are looking for partitions then look at the
		 * allocation layer
		*/
		l = part->child_alloc_lyr;
		matchstring = "PART";
	}
	else{
		/* else we are looking for applications so
		 * look at the scheduling layer
		*/
		l = part->child_sched_lyr;
		matchstring = "APPL";
	}

	/* Look through all layers of this partition */
	if (matchtype != PART)
		/* we are looking through scheduling layers */
		for (; l != (LAYER_T *) 0; l = l->next) {
			for (c = l->consumer; c != (CONSUMER_T *) 0; c = c->next) {
				if (c->type == matchtype){     
					/* Keep track of loaded objects
					*/
					(*napps)++;
				}
				if (c->type == PART){
					count = 0;
					/* look in this sub partition */
					count_objects_in_sublayers(matchtype,(PART_T *)c,&count);
					(*napps) += count;
				}
			}
		}
	else
		/* we are looking through the allocation layers */
		for (; l != (LAYER_T *) 0; l = l->next) {
			for (p = (PART_T *)l->consumer; p != (PART_T *) 0;p = p->alloc_next) {
				assert(p->type == PART);
				(*napps)++;
				count = 0;
				/* look in this sub partition */
				count_objects_in_sublayers(matchtype,p,&count);
				(*napps) += count;
			}
		}

	debug_allocation(5,"count_objects_in_sublayers %s part->inode %d napps %d\n",
			matchstring,part->inode,*napps);
}

/* Look through the list for layers and sublayers starting from the 
 * partition pointed to by part that we have write access to. Add
 * the subpartitions to the list of partition pointers pointed to by list.
 *
 *	Parameters:
 *		uid		user id for access validation
 *		gid		group id for access validate
 *		list		pointer to partition pointers
 *		index		pointer to the integer index into list
 *		part		pointer to the starting partition
 *
*/
void
add_to_part_list(uid_t uid, gid_t gid,PART_T **list,int *index,PART_T *part)
{
	int		error;
	LAYER_T		*l;
	PART_T		*p;
	
	/* If we do not have write access to this partition then we
	 * cannot remove any of the subpartitions
	*/
	if ( (p = validate_allocator_access(part->inode,part->inode,
		uid,gid,WRITE,&error)) == 
		(PART_T *)0)
		/* No write permission, so just return */
		return;

	/* Look through all the allocation layers */
	for (l = part->child_alloc_lyr; l != (LAYER_T *) 0; l = l->next) {
		for (p = (PART_T *)l->consumer; p != (PART_T *) 0;p = p->alloc_next) {
			assert(p->type == PART);
			/* add us to the list */
			list[(*index)++] = p;
			debug_allocation(5,"add_to_part_list part->inode %d\n",list[(*index)-1]->inode);
			/* see if we need to add any of our
			* subpartitions to the list
			*/
			add_to_part_list(uid,gid,list,index,p);
		}
	}
}

/* Build a list of pointers to partitions to be removed
 *
 *	Parameters:
 *		recursive	set to 1 if we are supposed to do a recursive
 *				descent
 *		uid		user id for access validation
 *		gid		group id for access validation
 *		part		pointer to starting partition
 *		n_list		size of partition pointer list
 *
 *	Returns:
 *		a pointer to a list of partition pointers
*/
PART_T **
build_part_list(int recursive,uid_t uid, gid_t gid,PART_T *part,int n_list)
{
	int		i,index;
	LAYER_T		*l;
	CONSUMER_T	*c;
	PART_T		*tmp_ptr,**ptr,**list;
	
	debug_allocation(5,"build_part_list part->inode %d %d\n",part->inode,n_list);
	list = (PART_T **) MALLOC(sizeof(PART_T *) * n_list);

	bzero((char *)list, sizeof(PART_T *) * n_list);

	ptr = list;

	if (list == (PART_T **) 0)
		errno = ENOMEM;
	else{
		index = 0;
		/* part points to the partition specified in rmpart 
		 * partname. We have already validated that we have the
		 * proper permission from its parent, so just add it to
		 * the list.
		*/
		list[index++] = part;
		if (n_list > 1)
			/* There are subpartitions */
			add_to_part_list(uid,gid,list,&index,part);
	}
	return(list);

}

/*
 * part_remove() - Exported MIG routine to remove a partition. Permission
 * checking is done in client side.
*/
kern_return_t
part_remove(serv_port, pathname, pathlen, inode, uid, gid, flags,error)
mach_port_t	serv_port;	/* Server Port */
char		*pathname;
int		pathlen;
ino_t		inode;
uid_t		uid;		/* user id */
gid_t		gid;		/* group id */
int		flags;
int		*error;		/* Errno */
{
	PART_T		*part;				/* Partition to remove */
	PART_T		*remove_from_part;		/* Partition to remove */
	PART_T		*ptr;
	PART_T		*parent_part;			/* Parent of partition to remove */
	LAYER_T		*alloc_layer;			/* Layer which contains partition to remove */
	LAYER_T		*alloc_list;			/* list of allocation layers */
	LAYER_T		*p;				
        CONSUMER_T      *c;
	int		nsubparts,napps;/* counter for subpartitions and
					* applications 
					*/
	int		sublayer_napps;	/* Number of applications in
					* sublayers
					*/
	int		sublayer_nparts;/* Number of partitions in
					* sublayers
					*/
	int		keep_going;	/* flag to indicate all is ok
					* set to TRUE or FALSE
					*/
	int		remove_active;	/* flag indicating if we are removing
					* the active layer
					*/
	PART_T		**list;		/* List of partitions for recursive
				 	* remove
					*/
	int		part_list_cnt;
	int		i,inode_list_cnt;
	kern_return_t	retcode;
	inode_list_t	inode_list;
	int		n_list;
	int		recursive,force;
	ino_t		parent_inode;
	
	/* Make sure all pointers are initialized */
	inode_list = (inode_list_t )0;
	part = remove_from_part = ptr = parent_part = (PART_T *)0;
	alloc_layer = alloc_list = p = (LAYER_T *)0;
	c = (CONSUMER_T *)0;
	list = (PART_T **)0;
	
	debug_allocation(5,"part_remove flags %d inode %d pathname %s pathlen %d\n",flags,inode,pathname,pathlen);
	*error = 0;

	if (flags & 1)
		recursive = 1;
	else
		recursive = 0;
	if (flags & 2)
		force = 1;
	else
		force = 0;
	remove_active = 0;

	/* Get the parent inode */
	if ( (part = HASH_LOOKUP_PART(inode)) == (PART_T *)0){
		errno = EPINVALPART;
		return 0;
	}

	if (part == root){
		*error = EACCES;
		return 0;
	}

	assert(part->parent_alloc_lyr != (LAYER_T *)0);
	parent_inode = part->parent_alloc_lyr->part->inode;

	/* make sure we have access to the partition */ 
	if ( (part = 
		validate_allocator_access(parent_inode,inode,
				uid,gid,(CHECK_AGAINST_PARENT | WRITE),error)) == 
				(PART_T *)0)
                return 0;

	/* we can't remove the root partition! */
	if (part == root){
		*error = EACCES;
		return 0;
	}

	/* Save this for later */
	remove_from_part = part;

	/* OK. There are four cases to think about here:
	 *	1. Partition is empty
	 *		it is ok to remove
	 *	2. Partition has applications
	 *		need -f flag to remove partition
	 *	3. Partition has subpartitions
	 *		need -r flag to remove partition and all
	 *		subpartitions
	 *	4. Partition has applications and subpartitions
	 *		need -r and -f flags
	 * In all cases we need permission to access the partition.
	*/

	sublayer_nparts = sublayer_napps = nsubparts = napps = 0;

	count_consumers_of_partition(part,&nsubparts,&napps);

	/* count the number of applications in all subpartitions */
	count_objects_in_sublayers(APPL,part,&sublayer_napps);
	/* count the number of partitions in all subpartitions */
	count_objects_in_sublayers(PART,part,&sublayer_nparts);

	if (allocation_debug){
	        debug_allocation(5,"remove partition\n");
		debug_allocation(5,"force flag %d recursive flag %d\n",force,recursive);
		debug_allocation(5,"nsubs %d napps %d sublayer_napps %d\n",nsubparts,napps,sublayer_napps);
		debug_allocation(5,"case 1 %d\n",(nsubparts == 0) && (napps == 0));
		debug_allocation(5,"case 2 %d\n",(napps > 0) && (nsubparts == 0) && force);
		debug_allocation(5,"case 3 %d\n",( (nsubparts > 0) && (napps == 0)
				 && (sublayer_napps == 0) && recursive));
		debug_allocation(5,"case 4 %d\n",( (nsubparts > 0) && 
			( (napps > 0) || (sublayer_napps > 0) )
			 && force && recursive));
	}

	/* Case 1 */
	if ( (nsubparts == 0) && (napps == 0) )
		keep_going = TRUE;
	else
		/* Case 2 */
		if ( (napps > 0) && (nsubparts == 0) && force)
			keep_going = TRUE;
		else
			/* Case 3 */
			if ( (nsubparts > 0) && (napps == 0) && (sublayer_napps == 0) && recursive)
				keep_going = TRUE;
			else
			/* Case 4 */
				if ( (nsubparts > 0) && 
					( (napps > 0) || (sublayer_napps > 0)) 
						&& force && recursive)
					keep_going = TRUE;
				else{
					keep_going = FALSE;
					*error = EPNOTEMPTY;
				}

	/* if keep_going is TRUE at this point then all is ok
	 * we can remove the partition(s)
	*/

	if (keep_going){

		/* No go through all the partitions and build a list
		 * of those that are ok to remove, ie we have write
		 * permission on the parent 
		*/
		if (recursive)
			n_list = sublayer_nparts + 1;
		else
			n_list = 1;
 
		list = build_part_list(recursive,uid,gid,part,n_list);

		if (list == (PART_T **)0)
			/* We don't have any memory so quit */
			goto done;
		
		if ( (inode_list = (ino_t *) MALLOC(n_list * sizeof(ino_t))) == (ino_t *)0){
			/* We don't have any memory so quit */
			errno = ENOMEM;
			goto done;
		}

		bzero( (char *) inode_list, sizeof(ino_t) * n_list);

		debug_allocation(5,"part_remove: dealloc partition %d\n",n_list);
		inode_list_cnt = n_list-1;

CSECT_ENTER
		/* Start from the end of the list */
		for(part_list_cnt = n_list-1; part_list_cnt >= 0;part_list_cnt--){
			
			if (list[part_list_cnt] == 0)
				inode_list[inode_list_cnt--] = 0;
			else{
				part = list[part_list_cnt];
				count_consumers_of_partition(part,&nsubparts,&napps);
				/* If there are no subpartitions of this partition
			 	* then remove the partition
				*/
				if (nsubparts == 0){
					/* Add this partition to the inode list */
					inode_list[inode_list_cnt--] = part->inode;
					debug_allocation(5,"add inode %d to list\n",inode_list[inode_list_cnt+1]);

					/* To remove a partition we need to deal with
					 * both the allocation tiles and the scheduling tiles
					*/

					/* Grab the layer that this partition resides in */
					alloc_layer = part->parent_alloc_lyr;
					/* Grab the list of layers that our layer resides in */
					alloc_list = alloc_layer->part->child_alloc_lyr;;

					/* remove the objects in the partition's scheduling layers */
					do_remove_using_sched_layer(part);

					/* If the partition priority is not NO_PRI then there was
					 * partition was active. We need to deactivate it by recursively
					 * removing it from its parent's scheduling layers
					*/
					if (part->cur_priority != NO_PRI)
						perk_remove_active_partition(part);
					
					parent_part = part->parent_alloc_lyr->part;
					/* free the partition from it's parent allocation layer */

					dump_alloc_consumers(part->parent_alloc_lyr);

					free_object_from_layer(ALLOC_TILE,(CONSUMER_T *)part,
									&(part->parent_alloc_lyr),
									&(part->parent_alloc_lyr->part->child_alloc_lyr));

					dump_alloc_consumers(part->parent_alloc_lyr);

					debug_allocation(5,"parent_part %d parent_part->child_alloc_lyr 0x%x\n",parent_part->inode,
							 parent_part->child_alloc_lyr);
					/* remove the objects in the partition's allocation layers */
					do_remove_using_alloc_layer(part);
					
				}
				else
					inode_list[inode_list_cnt--] = 0;
			}
		}
		*error = ESUCCESS;
		debug_allocation(5,"qsort list\n");
		/* Sort the inode list so that we can use bsearch */
		qsort((char *)inode_list,n_list,sizeof(ino_t),inode_compare);
		if (remove_part_using_list(pathname,inode_list,n_list,error) != 0)
			;
CSECT_EXIT
	}

done:
	if ( inode_list != (ino_t *)0)
		FREE( (void *)inode_list);

	FREE((void *)list);

	vm_deallocate(mach_task_self(),(vm_address_t *) pathname,
                (vm_size_t) pathlen);

	return 0;
}

int
count_consumers_of_partition(PART_T *part,int *nsubparts, int *napps)
{
	LAYER_T		*l;
	CONSUMER_T	*c;
	PART_T		*partition;

	*nsubparts = *napps = 0;

	/* Look through all allocation layers of this partition */
	for (l = part->child_alloc_lyr; l != (LAYER_T *) 0; l = l->next) {
		for (partition = (PART_T *)l->consumer; partition != (PART_T *) 0;
					partition = partition->alloc_next) {
			assert(partition->type == PART); 
			/* Keep track of number of subpartitions
			* and loaded applications
			*/
			(*nsubparts)++;
		}
	}

	/* Look through all scheduling layers of this partition */
	for (l = part->child_sched_lyr; l != (LAYER_T *) 0; l = l->next) {
		for (c = l->consumer; c != (CONSUMER_T *) 0; c = c->next) {
			/* Keep track of number of subpartitions
			* and loaded applications
			*/
			(*napps)  += (c->type == APPL);
		}
	}

	debug_allocation(5,"*nsubparts %d *napps %d\n",*nsubparts,*napps);
}

/*
 * do_remove_using_alloc_layer() - Internal routine to remove a partition and its children.
 */
void
do_remove_using_alloc_layer(PART_T *part)
{

	LAYER_T		*layer;	
	PART_T		*next_p,*partition;

	/*
	 * For each layer in list of layers.
	 */
	debug_allocation(5,"do_remove_using_alloc_layer()\n");
	assert(part != (PART_T *)0);

	layer = part->child_alloc_lyr;

	while (layer !=  (LAYER_T *) 0) {
		/*
		 * For each consumer in allocation layers.
		 */
		partition = (PART_T *)layer->consumer;
		while (partition !=  (PART_T *) 0) {
			/*
			* Consumer is a partition, recursively apply
			* do_remove().
			*/
			assert(partition->type == PART);
			next_p = partition->alloc_next;
			do_remove_using_alloc_layer(partition);
			debug_sched(5,"partition removed\n");
			partition = next_p;
		}
		layer = layer->next;
	}

	/*
	* Remove this partition from partition table and free up any 
	* memory it is consuming.
	*/
	assert(part != (PART_T *)0);
	HASH_REMOVE_PART(part->inode);
	if (part->sched == GANG)
		num_gang_parts--;
	FREE((void *) part->bitmap);
	FREE((void *) part->lp);
	FREE((void *) part);

	debug_allocation(5,"partition removed from internal tables\n");

}
/*
 * do_remove_using_sched_layer() - Internal routine to remove a partition and its children.
 */
void
do_remove_using_sched_layer(PART_T *part)
{

	LAYER_T		*next,*layer;	
	CONSUMER_T	*consumer; 

	/*
	 * For each layer in list of layers.
	 */
	debug_allocation(5,"do_remove_using_sched_layer()\n");
	debug_sched(5,"do_remove_using_sched_layer()\n");

	layer = part->child_sched_lyr;
	while (layer !=  (LAYER_T *) 0) {
		/*
		 * For each consumer in layers.
		 */
		consumer = layer->consumer;
		while (consumer !=  (CONSUMER_T *) 0) {
			if (consumer->type == PART) {
				/*
				 * Consumer is a partition, recursively apply
				 * do_remove().
				 */
				do_remove_using_sched_layer((PART_T *) consumer);
				debug_sched(5,"partition removed\n");
			}
			else {
				APPL_T *appl;

				/*
				 * Consumer is an application, kill it with
				 * extreme prejudice, remove it from the
				 * application table, and free up any memory
				 * it is consuming.
				 */
				appl = (APPL_T *) consumer;
				debug_sched(5,"do_remove_using_sched_layer, kill application %d\n",appl->pgroup);
				kill(-(appl->pgroup), SIGKILL);
				debug_sched(5,"do_remove_using_sched_layer, application killed\n");
				notify_smd(appl, APP_END);
				HASH_REMOVE_APPL(appl->pgroup);
				FREE((void *) appl->bitmap);
				FREE((void *) appl->lp);
				FREE((void *) appl);
			}
		consumer = consumer->next;
		}
		next = layer->next;
		FREE((void *) layer->bitmap);
        	FREE((void *) layer);
		layer = next;
	}

	/* We have removed all consumers from the scheduling tiles */
	part->child_sched_lyr = (LAYER_T *)0;

}

int
inode_compare(ino_t *in1,ino_t *in2)
{
        return(*in1 - *in2);
}

/*	is_inode_on_list
 *
 *	Description:
 *		Check to see if our inode is scheduled to be removed.
 *		If it is it will appear on the list returned from the
 *		allocator.
 *
 *
 *	Returns:
 *		1 if inode is on list
 *		0 otherwise
*/
int
is_inode_on_list(char *pathname,inode_list_t list,int n_list)
{
	ino_t	inode;
	ino_t	*ptr;

	inode = get_inode(pathname);

	debug_allocation(5,"is_inode_on_list %s inode %d\n",pathname,inode);

	ptr = (ino_t *) bsearch((char *)&inode,(char *)list,n_list,
		sizeof(ino_t *),inode_compare);

	if (ptr == (ino_t *) 0)
		return(0);
	else
		return(1);

}

/*		remove_partition
 *
 *	Description:
 *		remove the .partinfo file and the directory pointed
 *		to by pathname.
 *
 *
 *	Returns:
 *		-1 if an error occurs, errno is set
 *		0  if all is ok
 *
*/
int
remove_partition(char *pathname,int *error)
{
	char		*partinfo_path;
	PATHTYPE	path;
	int		status;

	debug_allocation(5,"remove_partition %s\n",pathname);

	status = -1;
	/* initialize a pathname structure */
	if (init_pathtype(&path) != NULL){
		/* add the base pathname */
		if (append_path(&path,pathname) != NULL)
			/* make is a .partinfo pathname */
			if (append_path(&path,"/.partinfo") != NULL){
				/* remove the .partinfo file */
				if (unlink(path.space) != -1 || 
					errno == ENOENT){
					/* if the file is removed, the
					 * remove the directory
					*/
					if ( (status = rmdir(pathname)) == -1)
						*error = errno;
				}
		}
		else
			*error = EPALLOCERR;
	}
	else
		*error = EPALLOCERR;
	return(status);
}


/* 		remove_part_using_list
 *
 *	Description:
 *		Starting from path recursively remove all files
 *		and directories whose inodes appear in the parameter list.
 *		The allocator supplies the list.
 *
 *	Parameters:
 *		path		char pointer to pathname
 *		ino_list	pointer to array of inodes
 *		n_list		number of inodes in list
 *
 *	Returns:
 *		VOID
 *
 *
*/
int
remove_part_using_list(char *partname,inode_list_t list,int n_list,int *error)
{
	DIR		dir;
	struct dirent	dir_entry;
	int		save_len;
	int		dir_opened,status;
	PATHTYPE	path;
	ino_t		inode;
	PART_T		*part;
	
	debug_allocation(5,"remove_part_using_list %s\n",partname);

	dir_opened = 0;
	if (init_pathtype(&path) != NULL){
		if (append_path(&path,partname) == NULL){
			*error = EPALLOCERR;
			return -1;
		}
	}
	else{
		*error = EPALLOCERR;
		return -1;
	}

	if (opendir_r(path.space, &dir) == -1){
		*error = EPINVALPART;
		return -1;
	}

	while (readdir_r(&dir, &dir_entry) != -1) {
		save_len = strlen(path.space);
		if ( append_path(&path,"/")  == NULL){
			*error = EPALLOCERR;
			goto remove_using_list_error;
		}
		if ( append_path(&path,dir_entry.d_name) == NULL){
			*error = EPALLOCERR;
			goto remove_using_list_error;
		}
		if ((isdir(path.space)) &&
                    (strcmp(".", dir_entry.d_name) != 0)         &&
		    (strcmp("..", dir_entry.d_name) != 0))
		{
			if (remove_part_using_list(path.space,list,n_list,error) == 0){
				if (is_inode_on_list(path.space,list,n_list)){
					if (remove_partition(path.space,error) == -1)
						;
				}
			}
			else
				goto remove_using_list_error;
		}
		/* truncate path to original length */
                path.space[save_len] = '\0';
	} 

	closedir(&dir);
	if (is_inode_on_list(partname,list,n_list))
		if (remove_partition(partname,error) == -1)
			;
	free_path(&path);
	return 0;
remove_using_list_error:
	closedir(&dir);
	free_path(&path);
	return -1;
}

