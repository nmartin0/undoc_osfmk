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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/reset_boot_node_list/reset_bnl.h,v 1.2 1994/11/21 16:47:39 mtm Exp $
 *
 *
 */

#define	BAD_NODE	-2	/* Used to mark node in node list as bad */

/*
 * reset_boot_node_list() is not in syscall table
 */
#define	reset_boot_node_list(str, nm, bool)	syscall(313, str, nm, bool)
