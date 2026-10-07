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
 * $Log: device_reply_hdlr.c,v $
 * Revision 1.9  1995/02/10  23:57:23  stans
 *  'lint' picking: void device_reply_loop() changed to any_t device_reply_loop().
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW05 sats
 *
 * Revision 1.8  1994/11/18  20:47:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/18  00:18:31  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.6  1994/01/12  17:47:11  jlitvin
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
 * Revision 1.5  1993/10/28  03:14:33  yazz
 * Augment panic() mesage to include affected port name.
 *
 * Revision 1.4  1993/07/14  18:40:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:00:05  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:26:26  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:53:40  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:42:34  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:51:22  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:51:23  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.5  1993/04/13  18:04:59  roy
 * 	Added device_read_reply_overwrite.
 * 	[93/04/09            roy]
 *
 * Revision 2.4  93/04/08  11:38:52  loverso
 * 	Revision 1.2.3.2  1992/08/06  17:02:22  emcmanus
 * 	Return FALSE from reply_hash_lookup if given a dead name.
 * 	[1992/08/06  13:59:54  emcmanus]
 * 
 * 	ux server threads are wired by default. (loverso)
 * 
 * Revision 2.3  92/03/09  13:30:55  durriya
 * 	92/02/25  17:48:36  condict
 * 	Change all calls to cthread_wire to ux_thread_wire, so ux_server_loop
 * 	can correctly compute required number of Mach kernel threads.
 * 
 * 	92/01/15  16:55:31  jose
 * 	Added reply_port_alias external for debug.
 * 
 * 	92/01/07  23:35:28  condict
 * 	Fix bug.  Was zfreeing uninitialized re variable.
 * 
 * 	91/12/27  17:26:17  jose
 * 	Added port aliasing for performance
 * 
 * Revision 2.2  91/08/31  14:25:32  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/06/27  15:52:18  sp
 * Use OSF/1 zinit interface
 * 
 * Revision 3.1  91/06/25  17:11:57  condict
 * Moved sys header files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.0  91/01/17  12:05:57  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.7  90/11/05  16:57:19  rpd
 * 	Increased NREPLY_HASH to 64, changed REPLY_HASH to something
 * 	reasonable.  Added per-bucket locks and counts.
 * 	[90/10/31            rpd]
 * 
 * Revision 2.6  90/06/02  15:27:24  rpd
 * 	Updated set_thread_priority call for the new priority range.
 * 	[90/04/23            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  20:12:33  rpd]
 * 
 * Revision 2.5  89/12/08  20:16:38  rwd
 * 	Call ux_thread_wire before setting thread priority.
 * 	[89/11/01            rwd]
 * 
 * Revision 2.4  89/09/15  15:29:26  rwd
 * 	Change includes
 * 	[89/09/11            rwd]
 * 
 * Revision 2.3  89/08/31  16:29:18  rwd
 * 	Added device_read_reply_inband
 * 	[89/08/15            rwd]
 * 
 * Revision 2.2  89/08/09  14:46:05  rwd
 * 	Added copyright to file by dbg.  Added device_write_reply_inband.
 * 	[89/07/17            rwd]
 * 
 */
/*
 * Handler for device read and write replies.  Simulates an
 * interrupt handler.
 */
#include <kern/queue.h>
#include <kern/zalloc.h>

#include <uxkern/import_mach.h>
#include <uxkern/device.h>

#include <uxkern/device_reply_hdlr.h>

#ifndef	REPLY_PORT_ALIAS
#define	NREPLY_HASH	64

/*
 *	We add the low 8 bits to the high 24 bits, because
 *	this interacts well with the way the IPC implementation
 *	allocates port names.
 */

#define	REPLY_HASH(port)	\
	((((port) & 0xff) + ((port) >> 8)) & (NREPLY_HASH-1))

struct reply_hash {
	struct mutex lock;
	queue_head_t queue;
	int count;
} reply_hash[NREPLY_HASH];
#endif	/* ! REPLY_PORT_ALIAS */

