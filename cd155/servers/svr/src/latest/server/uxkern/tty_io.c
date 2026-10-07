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
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: tty_io.c,v $
 * Revision 1.7  1995/03/13  17:11:57  stans
 *  Lint picking.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12675
 *  Testing:WW10 sats
 *
 * Revision 1.6  1994/11/18  20:49:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/01/12  17:45:56  jlitvin
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
 * Revision 1.4  1993/07/14  18:44:51  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:06:34  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:33:43  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:53:56  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.14  1993/04/29  13:51:37  klh
 * 	Revision 2.13  93/01/08  14:35:19  durriya
 * 		add node as arg to tty_close, tty_read, tty_write, tty_ioctl   (durriya)
 *
 * Revision 2.13  92/06/05  15:45:08  roman
 * Get rid of bogus #define at start of file (somehow creeped into
 * 	v0.89 distribution from OSF/1 AD).
 * 
 * Revision 2.12  92/05/12  00:05:33  loverso
 * 	Fixed "if (error = D_IO_ERROR) to (xx == yy)" (srl)
 * 
 * Revision 2.11  92/05/01  10:04:41  rabii
 * 	Modified to call device open on the proper node directly (rabii)
 * 
 * 	Grenoble 3.6 merge by srl. Also, added check to ttwakeup in 
 * 	case of input parity error and PARMRK (srl)
 * 
 * 	92/03/31  15:41:28  emcmanus
 * 	Correct behaviour on carrier loss: on reads, don't issue another read
 * 	request (which will immediately fail); on writes, flush the output
 * 	queue since further writes will also fail.  Report Mach return codes in
 * 	decimal since they are usually defined that way.
 * 
 * 	92/03/23  18:04:50  condict
 * 	Allow NCPUS == 1 in the server, to compile optimally for a uni-processor.
 * 
 * 	92/03/06  17:33:53  emcmanus
 * 	Template for correct XON and XOFF sending; these should be sent even if
 * 	output has been disabled.  Not yet implemented.
 * 	Restart output explicitly when we were XOFFed and we receive an XON.
 * 	Relying on the next output request to do this is not correct, because
 * 	there might not be one and there could be characters in the microkernel
 * 	waiting to be transmitted.
 * 	Add missing printf parameter in check_tty_return.
 * 	D_IO_ERROR in tty_read_reply indicates loss of carrier; behave
 * 	appropriately.
 * 
 * Revision 2.10  92/03/09  13:55:59  durriya
 * 	92/02/28  20:28:52  emcmanus
 * 	Check for hardware tab support incorrectly used XTABS instead of 
 * 	TF_XTABS.  Only print the "unable to clear TF_HUPCLS" message once.
 * 	Unify return-status checks with new check_tty_return() function.
 * 
 * 	92/02/19  16:23:29  emcmanus
 * 	Big changes, principally to reflect the fact that the master tty
 * 	configuration is the SYSV-style information, with BSD ioctls being
 * 	emulated.  Therefore, after an ioctl, SYSV state should be decoded 
 * 	from the tty structure and communicated to the microkernel.
 * 
 * 	91/12/27  17:26:21  jose
 * 	Changed interface to reply_hash_enter for port aliasing
 * 
 * 	91/12/20  17:57:51  barbou
 * 	Check the device_xxx() routines' return value.
 * 	Fix for bug #64: avoid sending Mach device requests for second console.
 * 
 * 	91/12/13  13:04:27  sp
 * 	Remove CMUCS conditionals
 * 
 * Revision 2.9  91/12/17  10:25:16  roy
 * 	91/12/13  13:04:27  sp
 * 	Remove CMUCS conditionals
 * 
 * 	91/11/15  16:27:59  bernadat
 * 	Fixed the console alias functionality (needed for Z1000)
 * 
 * 	91/10/17  18:34:20  barbou
 * 	Deleted include of "cmucs.h" (defunct option).
 * 
 * Revision 2.8  91/11/25  16:15:03  rabii
 * 	Added remote devices
 * 
 * Revision 2.7  91/10/17  10:08:34  sjs
 * fixed RCS comments
 * 
 * Revision 2.6  91/10/14  13:24:47  sjs
 * 	91/10/01  14:13:09  condict
 * 	Protect tty ops with a TTY_LOCK instead of spl's, which are no-ops now.
 * 
 * Revision 2.5  91/10/04  15:31:01  chrisp
 * Add Locus copyright.
 * 
 * Revision 2.4  91/09/17  11:06:33  sjs
 * Check in
 * 
 * Revision 2.3  91/09/17  09:09:09  sjs
 * integrate Locus changes	roman
 * Change tty structure to use pgrp and session ids rather than 
 * structures.
 * 
 * Revision 2.2  91/08/31  14:29:00  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.10  91/08/27  15:40:20  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.9  91/07/29  16:34:11  barbou
 * Modified to decide at run-time if running as a second server.
 * Added missing interrupt_exit(SPLTTY).
 * 
 * Revision 3.8  91/07/04  18:09:00  barbou
 * Added tty locking. Modified the way of sleeping.
 * Better handling to device error from Mach (simulate a carrier loss).
 * 
 * Revision 3.7  91/07/02  14:49:57  condict
 * Formatting change.
 * 
 * Revision 3.6  91/06/25  17:14:36  condict
 * Fix initialization/allocation of tty struct for comm ttys;
 * Fix initialization of state on open (use OSF/1 defaults).
 * 
 * Revision 3.5  91/06/12  09:40:56  condict
 * Convert OSF/1 baud rates to and from Mach baud rates (e.g. 9600 <-> B9600).
 * Eliminate panic when hardware error on tty (keep the console message).
 * 
 * Revision 3.4  91/06/09  15:51:31  condict
 * Eliminate debug messages on console.  Also, fix references to hi-water
 * mark in readding chars from tty's (was always reading 0 chars).
 * 
 * Revision 3.3  91/05/29  16:22:48  condict
 * Remove tty_ioctl debug message.  Add queue_init call to new tty structs.
 * 
 * Revision 3.2  91/05/07  15:58:17  condict
 * Uncomment some of the code to make char I/O work.
 * 
 * Revision 3.1  91/03/08  16:06:46  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.0  91/01/17  12:06:22  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.9  91/03/12  21:12:05  dbg
 * 	Use new macros to distinguish block and character devices.
 * 	Fix tty_open to allocate a reply port for ttys opened only
 * 	for write (from Jonathan Chew).
 * 	[91/02/21            dbg]
 * 
 * Revision 2.8  90/10/25  15:07:37  rwd
 * 	Fix tty_{read,write}_reply to better handle error and data=0 conditions.
 * 	[90/10/21            rwd]
 * 	Check status on device_write.
 * 	[90/10/17            rwd]
 * 	Initialize tty structure at creation.
 * 	[90/10/15            rwd]
 * 
 * Revision 2.7  90/09/09  14:13:59  rpd
 * 	Deallocate the device port after closing it.
 * 	[90/08/23            rpd]
 * 
 * Revision 2.6  90/06/02  15:28:37  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  20:23:58  rpd]
 * 
 * Revision 2.5  90/05/21  14:02:45  dbg
 * 	Fix bug in tty_select: pass device number to l_select, not tty
 * 	pointer.
 * 	[90/03/22            dbg]
 * 
 * Revision 2.4  89/09/15  15:29:44  rwd
 * 	Change includes
 * 	[89/09/11            rwd]
 * 	LITOUT is passed to device based on LITOUT & RAW here.
 * 	[89/09/01            rwd]
 * 
 * Revision 2.3  89/08/31  16:29:25  rwd
 * 	Open flags are FREAD,FWRITE etc when we get here!
 * 	[89/08/30            rwd]
 * 	Set terminal flags to 0 when grabbing a new tty struct
 * 	[89/08/29            rwd]
 * 
 * 		Only queue read to kernel if device is opened for READ.
 * 	[89/08/28            rwd]
 * 	Changed to use inband read
 * 	[89/08/15            rwd]
 * 
 * Revision 2.2  89/08/09  14:46:13  rwd
 * 	Added copyright to file by dbg.  Change to use inband write
 * 	requests.
 * 	[89/07/17            rwd]
 * 
 * $EndLog$
 */
