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
 *      Copyright 1993  Intel Corporation.
 *
 */

/*
 * macs.c	- Jerry Kearns	7/93
 *
 * This file contains the Multi User Accounting and Control System 
 * interface routines.
 *
 */
#include <mcmsg/mcmsg_appl.h>
#include <nx/schedule.h>
#include <mach/mach.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include "debug.h"

extern	int	do_MACS;

/*
 *  MACS socket file descriptor
 */
static	int		macs_fd = -1;
static	int		allocator_uid;

int lport = IPPORT_RESERVED - 1;

#define RESD_PORT (2001)	/* port address for the resource daemon */
#define	CPUCTRL		'q'
#define	C_VALID		((CPUCTRL<<8)|0x5)

struct	cpu_lblk {
	uid_t		id;		/* uid or agid */
	unsigned	weight:1;	/* weight time by number of nodes allocated */
	unsigned	inhibit:1;	/* account has exceeded cputime */
	unsigned	modify:1;	/* user may modify his allocations */
	unsigned	transfer:1;	/* user may transfer time to another agid*/
	unsigned	use:1;		/* user may NOT use time */
	unsigned	unlimit:1;	/* account with unlimited allocation */
	unsigned	killjobs:1;	/* if set kill jobs - dont try chkpnt */
	unsigned	lockjobs:1;	/* dont allow setting of killjobs bit*/
	unsigned	unused2:16;	/* some more unused space in structure */
	unsigned	percent:16;	/* percentage of time allocated to user */
	unsigned	maxnodes;	/* maximum number of allowed nodes */
	double		authorized;	/* total time authorized */
	double		used_time;	/* total time used */
	double		sbu_time;	/* total time used (weighted)*/
	time_t 		timestamp;	/* time last updated */
} ;

struct	cpu_ctrl {
	uid_t	 uid;	/* requestors real uid */
	gid_t 	agid;	/* requestors agid */
	struct cpu_lblk info;	/* data block to be used by command */
	short seq;		/* a seq number (rffu) */
};
struct client_request  {
   int command;			/* type of request made to resource daemon */
   int size;			/* number of data bytes following */
   struct cpu_ctrl msgbuf; 	/* structure to send to MACS */
};

struct resd_reply  {
   int returnvalue;		/* return value from the request */
   int errno;			/* if request failed, reason for failure */
   int size;			/* size of data buffer which will follow */
};

/*
 * macs_init() - Setup socket to communicate with MACS
 */
macs_init(server_port, socket_addr, socket_len, ret_code)
	mach_port_t	server_port;
	u_char		*socket_addr;
	int		socket_len;
	int		*ret_code;
{
	*ret_code = 0;
	if (!do_MACS)
		return(KERN_SUCCESS);

	allocator_uid = getuid(); 

	/*
	 *  This may be a restart of MACS.  If we already had a connection
	 *  then close it .
	 */
	if (macs_fd != -1)
		close(macs_fd);


	/*
	 *  Create the socket and connect.
	 */
	for (;;) {
		macs_fd = rresvport(&lport);
		if (macs_fd < 0) {
			if (errno == EAGAIN)
				debug_print(DEBUG_OTHER, 0, "MACS: All ports in use\n");
			else
				debug_print(DEBUG_OTHER, 0, "Can't get reserved port for MACS errno %d\n", errno);
			macs_fd=-1;
			*ret_code=-1;
			return (-1);
		}

		if (connect(macs_fd, socket_addr, socket_len) == 0) {
			break;
		}
		if (errno == EADDRINUSE) {
			lport--;
			(void) close(macs_fd);
			continue;
		}
		debug_print(DEBUG_OTHER, 0, "Can't connect with MACS errno %d\n", errno);
		*ret_code=-1;
		return (-1);
	}	
	*ret_code = 0;
	return (0);
}	


/*
 * ok_with_MACS(appl) - Check with MACS to see if application can start.
 *
 * Returns:	 1	- ok to proceed
 *		 0	- MACS authorization failed
 */

int ok_with_MACS(gid_t nx_acctid, uid_t uid, int size)
{
	int rval, reason, remain, numchar;
	struct resd_reply reply_hdr;
	struct client_request macs_msg;
	char *ptr;

	debug_sched(5, "Polling MACS: acct %d uid %d size %d\n", nx_acctid, uid, size);
	if (macs_fd == -1)
		return(0);

	macs_msg.command		= C_VALID;
	macs_msg.size			= sizeof(struct cpu_ctrl);
	macs_msg.msgbuf.uid		= allocator_uid;
	macs_msg.msgbuf.agid		= nx_acctid;
	macs_msg.msgbuf.info.id		= uid;
	macs_msg.msgbuf.info.maxnodes	= size;

	/*
	 * Send message to MACS.
	 */ 
	if (write (macs_fd, (char *)&macs_msg, sizeof(struct client_request)) <
		sizeof(struct client_request)) {
		debug_print(DEBUG_OTHER, 0, "MACS: write failed errno %d\n", errno);
		close(macs_fd);
		macs_fd = -1;
		return(0);
	}

	/*
	 * Get reply message from MACS.
	 */
	for (remain=sizeof(struct resd_reply), ptr=(char *)&reply_hdr; remain;
		remain-=numchar,ptr+=numchar) {
		numchar = read (macs_fd,ptr,remain);
		if (numchar<=0) {
			debug_print(DEBUG_OTHER, 0, "MACS: read failed errno %d\n", errno);
			close(macs_fd);
			macs_fd = -1;
			return(0);
		}
	}

	/*
	 * Read reply data 
	 */
	if ( reply_hdr.size != 0 ) {
		debug_print(DEBUG_OTHER, 0, "MACS interface changed!  Size returned !=0 \n");
		close(macs_fd);
		macs_fd = -1;
		return(0);
	}
	if (reply_hdr.errno < 0) {
		return(0);
	}
	
	debug_sched(5, "MACS returned ok\n");
	return(1);
}
