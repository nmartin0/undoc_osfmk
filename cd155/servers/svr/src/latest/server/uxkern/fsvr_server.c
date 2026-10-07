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
 * This file was created by the Center for High Performance Computing of
 * Worcester Polytechnic Institute (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: fsvr_server.c,v $
 * Revision 1.5  1994/11/18  20:48:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:42:01  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:02:30  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:29:03  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:54:38  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:43:23  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:52:55  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/06/08  18:30:18  pjg
 * 	Include fast_path_io files. These files are empty unless FAST_PATH_IO
 * 	is defined (pjg).
 * 
 * Revision 2.2  92/01/05  20:18:21  roy
 * 	1991/10/28  16:53:56  noemi
 * 	Initial revision
 * 
 */
/*
 * Wrapper to compile bsd servers without KERNEL defined.
 */
#include <sys/time.h>

#undef	KERNEL
#include <uxkern/fsvr_server.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <uxkern/fsvr_user.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <uxkern/fsvr2_server.c>

