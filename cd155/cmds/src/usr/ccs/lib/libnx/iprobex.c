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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/iprobex.c,v 1.3 1994/11/19 02:30:45 mtm Exp $
 *
 *  Check if a message of the specified type from specified process to arrive.
 * 
 *
 */

extern long	_iprobex();

extern int errno;

/*
 * iprobex
 *
 * Description:	Check if a message of a specified type from specified 
 *              process to arrive.
 *
 * Parameters:	
 *	typesel		Message type 
 *	nodesel    	Node number
 *	ptypesel   	Process type
 *	info    	Info regarding message
 *
 * Returns:	 0: Message has not yet arrived
 *		 1: Message has arrived
 *		-1: Invalid type parameter
 */

long
iprobex(typesel, nodesel, ptypesel, info)
long 	typesel;
long 	nodesel;
long 	ptypesel;
long 	*info;
{
	long	retval;
	if ((retval = _iprobex(typesel, nodesel, ptypesel, info)) == -1) {
		nx_perror("iprobex");
		exit(1);
	}
	return(retval);
}
