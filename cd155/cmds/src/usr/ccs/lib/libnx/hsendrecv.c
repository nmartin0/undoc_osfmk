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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/hsendrecv.c,v 1.3 1994/11/19 02:30:20 mtm Exp $
 *
 */

extern long	_hsendrecv();

/*
 * hsendrecv
 *
 * Description: Posts a send and a recv
 *
 * Parameters:
 *	type	Type of message to be sent
 *	buf	Pointer to send buffer
 *	count	Send buffer length
 *	node	Destination node
 *	pid	Destination pid
 *	typesel	Receive type selector
 *	rbuf	Pointer to receive buffer
 *	len	Receive buffer length
 *
 * Returns:	None
 */

hsendrecv(type, buf, count, node, pid, typesel, rbuf, len, proc)

long	type, count, node, pid, typesel, len;
char	*buf, *rbuf;
int	(*proc)();
{
	if (_hsendrecv(type, buf, count, node, pid,
		       typesel, rbuf, len, proc) == -1) {
		nx_perror("hsendrecv");
		exit(1);
	}
}
