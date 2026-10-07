/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: loose_ends.c,v $
 * Revision 2.10  1994/11/18  20:39:06  mtm
 * Copyright additions/changes
 *
 * Revision 2.9  1994/07/12  19:18:36  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 2.8.8.1  1994/04/26  20:37:16  andyp
 * Added bcmp().
 *
 * Revision 2.8  1993/06/30  22:31:01  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.7  1993/04/27  20:28:29  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 2.6.2.3  1993/04/27  00:18:02  dleslie
 * Patch release of April 23
 *
 * Revision 2.9  1993/04/16  16:46:19  SSD
 * move node_in_list to ../ipsc/bootenv.c
 *
 * Revision 2.8  1993/04/16  05:21:44  SSD
 * third code drop for page flow control fixes.
 *
 * Revision 2.7  1993/03/11  17:36:59  stans
 * node_in_list() rtn was checking an auto char * instead of input arg.
 * Name confusion.
 *
 * Revision 2.6  1993/02/01  23:53:37  andyp
 * Tree-of-pagers fixes from stans.
 *
 * Revision 2.5  1992/09/23  21:11:59  stans
 * OSF/nmk13 version
 *
 * Revision 2.4.2.2  92/09/15  18:41:57  jeffreyh
 * 	More changes from Stan
 * 	[92/09/15            jeffreyh]
 * 
 * Revision 2.4.2.1  92/09/15  17:17:32  jeffreyh
 * 	more bootenv parsing rtns; export_paging.
 * 	[92/09/10            stans@ssd.intel.com]
 * 
 * Revision 2.4  91/10/09  16:08:01  af
 * 	 Revision 2.3.1.1  91/09/22  14:56:13  rpd
 * 	 	Removed strlen.
 * 	 	[91/09/22            rpd]
 * 
 * Revision 2.3.1.1  91/09/22  14:56:13  rpd
 * 	Removed strlen.
 * 	[91/09/22            rpd]
 * 
 * Revision 2.3  91/06/18  20:51:29  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:56:25  jsb]
 * 
 * Revision 2.2  90/12/04  14:48:47  jsb
 * 	First checkin.
 * 	[90/12/03  21:30:25  jsb]
 * 
 */ 
#include <mach/boolean.h>

/*
 * Random functions: data copying, big <-> little endian, etc.
 * Derived from i386 version.
 */

/* 
 * ovbcopy - like bcopy, but recognizes overlapping ranges and handles 
 *           them correctly.
 */
ovbcopy(from, to, bytes)
	char *from, *to;
	int bytes;			/* num bytes to copy */
{
	/* Assume that bcopy copies left-to-right (low addr first). */
	if (from + bytes <= to || to + bytes <= from || to == from)
		bcopy(from, to, bytes);	/* non-overlapping or no-op*/
	else if (from > to)
		bcopy(from, to, bytes);	/* overlapping but OK */
	else {
		/* to > from: overlapping, and must copy right-to-left. */
		from += bytes - 1;
		to += bytes - 1;
		while (bytes-- > 0)
			*to-- = *from--;
	}
}

memcpy(b, a, c)
{
	bcopy(a, b, c);
}


int bcmp(a, b, len)
	register char	*a, *b;
	register int	len;
{
	if (len == 0)
		return 0;

	do
		if (*a++ != *b++)
			break;
	while (--len);

	return len;
}


ntohl(datain)
int datain;
{
return (  ((datain & 0xFF000000) >> 0x18 ) +
	  ((datain & 0x00FF0000) >> 0x08 ) +
	  ((datain & 0x0000FF00) << 0x08 ) +
	  ((datain & 0x000000FF) << 0x18 )  );
}

#ifdef C_HTONL
/* Defined in ntoh.s */
htonl(datain)
int datain;
{
return (  ((datain & 0xFF000000) >> 0x18 ) +
	  ((datain & 0x00FF0000) >> 0x08 ) +
	  ((datain & 0x0000FF00) << 0x08 ) +
	  ((datain & 0x000000FF) << 0x18 )  );
}
#endif /* C_HTONL */

htons (datain)
short datain;
{
return (  ((datain & 0xFF00) >> 0x08 ) +
	  ((datain & 0x00FF) << 0x08 )   );
}

ntohs (datain)
short datain;
{
return (  ((datain & 0xFF00) >> 0x08 ) +
	  ((datain & 0x00FF) << 0x08 )   );
}

/*
 * Determine wheather this mk is exporting a vnode paging service (bootenv) and
 * return a suffix string to uniquely identify the default paging file. 
 *
 * Parse the following boot magic string "EXPORT_PAGING" looking for my node #
 * in the arg string.
 *
 * inputs:
 *	me	node_self()
 *	bp	boot magic environment string EXPORT_PAGING
 */

char *
do_i_export_paging(int me, char *bp)
{
	static char	mynode[12];

	if ( bp == (char *)0 )
		return bp;

	if ( node_in_list(me,bp) ) {
		/* build the paging file suffix ".node_num" */
		mynode[0] = '.';
		itoa(me, &mynode[1]);
		return mynode;		/* I do export a paging service */
	}
	else
		return( (char *)0 ); /* I do NOT export a paging service */
}
