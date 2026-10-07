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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/hrecvx.c,v 1.2 1994/11/19 02:30:17 mtm Exp $
 *
 */

extern long	_hrecvx();
/*
 * hrecvx
 *
 * Description: Posts a receive into the buffer and sets up a completion
 *		handler. (Extended version)
 *
 * Parameters:
 *	type		Type of message requested
 *	buf			Pointer to receive buffer
 *	len			Buffer length
 *	nodesel		Node selector
 *	ptypesel	Ptype selector
 *	handler		completion handler address
 *	hparam		completion parameter
 *	
 *
 * Returns:	None
 */
hrecvx(type, buf, len, nodesel, ptypesel, proc, hparam)

long	type, len, nodesel, ptypesel, hparam;
char	*buf;
int	(*proc)();
{
	if (_hrecvx(type, buf, len, nodesel, ptypesel, proc, hparam) == -1) {
		nx_perror("hrecvx");
		exit(1);
	}
}
