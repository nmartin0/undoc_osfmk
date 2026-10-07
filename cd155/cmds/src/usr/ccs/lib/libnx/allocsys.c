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
 *      Copyright 1993  Intel Corporation.
 *
 *      $Id: allocsys.c,v 1.21 1995/03/22 18:43:47 carolr Exp $
 *
 */

/* History: 
 *	$Log: allocsys.c,v $
 * Revision 1.21  1995/03/22  18:43:47  carolr
 * Problem description:
 *   9756 PARAGON      OPEN      H        sdh       gregt               R1.3 WW23
 *        MESH UTILS   09-MAR-95 1        23-NOV-94 07-JUN-94 01-JAN-70
 *        repeated pspart -r sometimes core dumps
 *
 * Mandatory?:  n
 *
 * Description:
 *   allocsys.c:
 *   - construct_dir_name() function was modified.
 *     If the path name of '.compute' was handed off to this routine,
 *     all was fine.  If, however, a sub-partition of .compute (say
 *     sub1) was given, the .compute part of the pathname wasn't in
 *     the name.  The workaround for this was to use the full pathname
 *     of .compute.sub1.
 *   - removed char *sp from this routine as it is no longer used.
 *
 *  for description of changes to pspart.c, see cvs log for that file
 *
 *  Reviewer:  sdh
 *  Risk: med
 *  Benefit or PTS #: 9756
 *  Testing completed:
 *    EATS: controlc, rmcall, rmcmd, sched
 *    manual testing
 *  Modules affected:
 *     usr/bin/pspart/pspart.c
 *     usr/ccs/lib/libnx/allocsys.c
 *
 * Revision 1.20  1994/11/19  02:29:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.19  1994/01/12  18:31:05  carbajal
 *  7711 PARAGON      OPEN      M        carbajal  gregt               R1.2 WW50
 *       MESH UTILS   06-JAN-94 **       06-JAN-94 06-JAN-94
 *       Inconsistent definitions of what is a "free" node in doc and
 *       implementation.
 * Benefit: Easy to understand and matches documentation
 * Risk: Low
 * Eng: Carbajal Reviewer: cameron
 *
 * Main Issue: In computing the free space both the allocation and scheduling
 * layers were considered. While this is the right thing to do for most of the
 * R1.1 user model, it was deemed too confusing under R1.2. Instead free nodes
 * will be just those nodes that do not have applications on them.
 *
 * Revision 1.13.2.5  1994/01/12  18:07:01  carbajal
 *  7711 PARAGON      OPEN      M        carbajal  gregt               R1.2 WW50
 *       MESH UTILS   06-JAN-94 **       06-JAN-94 06-JAN-94
 *       Inconsistent definitions of what is a "free" node in doc and
 *       implementation.
 * Benefit: Easy to understand and matches documentation
 * Risk: Low
 * Eng: Carbajal Reviewer: Cameron
 *
 * Main Issue: In computing the free space both the allocation and scheduling
 * layers were considered. While this is the right thing to do for most of the
 * R1.1 user model, it was deemed too confusing under R1.2. Instead free nodes
 * will be just those nodes that do not have applications on them.
 *
 * Revision 1.13.2.4  1994/01/06  22:29:44  carbajal
 * Fixed nx_root_nodes() from core dumping if given invalid partition
 * name. Also added new names for:
 * 	nx_root_nodes()	  -> nx_part_nodes()
 * 	nx_failed_nodes() -> nx_empty_nodes()
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7303
 *  Testing: Bug report
 *  Module(s): allocsys.c
 *
 * Revision 1.13.2.3  1993/12/29  17:40:04  carbajal
 * Check for -1 to nx_app_nodes(), fill in the dimensions of nx_part_attr()
 * properly.
 *  Reviewer: John Litvin
 *  Risk: Low
 *  Benefit or PTS #: 7490,7301,7302
 *  Testing: EATs
 *  Module(s): allocsys.c
 *
 * Revision 1.13.2.2  1993/12/20  21:47:49  carbajal
 * In nx_free_node_list() check mapsize before malloc memory.
 *  Reviewer: cameron
 *  Risk: low
 *  Benefit or PTS #: 7257
 *  Testing: SATS, EATs, MUNOPS, bug test
 *  Module(s): allocsys.c
 *
 * Revision 1.13.2.1  1993/12/02  21:35:51  carbajal
 * In nx_get_node_list() call get_inode before making system call. This
 * was lost during a merge between WW47 and WW49.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Makes it work
 *  Testing:
 *  Module(s):
 *
 * Revision 1.13  1993/12/01  01:12:34  carbajal
 * Cleaned up types, extended free_map call, added a get partition names
 * with partition ids call
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.12  1993/11/22  17:41:03  carbajal
 * Removed extraneous calls to allocator, vm_dealloc buffers allocated by
 * allocator rpc.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Correctly deallocate memory
 *  Testing: rmcmd EATs
 *  Module(s):
 *
 * Revision 1.11  1993/11/18  19:24:48  dleslie
 *  Reviewer: shala
 *  Risk: low
 *  Benefit or PTS #: get nx and mcmsg headers out of export tree, not obj
 * 	tree.  This allows users to build without having an obj tree fully
 * 	populated with headers
 *  Testing: built libnx
 *  Module(s):
 *     Makefile _gcol.c _gcolx.c _gops.c _gsync.c _load.c allocUser.c
 *     allocsys.c allocsys_.c autoinit.c bitmap.c bitmap2.c create.c
 *     nodeparser.c nx_initve.c nx_load.c nx_load_.c nx_loadve.c
 *     nx_loadve_.c nx_lock.c nx_part_ops.c nx_part_ops_.c nx_port.c
 *     nx_pri.c nxlib.c parsepart.c parsesched.c partlock.c
 *     partprint.c partutils.c rkassert.c rklib.c rkmem.c utils.c
 *     writepart.c
 *
 *
 * VS:    writepart.c
 *
 * Revision 1.10  1993/11/17  21:41:58  carbajal
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: call vm_deallocate() on buffers coming from
 * 		   the allocator.
 *  Testing: rmcmd EATs
 *  Module(s):
 *
 * Revision 1.9  1993/11/17  02:40:52  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.8  1993/11/10  02:07:47  carbajal
 * Added support for R1.2 User Model allocator system calls
 *
 *  Reviewer: NONE
 *  Risk: LOW
 *  Benefit or PTS #: R1.2 User Model
 *  Testing: developer
 *  Module(s): allocsys.c, allocsys_.c, Makefile
 *
 * Revision 1.7  1993/10/26  00:33:20  carbajal
 * Repaired function declarations for 4.5 compilers.
 *
 * Revision 1.6  1993/07/18  19:51:10  carbajal
 * Changed nx_get_config_errors to use new system calls
 *
 * Revision 1.5  1993/07/13  21:27:10  carbajal
 * Changed calls to use new allocator system calls
 *
 * Revision 1.3  1993/05/26  21:38:31  carbajal
 * Changed all user visable calls and types to be prefixed by nx_
 *
 * Revision 1.2  1993/05/18  19:51:01  carbajal
 * Moved get_part_names from allocsys2.c to allocsys.c
 *
 * Revision 1.1  1993/05/11  21:05:50  carbajal
 * Allocator system calls for tools support
 *
*/


