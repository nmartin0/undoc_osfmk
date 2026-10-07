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
 * $Log: device_misc.c,v $
 * Revision 1.9  1994/12/02  21:20:42  yazz
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 11518 C-0
 *  Testing: Specific Testcase in PTS report.  Controlc EAT.
 *  Module(s):
 * 	server/i860/ipi_ops.c
 * 	server/uxkern/block_io.c
 * 	server/uxkern/device_misc.c
 * 	server/uxkern/disk_io.c
 * 	server/uxkern/raw_hippi.c
 * Make each user process open() of a device result in a microkernel
 * device_open() call, so MK device drivers can enforce 1-at-a-time usage.
 * Ensure that the number of MK device_open() & device_close() calls match.
 *
 * Revision 1.8  1994/11/18  20:47:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/09/28  20:53:04  suri
 *  Change description: Deleted the code in cdev_name_string() and
 *  bdev_name_string() that was erroneously assigning the device minor numbers in
 *  a modulo 8 and 128 fashion for disk and tape devices respectively.
 *  Reviewer: jerrie
 *  Risk: L
 *  Benefit or PTS #: 11073
 *  Testing: Specific testcase and fileio/vsx EATs
 *  Module(s): cdev_name_string() and bdev_name_string().
 *
 * Revision 1.6  1993/11/30  23:15:41  cfj
 * Bit bucket reads and writes to /dev/null at the emulator instead of sending
 * RPCs to the file server node.
 *
 *  Reviewer:brad, dbm
 *  Risk:M
 *  Benefit or PTS #:7261
 *  Testing:
 *  Module(s):server/sys/vnode.h
 * 	   server/uxkern/device_misc.c
 * 	   server/vfs/spec_vnops.c
 * 	   emulator/fsvr_user_side.c
 *
 * Revision 1.5  1993/07/14  18:40:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:59:59  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:26:17  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.3  1993/03/25  23:30:52  cfj
 * T9 Merge.
 *
 * Revision 1.2.8.1  1993/03/24  23:43:47  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 *     Revision 2.16  93/02/26  20:02:23  yazz
 *     Add forward declaration of char * routine itoa() so that this file
 *     would compile for the i860.
 * 
 *     Revision 2.15  93/02/25  17:49:26  nina
 *     Modified itoa to return string buffer pointer.
 * 
 * Revision 1.2  1992/11/30  22:53:37  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:42:31  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:51:18  cfj
 * Bump major revision number.
 *
 * Revision 2.17  1993/04/29  14:03:56  klh
 * 	Revision 2.16  93/04/08  11:38:38  loverso
 * 		Fix for bug #183: check the validity of the device name when
 * 		scanning through the bdevsw[].
 * 		[1992/09/23  12:37:31  barbou]
 * 		(replaces 2.13)
 *
 * 		Revision 3.16  92/04/22  16:50:00  barbou
 * 		Fix for bug #132: don't undo what char_select() does, if it has not
 * 		been done.
 *
 * 	Revision 2.15  93/01/08  14:34:55  durriya
 * 		add node # as arg to char_close, char_read, char_write, char_ioctl,
 * 		mmread and mmwrite                                       (durriya)
 *
 * Revision 2.16  93/02/26  20:02:23  yazz
 * Add forward declaration of char * routine itoa() so that this file
 * would compile for the i860.
 * 
 * Revision 2.15  93/02/25  17:49:26  nina
 * Modified itoa to return string buffer pointer.
 * 
 * Revision 2.14  92/09/11  09:29:12  rabii
 * 	Added Intel's mods to bdev_name_string and cdev_name_string (rabii)
 * 
 * Revision 2.13  92/08/26  12:12:50  loverso
 * 	parse_root_device() checks for null name to avoid exception.
 * 	[92/08/11            roy]
 * 
 * Revision 2.12  92/05/24  13:55:11  pjg
 * 	Revision 3.15  92/03/24  21:04:03  barbou
 * 	Fix for bug #114: SIGIO generation in char_select().
 * 
 * Revision 2.11  92/05/12  00:04:57  loverso
 * 	Changes ala OSF/1 1.1 to only call select_enqueue() once, and to
 * 	more quickly call select_dequeue().
 * 
 * Revision 2.10  92/05/01  09:59:26  rabii
 * 	Modified to call device open on the proper node directly (rabii)
 * 
 * Revision 2.9  92/03/09  13:21:21  durriya
 * 	92/02/28  18:00:06  barbou
 * 	Fix for bug #91: (for Sequent) compute disk minor number according to 
 * 	the number of partitions per drive.
 * 
 * 	92/02/12  10:58:39  bernadat
 * 	Check protection for mmwrite. (Bug 78)
 * 
 * 	92/01/15  16:55:28  jose
 * 	Fixed misplaced c_lock releasing in char_read [PhB].
 * 
 * 	91/12/27  17:26:15  jose
 * 	Changed interface to reply_hash_enter for port aliasing
 * 
 * 	91/12/20  17:57:26  barbou
 * 	Check the device_xxx() routines' return value.
 * 
 * 	91/12/19  15:59:09  bernadat
 * 	Fixed the code for select on character devices.
 * 
 * Revision 2.8  91/12/17  10:09:44  roy
 * 	91/12/13  13:03:49  sp
 * 	Remove old BSD code
 * 
 * Revision 2.7  91/11/26  13:30:20  rabii
 * 	Removed debugging macros
 * 
 * Revision 2.6  91/11/25  16:14:48  rabii
 * 	Added remote devices
 * 
 * Revision 2.5  91/10/14  18:30:07  roy
 * 	Revision 2.2.1.1  91/09/26  19:09:47  roy
 * 	Changed to match new interface of the dev table.
 * 
 * Revision 2.4  91/10/04  15:21:24  chrisp
 * Add in Locus copyright notice.
 * 
 * Revision 2.3  91/09/17  09:18:22  sjs
 * integrate Locus changes	roman
 * Use process group id when signalling terminal (new vproc interface).
 * 
 * Revision 2.2  91/08/31  14:25:24  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.7  91/08/27  15:37:54  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.6  91/07/02  14:46:45  condict
 * Remove printfs from parse_root_device (too early in bootstrap).
 * 
 * Revision 3.5  91/06/27  15:51:56  sp
 * Add more debugging output for bdev_ioctl
 * 
 * Revision 3.4  91/06/12  09:39:16  condict
 * Finish adapting to the OSF/1 select syscall data structures.
 * 
 * Revision 3.3  91/04/12  14:21:58  condict
 * Add bdev_*() stub functions;  Change uiomove calls to OSF/1 interface.
 * 
 * Revision 3.2  91/03/08  16:06:11  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.1  91/01/17  15:51:43  condict
 * Modified by jose and bernadat to fix bugs in parsing and major/minor
 * device num calculations
 * 
 * Revision 3.0  91/01/17  12:05:55  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.12  90/11/05  16:57:07  rpd
 * 	Fixed mmca for the new vm_region semantics.
 * 	[90/11/02            rpd]
 * 
 * Revision 2.11  90/09/09  14:13:35  rpd
 * 	Deallocate the device port after closing it.
 * 	[90/08/23            rpd]
 * 
 * Revision 2.10  90/08/09  16:38:16  rpd
 * 	Fixed char_select_read_reply to use gsignal3.
 * 	[90/08/09            rpd]
 * 
 * 	Added hack K_X_KDB_ENTER/K_X_KDB_EXIT support for i386.
 * 	[90/08/09            rpd]
 * 
 * Revision 2.9  90/06/02  15:27:16  rpd
 * 	Converted FIOASYNC code to new IPC.
 * 	[90/06/01            rpd]
 * 
 * 	Added interrupt_enter/interrupt_exit to
 * 	char_select_read_reply/char_select_write_reply.
 * 	[90/05/12            rpd]
 * 	Converted to new IPC.
 * 	Made device_pager_release do something.
 * 	Deallocate unwanted object name ports.
 * 	[90/03/26  20:11:59  rpd]
 * 
 * Revision 2.8  90/05/29  20:25:06  rwd
 * 	Added interrupt_enter/exit around char reply routines.
 * 	[90/05/12            rwd]
 * 
 * Revision 2.7  90/05/21  14:02:21  dbg
 * 	Implement char_read (at least for short amounts of data).
 * 	Add FIONBIO and FIOASYNC to character devices.
 * 	[90/05/17            dbg]
 * 
 * 	Redefine C_BLOCK(n) in d_flags to indicate the number of
 * 	partitions per minor device - it's not always 8.
 * 	[90/04/23            dbg]
 * 
 * Revision 2.6  89/12/08  20:18:37  rwd
 * 	Fixed select to actually work the way U*x does: a first call to
 * 	see what's there and a second one to see who woke me up.
 * 	I also dropped the priority to spltty(), but I am not really sure
 * 	it's ok.
 * 	[89/12/05  02:36:01  af]
 * 
 * Revision 2.5  89/11/15  13:27:36  dbg
 * 	Check for missing d_port routine in device_pager_create.
 * 	[89/11/15            dbg]
 * 
 * 		Uninitialized variable bug in parse_root_device.
 * 		Some stacks are lucky, some are not..
 * 	[89/11/11            af]
 * 	Add character device support.
 * 	[89/10/26            dbg]
 * 
 * Revision 2.4  89/10/17  11:27:07  rwd
 * 	Added a limited /dev/kmem.
 * 	[89/10/02            rwd]
 * 
 * Revision 2.3  89/09/15  15:29:22  rwd
 * 	Change includes
 * 	[89/09/11            rwd]
 * 
 * Revision 2.2  89/08/09  14:46:00  rwd
 * 	Added dev_pager code.
 * 	[89/08/05            rwd]
 * 
 * $EndLog$
 */
