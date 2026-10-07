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
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: load_level_mig.c,v $
 * Revision 1.7  1995/02/01  23:46:50  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/19  03:07:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/11/18  20:53:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:50:49  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.3  1993/05/06  19:28:38  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  21:16:58  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  23:01:38  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:49:41  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:01:45  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:57:34  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.0  1992/03/27  16:30:48  mbarnett
 * initial check-in
 *
 */

#undef KERNEL
#include "load_level_server.c"
#undef msgh_request_port
#undef msgh_reply_port
#include "load_level_user.c"
