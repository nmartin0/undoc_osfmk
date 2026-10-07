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
 *      $Id: partutils.c,v 1.36 1995/02/10 23:43:54 carolr Exp $
 *
 */

/*
 * partutils.c - 
 *
 * HISTORY
 * $Log: partutils.c,v $
 * Revision 1.36  1995/02/10  23:43:54  carolr
 * Corrected the memory leak indicated in the PTS description in the nx_pspart()
 * function by restructuring the overall flow (eliminated the GOTO statements) and
 * adding a call to free the pathname memory.
 *
 * Also corrected a 2nd memory leak by ensuring vm_deallocate() is called to return
 * the memory obtained in the nx_pspart_rpc() call.
 *
 *  Reviewer: sdh
 *  Risk: low
 *  Benefit or PTS #: 12047
 *  Testing: controlc, rmcall, rmcmd, sched, manual tests
 *  Module(s):
 * 	 cmds_libs/src/usr/bin/libnx/partutils.c
 *
 * Revision 1.35  1995/01/05  15:52:54  sdh
 * Added code to NX_mkpart_attr to adjust the node list to
 * match the bitmap when a selector list is specified in the
 * mkpart.
 *
 *  Reviewer: tracy
 *  Risk: low
 *  Benefit or PTS #: 11635, 11633, 11321
 *  Testing:
 * 	EATS: sched, rmcmd, rmcall, controlc
 * 	manual testing
 *  Module(s):
 * 	partutils.c
 *
 * Added some code to check the epl value passed in no matter
 * what the sched param is set to.
 *
 *  Reviewer: sdh (fix by tracy)
 *  Risk: low
 *  Benefit or PTS #: 7821
 *  Testing:
 * 	EATS: sched, rmcmd, rmcall, controlc
 * 	manual testing
 *  Module(s):
 * 	partutils.c
 *
 * Revision 1.34  1994/11/19  02:32:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.33  1994/11/02  23:12:52  sdh
 * Added code to NX_mkpart_attr to check for a selector string
 * and if so, to modify the bitmap to include only the nodes
 * that satisfy the selector string
 *
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 11321
 *  Testing:
 * 	EATS: controlc, rmcall, rmcmd, sched
 * 	manual tests
 *  Module(s):
 * 	partutils.c
 *
 * Revision 1.32  1994/09/06  22:03:04  sdh
 *  Removed a line that set the default behavior to relaxed. The default behavior
 *  is now strict.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: 10649
 *  Testing: EATS: controlc, rmcall, rmcmd
 * 	manual tests
 *  Module(s):
 * 	partutils.c
 *
 * Revision 1.31.2.1  1994/09/06  21:40:53  sdh
 * Removed a line that set the default behavior to relaxed. The default behavior
 * is now strict.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: 10649
 *  Testing: EATS: controlc, rmcall, rmcmd
 * 	  manual tests
 *  Module(s):
 * 	partutils.c
 *
 * Revision 1.31  1994/07/19  20:42:33  mag
 * Make sure mkpart does not fail when -rlx is given
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: No pts bug.
 *  Testing: developer, EATS: rmcmd, rmcall
 *  Module(s): partutils.c
 *
 * Revision 1.30  1994/07/06  18:15:51  sdh
 * Changed the first strcat in ok_to_rename() to an strcpy.
 * Otherwise the path built in topart would be incorrect if
 * the first byte wasn't \0.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: 10016 (and 7939)
 *  Testing: developer
 *  Module(s):
 * 	partutils.c
 *
 * Revision 1.29  1994/07/06  17:13:32  mag
 * nx_mkpart_attr w/ SPS and RQ should not succeed.
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 9878
 *  Testing: developer
 *  Module(s): parutils.c
 *
 * Revision 1.28  1994/06/01  20:15:31  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): Makefile, bitmap.c, bitmap2.c, nx_initve.c, nx_initve_.c,
 * 	    nx_part_ops.c, partprint.c, partutils.c, utils.c,
 * 	    _copyargs.c (new), attrprint.c (news), attributes.c (new)
 *  Related: server, emulator, allocator, bootmesh, mkpart, showpart, lspart
 *
 * Revision 1.27  1994/03/29  21:47:19  bks
 * Changed third arg for nx_pspart() from int to unsigned long to match man
 * page, C System Calls Reference Manual, and nx.h
 *
 * Revision 1.26  1994/03/29  21:39:36  bks
 * Changed second argument from type char ** to type nx_pspart_t ** (which is
 * what it was used as, and what it really is) to match nx.h, and the
 * specification for nx_pspart().
 *
 *  Reviewer: Cris Derr
 *  Risk: L
 *  Benefit or PTS #: 8766
 *  Testing: Compiled libnx, showfs, ls, tar, mount
 *  Module(s): gopen()
 *
 * Revision 1.25  1994/01/25  22:07:54  carbajal
 *  7816 PARAGON      OPEN      M        carbajal  karla               R1.2 WW02
 *       MESH UTILS   17-JAN-94 **       14-JAN-94 14-JAN-94
 *       mkpart indicates internal error if specify too many nodes
 * Issue: The wrong error code was being returned by the allocator when the size
 * requested exceeded the parent partition. The fix is to check for this prior to
 * calling the allocator.
 *
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: 7816
 *  Testing: rmcall, rmcmd, sched EATs, bug report1
 *  Module(s): partutils.c (libnx.a)
 *
 * Revision 1.20.2.5  1994/01/25  18:30:13  carbajal
 *  7816 PARAGON      OPEN      M        carbajal  karla               R1.2 WW02
 *       MESH UTILS   17-JAN-94 **       14-JAN-94 14-JAN-94
 *       mkpart indicates internal error if specify too many nodes
 * Issue: The wrong error code was being returned by the allocator when the size
 * requested exceeded the parent partition. The fix is to check for this prior to
 * calling the allocator.
 *
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: 7816
 *  Testing: rmcall, rmcmd, sched EATs, bug report1
 *  Module(s): partutils.c (libnx.a)
 *
 * Revision 1.20.2.4  1994/01/12  18:08:34  carbajal
 *  6392 PARAGON      OPEN      M        carbajal  davidl              R1.1 T12.1
 *       MESH UTILS   24-SEP-93 **       25-AUG-93 25-AUG-93
 *       chpart -g <samegroup> always fails with "Invalid group id"
 * Benefit: Can use chpart to set group id of a partition
 * Risk: Low
 * Eng: Carbajal Reviewer: cameron
 * Main Issue: The check for a valid group was being done by the allocator. The
 * call getgroup() uses the calling processes gid to get the group list. Well
 * the allocator gid is not the same as the calling processes group id. The
 * change was to make this call from the user process before calling the
 * allocator.
 *
 * Revision 1.20.2.3  1994/01/06  22:31:57  carbajal
 * Proper argc checking in get_partition_name().
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7288, 7289
 *  Testing: bug report
 *  Module(s): partutils.c
 *
 * Revision 1.20.2.2  1993/12/29  17:41:28  carbajal
 * Divide the byte count returned by nx_pspart_rpc() by the sizeof
 * the structure.
 *  Reviewer: John Litvin
 *  Risk: Low
 *  Benefit or PTS #: 7425
 *  Testing: EATs, bug report
 *  Module(s):
 *
 * Revision 1.20.2.1  1993/12/20  21:49:14  carbajal
 * Use macros FREE and MALLOC. in nx_pspart() vm_dealloc and malloc
 * user memory.
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7257
 *  Testing: SATs, EATs, MUNOPs, bug test
 *  Module(s): partutils.c
 *
 * Revision 1.20  1993/12/01  01:23:09  carbajal
 * Added nx_pspart()
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.19  1993/11/18  19:25:26  dleslie
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
 * Revision 1.18  1993/11/17  02:42:09  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.17  1993/10/27  01:48:28  carbajal
 * Added round_rq() to make sure rollin quantum is rounded to 100ms
 *
 * Revision 1.16  1993/08/18  20:25:13  carbajal
 * Added checks for EPL and RQ bounds: PTS #6118 and PTS #6132
 *
 * Revision 1.15  1993/07/29  00:07:14  carbajal
 * Changed getting of string lengths using strlen() to be strlen()+1
 *
 * Revision 1.14  1993/07/20  18:09:06  carbajal
 * Removed allocator_cmds_setup() and ask_for_allocator_port(). These
 * are now in partlock.c.
 * Cleaned up error checking in the NX_*part() calls.
 *
 * Revision 1.13  1993/07/18  19:52:39  carbajal
 * Added support for nx_chpart_rpc() system call
 *
 * Revision 1.12  1993/07/13  21:40:48  carbajal
 * Added allocator system call support
 * New parameters for allocator_cmds_setup()
 *
 * Revision 1.11  1993/05/06  18:20:55  carbajal
 * Removed write_partinfo() to writepart.c
 *
 * Revision 1.10  1993/04/29  01:18:51  carbajal
 * Use part.slots when writing out node list in write_partinfo(). This will
 * take of the bugs seen in chpart <something> . and mkpart .xx.
 *
 * Revision 1.9  1993/03/16  00:16:47  carbajal
 * Call chmod to set .partinfo files to rw-r-r in write_partinfo.
 *
 * Revision 1.8  1993/01/28  21:42:58  shala
 * Fixed multiply defined LP_MAP_T .
 *
 * Revision 1.7  1992/12/18  02:30:09  carbajal
 * allocator_cmds_setup, write_part_info
 *
 * Revision 1.6  1992/12/10  21:14:56  shala
 * Fixed path to mach_error.h include file.
 *
 * Revision 1.5  1992/11/04  18:30:20  carbajal
 * In lock_part_info check pathname length and return E2BIG if
 * we exceed namelen
 *
 * Revision 1.4  1992/11/03  00:15:58  carbajal
 * Broke up allocator_cmds_setup into two
 * parts: get_partition_name and allocator_cmds
 * setup.
 *
 * Revision 1.3  1992/10/17  01:02:44  carbajal
 * Corrected a bug I introduced by trying to use the defpart
 * if no partition name was specified.
 *
 * Revision 1.2  1992/10/13  00:44:36  carbajal
 * Added a new parameter defpart to allocator_cmds_setup.
 *
 * Revision 1.1  1992/10/08  19:30:23  carbajal
 * Consolidate in line code from mkpart for use by
 * other allocator commands
 *
 */
