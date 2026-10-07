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
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/infotype.c,v 1.5 1994/11/19 02:30:34 mtm Exp $
 *
 * Returns message type for message matched with probe or recv call.
 * 
 *
 */

extern long	_infotype();

/*
 * infotype
 * 
 * Description:	Gets the type of the message for the last synchronous
 *		probe or recv call.  This can also be used after a
 *		msgdone if the return value indicates the message was
 *		received.
 *
 * Parameters:	None
 *
 * Returns:	Message type of last recv or probe message
 */
long
infotype()
{
	return(_infotype());
}
