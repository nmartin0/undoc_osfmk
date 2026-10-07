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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance
 *	with the terms of that agreement.
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_copyname.c,v 1.2 1994/11/19 02:28:37 mtm Exp $
 */

/*
 * copyname utility for FORTRAN interface
 */

/*
	Function to copy a Fortran-format name string to a C-format name
	string.Operation is: (1) copy the characters (2) add a zero  
	terminator and (3) strip off trailing blanks.
*/

_copyname(src, dest, src_count, dest_count)

	char *src, *dest;
	int src_count, dest_count;

{
	int actual_count, i;

	actual_count = src_count;
	if (actual_count > dest_count-1) actual_count = dest_count-1;

	/* copy the string */

	for (i = 0; i < actual_count; i++) dest[i] = src[i];

	/* add zero terminator */

	dest[i] = 0;

	/* if null string, return */

	if (i == 0) return;

	/* now scan from the start to find the terminator */

	for (i = 0; dest[i] != '\0'; i++);

	/* scan off trailing blanks */

	for (i--; i >= 0; i--) if (dest[i] != ' ') return;
	else dest[i] = '\0';

}
