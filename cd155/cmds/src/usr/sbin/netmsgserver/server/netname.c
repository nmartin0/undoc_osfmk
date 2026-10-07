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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * netname.c
 *
 *
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: netname.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:11:32 $";
#endif

/*
 * Main routines for the network name service.
 */


#include "config.h"

#if	COMPAT
#define	MACH_INIT_SLOTS		1
#endif	COMPAT

#undef	NET_PRINT
#define	NET_PRINT	1

#include <pthread.h>
#include <sys/errno.h>
#include <mach.h>
#include <mach/boolean.h>
#include <mach/message.h>
#include <sys/types.h>
#include <mig_errors.h>

#include "debug.h"
#include "mem.h"
#include "netmsg.h"
#include "nm_extra.h"
#include "nn_defs.h"

#if	COMPAT
#define	MACH_INIT_SLOTS		1
#include <mach_init.h>
int				compat_pid = 0;
#endif	COMPAT

#if	CAMELOT
extern pthread_mutex_t	camelot_lock;
#endif	CAMELOT

port_t	netname_port;

#define NETNAME_NAME	"NEW_NETWORK_NAME_SERVER"

#define NN_MAX_MSG_SIZE	512
#define NN_NUM_THREADS	1

extern boolean_t netname_server();
extern void nn_procs_init();

/*
 * Memory management definitions.
 */
PUBLIC mem_objrec_t	MEM_NNREC;


/*
 * netname_main
 *	Main loop for network name service.
 *
 * Results:
 *	Should never return.
 *
 * Design:
 *	Wait for a message on the name service port.
 *	Call netname_server to process it.
 *	Send the reply message.
 *
 * Note:
 *	There may be multiple threads executing this main loop.
 *
 */
PRIVATE netname_main()
BEGIN("netname_main")
    msg_header_t	*req_msg_ptr, *rep_msg_ptr;
    kern_return_t	kr;
    boolean_t		send_reply;
    boolean_t		req_ok;

extern char			my_host_name[40];
extern netaddr_t		my_host_id;

#if	LOCK_THREADS
    pthread_mutex_lock(&thread_lock);
#endif	LOCK_THREADS

    MEM_ALLOC(req_msg_ptr,msg_header_t *,NN_MAX_MSG_SIZE, FALSE);

    MEM_ALLOC(rep_msg_ptr,msg_header_t *,NN_MAX_MSG_SIZE, FALSE);

    while (TRUE) {
	/*
	 * Wait for a name service request.
	 */
	req_msg_ptr->msg_size = NN_MAX_MSG_SIZE;
	req_msg_ptr->msg_local_port = netname_port;
	kr = netmsg_receive(req_msg_ptr);

	if (kr == RCV_SUCCESS) {
		req_ok = netname_server((caddr_t)req_msg_ptr, (caddr_t)rep_msg_ptr);
#if	CAMELOT
		if (!req_ok) {
			pthread_mutex_lock(&camelot_lock);
			req_ok = cm_server((caddr_t)req_msg_ptr, (caddr_t)rep_msg_ptr)
				|| cs_server((caddr_t)req_msg_ptr, (caddr_t)rep_msg_ptr)
				|| ca_server((caddr_t)req_msg_ptr, (caddr_t)rep_msg_ptr);
			pthread_mutex_unlock(&camelot_lock);
		}
#endif	CAMELOT
		if (!req_ok) {
			ERROR((msg, "netname_main.netname_server fails, msg id = %d.", req_msg_ptr->msg_id));
			send_reply = FALSE;
		} else {
			send_reply = (((death_pill_t *)rep_msg_ptr)->RetCode != MIG_NO_REPLY);
		}
		if (send_reply) {
			/*
			 * Send the reply back.
			 */
			kr = msg_send(rep_msg_ptr, MSG_OPTION_NONE, 0);
			if (kr != SEND_SUCCESS) {
			    ERROR((msg, "netname_main.msg_send fails, kr = %d.", kr));
			}
		}
	} else {
	    ERROR((msg, "netname_main.netmsg_receive fails, kr = %d.", kr));
	}

	LOGCHECK;
    }

END



/*
 * netname_init
 *	Initialises the network name service.
 *
 * Results:
 *	TRUE or FALSE.
 *
 * Design:
 *	Initialise the name hash table.
 *	Somehow initialise the network name service receive port.
 *	Start up some number of threads to handle name service requests.
 *
 * Note:
 *	First try to do a service_checkin to get a name service port
 *	otherwise we check in a port with the old netmsgserver.
 *
 */
EXPORT boolean_t netname_init()
BEGIN("netname_init")
    kern_return_t	kr;
    int			i;
    pthread_t		new_thread;
#if	COMPAT
    port_t		*init_port_list;
    long		init_port_list_count;
    int			retry;
    msg_header_t	sleep_msg;
    port_t		sleep_port;
