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
 *      $Id: utils.c,v 1.30 1995/03/07 00:48:00 carolr Exp $
 *
 */

/*
 * utils.c - Nandini Ajmani - 3/10/92
 *
 * HISTORY
 * $Log: utils.c,v $
 * Revision 1.30  1995/03/07  00:48:00  carolr
 * Description:
 *   The problem was the default priority got set in _nx_initve_attr() (in nx_initve.c),
 *   but the parsing for this is done in parse_for_applinfo() (in utils.c)  Added argument
 *   to the call and to the function to pass the priority variable in.  Also added the
 *   code to parse for NX_ATTR_PRI and appropriately deal w/ the attribute in
 *   parse_for_applinfo().
 *
 *  Reviewer: sdh
 *  Risk: low
 *  Benefit or PTS #: PTS 12081
 *  Testing:
 *       EATS: controlc, rmcall, rmcmd, sched (sans the 12 node req'd test)
 *       test listed in PTS
 *  Module(s):
 *         cmds_libs/src/usr/ccs/lib/libnx/nx_initve.c,
 *         cmds_libs/src/usr/ccs/lib/libnx/utils.c
 *
 * Revision 1.29  1994/11/19  02:32:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.28  1994/09/27  16:43:13  nandy
 * Added routines nx_sighold() and nx_sigrelease().
 *
 *  Reviewer: scott hahn
 *  Risk: L
 *  Benefit or PTS #: Several bugs relating to controlC
 *  Testing: controlc eats.
 *  Module(s): nx_loadve.c, nx_nfork.c utils.c
 *
 * Revision 1.27  1994/08/31  20:22:44  bradf
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.26.2.1  1994/08/17  23:37:24  sdh
 * Set the arg_used variable to 1 under NX_ATTR_ANCHOR in parse_for_map()
 * where the anchor value was -1. This allows nx_mkpart_rect to work.
 *
 *  Reviewer: mag
 *  Risk: Low
 *  Benefit or PTS #: 10400
 *  Testing: manual, EATS (rmcall, rmcmd, controlc)
 *  Module(s):
 * 	utils.c
 *
 *
 * Added code to parse_for_part_info() that sets the sched field of the
 * partinfo struct to NX_GANG if an RQ was specified, but no sched was
 * specified.
 *
 *  Reviewer: simont
 *  Risk: Low
 *  Benefit or PTS #: 10495
 *  Testing: manual, EATS (rmcall, rmcmd, controlc)
 *  Module(s):
 * 	utils.c
 *
 * Revision 1.26  1994/07/27  20:39:19  mag
 * Don't allow a blank string as arg to NX_ATTR_SEL
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 10368
 *  Testing: developer, EATS: rmcmd, rmcall, sched
 *  Module(s): utils.c
 *
 * Revision 1.25  1994/07/19  20:37:10  mag
 * Correct handling of extraneous/duplicated NX_ATTR_xxx arguments (10278, 10301, 10303)
 * Catch 0xN and Nx0 rectangles as error (10281)
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 10278, 10301, 10303, 10281
 *  Testing: developer, EATS: rmcmd, rmcall
 *  Module(s): utils.c, nx_initve.c, nx_part_ops.c
 *
 * Revision 1.24  1994/07/06  17:27:04  mag
 * nx_initve_attr changes selector string argument (10076)
 * Make -nt ,,, an EINVAL (10073)
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 10076, 10073
 *  Testing: developer
 *  Module(s): (10076, 10073): utils.c
 * 	    (10073): nx_part_ops.c, nx_initve.c,
 * 		     allocator/mkpart_rpc.c, mkpart/mkpart.c
 *
 * Revision 1.23  1994/06/15  00:53:44  mag
 * Correct error code return in nx_initve_rect when size = -2
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: 9841
 *  Testing: developer
 *  Module(s): utils.c
 *
 * Revision 1.22  1994/06/01  20:15:56  mag
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
 * Revision 1.21  1993/11/18  19:25:34  dleslie
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
 * Revision 1.20  1993/09/21  21:42:04  carbajal
 * Expanded MAXBUF to handle longer node lists.
 * PTS #6722, 6723, 6724
 *
 * Revision 1.19  1993/07/13  21:41:42  carbajal
 * Use sscanf to convert bitmap columns to unsigned longs
 *
 * Revision 1.18  1993/05/27  22:01:25  carbajal
 * Removed some ifdefed JMC's
 *
 * Revision 1.17  1993/01/28  19:58:43  carbajal
 * Deal with multiply defined LP_MAP_T stuff by including mcmsg_appl.h
 *
 * Revision 1.16  1993/01/18  19:57:54  carbajal
 * Recommit - bomb out of last commit
 *
 * Revision 1.15  1993/01/18  19:55:56  carbajal
 * made read_partinfo more picky about what it finds in the .partinfo files
 *
 * Revision 1.14  1992/12/18  02:31:50  carbajal
 * path name tools
 * get_partinfo no longer filters bad_nodes out
 *
 * Revision 1.13  1992/11/12  20:39:22  shala
 * Change parsing .badnode list. Fixed by Don Cameron.
 *
 * Revision 1.12  1992/10/29  01:36:22  carbajal
 * Made root be all powerful in check_access.
 *
 * Revision 1.11  1992/10/23  01:39:57  rkl
 * Changed all gids from uid_t to gid_t.
 *
 * Revision 1.10  1992/10/22  18:25:33  carbajal
 * Changed check access again! Root is now treated the
 * same as the owner of the object.
 *
 * Revision 1.9  1992/10/22  01:23:08  carbajal
 * changes to check_access:
 * 	New access code OWNER check to see if we
 * 	are the owner of the partition
 * 	IAM_ROOT - root uid now can do everything!
 * 	Make sure CHECK_AGAINST_PARENT is not set
 *
 * Revision 1.8  1992/10/12  17:13:51  carbajal
 * Moved opendir_r and readdir_r from allocator.c into
 * here so that rmpart can use them also.
 *
 * Revision 1.7  1992/10/11  00:12:48  cameron
 * Changed get_part_info() to read /etc/nx/.badnodes to mask out any bad nodes
 * found during booting.
 *
 * Revision 1.6  1992/10/08  19:28:29  carbajal
 * Broke up get_part_info and added a read_part_info call
 * to be used by allocator commands
 * Reverse slot and node representation as per cameron
 *
 * Revision 1.5  1992/10/06  00:34:33  cameron
 * Support for empty slots in root partition.
 * Made Tom's suggested change abour strncpy().
 *
 * Revision 1.4  92/06/09  21:27:32  stans
 * added ID and ident strings
 * 
 */