struct reply_entry {
	queue_chain_t	chain;
	mach_port_t	port;
	char *		object;
	kr_fn_t		read_reply;
	kr_fn_t		write_reply;
};
typedef struct reply_entry *reply_entry_t;

zone_t	reply_entry_zone;

mach_port_t	reply_port_set;

void	device_reply_loop();	/* forward */

extern void	ux_create_thread();

enum dev_reply_select {
	DR_READ,
	DR_WRITE
};
typedef enum dev_reply_select	dr_select_t;

void
reply_hash_enter(port, object, read_reply, write_reply)
#ifdef REPLY_PORT_ALIAS
	mach_port_t	*port;		/* OUT */
#else
	mach_port_t	port;
#endif
	char		*object;
	kr_fn_t		read_reply, write_reply;
{
	register reply_entry_t	re;
#ifndef REPLY_PORT_ALIAS
	register struct reply_hash *b;
#endif
	kern_return_t ret;

	re = (reply_entry_t) zalloc(reply_entry_zone);
	if (re == (reply_entry_t)0)
		panic("reply_hash_enter: NULL address");
#ifdef REPLY_PORT_ALIAS
	if ((ret = mach_port_allocate_name(mach_task_self(),
			MACH_PORT_RIGHT_RECEIVE,
			(mach_port_t)re)) != KERN_SUCCESS) {
		panic("reply_hash_enter: can't alloc reply port "
				"ret=0x%x name=0x%x", ret, re);
	}
	*port = re->port = (mach_port_t)re;
	(void) mach_port_move_member(mach_task_self(), re->port, reply_port_set);
#else
	re->port = port;
#endif

	re->object = object;
	re->read_reply = read_reply;
	re->write_reply = write_reply;

#ifndef REPLY_PORT_ALIAS
	b = &reply_hash[REPLY_HASH(port)];
	mutex_lock(&b->lock);
	queue_enter(&b->queue, re, reply_entry_t, chain);
	b->count++;
	mutex_unlock(&b->lock);
	(void) mach_port_move_member(mach_task_self(), port, reply_port_set);
#endif
}

boolean_t
reply_hash_lookup(port, which, object, func)
	mach_port_t	port;
	dr_select_t	which;
	char		**object;	/* OUT */
	kr_fn_t		*func;		/* OUT */
{
#ifdef REPLY_PORT_ALIAS
	register reply_entry_t	re;

	if (port == MACH_PORT_DEAD)
		return (FALSE);

	re = (reply_entry_t)port;

	if (re->port != port) {
		panic("reply_hash_lookup: bad reply port");
	}
	*object = re->object;
	*func   = (which == DR_WRITE) ? re->write_reply
		: re->read_reply;

	return (TRUE);
#else
	register reply_entry_t	re;
	register struct reply_hash *b;

	b = &reply_hash[REPLY_HASH(port)];
	mutex_lock(&b->lock);
	for (re = (reply_entry_t)queue_first(&b->queue);
	     !queue_end(&b->queue, (queue_entry_t)re);
	     re = (reply_entry_t)queue_next(&re->chain)) {
	    if (re->port == port) {
		*object = re->object;
		*func   = (which == DR_WRITE) ? re->write_reply
					      : re->read_reply;
		mutex_unlock(&b->lock);
		return (TRUE);
	    }
	}
	mutex_unlock(&b->lock);
	return (FALSE);
#endif
}

void
reply_hash_remove(port)
	mach_port_t	port;
{
	register reply_entry_t	re;
#ifndef REPLY_PORT_ALIAS
	register struct reply_hash *b;
#endif

	(void) mach_port_move_member(mach_task_self(), port, MACH_PORT_NULL);
#ifdef REPLY_PORT_ALIAS
	re = (reply_entry_t)port;
	if (re->port != port) {
		panic("reply_hash_remove: bad reply port");
	}
	zfree(reply_entry_zone, (vm_offset_t)re);
#else
	b = &reply_hash[REPLY_HASH(port)];
	mutex_lock(&b->lock);
	for (re = (reply_entry_t)queue_first(&b->queue);
	     !queue_end(&b->queue, (queue_entry_t)re);
	     re = (reply_entry_t)queue_next(&re->chain)) {
	    if (re->port == port) {
		queue_remove(&b->queue, re, reply_entry_t, chain);
		b->count--;
		mutex_unlock(&b->lock);
		zfree(reply_entry_zone, (vm_offset_t)re);
		return;
	    }
	}
	mutex_unlock(&b->lock);
#endif
}