#endif	COMPAT

    /*
     * Initialize the memory management facilities.
     */
    mem_initobj(&MEM_NNREC,"Netname record",
				((sizeof(nn_entry_t)) > (sizeof(nn_req_rec_t)) ?
					sizeof(nn_entry_t) : sizeof(nn_req_rec_t)),
				FALSE,170,50);


    nn_procs_init();

    (void)init_service(PORT_NULL);
    if (service_checkin(service_port, name_server_port, &netname_port) == KERN_SUCCESS) {
        if ((kr = _netname_check_in(PORT_NULL, NETNAME_NAME, task_self(), netname_port)) != KERN_SUCCESS)
	{
	    ERROR((msg, "netname_init._netname_check_in fails, kr = %d.", kr));
	    RETURN(FALSE);
	}
	LOG1(TRUE, 5, 1121, netname_port);
    }
    else {
#if	COMPAT
	if (param.compat) {
		ERROR((msg,"Not a primary network server - disabling compatibility mode"));
		param.compat = 0;
	}
#endif	COMPAT
	if ((kr = port_allocate(task_self(), &netname_port)) != KERN_SUCCESS) {
	    ERROR((msg, "netname_init.port_allocate fails, kr = %d.", kr));
	    RETURN(FALSE);
	}
        if ((kr = netname_check_in(name_server_port, NETNAME_NAME, PORT_NULL, netname_port))
		!= KERN_SUCCESS)
	{
	    ERROR((msg, "netname_init.netname_check_in fails, kr = %d.", kr));
	    RETURN(FALSE);
	}
	LOG1(TRUE, 5, 1122, netname_port);
    }

    kr = port_disable(task_self(), netname_port);

#if	COMPAT
	/*
	 * Start an old network server if possible. This old server will
	 * place its name service port in our port array, but that is fine
	 * because we always refer to our name service through netname_port
	 * and never through name_server_port.
	 *
	 * There should already be a process waiting to start the old network
	 * server. Just give it a go-ahead signal.
	 */
	if (param.compat) {
		task_t		compat_task;
		port_t		compat_notify;
		msg_header_t	compat_msg;

		ERROR((msg,"Sending a go-ahead to the old network server process."));
		kr = task_by_unix_pid(task_self(),compat_pid,&compat_task);
		if (kr != KERN_SUCCESS) {
			ERROR((msg,"[COMPAT] task_by_unix_pid failed, kr=%d",kr));
			param.compat = 0;
			goto nocompat;
		}
		kr = task_get_special_port(compat_task,TASK_NOTIFY_PORT,&compat_notify);
		if (kr != KERN_SUCCESS) {
			ERROR((msg,"[COMPAT] task_get_special_port failed, kr=%d",kr));
			param.compat = 0;
			goto nocompat;
		}
		compat_msg.msg_simple = TRUE;
		compat_msg.msg_size = sizeof(msg_header_t);
		compat_msg.msg_type = MSG_TYPE_NORMAL;
		compat_msg.msg_local_port = PORT_NULL;
		compat_msg.msg_remote_port = compat_notify;
		compat_msg.msg_id = 0;
		kr = msg_send(&compat_msg,MSG_OPTION_NONE,0);
		if (kr != KERN_SUCCESS) {
			ERROR((msg,"[COMPAT] msg_send failed, kr=%d",kr));
			param.compat = 0;
			goto nocompat;
		}

		/*
		 * Wait for the old netmsgserver to be ready.
		 */
		kr = port_allocate(task_self(),&sleep_port);
		if (kr != KERN_SUCCESS) {
			ERROR((msg,"[COMPAT] port_allocate(sleep_port) failed, kr=%d",kr));
			param.compat = 0;
			goto nocompat;
		}
		for (retry = 0; retry < 10; retry++) {
			DEBUG0(debug.netname,3,3022);
			kr = mach_ports_lookup(task_self(),
						&init_port_list,
						&init_port_list_count);
			DEBUG1(debug.netname,3,3023,kr);
			if ((kr == KERN_SUCCESS) &&
			    (init_port_list_count > NAME_SERVER_SLOT)) {
				name_server_port = 
					init_port_list[NAME_SERVER_SLOT];
				(void) vm_deallocate(task_self(),
					(vm_address_t)init_port_list,
					vm_page_size);
			}
			if (name_server_port != netname_port)
				break;
			ERROR((msg,"Waiting for old netmsgserver, retry=%d",retry));
			sleep_msg.msg_size = sizeof(msg_header_t);
			sleep_msg.msg_local_port = sleep_port;
			msg_receive(&sleep_msg, RCV_TIMEOUT, 3000);
		}
		if (retry >= 10) {
			ERROR((msg,"Timeout while waiting for old netmsgserver to initialize itself - disabling compatibility mode"));
			param.compat = 0;
		} else {
			ERROR((msg,"Compatibility mode in operation"));
		}
		(void)port_deallocate(task_self(),sleep_port);
	}
nocompat:
#endif	COMPAT

    for (i = 0; i < NN_NUM_THREADS; i++) {
	pthread_create(&new_thread, pthread_attr_default, (pthread_func_t)netname_main, (any_t)0);
	pthread_setname_np(new_thread, "netname_main");
	pthread_detach(new_thread);
    }

    RETURN(TRUE);

END
