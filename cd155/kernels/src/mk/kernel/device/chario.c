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
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 * $Log: chario.c,v $
 * Revision 1.5  1994/11/18  20:30:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:21:32  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:19:48  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:19:25  dleslie
 * First R1_0 release
 *
 * Revision 2.13.3.2  92/04/30  11:49:10  bernadat
 * 	30-Apr-92  emcmanus at gr.osf.org
 * 	Disabled out-of-band notification of inpuflush completion
 * 	due to server problems
 * 	16-Apr-92  emcmanus at gr.osf.org 
 * 	Increase tty_inq_size to 512.  Clear input and output queues on open,
 * 	in case stuff left over from last session.  Send out-of-band
 * 	notification when an input flush has completed.  Added conditional
 * 	debugging code to show tty structures.
 * 
 * 	17-Mar-92  emcmanus at gr.of.org
 * 	Removed incorrect attempt to restart stopped output in ttydrain()
 * 	[92/04/22  09:59:50  bernadat]
 * 
 * Revision 2.13.3.1  92/03/28  10:04:20  jeffreyh
 * 	Increase tty_inq_size from 64 to 256, reducing lost chars at
 * 	higher speeds.  Change Carrier Detect logic on read so that
 * 	status is only checked when there are no chars already buffered.
 * 	Wait for any ongoing output drain to finish before stopping in
 * 	TTY_STOP.
 * 	[92/03/20            emcmanus]
 * 	04-Mar-92  emcmanus at gr.osf.org
 * 		Move common code from char_read and char_read_done into new function
 * 		do_char_read.
 * 		Support for out-of-band events (BREAK and wrong parity):
 * 		quoting in tty input queues, new TF_OUT_OF_BAND bit to enable
 * 		D_OUT_OF_BAND return from device_read, new TTY_OUT_OF_BAND
 * 		device_get_status request.
 * 		Allow TF_HUPCLS to be cleared when it has been set: previously it
 * 		stuck as long as the device was open.
 * 		Output queues can be drained using new TTY_DRAIN device_set_status,
 * 		or as called from device driver.
 * 	[92/03/16  09:42:16  jeffreyh]
 * 
 * Revision 2.13  91/09/12  16:36:51  bohman
 * 	Added missing clear of TS_TTSTOP in char_write().
 * 	TS_INIT belongs in t_state, not t_flags.
 * 	[91/09/11  17:04:18  bohman]
 * 
 * Revision 2.12  91/08/28  11:11:08  jsb
 * 	Fixed char_write to check vm_map_copyout's return code.
 * 	[91/08/03            rpd]
 * 
 * Revision 2.11  91/08/24  11:55:34  af
 * 	Spl definitions.
 * 	[91/08/02  02:44:21  af]
 * 
 * Revision 2.10  91/05/14  15:39:09  mrt
 * 	Correcting copyright
 * 
 * Revision 2.9  91/02/05  17:07:55  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:26:20  mrt]
 * 
 * Revision 2.8  90/08/27  21:54:21  dbg
 * 	Fixed type declaration for char_open.
 * 	[90/07/16            dbg]
 * 
 * 	Added call to cb_alloc in ttychars..
 * 	[90/07/09            dbg]
 * 
 * Revision 2.7  90/06/02  14:47:02  rpd
 * 	Updated for new IPC.  Purged MACH_XP_FPD.
 * 	[90/03/26  21:42:42  rpd]
 * 
 * Revision 2.6  90/01/11  11:41:39  dbg
 * 	Fix test on 'i' (clist exhausted) in char_write.
 * 	Document what operations need locking, and which must be
 * 	serialized if the device driver only runs on one CPU.
 * 	[89/11/30            dbg]
 * 
 * Revision 2.5  89/11/29  14:08:50  af
 * 	char_write wasn't calling b_to_q() with the right char count.
 * 	[89/11/11            af]
 * 	Marked tty as initialized in ttychars.
 * 	[89/11/03  16:57:39  af]
 * 
 * Revision 2.4  89/09/08  11:23:01  dbg
 * 	Add in-band write check to char_write.
 * 	[89/08/30            dbg]
 * 
 * 	Make char_write copy data directly from user instead of wiring
 * 	down data buffer first.
 * 	[89/08/24            dbg]
 * 
 * 	Convert to run in kernel task.
 * 	[89/07/27            dbg]
 * 
 * Revision 2.3  89/08/31  16:17:20  rwd
 * 	Don't assume adequate spl when inserting/deleting ior's from
 * 	delay queues.
 * 	[89/08/31            rwd]
 * 
 * Revision 2.2  89/08/05  16:04:35  rwd
 * 	Added tty_queueempty for sun console input polling.
 * 	Added ttyoutput for sundev/kbd.c.  Allow inband data.
 * 	[89/06/02            rwd]
 * 
 * 18-May-89  David Golub (dbg) at Carnegie-Mellon University
 *	Check for uninitialized TTY queues in close/port_death.
 *
 * 12-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Added port_death routines.
 *
 * 24-Aug-88  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	8/88
