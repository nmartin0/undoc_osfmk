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

/*
 * defintions for the reentrant versions of random()
 */
#ifndef	_RANDOM_H_
#define	_RANDOM_H_

/*
 * random_r.h
 *
 * Include file for thread-safe version of random.
 */

 typedef struct random_data {
 	long	*fptr;
	long	*rptr;
	long	*state;
	int	rand_type;
	int	rand_deg;
	int	rand_sep;
	long	*end_ptr;
} RANDOMD;

/* functions */

#ifdef _NO_PROTO
extern	int	srandom_r();
extern	int	initstate_r();
extern	char	*setstate_r();
extern	long	random_r();
#else
extern	int	srandom_r(unsigned, RANDOMD *);
extern	int	initstate_r(unsigned, char *, int, RANDOMD *);
extern	char	*setstate_r(char *, RANDOMD *);
extern	int	random_r(RANDOMD *, int *);
#endif /* _NO_PROTO */

#endif	_RANDOM_H_