/*
 * Interface between MACH tty devices and BSD ttys.
 */
#include <com.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/ttyloc.h>
#include <sys/tty.h>
#include <sys/conf.h>
#include <sys/errno.h>
#include <sys/user.h>
#include <sys/file.h>
#include <sys/synch.h>

#include <uxkern/device.h>

#ifdef	OSF1_SERVER

/* We need to have the old (BSD 4.3) definitions for baud-rates, since
 * these are required by the Mach 3.0 tty driver.  So, just in case we
 * got the new OSF/1 values in some include file:
 */
#undef	B0
#undef	B50
#undef	B75
#undef	B110
#undef	B134
#undef	B150
#undef	B200
#undef	B300
#undef	B600
#undef	B1200
#undef	B1800
#undef	B2400
#undef	B4800
#undef	B9600
#undef	B19200
#undef	B38400
#undef	EXTA
#undef	EXTB

#include <sys/ttydev.h>		/* To define B* */

#ifndef B19200
#define B19200 (B9600 + 1)
#endif
#ifndef B38400
#define B38400 (B9600 + 2)
#endif

#endif	OSF1_SERVER

#include <device/tty_status.h>
#include <device/device_types.h>

#include <uxkern/device_reply_hdlr.h>
#include <uxkern/device_utils.h>
#include <sys/lock_types.h>

#ifdef	SECOND_SERVER
extern	int	second_server;
#endif	/* SECOND_SERVER */

