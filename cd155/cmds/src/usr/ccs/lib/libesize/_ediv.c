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
 * $Log: _ediv.c,v $
 * Revision 1.4  1994/11/19  02:19:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/04  20:29:08  brad
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
 * Revision 1.2  1994/01/07  02:19:15  brad
 * Merged libesize modifications from R1.2 branch.
 *
 * Revision 1.1.2.1  1994/01/06  18:52:33  brad
 * Checking in results of an extended math code review by Greg Tensa.
 *
 *  Reviewer: Greg Tensa, Brad Rullman.
 *  Risk: Low.
 *  Benefit or PTS #: 7150,5958,5213,5215,5216,1632.
 *  Testing: Ran PFS EATs.
 *  Module(s): libesize/*.c
 *
 * Revision 1.1  1993/09/28  00:51:13  brad
 * Initial revision
 *
 * Revision 1.4  1993/04/02  22:41:25  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.3  1993/01/21  17:25:14  brad
 * Added some real comments. mul() and div() are now mulx() and divx()
 * to avoid naming clashes with OSF/1 libc functions.
 *
 * Revision 1.2.2.2  1993/01/21  17:21:01  brad
 * Removed extern declaration of _eadd1() - this is now in nx.h.
 *
 * Revision 1.2.2.1  1993/01/20  21:10:03  brad
 * Added some real comments. mul() and div() are now mulx() and divx()
 * to avoid naming clashes with OSF/1 libc functions.
 *
 * Revision 1.2  1992/08/05  08:43:31  dbm
 * Added History
 *
 */

#include <errno.h>
#include <sys/types.h>
#include "nx.h"

extern long	__ediv();


/*
 * Name:
 *	_ediv
 *
 * Description:	
 *	Divide an extended number by an integer.
 *
 * Parameters:
 *	e	Extended number.
 *
 *	n	Divisor; integer to divide into e.
 *
 * Returns:
 *	The 32-bit quotient of e / d, or a -1 if an overflow occured.
 */
long
_ediv(e, n)
esize_t	e;
long	n;
{
	return(__ediv(e, n, &errno));
}
