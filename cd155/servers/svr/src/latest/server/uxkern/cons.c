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
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: cons.c,v $
 * Revision 1.9  1995/03/02  19:36:31  stans
 *  Use the micro-kernel constant for Max inline data instead of a hard coded
 *  numeric (128). CONS_BUF_SIZE == IO_INBAND_MAX == 128.
 *
 *  Reviewer:lenb & suri
 *  Risk:low
 *  Benefit or PTS #:8129
 *  Testing: WW07 sats
 *
 * Revision 1.8  1994/11/18  20:47:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/24  00:32:41  yazz
 *  Reviewer: Chris Peak
 *  Risk: lo
 *  Benefit or PTS #: #9952
 *  Testing: none (only matters *after* a panic)
 *  Module(s): server/bsd/subr_prf.c
 *             server/uskern/cons.c
 *
 * Stop pretending divide-by-zero halts the server and call Debugger() (and
 * if necessary, panic()) instead.
 *
 * Revision 1.6  1994/03/24  20:45:29  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: #8334
 *  Testing: boot + full multiuser (only panic messages changed)
 *  Module(s): server/uxkern/cons.c
 *
 * Print more info when a panic-within-a-panic problem occurs to help catch
 * unexpected MACH_RCV_TOO_LARGE return values.
 *
 * Revision 1.5  1994/01/12  17:47:21  jlitvin
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
 * Revision 1.4  1993/07/14  18:40:01  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:59:12  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:25:40  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:53:14  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:42:13  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:50:36  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:51:00  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.11  1993/04/08  11:38:03  loverso
 * 	[1992/06/25  13:12:23  emcmanus]
 * 	Charles Silvers changes from CMU (avoid compiler optimization)
 *
 * Revision 2.10  93/01/08  14:34:46  durriya
 * 	pass node # as arg to cons_write, cons_ioctl (durriya)
 * 
 * Revision 2.9  92/05/24  13:54:59  pjg
 * 	Revision 3.13  92/03/23  18:04:26  condict
 * 	Allow NCPUS==1 in the server, to compile optimally for a uni-processor.
 * 
 * Revision 2.8  92/05/01  09:48:06  rabii
 * 	Changed call to tty_open in cons_open to pass in this_node as the
 * 	node number indicating where the console device resides. (rabii)
 * 
 * Revision 2.7  92/03/09  13:15:40  durriya
 * 	92/01/30  16:05:52  sp
 * 	Fix for console printf problem with strings longer that 128 chars
 * 
 * 	91/12/20  17:57:15  barbou
 * 	Check the device_xxx() routines' return value.
 * 
 * 	91/12/13  13:03:40  sp
 * 	Removed CMUCS conditionals
 * 
 * Revision 2.6  92/01/17  19:04:03  roy
 * 	Reduce size of cons_buf_size to 120 so that device_write_inband
 * 	is happy (max. 128 chars).  (dlb, jeffreyh)
 * 
 * Revision 2.5  92/01/09  15:57:19  roy
 * 	Add missing args to tty_open call.
 * 
 * Revision 2.4  91/12/17  10:35:31  roy
 * 	91/12/13  13:03:40  sp
 * 	Removed CMUCS conditionals
 * 
 * Revision 2.3  91/10/14  13:23:54  sjs
 * 	91/10/01  14:11:13  condict
 * 	Protect console buf with a mutex instead of with spl's.
 * 
 * Revision 2.2  91/08/31  14:25:01  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.8  91/08/28  14:03:33  condict
 * Use console_port instead of tty struct to write to console, if we are
 * at point in bootstrap where cons_open hasn't yet been called.
 * 
 * Revision 3.7  91/08/27  15:37:28  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.6  91/07/29  16:32:18  barbou
 * Modified to decide at run-time if running as a second server.
 * 
 * Revision 3.5  91/07/04  17:59:45  barbou
 * Initialize the console lock (to allow sleeping for it instead of looping).
 * Added missing parameter to function called from cdevsw[].
 * 
 * Revision 3.4  91/07/02  14:46:14  condict
 * Add static tty struct for console.
 * 
 * Revision 3.3  91/06/25  17:11:34  condict
 * Fix initialization and allocation of the tty structs for console.
 * 
 * Revision 3.2  91/03/08  16:06:07  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.1  91/01/17  15:46:29  condict
 * Modified by bernadat to run as 2nd server.
 * 
 * Revision 1.1  90/12/10  11:38:19  bernadat
 * Initial revision
 * 
 * Revision 2.7  91/03/20  15:04:52  dbg
 * 	Pay attention to amount written in cnflush - it might not be the
 * 	entire buffer.
 * 	[90/12/20            dbg]
 * 
 * Revision 2.6  90/06/02  15:27:11  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  20:10:13  rpd]
 * 
 * Revision 2.5  90/05/21  14:02:08  dbg
 * 	Console is not always device (0,0).  Look for "console" in
 * 	cdevsw.  Still assumes that minor is 0.
 * 	[90/03/15            dbg]
 * 
 * Revision 2.4  89/12/08  20:18:34  rwd
 * 	Added cons_port().
 * 	[89/12/05  02:34:13  af]
 * 
 * Revision 2.3  89/09/15  15:29:16  rwd
 * 	Change includes
 * 	[89/09/11            rwd]
 * 
 * Revision 2.2  89/08/31  16:29:14  rwd
 * 	Have open use flags passed to it to open device.
 * 	[89/08/30            rwd]
 * 	Changed to use device_write_inband
 * 	[89/08/15            rwd]
 * 	Added spltty's around references to cons_buf.
 * 	[89/08/11            rwd]
 * 
 *
 */