mach_port_t node_to_master_device_port();

/*
 * We cannot deallocate the tty when it is closed, since other
 * structures have handles that are not reference-counted (p->p_ttyp).
 * Instead, we just close the device port.
 */

/*
 * Exported version of tty_hash_lookup.
 */
struct tty *
tty_find_tty(dev)
	dev_t	dev;
{
	return (&cdevsw[major(dev)].d_ttys[minor(dev)]);
}

kern_return_t	tty_read_reply();	/* forward */
kern_return_t	tty_write_reply();	/* forward */
int		tty_start();		/* forward */

/*
 * For compatibility with OSF/1 we must have an array of tty structs whose
 * address is entered into the d_ttys field of cdevsw:
 */
#if NCOM > 0
struct tty tty_tp[NCOM];
#endif

extern struct tty 	*cons_tp;	/* console TTY */
extern dev_t cons_dev_number;

tty_initialization()
{
	int			unit;
	extern struct tty	tty_tp[];

	for (unit = 0; unit < NCOM; ++unit) {
#if	UNIX_LOCKS && NCPUS > 1
		lock_init2(&tty_tp[unit].t_lock, TRUE, LTYPE_TTY);
#endif
		tty_tp[unit].t_device_port = MACH_PORT_NULL;
	}
}

/*
 * Open tty.
 */
tty_open(dev, flag, mod, flgp, node)
	dev_t	dev;
	int	flag;
	int	mod;
	int	*flgp;
	int	node;
{
	int	unit = minor(dev);
	register struct tty *tp;
	int error;
	kern_return_t	rc;

	/*
	 * Check whether tty is already open.
	 */
	if (unit >= NCOM)
		return (ENXIO);
	tp = &cdevsw[major(dev)].d_ttys[unit];
	TTY_LOCK(tp);

	if (tp->t_device_port == MACH_PORT_NULL) {
	    /*
	     * Device is closed - try to open it.
	     */
	    mach_port_t	device_port;
	    dev_mode_t	mode;
	    char	name[32];

	    /* get string name from device number */
	    rc = cdev_name_string(dev, name);
	    if (rc != 0) {
		TTY_UNLOCK(tp);
		return (rc);	/* bad name */
	    }

	    /* fix modes */
	    mode = 0;	/* XXX */
	    /* open device */
	    rc = device_open(node_to_master_device_port(node),
			     mode,
			     name,
			     &device_port);
	    if (rc != D_SUCCESS) {
		TTY_UNLOCK(tp);
		return (dev_error_to_errno(rc));
	    }

	    /*
	     * Check for alias of console.
	     */
	    if (cons_tp != 0 /* we use this to create cons_tp */
		    && device_port == cons_tp->t_device_port) {
		/*
		 * Opened console - use its tty.
		 */
		/*
		 * XXX - barbou@gr.osf.org - 
		 * The following code is wrong:
		 * 1. this overrides the whole cdevsw entry for all the 
		 *    ttys by the console cdevsw entry: all the other ttys are
		 *    lost.
		 * 2. this code doesn't take the console lock after replacing
		 *    the tty entry by the console.
		 * Unfortunately, I haven't managed to find out a working
		 * alternative to this code !
		 */
		tp = cons_tp;
		cdevsw[major(dev)] = cdevsw[major(cons_dev_number)];
	    }
	    else {
		/*
		 * Save device-port and tty-structure for device number.
		 */
		tp->t_device_port = device_port;
	    }
	}


	tp->t_oproc = tty_start;

	if ((tp->t_state & TS_ISOPEN) == 0) {

	    struct tty_status	ttstat;
	    unsigned int	ttstat_count;

	    queue_init(&tp->t_selq);
	    /*
	     * Set initial characters
	     */
	    ttychars(tp);

	    /* From OSF/1: */
	    tp->t_iflag = TTYDEF_IFLAG;
	    tp->t_oflag = TTYDEF_OFLAG;
	    tp->t_lflag = TTYDEF_LFLAG;
	    tp->t_cflag = CS8|CREAD;
	    tp->t_line = 0;
	    tp->t_state = TS_ISOPEN|TS_CARR_ON;

	    tp->t_nwrites = 0;

	    /*
	     * Get configuration parameters from device, put in tty structure
	     */
	    ttstat_count = TTY_STATUS_COUNT;
	    rc = device_get_status(tp->t_device_port,
				   TTY_STATUS,
				   (int *)&ttstat,
				   &ttstat_count);
	    check_tty_return(rc, tp, "tty_open", "device_get_status");
	    
	    tp->t_ispeed = baud_decode(ttstat.tt_ispeed);
	    tp->t_ospeed = baud_decode(ttstat.tt_ospeed);
	    ttsetwater(tp);
	    if (ttstat.tt_flags & TF_EVENP)
		tp->t_flags |= EVENP;
	    if (ttstat.tt_flags & TF_ODDP)
		tp->t_flags |= ODDP;
	    if (ttstat.tt_flags & TF_ECHO)
		tp->t_flags |= ECHO;
	    if (ttstat.tt_flags & TF_CRMOD)
		tp->t_flags |= CRMOD;
	    if (ttstat.tt_flags & TF_XTABS)
		tp->t_flags |= XTABS;
	    ttcompatsetflags(tp, &tp->t_termios);

	    /*
	     * Pretend that carrier is always on, until I figure out
	     * how to do it right.
	     */
	    tp->t_state |= TS_CARR_ON; /* should get from TTY_STATUS */

	    update_mach_tty_status(tp);
	}
	else if (tp->t_state & TS_XCLUDE && u.u_uid != 0) {
	    TTY_UNLOCK(tp);
	    return (EBUSY);
        }

	if (tp->t_reply_port == MACH_PORT_NULL) {
	    /*
	     * Create reply port for device read/write messages.
	     * Hook it up to device and tty.
	     */
#ifdef	REPLY_PORT_ALIAS
	    reply_hash_enter(&tp->t_reply_port,
			     (char *)tp,
			     tty_read_reply,
			     tty_write_reply);
#else
	    tp->t_reply_port = mach_reply_port();
	    reply_hash_enter(tp->t_reply_port,
			     (char *)tp,
			     tty_read_reply,
			     tty_write_reply);
#endif
	}

	if ((flag & FREAD) && !(tp->t_state & TS_RQUEUED)) {
	    /*
	     * Post initial read.
	     */
	    rc = device_read_request_inband(tp->t_device_port,
					    tp->t_reply_port,
					    0,
					    0,	/* recnum */
					    tp->t_hiwat);
	    check_tty_return(rc, tp, "tty_open", "device_read_request");
	    tp->t_state |= TS_RQUEUED;
	}
	/*
	 * Wait for CARR_ON
	 */
	if (flag & O_NDELAY) {
	    tp->t_state |= TS_ONDELAY;
	}
	else {
	    while ((tp->t_state & TS_CARR_ON) == 0) {
		tp->t_state |= TS_WOPEN;
		if (error = ttysleep(tp, (caddr_t)&tp->t_rawq, TTIPRI | PCATCH,
				     ttopen))
			goto out;
		/*
		 * some devices sleep on t_state...	XXX
		 */
	    }
	}

	error = (*linesw[tp->t_line].l_open)(dev, tp, flag);

out:
	TTY_UNLOCK(tp);
	return (error);
}