char _partutils_c_id[]="$Id: partutils.c,v 1.36 1995/02/10 23:43:54 carolr Exp $";

#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mode.h>
#include <strings.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <mach_error.h>
#include <allocsys.h>
#include <nx.h>
#include <mach/message.h>
#include <mach/norma_special_ports.h>
#include <mach/mach_host.h>

#include <mcmsg/mcmsg_appl.h>
#include <nx/allocator.h>
#include <nx/bitmap.h>
#include <nx/defines.h>
#include <nx/utils.h>
#include <nx/nx_getopt.h>
#include "create.h"

#define strrpbrk(s1, s2) rindex(s1, *(s2))

/* Forward reference */
char *
ok_to_rename(char *oldname,char *newname);
char *
marshall_mkpart_rpc(int flag,char *pathname,PARTREQ_T mypart, 
		BITMAP_T *my_bitmap,
		LP_MAP_T my_nodes,int num_nodes,
		int selector_cnt, char* selector_buf, int selector_buflen,
		int *buf_size);
unsigned long round_rq(unsigned long rq);
int
I_belong_in_group(gid_t my_gid,gid_t new_gid);


char *
get_partition_name(int argcount, int argc, char *argv[], char **name, \
		     char *progname,char *defpart)
{
	char	*partname;

	/*
	* Command must have the name of the partition as an argument
	*/
       
	partname = NULL;

        if( argc == argcount ){
        /* No partition name was specified so check defpart */
                if (defpart != NULL)
                        *name = strdup(defpart);
                else{
                        errno = EPINVALPART;
                        goto get_partition_name_error;
                }
        }
        else
        if (argc - 1 > argcount){
                goto get_partition_name_error;
        }
        else{
                if( nx_optind > argc - 1 )
                        goto get_partition_name_error;
                else
                        /* make an effort to not modify argv */
                        *name = strdup(argv[nx_optind]);
        }
     
#ifdef DEBUG
     printf("specified partition name is %s\n",*name);
#endif DEBUG

	/*
	* The name should comprise of letters, numbers, "_" and "." only
	*/

	if( verify_name(*name) < 0 ) {
		errno = EPINVALPART;
		goto get_partition_name_error;
	}

	partname = strdup(*name);

get_partition_name_error:
	return(partname);

}


