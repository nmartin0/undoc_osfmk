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
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * 05-Nov-92  David Bernard (bernard) at Open Software Foundation
 *	Use local minphys routine to limit transfer size to SECLIMIT
 *	 sectors total.
 *
 * 15-Oct-92  David Bernard (bernard) at Open Software Foundation
 *	Fixed NULLPARTITION case to NOT muck with b_count!
 *
 * $Log: hd.c,v $
 * Revision 1.5  1994/11/18  20:35:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:26:36  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:25:24  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:25:09  dleslie
 * First R1_0 release
 *
 * Revision 2.12.5.4  92/09/15  17:15:04  jeffreyh
 * 	   Fix value of total number of blocks to read and
 * 	   b_resid if requested size is bigger than number
 * 	   of remaining blocks and not an integral number of blocks.
 * 	   [barbou@gr.osf.org]
 * 	Added infinitely-fast disk code for partition p, some IO counters
 * 	[92/08/10            sjs]
 * 
 * Revision 2.12.5.3  92/04/30  11:55:26  bernadat
 * 	Parallelization for SystemPro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.12.5.2  92/03/28  10:06:18  jeffreyh
 * 	Added to the loop in probe to account for new stupid hardware.
 * 	[92/03/26            jeffreyh]
 * 	Changes from MK71
 * 	[92/03/20  12:15:07  jeffreyh]
 * 
 * Revision 2.17  92/03/01  00:39:53  rpd
 * 	Cleaned up syntactically incorrect ifdefs.
 * 	[92/02/29            rpd]
 * 
 * Revision 2.16  92/02/23  22:43:07  elf
 * 	Added (mandatory) DEV_GET_SIZE flavor of get_status.
 * 	[92/02/22            af]
 * 
 * Revision 2.15  92/02/19  16:29:51  elf
 * 	On 25-Jan, did not consider NO ACTIVE mach parition.
 * 	Add "BIOS" support -- always boot mach partition NOT active one.
 * 	[92/01/31            rvb]
 * 
 * Revision 2.14  92/01/27  16:43:06  rpd
 * 	Fixed hdgetstat and hdsetstat to return D_INVALID_OPERATION
 * 	for unsupported flavors.
 * 	[92/01/26            rpd]
 * 
 * Revision 2.13  92/01/14  16:43:51  rpd
 * 	Error in badblock_mapping code in the case there was sector replacement.
 * 	For all the sectors in the disk block before the bad sector, you
 * 	badblock_mapping should give the identity map and it was not.
 * 	[92/01/08            rvb]
 * 
 * Revision 2.12  91/11/18  17:34:19  rvb
 * 	For now, back out the hdprobe(), hdslave() probes and use
 * 	the old simple test and believe BIOS.
 * 
 * Revision 2.11  91/11/12  11:09:32  rvb
 * 	Amazing how hard getting the probe to work for all machines is.
 * 	V_REMOUNT must clear gotvtoc[].
 * 	[91/10/16            rvb]
 * 
 * Revision 2.10  91/10/07  17:25:35  af
 * 	Now works with 2 disk drives, new probe/slave routines, misc cleanup
 * 	[91/08/07            mg32]
 * 
 * 	From 2.5:
 *	Rationalize p_flag
 *	Kill nuisance print out.
 *	Removed "hdioctl(): do not recognize ioctl ..." printf().
 * 	[91/08/07            rvb]
 * 
 * Revision 2.9  91/08/28  11:11:42  jsb
 * 	Replace hdbsize with hddevinfo.
 * 	[91/08/12  17:33:59  dlb]
 * 
 * 	Add block size routine.
 * 	[91/08/05  17:39:16  dlb]
 * 
 * Revision 2.8  91/08/24  11:57:46  af
 * 	New MI autoconf.
 * 	[91/08/02  02:52:47  af]
 * 
 * Revision 2.7  91/05/14  16:23:24  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/02/05  17:17:01  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:43:01  mrt]
 * 
 * Revision 2.5  91/01/08  17:32:51  rpd
 * 	Allow ioctl's
 * 	[90/12/19            rvb]
 * 
 * Revision 2.4  90/11/26  14:49:37  rvb
 * 	jsb bet me to XMK34, sigh ...
 * 	[90/11/26            rvb]
 * 	Synched 2.5 & 3.0 at I386q (r1.8.1.15) & XMK35 (r2.4)
 * 	[90/11/15            rvb]
 * 
 * Revision 1.8.1.14  90/09/18  08:38:49  rvb
 * 	Typo & vs && at line 592.		[contrib]
 * 	Make Status printout on error only conditional on hd_print_error.
 * 	So we can get printout during clobber_my_disk.
 * 	[90/09/08            rvb]
 * 
 * Revision 1.8.1.13  90/08/25  15:44:38  rvb
 * 	Use take_<>_irq() vs direct manipulations of ivect and friends.
 * 	[90/08/20            rvb]
 * 
 * Revision 1.8.1.12  90/07/27  11:25:30  rvb
 * 	Fix Intel Copyright as per B. Davies authorization.
 * 	Let anyone who as opened the disk do absolute io.
 * 	[90/07/27            rvb]
 * 
 * Revision 1.8.1.11  90/07/10  11:43:22  rvb
 * 	Unbelievable bug in setcontroller.
 * 	New style probe/slave/attach.
 * 	[90/06/15            rvb]
 * 
 * Revision 1.8.1.10  90/03/29  19:00:00  rvb
 * 	Conditionally, print out state info for "state error".
 * 	[90/03/26            rvb]
 * 
 * Revision 1.8.1.8  90/03/10  00:27:20  rvb
 * 	Fence post error iff (bp->b_blkno + hh.blocktotal ) > partition_p->p_size)
 * 	[90/03/10            rvb]
 * 
 * Revision 1.8.1.7  90/02/28  15:49:35  rvb
 * 	Fix numerous typo's in Olivetti disclaimer.
 * 	[90/02/28            rvb]
 * 
 * Revision 1.8.1.6  90/01/16  15:54:14  rvb
 * 	FLush pdinfo/vtoc -> evtoc
 * 	[90/01/16            rvb]
 * 
 * 	Must be able to return "dos{cyl,head,sector}"
 * 	[90/01/12            rvb]
 * 
 * 	Be careful about p_size bound's checks if B_MD1 is true.
 * 	[90/01/12            rvb]
 * 
 * Revision 1.8.1.5  90/01/08  13:29:29  rvb
 * 	Add Intel copyright.
 * 	Add Olivetti copyright.
 * 	[90/01/08            rvb]
 * 
 * 	It is no longer possible to set the start and size of disk
 * 	partition "PART_DISK" -- it is always loaded from the DOS
 * 	partition data.
 * 	[90/01/08            rvb]
 * 
 * Revision 1.8.1.4  90/01/02  13:54:58  rvb
 * 	Temporarily regress driver to one that is known to work with Vectra's.
 * 
 */
 
 
