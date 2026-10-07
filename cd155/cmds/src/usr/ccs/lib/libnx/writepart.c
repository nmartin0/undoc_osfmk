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
 *      $Id: writepart.c,v 1.4 1994/11/19 02:32:43 mtm Exp $
 *
 */

/*
 * writepart.c write out partition files
 *
 * HISTORY
 * $Log: writepart.c,v $
 * Revision 1.4  1994/11/19  02:32:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/18  19:25:36  dleslie
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
 * Revision 1.2  1993/05/27  18:40:47  carbajal
 * Cleaned up some comments
 *
 * Revision 1.1  1993/05/06  18:21:46  carbajal
 * New module, pass in all necessary parameters
 *
*/

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

/*******************  write_partinfo  *****************************
 *
 *      Description:
 *             Updates the partition information in the .partinfo file
 *
 *      Parameters:
 *		partname	partition pathname
 *		part		pointer to partition structure
 *		bitmap		pointer to bitmap structure for partition
 *		p_ndlist	pointer to node list for partition
 *
 *      Returns:
 *		TRUE	all is ok 
 *		FALSE	otherwise
 *
 *      Called by:
 *              chpart,mkpart,allocator 
 *
*/

int
write_partinfo(char *partname,PARTREQ_T *part,BITMAP_T *bitmap,LP_MAP_T p_ndlist)
{
	int	i,ok;
	FILE	*fd;
	PATHTYPE path;

	ok = TRUE;

	if (init_pathtype(&path) == NULL)
		ok = FALSE;
	else
	if (append_path(&path,partname) != NULL){
		if (append_path(&path,PARTINFO_STR) == NULL)
			ok = FALSE;
	}
	else
		ok = FALSE;

	if (ok == FALSE)
		return(ok);

#ifdef DEBUG
printf("write_partinfo %s\n",partname);
printf("write_partinfo %s\n",path.space);
#endif DEBUG

        /*
         * In the new partition, create a .partinfo and fill it
         * with "part", bitmap and p_ndlist
         */

	if ( (fd = fopen(path.space,"w")) != NULL){
        	fprintf(fd,"%u\n",part->uid);
        	fprintf(fd,"%u\n",part->gid);
        	fprintf(fd,"%o\n",part->access);
        	fprintf(fd,"%d\n",part->nodes);
        	fprintf(fd,"%d\n",part->slots);
        	fprintf(fd,"%d\n",part->rect.rows);
        	fprintf(fd,"%d\n",part->rect.cols);
        	fprintf(fd,"%u\n",part->sched);
        	fprintf(fd,"%u\n",part->rq);
        	fprintf(fd,"%d\n",part->maxpri);
		/* Always use the slot count, because it is only the
		 * root partition that can have slots != nodes
		*/
        	for( i = 0; i < part->slots; i++ ) {
                  	if( i == 0 )
                         	fprintf(fd,"%d",p_ndlist[i]);
                  	else  {
	
			   	putc(',',fd);
                           	fprintf(fd,"%d",p_ndlist[i]);
                  	}
        	}
        	putc('\n',fd);
        	fprintf(fd,"%u\n",bitmap->rows);
        	fprintf(fd,"%u\n",bitmap->cols);
        	for( i = 0; i <  bitmap->cols; i++ ) {
                  	if( i == 0 )
                         	fprintf(fd,"%d",bitmap->colmap[i]);
                  	else  {
                           	putc(',',fd);
                           	fprintf(fd,"%d",bitmap->colmap[i]);
                  	}
        	}
        	putc('\n',fd);
        	fflush(fd);
		fclose(fd);
		ok = TRUE;
	}
	else
		ok = FALSE;

	/* Make sure that root is the owner of the .partinfo files
	 * and the permissions are set to rw-rw-r
	*/
	if ( chmod(path.space,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) == -1)
		ok = FALSE;

	free_path(&path);
	return(ok);
}
