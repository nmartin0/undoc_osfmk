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
 
#ifdef SDSC
/*
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 */
/*
 * HISTORY
 * $Log: res_msg.c,v $
 * Revision 1.5  1994/12/07  22:12:32  davidl
 *  Code-reviewed and cleaned up all socket code in MACS and NQS:
 *
 *  * Added some bzero() calls for sockaddr_in structures before bind()
 *    and accept() operations.
 *
 *  * Changed some occurrences of gethostbyname(host) (which can cause
 *    failures on systems with multiple Ethernets) to
 *    gethostbyname("localhost").
 *
 *  * Removed some dead (never executed) code.
 *
 *  * Replaced a few hard-coded 0's with appropriate socket #defines.
 *
 *  Reviewer: doyle
 *  Risk: Medium (has not been tested with multiple Enets)
 *  Benefit or PTS #: 11659
 *  Testing: EATs (only)
 *  Module(s):
 *
 *   MACS:	cmds_libs/src/usr/ccs/lib/libmacs/open.c
 * 	cmds_libs/src/usr/lib/macs/macd.c
 * 	cmds_libs/src/usr/lib/macs/ports.c
 *
 *   NQS:	cmds_libs/src/usr/ccs/lib/libnqs/establish.c
 * 	cmds_libs/src/usr/ccs/lib/libnqs/inter.c
 * 	cmds_libs/src/usr/lib/nqs/netdaemon.c
 * 	cmds_libs/src/usr/lib/nqs/res_msg.c
 * 	cmds_libs/src/usr/lib/nqs/smd_msg.c
 *
 * Revision 1.4  1994/11/19  02:53:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/10/07  00:54:21  kremenek
 *  Reviewer: davidl doyle
 *  Risk: Low
 *  Benefit or PTS #: 11067
 *  Testing: EATS
 *  Module(s): cmds_libs/src/usr/lib/nqs/res_msg.c
 *
 * Revision 1.2  1994/04/06  21:38:32  mwan
 * Update R1_3 to R1_2 WW15
 *
 *  Reviewer: kremenek
 *  Risk: M
 *  Benefit or PTS #: 7738,8087,8346, 8325,8599,8576,8600,8601,8088,8597,8876,8886
 *  Testing:
 *  Module(s):  macs_blk.c macs_job.c macs_lib.c macs_rootp.c macs_sched.c
 * 	     nqs_spawn.c res_msg.c smd_msg.c
 *
 * Revision 1.1.14.1  1994/03/30  02:01:25  mwan
 * Fixed pts 8088, 8597
 *
 *  Reviewer: kremenek
 *  Risk: L
 *  Benefit or PTS #: 8088, 8597
 *  Testing:
 *  Module(s): usr/lib/nqs/nqs_spawn.c, usr/lib/nqs/macs_lib.c,
 *          usr/lib/nqs/res_msg.c.
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 *
 */

#define HOSTNAMELENGTH (64)
#define SOCK_TIMEOUT 2
#define FDSETNULL ((fd_set *) NULL)

#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <pwd.h>

#include <netinet/in.h>

#include <netdb.h>
#include <errno.h>
#include "cpuctl.h"
#include "res.h"

extern	errno;

void c_close ();

/*
 * cpuctl (request, data)
 *	communicate with resource management daemon
 * it performs authentication, and send request and
 * get reply from and to resd.  It returns -1 for
 * error and 0 or reply-return-value when successful
 * reply data is stored in the data address passed in.
 */

int c_sock = -1;

char reqst_buf[C_REQUEST_HDRLEN+sizeof(struct cpu_ctrl)], 
	reply_hdr[C_REPLY_HDRLEN];
int *reqst_intptr=(int *)reqst_buf, *reply_intptr=(int *)reply_hdr;

