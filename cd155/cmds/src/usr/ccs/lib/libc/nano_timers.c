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
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: nano_timers.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:14 $";
#endif

#include <sys/time.h>
#include <errno.h>
#include <sys/timers.h>

#define NULL 0
#define NSEC_PER_USEC (1000)
#define NSEC_PER_SEC (1000000000)

#ifdef	_THREAD_SAFE
#include "rec_mutex.h"

extern struct rec_mutex _nanotimer_rmutex;
#define	TIMER_LOCK()	rec_mutex_lock(&_nanotimer_rmutex)
#define	TIMER_UNLOCK()	rec_mutex_unlock(&_nanotimer_rmutex)
#define	SETERR(err)	seterrno(err)
#define	RETURN(val)	return(rec_mutex_unlock(&_nanotimer_rmutex), val)
#else
#define	TIMER_LOCK()
#define	TIMER_UNLOCK()
#define	SETERR(err)	errno = err
#define	RETURN(val)	return(val)
#endif

static pid_t timer_allocated = (pid_t)0;

getclock(int clock_type, struct timespec *tp)
{
	struct timeval berktv;

	if (clock_type != TIMEOFDAY) {
		SETERR(EINVAL);
		return(-1);
	}
	if (gettimeofday(&berktv, NULL) < 0)
		return(-1);

	tp->tv_sec = berktv.tv_sec;
	tp->tv_nsec = berktv.tv_usec * NSEC_PER_USEC;
	return(0);
}

setclock(int clock_type, struct timespec *tp)
{
	struct timeval berktv;

	if (clock_type != TIMEOFDAY) {
		SETERR(EINVAL);
		return(-1);
	}
	if (tp->tv_nsec < 0 || tp->tv_nsec >= NSEC_PER_SEC) {
		SETERR(EINVAL);
		return(-1);
	}

	berktv.tv_sec = tp->tv_sec;
	berktv.tv_usec = tp->tv_nsec / NSEC_PER_USEC;
	return(settimeofday(&berktv, NULL));
}

int
resclock(int clock_type, struct timespec *res, struct timespec *maxval)
{
	SETERR(ENOSYS);
	return(-1);
}

timer_t
mktimer(int clock_type, int notify_type, struct itimercb *itimercbp)
{
	if (clock_type != TIMEOFDAY) {
		SETERR(EINVAL);
		return((timer_t)-1);
	}
	if (notify_type != DELIVERY_SIGNALS) {
		SETERR(EINVAL);
		return((timer_t)-1);
	}
	TIMER_LOCK();
	if (timer_allocated == getpid()) {
		SETERR(EAGAIN);
		RETURN((timer_t)-1);
	}
	timer_allocated = getpid();
	RETURN((timer_t)0);
}

/*
 * ITIMER_REAL_COE HACK XXXXX	Undocumented extension
 * for nano_timers which will be Cleared On Exec.
 * Hacks are in common with bsd/kern_{time,exec}.c 
 * *** To Be Removed when nano_timer support moves into the kernel ***
 */ 
#define ITIMER_REAL_COE 0xDEADBEEF

rmtimer(timer_t timerid)
{
	struct itimerval btimer;

	TIMER_LOCK();
	if (timer_allocated != getpid() || timerid != (timer_t)0) {
		SETERR(EINVAL);
		RETURN(-1);
	}
	timer_allocated = (pid_t)0;

	/* cancel pending timer, if any */

	btimer.it_value.tv_sec = 0;
	btimer.it_value.tv_usec = 0;
	btimer.it_interval.tv_sec = 0;
	btimer.it_interval.tv_usec = 0;
	if (setitimer( ITIMER_REAL_COE /*ITIMER_REAL*/, &btimer, NULL) < 0)
		RETURN(-1);

	RETURN(0);
}

gettimer(timer_t timerid, struct itimerspec *value)
{
	struct itimerval btimer;

	TIMER_LOCK();
	if (timer_allocated != getpid() || timerid != (timer_t)0) {
		SETERR(EINVAL);
		RETURN(-1);
	}
	if (getitimer(ITIMER_REAL, &btimer) < 0)
		RETURN(-1);

	value->it_value.tv_sec = btimer.it_value.tv_sec;
	value->it_value.tv_nsec = btimer.it_value.tv_usec * NSEC_PER_USEC;
	value->it_interval.tv_sec = btimer.it_interval.tv_sec;
	value->it_interval.tv_nsec = btimer.it_interval.tv_usec * NSEC_PER_USEC;
	RETURN(0);
}

reltimer(timer_t timerid, struct itimerspec *value, struct itimerspec *ovalue)
{
	struct itimerval btimer;
	struct itimerval obtimer;
	struct itimerval *obp;

	TIMER_LOCK();
	if (timer_allocated != getpid() || timerid != (timer_t)0) {
		SETERR(EINVAL);
		RETURN(-1);
	}
	if (value->it_value.tv_nsec >= NSEC_PER_SEC ||
	    value->it_interval.tv_nsec >= NSEC_PER_SEC ||
	    value->it_value.tv_nsec < 0 ||
	    value->it_interval.tv_nsec < 0) {
		SETERR(EINVAL);
		RETURN(-1);
	}
	btimer.it_value.tv_sec = value->it_value.tv_sec;
	btimer.it_value.tv_usec = value->it_value.tv_nsec / NSEC_PER_USEC;
	btimer.it_interval.tv_sec = value->it_interval.tv_sec;
	btimer.it_interval.tv_usec = value->it_interval.tv_nsec / NSEC_PER_USEC;
	if (setitimer( ITIMER_REAL_COE /*ITIMER_REAL*/,
					&btimer, ovalue ? &obtimer : NULL) < 0)
		RETURN(-1);
	if (ovalue != NULL) {
		ovalue->it_value.tv_sec = obtimer.it_value.tv_sec;
		ovalue->it_value.tv_nsec = obtimer.it_value.tv_usec * NSEC_PER_USEC;
		ovalue->it_interval.tv_sec = obtimer.it_interval.tv_sec;
		ovalue->it_interval.tv_nsec = obtimer.it_interval.tv_usec * NSEC_PER_USEC;
	}
	RETURN(0);
}

int
abstimer(timer_t timerid, struct itimerspec *value, struct itimerspec *ovalue)
{
	SETERR(ENOSYS);
	return(-1);
}

int
resrel(timer_t timerid, struct timespec *res, struct timespec *max)
{
	SETERR(ENOSYS);
	return(-1);
}

int
resabs(timer_t timerid, struct timespec *res, struct timespec *max)
{
	SETERR(ENOSYS);
	return(-1);
}

int
nanosleep(struct timespec *rqtp, struct timespec *rmtp)
{
	SETERR(ENOSYS);
	return(-1);
}

int
ressleep(struct timespec *res, struct timespec *max)
{
	SETERR(ENOSYS);
	return(-1);
}
