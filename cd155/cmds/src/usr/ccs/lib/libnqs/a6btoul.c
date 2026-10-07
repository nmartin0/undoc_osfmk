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
 
/*++ a6btoul.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/a6btoul.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	This module contains the functions:
 *
 *		a6btoul(), and
 *		is6bitstr()
 *
 *	which respectively evaluate a 6-bit name returning the long
 *	integer value, and return the boolean truth or falsehood of
 *	the specified name as a 6-bit name.
 *
 *	For more information, see pack6name.c
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	November 19, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:14 $ $State: Exp $)
 * $Log: a6btoul.c,v $
 * Revision 1.3  1994/11/19  02:26:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:08  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:08  root
 * Version 2.0 Source
 * 
 * Revision 2.2  86/07/21  09:29:37  hender
 * change those transaction descriptor files who started with a "," to
 * start with a "+" so the file sculkor will not remove them
 * changes supplied by Brent Kingsbury, SSC 7/21/86
 * 
 * Revision 2.1  86/07/09  15:42:08  hender
 * July 3 delivery
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)a6btoul.c	1.2 (a6btoul.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

/*
 *
 *
 *	7-bit Character to 6-bit value translation table.
 */

static short ctoi64 [128] = {
	127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127,   0, 127, 127, 127, 127,
	  1,   2,   3,   4,   5,   6,   7,   8,
	  9,  10, 127, 127, 127, 127, 127, 127,
	127,  11,  12,  13,  14,  15,  16,  17,
	 18,  19,  20,  21,  22,  23,  24,  25,
	 26,  27,  28,  29,  30,  31,  32,  33,
	 34,  35,  36, 127, 127, 127, 127,  37,
	127,  38,  39,  40,  41,  42,  43,  44,
	 45,  46,  47,  48,  49,  50,  51,  52,
	 53,  54,  55,  56,  57,  58,  59,  60,
	 61,  62,  63, 127, 127, 127, 127, 127
};


/*** a6btoul
 *
 *
 *	unsigned long a6btoul():
 *
 *	Evaluate the specified 6-bit alphabet name returning the
 *	unsigned long value.
 *
 *	WARNING:
 *		No check is made to enforce the requirement that
 *		the name be constructed from the appropriate 6-bit
 *		alphabet!  Use is6bitstr() if this is required.
 *
 */
unsigned long a6btoul (name, len)
register char *name;			/* Name to be evaluated */
register int len;			/* Length of name */
{
	register short ch;
	register unsigned long res;

	res = 0;
	while (len--) {
		ch = *name++;
		ch &= 0x7F;		/* Mask to 7-bits */
		res *= 64;
		res += ctoi64 [ch];
	}
	return (res);
}


/*** is6bitstr
 *
 *
 *	int is6bitstr():
 *
 *	Return non-zero (TRUE), if the specified string is a 6-bit
 *	alphabet name string.  Otherwise return zero (FALSE).
 */
int is6bitstr (name, len)
register char *name;			/* Name to be evaluated */
register int len;			/* Length of name */
{
	register short ch;

	if (len <= 0) return (0);	/* Definitely not a 6-bit */
					/* alphabet name string */
	while (len--) {
		ch = *name++;
		ch &= 0x7F;		/* Mask to 7-bits */
		if (ctoi64 [ch] == 127) {
			/*
			 *  The specified name is not a valid
			 *  6-bit alphabet name.
			 */
			return (0);
		}
	}
	/*
	 *  The specified name is a valid 6-bit alphabet name.
	 */
	return (1);
}