/*
 * 	TTY io.
 * 	Compatibility with old TTY device drivers.
 */

#include <mach/kern_return.h>
#include <mach/mig_errors.h>
#include <mach/vm_param.h>
#include <machine/machparam.h>		/* spl definitions */

#include <ipc/ipc_port.h>

#include <kern/lock.h>
#include <kern/queue.h>

#include <vm/vm_map.h>
#include <vm/vm_kern.h>

#include <device/device_types.h>
#include <device/io_req.h>
#include <device/ds_routines.h>

#include <device/tty.h>
#include <machine/machparam.h>	/* for spl */

short	tthiwat[16] =
   { 100,100,100,100,100,100,100,200,200,400,400,400,650,650,1300,2000 };
short	ttlowat[16] =
   {  30, 30, 30, 30, 30, 30, 30, 50, 50,120,120,120,125,125, 125, 125 };

void queue_delayed_reply();	/* forward */
void ttstart();			/* forward */
void ttyflush();		/* forward */
int  ttyinput();		/* forward */
int  ttymodem();		/* forward */
/*
 * Fake 'line discipline' switch for the benefit of old code
 * that wants to call through it.
 */
struct ldisc_switch	linesw[] = {
	{
	    char_read,
	    char_write,
	    ttyinput,
	    ttymodem,
	    (int (*)()) 0	/* only called if t_line != 0 */
	}
};

/*
 * Sizes for input and output circular buffers.
 */
int	tty_inq_size = 512;	/* XXX */
int	tty_outq_size = 256;	/* XXX */

/*
 * Open TTY, waiting for CARR_ON.
 * No locks may be held.
 * May run on any CPU.
 */
io_return_t char_open(dev, tp, mode, ior)
	int		dev;
	register struct tty *tp;
	dev_mode_t	mode;
	register io_req_t	ior;
{
	int	s;

	s = spltty();
	simple_lock(&tp->t_lock);

	tp->t_dev = dev;

	while (getc(&tp->t_inq) >= 0)
	    continue;
	while (getc(&tp->t_outq) >= 0)
	    continue;

	if ((tp->t_state & TS_CARR_ON) == 0) {
	    if (mode & D_NODELAY) {
		tp->t_state |= TS_ONDELAY;
	    }
	    else {
		boolean_t	char_open_done();	/* forward */

		tp->t_state |= TS_WOPEN;

		ior->io_dev_ptr = (char *)tp;

		queue_delayed_reply(&tp->t_delayed_open, ior, char_open_done);
		simple_unlock(&tp->t_lock);
		splx(s);
		return (D_IO_QUEUED);
	    }
	}
	tp->t_state |= TS_ISOPEN;
	simple_unlock(&tp->t_lock);
	splx(s);
	return (D_SUCCESS);
}

/*
 * Retry wait for CARR_ON for open.
 * No locks may be held.
 * May run on any CPU.
 */
boolean_t char_open_done(ior)
	register io_req_t	ior;
{
	register struct tty *tp = (struct tty *)ior->io_dev_ptr;
	int s = spltty();

	simple_lock(&tp->t_lock);
	if ((tp->t_state & TS_ISOPEN) == 0) {
	    queue_delayed_reply(&tp->t_delayed_open, ior, char_open_done);
	    simple_unlock(&tp->t_lock);
	    splx(s);
	    return (FALSE);
	}

	tp->t_state &= ~TS_WOPEN;
	simple_unlock(&tp->t_lock);
	splx(s);

	ior->io_error = D_SUCCESS;
	(void) ds_open_done(ior);
	return (TRUE);
}

boolean_t tty_close_open_reply(ior)
	register io_req_t	ior;
{
	ior->io_error = D_DEVICE_DOWN;
	(void) ds_open_done(ior);
	return (TRUE);
}

/*
 * Write to TTY.
 * No locks may be held.
 * Calls device start routine; must already be on master if
 * device needs to run on master.
 */
