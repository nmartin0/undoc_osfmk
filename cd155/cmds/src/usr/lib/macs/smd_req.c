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
 * File:	smd_req.c
 *
 * Abstract:	This file contains the routines which send
 *		requests to Schedule Monitor Daemon (SMD).
 */

#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include "macd.h"
#include "appacct.h"
#include "smd.h"
#include "filename.h"

extern	errno;
char *index();

/******************************************************************************
*
* smStatuReq()
*
* Abstract:     This routine sends APP_STATUS_REQUEST message to SMD.
*		It first checks the SMD connection, if sm_sock is -1,
*		which means the connection is down, it will try to
*		re-establish a connection with SMD, if successful, it
*		then send the status request thru.
*
* Arguments:    None
*
* Return value: 0 -	successful
*		-1 -	error
*
******************************************************************************/

smStatuReq()
{
	struct smd_req reqst;
	int rval, msglen;
	char *ptr;
	extern int sm_sock, sm_conn, errno;
	extern void _sendmail(), bzero();
	extern char *strcpy();
	extern char *timstr();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr, "Enter smStatuReq()\n");
	(void) fflush (stderr);
    }

/*
 * open sm_sock if down
 */
	if (_debug_ != NO_SMD && sm_sock == -1 && smOpen() != 0) {
		if (sm_conn == UP) {
                        (void) _sendmail (SMD_DISCONN, NULL);
                        sm_conn = DOWN;
			(void) printf("FAILSMD   : %s - SMD not responding\n",
				timstr(0));
			(void) fflush (stdout);
                }
		sm_sock = -1;
		errno = EPIPE;
    if (_debug_) {
	(void) fprintf (stderr, "smOpen() failed\n");
	(void) fflush (stderr);
    }
		(void) unlink (MACD_TEMPLOG_FNAME);
		(void) unlink (MACD_LASTLOG_FNAME);
		return (-1);
	}


/*
 * Pack a status request messge and send to scheduler monitor
 */
	msglen = sizeof (struct smd_req);
	reqst.hdr.type = SMD_GET_STATUS_REQ;
	reqst.hdr.len = msglen - sizeof(int)*2;
	reqst.q_id.acct_id = QUALIFY_ANY;
	reqst.q_id.uid = QUALIFY_ANY;
	reqst.q_id.part_id = QUALIFY_ANY;
	reqst.q_id.pgid = QUALIFY_ANY;
	(void) strcpy (reqst.q_id.req_id, "macd");
	if (_debug_ != NO_SMD)
	    for (ptr=(char *)&reqst; msglen>0; msglen-=rval, ptr+=rval) {
		rval = write(sm_sock, ptr, msglen);
		if (rval <= 0) {
    if (_debug_) {
	(void) fprintf (stderr, "Failed in write to smd\n");
	(void) fflush (stderr);
    }
			return (-1);
		}
	}

/*
 * Pack a partition usage request messge and send to scheduler monitor
	msglen = sizeof (struct smd_req);
	(void) bzero ((char *)&reqst, msglen);
	reqst.hdr.type = SMD_GET_PARTITION_STATUS_REQ;
	reqst.hdr.len = msglen - sizeof(int)*2;
	reqst.q_id.acct_id = QUALIFY_ANY;
	reqst.q_id.uid = QUALIFY_ANY;
	reqst.q_id.part_id = QUALIFY_ANY;
	reqst.q_id.pgid = QUALIFY_ANY;
	(void) strcpy (reqst.q_id.req_id, "macd");
	if (_debug_ != NO_SMD)
	    for (ptr=(char *)&reqst; msglen>0; msglen-=rval, ptr+=rval) {
		rval = write(sm_sock, ptr, msglen);
		if (rval <= 0) return (-1);
	}
 */
	return (0);
}


/******************************************************************************
*
* smAlarmReq()
*
* Abstract:     This routine sends APP_ALARM_REQUEST message to SMD.
*		It first checks the SMD connection, if sm_sock is -1,
*		which means the connection is down, it will try to
*		re-establish a connection with SMD, if successful, it
*		then send the alarm request thru.  It assumes the
*		pointers in macd_ref are set to point to all relevant
*		entries for the applications in the application tree.
*		An account-level alarm will effect all users/processes
*		under the specified account, but an user-level alarm
*		will effect only the processes under the specified
*		user account.
*
* Arguments:    type -	alarm type, either account level or user level
*		alarm_val -	CPU node-time usage level to "trigger alarm"
*
* Return value: 0 -	successful
*		-1 -	error
*
******************************************************************************/
smAlarmReq(type, alarm_val)
int type;
long alarm_val;
{
	struct smd_req reqst;
	int rval, msglen;
	char *ptr, buf[80];
	extern struct app_ref macd_ref;
	extern int sm_sock, sm_conn, errno;
	extern void _sendmail();
	extern void bzero();
	extern char *strcpy();
	extern char *strncpy();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr,
            "Enter smAlarmReq(type=%d, alarm_val=%d)\n", type, alarm_val);
	(void) fflush (stderr);
    }

	if (type != ACCT_ALARM && type != USER_ALARM) return (-1);
	if (macd_ref.acct_ptr == NULL) return (-1);
	else if (macd_ref.user_ptr == NULL) return (-1);

/*
 * open sm_sock if down
 */
	if (_debug_ != NO_SMD && sm_sock == -1 && smOpen() != 0) {
		if (sm_conn == UP) {
                        (void) _sendmail (SMD_DISCONN, NULL);
                        sm_conn = DOWN;
                }
		sm_sock = -1;
		errno = EPIPE;
		return (-1);
	}