char _utils_c_id[]="$Id: utils.c,v 1.30 1995/03/07 00:48:00 carolr Exp $";


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mode.h>
#include <sys/dir.h>
#include <strings.h>
#include <errno.h>
#include <mcmsg/mcmsg_appl.h>
#include <nx/allocator.h>
#include <nx/bitmap.h>
#include <nx/defines.h>
#include <nx/mkpart.h>
#include <nx.h>
#include <allocsys.h>

#define	MAXBUF	5 * 8192	

/* Forward references */
char *init_pathtype();
char *append_path();
void free_path();
/*********************/

#define strrpbrk(s1, s2) rindex(s1, *(s2))

/***************************** verify_name ***********************
 *
 *      Calling Sequence:
 *             verify_name(name);
 *
 *      Description:
 *              Verify that the name passed has letters, numbers and 
 *              "_" only
 *
 *      Parameters:
 *              NONE
 *
 *      Returns:
 *               status
 *
 *
 */

int
verify_name(name)
char *name;
{
  
  register i,len;


  len = strlen(name);
  for( i = 0; i < len; i++) {
          if(( *name >= 'a' && *name <= 'z' ) || ( *name >= 'A' 
		&& *name  <= 'Z') || ( *name >= '0' && *name <= '9' )
			 || *name == '_' || *name == '.' ) {
                         name++;
          } else {
                         errno = EPINVALPART;
                         return(-1);
           }
   }
           
   return(0);
} 



/*******************************  check_access  ********************************
 *
 *	Calling Sequence:
 *		check_access(uid, gid, obj_uid, obj_gid, op, rights)
 *
 *	Description:
 * 		Checks that the operation specified by "op" can be performed
 *		by a process with the specified uid and gid on an object with
 *		specified by access rights. 
 *
 *	Parameters:
 *		uid	User ID of process.
 *		gid	Group ID of process.
 *		obj_uid	User ID of object.
 *		obj_gid	Group ID of object.
 *		op	Operation to perform: READ, WRITE, EXECUTE, or OWNER
 *		rights	Access rights.	
 *
 *	Returns:
 *		1: Process has rights to perform operation.
 *		0: Process does not have rights to perform operation.
 */

int
check_access(uid, gid, obj_uid, obj_gid, doop, rights)
uid_t	uid;		/* User ID of process */
gid_t	gid;		/* Group ID  of process */
uid_t	obj_uid;	/* User ID of object */
gid_t	obj_gid;	/* Group ID of object*/
int	doop;		/* Operation to perform: READ, WRITE, EXECUTE, or OWNER */
int	rights;		/* Access rights */
{

	unsigned int	perm_user;	/* User permissions */
	unsigned int	perm_group;	/* Group permissions */
	unsigned int	perm_world;	/* World permissions */
	int		op,ok; 

	ok = FALSE;
	op = (~CHECK_AGAINST_PARENT) & doop;

#ifdef DEBUG
	printf("check access doop %d op %d\n",doop,op);
#endif DEBUG

	if ( (op != OWNER) && (uid != IAM_ROOT) ){
		if (op == WRITE) {
			perm_world = rights & 0002;
			perm_group = rights & 0020;
			perm_user = rights & 0200;
		} else if (op == READ) {
			perm_world = rights & 0004;
			perm_group = rights & 0040;
			perm_user = rights & 0400;
		} else if (op == EXEC) {
			perm_world = rights & 0001;
			perm_group = rights & 0010;
			perm_user = rights & 0100;
		}
		/* let root have the same access priviledges as the owner */
		if ( (uid == obj_uid ) && perm_user) 
			ok = TRUE;
		else 
			if ((gid == obj_gid) && perm_group) 
				ok = TRUE;
			else 
				if((uid != obj_uid ) && (gid != obj_gid) && 
						perm_world) 
					ok = TRUE;
	}
	else 
	if (uid == IAM_ROOT)
		/* root is all powerful */
		ok = TRUE;
	else{
		/* We need to own this partition to perform the requested
		 * action
		*/
		if ( uid == obj_uid ) 
			ok = TRUE;
	}

#ifdef DEBUG
printf("check access %d\n",ok);
#endif DEBUG
	return(ok);
}

/*******************************  get_inode  ********************************
 *
 *      Calling Sequence:
 *             get_inode(filename)
 *
 *      Description:
 *             returns the inode number of the filename
 *
 *      Parameters:
 *              NONE
 *
 *      Returns:
 *              inode 
 *
 *
 */

ino_t
get_inode(filename)
char *filename;
{
 long status;

 struct stat fs;    /* file status structure */ 

#ifdef DEBUG
printf("Filename to get_inode %s\n",filename);
#endif
 if(( stat(filename,&fs)) < 0 ) {
         errno = EPINVALPART;
         return(0); 
}

 return(fs.st_ino);

}

