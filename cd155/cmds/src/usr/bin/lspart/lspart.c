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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/lspart/lspart.c,v 1.33 1995/03/28 19:59:16 carolr Exp $
 *
*/

/* History
 * $Log: lspart.c,v $
 * Revision 1.33  1995/03/28  19:59:16  carolr
 * Problem description:
 *  11811 PARAGON      OPEN      M        sdh       scotth              R1.3 WW48
 *        MESH UTILS   21-MAR-95 **       12-JAN-95 07-DEC-94
 *        lspart dumps core during parallel sats
 *
 * Mandatory?:  n
 *
 * Description:
 *  - added sanity checks to look at the return value from subroutines
 *    (may as well use them since they are there)  If a subroutine returns
 *    an error condition, bail out of the routine we are in before someone
 *    attempts to use any bogus values.
 *
 *  - additional debugging statements added
 *
 *  - changed free() -> FREE()
 *
 *
 *    NOTE - these changes have reduced the lspart core dumps from approx.
 *           12 per 12 hours of running to 1 per 12 hours of running.  The
 *           PTS will not be marked as fixed at this time.
 *
 *  Reviewer: sdh
 *  Risk: low
 *  Benefit or PTS #: 11811
 *  Testing:
 *   EATS: controlc, rmcall, rmcmd, schedule
 *     manual testing
 *  Module(s):
 *   usr/bin/lspart/lspart.c
 *
 * Revision 1.32  1995/02/10  23:40:14  carolr
 * Added tests to the  processing of the -l flag and the -p flag to ensure
 * the other was not specified.  Will now display the usage message and exit
 * if these flags are used together.
 *
 * Also, corrected the usage statement to match what is specified in the man page.
 *
 *  Reviewer: sdh
 *  Risk: low
 *  Benefit or PTS #: 10857
 *  Testing: controlc, rmcall, rmcmd, sched, manual tests
 *  Module(s):
 * 	cmds_libs/src/usr/bin/lspart/lspart.c
 * 	cmds_libs/src/usr/bin/showpart/showpart.c
 *
 * Revision 1.31  1994/11/19  01:29:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.30  1994/07/13  17:49:04  mag
 * Added -p (physical) and changed -l (logical) options to node attribute printing
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 10207
 *  Testing: developer
 *  Module(s): lspart.c
 *
 * Revision 1.29  1994/06/02  18:15:54  mag
 * Remove extraneous arg from call to print_part_ext
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Correct compilation error found during build on Sun that
 * 		   was not caught on 486 build machine
 *  Testing: none
 *  Module(s): lspart.c
 *
 * Revision 1.28  1994/06/01  20:54:29  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): Makefile, lspart.c
 *  Related: libnx, server, emulator, allocator, mkpart, showpart, bootmesh
 *
 * Revision 1.27  1994/01/07  20:43:01  carbajal
 *  *  7258 PARAGON      OPEN      M        carbajal  tracy               R1.2 WW47
 *  *       MESH UTILS   29-NOV-93 **       29-NOV-93 29-NOV-93
 *  *       lspart & showpart are not displaying correct info when bad nodes are
 *  *       present
 *  *  Reviewer: cameron
 *  *  Risk: Low
 *  *  Benefit or PTS #: 7258
 *  *  Testing: bug report
 *  *  Module(s): lspart.c
 *
 * Revision 1.26.2.1  1994/01/06  23:02:16  carbajal
 *  7258 PARAGON      OPEN      M        carbajal  tracy               R1.2 WW47
 *       MESH UTILS   29-NOV-93 **       29-NOV-93 29-NOV-93
 *       lspart & showpart are not displaying correct info when bad nodes are
 *       present
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7258
 *  Testing: bug report
 *  Module(s): lspart.c
 *
 * Revision 1.26  1993/11/18  20:14:05  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of export tree
 *  Testing: built on Suns and 486
 *  Module(s): Makefile lspart.c
 *
 * Revision 1.25  1993/11/17  20:33:01  carbajal
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Changed call nx_get_free_nodes() to nx_free_nodes()
 * 		   as per R1.2 User Model Spec
 *  Testing:
 *  Module(s):
 *
 * Revision 1.24  1993/11/17  02:57:16  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.23  1993/09/16  23:47:40  carbajal
 * Made usage message match the man page. PTS #3883
 *
 * Revision 1.22  1993/07/19  20:29:06  carbajal
 * Changed make_partition_name to make_part_name()
 *
 * Revision 1.21  1993/07/16  17:55:08  carbajal
 * Call make_partition_name() now because of get_partition_name changes
 *
 * Revision 1.20  1993/04/08  23:04:33  carbajal
 * Fixed error 0 message
 *
 * Revision 1.19  1993/03/18  21:41:42  carbajal
 * Corrected permissions and exit statuses
 *
 * Revision 1.18  1993/03/18  18:46:47  carbajal
 * Set exit status to 0 if ok
 *
 * Revision 1.17  1993/01/28  19:50:07  carbajal
 * Deal with multiply defined LP_MAP_T
 *
 * Revision 1.16  1993/01/27  22:28:05  ericr
 * Corrected usage ("partition" instead of "name) re PTS 3883
 *
 * Revision 1.15  1993/01/27  19:22:21  carbajal
 * Split out printing of partition info into partprint.c
 * which now lives in libnx.a.
 *
 * Revision 1.14  1993/01/18  20:02:49  carbajal
 * check to see if the bad node bitmap is NULL before
 * dereferencing
 *
 * Revision 1.13  1992/12/24  03:21:58  carbajal
 * Fixed output alignment
 *
 * Revision 1.12  1992/12/18  02:49:35  carbajal
 * Moved pathname code out of here into partutils.c
 * of libnx.a.
 * Put a colon onto the partition name if using -r
 * For the -r option only add partitions to the list
 * if we have access to them.
 * Print out partial info if we cannot read the
 * partition but we have read access to the parent.
 * Bad node support
 *
 * Revision 1.11  1992/11/03  00:53:41  carbajal
 * Lspart now uses routines in libnx.a to build
 * pathnames. Pathnames are allowed to be as
 * big as the can be.
 *
 * Revision 1.10  1992/10/22  00:53:16  carbajal
 * Fixed problem with long pathnames by making sure
 * that they pathname is null terminated.
 *
 * Revision 1.9  1992/10/13  00:36:20  carbajal
 * Added support for -r option
 * If no partition name is specified then read
 * the environment var NX_DFLT_PART (defined in allocator.h)
 * Changed usage line to reflect that partition name is now
 * optional.
 *
*/

