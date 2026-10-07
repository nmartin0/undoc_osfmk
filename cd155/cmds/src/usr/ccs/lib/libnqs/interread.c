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
 
/*++ interread.c.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/interread.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the procedures used to unpack
 *	(a possibly INTER-MACHINE) NQS message packet.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	September 30, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:57 $ $State: Exp $)
 * $Log: interread.c,v $
 * Revision 1.3  1994/11/19  02:26:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:46  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:51  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:50:22  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"

extern void bytecopy();			/* Copy exactly N bytes */

/*
 *
 *	Packet structure:
 *	-----------------
 *
 *		.-------------------------------------------------------.
 *		| 2-bit packet type and characteristics.		|
 *		|							|
 *		|	0: Packet contains exactly one character	|
 *		|	   string of length LESS than 31, or the	|
 *		|	   packet does not contain any data at		|
 *		|	   all.  If the packet contains only a		|
 *		|	   single character string, then no		|
 *		|	   other data is present in the packet.		|
 *		|							|
 *		|	1: Packet contains only character string	|
 *		|	   data.  No integer data is present.		|
 *		|							|
 *		|	2: Packet contains only integer data.  No	|
 *		|	   character string data is present.		|
 *		|							|
 *		|	3: Packet contains both character and integer	|
 *		|	   string data.					|
 *		|							|
 *		|-------------------------------------------------------|
 *		| 1-bit packet-contents size bit.			|
 *		|							|
 *		|	0: Contents size (in bytes) is LESS than 31.	|
 *		|	1: Contents size (in bytes) is GREATER than 30.	|
 *		|							|
 *		|-------------------------------------------------------|
 *		| Packet-contents size:					|
 *		|							|
 *		|	If the size of the contents (in bytes) is LESS	|
 *		|	than 31, then:					|
 *		|	  .-----------------------------------------.	|
 *		|	  |  5-bit contents-size in bytes [0..30].  |	|
 *		|	  |  (The value of 31 in this field	    |	|
 *		|	  |  indicates that no data is present in   |	|
 *		|	  |  the packet, at all).		    |	|
 *		|	  `-----------------------------------------'	|
 *		|							|
 *		|	If the size of the contents (in bytes) is	|
 *		|	GREATER than 30, then:				|
 *		|	  .------------------------------------------.	|
 *		|	  | 13-bit contents-size in bytes [31..8191].|	|
 *		|	  `------------------------------------------'	|
 *		|							|
 *		|-------------------------------------------------------|
 *		| Packet contents:					|
 *		|							|
 *		|	If packet type=0, then:				|
 *		|	  .-------------------------------------.	|
 *		|	  | If the contents-size field contains |	|
 *		|	  | a value of 31, then no data is	|	|
 *		|	  | present in the packet.  Otherwise,	|	|
 *		|	  | The contents-size field yields	|	|
 *		|	  | the length of the single character	|	|
 *		|	  | string datum contained in the	|	|
 *		|	  | packet.				|	|
 *		|	  `-------------------------------------'	|
 *		|							|
 *		|	If packet type=1, then:				|
 *		|	  .-------------------------------------.	|
 *		|	  | 1-byte number of character strings	|	|
 *		|	  |	   [1..255] in packet.		|	|
 *		|	  |-------------------------------------|	|
 *		|	  | Character string descr #1:		|	|
 *		|	  |					|	|
 *		|	  |   .-------------------------------.	|	|
 *		|	  |   | N-byte varying network format |	|	|
 *		|	  |   |	       (signed) length of     | |	|
 *		|	  |   |	       character string #1.   | |	|
 *		|	  |   |-------------------------------|	|	|
 *		|	  |   | N-byte character string #1.   | |	|
 *		|	  |   `-------------------------------' |	|
 *		|	  |					|	|
 *		|	  |-------------------------------------|	|
 *		|	  | Character string descr #2:		|	|
 *		|	  |					|	|
 *		|	  |   .-------------------------------. |	|
 *		|	  |   | N-byte varying network format |	|	|
 *		|	  |   |	       (signed) length of     | |	|
 *		|	  |   |	       character string #2.   | |	|
 *		|	  |   |-------------------------------|	|	|
 *		|	  |   | N-byte character string #2.   | |	|
 *		|	  |   `-------------------------------' |	|
 *		|	  |					|	|
 *		|	  |-------------------------------------|	|
 *		|	  | Character string descr #3:		|	|
 *		|	  |					|	|
 *		|	  |   .-------------------------------. |	|
 *		|	  |   | N-byte varying network format |	|	|
 *		|	  |   |	       (signed) length of     | |	|
 *		|	  |   |	       character string #3.   | |	|
 *		|	  |   |-------------------------------|	|	|
 *		|	  |   | N-byte character string #3.   | |	|
 *		|	  |   `-------------------------------' |	|
 *		|	  |					|	|
 *		|	  |-------------------------------------|	|
 *		|	  |					|	|
 *		|			     :				|
 *		|			     :				|
 *		|	  |					|	|
 *		|	  |-------------------------------------|	|
 *		|	  | Character string descr #n-1:	|	|
 *		|	  |					|	|
 *		|	  |   .-------------------------------.	|	|
 *		|	  |   | N-byte varying network format |	|	|
 *		|	  |   |	       (signed) length of     | |	|
 *		|	  |   |	       character string #n-1. | |	|
 *		|	  |   |-------------------------------|	|	|
 *		|	  |   | N-byte character string #n-1. | |	|
 *		|	  |   `-------------------------------'	|	|
 *		|	  |					|	|
 *		|	  |-------------------------------------|	|
 *		|	  | Character string descr #n:		|	|
 *		|	  |					|	|
 *		|	  |   .-------------------------------. |	|
 *		|	  |   | N-byte varying network format |	|	|
 *		|	  |   |	       (signed) length of     | |	|
 *		|	  |   |	       character string #n.   | |	|
 *		|	  |   |-------------------------------|	|	|
 *		|	  |   | N-byte character string #n.   | |	|
 *		|	  |   `-------------------------------' |	|
 *		|	  |					|	|
 *		|	  `-------------------------------------'	|
 *		|							|
 *		|	If packet type=2, then:				|
 *		|	  .-------------------------------------.	|
 *		|	  | 1-byte number of varying network	|	|
 *		|	  |	   format integers [1..255]	|	|
 *		|	  |	   in packet.			|	|
 *		|	  |-------------------------------------|	|
 *		|	  | N-byte varying network format	|	|
 *		|	  |        integer #1.			|	|
 *		|	  |-------------------------------------|	|
 *		|	  | N-byte varying network format	|	|
 *		|	  |        integer #2.			|	|
 *		|	  |-------------------------------------|	|
 *		|	  | N-byte varying network format	|	|
 *		|	  |        integer #3.			|	|
 *		|	  |-------------------------------------|	|
 *		|	  |					|	|
 *		|			     :				|
 *		|			     :				|
 *		|	  |					|	|
 *		|	  |-------------------------------------|	|
 *		|	  | N-byte varying network format	|	|
 *		|	  |        integer #n-1.		|	|
 *		|	  |-------------------------------------|	|
 *		|	  | N-byte varying network format	|	|
 *		|	  |        integer #n.			|	|
 *		|	  `-------------------------------------'	|
 *		|							|
 *		|	If packet type=3, then:				|
 *		|	  .-------------------------------------.	|
 *		|	  |  Data exactly as described for	|	|
 *		|	  |  packet of type 1.			|	|
 *		|	  |-------------------------------------|	|
 *		|	  |  Data exactly as described for	|	|
 *		|	  |  packet of type 2.			|	|
 *		|	  `-------------------------------------'	|
 *		|							|
 *		`-------------------------------------------------------'
 *
 */

