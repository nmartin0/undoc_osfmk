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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/showpart/showpart.c,v 1.19 1995/02/10 23:41:46 carolr Exp $
 *
 */

/* History
 * $Log: showpart.c,v $
 * Revision 1.19  1995/02/10  23:41:46  carolr
 * corrected the usage statement to indicate that the -l and -p flags
 * are mutually exclusive.
 *
 *  Reviewer: sdh
 *  Risk: low
 *  Benefit or PTS #: 10857
 *  Testing: controlc, rmcall, rmcmd, sched, manual testing
 *  Module(s):
 *     cmds_libs/src/usr/bin/lspart/lspart.c
 * 	cmds_libs/src/usr/bin/showpart/showpart.c
 *
 * Revision 1.18  1994/11/19  01:39:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.17  1994/10/14  23:28:48  sdh
 * Added the -w flag and the calls to show_wide_matching_bitmap()
 *
 *  Reviewer: david
 *  Risk: low
 *  Benefit or PTS #: 10566
 *  Testing:
 * 	manual, EATS: rmcmd
 *  Module(s):
 * 	usr/ccs/lib/libnx/bitmap.c
 * 	usr/bin/showpart/showpart.c
 *
 * Revision 1.16  1994/07/13  17:48:24  mag
 * Added -p (physical) and changed -l (logical) options to node attribute printing
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 10207
 *  Testing: developer
 *  Module(s): showpart.c
 *
 * Revision 1.15  1994/06/15  00:44:43  mag
 * Better handling of syntax errors in -nt arg (9845)
 * Flag multiple -nt args as error (9846)
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: 9845, 9846
 *  Testing: developer
 *  Module(s): showpart.c
 *
 * Revision 1.14  1994/06/08  17:17:23  mag
 * Correct core dumps when user passes bad args to showpart
 *  Reviewer: gregt
 *  Risk: Low
 *  Benefit or PTS #: 9720
 *  Testing: EATS: rmcmd
 *  Module(s): allocator/{attribute.c, attribute.h, init_appl.c, mkpart_rpc.c}
 * 	    showpart/showpart.c
 *  Related: None
 *
 * Revision 1.13  1994/06/01  20:54:08  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): Makefile, showpart.c
 *  Related: libnx, server, emulator, allocator, mkpart, bootmesh, lspart
 *
 * Revision 1.12  1993/12/07  01:21:15  carbajal
 * Added a parameter to nx_get_free_map(), this was causing the WW49 showpart
 * to core dump.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Makes it work
 *  Testing:
 *  Module(s):
 *
 * Revision 1.11  1993/11/18  20:15:45  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get mcmsg and nx headers and libs out of export tree
 *  Testing: built on Suns and 486
 *  Module(s): Makefile showpart.c
 *
 * Revision 1.10  1993/11/17  02:58:06  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.9  1993/03/30  17:51:06  carbajal
 * Changed a return statement out of main() to an exit(0)
 *
 * Revision 1.8  1993/01/28  19:52:18  carbajal
 * Deal with multiply defined LP_MAP_T
 *
 * Revision 1.7  1993/01/27  19:26:12  carbajal
 * Display partition information ala lspart before showing
 * bitmap
 *
 * Revision 1.6  1992/12/24  03:22:20  carbajal
 * Treat root as special by creating a bitmap
 * that represents the complete mesh (slots).
 *
 * Revision 1.5  1992/12/18  02:48:58  carbajal
 * Show bad and missing nodes
 *
 * Revision 1.4  1992/10/22  01:03:34  carbajal
 * Use .compute if there is no partition name and
 * NX_DFLT_PART is null.
 * MAke sure that pathnames are null terminated. This
 * fixes a bug we saw with long pathnames getting an
 * invalid partition.
 *
 * Revision 1.3  1992/10/17  00:42:59  carbajal
 * Get NX_DFLT_PART from environment if no partition name
 * is specified on the command line.
 *
*/

static char rcsid[] = "$Id: showpart.c,v 1.19 1995/02/10 23:41:46 carolr Exp $";

#include <stdlib.h>
#include <stdio.h>
#include <grp.h>
#include <pwd.h>
#include <sys/dir.h>
#include <errno.h>
#include <allocsys.h>

#include <mcmsg/mcmsg_appl.h>
#include <nx/allocator.h>
#include <nx/bitmap.h>
#include <nx/defines.h>
#include <nx/partprint.h>
#include <nx/nx_getopt.h>


void Usage();


BITMAP_T        *root_bitmap;
BITMAP_T        *bad_bitmap;
int		heading_printed;