#ifdef OSF1_ADFS
tty_close(dev, node, flag)
#else
tty_close(dev, flag)
#endif
	dev_t	dev;
	int	flag;
#ifdef OSF1_ADFS
        node_t  node;
#endif
{
	register struct tty *tp;
	kern_return_t rc;

	/* get tty structure and port from dev */
	tp = &cdevsw[major(dev)].d_ttys[minor(dev)];
	TTY_LOCK(tp);

	(*linesw[tp->t_line].l_close)(tp);

	/*
	 * Do not close the console (special case HACK HACK)
	 */
	if (tp != cons_tp) {
	    /*
	     * Remove the reply port
	     */
	    reply_hash_remove(tp->t_reply_port);
	    (void) mach_port_mod_refs(mach_task_self(), tp->t_reply_port,
				      MACH_PORT_RIGHT_RECEIVE, -1);
	    tp->t_reply_port = MACH_PORT_NULL;

	    /*
	     * And close the device
	     */
	    rc = device_close(tp->t_device_port);
	    check_tty_return(rc, tp, "tty_close", "device_close");
	    (void) mach_port_deallocate(mach_task_self(), tp->t_device_port);
	    tp->t_device_port = MACH_PORT_NULL;

	    /*
	     * Disable output
	     */
	    tp->t_oproc = 0;
	}
	/*
	 * Leave tty structure, but mark it closed.
	 */
	ttyclose(tp);
	TTY_UNLOCK(tp);
	return (0);
}

#ifdef OSF1_ADFS
tty_read(dev, node, uio, flag)
#else
tty_read(dev, uio, flag)
#endif
	dev_t	dev;
#ifdef OSF1_ADFS
        node_t  node;
#endif
	struct uio *uio;
{
	register struct tty *tp;
	int error;


	/* get tty from device */
	tp = &cdevsw[major(dev)].d_ttys[minor(dev)];
	TTY_LOCK(tp);
	error = (*linesw[tp->t_line].l_read)(tp, uio, flag);
	TTY_UNLOCK(tp);
	return (error);
}

#ifdef OSF1_ADFS
tty_write(dev, node, uio, flag)
#else
tty_write(dev, uio, flag)
#endif
	dev_t	dev;
#ifdef OSF1_ADFS
        node_t  node;
#endif
	struct uio *uio;
{
	register struct tty *tp;
	int error;

