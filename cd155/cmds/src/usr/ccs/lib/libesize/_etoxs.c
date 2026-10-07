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
 *      Copyright 1993  Intel Corporation.
 *
 *
 * HISTORY:
 * $Log: _etoxs.c,v $
 * Revision 1.4  1994/11/19  02:19:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/04  20:29:16  brad
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
 * Revision 1.2  1994/01/07  02:19:20  brad
 * Merged libesize modifications from R1.2 branch.
 *
 * Revision 1.1.2.1  1994/01/06  18:52:39  brad
 * Checking in results of an extended math code review by Greg Tensa.
 *
 *  Reviewer: Greg Tensa, Brad Rullman.
 *  Risk: Low.
 *  Benefit or PTS #: 7150,5958,5213,5215,5216,1632.
 *  Testing: Ran PFS EATs.
 *  Module(s): libesize/*.c
 *
 * Revision 1.1  1993/11/13  06:27:13  brad
 * Initial revision
 *
 */

#include <errno.h>
#include <sys/types.h>
#include "nx.h"

extern long	__etoxs();


/*
 * Name:
 *	_etoxs
 *
 * Description:	
 *	Convert an extended number to a string containing an ASCII hexadecimal
 *	representation of the number.
 *
 * Parameters:
 *	e	Extended number.
 *
 *	s	Pointer to a character string (at least 20 characters in 
 *		length) which is filled by this function.
 *
 * Returns:
 *	0 if successful, else -1 if an overflow occurred.
 */
long
_etoxs(e, s)
esize_t	e;
char	*s;
{
	return(__etoxs(e, s, &errno));
}