static short n_i32s = 0;		/* Number of integers */
static short n_strs = 0;		/* Number of strings */
static unsigned long sign[(MAX_PKTINTEGERS+PRECISION_ULONG-1)/PRECISION_ULONG];
					/* Integer sign bits */
static unsigned long u32s[MAX_PKTINTEGERS];
					/* Packet integer data */
static short strlength [MAX_PKTSTRINGS];/* Length of respective string data */
static char strspace [MAX_PACKET-1+MAX_PKTSTRINGS];
					/* String data storage */
					/* + MAX_PKTSTRINGS for trailing */
					/* null characters */


/*** intern32i
 *
 *
 *	int intern32i():
 *
 *	Return the number of 32-bit precision signed-integer quantities
 *	that were contained in the message packet.
 */
int intern32i()
{
	return (n_i32s);
}


/*** internstr
 *
 *
 *	int internstr():
 *
 *	Return the number of character/byte strings that were contained
 *	in the message packet.
 */
int internstr()
{
	return (n_strs);
}


/*** interr32i
 *
 *
 *	long interr32i():
 *
 *	Return the signed value of the i-th 32-bit precision integer
 *	contained in the message packet.  (The packet integers are
 *	numbered [1..n].)
 */
long interr32i (i32_i)
register int i32_i;
{
	register unsigned long mask;
	register unsigned long u32;

	if (i32_i < 1 || i32_i > n_i32s) return (0);
	i32_i -= 1;
	mask = (1 << (i32_i % PRECISION_ULONG));
	u32 = u32s [i32_i];
	if (sign [i32_i / PRECISION_ULONG] & mask) {
		return ( -((long) u32) );
	}
	if (u32 > 2147483647) u32 = 0;
	return ((long) u32);
}


