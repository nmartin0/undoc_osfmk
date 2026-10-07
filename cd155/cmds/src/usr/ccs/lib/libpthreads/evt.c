/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.3
 */

#ifndef	lint
static char rcs_id[] = "$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libpthreads/evt.c,v 1.6 1995/03/17 18:54:24 sean Exp $";
#endif	not lint

/*
 * File: evt.c
 *
 * This file contains the functions to suspend a vp and wake a vp for a
 * specified reason. This is simply a cover for send a message on a port.
 * When a vp is allocated, and event port is also allocated.
 */

#include <pthread.h>
#include <sys/timers.h>
#include <mach/message.h>
#include <errno.h>
#include "internal.h"

#define	SECONDS_TO_MILLISECONDS(s)	((s) * 1000)
#define	SECONDS_TO_NANOSECONDS(s)	((s) * 1000000000)
#define	NANOSECONDS_TO_MILLISECONDS(n)	(((n) + 999999) / 1000000)
#define	MILLISECONDS_TO_NANOSECONDS(m)	((m) * 1000000)

unsigned int
vp_abs_to_mseconds(struct timespec * ts)
{
	unsigned	result;
	int		nsecs;
	int		seconds;
	int		millisecs;
	struct timespec	now;

	if (getclock(TIMEOFDAY, &now) != 0) {
		pthread_internal_error("vp_abs_to_mseconds(): getclock failed");
	}
	seconds = ts->tv_sec - now.tv_sec;
	nsecs = ts->tv_nsec - now.tv_nsec;
	if (nsecs < 0) {
		--seconds;
		nsecs += SECONDS_TO_NANOSECONDS(1);
	}
	/* if time is up return 1 so that the timeout flag will be set
	 * before calling mach_msg().   This happens in vp_event_wait().
	 * When the user calls pthread_cond_timedwait(), and the time
	 * is up by the time we get here, we don't want to return 0
	 * because that would be the same as calling pthread_cond_wait()
	 * -- i.e. without a timeout.
	 */
	millisecs = SECONDS_TO_MILLISECONDS(seconds)
		+ NANOSECONDS_TO_MILLISECONDS(nsecs);

	result = (millisecs > 0) ? millisecs : 1;

	return (result);
}

/*
 * Function:
 *	vp_event_wait
 *
 * Parameters:
 *	vp - the vp structure of the calling thread.
 *	message - a pointer to where to store the event message
 *	timeout - A pointer to a timeout value. This may be NULL.
 *
 * Description:
 *	The action of wating for an event simply involves waiting on
 *	the vp's event port. There may be a timeout specified in
 *	absolute time in the struct timespec. If this timeout is specified
 *	and now then the call returns without waiting on the port.
 */
void
vp_event_wait(vp_t vp, mach_msg_id_t *message, unsigned wait)
{
	kern_return_t		kr;
	mach_msg_header_t	msg;
	mach_msg_option_t	option;
	char			errbuf[1024];

#if	DEBUG2
	PTHREAD_LOG("vp_event_wait: Enter timeout %x\n",timeout);
#endif
#if	DEBUG2
	if ( vp->event_port == MACH_PORT_NULL )
                pthread_internal_error("vp_event_wait: null event port?");
#endif
	/*
	 * set up the message structure
	 */
	bzero(&msg, sizeof(msg));
	msg.msgh_size = sizeof(msg);
	msg.msgh_local_port = vp->event_port;
	msg.msgh_remote_port = MACH_PORT_NULL;

        msg.msgh_bits = MACH_MSGH_BITS(0,0); /* (remote,local) */

	option = wait ? MACH_RCV_MSG | MACH_RCV_TIMEOUT : MACH_RCV_MSG;

	/*
	 * Wait for the event. When it arrives check for a timeout, if
	 * not the event is the message id.
	 */
#if	DEBUG2
	printf("[0x%x] vp_event_wait: mach_msg_recv(port %x)\n",vp,
		vp->event_port);
#endif
	kr = mach_msg(
		&msg,
		option,
		0,
		sizeof(msg),
		vp->event_port,
		(mach_msg_timeout_t)wait,
		MACH_PORT_NULL
	);
	switch (kr) {
	case	MACH_RCV_TIMED_OUT:
		*message = (mach_msg_id_t)EVT_TIMEOUT;
		break;
	case	MACH_MSG_SUCCESS:
		*message = msg.msgh_id;
		break;
	default:
		sprintf(errbuf,"vp_event_wait: msg_recv %s\n",
				mach_error_string(kr));
		pthread_internal_error(errbuf);
	}
#if	DEBUG
	PTHREAD_LOG2("vp_event_wait: exit kr %d mesg 0x%x\n", kr,*message);
#endif
}

/*
 * Function:
 *	vp_event_notify
 *
 * Parameters:
 *	vp - the vp structure of the suspended vp
 *	message - the event that the vp is to receive
 *
 * Description:
 *	The message is sent on the event port of the target vp. The message
 *	is parcelled up in a msg_header and sent as the ipc message id.
 */
void
vp_event_notify(vp_t vp, mach_msg_id_t message)
{
	kern_return_t		kr;
	mach_msg_header_t	msg;
	mach_msg_option_t	options;
	char			errbuf[1024];

	/*
	 * set up the ipc message header. Put the event message in the
	 * ipc message id field.
	 */
	bzero(&msg, sizeof(msg) );

	/* not really sure why msgh_bits is required, if not used the mk
	 * complains about an invalid msg header. stan
	 */
#if 1
	msg.msgh_bits = MACH_MSGH_BITS_REMOTE(MACH_MSG_TYPE_MAKE_SEND);
#else
	msg.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND,0); /* remote,local */
