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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_gsync.c,v 1.4 1994/11/19 02:28:51 mtm Exp $
 *
 */

/* Copyright info:
 *
 *    This code was developed at Battelle / U.S. Department of Energy (DOE)
 *    Pacific Northwest Laboratory (PNL) in Richland, Washington, under U.S.
 *    government funding.  Neither Battelle, PNL, DOE, the U.S. government,
 *    nor any employee of those organizations makes any claim about the
 *    correctness or suitability of this code for any purpose whatsoever,
 *    nor assumes any responsibility for damages resulting from its use.
 *
 *    This code is transferred to Intel for possible modification and
 *    incorporation into the operating system on the Concurrent
 *    Supercomputing Consortium Touchstone DELTA computer at Caltech.
 *    Battelle/PNL retains all other rights regarding its use.  The code
 *    cannot be further redistributed without written permission from
 *    Battelle/PNL.
 *
 */

/*
 *	_gsync.c
 *
 * Global synchronization operation.
 *
 * Written by Rik Littlefield of PNL (Pacific Northwest Labs)
 *
 * Communication utilities use their reserved message types.
 * They range from 1000000001 to 1000000007.  They are the same for
 * all participating processes.  There must be no other messages of
 * these types in the system.  All participating processes must have
 * the same PID.
 * 
 * input..
 * 
 *   none.
 * 
 * output..
 * 
 *   none.
 * 
 * error conditions
 * 
 *	 none.
 * 
 * calls:  mynode, myptype, numnodes, _crecv, _csend
 *
 */
#include <errno.h>
#include <nx/gops.h>

/* Binary tree global sync.  Causes no more than 2 messages to be */
/* queued at any node.  Rik Littlefield, 12/3/91                  */

long
_gsync()
{
        int me, num, child1, child2, parent, pid;

        me = mynode();
        num = numnodes();
	pid = myptype();

	parent = (me-1)/2;
	child1 = 2*me + 1;
	child2 = child1 + 1;

	if (child1 < num) _crecv (COMM_TYPE_1,0,0);
	if (child2 < num) _crecv (COMM_TYPE_1,0,0);
	if (me > 0) {
		_csend (COMM_TYPE_1,0,0,parent,pid);
		_crecv (COMM_TYPE_2,0,0);
	}
	if (child1 < num) _csend (COMM_TYPE_2,0,0,child1,pid);
	if (child2 < num) _csend (COMM_TYPE_2,0,0,child2,pid);
	return 0;
}
