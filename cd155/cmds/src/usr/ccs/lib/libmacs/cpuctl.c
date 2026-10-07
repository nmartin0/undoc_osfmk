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
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * File:	cpuctl.c
 *
 * This file contains the client-MACD interface routine.
 * all communication between clients and MACD is done through
 * cpuctl() call.
 */

#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include "mac.h"
#include "macd.h"


/***************************************************************************
 * cpuctl (request, data)
 *
 * Abstract:  	This system-call-like routine communicates with MACD
 * 		it performs authentication, and send request and
 * 		get reply from and to macd.  
 *
 * Arguments:	request -	request type
 *		data -	pointer to a memory space where to store data
 *			to be send to MACD and to store data received
 *			MACD
 *
 * Return value:
 *		>=0	reply-return-value from MACD if successful
 *		-1 	error  and errno will be set
 *****************************************************************************/

int c_sock = -1;

struct macd_reply reply_hdr;
struct client_request reqst_hdr;

cpuctl( request, data )
int request;
char *data;
{
	int rsu_flag;	/* real user id must be super-user  */
	int esu_flag;	/* effective user id must be super-user */
	int data_flag;	/* there is data to be send */
	int writeout, remain, numchar, datasize, msgsize;
	char *reqst_buf, *ptr;
	extern int errno, c_open(), c_close();
	extern char *malloc();

/*
 * Separate routines for establishing and closing connection to RSC daemon
 */
	if (request == C_OPEN)	return(c_open());
	if (request == C_CLOSE) return(c_close());

/*
 * Invalid request number
 */
	if (request > MAX_REQNUM) {
#ifdef DEBUG
		(void) fprintf(stderr,"Invalid request number\n");
#endif
		errno = EINVAL;
		return(-1);
	}

/*
 * The mact requests: must have established a connection already
 */
	if (c_sock <= 0) {
#ifdef DEBUG
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
	else if (request == C_BOOT) rsu_flag = 1;
	else if (request == C_CREACCT) esu_flag = data_flag = 1;
	else if (request == C_ON_COUNT) rsu_flag = 1;
	else if (request == NQS_JOB_START) rsu_flag = data_flag = 1;
	else if (request == NQS_JOB_END) rsu_flag = data_flag = 1;
	else if (request == SCHED_DOWN) rsu_flag = data_flag = 1;
	else if (request == C_UPD_HOLD) rsu_flag = 1;
	else if (request == C_UPD_RESUME) rsu_flag = 1;
	else if (request == C_ISMACWATCH) rsu_flag = data_flag = 0;
	else {
#ifdef DEBUG
		(void) fprintf(stderr,"Invalid request number\n");
#endif
		errno = EINVAL;
		return(-1);
	}

/*
 * Real-super-user-only requests, check real user id
 */
#ifdef DEBUG
	if (rsu_flag) {
		(void) fprintf(stderr,"Cpuctl:  Check real user id...\n");
		(void) fprintf (stderr, "Cpuctl: Real user id = %d\n",
			getuid());
	}
#endif
	if (rsu_flag && (getuid() != 0)) {
#ifdef DEBUG
		(void) fprintf(stderr,"Cpuctl:  real user id = root required\n");
#endif
		errno = EACCES;
		return(-1);
	}

/*
 * Effective-super-user-only requests, check effictive user id
 */
#ifdef DEBUG
	if (esu_flag) (void) fprintf(stderr,"Cpuctl:  Check effective user id...\n");
#endif
	if (esu_flag && geteuid() != 0) {
#ifdef DEBUG
		(void) fprintf(stderr,"Cpuctl:  Effective user id = root required\n");
#endif
		errno = EACCES;
		return(-1);
	}

/*
 * Pack a request messge and send to MACD 
 */ 
	reqst_hdr.command = request; 
	if (data_flag) {
		if (request == NQS_JOB_START || request == NQS_JOB_END)
			reqst_hdr.size = sizeof(struct nqs_job_info);
		else if (request == SCHED_DOWN)
			reqst_hdr.size = sizeof(struct si_info);
		else reqst_hdr.size = sizeof(struct cpu_ctrl); 
	}
	else reqst_hdr.size = 0;
	msgsize = sizeof(struct client_request) + reqst_hdr.size;
	reqst_buf = (char *) malloc (msgsize);
	bcopy ( (char *)&reqst_hdr, reqst_buf, sizeof(struct client_request));
	bcopy ( data, reqst_buf+sizeof(struct client_request), reqst_hdr.size );


#ifdef DEBUG
	(void) dump_buf (reqst_buf, msgsize);
	writeout = write(c_sock, reqst_buf, msgsize);
	(void) fprintf(stderr,"Cpuctl:  %d/%d request bytes sent\n",
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
	if (request == NQS_JOB_START || request == NQS_JOB_END
		|| request == C_OFF || request == C_BOOT || request == C_SYNC
		|| request == SCHED_DOWN) return (0);

/*
 * Get reply message from MACD
 */
	for (remain=sizeof(struct macd_reply),ptr=(char *)&reply_hdr;remain;
		remain-=numchar,ptr+=numchar) {
		numchar = read (c_sock,ptr,remain);
#ifdef DEBUG
		(void) fprintf (stderr, "Cpuctl: %d/%d header bytes read:\n",
			numchar, remain);
#endif
		if (numchar<=0) {
			if(errno == EWOULDBLOCK) {
				numchar = 0;
#ifdef DEBUG
				(void) fprintf(stderr, 
					"Cpuctl:  errno=%d\n", errno);
#endif
				(void) sleep(1);
				continue;
			}
#ifdef DEBUG
			(void) fprintf(stderr, "Cpuctl:  errno=%d\n", errno);
#endif
			errno = EPIPE;
			return(-1);
		}
	}

#ifdef DEBUG
	(void) fprintf(stderr, "Cpuctl: %d bytes are read:\n", sizeof(struct macd_reply));
	(void) dump_buf((char *)&reply_hdr,sizeof(struct macd_reply));
#endif

/*
 * No reply data
 */
	if (reply_hdr.size == 0) {
		errno = reply_hdr.errno;
		return(reply_hdr.returnvalue);
	}

/*
 * Read reply data 
 */
	for(remain=reply_hdr.size,ptr=(char *)data;remain;
		remain-=numchar,ptr+=numchar) {
		numchar = read (c_sock,ptr,remain);
#ifdef DEBUG
		(void) fprintf(stderr, "Cpuctl: %d/%d data bytes read, errno=%d\n",
			numchar, remain, errno);
#endif
		if (numchar<=0) {
			if(errno == EWOULDBLOCK) {
				numchar = 0;
#ifdef DEBUG
				(void) fprintf(stderr, 
					"Cpuctl:  errno=%d\n", errno);
#endif
				(void) sleep(1);
				continue;
			}
#ifdef DEBUG
			(void) fprintf(stderr, "Cpuctl:  errno=%d\n", errno);
#endif
			errno = EPIPE;
			return(-1);
		}
	}
#ifdef DEBUG
	(void) fprintf(stderr, "Cpuctl: %d bytes are read:\n", 
		sizeof(struct macd_reply)+reply_hdr.size);
	(void) dump_buf((char *)&reply_hdr,sizeof(struct macd_reply));
	(void) dump_buf((char *)data,reply_hdr.size);
#endif

	errno = reply_hdr.errno;
	return(reply_hdr.returnvalue);
}
