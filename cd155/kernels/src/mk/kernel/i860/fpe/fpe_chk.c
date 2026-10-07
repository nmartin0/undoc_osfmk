/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*      Copyright (c) 1989,1990 Intel Corporation.         */
/*      All rights reserved.                               */
/*                                                         */
/*        INTEL CORPORATION PROPRIETARY INFORMATION        */
/*                                                         */
/* This software is supplied under the terms of a license  */
/* agreement or nondisclosure agreement with Intel Corp.   */
/* and may not be copied or disclosed except in accordance */
/* with the terms of that agreement.                       */

/*
 * $Id: fpe_chk.c,v 1.4 1994/11/18 20:40:19 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_chk.c,v $
 * Revision 1.4  1994/11/18  20:40:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:07  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe_chk.c     2.1" 

#include "sys/types.h"
#include <i860/reg.h>
#include <i860/fpe/tss.h>
#include "fpe.h"

/**********************************************************************
 *
 * check_if_to_ignore_SE
 *	Ignore the SE bit for fp traps on fld, pfld, fst, pst
 *	and ixfr instructions when in single instruction mode
 *	or when in dual-instruction mode and the companion
 *	instruction is not a multiplier or adder operation
 *	(It is assumed that SE occured).
 *
 *	input parameters:
 *
 *	none.
 *
 *	output parameter:
 *
 *	SE bit in fsr cleared if SE is to be ignored.
 *
 **********************************************************************/

int check_if_to_ignore_SE(locr0)
unsigned int *locr0;
{
    register unsigned long inst;
    register unsigned long finst;

    if (!(locr0[FSR] & FSR_SE))
	return(0);
    if (locr0[PSR] & 0x4000) /* SE occured in dual-instruction mode */
        inst = *(unsigned long *)(locr0[PC] + 4);
    else  /* SE occured in single-instruction mode */
        inst = *(unsigned long *)locr0[PC];

    switch (inst >> 27) {
    case 1:  /* ixfr */
    case 4:  /* fld  */
    case 5:  /* fst  */
    case 7:  /* pst  */
    case 12: /* pfld */
        if (locr0[PSR] & 0x4000) { /* SE occured in dual-instruction mode */
   	    finst = *(unsigned long *)locr0[PC];
	    if (finst & 0x40)   {   /* i unit instructions */
	        locr0[PSV_FSR3] &= ~0x100; /* clear SE bit in fsr */
	        return(1);
	    }
	}
	else { /* SE occured in single-instruction mode */
	    locr0[PSV_FSR3] &= ~0x100;     /* clear SE bit in fsr */
	    return(1);
        }
    }
    return(0);
}


/***********************************************************************
 *
 * set_trapped_opcode
 *	Set a code in the trapped_opcode field of the u area which
 *	represents for the FPEH the instructio.
 *
 *	input parameters:
 *
 *	inst - the trapped instruction
 *
 *	output parameter:
 *
 *	A code to set the trapped_opcode field of the u area.
 *
 ***********************************************************************/

int set_trapped_opcode(inst)
unsigned long inst;
{
	inst &= 0x37;
	switch (inst)
	{
		case 0x20:
			return FMUL;

		case 0x22:
			return FRCP;

		case 0x23:
			return FRSQR;

		case 0x30:
			return FADD;

		case 0x31:
			return FSUB;

		case 0x32:
			return FIX;

		case 0x33:
			return FAMOV;

		case 0x34:
			return PFGT;

		case 0x35:
			return PFEQ;

		case 0x3a:
			return FTRUNC;

		default:
			if ((inst >= 0) && (inst <= 0xf))
				return PFAM;
			else if ((inst >= 0x10) && (inst <= 0x1f))
				return PFSM;
			else
				return 0;
	}
}