/*
  Copyright 1988, 1989 by Intel Corporation, Santa Clara, California.

		All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Intel
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/*
 *  AT Hard Disk Driver
 *  Copyright Ing. C. Olivetti & S.p.A. 1989
 *  All rights reserved.
 *
 */
/*
  Copyright 1988, 1989 by Olivetti Advanced Technology Center, Inc.,
Cupertino, California.

		All Rights Reserved

  Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Olivetti
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

  OLIVETTI DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL OLIVETTI BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUR OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <hd.h>

#include <sys/types.h>
#ifdef	MACH_KERNEL
#define PRIBIO 20
#include <device/buf.h>
#include <device/errno.h>
#else	MACH_KERNEL
#include <sys/buf.h>
#include <sys/user.h> 
#endif	MACH_KERNEL
#include <sys/ioctl.h>
#include <i386/pio.h>
#include <i386/ipl.h>
#include <i386at/disk.h>
#include <chips/busses.h>
#include <i386at/hdreg.h>

#include <cpus.h>
#include <i386at/mp/mp.h>

#if	NCPUS > 1 && MBUS
#include <mbus/mbus.h>
struct mp_dev_lock	hd_dev_lock[NHD/2];
#define	at386_io_lock(op)	mbus_dev_lock(&hd_dev_lock[ctrl], (op))
#define	at386_io_unlock()	mbus_dev_unlock(&hd_dev_lock[ctrl])
#endif	NCPUS > 1 && MBUS

/* From sys/systm.h */
struct buf *geteblk();

#define b_cylin		b_resid
#define PAGESIZ 4096

int devaddr[NHD/2];
int hdalive[NHD];
int hdgotvtoc[NHD];
struct hh hh[NHD/2];
struct alt_info alt_info[NHD];
struct buf hdbuf[NHD], hdunit[NHD];

/*
 * metric variables
 */
int	hd_read_count = 0;
int	hd_write_count = 0;
int	hd_queue_length = 0;

partition_t	partition_struct[NHD][V_NUMPAR];

#define	DISKTEST
#ifdef	DISKTEST
#define	NULLPARTITION(z)	((z) == 0xf)	/* last partition */
#endif	DISKTEST

typedef struct {
	unsigned short	ncyl;
	unsigned short	nheads;
	unsigned short	precomp;
	unsigned short	nsec;
} hdisk_t;

hdisk_t	hdparm[NHD], rlparm[NHD];

int hdstrategy(), hdminphys(), hdprobe(), hdslave(), hdattach(), hdintr();
struct bus_device *hdinfo[NHD];		/* ???? */

caddr_t	hd_std[NHD] = { 0 };
struct	bus_device *hd_dinfo[NHD*NDRIVES];
struct	bus_ctlr *hd_minfo[NHD];
struct	bus_driver	hddriver = {
	hdprobe, hdslave, hdattach, 0, hd_std, "hd", hd_dinfo, "hdc", hd_minfo, 0};

int hdintr(), hdstart();


hdprobe(port, ctlr)
struct bus_ctlr *ctlr;
{
	int 	i,
		ctrl = ctlr->unit;
	int 	addr = devaddr[ctrl] = (int)ctlr->address;

	outb(PORT_DRIVE_HEADREGISTER(addr),0);
	outb(PORT_COMMAND(addr),CMD_RESTORE);
	for (i=450000; i && inb(PORT_STATUS(addr))&STAT_BUSY; i--);
	if (i && inb(PORT_STATUS(addr))&STAT_READY) {
		take_ctlr_irq(ctlr);
		hh[ctrl].curdrive = ctrl<<1;
		printf("%s%d: port = %x, spl = %d, pic = %d.\n", ctlr->name,
			ctlr->unit, ctlr->address, ctlr->sysdep, ctlr->sysdep1);
#if	0
		/* may be necesary for two controllers */
		outb(FIXED_DISK_REG(ctrl), 4);
		for(i = 0; i < 10000; i++);
		outb(FIXED_DISK_REG(ctrl), 0);
#endif
#if	NCPUS > 1 && MBUS
		simple_lock_init(&hd_dev_lock[ctrl].lock);
		hd_dev_lock[ctrl].unit = ctrl;
		hd_dev_lock[ctrl].pending_ops = 0;
		hd_dev_lock[ctrl].op[MP_DEV_OP_START] = hdstart;
		hd_dev_lock[ctrl].op[MP_DEV_OP_INTR] = hdintr;
#endif	NCPUS > 1 && MBUS
		return(1);
	}
	return(0);
}

/*
 * hdslave:
 *
 *	Actually should be thought of as a slave probe.
 *
 */

hdslave(dev, xxx)
struct bus_device *dev;
{
	int	i,
		addr = devaddr[dev->ctlr];
	u_char	*bios_magic = (u_char *)(0xc0000475);