/*
 * Device interface for out-of-kernel UX kernel.
 */
#include <uxkern/import_mach.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/synch.h>

#include <sys/user.h>

#include <sys/conf.h>
#include <sys/errno.h>
#include <sys/systm.h>
#include <sys/uio.h>
#include <sys/buf.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/synch.h>

#include <uxkern/device.h>
#include <uxkern/device_utils.h>
#include <uxkern/device_reply_hdlr.h>
#include <mach/kern_return.h>
#include <sys/poll.h>

int	char_select_read_reply();	/* forward */
int	char_select_write_reply();	/* forward */
char	*itoa();			/* forward */
mach_port_t node_to_master_device_port();

/*
 * Open device (not TTY).
 * Call device server, then enter device and port in hash table.
 * Note that the MK device driver open is *always* called so it can
 * enforce, for example, one-at-a-time use.  Extra MK device open calls
 * that do succeed are matched with an immediate MK device close call.
 */
char_open(dev, flag, mod, flgp, node)
	dev_t	       	dev;
	int		flag;
	int		mod;
	int		*flgp;
	int		node;
{
	char			name[32];
	kern_return_t		rc;
	mach_port_t		device_port;
	int			mode;
	register char_device_t 	*cp;

	rc = cdev_name_string(dev, name);
	if (rc != 0)
		return (rc);	/* bad name */

	/* fix modes */
	mode = 0;	/* XXX */
	rc = device_open(node_to_master_device_port(node),
			 mode,
			 name,
			 &device_port);
	if (rc != D_SUCCESS) {
		return (dev_error_to_errno(rc));
	}

	/*
	 * See whether we had the device open already.
	 */
	if (dev_lookup(dev, node, CHAR_DEV)) {
		(void)device_close(device_port); /* match extra open w/ close */
		return (0);
	}

	/*
	 * Create new char_device structure.
	 * XXX Make this a zone.
	 */
	cp = (char_device_t *) malloc(sizeof(char_device_t));
	cp->c_device_port = device_port;
	queue_init(&cp->c_read_selq);
	cp->c_read_sel_state = 0;
	queue_init(&cp->c_write_selq);
	cp->c_write_sel_state = 0;
	cp->c_mode = 0;
	cp->c_pgrp = u.u_procp->p_pgid;
	simple_lock_init(&cp->c_lock);

	dev_enter(dev, node, CHAR_DEV, (char *)cp);

	return (0);
}

