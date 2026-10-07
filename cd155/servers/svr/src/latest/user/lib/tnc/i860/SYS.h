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
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)SYS.h	5.4 (Berkeley) 6/27/88
 */
/*
 * HISTORY
 * $Log: SYS.h,v $
 * Revision 1.6  1994/11/18  20:56:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/01/17  21:17:26  slk
 * Fix for #7797, programs that return from main() core dump.
 *
 *  Reviewer: Chris Peak and Charlie Johnson
 *  Risk: Low.
 *  Benefit or PTS #: #7797, allows Locus test suites to complete properly.
 *  Testing: Ran Locus test suites
 *  Module(s): SYS.h
 *
 * Revision 1.4  1993/07/14  18:52:53  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.3  1993/05/06  19:31:44  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  21:20:23  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  23:03:34  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:49:48  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:05:08  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:58:49  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1  1992/04/22  14:39:50  cfj
 * Initial revision
 *
 * $EndLog$
 */

#ifndef	_IPSC680_SYS_H_
#define	_IPSC680_SYS_H_

#include <sys/syscall.h>
#include <i860/asm.h>

#endif	/* _IPSC680_SYS_H_ */