void device_reply_hdlr()
{
#ifndef REPLY_PORT_ALIAS
	register int	i;

	for (i = 0; i < NREPLY_HASH; i++) {
	    mutex_init(&reply_hash[i].lock);
	    queue_init(&reply_hash[i].queue);
	    reply_hash[i].count = 0;
	}
#endif

	reply_entry_zone =
		zinit((vm_size_t)sizeof(struct reply_entry),
		      (vm_size_t)sizeof(struct reply_entry) * 4096,
		      vm_page_size,
		      "device_reply_port to device");
	(void) mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_PORT_SET,
				  &reply_port_set);

	ux_create_thread(device_reply_loop);
}

void
device_reply_loop(arg)
	any_t	arg;
{
	kern_return_t		rc;

	union reply_msg {
	    mach_msg_header_t	hdr;
	    char		space[8192];
	} reply_msg;

	/*
	 * We KNOW that none of these messages have replies...
	 */
	mig_reply_header_t	rep_rep_msg;

	cthread_set_name(cthread_self(), "device reply handler");

	/*
	 * Make this thread high priority.
	 */
	set_thread_priority(mach_thread_self(), 2);

	for (;;) {
	    rc = mach_msg(&reply_msg.hdr, MACH_RCV_MSG,
			  0, sizeof reply_msg, reply_port_set,
			  MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	    if (rc == MACH_MSG_SUCCESS) {
		if (device_reply_server(&reply_msg.hdr,
					&rep_rep_msg.Head))
		{
		    /*
		     * None of these messages need replies
		     */
		}
	    }
	}
}

kern_return_t
device_open_reply(reply_port, return_code, device_port)
	mach_port_t	reply_port;
	kern_return_t	return_code;
	mach_port_t	device_port;
{
	/* NOT USED */
}

kern_return_t
device_write_reply(reply_port, return_code, bytes_written)
	mach_port_t	reply_port;
	kern_return_t	return_code;
	int		bytes_written;
{
	char		*object;
	kr_fn_t		func;

	if (!reply_hash_lookup(reply_port, DR_WRITE, &object, &func))
	    return(0);

	return ((*func)(object, return_code, bytes_written));
}

kern_return_t
device_write_reply_inband(reply_port, return_code, bytes_written)
	mach_port_t	reply_port;
	kern_return_t	return_code;
	int		bytes_written;
{
	char		*object;
	kr_fn_t		func;

	if (!reply_hash_lookup(reply_port, DR_WRITE, &object, &func))
	    return(0);

	return ((*func)(object, return_code, bytes_written));
}

kern_return_t
device_read_reply(reply_port, return_code, data, data_count)
	mach_port_t	reply_port;
	kern_return_t	return_code;
	io_buf_ptr_t	data;
	unsigned int	data_count;
{
	char		*object;
	kr_fn_t		func;

	if (!reply_hash_lookup(reply_port, DR_READ, &object, &func))
	    return(0);

	return ((*func)(object, return_code, data, data_count));
}

kern_return_t
device_read_reply_inband(reply_port, return_code, data, data_count)
	mach_port_t	reply_port;
	kern_return_t	return_code;
	io_buf_ptr_t	data;
	unsigned int	data_count;
{
	char		*object;
	kr_fn_t		func;

	if (!reply_hash_lookup(reply_port, DR_READ, &object, &func))
	    return(0);

	return ((*func)(object, return_code, data, data_count));
}

kern_return_t
device_read_reply_overwrite(reply_port, return_code, data_count)
	mach_port_t	reply_port;
	kern_return_t	return_code;
	unsigned int	data_count;
{
	char		*object;
	kr_fn_t		func;

	if (!reply_hash_lookup(reply_port, DR_READ, &object, &func))
	    return(0);

	return ((*func)(object, return_code, data_count));
}