	/* get tty from device */
	tp = &cdevsw[major(dev)].d_ttys[minor(dev)];
	TTY_LOCK(tp);
	error = (*linesw[tp->t_line].l_write)(tp, uio, flag);
	TTY_UNLOCK(tp);
	return (error);
}

tty_select(dev, rw)
	dev_t	dev;
	int	rw;
{
#ifdef	OSF1_SERVER
	printf("tty_select()\n");
	return 0;
#else	OSF1_SERVER
	register struct tty *tp;

	/* get tty from device */
	tp = tty_hash_lookup(dev);
	return ((*linesw[tp->t_line].l_select)(dev, rw));
#endif	OSF1_SERVER
}

#ifdef OSF1_ADFS
tty_ioctl(dev, node, cmd, data, flag)
#else
tty_ioctl(dev, cmd, data, flag)
#endif
	dev_t	dev;
#ifdef OSF1_ADFS
        node_t  node;
#endif
	int	cmd;
	caddr_t	data;
	int	flag;
{
	register struct tty *tp;
	int	error;
	kern_return_t rc;
	unsigned int		ttstat_count;
	int			word;

	/* get tty from device */
	tp = &cdevsw[major(dev)].d_ttys[minor(dev)];
	TTY_LOCK(tp);

	error = (*linesw[tp->t_line].l_ioctl)(tp, cmd, data, flag);
	if (error >= 0)
		goto out;
	    
	error = ttioctl(tp, cmd, data, flag);
	if (error >= 0) {
	    update_mach_tty_status(tp);
	    goto out;
	}
	/* if command is one meant for device,
	   translate it into a device_set_status() and issue it.
	 */
	switch (cmd) {
	    case TIOCMODG:
	    case TIOCMGET:
#ifdef	SECOND_SERVER
		if (second_server && tp == cons_tp)
			/* a second server can't access the console device */
			break;
#endif	/* OSF1_SERVER */
		ttstat_count = TTY_MODEM_COUNT;
		rc = device_get_status(tp->t_device_port,
				       TTY_MODEM,
				       &word,
				       &ttstat_count);
		check_tty_return(rc, tp, "tty_ioctl", "device_get_status");
		*(int *)data = word;
		break;
	    case TIOCMODS:
	    case TIOCMSET:
#ifdef	SECOND_SERVER
		if (second_server && tp == cons_tp)
			/* a second server can't access the console device */
			break;
#endif	/* OSF1_SERVER */
		word = *(int *)data;
		rc = device_set_status(tp->t_device_port,
				       TTY_MODEM,
				       &word,
				       TTY_MODEM_COUNT);
		check_tty_return(rc, tp, "tty_ioctl", "device_get_status");
		break;
	    case TIOCMBIS:
	    case TIOCMBIC:
	    case TIOCSDTR:
	    case TIOCCDTR:
#ifdef	SECOND_SERVER
		if (second_server && tp == cons_tp)
			/* a second server can't access the console device */
			break;
#endif	/* OSF1_SERVER */
		ttstat_count = TTY_MODEM_COUNT;
		rc = device_get_status(tp->t_device_port,
				       TTY_MODEM,
				       &word,
				       &ttstat_count);
		check_tty_return(rc, tp, "tty_ioctl", "device_get_status");
		switch (cmd) {
		    case TIOCMBIS:
			word |= *(int *)data;
			break;
		    case TIOCMBIC:
			word &= ~*(int *)data;
			break;
		    case TIOCSDTR:
			word |= TM_DTR;
			break;
		    case TIOCCDTR:
			word &= ~TM_DTR;
			break;
		}
		rc = device_set_status(tp->t_device_port,
				       TTY_MODEM,
				       &word,
				       TTY_MODEM_COUNT);
		check_tty_return(rc, tp, "tty_ioctl", "device_set_status");
		break;
	    case TIOCSBRK:
#ifdef	SECOND_SERVER
		if (second_server && tp == cons_tp)
			/* a second server can't access the console device */
			break;
#endif	/* OSF1_SERVER */
		rc = device_set_status(tp->t_device_port,
				       TTY_SET_BREAK,
				       &word,
				       0);
		check_tty_return(rc, tp, "tty_ioctl", "device_set_status");
		break;
	    case TIOCCBRK:
		rc = device_set_status(tp->t_device_port,
				       TTY_CLEAR_BREAK,
				       &word,
				       0);
		check_tty_return(rc, tp, "tty_ioctl", "device_set_status");
		break;
#if 1	/* XXX: tcsbreak library routine should use TIOC[SC]BRK. */
	    case TCSBREAK:
#ifdef	SECOND_SERVER
		if (second_server && tp == cons_tp)
			break;
#endif
		rc = device_set_status(tp->t_device_port, TTY_SET_BREAK,
					&word, 0);
		if (rc == D_SUCCESS) {
			int time_out;
			assert_wait((int) tty_ioctl, TRUE);
				/* Condition never woken. */
			time_out = *(int *)data * hz / 1000;
			if (time_out <= 0)
				time_out = 250 * hz / 1000;
			thread_set_timeout(time_out);
			thread_block();
			rc = device_set_status(tp->t_device_port,
						TTY_CLEAR_BREAK, &word, 0);
		}
		check_tty_return(rc, tp, "tty_ioctl", "device_set_status");
		break;
#endif	/* 1 */
	    default:
	    {
		/*
		 * Not one of the TTY ioctls - try sending
		 * the code to the device, and see what happens.
		 */
		unsigned int	count;

#ifdef	SECOND_SERVER
		if (second_server && tp == cons_tp) {
			/* a second server can't access the console device */
			error = ENOTTY;
			goto out;
		}
#endif	/* OSF1_SERVER */
		count = (cmd & ~(IOC_INOUT|IOC_VOID)) >> 16; /* bytes */
		count = (count + 3) >> 2;		     /* ints */
		if (count == 0)
		    count = 1;

		if (cmd & (IOC_VOID|IOC_IN)) {
		    error = device_set_status(tp->t_device_port,
					      cmd,
					      (int *)data,
					      count);
		    if (error) {
			    error = ENOTTY;
			    goto out;
		    }
		}
		if (cmd & IOC_OUT) {
		    error = device_get_status(tp->t_device_port,
					      cmd,
					      (int *)data,
					      &count);
		    if (error) {
			    error = ENOTTY;
			    goto out;
		    }
		break;
		}
	    }
	}

	error = 0;

 out:
	TTY_UNLOCK(tp);
	return (error);
}