/********************************  create_partname ************************
 *
 *      Calling Sequence:
 *             create_partname(name);
 *
 *      Description:
 *             creates a partition name from the argument. If the name
 *             starts with a / consider it relative to /etc/nx otherwise
 *             it is relative to /etc/nx/compute.
 *
 *      Parameters:
 *              name:  Char pointer to the name of the partition
 *                     passed by the user.
 *
 *      Returns:
 *               status
 *
 *
 */

char *
create_partname(name)
char *name;
{

	PATHTYPE	partname;
	char		*tmpptr;

#ifdef DEBUG
printf("create_partname: %s\n",name);
#endif DEBUG

	if (init_pathtype(&partname) != NULL){
		/*
		* If the first character is a /, make it relative to  /etc/nx 
		*/
		if( !(strncmp(name,"/",1) )) 
			tmpptr = "/etc/nx";
		else
			tmpptr = "/etc/nx/compute/";
		if (append_path(&partname,tmpptr) != NULL)
			if (append_path(&partname,name) == NULL);
#ifdef DEBUG
printf("return name %s\n",partname.space);		
#endif DEBUG
	}

	return(partname.space);

}

/********************************  dottoslash ************************
 *
 *      Calling Sequence:
 *             dottoslash(name);
 *
 *      Description:
 *             substitutes . with / in the name    
 *
 *      Parameters:
 *              name:  chartacter pointer
 *
 *      Returns:
 *               NONE
 *
 *
 */

 
void
dottoslash(name)
char *name;
{

	char *tmpptr;     /* Temporary pointer */
 
	while(( tmpptr = strpbrk(name,".")) != NULL ) {
		*tmpptr = '/';
	}
 
	return;
}


/********************************  slashtodot ************************
 *
 *      Calling Sequence:
 *             slashtodot(name);
 *
 *      Description:
 *             substitutes / with . in the name    
 *
 *      Parameters:
 *              name:  chartacter pointer
 *
 *      Returns:
 *               NONE
 *
 *
 */

 
void
slashtodot(name)
char *name;
{
	char *tmpptr;     /* Temporary pointer */

	while(( tmpptr = strpbrk(name,"/")) != NULL )
		*tmpptr = '.';
	return;
}

int
read_partinfo(fd ,partreq, bitmap, lp_map)
FILE		*fd;
PARTREQ_T	*partreq;	/* Partition information */
BITMAP_T	**bitmap;	/* Address of malloced bitmap */
LP_MAP_T	*lp_map;	/* Address of malloced logical/physical map */
{
	int	i,linenum;
	int	bitmaprows, bitmapcols;
	char	buf[MAXBUF];

	for (linenum = 1; linenum <= 14; linenum++) {
		if (fgets(buf, sizeof(buf), fd) == NULL)
			return -1;
		
		switch(linenum) {
 			case 1:
				partreq->uid = atoi(buf);
				break;
			case 2:
				partreq->gid = atoi(buf);
				break;
			case 3:
				partreq->access = strtol(buf, (char**) NULL, 8);
				break;
			case 4:
				partreq->nodes = atoi(buf);
				break;
			case 5:
				partreq->slots = atoi(buf);
				break;
			case 6:
				partreq->rect.rows = atoi(buf);
				break;
			case 7:
				partreq->rect.cols = atoi(buf);
				break;
			case 8:
				partreq->sched = atoi(buf);
				break;
			case 9:
				partreq->rq = atoi(buf);
				break;
			case 10:
				partreq->maxpri = atoi(buf);
				break;
			case 11:
			{
				char *start, *end;

				*lp_map = (LP_MAP_T) MALLOC(
				  (partreq->slots +1) * sizeof(LP_MAP_ENTRY_T));
				if( *lp_map == (LP_MAP_T) 0 ) {
					return(-1);
				}
				start = buf;
				for (i = 0; (end = strpbrk(start, "," )); i++) {
					*end = '\0';
					(*lp_map)[i] = atoi(start);
					start = end + 1;
				}
				(*lp_map)[i] = atoi(start);
				(*lp_map)[++i] = -1;
				break;
			}
			case 12:
				bitmaprows = atoi(buf);
				break;
			case 13:
				bitmapcols = atoi(buf);
				break;
			case 14:
				/*
				 * Allocate space for a bitmap
				 */
			{
				char *start, *end;

				*bitmap =
					allocate_bitmap(bitmapcols,bitmaprows);
				init_bitmap(0, *bitmap); 
				start = buf;
				for (i = 0; (end = strpbrk(start, "," )); i++) {
					*end = '\0';
					sscanf(start,"%u",&((*bitmap)->colmap[i]));
					start = end + 1;
				}
				sscanf(start,"%u",&((*bitmap)->colmap[i]));
				break;
			}
		}
	}
	return(0);
} /* end of read_partinfo */


/********************************  get_part_info ************************
 *
 *      Calling Sequence:
 *             get_part_info(path, partreq, bitmap, lp_map);
 *
 *      Description:
 *             opens the .partinfo file of the partition and sets the 
 *             out parameters
 *
 *      Parameters:
 *		path:		partition name
 *              partreq:	partition information
 *		bitmap:		address of the bitmap
 *		lp_map:		logical/phyical list
 *
 *      Returns:
 *               status
 *
 *
 */


/*
 * get_part_info()
 *
 * Get partition information from .partinfo file.
 */

