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
 *      $Id: partlock.c,v 1.10 1994/11/19 02:32:15 mtm Exp $
 *
 */

/*
 * partlock.c- 
 *
 * HISTORY
 * $Log: partlock.c,v $
 * Revision 1.10  1994/11/19  02:32:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1993/11/18  19:25:23  dleslie
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
 * Revision 1.8  1993/07/29  18:32:11  carbajal
 * Removed ask_for_allocator_port()
 *
 * Revision 1.7  1993/07/20  17:54:23  carbajal
 * Moved allocator_cmds_setup and ask_for_allocator_port() to
 * here.
 *
 * Revision 1.6  1993/04/12  22:23:48  carbajal
 * Moved number of retries up
 *
 * Revision 1.5  1993/01/28  21:42:53  shala
 * Fixed multiply defined LP_MAP_T .
 *
 * Revision 1.4  1992/12/10  21:14:59  shala
 * Fixed path to mach_error.h include file.
 *
 * Revision 1.3  1992/11/17  19:24:13  carbajal
 * Deleted EPLOCK, this is now in errno.h.
 * Corrected setting of error code if the allocator cannot
 * be reached.
 *
 * Revision 1.2  1992/11/03  01:40:17  carbajal
 * Added EPLOCK
 *
 * Revision 1.1  1992/11/03  00:16:16  carbajal
 * New module to make lock and unlock
 * calls to the allocator.
 *
 */
char _partlock_c_id[]="$Id: partlock.c,v 1.10 1994/11/19 02:32:15 mtm Exp $";

#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mode.h>
#include <strings.h>
#include <errno.h>
#include <mach_error.h>
#include <mach/message.h>
#include <mach/norma_special_ports.h>
#include <mach/mach_host.h>

#include <mcmsg/mcmsg_appl.h>
#include <nx/allocator.h>
#include <nx/bitmap.h>
#include <nx/defines.h>
#include <nx/mkpart.h>
#include <nx/utils.h>
#include <nx/nx_getopt.h>

#define	MAX_RETRIES	40

int
try_lock()
{
	mach_port_t	serv_port;
	kern_return_t	retcode;
	int		error;
	int		ok;
	int		retries;

	ok = FALSE;
	retries = 0;
	error = 0;

	if ( ask_for_allocator_port(&serv_port)){
		do{
			if (retries > 0) 
				sleep(1);
			retcode = lock_part(serv_port,&ok,&error);
		
			if(retcode != KERN_SUCCESS) {
				errno = EPALLOCERR;
			}
			else
			if( error != ESUCCESS ) {
				errno = error;
			}
			else
				if (ok) 
					break;
			retries++;
		}while (retries < MAX_RETRIES);
		/* check to see that we properly asked the allocator for the
		 * lock, and the allocator properly responded with a yes or
		 * no.
		*/
		if ((retcode == KERN_SUCCESS) && (error == ESUCCESS) && !ok)
			errno = EPLOCK;
	}

	return(ok);

}

int
release_lock()
{
	mach_port_t	serv_port;
	kern_return_t	retcode;
	int		error = 0;
	int		ok;

	ok = TRUE;

	if ( ask_for_allocator_port(&serv_port)){
#ifdef DEBUG
printf("try lock \n");
#endif DEBUG
		retcode = unlock_part(serv_port,&error);
		if(retcode != KERN_SUCCESS) {
			errno = EPALLOCERR;
			ok = FALSE;
		}
		else
		if( error != ESUCCESS ) {
			errno = error;
			ok = FALSE;
		}
	}
	return(ok);
}


/*	allocator_cmds_setup		*
 *	
 *	Generic routine for allocator commands to validate partition
 *	name and get partition information from .partinfo file
 *
 *	CALLED BY:
 *		chpart
 *	PARAMETERS:
 *		argcount=	argument count
 *		argc	=	argument count from main
 *		argv	=	argument list from main()
 *		name	=	char pointer to partition name in argv
 *		progname=	argv[0] from main() for perror
 *		defpart = 	NULL if it is not ok to use the
 *				environment variable NX_DFLT_PART
 *				otherwise it is set to the value of
 *				NX_DFLT_PART
 *	RETURNS:
 *		exit(1) if an error occurs 
 *	EXTERNAL DEPENDENCIES:
 *		Function Usage()
 *		p_ndlist
 *		bitmap
 *		part
*/		 
void
allocator_cmds_setup(int argcount, int argc, char *argv[], char **name, \
		     char **partname,char *progname,char *defpart,
			PARTREQ_T *part, BITMAP_T **bitmap, LP_MAP_T *p_ndlist)
{
	char	*tmpname;

	tmpname = get_partition_name(argcount,argc,argv,
				name,progname,defpart);

	/* 
	* The user sees the partition name to be of the form abc.xyz.jkl
	* We need to replace those .s with /s
	*/

	dottoslash(tmpname);

	/*
	* create a partition name from the argument. If the name starts
	* with a / consider it relative to /etc/nx otherwise
	* make it relative to /etc/nx/compute
	*/      
	if( ( *partname = create_partname(tmpname)) == NULL ) {
		perror(progname);
		exit(1);
	}

	/* get partition information */ 
	if ( get_part_info(*partname, part, bitmap, p_ndlist) < 0 ) {
		perror(progname);
		exit(1);
	}
}

