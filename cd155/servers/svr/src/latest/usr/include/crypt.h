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
 * $Log: crypt.h,v $
 * Revision 1.3  1994/11/18  21:09:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:13:29  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:56:49  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:03  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:17:43  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:34:42  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:26:24  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:43:51  gm]
 * 
 * Revision 1.2  90/08/09  14:26:04  devrcs
 * 	Initial Version
 * 	[90/08/02  10:19:54  sp]
 * 
 * $EndLog$
 */

/*
 * definitions for the reentrant versions of crypt()
 */

#ifndef	_CRYPT_H_
#define	_CRYPT_H_

#if defined(_REENTRANT) || defined(_THREAD_SAFE)
typedef	struct crypt_data {
	char C[28], D[28];
	char E[48];
	char KS[16][48];
	char block[66], iobuf[16];
} CRYPTD;

#ifdef _NO_PROTO
extern	char	*crypt_r();
extern	int	setkey_r();
extern	int	encrypt_r();
#else
extern	char	*crypt_r(char *, char *, CRYPTD *);
extern	int	setkey_r(char *, CRYPTD *);
extern	int	encrypt_r(char *, int, CRYPTD *);
#endif /* _NO_PROTO */
#endif
#endif	/* _CRYPT_H_ */