char_close(dev, node, flag)
	dev_t			dev;
        node_t                  node;
	int			flag;
{
	register char_device_t 	*cp;
	register int		error;

	cp = (char_device_t *) dev_lookup(dev, node, CHAR_DEV);
	if (cp == NULL)
		panic("char_close null dev");

	error = dev_error_to_errno(device_close(cp->c_device_port));
	(void) mach_port_deallocate(mach_task_self(), cp->c_device_port);
	cp->c_device_port = MACH_PORT_NULL;

	if (cp->c_select_port != MACH_PORT_NULL) {
		reply_hash_remove(cp->c_select_port);
		(void) mach_port_mod_refs(mach_task_self(), cp->c_select_port,
					  MACH_PORT_RIGHT_RECEIVE, -1);
		cp->c_select_port = MACH_PORT_NULL;
	}

	queue_init(&cp->c_read_selq);
	cp->c_read_sel_state = 0;
	queue_init(&cp->c_write_selq);
	cp->c_write_sel_state = 0;

	dev_remove(dev, node, CHAR_DEV);
	free((char *)cp);

	return (dev_error_to_errno(error));
}

mach_port_t char_port(dev, node)
	dev_t 			dev;
        node_t                  node;
{
	return (((char_device_t *)dev_lookup(dev, node, CHAR_DEV))->c_device_port);
}

