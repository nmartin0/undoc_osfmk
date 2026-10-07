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
 *      $Id: setptype.c,v 1.5 1994/11/19 02:32:37 mtm Exp $
 *
 * Set process type for a process.
 * 
 * HISTORY
 * $Log: setptype.c,v $
 * Revision 1.5  1994/11/19  02:32:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1992/07/14  15:28:24  shala
 * Use nx_perror call
 *
 * Revision 1.3  92/06/09  21:24:41  stans
 * added ID and ident strings
 * 
 */
char _setptype_c_id[]="$Id: setptype.c,v 1.5 1994/11/19 02:32:37 mtm Exp $";

extern long	_setptype();

/*
 * setptype 
 *
 * Description:	Set process type for a process.
 *
 * Parameters: 	
 *	ptype	process type
 *
 * Returns: 	None
 */

setptype(ptype)
long 	ptype;
{

	if (_setptype(ptype) == -1) {
		nx_perror("setptype");
		exit(1);
	}
}
