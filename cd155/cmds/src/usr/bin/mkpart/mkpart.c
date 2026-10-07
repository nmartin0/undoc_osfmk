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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/mkpart/mkpart.c,v 1.41 1994/11/19 01:30:26 mtm Exp $
 * History:
 * $Log: mkpart.c,v $
 * Revision 1.41  1994/11/19  01:30:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.40  1994/07/06  17:35:52  mag
 * Make -nt ,,, an error
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 10073
 *  Testing: developer
 *  Module(s): libnx/attributes.c, libnx/utils.c, libnx/nx_part_ops.c,
 * 	    libnx/nx_initve.c, allocator/mkpart_rpc.c, mkpart.c
 *
 * Revision 1.39  1994/06/15  01:36:35  mag
 * mkpart -epl 5 -sps fails, while mkpart -sps -epl 5 succeeds
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: 9767
 *  Testing: developer
 *  Module(s): mkpart.c
 *
 * Revision 1.38  1994/06/15  00:46:18  mag
 * Better handling of syntax errors in -nt arg (9845)
 * Better handling of multiple args (9846)
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: 9845, 9846
 *  Testing: developer
 *  Module(s): mkpart.c
 *
 * Revision 1.37  1994/06/10  17:54:49  mag
 * Added -rlx to Usage error message
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: 9789
 *  Testing: developer
 *  Module(s): mkpart.c
 *
 * Revision 1.36  1994/06/01  20:53:17  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): mkpart.c
 *  Related: libnx, server, emulator, allocator, bootmesh, showpart, lspart
 *
 * Revision 1.34  1993/12/01  18:45:58  carbajal
 * Only set gang scheduling if it is specified or they have specified a
 * rq value
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.33  1993/11/18  20:15:09  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get mcmsg and nx headers and libs out of export tree
 *  Testing: built on Suns and 486
 *  Module(s): Makefile mkpart.c
 *
 * Revision 1.32  1993/11/17  06:56:08  carbajal
 *  Set partinfo.rq == NO_SCHED_SPECIFIED only if there is no rq or epl
 *  specified
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: More R1.2 User Model Changes
 *  Testing:
 *  Module(s):
 *
 * Revision 1.31  1993/11/17  04:35:18  carbajal
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Set scheduling to NONE if not specified
 *  Testing:
 *  Module(s):
 *
 * Revision 1.30  1993/11/17  02:58:45  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.29  1993/09/17  00:21:05  carbajal
 * Fixed PTS #6117 - extraneous error messages
 *
 * Revision 1.28  1993/09/16  23:47:08  carbajal
 * Made usage match man page
 *
 * Revision 1.27  1993/07/20  18:15:08  carbajal
 * Call nx_init_partinfo() to properly setup partinfo structure.
 * Cleaned up some error reporting
 *
 * Revision 1.26  1993/07/14  02:04:56  carbajal
 * Deleted create.h from include list
 *
 * Revision 1.25  1993/07/13  21:03:17  carbajal
 * Make NX_mkpart() system call
 *
 * Revision 1.24  1993/05/06  18:25:10  carbajal
 * Added include file writepart.h
 *
 * Revision 1.23  1993/05/06  18:24:10  carbajal
 * Changed parameters to write_partinfo()
 *
 * Revision 1.22  1993/04/29  00:31:33  carbajal
 * Corrected error message is allocator is dead.
 * Fixed the problem where -ss and -mod would not work together. This also
 * no checks to make sure that -ss and -epl were not specified together.
 *
 * Revision 1.21  1993/02/18  03:48:06  carbajal
 * Changed partition argument from NX_DFLT_PART to NULL in get
 * partition name call. PTS# 4075
 *
 * Revision 1.20  1993/01/28  19:53:01  carbajal
 * Deal with multiply defined LP_MAP_T
 *
 * Revision 1.19  1993/01/18  22:06:04  carbajal
 * Parameters to check_access were backwards.
 *
 * Revision 1.18  1993/01/18  20:02:09  carbajal
 * mkpart.c: added n_rects to create_ndlist_bitmap
 *
 * Revision 1.17  1992/12/24  03:13:38  carbajal
 * removed nodes from send_nd_allocator()
 *
 * Revision 1.16  1992/12/24  03:09:51  carbajal
 * Backed out SIZEANY capability for PT7.
 *
 * Revision 1.15  1992/12/18  02:58:03  carbajal
 * use routines in partutils.c
 * rmdir(partname) if we get an error from write_partinfo
 * Implemented use of SIZEANY if -sz or -nd are not used.
 *
 * Revision 1.13  1992/11/15  23:08:18  shala
 * Save parent's slot size and pass this to create_ndlist. The part.slots
 * is readjusted after calling parse_nodelist and is no longer valid for
 * the parent.
 *
 *
 */

