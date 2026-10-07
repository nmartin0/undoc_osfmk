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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/mkpart_rpc.c,v 1.34 1994/11/19 03:04:22 mtm Exp $
 *
 */

/* History:
 *	$Log: mkpart_rpc.c,v $
 * Revision 1.34  1994/11/19  03:04:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.33  1994/09/16  16:35:23  sdh
 *  Changed to mkpart code to adjust the nodes field if the part
 *  structure to account for empty slots. Now slots will show the
 *  total number of slots in a partition and nodes will only show
 *  the number of nodes that shoudl have boot (i.e. those listed in
 *  SYSCONFIG.TXT
 *
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 10872
 *  Testing: manual testing; EATS: rmcall, rmcmd, controlc
 *  Module(s):
 * 	mkpart_rpc.c
 * 	init_appl.c
 *
 * Revision 1.32  1994/09/07  16:56:21  sdh
 * Added a call to to mark any bad nodes as "allocated" in a layer before
 * looking for unallocated nodes to assign to the mkpart request.
 *
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 10541
 *  Testing: EATS: controlc, rmcall, rmcmd
 * 	  manual testing
 *  Module(s):
 * 	mkpart_rpc.c
 *
 * Revision 1.31  1994/07/18  20:57:41  sdh
 * If mkpart failed because of a bad node, it freed the layer struct
 * even if the layer already contained valid partitions. This caused
 * the next call to succeed and create an overlapping partition. The
 * fix is in do_mkpart().
 *
 *
 *  Reviewer: mag
 *  Risk: low
 *  Benefit or PTS #: 10185
 *  Testing: developer, EATS: rmcall, rmcmd, sched, controlc
 *  Module(s):
 * 	mkpart_rpc.c
 *
 * Revision 1.30  1994/07/09  00:16:33  mag
 * Correct count of free layers in node
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 10148, 10150
 *  Testing: developer
 *  Module(s): mkpart_rpc.c
 *
 * Revision 1.29  1994/07/06  17:34:16  mag
 * Correct -rlx on SPS partition (9954)
 * Make -nt ,,, an error (10073)
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 9954
 *  Testing: developer
 *  Module(s): (9954): misc_rpcs.c allocutils.c allocutils.h
 * 	    (9954, 10073): mkpart_rpc.c
 *
 * Revision 1.28  1994/06/13  17:13:58  sdh
 * Changed debug messages to go through debug print routine.
 *
 *  Reviewer: mag
 *  Risk: Low
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
 * Revision 1.27  1994/06/08  17:17:08  mag
 * Correct core dumps when user passes bad args to showpart
 *  Reviewer: gregt
 *  Risk: Low
 *  Benefit or PTS #: 9720
 *  Testing: EATS: rmcmd
 *  Module(s): allocator/{attribute.c, attribute.h, init_appl.c, mkpart_rpc.c}
 * 	    showpart/showpart.c
 *  Related: None
 *
 * Revision 1.26  1994/06/02  18:14:01  mag
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
 * Revision 1.25  1994/04/06  19:43:11  sdh
 * Merge of PTS# 8424 from R1.2 branch.
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #: 8424
 *  Testing:
 *  Module(s):
 *
 * Revision 1.21.2.3  1994/04/06  19:30:17  sdh
 * Moved the place where num_gang_parts gets incremented, so it is after
 * all the error checks.
 *  Reviewer: jkearns
 *  Risk: low
 *  Benefit or PTS #: 8424
 *  Testing: EATS: sched, controlc, rmcmd, rmcall
 *  Module(s): mkpart_rpc.c
 *
 * Revision 1.21.2.2  1993/12/29  17:45:08  carbajal
 * Set the rectangle dimension for the root partition
 *  Reviewer: John Litvin
 *  Risk: Low
 *  Benefit or PTS #: 7301,7302
 *  Testing: Bug report
 *  Module(s): mkpart_rpc.c
 *
 * Revision 1.21.2.1  1993/12/20  21:55:00  carbajal
 *   Be careful to vm_dealloc() memory properly. Use macros FREE and
 *   MALLOC to make sure all memory allocated is properly deallocated.
 *    Reviewer: cameron
 *    Risk: Low
 *    Benefit or PTS #: 7257
 *    Testing: SATs, EATs, MUNOPS, bug test
 *    Module(s): misc_rpcs.c, mkpart_rpc.c, schedule.c, rmpart_rpc.c
 *
 * Revision 1.22  1993/12/10  20:38:31  carbajal
 * Call vm_dealloc on any out of line data from MIG
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: Works towards fixing 7257
 *  Testing: mkpart_leak and developer testing
 * 	(this does not completely solve the problem)
 *  Module(s):
 *
 * Revision 1.21  1993/12/01  01:36:33  carbajal
 * setup for using effective ids, initialize partition's elapsed time to 0.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.20  1993/11/22  17:54:15  carbajal
 * Allow specification of EPL on SPS partitions
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: As per R1.2 Usage Model
 *  Testing: EATs
 *  Module(s):
 *
 * Revision 1.19  1993/11/18  20:23:57  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of the export tree
 *  Testing: built on Suns and 486
 *  Module(s): scripts.mk standard.mk
 *
 * Revision 1.18  1993/11/17  04:34:24  carbajal
 * When inheriting scheduling characteristics if we
 * would exceed the configuration file then default to space share.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.17  1993/11/17  02:54:43  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.16  1993/11/10  02:44:47  carbajal
 * Changes due to R1.2 Usage Model, myapp -sz HxW
 *  Reviewer: NONE
 *  Risk: LOW
 *  Benefit or PTS #: Implement R1.2 Usage Model
 *  Testing: Developer and EATs
 *  Module(s): mkpart_rpc.c
 *
 * Revision 1.15  1993/11/04  18:06:38  carbajal
 * Pass  partinfo.rect.cols * partinfo.rect.rows for the size to assign_rect_lp.
 *  Reviewer: NONE
 *  Risk: LOW
 *  Benefit or PTS #: corrects EATs failure
 *  Testing: EATs and developer
 *  Module(s): mkpart_rpc.c
 *
 * Revision 1.14  1993/11/04  17:47:14  carbajal
 * Pass number of nodes wanted into assign_rect_lp() to prevent overrunning
 * the node list with numnodes is a prime number.
 *  Reviewer: NONE
 *  Risk: LOW
 *  Benefit or PTS #: Fixes rmcmd EAT failure
 *  Testing: Developer and rmcmd EATS
 *  Module(s): allocutils.c,allocutils.h,mkpart_rpc.c,tiles.c,init_appl.c
 *
 * Revision 1.13  1993/10/27  02:01:29  carbajal
 * Look for rectangles first
 *
 * Revision 1.10.2.2  1993/09/21  22:10:11  shala
 * Added use of EAOVLP if -tile and overlap happen. PTS #6493.
 * Fixed by carbajal.
 *
 * Revision 1.10.2.1  1993/09/09  17:49:43  shala
 * Make partition directories world readable. PTS #6554. Fixed by carbajal.
 *
 * Revision 1.10  1993/08/31  17:08:36  carbajal
 * PTS #5814
 *
 * Revision 1.9  1993/08/31  01:47:46  carbajal
 * Do a chmod on the partition directory to setup proper
 * access codes. PTS #5814
 *
 * Revision 1.8  1993/08/23  17:19:30  carbajal
 * Initialize sched_list pointers in do_mkpart(). PTS #6095
 *
 * Revision 1.7  1993/07/29  00:28:34  carbajal
 * Use alloc_prev and alloc_next for allocation linked list
 * maintainence
 *
 * Revision 1.6  1993/07/22  16:57:32  carbajal
 * Initialize all pointers to null
 *
 * Revision 1.5  1993/07/21  21:31:57  carbajal
 * For STD scheduled partitions make sure the rq is set to 0
 *
 * Revision 1.4  1993/07/20  18:12:20  carbajal
 * Changed MIG_BITMAP_T to char * for clean compiling
 *
 * Revision 1.3  1993/07/13  22:21:01  carbajal
 * New allocator system call support combines part_sz and part_nd into
 * one MIG call allocator_mkpart
 *
 *
 * Revision 1.2  1993/06/16  22:59:12  carbajal
 * Enforce the OUT parameters to part_sz_allocate() to be on different
 * VM pages.
 *
 * Revision 1.1  1993/05/25  23:54:35  carbajal
 * Revision 1.1  1993/05/25  23:54:35  carbajal
 * New module dealing with mkpart RPC code was pulled out of allocator.c
 *
*/
#include <sys/dir.h>
#include <signal.h>
#include <stdio.h>
#include <sys/mode.h>