char_read(dev, node, uio)
	dev_t			dev;
        node_t                  node;
	register struct uio 	*uio;
{
	register int		c;
	register kern_return_t	rc;
	io_buf_ptr_inband_t	data;	/* inline array */
	unsigned int		count;
	register char_device_t 	*cp;
	boolean_t		first = TRUE;

	cp = (char_device_t *) dev_lookup(dev, node, CHAR_DEV);

	while (uio->uio_resid > 0) {

	    c = uio->uio_iov->iov_len;

	    if (c > IO_INBAND_MAX)
		c = IO_INBAND_MAX;

	    count = IO_INBAND_MAX;
	    rc = device_read_inband(cp->c_device_port,
				((cp->c_mode & C_NBIO) || !first)
					? D_NOWAIT
					: 0,
				0,
				c,
				&data[0],
				&count);
	    if (rc != 0) {
		if (rc == D_WOULD_BLOCK && !first)
		    break;
		return (dev_error_to_errno(rc));
	    }
	    first = FALSE;

	    uiomove(&data[0], count, uio);
	}

	if (cp->c_mode & C_ASYNC) {
	    /*
	     * Post read request if not already posted.
	     */
	    simple_lock(&cp->c_lock);
	    if ((cp->c_read_sel_state & CD_SEL_MSENT) == 0) {
		cp->c_read_sel_state |= CD_SEL_MSENT;
		rc = device_read_request_inband(cp->c_device_port,
						cp->c_select_port,
						0,	/* mode */
						0,	/* recnum */
						0);	/* bytes wanted */
		if (rc != KERN_SUCCESS) {
			printf("char_read: device_read_request_inband() on dev=(%d,%d) returned 0x%x\n", major(dev), minor(dev), rc);
		}
	    }
	    simple_unlock(&cp->c_lock);
	}
	return (0);
}

char_write(dev, node, uio)
	dev_t			dev;
        node_t                  node;
	struct uio 		*uio;
{
	/* break up the uio into individual device_write calls */
}

char_ioctl(dev, node, cmd, data, flag)
	dev_t			dev;
        node_t                  node;
	int			cmd;
	caddr_t			data;
	int			flag;
{
	register char_device_t	*cp;
	unsigned int 		count;
	register int		error;
	kern_return_t		rc;

	cp = (char_device_t *) dev_lookup(dev, node, CHAR_DEV);

	if (cmd == FIONBIO) {
	    if (*(int *)data)
		cp->c_mode |= C_NBIO;
	    else
		cp->c_mode &= ~C_NBIO;
	    return (0);
	}

	if (cmd == FIOASYNC) {
	    if (*(int *)data) {
		simple_lock(&cp->c_lock);
		cp->c_mode |= C_ASYNC;

		/*
		 * If select reply port does not exist, create it.
		 */
		if (cp->c_select_port == MACH_PORT_NULL) {
#ifdef	REPLY_PORT_ALIAS
		    reply_hash_enter(&cp->c_select_port,
				     (char *)cp,
				     char_select_read_reply,
				     char_select_write_reply);
		}
#else
		    cp->c_select_port = mach_reply_port();
		    reply_hash_enter(cp->c_select_port,
				     (char *)cp,
				     char_select_read_reply,
				     char_select_write_reply);
		}
#endif
		/*
		 * Post read request if not already posted.
		 */
		if ((cp->c_read_sel_state & CD_SEL_MSENT) == 0) {
		    cp->c_read_sel_state |= CD_SEL_MSENT;
		    rc = device_read_request_inband(cp->c_device_port,
						    cp->c_select_port,
						    0,	/* mode */
						    0,	/* recnum */
						    0);	/* bytes wanted */
		    if (rc != KERN_SUCCESS) {
			    printf("char_ioctl: device_read_request_inband() on dev=(%d,%d) returned 0x%x\n", major(dev), minor(dev), rc);
		    }
	        }
		simple_unlock(&cp->c_lock);
	    } else {
		cp->c_mode &= ~C_ASYNC;
	    }
	    return (0);
	}

	count = (cmd & ~(IOC_INOUT|IOC_VOID)) >> 16; /* bytes */
	count = (count + 3) >> 2;		     /* ints */
	if (count == 0)
	    count = 1;

	if (cmd & (IOC_VOID|IOC_IN)) {
	    error = device_set_status(cp->c_device_port,
				      cmd,
				      (int *)data,
				      count);
	    if (error)
		return (ENOTTY);
	}
	if (cmd & IOC_OUT) {
	    error = device_get_status(cp->c_device_port,
				      cmd,
				      (int *)data,
				      &count);
	    if (error)
		return (ENOTTY);
	}
	return (0);
}

