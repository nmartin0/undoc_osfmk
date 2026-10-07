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
 * OSF/1 Release 1.0
 */

/*
 * @(#)$RCSfile: cm_cmdpkt.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:19 $
 */

#define LOCALSOCKNAME   "/dev/cfgmgr"		/* Local Unix socket name */
#define SERVICENAME     "cfgmgr"		/* Remote Inet service name */

/*
 *      Communication command structure
 */

typedef struct {
	int	cmdpkt_op;				/* Subsys command */
	int	cmdpkt_loglvl;				/* Subsys log level */
	char	cmdpkt_name[128];			/* Subsys name */
	char	cmdpkt_opts[128];			/* Pass thru options */
} cmgr_cmdpkt_t;

#define PKTSZ   sizeof(cmgr_cmdpkt_t)