io_return_t char_write(tp, ior)
	register struct tty *	tp;
	register io_req_t	ior;
{
	int		s;
	register int	i, c;
	register int	count;
	register char	*data;
	char		obuf[100];
	vm_offset_t	addr;

	data  = ior->io_data;
	count = ior->io_count;
	if (count == 0)
	    return (D_SUCCESS);

	if (!(ior->io_op & IO_INBAND)) {
	    vm_map_copy_t copy = (vm_map_copy_t) data;
	    kern_return_t kr;

	    kr = vm_map_copyout(device_io_map, &addr, copy);
	    if (kr != KERN_SUCCESS)
		return (kr);
	    data = (char *) addr;
	}

	/*
	 * Loop is entered at spl0 and tty unlocked.
	 * All exits from the loop leave spltty and tty locked.
	 */
	while (TRUE) {
	    /*
	     * Copy a block of characters from the user.
	     */
	    c = count;
	    if (c > sizeof(obuf))
		c = sizeof(obuf);

	    /*
	     * Will fault on data.
	     */
	    bcopy(data, obuf, (unsigned) c);

	    /*
	     * Check for tty operating.
	     */
	    s = spltty();
	    simple_lock(&tp->t_lock);

	    if ((tp->t_state & TS_CARR_ON) == 0) {

		if ((tp->t_state & TS_ONDELAY) == 0) {
		    /*
		     * No delayed writes - tell caller that device is down
		     */
		    simple_unlock(&tp->t_lock);
		    splx(s);
		    if (!(ior->io_op & IO_INBAND))
		    (void) vm_deallocate(device_io_map, addr, ior->io_count);
		    return (D_IO_ERROR);
		}

		if (ior->io_mode & D_NOWAIT) {
		    simple_unlock(&tp->t_lock);
		    splx(s);
		    if (!(ior->io_op & IO_INBAND))
		    (void) vm_deallocate(device_io_map, addr, ior->io_count);
		    return (D_WOULD_BLOCK);
		}

	    }

	    /*
	     * If output clist is full, stop now.
	     */
	    if (tp->t_outq.c_cc > TTHIWAT(tp))
		break;

	    /*
	     * Copy data into the output clist.
	     */
	    i = b_to_q(obuf, c, &tp->t_outq);
	    c -= i;

	    data  += c;
	    count -= c;

	    /*
	     * Stop copying if high-water mark exceeded
	     * or clist is full
	     * or data is exhausted.
	     */
	    if (tp->t_outq.c_cc > TTHIWAT(tp) || i != 0 || count == 0)
		break;

	    /*
	     * Unlock tty for the next copy from user.
	     */
	    simple_unlock(&tp->t_lock);
	    splx(s);
	}

	/*
	 * Report the amount not copied, and start hardware output.
	 */
	ior->io_residual = count;
	tp->t_state &= ~TS_TTSTOP;
	ttstart(tp);

	if (tp->t_outq.c_cc > TTHIWAT(tp) ||
	    (tp->t_state & TS_CARR_ON) == 0) {

	    boolean_t	char_write_done();	/* forward */

	    /*
	     * Do not send reply until some characters have been sent.
	     */
	    ior->io_dev_ptr = (char *)tp;
	    queue_delayed_reply(&tp->t_delayed_write, ior, char_write_done);

	    simple_unlock(&tp->t_lock);
	    splx(s);

	    /*
	     * Incoming data has already been copied - delete it.
	     */
	    if (!(ior->io_op & IO_INBAND))
	    (void) vm_deallocate(device_io_map, addr, ior->io_count);
	    return (D_IO_QUEUED);
	}

	simple_unlock(&tp->t_lock);
	splx(s);

	if (!(ior->io_op & IO_INBAND))
	(void) vm_deallocate(device_io_map, addr, ior->io_count);
	return (D_SUCCESS);
}

/*
 * Retry wait for output queue emptied, for write.
 * No locks may be held.
 * May run on any CPU.
 */
boolean_t char_write_done(ior)
	register io_req_t	ior;
{
	register struct tty *tp = (struct tty *)ior->io_dev_ptr;
	register int s = spltty();

	simple_lock(&tp->t_lock);
	if (tp->t_outq.c_cc > TTHIWAT(tp) ||
	    (tp->t_state & TS_CARR_ON) == 0) {

	    queue_delayed_reply(&tp->t_delayed_write, ior, char_write_done);
	    simple_unlock(&tp->t_lock);
	    splx(s);
	    return (FALSE);
	}
	simple_unlock(&tp->t_lock);
	splx(s);

	if (IP_VALID(ior->io_reply_port)) {
	    (void) ds_device_write_reply(ior->io_reply_port,
				ior->io_reply_port_type,
				ior->io_error,
				(int)(ior->io_count - ior->io_residual));
	}
	device_deallocate(ior->io_device);
	return (TRUE);
}

boolean_t tty_close_write_reply(ior)
	register io_req_t	ior;
{
	ior->io_residual = ior->io_count;
	ior->io_error = D_DEVICE_DOWN;
	(void) ds_write_done(ior);
	return (TRUE);
}

/*
 * Read from TTY.
 * No locks may be held.
 * May run on any CPU - does not talk to device driver.
 */
