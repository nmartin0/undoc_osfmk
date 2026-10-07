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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_hsendrecv.c,v 1.5 1994/11/19 02:28:52 mtm Exp $
 *
 */

#include <sys/types.h>
#include <dbglib.h>

extern long	ipd_call_type;

/*
 * _hsendrecv
 *
 * Description: Post send and a recv with a handler.
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
 * Returns:	0 if correct, -1 on error
 */

long
_hsendrecv(type, buf, count, node, ptype, typesel, rbuf, len, handler)
	long	type;
	char	*buf;
	long	count;
	long	node;
	long	ptype;
	long	typesel;
	char	*rbuf;
	long	len;
	void	(*handler)();
{
	long	mid;
	long	save_ipd_call_type;
	long	retval;

	if ((save_ipd_call_type = ipd_call_type) == IPD_UNKNOWN_TYPE)
		ipd_call_type = IPD_HSENDRECV_TYPE;

	mid = _isend(type, buf, count, node, ptype);

	if (mid != -1) {

		_msgignore(mid);
		retval = _hrecv(typesel, rbuf, len, handler);
		if (retval == -1) {
			return -1;
		}

	} else {
		return -1;
	}
	ipd_call_type = save_ipd_call_type;
	return 0;
}
