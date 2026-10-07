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
/* smd_msg.c
 *
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 */
/*
 * HISTORY
 * $Log: smd_msg.c,v $
 * Revision 1.8  1995/03/21  22:11:01  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 10507
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/include/nqs/buddy.h
 * 	cmds_libs/src/usr/include/nqs/buddyvar.h
 * 	cmds_libs/src/usr/lib/nqs/macs_lib.c
 * 	cmds_libs/src/usr/lib/nqs/nqs_spawn.c
 * 	cmds_libs/src/usr/lib/nqs/smd_msg.c
 *
 * Revision 1.7  1994/12/07  22:12:39  davidl
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
 * Revision 1.6  1994/11/19  02:53:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/04/06  21:38:34  mwan
 * Update R1_3 to R1_2 WW15
 *
 *  Reviewer: kremenek
 *  Risk: M
 *  Benefit or PTS #: 7738,8087,8346, 8325,8599,8576,8600,8601,8088,8597,8876,8886
 *  Testing:
 *  Module(s):  macs_blk.c macs_job.c macs_lib.c macs_rootp.c macs_sched.c
 * 	     nqs_spawn.c res_msg.c smd_msg.c
 *
 * Revision 1.4  1993/01/28  19:41:27  hobbes
 * modified smd_open to bind the local endpoint. this shouldn't
 * be necessary but there seems to be some problem with sockets
 * if you don't.
 *
 * Revision 1.3  1992/10/26  18:47:25  mwan
 * T6 update 1
 *
 * Revision 1.2  1992/10/09  22:27:07  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 *
 */

#ifdef	LIBONET
#define OLDTCP
#endif
#define MAX_RESP 100		/* number of smd_resp struct to read */
#define SOCK_TIMEOUT 2		/* 2 sec timeout */
#define FDSETNULL ((fd_set *) NULL)
#ifdef SMD_IP_IFACE
#define HOSTNAMELENGTH (64)
#else
#define SMD_BASE_DIR	"/etc/nx/"
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <ctype.h>
#include <pwd.h>

#include <netinet/in.h>

#include <netdb.h>
#include <errno.h>
#include <signal.h>                     /* Signal definitions */
#ifdef SDSC
#include "nqs.h"
#include "nx/smd.h"
#include "buddyxvar.h"
#endif

extern	errno;

/* smd_set_alarm
 *
 * routine to send a msg to SMD to set an alarm for a given job.
 * Sig_pid is the pid that should receive the signal - the shepherd 
 * daemon.
 */

int smd_set_alarm (smd_sock, j_req, sig_pid, grace_time)
int smd_sock;
struct job_req *j_req;
int sig_pid;		/* The pid to receive the alarm signal */
int grace_time;
{
	struct smd_req reqst;
	int rval, msglen;
	char *ptr;

	if (WALL_CLOCK != 0)
	        return(0);    /* using wall clock time */
	if (j_req == NULL)
		return (-1);

	if (smd_sock < 0)
		return (-1);

	/*
	 * Now set alarm.
	 */

	msglen = sizeof (struct smd_req);
	reqst.hdr.type = SMD_SET_APP_ALARM_REQ;
	reqst.hdr.len = msglen - sizeof (int) * 2;
	reqst.q_id.pgid = QUALIFY_ANY;
	reqst.q_id.part_id = j_req->part_id;
	reqst.q_id.acct_id = QUALIFY_ANY;
	reqst.q_id.uid = QUALIFY_ANY;
	bzero (reqst.q_id.req_id, SMD_REQ_ID_LEN);
	strcpy (reqst.q_id.req_id, "NQS");
	reqst.param.events = 0;
	reqst.param.flags = KEEP_AFTER_CLOSE | SEND_SIGNAL|PERSISTENT;
	reqst.param.sig_pid = sig_pid;
	reqst.param.sig_num = SIGALRM;
	reqst.param.alarm_val = j_req->req_nodes * 
	(j_req->req_time + grace_time);
        for (ptr = (char *) &reqst; msglen > 0; msglen -= rval, ptr += rval) {
                rval = write(smd_sock, ptr, msglen);
		printf ("I$smd_set_alarm:msglen=%d, rval=%d\n", msglen, rval);
                if (rval <= 0) {
			errno = EPIPE;
			return (-1);
		}
        }

	return(0);
}