/*
 * Console output.  Handles aliasing other ttys to console.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/ttyloc.h>
#include <sys/tty.h>
#include <sys/conf.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/lock_types.h>

#include <uxkern/device_utils.h>

struct tty	*cons_tp;	/* Points to cons_tty (see conf.c) */

dev_t			cons_dev_number = NODEV;
mach_port_t		console_port;
extern mach_port_t	device_server_port;
extern node_t		this_node;

extern struct tty *tty_find_tty();

#ifdef	SECOND_SERVER
extern	int	second_server;
#endif	/* SECOND_SERVER */

cons_initialization()
{
#if	UNIX_LOCKS && NCPUS > 1
	lock_init2(&cons_tp->t_lock, TRUE, LTYPE_TTY);
#endif
	cons_tp->t_device_port = MACH_PORT_NULL;
}

cons_open(dev, flag)
	dev_t	dev;
	int	flag;
{
	register int error;
	register int major_num;

#ifdef	SECOND_SERVER
	if (second_server) {
		return second_cons_open(dev, flag);
	}
#endif	/* SECOND_SERVER */

	if (cons_dev_number == NODEV) {
	    /*
	     * Look for console.
	     */
	    for (major_num = 0; major_num < nchrdev; major_num++) {
		if (!strcmp(cdevsw[major_num].d_name, "console")) {
		    cons_dev_number = makedev(major_num, 0);
		    break;
		}
	    }
	    if (cons_dev_number == NODEV) {
		panic("no console configured");
	    }
	}

	error = tty_open(cons_dev_number, flag, 0, (int *)0, this_node);

	return (error);
}

/* to satisfy console redirection */
#ifdef OSF1_ADFS
cons_write(dev, node, uio, flag)
	dev_t	dev;
        node_t node;
	struct uio *uio;
{
	return (tty_write(dev, node, uio, flag));
}
#else
cons_write(dev, uio, flag)
	dev_t	dev;
	struct uio *uio;
{
	return (tty_write(dev, uio, flag));
}
#endif

