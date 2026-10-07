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
 * Revision 1.5  1994/11/18  20:30:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/01/12  17:47:42  jlitvin
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
 * Revision 1.3  1993/07/14  17:55:29  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:14:46  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:16  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:19:28  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:12:44  cfj
 * Bump major revision number.
 *
 * Revision 2.8  93/01/29  13:58:35  durriya
 * 	set return_short_reads in devinfo struct to TRUE in tape_open  (durriya)
 * 
 * Revision 2.7  93/01/08  14:29:47  durriya
 * 	add node # as arg to tape_read, write, ioctl, close, isa_disk_ioctl,
 * 	kbd_ioctl, iopl_close. fix args to tape_open                 (durriya)
 * 
 * Revision 2.6  1992/07/14  14:56:49  rabii
 * 	dev_rawinfo_t -> devinfo_t
 * 	[92/07/10            roy]
 *
 * Revision 2.5  92/03/09  12:03:58  durriya
 * 	[Revision 3.15  92/02/28  17:59:48  barbou]
 * 	Fixed Sequent's disk cdevsw entries, to allow vtoc to work.
 * 
 * 	[Revision 3.14  92/02/12  10:58:29  bernadat]
 * 	Added tape support
 * 
 * 	[Revision 3.13  91/12/19  15:59:02  bernadat]
 * 	Added support for mapping of the vga for PC/ATs (/dev/iopl)
 * 
 * Revision 2.4  91/12/16  19:07:42  roy
 * 	91/11/26  15:33:19  sp
 * 	Upgrade to 1.0.3
 * 
 * 	91/10/17  18:33:23  barbou
 * 	LVM configuration.
 * 
 * Revision 2.3  91/10/14  12:14:42  sjs
 * 	91/09/27  12:00:24  emcmanus
 * 	Added clone device, with major number 30.  This is not the same
 * 	as monolithic OSF/1, which uses number 24.
 * 
 * Revision 2.2  91/08/31  13:33:07  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.9  91/08/28  10:56:10  barbou
 * Upgrade to OSF/1.0.2.
 * 
 * Revision 1.7.3.2  91/08/26  16:34:57  garyf
 *	non-existent entries should get nodev, not nulldev
 *	[91/08/23  14:15:19  garyf]
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
#include <cputypes.h>
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
#include <streams.h>

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

#if NLV > 2
#error Need to add more declarations to conf.c to configure this many LVMs.
#endif

#define blv0_ops lv_open0, lv_close0, lv_strategy0, nulldev, 0, lv_ioctl0
#define blv1_ops lv_open1, lv_close1, lv_strategy1, nulldev, 0, lv_ioctl1

#define MAX_BDEVSW      32
#define MAX_CDEVSW      64

/*
 * Block devices all use the same open/close/strategy routines.
 */
extern int bdev_open(), bdev_close(), bio_strategy(), bdev_dump(), bdev_size(), bdev_ioctl();

#define	bdev_ops bdev_open, bdev_close, bio_strategy, bdev_dump, bdev_size, bdev_ioctl
#define bdev_nop nodev, nodev, nodev, nodev, nodev, nodev

dswlock_t       bdevlock[MAX_BDEVSW];           /* bdevsw lock structure */

struct bdevsw	bdevsw[MAX_BDEVSW] =
{
/*0*/	{ "hd", 	C_BLOCK(16),	bdev_ops DEV_FUNNEL_NULL }, /* isa */
/*1*/	{ "fd",		C_BLOCK(64),	bdev_ops DEV_FUNNEL_NULL }, /* isa */
/*2*/	{ "wt",		B_TAPE,		bdev_ops DEV_FUNNEL_NULL }, /* isa */
/*3*/	{ "sd",		C_BLOCK(16),	bdev_ops DEV_FUNNEL_NULL }, /* ipsc */
			/* but treat as C_BLOCK(8) for Sequent */
/*4*/	{ "zd",		C_BLOCK(8),	bdev_ops DEV_FUNNEL_NULL }, /* sqt */
/*5*/	{ "ts",		B_TAPE,		bdev_ops DEV_FUNNEL_NULL }, /* sqt */
/*6*/	{ "xt",		B_TAPE,		bdev_ops DEV_FUNNEL_NULL },
/*7*/	{ "st",	     B_TAPE|C_BLOCK(1), bdev_ops DEV_FUNNEL_NULL }, /* isa */
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
/*18*/	{ "",		0,		blv1_ops DEV_FUNNEL_NULL }  /* lvm1 */

};

int	nblkdev = sizeof (bdevsw) / sizeof (bdevsw[0]);
extern int	seltrue();

extern int      char_open(), char_close(), char_read(), char_write();
extern int      char_ioctl(), char_select();
#define char_ops \
        char_open, char_close, char_read, char_write,  char_ioctl, \
        nulldev,   nulldev,    0,         char_select, nulldev