tty_stop(tp, rw)
	register struct tty *tp;
	int	rw;
{
	kern_return_t	rc;

#ifdef	SECOND_SERVER
	if (second_server && tp == cons_tp) {
		second_tty_stop(tp, rw);
		return;
	}
#endif	/* SECOND_SERVER */


	if (tp->t_state & TS_BUSY) {
		rc = device_set_status(tp->t_device_port,
				       (rw) ? TTY_FLUSH : TTY_STOP,
				       (int *)&rw,
				       1);
		check_tty_return(rc, tp, "tty_stop", "device_set_status");
		if (rw)
		  tp->t_state |= TS_ISSTOPPED;
		if ((tp->t_state & TS_TTSTOP) == 0)
			tp->t_state |= TS_FLUSH;
	}
}

/* Send an XOFF character now,  even if we have ourselves been XOFFed.
   Not yet implemented correctly. XXX */
int
tty_xoff(tp)
     register struct tty *tp;
{
  return putc(tp->t_cc[VSTOP], &tp->t_outq)==0;
}

int
tty_xon(tp)
     register struct tty *tp;
{
  return putc(tp->t_cc[VSTART], &tp->t_outq)==0;
}



tty_read_reply(tp_ptr, error, data, data_count)
	char *		tp_ptr;
	int		error;
	char		data[];
	unsigned int	data_count;
{
	register struct tty *tp = (struct tty *)tp_ptr;
	register int	i;
	kern_return_t 	rc;
	register unsigned char *udata = (unsigned char *) data;

	interrupt_enter(SPLTTY);
	TTY_LOCK(tp);
	if (!error && data_count > 0 && (tp->t_cflag & CREAD)) {
	    for (i = 0; i < data_count; i++)
		(*linesw[tp->t_line].l_rint)(udata[i], tp);
	}
#ifdef D_OUT_OF_BAND
	else if (error == D_OUT_OF_BAND) {
	    unsigned int count = TTY_OUT_OF_BAND_COUNT;
	    struct tty_out_of_band toob;
	    rc = device_get_status(tp->t_device_port,
				   TTY_OUT_OF_BAND,
				   (dev_status_t)&toob, &count);
	    if (rc != D_SUCCESS) {
		check_tty_return(rc, tp, "tty_read_reply", "device_get_status");
	    } else {
		switch (toob.toob_event) {
		case TOOB_NO_EVENT:
		    printf("tty_read_reply: unexpected lack of event\n");
		    break;
		case TOOB_BREAK:
		    (*linesw[tp->t_line].l_rint)(TTY_FE|toob.toob_arg, tp);
		    break;
		case TOOB_BAD_PARITY:
		    (*linesw[tp->t_line].l_rint)(TTY_PE|toob.toob_arg, tp);
		    break;
		default:
		    printf("tty_read_reply: unknown event 0x%x\n",
			   toob.toob_event);
		    break;
		}
	    }
	}
#endif	/* D_OUT_OF_BAND */
	else if (error == D_IO_ERROR) {
	  ttymodem(tp,0);
	  TTY_UNLOCK(tp);
	  interrupt_exit(SPLTTY);
	  return;
	} else if (error) {
	    check_tty_return(error, tp, "tty_read_reply", "device_read_reply");
	    /* panic("tty_read_reply"); -- NO!!!! Turning off the tty
					-- should not panic the server! */
	    (*linesw[tp->t_line].l_modem)(tp, 0);
	    /* from ttyclose */
	    tp->t_sid = 0;
	    tp->t_pgid = 0;
	    tp->t_state = 0;
	    tp->t_col = tp->t_rocol = 0;
	    tp->t_gen++;
	    TTY_UNLOCK(tp);
	    interrupt_exit(SPLTTY);
	    return;
	}
	TTY_UNLOCK(tp);
	interrupt_exit(SPLTTY);

	rc = device_read_request_inband(tp->t_device_port,
					tp->t_reply_port,
					0,		/* mode */
					0,		/* recnum */
					tp->t_hiwat);
	check_tty_return(rc, tp, "tty_read_reply", "device_read_request_inband");
}