#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mode.h>
#include <sys/errno.h>
#include <sys/limits.h>
#include <sys/syscall.h>
#include <errno.h>
#include <dirent.h>
#include <malloc.h>
#include <unistd.h>
#include <strings.h>
#include <mach.h>
#include <mach_error.h>
#include <mach/message.h>
#include <mach/norma_special_ports.h>
#include <mach/mach_host.h>

#include	<mcmsg/mcmsg_appl.h>
#include	<nx/allocator.h>
#include	<nx/schedule.h>
#include	<nx/bitmap.h>
#include	<nx/defines.h>
#include	<nx/utils.h>
#include	<allocsys.h>
#include	<nx/partutils.h>
#include	<nx/alloc_types.h>

#define MAXBUF  8192    /* Allow for 1000 node numbers with a little extra */
#ifndef NULLPTR
#define NULLPTR(p)  ((char *)(p) == (char *)NULL)
#endif
 
#define NAMES_INCREMENT   32
#define PARTITION_PATH    "/etc/nx/"
 
struct names_array_info {
    char **names;
    ino_t *ids;
    int  *num_names;
    int  max_names;
};


static PARTREQ_T	_nx_partreq;
static ino_t		_nx_inode=-1;

/* 	Initialize the nx_part_info_t structure to contain
 *	default values.
 *	
 *	Parameters:
 *		partinfo	pointer to nx_part_info_t 
 *
 *	Returns:
 *		0		pointer was valid
 *		-1		bad pointer
*/
int
nx_init_partinfo(nx_part_info_t *part)
{
	if (part != (nx_part_info_t *)0){
		part->uid	= -1;
		part->gid	= -1;
		part->access 	= NO_ACCESS_SPECIFIED;
		part->sched  	= NO_SCHED_SPECIFIED;
		part->rq     	= NO_RQ_SPECIFIED;
		part->epl    	= NO_EPL_SPECIFIED;
		part->nodes	= NO_NODES_SPECIFIED;
		part->mesh_x	= -1;
		part->mesh_y	= -1;
		part->enclose_mesh_x	= -1;
		part->enclose_mesh_y	= -1;
		part->flags_or_size	= 0;
		part->part_id	= 0;
		bzero(&(part->reserved),sizeof(part->reserved));
		return(0);
	}
	else
		return(-1);
}

