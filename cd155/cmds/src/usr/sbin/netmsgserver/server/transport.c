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
 * transport.c
 *
 *
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: transport.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:12:25 $";
#endif

/*
 * Generic transport stuff.
 */


#include "netmsg.h"
#include "nm_defs.h"
#include "transport.h"

/*
 * Memory management definitions.
 */
PUBLIC mem_objrec_t	MEM_TRBUFF;


EXPORT transport_sw_entry_t	transport_switch[TR_MAX_ENTRY];


/*
 * transport_no_function --
 *
 * Default function for inexistant transport calls.
 *
 * Parameters:
 *
 * Results:
 *
 * TR_FAILURE
 *
 * Side effects:
 *
 * Prints an error message.
 *
 * Design:
 *
 * Note:
 *
 */
EXPORT int transport_no_function()
BEGIN("transport_no_function")

	ERROR((msg,"** Transport: invalid function **"));
	RETURN(TR_FAILURE);
END



/*
 * transport_noop_send --
 *
 * Function for dummy send operation.
 *
 * Parameters:
 *
 * Results:
 *
 * TR_SUCCESS
 *
 * Side effects:
 *
 * Design:
 *
 * Note:
 *
 */
EXPORT int transport_noop_send()
BEGIN("transport_noop_send")

	RETURN(TR_SUCCESS);
END



/*
 * transport_init --
 *	Initialise the transport module.
 *
 * Results:
 *	TRUE or FALSE
 *
 * Design:
 *	Set up the transport_switch.
 *
 */
EXPORT boolean_t transport_init()
BEGIN("transport_init")
    int	i;

    /*
     * Initialize the memory management facilities.
     */
    mem_initobj(&MEM_TRBUFF,"Transport buffer",2000,FALSE,16,4);

    /*
     * Initialize the transport switch.
     */
    for (i = 0; i < TR_MAX_ENTRY; i++) {
	transport_switch[i].send = transport_no_function;
#if	RPCMOD
	transport_switch[i].sendrequest = transport_no_function;
	transport_switch[i].sendreply = transport_no_function;
#endif	RPCMOD
    }

    transport_switch[TR_NOOP_ENTRY].send = transport_noop_send;

    RETURN(TRUE);

END


