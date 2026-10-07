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
 * $Log: ns_error.h,v $
 * Revision 1.5  1994/11/18  20:36:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:13:02  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:35:22  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:01:49  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:30:59  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:28:55  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:23:08  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:55  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:45:57  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:38:29  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:37:27  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:18:41  gm]
 * 
 * Revision 1.4  90/07/27  09:03:11  devrcs
 * 	XNS protocols, passthrough from BSD Reno release.
 * 	Operating under uniprocessor compat in OSF/1.
 * 	[90/07/19  17:10:08  tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1984, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Base:	ns_error.h	7.5 (Berkeley) 6/28/90
 */

/*
 * Xerox NS error messages
 */

struct ns_errp {
	u_short		ns_err_num;		/* Error Number */
	u_short		ns_err_param;		/* Error Parameter */
	struct idp	ns_err_idp;		/* Initial segment of offending
						   packet */
	u_char		ns_err_lev2[12];	/* at least this much higher
						   level protocol */
};
struct  ns_epidp {
	struct idp ns_ep_idp;
	struct ns_errp ns_ep_errp;
};

#define	NS_ERR_UNSPEC	0	/* Unspecified Error detected at dest. */
#define	NS_ERR_BADSUM	1	/* Bad Checksum detected at dest */
#define	NS_ERR_NOSOCK	2	/* Specified socket does not exist at dest*/
#define	NS_ERR_FULLUP	3	/* Dest. refuses packet due to resource lim.*/
#define	NS_ERR_UNSPEC_T	0x200	/* Unspec. Error occured before reaching dest*/
#define	NS_ERR_BADSUM_T	0x201	/* Bad Checksum detected in transit */
#define	NS_ERR_UNREACH_HOST	0x202	/* Dest cannot be reached from here*/
#define	NS_ERR_TOO_OLD	0x203	/* Packet x'd 15 routers without delivery*/
#define	NS_ERR_TOO_BIG	0x204	/* Packet too large to be forwarded through
				   some intermediate gateway.  The error
				   parameter field contains the max packet
				   size that can be accommodated */
#define NS_ERR_MAX 20

/*
 * Variables related to this implementation
 * of the network systems error message protocol.
 */
struct	ns_errstat {
/* statistics related to ns_err packets generated */
	int	ns_es_error;		/* # of calls to ns_error */
	int	ns_es_oldshort;		/* no error 'cuz old ip too short */
	int	ns_es_oldns_err;	/* no error 'cuz old was ns_err */
	int	ns_es_outhist[NS_ERR_MAX];
/* statistics related to input messages processed */
	int	ns_es_badcode;		/* ns_err_code out of range */
	int	ns_es_tooshort;		/* packet < IDP_MINLEN */
	int	ns_es_checksum;		/* bad checksum */
	int	ns_es_badlen;		/* calculated bound mismatch */
	int	ns_es_reflect;		/* number of responses */
	int	ns_es_inhist[NS_ERR_MAX];
	u_short	ns_es_codes[NS_ERR_MAX];/* which error code for outhist
					   since we might not know all */
};

#ifdef _KERNEL
extern	struct	ns_errstat ns_errstat;
#endif