char_select(dev, events, revents, scanning, node)
	int scanning;
	dev_t dev;
	short *events, *revents;
        node_t  node;
{
	register char_device_t	*cp;
	register int 		s;
	kern_return_t		rc;
	int			enqueue = 0;

	s = spltty();
	DEVSW_FUNNEL(c,major(dev));

	cp = (char_device_t *) dev_lookup(dev, node, CHAR_DEV);

	simple_lock(&cp->c_lock);
	/*
	 * If select reply port does not exist, create it.
	 */
	if (cp->c_select_port == MACH_PORT_NULL) {
#ifdef	REPLY_PORT_ALIAS
	    reply_hash_enter(&cp->c_select_port,
			     (char *)cp,
			     char_select_read_reply,
			     char_select_write_reply);
#else
	    cp->c_select_port = mach_reply_port();
	    reply_hash_enter(cp->c_select_port,
			     (char *)cp,
			     char_select_read_reply,
			     char_select_write_reply);
#endif
	}

	if (!scanning) {
		select_dequeue(&cp->c_read_selq);
		goto done;
	}

	if (*events & POLLNORM) {
		/*
		 * If reply available, consume it
		 */
		if (cp->c_read_sel_state & CD_SEL_MRCVD) {
		    cp->c_read_sel_state = 0;
		    *revents |= POLLNORM;
		} else {
		  /*
		   * Post read request if not already posted.
		   */
		  if ((cp->c_read_sel_state & CD_SEL_MSENT) == 0) {
			  cp->c_read_sel_state |= CD_SEL_MSENT;
			  rc = device_read_request_inband(cp->c_device_port,
							  cp->c_select_port,
							  0,	/* mode */
							  0,	/* recnum */
							  0);	/* bytes wanted */
			  if (rc != KERN_SUCCESS) {
				  printf("char_select: device_read_request_inband() on dev=(%d,%d) returned 0x%x\n", major(dev), minor(dev), rc);
			  }
		  }
		  enqueue++;
		}
	}
	if (*events & POLLOUT) {
		/*
		 * If reply available, consume it
		 */
		if (cp->c_write_sel_state & CD_SEL_MRCVD) {
		  cp->c_write_sel_state = 0;
		  *revents |= POLLOUT;
		} else {
		  /*
		   * Post write request if not already posted.
		   */
		  if ((cp->c_write_sel_state & CD_SEL_MSENT) == 0) {
			  cp->c_write_sel_state |= CD_SEL_MSENT;
			  rc = device_write_request_inband(cp->c_device_port,
							   cp->c_select_port,
							   0,	/* mode */
							   0,	/* recnum */
							   0,	/* data */
							   0);	/* bytes wanted */
			  if (rc != KERN_SUCCESS) {
				  printf("char_select: device_write_request_inband() on dev=(%d,%d) returned 0x%x\n", major(dev), minor(dev), rc);
			  }
		  }
		  enqueue++;
	      }
	}

	/* Only enqueue if we don't have a response */
	if (enqueue && *revents == 0)
		select_enqueue(&cp->c_read_selq);

done:
	simple_unlock(&cp->c_lock);
	DEVSW_UNFUNNEL(c,major(dev));
	splx(s);
	return (0);
}

/*
 * Handler for c_select_port replies.
 */
char_select_read_reply(cp, error, data, size)
	register char_device_t	*cp;
	int			error;
	char			*data;
	unsigned int		size;
{
	interrupt_enter(SPLTTY);
	simple_lock(&cp->c_lock);
	cp->c_read_sel_state = CD_SEL_MRCVD;
	select_wakeup(&cp->c_read_selq);
	simple_unlock(&cp->c_lock);
	if (cp->c_mode & C_ASYNC)
	    gsignal4(cp->c_pgrp, SIGIO, 0, FALSE);
	interrupt_exit(SPLTTY);
}

