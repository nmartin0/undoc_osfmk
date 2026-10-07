/*
 *
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 */

/*
 * $Id: conslog.c,v 1.8 1995/03/28 02:53:50 stans Exp $
 *
 * HISTORY
 * $Log: conslog.c,v $
 * Revision 1.8  1995/03/28  02:53:50  stans
 *  Removed code which flushes panic() and assert() messages to the system
 *  system console; called by panic() and assert().
 *
 *  Reviewer:lenb
 *  Risk: medium-high
 *  Benefit or PTS #: 12734
 *  Testing:
 *         Installed & booted kernel on systems: 'a5', XPS150 and testjig.
 *         WW10 sats
 *         Developer tests:
 *                 Verified console logging works for kernel printf()'s at
 *                 interrupt and non-interrupt levels.
 *
 */

#include "cpus.h"
#include "norma_ipc.h"
#include "mach_kdb.h"
#include "mach_assert.h"
#include "console_logging.h"

#if	CONSOLE_LOGGING

#include <device/cirbuf.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/norma_special_ports.h>
#include <mach/mach_types.h>
#include <mach/message.h>
#include <mach/port.h>
#include <ipc/ipc_port.h>
#include <kern/assert.h>
#include <kern/lock.h>
#include <kern/host.h>
#include <kern/ast.h>
#include <i860paragon/conslog.h>
#include <i860paragon/mp.h>

extern int	boot_msg_proc;

/*
 * A console logger to record distributed kernel console output.
 *
 * Kernel printf/panic/assert have been instrumented to call cnputc_logged()
 * rather than calling cnputc() directly.  This allows us to conslog_putc()
 * every character that passes by.  We dump it into conslog_cirbuf.
 * When the NORMA special port 'NORMA_CONSOLE_LOG_PORT' is inserted by the
 * boot-node 'mklogd' daemon we create a conslog thread to drain the buffered
 * conslog_putc() output and send it to 'mklogd'. The daemon will then write
 * the message to syslog and '/dev/console'. If printf() is called from
 * interrupt level a conslog AST is requested. If the message co-processor
 * calls printf() then a conslog doorbell interrupt is delivered to the
 * 'master_cpu' which then requests a conslog AST.
 */

mach_port_t	conslog_port = MACH_PORT_NULL; 
ipc_port_t	conslog_ipc_port; 
thread_t	conslog_thread;

volatile boolean_t conslog_blocked;

static boolean_t	do_conslog = TRUE;
static boolean_t	conslog_initialized;
static void		conslog_putc(int c);

#define	CONSLOG_CIRCULAR_BUFSIZ	4096	/* size of our circular data buffer */

char	conslog_buf[CONSLOG_CIRCULAR_BUFSIZ];	

struct cirbuf conslog_cirbuf;

int
cnputc_logged(c)
{
	if ( do_conslog )
		conslog_putc(c);
	return (cnputc(c));
}

/*
 * disable the console logging machinery
 */
conslog_stop()
{
	do_conslog = FALSE;
}

static void
conslog_init()
{
	/*
 	 * can't use cb_alloc() b/c we're called before kalloc() is initialized.
	 */
	conslog_cirbuf.c_start = conslog_buf;
	conslog_cirbuf.c_end = conslog_buf + CONSLOG_CIRCULAR_BUFSIZ;
	conslog_cirbuf.c_cf = conslog_buf;
	conslog_cirbuf.c_cl = conslog_buf;
	conslog_cirbuf.c_cc = 0;

	conslog_initialized = TRUE;

	/*
	 * make sure we really want to do console logging.
	 */
	do_conslog = getbootint("CONSLOG",TRUE);

	return;
}

static void
conslog_putc(int c)
{
	if (!conslog_initialized)
		conslog_init();

	(void) putc(c, &conslog_cirbuf);
	return;
}

/*
 * send console output to the console logger IPC port.
 *
 * inputs:
 *	express		boolean: True == MACH_SEND_ALWAYS,
 *			 otherwise observe port queue limts.
 *	dest		destination ipc port
 *	msg		console message pointer
 *	msg_data	address of console output data
 *	msg_data_size	# of bytes of output data
 *
 * output:
 *	mach_msg_return_t, MACH_MSG_SUCCESS;
 */

