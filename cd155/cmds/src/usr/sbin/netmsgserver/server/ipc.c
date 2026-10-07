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
 * ipc.c
 *
 *
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ipc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:10:58 $";
#endif

/*
 * Main routines of the IPC module.
 */


#include <mach.h>
#include <mach/boolean.h>
#include <mach/message.h>

#include "ipc.h"
#include "ipc_rec.h"
#include "netmsg.h"
#include "ipc_internal.h"

/*
 * Memory management definitions.
 */
PUBLIC mem_objrec_t	MEM_IPCBUFF;
PUBLIC mem_objrec_t	MEM_ASSEMBUFF;
PUBLIC mem_objrec_t	MEM_IPCBLOCK;
#if	RPCMOD
PUBLIC mem_objrec_t	MEM_IPCREC;
PUBLIC mem_objrec_t	MEM_IPCABORT;
#endif	RPCMOD



/*
 * ipc_init
 *
 * Results:
 *	TRUE or FALSE.
 *
 * Design:
 *
 */
EXPORT boolean_t ipc_init()
BEGIN("ipc_init")

	/*
	 * Initialize the memory management facilities.
	 */
	mem_initobj(&MEM_IPCBUFF,"IPC receive buffer",MSG_SIZE_MAX,TRUE,7,1);
	mem_initobj(&MEM_ASSEMBUFF,"IPC assembly buffer",IPC_ASSEM_SIZE,
								TRUE,3,1);
	mem_initobj(&MEM_IPCBLOCK,"IPC block record",sizeof(ipc_block_t),
								FALSE,500,5);
#if	RPCMOD
	mem_initobj(&MEM_IPCREC,"IPC record",sizeof(ipc_rec_t),FALSE,12,2);
	mem_initobj(&MEM_IPCABORT,"IPC abort record",sizeof(ipc_abort_rec_t),
								FALSE,50,2);
#endif	RPCMOD

	/*
	 * Start the IPC module.
	 */
#if	RPCMOD
	RETURN(ipc_rpc_init());
#else 	RPCMOD
    ipc_in_init();

    ipc_out_init();

    RETURN(TRUE);
#endif	RPCMOD

END