	if (dev->ctlr == 0)				/* for now: believe DOS */
		if (*bios_magic >= 1 + dev->slave)
			return 1;
		else
			return 0;
	else
		return 1;
		
#if	0
	/* it is working on all types of PCs */
	outb(PORT_DRIVE_HEADREGISTER(addr),dev->slave<<4);
	outb(PORT_COMMAND(addr),CMD_RESTORE);

	for (i=350000; i && inb(PORT_STATUS(addr))&STAT_BUSY; i--);
	if (i == 0) {
		outb(FIXED_DISK_REG(dev->ctlr), 4);
		for(i = 0; i < 10000; i++);
		outb(FIXED_DISK_REG(dev->ctlr), 0);
		setcontroller(dev->slave);
		return 0;
	}
	return(i&STAT_READY);
#endif
}

/*
 * hdattach:
 *
 *	Attach the drive unit that has been successfully probed.  For the
 *	AT ESDI drives we will initialize all driver specific structures
 *	and complete the controller attach of the drive.
 *
 */

hdattach(dev)
struct bus_device *dev;
{
	int 	unit = dev->unit;
	u_long	n;
	u_char	*tbl;

	hdalive[unit] = 1;
	n = *(unsigned long *)phystokv(dev->address);
	tbl = (unsigned char *)phystokv((n&0xffff) + ((n>>12)&0xffff0));
	rlparm[unit].ncyl   =hdparm[unit].ncyl   = *(unsigned short *)tbl;
	rlparm[unit].nheads =hdparm[unit].nheads = *(unsigned char  *)(tbl+2);
	rlparm[unit].precomp=hdparm[unit].precomp= *(unsigned short *)(tbl+5);
	rlparm[unit].nsec   =hdparm[unit].nsec   = *(unsigned char  *)(tbl+14);
	printf(", stat = %x, spl = %d, pic = %d\n",
		dev->address, dev->sysdep, dev->sysdep1);
	if (unit<2)
		printf(" hd%d:   %dMeg, cyls %d, heads %d, secs %d, precomp %d",
			unit,
			hdparm[unit].ncyl*hdparm[unit].nheads*hdparm[unit].nsec
			 * 512/1000000,
			hdparm[unit].ncyl, hdparm[unit].nheads,
			hdparm[unit].nsec, hdparm[unit].precomp);
	else
		printf("hd%d:   Capacity not available through bios\n",unit);
	setcontroller(unit);
	return;
}

hdopen(dev, flags)
{
	u_char	unit = UNIT(dev),
		part = PARTITION(dev);

#ifdef	DISKTEST
	if (NULLPARTITION(part)) {
		partition_struct[unit][part].p_flag |= V_OPEN;
		return(0);
	}
#endif	DISKTEST
	if (!hdalive[unit] || part >= V_NUMPAR)
		return(ENXIO);
	getvtoc(unit);
	if (!(partition_struct[unit][part].p_flag & V_VALID))
		return(ENXIO);
	partition_struct[unit][part].p_flag |= V_OPEN;
	return(0);
}

hdclose(dev)
{
	partition_struct[UNIT(dev)][PARTITION(dev)].p_flag &= ~V_OPEN;
}

#ifdef	MACH_KERNEL
hdminphys(bp)
struct buf *bp;
{
	if (bp->b_bcount > SECLIMIT*SECSIZE)
		bp->b_bcount = SECLIMIT*SECSIZE;
}
int hdread(dev, ior)
dev_t dev;
io_req_t ior;
{
	hd_read_count++;
	return (block_io(hdstrategy, hdminphys, ior));
}

int hdwrite(dev, ior)
dev_t dev;
io_req_t ior;
{
	hd_write_count++;
	return (block_io(hdstrategy, hdminphys, ior));
}
#else	MACH_KERNEL
hdminphys(bp)
struct buf *bp;
{
	if (bp->b_bcount > PAGESIZ)
		bp->b_bcount = PAGESIZ;
}

hdread(dev,uio)
dev_t dev;
struct uio *uio;
{
	hd_read_count++;
	return(physio(hdstrategy,&hdbuf[UNIT(dev)],dev,B_READ,hdminphys,uio));
}

hdwrite(dev,uio)
dev_t dev;
struct uio *uio;
{
	hd_write_count++;
	return(physio(hdstrategy,&hdbuf[UNIT(dev)],dev,B_WRITE,hdminphys,uio));
}
#endif	MACH_KERNEL

