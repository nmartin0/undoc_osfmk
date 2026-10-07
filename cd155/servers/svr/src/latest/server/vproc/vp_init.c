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
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: vp_init.c,v $
 * Revision 1.8  1994/11/18  20:51:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/07/14  18:47:40  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:11:18  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:11:38  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.5  1993/04/03  03:13:26  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:09:24  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/22  18:17:45  cfj
 * 01-20-93 Locus code drop.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:06:24  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.1.1  1993/05/03  17:55:35  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.9  1993/04/22  16:43:35  roman
 * Fix comment for accuracy.
 *
 * Revision 2.8  92/12/29  13:42:15  chrisp
 * vproc_init() now does nothing save initialize the vproc table pointers
 * 	statically (to NULL).
 * 
 * Revision 2.7  92/12/02  11:14:54  chrisp
 * The vproc hash table becomes dynamically allocated.
 * 
 * Revision 2.6  92/09/29  08:35:29  roman
 * Initialize lock using new locking macro.
 * 
 * Revision 2.5  92/03/12  15:52:57  roman
 * Initialization of the (per vproc) vproc reference counting mutex added.
 * 
 * Revision 2.4  91/10/30  12:56:41  roman
 * Change the name of the indirect routine called by LOCATE_VPROC_PID() macro.
 * 
 * Revision 2.3  91/10/25  10:50:33  roman
 * Added initialization of  routine used by LOCATE_VPROC_PID() macro.
 * 
 * Revision 2.2  91/09/16  16:25:45  rabii
 * 	Initial Checkin (locus check-in by roman)
 * 
 *
 */
#include <sys/vproc.h>
#include <kern/zalloc.h>

/*
 * Actual declarations of vproc table and vproc hash table.
 */
struct vproc *vproc = NULL, *vprocNVPROC = NULL, *vprocfree = NULL;
struct vproc **vproc_hash;
zone_t	     vproc_zone;
zone_t	     pvproc_zone;

/*
 * Declaration of pointer to LOCATE_VPROC_PID() routine.
 */
struct vproc *(*locate_vproc_pid_routine)(pid_t pid);

/*
 * Initialize vproc table.
 */
vproc_init()
{
	/*
	 * vproc table dynamically allocated, hence no initialization need be
	 * done.
	 */
}