/* smd_cancel
 *
 * routine to cancel a set_alarm request.
 */

int smd_cancel (smd_sock ,j_req)
int smd_sock;
struct job_req *j_req;
{
	struct smd_req reqst;
	int rval, msglen;
	char *ptr;

	if (WALL_CLOCK != 0)
	        return(0);    /* using wall clock time */
	if (j_req == NULL)
		return (-1);

	if (smd_sock < 0)
		return (-1);

	/*
 	 * Cancel the monitor request.
	 */

	msglen = sizeof (struct smd_req);
	reqst.hdr.type = SMD_CANCEL_REQ;
	reqst.hdr.len = msglen - sizeof (int) * 2;
	reqst.q_id.pgid = QUALIFY_ANY;
	reqst.q_id.part_id = j_req->part_id;
	reqst.q_id.acct_id = QUALIFY_ANY;
	reqst.q_id.uid = QUALIFY_ANY;
	bzero (reqst.q_id.req_id, SMD_REQ_ID_LEN);
	strcpy (reqst.q_id.req_id, "NQS");
	reqst.param.events = 0;
	reqst.param.service = SMD_SET_APP_ALARM_REQ;
	reqst.param.flags = KEEP_AFTER_CLOSE | SEND_SIGNAL|PERSISTENT;
        for (ptr = (char *) &reqst; msglen > 0; msglen -= rval, ptr += rval) {
                rval = write(smd_sock, ptr, msglen);
printf ("I$smd_cancel:msglen=%d, rval=%d\n", msglen, rval);
                if (rval <= 0) {
			(void) close (smd_sock);
			errno = EPIPE;
			return (-1);
		}
        }
	return(0);
}

/* smd_open
 *
 * open a socket.
 * Return the socket number if OK. Otherwise, return -1.
 * 
 */

