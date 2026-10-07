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
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log: debug_portal.h,v $
 * Revision 1.5  1994/11/18  20:31:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:58:30  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:17:50  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:09:13  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:29:50  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:21:43  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:52  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1992/01/09  16:51:13  roy
 * 	Initial revision from Intel.
 *
 */

/*
 * Intel server debug portal interface.
 */

#ifndef	_I860_DEBUG_PORTAL_H_
#define	_I860_DEBUG_PORTAL_H_

#define	DP_syscalltrace		0

#define	DP_prof_start		1

#define	DP_prof_stop		2

#define	DP_prof_clear_data	3

#define	DP_prof_return_data	4

#endif	/* _I860_IDEBUG_PORTAL_H_ */