#include <mach/mach.h>
#include <mach_error.h>
#include <mach/mig_errors.h>
#include <mach/message.h>
#include <mach/norma_special_ports.h>
#include <mach/mach_host.h>
#include <sys/errno.h>
#include <assert.h>
#include <mcmsg/mcmsg_appl.h>
#include <nx/defines.h>
#include <nx/bitmap.h>
#include <nx/hash.h>
#include <nx/allocator.h>
#include <nx/schedule.h>
#include <nx/smd.h>
#include <nx/utils.h>
#include <nx/writepart.h>
#include "debug.h"
#include "macros.h"
#include "conflict.h"
#include "tiles.h"
#include "allocutils.h"

extern int
do_update_partinfo(char *pathname,ino_t inode,uid_t uid,gid_t gid);

void
do_mkpart(int init_flg,int nd_spec, LP_MAP_T nd_list, int *new_size, 
		BITMAP_T *bitmap, PARTREQ_T partinfo,
		BITMAP_T *available_nodes,
		int *error);

extern PART_T  		*root;
extern int		tile,allocation_debug,num_gang_parts,attrib_debug;
extern BITMAP_T        *unusable_node_map;
extern BITMAP_T        *bad_bitmap,*inverted_root;
extern HASH_TBL_T      part_tbl[];
extern alloc_config_t	alloc_config;

/* Forward references */
int build_partinfo(ino_t inode, PARTREQ_T *partinfo);
/*
 * Exported MIG routine to allocate a partition when
 * the -sz option of mkpart is used.
 */
kern_return_t
allocator_mkpart(serv_port,uid,gid,buffer,buf_len,num_nodes,error)
mach_port_t	serv_port;		/* Server port */
uid_t		uid;
gid_t		gid;
char		*buffer;
int		buf_len;
int		*num_nodes;
int		*error;			/* Errno */
{
	int	flag;
	int	retcode;
	
	if ((char *)buffer != NULL){
		bcopy(buffer,&flag,sizeof(int));
		if (flag == 0)
			retcode = part_nd_allocate(serv_port,uid,gid,
				uid,gid,buffer,buf_len,num_nodes,error);
		else
		if (flag == 1)
			retcode = part_sz_allocate(serv_port,uid,gid,
				uid,gid,buffer,buf_len,num_nodes,error);
		else
			*error = EPALLOCERR;
	}
	vm_deallocate(mach_task_self(),(vm_address_t) buffer,
		(vm_size_t) buf_len);

	return 0;
}


/*
 * part_sz_allocate() - Exported MIG routine to allocate a partition when
 * the -sz option of mkpart is used.
 */