/* 		delete_partitions	
 *
 *	Description:
 *		Starting from path recursively remove all files
 *		and directories. Any problems along the way are
 *		ignored.
 *
 *	Parameters:
 *		path	char pointer to pathname
 *
 *	Returns:
 *		VOID
 *
 *
*/
void
delete_partitions(char *partname)
{
	DIR		dir;
	struct dirent	dir_entry;
	int		save_len;
	int		status;
	PATHTYPE	path;
	
#ifdef DEBUG
printf("delete_partitions %s \n",partname);
#endif DEBUG

	if (init_pathtype(&path) != NULL){
		if (append_path(&path,partname) == NULL){
			perror("delete_partitions");
			return;
		}
	}
	else{
		perror("delete_partitions");
		return;
	}

	opendir_r(path.space, &dir);

	while (readdir_r(&dir, &dir_entry) != -1) {
		save_len = strlen(path.space);
		if ( append_path(&path,"/")  == NULL){
			perror("rmpart");
			return;
		}
		if ( append_path(&path,dir_entry.d_name) == NULL){
			perror("rmpart");
			return;
		}
#ifdef DEBUG
printf("delete path %s\n",path.space);
#endif DEBUG
		if ((isdir(path.space)) &&
                    (strcmp(".", dir_entry.d_name) != 0)         &&
		    (strcmp("..", dir_entry.d_name) != 0))
		{
			delete_partitions(path.space);
			status = rmdir(path.space);
		}
		else if (!isdir(path.space)){
			status = unlink(path.space);
		}
		/* truncate path to original length */
                path.space[save_len] = '\0';
	} 

	closedir(&dir);
	status = rmdir(path.space);
	free_path(&path);
}


char *
get_parent_pathname(char *partname)
{
	char *parent_name,*rightmost_dot;

	/*
	* partname now contains the name of the new partition, we
	* must truncate it temporarily why we get the parent's info.
	*/
	rightmost_dot = strrchr(partname, '.');
	if (rightmost_dot != NULL){
		*rightmost_dot = '\0';
		*rightmost_dot = '\0';
		/* see if this is a subpartition of root */
		if (partname[0] == '\0')
			parent_name = ".";
		else
			parent_name = strdup(partname);
		*rightmost_dot = '.';
	}
	else{
		/* This is a relative pathname */
		parent_name = strdup(".compute");
	}

	return(parent_name);
}