extern int      disk_open(), disk_close(), disk_read(), disk_write();
extern int	disk_ioctl();
int		isa_disk_ioctl();	/* forward */
#define disk_ops \
	disk_open, disk_close, disk_read, disk_write, disk_ioctl, \
	nodev,     nulldev,    0,         seltrue,     nodev

#define	isa_disk_ops \
	disk_open, disk_close, disk_read, disk_write, isa_disk_ioctl, \
	nulldev,   nulldev,    0,         seltrue,     nulldev

extern int	tape_open(), tape_close(), tape_read(), tape_write();
extern int	tape_ioctl();
#define	tape_ops \
	tape_open, tape_close, tape_read, tape_write, tape_ioctl, \
	nulldev,   nulldev,    0,	  seltrue,     nulldev

extern int	tty_open(), tty_close(), tty_read(), tty_write();
extern int	tty_ioctl(), ttselect(), tty_stop();
extern struct tty tty_tp[];
#define	tty_ops	\
	tty_open,  tty_close,  tty_read,  tty_write,   tty_ioctl, \
	tty_stop,  nulldev,    tty_tp,    ttselect,    nodev

extern int	cons_open(), cons_write(), cons_ioctl();

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

int	iopl_open(), iopl_close(), iopl_mmap();
mach_port_t	iopl_port();

#define	iopl_ops \
	iopl_open,  iopl_close,  nodev,     nodev,       nodev, \
	nodev,     nulldev,    0,         nodev,       iopl_mmap

#define lv0_ops \
	lv_open0,  lv_close0,  lv_read0,      lv_write0,  lv_ioctl0, \
	nodev,     nodev,      lv_volgrp0,    nodev,      nodev

#define lv1_ops \
	lv_open1,  lv_close1,  lv_read1,      lv_write1,  lv_ioctl1, \
	nodev,     nodev,      lv_volgrp1,    nodev,      nodev

/*
 * Special IOCTL handling for keyboard.
 */
int	kbd_ioctl();	/* forward */

#define	kbd_ops \
	char_open, char_close, char_read, char_write, kbd_ioctl, \
	nodev,     nodev,      0,         char_select, nodev

#if STREAMS
int	clone_open();
#define clone_ops \
	clone_open, nodev, nodev, nodev, nodev, \
	nodev,	    nodev, 0,     nodev, nodev
#else
#define clone_ops no_ops
#endif

struct cdevsw	cdevsw[MAX_CDEVSW] =
{
/* 0*/ { "com",     0,       tty_ops         DEV_FUNNEL_NULL}, /* tty00 isa  */
/* 1*/ { "console", 0,       console_ops     DEV_FUNNEL_NULL}, /* console    */
/* 2*/ { "",        0,       sy_ops          DEV_FUNNEL_NULL}, /* tty        */
/* 3*/ { "",        0,       mm_ops          DEV_FUNNEL_NULL}, /* kmem, null */
/* 4*/ { "hd",  C_BLOCK(16), isa_disk_ops    DEV_FUNNEL_NULL}, /* rhd00 isa  */
/* 5*/ { "fd",  C_BLOCK(64), isa_disk_ops    DEV_FUNNEL_NULL}, /* isa        */
/* 6*/ { "wt",      0,       char_ops        DEV_FUNNEL_NULL}, /* isa        */
/* 7*/ { "blit",    0,       char_ops        DEV_FUNNEL_NULL}, /* isa        */
/* 8*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, /* ln (?)     */
/* 9*/ { "",        0,       pts_ops         DEV_FUNNEL_NULL}, /* ttyp00     */
/*10*/ { "",        0,       ptc_ops         DEV_FUNNEL_NULL}, /* ptyp00     */
/*11*/ { "kbd",     0,       kbd_ops         DEV_FUNNEL_NULL}, /* isa kbd    */
/*12*/ { "mouse",   0,       char_ops        DEV_FUNNEL_NULL}, /* isa mouse  */
/*13*/ { "qd",      0,       tty_ops         DEV_FUNNEL_NULL}, /*            */
/*14*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, /* time       */
/*15*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, /* ec (?)     */
/*16*/ { "iopl",    0,       iopl_ops        DEV_FUNNEL_NULL, iopl_port }, /* isa        */
       /* Logical Volume Manager devices - cmajor and bmajor must match */
/*17*/ { "",        0,       lv0_ops         DEV_FUNNEL_NULL}, /* lvm0       */
/*18*/ { "",        0,       lv1_ops         DEV_FUNNEL_NULL}, /* lvm1       */
/*19*/ { "",        0,       log_ops         DEV_FUNNEL_NULL}, /* klog       */
/*20*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, /* <UNUSED>   */
/*21*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, /* <UNUSED>   */
/*22*/ { "",        0,       no_ops          DEV_FUNNEL_NULL}, /* <UNUSED>   */
/*23*/ { "sd",  C_BLOCK(16), isa_disk_ops    DEV_FUNNEL_NULL}, /* rsd00 ipsc */
/*24*/ { "zd",  C_BLOCK(8),  isa_disk_ops    DEV_FUNNEL_NULL}, /* sqt        */
/*25*/ { "ts",      0,       disk_ops        DEV_FUNNEL_NULL}, /* sqt        */
/*26*/ { "xt",      0,       disk_ops        DEV_FUNNEL_NULL}, /* sqt        */
/*27*/ { "co",      0,       tty_ops         DEV_FUNNEL_NULL}, /* sqt        */
/*28*/ { "st",      0,       tape_ops        DEV_FUNNEL_NULL}, /* sqt        */
/*29*/ { "",        0,       no_ops          DEV_FUNNEL_NULL},
/*30*/ { "clone",   0,	     clone_ops	     DEV_FUNNEL_NULL}

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
	printf("check_dev: ---\n"); 
	return 0;
}