mach_msg_return_t
send_cons_msg(	boolean_t	express,
		mach_port_t	dest,
		cons_msg_t 	msg,
		char 		*msg_data,
		int 		msg_data_size )
{
	mach_msg_type_t		*mt, *mt_start;
	mach_msg_return_t	mm;
	int			send_size;
	int			max_send_size;

	extern mach_msg_return_t
		mach_msg_send_from_kernel(
			mach_msg_header_t	*msg,
			mach_msg_size_t		send_size);

	max_send_size = CONS_DATA_MAX_SIZE;

	/*
	 * send the entire console message in possibly multiple Mach messages.
	 */
	while( msg_data_size > 0 ) {
		if (msg_data_size > max_send_size)
			send_size = max_send_size;
		else
			send_size = msg_data_size;

		msg_data_size -= send_size;

		mt = mt_start = (mach_msg_type_t *)&msg->node_desc;

		/* start with a clean header and data-descriptor */
		bzero(msg, sizeof(mach_msg_header_t) +
				2*sizeof(mach_msg_type_t) + sizeof(unsigned));

		/* Initialize the mach msg header */
		msg->hdr.msgh_remote_port = dest;
		msg->hdr.msgh_local_port = MACH_PORT_NULL;
		msg->hdr.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND,0);

		/*
		 * Initialize inline data descriptor(s) (short form)
		 * set our node number, then data.
		 */
		mt->msgt_inline = 1;
		mt->msgt_name = MACH_MSG_TYPE_INTEGER_32;
		mt->msgt_size = 32;     /* bits/datum */
		mt->msgt_number = 1; /* # ints */
		mt += 2;	/* skip over descriptor & src node num */
		msg->nodeNum = node_self(); 

		/* message data */
		mt->msgt_inline = 1;
		mt->msgt_name = MACH_MSG_TYPE_STRING_C;
		mt->msgt_size = 8;     /* bits/datum */
		mt->msgt_number = send_size; /* # of chars */
		mt++;	/* skip over descriptor, to start of inline data */

		bcopy( msg_data, (char *)mt, send_size);

		msg_data += send_size;	/* advance current data start ptr */

		/* skip over data to next descriptor */
		mt += (send_size / sizeof(mach_msg_type_t)) + 1;

		/*
		 * set correct(rounded-up) size in Mach msg header
		 *
		 * sizeof( Mach-Msg header + body(desc+data,desc+data))
		 */
		msg->hdr.msgh_size = sizeof(mach_msg_header_t) +
			((mt - mt_start) * sizeof(mach_msg_type_t));

		/*
		 * send the console log Mach message
		 */
		if ( express ) {
			/* MACH_SEND_ALWAYS */
			mm = mach_msg_send_from_kernel(
						(mach_msg_header_t *)msg,
						msg->hdr.msgh_size );
		}
		else {	/* observe port queue limits */
			mm = mach_msg(	msg,
					MACH_SEND_MSG,
					msg->hdr.msgh_size,
					0,
					MACH_PORT_NULL,
					MACH_MSG_TIMEOUT_NONE,
					MACH_PORT_NULL);
		}

		if ( mm != MACH_MSG_SUCCESS )
			break;
	}
	return	mm;
}

/*
 * Drain the console log circular-buffer by composing Mach messages and sending
 * them to the norma special port 'conslog'.
 *
 * inputs:
 *	express	(boolean) TRUE == message send obeys port queue limits.
 *			  FALSE == MACH_SEND_ALWAYS 
 outputs:
 *	(void)
 */

conslog_drain( boolean_t express )
{
	register int 		count;
	mach_msg_return_t	kr;
	cons_msg        	cmsg;

	while( (count=conslog_cirbuf.c_cc) > 0 ) {
		/*
		 * send the console output to the console logging port at the.
		 * boot node (mklog daemon). Errors are handled by local
		 * printf() with message data dumped.
		 */
		kr = send_cons_msg(	express,
					conslog_port,
					(cons_msg_t)&cmsg,
					(char *)conslog_cirbuf.c_cf,
					count );
		/*
		 * during the shutdown process or 'mklog stop', the user-mode
		 * logger daemon 'mklogd' can die which will generate the
		 * invalid-destination error.
		 */
		if ( kr != MACH_MSG_SUCCESS ) {
			if ( kr != MACH_SEND_INVALID_DEST ) {
				db_printf("send_cons_msg: failed kr %x cnt %d \n",
					kr,count);
			}
			return;
		}
		ndflush(&conslog_cirbuf, count); /* flush cirbuf */
	}
}

/*
 * Micro-Kernel console logging thread. Drain console log buffer by sending
 * buffer contents to the register norma special port on the boot node. A
 * logging daemon will interface to syslogd there. When the buffer is drained
 * we sleep until wakened from the botton of printf().
 */

console_logger()
{
	register int 	s;

	s = splsched();
	conslog_blocked = FALSE;
	splx (s);

	conslog_drain( FALSE );

	/*
	 * indicate we will be blocking for lack of anything to do.
	 */
	s = splsched();
	conslog_blocked = TRUE;
        assert_wait( &conslog_blocked, FALSE );
	splx (s);

	thread_block( console_logger );

	/* CONTROL DOES NOT RETURN HERE */
}

/*
 * Initialize the console logger thread to a high scheduling priority with a
 * wired stack and the VM privileges to always allocate memory.
 */