#endif

	msg.msgh_size = sizeof(msg);
	msg.msgh_local_port = MACH_PORT_NULL;
	msg.msgh_remote_port = vp->event_port;
	msg.msgh_kind = MACH_MSGH_KIND_NORMAL;
	msg.msgh_id = message;

	options = MACH_SEND_MSG | MACH_SEND_TIMEOUT;

	/*
	 * Send the message to the target vp and check that the send was good.
	 * It does not matter whether the target vp is blocked yet
	 * or not as this is a queue. We know that it will call
	 * vp_event_wait sometime.
	 */
#if	DEBUG2
	printf("[0x%x] vp_event_notify: send mesg 0x%x port 0x%x\n",
		vp,message,vp->event_port);
#endif
	kr = mach_msg( &msg, options, sizeof(msg), 0, MACH_PORT_NULL, 0,
			MACH_PORT_NULL);

	if (kr != MACH_MSG_SUCCESS && kr != MACH_SEND_TIMED_OUT) {
		sprintf(errbuf,"vp_event_notify: msg_send %s\n",
				mach_error_string(kr));
		pthread_internal_error(errbuf);
	}
#if	DEBUG
	printf("[0x%x] vp_event_notify: exit kr %d\n",vp,kr);
#endif
}

/*
 * Function:
 *	vp_event_flush
 *
 * Parameters:
 *	vp - the vp structure of the calling thread.
 *
 * Description:
 *	We are expecting (normally one) bogus event on the queue caused by
 *	a race between timeouts and cancellation. Get all the messages
 *	and throw them away until there aren't any left.
 */
void
vp_event_flush(vp_t vp)
{
	unsigned	timeout;
	int		message;

#if	DEBUG2
	printf("[0x%x]vp_event_flush: Enter\n",vp);
#endif
	/*
	 * What we really want is an option to msg_receive which returns if
	 * the port is empty but that doesn't exist so we use a timeout instead.
	 */
	timeout = 1;

	do {
		vp_event_wait(vp, &message, timeout);
	} while (message != EVT_TIMEOUT);
#if	DEBUG2
	printf("[0x%x]vp_event_flush: Exit\n",vp);
#endif
}

/*
 * Function:
 *	allocate_event_port
 *
 * Parameters:
 *	port - pointer to where the port id should be stored.
 *
 * Return value:
 *	TRUE	Success
 *	FALSE	The port could not be allocated
 *
 * Description:
 *	Allocate a port and set it up to be used.
 *
 */
int
allocate_event_port(mach_port_t *port)
{
	register mach_port_t	mytask = mach_task_self();
	register kern_return_t	rc;
	char			err[1024];

	/*
	 * allocate a new port on which to receive events
	 */
	if ((rc=mach_port_allocate(mytask,MACH_PORT_RIGHT_RECEIVE,port))
	    != KERN_SUCCESS) {
		set_errno(EAGAIN);
#if	defined(DEBUG) || defined(DEBUG2)
		sprintf(err,"allocate_event_port: mach_port_allocate %s",
			mach_error_string(rc));
		pthread_internal_error(err);
#endif
		return(FALSE);
	}

	/*
	 * Allocate a send right to port
	 */
	if ((rc=mach_port_insert_right(mytask,*port,*port,MACH_MSG_TYPE_MAKE_SEND))
	    != KERN_SUCCESS) {
		set_errno(EAGAIN);
		sprintf(err,"allocate_event_port: mach_port_insert_right %s",
			mach_error_string(rc));
		pthread_internal_error(err);
		return(FALSE);
	}

	/*
	 * only allow one outstanding event to be sent to a thread
	 * at a time.
	 */
	if ((rc=mach_port_set_qlimit(mytask, *port, 1)) != KERN_SUCCESS) {
		sprintf(err,"allocate_event_port: mach_port_set_qlimit %s",
			mach_error_string(rc));
		pthread_internal_error(err);
		return(FALSE);
	}

#ifdef	DEBUG2
	printf("allocate_event_port: port %x\n",*port);
#endif
	return(TRUE);
}

/*
 * Function:
 *	deallocate_event_port
 *
 * Parameters:
 *	port - the id of the port to be deallocated
 *
 * Description:
 *	Return the event port to the system.
 */
void
deallocate_event_port(mach_port_t port)
{
#if	DEBUG2
	printf("deallocate_event_port: port %x\n",port);
#endif
	/*
	 * This may fail as we could be called during the cleanup after
	 * a fork() in which case the port no longer belongs to our task.
	 * So we ignore any error that could occur.
	 */
	mach_port_deallocate(mach_task_self(), port);
}


/*
 * Function:
 *	sleep
 *
 * Parameters:
 *	secs - the number of seconds to sleep
 *
 * Return value:
 *	always 0
 *
 * Description:
 *	wait on the event port with a timeout, knowing that a message will
 *	not arrive
 */
unsigned int
sleep(unsigned int secs)
{
	int		message;
	vp_t		vp;

#if	DEBUG
	printf("[0x%x] sleep: enter\n",vp_self());
#endif

	vp = vp_self();
	vp_event_wait(vp, &message, SECONDS_TO_MILLISECONDS(secs));

#if	DEBUG
	printf("[0x%x] sleep: exit\n",vp_self());
#endif
	return(0);
}
