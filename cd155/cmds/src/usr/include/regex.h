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
 * definitions for the reentrant versions of regular expression functions
 */

#ifndef	_REGEX_H_
#define	_REGEX_H_

#if defined(_REENTRANT) || defined(_THREAD_SAFE)

#define	ESIZE	512
#define	NBRA	9

typedef	struct regex_data {
	char	expbuf[ESIZE],
		*braslist[NBRA],
		*braelist[NBRA],
		circf;
} REGEXD;

#ifdef	_NO_PROTO
extern	char	*re_comp_r();
extern	int	re_exec_r();
#else
extern	char	*re_comp_r(char *, REGEXD *);
extern	int	re_exec_r(char *, REGEXD *);
#endif	/* _NO_PROTO */
#endif	/* _REENTRANT || _THREAD_SAFE */
#endif	/* _REGEX_H_ */
