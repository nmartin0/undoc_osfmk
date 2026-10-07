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
 * dispatcher.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/dispatcher.h,v 1.2 1994/11/19 03:10:51 mtm Exp $
 *
 */

/*
 * Functions exported by the dispatcher module.
 */


#ifndef	_DISPATCHER_
#define	_DISPATCHER_

#include "disp_hdr.h"

/*
 * Dispatcher switch.
 */
typedef struct {
    int		(*disp_indata)();
    int		(*disp_inprobe)();
    int		(*disp_indata_simple)();
    int		(*disp_rr_simple)();
#if	RPCMOD
    int		(*disp_in_request)();
#endif	RPCMOD
} dispatcher_switch_t;

extern dispatcher_switch_t	dispatcher_switch[DISP_TYPE_MAX];


/*
 * Functions exported by the dispatcher module.
 */

extern boolean_t disp_init();
/*
*/

extern int disp_indata();
/*
int		trid;
sbuf_ptr_t	data;
netaddr_t	from;
int		(*tr_cleanup)();
int		trmod;
int		client_id;
int		crypt_level;
boolean_t	broadcast;
*/

extern int disp_inprobe();
/*
int		trid;
sbuf_ptr_t	pkt;
netaddr_t	from;
int		(*(*cancel))();
int		trmod;
int		*client_id;
int		crypt_level;
boolean_t	broadcast;
*/

extern int disp_indata_simple();
/*
int		client_id;
sbuf_ptr_t	data;
netaddr_t	from;
boolean_t	broadcast;
int		crypt_level;
*/

extern int disp_rr_simple();
/*
sbuf_ptr_t	data;
netaddr_t	from;
boolean_t	broadcast;
int		crypt_level;
*/

#if	RPCMOD
extern int disp_in_request();
/*
int		trmod;
int		trid;
sbuf_ptr_t	data_ptr;
netaddr_t	from;
int		crypt_level;
boolean_t	broadcast;
*/
#endif	RPCMOD

#endif	_DISPATCHER_