int 
do_mkpart_rpc(int flag, ino_t parent_inode,char *partname, 
		PARTREQ_T *my_part,BITMAP_T *my_bitmap,
		LP_MAP_T my_nodes,int *num_nodes,
		int selector_cnt, char* selector_buf, int selector_buflen)
{
	mach_port_t	serv_port;
	kern_return_t	retcode;
	int		buf_size,save_errno,directory_created;
	char		*name,*pathname;
	char		*buffer;

	name = strdup(partname);
	dottoslash(name);
	if ( (pathname = create_partname(name)) == NULL)
		goto do_mkpart_rpc_error;
	FREE(name);

	buffer = marshall_mkpart_rpc(flag,pathname,*my_part,
			my_bitmap,my_nodes,
			my_part->slots,
			selector_cnt, selector_buf, selector_buflen,
			&buf_size);
	if (buffer == (char * )0)
		goto do_mkpart_rpc_error;
	if (nx_mkpart_rpc(buffer,buf_size,num_nodes) != 0)
			goto do_mkpart_rpc_error;
	if (pathname != NULL)
		FREE(pathname);
	return(0);
do_mkpart_rpc_error:
	if (pathname != NULL)
		FREE(pathname);
	return(-1);
}

int
NX_mkpart_attr(int flag,char *partname, nx_part_info_t partinfo,
  nx_node_list_t node_list, int relaxed,
  int selector_cnt, char* selector_buf, int selector_buflen)
{
	char		*parent_name,*rightmost_dot;	/* Temporary place holder */
	BITMAP_T	*my_bitmap,*parent_bitmap;
	BITMAP_T	*sel_bitmap;
	PARTREQ_T	my_part;
	nx_part_info_t	parent_part;
	LP_MAP_T	my_nodes;
	nx_node_list_t	parent_node_list;
	int		status,i,num_nodes;
	int 		j,k;
	unsigned long	bitmapCnt,nodelistCnt;
	
	my_bitmap = (BITMAP_T *)0;
	parent_bitmap = (BITMAP_T *)0;
	my_nodes = (LP_MAP_T )0;
	
	if (verify_name(partname) < 0){
		goto nx_mkpart_error;
	}

	parent_name = get_parent_pathname(partname);

	status = -1;
	/* ask for the parent partition information */
	if (nx_get_partition_attributes(parent_name,&parent_part) == 0){
		if (nx_get_node_list(parent_name,&parent_node_list,
			&nodelistCnt) == 0){
			if (nx_get_part_bitmap(parent_name,&parent_bitmap,
							&bitmapCnt) != 0){
				goto nx_mkpart_error;
			}
			else
				status = 0;
		}
	}
	
	if (status == -1)
		goto nx_mkpart_error;


	if (partinfo.sched != -1 && 
		partinfo.sched != GANG && partinfo.sched != UNIX &&
		partinfo.sched != SPACE_SHARE){
		errno = EPINVALSCHED;
		goto nx_mkpart_error;
	}

	/* see if any nodes were specified */
	if (partinfo.nodes != -1)
		if (partinfo.nodes < 0){
			errno = EPBADNODE;
			goto nx_mkpart_error;
		}


	/* Now check to make sure the parameters we are specifying are not
	 * contrary to what the parent has.
	*/
		
	/* Make sure the parent is not an standard scheduled partition */
	if ( parent_part.sched == UNIX){
		errno = EPINVALSCHED;
		goto nx_mkpart_error;
	}

	/* If there was an rq specified then make sure it is valid */
	if (partinfo.rq != NO_RQ_SPECIFIED && partinfo.sched == GANG){
		partinfo.rq = round_rq(partinfo.rq);
		/* Make sure the rollin quantum is within bounds */
		if (partinfo.rq > MAXRQ || partinfo.rq < MINRQ){
			errno = EPINVALSCHED;
			goto nx_mkpart_error;
		}
		else
		if (partinfo.rq !=  MINRQ && partinfo.rq < MIN_ALLOWED_RQ){
			errno = EPINVALSCHED;
			goto nx_mkpart_error;
		}
	}
	else
	if (partinfo.rq != NO_RQ_SPECIFIED && partinfo.sched != GANG){
		/* they have specified an rq but not gang scheduling */
		errno = EPINVALSCHED;
		goto nx_mkpart_error;
	}

	/* If there was an epl specified then make sure it is valid */
	if (partinfo.epl != NO_EPL_SPECIFIED && (partinfo.sched == GANG ||
	    partinfo.sched == SPACE_SHARE || partinfo.sched == NO_SCHED_SPECIFIED)) {
		if (partinfo.epl < MINPRI || partinfo.epl > MAXPRI){
			errno = EPINVALPRI;
			goto nx_mkpart_error;
		}
	}
	else
	/* they have specified and epl but not gang scheduling */
	if (partinfo.epl != NO_EPL_SPECIFIED && partinfo.sched == UNIX){
		errno = EPINVALSCHED;
		goto nx_mkpart_error;
	}
	
        /* Check the access mode */
        if (partinfo.access != NO_ACCESS_SPECIFIED)
                if (partinfo.access < 0 || partinfo.access > 0777){
                        errno = EPINVALMOD;
                        goto nx_mkpart_error;
                }

	my_part.rect.rows = RECTROWSNOTSET;
	my_part.rect.cols = RECTCOLSNOTSET;
	my_part.parent_inode = parent_part.part_id;  

	if (flag == MKPART_ND){
		/* we are performing a mkpart -nd request */
		if (partinfo.nodes > 0) {
			if ((my_bitmap = create_ndlst_bitmap(parent_part.nodes,
					parent_node_list,parent_bitmap,
					partinfo.nodes,(LP_MAP_T)node_list,
					&num_nodes,&my_nodes))
						 == (BITMAP_T *)0)
				goto nx_mkpart_error;
			my_part.nodes = num_nodes;
			my_part.slots = num_nodes;
		}
		else{
			/* No specification was given so we will inherit 
			 * everything from the parent.
			 */
			my_part.nodes = parent_part.nodes;
			my_part.slots = parent_part.nodes;
			my_nodes      = parent_node_list; 
			my_bitmap     = parent_bitmap;
			/*
			* If there is a selector string with this mkpart
			* then set the bitmap to include only thos node that 
			* meet the selection critieria
			*/
			if (selector_cnt) {
				if (nx_get_matching_bitmap(&sel_bitmap, &bitmapCnt, selector_buf) == -1) {
                                        errno = EPXRS;
                                        goto nx_mkpart_error;
				}
				my_bitmap = AND_bitmaps(sel_bitmap, parent_bitmap);
				my_part.nodes = my_part.slots = num_nodes_in_map(my_bitmap);
				my_nodes = (LP_MAP_T) MALLOC(my_part.nodes * sizeof(LP_MAP_ENTRY_T));
				for (k=0; k < nodelistCnt; k++) {
					if (getbit_bitmap(my_bitmap, parent_node_list[k])) {
						my_nodes[j++] = parent_node_list[k];
					}
				}

			}

		}
	}
	else{
		/* We are performing a mkpart -sz request. 
		 * Check the node list to see if -sz HxW was used
		*/
		if (node_list != (nx_node_list_t)0 && partinfo.nodes == 2){
			my_part.rect.rows = -node_list[0];
			my_part.rect.cols = -node_list[1];
			my_part.slots = -1;
			my_part.nodes = -1;
		}
		else{
			/* if we have specfied a node value */
			if (partinfo.nodes != -1)
				/* Make sure we are within the bounds of
				 * the parent 
				*/
				if (partinfo.nodes > parent_part.nodes &&
				  !relaxed){
					/* we are out of bounds, so bail
					 * out, without bothering the
					 * allocator
					*/
					errno = EPXRS;
					goto nx_mkpart_error;
				}
			my_part.slots = partinfo.nodes;
			my_part.nodes = partinfo.nodes;
		}
	}

	/* These data structures really ought to be the same */
	my_part.uid = partinfo.uid;
	my_part.gid = partinfo.gid;
	my_part.access = partinfo.access;
	my_part.sched = partinfo.sched;
	my_part.rq = partinfo.rq;
	my_part.maxpri = partinfo.epl;
	my_part.parent_inode = parent_part.part_id;

	my_part.relaxed = relaxed;

	if (do_mkpart_rpc(flag,my_part.parent_inode,partname,&my_part,
			my_bitmap,(LP_MAP_T)my_nodes,&num_nodes,
			selector_cnt, selector_buf, selector_buflen) != 0)
		goto nx_mkpart_error;

	return(num_nodes);
nx_mkpart_error:
	return(-1);
}

