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
 *      $Header:
 *
 * History:
 * $Log: epoch_time.c,v $
 * Revision 1.2  1994/11/21  16:39:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/12/21  18:05:55  cameron
 * New command to print seconds since the epoch to stdout.
 *
 *
 */

/*
 * epoch_time.c	- Don Cameron	12/92
 *
 * Print seconds since epoch to keep track of when mesh was booted.
 *
 */
#include <time.h>
#include <stdio.h>

main() {

	printf("%d\n", time((time_t *) 0));
}