/*** interr32sign
 *
 *
 *	int interr32sign():
 *
 *	Return non-zero, if the i-th 32-bit precision integer contained
 *	in the message packet is negative.  Otherwise return zero.  (The
 *	packet integers are number [1..n].)
 */
int interr32sign (i32_i)
register int i32_i;
{
	register unsigned long mask;

	if (i32_i < 1 || i32_i > n_i32s) return (0);
	i32_i -= 1;
	mask = (1 << (i32_i % PRECISION_ULONG));
	if (sign [i32_i / PRECISION_ULONG] & mask) return (1);
	return (0);
}


/*** interr32u
 *
 *
 *	unsigned long interr32u():
 *
 *	Return the unsigned value of the i-th 32-bit precision integer
 *	contained in the message packet.  (The packet integers are
 *	numbered [1..n].)
 */
unsigned long interr32u (u32_i)
register int u32_i;
{
	if (u32_i < 1 || u32_i > n_i32s) return (0);
	return (u32s [u32_i-1]);
}


/*** interlstr
 *
 *
 *	int interlstr():
 *
 *	Return the length of the i-th character/byte string in the
 *	message packet.  (The strings are numbered [1..n].)
 */
int interlstr (string_i)
register int string_i;
{
	if (string_i < 1 || string_i > n_strs) return (0);
	return (strlength [string_i-1]);
}


/*** interrstr
 *
 *
 *	char *interrstr():
 *
 *	Return a pointer to the i-th character/byte string in the
 *	message packet.  (The strings are numbered [1..n].)
 */
char *interrstr (string_i)
register int string_i;
{
	register unsigned index;

	if (string_i < 1 || string_i > n_strs) return ((char *) 0);
	index = 0;
	string_i--;			/* Map to [0..string_i-1] */
	while (string_i > 0) {
		index += strlength [--string_i] + 1;
	}
	return (strspace + index);
}


/*** interread
 *
 *
 *	int interread():
 *
 *	Read and interpret another NQS inter-process (possibly INTER-
 *	MACHINE) message packet.
 *
 *	Returns:
 *		 0: if another message packet was successfully
 *		    received.
 *		-1: if the source of message packets has
 *		    returned EOF, in which case there are no
 *		    more message packets to be read.
 *		-2: if a bad message packet was received.
 *		    Under such circumstances, it is NOT
 *		    possible to recover message packet
 *		    synchronization on the message packet
 *		    source in a reliable fashion.
 */
