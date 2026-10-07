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
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log: conf.c,v $
 * Revision 1.16  1994/11/18  20:31:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.15  1994/06/28  23:01:48  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.14  1994/06/08  18:59:23  hobbes
 * Initial changes for ipi3 changes in R1.3.
 *
 * Revision 1.13  1994/01/12  17:47:46  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.12  1993/09/28  04:22:45  robboy
 * Corrected "#if FULLSERVER" around ctape_open(), so the Lite server will link
 *
 * Revision 1.11  1993/09/27  04:32:09  robboy
 * Iffed out disk and tape functions if not FULLSERVER
 *
 * Revision 1.10  1993/09/23  15:54:46  cfj
 * In tape_open() and ctape_open(), call disk_close() if the set_device_status()
 * call returns an error.
 *
 * Revision 1.9  1993/09/20  23:06:23  cfj
 * Add support for the DAT which supports compression.
 *
 * Revision 1.8  1993/07/14  17:58:22  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:17:36  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/06/24  17:13:44  wunder
 * added the TAPE_UNIT macro to the dev arg of lookup in tape_open.
 *
 * Revision 1.6  1993/05/27  22:38:41  hobbes
 * Added entries for HIPPI character device.
 *
 * Revision 1.5  1993/05/06  19:08:59  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:29:47  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/03  03:05:56  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1993/02/16  20:04:01  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.3  1993/01/19  16:24:09  cfj
 * Modified tape_open() arguments to match disk_open().
 *
 * Revision 1.2  1992/11/30  22:21:39  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:45  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.8  1993/01/29  13:58:46  durriya
 * 	set return_short_reads in devinfo struct to TRUE in tape_open  (durriya)
 *
 * Revision 2.7  93/01/08  14:30:07  durriya
 * 	add node as arg to tape_close, read,write,ioctl, Fix arg specification
 * 	for tape_open.  Also pass node # as arg to the various disk_* ops 
 * 	called from the tape_* functions.
 * 
 * Revision 2.6  92/09/17  13:43:00  rabii
 * 	Replaced "sd" with "rz" (rabii)
 * 
 * Revision 2.5  92/07/14  14:56:17  rabii
 * 	dev_rawinfo -> devinfo
 * 	[92/07/10            roy]
 * 
 * Revision 2.4  92/06/30  22:46:42  loverso
 * 	Revision 2.4  92/06/12  09:22:55  stans
 * 	support SCSI tape ala cfj's fixes/cleanup.
 * 
 * 	Revision 2.4  92/05/23  10:18:57  cfj
 * 	Removed block special tape.
 * 
 * Revision 2.3  92/01/09  16:48:22  roy
 * 	91/12/20  11:52:58  stans
 * 	logical volume manager support (lvm) included.
 * 
 * Revision 2.2  91/11/13  12:17:44  rabii
 * 	Initial checkin
 * 
 * Revision 2.2  91/08/31  13:33:07  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.9  91/08/28  10:56:10  barbou
 * Upgrade to OSF/1.0.2.
 * 
 * Revision 1.7  90/12/06  14:02:18  devrcs
 * 	Changes for osc1.0 release.
 * 	[90/12/01  18:26:00  gm]
 * 
 * Revision 1.6  90/11/03  10:42:43  devrcs
 * 	Add klog device at cdev major 19.
 * 	[90/11/01  16:27:05  brezak]
 * 
 * 	Add back iopl device at major 16.
 * 	[90/10/26  14:51:57  brezak]
 * 
 * 	Add STREAMS clone device, with padding for other streams drivers.
 * 	[90/10/25  12:49:43  tmt]
 * 
 * 	Add LVM device driver entries (2).
 * 	[90/10/25  10:04:08  jeffc]
 * 
 * 	Make [bc]devsw arrays the correct size! Delete dead code.
 * 	[90/10/24  13:28:10  tmt]
 * 
 * 	Fix typo - kd_mmap --> kdmmap.
 * 	[90/10/09  09:09:36  brezak]
 * 
 * Revision 1.5  90/10/07  13:33:56  devrcs
 * 	Modified select function for X ./kernel/i386/AT386/conf.c
 * 	[90/10/05  16:31:49  dmr]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:25:22  gm]
 * 
 * 	Changed the ioctl fields for the floppy and tape bdev entries from
 * 	0 to xxioctl !!!!!
 * 	[90/09/21  15:10:34  jd]
 * 
 * Revision 1.4  90/08/24  11:33:49  devrcs
 * 	fixed error
 * 	[90/08/19  19:31:58  kevins]
 * 
 * 	Fix compile errors.
 * 	[90/08/19  15:06:31  kevins]
 * 
 * 	Merged boot time driver attch code
 * 	[90/08/17  17:14:48  kevins]
 * 
 * 	Merged boot time driver attch code
 * 	[90/08/17  17:14:48  kevins]
 * 
 * Revision 1.3  90/07/17  11:25:19  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:09:00  kevins]
 * 
 * Revision 1.2  90/04/27  18:58:07  devrcs
 * 	Added cdevlock and bdevlock
 * 	[90/04/26  15:21:01  jd]
 * 
 * 	Moved file to i386/AT386 directory
 * 	[90/04/17  12:20:16  jd]
 * 
 * Revision 1.2  90/02/27  21:14:36  devrcs
 * 	Added OSF Copyright marker.
 * 	[90/02/27  20:06:57  gm]
 * 
 * Revision 1.1  90/02/23  00:27:12  devrcs
 * 	Latest version for osc.5
 * 	[90/02/20  11:47:16  kevins]
 * 
 * Revision 1.9  89/09/25  12:26:28  rvb
 * 	Replace qt with wt
 * 	[89/09/23            rvb]
 * 
 * Revision 1.8  89/09/20  17:27:28  rvb
 * 	Support for Olivetti ln and ec.
 * 	[89/09/20            rvb]
 * 
 * Revision 1.7  89/09/09  15:21:34  rvb
 * 	Make devices configurable.
 * 	[89/09/09            rvb]
 * 
 * 	Olivetti Changes to X79 upto 5/9/89:
 * 	[89/07/11            rvb]
 * 
 * Revision 1.6  89/09/05  20:41:45  jsb
 * 	Added Mach time device at #14.
 * 	[89/09/05  19:16:01  jsb]
 * 
 * Revision 1.5  89/07/17  10:40:18  rvb
 * 	Olivetti Changes to X79 upto 5/9/89:
 * 
 * Revision 1.4  89/03/09  20:05:20  rpd
 * 	More cleanup.
 * 
 * Revision 1.3  89/02/26  12:36:24  gm0w
 * 	Changes for cleanup.
 * 
 * $EndLog$
 */
 
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)conf.c	7.1 (Berkeley) 6/5/86
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/conf.h>
#include <sys/errno.h>

