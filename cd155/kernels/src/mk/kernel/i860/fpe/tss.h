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
 
/*	Copyright (c) 1990, 1991 UNIX System Laboratories, Inc.	*/
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989, 1990 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF     	*/
/*	UNIX System Laboratories, Inc.                     	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*      Copyright (c) 1989, Intel Corporation.             */
/*      All rights reserved.                               */
/*                                                         */
/*        INTEL CORPORATION PROPRIETARY INFORMATION        */
/*                                                         */
/* This software is supplied under the terms of a license  */
/* agreement or nondisclosure agreement with Intel Corp.   */
/* and may not be copied or disclosed except in accordance */
/* with the terms of that agreement.                       */

/*
 * $Id: tss.h,v 1.4 1994/11/18 20:40:32 mtm Exp $
 *
 * HISTORY
 * $Log: tss.h,v $
 * Revision 1.4  1994/11/18  20:40:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:33  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */



#ifndef _SYS_TSS_H
#define _SYS_TSS_H


#ident	"@(#)head.sys:sys/tss.h	1.1.2.1"

/*
 * i860 processor status register
 */

#define PSR_BR		0x00000001	/* break read */
#define PSR_BW		0x00000002	/* break write */
#define PSR_CC		0x00000004	/* condition code */
#define PSR_LCC		0x00000008	/* loop condition code */

#define PSR_IM		0x00000010	/* interrupt mode */
#define PSR_PIM		0x00000020	/* prev. interrupt mode */
#define PSR_U		0x00000040	/* user mode */
#define PSR_PU		0x00000080	/* prev. user mode */

#define PSR_IT		0x00000100	/* instruction trap */
#define PSR_IN		0x00000200	/* interrupt */
#define PSR_IAT		0x00000400	/* instruction access trap */
#define PSR_DAT		0x00000800	/* data access trap */
#define PSR_FT		0x00001000	/* floating-point trap */

#define PSR_DS		0x00002000	/* delayed switch */
#define PSR_DIM		0x00004000	/* dual instruction mode */
#define PSR_KNF		0x00008000	/* kill next float instr. */

#define PSR_RSVD	0x00010000	/* reserved */

#define PSR_SC_MASK	0x003e0000	/* shift count */
#define PSR_PS_MASK	0x00c00000	/* pixel size */
#define PSR_PM_MASK	0xff000000	/* pixel mask */

#define PSR_USERSET	(PSR_U)		/* bits to set for ptrace */
#define PSR_USERCLR	(PSR_IM|PSR_PIM) /* bits to clear for ptrace */

/*
 *  i860 Extended Processor Status Register
 */

#define EPSR_PTYPE_MASK	0x000000ff	/* processor type mask in epsr */
#define EPSR_STEP_MASK	0x00001f00	/* processor stepping mask in epsr */
#define EPSR_STEP_SHIFT	8
#define EPSR_IL		0x00002000	/* interlock */
#define EPSR_WP		0x00004000	/* write protect */
#define EPSR_PEF	0x00008000	/* parity error flag */
#define EPSR_BEF	0x00010000	/* bus error flag */
#define EPSR_INT	0x00020000	/* interrupt */
#define EPSR_DCS_MASK	0x003C0000	/* data cache size mask */
#define EPSR_PBM	0x00400000	/* page table bit mode */
#define EPSR_BE		0x00800000	/* big endian mode */
#define EPSR_OF		0x01000000	/* overflow flag */
#define EPSR_BS		0x02000000	/* BEF or PEF at supervisor level */
#define EPSR_DI		0x04000000	/* trap on delayed instruction */
#define EPSR_AI		0x08000000	/* trap on auto increment */
#define EPSR_PT		0x10000000	/* trap on pipeline use */
#define EPSR_PI		0x20000000	/* trap on pipeline instruction */
#define EPSR_SO		0x40000000	/* strong ordering mode */ 

#define P_i860XR	1		/* i860XR type value */
#define P_XR_B2		3		/* i860XR B2 stepping value */
#define P_XR_C0		4		/* i860XR C0 stepping value */
#define P_XR_B3		5		/* i860XR B3 stepping value */
#define P_XR_C1		6		/* i860XR C1 stepping value */	
#define P_XR_D0		7		/* i860XR D0 stepping value */
#define P_i860XP	2		/* i860XP type value */
#define P_XP_A0		0		/* i860XP A0 stepping value */

#ifndef	ASSEMBLY
typedef	int		flags_t;
#endif	/* ASSEMBLY */

/*
 *	i860 directory base register
 */

#define DIR_ATE		0x00000001	/* address translation enable */
#define DIR_BL		0x00000010	/* bus lock */
#define DIR_ITI		0x00000020	/* tlb & i-cache invalidate  */
#define DIR_LB		0x00000040	/* late back-off mode */
#define DIR_CS8		0x00000080	/* code size 8 bit */
#define DIR_RB_MASK	0x00000300
#define DIR_RC_MASK	0x00000C00

/*
 * 	i860 dirbase RC fields 
 */
#define DIR_RC_00	0x00000000	 
#define DIR_RC_01 	0x00000400
#define DIR_RC_10  	0x00000800
#define DIR_RC_11	0x00000C00

/*
 *	i860 dirbase RB fields
 */
#define DIR_RB_00	0x00000000
#define DIR_RB_01	0x00000100	
#define DIR_RB_10	0x00000200
#define DIR_RB_11	0x00000300

/* 
 * FSR register bit fields 
 */
#define FSR_FZ		0x00000001
#define	FSR_TI		0x00000002
#define FSR_RM		0x0000000c
#define	FSR_U		0x00000010
#define	FSR_FTE		0x00000020
#define	FSR_SI		0x00000080
#define	FSR_SE		0x00000100
#define	FSR_MU		0x00000200
#define	FSR_MO		0x00000400
#define	FSR_MI		0x00000800
#define	FSR_MA		0x00001000
#define	FSR_AU		0x00002000
#define	FSR_AO		0x00004000
#define	FSR_AI		0x00008000
#define	FSR_AA		0x00010000
#define	FSR_RR		0x003e0000
#define	FSR_AE		0x01c00000
#define FSR_LRP0	0x02000000
#define	FSR_LRP1	0x04000000
#define	FSR_IRP		0x08000000
#define	FSR_MRP		0x10000000
#define	FSR_ARP		0x20000000

#define	FSR_MBITS	(FSR_MU | FSR_MO | FSR_MI | FSR_MA | FSR_SI | FSR_MRP)
#define	FSR_ABITS	(FSR_AU | FSR_AO | FSR_AI | FSR_AA | FSR_SI | FSR_ARP \
			| FSR_AE)
#define FSR_RBITS	(FSR_AI|FSR_AO|FSR_AU|FSR_MI|FSR_MO|FSR_MU)
#define	FSR_OTHRBITS	(FSR_IRP|FSR_LRP0|FSR_LRP1|FSR_FTE|FSR_RM|FSR_TI|FSR_FZ)

#define FSR_RM_SHIFT    2
#define FSR_RR_SHIFT    17
#define FSR_AE_SHIFT    22
#define FSR_RBITS_SHIFT 9

/* 
 * bits fields of RM bits of FSR
 */
#define RM_TO_NEAREST	0
#define RM_ROUND_DOWN	1
#define RM_ROUND_UP	2
#define RM_CHOP		3

#endif	/* _SYS_TSS_H */