/* to satisfy console redirection */
#ifdef OSF1_ADFS
cons_ioctl(dev, node, cmd, data, flag)
	dev_t	dev;
        node_t  node;
	int	cmd;
	caddr_t	data;
	int	flag;
{
	return (tty_ioctl(dev, node, cmd, data, flag));
}
#else
cons_ioctl(dev, cmd, data, flag)
	dev_t	dev;
	int	cmd;
	caddr_t	data;
	int	flag;
{
	return (tty_ioctl(dev, cmd, data, flag));
}
#endif

/*
 * device_write_inband() can't deal with more than 128 characters at
 * a time. It will return MIG_ARRAY_TOO_LARGE otherwise...
 * Check the file {mk_release_directory}/include/device/device.defs for
 * the MiG stub, and device_types.defs for the argument type and its length
 * restriction.
 */
#define	CONS_BUF_SIZE	IO_INBAND_MAX
struct {
	char		chars[CONS_BUF_SIZE];
	int		pos;
	struct mutex	lock;
	int		initialized;
} cons_buf;

cnputc(c)
	int	c;
{
#ifdef	SECOND_SERVER
	if (second_server) {
		second_cnputc(c);
		return;
	}
#endif	/* SECOND_SERVER */

	if (!cons_buf.initialized) {
		/* There is no race condition here because first call to
		 * cnputc/cnflush happens when kernel is still single threaded
		 */
		cons_buf.initialized = 1;
		mutex_init(&cons_buf.lock);
	}
	mutex_lock(&cons_buf.lock);
	cons_buf.chars[cons_buf.pos++] = c;
	if (cons_buf.pos >= CONS_BUF_SIZE) {
		mutex_unlock(&cons_buf.lock);
		cnflush();
	} else
		mutex_unlock(&cons_buf.lock);

	if (c == '\n')
	    cnputc('\r');
}

cnflush()
{
	unsigned int cw;
	int start_pos;
	kern_return_t rc;

#ifdef	SECOND_SERVER
	if (second_server) {
		return;
	}
#endif	/* SECOND_SERVER */

	if (!cons_buf.initialized) {
		/* There is no race condition here because first call to
		 * cnputc/cnflush happens when kernel is still single threaded
		 */
		cons_buf.initialized = 1;
		mutex_init(&cons_buf.lock);
	}
	mutex_lock(&cons_buf.lock);
	start_pos = 0;
	while (start_pos < cons_buf.pos) {
	    /* In case the console is not yet open (we haven't yet called
	     * cons_open in bootstrap) then use port that was saved by
	     * console_init, below:
	     */
	    if (!cons_tp->t_device_port)
		    rc = device_write_inband(console_port,
				    0, 0,
				    &cons_buf.chars[start_pos],
				    cons_buf.pos - start_pos,
				    &cw);
	    else
		    rc = device_write_inband(cons_tp->t_device_port,
				    0, 0,
				    &cons_buf.chars[start_pos],
				    cons_buf.pos - start_pos,
				    &cw);
	    if (rc != D_SUCCESS) {
		    int zero = 0;
		    int len = cons_buf.pos - start_pos;
		    char text[128];

		    sprintf(text, "non-std panic in cnflush() rc=0x%x len=%d\n",
				rc, len);
		    (void) device_write_inband(console_port, 0, 0,
					       text, strlen(text), &cw);
		    /*
		     * If write to console failed, panic() can't help much
		     * so force us directly into the debugger.  If user
		     * pushes back, just panic conventionally.  Maybe they
		     * know something we don't!
		     */
		    Debugger();
		    panic("cnflush(): returned from Debugger call!");
	    }
	    start_pos += cw;
	}
	cons_buf.pos = 0;
	mutex_unlock(&cons_buf.lock);
}

console_init()
{
	kern_return_t	rc;

#ifdef	SECOND_SERVER
	if (second_server) {
		return;
	}
#endif	/* SECOND_SERVER */

	/*
	 * Open the console
	 */
	rc = device_open(device_server_port,
			 D_READ|D_WRITE,
			 "console",
			 &console_port);
	if (rc != KERN_SUCCESS)
	    panic("console_init");

}