int
get_part_info(path, partreq, bitmap, lp_map)
char		*path;		/* Pathname of dir corresponding to partition */
PARTREQ_T	*partreq;	/* Partition information */
BITMAP_T	**bitmap;	/* Address of malloced bitmap */
LP_MAP_T	*lp_map;	/* Address of malloced logical/physical map */
{
	int		i;
	char		*p;
	FILE		*fd;
	PATHTYPE	infopath;

	/*
 	* Get inode number of the partition.
 	*/
	if ((partreq->inode = get_inode(path)) <= 0) {
		errno = EPINVALPART;
		return -1;
	} 

	if (init_pathtype(&infopath) == NULL)
		return -1;

	/* 
 	* Construct pathname to the .partinfo file.
 	*/
	if (append_path(&infopath,path) != NULL)
		if (append_path(&infopath,PARTINFO_STR) == NULL);

	if ((fd = fopen(infopath.space, "r")) == NULL) {
		free_path(&infopath);
		return(-1);
	}

	/* process the partition file */
	if (read_partinfo(fd, partreq, bitmap, lp_map) == -1){
		free_path(&infopath);
		return(-1);
	}

	fclose(fd);

	/*
 	* Lop off two right most components to get parent partition pathname.
 	*/
	if ((p = strrpbrk(infopath.space, "/" )) != (char *) 0)
		*p = '\0';
	if ((p = strrpbrk(infopath.space, "/" )) != (char *) 0)
		*p = '\0';
	/*
 	* Get inode number of the parent partition.
 	*/
	if ((partreq->parent_inode = get_inode(infopath.space)) <= 0) {
		errno = EPINVALPART;
		free_path(&infopath);
		return -1;
	} 

	free_path(&infopath);

	return 0;
}


/*
 * build_badbitmap()
 *
*/

BITMAP_T *
build_badbitmap(max_nodes,src)
int		max_nodes;
BITMAP_T	*src;
{
	PARTREQ_T	partreq;	/* Partition information */
	FILE		*fd;
	PATHTYPE	infopath;
	LP_MAP_T	bad_node;
	int		num_bad;
	BITMAP_T	*bitmap;
	int		i;

	bitmap = (BITMAP_T *)0;

	bad_node = (LP_MAP_T) MALLOC(max_nodes * sizeof(LP_MAP_ENTRY_T));
	if (bad_node == (LP_MAP_T) NULL) 
		errno = ENOMEM;
	else{
		num_bad = get_bad_nodes(max_nodes, bad_node);
		if (num_bad > 0){
			bitmap = bitmap_clone(src);
			if (bitmap != (BITMAP_T *)0){
				init_bitmap(0, bitmap); 
				for (i = 0; i < num_bad; i++)
					setbit_bitmap(bitmap, 1, bad_node[i]);
			}
		}
		free(bad_node);
	}
	return(bitmap);
}



/********************************  parse_mod ************************
 *
 *      Calling Sequence:
 *             parse_mod(argument,access);
 *
 *      Description:
 *             converts the unix file system permissions in "rwxrwxrwx"
 *             format to an octal number.
 *
 *      Parameters:
 *              argument:  command line input.
 *              access:    parent's access.
 *
 *      Returns:
 *               status
 *
 *
 */


#define M_READ 04
#define M_WRITE 02
#define M_EXEC 01

int
parse_mod(argument,access)
char *argument;
int  *access;
{
   int i,j;
   int perm ;
   int ugw[3];

#ifdef DEBUG
printf("parse_mod: received argument -%s- \n",argument);
#endif
  if( argument[0] <  '0' || argument[0] > '9' ) {

         for(j = 0; j <= 2; j++) {

                  perm = 000;
                  for(i = 0; i <= 2; i++) {

                          switch( *argument ) {

                                  case 'r':
                                             perm |= M_READ;
                                             break;

                                  case 'w':
                                             perm |= M_WRITE;
                                             break;

                                 case 'x':
                                             perm |= M_EXEC;
                                             break;

                                 case '-':
                                             break;

                                default:
                                             return(-1);
                         }
                         argument++;
                  }

                  ugw[j] = perm;
          }

          if( *(argument) != '\0' ) {
#ifdef DEBUG
printf("parse_mod: error: argument had more than 9 characters \n");
#endif
                   return(-1);
          }
          sprintf(argument,"%o%o%o",ugw[0],ugw[1],ugw[2] );

    }


    for(i = 0; i <=2 ; i++) {
            if( argument[i] > '7' || argument[i] < '0' ) {
#ifdef DEBUG
printf("parse_mod: error: an element was > 7 or < 0 \n");
#endif
                        return(-1);
            }
    }
    *access = strtol(argument,(char**)NULL,8 );

    if( *access > 0777 || *access < 0000 ) {
#ifdef DEBUG
printf("parse_mod: error: access > 777 or < 000 *access = %o\n" ,*access);
#endif
                return(-1);
    }

    return(0);

}

/********************************  is_dir ************************
 *
 *      Calling Sequence:
 *             isdir(path);
 *
 *      Description:
 *             Verifies that "path" exists and is a directory.
 *
 *      Parameters:
 *              path:	pathname of directory
 *
 *      Returns:
 *              1:	If file exists (can be stat()'d) and is a directory.
 *		0:	Otherwise. 
 */
int
isdir(path)
char	*path;	/* Directory pathname */
{
	struct stat	status;

	if (stat(path, &status) == 0) {
		if (S_ISDIR(status.st_mode)) {
			return 1;
		}
	}
	return 0;
}

/***************************** isnumber ***********************
 *
 *      Calling Sequence:
 *             isnumber(descr);
 *
 *      Description:
 *              Verify that each element of the description is
 *		a digit. 
 *
 *      Parameters:
 *              NONE
 *
 *      Returns:
 *		1:      If each element is a digit.
 *		0:	Otherwise
 *
 *
 */


int
isnumber(descr)
char	*descr;		/* input string */
{

	char	*tmpptr;

	tmpptr = descr;
	while( *tmpptr != '\0' ) {

		if( *tmpptr >= '0' && *tmpptr <= '9' ) {
			tmpptr++;
		} else {
			return 0;
		}
	}
	return 1;
}
		