io_return_t char_read(tp, ior)
	register struct tty *tp;
	register io_req_t ior;
{
	int	s;
	kern_return_t	rc;

	/*
	 * Allocate memory for read buffer.
	 */
	rc = device_read_alloc(ior, (vm_size_t)ior->io_count);
	if (rc != KERN_SUCCESS)
	    return (rc);

	s = spltty();
	simple_lock(&tp->t_lock);
	rc = do_char_read(tp, ior);
	simple_unlock(&tp->t_lock);
	splx(s);
	return rc;
}


kern_return_t
do_char_read(tp, ior)
	register struct tty *tp;
	register io_req_t ior;
{
	int nread;

	if (tp->t_outofband != TOOB_NO_EVENT)
	    return D_OUT_OF_BAND;

	if (tp->t_inq.c_cc <= 0) {
	    boolean_t	char_read_done();	/* forward */
	    if (!(tp->t_state & TS_CARR_ON)) {
		if (!(tp->t_state & TS_ONDELAY)) {
		    /*
		     * No delayed reads - tell caller that device is down
		     */
		    return D_IO_ERROR;
		} else if (ior->io_mode & D_NOWAIT) {
		    return D_WOULD_BLOCK;
		}
	    }
	    ior->io_dev_ptr = (char *)tp;
	    queue_delayed_reply(&tp->t_delayed_read, ior, char_read_done);
	    return D_IO_QUEUED;
	}
	
	if (tp->t_nquoted > 0) {
	    nread = char_read_quoted(tp, ior->io_data, ior->io_count);
	    if (tp->t_outofband != TOOB_NO_EVENT) {
		if (nread == 0)
		    return D_OUT_OF_BAND;
	    } else {
		nread += q_to_b(&tp->t_inq,
				ior->io_data + nread,
				(int)ior->io_count - nread);
	    }
	} else {
	    /* Degenerate but typical case: we could call char_read_quoted
	     * always, but we avoid the overhead.
	     */
	    nread = q_to_b(&tp->t_inq,
			   ior->io_data,
			   (int)ior->io_count);
	}

	ior->io_residual = ior->io_count - nread;
	return D_SUCCESS;
}


/*
 * Read from TTY when we know there are some quoted characters in there.
 * Return number of characters read; stop when there are no more quoted
 * characters so the efficient q_to_b can be used.
 */
int
char_read_quoted(tp, data, count)
	register struct tty *tp;
	register char *data;
	int count;
{
	int nread = 0;
	int c;
	while (nread < count) {
	    if ((c = getc(&tp->t_inq)) < 0) {
		printf("char_read_quoted: t_nquoted lied\n");
		tp->t_nquoted = 0;
		break;
	    }
	    if (c == TTY_QUOTEC) {
		switch (getc(&tp->t_inq)) {
		case -1:
		    printf("char_read_quoted: quote without quoted\n");
		    break;
		case TQ_QUOTEC:
		    *data++ = TTY_QUOTEC;
		    nread++;
		    break;
		case TQ_BREAK:
		    tp->t_outofband = TOOB_BREAK;
		    goto getarg;
		case TQ_BAD_PARITY:
		    tp->t_outofband = TOOB_BAD_PARITY;
getarg:
		    if ((c = getc(&tp->t_inq)) < 0) {
			printf("char_read_quoted: missing event arg\n");
			tp->t_outofbandarg = 0;
		    } else tp->t_outofbandarg = c;
		    break;
		case TQ_FLUSHED:
		    tp->t_outofband = TOOB_FLUSHED;
		    break;
		default:
		    printf("char_read_quoted: bad quoted value\n");
		    break;
		}
		tp->t_nquoted--;
		if (tp->t_outofband != TOOB_NO_EVENT || tp->t_nquoted == 0)
		    break;
	    } else {
		*data++ = c;
		nread++;
	    }
	}
	return nread;
}

/*
 * Retry wait for characters, for read.
 * No locks may be held.
 * May run on any CPU - does not talk to device driver.
 */
boolean_t char_read_done(ior)
	register io_req_t	ior;
{
	register struct tty *tp = (struct tty *)ior->io_dev_ptr;
	register int s = spltty();
	kern_return_t rc;

	simple_lock(&tp->t_lock);

	rc = do_char_read(tp, ior);

	simple_unlock(&tp->t_lock);
	splx(s);

	if (rc == D_IO_QUEUED)
	    return (FALSE);

	ior->io_error = rc;

	(void) ds_read_done(ior);
	return (TRUE);
}

boolean_t tty_close_read_reply(ior)
	register io_req_t	ior;
{
	ior->io_residual = ior->io_count;
	ior->io_error = D_DEVICE_DOWN;
	(void) ds_read_done(ior);
	return (TRUE);
}