static char rcsid[] = "$Id: lspart.c,v 1.33 1995/03/28 19:59:16 carolr Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
#include <allocsys.h>
#include <sys/dir.h>

#include <mcmsg/mcmsg_appl.h>
#include <nx/allocator.h>
#include <nx/bitmap.h>
#include <nx/defines.h>
#include <nx/utils.h>
#include <nx/nx_getopt.h>
#include <nx/partprint.h>
#include <nx/partutils.h>

void Usage();
int ls_partitions(uid_t uid, gid_t gid, int rflag,int pflg,char *partname);
int 
get_partition_stuff(char *name, nx_part_info_t *partinfo, nx_nodes_t *nodes);

int	heading_printed;		/* flag to tell whether the heading
					* has already been printed
					*/
int	partitions_found;		/* Keep track of how many partitions were 
					 * found
					*/
char		*indent_str;
BITMAP_T	*root_bitmap;
BITMAP_T	*bad_bitmap;
PARTREQ_T	root;

int		lflg;

struct list {
	struct list	*next;
	char		*name;
};

struct list	*top,*ptr;


char *
make_part_name(char *partname)
{
	char *pathname,*name;

	if (partname != NULL){
		name = (char *)strdup(partname);
		dottoslash(name);
		if ( (pathname = create_partname(name)) == NULL)
			errno = EPINVALPART;
		free(name);
	}
	else
		pathname = NULL;
	return(pathname);
}
struct list *create_item(name)
char    *name;
{

        struct list     *l;
        l = (struct list *) malloc(sizeof(struct list));
        if (l != (struct list *)0){
                l->name = (char *)strdup(name);
		l->next = (struct list *)0;
	}
        return(l);

}


void add_to_list(ptr,l)
struct list **ptr,*l;
{
        (*ptr)->next = l;
        *ptr = l;
}

