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
 
/*++ verifyhdr.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/verifyhdr.c,v $
 *
 * DESCRIPTION:
 *
 *	Verify the common portion of a rawreq structure.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	March 28, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:28:03 $ $State: Exp $)
 * $Log: verifyhdr.c,v $
 * Revision 1.3  1994/11/19  02:28:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:00:06  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:20  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:57:08  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"


/*** verifyhdr
 *
 *
 *	int verifyhdr():
 *	Verify the common portion of a rawreq structure.
 *
 *	Returns:
 *		 0: if common header is valid.
 *		-1: otherwise.
 */
int verifyhdr (rawreq)
struct rawreq *rawreq;			/* Raw request header */
{
	/*
	 * "rawreq->ndatafiles > 32767" will be an invariant comparison
	 * when compiled on a machine with 16 bit shorts.
	 */
	if (rawreq->magic1 != REQ_MAGIC1 ||
	    rawreq->trans_id < 0 || rawreq->trans_id > MAX_TRANSACTS ||
	    rawreq->trans_quename [MAX_QUEUENAME] != '\0' ||
	    rawreq->quename [0] == '\0' ||
	    rawreq->quename [MAX_QUEUENAME] != '\0' ||
	    rawreq->type != RTYPE_BATCH && rawreq->type != RTYPE_DEVICE ||
	    rawreq->orig_seqno < -1 || rawreq->orig_seqno > MAX_SEQNO_USER ||
	    rawreq->rpriority < -1 || rawreq->rpriority > MAX_RPRIORITY ||
	    rawreq->ndatafiles < 0 || rawreq->ndatafiles > 32767 ||
	    rawreq->reqname [0] == '\0' ||
	    rawreq->reqname [MAX_REQNAME] != '\0' ||
	    rawreq->username [0] == '\0' ||
	    rawreq->username [MAX_ACCOUNTNAME] != '\0' ||
	    rawreq->mail_name [0] == '\0' ||
	    rawreq->mail_name [MAX_ACCOUNTNAME] != '\0') {
		/*
		 *  Values out of bounds.
		 */
		return (-1);
	}
	/*
	 *  No errors detected in the common portion of the rawreq.
	 */
	return (0);
}
