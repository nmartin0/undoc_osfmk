// 
// $Copyright
// Copyright 1993, 1994, 1995  Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
/*
 * Copyright 1988, 1989, 1990, 1991, 1992, by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

/*
 *     struct sigstack {
 *        caddr_t     ss_sp;
 *        int    ss_onstack;
 *     };
 *
 *     sigstack(ss, oss);
 *	struct sigstack *ss, *oss;
 *
 *     Sigstack allows users to define an alternate stack on which
 *     signals are to be processed.  If ss is non-zero, it speci-
 *     fies a signal stack on which to deliver signals and tells
 *     the system if the process is currently executing on that
 *     stack.  When a signal's action indicates its handler should
 *     execute on the signal stack (specified with a sigvec(2)
 *     call), the system checks to see if the process is currently
 *     executing on that stack.  If the process is not currently
 *     executing on the signal stack, the system arranges a switch
 *     to the signal stack for the duration of the signal handler's
 *     execution. If oss is non-zero, the current signal stack
 *     state is returned.
 */

        .file "sigstack.s"

#include "SYS.h"

RSYSCALL(sigstack)