/* See nx_get_partition_attributes */
int
nx_part_attr(char *name,nx_part_info_t *part)
{
	return(nx_get_partition_attributes(name,part));
}

/*      Get the attributes for the partition specified in name
 *
 *      Description:
 *      Returns the attributes of a partition, see struct part_info_t.
 *
 *      Parameters:
 *              name    =       path name of partition
 *              part    =       pointer to part_info_t to be filled in by
 *                              the call
 *      Returns:
 *              0 if successful
 *              -1 on error (errno is set)
*/
int
nx_get_partition_attributes(char *name,nx_part_info_t *part)
{
	char		*tmp_name,*partname;
	ino_t		inode;
	PARTREQ_T	nx_partreq;

	partname = (char *)NULL;

	tmp_name = strdup(name);

	if (verify_name(tmp_name) < 0)
		goto error;

	dottoslash(tmp_name);

	/* Create a partition name */
	if ( (partname = create_partname(tmp_name)) == NULL)
		goto error;

	if ( (_nx_inode = get_inode(partname)) == 0)
		goto error;
	
	if (nx_part_attributes(_nx_inode,&nx_partreq) != 0)
		goto error;

	part->uid = nx_partreq.uid;
	part->gid = nx_partreq.gid;
	part->access = nx_partreq.access;
	part->sched = nx_partreq.sched;
	part->rq  = nx_partreq.rq;
	part->epl = nx_partreq.maxpri;
	part->nodes = nx_partreq.slots;
	part->flags_or_size = nx_partreq.flags;
	part->part_id = _nx_inode;

        part->mesh_x = nx_partreq.rect.cols;
        part->mesh_y = nx_partreq.rect.rows;

        /* YECH YECH YECH
         * These should be field elements in their own
         * right
        */

        part->enclose_mesh_x = nx_partreq.flags;
        part->enclose_mesh_y = nx_partreq.free;

	FREE(tmp_name);
	FREE(partname);
	return(0);
error:
	FREE(tmp_name);
	FREE(partname);
	return(-1);
}

/* See nx_get_node_list() */
int
nx_root_nodes(char *name,nx_node_list_t *map,unsigned long *mapsize)
{
	return(nx_get_node_list(name,map,mapsize));
}

/* See nx_get_node_list() */
int
nx_part_nodes(char *name,nx_node_list_t *map,unsigned long *mapsize)
{
	return(nx_get_node_list(name,map,mapsize));
}

/*      Get the root node list for the partition
 *
 *      Description:
 *      Return the logical to root node mapping for a given partition. A logical
 *      to root map is denoted by an array where the index into the array represents
 *      the logical node value and the array element represents the root node number.
 *      For instance:
 *              lp_map[10] = 500
 *      implies that logical node 10 is mapped to root node 500.
 *
 *      Parameters:
 *              name    =       pointer to pathname of partition
 *              map     =       pointer to pointer to physical root map
 *              mapsize =       size of map
 *
 *      Returns:
 *              0       =       success
 *              -1      =       error (errno is set)
*/
int
nx_get_node_list(char *name, nx_node_list_t *map,unsigned long *mapsize)
{
	nx_part_info_t	partinfo;
	LP_MAP_T	lp_map;
	int		i;
	char		*tmp_name,*partname;

	tmp_name = strdup(name);
	partname = (char *)NULL;
	lp_map = (LP_MAP_T )0;
	*mapsize = 0;
	*map = (nx_node_list_t)0;

	if (verify_name(tmp_name) < 0)
		goto error;

	dottoslash(tmp_name);

	/* Create a partition name */
	if ( (partname = create_partname(tmp_name)) == NULL)
		goto error;

	if ( (_nx_inode = get_inode(partname)) == 0)
		goto error;

	if (nx_node_list(_nx_inode,&lp_map,mapsize) != 0){
		goto error;
	}

	*map = (nx_node_list_t )MALLOC(
		sizeof(nx_node_list_element_t)* (*mapsize));
	if (*map == (nx_node_list_t) 0){ 
		errno = ENOMEM;
		goto error;
	}
	for (i = 0; i < *mapsize;i++){
		(*map)[i] = (nx_node_list_element_t)lp_map[i];
	}

	vm_deallocate(mach_task_self(),(vm_address_t ) lp_map,
		(vm_size_t) sizeof(nx_node_elem_t) * *mapsize);

	FREE(tmp_name);
	FREE(partname);
	return(0);
error:
	if (lp_map != (LP_MAP_T )0)
		vm_deallocate(mach_task_self(),(vm_address_t) lp_map,
			(vm_size_t) sizeof(nx_node_elem_t) * *mapsize);
	if (*map != (nx_node_list_t)0)
		FREE((void *)*map);
	FREE( (void *)tmp_name);
	FREE( (void *)partname);
	return(-1);
}

