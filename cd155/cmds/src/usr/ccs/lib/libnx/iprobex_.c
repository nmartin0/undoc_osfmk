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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance
 *	with the terms of that agreement.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/iprobex_.c,v 1.4 1994/11/19 02:30:46 mtm Exp $
 *
 * FORTRAN interface to iprobex
 */

#include <sys/errno.h>

extern int errno;


iprobex_(typesel, nodesel, ptypesel, info, infolen)
long    *typesel;
long    *nodesel;
long    *ptypesel;
long    *info;
long    infolen;
{
	long	retval;
        if ((retval = _iprobex(*typesel, *nodesel, *ptypesel, info)) < 0 ) {
		nx_perror("iprobex");
		exit(1);
	}
	return (retval);
}
