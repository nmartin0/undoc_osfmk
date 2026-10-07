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
 */
/*
 * HISTORY
 * $Log: lnsvr.c,v $
 * Revision 1.7  1994/11/18  20:33:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/05/16  21:38:19  cfj
 * Made the printf in the after to call to mach_server() to
 * only come out if print_debug_messages is non-zero.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.5  1994/05/16  18:13:55  cfj
 * The local nameserver should never return.  The system depends on
 * it in order function.
 *
 *  Reviewer:suri
 *  Risk:L
 *  Benefit or PTS #:9437
 *  Testing:test case
 *  Module(s):server/lnsvr/lnsvr.c
 *
 * Revision 1.4  1993/07/14  18:03:05  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:22:54  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:18:12  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:31:15  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:23:51  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:22:43  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.4  1993/04/08  11:30:13  loverso
 * 	Define a name for our cthread.  (loverso)
 *
 * Revision 2.3  92/07/29  08:31:11  rabii
 * 	Call the local (lnsvr) notify server.  (loverso)
 * 
 * Revision 2.2  92/01/16  14:43:06  roy
 * 	Move to uxkern directory (durriya).
 * 
 * Revision 2.2  91/11/27  11:15:06  rabii
 * 	initial check-in
 * 
 */

#undef KERNEL

#include <strings.h>
#include <mach.h>
#include <mach/boolean.h>
#include <mach/norma_special_ports.h>
#include <mach/message.h>
#include <mach/notify.h>
#include <cthreads.h>

void netname_init();

extern int	print_debug_messages; /* Global flag for turning on messages */
#define print_debug(x) \
	if (print_debug_messages) printf x; else 0

char *program;

static boolean_t lnsvr_demux();
mach_port_t nameservice_port;
mach_port_t lnsvr_notify_port;
extern mach_port_t privileged_host_port;

void
local_node_nameserver()
{
    int i;
    kern_return_t kr;
    mach_port_t pset;

    program = "local_node_nameserver ";
    cthread_set_name(cthread_self(), "lnsvr");

    if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &nameservice_port)) != KERN_SUCCESS) {
        printf("%s : Unable to allocate nameservice_port with receive rights : %d\n",
             program,kr);
    }
    
    if ((kr = mach_port_insert_right(mach_task_self(), nameservice_port, 
                                     nameservice_port, MACH_MSG_TYPE_MAKE_SEND)) 
        != KERN_SUCCESS) {
        printf("%s : Unable to insert send right for nameservice port : %d\n",
             program, kr);
    }
    
    print_debug(("%s: nameservice_port allocated \n",program));

    /* now set our selves to be the local name server */

    if ((kr = norma_set_nameserver_port(privileged_host_port, nameservice_port)) != 
	KERN_SUCCESS) {
        printf("%s : Unable to set the nameserver port : %d\n",program, kr);
    }

    print_debug(("%s: Nameserver port set\n", program));

    /* 
     * now allocate our notification port. This is the port we will use to request
     * notification of dead ports
     */
    kr = mach_port_allocate(mach_task_self(),
			    MACH_PORT_RIGHT_RECEIVE, &lnsvr_notify_port);
    if (kr != KERN_SUCCESS)
	printf("%s: mach_port_allocate: %d\n", program, kr);


    netname_init();

    kr = mach_port_allocate(mach_task_self(),
			    MACH_PORT_RIGHT_PORT_SET, &pset);
    if (kr != KERN_SUCCESS)
	printf("%s: mach_port_allocate: %d\n",program, kr);

    kr = mach_port_move_member(mach_task_self(), nameservice_port, pset);
    if (kr != KERN_SUCCESS)
	printf("%s: mach_port_move_member: %d\n", program, kr);

    kr = mach_port_move_member(mach_task_self(), lnsvr_notify_port, pset);
    if (kr != KERN_SUCCESS)
	printf("%s: mach_port_move_member: %d\n",program, kr);


    /*
     *  The local nameserver should NOT be allowed
     *  to return.
     */
    while(1) {
	kr = mach_msg_server(lnsvr_demux, 256, pset);

	print_debug(("%s: mach_msg_server returned : %d 0x%x\n", program, kr, kr));
    }
}

static boolean_t 
lnsvr_demux(request, reply)
    mach_msg_header_t *request, *reply;
{
    if (request->msgh_local_port == nameservice_port) {
        print_debug(("%s: Received request on nameservice_port\n",program));
	return netname_server(request, reply);

    } else if (request->msgh_local_port == lnsvr_notify_port) {
        print_debug(("%s: Received request on notify_port\n", program));
        return lnsvr_notify_server(request, reply);
    } else {
        printf("%s: Recevied request on invalid local port %d\n", program,
             request->msgh_local_port);
    }
}