main(argc, argv)
int argc;
char *argv[];
{
	char		*name,*path;		/* Partition name */
	char		*tmp_name,*root_path;
	
	int		part_idx,no_part,num_free,
			fflg,i,rootsize,len,print_name,
			lflg, wflg, ntflg, pflg;
	char		*indent_str;
	nx_part_info_t	partinfo;
	nx_part_info_t	root_partinfo;
	nx_node_list_t	lp,unavail_map;
	long		bmap_size,root_bmap_size,lp_size,
			unavail_size, matching_size;
	BITMAP_T        *free_map,*mesh,*bitmap, *matching;
	long            optindold; /* Argv index of the prevoius argument */
	long argcount;             /* Number of arguments in the command */
	char		*buf;
	char		*selectors;

	buf = indent_str = path = root_path = NULL;
	matching = free_map = mesh = bitmap = (BITMAP_T *)0;
	lp = unavail_map = (nx_node_list_t)0;
	
	optindold = nx_optind;
	argcount = 1;
	fflg = 0;
	lflg = 0;
	ntflg = 0;
	pflg = 0;
	wflg = 0;

	/*
        *  Parse options.
        */

        while((buf = nx_getopt(argc,argv, "f,nt:,l,p,w")) != NULL) {
                if( strcmp(buf,"f" ) == 0 )  {
                        argcount += (nx_optind - optindold);
                        optindold = nx_optind;
                        fflg = 1;
                }
                else if( strcmp(buf,"nt" ) == 0 )  {
			if (ntflg) {
				Usage();
				exit(USAGE_EXIT);
			}
			argcount += (nx_optind - optindold);
			optindold = nx_optind;
			ntflg = 1;
			selectors = nx_optarg;
		}
                else if( strcmp(buf,"l" ) == 0 )  {
			if (pflg) {
		                Usage();
       		         	exit(USAGE_EXIT);
			}	
                        argcount += (nx_optind - optindold);
                        optindold = nx_optind;
			lflg = 1;
		}
                else if( strcmp(buf,"w" ) == 0 )  {
                        argcount += (nx_optind - optindold);
                        optindold = nx_optind;
			wflg = 1;
		}
                else if( strcmp(buf,"p" ) == 0 )  {
			if (lflg) {
		                Usage();
                		exit(USAGE_EXIT);
			}
                        argcount += (nx_optind - optindold);
                        optindold = nx_optind;
			pflg = 1;
			lflg = 1;
		}
                else{
                        Usage();
                        exit(USAGE_EXIT);
                }
        }

	buf = NULL;
        if ( (buf = getenv(NX_DFLT_PART)) == NULL)
                buf = ".compute";

        tmp_name = get_partition_name(argcount,argc,argv,&name,argv[0],buf);
	if (!name) {
		Usage();
		exit(USAGE_EXIT);
	}
	
	if (nx_get_partition_attributes(name,&partinfo) == -1){
                        perror("showpart");
                        exit(1);
        }

	if (nx_get_node_list(name,&lp,&lp_size) == -1){
                        perror("showpart");
                        exit(1);
        }

	if (nx_get_part_bitmap(name,&bitmap,&bmap_size) == -1){
                        perror("showpart");
                        exit(1);
        }
	/*
	 * Get the root partition information
	 */
	if (nx_get_part_bitmap(".",&root_bitmap,&root_bmap_size) == -1){
		perror("showpart");
		exit(1);
        }

	if (nx_get_partition_attributes(".",&root_partinfo) == -1){
                        perror("showpart");
                        exit(1);
        }

	/* Get the bad nodes */
	if (nx_failed_nodes(&unavail_map,&unavail_size) == -1){
		perror("showpart");
		exit(1);
	}

	if (ntflg) {
		if (nx_get_matching_bitmap(&matching, &matching_size,
		  selectors) == -1) {
		    perror("showpart");
		    exit(1);
		}
	} else
	        matching = (BITMAP_T*) 0;

	/* if we are supposed to show free nodes then get the
	 * list of free nodes 
	*/
	if (nx_get_free_map(NX_ALL_SPACE,name,&free_map) == -1){
		perror("showpart");
		exit(1);
	}

	partinfo.free = num_nodes_in_map(free_map);

	bad_bitmap = bitmap_clone(root_bitmap);
	init_bitmap(0,bad_bitmap);
	for (i = 0; i < unavail_size; i++)
		setbit_bitmap(bad_bitmap,1,unavail_map[i]);

	indent_str = (char *)strdup(" ");
	print_part_info_ext(FALSE,FALSE,FALSE,&print_name,name,name,
		indent_str,&partinfo,lp, lflg, pflg);

	if ( strcmp(name,".") == 0){
		/* build a bitmap that represents the x y dimension
		 * of root
		*/
		mesh = allocate_bitmap(root_bitmap->cols,root_bitmap->rows);
		init_bitmap(1,mesh);
		if (wflg)
			show_wide_matching_bitmap(bad_bitmap,root_bitmap,mesh,
			fflg?free_map:(BITMAP_T*)0, matching);
		else	
			show_free_matching_bitmap(bad_bitmap,root_bitmap,mesh,
			fflg?free_map:(BITMAP_T*)0, matching);
	}
	else
		if (wflg)
			show_wide_matching_bitmap(bad_bitmap,root_bitmap,bitmap,
			fflg?free_map:(BITMAP_T*)0, matching);
		else
			show_free_matching_bitmap(bad_bitmap,root_bitmap,bitmap,
			fflg?free_map:(BITMAP_T*)0, matching);

	exit(0);


}

void
Usage()
{
	fprintf(stderr,"showpart: syntax error\n");
	fprintf(stderr,"Usage: showpart [-f] [-w] [-p | -l] [-nt node_types] [name]\n");
}
