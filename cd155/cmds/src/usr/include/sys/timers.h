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

#ifndef _TIMERS_H
#define _TIMERS_H

struct timespec {
	unsigned long	tv_sec;		/* seconds */
	long		tv_nsec;	/* and nanoseconds */
};

struct itimerspec {
	struct timespec	it_interval;	/* timer period */
	struct timespec	it_value;	/* timer expiration */
};

struct itimercb {
#ifdef _POSIX_AYNCHRONOUS_EVENTS
	struct event	itcb_event;	/* timer event definition */
#endif
	int		itcb_count;	/* timer "overrun" count */
};

typedef unsigned int timer_t;

#define TIMEOFDAY	1	/* time of day clock type */

/*
 * Notification types
 */
#define DELIVERY_SIGNALS	1
#define DELIVERY_EVENTS		2

/*
 * Functions
 */
int getclock(int clock_type, struct timespec *tp);
int setclock(int clock_type, struct timespec *tp);
int resclock(int clock_type, struct timespec *res, struct timespec *maxval);
timer_t mktimer(int clock_type, int notify_type, struct itimercb *itimercbp);
int rmtimer(timer_t timerid);
int gettimer(timer_t timerid, struct itimerspec *value);
int reltimer(timer_t timerid, struct itimerspec *value, struct itimerspec *ovalue);
int abstimer(timer_t timerid, struct itimerspec *value, struct itimerspec *ovalue);
int resrel(timer_t timerid, struct timespec *res, struct timespec *max);
int resabs(timer_t timerid, struct timespec *res, struct timespec *max);
int nanosleep(struct timespec *rqtp, struct timespec *rmtp);
int ressleep(struct timespec *res, struct timespec *max);

#endif	/* _TIMERS_H */