#include <fullserver.h>
#include <sys/user.h>
#include <uxkern/device.h>
#include <uxkern/device_utils.h>

#if	SER_COMPAT
#define	DEV_FUNNEL_NULL	,FUNNEL_NULL
#else
#define	DEV_FUNNEL_NULL
#endif

int	nulldev();
int	nodev();

int	timeopen();
int	timeclose();
int	timemap();

#ifndef	OSF1_SERVER

#include "hd.h"
#if	NHD > 0
int	hdopen(), hdstrategy(), hdread(), hdwrite(), hddump(), hdioctl(), hdsize();
#else
#define	hdopen		nodev
#define	hdstrategy	nodev
#define	hdread		nodev
#define	hdwrite		nodev
#define	hddump		nodev
#define	hdioctl		nodev
#define	hdsize		nodev
#endif

#include "fd.h"
#if	NFD > 0
int	fdopen(), fdstrategy(), fdread(), fdwrite(), fddump(), fdioctl(), fdsize();
#else
#define	fdopen		nodev
#define	fdstrategy	nodev
#define	fdread		nodev
#define	fdwrite		nodev
#define	fddump		nodev
#define	fdioctl		nodev
#define	fdsize		nodev
#endif

#include "wt.h"
#if	NWT > 0
int	wtopen(), wtstrategy(), wtread(), wtwrite(), wtdump(), wtioctl(), wtsize(), wtclose();
#else
#define	wtopen		nodev
#define	wtclose		nodev
#define	wtread		nodev
#define	wtwrite		nodev
#define	wtioctl		nodev
#define	wtstrategy	nodev
#define	wtdump		nodev
#define	wtsize		nodev
#endif