#include <sys/types.h>
#include <sys/dir.h>
#include <mach/mach.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <allocsys.h>
#include <nx.h>

#include <mach/mach.h>
#include <mach_error.h>
#include <mach/mig_errors.h>
#include <mach_init.h>
#include <mach/norma_special_ports.h>

#include <mcmsg/mcmsg_appl.h>
#include <nx/bitmap.h>
#include <nx/allocator.h>
#include <nx/defines.h>
#include <nx/partutils.h>
#include <nx/writepart.h>
#include <nx/nx_getopt.h>
#include <nx/utils.h>
#include <nx/parsesched.h>
#include <nx/parsepart.h>


void Usage();

/********************************  mkpart ********************************
 *        
 *      Calling Sequence:
 *             NONE
 *
 *      Description:
 *            mkpart creates a partition from the arguments. The user
 *            may want specific set of nodes or want a certain size.
 *            If the request is for certain set of nodes, the arguments
 *            are parsed to create a bitmap and a logical-physical node
 *            list and are passed to the allocator. Otherwise the requested
 *            size is passed to the allocator which in turn returns the
 *            the bitmap and the logical-physical nodelist. 
 *  
 *      Parameters:
 *              NONE
 *
 *      Returns:
 *               status
 *
 *
 */

main(argc,argv)
int argc;
char *argv[];
{
    register i;
    
    long status;          /* Status */
    long optindold;       /* argv index of the next argument - 1 */ 
    long argcount;        /* count of flags processed */
    char *rollquant;      /* argument to -rq */
    char *nodedesc;       /* argument to -nd or -sz */
    char *access;         /* argument  to mod */
    char *s;              /* option */
    char *name;           /* Last argument of mkpart */
    char *partname;       /* Name of the partition */
    int  maxpri;          /* Maximum Priority */
    short ndflg, szflg, modflg, spsflg, ntflg,
	  eplflg, ssflg, rqflg; /* Option flags */
    char *selector_buf;
    char *selectors;
    int  selector_cnt;
    int selector_buflen;
    int rlxflg;

    LP_MAP_T l_ndlist;
    nx_part_info_t	partinfo;

    optindold = nx_optind; /* nx_optind is initialized to 1 in nx_getopt() */
    ndflg = 0;
    rqflg = 0;
    ssflg = 0;
    modflg = 0;
    eplflg = 0;
    szflg = 0;
    spsflg = 0;
    rlxflg = 0;
    ntflg = 0;
    argcount = 1;
    selector_cnt = 0;
    selector_buflen = 0;
    selector_buf = (char*)0;
    
    /*
     *  Parse options.
     */
    while((s = nx_getopt(argc,argv, "sz:,nd:,rq:,epl:,mod:,ss,sps,nt:,rlx")) != NULL) {

#ifdef DEBUG
printf("and the option flag is %s \n",s);
#endif

            if(  strcmp(s,"sz") == 0 )  {
                              if(ndflg || szflg) {
                                      Usage();
                                      exit(USAGE_EXIT);
                                }
                                argcount += (nx_optind - optindold);
                                optindold = nx_optind;
                                szflg = 1;
                                nodedesc = nx_optarg;

               } else if( strcmp(s,"nd" ) == 0 )  {
             
                                if(szflg || ndflg) {
                                      Usage();
                                      exit(USAGE_EXIT);
                                }
                                argcount += (nx_optind - optindold);
                                optindold = nx_optind;
                                ndflg = 1;
                                nodedesc = nx_optarg; 

                } else if( strcmp(s,"rq" ) == 0)  {
                                if(ssflg || spsflg || rqflg) {
                                      Usage();
                                      exit(USAGE_EXIT);
                                }
                                argcount += (nx_optind - optindold);
                                optindold = nx_optind;
                                rqflg = 1;
                                rollquant = nx_optarg;

                } else if( strcmp(s,"epl") == 0 )  {
                                if(ssflg || eplflg) {
                                      Usage();
                                      exit(USAGE_EXIT);
                                }
                                argcount += (nx_optind - optindold);
                                optindold = nx_optind;
                                eplflg = 1;
				if( isnumber(nx_optarg) == 0 ) {
					errno = EPINVALPRI;
					goto internal_error;
				}
                                maxpri = atoi(nx_optarg);

                } else if( strcmp(s,"mod") == 0 )  {
                                if(modflg) {
                                      Usage();
                                      exit(USAGE_EXIT);
                                }
                                 argcount += (nx_optind - optindold);
                                 optindold = nx_optind;
                                 modflg = 1;
                                 access = nx_optarg;

                } else if( strcmp(s,"ss") == 0 )  {
                                if( spsflg || rqflg || eplflg || ssflg) {
                                      Usage();
                                      exit(USAGE_EXIT);
                                }
                                argcount += (nx_optind - optindold);
                                optindold = nx_optind;
                                ssflg = 1;
                } else if( strcmp(s,"sps") == 0 )  {
                                if( ssflg || rqflg || spsflg) {
                                      Usage();
                                      exit(USAGE_EXIT);
                                }
                                argcount += (nx_optind - optindold);
                                optindold = nx_optind;
                                spsflg = 1;
                } else if( strcmp(s,"nt") == 0 )  {
                                if( ntflg ) {
                                      Usage();
                                      exit(USAGE_EXIT);
                                }
                                argcount += (nx_optind - optindold);
                                optindold = nx_optind;
                                ntflg = 1;
                                selectors = nx_optarg; 
                } else if( strcmp(s,"rlx") == 0 )  {
                                if( rlxflg ) {
                                      Usage();
                                      exit(USAGE_EXIT);
                                }
                                argcount += (nx_optind - optindold);
                                optindold = nx_optind;
                                rlxflg = 1;
               } else {
                                Usage();
                                exit(USAGE_EXIT);
                  }
     }

     if ( (partname = get_partition_name(argcount,argc,argv,
				&name,argv[0],NULL)) == NULL){
		errno = USAGE_ERROR;
		goto internal_error;
     }

	/* Initialize the partinfo structure */
	nx_init_partinfo(&partinfo);

	l_ndlist = (LP_MAP_T)0;
	if (szflg) { 
		if ( (partinfo.nodes = parse_sdesc(nodedesc,&l_ndlist)) == 0)
			goto internal_error;
	}
	else if (ndflg) {
		if ( (partinfo.nodes = parse_ndesc(nodedesc,&l_ndlist)) == 0)
			goto internal_error;
	}

	if( rqflg ) {
		status = parse_rq(rollquant,&partinfo.rq); 
		if( status < 0 ) {
			goto internal_error;
		}
	}

	if( modflg ) { 
		if( parse_mod(access,&partinfo.access) < 0 ) {
			errno = EPINVALMOD;
			goto internal_error;
		}
#ifdef DEBUG
printf("new access %o \n",partinfo.access);
#endif
	}

	if(eplflg) 
		partinfo.epl = maxpri; 

	if (ssflg)
		partinfo.sched = NX_STD;
	else
	if (spsflg)
		partinfo.sched = NX_SPS;
	else
	if (rqflg)
		partinfo.sched = NX_GANG;
	else
		partinfo.sched = NO_SCHED_SPECIFIED;

	if (ntflg) {
	    selector_cnt =
	      NX_analyse_selector_string(selectors, &selector_buflen);
	    if (selector_cnt == -1)
		goto internal_error;
	    selector_buf = (char*) malloc(selector_buflen);
	    s = selector_buf;
	    NX_parse_selector_string(selectors, &s);
	} else
	    selector_buf = (char*)0;

	if (szflg)
		status = NX_mkpart_attr(MKPART_SZ,partname,partinfo,l_ndlist, rlxflg, selector_cnt, selector_buf, selector_buflen);
	else
		status = NX_mkpart_attr(MKPART_ND,partname,partinfo,l_ndlist, rlxflg, selector_cnt, selector_buf, selector_buflen);
		
	if (status > 0)
        	exit(0);
	else
		goto internal_error;


internal_error:
	if( errno == USAGE_ERROR ) 
		Usage();
	else  
		perror("mkpart");
	exit(1);

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
 *      Called by:
 *              mkpart
 *
 */

void Usage()
{
  fprintf(stderr,"mkpart: syntax error\n");
  fprintf(stderr,"Usage: mkpart [ -sz size | -sz hXw | -nd nodespec ] [\n");
  fprintf(stderr,"       -ss | -sps | [ [ -rq time ] [ -epl priority ] ] ] \n");
  fprintf(stderr,"       [ -mod mode ] [ -nt spec ] [ -rlx ]\n");
  fprintf(stderr,"       partition\n");
}

