/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log: pcb.h,v $
 * Revision 1.5  1994/11/18  20:31:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:59:18  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:18:30  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:10:05  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:01  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:22:05  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:10  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1991/11/13  12:22:56  rabii
 * 	Initial checkin
 *
 * Revision 2.2  91/09/03  11:12:46  jsb
 * 	First checkin. Derived from i386 sources by Intel SSD.
 * 	[91/09/03  10:19:50  jsb]
 * 
 */

#ifndef _PCB_
#define _PCB_

struct pcb {
	struct pt_entry *pcb_direct;	/* page directory pointer */
	label_t	pcb_context;		/* save context for kernel */
	int	pcb_cmap2;
	int	*pcb_sswap;
	int	pcb_sigc[5];
};

#endif	/* _PCB_ */
