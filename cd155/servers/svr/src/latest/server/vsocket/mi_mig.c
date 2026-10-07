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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * Taken from tnc_mig.c bhk 2/14/92
 *
 * HISTORY
 * $Log: mi_mig.c,v $
 * Revision 1.6  1995/02/01  23:15:39  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:51:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/09/01  01:39:57  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 3.5  93/08/31  09:28:27  mjl
 * [LCC #0386] <vsocket/vs_types.h> now requires <sys/param.h>.
 * 
 * Revision 3.4  93/08/26  17:02:16  mjl
 * Add RCS $Log keyword.
 * 
 */
/*
 * Wrapper to compile bsd servers without KERNEL defined.
 */
#include <sys/param.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <vsocket/vs_types.h>
#include <vsocket/mi_types.h>
#include <vsocket/vs_chouse.h>

#undef	KERNEL
#include <vsocket/mi_server.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <vsocket/mi_user.c>
