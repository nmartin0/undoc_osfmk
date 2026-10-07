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
 *      Copyright 1993  Intel Corporation.
 *
 *      $Id: mypart_.c,v 1.3 1994/11/19 02:31:29 mtm Exp $
 *
 */

/* History: 
 *	$Log: mypart_.c,v $
 * Revision 1.3  1994/11/19  02:31:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/01/07  20:39:33  carbajal
 *   7578 PARAGON      OPEN      L        carbajal  davidl              R1.2 WW50
 *       MESH UTILS   17-DEC-93 **       17-DEC-93 17-DEC-93
 *       New R1.2 user model call names are not descriptive.
 *
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7578
 *  Testing:
 *  Module(s): nx.h, fnx.h, allocsys.h, allocsys.c, allocsys_.c
 *         mypart.c, mypart_.c
 *
 * Revision 1.1.2.1  1994/01/06  23:36:10  carbajal
 *   7578 PARAGON      OPEN      L        carbajal  davidl              R1.2 WW50
 *       MESH UTILS   17-DEC-93 **       17-DEC-93 17-DEC-93
 *       New R1.2 user model call names are not descriptive.
 *
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7578
 *  Testing:
 *  Module(s): nx.h, fnx.h, allocsys.h, allocsys.c, allocsys_.c
 *         mypart.c, mypart_.c
 *
 * Revision 1.1  1993/11/17  02:41:45  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
*/

/*	MYPART()	FORTRAN INTERFACE
 *
 *	Description:
 *		returns the rectangular dimensions of the node set
 *		that the calling application is running on.
 *
 *	Parameters:
 *		rows	pointer to long, will be set to number of rows in
 *			rectangle. If the node set is not rectangular then
 *			this will be set to 1.
 *		cols	pointer to long, will be set to number of columns
 *			if the node set is rectangular. If not then it
 *			is set to numnodes().
 *
 *	Returns:
 *		0	call was successful
 *		-1	otherwise, errno will be set.
 *
*/
int
mypart_(long *rows, long *cols)
{
	return(nx_mypart(rows,cols));
}

int
nx_app_rect_(long *rows, long *cols)
{
	return(nx_mypart(rows,cols));
}