int
check_range( value, lower, upper )
long	value;		/* Value to be checked */
long	lower;		/* Lower boundry */
long	upper;		/* Upper boundry */
{

	if( value >= lower && value <= upper ) {
		return(0);
	}
	return(-1);
}
 
/*
 * The opendir_r and readdir_r function are specified in the OSF programmers
 * referfence, but do not appear to be implemented, so we do it here.
 */
int		
opendir_r(path, dir)
char	*path;		/* Pathname of directory to open */
DIR	*dir;		/* Pointer to directory structure to be filled in */
{
	DIR	*p;	/* Directory pointer */

	p = opendir(path);
	if(p) {
		bcopy((char *) p, (char *) dir, sizeof(DIR));
		return 0;
	}
	else {
		return -1;
	}
}

int
readdir_r(dir, dent) 
DIR		*dir;	/* Directory to read */
struct dirent	*dent;	/* Directory entry to fill in */
{
struct dirent	*p;	/* Pointer to directory entry */

	p = readdir(dir);
	if (p) {
		bcopy((char *) p, (char *) dent, sizeof(struct dirent));
		return 0;
	}
	else {
		return -1;
	}
}

/*
 * get_bad_nodes()
 *
 * Read the bad node file and return a node list and count
 *
 */
get_bad_nodes(max_nodes, lp_map)
int		max_nodes;	/* number of nodes in lp_map */
LP_MAP_T	lp_map;		/* node list */
{
	char	buf[MAXBUF];
	FILE	*fd;
	int	val;
	int	count;

	count = 0;

	if ((fd = fopen("/etc/nx/.badnodes", "r")) == NULL) {
		return 0;
	}

	while ((fgets(buf, sizeof(buf), fd) != NULL) && (count < max_nodes)) {
		if (string_to_num(buf, &val) != -1) {
			lp_map[count++] = val;
		}
	}
	
	return count;
}

/*
 * string_to_num(string, &result)
 *
 * Convert a string to it's numeric value, return -1 on error.
 */
int
string_to_num(string, result)
char	*string;
int	*result;
{
	char	*end;

	*result = strtol(string, &end, 10);
	if (end == string)
		return -1;
	else
		return 0;
}

/* init_pathtype: Initialize the pathtype p
 *
*/
char *
init_pathtype(p)
PATHTYPE        *p;
{
        p->space = (char *) malloc(NAMELEN);
	if (p->space != NULL)
		bzero(p->space,NAMELEN);
        p->maxspace = NAMELEN;
        return(p->space);
}

/* append name onto p->space. If p->maxspace is exceeded then grab a bigger
 * chunk of memory and then perform the append
*/
char *
append_path(p,name)
PATHTYPE        *p;
char            *name;
{
        char *newp;

        if ( strlen(name) + (strlen(p->space)) > p->maxspace){
                newp = (char *) malloc (2 * p->maxspace);
		if (newp != NULL){
			bzero(newp,(2 * p->maxspace));
                	strcat(newp,p->space);
                	p->maxspace *= 2;
                	free(p->space);
                	p->space = newp;
		}
        }
        else{
                strcat(p->space,name);
                newp = p->space;
        }

        return(newp);
}

void
free_path(p)
PATHTYPE	*p;
{
	free(p->space);
}


/***************************** begin_parse_validation ***********************
 *
 *      Calling Sequence:
 *		begin_parse_validation(
 *
 *      Description:
 *              Count the # of NX_ATTR_xxx args.  Allocate a set of flags
 *		so that the args actually used by a call can be checked
 *
 *      Parameters:
 *		va_list    	parse_args	list of NX_ATTR_xxx args
 *
 *	Return:
 *		0		Success
 *		-1		Duplicated NX_ATTR_xxx arg, errno set to EINVAL
 *
 */

static char *args_used = 0;
static int n_args_used;

int
begin_parse_validation(va_list parse_args)
{
	va_list ap;
	int attr;
	long val;
	int min, max;
	char* params;

/*
 * Count the NX_ATTR_xxx args, find min and max
 */

	ap = parse_args;
	n_args_used = 0;
	for (attr = va_arg(ap, int); attr != NX_ATTR_END;
	  attr = va_arg(ap, int)) {
	    val = va_arg(ap, long);
	    if (!n_args_used) {
		min = attr;
		max = attr;
	    } else {
		min = attr < min ? attr : min;
		max = attr > max ? attr : max;
	    }
	    n_args_used++;
	}

/*
 * Check for duplicates
 */

	params = (char*) malloc((max-min)+1);
	bzero(params, (max-min)+1);

	ap = parse_args;
	for (attr = va_arg(ap, int); attr != NX_ATTR_END;
	  attr = va_arg(ap, int)) {
	    val = va_arg(ap, long);
	    if (params[attr-min]) {
		free((void*)params);
		errno = EINVAL;
		return -1;
	    }
	    params[attr-min] = 1;
	}

	free((void*)params);

/*
 * Free previous flags, if any.  Allocate and zero a new set.
 */

	if (args_used)
	    free((void*)args_used);

	args_used = (char*) malloc(n_args_used);
	bzero(args_used, n_args_used);
}


/***************************** check_args_used ***********************
 *
 *      Calling Sequence:
 *             rval = check_args_used();
 *
 *      Description:
 *              Make sure all NX_ATTR_xxx args were used
 *
 *      Parameters:
 *		none
 *
 *      Returns:
 *		0 on success 
 *		-1 on error (errno set to EINVAL)
 *
 *
 */

