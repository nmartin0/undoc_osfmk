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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance
 *	with the terms of that agreement.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_killproc.c,v 1.3 1994/11/19 02:28:54 mtm Exp $
 *
 * Under score version of killproc
 * 
 */

/***************************  _killproc  ******************************
 *
 *      Description:
 *            User interface to kill process(es).
 *
 *      Calling Sequence:
 *            _killproc(node, pid);
 *
 *      Parameters:
 *            node:     Node number
 *            pid:      PID
 *
 *      Returns:
 *            -1 if error
 *
 *      Called by:
 *            User
 */

#include <errno.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>

_killproc(node, pid)
long node, pid;
{
	if (node == -1 && pid == -1) {
		int oldpgrp;
		int chpid;
		int err;
		int wstat;
		struct rusage ruse; 

		oldpgrp = getpgrp();

		if (chpid = vfork()) {
			do
				setpgid(0, chpid);
			while(getpgrp() == oldpgrp);
		}
		else {	/* Child process. */
			setpgid(0, 0);
			for (;;) sleep(10000);
		}

		do {
			err = killpg(oldpgrp, SIGKILL);
			if (!err) {
				err = wait3(&wstat, WNOHANG, &ruse);
				if (err == -1) {
					if (errno != ECHILD)
						break;
					else
						err = 0;
				}
			}
		} while (err != -1);

		err = errno;

		setpgid(0, 0);
		kill(chpid, SIGKILL);

		if (err != ESRCH)
			return (-1);
		else
			return (0);
	} else {
		errno = EINVAL;
		return (-1);
	}
}