char *
marshall_mkpart_rpc(int flag,char *pathname,PARTREQ_T mypart, 
		BITMAP_T *my_bitmap,
		LP_MAP_T my_nodes,int num_nodes,
		int selector_cnt, char* selector_buf, int selector_buflen,
		int *buf_size)
{
	int		stringlen,bitmapsize,node_list_size,index;
	char 		*buffer;
	int 		i, sel_len;;

/*
 * Selectors require space for the selector count (an int)
 * a string buffer len (an int) plus the actual selector strings.
 */

	sel_len = sizeof(selector_cnt) + sizeof(selector_buflen)
	  + selector_buflen;

	stringlen  = strlen(pathname)+1;
	bitmapsize = bitmap_size(my_bitmap);
	if (num_nodes > 0 && my_nodes != (LP_MAP_T*)0)
		node_list_size = sizeof(LP_MAP_T) * num_nodes;
	else
		node_list_size = 0;
	*buf_size = sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int) +
		 stringlen + sizeof(PARTREQ_T) + 
		bitmapsize + node_list_size + sel_len;

	buffer = (char *)MALLOC(*buf_size);

	if (buffer == (char *)0)
		errno = ENOMEM;
	else{
		index = 0;
		bcopy((char *)&flag,buffer+index,sizeof(int));
		index += sizeof(int);
		bcopy((char *)&stringlen,buffer+index,sizeof(int));
		index += sizeof(int);
		bcopy((char *)&node_list_size,buffer+index,sizeof(int));
		index += sizeof(int);
		bcopy((char *)&bitmapsize,buffer+index,sizeof(int));
		index += sizeof(int);
		bcopy(pathname,buffer+index,stringlen);
		index += stringlen;
		bcopy((char *)&mypart,buffer+index,sizeof(PARTREQ_T));
		index += sizeof(PARTREQ_T);
		bcopy((char *)my_bitmap,buffer+index,bitmapsize);
		index += bitmapsize;
		if (node_list_size > 0) {
			bcopy((char *)my_nodes,buffer+index,node_list_size);
			index += node_list_size;
		}
		bcopy((char*)&selector_cnt, buffer+index, sizeof(selector_cnt));
		index += sizeof(selector_cnt);
		bcopy((char*)&selector_buflen, buffer+index,
		  sizeof(selector_buflen));
		index += sizeof(selector_buflen);
		if (selector_buflen)
		    bcopy(selector_buf, buffer+index, selector_buflen);
		index += selector_buflen;
	}

	return(buffer);
}