kern_return_t
part_sz_allocate(serv_port,uid,gid,euid,egid,buffer,buf_len,num_nodes,error)
mach_port_t	serv_port;		/* Server port */
uid_t		uid;
gid_t		gid;
uid_t		euid;
gid_t		egid;
char 		*buffer;
int		buf_len;
int		*num_nodes;
int		*error;			/* Errno */
{

	BITMAP_T	*bitmap;	/* Ptr to malloc()'d bitmap */
	LP_MAP_T	lp;		/* Ptr to malloc()'d node list */
	int		nd_spec;	/* RECT or SIZE */
	int		new_size;
	PART_T		*part;
	PARTREQ_T	partinfo;
	char		*pathname;
	int		flag,status;
	int		selector_cnt;
	char*		selector_buf;
	int		selector_buflen;
	BITMAP_T*	selector_mask;
	BITMAP_T*	available_nodes;
	BITMAP_T*	free_nodes;
	PART_T*		parent;
	
	/* initialize all pointers to null */
	bitmap = (BITMAP_T *)0;
	pathname = (char *)0;
	part = (PART_T *)0;
	lp = (LP_MAP_T)0;
	selector_mask = (BITMAP_T *)0;
	available_nodes = (BITMAP_T *)0;

	*num_nodes = 0;
	if (unmarshall_mkpart_rpc(buf_len,buffer,&flag,&pathname,
				&partinfo,&bitmap,
				&lp,&new_size, &selector_cnt,
				&selector_buf, &selector_buflen ) == -1)
		goto internal_error;

        if (BuildMatchingBitmap(selector_cnt, selector_buf,
          0, &selector_mask) == -1) {
                *error = EINVAL;
                goto internal_error;
        }

	if (root != (PART_T*) 0) {
	    parent = HASH_LOOKUP_PART(partinfo.parent_inode);
	    if (parent == (PART_T*) 0)
		goto internal_error;
	} else
	    parent = (PART_T*) 0;

	if (parent != (PART_T*) 0) {
	    if (selector_mask != (BITMAP_T*)0)
		available_nodes = AND_bitmaps(parent->bitmap, selector_mask);
	    else
		available_nodes = bitmap_clone(parent->bitmap);
	    if (parent->sched == SPACE_SHARE) {
		free_nodes = overlay_layers(parent->child_alloc_lyr);
		if (free_nodes) {
		    bitmap_mask(free_nodes, available_nodes);
		    FREE(free_nodes);
		}
	    }
	}
	    

	/* Look up the parent partition and check to see if we have permission
	 * to make partitions
	*/
	if ( (part = validate_allocator_access(partinfo.parent_inode,
			partinfo.parent_inode,
			euid,egid,WRITE,error)) == (PART_T *)0  ){
		/* Permission has been denied */
		goto internal_error;
	}

	partinfo.uid = uid;
	partinfo.gid = gid;

	debug_allocation(5,"part_sz_allocate ");
	debug_allocation(5,"rows=%d cols=%d \n", partinfo.rect.rows, partinfo.rect.cols);


	/*
	 * Allocate a bitmap and logical to physical node map using malloc()
	 * rather than vm_allocate() so that we can use free() to deallocate it
	 * along with the internal data structures that will point to them.
	 * 
	 * The bitmap is malloc()'d in clone_bitmap(). We call this
	 * to make a copy of the root partition so that we have a
	 * bitmap of the right size.
	 */
	bitmap = (BITMAP_T *) 0;
	if ((bitmap = bitmap_clone(root->bitmap)) == (BITMAP_T *) 0) {
	if (bitmap != (BITMAP_T *)0)
		free_bitmap(bitmap);

		*error = EPALLOCERR;	/* Allocator internal error */
		goto internal_error;
	}
        init_bitmap(0, bitmap);
	
	/*
	 * Determine what flavor of node specification was used.
	 */
	if ((partinfo.rect.rows == RECTROWSNOTSET) ||
	    (partinfo.rect.cols == RECTCOLSNOTSET)) {
		nd_spec = SIZE;
		debug_allocation(5,"SIZE\n");
		new_size = partinfo.slots;
		if (new_size != SIZEANY && partinfo.relaxed) {
		    int i = num_nodes_in_map(available_nodes);
		    new_size = MIN(new_size, i);
		}
		partinfo.nodes = new_size;
		partinfo.slots = new_size;
	}
	else {
		if (partinfo.relaxed) {
		    *error = EINVAL;
		    goto internal_error;
		}
		if (partinfo.rect.rows > MAX_ROWS) {
		    *error = EPXRS;;
		    goto internal_error;
		}
		nd_spec = RECT;
		debug_allocation(5,"RECT\n");
		new_size = partinfo.rect.rows * partinfo.rect.cols;
		partinfo.nodes = new_size;
		partinfo.slots = new_size;
	}

	if (lp != (LP_MAP_T)0)
		FREE((void *)lp);

	lp = (LP_MAP_T) MALLOC(new_size * sizeof(LP_MAP_ENTRY_T));
	if (lp == (LP_MAP_T) 0) {
		*error = EPALLOCERR;	/* Allocator internal error */
		goto internal_error;
	}

	CSECT_ENTER
	/* create the partition directory now */
	status = mkdir(pathname,0755);
	if (status < 0){
		if (errno == EEXIST)
			*error = EPPARTEXIST;
		goto internal_error;
	}
	/* Make sure the permissions are set on the partition directory */
        if (chmod(pathname,S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH) != 0){
		*error = errno;
		goto internal_error;
	}

	partinfo.inode = get_inode(pathname);

	/*
	 * Call common routine to actually make the partition.
	 */
	do_mkpart(FALSE,nd_spec, lp, &new_size, bitmap, partinfo,
	  available_nodes, error);
	if (*error == ESUCCESS) {
		if (build_partinfo(partinfo.inode,&partinfo) == -1){
			*error = EPALLOCERR;
			goto internal_error;
		}
		/*
		 * Partition creation succeeded.
		 */
		if ( write_partinfo(pathname,&partinfo,bitmap,lp) == FALSE){
			status = rmdir(pathname);
			*error = errno;
			goto internal_error;
		}
		CSECT_EXIT
		*num_nodes = new_size;
		FREE(pathname);
		*error = 0;
		return 0;
	}
	else{
		debug_allocation(5,"part_sz_allocate error %d\n",*error);
		rmdir(pathname);
		CSECT_EXIT
	}
		
internal_error:
	if (bitmap)
		free_bitmap((void *) bitmap);
	if (lp)
		FREE((void *) lp);
	if (pathname != NULL)
		FREE((void *)pathname);

	if (selector_mask)
	    FREE((void*)selector_mask);

	if (available_nodes)
	    FREE((void*)available_nodes);

	return 0;
}

/*
 * part_nd_allocate() - Exported MIG routine to allocate a partition when
 * the -nd option of mkpart is used.
 */
kern_return_t
part_nd_allocate(serv_port, uid, gid, euid, egid,
		buffer, buf_len, num_nodes, error)