#include "ln.h"
#if	NLN > 0
int lnopen(), lnclose(), lnioctl(), lnselect();
#else
#define lnopen    nodev
#define lnclose   nodev
#define lnioctl   nodev
#define lnselect  nodev
#endif

#include "ec.h"
#if	NEC > 0
int ecopen(), ecclose(), ecioctl();
#else
#define ecopen    nodev
#define ecclose   nodev
#define ecioctl   nodev
#endif

#endif	/* OSF1_SERVER */

/* Logical Volume Manager pseudo-device */
#include "lv.h"
#if NLV > 0
int lv_nlv = NLV;
#include <lvm/lvmd.h>
int	lv_open(), lv_close();
void	lv_strategy();
int	lv_read(), lv_ioctl(), lv_write();
struct volgrp lv_volgrp[NLV];
#endif

#if NLV > 0
#define	lv_open0	lv_open
#define	lv_close0	lv_close
#define	lv_strategy0	(int (*)())lv_strategy
#define	lv_read0	lv_read
#define	lv_write0	lv_write
#define	lv_ioctl0	lv_ioctl
#define	lv_volgrp0	((struct tty *)&lv_volgrp[0])
#else
#define	lv_open0	nodev
#define	lv_close0	nodev
#define	lv_strategy0	nodev
#define	lv_read0	nodev
#define	lv_write0	nodev
#define	lv_ioctl0	nodev
#define lv_volgrp0	0
#endif

#if NLV > 1
#define	lv_open1	lv_open
#define	lv_close1	lv_close
#define	lv_strategy1	(int (*)())lv_strategy
#define	lv_read1	lv_read
#define	lv_write1	lv_write
#define	lv_ioctl1	lv_ioctl
#define	lv_volgrp1	((struct tty *)&lv_volgrp[1])
#else
#define	lv_open1	nodev
#define	lv_close1	nodev
#define	lv_strategy1	nodev
#define	lv_read1	nodev
#define	lv_write1	nodev
#define	lv_ioctl1	nodev
#define lv_volgrp1	0
#endif

/* This causes a cpp error 
#if NLV > 2
#error Need to add more declarations to conf.c to configure this many LVMs.
#endif
*/

#define blv0_ops lv_open0, lv_close0, lv_strategy0, nulldev, 0, lv_ioctl0
#define blv1_ops lv_open1, lv_close1, lv_strategy1, nulldev, 0, lv_ioctl1

#define MAX_BDEVSW      40
#define MAX_CDEVSW      64

/*
 * Block devices all use the same open/close/strategy routines.
 */
extern int bdev_open(), bdev_close(), bio_strategy(), bdev_dump(), bdev_size(), bdev_ioctl();

#define	bdev_ops bdev_open, bdev_close, bio_strategy, bdev_dump, bdev_size, bdev_ioctl
#define bdev_nop nodev, nodev, nodev, nodev, nodev, nodev

/* IPI start */
/*
 * IPI devices must have contiguous major numbers
 * BLK_IPI_BASE must be defined as the lowest major number
 */
#define BLK_IPI_BASE    23

int bipi_base = BLK_IPI_BASE;
extern int bipi_open();
#define bipi_ops bipi_open, bdev_close, bio_strategy, bdev_dump, bdev_size, bdev_ioctl
/* IPI end */

dswlock_t       bdevlock[MAX_BDEVSW];           /* bdevsw lock structure */

