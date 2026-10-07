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
 *      $Id: setptype_.c,v 1.4 1994/11/19 02:32:40 mtm Exp $
 *
 * FORTRAN interface to setptype
 * 
 * HISTORY
 * $Log: setptype_.c,v $
 * Revision 1.4  1994/11/19  02:32:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/07/14  13:44:41  shala
 * Use nx_perror call
 *
 * Revision 1.2  92/06/09  21:24:50  stans
 * added ID and ident strings
 * 
 */
char _setptype__c_id[]="$Id: setptype_.c,v 1.4 1994/11/19 02:32:40 mtm Exp $";


setptype_(ptype)
long	*ptype;
{
	if (_setptype(*ptype) < 0) {
		nx_perror("setptype");
		exit(1);
	}
}