/*
 * Pack a alarm request messge and send to scheduler monitor
 */
	msglen = sizeof (struct smd_req);
	(void) bzero ((char *)&reqst, msglen);
	reqst.hdr.type = SMD_SET_APP_ALARM_REQ;
	reqst.hdr.len = msglen - sizeof(int)*2;
	reqst.q_id.acct_id = macd_ref.acct_ptr->acct_id;
	if (type == ACCT_ALARM) reqst.q_id.uid = QUALIFY_ANY;
	else reqst.q_id.uid = macd_ref.user_ptr->uid;
	reqst.q_id.part_id = QUALIFY_ANY;
	reqst.q_id.pgid = QUALIFY_ANY;
	reqst.param.alarm_val = alarm_val;
	(void) sprintf (buf, "%d%c%d", reqst.q_id.acct_id,
		(type == ACCT_ALARM) ? '\0' : '.', reqst.q_id.uid);
	(void) strncpy (reqst.q_id.req_id, buf, SMD_REQ_ID_LEN-1);
	reqst.q_id.req_id[SMD_REQ_ID_LEN-1] = '\0';
	if (type == ACCT_ALARM) 
	    (void) strcpy (macd_ref.acct_ptr->req_id, reqst.q_id.req_id);
	else (void) strcpy (macd_ref.user_ptr->req_id, reqst.q_id.req_id);
	if (_debug_ != NO_SMD)
	    for (ptr=(char *)&reqst; msglen>0; msglen-=rval, ptr+=rval) {
		rval = write(sm_sock, ptr, msglen);
		if (rval <= 0) return (-1);
	}

/*
 * Pack a partition usage request messge and send to scheduler monitor
	msglen = sizeof (struct smd_req);
	reqst.hdr.type = SMD_GET_PARTITION_USAGE_REQ;
	reqst.hdr.len = msglen - sizeof(int)*2;
	reqst.q_id.acct_id = QUALIFY_ANY;
	reqst.q_id.uid = QUALIFY_ANY;
	reqst.q_id.part_id = QUALIFY_ANY;
	reqst.q_id.pgid = QUALIFY_ANY;
	(void) strcpy (reqst.q_id.req_id, "macd");
	if (_debug_ != NO_SMD)
	    for (ptr=(char *)&reqst; msglen>0; msglen-=rval, ptr+=rval) {
		rval = write(sm_sock, ptr, msglen);
		if (rval <= 0) return (-1);
	}
 */
	return (0);
}

/******************************************************************************
*
* smAlarmCancel()
*
* Abstract:     This routine sends APP_ALARM_CANCEL message to SMD to
*		cancel a previously requested alarm.
*		It first checks the SMD connection, if sm_sock is -1,
*		which means the connection is down, it will try to
*		re-establish a connection with SMD, if successful, it
*		then send the alarm request thru.  It assumes the
*		pointers in macd_ref are set to point to all relevant
*		entries for the applications in the application tree.
*		An account-level alarm will effect all users/processes
*		under the specified account, but an user-level alarm
*		will effect only the processes under the specified
*		user account.
*
* Arguments:    type -	alarm type, either account level or user level
*
* Return value: 0 -	successful
*		-1 -	error
*
******************************************************************************/

smAlarmCancel(type, pass_ref)
struct app_ref *pass_ref;
int type;
{
	struct smd_req reqst;
	int rval, msglen;
	char *ptr;
	extern int sm_sock, sm_conn, errno;
	extern void _sendmail();
	extern void bzero();
	extern char *strncpy();
    extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr, "Enter smAlarmCancel(type=%d)\n", type);
	(void) fflush (stderr);
    }

	if (type != ACCT_ALARM && type != USER_ALARM) return (-1);
	if (pass_ref->acct_ptr == NULL) return (-1);
	else if (pass_ref->user_ptr == NULL) return (-1);

/*
 * open sm_sock if down
 */
	if (_debug_ != NO_SMD && sm_sock == -1 && smOpen() != 0) {
		if (sm_conn == UP) {
                        (void) _sendmail (SMD_DISCONN, NULL);
                        sm_conn = DOWN;
                }
		sm_sock = -1;
		errno = EPIPE;
		return (-1);
	}


/*
 * Pack an alarm cancel request messge and send to scheduler monitor
 */
	msglen = sizeof (struct smd_req);
	(void) bzero ((char *)&reqst, msglen);
	reqst.hdr.type = SMD_CANCEL_REQ;
	reqst.hdr.len = msglen - sizeof(int)*2;
	reqst.q_id.acct_id = pass_ref->acct_ptr->acct_id;
	if (type == ACCT_ALARM) reqst.q_id.uid = QUALIFY_ANY;
	else reqst.q_id.uid = pass_ref->user_ptr->uid;
	reqst.q_id.part_id = QUALIFY_ANY;
	reqst.q_id.pgid = QUALIFY_ANY;
	reqst.param.service = SMD_SET_APP_ALARM_REQ;
	(void) strncpy (reqst.q_id.req_id, (type == ACCT_ALARM) ?
	    pass_ref->acct_ptr->req_id : pass_ref->user_ptr->req_id, 
	    SMD_REQ_ID_LEN-1);
	reqst.q_id.req_id[SMD_REQ_ID_LEN-1] = '\0';
	if (_debug_ != NO_SMD)
	    for (ptr=(char *)&reqst; msglen>0; msglen-=rval, ptr+=rval) {
		rval = write(sm_sock, ptr, msglen);
		if (rval <= 0) return (-1);
	}
	if (type == ACCT_ALARM) pass_ref->acct_ptr->req_id[0] = '\0';
	else pass_ref->user_ptr->req_id[0] = '\0';
	return (0);
}