conslog_thread_init()
{
	thread_t	self=current_thread();

	thread_set_own_priority( 4 );
	(void) thread_policy( self, POLICY_FIXEDPRI, 5 );
	thread_wire( realhost.host_self, self, TRUE );
	conslog_thread = self;
	console_logger();
}

/*
 * startup the MK console logging thread. Called in response to a norma special
 * port being installed; see norma2/dipc_special.c
 */

void
conslog_thread_startup( ipc_port_t consPort, boolean_t new_port )
{
	extern mach_port_t	task_insert_send_right();

	/*
	 * Really doing console logging?
	 */
	if ( !do_conslog )
		return;

	assert(IP_VALID(consPort));

	/*
	 * Remove the special port identification for the principle port.
	 * The DIPC subsystem will not remove special ports; should the
	 * remote kernel console logger daemon 'mklogd' exit().
	 */
	if ( ! DIPC_IS_PROXY(consPort) )
		consPort->dipc_is_special = FALSE;

	/*
	 * Insert a send-right into the kernel task namespace so the
	 * conslog thread can send messages. Set a global (to this
	 * module) mach_port name which the actual console logger
	 * thread will use.
	 */
	conslog_ipc_port = consPort;
	conslog_port = task_insert_send_right( kernel_task, consPort );

	if ( !MACH_PORT_VALID(conslog_port) ) {
		printf("conslog: insert srights failed for port 0x%x\n",
				consPort);
		return;
	}

	/*
	 * Create a console logger thread.
	 */
	if ( new_port )
		(void) kernel_thread( kernel_task,
				      conslog_thread_init,
				      (char *)0 );
}

/*
 * wakeup the conslog thread if it is blocked; there are characters waiting
 * to be drained.
 */

void
wakeup_conslog_thread()
{
	int	my_cpu = cpu_number();

	/* Still doing console logging? */
	if ( do_conslog == FALSE )
		return;

	/*
	 * ASSUMPTION: if the message co-processor is enabled it is
	 * ALWAYS CPU #1. Don't let the msgp schedule Mach threads....
	 * Ring the master's doorbell indicating conslog output to drain.
	 */
	if ( boot_msg_proc && my_cpu == 1 ) {
		CPU_INT_REASON_SET(MP_CONSLOG_AST,master_cpu);
		cpu_interrupt(master_cpu);
		return;
	}

	/*
	 * skip the recursion.
	 */
	if ( current_thread() == conslog_thread )
		return;

	/*
	 * Make sure we have a valid conslog IPC port; this also assumes a
	 * conslog thread created the conslog port.  If the logger thread is
	 * blocked, then set an AST which will eventually wake it.
	 */
	if ( MACH_PORT_VALID(conslog_port) ) {
		register int	s, blocked;

		s=sploff();
		blocked = conslog_blocked;
		splon(s);
		if ( blocked )
			ast_on(my_cpu,AST_CONSLOG);
	}
}

/*
 * Service the conslog AST.
 *
 * if the conslog thread is not already running, then wake it as there is
 * work to do.
 *
 */

void
conslog_ast()
{
	register int	s, blocked;

	ast_off(cpu_number(), AST_CONSLOG);

	if ( current_thread() == conslog_thread )
		return;

	if ( do_conslog == FALSE )
		return;

	s=sploff();
	blocked = conslog_blocked;
	splon(s);

	if ( blocked )
		thread_wakeup_one( &conslog_blocked );
}

#if 0	/* test harness, will go away */

/*
 * Debug syscall interface: will go away 
 *
 * types of calls
 *
 *	0 == panic() with supplied string.
 *	1 == kernel printf() with supplied string.
 *	2 == assert()
 *	3 == printf() from interrupt level
 *	4 == panic() with supplied string, from interrupt level.
 */
#define	BSZ 2048	/* max size of string from user land */

char		it_msg[200];
int		it_code;
extern int	it_test;

kernel_printf_syscall(char *s, int size, int style)
{
	char	string[BSZ];

	if ( size > BSZ )
		return 1;

	bzero(string,BSZ);
	if ( copyin(s, string, size) != KERN_SUCCESS )
		return 1;

	switch ( style ) {
	  case 0:
		panic("%s",string);
		break;
	  case 1:
		printf("%s",string);
		break;
	  case 2:
		assert( style == 0 );
		break;
	  case 3:
	  case 4:
	  case 5:
		bcopy(string,it_msg,size);
		it_msg[size] = '\0';
		it_code = style;
		it_test = 1;
		break;
	}

	return 0;
}

void
conslog_interrupt_test(int nest_level)
{
	switch( it_code ) {
	  case 3:
		printf("nest %d '%s'",nest_level,it_msg);
		break;
	  case 4:
		panic("nest %d '%s'",nest_level,it_msg);
		break;
	  case 5:
		assert( it_code == 0 );
		break;
	}
}
#endif

#endif	/* CONSOLE_LOGGING */