/* Drain output queue of tp.  Call at spltty with tty locked. */
ttydrain(tp)
	struct tty *tp;
{
	struct io_req iorq;
	simple_lock_init(&iorq.io_req_lock);
	while (tp->t_outq.c_cc) {
		iorq.io_op = 0;
		queue_delayed_reply(&tp->t_delayed_write, &iorq,
				    (boolean_t (*)()) 0);
		tp->t_state |= TS_ASLEEP;
		thread_sleep((int) &iorq, simple_lock_addr(tp->t_lock), FALSE);
		simple_lock(&tp->t_lock);
		tp->t_state &= ~TS_ASLEEP;
	}
}

/*
 * Close the tty.
 * Tty must be locked (at spltty).
 * May run on any CPU.
 */
ttyclose(tp)
	register struct tty *tp;
{
	register io_req_t	ior;

	/*
	 * Flush the read and write queues.  Signal
	 * the open queue so that those waiting for open
	 * to complete will see that the tty is closed.
	 */
	while ((ior = (io_req_t)dequeue_head(&tp->t_delayed_read)) != 0) {
	    ior->io_done = tty_close_read_reply;
	    iodone(ior);
	}
	while ((ior = (io_req_t)dequeue_head(&tp->t_delayed_write)) != 0) {
	    ior->io_done = tty_close_write_reply;
	    iodone(ior);
	}
	while ((ior = (io_req_t)dequeue_head(&tp->t_delayed_open)) != 0) {
	    ior->io_done = tty_close_open_reply;
	    iodone(ior);
	}
	tp->t_state = 0;
}

/*
 * Port-death routine to clean up reply messages.
 */
boolean_t
tty_queue_clean(q, port, routine)
	register queue_t	q;
	register ipc_port_t	port;
	boolean_t		(*routine)();
{
	register io_req_t	ior;

	ior = (io_req_t)queue_first(q);
	while (!queue_end(q, (queue_entry_t)ior)) {
	    if (ior->io_reply_port == port) {
		remqueue(q, (queue_entry_t)ior);
		ior->io_done = routine;
		iodone(ior);
		return (TRUE);
	    }
	    ior = ior->io_next;
	}
	return (FALSE);
}

/*
 * Handle port-death (dead reply port) for tty.
 * No locks may be held.
 * May run on any CPU.
 */
boolean_t
tty_portdeath(tp, port)
	register struct tty *tp;
	ipc_port_t	port;
{
	register int	spl = spltty();
	register boolean_t	result;

	simple_lock(&tp->t_lock);

	/*
	 * The queues may never have been initialized
	 */
	if (tp->t_delayed_read.next == 0) {
	    result = FALSE;
	}
	else {
	    result =
		tty_queue_clean(&tp->t_delayed_read,  port,
				tty_close_read_reply)
	     || tty_queue_clean(&tp->t_delayed_write, port,
				tty_close_write_reply)
	     || tty_queue_clean(&tp->t_delayed_open,  port,
				tty_close_open_reply);
	}
	simple_unlock(&tp->t_lock);
	splx(spl);

	return (result);
}

/*
 * Get TTY status.
 * No locks may be held.
 * May run on any CPU.
 */
io_return_t tty_get_status(tp, flavor, data, count)
	register struct tty *tp;
	int		flavor;
	int *		data;		/* pointer to OUT array */
	unsigned int	*count;		/* out */
{
	int	s;

	switch (flavor) {
	    case TTY_STATUS:
	    {
		register struct tty_status *tsp =
			(struct tty_status *) data;

		s = spltty();
		simple_lock(&tp->t_lock);

		tsp->tt_ispeed = tp->t_ispeed;
		tsp->tt_ospeed = tp->t_ospeed;
		tsp->tt_breakc = tp->t_breakc;
		tsp->tt_flags  = tp->t_flags;
		if (tp->t_state & TS_HUPCLS)
		    tsp->tt_flags |= TF_HUPCLS;

		simple_unlock(&tp->t_lock);
		splx(s);

		*count = TTY_STATUS_COUNT;
		break;

	    }
	    case TTY_OUT_OF_BAND:
	    {
		register struct tty_out_of_band *toobp =
			(struct tty_out_of_band *) data;

		s = spltty();
		simple_lock(&tp->t_lock);

		toobp->toob_event = tp->t_outofband;
		toobp->toob_arg = tp->t_outofbandarg;
		tp->t_outofband = TOOB_NO_EVENT;

		simple_unlock(&tp->t_lock);
		splx(s);

		*count = TTY_OUT_OF_BAND_COUNT;
		break;

	    }
	    default:
		return (D_INVALID_OPERATION);
	}
	return (D_SUCCESS);
}

/*
 * Set TTY status.
 * No locks may be held.
 * Calls device start or stop routines; must already be on master if
 * device needs to run on master.
 */
