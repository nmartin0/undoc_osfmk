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
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/infocount.c,v 1.4 1994/11/19 02:30:27 mtm Exp $
 *
 * Returns length of message matched by probe or recv call.
 * 
 *
 */

extern long	_infocount();

/*
 * infocount
 * 
 * Description:	Returns the actual length of the message matched
 *		by a probe or recv call.
 *
 * Parameters:	None
 *
 * Returns:	Message length
 */
long
infocount()
{
	return(_infocount());
}
