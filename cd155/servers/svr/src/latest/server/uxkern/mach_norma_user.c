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
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * This source file was created by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: mach_norma_user.c,v $
 * Revision 1.5  1994/11/18  20:48:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:42:35  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:03:36  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:30:28  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:55:03  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:43:46  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:53:39  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:52:55  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1993/01/06  10:37:38  loverso
 * 	Fix notice.
 *
 * Revision 2.2  92/01/05  20:17:27  roy
 * 	1991/12/13  00:56:58  noemi
 * 	Initial revision
 * 
 */
/*
 * Wrapper to compile bsd servers without KERNEL defined.
 */
#include <sys/time.h>

#undef	KERNEL
#undef	msgh_request_port
#undef	msgh_reply_port
#include <uxkern/mach_norma_user.c>