io_return_t tty_set_status(tp, flavor, data, count)
	register struct tty *tp;
	int		flavor;
	int *		data;
	unsigned int	count;
{
	int	s;

	switch (flavor) {
	    case TTY_FLUSH:
	    {
		register int	flags;
		if (count < TTY_FLUSH_COUNT)
		    return (D_INVALID_OPERATION);

		flags = *data;
		if (flags == 0)
		    flags = D_READ | D_WRITE;
		else
		    flags &= (D_READ | D_WRITE);
		ttyflush(tp, flags);
		break;
	    }
	    case TTY_STOP:
		/* stop output */
		s = spltty();
		simple_lock(&tp->t_lock);
		if ((tp->t_state & TS_TTSTOP) == 0) {
		    /* If we're draining output, stopping now could lead
		       to deadlock.  Wait for the drain to finish. XXX */
		    while (tp->t_state & TS_ASLEEP) {
			ttydrain(tp);
			ttstart(tp);
		    }
		    tp->t_state |= TS_TTSTOP;
		    (*tp->t_stop)(tp, 0);
		}
		simple_unlock(&tp->t_lock);
		splx(s);
		break;

	    case TTY_START:
		/* start output */
		s = spltty();
		simple_lock(&tp->t_lock);
		if (tp->t_state & TS_TTSTOP) {
		    tp->t_state &= ~TS_TTSTOP;
		    ttstart(tp);
		}
		simple_unlock(&tp->t_lock);
		splx(s);
		break;

	    case TTY_STATUS:
		/* set special characters and speed */
	    {
		register struct tty_status *tsp;

		if (count < TTY_STATUS_COUNT)
		    return (D_INVALID_OPERATION);

		tsp = (struct tty_status *)data;

		s = spltty();
		simple_lock(&tp->t_lock);

		tp->t_ispeed = tsp->tt_ispeed;
		tp->t_ospeed = tsp->tt_ospeed;
		tp->t_breakc = tsp->tt_breakc;
		tp->t_flags  = tsp->tt_flags & ~TF_HUPCLS;
		if (tsp->tt_flags & TF_HUPCLS)
		    tp->t_state |= TS_HUPCLS;
		else tp->t_state &= ~TS_HUPCLS;

		simple_unlock(&tp->t_lock);
		splx(s);
		break;
	    }
	    case TTY_DRAIN:
		s = spltty();
		simple_lock(&tp->t_lock);
		ttydrain(tp);
		simple_unlock(&tp->t_lock);
		splx(s);
		break;
	    default:
		return (D_INVALID_OPERATION);
	}
	return (D_SUCCESS);
}


/*
 * [internal]
 * Queue IOR on reply queue, to wait for TTY operation.
 * TTY must be locked (at spltty).
 */
void queue_delayed_reply(qh, ior, io_done)
	queue_t		qh;
	io_req_t	ior;
	boolean_t	(*io_done)();
{
	ior->io_done = io_done;
	enqueue_tail(qh, (queue_entry_t)ior);
}

/*
 * Retry delayed IO operations for TTY.
 * TTY containing queue must be locked (at spltty).
 */
void tty_queue_completion(qh)
	register queue_t	qh;
{
	register io_req_t	ior;

	while ((ior = (io_req_t)dequeue_head(qh)) != 0) {
	    iodone(ior);
	}
}

/*
 * Set the default special characters.
 * Since this routine is called whenever a tty has never been opened,
 * we can initialize the queues here.
 */
ttychars(tp)
	register struct tty *tp;
{
	if ((tp->t_flags & TS_INIT) == 0) {
	    /*
	     * Initialize queues
	     */
	    queue_init(&tp->t_delayed_open);
	    queue_init(&tp->t_delayed_read);
	    queue_init(&tp->t_delayed_write);

	    /*
	     * Initialize character buffers
	     */
	    cb_alloc(&tp->t_inq,  tty_inq_size);
	    cb_alloc(&tp->t_outq, tty_outq_size);

	    tp->t_nquoted = 0;
	    tp->t_outofband = TOOB_NO_EVENT;

	    /*
	     * Mark initialized
	     */
	    tp->t_state |= TS_INIT;
	}

	tp->t_breakc = 0;
}

/*
 * Flush all TTY queues.
 * No locks may be held.
 * Calls device STOP routine; must already be on master if
 * device needs to run on master.
 */
void ttyflush(tp, rw)
	register struct tty *tp;
	int	rw;
{
	register int s = spltty();
	simple_lock(&tp->t_lock);

	if (rw & D_READ) {
	    while (getc(&tp->t_inq) >= 0)
		continue;
	    tty_queue_completion(&tp->t_delayed_read);
	}
	if (rw & D_WRITE) {
	    tp->t_state &= ~TS_TTSTOP;
	    (*tp->t_stop)(tp, rw);
	    while (getc(&tp->t_outq) >= 0)
		continue;
	    tty_queue_completion(&tp->t_delayed_write);
	}
	if (rw & D_READ) {
	    while (getc(&tp->t_inq) >= 0)
		continue;
	    tp->t_nquoted = 0;
#if 0
	    if (tp->t_flags & TF_OUT_OF_BAND)
		ttyquoted(TQ_FLUSHED, -1, tp);
#endif 0
	}
	simple_unlock(&tp->t_lock);
	splx(s);
}
		