check_args_used()
{
	int cnt;

	for (cnt=0; cnt<n_args_used; cnt++)
	    if (!args_used[cnt]) {
		errno = EINVAL;
		return -1;
	    }

	free(args_used);
	args_used = (char*)0;
	return 0;
}

    
/***************************** parse_for_applinfo ***********************
 *
 *      Calling Sequence:
 *             parse_for_applinfo(APPLINFO_T* app_info, va_list parse_args);
 *
 *      Description:
 *              Parse list of NX_ATTR_xxx args into an APPLINFO_T structure
 *
 *      Parameters:
 *              APPLINFO_T* app_info	Ptr to structure to be init'ted
 *		va_list    parse_args	list of NX_ATTR_xxx args
 *
 *      Returns:
 *		0 on success (currently, always succeeds)
 *		-1 on error
 *
 *
 */

parse_for_applinfo(app_info, priority, parse_args)
APPLINFO_T* app_info;
int *priority;
va_list parse_args;
{
	va_list ap;
	long temp;
	int attr;
	int cnt;

	ap = parse_args;
	cnt = 0;
	for (attr = va_arg(ap, int); attr != NX_ATTR_END;
	  cnt++, attr = va_arg(ap, int)) {
		switch (attr) {
		case NX_ATTR_PKT:
			app_info->pkt_size = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		case NX_ATTR_MBF:
			app_info->memory_buffer = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		case NX_ATTR_MEX:
			app_info->memory_export = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		case NX_ATTR_MEA:
			app_info->memory_each = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		case NX_ATTR_NOC:
			app_info->noc = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		case NX_ATTR_STH:
			app_info->send_threshold = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		case NX_ATTR_SCT:
			app_info->send_count = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		case NX_ATTR_GTH:
			app_info->give_threshold = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		case NX_ATTR_PLK:
			app_info->process_lock = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		case NX_ATTR_PRI:
			*priority = va_arg(ap, int);
			args_used[cnt] = 1;
			break;
		default:
/*
 * Ignore unknown arguments
 */
			temp = va_arg(ap, long);
			break;
		}
	}
	return 0;
}


/***************************** parse_for_part_info ***********************
 *
 *      Calling Sequence:
 *            parse_for_part_info(nx_part_info_t* part_info, va_list parse_args)
 *
 *      Description:
 *		Initialize a supplied nx_part_info_t structure and then
 *              parse list of NX_ATTR_xxx args into it
 *
 *      Parameters:
 *              nx_part_info_t*	part_info	Ptr to structure to be init'ted
 *		va_list    	parse_args	list of NX_ATTR_xxx args
 *
 *      Returns:
 *		0 on success (currently, always succeeds)
 *		-1 on error
 *
 *
 */

parse_for_part_info(part_info, parse_args)
nx_part_info_t *part_info;
va_list parse_args;
{
	va_list ap;
	long temp;
	int attr;
	int cnt;

	nx_init_partinfo(part_info);

	ap = parse_args;
	cnt = 0;
	for (attr = va_arg(ap, int); attr != NX_ATTR_END;
	  cnt++, attr = va_arg(ap, int)) {
		switch (attr) {
		case NX_ATTR_SCHED:
			part_info->sched = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		case NX_ATTR_EPL:
			part_info->epl = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		case NX_ATTR_RQ:
			part_info->rq = va_arg(ap, long);
			/* 
			 * If they haven't specified a sched type,
			 * set it to gang
			*/
			if (part_info->sched = NO_SCHED_SPECIFIED) {
				part_info->sched = NX_GANG;
			}
			args_used[cnt] = 1;
			break;
		case NX_ATTR_MOD:
			part_info->access = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		default:
/*
 * Ignore unknown arguments
 */
			temp = va_arg(ap, long);
			break;
		}
	}
	return 0;
}


/***************************** parse_for_map ***********************
 *
 *      Calling Sequence:
 *             parse_for_map(nx_nodes_t* node_parse_list,
 *			     int*relaxed, va_list parse_args)
 *
 *      Description:
 *              Parse list of NX_ATTR_xxx args into a a node map
 *
 *      Parameters:
 *              nx_nodes_t*	node_parse_list	ptr to node list to be init'ted
 *		int*		relaxed		RELAXED flag, if present
 *		va_list    	parse_args	list of NX_ATTR_xxx args
 *
 *      Returns:
 *		SIZE on success 
 *		-1 on error
 *
 *
 */

#define NOT_SPECIFIED	-2

