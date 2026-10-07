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
 *
 * HISTORY:
 * $Log: _esub.c,v $
 * Revision 1.4  1994/11/19  02:19:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/04  20:29:11  brad
 * Modified extended math support so that the routines that actually
 * implement the math are pulled in from the server/emulator source
 * tree, rather than duplicating the same code in the libesize source.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #:
 *  Testing: Ran PFS EATs, ran emath tests.
 *  Module(s): Makefile _eadd.c _eadd1.c _ecmp.c _ediv.c _emod.c _emul.c
 *             _esub.c _esub1.c _etos.c _etoxs.c _stoe.c _xstoe.c
 *
 * Revision 1.2  1994/01/07  02:19:18  brad
 * Merged libesize modifications from R1.2 branch.
 *
 * Revision 1.1.2.1  1994/01/06  18:52:37  brad
 * Checking in results of an extended math code review by Greg Tensa.
 *
 *  Reviewer: Greg Tensa, Brad Rullman.
 *  Risk: Low.
 *  Benefit or PTS #: 7150,5958,5213,5215,5216,1632.
 *  Testing: Ran PFS EATs.
 *  Module(s): libesize/*.c
 *
 * Revision 1.1  1993/09/28  00:51:21  brad
 * Initial revision
 *
 * Revision 1.4  1993/01/21  17:25:21  brad
 * Added some real comments. mul() and div() are now mulx() and divx()
 * to avoid naming clashes with OSF/1 libc functions.
 *
 * Revision 1.3  1992/08/05  08:54:21  dbm
 * *** empty log message ***
 *
 * Revision 1.2  92/08/05  08:50:50  dbm
 * Added History.
 *
 */

#include <errno.h>
#include <sys/types.h>
#include "nx.h"

extern esize_t	__esub();


/*
 * Name:
 *	_esub
 *
 * Description:	
 *	Subtract two extended numbers.
 *
 * Parameters:
 *	e1	Extended number to subtract from.
 *
 *	e2	Extended number to subtract from e1.
 *
 * Returns:
 *	The extended result of the subtraction e1 - e2, or an extended -1 if
 *	an overflow occurred.
 */
esize_t
_esub(e1, e2)
esize_t	e1;
esize_t	e2;
{
	return(__esub(e1, e2, &errno));
}
