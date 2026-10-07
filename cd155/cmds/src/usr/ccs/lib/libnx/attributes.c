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
 *      $Id: attributes.c,v 1.4 1995/03/03 20:15:29 carolr Exp $
 *
 */

/*
 * attributes.c - 
 *
 * HISTORY
 * $Log: attributes.c,v $
 * Revision 1.4  1995/03/03  20:15:29  carolr
 * added 1 to the buffer length on the call to nx_get_matching_bitmap()
 * to include the null terminator.
 *
 * Also updated the 'malloc' calls to 'MALLOC' and 'free' to 'FREE'
 *
 *  Reviewer: sdh
 *  Risk: low
 *  Benefit or PTS #: 12230
 *  Testing: EATS: controlc, rmcall, rmcmd, sched
 * 	      manual testing
 *  Module(s): attributes.c
 *
 * Revision 1.3  1994/11/19  02:29:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/06  17:26:08  mag
 * Correct lspart core dump (9933)
 * Make -nt ,,, an EINVAL (10073)
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 9933, 10073
 *  Testing: developer
 *  Module(s): (9933, 10073): attributes.c,
 * 	    (10073): utils.c, nx_part_ops.c, nx_initve.c,
 * 		     allocator/mkpart_rpc.c, mkpart/mkpart.c
 *
 * Revision 1.1  1994/06/01  20:22:11  mag
 * Initial revision
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <mach/mach_types.h>
#include <mcmsg/mcmsg_appl.h>
#include <malloc.h>
#include <nx.h>
#include <nx/bitmap.h>
#include <nx/defines.h>

#define	ATTR_FILE	"/etc/nx/nodeinfo"

int nx_get_exact_matching_bitmap(BITMAP_T **map,
  unsigned long *mapsize, char* selectors, int exact);


/*****************************************************************************
 * Retrieve the node attribute strings for a partition
 *
 * Arguments:
 *
 *	pszPartName		Name of partition
 *	pppszAttributes		Pointer to location to receive pointer to
 *				array of returned strings
 *
 * Return:
 *	-1			error
 *
 *	>0			# of nodes in partition
 *****************************************************************************/

int
nx_node_attr(char* pszPartName, char*** pppszAttributes)
{
	nx_nodes_t	Log2Phys;
	nx_nodes_t	Phys2Log;
	unsigned long	nSize;
	char**		ppszAttrList;
	FILE*		fd;
	int		nMaxPhys;
	char		szBuff[1024];
	int		nStringLen;
	char**		ppszRetVal;
	char*		pszTemp;
	int		i, j;

	if (nx_get_node_list(pszPartName, &Log2Phys, &nSize) == -1)
		return -1;

/*
 * Build physical-to-logical map
 */
	nMaxPhys = Log2Phys[0];

	for (i=1; i<nSize; i++)
	    if (Log2Phys[i] > nMaxPhys)
		nMaxPhys = Log2Phys[i];

	Phys2Log = (nx_nodes_t) MALLOC((nMaxPhys+1) * sizeof(Log2Phys[0]));

	for (i=0; i<nMaxPhys; i++)
		Phys2Log[i] = -1;

	for (i=0; i<nSize; i++)
		Phys2Log[Log2Phys[i]] = i;

/*
 * Allocate space for a temp array of pointers
 */
	ppszAttrList = (char**) MALLOC(nSize * sizeof(char*));
	bzero(ppszAttrList, nSize*sizeof(char*));

/*
 * Open the attribute file
 */

	fd = fopen(ATTR_FILE, "r");
	if (!fd) {
/*
 * No file.  Just return a list of empty attributes
 */
		*pppszAttributes = ppszAttrList;

		FREE((void*)Log2Phys);
		FREE((void*)Phys2Log);

		return nSize;
	}

	nStringLen = 0;

/*
 * Read in the strings. Make copies of the ones we need.
 */
	for (i=0; i<=nMaxPhys; i++) {
	    if (!fgets(szBuff, sizeof(szBuff), fd))
		break;
	    
	    j = strlen(szBuff);
	    szBuff[j-1] = '\0';

	    j = Phys2Log[i];
	    if (j >= 0)
		ppszAttrList[j] = strdup(szBuff);
	    
	    nStringLen += strlen(szBuff)+1;
	}
/*
 * Now allocate a single block for the array of pointers and the strings
 */
	ppszRetVal = (char**) MALLOC(nSize*sizeof(char*) + nStringLen);
	pszTemp = (char*) (ppszRetVal + nSize);
	
	for (i=0; i<nSize; i++) {
	    ppszRetVal[i] = pszTemp;
	    strcpy(pszTemp, ppszAttrList[i]);
	    pszTemp += strlen(pszTemp)+1;
	    FREE((void*)ppszAttrList[i]);
	}

/*
 * Give back the copy, which is set up so that it can be freed with a
 * single call to free
 */
	*pppszAttributes = ppszRetVal;

/*
 * Free the maps and the array of string pointers.  The string data
 * was freed above in the copy loop.
 */
	FREE((void*)Log2Phys);
	FREE((void*)Phys2Log);
	FREE((void*)ppszAttrList);

	return nSize;
}



/*****************************************************************************
 *      Get the bitmap of nodes matching selector specification
 *
 *      Description:
 *	Return the bitmap of nodes matching selector specification
 *
 *      Parameters:
 *              map             =       pointer to pointer to the bitmap
 *              mapsize         =       size of map
 *		selectors	=	char string containing selectors
 *		exact		=	1 if selectors must match exactly
 *					0 if superset of requested is ok
 *
 *      Returns:
 *              0       =       success
 *              -1      =       error (errno is set)
 *****************************************************************************/

int
nx_get_matching_bitmap(BITMAP_T **map,unsigned long *mapsize, char* selectors)
{
    return nx_get_exact_matching_bitmap(map, mapsize, selectors, 0);
}

int
nx_get_exact_matching_bitmap(BITMAP_T **map,
  unsigned long *mapsize, char* selectors, int exact)
{
	BITMAP_T	*map_buf;

	*mapsize = 0;
	*map =  (BITMAP_T *)0;
	map_buf = (BITMAP_T *)0;
	
	if (nx_matching_bitmap(&map_buf,mapsize,selectors,
	  strlen(selectors)+1, exact) != 0)
		goto error;

	*map = (BITMAP_T *)MALLOC(*mapsize);
	if (*map == (BITMAP_T *) 0){ 
		errno = ENOMEM;
		goto error;
	}
	bcopy(map_buf,*map,*mapsize);
	vm_deallocate(mach_task_self(),(vm_address_t ) map_buf,
		(vm_size_t) *mapsize);
	return(0);
error:
	if (*map != (BITMAP_T *)0)
		FREE( (void *) (*map) );
	return(-1);
}
