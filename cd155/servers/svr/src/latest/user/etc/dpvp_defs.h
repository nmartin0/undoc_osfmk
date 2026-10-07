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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log:
 */
/*
 * flag values for the pvproc structure
 */
#define	PV_SCTTY		0x0001	/* Process has controlling terminal
					   (if defined for the session) */
#define PV_PGRPLEADER		0x0002	/* Process is a pgrp leader and this
					   is also the execution site */
#define PV_SESSIONLEADER	0x0004	/* Process is a session leader and this
					   is also the execution site */
#define	PV_SZOMB		0x0010	/* Process is in zombie state. Set in
					   child vproc on execution node of
					   parent when child process exits
					   and also on the actual
					   process's execution node*/
#define	PV_SSTOP		0x0020	/* Process is in stopped state. Set in
					   child vproc on execution node of
					   parent when child process becomes
					   stopped and also on the actual
					   process's execution node. */
#define	PV_HAS_PORT_RIGHT	0x0040	/* Server has a send or receive right 
					   to the vproc port */
#define PV_HAS_REMOTE_RIGHT	0x0080	/* Server either gave away vproc port
					   rights to another node, or 
					   initially received its vproc port 
					   rights from another node */
#define	PV_HAS_RECEIVE_RIGHT	0x0100	/* Server has receive rights to
					   vproc port. Server may or may not
					   also have send rights. */
#define	PV_CTTY_NODE		0x0200	/* Session leader proc on the node of
					   the session's controlling terminal */

