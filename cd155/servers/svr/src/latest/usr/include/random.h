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
 * $Log: random.h,v $
 * Revision 1.3  1994/11/18  21:10:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:41  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:35  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:18:14  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:45  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:38:48  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:33:59  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:55:08  gm]
 * 
 * Revision 1.2  90/08/09  14:27:54  devrcs
 * 	Initial Version
 * 	[90/08/02  10:20:13  sp]
 * 
 * $EndLog$
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
