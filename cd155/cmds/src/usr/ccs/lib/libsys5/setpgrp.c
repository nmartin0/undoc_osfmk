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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */

#ifndef lint
#ifndef _NOIDENT
static char rcsid[] = "@(#) $RCSfile: setpgrp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:37:12 $";
#endif
#endif

#include <sys/types.h>

extern pid_t setsid();

pid_t
setpgrp(void)
{
  return setsid();
}
