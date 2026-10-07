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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/crecv.c,v 1.3 1994/11/19 02:29:15 mtm Exp $
 *
 * Wait until the message has actually been transferred into the message
 * buffer.
 * 
 */

extern long	_crecv();

/*
 * crecv
 *
 * Description: Waits until the message of the requested type has actually
 *		arrived and has been transferred to the message buffer.
 *
 * Parameters:
 *	type	Type of message requested
 *	buf	Pointer to receive buffer
 *	len	Buffer length
 *
 * Returns:	None
 */
crecv(type, buf, len)

long	type, len;
char	*buf;
{
	if (_crecv(type, buf, len) == -1) {
		nx_perror("crecv");
		exit(1);
	}
}