/********************************  lspart ********************************
 *
 *      Calling Sequence:
 *             NONE
 *
 *      Description:
 *            lspart displays information on the subpartitions of the
 *            named partition. User must have READ permission on the
 *            partition.   
 *
 *      Parameters:
 *              NONE
 *
 *      Returns:
 *               status
 *
 *
 */


main(argc, argv)
int argc;
char *argv[];
{
	char		*name;		/* Name of the partition (.a.b form) */
	char		*tmp_name,*path;
	int		rflg;
	int		pflg;
	long 		optindold;      /* Argv index of the prevoius argument */
	char *buf;             		/* Temorary buffer */
	long argcount;         		/* Number of arguments in the command */
	uid_t		uid;		/* User id */
	gid_t		gid;		/* Group id */
	struct list	*recall,*l;
	nx_node_list_t	unavail_nodes,bad_nodes,rlp_map;
	unsigned long	unavail_size,root_bmap_size;
	int		i;

	root_bitmap = bad_bitmap = (BITMAP_T *)0;
	partitions_found = 0;
	optindold = nx_optind;
	rflg = 0;
	lflg = 0;
	pflg = 0;
	heading_printed = FALSE;
	argcount = 1;
	/*
	*  Parse options.
	*/

	while((buf = nx_getopt(argc,argv, "r,l,p")) != NULL) {
		if( strcmp(buf,"r" ) == 0 )  {
			argcount += (nx_optind - optindold);
			optindold = nx_optind;
			rflg = 1;
		} else if( strcmp(buf,"l" ) == 0 )  {
            if (pflg) {
                Usage();
                exit(USAGE_EXIT);
            }
			argcount += (nx_optind - optindold);
			optindold = nx_optind;
			lflg = 1;
		} else if( strcmp(buf,"p" ) == 0 )  {
            if (lflg) {
                Usage();
                exit(USAGE_EXIT);
            }
			argcount += (nx_optind - optindold);
			optindold = nx_optind;
			pflg = 1;
			lflg = 1;
		} else {
			Usage();
			exit(USAGE_EXIT);
		}
	}

	buf = NULL;
	buf = getenv(NX_DFLT_PART);

	if ((buf == NULL) || (*buf == 0))
		buf = ".compute";

	tmp_name = get_partition_name(argcount,argc,argv,&name,argv[0],buf);

#ifdef DEBUG
   printf("partname from get_partition_name: %s\n", tmp_name);
#endif

	if (tmp_name == NULL){
		Usage();
		exit(USAGE_EXIT);
	}

	path = make_part_name(tmp_name);

	if (!rflg) 
		indent_str = (char *)strdup(" ");
	else
		indent_str = (char *)strdup("   ");

	uid = getuid();
	gid = getgid();

	l = create_item(path);
	top = l;
	ptr = top;

	/*
	 * Get the root partition information
	 */
	if (nx_get_part_bitmap(".",&root_bitmap,&root_bmap_size) == -1){
		perror("lspart");
		exit(1);
        }

	/* Get the bad nodes */
	if (nx_failed_nodes((nx_nodes_t *)&unavail_nodes,&unavail_size) == -1){
		perror("lspart");
		exit(1);
	}

	/* Build the bad nodes bitmap */
	bad_bitmap = bitmap_clone(root_bitmap);
        init_bitmap(0,bad_bitmap);
        for (i = 0; i < unavail_size; i++)
                setbit_bitmap(bad_bitmap,1,unavail_nodes[i]);

	/* free memory given to us by call */
    if(unavail_nodes != (nx_node_list_t *) 0)
	  FREE((void *)unavail_nodes);

	/* Get the configuration errors */
	if (nx_config_errors(&unavail_nodes,&unavail_size) == -1){
		perror("lspart");
		exit(1);
	}

	for(recall = top; recall != (struct list *)0; recall = recall->next){
		if (ls_partitions(uid,gid,rflg,pflg,recall->name) != 0){
			if (partitions_found == 0){
				goto lspart_error;
			}
		}
	}

	exit(0);
lspart_error:
	if (errno == 0)
		errno = EPINVALPART;
	perror("lspart");
	exit(1);
}

