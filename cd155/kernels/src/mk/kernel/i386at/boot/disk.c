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
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * disk.c,v
 * Revision 1.1.4.2  1993/02/12  17:35:44  dleslie
 * Adding new code from vendor
 *
 * Revision 1.1  1992/09/30  02:03:30  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.1.2.1  92/04/30  11:53:17  bernadat
 * 	Fixed code to use mk includes only (not sys/inode.h ....).
 * 	Copied from main line
 * 	[92/03/19            bernadat]
 * 
 * Revision 2.2  92/04/04  11:35:49  rpd
 * 	Fabricated from 3.0 bootstrap and 2.5 boot disk.c:
 * 	with support for scsi
 * 	[92/03/30            mg32]
 * 
 */
/* CMU_ENDHIST */
/*
 * Mach Operating System
 * Copyright (c) 1992, 1991 Carnegie Mellon University
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
 */

#include "boot.h"
#include <i386at/disk.h>

#define	BIOS_DEV_FLOPPY	0x0
#define	BIOS_DEV_WIN	0x80

#define BPS		512
#define	SPT(di)		((di)&0xff)
#define	HEADS(di)	((((di)>>8)&0xff)+1)
#define	BDEV(dev,unit)	(((dev)==1 ? BIOS_DEV_FLOPPY:BIOS_DEV_WIN)+(unit))

char *devs[] = {"hd", "fd", "wt", "sd", "ha", 0};

struct alt_info alt_info;
int spt, spc;

char *iodest;
struct fs *fs;
struct file inode;
int unit, part, maj, boff, poff, bnum, cnt;

devopen()
{
	char *altptr;
	struct ipart *iptr;
	int dev = inode.f_dev;
	int i, sector, di = get_diskinfo(BDEV(dev,unit));
	spc = (spt = SPT(di)) * HEADS(di);
	if (dev == 1) {
		boff = 0;
		part = (spt == 15 ? 3 : 1);
	}
	else {
		if (maj == 0 && spt == 32 && spc == 32*64) /* SCSI */
			maj = 3;
		Bread(dev, unit, 0);
		iptr = (struct ipart *)((struct mboot *)0)->parts;
		for (i = 0; i < FD_NUMPART; i++, iptr++)
			if (iptr->systid == UNIXOS)
				break;
		sector = iptr->relsect + HDPDLOC;
		Bread(dev, unit, sector++);
		if (((struct evtoc *)0)->sanity != VTOC_SANE) {
			printf("vtoc insane");
			return 1;
		}
		boff = ((struct evtoc *)0)->part[part].p_start;
		altptr = (char *)(&alt_info);
		for (i = 0; i++ < 4; altptr += BPS, sector++) {
			Bread(dev, unit, sector);
			bcopy(0, altptr, BPS);
		}
		if (alt_info.alt_sanity != ALT_SANITY) {
			printf("Bad alt_sanity\n");
			return 1;
		}
	}
	return 0;
}

devread()
{
	int offset, sector = bnum;
	int dev = inode.f_dev;
	for (offset = 0; offset < cnt; offset += BPS) {
		Bread(dev, unit, badsect(dev, sector++));
		bcopy(0, iodest+offset, BPS);
	}
}

Bread(dev,unit,sector)
     int dev,unit,sector;
{
	int cyl = sector/spc, head = (sector%spc)/spt, secnum = sector%spt;
	while (biosread(BDEV(dev,unit), cyl,head,secnum))
		printf("Error: C:%d H:%d S:%d\n",cyl,head,secnum);
}

badsect(dev, sector)
     int dev, sector;
{
	int i;
	if (!dev) {
		for (i = 0; i < alt_info.alt_trk.alt_used; i++)
			if (sector/spt == alt_info.alt_trk.alt_bad[i])
				return (alt_info.alt_trk.alt_base + 
					i*spt + sector%spt);
		for (i = 0; i < alt_info.alt_sec.alt_used; i++)
			if (sector == alt_info.alt_sec.alt_bad[i])
				return (alt_info.alt_sec.alt_base + i);
	}
	return sector;
}
