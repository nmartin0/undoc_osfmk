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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: timers.h,v $
 * Revision 1.3  1994/11/18  21:11:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:18:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:01:29  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:20:45  cfj
 * Bump major revision number.
 *
 * Revision 1.4  1992/04/13  14:33:27  stans
 * "OSF-distribution-ad8.5.2"
 *
 * Revision 1.4  90/10/07  20:37:37  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:00:38  gm]
 * 
 * Revision 1.3  90/05/13  20:43:30  devrcs
 * 	Define unsupported timer function prototypes. They exist but
 * 	return ENOSYS.
 * 	[90/05/07  09:07:15  sp]
 * 
 * 	Add definition of struct itimercb to keep gcc quiet,
 * 	although it is never used.
 * 	Add function prototype definitions of supported functions
 * 	[90/05/07  08:38:18  sp]
 * 
 * Revision 1.2  90/04/27  23:07:45  devrcs
 * 	Initial version of 1003.4 timer defintions
 * 	[90/04/18  14:30:02  sp]
 * 
 * $EndLog$
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