parse_for_map(node_parse_list, relaxed, parse_args)
nx_nodes_t* node_parse_list;
int* relaxed;
va_list parse_args;
{
	va_list ap;
	long temp;
	long size = NOT_SPECIFIED;
	long* rect = NULL;
	long* map = NULL;
	long anchor = NOT_SPECIFIED;
	int done;
	int i;
	int attr;
	int cnt;

/*
 * Scan for SIZE, RECT, MAP and ANCHOR
 */
	ap = parse_args;
	cnt = 0;
	done = 0;
	*relaxed = 0;
	for (attr = va_arg(ap, int); attr != NX_ATTR_END;
	  cnt++, attr = va_arg(ap, int)) {
		switch (attr) {
		case NX_ATTR_SZ:
			if (size != NOT_SPECIFIED) {
				errno = EINVAL;
				return -1;	/* size given twice */
			}
			size = va_arg(ap, long);
			if (size < -1) {
				errno = EPBADNODE;
				return -1;	/* invalid size given */
			}
			args_used[cnt] = 1;
			break;
		case NX_ATTR_RECT:
			if (rect) {
				errno = EINVAL;
				return -1;	/* rect given twice */
			}
			rect = va_arg(ap, long*);
			if (!rect || rect[0] <= 0 || rect[1] <= 0) {
				errno = EPBADNODE;
				return -1;	/* invalid rect given */
			}
			args_used[cnt] = 1;
			break;
		case NX_ATTR_MAP:
			if (map) {
				errno = EINVAL;
				return -1;	/* map given twice */
			}
			map = va_arg(ap, long*);
			if (!map) {
				errno = EINVAL;
				return -1;	/* invalid map given */
			}
			args_used[cnt] = 1;
			break;
		case NX_ATTR_ANCHOR:
			if (anchor != NOT_SPECIFIED) {
				errno = EINVAL;
				return -1;	/* anchor given twice */
			}
			anchor = va_arg(ap, long);
			if (anchor == -1) {
			    args_used[cnt] = 1;
			    anchor = NOT_SPECIFIED;
			    break;
			}
			if (anchor < 0) {
				errno = EPBADNODE;
				return -1;	/* invalid anchor given */
			}
			args_used[cnt] = 1;
			break;
		case NX_ATTR_RELAXED:
			*relaxed = va_arg(ap, long);
			args_used[cnt] = 1;
			break;
		default:
			temp = va_arg(ap, long);
			break;
		}
	}
/*
 * Cannot have rect and map
 */
	if (rect && map) {
		errno = EINVAL;
		return -1;
	}
/*
 * A rectangle?
 */
	if (rect) {
		if (size != NOT_SPECIFIED) {
			errno = EINVAL;
			return -1;
		}
		size = 2 + (anchor >= 0);
		*node_parse_list = (nx_nodes_t) MALLOC(
		  size * sizeof(nx_node_elem_t));
		if( *node_parse_list == (nx_nodes_t) 0 ) {
			errno = EINVAL;
			return -1;
		}
		(*node_parse_list)[0] = -rect[0];
		(*node_parse_list)[1] = -rect[1];
		if (size == 3)
			(*node_parse_list)[2] = -anchor;
		return size;
/*
 * A map?
 */
	} else if (map) {
		if (size < 1) {
			errno = EPXRS;
			return -1;
		}
		*node_parse_list = (nx_nodes_t) MALLOC(
		  size * sizeof(nx_node_elem_t));
		if( *node_parse_list == (nx_nodes_t) 0 ) {
			errno = EINVAL;
			return -1;
		}
		for (i=0; i<size; i++) {
			if (map[i] < 0) {
				errno = EPBADNODE;
				return -1;
			}
			(*node_parse_list)[i] = map[i];
		}
		return size;
	}
/*
 * Just a size or nothing (= size 0)
 */
	if (size == -1 || size == NOT_SPECIFIED)
		size = 0;
	*node_parse_list = NULL;
	return size;
}


/************************** NX_analyse_selector_string ***********************
 *
 *	Calling Sequence:
 *		sel_cnt = NX_analyse_selector_string(char* string, int* len);
 *
 *	Description:
 *		Look at a string a determine how many comma-separated
 *		selectors it contains.  Also determine how many bytes
 *		of storage it will require after leading and training
 *		white space has been remove.
 *
 *	Parameters:
 *		char* string	The string to analyse
 *		int* len	# of bytes of storage needed is stored
 *				thru this pointer.
 *
 *	Returns:
 *		Number of selectors found
 *		-1 for error
 *
 */

int
NX_analyse_selector_string(char* string, int* len)
{
    int cnt = 0;
    char* cp;
    char* sp;
    char* end;
    
    if (string == (char*)0) {
	*len = 0;
	return 0;
    }

    if (strlen(string) == 0) {
	/* Node isn't there */
	*len = 0;
	return 0;
    }

    end = string+strlen(string);

    *len = 0;

    while (string < end) {

/* Delete leading white space */

	while (isspace(*string))
	    string++;
	
/* Have we skipped the rest of the string? */

	if (!*string)
	    break;

	cp = strchr(string, ',');
	if (!cp)
	    cp = string + strlen(string);

/* Is it empty? */

	if (cp == string) {
	    errno = EINVAL;
	    return -1;
	}

/* Back up over white space before comma */

	sp = cp-1;
	while (isspace(*sp))
	    sp--;

/* Was is nothing but space? */

	if (cp == string) {
	    errno = EINVAL;
	    return -1;
	}

/* Hey!  We found something! */

	cnt++;
	*len += (sp - string) + 2; 	/* +1 for the null */

	string = cp+1;
    }

    if (cnt == 0) {
	errno = EINVAL;
	return -1;
    }

    return cnt;
}


/***************************** NX_parse_selector_string ***********************
 *
 *	Calling Sequence:
 *		sel_cnt = NX_parse_selector_string(char* string, char** buf);
 *
 *	Description:
 *		Parse the selector string for the comma- separated
 *		selectors it contains.  Remove white space before and
 *		after the commas.  Add each selector to the character
 *		array.
 *
 *	Parameters:
 *		char* string	The string to analyse
 *		char** buf	Buffer to receive the string data.  Updated
 *				by total characters copied.
 *
 *	Returns:
 *		None
 *
 */

NX_parse_selector_string(char* string, char** buf)
{
    char* sp;
    char* cp;
    int cnt;
    char* end;
    char c;
    
    if (string == (char*)0) {
	if (*buf != (char*)0)
	    **buf = '\0';
	return;
    }
	
    end = string+strlen(string);

    while (string < end) {

/* Delete leading white space */

	while (isspace(*string))
	    string++;
	
/* Have we skipped the rest of the string? */

	if (!*string)
	    break;

	cp = strchr(string, ',');
	if (!cp)
	    cp = string + strlen(string);

/* Is it empty? */

	if (cp == string) {
	    string++;
	    continue;
	}

/* Back up over white space before comma */

	sp = cp-1;
	while (isspace(*sp))
	    sp--;

	sp++;

/* Hey!  We found something! */

	c = *sp;
	*sp = '\0';
	strcpy(*buf, string);
	*buf += strlen(string) + 1;
	*sp = c;

	string = cp+1;
    }

    return cnt;
}