char_select_write_reply(cp, error, size)
	register char_device_t	*cp;
	int			error;
	unsigned int		size;
{
	interrupt_enter(SPLTTY);
	simple_lock(&cp->c_lock);
	cp->c_write_sel_state = CD_SEL_MRCVD;
	select_wakeup(&cp->c_write_selq);
	simple_unlock(&cp->c_lock);
	interrupt_exit(SPLTTY);
}

/*
 * Device memory object support.
 */
memory_object_t
device_pager_create(dev, offset, size, protection)
	dev_t		dev;
	vm_offset_t	offset;
	vm_size_t	size;
	vm_prot_t	protection;
{
	mach_port_t	(*d_port_routine)();
	mach_port_t	device_port;
	memory_object_t	pager;
	int		status;

	d_port_routine = cdevsw[major(dev)].d_port;
	if (d_port_routine == (mach_port_t (*)())0)
		return (MACH_PORT_NULL);
	device_port = (*d_port_routine)(dev);
	if (device_port == MACH_PORT_NULL)
		return (MACH_PORT_NULL);

	status = device_map(device_port, protection, offset, 
				   size, &pager, 0); 
	if (status != KERN_SUCCESS) {
		return (MACH_PORT_NULL);
	}
	return (pager);
}

device_pager_release(mem_obj)
	memory_object_t	mem_obj;
{
	kern_return_t kr;

	kr = mach_port_deallocate(mach_task_self(), mem_obj);
	if (kr != KERN_SUCCESS)
		panic("device_pager_release");
}

/*
 * Memory device.
 */
#define M_KMEM		1	/* /dev/kmem - virtual kernel memory & I/O */
#define M_NULL		2	/* /dev/null - EOF & Rathole */

mmopen(dev, flag)
	dev_t	dev;
	int	flag;
{
	switch (minor(dev)) {
	    case M_KMEM:
		return (0);
	    case M_NULL:
		return (ELOCAL);
	}
	return (ENXIO);
}

/*ARGSUSED*/
mmread(dev, node,uio)
	dev_t	dev;
	struct uio *uio;
        node_t  node;
{
	return (mmrw(dev, uio, UIO_READ));
}

mmwrite(dev, node, uio)
	dev_t	dev;
	struct uio *uio;
        node_t    node;
{
	return (mmrw(dev, uio, UIO_WRITE));
}	

mmrw(dev, uio, rw)
	dev_t dev;
	struct uio *uio;
	enum uio_rw rw;

{
	register u_int c;
	register struct iovec *iov;
	int error = 0;

	while (uio->uio_resid > 0 && error == 0) {
		iov = uio->uio_iov;
		if (iov->iov_len == 0) {
			uio->uio_iov++;
			uio->uio_iovcnt--;
			if (uio->uio_iovcnt < 0)
				panic("mmrw");
			continue;
		}
		switch (minor(dev)) {

		case M_KMEM:
			c = iov->iov_len;
			if (mmca((vm_address_t)uio->uio_offset, (vm_size_t)c,
			    rw == UIO_READ ? B_READ : B_WRITE)) {
				error = uiomove((caddr_t)uio->uio_offset,
					(int)c, uio);
				continue;
			}
			return (EFAULT);

		case M_NULL:
			if (rw == UIO_READ)
				return (0);
			c = iov->iov_len;
			iov->iov_base += c;
			iov->iov_len -= c;
			uio->uio_offset += c;
			uio->uio_resid -= c;
			break;

		}
	}
	return (error);
}

/*
 *	Check protection of memory region
 *	Returns true if the given protection is ok.
 */

mmca(address, count, prot)
    vm_address_t address;
    vm_size_t count;
{
    register vm_offset_t	addr;
    vm_offset_t			r_addr;
    vm_size_t			r_size;
    vm_prot_t			r_protection,
				r_max_protection;
    vm_inherit_t		r_inheritance;
    boolean_t			r_is_shared;
    memory_object_name_t	r_object_name;
    vm_offset_t			r_offset;

    addr = address;
    while (addr < address + count) {
	r_addr = addr;
	if (vm_region(mach_task_self(),
		      &r_addr,
		      &r_size,
		      &r_protection,
		      &r_max_protection,
		      &r_inheritance,
		      &r_is_shared,
		      &r_object_name,
		      &r_offset) != KERN_SUCCESS)
	    return (0);

	if (MACH_PORT_VALID(r_object_name))
	    (void) mach_port_deallocate(mach_task_self(), r_object_name);

	/* is there a gap? */
	if (r_addr > addr)
	    return (0);

	/* is this region not readable? */
	if (((prot == B_READ) && ((r_protection & VM_PROT_READ) != VM_PROT_READ)) ||
	    ((prot == B_WRITE) && ((r_protection & VM_PROT_WRITE) != VM_PROT_WRITE)))
	    return (0);

	/* continue to next region */
	addr = r_addr + r_size;
    }
    return (1);
}

