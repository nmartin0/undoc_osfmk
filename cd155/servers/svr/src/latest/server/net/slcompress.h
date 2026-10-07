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
 * $Log: slcompress.h,v $
 * Revision 1.5  1994/11/18  20:34:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:09:49  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:29:35  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:00:37  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:27:34  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:26:06  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:19:55  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:15  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:41:58  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.0  91/07/31  15:30:50  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:33:23  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:12:35  gm]
 * 
 * Revision 1.3  90/07/27  09:00:11  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  16:59:57  tmt]
 * 
 * Revision 1.2  90/04/27  19:14:26  devrcs
 * 	SLIP header compression (RFC1144), from BSD.
 * 	[90/04/20  12:37:01  tmt]
 * 
 * $EndLog$
 */

/*
 * Definitions for tcp compression routines.
 *
 * Copyright (c) 1989 Regents of the University of California.
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
 *	Van Jacobson (van@helios.ee.lbl.gov), Dec 31, 1989:
 *	- Initial distribution.
 *
 *	Base:	slcompress.h	7.3	90/01/20
 *	Base:	slcompress.h,v 1.10 89/12/31 08:53:02 van Exp
 *	Merged:	slcompress.h	7.4	90/06/28
 */

#define MAX_STATES 16		/* must be > 2 and < 256 */
#define MAX_HDR MLEN		/* XXX 4bsd-ism: should really be 128 */

/*
 * Compressed packet format:
 *
 * The first octet contains the packet type (top 3 bits), TCP
 * 'push' bit, and flags that indicate which of the 4 TCP sequence
 * numbers have changed (bottom 5 bits).  The next octet is a
 * conversation number that associates a saved IP/TCP header with
 * the compressed packet.  The next two octets are the TCP checksum
 * from the original datagram.  The next 0 to 15 octets are
 * sequence number changes, one change per bit set in the header
 * (there may be no changes and there are two special cases where
 * the receiver implicitly knows what changed -- see below).
 * 
 * There are 5 numbers which can change (they are always inserted
 * in the following order): TCP urgent pointer, window,
 * acknowlegement, sequence number and IP ID.  (The urgent pointer
 * is different from the others in that its value is sent, not the
 * change in value.)  Since typical use of SLIP links is biased
 * toward small packets (see comments on MTU/MSS below), changes
 * use a variable length coding with one octet for numbers in the
 * range 1 - 255 and 3 octets (0, MSB, LSB) for numbers in the
 * range 256 - 65535 or 0.  (If the change in sequence number or
 * ack is more than 65535, an uncompressed packet is sent.)
 */

/*
 * Packet types (must not conflict with IP protocol version)
 *
 * The top nibble of the first octet is the packet type.  There are
 * three possible types: IP (not proto TCP or tcp with one of the
 * control flags set); uncompressed TCP (a normal IP/TCP packet but
 * with the 8-bit protocol field replaced by an 8-bit connection id --
 * this type of packet syncs the sender & receiver); and compressed
 * TCP (described above).
 *
 * LSB of 4-bit field is TCP "PUSH" bit (a worthless anachronism) and
 * is logically part of the 4-bit "changes" field that follows.  Top
 * three bits are actual packet type.  For backward compatibility
 * and in the interest of conserving bits, numbers are chosen so the
 * IP protocol version number (4) which normally appears in this nibble
 * means "IP packet".
 */

/* packet types */
#define TYPE_IP 0x40
#define TYPE_UNCOMPRESSED_TCP 0x70
#define TYPE_COMPRESSED_TCP 0x80
#define TYPE_ERROR 0x00

/* Bits in first octet of compressed packet */
#define NEW_C	0x40	/* flag bits for what changed in a packet */
#define NEW_I	0x20
#define NEW_S	0x08
#define NEW_A	0x04
#define NEW_W	0x02
#define NEW_U	0x01

/* reserved, special-case values of above */
#define SPECIAL_I (NEW_S|NEW_W|NEW_U)		/* echoed interactive traffic */
#define SPECIAL_D (NEW_S|NEW_A|NEW_W|NEW_U)	/* unidirectional data */
#define SPECIALS_MASK (NEW_S|NEW_A|NEW_W|NEW_U)

#define TCP_PUSH_BIT 0x10


/*
 * "state" data for each active tcp conversation on the wire.  This is
 * basically a copy of the entire IP/TCP header from the last packet
 * we saw from the conversation together with a small identifier
 * the transmit & receive ends of the line use to locate saved header.
 */
struct cstate {
	struct cstate *cs_next;	/* next most recently used cstate (xmit only) */
	u_short cs_hlen;	/* size of hdr (receive only) */
	u_char cs_id;		/* connection # associated with this state */
	u_char cs_filler;
	union {
		char csu_hdr[MAX_HDR];
		struct ip csu_ip;	/* ip/tcp hdr from most recent packet */
	} slcs_u;
};
#define cs_ip slcs_u.csu_ip
#define cs_hdr slcs_u.csu_hdr

/*
 * all the state data for one serial line (we need one of these
 * per line).
 */
struct slcompress {
	struct cstate *last_cs;	/* most recently used tstate */
	u_char last_recv;	/* last rcvd conn. id */
	u_char last_xmit;	/* last sent conn. id */
	u_short flags;
#ifndef SL_NO_STATS
	int sls_packets;	/* outbound packets */
	int sls_compressed;	/* outbound compressed packets */
	int sls_searches;	/* searches for connection state */
	int sls_misses;		/* times couldn't find conn. state */
	int sls_uncompressedin;	/* inbound uncompressed packets */
	int sls_compressedin;	/* inbound compressed packets */
	int sls_errorin;	/* inbound unknown type packets */
	int sls_tossed;		/* inbound packets tossed because of error */
#endif
	struct cstate tstate[MAX_STATES];	/* xmit connection states */
	struct cstate rstate[MAX_STATES];	/* receive connection states */
};
/* flag values */
#define SLF_TOSS 1		/* tossing rcvd frames because of input err */