struct bdevsw	bdevsw[MAX_BDEVSW] =
{
/*0*/	{ "", 		0,		bdev_nop DEV_FUNNEL_NULL },
/*1*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*2*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*3*/	{ "rz",		C_BLOCK(16),	bdev_ops DEV_FUNNEL_NULL }, /* ipsc */
/*4*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*5*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*6*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
#ifdef	i860
/*7*/	{ "md",		C_BLOCK(4),	bdev_ops DEV_FUNNEL_NULL }, /* ipsc */
#else
/*7*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
#endif
/*8*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*9*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*10*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*11*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*12*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*13*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*14*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*15*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*16*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
	/* Logical Volume Manager devices - cmajor and bmajor must match */
/*17*/	{ "",		0,		blv0_ops DEV_FUNNEL_NULL }, /* lvm0 */
/*18*/	{ "",		0,		blv1_ops DEV_FUNNEL_NULL }, /* lvm1 */
/*19*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*20*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*21*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/*22*/	{ "",		0,		bdev_nop DEV_FUNNEL_NULL },
/* IPI start */
/* IPI magnetic disk devices */
/*23*/	{ "imd",	C_BLOCK(16),	bipi_ops DEV_FUNNEL_NULL }, /* IPI */
/*24*/	{ "imd",	C_BLOCK(16),	bipi_ops DEV_FUNNEL_NULL }, /* IPI */
/*25*/	{ "imd",	C_BLOCK(16),	bipi_ops DEV_FUNNEL_NULL }, /* IPI */
/*26*/  { "imd",	C_BLOCK(16),	bipi_ops DEV_FUNNEL_NULL }, /* IPI */
/*27*/  { "imd",	C_BLOCK(16),	bipi_ops DEV_FUNNEL_NULL }, /* IPI */
/*28*/  { "imd",	C_BLOCK(16),	bipi_ops DEV_FUNNEL_NULL }, /* IPI */
/*29*/  { "imd",	C_BLOCK(16),	bipi_ops DEV_FUNNEL_NULL }, /* IPI */
/*30*/  { "imd",	C_BLOCK(16),	bipi_ops DEV_FUNNEL_NULL }, /* IPI */
/* IPI end */
};

int	nblkdev = sizeof (bdevsw) / sizeof (bdevsw[0]);
extern int	seltrue();

extern int      char_open(), char_close(), char_read(), char_write();
extern int      char_ioctl(), char_select();
#define char_ops \
        char_open, char_close, char_read, char_write,  char_ioctl, \
        nulldev,   nulldev,    0,         char_select, nulldev

#if	FULLSERVER
extern int      disk_open(), disk_close(), disk_read(), disk_write();
extern int	disk_ioctl();
int		isa_disk_ioctl();	/* forward */
#else	/* FULLSERVER */
#define disk_open       NULL
#define disk_close      NULL
#define disk_read       NULL
#define disk_write      NULL
#define disk_ioctl      NULL
#define isa_disk_ioctl  NULL
#endif	/* FULLSERVER */
#define disk_ops \
	disk_open, disk_close, disk_read, disk_write, disk_ioctl, \
	nodev,     nulldev,    0,         seltrue,     nodev

#if	FULLSERVER
extern int	tape_open(), tape_close(), tape_read(), tape_write();
extern int	tape_ioctl();
#else	/* FULLSERVER */
#define tape_open       NULL
#define tape_close      NULL
#define tape_read       NULL
#define tape_write      NULL
#define tape_ioctl      NULL
#endif	/* FULLSERVER */
#define	tape_ops \
	tape_open, tape_close, tape_read, tape_write, tape_ioctl, \
	nulldev,   nulldev,    0,	  seltrue,     nulldev

/*
 * Support for compression tapes
 */
#if	FULLSERVER
extern int	ctape_open();
#else	/* FULLSERVER */
#define ctape_open       NULL
#endif	/* FULLSERVER */
#define	ctape_ops \
	ctape_open, tape_close, tape_read, tape_write, tape_ioctl, \
	nulldev,   nulldev,    0,	  seltrue,     nulldev
/*
 * Support for Media Changer devices
 */
#if	FULLSERVER
#define changer_open  disk_open
#define changer_close disk_close
#define changer_ioctl disk_ioctl
#else /* FULLSERVER */
#define changer_open  NULL
#define changer_close NULL
#define changer_ioctl NULL
#endif	/* FULLSERVER */
#define       changer_ops \
      changer_open, changer_close, nodev, nodev, changer_ioctl, \
      nulldev,   nulldev,    0,         seltrue,     nulldev