/*
 * IOPL device.  Must give send rights to port to task.
 */
mach_port_t	iopl_device_port = MACH_PORT_NULL;

int
iopl_open(dev, flag)
	dev_t	dev;
	int	flag;
{
	kern_return_t	rc;
	mach_port_t	name;

	rc = device_open(device_server_port,
			 0,
			 "iopl0",
			 &iopl_device_port);
	if (rc != D_SUCCESS)
	    return (dev_error_to_errno(rc));

	/*
	 * Give send rights to task to let it access IO
	 */
	name = 0x10000;	/* XXX */
	do {
	    rc = mach_port_insert_right(u.u_procp->p_task,
					name++,
					iopl_device_port,
					MACH_MSG_TYPE_COPY_SEND);
	} while (rc == KERN_NAME_EXISTS);
	if (rc != KERN_SUCCESS) {
	    (void) device_close(iopl_device_port);
	    (void) mach_port_deallocate(mach_task_self(), iopl_device_port);
	    iopl_device_port = MACH_PORT_NULL;
	    return (EACCES);
	}

	return (0);
}

int
#ifdef OSF1_ADFS
iopl_close(dev, node, flag)
#else
iopl_close(dev, flag)
#endif
	dev_t	dev;
#ifdef OSF1_ADFS
        node_t  node;
#endif        
	int	flag;
{
	(void) device_close(iopl_device_port);
	(void) mach_port_deallocate(mach_task_self(), iopl_device_port);
	iopl_device_port = MACH_PORT_NULL;
	return (0);
}

mach_port_t
iopl_port(dev)
	dev_t	dev;
{
	return (iopl_device_port);
}

/*ARGSUSED*/
int
iopl_mmap(dev, off, prot)
dev_t dev;
off_t off;
int prot;
{
	if ((u_int) off > 0x60000)
		return(-1);
	return(0);
}


/*
 * Keyboard IOCTL
 */
int
#ifdef OSF1_ADFS
kbd_ioctl(dev, node, cmd, data, flag)
#else
kbd_ioctl(dev, cmd, data, flag)
#endif
	dev_t	dev;
#ifdef OSF1_ADFS
        node_t  node;