int interread (readfn)
int (*readfn)();			/* This function must return */
					/* the next byte to be interpreted */
					/* as the next message packet byte.*/
					/* It must not take any parameters.*/
{
	char packet [MAX_PACKET];	/* Message packet buffer */
	register short ch;		/* Character returned from readfn()*/
	register unsigned index;	/* Message packet contents index */
	register unsigned read_count;	/* Number of characters to read */

	ch = (*readfn)();		/* Read first packet character */
	if (ch == -1) {
		/*
		 *  End of file reached on packet source.
		 */
		return (-1);		/* Return EOF. */
	}
	index = 0;			/* Packet write-index = 0 */
	packet [index++] = ch;		/* Store first packet character */
					/* in packet buffer. */
	if (ch == 31) {
		/*
		 *  This packet does not contain any data at all.
		 */
		return (interscan (packet));
	}
	if (ch & 040) {
		/*
		 *  The size of the packet contents (in bytes), is
		 *  in [31..8191].
		 */
		read_count = (ch & 037) * 256;
		if ((ch = (*readfn)()) == -1) {
			/*
			 *  End of file reached on packet source,
			 *  when another packet byte was required.
			 */
			return (-2);	/* Bad packet */
		}
		packet [index++] = ch;	/* Store second packet character */
					/* in packet buffer, and compute */
		read_count += ch;	/* packet contents size. */
	}
	else {
		/*
		 *  The size of the packet contents (in bytes), is
		 *  in [0..30].
		 */
		read_count = ch & 037;
	}
	if (index + read_count > MAX_PACKET) {
		/*
		 *  The packet will be too large.
		 */
		return (-2);		/* Bad packet */
	}
	/*
	 *  Loop to read read_count more packet bytes.
	 */
	while (read_count--) {
		if ((ch = (*readfn)()) == -1) {
			/*
			 *  End of file reached on packet source,
			 *  when another packet byte was required.
			 */
			return (-2);	/* Bad packet */
		}
		packet [index++] = ch;	/* Store the character into the */
					/* packet buffer. */
	}
	/*
	 *  Now, interpret the received message packet.
	 */
	return (interscan (packet));
}


/*** interscan
 *
 *
 *	int interscan():
 *
 *	Scan the message packet in the specified buffer unpacking
 *	the integer and string values contained therein.
 *
 *	Returns:
 *		 0: if successful.
 *		-2: if the formatted message packet was
 *		    invalid, or otherwise corrupted.
 */
static int interscan (packet)
char packet [MAX_PACKET];		/* Packet buffer */
{
	register short ch;		/* Character */
	register unsigned index;	/* Packet scan index */
	register unsigned write_i;	/* Character data write-index */
	register unsigned length;	/* Length of character/byte string */
	register unsigned i;		/* Iteration */
	unsigned count;			/* Counter */
	unsigned packet_size;		/* Size of packet in bytes */
	unsigned long mask;		/* Sign bit mask */
	unsigned long ulen;		/* Unsigned long length integer */
	short signbit;			/* Sign bit of network integer */
	short packet_type;

	n_i32s = 0;
	n_strs = 0;
	index = 0;			/* Scan index */
	ch = (packet [index++] & 0377);	/* Ch = first character of packet */
	if (ch == 31) {
		/*
		 *  Special case, the packet is completely empty.
		 *  No data is contained in the packet.
		 */
		return (0);
	}
	packet_type = (ch & 0300) >> 6;	/* Get packet type */
	if (ch & 040) {
		packet_size = (ch & 037) * 256 + (packet [index++] & 0377) + 2;
	}
	else packet_size = (ch & 037) + 1;
	if (packet_type == 0) {
		/*
		 *  The packet contains a single character string of length
		 *  less than 31.
		 */
		length = packet_size - 1;
		if (length > MAX_PACKET+MAX_PKTSTRINGS-1) {
			/*
			 *  The string will not fit in the available space.
			 */
			return (-2);	/* Bad packet */
		}
		strlength [0] = length;
		n_strs = 1;
		bytecopy (strspace, packet + 1, length);
		strspace [length] = '\0';	/* Null terminate */
		return (0);			/* Success */
	}
	if (packet_type == 1 || packet_type == 3) {
		/*
		 *  The packet contains some character/byte string data.
		 */
		count = (packet [index++] & 0377);	/* #-of-strings */
		if (count > MAX_PKTSTRINGS) {
			/*
			 *  The packet contains too many characters, or
			 *  is otherwise corrupt.
			 */
			return (-2);
		}
		i = 0;
		write_i = 0;
		while (i < count) {
			if ((index = i32vread (&ulen, &signbit, packet, index,
					       packet_size)) == -2 || signbit){
				/*
				 *  The packet is corrupt.
				 */
				return (-2);
			}
			strlength [i++] = ulen;
			length = ulen;
			if (length + index > packet_size ||
			    length + write_i >= MAX_PACKET+MAX_PKTSTRINGS-1) {
				/*
				 *  The packet is corrupt.
				 *
				 *  Note the >= in the above comparison
				 *  because we also need room for a
				 *  trailing null byte.
				 */
				return (-2);
			}
			bytecopy (strspace + write_i, packet + index, length);
			write_i += length;
			index += length;
			strspace [write_i++] = '\0';	/* Null terminate */
			n_strs++;			/* One more string */
		}
	}
	if (packet_type == 2 || packet_type == 3) {
		/*
		 *  The packet contains some integer data.
		 */
		count = (packet [index++] & 0377);	/* #-of-integers */
		if (count > MAX_PKTINTEGERS) {
			/*
			 *  The packet contains too many integers, or
			 *  is otherwise corrupt.
			 */
			return (-2);
		}
		i = 0;
		mask = 1;
		while (i < count) {
			if ((index = i32vread (&u32s [i++], &signbit, packet,
					       index, packet_size)) == -2) {
				/*
				 *  The packet is corrupt.
				 */
				return (-2);
			}
			if (signbit) sign [n_i32s / PRECISION_ULONG] |= mask;
			else sign [n_i32s / PRECISION_ULONG] &= ~mask;
			n_i32s++;			/* One more integer */
			if (n_i32s % PRECISION_ULONG == 0) mask = 1;
			else mask <<= 1;
		}
	}
	/*
	 *  There should not be any more bytes left in the packet at
	 *  this point.
	 */
	if (index != packet_size) {
		/*
		 *  The packet is corrupt.
		 */
		return (-2);
	}
	/*
	 *  The packet was valid, and has been successfully scanned.
	 */
	return (0);
}


