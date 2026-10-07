/*
 *    i860/ptrace.h
 */
/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */


#ifndef _I860_PTRACE_H_
#define _I860_PTRACE_H_

#ifndef _KERNEL

#include <standards.h>

_SSD_FUNC_PROTO_BEGIN_

extern int ptrace __protomagic((int, int, int*, int));

_SSD_FUNC_PROTO_END_

#endif  /* _KERNEL */

#endif /*_I860_PTRACE_H_*/