/***************************** parse_for_selectors ***********************
 *
 *      Calling Sequence:
 *             sel_cnt =
 *		    parse_for_selectors(char** selector_buf,
 *					int*   selector_buflen,
 *					va_list parse_args)
 *
 *      Description:
 *              Parse list of NX_ATTR_xxx args and return a list of strings.
 *		The number of strings is the return value of the routine.  A
 *		Buffer large enough to hold all the strings, 0-separated, is
 *		allocated and its address and size are returned through
 *		selector_buf and selector_buflen, respectively.
 *
 *		We permit multiple NX_ATTR_SEL arguments, each of which can
 *		contain multiple, comma-separated string.
 *
 *      Parameters:
 *              char**	selector_buf		Address of allocated strings
 *						buffer is stored through this
 *		int*	selector_buflen		# of bytes in string buffer is
 *						stored through this
 *		va_list	parse_args		list of NX_ATTR_xxx args
 *
 *      Returns:
 *		Number of selector strings
 *		-1 for error
 *
 *
 */

int
parse_for_selectors(char** selector_buf, int* selector_buflen,
 va_list parse_args)
{
	va_list ap;
	long temp;
	int scnt = 0;
	int charcnt = 0;
	char* rval;
	char* cp;
	char* bp;
	int attr;
	int cnt;
	int cc;
	int selcnt;

/*
 * Count how many bytes and selectors we have
 */

	ap = parse_args;
	cnt = 0;
	for (attr = va_arg(ap, int); attr != NX_ATTR_END;
	  cnt++, attr = va_arg(ap, int)) {
		switch (attr) {
		case NX_ATTR_SEL:
			cp = va_arg(ap, char*);
			selcnt = NX_analyse_selector_string(cp, &cc);
			if (selcnt == -1)
				return -1;
			scnt += selcnt;
			charcnt += cc;
			args_used[cnt] = 1;
			break;
		default:
			temp = va_arg(ap, long);
			break;
		}
	}

	if (charcnt == 0) {
	    *selector_buf = (char*) 0;
	    *selector_buflen = 0;
	    return 0;
	}

	rval = (char*) MALLOC(charcnt);
	bp = rval;
/*
 * Now parse and copy them
 */
	ap = parse_args;
	for (attr = va_arg(ap, int); attr != NX_ATTR_END;
	  attr = va_arg(ap, int)) {
		switch (attr) {
		case NX_ATTR_SEL:
			cp = va_arg(ap, char*);
			NX_parse_selector_string(cp, &bp);
			break;
		default:
			temp = va_arg(ap, long);
			break;
		}
	}
	*selector_buf = rval;
	*selector_buflen = charcnt;

	return scnt;
}



/***************************** parse_for_attribute ***********************
 *
 *      Calling Sequence:
 *             found =
 *		    parse_for_attribute(int attribute, long* value,
 *				        va_list parse_args)
 *
 *      Description:
 *              Parse list of NX_ATTR_xxx args for a specified attribute
 *
 *      Parameters:
 *		int		attribute	The attribute to look for
 *              long*		value		Ptr to value
 *		va_list    	parse_args	list of NX_ATTR_xxx args
 *
 *      Returns:
 *		1 if attribute found (and value stored through value ptr
 *		0 if attribute not found
 *
 *	If the attribute appears more than once, only the first instance will
 *	be returned.
 *
 *
 */

int
parse_for_attribute(int attribute, long* value,
  va_list parse_args)
{
	va_list ap;
	int attr;
	long val;
	int cnt;

/*
 * Scan ap for attribute
 */

	ap = parse_args;
	cnt = 0;
	for (attr = va_arg(ap, int); attr != NX_ATTR_END;
	  cnt++, attr = va_arg(ap, int)) {
		val = va_arg(ap, long);
		if (attr == attribute) {
			if (value)
				*value = val;
			args_used[cnt] = 1;
			return 1;
		}
	}

	return 0;
}

/***************************** nx_sighold ***********************
 *
 *      Calling Sequence:
 *             nx_sighold( sig );
 *
 *      Description:
 *              Block the signal passed as the parameter. Returns 
 *		a set of signals that were blocked before.
 *
 *      Parameters:
 *              sig: signal no.
 *
 *      Returns:
 *              -1:      Failure
 *               +ve:    Old mask. 
 *
 *
 */

sigset_t
nx_sighold( sig )
int sig;
{

        sigset_t set, oset;

        /*
         *  Build a signal mask for sig.
         */
        if ( sigemptyset( &set ) != 0 ) {
                return(-1);
        }

        if ( sigaddset( &set, sig ) != 0 ) {
                return(-1);
        }

        /*
         *  Block the signals.
         */
        if ( sigprocmask( SIG_BLOCK, &set, &oset ) != 0 ) {
                return(-1);
        }
        return(oset);
}

/***************************** nx_sigrelease ***********************
 *
 *      Calling Sequence:
 *             nx_sigrelease( sig , rset);
 *
 *      Description:
 *		unblock the signal in sig. Don't unblock the signals in 
 *		rset.
 *
 *      Parameters:
 *              sig: signal no.
 *		rset: signal mask to be restored.
 *
 *      Returns:
 *              -1:      Failure
 *               0:      Success  
 *
 *
 */

int
nx_sigrelease( sig , rset)
int sig;
sigset_t rset;
{
        sigset_t set;

	/*
	 * Return without unblocking if the signal is already in the mask
	 * to be restored.
	 */
	if ( sigismember( &rset, sig ))
		return(0);
	

        /*
         * Build a signal mask for sig.
         */
        if( sigemptyset( &set ) != 0 ) {
                return(-1);
        }

        if ( sigaddset( &set, sig ) != 0 ) {
                return(-1);
        }

        /*
         * Unblock the signal.
         */
        if ( sigprocmask( SIG_UNBLOCK, &set, NULL ) != 0 ) {
                return(-1);
        }

}