#ifdef	MACH_KERNEL
int abs_sec   = -1;
int abs_count = -1;
/* IOC_OUT only and not IOC_INOUT */
io_return_t hdgetstat(dev, flavor, data, count)
	dev_t dev;
	int flavor;
	int *data;		/* pointer to OUT array */
	unsigned int *count;	/* OUT */
{
	int		unit = UNIT(dev);
	struct buf	*bp1;
	int		i;

	switch (flavor) {

	/* Mandatory flavors */

	case DEV_GET_SIZE: {
		int part = PARTITION(dev);
		data[DEV_GET_SIZE_DEVICE_SIZE] = partition_struct[unit][part].p_size * SECSIZE;
		data[DEV_GET_SIZE_RECORD_SIZE] = SECSIZE;
		*count = DEV_GET_SIZE_COUNT;
		break;
	}

	/* Extra flavors */

	case V_GETPARMS: {
		struct disk_parms *dp;
		int part = PARTITION(dev);
		if (*count < sizeof (struct disk_parms)/sizeof(int))
			return (D_INVALID_OPERATION);
		dp = (struct disk_parms *) data;
		dp->dp_type = DPT_WINI;
		dp->dp_heads = hdparm[unit].nheads;
		dp->dp_cyls = hdparm[unit].ncyl;
		dp->dp_sectors  = hdparm[unit].nsec;
  		dp->dp_dosheads = rlparm[unit].nheads;
		dp->dp_doscyls = rlparm[unit].ncyl;
		dp->dp_dossectors  = rlparm[unit].nsec;
		dp->dp_secsiz = SECSIZE;
		dp->dp_ptag = 0;
		dp->dp_pflag = partition_struct[unit][part].p_flag;
		dp->dp_pstartsec = partition_struct[unit][part].p_start;
		dp->dp_pnumsec = partition_struct[unit][part].p_size;
		*count = sizeof(struct disk_parms)/sizeof(int);
		break;
	}
	case V_RDABS: {
		/* V_RDABS is relative to head 0, sector 0, cylinder 0 */
		if (*count < SECSIZE/sizeof (int)) {
			printf("hd%d: RDABS bad size %x", unit, count);
			return (D_INVALID_OPERATION);
		}
		bp1 = geteblk(SECSIZE);
		bp1->b_flags = B_READ | B_MD1;	/* MD1 is be absolute */
		bp1->b_blkno = abs_sec;
		bp1->b_dev = WHOLE_DISK(unit);	/* C partition */
		bp1->b_bcount = SECSIZE;
		hdstrategy(bp1);
		biowait(bp1);
		if (bp1->b_flags & B_ERROR) {
			printf("hd%d hdsetstat(): read failure RDABS\n", unit);
			brelse(bp1);
			return (ENXIO);
		}
		bcopy(bp1->b_un.b_addr, (caddr_t)data, SECSIZE);
		brelse(bp1);
		*count = SECSIZE/sizeof(int);
		break;
	}
	case V_VERIFY: {
		unsigned int snum;
		int xcount;
		int code = 0;
		bp1 = geteblk(PAGESIZ);
		bp1->b_flags = B_READ;
		bp1->b_blkno = abs_sec;
		bp1->b_dev = WHOLE_DISK(unit);	/* C partition */
		xcount = abs_count;
		snum = PAGESIZ >> 9;
		while (xcount > 0) {
			i = (xcount > snum) ? snum : xcount;
			bp1->b_bcount = i << 9;
			bp1->b_flags |= B_MD1;
			hdstrategy(bp1);
			biowait(bp1);
			if (bp1->b_flags & B_ERROR) {
				code = BAD_BLK;
				break;
			}
			xcount -= i;
			bp1->b_blkno += i;
			bp1->b_flags &= ~B_DONE;
		}
		brelse(bp1);
		data[0] = code;
		*count = 1;
		break;
	}
	default:
		return (D_INVALID_OPERATION);
	}
	return (D_SUCCESS);
}

/* IOC_VOID or IOC_IN or IOC_INOUT */
io_return_t hdsetstat(dev, flavor, data, count)
dev_t dev;
int flavor;
int * data;
unsigned int count;
{
	struct buf	*bp1;
	io_return_t	errcode = D_SUCCESS;
	int		unit = UNIT(dev);

	switch (flavor) {
	case V_REMOUNT:
		hdgotvtoc[unit] = 0;
		getvtoc(unit);
		break;
	case V_ABS:
		abs_sec = *(int *)data;
		if (count == 2)
			abs_count = data[1];
		break;
	case V_WRABS:
		/* V_WRABS is relative to head 0, sector 0, cylinder 0 */
		if (count < SECSIZE/sizeof (int)) {
			printf("hd%d: WRABS bad size %x", unit, count);
			return (D_INVALID_OPERATION);
		}
		bp1 = geteblk(SECSIZE);
		bcopy((caddr_t)data, bp1->b_un.b_addr, SECSIZE);
		bp1->b_flags = B_WRITE | B_MD1;	/* MD1 is be absolute */
		bp1->b_blkno = abs_sec;
		bp1->b_dev = WHOLE_DISK(unit);	/* C partition */
		bp1->b_bcount = SECSIZE;
		hdstrategy(bp1);
		biowait(bp1);
		if (bp1->b_flags & B_ERROR) {
			printf("hd%d: hdsetstat() read failure WRABS\n", unit);
			errcode = ENXIO;
		}
		brelse(bp1);
		break;
	default:
		return (D_INVALID_OPERATION);
	}
	return (errcode);
}
#else	MACH_KERNEL
hdioctl(dev, cmd, arg, mode)
dev_t dev;
int cmd;
caddr_t arg;
{
	u_char		unit = UNIT(dev),
			ctrl = unit>>1;
	union io_arg	*arg_kernel;
	int		snum, xcount, i;
	struct buf	*bp1;

	switch (cmd) {
	case V_CONFIG:
		arg_kernel = (union io_arg *)arg;
		if (arg_kernel->ia_cd.secsiz != SECSIZE)
			/* changing sector size NOT allowed */
		  	return(EINVAL);
		hdparm[unit].ncyl=(unsigned short)arg_kernel->ia_cd.ncyl;
		hdparm[unit].nheads=(unsigned short)arg_kernel->ia_cd.nhead;
		hdparm[unit].nsec=(unsigned short)arg_kernel->ia_cd.nsec;
		setcontroller(unit);
		break;
	case V_REMOUNT:
		hdgotvtoc[unit] = 0;
		getvtoc(unit);	
		break;
	case V_ADDBAD:
		/* this adds a bad block to IN CORE alts table ONLY */
		alt_info[unit].alt_sec.alt_used++;
		alt_info[unit].alt_sec.alt_bad[alt_info[unit].alt_sec.alt_used]
			= ((union io_arg *)arg)->ia_abs.bad_sector;
		break;
	case V_GETPARMS:
		{
		unsigned char part = PARTITION(dev);
		struct disk_parms *disk_parms = (struct disk_parms *)arg;
		disk_parms->dp_type = DPT_WINI;
		disk_parms->dp_heads = hdparm[unit].nheads;
		disk_parms->dp_cyls = hdparm[unit].ncyl;
		disk_parms->dp_sectors  = hdparm[unit].nsec;
  		disk_parms->dp_dosheads = rlparm[unit].nheads;
		disk_parms->dp_doscyls = rlparm[unit].ncyl;
		disk_parms->dp_dossectors  = rlparm[unit].nsec;
		disk_parms->dp_secsiz = SECSIZE;
		disk_parms->dp_ptag = 0;
		disk_parms->dp_pflag = partition_struct[unit][part].p_flag;
		disk_parms->dp_pstartsec =partition_struct[unit][part].p_start;
		disk_parms->dp_pnumsec =partition_struct[unit][part].p_size;
		break;
		}
	case V_PDLOC:
		*(unsigned int *)arg=hh[ctrl].start_of_unix[unit&1]+PDLOCATION;
		break;
	case V_RDABS:
		/* V_RDABS is relative to head 0, sector 0, cylinder 0 */
		bp1 = geteblk(SECSIZE);
		bp1->b_flags = B_READ | B_MD1;	/* MD1 is be absolute */
		bp1->b_blkno = ((struct absio *)arg)->abs_sec;
		bp1->b_dev = WHOLE_DISK(unit);	/* C partition for RDABS */
		bp1->b_bcount = SECSIZE;
		hdstrategy(bp1);
		biowait(bp1);
		if (bp1->b_flags & B_ERROR) {
			printf("hd: read failure on ioctl\n");
			brelse(bp1);
			return(ENXIO);
		}
		if(copyout(bp1->b_un.b_addr,((struct absio *)arg)->abs_buf,
			   SECSIZE)){
			brelse(bp1);
			return(ENXIO);
		}
		brelse(bp1);
		break;
	case V_WRABS:
		/* V_WRABS is relative to head 0, sector 0, cylinder 0 */
		bp1 = geteblk(SECSIZE);
		if (copyin(((struct absio *)arg)->abs_buf,bp1->b_un.b_addr,
			   SECSIZE)){
			brelse(bp1);
			return(ENXIO);
		}
		bp1->b_flags = B_WRITE | B_MD1;	/* MD1 is be absolute */
		bp1->b_blkno = ((struct absio *)arg)->abs_sec;
		bp1->b_dev = WHOLE_DISK(unit);	/* C partition for RDABS */
		bp1->b_bcount = SECSIZE;
		hdstrategy(bp1);
		biowait(bp1);
		if (bp1->b_flags & B_ERROR) {
			printf("hd: write failure on ioctl\n");
			brelse(bp1);
			return(ENXIO);
		}
		brelse(bp1);
		break;
	case V_VERIFY:
		if (u.u_uid)
			return(ENXIO);
		bp1 = geteblk(PAGESIZ);
		bp1->b_flags = B_READ | B_MD1;
		bp1->b_blkno = ((union vfy_io *)arg)->vfy_in.abs_sec;
		bp1->b_dev = WHOLE_DISK(unit);	/* C partition for RDABS */
		xcount = ((union vfy_io *)arg)->vfy_in.num_sec;
		((union vfy_io *)arg)->vfy_out.err_code = 0;
		snum = PAGESIZ >> 9;
		while (xcount > 0) {
			i = (xcount > snum) ? snum : xcount;
			bp1->b_bcount = i << 9;
			hdstrategy(bp1);
			biowait(bp1);
			if (bp1->b_flags & B_ERROR) {
			       ((union vfy_io *)arg)->vfy_out.err_code=BAD_BLK;
			       break;
			}
			xcount -= i;
			bp1->b_blkno += i;
			bp1->b_flags &= ~B_DONE;
		}
		brelse(bp1);
		break;
	default:
		return(EINVAL);
	}
	return(0);
}
#endif	MACH_KERNEL

