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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/part_service.c,v 1.2 1994/11/19 03:16:46 mtm Exp $
 *
 * HISTORY
 * $Log: part_service.c,v $
 * Revision 1.2  1994/11/19  03:16:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/10/05  23:30:26  rkl
 * Initial revision
 *
 *
 */

#include "defs.h"

#ifdef not_now
/*
 *  part_arlam:
 *
 *	Service partition alarm requests.
 */
part_alarm(user, req)
	smd_user_t	*user;
	smd_req_t	*req;
{
	TRACE(ENTRY, ("part_alarm(user=%x, req=%x)\n", user, req));
}

/*
 *  mon_part_usage:
 *
 *	Service SMD_MON_PARTITION_USAGE_REQ requests.
 */
mon_part_usage(user, req)
	smd_user_t	*user;
	smd_req_t	*req;
{
	TRACE(ENTRY, ("mon_part_usage(user=%x, req=%x)\n", user, req));
}

/*
 *  get_part_usage:
 *
 *	Service SMD_GET_PARTITION_USAGE_REQ requests.
 */
get_part_usage(user, req)
	smd_user_t	*user;
	smd_req_t	*req;
{
	TRACE(ENTRY, ("get_part_usage(user=%x, req=%x)\n", user, req));
}
#endif