/*
 * Restart typewriter output following a delay
 * timeout.
 * The name of the routine is passed to the timeout
 * subroutine and it is called during a clock interrupt.
 *
 * Calls device START routine - must be on master.
 *
 * XXX called from softclock, which only runs on master.
 *     What if device only runs on a different CPU?
 */
ttrstrt(tp)
	register struct tty *tp;
{
	register int s;

	if (tp == 0)
		panic("ttrstrt");

	s = spltty();
	simple_lock(&tp->t_lock);

	tp->t_state &= ~TS_TIMEOUT;
	ttstart(tp);

	simple_unlock(&tp->t_lock);
        splx(s);
}

/*
 * Start output on the typewriter. It is used from the top half
 * after some characters have been put on the output queue,
 * from the interrupt routine to transmit the next
 * character, and after a timeout has finished.
 *
 * Called at spltty, tty already locked.
 * Must be on master CPU if device runs on master.
 */
void ttstart(tp)
	register struct tty *tp;
{
	if ((tp->t_state & (TS_TIMEOUT|TS_TTSTOP|TS_BUSY)) == 0) {
	    /*
	     * Start up the hardware again
	     */
	    (*tp->t_start)(tp);

	    /*
	     * Wake up those waiting for write completion.
	     */
	    if (tp->t_outq.c_cc <= TTLOWAT(tp))
		tty_queue_completion(&tp->t_delayed_write);
	}
}


/*
 * Add quoted character of type qc to input queue.  Tty is locked.
 */
ttyquoted(qc, qarg, tp)
	int qc, qarg;
	register struct tty	*tp;
{
	int spaceneeded = (qarg >= 0) ? 3 : 2;
	if (cb_space(&tp->t_inq) >= spaceneeded) {
	    if (putc(TTY_QUOTEC, &tp->t_inq) || putc(qc, &tp->t_inq) ||
		(qarg >= 0 && putc(qarg, &tp->t_inq)))
		panic("ttyquoted: cb_space lied");
	    tp->t_nquoted++;
	    tty_queue_completion(&tp->t_delayed_read);
	}
}


/*
 * Put input character on input queue.
 *
 * Called at spltty, tty already locked.
 */
ttyinput(c, tp)
	register int	c;
	register struct tty	*tp;
{
	c &= 0377;

	if (c == TTY_QUOTEC)
	    ttyquoted(TQ_QUOTEC, -1, tp);
	else
	    (void) putc(c, &tp->t_inq);
	tty_queue_completion(&tp->t_delayed_read);
}


/*
 * Record a received break or framing error, the incorrectly framed
 * character being c.  Called at spltty, tty already locked.
 */
ttybreak(c, tp)
	int c;
	register struct tty	*tp;
{
	if (tp->t_flags & TF_OUT_OF_BAND)
	    ttyquoted(TQ_BREAK, c, tp);
	else ttyinput(tp->t_breakc, tp);
}


ttyinputbadparity(c, tp)
	int c;
	register struct tty	*tp;
{
	if (tp->t_flags & TF_INPCK) {
	    if (tp->t_flags & TF_OUT_OF_BAND)
		ttyquoted(TQ_BAD_PARITY, c, tp);
	    /* else drop the character. */
	} else ttyinput(c, tp);
}


/*
 * Check if anything is on the input queue.
 * Called at spltty, tty already locked.
 *
 * Needed by sun console driver with prom monitor
 */

boolean_t tty_queueempty(tp, queue)
	register struct tty *tp;
	int queue;
{
	if (queue & D_READ) return (tp->t_inq.c_cc <= 0);
	if (queue & D_WRITE) return (tp->t_outq.c_cc <= 0);
	return (TRUE);
}

/*
 * Handle modem control transition on a tty.
 * Flag indicates new state of carrier.
 * Returns 0 if the line should be turned off, otherwise 1.
 *
 * Called at spltty, tty already locked.
 */
