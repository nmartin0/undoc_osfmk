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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/create.c,v 1.7 1994/11/19 02:29:14 mtm Exp $
 *
 */

/* History
 * $Log: create.c,v $
 * Revision 1.7  1994/11/19  02:29:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/11/01  17:49:39  sdh
 * Added a check in create_ndlst_bitmap() to make sure that the
 * anchor node is not larger than the largest logical node in the
 * parent partition.
 *
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 10675 and 11085
 *  Testing:
 *  	EATS: controlc, rmcall, rmcmd, sched
 *  	manual tests
 *
 *  Module(s):
 * 	create.c
 *
 * Revision 1.5  1994/07/06  17:14:50  mag
 * mkpart -nd 1x5000:0 unexpectedly succeeds on some systems
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 9875
 *  Testing: developer
 *  Module(s): create.c
 *
 * Revision 1.4  1994/01/19  22:14:14  carbajal
 * MESH UTILS   18-JAN-94 **       18-JAN-94 18-JAN-94
 * mkpart fails mkpart_nd_random EAT
 *
 * Reviewer: Cameron
 * Risk: Low
 * Benefit or PTS #: Fixes EAT failure
 * Testing: rmcmd EAT
 * Module(s): create.c
 *
 * Revision 1.2.2.2  1994/01/19  19:15:12  carbajal
 *  7861 PARAGON      OPEN      H        carbajal  carbajal            R1.2 WW03
 *       MESH UTILS   18-JAN-94 **       18-JAN-94 18-JAN-94
 *       mkpart fails mkpart_nd_random EAT
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: Fixes EAT failure
 *  Testing: rmcmd EAT
 *  Module(s): create.c
 *
 * Revision 1.2.2.1  1994/01/06  23:06:51  carbajal
 *  5848 PARAGON      OPEN      M        carbajal  tracy               R1.1 T10.4
 *       MESH UTILS   12-NOV-93 **       23-SEP-93 21-JUL-93
 *       Some invalid mkpart -nd switches cause segmentation fault
 *
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 5848
 *  Testing: bug report
 *  Module(s): create.c
 *
 * Revision 1.2  1993/11/18  19:24:56  dleslie
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
 * Revision 1.1  1993/07/13  21:28:01  carbajal
 * Moved from ../usr/bin/mkpart subdirectory to here
 *
 * Revision 1.10  1993/04/29  00:27:54  carbajal
 * Make sure that we stay within the bounds of the starting node's column
 *
 * Revision 1.9  1993/02/23  20:45:10  carbajal
 * Fixed bug #4105 where -nd 1,2x1:3,4 was not being handled
 * correctly.
 *
 * Revision 1.8  1993/01/28  19:53:03  carbajal
 * Deal with multiply defined LP_MAP_T
 *
 * Revision 1.7  1993/01/18  20:02:28  carbajal
 * create.c: keep track of the number of rectangles
 * 	specified on the command line. If we
 * 	are allocating a rectangle then make sure
 * 	we are given the number of nodes we asked for.
 * 	This fixes the core dump seen in T7.
 *
 * Revision 1.6  1992/11/15  23:08:22  shala
 * Save parent's slot size and pass this to create_ndlist. The part.slots
 * is readjusted after calling parse_nodelist and is no longer valid for
 * the parent.
 *
 * Revision 1.5  1992/10/19  17:40:36  carbajal
 * When figuring out the logical to physical node list
 * on a rectangular specification look at the parent's
 * bitmap to obtain the physical node number.
 *
*/

#include <stdio.h>
#include  <mcmsg/mcmsg_appl.h>
#include  <nx/allocator.h>
#include  <nx/bitmap.h>
#include  <errno.h>
#include  <nx/defines.h>
#include  <nx/mkpart.h>

/********************************  create_ndlst_bitmap ***********************
 *
 *      Calling Sequence:
 *             create_ndlst_bitmap(parentsize,n_rects,rect);
 *
 *      Description:
 *             create a physical node list (an array of physical node numbers
 *             indexed by logical node numbers ) and a bitmap from the
 *	       logical node list.
 *
 *      Parameters:
 *		parentsize = number of slots in parent partition
 *		p_ndlist   = node list for parent
 *		parent_bitmap = bitmap for parent
 *		my_size    = number of nodes in l_ndlist
 *		l_ndlist   = node list filled in by caller
 *		num_nodes  = resulting number of nodes filled in by this
 *				procedure
 *		my_nodes   = resulting node list
 *
 *      Returns:
 *               status
 *
 *
*/

