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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_killcube.c,v 1.4 1994/11/19 02:28:53 mtm Exp $
 *
 * Under score version of killcube
 * 
 */

/***************************  _killcube  ******************************
 *
 *      Description:
 *            User interface to kill process(es).
 *
 *      Calling Sequence:
 *            _killcube(node, pid);
 *
 *      Parameters:
 *            node:     Node number
 *            pid:      PID
 *
 *      Returns:
 *            -1 if error
 *
 *      Called by:
 *            User
 */

#include <errno.h>
#include <sys/signal.h>

_killcube(node, pid)
long node, pid;
{
	if (node == -1 && pid == -1) {
		if (_killproc(node, pid) < 0)
			return (-1);
		else
			return (0);
	} else {
		errno = EINVAL;
		return (-1);
	}
}