/*      Get the application attributes 
 *
*/
int
nx_app_attr(pid_t pgroup, nx_app_info_t *appl)
{
	if (pgroup == 0){
		if (nx_app_info(getpgrp(),appl) != 0)
			goto error;
	}
	else
	if (pgroup > 0){
		if (nx_app_info(pgroup,appl) != 0)
			goto error;
	}
	else{
		errno = EANOEXIST;
		goto error;
	}

	return(0);
error:
	return(-1);
}

/*      Get the root node list for the specified application
 *
 *      Description:
 *      Return the logical to root node mapping for a given application.
 *	A logical to root map is denoted by an array where the index into 
 *	the array represents the logical node value and the array element 
 *	represents the root node number.
 *      For instance:
 *              lp_map[10] = 500
 *      implies that logical node 10 is mapped to root node 500.
 *
 *      Parameters:
 *              name    =       pointer to pathname of partition
 *              map     =       pointer to pointer to physical root map
 *              mapsize =       size of map
 *
 *      Returns:
 *              0       =       success
 *              -1      =       error (errno is set)
*/
int
nx_app_nodes(pid_t pgroup, nx_node_list_t *lp_map,unsigned long *mapsize)
{
	int		i;
	pid_t		pgrp;
	nx_node_list_t	node_buf;

	if (pgroup == 0)
		pgrp = getpgrp();
	else
	if (pgroup > 0)
		pgrp = pgroup;
	else{
               errno = EANOEXIST;
               goto error;
	}

	*mapsize = 0;
	if (nx_app_node_list(pgrp,&node_buf,mapsize) != 0)
		goto error;
	if (*mapsize > 0){
		/* allocate space for the node list */
		*lp_map = (nx_node_list_t)
			MALLOC(sizeof(nx_node_list_element_t) * (*mapsize));
		if (*lp_map == (nx_node_list_t) 0){
			errno = ENOMEM;
			goto error;
		}
		bcopy(node_buf,*lp_map,sizeof(nx_node_elem_t) * *mapsize);
		vm_deallocate(mach_task_self(),(vm_address_t ) node_buf,
			(vm_size_t) sizeof(nx_node_elem_t) * *mapsize);
	}
	else
		*lp_map = (nx_node_list_t)0;
		
	return(0);

error:
	return(-1);
}

/*      Get the bitmap representation for the free nodes in a 
 *	partition
 *
 *      Description:
 *	Return the bitmap for the specified partition
 *
 *      Parameters:
 *              name    =       pointer to pathname of partition
 *              map     =       pointer to pointer to the bitmap
 *              mapsize =       size of map
 *
 *      Returns:
 *              0       =       success
 *              -1      =       error (errno is set)
*/
int
nx_get_free_map(int map_type,char *name, BITMAP_T **map)
{
	nx_part_info_t	partinfo;
	int		i,j;
	char		*tmp_name,*partname;
	unsigned long	mapsize;
	BITMAP_T	*map_buf;

	partname = NULL;
	*map = (BITMAP_T *)0;
	map_buf = (BITMAP_T *)0;
	*map = (BITMAP_T *)0;

	tmp_name = strdup(name);

	if (verify_name(tmp_name) < 0)
		goto error;

	dottoslash(tmp_name);

	/* Create a partition name */
	if ( (partname = create_partname(tmp_name)) == NULL)
		goto error;


	mapsize = 0;
	if ( (_nx_inode = get_inode(partname)) == 0)
		goto error;
	if (nx_free_node_list(_nx_inode,&map_buf,&mapsize) != 0)
		goto error;
	if (mapsize > 0){
		*map = (BITMAP_T *)MALLOC(mapsize);
		if (*map == (BITMAP_T *) 0){ 
			errno = ENOMEM;
			goto error;
		}
		bcopy(map_buf,*map,mapsize);
		vm_deallocate(mach_task_self(),(vm_address_t ) map_buf,
			(vm_size_t) mapsize);
	}

	return(0);

error:
	if (tmp_name != NULL)
		FREE( (void *)tmp_name);
	if (partname != NULL)
		FREE( (void *)partname);
	if (*map != (BITMAP_T *)0)
		free_bitmap(*map);
	if (map_buf != (BITMAP_T *)0)
		vm_deallocate(mach_task_self(),(vm_address_t ) map_buf,
                        (vm_size_t) mapsize);
	return(-1);
}

