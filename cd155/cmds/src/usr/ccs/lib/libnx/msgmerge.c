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
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/msgmerge.c,v 1.3 1994/11/19 02:31:22 mtm Exp $
 *
 * Merge two mids into one.
 * 
 *
 */

extern long	_msgmerge();

/*
 * msgmerge
 *
 * Description:	Merge the second mid with the first one, and return a mid
 * 		to be used for both.
 *
 * Parameters:
 *	mid1	First message request identifier
 *	mid2	Second message request identifier
 *
 * Returns:	
 *	mid	A new mid
 */

long
msgmerge(mid1, mid2)
long	mid1;
long	mid2;
{
	long	retval;

	if ((retval = _msgmerge(mid1, mid2) ) == -1) {
		nx_perror("msgmerge");
		exit(1);
	}
	return(retval);
}
