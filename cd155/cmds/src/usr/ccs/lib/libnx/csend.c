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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/csend.c,v 1.3 1994/11/19 02:29:18 mtm Exp $
 *
 * Initiates a message send and returns when the message buffer
 * is available for use.
 *
 */

extern long	_csend();

/*
 * csend
 *
 * Description: Causes a message to be sent. Returns when the message
 *		buffer is available for reuse.
 *
 * Parameters:
 *	type	Message type
 *	buf	Pointer to message
 *	len	Length of message
 *	node	Destination node
 *	pid	Destination pid
 *
 * Returns:	None
 */
csend(type, buf, len, node, pid)

long	type, len, node, pid;
char	*buf;
{
	if(_csend(type, buf, len, node, pid) == -1) {
		nx_perror("csend");
		exit(1);
	}
}
