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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/allocsys_.c,v 1.7 1994/11/19 02:29:04 mtm Exp $
*/

/* Fortran interface to allocator system calls */

#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mode.h>
#include <sys/errno.h>
#include <sys/limits.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <errno.h>
#include <dirent.h>
#include <malloc.h>
#include <unistd.h>
#include <strings.h>
#include <mach_error.h>
#include <mach/message.h>
#include <mach/norma_special_ports.h>
#include <mach/mach_host.h>

#include        <mcmsg/mcmsg_appl.h>
#include        <nx/allocator.h>
#include        <nx/bitmap.h>
#include        <nx/defines.h>
#include        <nx/utils.h>
#include        <allocsys.h>
#include        <nx/partutils.h>


static char	newname[256];

int
nx_init_partinfo_(nx_part_info_t *part)
{
	return(nx_init_partinfo(part));
}

int
nx_part_attr_(char *name,nx_part_info_t *part,int nlen )
{
	_copyname(name, newname, nlen, sizeof(newname));
	return(nx_get_partition_attributes(newname,part));
}

int
nx_part_nodes_(char *name, nx_node_list_t *map,
		unsigned long *mapsize,long nlen)
{
	_copyname(name, newname, nlen, sizeof(newname));
	return(nx_get_node_list(newname,map,mapsize));
}

int
nx_app_nodes_(pid_t *pgroup,nx_node_list_t *map,
		unsigned long *mapsize)
{
	return(nx_app_nodes(*pgroup,map,mapsize));
}

int
nx_free_nodes_(char *name, nx_node_list_t *map,
		unsigned long *mapsize,long nlen)
{
	_copyname(name, newname, nlen, sizeof(newname));
	return(nx_free_nodes(newname,map,mapsize));
}

int nx_failed_nodes_(nx_node_list_t *unavail_map, 
		unsigned long *failed_map_size)
{
	return(nx_get_failed_nodes(unavail_map,failed_map_size));
}

int nx_empty_nodes_(nx_node_list_t *error_map, 
			unsigned long *error_map_size)
{
	return(nx_get_config_errors(error_map,error_map_size));
}

