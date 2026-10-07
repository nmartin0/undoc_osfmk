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
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/gsendx_.c,v 1.3 1994/11/19 02:30:03 mtm Exp $
 *
 * This module contains the application interface to the gsendx_ for FORTRAN.
 * 
 */



extern long	_gsendx();


/***********************  gsendx_  ******************************
 *
 *    This subroutine sends a vector to a set of nodes specified by
 *    a node list.  All the nodes which are to receive the vector must
 *    call crecv or irecv.
 * 
 *    All participating processes must have the same PID.
 * 
 *    input..
 * 
 *      type      the message type.
 *                Must be the same for all participating
 *                processes.  there must be no other messages of this type
 *                in the system.
 *      x         the input vector to be sent.
 *      len       the number of bytes of data in vector.
 *      nodenums  the list of node numbers data to be sent to.
 *      nlen      the number of node numbers in list.
 * 
 *    output..
 * 
 *       none.
 * 
 *    Errors Conditions
 *
 *       If a node number is a negative value, an error message is sent
 *       to stderr and the process is terminated.
 *
 * calls:  _gsendx, nx_perror, exit
 *
 */

gsendx_(type, x, len, nodenums, nlen)
	long *type;
	long x[];
	long *len;
	long nodenums[];
	long *nlen;
{
	if (_gsendx(*type, x, *len, nodenums, *nlen)) {
		nx_perror("gsendx");
		exit(1);
	}
}
