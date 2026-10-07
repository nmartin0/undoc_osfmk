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
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_copyargs.c,v 1.2 1994/11/19 02:28:37 mtm Exp $
 */

#include <nx.h>
#include <stdarg.h>

/*
 * copyargs utility for FORTRAN interface
 */

/*
	Function to remove the indirection of a Fortran argument list

	Fortran call looks like this:

call foo(fixed_arg1 [, fixed_arg2]  ... [, NX_ATTR_xxx, val] ... , NX_ATTR_END)

	Thus, we have some # of fixed arguments, followed by some number of
	NX_ATTR_xxx argument pairs, followed by the NX_ATTR_END terminator.

	The C-receiver of this looks like this:

	foo(fixed_args, NX_ATTR_xxx_arg_pairs, NX_ATTR_END,
	  fixed_arg_lengths, NX_ATTR_xxx_arg_lengths)

	Thus, we have, in order:

	1. The fixed arguments.  Fortran is call by reference, so they are
	   all pointers.

	2. The NX_ATTR_xxx argument pairs.  Again, pointers.

	3. A pointer to the NX_ATTR_END terminator.

	4. For each fixed arg that is a CHARACTER string, its length.

	5. For each NX_ATTR_xxx argument value that is a CHARACTER string,
	   its length.

Arguments:
	
	va_list ap		Should point to first NX_ATTR_xxx arg.
	int nFixedStrings	Number of strings as fixed arguments
	long* pnArgs		Pointer to array of location to receive args
	int nArgMax		Number of long's in pnArgs array

Return:

	>0			Number of arguments built into pnArgs array
	-1			Error

*/

static char szBuff[1024];

int
_copyargs(va_list ap, int nFixedStrings, long* pnArgs, int nArgMax)
{
    va_list local_ap;
    va_list length_ap;
    long* pnArg;
    int nArgs;
    int nBytes = 0;
    char* pszName;
    long nStrLen;

/*
 * Find NX_ATTR_END
 */

    for (local_ap = ap; ;) {
	pnArg = va_arg(local_ap, long*);
	if (*pnArg == NX_ATTR_END)
	    break;
	pnArg = va_arg(local_ap, long*);
    }
    
/*
 * Skip lengths for fixed strings
 */

    while (nFixedStrings--)
	nStrLen = va_arg(local_ap, long);

/*
 * Pointing at 1st length
 */

    length_ap = local_ap;

    nArgs = 0;
    for (local_ap = ap; ; ) {

	pnArg = va_arg(local_ap, long*);
	switch (pnArgs[nArgs++] = *pnArg) {

	case NX_ATTR_END:
	    return nArgs;

	case NX_ATTR_PKT:
	case NX_ATTR_MBF:
	case NX_ATTR_MEX:
	case NX_ATTR_MEA:
	case NX_ATTR_NOC:
	case NX_ATTR_STH:
	case NX_ATTR_SCT:
	case NX_ATTR_GTH:
	case NX_ATTR_PLK:
	case NX_ATTR_SCHED:
	case NX_ATTR_EPL:
	case NX_ATTR_RQ:
	case NX_ATTR_MOD:
	case NX_ATTR_SZ:
	case NX_ATTR_ANCHOR:
	case NX_ATTR_RELAXED:
	    pnArgs[nArgs++] = *(va_arg(local_ap, long*));
	    break;

	case NX_ATTR_RECT:
	case NX_ATTR_MAP:
	    pnArg = va_arg(local_ap, long*);
	    pnArgs[nArgs++] = (long) pnArg;
	    break;

	case NX_ATTR_SEL:
	    pszName = va_arg(local_ap, char*);
	    nStrLen = va_arg(length_ap, long);

	    _copyname(pszName, szBuff+nBytes, nStrLen, sizeof(szBuff) - nBytes);
	    pnArgs[nArgs++] = (long) (szBuff+nBytes);

	    nBytes += strlen(szBuff+nBytes) + 1;
	    break;
	default:
	    return -1;
	}

	if ((nArgs+2) > nArgMax)
	    return -1;
    }
    return nArgs;
}