tty_write_reply(tp_ptr, error, bytes_written)
	char *		tp_ptr;
	int		error;
	int		bytes_written;
{
	register struct tty *tp = (struct tty *)tp_ptr;

	interrupt_enter(SPLTTY);
	TTY_LOCK(tp);

	if (error) {
	    check_tty_return(error, tp, "tty_write_reply", "device_write_reply");
	    /* panic("tty_write_reply"); -- NO!!!! Turning off the tty
					 -- should not panic the server! */
	    bytes_written = 0;
	    (*linesw[tp->t_line].l_modem)(tp, 0);
	    /* from ttyclose */
	    ttyflush(tp, FWRITE);
	    tp->t_sid = 0;
	    tp->t_pgid = 0;
	    tp->t_state = 0;
	    tp->t_col = tp->t_rocol = 0;
	    tp->t_gen++;
	    TTY_UNLOCK(tp);
	    interrupt_exit(SPLTTY);
	    return;
	    
	}

	tp->t_state &= ~TS_BUSY;
	if (tp->t_state & TS_FLUSH)
	    tp->t_state &= ~TS_FLUSH;
	else
	    ndflush(&tp->t_outq, bytes_written);

	if (tp->t_line)
	    (*linesw[tp->t_line].l_start)(tp);
	else
	    tty_start(tp);

	TTY_UNLOCK(tp);
	interrupt_exit(SPLTTY);

}

tty_start(tp)
	register struct tty *tp;
{
	int	cc;
	kern_return_t result;

	if ((tp->t_state & (TS_TIMEOUT|TS_BUSY|TS_TTSTOP)) == 0) {
	    if (tp->t_outq.c_cc <= tp->t_lowat) {
		if (tp->t_state & TS_ASLEEP) {
		    tp->t_state &= ~TS_ASLEEP;
		    wakeup((caddr_t)&tp->t_outq);
		}
		select_wakeup(&tp->t_selq);
	    }

	    if (tp->t_state & TS_ISSTOPPED) {
	      result = device_set_status(tp->t_device_port, TTY_START,
					 (int *)NULL, 0);
	      check_tty_return(result, tp, "tty_start", "device_set_status");
	      tp->t_state &= ~TS_ISSTOPPED;
	    }

	    /* get characters from tp->t_outq,
	       send to device */
	    if (tp->t_outq.c_cc != 0) {

		cc = ndqb(&tp->t_outq, 0);	/* device handles timeouts! */
		/* This assumes inband data size is large enough. */
		result = device_write_request_inband(tp->t_device_port,
						     tp->t_reply_port,
						     0,	/* mode */
						     0,	/* recnum */
						     tp->t_outq.c_cf,
						     cc);
		if (result != KERN_SUCCESS) {
		    check_tty_return(result, tp, "tty_start",
				     "device_write_request");
		    Debugger("tty_start");
		} else 
		    tp->t_state |= TS_BUSY;
	    }
	}
}

/* Wait for output to finish on tp.  The microkernel tends to acknowledge
   device_writes to the tty before they have finished, even before they
   have started.  This behaviour should be fixed, but doing so naively
   would introduce too great a latency between the end of one device_write
   and the start of the next.  So instead we sync up by asking it to
   explicitly to drain its output.
   If we compiled against an old kernel without TTY_DRAIN, or if we are
   running with such a kernel, we don't get this functionality. */
tty_drain(tp)
	register struct tty *tp;
{
#ifdef TTY_DRAIN
	int word;
	(void) device_set_status(tp->t_device_port, TTY_DRAIN, &word, 0);
#endif	/* TTY_DRAIN */
}

/*ARGSUSED*/
struct tty *
nulltty(dev)
	dev_t	dev;
{
	return ((struct tty *)0);
}