extern int	tty_open(), tty_close(), tty_read(), tty_write();
extern int	tty_ioctl(), ttselect(), tty_stop();
extern struct tty tty_tp[];
#define	tty_ops	\
	tty_open,  tty_close,  tty_read,  tty_write,   tty_ioctl, \
	tty_stop,  nulldev,    tty_tp,    ttselect,    nodev

extern int	cons_open(), cons_write(), cons_ioctl();
extern mach_port_t	cons_port();

struct tty	cons_tty;
struct tty	*cons_tp = &cons_tty;
#define	console_ops	\
	cons_open, tty_close,  tty_read,  cons_write,  cons_ioctl, \
	tty_stop,  nulldev,    &cons_tty, ttselect,    nodev

extern int      syopen(), syread(), sywrite(), syioctl(), syselect();
#define sy_ops \
        syopen,   nulldev,   syread,   sywrite,   syioctl, \
        nulldev,  nulldev,   0,        syselect,  nodev

extern int	logopen(), logclose(), logread(), logioctl(), logselect();
#define	log_ops \
	logopen,  logclose,  logread,  nodev,     logioctl, \
	nulldev,  nodev,     0,        logselect, nodev

extern int      mmopen(), mmread(), mmwrite();
#define mm_ops \
        mmopen,   nulldev,   mmread,   mmwrite,   nodev, \
        nulldev,  nulldev,   0,        seltrue,   nodev

extern int	hippi_open(), hippi_close(), hippi_read(), hippi_write();
extern int	hippi_c_ioctl();
#define hippi_ops \
	hippi_open, hippi_close, hippi_read, hippi_write, hippi_c_ioctl, \
	nodev,     nulldev,    0,         seltrue,     nodev

#include "pty.h"
#if     NPTY > 0
extern int      ptsopen(), ptsclose(), ptsread(), ptswrite();
extern int      ptyioctl(), ptsstop();
extern struct   tty pt_tty[];
extern int      ptcopen(), ptcclose(), ptcread(), ptcwrite();
extern int      ptcselect();
#else
#define ptsopen         nodev
#define ptsclose        nodev
#define ptsread         nodev
#define ptswrite        nodev
#define ptcopen         nodev
#define ptcclose        nodev
#define ptcread         nodev
#define ptcwrite        nodev
#define ptyioctl        nodev
#define pt_tty          0
#define ptcselect       nodev
#define ptsstop         nodev
#endif  NPTY > 0

#define pts_ops \
        ptsopen,  ptsclose,    ptsread,   ptswrite,    ptyioctl, \
        ptsstop,   nodev,      pt_tty,	  ttselect,    nodev

#define ptc_ops \
        ptcopen,  ptcclose,    ptcread,   ptcwrite,    ptyioctl, \
        nulldev,   nodev,      pt_tty,	  ptcselect,   nodev

extern mach_port_t	char_port();
#define time_ops \
	char_open,char_close,  nulldev,   nulldev,     nulldev, \
	nulldev,  nulldev,     0,         seltrue,     nodev

#define no_ops \
        nodev,     nodev,      nodev,     nodev,       nodev, \
        nodev,     nodev,      0,         nodev,       nodev


#define lv0_ops \
        lv_open0,  lv_close0,  lv_read0,      lv_write0,  lv_ioctl0, \
        nodev,     nodev,      lv_volgrp0,    nodev,      nodev

#define lv1_ops \
        lv_open1,  lv_close1,  lv_read1,      lv_write1,  lv_ioctl1, \
        nodev,     nodev,      lv_volgrp1,    nodev,      nodev

/* IPI start */
#if     FULLSERVER
extern int      cipi_open();
#else   /* FULLSERVER */
#define cipi_open       NULL
#endif  /* FULLSERVER */
/*
 * IPI devices must have contiguous major numbers
 * RAW_IPI_BASE must be defined as the lowest major number
 */
#define RAW_IPI_BASE    25

