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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/isendrecv.c,v 1.3 1994/11/19 02:30:54 mtm Exp $
 *
 */

extern long	_isendrecv();

/*
 * isendrecv
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
 * Returns:	Positive message identifier
 */

long
isendrecv(type, buf, count, node, pid, typesel, rbuf, len)

long	type, count, node, pid, typesel, len;
char	*buf, *rbuf;
{
	long	retval;

	retval = _isendrecv(type, buf, count, node, pid, typesel, rbuf, len);
	if (retval < 0) {
		nx_perror("isendrecv");
		exit(1);
	}
	return(retval);
}