hdstrategy(bp)
struct buf *bp;
{
	u_char		unit = UNIT(bp->b_dev),
			ctrl = unit>>1;
	partition_t	*part_p = &partition_struct[unit][PARTITION(bp->b_dev)];
	u_int		opri;

#ifdef	DISKTEST
	if (NULLPARTITION(PARTITION(bp->b_dev))) {
		bp->b_resid = 0;
		goto done;
	}
#endif	DISKTEST

	if (!bp->b_bcount)
		goto done;
	if (!(part_p->p_flag & V_VALID)) {
		bp->b_error = ENXIO;
		goto bad;
	}
	/* if request is off the end or trying to write last block on out */
	if (bp->b_flags & B_MD1) {
		if (bp->b_blkno > partition_struct[unit][PART_DISK].p_start +
		    partition_struct[unit][PART_DISK].p_size - 1) {
			bp->b_error = ENXIO;
			goto bad;
		}
	} else {
		if ((bp->b_blkno > part_p->p_size) ||
		    (bp->b_blkno==part_p->p_size && !(bp->b_flags & B_READ))) {
			bp->b_error = ENXIO;
			goto bad;
		}
		if (bp->b_blkno == part_p->p_size) {
			/* indicate (read) EOF by setting b_resid to b_bcount on last block */ 
			bp->b_resid = bp->b_bcount;
			goto done;
		}
	}
	bp->b_cylin = ((bp->b_flags&B_MD1 ? 0 : part_p->p_start) + bp->b_blkno)
		/ (hdparm[unit].nsec * hdparm[unit].nheads);
	opri = spl5();
	simple_lock(&hdunit[unit].io_req_lock);

	disksort(&hdunit[unit], bp);
	if (++hdunit[unit].io_total > hd_queue_length)
		hd_queue_length = hdunit[unit].io_total;
	simple_unlock(&hdunit[unit].io_req_lock);
	if (!hh[ctrl].controller_busy)
		hdstart(ctrl);
	splx(opri);
	return;
bad:
	bp->b_flags |= B_ERROR;
done:
	iodone(bp);
	return;
}

