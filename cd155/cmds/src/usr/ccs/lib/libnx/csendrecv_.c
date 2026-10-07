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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/csendrecv_.c,v 1.4 1994/11/19 02:29:20 mtm Exp $
 *
 * FORTRAN interface to csendrecv
 */

long 
csendrecv_(type, buf, count, node, pid, typesel, rbuf, len, buflen, rbuflen) 

long *type, *count, *node, *pid, *typesel, *len;
char *buf, *rbuf;
long buflen, rbuflen;
{

	long msglen;

	if ((msglen = _csendrecv(*type, buf, *count, *node, *pid, *typesel, rbuf, *len)) < 0) {
		nx_perror("csendrecv");
		exit(1);
	}
	return msglen;
}