int
nx_free_nodes(char *name, nx_nodes_t *nodes, unsigned long *num_nodes)
{
	int		i,j;
	BITMAP_T	*map;
	unsigned long	mapsize;
	nx_nodes_t	part_nodes;

	map = (BITMAP_T *)0;
	part_nodes = (nx_nodes_t)0;
	*nodes = (nx_nodes_t )0;
	*num_nodes = 0;	

	if (nx_get_free_map(NX_APP_SPACE,name,&map) != 0)
		goto error;

	if (nx_root_nodes(name,&part_nodes,num_nodes) != 0)
		goto error;

	*nodes = (nx_nodes_t )MALLOC(*num_nodes * sizeof(nx_node_elem_t));
	if (*nodes == (nx_nodes_t )0){
		errno = ENOMEM;
		goto error;
	}

	j = 0;
	for (i = 0; i < *num_nodes; i++){
		if (getbit_bitmap(map,part_nodes[i])){
			(*nodes)[j++] = part_nodes[i];
		}
	}
	*num_nodes = j;

	free_bitmap(map);
	FREE( (void *)part_nodes);
	return(0);
error:
	if (map != (BITMAP_T *)0)
		free_bitmap(map);
	if (part_nodes != (nx_nodes_t)0)
		FREE( (void *)part_nodes);
	return(-1);
}


/*      Get the bitmap representation for the specified partition
 *
 *      Description:
 *	Return the bitmap for the specified partition
 *
 *      Parameters:
 *              name    =       pointer to pathname of partition
 *              map     =       pointer to pointer to the bitmap
 *              mapsize =       size of map
 *
 *      Returns:
 *              0       =       success
 *              -1      =       error (errno is set)
*/
int
nx_get_part_bitmap(char *name, BITMAP_T **map,unsigned long *mapsize)
{
	nx_part_info_t	partinfo;
	int		i;
	char		*tmp_name,*partname;
	BITMAP_T	*map_buf;

	*mapsize = 0;
	*map =  (BITMAP_T *)0;
	map_buf = (BITMAP_T *)0;
	
	tmp_name = strdup(name);

	if (verify_name(tmp_name) < 0)
		goto error;

	dottoslash(tmp_name);

	/* Create a partition name */
	if ( (partname = create_partname(tmp_name)) == NULL)
		goto error;

	if ( (_nx_inode = get_inode(partname)) == 0)
		goto error;

	if (nx_part_bitmap(_nx_inode,&map_buf,mapsize) != 0)
		goto error;

	*map = (BITMAP_T *)MALLOC(*mapsize);
	if (*map == (BITMAP_T *) 0){ 
		errno = ENOMEM;
		goto error;
	}
	bcopy(map_buf,*map,*mapsize);
	vm_deallocate(mach_task_self(),(vm_address_t ) map_buf,
		(vm_size_t) *mapsize);
	FREE(tmp_name);
	FREE(partname);
	return(0);
error:
	if (*map != (BITMAP_T *)0)
		FREE( (void *) (*map) );
	FREE( (void *)tmp_name);
	FREE( (void *)partname);
	return(-1);
}

/* See nx_get_failed_nodes() */
int
nx_failed_nodes(nx_node_list_t *unavail_map,unsigned long *failed_map_size)
{
	return(nx_get_failed_nodes(unavail_map,failed_map_size));
}

/*	Get the failed nodes list 
 *
 *	Description:
 *	Return the list of nodes that have failed to boot.
 *
 *	Parameters:
 *		failed_map	=	pointer to user allocated space to 
 *					contain the root node list
 *		failed_map_size =	size of failed map
 *		
 *
 *	Returns:
 *		0	=	success
 *		-1	=	error (errno is set)
 *
*/

