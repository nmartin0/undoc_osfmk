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
 * HISTORY
 * $Log: _eadd1.c,v $
 * Revision 1.3  1994/11/19  02:19:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/02/04  20:29:04  brad
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
 * Revision 1.1  1993/09/28  00:50:13  brad
 * Initial revision
 *
 * Revision 1.3  1993/01/21  17:25:09  brad
 * Added some real comments. mul() and div() are now mulx() and divx()
 * to avoid naming clashes with OSF/1 libc functions.
 *
 * Revision 1.2  1992/08/05  08:41:11  dbm
 * Added History.
 *
 */

#include <errno.h>
#include <sys/types.h>
#include "nx.h"

extern esize_t	__eadd1();


/*
 * Name:
 *	_eadd1
 *
 * Description:	
 *	Add an integer to an extended number, producing an extended result.
 *
 * Parameters:
 *	e	Extended number.
 *
 *	n	Integer (long).
 *
 * Returns:
 *	The extended result of the addition e + n, or an extended -1 if an
 *	overflow occurred.
 */
esize_t
_eadd1(e, n)
esize_t	e;
long	n;
{
	return(__eadd1(e, n, &errno));
}
