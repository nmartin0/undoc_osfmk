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
 *      Copyright 1993  Intel Corporation.
 *
 */

/*
 * devstat()
 *
 * Description:	System call for device stat.
 *		
 * devstat(char *device_name, struct devstat *dstat)
 *
 * Returns:
 *	*dstat structure filled in. An interesting item is the node number
 *	of where the device is physically located. Not to be confused with
 *	the node number of where the UX server lives which provides higher
 *	level device service (see the "DEV_TAB" bootmagic string).
 */

        .file "devstat.s"

#include "SYS.h"

RSYSCALL(devstat)
