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
 
/*++ pack6name.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/pack6name.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	This module contains a function which breaks up to 3 integer
 *	values into 6-bit parcels, producing a strange, but printable
 *	name.
 *
 *	This function is used by many other NQS functions and is called
 *	to generate the names of the various NQS database files, and
 *	transaction descriptors.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	November 13, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:30 $ $State: Exp $)
 * $Log: pack6name.c,v $
 * Revision 1.3  1994/11/19  02:27:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:04  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:30  root
 * Version 2.0 Source
 * 
 * Revision 2.2  86/07/21  09:31:01  hender
 * change those transaction descriptor files who started with a "," to
 * start with a "+" so the file sculkor will not remove them
 * changes supplied by Brent Kingsbury, SSC 7/21/86
 * 
 * Revision 2.1  86/07/09  15:51:15  hender
 * July 3 delivery
 * 
 *
 */

#define	SUBDIRNAME_LENGTH	6	/* Define the number of characters */
					/* in a subdirectory name (enough */
					/* for 32-bits) */

/*** pack6name
 *
 *
 *	void pack6name():
 *
 *	Generate a name based on 6-bit parcels extracted from up
 *	to 3 integer values, placing the result in the specified
 *	name string.
 *
 *	WARNING/NOTE:
 *		It is the responsibility of the caller to ensure
 *		that sufficient space exists in the name array.
 *
 */
void pack6name (name, dir, subdirno, prefix, long1, parcels1,
		long2, parcels2, salt, saltparcels)
register char *name;		/* Destination array */
char *dir;			/* Directory name (can be null). */
int subdirno;			/* Sub-directory number (-1 = no subdir). */
char *prefix;			/* Name prefix (can be null). */
long long1;			/* First long integer */
int parcels1;			/* Number of 6-bit parcels to use from */
				/* long1. */
long long2;			/* Second long integer */
int parcels2;			/* Number of 6-bit parcels to use from */
				/* long2. */
int salt;			/* An additional "salt" value */
int saltparcels;		/* Number of 6-bit parcels to use from */
				/* salt. */
{
	static char c64 [64] = {'+', '0', '1', '2', '3', '4', '5', '6',
				'7', '8', '9', 'A', 'B', 'C', 'D', 'E',
				'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
				'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
				'V', 'W', 'X', 'Y', 'Z', '_', 'a', 'b',
				'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
				'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
				's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

	register short parcelcount;
	register long longint;

	if (dir != (char *) 0) {
		/*
		 *  A directory specification was given.
		 */
		while (*dir) *name++ = *dir++;
		*name++ = '/';			/* Directory separator */
	}
	longint = subdirno;
	if (longint >= 0) {
		/*
		 *  A subdirectory name must be formatted.
		 */
		parcelcount=SUBDIRNAME_LENGTH;	/* Enough for 32-bits */
		name += parcelcount;
		do {
			*--name = c64 [longint & 077];
			longint >>= 6;
		} while (--parcelcount);
		name += SUBDIRNAME_LENGTH;
		if (parcels1 > 0 || parcels2 > 0 || saltparcels > 0 ||
		    prefix != (char *) 0) {
			/*
			 *  Don't add the slash if we are just generating
			 *  the name of an NQS subdirectory.  Nqs_failed.c
			 *  and nqs_rbuild.c depend on this....
			 */
			*name++ = '/';		/* Directory separator */
		}
	}
	if (prefix != (char *) 0) {
		/*
		 *  A name prefix was given.
		 */
		while (*prefix) *name++ = *prefix++;
	}
	parcelcount = parcels1;
	longint = long1;
	if (parcelcount > 0) {
		name += parcelcount;
		do {
			*--name = c64 [longint & 077];
			longint >>= 6;
		} while (--parcelcount);
		name += parcels1;
	}
	parcelcount = parcels2;
	longint = long2;
	if (parcelcount > 0) {
		name += parcelcount;
		do {
			*--name = c64 [longint & 077];
			longint >>= 6;
		} while (--parcelcount);
		name += parcels2;
	}
	parcelcount = saltparcels;
	longint = (unsigned) salt;
	if (parcelcount > 0) {
		name += parcelcount;
		do {
			*--name = c64 [longint & 077];
			longint >>= 6;
		} while (--parcelcount);
		name += saltparcels;
	}
	*name = '\0';		/* Terminate with a null string */
}

#if OSF
#include "core_dump.c"
#endif

