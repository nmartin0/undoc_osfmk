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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/irecvx.c,v 1.3 1994/11/19 02:30:51 mtm Exp $
 *
 * Initiate message receive from specified process.
 *
 * 
 */

extern long	_irecvx();

/*
 * irecvx
 *
 * Description: Sets up to receive a message from specified process.
 *
 * Parameters:
 *	typesel		Type of message requested
 *	buf		Pointer to receive buffer
 *	count		Buffer length
 *      nodesel         Node number
 *      ptypesel        Process type
 *      info            Info regarding message
 *
 * Returns:	Positive message identifier
 */

long
irecvx(typesel, buf, count, nodesel, ptypesel, info)
long	typesel;
char	*buf;
long	count;
long	nodesel;
long	ptypesel;
long	*info;
{
	long	retval;
	if ((retval = _irecvx(typesel, buf, count, nodesel, ptypesel, info)) == 
			-1) {
		nx_perror("irecvx");
		exit(1);
	}
	return(retval);
}