update_mach_tty_status(tp)
struct tty *tp;
{
	struct tty_status	ttstat;
	unsigned int		ttstat_count;
	int			word, oldword, anychange;
	kern_return_t		rc;
	static enum {DONT_KNOW, WORKS, DOESNT_WORK} hupcl_state = DONT_KNOW;

#ifdef	SECOND_SERVER
	if (second_server && tp == cons_tp) {
		/* a second server can't access the console device */
		return;
	}
#endif	/* SECOND_SERVER */
	ttstat_count = TTY_STATUS_COUNT;
	rc = device_get_status(tp->t_device_port,
			       TTY_STATUS,
			       (int *)&ttstat,
			       &ttstat_count);
	check_tty_return(rc, tp, "update_mach_tty_status", "device_get_status");
	anychange = 0;

	word = baud_encode(tp->t_ispeed);
	anychange |= (word != ttstat.tt_ispeed);
	ttstat.tt_ispeed = word;
	word = baud_encode(tp->t_ospeed);
	anychange |= (word != ttstat.tt_ospeed);
	ttstat.tt_ospeed = word;

	/* If IGNBRK is true breaks should be ignored completely.
	 * We can't ask Mach to do that; the best we can manage is to
	 * have it return a null character.  If we have out-of-band
	 * handling then this is obsolete.
	 */
	if ((tp->t_iflag & BRKINT) && !(tp->t_iflag & IGNBRK)) {
	    anychange |= (ttstat.tt_breakc != tp->t_cc[VINTR]);
	    ttstat.tt_breakc = tp->t_cc[VINTR];
	} else {
	    anychange |= ttstat.tt_breakc;
	    ttstat.tt_breakc = 0;
	}

#ifdef TF_OUT_OF_BAND
#define TF_OUT TF_OUT_OF_BAND
#else
#define TF_OUT 0
#endif
	oldword = ttstat.tt_flags & 
	    (TF_TANDEM|TF_ODDP|TF_EVENP|TF_LITOUT|TF_MDMBUF|
	     TF_NOHANG|TF_HUPCLS|TF_OUT);
	ttstat.tt_flags &= ~oldword;
	word = TF_OUT;	/* We always want OUT_OF_BAND if available. */
	if (tp->t_iflag & IXOFF)
	    word |= TF_TANDEM;
	if (tp->t_cflag & PARENB) {
	    word |= (tp->t_cflag & PARODD) ? TF_ODDP : TF_EVENP;
#ifdef TF_INPCK
	    if (tp->t_iflag & INPCK)
		word |= TF_INPCK;
#endif
	}
	if ((tp->t_cflag & CSIZE) == CS8)
	    word |= TF_LITOUT;
	if (tp->t_lflag & MDMBUF)
	    word |= TF_MDMBUF;
	if ((tp->t_cflag & CLOCAL) || (tp->t_lflag & NOHANG))
	    word |= TF_NOHANG;
	/* There are two different ways of asking for hangup (dropped DTR)
	 * on last close: BSD's TIOCHPCL and POSIX's HUPCL bit in c_cflag.
	 * The OSF/1 code should treat them both the same but doesn't. XXX
	 */
	if ((tp->t_state & TS_HUPCLS) || (tp->t_cflag & HUPCL))
	    word |= TF_HUPCLS;
	ttstat.tt_flags |= word;
	anychange |= (word != oldword);

	if (anychange) {
	    rc = device_set_status(tp->t_device_port,
				   TTY_STATUS,
				   (int *)&ttstat,
				   ttstat_count);
	    if (rc != D_SUCCESS) {
		    check_tty_return(rc, tp, "update_mach_tty_status",
				     "device_set_status");
		    return;
	    }
#ifndef CAN_CLEAR_TF_HUPCLS
	    /* Turning off HUPCLS may not work, depending on microkernel. */
	    if ((oldword & TF_HUPCLS) && !(word & TF_HUPCLS)) {
		if (hupcl_state == DONT_KNOW) {
		    ttstat_count = TTY_STATUS_COUNT;
		    rc = device_get_status(tp->t_device_port,
					   TTY_STATUS,
					   (int *)&ttstat,
					   &ttstat_count);
		    if (rc == D_SUCCESS && !(ttstat.tt_flags & TF_HUPCLS))
			hupcl_state = WORKS;
		    else {
			hupcl_state = DOESNT_WORK;
			printf("unable to make kernel clear TF_HUPCLS\n");
		    }
		}
		if (hupcl_state == DOESNT_WORK) {
		    tp->t_state |= TS_HUPCLS;
		    tp->t_cflag |= HUPCL;
		}
	    }
#endif	/* CAN_CLEAR_TF_HUPCLS */
	}
}

check_tty_return(rc, tp, funcname, syscallname)
kern_return_t rc;
struct tty *tp;
char *funcname, *syscallname;
{
	if (rc != KERN_SUCCESS) {
		dev_t dev = tp->t_dev;
		printf("%s: %s returned %d, device (%d,%d)\n",
		       funcname, syscallname, rc, major(dev), minor(dev));
	}
}
