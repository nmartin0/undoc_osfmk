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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/rmpart/rmpart.c,v 1.19 1994/11/19 01:37:10 mtm Exp $
 *
 */


/* History
 * $Log: rmpart.c,v $
 * Revision 1.19  1994/11/19  01:37:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.18  1993/11/18  20:15:32  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get mcmsg and nx headers and libs out of export tree
 *  Testing: built on Suns and 486
 *  Module(s): Makefile rmpart.c
 *
 * Revision 1.17  1993/09/16  23:48:01  carbajal
 * Made usage message match man page #3883
 *
 * Revision 1.16  1993/07/13  21:02:12  carbajal
 * Make NX_rmpart() system call
 *
 * Revision 1.15  1993/05/06  18:52:00  carbajal
 * Changed call to part_remove. Rmpart now get an inode list from
 * the allocator of the partitions to remove
 *
 * Revision 1.12  1992/12/20  00:37:45  carbajal
 * Ifdefed out delete_partitions
 *
 * Revision 1.11  1992/12/20  00:35:38  carbajal
 * Changed path to mach_error.h
 *
 * Revision 1.10  1992/12/18  03:01:49  carbajal
 * Better path name support
 *
 * Revision 1.8  1992/11/03  00:59:23  carbajal
 * Call try_lock and release_lock
 *
 * Revision 1.7  1992/10/22  01:01:42  carbajal
 * Mucho changes:
 * Rmpart now uses routines from partutils.c to
 * verify the partition name and create the partition
 * pathname.
 * send_rmpart_allocator calls a funtion in
 * partutils.c to get the allocator port.
 * send_rmpart_allocator is no longer in a separate file.
 * send_rmpart_allocator passes partinfo to the allocator.
 * All permission checks happen in the allocator.
 *
 * Revision 1.6  1992/10/15  21:51:46  carbajal
 * Moved the call to delete_partitions back into the
 * critical section.
 *
 * Revision 1.5  1992/10/12  17:03:15  carbajal
 * Added support for -r flag
 * Moved delete .partinfo code after the critical section
 *
*/

static char rcsid[] = "$Id: rmpart.c,v 1.19 1994/11/19 01:37:10 mtm Exp $";

#include <sys/types.h>
#include <sys/dir.h>
#include <mach/mach.h>
#include <mach_error.h>
#include <mach/mig_errors.h>
#include <mach_init.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <mcmsg/mcmsg_appl.h>
#include <nx/allocator.h>
#include <nx/bitmap.h>
#include <nx/defines.h>
#include <nx/nx_getopt.h>
#include <allocsys.h>
#include <nx/partutils.h>
#include <nx/mkpart.h>

void Usage();

/********************************  rmpart ********************************
 *        
 *      Calling Sequence:
 *             NONE
 *
 *      Description:
 *              Remove a partition. The user should have the write
 *              permissions on the partition to be able to remove it.
 *              If the force flag was set, the allocator will remove
 *              the partition and kill all the applications scheduled
 *              to run in that partition.
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
	int i;
	char *buf;             /* Temorary buffer */
	char *name;            /* Last argument of rmpart */
	char *partname;        /* Name of the partition */
	long optindold;        /* Argv index of the prevoius argument */
	long argcount;         /* Number of arguments in the command */
	short fflg;            /* option flags */
	short rflg;            /* option flags */

	optindold = nx_optind;
	fflg = 0;
	rflg = 0;
	argcount = 1;
    
	/*
	*  Parse options.
	*/

	while((buf = nx_getopt(argc,argv, "r,f")) != NULL) {
		if( strcmp(buf,"f" ) == 0 )  {
			argcount += (nx_optind - optindold);
			optindold = nx_optind;
                                fflg = 1;

            } else if( strcmp(buf,"r" ) == 0 )  {
                                argcount += (nx_optind - optindold);
                                optindold = nx_optind;
                                rflg = 1;
            }
	    else{
                                Usage();
                                exit(USAGE_EXIT);
            }
     }

	if ( (partname = get_partition_name(argcount,argc,argv,
                                &name,argv[0],NULL)) == NULL){
		Usage();
		exit(USAGE_EXIT);
	}

     if (NX_rmpart(partname,rflg,fflg) != 0){
		perror("rmpart");
		exit(1);
     }
     else
		exit(0);

}


/********************************  Usage  ********************************
 *
 *      Calling Sequence:
 *             NONE
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
  fprintf(stderr,"rmpart: syntax error\n");
  fprintf(stderr,"Usage: rmpart [-f] [-r] partition\n");
}
   
