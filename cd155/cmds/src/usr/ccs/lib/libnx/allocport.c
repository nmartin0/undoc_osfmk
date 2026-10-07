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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Id: allocport.c,v 1.2 1994/11/19 02:29:02 mtm Exp $
 *
 */

/*
 *
 * HISTORY
 * $Log: allocport.c,v $
 * Revision 1.2  1994/11/19  02:29:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/07/29  18:32:49  carbajal
 * Moved ask_for_allocator_port() into here so that
 * it would not cause dependencies on partutils.c
 *
 *
 */
char _allocport[]="$Id: allocport.c,v 1.2 1994/11/19 02:29:02 mtm Exp $";

#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mode.h>
#include <strings.h>
#include <errno.h>
#include <mach_error.h>
#include <mach/message.h>
#include <mach/norma_special_ports.h>
#include <mach/mach_host.h>

/*		ask_for_allocator_port
*
*	This is a generic mig setup for the allocator commands.
*
*	PARAMETERS:
*		serv_port = pointer to mach_port_t
*			(allocator port)
*	RETURNS:
*		1 == successful
*		0 otherwise
*			currently errno is set to EPALLOCERR
*/
int
ask_for_allocator_port(mach_port_t *serv_port)
{
	kern_return_t retcode;
	mach_port_t name_server_port;
	mach_port_t privileged_host_port;
	int ok;

	ok = 1;		/* assume the best */

	/* Must be priviledged to get this port */
	privileged_host_port = task_by_pid(-1);

	if (privileged_host_port != MACH_PORT_NULL){
		/* Need to remove node 0 dependencies */	
		if ((retcode = norma_get_nameserver_port(privileged_host_port,
                                            nx_allocator_node(),&name_server_port)) != KERN_SUCCESS){
			errno = EPALLOCERR;
			ok = 0;
   		}
		else{
			retcode = netname_look_up(name_server_port,"","ALLOCATOR", 
							serv_port);
			if(retcode != KERN_SUCCESS){
				errno = EPALLOCERR;
				ok = 0;
			}
		}
	}
	else{
		errno = EPALLOCERR;
		ok = 0;
	}
	return(ok);
}
