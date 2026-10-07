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
 * Revision 1.5  1994/11/18  20:53:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:50:25  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.3  1993/05/06  19:28:04  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  21:16:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  23:01:12  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:49:13  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:01:21  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:57:27  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/11/27  11:15:06  rabii
 * 	initial check-in
 *
 */

#include <stdio.h>
#include <strings.h>
#include <mach.h>
#include <mach/norma_special_ports.h>
#include <mach/message.h>
#include <mach/notify.h>

void netname_init();

char *program = NULL;
boolean_t Debug = FALSE;

static boolean_t lnsvr_demux();
mach_port_t nameservice_port;

static void
usage()
{
    quit(1, "usage: %s [-d]\n", program);
}

void
main(argc, argv)
    int argc;
    char *argv[];
{
    int i;
    kern_return_t kr;
    mach_port_t priv_hostid;
    mach_port_t pset;


    program = rindex(argv[0], '/');
    if (program == NULL)
	program = argv[0];
    else
	program++;

    for (i = 1; i < argc; i++) 
	if ((argv[i][0] == '-') && (argv[i][1] == 'd'))
	    Debug = TRUE;
	else {
            usage();
        }


    /* get the priviliged host id - NOte we must be running setuid root to be able 
     * to do this.
     */
    priv_hostid = task_by_pid(-1);
    if (priv_hostid == -1) {
      quit(1, "%s: Unable to get priviliged host id\n", program);
    } 
    
    if (Debug) {
      printf("Priviliged host id = %d\n", priv_hostid);
    }

    if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &nameservice_port)) != KERN_SUCCESS) {
        quit(1, "%s : Unable to allocate nameservice_port with receive rights : %s\n",
             program, mach_error_string(kr));
    }
    
    if (Debug) {
        printf("mach_port_allocate(nameservice_port) successful\n");
    }

    if ((kr = mach_port_insert_right(mach_task_self(), nameservice_port, 
                                     nameservice_port, MACH_MSG_TYPE_MAKE_SEND)) 
        != KERN_SUCCESS) {
        quit(1, "%s : Unable to insert send right for nameservice port : %s\n",
             program, mach_error_string(kr));
    }
    
    if (Debug) {
        printf("mach_port_insert_right(nameservice_port, SEND_RIGHT) successful\n");
    }
			  
    /* now set our selves to be the local name server */

    if ((kr = norma_set_nameserver_port(priv_hostid, nameservice_port)) != 
	KERN_SUCCESS) {


      quit(1,"%s : Unable to set the nameserver port : %s\n",program, 
           mach_error_string(kr));
    }

    if (Debug) {
      printf("Nameserver port set\n");
    }

    netname_init();


    kr = mach_msg_server(lnsvr_demux, 256, nameservice_port);

    quit(1,"%s: mach_msg_server returned : %s\n", program,mach_error_string(kr));
}

static boolean_t 
lnsvr_demux(request, reply)
    mach_msg_header_t *request, *reply;
{
    if (request->msgh_local_port == nameservice_port) {
        if (Debug) {
            printf("Received request on nameservice_port\n");
        }
	return netname_server(request, reply);
    } else {
        quit(1, "%s: Recevied request on invalid local port %d\n", program,
             request->msgh_local_port);
    }
}
