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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ulimit.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 02:09:22 $";
#endif

#include <stdarg.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <ulimit.h>
#include <signal.h>
#include <sys/param.h>

#define	BLK	UBSIZE		/* number of bytes in filesize unit */

#ifdef  _THREAD_SAFE
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#endif

long
ulimit(int cmd, ...)
{
	struct rlimit rl;
	long	newlimit;
	va_list	ap;
	long	retval;
	struct sigaction action;

	va_start(ap, cmd);
	switch (cmd) {
	case UL_GETFSIZE:		/* get process file size limit */
		/*
		 * Warning: Sigaction hack.
		 * As we implement ulimit in terms of rlimits there
		 * is a problem. According to rlimits if you excede
		 * the file size limit then you get sent the signal
		 * SIGXFSZ (which by default terminates the process)
		 * Ulimit however want EFBIG returned. The kernel
		 * does both so we ignore the signal for people who call
		 * ulimit.
		 */
		action.sa_handler = SIG_IGN;
		sigaction(SIGXFSZ, &action, NULL);
		if (getrlimit(RLIMIT_FSIZE, &rl) == 0)
			retval = (long)(rl.rlim_cur / BLK);
		else
			retval = (long)-1;
		break;
	case UL_SETFSIZE:		/* set process file size limit */
		/*
		 * Warning: Sigaction hack.
		 * see above.
		 */
		action.sa_handler = SIG_IGN;
		sigaction(SIGXFSZ, &action, NULL);
		/*
		 * We have a small problem here that ulimit only knows
		 * about one number and our resource limits have two so
		 * we have to try and keep them in step in a sensible manner,
		 * here's what happens:
		 *
		 * If new > max
		 *	if privileged
		 *		set both cur and max to new
		 *	else
		 *		EPERM
		 * else if new < cur
		 *	set both cur and max to new
		 * else if cur < new < max
		 *	set max to cur
		 *	if privileged
		 *		set cur to new
		 *	else
		 *		EPERM
		 *
		 * The case that is not dealt with very elegantly is when
		 * cur > max but to do this you must be privileged so attempts
		 * at keeping cur and max in step are pretty futile.
		 */
		newlimit = va_arg(ap, long) * BLK;
		if (getrlimit(RLIMIT_FSIZE, &rl) < 0) {
			retval = (long)-1;
			break;
		}
		if ((newlimit <= rl.rlim_cur) || (newlimit > rl.rlim_max))
			rl.rlim_max = (unsigned int)newlimit;
		else if (rl.rlim_cur < rl.rlim_max) {
			rl.rlim_max = rl.rlim_cur;
			if (setrlimit(RLIMIT_FSIZE, &rl) < 0) {
				retval = (long)-1;
				break;
			}
		}
		rl.rlim_cur = (unsigned int)newlimit;
		if (setrlimit(RLIMIT_FSIZE, &rl) == 0)
			retval = (long)(rl.rlim_cur / BLK);
		else
			retval = (long)-1;
		break;
	case UL_GETBREAK:		/* get maximum possible break value */
		if (getrlimit(RLIMIT_DATA, &rl) == 0)
			retval = (long)rl.rlim_max;
		else
			retval = (long)-1;
		break;
	case UL_GETNOFILES:		/* get maximum number of open files */
		retval = getdtablesize();
		break;
	default:			/* oops. bad cmd argument. */
		SETERR(EINVAL);
		retval = (long)-1;
		break;
	}
	va_end(ap);
	return(retval);
}