BITMAP_T *
create_ndlst_bitmap(int parentsize,LP_MAP_T p_ndlist,BITMAP_T *parent_bitmap,
			int my_size,LP_MAP_T l_ndlist,int *num_nodes,
			LP_MAP_T *my_nodes)
{
	register	i,j,k,l;
	int		starting_node;		/* logical node 0 */
	long		status;			/* status */
	int		position;		/* position of the node in the bitmap */
	int		starting_col;		/* starting column */
	int		current_col;		/* current column for error checking */
	int		rows,cols;		/* for rectangular specification */
	int		logical_index;		
	BITMAP_T	*my_bitmap;

#ifdef DEBUG
printf("create_ndlst_bitmap: Entered \n");
if (rect != (RECT_T *)0)
	printf("rect->rows %d rect->cols %d\n",rect->rows,rect->cols);
#endif

  /*
   * Allocate and initialize space for bitmap
   */
   if(( my_bitmap = allocate_bitmap(parent_bitmap->cols,parent_bitmap->rows))
                     == (BITMAP_T *) 0 ) {
		errno = ENOMEM;
		goto create_ndlst_error;
   }

   init_bitmap(0,my_bitmap);

   /* Allocate space for the node list to be returned */
   *my_nodes = (LP_MAP_T)malloc(sizeof(LP_MAP_ENTRY_T) * parentsize);
   if( *my_nodes == (LP_MAP_T) 0) {
	errno = ENOMEM;
	goto create_ndlst_error;
   }

   /*
    * For every element in the logical nodelist of the new partition, 
    * find the physical node number of that node by refering to the 
    * logical-physical nodelist of the parent 
    */

    i = 0;
    logical_index = 0;
    while ( i < parentsize && logical_index < my_size){
	   /* Make sure the logical node is within the parent's 
	    * bounds
	   */
	   if ( l_ndlist[logical_index] >= parentsize){
		errno = EPXRS;
		goto create_ndlst_error;
	   }
	   if (l_ndlist[logical_index] >= 0){
                     (*my_nodes)[i] = p_ndlist[l_ndlist[logical_index]];
                     status = setbit_bitmap(my_bitmap,1, (*my_nodes)[i]) ;
                     if( status < 0 )
                            goto create_ndlst_error;
                     i++;
		     logical_index++;
           } else {

		  if (logical_index + 2 > my_size)
			goto create_ndlst_error;
		  /* Get the dimensions of the rectangle */
		  rows = -l_ndlist[logical_index];
		  cols = -l_ndlist[logical_index+1];
		  starting_node = -l_ndlist[logical_index+2];
		  if (starting_node >= parentsize) {
                            errno = EPXRS;
                            goto create_ndlst_error;
                  }

		  logical_index += 3; 
                  (*my_nodes)[i] = p_ndlist[starting_node];
		  starting_node = (*my_nodes)[i];
		  starting_col  = starting_node % my_bitmap->cols;
		  if ((starting_col+cols) > my_bitmap->cols) {
			    errno = EPXRS;
			    goto create_ndlst_error;
		  }

                  for( j = 0; i < parentsize && j < rows; j++ ) {
                        for( k = 0; i < parentsize && k < cols; k++ ) {
#ifdef DEBUG
printf("looking for %d\n",starting_node+k);
#endif DEBUG
				/* Make sure we do not exceed our mesh
				 * dimensions
				*/
				current_col = (starting_node+k) % my_bitmap->cols;
				if (current_col < starting_col)
				{
					errno = EPXRS;
					goto create_ndlst_error;
				}

				if (getbit_bitmap(parent_bitmap,k+starting_node) == 1){
                                	(*my_nodes)[i] = k+starting_node;
#ifdef DEBUG
printf("setting %d\n",starting_node+k);
#endif DEBUG
                                	status = setbit_bitmap(my_bitmap,1,k+starting_node);
                                	if( status < 0 )
                                              goto create_ndlst_error;
                                	i++; 
				}
				else{
					/* bail out here we have exceeded our
					 * resources
					*/
					errno = EPXRS;
					goto create_ndlst_error;
				}
                        }
                       starting_node += my_bitmap->cols;
                  }
          }
     }

    	/* If the logical_index != my_size then we
	 * are exceeding our resources
	*/
	if (logical_index != my_size){
		errno = EPXRS;
		goto create_ndlst_error;
	}

	/* save the number of nodes */
	*num_nodes = i;

    
     /*
      * One more check before we are done
      */

	for ( i = 0; i < my_bitmap->cols; i++ ){
		if( my_bitmap->colmap[i] >  parent_bitmap->colmap[i] ){
			errno = EPXRS;
			goto create_ndlst_error;
		}
	}
         
	return(my_bitmap);
create_ndlst_error:
	*num_nodes = 0;
	if (my_bitmap != (BITMAP_T *)0)
		free(my_bitmap);
	return((BITMAP_T *)0);
}
