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
 * @OSF_FREE_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: machid_lib.c,v $
 * Revision 1.2  1994/11/19  02:25:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/05/04  00:26:43  stans
 * Initial revision
 *
 * Revision 1.3  1992/06/15  15:27:23  devrcs
 * 	Source changes for NORMA_MK9...
 * 	[1992/06/11  08:22:17  david]
 *
 * Revision 2.2  90/09/12  16:31:46  rpd
 * 	Created.
 * 	[90/06/18            rpd]
 * 
 * $EndLog$
 */

#include <servers/machid_types.h>

char *
mach_type_string(type)
    mach_type_t type;
{
    switch (type) {
      case MACH_TYPE_NONE:
	return "none";
      case MACH_TYPE_TASK:
	return "task";
      case MACH_TYPE_THREAD:
	return "thread";
      case MACH_TYPE_PROCESSOR_SET:
	return "processor set";
      case MACH_TYPE_PROCESSOR_SET_NAME:
	return "processor set name";
      case MACH_TYPE_PROCESSOR:
	return "processor";
      case MACH_TYPE_HOST:
	return "host";
      case MACH_TYPE_HOST_PRIV:
	return "privileged host";
      case MACH_TYPE_OBJECT:
	return "memory object";
      case MACH_TYPE_OBJECT_CONTROL:
	return "memory object control";
      case MACH_TYPE_OBJECT_NAME:
	return "memory object name";
      case MACH_TYPE_MASTER_DEVICE:
	return "master device";
      default:
	return "unknown";
    }
}