/*** i32vread
 *
 *
 *	int i32vread():
 *
 *	Read the varying network format 32-bit precision signed-
 *	integer from the packet at the specified index.
 *
 *	Returns:
 *	       >0: if successful.  The value returned is
 *		   the index of the next available byte
 *		   in the packet.
 *	       -2: if there was an error in the packet.
 */
static int i32vread (u32, signbit, packet, index, packet_size)
unsigned long *u32;		/* Unsigned value of integer */
short *signbit;			/* Boolean sign bit of network integer */
char packet [MAX_PACKET];	/* Packet under construction */
unsigned index;			/* Index of next byte in packet */
unsigned packet_size;		/* Size of packet in bytes */
{
	register unsigned long ru32;	/* 32-bit unsigned result */
	register short ch;		/* Packet character */

	if (index >= packet_size) return (-2);
	ch = packet [index++];		/* Get packet character */
	if ((ch & 0300) == 0300) {
		/*
		 *  Four or five byte integer.
		 */
		*signbit = (ch & 020);
		if (ch & 040) {
			/*
			 *  We have a five byte integer.
			 */
			if (index + 4 > packet_size) return (-2);
			ru32 =  (packet [index++] & 0377);
			ru32 += (packet [index++] & 0377) * 256L;
			ru32 += (packet [index++] & 0377) * 65536L;
			ru32 += (packet [index++] & 0377) * 16777216L;
		}
		else {
			/*
			 *  We have a four byte integer.
			 */
			if (index + 3 > packet_size) return (-2);
			ru32 =  (ch & 017);
			ru32 += (packet [index++] & 0377) * 16;
			ru32 += (packet [index++] & 0377) * 4096L;
			ru32 += (packet [index++] & 0377) * 1048576L;
		}
	}
	else {
		/*
		 *  One, two, or three byte integer.
		 */
		*signbit = (ch & 040);
		if (ch & 0200) {
			/*
			 *  Three byte integer.
			 */
			if (index + 2 > packet_size) return (-2);
			ru32 =  (ch & 037);
			ru32 += (packet [index++] & 0377) * 32;
			ru32 += (packet [index++] & 0377) * 8192L;
		}
		else if (ch & 0100) {
			/*
			 *  Two byte integer.
			 */
			if (index + 1 > packet_size) return (-2);
			ru32 =  (ch & 037);
			ru32 += (packet [index++] & 0377) * 32;
		}
		else {
			/*
			 *  One byte integer.
			 */
			ru32 = (ch & 037);
		}
	}
	*u32 = ru32;		/* Return result */
	return (index);
}