int
nx_get_failed_nodes(nx_node_list_t *unavail_map,unsigned long *failed_map_size)
{
	char	buf[MAXBUF];
	FILE	*fd;
	int	val;
	int	i,count;

	*unavail_map = (nx_node_list_t )0;
	*failed_map_size = 0;
	count = 0;

	/* First count the number of nodes in the file */

	if ((fd = fopen("/etc/nx/.badnodes", "r")) == NULL)
		goto error;

	while ((fgets(buf, sizeof(buf), fd) != NULL))
		if (string_to_num(buf, &val) != -1) 
			count++;
	
	fclose(fd);

	if (count > 0){
		/* allocate space for bad node list */
		*unavail_map = (nx_node_list_t ) MALLOC(sizeof(unsigned long) * count);

		if (*unavail_map == (nx_node_list_t ) 0){
			errno = ENOMEM;
			goto error;
		}

		if ((fd = fopen("/etc/nx/.badnodes", "r")) == NULL)
			goto error;

		i = 0;
		while ((fgets(buf, sizeof(buf), fd) != NULL) &&
		 	(i < count)) 
			if (string_to_num(buf, &val) != -1) 
				(*unavail_map)[i++] = val;
	
		fclose(fd);
	}

	*failed_map_size = count;
	return(0);
error:
	*failed_map_size = 0;
	return(-1);
}

/* See nx_get_config_errors() */
int 
nx_config_errors(nx_node_list_t *error_map,unsigned long *error_map_size)
{
	return(nx_get_config_errors(error_map,error_map_size));
}

/* See nx_get_config_errors() */
int 
nx_empty_nodes(nx_node_list_t *error_map,unsigned long *error_map_size)
{
	return(nx_get_config_errors(error_map,error_map_size));
}

/*	Return the list of nodes that are missing because of configuration errors.
 *
 *	Description:
 *	Return the list of nodes that are missing.
 *
 *	Parameters:
 *		error_map	=	pointer to user allocated space 
 *					to contain
 *					root node list
 *		error_map_size =	size of error map
 *		
 *
 *	Returns:
 *		0	=	success
 *		-1	=	error (errno is set)
 *
*/

int
nx_get_config_errors(nx_node_list_t *error_map,unsigned long *error_map_size)
{
	BITMAP_T	*bitmap;
	BITMAP_T	*inverted_bitmap;
	int		status,count,i;
	nx_part_info_t	root_info;
	char		*root_name;
	unsigned long	bitmapCnt;

	*error_map = (nx_node_list_t)0;
	*error_map_size = 0;
	inverted_bitmap = (BITMAP_T *)0;
	bitmap = (BITMAP_T *)0;
	root_name = ".";
	
	status = -1;
	/* Get the root partition attributes */
	if (nx_get_partition_attributes(root_name,&root_info) == 0)
		/* Get the root partition bitmap information */
		if (nx_get_part_bitmap(root_name,&bitmap,&bitmapCnt) != 0)
			goto error;
		else
			status = 0;
	else
		goto error;
	
	/* Invert the root bitmap. With this done all bits that are
	 * 1 will represent missing nodes
	*/
	inverted_bitmap = invert_bitmap(bitmap);

	/* First count how many missing nodes there are */
	count = 0;
	for(i = 0; i < root_info.nodes;i++){
		if (getbit_bitmap(inverted_bitmap,i) == 1)
			count++;	
	}

	if (count > 0){
		/* allocate node space */
		*error_map = (nx_node_list_t) MALLOC( sizeof(unsigned long) * count);
		if ( *error_map == (nx_node_list_t)0){
			errno = ENOMEM;
			goto error;
		}
		/* Fill in the error map */
		for(count = 0,i = 0; i < root_info.nodes ;i++){
			if (getbit_bitmap(inverted_bitmap,i) == 1)
				(*error_map)[count++] = i;
	}
	}
	else
		*error_map = (nx_node_list_t)0;

	*error_map_size = count;

	FREE( (BITMAP_T *) bitmap);
	FREE( (BITMAP_T *) inverted_bitmap);

	return(0);
error:
	*error_map_size = 0;
	*error_map = (nx_node_list_t) 0;
	if ( bitmap != (BITMAP_T *) 0)
		FREE( (BITMAP_T *) bitmap);
	if ( inverted_bitmap != (BITMAP_T *)0)
		FREE( (BITMAP_T *) inverted_bitmap);
	return(-1);
}

/***************************************************************************
 *    
 *    Calling Sequence:
 *        construct_path_name( full_path_name, dir_name, part_name )
 *    
 *    Description:
 *        Construct a full path name given a directory name and a 
 *        name within the directory.
 *        
 *    Parameters:
 *        full_path_name - pointer to buffer to contain path name
 *        dir_name       - pointer to directory path name
 *        name           - pointer to file name to be added to dir_name
 *
 *    Returns:
 *        - none -
 *    
 **************************************************************************/
static void
construct_path_name( full_path_name, dir_name, name )
char *full_path_name;
char *dir_name;
char *name;
{
int length;

   strncpy( full_path_name, dir_name, NAME_MAX - 1 );
   length = strlen(full_path_name); 
   if ( length >= NAME_MAX ) {
       return;
   }
   if ( full_path_name[length - 1] != '/' ) {
       strcat( full_path_name, "/" );
   }
   length = (NAME_MAX - 1) - (length + 1);
   strncat( full_path_name, name, length );
}

