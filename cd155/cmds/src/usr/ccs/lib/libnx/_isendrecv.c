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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_isendrecv.c,v 1.5 1994/11/19 02:28:52 mtm Exp $
 *
 */

#include <sys/types.h>
#include <dbglib.h>
#include "nx.h"
extern long	ipd_call_type;

/*
 * _isendrecv
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

long
_isendrecv(type, buf, count, node, pid, typesel, rbuf, len)
	long	type;
	char	*buf;
	long	count;
	long	node;
	long	pid;
	long	typesel;
	char	*rbuf;
	long	len;
{
	long	mid1;
	long	mid2;
	long	save_ipd_call_type;

	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE)
		ipd_call_type = IPD_ISENDRECV_TYPE;

	mid1 = _irecvx(typesel, rbuf, len, -1, -1, msginfo);
	if (mid1 == -1) {
		ipd_call_type = save_ipd_call_type;
		return -1;
	}
	mid2 = _isend(type, buf, count, node, pid);
	if (mid2 == -1) {
		_msgcancel(mid1);
		ipd_call_type = save_ipd_call_type;
		return -1;
	}
	ipd_call_type = save_ipd_call_type;
	return _msgmerge(mid1, mid2);
}