int
unmarshall_mkpart_rpc(int buf_size,char *buffer,
		int	*flag,
		char	**pathname,
		PARTREQ_T *my_part,
		BITMAP_T **my_bitmap,LP_MAP_T *my_nodes,int *nodelistsize,
		int* selector_cnt, char** selector_buf, int* selector_buflen)
{
	int	stringlen,index,bitmapsize;
	int	i;

	index = 0;
	bcopy(buffer+index,flag,sizeof(int));
	index += sizeof(int);
	bcopy(buffer+index,&stringlen,sizeof(int));
	index += sizeof(int);
	bcopy(buffer+index,nodelistsize,sizeof(int));
	index += sizeof(int);
	bcopy(buffer+index,&bitmapsize,sizeof(int));
	index += sizeof(int);
	*pathname = (char *)MALLOC(stringlen);
	if (*pathname == NULL){
		errno = ENOMEM;
		goto unmarshall_error;
	}

	if (*nodelistsize > 0){
		*my_nodes = (LP_MAP_T ) MALLOC(*nodelistsize);
		if (*my_nodes == (LP_MAP_T)0){
			errno = ENOMEM;
			goto unmarshall_error;
		}
	}
	else
		*my_nodes = (LP_MAP_T ) 0;

	if (bitmapsize > 0){
		*my_bitmap = (BITMAP_T *) MALLOC(bitmapsize);
		if (*my_bitmap == (BITMAP_T *)0){
			errno = ENOMEM;
			goto unmarshall_error;
		}
	}
	else{
		*my_bitmap = (BITMAP_T *) 0;
	}

	bzero(*pathname,stringlen);
	bcopy(buffer+index,*pathname,stringlen);
	index += stringlen;
	bcopy(buffer+index,my_part,sizeof(PARTREQ_T));
	index += sizeof(PARTREQ_T);
	bcopy(buffer+index,*my_bitmap,bitmapsize);
	index += bitmapsize;
	if (*nodelistsize > 0) {
		bcopy(buffer+index,*my_nodes,*nodelistsize);
		index += *nodelistsize;
	}

	bcopy(buffer+index, (char*)selector_cnt, sizeof(*selector_cnt));
	index += sizeof(*selector_cnt);
	bcopy(buffer+index, (char*)selector_buflen, sizeof(*selector_buflen));
	index += sizeof(*selector_buflen);

	*selector_buf = buffer + index;
	index += *selector_buflen;

	return 0;
unmarshall_error:
	return -1;
	
}

char *
make_partition_name(char *partname)
{
	char *pathname,*name;

	if (partname != NULL){
		name = strdup(partname);
		dottoslash(name);
		if ( (pathname = create_partname(name)) == NULL)
			errno = EPINVALPART;
		FREE(name);
	}
	else
		pathname = NULL;
	return(pathname);
}

int
NX_rmpart(char *partname,int rflag, int fflag)
{
	char		*pathname;
	int		status;
	ino_t		inode;
	int		flags;

	if ( (pathname = make_partition_name(partname)) == NULL){
		status = -1;
		goto rmpart_rpc_error;
	}

	flags = rflag | (fflag << 1);

	if ( (inode = get_inode(pathname)) == 0){
		status = -1;
		errno = EPINVALPART;
		goto rmpart_rpc_error;
	}
	else
	if (nx_rmpart_rpc(pathname,strlen(pathname)+1,inode,flags) != 0)
		status = -1;
	else
		status = 0;

rmpart_rpc_error:

	return(status);
	
}

char *
marshall_chpart_rpc(int *buf_size,char *pathname,PARTREQ_T *mypart,char *new_pathname) 
{
	int		stringlen,new_stringlen,index;
	char 		*buffer;

	stringlen  = strlen(pathname)+1;
	if (new_pathname == NULL)
		new_stringlen = 0;
	else
		new_stringlen  = strlen(new_pathname)+1;
	*buf_size = sizeof(int) + sizeof(int) + 
		 new_stringlen + stringlen + sizeof(PARTREQ_T);

	buffer = (char *)MALLOC(*buf_size);

	if (buffer == (char *)0)
		errno = ENOMEM;
	else{
		index = 0;
		bcopy((char *)&stringlen,buffer+index,sizeof(int));
		index += sizeof(int);
		bcopy((char *)&new_stringlen,buffer+index,sizeof(int));
		index += sizeof(int);
		bcopy(pathname,buffer+index,stringlen);
		index += stringlen;
		bcopy(new_pathname,buffer+index,new_stringlen);
		index += new_stringlen;
		bcopy((char *)mypart,buffer+index,sizeof(PARTREQ_T));
		index += sizeof(PARTREQ_T);
	}

	return(buffer);
}