/* hdstart() is called at spl5 */
hdstart(ctrl)
int ctrl;
{
	partition_t		*part_p;
	register struct buf	*bp;

	if (!at386_io_lock(MP_DEV_OP_START)) 
	  	return;
	if (hh[ctrl].controller_busy)
		goto done;
	if (bp = hdunit[hh[ctrl].curdrive^1].b_actf)
		hh[ctrl].curdrive^=1;
	else if (!(bp = hdunit[hh[ctrl].curdrive].b_actf))
		goto done;
	hh[ctrl].controller_busy = 1;
	hh[ctrl].blocktotal = (bp->b_bcount + 511) >> 9;
	part_p = &partition_struct[UNIT(bp->b_dev)][PARTITION(bp->b_dev)];
	/* see V_RDABS and V_WRABS in hdioctl() */
	if (bp->b_flags & B_MD1) {
		int end = partition_struct[hh[ctrl].curdrive][PART_DISK].p_start +
			partition_struct[hh[ctrl].curdrive][PART_DISK].p_size - 1;
		hh[ctrl].physblock = bp->b_blkno;
		if ((bp->b_blkno + hh[ctrl].blocktotal) > end)
			hh[ctrl].blocktotal = end - bp->b_blkno + 1;
	} else {
 		hh[ctrl].physblock = part_p->p_start + bp->b_blkno;
		if ((bp->b_blkno + hh[ctrl].blocktotal) > part_p->p_size)
			hh[ctrl].blocktotal = part_p->p_size - bp->b_blkno;
	}
	hh[ctrl].blockcount = 0;
	hh[ctrl].rw_addr = (int)bp->b_un.b_addr;
	hh[ctrl].retry_count = 0;
	start_rw(bp->b_flags & B_READ, ctrl);
done:
	at386_io_unlock();
}

int hd_print_error = 0;
hdintr(ctrl)
int ctrl;
{
	register struct buf	*bp;
	int			addr = devaddr[ctrl],
				unit = hh[ctrl].curdrive;
	u_char status;

	if (!at386_io_lock(MP_DEV_OP_INTR))
		return;
	if (!hh[ctrl].controller_busy)
		goto done;
	waitcontroller(ctrl);
	status = inb(PORT_STATUS(addr));
	bp = hdunit[unit].b_actf;
	if (hh[ctrl].restore_request) { /* Restore command has completed */
		hh[ctrl].restore_request = 0;
		if (status & STAT_ERROR)
			hderror(bp,ctrl);
		else if (bp)
			start_rw(bp->b_flags & B_READ,ctrl);
		goto done;
	}
	if (status & STAT_WRITEFAULT) {
		if (hd_print_error) {
			 printf("hdintr: write fault. block %d, count %d, total %d\n",
			 	hh[ctrl].physblock, hh[ctrl].blockcount,
				hh[ctrl].blocktotal);
			 printf("hdintr: write fault. cyl %d, head %d, sector %d\n",
			 	hh[ctrl].cylinder, hh[ctrl].head,
				hh[ctrl].sector);

		}
		panic("hd: write fault\n");
	}
	if (status & STAT_ERROR) {
		if (hd_print_error) {
			 printf("hdintr: state error %x, error = %x\n",
			 	status, inb(PORT_ERROR(addr)));
			 printf("hdintr: state error. block %d, count %d, total %d\n",
			 	hh[ctrl].physblock, hh[ctrl].blockcount,
				hh[ctrl].blocktotal);
			 printf("hdintr: state error. cyl %d, head %d, sector %d\n",
			 	hh[ctrl].cylinder, hh[ctrl].head,
				hh[ctrl].sector);

		}
		hderror(bp,ctrl);
		goto done;
	}
	if (status & STAT_ECC) 
		printf("hd: ECC soft error fixed, unit %d, partition %d, physical block %d \n",
			unit, PARTITION(bp->b_dev), hh[ctrl].physblock);
	if (!bp) {
		/* there should be a read/write buffer queued at this point */
		printf("hd%d: no bp buffer to read or write\n",unit);
		goto done;
	}
	if (bp->b_flags & B_READ) {
		while (!(inb(PORT_STATUS(addr)) & STAT_DATAREQUEST));
		linw(PORT_DATA(addr), hh[ctrl].rw_addr, SECSIZE/2); 
	}
	if (++hh[ctrl].blockcount == hh[ctrl].blocktotal) {
		simple_lock(&hdunit[unit].io_req_lock);
		hdunit[unit].b_actf = hdunit[unit].b_actf->av_forw;
		hdunit[unit].io_total--;
		simple_unlock(&hdunit[unit].io_req_lock);
		bp->b_resid = bp->b_bcount - (hh[ctrl].blocktotal << 9);
		if (bp->b_resid < 0) 
			bp->b_resid = 0;
		iodone(bp);
		hh[ctrl].controller_busy = 0;
		hdstart(ctrl);
	} else {
		hh[ctrl].rw_addr += SECSIZE;
		hh[ctrl].physblock++;
		if (hh[ctrl].single_mode)
			start_rw(bp->b_flags & B_READ,ctrl);
		else if (!(bp->b_flags & B_READ)) {
			/* Load sector into controller for next write */
			while (!(inb(PORT_STATUS(addr)) & STAT_DATAREQUEST));
			loutw(PORT_DATA(addr), hh[ctrl].rw_addr, SECSIZE/2);
		}
	}
done:
	at386_io_unlock();
}

hderror(bp,ctrl)
struct buf *bp;
int ctrl;
{
	int	addr = devaddr[ctrl],
		unit = hh[ctrl].curdrive;

	if(++hh[ctrl].retry_count > 3) {
		if(bp) {
			int i;
			/****************************************************
			* We have a problem with this block, set the error  *
			* flag, terminate the operation and move on to the  *
			* next request.  With every hard disk transaction   *
			* error we set the reset requested flag so that the *
			* controller is reset before the next operation is  *
			* started.					    *
			****************************************************/
			simple_lock(&hdunit[unit].io_req_lock);
			hdunit[unit].b_actf = hdunit[unit].b_actf->av_forw;
			hdunit[unit].io_total--;
			simple_unlock(&hdunit[unit].io_req_lock);
			bp->b_flags |= B_ERROR;
			bp->b_resid = 0;
			iodone(bp);
			outb(FIXED_DISK_REG(ctrl), 4);
			for(i = 0; i < 10000; i++);
			outb(FIXED_DISK_REG(ctrl), 0);
			setcontroller(unit);
			hh[ctrl].controller_busy = 0;
			hdstart(ctrl);
		}
	}
	else {
		/* lets do a recalibration */
		waitcontroller(ctrl);
		hh[ctrl].restore_request = 1;
		outb(PORT_PRECOMP(addr), hdparm[unit].precomp>>2);
		outb(PORT_NSECTOR(addr), hdparm[unit].nsec);
		outb(PORT_SECTOR(addr), hh[ctrl].sector);
		outb(PORT_CYLINDERLOWBYTE(addr), hh[ctrl].cylinder & 0xff);
		outb(PORT_CYLINDERHIBYTE(addr), (hh[ctrl].cylinder>>8) & 0xff);
		outb(PORT_DRIVE_HEADREGISTER(addr), (unit&1)<<4);
		outb(PORT_COMMAND(addr), CMD_RESTORE);
	}
}