int smd_open ()
{
#ifdef SMD_IP_IFACE
        struct sockaddr_in sin;
        struct sockaddr_in mysin;
        int lport = IPPORT_RESERVED - 1 - (getpid() % (IPPORT_RESERVED / 4));
        struct hostent *hp;   
        char host[HOSTNAMELENGTH];
	int smd_sock;

/*
 * Open a socket, bind it with a previledged port then connect to SMD
 */

        hp = gethostbyname("localhost");

        for (;;) {
		smd_sock = socket (AF_INET, SOCK_STREAM, 0);
                if (smd_sock < 0) {
                    fprintf(stderr, "I$Unable to open socket\n"); 
                    return (-1);
                }
		bzero((caddr_t)&sin, sizeof(sin));
                sin.sin_family = AF_INET;
                bcopy(hp->h_addr, &sin.sin_addr.s_addr, hp->h_length);
                sin.sin_port = htons (SMD_IFACE_PORT);
				/*
				 * Bind my local endpoint
				 */
				bzero((caddr_t)&mysin, sizeof(mysin));
				mysin.sin_family = AF_INET;
				mysin.sin_addr.s_addr = INADDR_ANY;
				mysin.sin_port = htons(0);
				if((bind (smd_sock, &mysin, sizeof (mysin))) != 0) {
					perror("nqsdaemon can't bind");
					exit(1);
				}
                if (connect(smd_sock, (caddr_t)&sin, sizeof (sin)) >= 0)
                        break;
                (void) close(smd_sock);
                fprintf(stderr, "I$Unable to connect to SMD, errno = %d\n",
		errno); 

                return (-1);
        }
	return (smd_sock);
#else
	struct sockaddr_un server;
	int smd_sock;

	/*
	 * Open a socket, bind it with a previledged port then connect to
	 * scheduler monitor
 	 */

	bzero((caddr_t)&server, sizeof(struct sockaddr_un));
	if ((smd_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return (-1);
	server.sun_family = AF_UNIX;
	strcpy (server.sun_path, SMD_BASE_DIR);
	strcat (server.sun_path, SMD_IFACE_NAME);
	if (connect(smd_sock, &server, sizeof (struct sockaddr_un)) < 0) {
		(void) close(smd_sock);
		return (-1);
	}
	return (smd_sock);
#endif
}

/* smd_cancel_all
 *
 * routine to cancel all smd requests.
 */

int smd_cancel_all (smd_sock)
int smd_sock;
{
	struct smd_req reqst;
	int rval, msglen;
	char *ptr;

	if (WALL_CLOCK != 0)
	        return(0);    /* using wall clock time */
	if (smd_sock < 0)
		return (-1);

	/*
 	 * Cancel the monitor request.
	 */

	msglen = sizeof (struct smd_req);
	reqst.hdr.type = SMD_CANCEL_REQ;
	reqst.hdr.len = msglen - sizeof (int) * 2;
	reqst.q_id.pgid = QUALIFY_ANY;
	reqst.q_id.part_id = QUALIFY_ANY;
	reqst.q_id.acct_id = QUALIFY_ANY;
	reqst.q_id.uid = QUALIFY_ANY;
	strcpy (reqst.q_id.req_id, "NQS");
	reqst.param.events = 0;
	reqst.param.service = SMD_SET_APP_ALARM_REQ;
	reqst.param.flags = KEEP_AFTER_CLOSE | SEND_SIGNAL|PERSISTENT;
        for (ptr = (char *) &reqst; msglen > 0; msglen -= rval, ptr += rval) {
                rval = write(smd_sock, ptr, msglen);
		printf ("I$smd_cancel_all:msglen=%d, rval=%d\n", msglen, rval);
                if (rval <= 0) {
			(void) close (smd_sock);
			errno = EPIPE;
			return (-1);
		}
        }
	return(0);
}

/* smd_read:
 *
 *      Read a message from the interface.
 */
int smd_read (smd_sock, hdr, data, max_size)
int smd_sock;
smd_hdr_t *hdr;
char *data;
int max_size;
{
        char    *p;
        int     byte_left, byte_read;
 
	if (hdr == NULL)
	    return (-1);

	if (data == NULL)
	    return (-1);

        /*
         *  Read the header and make sure it is sane.
         */

	byte_left = sizeof (smd_hdr_t);
	p = (char *) hdr;
	while (byte_left > 0) {
            if ((byte_read = read(smd_sock, p, byte_left)) <= 0) {
		printf ("$Ismd_read: Cannot read header \n");
		return (-1);
	    }
            p += byte_read;
	    byte_left -= byte_read;
	}
        if (hdr->len > max_size) {
	    printf ("$Ismd_read: data len = %d too long. Truncated to %d\n", 
	    hdr->len, max_size);
	    hdr->len = max_size;
        }
 
        /*
         *  Read hdr->len amount of data.  The socket interface does not
         *  guarantee to block until the specified length is read so we
         *  may have to make several attempts to get is all.
         */
 
        p = (char*) data;
        byte_left = hdr->len;
        while(byte_left) {
            if ((byte_read = read(smd_sock, p, byte_left)) <= 0) {
		printf ("$Ismd_read: read data error\n");
		return (-1);
	    }
            p += byte_read;
	    byte_left -= byte_read;
        }
        return (hdr->len);
}

/* smd_status
 *
 * send a SMD_GET_STATUS_REQ to SMD and get the SMD_STATUS_RESP response
 * for a specific part_id.
 * The response will be written into hdr and data.
 *
 * return the number of bytes read.
 */
int smd_status (part_id, uid, hdr, data, max_size)
int part_id;
int uid;
smd_hdr_t *hdr;
char *data;
int max_size;
{
	int smd_sock;
        struct smd_req reqst;
        int rval, msglen;
        char *ptr;
	int byte_read;
        extern int errno;
	fd_set readysock;
	struct timeval timeout;
	int nb;
 
	if (hdr == NULL)
	    return (-1);

	if (data == NULL)
	    return (-1);

	if (part_id < 0 && part_id != QUALIFY_ANY)
	    return (-1);

	if (uid < 0 && uid != QUALIFY_ANY)
	    return (-1);
	/*
	 * open smd_sock.
 	*/
 
	if ((smd_sock = smd_open ()) < 0) {
	    printf ("I$smd_status: Cannot open socket\n");
	    return (-1);
	}
 
	/*
	 * Pack a status request messge and send to scheduler monitor
 	*/

        msglen = sizeof (struct smd_req);
        reqst.hdr.type = SMD_GET_STATUS_REQ;
        reqst.hdr.len = msglen - sizeof (int) * 2;
        reqst.q_id.pgid = QUALIFY_ANY;
        reqst.q_id.part_id = part_id;
        reqst.q_id.acct_id = QUALIFY_ANY;
        reqst.q_id.uid = uid;

	/* send the message */

	FD_ZERO (&readysock);
	FD_SET (smd_sock, &readysock);
	timeout.tv_sec = SOCK_TIMEOUT;
	timeout.tv_usec = 0;

/*
	nb = select (smd_sock + 1, FDSETNULL, &readysock, FDSETNULL, &timeout);
	if (nb <= 0) {
	    printf ("I$smd_status: write socket timeout\n");
	    (void) close (smd_sock);
	    return (-1);
	}
*/

        for (ptr = (char *) &reqst; msglen > 0; msglen -= rval, ptr += rval) {
                rval = write (smd_sock, ptr, msglen);
                if (rval <= 0) {
			(void) close (smd_sock);
	    		printf ("I$smd_status: write error\n");
			return (-1);
		}
        }

	/* now read the response */

	FD_ZERO (&readysock);
	FD_SET (smd_sock, &readysock);

	nb = select (smd_sock + 1, &readysock, FDSETNULL, FDSETNULL, &timeout);
	if (nb <= 0) {
	    printf ("I$smd_status: read socket timeout\n");
	    (void) close (smd_sock);
	    return (-1);
	}

	byte_read = smd_read (smd_sock, hdr, data, max_size);
	if (hdr->type != SMD_STATUS_RESP) {
	    printf ("I$smd_status: smd type mismatch\n");
	    (void) close (smd_sock);
	    return (-1);
	}
	(void) close (smd_sock);
	    
        return (byte_read);
}

/* kill_parallel
 *
 * send a signal to all parallel applications running in a partition.
 *
 */
int kill_parallel (part_id, sig)
int part_id;
int sig;
{
	smd_hdr_t hdr;
	smd_resp_t data[MAX_RESP];
	int i, byte_read;
	int n_resp;
        extern int errno;
 
#ifdef SDSC
#ifdef DEBUG
	printf ("I$kill_parallel: calling smd_status, part_id = %d\n", part_id);
	fflush (stdout);
#endif
	byte_read = smd_status (part_id, QUALIFY_ANY, &hdr, (char *) &data[0], 
	sizeof (smd_resp_t) * MAX_RESP);
#ifdef DEBUG
	printf ("I$kill_parallel: Done calling smd_status, part_id = %d\n", 
	part_id);
	fflush (stdout);
#endif
#endif
	if (byte_read < 0) {
	    printf ("I$kill_parallel: smd error, cannot kill part %d\n",
	    part_id);
	    return (-1);
	}

	if (data[0].value.status != SMD_OK)
            return (0);

	/* process data */

	n_resp = byte_read / sizeof (smd_resp_t);
	for (i = 0; i < n_resp; i ++) {
	    if (data[i].q_id.part_id != part_id) {
	    	printf ("I$kill_para: mismatch, part_id =%d, smd_part_id =%d\n"
	    	, part_id, data[i].q_id.part_id);
		continue;
	    }
#ifdef DEBUG
	    printf ("I$kill_parallel: killing part_id =%d, pgid =%d, sig =%d\n",
	    part_id, -data[i].q_id.pgid, sig);
#endif
	    kill (-data[i].q_id.pgid, sig);
	}
	return (0);
    }

#endif