/***************************************************************************
 *    
 *    Calling Sequence:
 *        construct_dir_name( dir_name, part_name )
 *    
 *    Description:
 *        Construct a directory name based upon a partition name.  Just
 *        replace all "." with "/".
 *        
 *    Parameters:
 *        dir_name   - pointer to directory name corresponding to partition
 *        part_name  - pointer to to partition name
 *
 *    Returns:
 *        - none -
 *    
 **************************************************************************/
static void
construct_dir_name( part_dir_name, part_name )
char *part_dir_name;
char *part_name;
{
char dir_name[NAME_MAX];
int  i;
int  length;

   /*
    * If first character of partition name is a "." then skip over it 
    * since this represents the root partiton of "/etc/nx"
    *
    *  Copy partition name to local name so that replacement of
    *  "." with "/" does not destroy original part_name.
    */

   if( *part_name == '.' ) {
     strncpy( dir_name, part_name + 1, NAME_MAX - 1 );
   } else {
	 /* we have the a portion of the sub name - need to add 'compute.'
	  * in there before attempting the full path name construction */
	 strcpy( dir_name, "compute." );
	 strcat( dir_name, part_name );
   }

#ifdef DEBUG
printf(" dir_name: %s\n", dir_name);
#endif

   /*
    *  Replace all '.' with '/' to convert partition name to directory name
    */
   length = strlen(dir_name);
   for ( i = 0; i < length; ++i ) {
       if ( dir_name[i] == '.' ) {
           dir_name[i] = '/';
       }
   }

   /*
    * Construct full path name to partition directory
    */
   construct_path_name( part_dir_name, PARTITION_PATH, dir_name );
}

/***************************************************************************
 *    
 *    Calling Sequence:
 *        new_part_name( parent_name, part_name )
 *    
 *    Description:
 *        Create a new partition name given the parent partition and the
 *        partition name.  Space is allocated via MALLOC() so that it
 *        can be freed later.
 *        
 *    Parameters:
 *        parent_name - pointer to name of partition's parent
 *        part_name   - pointer to directory partition name
 *
 *    Returns:
 *        Returns pointer to new sub partition name.
 *    
 **************************************************************************/
static char *
new_part_name( parent_name, part_name )
char *parent_name;
char *part_name;
{
char *sub_name;

   sub_name = (char *)MALLOC( strlen(parent_name) + strlen(part_name) + 1 );
   if ( !NULLPTR(sub_name) ) {
       strcpy( sub_name, parent_name ); 
       if( strcmp( ".", parent_name ) != 0 ) {
           strcat( sub_name, "." );
       }
       strcat( sub_name, part_name );
   }
   return( sub_name );
}

/***************************************************************************
 *    
 *    Calling Sequence:
 *        add_name( part_name, n )
 *    
 *    Description:
 *        Add the partition name to the list of names in the 
 *        names_arra_info structure.
 *        
 *    Parameters:
 *        part_name  - pointer to partition name to be added to names
 *        n          - pointer to names_array_info structure
 *
 *    Returns:
 *        Returns 0 if successful otherwise returns -1
 *    
 **************************************************************************/
static int
add_name( part_name, part_id, n )
char                   *part_name;
ino_t			part_id;
struct names_array_info *n;
{

   /*
    *  Make sure there is space for new charater pointer.  If not then 
    *  allocate it or increase the size of the existing names array. 
    */
   if ( *(n->num_names) >= n->max_names ) {
       n->max_names += NAMES_INCREMENT;
       n->names = (char **)realloc( n->names, (n->max_names * sizeof(char *)) );
       n->ids   = (ino_t *)realloc( n->ids, (n->max_names * sizeof(ino_t)) );
       if ( NULLPTR(n->names) ) {
           return( -1 );
       }
       if ( NULLPTR(n->ids) ) {
           return( -1 );
       }
   }

   /*
    *  Add name to names array
    */
   n->names[*(n->num_names)] = part_name;

   /*
    *  Add partition id to ids array
    */
   n->ids[*(n->num_names)] = part_id;

   *(n->num_names) += 1;

   return( 0 );
}

/***************************************************************************
 *    
 *    Calling Sequence:
 *        get_names( parent_name, parent_dir_name, name_info )
 *    
 *    Description:
 *        Get the list of all subpartition names given a partition name.
 *        
 *    Parameters:
 *        parent_name     - Pointer to parent partition name 
 *        parent_dir_name - Pointer to parent partition directory name 
 *        name_info       - pointer to names_array_info structure
 *
 *    Returns:
 *        Returns 0 if successful otherwise returns -1
 *    
 **************************************************************************/