static struct boot_record mempty = {0};
getvtoc(unit)
unsigned char unit;
{
	u_int			n;
	struct boot_record	*boot_record_p;
	struct evtoc		*evp;
	struct buf		*bp1;

	if (hdgotvtoc[unit]) {
		if (hdgotvtoc[unit] == -1)
			sleep(&hdgotvtoc[unit],PRIBIO);
		return;
	}
	hdgotvtoc[unit] = -1;
	bp1 = geteblk(SECSIZE);		/* for evtoc */
	/* make partition 0 the whole disk in case of failure then get pdinfo*/
	partition_struct[unit][PART_DISK].p_flag = V_OPEN|V_VALID;
	partition_struct[unit][PART_DISK].p_start = 0;
	partition_struct[unit][PART_DISK].p_size = hdparm[unit].ncyl *
		hdparm[unit].nheads * hdparm[unit].nsec;
	/* get active partition */
	bp1->b_flags = B_READ | B_MD1;
	bp1->b_blkno = 0;
	bp1->b_dev = WHOLE_DISK(unit);	/* C partition */
	bp1->b_bcount = SECSIZE;
	hdstrategy(bp1);
	biowait(bp1);
	if (bp1->b_flags & B_ERROR) {
		printf("hd%d: can't read sector 0 boot record\n",unit);
		goto done;
	}
	if (*(unsigned char *)(bp1->b_un.b_addr+511) != BOOTRECORDSIGNATURE) {
		printf("hd%d: sector 0 corrupt, can't read boot record\n",unit);
		goto done;
	}
	boot_record_p = (struct boot_record *)(bp1->b_un.b_addr+446);
#ifdef	old
	for (n=0; n<4; n++, boot_record_p++)
		if (boot_record_p->boot_ind == 0x80) break;
	if (n==4) {
		printf("hd%d: no active partition\n", unit);
		goto done;
	}
#else	old
	{
		int nmach = 0;
		struct boot_record	*mabr = (struct boot_record *) 0,
					*mbr = (struct boot_record *) 0;
		for (n = 0; n < 4; n++, boot_record_p++) {
			if (boot_record_p->sys_ind == UNIXOS) {
				if (!nmach++)
					mabr = boot_record_p;
				if (boot_record_p->boot_ind == ACTIVE)
					mbr = boot_record_p;
			}
		}
		if ((int)mabr)
			boot_record_p = mabr;
		else if (nmach == 1)
			boot_record_p = mbr;
		else if (!nmach)
			boot_record_p = &mempty;
		else {
			printf("More than one mach partition and none active.\n");
			printf("Try to fix this from floppies or DOS.\n");
			for (;;);
		}
	}
#endif	old
	hh[unit>>1].start_of_unix[unit&1] = boot_record_p->rel_sect;
	/* set correct partition information */
	partition_struct[unit][PART_DISK].p_start = boot_record_p->rel_sect;
	partition_struct[unit][PART_DISK].p_size = hdparm[unit].ncyl *
		hdparm[unit].nheads * hdparm[unit].nsec -
		hh[unit>>1].start_of_unix[unit&1];
	/* get evtoc out of active unix partition */
	bp1->b_flags = B_READ;
	bp1->b_blkno = PDLOCATION;
	hdstrategy(bp1);
	biowait(bp1);
	if (bp1->b_flags & B_ERROR) {
		printf("hd%d: can't read evtoc\n", unit);
		goto done;
	}
	if ((evp = (struct evtoc *)bp1->b_un.b_addr)->sanity != VTOC_SANE) {
		printf("hd%d: evtoc corrupt\n",unit);
		goto done;
	}
	/* pd info from disk must be more accurate than that in cmos thus
	   override hdparm and re- setcontroller() */
	hdparm[unit].ncyl = evp->cyls;
	hdparm[unit].nheads = evp->tracks;
	hdparm[unit].nsec = evp->sectors;
	partition_struct[unit][PART_DISK].p_size = hdparm[unit].ncyl *
		   hdparm[unit].nheads * hdparm[unit].nsec - 
		   hh[unit>>1].start_of_unix[unit&1];
	setcontroller(unit);
	/* copy info on all valid partition, zero the others */
	for (n = 0; n < evp->nparts; n++) {
		if (n == PART_DISK)
			continue;
		/* this is a STRUCTURE copy */
		partition_struct[unit][n] = evp->part[n];
		if (evp->part[n].p_start >= 0 && evp->part[n].p_size >= 0)
			partition_struct[unit][n].p_flag = V_VALID;
		else
			partition_struct[unit][n].p_flag = 0;
	}
	for ( ; n < V_NUMPAR; n++) {
		if (n == PART_DISK)
			continue;
		partition_struct[unit][n].p_flag = 0;
		partition_struct[unit][n].p_size = 0;
	}
	/* leave partition C "open" for raw I/O */
	partition_struct[unit][PART_DISK].p_flag |= V_OPEN;
	/* get alternate sectors out of active unix partition */
	bp1->b_blkno = evp->alt_ptr/SECSIZE;
	for (n = 0; n < 4; n++) {
		bp1->b_flags = B_READ;
		hdstrategy(bp1);
		biowait(bp1);
		if (bp1->b_flags & B_ERROR) {
			printf("hd%d: can't read alternate sectors\n", unit);
			goto done;
		}
		bcopy(bp1->b_un.b_addr,(char *)&alt_info[unit]+n*SECSIZE,
		      SECSIZE);
		bp1->b_blkno++;
	}
	if (alt_info[unit].alt_sanity != ALT_SANITY)
		printf("hd%d: alternate sectors corrupt\n", unit);
done:	brelse(bp1);
	hdgotvtoc[unit]=1;
	wakeup(&hdgotvtoc[unit]);
}

