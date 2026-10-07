/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * taken from tnc_mig.c bhk 2/14/92
 * 
 */
/*
 * Wrapper to compile bsd servers without KERNEL defined.
 */
#include <sys/param.h>
#include <sys/socket.h>
#include <net/if.h>
#include <vsocket/vs_chouse.h>
#include <vsocket/vs_types_gen.h>

#undef	KERNEL
#include <vsocket/vs_server.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <vsocket/vs_user.c>
