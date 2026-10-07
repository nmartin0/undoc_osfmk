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
 * $Log: _ecmp.c,v $
 * Revision 1.3  1994/11/19  02:19:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/02/04  20:29:06  brad
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
 * Revision 1.1  1993/09/28  00:50:25  brad
 * Initial revision
 *
 * Revision 1.3  1993/01/21  17:25:11  brad
 * Added some real comments. mul() and div() are now mulx() and divx()
 * to avoid naming clashes with OSF/1 libc functions.
 *
 * Revision 1.2  1992/08/05  08:42:25  dbm
 * Added History.
 *
 */

#include <errno.h>
#include <sys/types.h>
#include "nx.h"

extern long	__ecmp();


/*
 * Name:
 *	_ecmp
 *
 * Description:	
 *	Compare two extended numbers.
 *
 * Parameters:
 *	e1	First extended number.
 *
 *	e2	Second extended number.
 *
 * Returns:
 *	The result of the compare:
 *
 *		e1 < e2:	returns -1
 *		e1 = e2:	returns  0
 *		e1 > e2:	returns  1
 */
long _ecmp(e1, e2)
esize_t e1, e2;
{
	return(__ecmp(e1, e2));
}