int
cdev_name_string(dev, str)
	dev_t	dev;
	char	str[];	/* REF OUT */
{
	int	major_num = major(dev);
	int	minor_num = minor(dev);
	int	d_flag;

	if (major_num < 0 || major_num >= nchrdev)
	    return (ENXIO);

	d_flag = cdevsw[major_num].d_flags;
	if (d_flag & C_MINOR) {
	    /*
	     * Must check minor device number -
	     * not all minors for this device translate to
	     * the same name
	     */
	    return (check_dev(dev, str));
	}
	else {
	    strcpy(str, cdevsw[major_num].d_name);
	    if (d_flag & C_BLOCK(0)) {
		/*
		 * Disk device - break minor into dev/partition
		 */
		char num[3];
		register int part_count;

		part_count = C_BLOCK_GET(d_flag);
		(void)itoa(minor_num/part_count, num);
		strcat(str, num);
		num[0] = 'a' + (minor_num % part_count);
		num[1] = 0;
		strcat(str, num);
	    }
	    else {
		/*
		 * Add minor number
		 */
		char num[4];
		(void)itoa(minor_num, num);
		strcat(str, num);
	    }
	}
	return (0);
}

int
bdev_name_string(dev, str)
	dev_t	dev;
	char	str[];	/* REF OUT */
{
	int	major_num = major(dev);
	int	minor_num = minor(dev);
	char	num[3];
	int	part_count;

	if (major_num < 0 || major_num >= nblkdev)
	    return (ENXIO);

	strcpy(str, bdevsw[major_num].d_name);
	part_count = C_BLOCK_GET(bdevsw[major_num].d_flags);

	if (part_count <= 0) {
	    return (ENXIO);
	}
	(void)itoa(minor_num/part_count, num);
	strcat(str, num);
	num[0] = 'a' + (minor_num % part_count);
	num[1] = '\0';
	strcat(str, num);
	return (0);
}

char *
itoa(num, str)
	int	num;
	char	str[];
{
	char	digits[11];
	register char *dp;
	register char *cp = str;

	if (num == 0) {
	    *cp++ = '0';
	}
	else {
	    dp = digits;
	    while (num) {
		*dp++ = '0' + num % 10;
		num /= 10;
	    }
	    while (dp != digits) {
		*cp++ = *--dp;
	    }
	}
	*cp++ = '\0';
	return(cp);
}

/*
 * Parse root device name into a block device number.
 */
dev_t
parse_root_device(str)
	char	*str;
{
	register char c;
	register char *cp = str;
	char *name_end;
	register int minor_num = 0;
	register int major_num;
	register struct bdevsw *bdp;
	int name_len;

	/*
	 * Find device type name (characters before digit)
	 */
	while ((c = *cp) != '\0' &&
		!(c >= '0' && c <= '9'))
	    cp++;
	name_end = cp;

	name_len = (int)name_end - (int)str; /* string length for lookup */
	for (major_num = 0, bdp = bdevsw;
	     major_num < nblkdev;
	     major_num++, bdp++) {
		if (bdp->d_name && !strncmp(str, bdp->d_name, name_len))
			break;
	}
	if (major_num == nblkdev) {
	    /* not found */
	    return ((dev_t)-1);
        }

	if (c != '\0') {
	    /*
	     * Find minor_num number
	     */
	    while ((c = *cp) != '\0' &&
		    c >= '0' && c <= '9') {
		minor_num = minor_num * 10 + (c - '0');
		cp++;
	    }
	}
	if (c >= 'a' && c <= 'h') {
	    /*
	     * Compute disk minor number according to the number of 
	     * partitions per drive.
	     */
	    minor_num = minor_num * C_BLOCK_GET(bdevsw[major_num].d_flags) +
		        (c - 'a');
	}
	return (makedev(major_num, minor_num));
}