mach_port_t	serv_port;		/* Server port */
uid_t		uid;
gid_t		gid;
uid_t		euid;
gid_t		egid;
char		*buffer;
int		buf_len;
int		*num_nodes;
int		*error;			/* Errno */
{

	BITMAP_T	*bitmap;	/* Ptr to malloc()'d bitmap */
	LP_MAP_T	lp;		/* Ptr to malloc()'d node list */
	int		new_size;
	PART_T		*part;
	LP_MAP_T	nd_list;	/* Array of nodes to allocate */
	unsigned int	nd_list_size;	/* Number of bytes in nd_list */
	char		*mig_bitmap;	/* Bitmap of nodes to allocate */
	unsigned int	mig_bitmap_size;/* Size of bitmap in bytes */
	PARTREQ_T	partinfo;	/* Partition information */
	int		flag,status;
	char		*pathname;
	int		selector_cnt;
	char*		selector_buf;
	int		selector_buflen;
	BITMAP_T*	selector_mask;
	BITMAP_T*	available_nodes;
	BITMAP_T*	free_nodes;
	PART_T*		parent;

	/* initialize all pointers to null */
	bitmap = (BITMAP_T *)0;
	lp = (LP_MAP_T)0;
	part = (PART_T *)0;
	mig_bitmap = (char *)0;
	pathname = (char *)0;
	selector_mask = (BITMAP_T*)0;
	available_nodes = (BITMAP_T*)0;

	if (unmarshall_mkpart_rpc(buf_len,buffer,&flag,
				&pathname,&partinfo,&mig_bitmap,
				&nd_list,&nd_list_size, &selector_cnt,
				&selector_buf, &selector_buflen) == -1)
		goto internal_error;

        if (BuildMatchingBitmap(selector_cnt, selector_buf,
          0, &selector_mask) == -1) {
                *error = EINVAL;
                goto internal_error;
        }

	if (root != (PART_T*) 0) {
	    parent = HASH_LOOKUP_PART(partinfo.parent_inode);
	    if (parent == (PART_T*) 0)
		goto internal_error;
	} else
	    parent = (PART_T*) 0;

	if (parent != (PART_T*) 0) {
	    if (selector_mask != (BITMAP_T*)0)
		available_nodes = AND_bitmaps(parent->bitmap, selector_mask);
	    else
		available_nodes = bitmap_clone(parent->bitmap);
	    if (parent->sched == SPACE_SHARE) {
		free_nodes = overlay_layers(parent->child_alloc_lyr);
		if (free_nodes) {
		    bitmap_mask(free_nodes, available_nodes);
		    FREE(free_nodes);
		}
	    }
	}

	*num_nodes = 0;
	/* Look up the parent partition and check to see if we have permission
	 * to make partitions
	*/
	if ( (part = validate_allocator_access(partinfo.parent_inode,
				partinfo.parent_inode,euid,egid,
				WRITE,error)) == (PART_T *)0  ){
		/* Permission has been denied */
		goto internal_error;
	}

	partinfo.uid = uid;
	partinfo.gid = gid;

	debug_allocation(5,"ALLOCATOR : IN PART_ND_ALLOCATE\n" );
	bitmap = (BITMAP_T *) 0;
	lp = (LP_MAP_T) 0;

	/*
	 * Call bitmap_clone() to make an exact copy of mig_bitmap except
	 * that it the memory has been malloc()'d rather than vm_allocate()'d
	 * so that we can use free() to deallocate it along with the internal
	 * data structures that will point to it.
	 */
	if ((bitmap = bitmap_clone((BITMAP_T *)mig_bitmap)) == (BITMAP_T *) 0) {
		*error = EPALLOCERR;	/* Allocator internal error */
		goto internal_error;
	}


	/*
	 * If relaxed, remove non-matching nodes from the bitmap
	 * We also need to remove nodes from the map that don't match,
	 * and correct the size of the map.
	 */

	if (partinfo.relaxed) {
	    int src, dst;

	    if (bitmap_mask(available_nodes, bitmap) == -1) {
		*error = EPALLOCERR;	/* Allocator internal error */
		goto internal_error;
	    }

	    for (src=0, dst=0;
	      src<(nd_list_size/sizeof(LP_MAP_ENTRY_T)); src++)
		if (getbit_bitmap(bitmap, nd_list[src]))
		    nd_list[dst++] = nd_list[src];

	    nd_list_size = dst*sizeof(LP_MAP_ENTRY_T);
	    partinfo.nodes = dst;
	    partinfo.slots = dst;

	    if (nd_list_size == 0) {
		*error = EPXRS;
		goto internal_error;
	    }

	/*
	 * If not relaxed, check for node that don't match the selector
	 */

	} else {
	    if (!bitmap_space_there(bitmap, available_nodes)) {
		*error = EPNOMATCH;
		goto internal_error;
	    }
	}

	/*
	 * Allocate a logical to physical node map using malloc() for same
	 * reason as above. Use bcopy() to copy node map values.
	 */
	new_size = nd_list_size / sizeof(LP_MAP_ENTRY_T);

	lp = (LP_MAP_T) MALLOC(nd_list_size);

	if (lp == (LP_MAP_T) 0) {
		*error = EPALLOCERR;	/* Allocator internal error */
		goto internal_error;
	}
	bcopy((void *) nd_list, (void *) lp, nd_list_size);

	/* Free data malloc'd from unmarshall_mkpart() call */
	if (nd_list != (LP_MAP_T)0)
		FREE((void *)nd_list);
	if (mig_bitmap != (char*)0)
		FREE((void *)mig_bitmap);

	CSECT_ENTER

	/* create the partition directory now */
	status = mkdir(pathname,0755);
	if (status < 0){
		if (errno == EEXIST)
			*error = EPPARTEXIST;
		else
			*error = errno;
		goto internal_error;
	}
	/* Make sure the permissions are set on the partition directory */
        if (chmod(pathname,S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH) != 0){
		*error = errno;
		goto internal_error;
	}

	partinfo.inode = get_inode(pathname);

	/*
	 * Call common routine to actually make the partition.
	 */
	do_mkpart(FALSE,EXACT, lp, &new_size, bitmap, partinfo,
	  available_nodes, error);

	*num_nodes = new_size;

internal_error:
	if (*error != ESUCCESS) {
		status = rmdir(pathname);
		/*
		 * Error making partition, free allocated memory.
		 */
		if (bitmap)
			free_bitmap((void *) bitmap);
		if (lp)
			FREE((void *) lp);
	}
	else{
		if (build_partinfo(partinfo.inode,&partinfo) == -1){
			/* XXX need to free bitmap and lp map */
			status = rmdir(pathname);
			*error = EPALLOCERR;
		}
		else
		if ( write_partinfo(pathname,&partinfo,bitmap,lp) == FALSE){
			/* XXX need to free bitmap and lp map */
			*error = errno;
			status = rmdir(pathname);
		}
		*error = 0;
	}
	CSECT_EXIT

	if (pathname != NULL)
		FREE((void *)pathname);

	if (selector_mask)
	    FREE((void*)selector_mask);

	if (available_nodes)
	    FREE((void*)available_nodes);

	return 0;
}