int
unmarshall_chpart_rpc(int buf_size,char *buffer,
		char	**pathname,
		char	**new_pathname,
		PARTREQ_T *my_part)
{
	int	new_stringlen,stringlen,index;

	index = 0;
	bcopy(buffer+index,&stringlen,sizeof(int));
	index += sizeof(int);
	bcopy(buffer+index,&new_stringlen,sizeof(int));
	index += sizeof(int);

	*pathname = (char *)MALLOC(stringlen);
	if (*pathname == NULL){
		errno = ENOMEM;
		goto chpart_unmarshall_error;
	}

	if (new_stringlen > 0){
		*new_pathname = (char *)MALLOC(new_stringlen);
		if (*new_pathname == NULL){
			errno = ENOMEM;
			goto chpart_unmarshall_error;
		}
	}
	else
		*new_pathname = NULL;

	bzero(*pathname,stringlen);
	bcopy(buffer+index,*pathname,stringlen);
	index += stringlen;
	if (new_stringlen > 0){
		bzero(*new_pathname,new_stringlen);
		bcopy(buffer+index,*new_pathname,new_stringlen);
		index += new_stringlen;
	}
	bcopy(buffer+index,my_part,sizeof(PARTREQ_T));
	index += sizeof(PARTREQ_T);

	return 0;
chpart_unmarshall_error:
	return -1;
	
}

int
NX_chpart(char *partname, nx_part_info_t partinfo, char *newname)
{
	char 		*tmp,*parent_name,*new_pathname,*pathname;
	PARTREQ_T	my_part;
	int		status;
	struct group   *grp;
	struct passwd   *user;
	int		buf_size;
	char		*buffer;

	buffer = NULL;
	parent_name = NULL;
	new_pathname = NULL;
	pathname = NULL;

	if (newname != NULL)
		/* make sure that the new partition name is a good one */
		if (verify_name(newname) < 0){
			status = -1;
			goto chpart_rpc_error;
		}

	if (partinfo.flags_or_size & RQFLG){
		partinfo.rq = round_rq(partinfo.rq);
		/* Make sure the rollin quantum is within bounds */
		if (partinfo.rq > MAXRQ || partinfo.rq < MINRQ){
			errno = EPINVALSCHED;
			status = -1;
			goto chpart_rpc_error;
		}
		else
		if (partinfo.rq !=  MINRQ && partinfo.rq < MIN_ALLOWED_RQ){
			errno = EPINVALSCHED;
			status = -1;
			goto chpart_rpc_error;
		}
	}

	if (partinfo.flags_or_size & EPLFLG)
		/* Make sure the epl is within limits */
		if (partinfo.epl < MINPRI || partinfo.epl > MAXPRI){
			errno = EPINVALPRI;
			status = -1;
			goto chpart_rpc_error;
		}

        /* Check the access mode */
        if (partinfo.flags_or_size & MODFLG)
                if (partinfo.access < 0 || partinfo.access > 0777){
                        status = -1;
                        errno = EPINVALMOD;
                        goto chpart_rpc_error;
                }


	/* open user database */
	setpwent();
	/* open group database */
	setgrent();

	status = 0;
	if ( (pathname = make_partition_name(partname)) == NULL){
		status = -1;
		goto chpart_rpc_error;
	}

	my_part.inode = get_inode(pathname);
	tmp = get_parent_pathname(partname);
	if ( (parent_name = make_partition_name(tmp)) != NULL)
		my_part.parent_inode = get_inode(parent_name);
	else
		my_part.parent_inode = 0;

	new_pathname = NULL;

	/* These data structures really ought to be the same */
	my_part.uid = partinfo.uid;
	my_part.gid = partinfo.gid;
	my_part.access = partinfo.access;
	my_part.sched = partinfo.sched;
	my_part.rq = partinfo.rq;
	my_part.maxpri = partinfo.epl;
	my_part.flags = partinfo.flags_or_size;

	if (my_part.flags & NMFLG)
		/* we are asking to rename a partition 
		 * Validate that the rename is ok
		*/
		if ( (new_pathname = ok_to_rename(partname,newname)) == NULL){
			status = -1;
			goto chpart_rpc_error;
		}

	if (my_part.flags & OWNERFLG){
		if (my_part.uid != -1){
			/* validate user id */
			user = getpwuid(my_part.uid);
			if ( (char *) user == NULL){
				/* Invalid user */
				errno = EPINUSER;
                        	status = -1;
               		}
		}
		if (my_part.gid != -1){
			/* validate group id */
			grp = getgrgid(my_part.gid);
			if ( (char *) grp == NULL){
				/* Invalid group */
				errno = EPINGRP;
                        	status = -1;
               		}
			if (!I_belong_in_group(getgid(),my_part.gid)){
				/* Invalid group */
				errno = EPINGRP;
                        	status = -1;
			}

		}
	}

	if (status == -1)
		goto chpart_rpc_error;
#ifdef DEBUG
printf("NX_chpart: pathname=%s, new_pathname=%s\n", pathname, new_pathname);
#endif
	buffer = marshall_chpart_rpc(&buf_size,pathname,&my_part,new_pathname);
	if (buffer != NULL){
		if (nx_chpart_rpc(buffer,buf_size) != 0)
			status = -1;
	}
	else
		status = -1;

chpart_rpc_error:

	if (buffer != NULL)
		FREE(buffer);
	if (pathname != NULL)
		FREE(pathname);
	if (parent_name != NULL)
		FREE(parent_name);
	endpwent();
	/* close group database */
	endgrent();

	return(status);

}

