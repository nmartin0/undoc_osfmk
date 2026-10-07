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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/initpart/initpart.c,v 1.17 1994/11/21 16:41:59 mtm Exp $
 *
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <sys/table.h>
#include <sys/user.h>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include <mcmsg/mcmsg_appl.h>
#include <nx/h.h>
#include <nx/bitmap.h>
#include <nx/allocator.h>
#include <nx/defines.h>
#include <nx/utils.h>
#include "bootmagic.h"
#include <nx/writepart.h>

extern 	int optind;
extern 	char *optarg;

/* option flags */
char 	*progname;
char	*root_dir =	"/etc/nx";
int	whitespace;

void build_config();

main(argc, argv)
int argc;
char *argv[];
{
	int	n;

	progname = argv[0];

	/* read in bootmagic */
	if (get_magic(bootmagicfile) < 0) {
		exit(1);
	}

	init_vars();
		
	if (check_root() != 0){
		perror("initpart");
		exit(1);
	}
	else
		exit(0);
}

Usage()
{
        printf("Usage %s:\n",progname);
}

char	*extract_bootenv();
void	update_bitmap();

int		paragon_mesh_x;
int		paragon_mesh_y;
char		*nodestring,*alt_nodestring;

/* global data declarations */
BITMAP_T *bitmap;
PARTREQ_T part;
LP_MAP_T  alt_p_ndlist,p_ndlist;
/* end global data */


char *
getbootenv(optional,v)
	int	optional;
	char	*v;
{
	char	*s;

	s = extract_bootenv(v);
	if ((optional == 0) && s == 0) {
		fprintf(stderr, "%s not found in bootmagic\n", v);
		exit(1);
	}
	return s;
}

init_vars()
{
	char	*s;

	s = getbootenv(0,"BOOT_MESH_X");
	paragon_mesh_x = atoi(s);
	s = getbootenv(0,"BOOT_MESH_Y");
	paragon_mesh_y = atoi(s);
	nodestring = getbootenv(0,"BOOT_NODE_LIST");
	alt_nodestring = getbootenv(1,"BOOT_ALT_NODE_LIST");
}

int
check_root()
{
	int		j,i,delete_it,create_it;
	int		status,alt_nodecnt,nodecnt,n;
	LP_MAP_T	lp_map;
	BITMAP_T	old_bitmap;

	create_it = FALSE;
	delete_it = FALSE;

	n = paragon_mesh_x * paragon_mesh_y;
	if ( (p_ndlist = (LP_MAP_T )malloc(n * sizeof(node_t))) == NULL){
		errno = ENOMEM;
		return -1;
	}

	if ( (alt_p_ndlist = (LP_MAP_T )malloc(n * sizeof(node_t))) == NULL){
		errno = ENOMEM;
		return -1;
	}

	bitmap = allocate_bitmap(paragon_mesh_x,paragon_mesh_y);
	init_bitmap(0,bitmap);
	if ( (lp_map = (LP_MAP_T)malloc(n * sizeof(node_t))) == NULL){
		errno = ENOMEM;
		return -1;
	}

	if ( get_part_info(root_dir,&part,&old_bitmap,&lp_map) == -1){
		/* no root partition so create it */
		create_it = TRUE;
	}
	else{
		/* check dimensions of root */
		if (n != part.slots)
			/* they have added/subtracted cabinets */
			delete_it = TRUE;
	}

	/* build boot node list from boot magic */
	nodecnt = nodeparser(nodestring,p_ndlist);
	if (alt_nodestring != (char *)0)
		alt_nodecnt = nodeparser(alt_nodestring,alt_p_ndlist);
	else
		alt_nodecnt = 0;

	/* append the alternate node list onto the boot node list.
	 * This list does not have to be sorted, we just use it 
	 * to create a bitmap.
	*/
	for (j = 0,i = nodecnt;(j < alt_nodecnt) && (i < n); i++,j++)
		p_ndlist[i] = alt_p_ndlist[j];

	nodecnt += alt_nodecnt;

	if (delete_it){
		delete_partitions(root_dir);
	}

	if (delete_it || create_it){
		status = mkdir(root_dir,0755);
		/* Make sure the permissions are 0755 on /etc/nx */
		if (chmod(root_dir,S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH) != 0)
			return(-1);
		build_config();
		part.uid = 0;
		part.gid = 1;
		part.access = 0754;
		part.slots = n;
		part.rect.rows = -1;
		part.rect.cols = -1;
		part.sched = SPACE_SHARE;
		part.rq = 0;
		part.maxpri = 5;
	}
	part.nodes = nodecnt;
	/* use the p_ndlist set by the bootmagic to set out bitmap */
	update_bitmap(paragon_mesh_x,nodecnt,p_ndlist,bitmap);
	/* use the complete logical node space determined by mesh_x and mesh_y
	 * as the root partition's physical node list
	*/
	for(i = 0; (i < n); i++){
		p_ndlist[i] = i;
	}
	if ( write_partinfo(root_dir,&part,bitmap,p_ndlist) != TRUE)
		return -1;
	else
		return 0;
}

void
update_bitmap(cols,nodecnt,node_array,bitmap)
int		cols;
int		nodecnt;
node_t		*node_array;
BITMAP_T	*bitmap;
{
	int	mod,i,div;

	for (i = 0; i < nodecnt; i++){
		div = node_array[i] / cols;
		mod = node_array[i] % cols;
		bitmap->colmap[mod] |= (1 << div);
	}
	
}

void
build_config()
{
	FILE	*fp;

	if ( (fp = fopen("/etc/nx/allocator.config","a")) != NULL){
		fprintf(fp,"NUM_GANG_PARTS=1\n");
		fprintf(fp,"DEGREE_OF_OVERLAP=2\n");
		fprintf(fp,"MIN_RQ_ALLOWED=1h\n");
		fclose(fp);
	}
}