cpuctl( request, data )
int request;
char *data;
{
	int rsu_flag;	/* real user id must be super-user  */
	int esu_flag;	/* effective user id must be super-user */
	int data_flag;	/* there is data to be send */
	int remain, numchar, msgsize;
	char *ptr;
	fd_set readysock;
        struct timeval timeout;
        int nb;


/*
 * Separate routines for establishing and closing connection to RSC daemon
 */

/*
 * Invalid request number
 */
	if (request > MAX_REQNUM) {
#ifdef RES_DEBUG
		(void) fprintf(stderr,"invalid request number\n");
#endif
		errno = EINVAL;
		return(-1);
	}

/*
 * The rest requests: must have established a connection already
 */
	if (c_sock <= 0) {
#ifdef RES_DEBUG
		(void) fprintf(stderr,"c_sock = %d\n",c_sock);
#endif
		errno = EPIPE;
		return(-1);
	}

/*
 * Initialization
 */
	rsu_flag = 0;
	esu_flag = 0;
	data_flag = 0;
	if (request == C_SYNC) rsu_flag = 1;
	else if (request == C_GETLIMIT) data_flag = 1;
	else if (request == C_DELACCT) esu_flag = data_flag = 1;
	else if (request == C_DELLIMIT) esu_flag = data_flag = 1;
	else if (request == C_SETLIMIT) esu_flag = data_flag = 1;
	else if (request == C_GETACCT) data_flag = 1;
	else if (request == C_SETACCT) esu_flag = data_flag = 1;
	else if (request == C_VALID) data_flag = 1;
	else if (request == C_OFF) rsu_flag = 1;
	else if (request == C_CREACCT) esu_flag = data_flag = 1;
	else if (request == C_ON_COUNT) rsu_flag = 1;
	else if (request == NQS_JOB_START) rsu_flag = data_flag = 1;
	else if (request == NQS_JOB_END) rsu_flag = data_flag = 1;
	else if (request == SI_ON) rsu_flag = 1;
	else if (request == SI_OFF) rsu_flag = 1;
	else {
#ifdef RES_DEBUG
		(void) fprintf(stderr,"invalid request number\n");
#endif
		errno = EINVAL;
		return(-1);
	}

/*
 * Real-super-user-only requests, check real user id
 */
#ifdef RES_DEBUG
	if (rsu_flag) {
		(void) fprintf(stderr,"cpuctl:  Check real user id...\n");
		(void) fprintf (stderr, "cpuctl: real user id = %d\n",
			getuid());
	}
#endif
	if (rsu_flag && (getuid() != 0)) {
#ifdef RES_DEBUG
		(void) fprintf(stderr,"cpuctl:  real user id = root required\n");
#endif
		errno = EACCES;
		return(-1);
	}

/*
 * Effective-super-user-only requests, check effictive user id
 */
#ifdef RES_DEBUG
	if (esu_flag) (void) fprintf(stderr,"cpuctl:  Check effective user id...\n");
#endif
	if (esu_flag && geteuid() != 0) {
#ifdef RES_DEBUG
		(void) fprintf(stderr,"cpuctl:  effective user id = root required\n");
#endif
		errno = EACCES;
		return(-1);
	}

/*
 * Pack a request messge and send to resource manager daemon 
 */ 
	reqst_reqnum = request; 
	if (data_flag) {
		if (request == NQS_JOB_START || request == NQS_JOB_END)
			reqst_datasize = sizeof(struct nqs_job_info);
		else reqst_datasize = sizeof(struct cpu_ctrl); 
		bcopy ( data, reqst_buf+C_REQUEST_HDRLEN, reqst_datasize );
	}
	else reqst_datasize = 0;
	msgsize = C_REQUEST_HDRLEN + reqst_datasize;


        FD_ZERO (&readysock);
        FD_SET (c_sock, &readysock);
        timeout.tv_sec = SOCK_TIMEOUT;
        timeout.tv_usec = 0;
 
        nb = select (c_sock + 1, FDSETNULL, &readysock, FDSETNULL, &timeout);
        if (nb <= 0) {
            printf ("I$cpuctl: write socket timeout\n");
            return (-1);
        }

#ifdef RES_DEBUG
	(void) dump_buf (reqst_buf, msgsize);
	writeout = write(c_sock, reqst_buf, msgsize);
	(void) fprintf(stderr,"cpuctl:  %d/%d request bytes sent\n",
		writeout, msgsize);
	if (writeout < msgsize) {
#else
	if (write (c_sock, reqst_buf, msgsize) < msgsize) {
#endif
		errno = EPIPE;
		return(-1);
	}

/*
 * send successful and not wait for reply
 */
#ifndef R1_1
        if (request == NQS_JOB_START /* for R1.2 JOB_end return cost information back || request == NQS_JOB_END */
#else
        if (request == NQS_JOB_START || request == NQS_JOB_END
#endif
		|| request == SI_ON || request == SI_OFF) return (0);

/*
 * Get reply message from resource management daemon
 */
	for (remain=C_REPLY_HDRLEN,ptr=reply_hdr;remain;
		remain-=numchar,ptr+=numchar) {
		numchar = read (c_sock,ptr,remain);
#ifdef RES_DEBUG
		(void) fprintf (stderr, "cpuctl: %d/%d header bytes read:\n",
			numchar, remain);
#endif
		if (numchar<=0) {
			if(errno == EWOULDBLOCK) {
				numchar = 0;
#ifdef RES_DEBUG
				(void) fprintf(stderr, 
					"cpuctl:  errno=%d\n", errno);
#endif
				(void) sleep(1);
				continue;
			}
#ifdef RES_DEBUG
			(void) fprintf(stderr, "cpuctl:  errno=%d\n", errno);
#endif
			errno = EPIPE;
			return(-1);
		}
	}

#ifdef RES_DEBUG
	(void) fprintf(stderr, "cpuctl: %d bytes are read:\n", C_REPLY_HDRLEN);
	(void) dump_buf(reply_hdr,C_REPLY_HDRLEN);
        (void) printf ("I$res_msg.c: header %d bytes are read:\n", C_REPLY_HDRLEN);
#endif

/*
 * No reply data
 */
	if (reply_datasize == 0) {
		errno = reply_errno;
		return(reply_return);
	}

        FD_ZERO (&readysock);
        FD_SET (c_sock, &readysock);
 
        nb = select (c_sock + 1, &readysock, FDSETNULL, FDSETNULL, &timeout);
        if (nb <= 0) {
            printf ("I$cpuctl: read socket timeout\n");
            return (-1);
        }

/*
 * Read reply data 
 */
	for(remain=reply_datasize,ptr=(char *)data;remain;
		remain-=numchar,ptr+=numchar) {
		numchar = read (c_sock,ptr,remain);
#ifdef RES_DEBUG
		(void) fprintf(stderr, "cpuctl: %d/%d data bytes read, errno=%d\n",
			numchar, remain, errno);
#endif
		if (numchar<=0) {
			if(errno == EWOULDBLOCK) {
				numchar = 0;
#ifdef RES_DEBUG
				(void) fprintf(stderr, 
					"cpuctl:  errno=%d\n", errno);
#endif
				(void) sleep(1);
				continue;
			}
#ifdef RES_DEBUG
			(void) fprintf(stderr, "cpuctl:  errno=%d\n", errno);
#endif
			errno = EPIPE;
			return(-1);
		}
	}
#ifdef RES_DEBUG
	msgsize = C_REPLY_HDRLEN + reply_datasize;
	(void) fprintf(stderr, "cpuctl: %d bytes are read:\n", msgsize);
	(void) dump_buf(reply_hdr,C_REPLY_HDRLEN);
	(void) dump_buf((char *)data,reply_datasize);
        (void) printf ("I$res_msg.c: message body %d bytes are read:\n", reply_datasize);
#endif

	errno = reply_errno;
	return(reply_return);
}

/* my_open ()
 *
 * This is essentially the same as c_open except taking out the timeout
 * stuff.
 *
 */
int my_open()
{

/*
 * Local variables
 */
	struct sockaddr_in sin;
	int lport;
	struct hostent *hp;

/*
 * No more than one connection per client
 */
	if (c_sock > 0) {
		errno = EISCONN;
		return(-1);
	}


/*
 * Open a socket, bind it with a previledged port then connect to resource
 * management daemon
 */
	c_sock = socket (AF_INET, SOCK_STREAM, 0);
	if (c_sock < 0) {
	    fprintf(stderr, "I$Unable to open socket\n");
	    return (-1);
	}
	/* must zero out sockaddr_in for bind to work */
        bzero ((char *) &sin, sizeof (struct sockaddr_in));
        hp = gethostbyname("localhost");
	sin.sin_family = AF_INET;
        bcopy(hp->h_addr_list[0], (caddr_t)&sin.sin_addr, hp->h_length);
	
	lport = IPPORT_RESERVED - 1;
	for (;;) {
	    sin.sin_port = htons (lport);
	    if (bind (c_sock, (caddr_t) &sin, sizeof (sin)) >= 0)
		break;
            if (errno != EADDRINUSE && errno != EADDRNOTAVAIL) {
	    	fprintf(stderr, "I$Unable to bind, errno=%d\n", errno);
		return (-1);
	    }
            lport--;
	    if (lport == IPPORT_RESERVED / 4) {
	    	fprintf(stderr, "I$My_open : All port in use\n");
		return (-1);
	    }
        }

/* this doesn't work for 3 network interfaces 
        1. paragonboot
        2. paragon_input_only
        3. paragon_output_only
 
	gethostname(host, HOSTNAMELENGTH);
        hp = gethostbyname(host);
 
                MACS refused to talk to the NQS */

        bzero ((char *) &sin, sizeof (struct sockaddr_in));
	sin.sin_family = AF_INET;
        bcopy(hp->h_addr_list[0], (caddr_t)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons (RESD_PORT);
        sin.sin_family = hp->h_addrtype;
	if (connect(c_sock, (caddr_t)&sin, sizeof (sin)) < 0) {
	    fprintf(stderr, "I$My_open : unable to connect, errno=%d\n", errno);
	    (void) c_close ();
	    return (-1);
	}

/*
 * Pack a request messge and send to resource manager daemon
 */
	reqst_reqnum = C_OPEN;
	reqst_datasize = 0;
	if (write(c_sock, reqst_buf, C_REQUEST_HDRLEN) < C_REQUEST_HDRLEN) {
		errno = EPIPE;
		return(-1);
	}

/*
 * Get reply message from resource management daemon
 */
	if (read(c_sock, reply_hdr, C_REPLY_HDRLEN) < C_REPLY_HDRLEN) {
		errno = EPIPE;
		return(-1);
	}
	errno = reply_errno;

/*
 * Unseccessful open: close the socket
 */
	if (reply_return == -1) {
		close(c_sock);
		c_sock = -1;
	}

	return(reply_return);
}

/* my_runok ()
 *
 * This routine is the same as res_runok () except the socket is assumed to
 * have already been open.
 *
 * Input:       int     agid;
 *              int     uid;
 *              int     nnodes;
 * Return:      TRUE    - ok
 *              FALSE   - fail in query resd
 *              -1      - account invalid
 *              -2      - account not allowed to use
 *              -3      - overrun allocation
 *              -4      - exceed max. number of nodes
 *              -5      - other, check errno
 *
 */
my_runok (agid, uid, nnodes)
int	agid;
int	uid;
int	nnodes;
{
	int rval, reason;
	struct cpu_ctrl *user_data;

/*
 * Fill in information for C_VALID request
 */

	if ((user_data = (struct cpu_ctrl *)
		calloc (1, sizeof (struct cpu_ctrl))) == NULL) 
		return (-5);

	user_data->uid = getuid();
	user_data->agid = agid;
	user_data->info.id = uid;
	user_data->info.maxnodes = nnodes;

	rval = cpuctl (C_VALID, user_data);
	reason = errno;
	(void) free (user_data);
	(void) c_close();

	if (rval < 0) {
		errno = EACCES;
		return (-5);
	}

	if (rval) return (TRUE);

	return (reason);
}

/* my_sendmsg ()
 *
 * Input:	int	type;
 *		char *	data;
 * Return:	0  	--  successful
 *		-1	--  failure
 * 
 * this is the same as res_sendmsg except for c_pen is mission.
 *
 */
my_sendmsg (type, data)
int	type;
char *	data;
{
/*
 * Super-user privilege is required
 */

        if (getuid() != 0) {
#ifdef RES_DEBUG
                (void) fprintf(stderr,"cpuctl:  real user id = root required\n")
;
#endif
                errno = EACCES;
                return(-1);
        }

	/*
	  Fill in the client request and send the data to the resource daemon
	*/

	if (cpuctl (type, data) != 0) {
                (void) c_close();
                return (-1);
        }

        (void) c_close();
	return (0);
}

void c_close()
{
	if (c_sock >= 0) (void) close(c_sock);
	c_sock = -1;
}

#endif
