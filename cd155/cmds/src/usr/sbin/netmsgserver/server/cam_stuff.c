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
 * cam_stuff.c
 *
 *
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: cam_stuff.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:10:33 $";
#endif

/*
 * Main routines for the camelot support.
 */


#include "config.h"
#if	CAMELOT

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

#include "../camelot/cam.h"
#include "../camelot/global.h"


#define CAM_STUFF_MAX_MSG_SIZE	512
#define CAM_STUFF_NUM_THREADS	1

extern boolean_t ct_server();

/*
 * Global lock to prevent camelot routines from stepping on each other.
 */
struct mutex	camelot_lock;



/*
 * cam_stuff_main
 *	Main loop for ct service.
 *
 * Results:
 *	Should never return.
 *
 * Design:
 *	Wait for a message on the ct port.
 *	Call ct_server to process it.
 *	Send the reply message.
 *
 * Note:
 *	There may be multiple threads executing this main loop.
 *
 */
PRIVATE cam_stuff_main()
BEGIN("cam_stuff_main")
    msg_header_t	*req_msg_ptr, *rep_msg_ptr;
    kern_return_t	kr;
    boolean_t		send_reply;

extern char			my_host_name[40];
extern netaddr_t		my_host_id;

#if	LOCK_THREADS
    pthread_mutex_lock(&thread_lock);
#endif	LOCK_THREADS

    MEM_ALLOC(req_msg_ptr,msg_header_t *,CAM_STUFF_MAX_MSG_SIZE, FALSE);
    MEM_ALLOC(rep_msg_ptr,msg_header_t *,CAM_STUFF_MAX_MSG_SIZE, FALSE);

    while (TRUE) {
	/*
	 * Wait for a name service request.
	 */
	req_msg_ptr->msg_size = CAM_STUFF_MAX_MSG_SIZE;
	req_msg_ptr->msg_local_port = ctPort;
	kr = netmsg_receive(req_msg_ptr);

	if (kr == RCV_SUCCESS) {
		pthread_mutex_lock(&camelot_lock);
		if (!(ct_server((caddr_t)req_msg_ptr, (caddr_t)rep_msg_ptr))) {
			ERROR((msg, "cam_stuff_main.ct_server fails, msg id = %d.", req_msg_ptr->msg_id));
			send_reply = FALSE;
		} else {
			send_reply = (((death_pill_t *)rep_msg_ptr)->RetCode != MIG_NO_REPLY);
		}
		pthread_mutex_unlock(&camelot_lock);
		if (send_reply) {
			/*
			 * Send the reply back.
			 */
			kr = msg_send(rep_msg_ptr, MSG_OPTION_NONE, 0);
			if (kr != SEND_SUCCESS) {
			    ERROR((msg, "cam_stuff_main.msg_send fails, kr = %d.", kr));
			}
		}
	} else {
	    ERROR((msg, "cam_stuff_main.netmsg_receive fails, kr = %d.", kr));
	}

	LOGCHECK;
    }

END



/*
 * cam_stuff_init
 *	Initialises the ct service.
 *
 * Results:
 *	TRUE or FALSE.
 *
 * Design:
 *	Call Cam_Initialize.
 *	Start up some number of threads to handle ct service requests.
 *
 * Note:
 *
 */
EXPORT boolean_t cam_stuff_init()
BEGIN("cam_stuff_init")
    int			i;
    pthread_t		new_thread;

    pthread_mutex_create(&camelot_lock, pthread_mutexattr_default);

    pthread_mutex_lock(&camelot_lock);
    Cam_Initialize();
    pthread_mutex_unlock(&camelot_lock);

    /*
     * ctPort starts by being enabled. If somebody
     * sends a message to it right now, you lose.
     */
    (void)port_disable(task_self(), ctPort);

    for (i = 0; i < CAM_STUFF_NUM_THREADS; i++) {
	if( pthread_create(&new_thread, pthread_att_default, (pthread_func_t)cam_stuff_main, (any_t)0) != 0)
		printf("pthread_create error number %d\n", errno);
	pthread_setname_np(new_thread, "cam_stuff_main");
	pthread_detach(new_thread);
    }

    RETURN(TRUE);

END

#endif	CAMELOT