int
nx_pspart(char *name,nx_pspart_t **buffer, unsigned long *buf_len)
{
	ino_t	inode;
	char	*pathname = NULL;
	char	*buf = NULL;
	int	    len;
	int     ERR_CODE = 0;  /* ok status */

	if ( (pathname = make_partition_name(name)) == NULL)
		ERR_CODE = -1;

	else if ( (inode = get_inode(pathname)) == 0){
		errno = EPINVALPART;
		ERR_CODE = -1;
	}

	else if (nx_pspart_rpc(inode,&buf,&len) != 0)
			ERR_CODE = -1;

	else {
	  *buf_len = len / sizeof(nx_pspart_t);
	  *buffer  = (nx_pspart_t *) 0;

	  if (len > 0){
  
        	  if ((*buffer = (nx_pspart_t *) MALLOC(len)) 
				  == (nx_pspart_t *)0){
                	  errno = ENOMEM;
                	  ERR_CODE = -1;
        	  }

			  else
        	    bcopy(buf,*buffer,len);

			  /* clean up buf */
        	  vm_deallocate(mach_task_self(),(vm_address_t *) buf,
                	  (vm_size_t) len);
	  }
	}

	if (pathname != NULL)
	  free(pathname);  /* return memory */

	return(ERR_CODE);
}

/********************************  ok_to_rename ******************
 *
 *	Calling Sequence:
 *		ok_to_rename(from,to)
 *
 *	Description:
 *		check to see if it is ok to rename a partition from oldname to
 *		new name. The partition's must share the same parent.
 *
 *	Parameters:
 *		from	char pointer to the from string
 *		to	char pointer to the to string
 *
 *	Returns:
 *		Pointer to new partition name
 *		NULL if the rename is invalid
*/
char *
ok_to_rename(char *oldname,char *newname)
{
	int	i;
	char 	*from,*to,*rightmostslash,*topart,*filename,*frompart;
	FILE	*fd;

#ifdef DEBUG
printf("ok_to_rename: oldname=%s, newname=%s\n", oldname, newname);
#endif
	from = strdup(oldname);
	to = strdup(newname);

	/*
	* The user sees the partition name to be of the form abc.xyz.jkl
	* We need to replace those .s with /s
	*/

	dottoslash(to);
	dottoslash(from);

	/* are there any "/"'s in the new name? */
	if (strchr(to,'/') != NULL){
		/* Yes, this must be a simple name */
		errno = EPINRN;
		topart = NULL;
		goto rename_error;
	}

	/*
	* create a partition name from the argument. If the name starts
	* with a / consider it relative to /etc/nx otherwise
	* make it relative to /etc/nx/compute
	*/
	
	if( ( frompart = create_partname(from)) != NULL){
		if ( (rightmostslash = strrchr(frompart,'/')) != NULL){
			*rightmostslash = '\0';
			topart = (char *)MALLOC( strlen(frompart) + strlen(to) + 1);
			strcpy(topart,frompart);
			strcat(topart,"/");
			strcat(topart,to);
			*rightmostslash = '/';	
		}
		else
			topart = NULL;
	}
	else
		topart = NULL;

rename_error:
#ifdef DEBUG
printf("ok_to_rename: from=%s, to=%s, frompart=%s topart=%s\n", from, to, frompart, topart);
#endif

	if (to != NULL)
		FREE(to);
	if (from != NULL)
		FREE(from);
	return(topart);
}


unsigned long
round_rq(unsigned long rq)
{
	if (rq == 0)
		return(0);
	else
		return(((rq + 99) / 100) * 100);
}

int
find_initial(char *s1, char *s2)
{
	int	i;
	int	len1,len2;

	len1 = strlen(s1);
	len2 = strlen(s2);
	if (len1 < len2)
		return(0);
	for (i = 0; i < len2; i++)
		if (s1[i] != s2[i])
			break;
	return(i);
}

char *
filename_to_partname(char *filename)
{
	char	*partname;
	char	*tmpptr;
	int	i;

	/* See if the name is relative to /etc/nx */
	if ( (i = find_initial(filename,"/etc/nx//")) > 0){
		partname = strdup(&(filename[i-1]));
	}
	else
	if ( (i = find_initial(filename,"/etc/nx")) > 0){
		partname = strdup(&(filename[i]));
	}
	else
		partname = strdup(filename);
	slashtodot(partname);
	return(partname);
}


int
I_belong_in_group(gid_t my_gid,gid_t new_gid)
{
        gid_t   grouplist[NGROUPS_MAX];
        int     i,ok,n;

	if (geteuid() == 0)
		/* Root can do anything */
		return(TRUE);

        ok = FALSE;
        grouplist[0] = my_gid;
        n = getgroups(NGROUPS_MAX,&grouplist[0]);

        if (n > 0)
                for (i = 0; (!ok) && (i < n); i++){
                        if (grouplist[i] == new_gid)
                                ok = TRUE;
                }
        if (!ok)
                errno = EPINGRP;

        return(ok);
}