#endif
	int	cmd;
	caddr_t	data;
	int	flag;
{
	int	error;

	struct X_kdb {
	    u_int *ptr;
	    u_int size;
	};

#define K_X_KDB_ENTER	_IOW('K', 16, struct X_kdb)
#define K_X_KDB_EXIT	_IOW('K', 17, struct X_kdb)

	if ((cmd == K_X_KDB_ENTER) ||
	    (cmd == K_X_KDB_EXIT))
	{
	    u_int X_kdb_buffer[512];
	    struct X_kdb *X_kdb = (struct X_kdb *) data;

	    /* make sure that copyin will do the right thing */
	    if (u.u_reply_msg == 0)
		panic("X_K_KDB ioctl");

	    if (X_kdb->size > sizeof X_kdb_buffer)
		return (ENOENT);

	    if (copyin(X_kdb->ptr, X_kdb_buffer, X_kdb->size))
		return (EFAULT);

#ifdef OSF1_ADFS
	    error = device_set_status(char_port(dev, node), cmd,
#else
	    error = device_set_status(char_port(dev), cmd,
#endif
				      X_kdb_buffer, X_kdb->size>>2);
	    if (error)
		return (ENOTTY);

	    return (0);
	}
#ifdef OSF1_ADFS        
	return (char_ioctl(dev, node, cmd, data, flag));
#else
	return (char_ioctl(dev, cmd, data, flag));
#endif        
}

/*
 * ISA disk IOCTL
 */
#undef	p_flag			/* conflict from sys/{proc,user}.h */
#include <i386at/disk.h>

extern mach_port_t	disk_port();

int
#ifdef OSF1_ADFS        
isa_disk_ioctl(dev, node, cmd, data, flag)
#else
isa_disk_ioctl(dev, cmd, data, flag)
#endif
	dev_t	dev;
#ifdef OSF1_ADFS        
        node_t  node;
#endif
	int	cmd;
	caddr_t	data;
	int	flag;
{

#ifdef OSF1_ADFS
	mach_port_t	device_port = disk_port(dev, node);
#else
	mach_port_t	device_port = disk_port(dev);
#endif
	unsigned int	count;
	register int	error;

	switch (cmd) {
	    case V_RDABS:
	    {
		char buf[512];

		error = device_set_status(device_port,
					  V_ABS,
					  &((struct absio *)data)->abs_sec,
					  1);
		if (error)
		    return (dev_error_to_errno(error));
		count = 512/sizeof(int);
		error = device_get_status(device_port,
					  cmd,
					  buf,
					  &count);

		if (error)
		    return (dev_error_to_errno(error));
		if (copyout(buf, ((struct absio *)data)->abs_buf, 512))
		    return (EFAULT);
		break;
	    }

	    case V_VERIFY:
	    {
		union vfy_io *vfy_io = (union vfy_io *) data;
		int vfy[2] = {	vfy_io->vfy_in.abs_sec,
				vfy_io->vfy_in.num_sec};

		error = device_set_status(device_port,
					  V_ABS,
					  vfy,
					  2);
		if (error)
		    return (dev_error_to_errno(error));
		count = sizeof (int)/sizeof(int);
		error = device_get_status(device_port,
					  cmd,
					  vfy,
					  &count);

		vfy_io->vfy_out.err_code = vfy[0];
		if (error)
		    return (dev_error_to_errno(error));
		break;
	    }

	    case V_WRABS:
	    {
		char buf[512];

		error = device_set_status(device_port,
					  V_ABS,
					  &((struct absio *)data)->abs_sec,
					  1);
		if (error)
		    return (dev_error_to_errno(error));
		if (copyin(((struct absio *)data)->abs_buf, buf, 512))
		    return (EFAULT);
		count = 512/sizeof(int);
		error = device_set_status(device_port,
					  cmd,
					  buf,
					  count);
		if (error)
		    return (dev_error_to_errno(error));
		break;
	    }

	    default:
	    {
		return (disk_ioctl(dev, cmd, data, flag));
	    }
	}
	return (0);
}

#include <device/tape_status.h>

/*
 * More special code (but not quite machdep) to
 * handle the rewind/norewind business.  Also,
 * a good place to add density/speed selection
 * based on the tape's minor.
 */
#if	!AT386
#define	TAPE_UNIT(dev)		((dev)&(~0xff))|(((dev)&0xe0)>>3)|((dev)&0x3)
#define	TAPE_REWINDS(dev)	(((dev)&0x1c)==0)||(((dev)&0x1c)==8)
/* NOTE: the above defs (Ultrix-compatible) are good for 
   the scsi tape "st".  Donno for other tapes */   
#else	AT386
#define	TAPE_UNIT(dev)		((dev)&(~0x8)) /* allow 8 units */
#define	TAPE_REWINDS(dev)	(((dev)&0x8)==0)
#endif	AT386

tape_open(dev, mode, flag, newdev, node)
dev_t dev;
int   mode;
int flag;
int  *newdev;
node_t  node;
{
	devinfo_t	*devinfo;

	int ret = disk_open(TAPE_UNIT(dev), mode,flag, newdev, node);
        if (ret == 0) {
                devinfo = (devinfo_t *) dev_lookup(dev, node, CHAR_DEV);
                if (devinfo == NULL)
                  return (ENXIO);	/* shouldn't happen */
                devinfo->return_short_reads = TRUE;
        }

	if ((ret == 0) && (TAPE_REWINDS(dev))) {
		struct tape_status ts;
		bzero(&ts, sizeof(ts));
		ts.flags = TAPE_FLG_REWIND;

		(void) device_set_status(
			devinfo->devport,
			TAPE_STATUS,
			&ts,
			TAPE_STATUS_COUNT);
	}
	return ret;
}

tape_close(dev, node, flag)
dev_t       dev;
node_t      node;
{
	return(disk_close(TAPE_UNIT(dev), node, flag));
}

tape_read(dev, node, uio)
dev_t       dev;
node_t      node;
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
dev_t        dev;
node_t       node;
{
	return(disk_ioctl(TAPE_UNIT(dev), node, cmd, data, flag));
}
