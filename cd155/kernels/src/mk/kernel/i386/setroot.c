/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
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
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890

 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: setroot.c,v $
 * Revision 1.5  1994/11/18  20:34:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:25:08  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:23:45  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:23:38  dleslie
 * First R1_0 release
 *
 * Revision 2.9.9.2  92/03/28  10:05:40  jeffreyh
 * 	Allow boot from floppy device
 * 	[92/03/04            bernadat]
 * 
 * Revision 2.9.9.1  92/02/18  18:49:22  jeffreyh
 * 	Get root partition from boot args
 * 	[91/07/04            bernadat]
 * 
 * 	Support for SCSI disk
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.9  91/06/19  11:55:36  rvb
 * 	cputypes.h->platforms.h
 * 	[91/06/12  13:45:24  rvb]
 * 
 * Revision 2.8  91/05/14  16:16:26  mrt
 * 	Correcting copyright
 * 
 * Revision 2.7  91/05/08  12:42:00  dbg
 * 	Include platforms.h to get CPU names.
 * 	[91/03/21            dbg]
 * 
 * Revision 2.6  91/02/05  17:14:38  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:37:59  mrt]
 * 
 * Revision 2.5  90/12/04  14:46:35  jsb
 * 	iPSC2 -> iPSC386.
 * 	[90/12/04  11:19:36  jsb]
 * 
 * Revision 2.4  90/11/24  15:14:53  jsb
 * 	Added missing newline.
 * 	[90/11/24  11:45:51  jsb]
 * 
 * Revision 2.3  90/09/23  16:47:18  jsb
 * 	If iPSC386, root device is sd (scsi), not hd.
 * 
 * Revision 2.2  90/05/03  15:37:31  dbg
 * 	Created.
 * 	[90/02/20            dbg]
 * 
 */

#include <platforms.h>
#include <sys/reboot.h>

/*
 * Get root device.  Temporarily hard-coded.
 */
#if	iPSC386
char *root_name = "sd0a";
#else	iPSC386

struct boot_devices {
	int	dev;
	char	*name;
} boot_devices[] = {
	0,	"hd0a",
	1,	"fd0a",
	2,	"qt",
	3,	"sd0a"
};
char *root_name;

#endif	iPSC386

get_root_device()
{
#ifndef	iPSC386
	extern boottype;
	int unit,part;

 	root_name = boot_devices[(boottype>>B_TYPESHIFT)&B_TYPEMASK].name;
	if (unit = ((boottype>>B_UNITSHIFT)&B_UNITMASK) )
	  	root_name[2] = "01234567"[unit];
	if (part = ((boottype>>B_PARTITIONSHIFT)&B_PARTITIONMASK) )
	  	root_name[3] = "abcdefghijklmnop"[part];
#endif	iPSC386

	printf("root on %s\n", root_name);
}