/* 		ls_partitions	
 *
 *
 *	Return:
 *		0 upon success
 *		-1 if error
 *
*/
int
ls_partitions(uid_t uid, gid_t gid, int rflag,int pflg,char *partname)
{
	PATHTYPE	path;
	DIR		dir;
	struct dirent	dir_entry;
	int		save_len;
	int		status;
	int		print_name;
	int		got_parent;
        nx_part_info_t  part;		/* Partition info */
        BITMAP_T        *bitmap;        /* Bitmap pointer */
        nx_nodes_t      lp_map;     	/* logical to phys node map */
	struct list	*l;
	char		*parent_name,*name;
    int     ret;

	print_name = FALSE;
	got_parent = FALSE;

	if (init_pathtype(&path) != NULL){
		if (append_path(&path,partname) == NULL){
			goto lspart_error;
		}
	}
	else{
		goto lspart_error;
	}

#ifdef DEBUG
    printf("file to open:  %s\n", path.space);
#endif

	ret = opendir_r(path.space, &dir);  /* check for error condition on open */
    if (ret == -1) {
#ifdef DEBUG
      printf("opendir for %s failed\n", path.space);
#endif
      goto lspart_error;
    }

	if ((parent_name = filename_to_partname(path.space)) == (char *)0) {
    /* ensure we found parent */
#ifdef DEBUG
      printf("parent_name is null\n");
#endif
      goto lspart_error;
    }

	if (get_partition_stuff(parent_name,&part,&lp_map) != 0)
		goto lspart_error;

	got_parent = TRUE;

	while ( (readdir_r(&dir, &dir_entry) != -1)) {
		save_len = strlen(path.space);
		if ( append_path(&path,"/") == NULL){
			goto lspart_error;
		}
		if ( append_path(&path,dir_entry.d_name) == NULL){
			goto lspart_error;
		}
		if ((isdir(path.space)) &&
                    (strcmp(".", dir_entry.d_name) != 0)         &&
		    (strcmp("..", dir_entry.d_name) != 0))
		{
			name = filename_to_partname(path.space);
			if (get_partition_stuff(name,&part,&lp_map) == 0){
				print_part_info_ext(TRUE,FALSE,rflag,
						&print_name,parent_name,
						dir_entry.d_name,indent_str,
						&part,lp_map, lflg, pflg);
				if (rflag){
					l = create_item(path.space);
                        		add_to_list(&ptr,l);
				}
				partitions_found++;
				FREE( lp_map);
			}
			else{
			/* We don't have read access to this partition
			* but we can read the parent. So we will display
			* only part of the partition information
			*/
				print_part_info(TRUE,TRUE,rflag,
						&print_name,parent_name,
						dir_entry.d_name,
						indent_str,&part,
						lp_map);
				partitions_found++;
			}
			FREE(name);
		}
		/* truncate path to original length */
		path.space[save_len] = '\0';
	} 
	free(parent_name);
	closedir(&dir);
	free_path(&path);
lspart_error:
	if ( (partitions_found == 0) && (got_parent == FALSE) )
		return(-1);
	else
		return(0);
}

/********************************  Usage  ********************************
 *
 *      Calling Sequence:
 *             Usage();
 *
 *      Description:
 *             Prints the Usage message
 *
 *      Parameters:
 *              NONE
 *
 *      Returns:
 *             NONE
 *
 *
 */

void 
Usage()
{
	fprintf(stderr,"lspart: syntax error\n");
	fprintf(stderr,"Usage: lspart [ -l | -p ] [ -r ] [ partition ]\n");
}

int 
get_partition_stuff(char *name, nx_part_info_t *partinfo, nx_nodes_t *nodes)
{
	int	ret;
	unsigned long num_free,num_nodes;
	nx_nodes_t free_nodes;

	if (nx_get_partition_attributes(name,partinfo) == -1) {
		ret = -1;
#ifdef DEBUG
        printf("nx_get_partition_attributes failed\n");
#endif
	}
	else
	if (nx_get_node_list(name,nodes,&num_nodes) == -1) {
		ret = -1;
#ifdef DEBUG
        printf("nx_get_node_list failed\n");
#endif
	}
	else  {
#ifdef DEBUG
      printf("back from nx_get_node_list, nodes: %x\n",nodes);
#endif
	  if (nx_free_nodes(name,&free_nodes,&(partinfo->free)) == -1) {
		  ret = -1;
#ifdef DEBUG
          printf("nx_free_nodes failed\n");
#endif
	  }
	  else {
		  ret = 0;
      }
    }

	return(ret);
}
