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
 
/*++ interwrite.c.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/interwrite.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the procedures used to construct, and
 *	format (a possibly INTER-MACHINE) NQS message packet.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	September 26, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:00 $ $State: Exp $)
 * $Log: interwrite.c,v $
 * Revision 1.3  1994/11/19  02:27:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:48  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:53  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:50:31  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"

extern void bytecopy();		/* Copy bytes */
extern int strlen();		/* Length of string */

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
static unsigned next_free = 0;		/* Index of next free character */
					/* position in the strspace */
					/* buffer */
static short limit_exceeded = 0;	/* No limit exceeded */
static unsigned long sign[(MAX_PKTINTEGERS+PRECISION_ULONG-1)/PRECISION_ULONG];
					/* Integer sign bits */
static unsigned long u32s[MAX_PKTINTEGERS];
					/* Packet integer data */
static short strlength [MAX_PKTSTRINGS];/* Length of respective string data */
static char strspace [MAX_PACKET-1];	/* String data storage */


/*** interclear
 *
 *
 *	void interclear():
 *
 *	Initialize packet space to allow the construction of a new
 *	inter-process communication message.
 */
void interclear()
{
	n_i32s = 0;
	n_strs = 0;
	next_free = 0;
	limit_exceeded = 0;		/* No limit exceeded */
}


/*** interw32i
 *
 *
 *	void interw32i():
 *
 *	Add a 32-bit signed integer quantity to the current inter-process
 *	communication message under construction.
 */
void interw32i (i32)
register long i32;			/* Signed 32-bit integer */
{
	register unsigned long mask;

	if (n_i32s >= MAX_PKTINTEGERS) limit_exceeded = 1;
	else {				/* Save integer */
		mask = (1 << (n_i32s % PRECISION_ULONG));
		if (i32 >= 0) {
			sign [n_i32s / PRECISION_ULONG] &= ~mask;
			u32s [n_i32s++] = i32;
		}
		else {
			sign [n_i32s / PRECISION_ULONG] |= mask;
			if (i32 < -2147483647) {
				u32s [n_i32s++] = 2147483648;
			}
			else u32s [n_i32s++] = -i32;
		}
	}
}


/*** interw32u
 *
 *
 *	void interw32u():
 *
 *	Add a 32-bit unsigned integer quantity to the current inter-process
 *	communication message under construction.
 */
void interw32u (u32)
register unsigned long u32;		/* Unsigned 32-bit integer */
{
	register unsigned long mask;

	if (n_i32s >= MAX_PKTINTEGERS) limit_exceeded = 1;
	else {				/* Save unsigned integer */
		mask = (1 << (n_i32s % PRECISION_ULONG));
		sign [n_i32s / PRECISION_ULONG] &= ~mask;
		u32s [n_i32s++] = u32;
	}
}


/*** interwstr
 *
 *
 *	void interwstr():
 *
 *	Add a null-terminated character string to the current inter-
 *	process communication message under construction.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if the maximum number of strings allowed in
 *		   an NQS inter-process communication packet
 *		   has been exceeded, or if the maximum string
 *		   space allowed in a packet has been exceeded.
 */
void interwstr (string)
char *string;				/* String to be written (not */
					/* necessarily null terminated) */
{
	register unsigned stringsize;

	stringsize = strlen (string);
	if (n_strs >= MAX_PKTSTRINGS ||
	    next_free + stringsize > MAX_PACKET-1) {
		limit_exceeded = 1;
	}
	else {
		/*
		 *  Save the string away.
		 */
		strlength [n_strs++] = stringsize;
		while (stringsize--) {
			strspace [next_free++] = *string++;
		}
	}
}


/*** interwbytes
 *
 *
 *	void interwbytes():
 *
 *	Add a byte string (not-necessarily null terminated) to the
 *	current inter-process communication message under construction.
 */
void interwbytes (bytes, n_bytes)
char *bytes;				/* Bytes to be written */
unsigned n_bytes;			/* Number of bytes to write */
{
	if (n_strs == MAX_PKTSTRINGS ||
	    next_free + n_bytes > MAX_PACKET-1) {
		limit_exceeded = 1;
	}
	else {
		/*
		 *  Save the string away.
		 */
		strlength [n_strs++] = n_bytes;
		while (n_bytes--) {
			strspace [next_free++] = *bytes++;
		}
	}
}


/*** interfmt
 *
 *
 *	int interfmt():
 *
 *	Format the message packet into the specified buffer which
 *	MUST be at least MAX_PACKET bytes in size.
 *
 *	Returns:
 *		>0: if successful, as size of packet.
 *		-1: if the formatted message packet was too
 *		    large, or some other limit was exceeded.
 */