int ttymodem(tp, flag)
	register struct tty *tp;
	int	flag;
{

	if ((tp->t_state&TS_WOPEN) == 0 && (tp->t_flags & MDMBUF)) {
		/*
		 * MDMBUF: do flow control according to carrier flag
		 */
		if (flag) {
			tp->t_state &= ~TS_TTSTOP;
			ttstart(tp);
		} else if ((tp->t_state&TS_TTSTOP) == 0) {
			/* If we're draining output, stopping now could lead
			   to deadlock.  Wait for the drain to finish. XXX */
			while (tp->t_state & TS_ASLEEP)
				ttydrain(tp);
			tp->t_state |= TS_TTSTOP;
			(*tp->t_stop)(tp, 0);
		}
	} else if (flag == 0) {
		/*
		 * Lost carrier.
		 */
		tp->t_state &= ~TS_CARR_ON;
		if (tp->t_state & TS_ISOPEN) {
			if ((tp->t_flags & NOHANG) == 0) {
/*
				gsignal(tp->t_pgrp, SIGHUP);
				gsignal(tp->t_pgrp, SIGCONT);
				ttyflush(tp, FREAD|FWRITE);
 */
				return (FALSE);
			}
		}
	} else {
		/*
		 * Carrier now on.
		 */
		tp->t_state |= TS_CARR_ON;
		tt_open_wakeup(tp);
	}
	return (TRUE);
}

ttyoutput(c, tp)
	char c;
	register struct tty *tp;
{
	register int s;

	s = spltty();
	simple_lock(&tp->t_lock);
	(void) putc(c, &tp->t_outq);
	ttstart(tp);
	simple_unlock(&tp->t_lock);
	splx(s);
}

#ifdef TTY_DEBUG	/* showtty() function. */

struct flagnames {
    char *name;
    int bit;
};

struct flagnames tty_state_names[] = {
    "INIT", TS_INIT, "TIMEOUT", TS_TIMEOUT, "WOPEN", TS_WOPEN,
    "ISOPEN", TS_ISOPEN, "FLUSH", TS_FLUSH, "CARR_ON", TS_CARR_ON,
    "BUSY", TS_BUSY, "ASLEEP", TS_ASLEEP, "TTSTOP", TS_TTSTOP,
    "HUPCLS", TS_HUPCLS, "TBLOCK", TS_TBLOCK, "NBIO", TS_NBIO,
    "ONDELAY", TS_ONDELAY, "OUT", TS_OUT, "TRANSLATE", TS_TRANSLATE,
    "KDB", TS_KDB, 0, 0,
};

struct flagnames tty_flag_names[] = {
    "TANDEM", TF_TANDEM, "ODDP", TF_ODDP, "EVENP", TF_EVENP,
    "LITOUT", TF_LITOUT, "MDMBUF", TF_MDMBUF, "NOHANG", TF_NOHANG,
    "HUPCLS", TF_HUPCLS, "ECHO", TF_ECHO, "CRMOD", TF_CRMOD,
    "XTABS", TF_XTABS, "OUT_OF_BAND", TF_OUT_OF_BAND, "INPCK", TF_INPCK,
    0, 0,
};

showflags(name, bits, names)
char *name;
int bits;
struct flagnames *names;
{
    char *sep = "";
    printf("%s: %x <", name, bits);
    for ( ; names->name; names++) {
	if (bits & names->bit) {
	    printf("%s%s", sep, names->name);
	    sep = "|";
	    bits &= ~names->bit;
	}
    }
    if (bits)
	printf("%s%x", sep, bits);
    printf(">; ");
}

showtty(tp)
struct tty *tp;
{
    printf("tty %x: ", tp);
    showcirbuf("input", &tp->t_inq);
    showcirbuf("output", &tp->t_outq);
    printf("addr %x; dev %x; start %x; stop %x; ispeed %d; ospeed %d; breakc %x; ",
	   tp->t_addr, tp->t_dev, tp->t_start, tp->t_stop, tp->t_ispeed,
	   tp->t_ospeed, tp->t_breakc);
    showflags("flags", tp->t_flags, tty_flag_names);
    showflags("state", tp->t_state, tty_state_names);
    printf("line %d; outofband %d; outofbandarg %x; nquoted %d; ",
	   tp->t_line, tp->t_outofband, tp->t_outofbandarg, tp->t_nquoted);
    showqueue("read q", &tp->t_delayed_read);
    showqueue("write q", &tp->t_delayed_write);
    showqueue("open q", &tp->t_delayed_open);
    printf("getstat %x; setstat %x; tops %x; ", tp->t_getstat, tp->t_setstat,
	   tp->t_tops);
}

showqueue(name, q)
char *name;
queue_head_t *q;
{
    queue_chain_t *p;
    int n;
    printf("%s: head %x, queue:", name, q);
    if (q->next == q)
	printf(" empty");
    else {
	for (p = q->next, n = 0; p != q && n < 10; p = p->next, n++)
	    printf(" %x", p);
	if (p != q)
	    printf("...");	/* Probably a loop in a bogus tty struct. */
    }
    printf("; ");
}

showcirbuf(name, q)
char *name;
struct cirbuf *q;
{
    printf("%s: %d chars; ", name, q->c_cc);	/* Could do better. */
}
#endif /* TTY_DEBUG */