/*
 * do_mkpart() - Allocate nodes for partition. Common routine for both -sz and
 * -nd versions of mkpart.
*/
void
do_mkpart(int init_flg,int nd_spec, LP_MAP_T nd_list, int *new_size, 
		BITMAP_T *bitmap, PARTREQ_T partinfo, 
		BITMAP_T *available_nodes, int *error)
/*
init_flg	if TRUE means we are being called from init_part
nd_spec		Type of node specification; i.e.,
		SIZE (-sz n), RECT (-sz RxC),
		EXACT (-nd n..m). 
nd_list		Array of nodes to allocate (or allocated) 
*new_size
*bitmap		Bitmap of nodes to allocate (or allocated) 
partinfo	Partition information
*error		Errno 
*/

{

	PART_T		*parent;	/* Parent partition */
	PART_T		*child;		/* New partition */
	int		row, col;	/* For bitmap_find_space() */
	BITMAP_T	*rect;		/* Bitmap for RECT case */	
	BITMAP_T	*good;		/* Bitmap for good nodes */
	BITMAP_T	*bptr;		
	BITMAP_T	*available_in_layer;		
	LAYER_T		*layer;
	int		default_szflg;
	int		strictly_rect,foundit,width,height;
	int		nrows,ncols;
	int		num_free;
	int		new_layer;
        BITMAP_T        *missing_bitmap;
        int             num_missing_nodes;

	/* initialize all pointers to null */
	parent = (PART_T *)0;
	child = (PART_T *)0;
	rect = (BITMAP_T *)0;
	good = (BITMAP_T *)0;
	bptr = (BITMAP_T *)0;
	layer = (LAYER_T *)0;

	debug_allocation(5,"do_mkpart() partinfo.inode %d sched %d\n",partinfo.inode,partinfo.sched);

	default_szflg = 0;

	/*
	 * If RECT form of node specification was used we will allocate
	 * a bitmap structure for use in the bitmap_find_space() call.
	 */
	if (nd_spec == RECT) {
		if ((rect = allocate_bitmap(partinfo.rect.cols,
		                partinfo.rect.rows)) == (BITMAP_T *) 0) {
			debug_allocation(5,"can't allocate bitmap %d\n",errno);
			goto internal_error;
		}
		init_bitmap(1, rect);

		if (allocation_debug){
		    debug_allocation(3, "alloc and init rect bitmap\n");
		    debug_print_bitmap(DEBUG_ALLOC, rect);
		}

	}
	else {
		rect = (BITMAP_T *) 0;
	}

	/*
	 * Allocate new partition.
	 */
	if ((child = ALLOC_PART()) == (PART_T *) 0) {
		goto internal_error;
	}

	/* Look up the parent partition as long as we are not the root 
	 * partition, and there is a root partition.
	*/
	if (root != (PART_T *)0 ){
		parent = HASH_LOOKUP_PART(partinfo.parent_inode);
		if (parent == (PART_T *)0)
			/* Can't seem to find the parent */
			goto internal_error;
	}
	else
		parent = (PART_T *)0;

	if (available_nodes == (BITMAP_T*)0 && parent != (PART_T*)0)
		available_nodes = parent->bitmap;

	if (alloc_config.num_of_gang_parts > 0){
		/* we have a limit on the number of gang scheduled
		 * partitions that we will allow
		*/
		if ( (partinfo.sched == GANG) || 
			(partinfo.sched == NO_SCHED_SPECIFIED &&
				parent->sched == GANG)){  
			/* We want to make a gang scheduled partition,
		 	* make sure that we do not exceed our limit
			*/
				if (num_gang_parts == alloc_config.num_of_gang_parts){
					/* The only indication we have about gang scheduled
					 * partition is the specification of an rq
					*/
					if ( partinfo.rq != NO_RQ_SPECIFIED     ||
			     		     partinfo.sched != NO_SCHED_SPECIFIED  ){
						/* We would exceed our limit, so error out */
						*error = EEXCEEDCONF;
						goto internal_error1;
					}
					else{
						/* we just want to inherit from the parent
					 	* but doing so would violate the config
				 		* file setup. Let's be nice and just set
				 		* things to space share
						*/
						partinfo.sched = SPACE_SHARE;
					}
			}
		}
	}

	if (partinfo.rq != NO_RQ_SPECIFIED)
		if ( (alloc_config.min_rq > 0) && (partinfo.rq > 0) )
			if (partinfo.rq < alloc_config.min_rq){
				*error = ESCHEDCONF;
				goto internal_error1;
			}

	/*
	 * Initialize new partition.
	 */
	child->type = PART;
	child->bitmap = bitmap;
	child->lp = nd_list;
	child->slots = partinfo.slots;
	child->nodes = partinfo.nodes;
	/* See if an epl was specified */
	if (partinfo.maxpri != NO_EPL_SPECIFIED)
		/* Yes, so use the specified one */
		child->max_priority = partinfo.maxpri;
	else
		/* No, inherit it from the parent */
		child->max_priority = parent->max_priority;	
	child->cur_priority = NO_PRI;
	child->inode = partinfo.inode;
	if (partinfo.sched != NO_SCHED_SPECIFIED)
		/* Yes, so use the specified one */
		child->sched = partinfo.sched;
	else
		/* No, inherit it from the parent */
		child->sched = parent->sched;
	if (partinfo.rq != NO_RQ_SPECIFIED)
		/* Yes, so use the specified one */
		child->rollin_quantum = partinfo.rq;
	else
		/* No, inherit it from the parent */
		child->rollin_quantum = parent->rollin_quantum;
	child->rolled_in = 0;
	child->elapsed = 0;
	child->owner = partinfo.uid;
	child->group = partinfo.gid;
	if (partinfo.access != NO_ACCESS_SPECIFIED)
		/* Yes, so use the specified one */
		child->protection = partinfo.access;
	else
		/* No, inherit it from the parent */
		child->protection = parent->protection;
	child->active_lyr = (LAYER_T *) 0;
	child->child_sched_lyr = (LAYER_T *) 0;
	child->child_alloc_lyr = (LAYER_T *) 0;
	child->parent_sched_lyr = (LAYER_T *) 0;
	child->parent_alloc_lyr = (LAYER_T *) 0;
	child->alloc_prev = (PART_T *)0;
	child->alloc_next = (PART_T *)0;
	child->sched_list = (LAYER_T *)0;
	child->conflict.list_size = 0;
	child->conflict.n_list = 0;
	child->conflict.free_index = 0;
	child->conflict.list = NULL;
	/* Make UNIX partitions have a really high EPL */
	if (child->sched == UNIX){
		child->max_priority = STD_PRI;
		child->rollin_quantum  = 0;
		/* A standard scheduled layer has no layer limits */
		child->max_num_layers = -1;
		child->gang_ancestor = FALSE;
	}
	else{
		/* Make space shared partition always have  
	 	* an rq of 0
		*/
		if (child->sched == SPACE_SHARE){
			child->rollin_quantum  = 0;
			/* We are a space shared partition, see if we have a gang scheduled
			 * ancestor someplace
			*/
			if (parent != (PART_T *)0)
				child->gang_ancestor = parent->gang_ancestor;
		}
		else{
			/* We are a gang scheduled partition, so set gang_ancestor to
			 * true. This is to control the use of -plk
			*/
			child->gang_ancestor = TRUE;
		}
		child->max_num_layers = alloc_config.degree_of_overlap;
	}

	/* IF SPACE_SHARE==1 in the configuration file then force all 
 	 * gang scheduled partitions to be space shared
	*/
	if (alloc_config.space_share && child->sched == GANG){
		child->sched = SPACE_SHARE;
		child->rollin_quantum = 0;
		child->gang_ancestor = FALSE;
	}

	child->status = PART_ROLLED_OUT;

	if (allocation_debug){
	    debug_allocation(3, "New partition:\n");
	    debug_print_bitmap(DEBUG_ALLOC, child->bitmap);
	}

	if (root == (PART_T *) 0) {
		/*
		 * This is the first partition, make it the root partition.
		 */
		root = child;
		layer = (LAYER_T *) 0;
		child->parent_alloc_lyr = layer;
		if (init_flg){
			inverted_root = invert_bitmap(root->bitmap);
			if (allocation_debug){
				debug_allocation(5,"Inverted root bitmap\n");
				debug_print_bitmap(DEBUG_ALLOC, inverted_root);
			}
			bad_bitmap = build_badbitmap(root->slots,root->bitmap);
			if (bad_bitmap != (BITMAP_T *)0){
				if (allocation_debug){
					debug_allocation(5,"bad node bitmap\n");
					debug_print_bitmap(DEBUG_ALLOC, bad_bitmap);
				}
				/* Add the invert root bitmap nodes to the bad
				 * bitmap
				*/
				inverted_root = invert_bitmap(root->bitmap);
				unusable_node_map = or_bitmaps(inverted_root,bad_bitmap);
			}
			else
				/* There are no bad nodes just use the inverted
				 * root bitmap
				*/
				unusable_node_map = invert_bitmap(root->bitmap);

			if (unusable_node_map != (BITMAP_T *)0)
				if (allocation_debug){
					debug_allocation(5,"unusable node bitmap\n");
					debug_print_bitmap(DEBUG_ALLOC, unusable_node_map);
				}

			/* Ok, now we have knowledge of the missing nodes, so we 
			 * can go ahead and turn on all the bits in the root
			 * bitmap
			*/
			bptr = root->bitmap;
			root->bitmap = or_bitmaps(bptr,inverted_root);
			free_bitmap(bptr);	

			nrows = 0;
			ncols = 0;
			if (bitmap_encl_rect(root->bitmap,&col,&row,&width,&height)){
				if ( (col != -1) && (row != -1))
					if (width * height == partinfo.nodes){
						/* we have a contiguous rectangle */
						nrows = height;
						ncols = width;
						debug_allocation(5,"nrows %d ncols %d\n",nrows,ncols);
					}
			}
		}
	}
	else {
               if (parent->slots != parent->nodes) {
                    /*
                     * This means that there are some slots that
                     * do not have useable nodes in them. We need to
                     * find out if these nodeless slots are in this
                     * partition and adjust the node count if they are.
                     */
		   debug_allocation(5, "Adjusting for nodes without slots\n");
		   missing_bitmap = AND_bitmaps(inverted_root, child->bitmap);
		   num_missing_nodes = num_nodes_in_map(missing_bitmap);
		   if (child->slots == child->nodes) {
		       /*
			* No empty slots have been accounted for, so acount for all 
			* of them.
			*/
		       child->nodes -= num_missing_nodes;
		       debug_allocation(5, "Adjusting for all empty slots\n");
		   } else if ((child->slots - child->nodes) != num_missing_nodes) {
		       /* 
			* Some of the empty slots are have been accounted for in the
			* partinfo file, so just adjust for the new ones.
			*/
		       child->nodes -= num_missing_nodes - (child->slots - child->nodes);
		       debug_allocation(5, "Adjusting for some empty slots\n");
		   } else {
		       debug_allocation(5, "Empty slots already accounted for in partinfo\n");
		   }
                }

		if(parent->sched == UNIX)    /* Parent is Unix scheduled */
		{
			/* A STD scheduled partition can not have any 
			 * subpartitions
			*/
			goto internal_error;
		}

		/*
		 * If default size is used, set partinfo to reflect parent's
		 * size
		 */

		if (partinfo.nodes == SIZEANY) {
			assert(available_nodes != (BITMAP_T*)0);

			partinfo.nodes = num_nodes_in_map(available_nodes);
			partinfo.slots = partinfo.nodes;
		} 

		/* If the init_flg is TRUE then we will skip any checks here
		 * to make sure the partition we are making fits into the 
		 * parent. This check will be done at init_appl time.
		*/
		if (!init_flg){
			/*
		 	* Make sure that there is room in parent.
			*/
			assert(available_nodes != (BITMAP_T*)0);
			if (nd_spec == EXACT){
				assert(bitmap != (BITMAP_T *)0);
			}
			else
			if (nd_spec == RECT)
				assert(rect != (BITMAP_T *)0);
			if ((nd_spec == EXACT) &&
		     	  (!bitmap_space_there(bitmap, available_nodes)))
			{
				*error = EPXRS; /* Exceeds part resources */
				goto internal_error1;
			}
		    	if ((nd_spec == RECT) &&
		     	(!bitmap_find_space(rect, available_nodes, &col, &row)))
			{
				*error = EPXRS; /* Exceeds part resources */
				goto internal_error1;
			}

			/* see if we have any unusable nodes */
			if (unusable_node_map != (BITMAP_T *)0){
				/*
		 		* Make sure that there are enough good nodes in the
		 		* parent 
		 		*/
				good = bitmap_clone(available_nodes);
				assert(good != (BITMAP_T *)0);
				if (allocation_debug){
					debug_allocation(5,"available nodes \n");
					debug_print_bitmap(DEBUG_ALLOC, good);
				}
				bitmap_allocate_space(unusable_node_map,good);
				if (allocation_debug){
					debug_allocation(5,"available nodes w/o unusable nodes\n");
					debug_print_bitmap(DEBUG_ALLOC, good);
				}
				if ( ((nd_spec == EXACT) &&
		     		( (!bitmap_space_there(bitmap, good)) ||
					/* XXX Do we still need to check root->bitmap???? */
				(!bitmap_space_there(bitmap,root->bitmap))) )
		  		||
		    		((nd_spec == RECT) &&
		     		(!bitmap_find_space(rect, good, &col, &row))))
				{
					*error = EPBXRS; /* Exceeds partition resources */
					free_bitmap((void *) good);
					goto internal_error1;
				}
				else
					free_bitmap((void *) good);
			}
		} /* if !init_flg */	
 
		if (nd_spec == SIZE) {
		    if (partinfo.nodes > num_nodes_in_map(parent->bitmap))
			/* command and allocator are out of sync */
			goto internal_error;
		    if (partinfo.nodes > num_nodes_in_map(available_nodes)) {
			*error = EPXRS; /* exceeds part resources */
			goto internal_error1;
		    }
		}

		/*
		 * Find first allocation layer with room in it. 
		 * Layers are in order of least free space to 
		 * most free space.
		 */
		strictly_rect = TRUE;
		foundit = FALSE;
		do{
		debug_allocation(5,"start search thru layers\n");
		layer = parent->child_alloc_lyr;
		while (layer != (LAYER_T *) 0) {
			good = (BITMAP_T*) 0;
			debug_allocation(5,"looking at layer 0x%x\n",layer);
			if (!init_flg){
				if (available_nodes != (BITMAP_T*)0) {
				    good = AND_bitmaps(layer->bitmap, available_nodes);
				} else {
				    good = bitmap_clone(layer->bitmap);
				}
				assert(good != (BITMAP_T *)0);
	
				/* See if we have any bad nodes */
				if (unusable_node_map != (BITMAP_T *)0) {
					/*
					* Mark the bad nodes as allocated in the good
					* bitmap, so they don't get allocated
					*/
					bitmap_allocate_space(unusable_node_map,good);
				}

				num_free = num_nodes_in_map(good);
				bptr = good;
			}
			else
				bptr = layer->bitmap;
			if (allocation_debug){
				debug_print_bitmap(DEBUG_ALLOC, bptr);
				if ((nd_spec == SIZE))
					debug_allocation(5,"partinfo.nodes %d num_free %d\n",partinfo.nodes,
							num_free);
			}
			assert(bptr != (BITMAP_T *)0);
			if (((nd_spec == EXACT) &&
			     (bitmap_space_there(bitmap, bptr)))
			 ||
				/* if we are looking for size nodes then
				 * first try to find a rectangle and if 
				 * that fails try just any size nodes
				*/
			    ( (nd_spec == SIZE) &&
			      (partinfo.nodes <= num_free) &&
			      (bitmap_find_size(partinfo.nodes,bptr,&row,&col,
						&width,&height) || 
				(strictly_rect == FALSE) )) 
			 ||
			    ((nd_spec == RECT) &&
			     (bitmap_find_space(rect, bptr,
			                        &col, &row))))
			{
				foundit = TRUE;
				break;
			}
			layer = layer->next;
			if (good != (BITMAP_T*)0)
				free_bitmap((void *)good);
		}
		if (foundit)
			break;
		if (strictly_rect == TRUE)
			strictly_rect = FALSE;
		else
			strictly_rect = TRUE;
		} while (strictly_rect != TRUE);

		if (layer != (LAYER_T *) 0) {
			new_layer = FALSE;
			/*
			 * We found a layer with room. Take layer out of list so
			 * we can insert it back in its proper place.
			 */
			assert(parent->child_alloc_lyr != (LAYER_T *)0);
			REMOVE(parent->child_alloc_lyr, layer);
		} else {
			new_layer = TRUE;
			/*
			 * We didn't find a layer with room, so we will allocate
			 * a new one and clone bitmap of the parent partition.
			 */
			if (deny_overlap(parent,parent->child_alloc_lyr)){
                                /*
                                 * We would overlap, return error.
                                 */
                                *error = EAOVLP;
				/* Request overlaps with node in use */
                                goto internal_error1;
                        }

			debug_allocation(5,"Need to allocate a new layer\n");
			if ((layer = ALLOC_LAYER()) == (LAYER_T *) 0) {
				goto internal_error;
			}
			assert(parent->bitmap != (BITMAP_T *)0);
			layer->bitmap = bitmap_clone(parent->bitmap);
			if (layer->bitmap == (BITMAP_T *) 0) {
				goto internal_error;
			}
			if (allocation_debug){
				debug_allocation(5,"allocate new layer clone parent\n");
				debug_print_bitmap(DEBUG_ALLOC, layer->bitmap);
			}
			layer->part = parent;
			layer->priority = NO_PRI;
			layer->num_free = parent->nodes;
			layer->consumer = (CONSUMER_T *) 0;
		}

		/*
		 * At this point we have a layer that has room in it for the
		 * nodes specified in bitmap. We will allocate the nodes, update
	 	 * the count of free nodes, and then insert it in the proper
		 * place in the list of layers.
		 */

		available_in_layer = AND_bitmaps(layer->bitmap, available_nodes);

		if (allocation_debug){
			debug_allocation(5,"allocation layer 0x%x\n",layer);
			debug_allocation(5,"Layer before allocate:\n");
			debug_print_bitmap(DEBUG_ALLOC, layer->bitmap);
			debug_allocation(5,"Available nodes from layer:\n");
			debug_print_bitmap(DEBUG_ALLOC, available_in_layer);
		}
		child->parent_alloc_lyr = layer;
		if (nd_spec == SIZE) {
			*new_size = select_nodes(FALSE,default_szflg,partinfo.nodes, 
				available_in_layer,bitmap,parent->lp, parent->slots,
				child->lp,&nrows,&ncols);
			if (*new_size == -1){
				*error = EPBXRS; /* Exceeds partition resources */
				goto internal_error1;
			}

		}
		else if (nd_spec == RECT) {
			if (allocation_debug){
				debug_allocation(5,"rect bitmap before last find space:\n");
				debug_print_bitmap(DEBUG_ALLOC, rect);
			}
			good = (BITMAP_T*) 0;
			if (!init_flg){
				/* see if we have any bad nodes */
				if (unusable_node_map != (BITMAP_T *)0){
					good = bitmap_clone(available_in_layer);
					bitmap_allocate_space(unusable_node_map,good);
					bptr = good;
				}
				else
					bptr = available_in_layer;
			}
			else
				bptr = available_in_layer;
			bitmap_find_space(rect, bptr, &col, &row);
			bitmap_fill_rect(bitmap, 1, col, row,
			                 partinfo.rect.cols,
			                 partinfo.rect.rows);
			assign_rect_lp(bitmap, 
				partinfo.rect.cols*partinfo.rect.rows,
				col, row, 
				partinfo.rect.cols,partinfo.rect.rows, 
				child->lp);
			if (good != (BITMAP_T*) 0)
				free_bitmap((void *)good);
			nrows = partinfo.rect.rows;
			ncols = partinfo.rect.cols;
		}
		else{
			/* nd_spec == EXACT */
			/* see if we have a rectangle */
			debug_allocation(5,"nd_spec == EXACT check for rectangle\n");
			nrows = 0;
			ncols = 0;
			if (bitmap_encl_rect(bitmap,&col,&row,&width,&height)){
				if ( (col != -1) && (row != -1))
					if (width * height == partinfo.nodes){
						/* we have a contiguous rectangle */
						nrows = height;
						ncols = width;
						debug_allocation(5,"nrows %d ncols %d\n",nrows,ncols);
					}
			}
		}
		bitmap_allocate_space(bitmap, layer->bitmap);
		layer->num_free -= partinfo.nodes;
		if (allocation_debug){
			debug_allocation(5,"Layer after allocate:\n");
			debug_print_bitmap(DEBUG_ALLOC, layer->bitmap);
		}
		INSERT_LAYER(parent->child_alloc_lyr, layer);

		/*
		 * Insert child partition in list of consumer objects
		 */
		insert_in_partlist(layer, child);

		free((void*)available_in_layer);

	} /* endif !first partition */
	
	if ( (nrows != 0) && (ncols != 0)){
		child->rows = nrows;
		child->cols = ncols;
	}
	else{
		/* No rectangle was found */
		child->rows = -1;
		child->cols = -1;
	}
	debug_allocation(5,"rect dimensions of partition rows %d cols %d\n",child->rows,child->cols);
	/*
	 * Insert new partition in partition table.
	 */
	if (HASH_INSERT_PART(partinfo.inode, child) == -1) {
		goto internal_error;
	}
	
	if (child->sched == GANG)
		num_gang_parts++;

	/*
	 * Normal exit, everything went OK.
	 */
	*error = ESUCCESS;
	return;

internal_error:
	*error = EPALLOCERR;
internal_error1:

	if (child != (PART_T *) 0)
		FREE((void *) child);

	if (layer != (LAYER_T *)0){
		if (new_layer) {
			if ( layer->bitmap != (BITMAP_T *)0)
				FREE( (void *) layer->bitmap);
			FREE((void *) layer);
		} else {
			/* Put the layer back in the layer list */
	                if (allocation_debug){
				debug_allocation(5,"Putting layer back:\n");
				debug_print_bitmap(DEBUG_ALLOC, layer->bitmap);
			}
			INSERT_LAYER(parent->child_alloc_lyr, layer);
		}
	}

	FREE((void *) nd_list);
	free_bitmap((void *) bitmap);
	if (rect != (BITMAP_T *) 0)
		FREE((void *) rect);
	return;
}

		
int
build_partinfo(ino_t inode, PARTREQ_T *partinfo)
{
	PART_T	*part;

	part = HASH_LOOKUP_PART(inode);
	if (part == (PART_T *) 0) {
		errno = EPINVALPART;
		return(-1);
	}
	else{
		partinfo->uid = part->owner;
		partinfo->gid = part->group;
		partinfo->access = part->protection;
		partinfo->sched = part->sched;
		partinfo->rq = part->rollin_quantum;
		partinfo->maxpri = part->max_priority;
		partinfo->parent_inode = part->parent_alloc_lyr->part->inode;
		partinfo->rect.rows = -1;
		partinfo->rect.cols = -1;
		partinfo->slots = part->slots;
		partinfo->nodes = part->nodes;
		partinfo->flags = 0;
	}
	return(0);
}
