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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/hsend.c,v 1.3 1994/11/19 02:30:18 mtm Exp $
 *
 */

extern long	_hsend();

/*
 * hsend
 *
 * Description: Posts a send from the buffer and sets up a completion handler.
 *
 * Parameters:
 *	type	Type of message requested
 *	buf	Pointer to receive buffer
 *	count	Buffer length
 *	node	Destination node
 *	pid	Destination pid
 *	proc	Handler procedure
 *
 * Returns:	None
 */
hsend(type, buf, count, node, pid, proc)

long	type, count, node, pid;
char	*buf;
int	(*proc)();
{
	if (_hsend(type, buf, count, node, pid, proc) == -1) {
		nx_perror("hsend");
		exit(1);
	}
}