int cipi_base = RAW_IPI_BASE;
#define cipi_ops \
        cipi_open, disk_close, disk_read, disk_write, disk_ioctl, \
        nodev,     nulldev,    0,         seltrue,     nodev
/* IPI end */


struct cdevsw	cdevsw[MAX_CDEVSW] =
{
/* 0*/ { "com",     0,       tty_ops         DEV_FUNNEL_NULL}, /* tty00 isa  */
/* 1*/ { "console", 0,       console_ops     DEV_FUNNEL_NULL}, /* console    */
/* 2*/ { "",        0,       sy_ops          DEV_FUNNEL_NULL}, /* tty        */
/* 3*/ { "",        0,       mm_ops          DEV_FUNNEL_NULL}, /* kmem, null */
/* 4*/ { "",	    0,       no_ops          DEV_FUNNEL_NULL}, 
/* 5*/ { "",        0,       no_ops          DEV_FUNNEL_NULL},
/* 6*/ { "jz",      0,       changer_ops     DEV_FUNNEL_NULL},
/* 7*/ { "tz",      0,       ctape_ops       DEV_FUNNEL_NULL}, 
/* 8*/ { "tz",      0,       tape_ops        DEV_FUNNEL_NULL}, 
/* 9*/ { "",        0,       pts_ops         DEV_FUNNEL_NULL}, /* ttyp00     */
/*10*/ { "",        0,       ptc_ops         DEV_FUNNEL_NULL}, /* ptyp00     */
/*11*/ { "",        0,       no_ops          DEV_FUNNEL_NULL},
/*12*/ { "",        0,       no_ops          DEV_FUNNEL_NULL},
/*13*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, /*            */
/*14*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, /* time       */
/*15*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, /* ec (?)     */
/*16*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, 
	/* Logical Volume Manager devices - cmajor and bmajor must match */
/*17*/ { "",        0,       lv0_ops         DEV_FUNNEL_NULL}, /* lvm0       */
/*18*/ { "",        0,       lv1_ops         DEV_FUNNEL_NULL}, /* lvm1       */
/*19*/ { "",        0,       log_ops         DEV_FUNNEL_NULL}, /* klog       */
/*20*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, /* <UNUSED>   */
/*21*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, /* <UNUSED>   */
#ifdef	i860
/*22*/ { "md",  C_BLOCK(4), disk_ops        DEV_FUNNEL_NULL}, /* rmd00 ipsc   */
#else
/*22*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, /* <UNUSED>   */
#endif
/*23*/ { "rz",  C_BLOCK(16), disk_ops        DEV_FUNNEL_NULL}, /* rsd00 ipsc */
/*24*/ { "rhippi",   0,      hippi_ops       DEV_FUNNEL_NULL}, /* HIPPI */
/* IPI start */
/* IPI master devices */
/*25*/ { "ipm",     0,       cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*26*/ { "ipm",     0,       cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*27*/ { "ipm",     0,       cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*28*/ { "ipm",     0,       cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*29*/ { "ipm",     0,       cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*30*/ { "ipm",     0,       cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*31*/ { "ipm",     0,       cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*32*/ { "ipm",     0,       cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
	/* IPI magnetic disk devices */
/*33*/ { "imd", C_BLOCK(16), cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*34*/ { "imd", C_BLOCK(16), cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*35*/ { "imd", C_BLOCK(16), cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*36*/ { "imd", C_BLOCK(16), cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*37*/ { "imd", C_BLOCK(16), cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*38*/ { "imd", C_BLOCK(16), cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*39*/ { "imd", C_BLOCK(16), cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/*40*/ { "imd", C_BLOCK(16), cipi_ops        DEV_FUNNEL_NULL}, /* IPI */
/* IPI end */
};

int	nchrdev = sizeof(cdevsw)/sizeof(cdevsw[0]);
dswlock_t       cdevlock[MAX_CDEVSW];           /* OSF1_SERVER cdevsw lock structure */

dev_t	sydev = makedev(2, 0);

/*
 * Conjure up a name string for funny devices (not all minors have
 * the same name).
 */
int
check_dev(dev, str)
	dev_t	dev;
	char	*str;
{
	return 0;
}

#define PARAGON860   /* XXX Hack */
#include <device/tape_status.h>
#undef PARAGON860

/*
 * More special code (but not quite machdep) to
 * handle the rewind/norewind business.  Also,
 * a good place to add density/speed selection
 * based on the tape's minor.
 */
#define TAPE_NO_REWIND_ON_CLOSE    0x8
#define	TAPE_UNIT(dev)		((dev) & ~TAPE_NO_REWIND_ON_CLOSE)
#define	TAPE_REWINDS(dev)	(((dev)&TAPE_NO_REWIND_ON_CLOSE) == 0)

#if	FULLSERVER
/* Uncompressed tape open */
tape_open(dev, mode, flag, newdev, node)
dev_t dev;
int   mode;
int flag;
int  *newdev;
node_t  node;
{
	devinfo_t	*devinfo;

	int ret = disk_open(TAPE_UNIT(dev), mode, flag, newdev, node);
        if (ret == 0) {
                devinfo = (devinfo_t *) dev_lookup(TAPE_UNIT(dev), node, 
						   CHAR_DEV);
                if (devinfo == NULL)
                  return (ENXIO);	/* shouldn't happen */
                devinfo->return_short_reads = TRUE;
        }

	if (ret == 0) {
		kern_return_t	   rc;
		struct tape_status ts;

		/*
		 * Always make the device_set_status call.
		 * Clearing ts disables write data compression.
		 * Data decompression is always performed on reads
		 * provided the tape drive supports it.
		 */
		bzero(&ts, sizeof(ts));

		if (TAPE_REWINDS(dev)) ts.flags = TAPE_FLG_REWIND;


		rc = device_set_status(
			devinfo->devport,
			TAPE_STATUS,
			&ts,
			TAPE_STATUS_COUNT);

		if (rc != 0) {
			disk_close(TAPE_UNIT(dev), node, flag);
			ret = dev_error_to_errno(rc);
		}
	}
	return ret;
}

/* Compressed tape open */
ctape_open(dev, mode, flag, newdev, node)
dev_t dev;
int   mode;
int flag;
int  *newdev;
node_t  node;
{
	devinfo_t	*devinfo;

	int ret = disk_open(TAPE_UNIT(dev), mode, flag, newdev, node);
        if (ret == 0) {
                devinfo = (devinfo_t *) dev_lookup(TAPE_UNIT(dev), node, 
						   CHAR_DEV);
                if (devinfo == NULL)
                  return (ENXIO);	/* shouldn't happen */
                devinfo->return_short_reads = TRUE;
        }

	if (ret == 0) {
		kern_return_t	   rc;
		struct tape_status ts;

		/*
		 * Always make the device_set_status call.
		 * Set the flag to enable write data compression.
		 * Data decompression is always performed on reads
		 * provided the tape drive supports it.
		 */
		bzero(&ts, sizeof(ts));

		if (TAPE_REWINDS(dev)) ts.flags = TAPE_FLG_REWIND;

		ts.flags |= TAPE_FLG_WRITE_COMPRESSION;

		rc = device_set_status(
			devinfo->devport,
			TAPE_STATUS,
			&ts,
			TAPE_STATUS_COUNT);

		if (rc != 0) {
			disk_close(TAPE_UNIT(dev), node, flag);
			ret = dev_error_to_errno(rc);
		}

	}

	return ret;
}

tape_close(dev, node, flag)
dev_t dev;
node_t node;
{
	return(disk_close(TAPE_UNIT(dev), node, flag));
}

tape_read(dev, node, uio)
dev_t dev;
node_t node;
{
	return(disk_read(TAPE_UNIT(dev), node, uio));
}

tape_write(dev, node, uio)
dev_t dev;
node_t node;
{
	return(disk_write(TAPE_UNIT(dev), node, uio));
}

tape_ioctl(dev, node, cmd, data, flag)
dev_t dev;
node_t node;
{
	return(disk_ioctl(TAPE_UNIT(dev), node, cmd, data, flag));
}
#endif	/* FULLSERVER */
