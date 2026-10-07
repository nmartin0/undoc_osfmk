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
 * HISTORY
 * $Log: nx_svr_server.c,v $
 * Revision 1.3  1994/11/18  20:37:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  22:33:31  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:31:40  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:25:55  cfj
 * Bump major revision number.
 *
 * Revision 1.2  1992/07/28  09:50:12  nandy
 * NX integration
 *
 * Revision 1.1  92/05/08  13:32:35  cfj
 * Initial revision
 * 
 * 
 */
/*
 * Wrapper to compile bsd servers without KERNEL defined.
 */
#include <sys/time.h>

#undef	KERNEL
#include <nx/nx_svr_server.c>
