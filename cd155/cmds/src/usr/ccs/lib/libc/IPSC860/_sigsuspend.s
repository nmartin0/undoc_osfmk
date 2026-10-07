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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */

/*
 *     int _sigsuspend(sigmaskp)
 *	sigset_t sigmaskp;
 *
 *     sigsuspend() replaces the process's signal mask with the set
 *     of signals contained  by sigmask and then suspends the pro-
 *     cess until delivery of a signal whose action  is  either  to
 *     execute  a signal-catching function or to terminate the pro-
 *     cess.  If the action  is  to  terminate  the  process,  sig-
 *     suspend()  does  not  return.  If the action is to execute a
 *     signal-catching function,  sigsuspend()  returns  after  the
 *     signal-catching  function  returns,  with  the  signal  mask
 *     restored to the setting  that  existed  prior  to  the  sig-
 *     suspend()  call.   It is not possible to block those signals
 *     that cannot be ignored, as documented in <signal.h> this  is
 *     enforced by the system without indicating an error.
 */

        .file   "_sigsuspend.s"

#include   "SYS.h"

RPSEUDO(_sigsuspend,sigsuspend) /* status = _sigsuspend(mask) */