setcontroller(unit)
{
	int	ctrl = unit >> 1;
	int	addr = devaddr[ctrl];

	waitcontroller(ctrl);
	outb(PORT_DRIVE_HEADREGISTER(addr), (hdparm[unit].nheads - 1) |
	     ((unit&1) << 4) | FIXEDBITS);
	outb(PORT_NSECTOR(addr), hdparm[unit].nsec);
	outb(PORT_COMMAND(addr), CMD_SETPARAMETERS);
	waitcontroller(ctrl);
}

waitcontroller(ctrl)
{
	u_int	n = PATIENCE;

	while (--n && inb(PORT_STATUS(devaddr[ctrl])) & STAT_BUSY);
	if (n)
		return;
	panic("hd%d: waitcontroller() timed out",ctrl);
}

start_rw(read,ctrl)
     int read, ctrl;
{
	u_int	track, disk_block, xblk;
	int	addr = devaddr[ctrl],
		unit = hh[ctrl].curdrive;

	disk_block = hh[ctrl].physblock;
	xblk=hh[ctrl].blocktotal - hh[ctrl].blockcount; /*# blks to transfer*/
	if (!(hdunit[unit].b_actf->b_flags & B_MD1) &&
	    (hh[ctrl].single_mode = xfermode(ctrl))) {
		xblk = 1;
		if (PARTITION(hdunit[unit].b_actf->b_dev) != PART_DISK)
			disk_block = badblock_mapping(ctrl);
	}
	/* disk is formatted starting sector 1, not sector 0 */
	hh[ctrl].sector = (disk_block % hdparm[unit].nsec) + 1;
	track = disk_block / hdparm[unit].nsec;
	hh[ctrl].head = track % hdparm[unit].nheads | 
		(unit&1) << 4 | FIXEDBITS;
	hh[ctrl].cylinder = track / hdparm[unit].nheads;
	waitcontroller(ctrl);
	outb(PORT_PRECOMP(addr), hdparm[unit].precomp >>2);
	outb(PORT_NSECTOR(addr), xblk);
	outb(PORT_SECTOR(addr), hh[ctrl].sector);
	outb(PORT_CYLINDERLOWBYTE(addr), hh[ctrl].cylinder & 0xff );
	outb(PORT_CYLINDERHIBYTE(addr),  (hh[ctrl].cylinder >> 8) & 0xff );
	outb(PORT_DRIVE_HEADREGISTER(addr), hh[ctrl].head);
	if(read) {
		outb(PORT_COMMAND(addr), CMD_READ);
	} else {
 		outb(PORT_COMMAND(addr), CMD_WRITE);
		waitcontroller(ctrl);
		while (!(inb(PORT_STATUS(addr)) & STAT_DATAREQUEST));
		loutw(PORT_DATA(addr), hh[ctrl].rw_addr, SECSIZE/2);
	}
}

int badblock_mapping(ctrl)
int ctrl;
{
	u_short	n;
	u_int	track,
		unit = hh[ctrl].curdrive,
		block = hh[ctrl].physblock,
		nsec = hdparm[unit].nsec;

	track = block / nsec;
	for (n = 0; n < alt_info[unit].alt_trk.alt_used; n++)
		if (track == alt_info[unit].alt_trk.alt_bad[n])
			return alt_info[unit].alt_trk.alt_base +
			       nsec * n + (block % nsec);
	/* BAD BLOCK MAPPING */
	for (n = 0; n < alt_info[unit].alt_sec.alt_used; n++)
		if (block == alt_info[unit].alt_sec.alt_bad[n])
			return alt_info[unit].alt_sec.alt_base + n;
	return block;
}

/*
 *  determine single block or multiple blocks transfer mode
 */
int xfermode(ctrl)
int ctrl;
{
	int	n, bblk, eblk, btrk, etrk;
	int	unit = hh[ctrl].curdrive;

	bblk = hh[ctrl].physblock;
	eblk = bblk + hh[ctrl].blocktotal - 1;
	btrk = bblk / hdparm[unit].nsec;
	etrk = eblk / hdparm[unit].nsec;
	
	for (n = 0; n < alt_info[unit].alt_trk.alt_used; n++)
		if ((btrk <= alt_info[unit].alt_trk.alt_bad[n]) &&
		    (etrk >= alt_info[unit].alt_trk.alt_bad[n]))
			return 1;
	for (n = 0; n < alt_info[unit].alt_sec.alt_used; n++)
		if ((bblk <= alt_info[unit].alt_sec.alt_bad[n]) &&
		    (eblk >= alt_info[unit].alt_sec.alt_bad[n]))
			return 1;
	return 0;
}

hdsize()
{
	printf("hdsize()		-- not implemented\n");
}

hddump()
{
	printf("hddump()		-- not implemented\n");
}

#ifdef	MACH_KERNEL
/*
 *	Routine to return information to kernel.
 */
int
hddevinfo(dev, flavor, info)
dev_t	dev;
int	flavor;
char	*info;
{
	register int	result;

	result = D_SUCCESS;

	switch (flavor) {
	case D_INFO_BLOCK_SIZE:
		*((int *) info) = SECSIZE;
		break;
	default:
		result = D_INVALID_OPERATION;
	}

	return(result);
}
#endif	MACH_KERNEL