int interfmt (packet)
char packet [MAX_PACKET];		/* Packet buffer */
{
	register unsigned write_i;
	register unsigned i;
	register unsigned j;
	register unsigned k;
	register unsigned long mask;	/* Sign array mask */

	if (limit_exceeded) return (-1);
	/*
	 *  Construct the packet, handling quirkish special cases first.
	 */
	if (n_i32s == 0) {
		/*
		 *  This packet will not contain any integer data.
		 */
		if (n_strs == 0) {
			/*
			 *  The packet will not have any data at all!
			 */
			packet [0] = 31;		/* 1-byte packet */
			return (1);
		}
		if (n_strs == 1) {
			i = strlength [0];
			if (i < 31) {
				if (MAX_PACKET-1 < i) {
					/*
					 *  The string just will not fit.
					 */
					return (-1);
				}
				packet [0] = i;
				bytecopy (packet+1, strspace, i);
				return (i+1);
			}
		}
	}
	/*
	 *  Compute size of packet contents in bytes.
	 */
	write_i = 0;
	i = n_strs;
	if (i) {				/* String data to be sent */
		write_i += 1;			/* +1 for #-of-strings-byte */
		while (write_i < MAX_PACKET && i > 0) {
			write_i += u32size ((unsigned long) strlength [--i]);
			write_i += strlength [i];
		}
		if (i || write_i > MAX_PACKET) {	/* Packet too large */
			return (-1);
		}
	}
	i = n_i32s;
	if (i) {				/* Integer data to be sent */
		write_i += 1;			/* +1 for #-of-integers-byte */
		while (write_i < MAX_PACKET && i > 0) {
			write_i += u32size (u32s [--i]);
		}
		if (i || write_i > MAX_PACKET) {	/* Packet too large */
			return (-1);
		}
	}
	if (write_i > 30) {
		/*
		 *  Packet will have a 2-byte header.
		 */
		if (write_i > MAX_PACKET-2) return (-1);	/* Too big */
		packet [0] = (write_i / 256) | 0040;
		packet [1] = write_i % 256;
		write_i = 2;
	}
	else {
		/*
		 *  Packet will have a 1-byte header.
		 */
		if (write_i > MAX_PACKET-1) return (-1);	/* Too big */
		packet [0] = write_i;
		write_i = 1;
	}
	i = n_strs;
	if (i) {				/* Character data to be sent */
		j = 0;
		next_free = 0;
		packet [write_i++] = i;		/* #-of-strings */
		while (i--) {
			k = strlength [j++];
			write_i = i32vfmt ((unsigned long) k, 0, packet,
					   write_i);
			bytecopy (packet + write_i, strspace + next_free, k);
			write_i += k;
			next_free += k;
		}
		packet [0] |= 0100;		/* Packet contains string */
	}					/* data */
	i = n_i32s;
	if (i) {				/* Integer data to be sent */
		j = 0;				/* u32s [] subscript */
		k = 0;				/* sign [] subscript */
		mask = sign [0];		/* First N sign bits */
		packet [write_i++] = i;		/* #-of-integers */
		while (i--) {
			if (mask & 1) {
				write_i = i32vfmt (u32s [j++], 1, packet,
						   write_i);
			}
			else {
				write_i = i32vfmt (u32s [j++], 0, packet,
						   write_i);
			}
			mask >>= 1;
			if ((j % PRECISION_ULONG) == 0) {
				/*
				 *  Step to the next sign [] aray element.
				 *  Get the next N sign bits.
				 */
				mask = sign [++k];
			}
		}
		packet [0] |= 0200;		/* Packet contains integer */
	}					/* data */
	return (write_i);			/* Return size of packet */
}


/*** u32size
 *
 *
 *	int u32size():
 *
 *	Return size (in bytes) of varying network format 32-bit
 *	precision unsigned-integer (+ room for sign bit).
 */
static int u32size (u32)
register unsigned long u32;
{
	if (u32 > 268435455) return (5);	/* 2^28 - 1 */
	if (u32 > 2097151) return (4);		/* 2^21 - 1 */
	if (u32 > 8191) return (3);		/* 2^13 - 1 */
	if (u32 > 31) return (2);		/* 2^5  - 1 */
	return (1);
}


/*** i32vfmt
 *
 *
 *	int i32vfmt():
 *
 *	Format 32-bit precision signed-integer into varying
 *	network format integer.
 *
 *	Returns:
 *		The index of the next available byte
 *		in the packet.
 */
static int i32vfmt (u32, signbit, packet, index)
register unsigned long u32;	/* Unsigned integer to format */
int signbit;			/* Sign bit for integer being formatted */
char packet [MAX_PACKET];	/* Packet under construction */
register unsigned index;	/* Index of next byte in packet */
{
	register short signbit1;/* Sign bit #1 */
	register short signbit2;/* Sign bit #2 */

	signbit1 = 0;		/* Assume positive */
	signbit2 = 0;
	if (signbit) {
		signbit1 = 040;	/* Number is negative */
		signbit2 = 020;
	}
	if (u32 > 268435455) {			/* 2^28 - 1 */
		packet [index++] = 0340 | signbit2;
		packet [index++] = u32 % 256;	/* LSB */
		u32 /= 256;
		packet [index++] = u32 % 256;
		u32 /= 256;
		packet [index++] = u32 % 256;
		packet [index++] = u32 / 256;	/* MSB */
		return (index);
	}
	if (u32 > 2097151) {			/* 2^21 - 1 */
		packet [index++] = 0300 | signbit2 | (u32 % 16);
		u32 /= 16;
		packet [index++] = u32 % 256;
		u32 /= 256;
		packet [index++] = u32 % 256;
		packet [index++] = u32 / 256;	/* MSB */
		return (index);
	}
	if (u32 > 8191) {			/* 2^13 - 1 */
		packet [index++] = 0200 | signbit1 | (u32 % 32);
		u32 /= 32;
		packet [index++] = u32 % 256;
		packet [index++] = u32 / 256;	/* MSB */
		return (index);
	}
	if (u32 > 31) {				/* 2^5  - 1 */
		packet [index++] = 0100 | signbit1 | (u32 % 32);
		packet [index++] = u32 / 32;	/* MSB */
		return (index);
	}
	packet [index++] = signbit1 | u32;
	return (index);
}
