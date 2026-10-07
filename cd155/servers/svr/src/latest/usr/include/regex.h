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
 * $Log: regex.h,v $
 * Revision 1.3  1994/11/18  21:10:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:46  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:37  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:18:16  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:48  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:38:52  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:34:03  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:55:14  gm]
 * 
 * Revision 1.2  90/08/09  14:27:57  devrcs
 * 	Initial Version
 * 	[90/08/02  10:20:31  sp]
 * 
 * $EndLog$
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