int
get_names( parent_name, parent_dir_name, name_info )
char                   *parent_name;
char                   *parent_dir_name;
struct names_array_info *name_info;
{
DIR           *dir_pointer;
struct dirent *entry;
struct stat    sbuf; 
char           sub_dir_name[NAME_MAX];
char          *sub_name;

   /*
    *  Open partition directory
    */
   dir_pointer = opendir( parent_dir_name );
   if ( NULLPTR(dir_pointer) ) {
       return( -1 );
   }

   /*
    *  Scan thru the partition directory
    */
   while ( (entry = readdir( dir_pointer )) != (struct dirent *)NULL ) {
       /*
        *  Skip past ., .. or .partinfo
        */
       if ( (strcmp( ".", entry->d_name ) == 0) 
                   || (strcmp( "..", entry->d_name ) == 0) 
                             || (strcmp( ".partinfo", entry->d_name) ==0) ) {
           continue;
       }

       /*
        *  Construct full path name to sub partition 
        */
       construct_path_name( sub_dir_name, parent_dir_name, entry->d_name );

       /*
        * If can't stat name then just skip it
        */
       if ( stat( sub_dir_name, &sbuf ) < 0 ) {
           continue;
       }
    
       /*
        *  If it is not a directory then just skip it
        */
       if ( !S_ISDIR( sbuf.st_mode ) ) {
           continue;
       }
  
       /*
        *  Construct sub partition name
        */
       sub_name = new_part_name( parent_name, entry->d_name );
       if ( NULLPTR(sub_name) ) {
           return( -1 );
       }
           
       /*
        *  Add sub partition to list of names
        */
       if ( add_name( sub_name, sbuf.st_ino, name_info ) < 0 ) {
           return( -1 );
       }

       /*
        *  Call get_names recursively to get the subren of this partition.
        */
       if ( get_names( sub_name, sub_dir_name, name_info ) < 0 ) {
           return( -1 );
       }
   }

   /*
    *  Close the directory and return success
    */
   closedir( dir_pointer );
   return( 0 );
}

/***************************************************************************
 *    
 *    Calling Sequence:
 *        (char **) get_partition_names( part_name, num_names );
 *    
 *    Description:
 *        
 *        
 *    Parameters:
 *        part_name  - Pointer to parent partition name
 *        num_names  - Pointer to number of sub partitions found
 *
 *    Returns:
 *        Return a pointer to an array of names.
 *    
 **************************************************************************/
char **
nx_get_partition_names( part_name, num_names )
char *part_name;
int  *num_names;
{
struct names_array_info name_info;
char   part_dir_name[NAME_MAX];
int    ret;

   construct_dir_name( part_dir_name, part_name ); 
   
   name_info.max_names = *num_names = 0;
   name_info.num_names = num_names;
   name_info.names = (char **)NULL;
   name_info.ids   = (ino_t *)NULL;
   ret = get_names( part_name, part_dir_name, &name_info );
   
   return( name_info.names );
}

/***************************************************************************
 *    
 *    Calling Sequence:
 *        (char **) get_partition_names_ids( part_name, ids, num_names );
 *    
 *    Description:
 *        
 *        
 *    Parameters:
 *        part_name  - Pointer to parent partition name
 *	  ids        - Pointer to an array of ids
 *        num_names  - Pointer to number of sub partitions found
 *
 *    Returns:
 *        Return a pointer to an array of names.
 *    
 **************************************************************************/
char **
nx_get_partition_names_ids( part_name, ids, num_names )
char *part_name;
int **ids;
int  *num_names;
{
struct names_array_info name_info;
char   part_dir_name[NAME_MAX];
int    ret;

   construct_dir_name( part_dir_name, part_name ); 
   
   name_info.max_names = *num_names = 0;
   name_info.num_names = num_names;
   name_info.names = (char **)NULL;
   name_info.ids   = (ino_t *)NULL;
   ret = get_names( part_name, part_dir_name, &name_info );
   *ids = (int *)name_info.ids; 
   return( name_info.names );
}

/***************************************************************************
 *    
 *    Calling Sequence:
 *        (char **) nx_free_partition_names( char *names[], int num_names )
 *    
 *    Description:
 *        
 *        
 *    Parameters:
 *
 *    
 **************************************************************************/
void
nx_free_partition_names( char *names[], int num_names )
{
int    i;

     for ( i = 0; i < num_names; ++i ) {
         FREE( names[i] );
     }
     FREE( names );
}



