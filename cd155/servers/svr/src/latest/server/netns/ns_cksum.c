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
 * $Log: ns_cksum.c,v $
 * Revision 1.5  1994/11/18  20:36:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:12:41  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:35:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:01:37  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:30:51  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:28:49  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:22:54  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:50  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:45:43  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:38:20  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  14:37:14  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:06:14  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:18:20  gm]
 * 
 * Revision 1.2  90/07/27  09:03:04  devrcs
 * 	XNS protocols, passthrough from BSD Reno release.
 * 	Operating under uniprocessor compat in OSF/1.
 * 	[90/07/19  17:08:46  tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1988 Regents of the University of California.
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
 *	Base:	ns_cksum.c	7.5 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"

#include "sys/param.h"
#include "sys/mbuf.h"

/*
 * Checksum routine for Network Systems Protocol Packets (Portable version)
 *
 * This routine is very heavily used in the network
 * code and should be modified for each CPU to be as fast as possible.
 */

#define ADDCARRY(x)  { if ((x) > 65535) (x) -= 65535; }
#define FOLD(x) {l_util.l = (x); (x) = l_util.s[0] + l_util.s[1]; ADDCARRY(x);}

u_short
ns_cksum(m, len)
	register struct mbuf *m;
	register int len;
{
	register u_short *w;
	register int sum = 0;
	register int mlen = 0;
	register int sum2;

	union {
		u_short s[2];
		long	l;
	} l_util;

	for (;m && len; m = m->m_next) {
		if (m->m_len == 0)
			continue;
		/*
		 * Each trip around loop adds in
		 * word from one mbuf segment.
		 */
		w = mtod(m, u_short *);
		if (mlen == -1) {
			/*
			 * There is a byte left from the last segment;
			 * ones-complement add it into the checksum.
			 */
#if	BYTE_ORDER == BIG_ENDIAN
			sum  += *(u_char *)w;
#else
			sum  += *(u_char *)w << 8;
#endif
			sum += sum;
			w = (u_short *)(1 + (char *)w);
			mlen = m->m_len - 1;
			len--;
			FOLD(sum);
		} else
			mlen = m->m_len;
		if (len < mlen)
			mlen = len;
		len -= mlen;
		/*
		 * We can do a 16 bit ones complement sum using
		 * 32 bit arithmetic registers for adding,
		 * with carries from the low added
		 * into the high (by normal carry-chaining)
		 * so long as we fold back before 16 carries have occured.
		 */
		if (1 & (int) w)
			goto uuuuglyy;
#ifndef TINY
/* -DTINY reduces the size by ~55%, but slows it down by ~20% */
		while ((mlen -= 32) >= 0) {
			sum += w[0]; sum += sum; sum += w[1]; sum += sum;
			sum += w[2]; sum += sum; sum += w[3]; sum += sum;
			sum += w[4]; sum += sum; sum += w[5]; sum += sum;
			sum += w[6]; sum += sum; sum += w[7]; sum += sum;
			FOLD(sum);
			sum += w[8]; sum += sum; sum += w[9]; sum += sum;
			sum += w[10]; sum += sum; sum += w[11]; sum += sum;
			sum += w[12]; sum += sum; sum += w[13]; sum += sum;
			sum += w[14]; sum += sum; sum += w[15]; sum += sum;
			FOLD(sum);
			w += 16;
		}
		mlen += 32;
#endif
		while ((mlen -= 8) >= 0) {
			sum += w[0]; sum += sum; sum += w[1]; sum += sum;
			sum += w[2]; sum += sum; sum += w[3]; sum += sum;
			FOLD(sum);
			w += 4;
		}
		mlen += 8;
		while ((mlen -= 2) >= 0) {
			sum += *w++; sum += sum;
		}
		goto commoncase;
uuuuglyy:
#if	BYTE_ORDER == BIG_ENDIAN
#define ww(n) (((u_char *)w)[n + n + 1])
#define vv(n) (((u_char *)w)[n + n])
#else
#define ww(n) (((u_char *)w)[n + n])
#define vv(n) (((u_char *)w)[n + n + 1])
#endif
		sum2 = 0;
#ifndef TINY
		while ((mlen -= 32) >= 0) {
		    sum += ww(0); sum += sum; sum += ww(1); sum += sum;
		    sum += ww(2); sum += sum; sum += ww(3); sum += sum;
		    sum += ww(4); sum += sum; sum += ww(5); sum += sum;
		    sum += ww(6); sum += sum; sum += ww(7); sum += sum;
		    FOLD(sum);
		    sum += ww(8); sum += sum; sum += ww(9); sum += sum;
		    sum += ww(10); sum += sum; sum += ww(11); sum += sum;
		    sum += ww(12); sum += sum; sum += ww(13); sum += sum;
		    sum += ww(14); sum += sum; sum += ww(15); sum += sum;
		    FOLD(sum);
		    sum2 += vv(0); sum2 += sum2; sum2 += vv(1); sum2 += sum2;
		    sum2 += vv(2); sum2 += sum2; sum2 += vv(3); sum2 += sum2;
		    sum2 += vv(4); sum2 += sum2; sum2 += vv(5); sum2 += sum2;
		    sum2 += vv(6); sum2 += sum2; sum2 += vv(7); sum2 += sum2;
		    FOLD(sum2);
		    sum2 += vv(8); sum2 += sum2; sum2 += vv(9); sum2 += sum2;
		    sum2 += vv(10); sum2 += sum2; sum2 += vv(11); sum2 += sum2;
		    sum2 += vv(12); sum2 += sum2; sum2 += vv(13); sum2 += sum2;
		    sum2 += vv(14); sum2 += sum2; sum2 += vv(15); sum2 += sum2;
		    FOLD(sum2);
		    w += 16;
		}
		mlen += 32;
#endif
		while ((mlen -= 8) >= 0) {
		    sum += ww(0); sum += sum; sum += ww(1); sum += sum;
		    sum += ww(2); sum += sum; sum += ww(3); sum += sum;
		    FOLD(sum);
		    sum2 += vv(0); sum2 += sum2; sum2 += vv(1); sum2 += sum2;
		    sum2 += vv(2); sum2 += sum2; sum2 += vv(3); sum2 += sum2;
		    FOLD(sum2);
		    w += 4;
		}
		mlen += 8;
		while ((mlen -= 2) >= 0) {
			sum += ww(0); sum += sum;
			sum2 += vv(0); sum2 += sum2;
			w++;
		}
		sum += (sum2 << 8);
commoncase:
		if (mlen == -1) {
#if	BYTE_ORDER == BIG_ENDIAN
			sum += *(u_char *)w << 8;
#else
			sum += *(u_char *)w;
#endif
		}
		FOLD(sum);
	}
	if (mlen == -1) {
		/* We had an odd number of bytes to sum; assume a garbage
		   byte of zero and clean up */
		sum += sum;
		FOLD(sum);
	}
	/*
	 * sum has already been kept to low sixteen bits.
	 * just examine result and exit.
	 */
	if(sum==0xffff) sum = 0;
	return (sum);
}
