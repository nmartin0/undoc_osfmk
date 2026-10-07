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
 * SSD HISTORY
 * $Log: tr.c,v $
 * Revision 1.7  1994/11/18  20:56:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/09/28  18:04:43  andyp
 * Update for the 1.2 release.
 *
 *
 *	Add a primitive pattern matching facility for
 *	scanning the tr buffer.  The pattern must be placed
 *	in a static buffer, parse_tr_buffer, which is a bit
 *	awkward.  We should add a ddb interface option that
 *	understands string constants.  The compiled-in
 *	pattern constant enables the user to scan the tr
 *	buffer for all entries containing the word "KMSG".
 *	[alanl@osf.org]
 *
 * Revision 1.5  1993/07/22  02:21:10  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:51:59  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:46:19  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:19:55  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:37:07  SSD
 * pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: tr.c,v
 * Revision 1.2.4.2  1993/02/02  13:40:11  dwm
 * 	Skip over garbaged/zeroed tr buffer entries.
 * 	Common case:  system startup, buffer is not yet full. (alanl)
 * 	[1993/02/01  22:36:24  dwm]
 * 
 * Revision 1.2  1992/11/25  01:15:29  robert
 * 	fix history
 * 	[1992/11/09  22:01:26  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:47:59  robert]
 * 
 * Revision 0.0  92/10/15            alan
 * 		More clean up for nicer output.
 * 	[92/10/15            alan]
 * 
 * Revision 0.0  92/10/07            sjs
 * 		Clean up to make ddb friendly; changed decimal output to hex.
 * 
 * 	Revision 1.1  1992/11/05  21:00:08  robert
 * 	Initial revision
 * 	[92/10/07            sjs]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.1.2.1  92/09/15  17:35:07  jeffreyh
 * 	Created.
 * 	[92/09/15  15:31:28  jeffreyh]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
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
 */
/*
 *	File:		norma/tr.c
 *	Authors:	Alan Langerman, Jeffrey Heller
 *	Date:		1992
 *
 *	Internal trace routines.  Like old-style XPRs but
 *	less formatting.
 */

#include <norma/tr.h>


#if	TRACE_BUFFER
/*
 *	Primitive event tracing facility for kernel debugging.  Yes,
 *	this has some resemblance to XPRs.  However, it is primarily
 *	intended for post-mortem analysis through ddb.  Not MP-safe.
 */

#define	TRACE_MAX	(4 * 1024)
#define	TRACE_WINDOW	40

typedef struct trace_event {
	char		*funcname;
	char		*file;
	unsigned int	lineno;
	char		*fmt;
	unsigned int	tag1;
	unsigned int	tag2;
	unsigned int	tag3;
	unsigned int	tag4;
} trace_event;

trace_event	trace_buffer[TRACE_MAX];
unsigned long	trace_index;
unsigned long	trace_wraps;


tr(funcname, file, lineno, fmt, tag1, tag2, tag3, tag4)
char		*funcname;
char		*file;
unsigned int	lineno;
char		*fmt;
unsigned int	tag1, tag2, tag3, tag4;
{
	if (trace_index >= TRACE_MAX) {
		trace_index = 0;
		++trace_wraps;
	}
	trace_buffer[trace_index].funcname = funcname;
	trace_buffer[trace_index].file = file;
	trace_buffer[trace_index].lineno = lineno;
	trace_buffer[trace_index].fmt = fmt;
	trace_buffer[trace_index].tag1 = tag1;
	trace_buffer[trace_index].tag2 = tag2;
	trace_buffer[trace_index].tag3 = tag3;
	trace_buffer[trace_index].tag4 = tag4;
	++trace_index;
}


show_tr(index,range,show_extra)
unsigned long	index;
unsigned long	range;
unsigned long	show_extra;
{
	int		i;
	char		*filename, *cp;

	if (index == 0) {
		index = trace_index - (TRACE_WINDOW-4);
		range = TRACE_WINDOW;
		show_extra = 0;
	}
	if (index + range > TRACE_MAX)
		range = TRACE_MAX - index;
	for (i = index; i < index + range; ++i) {
		if (trace_buffer[i].file == (char *) 0 ||
		    trace_buffer[i].funcname == (char *) 0 ||
		    trace_buffer[i].lineno == 0 ||
		    trace_buffer[i].fmt == 0) {
			db_printf("[%04x%s]\n", i,
				  i >= trace_index ? "*" : "");
			continue;
		}
		for (cp = trace_buffer[i].file; *cp; ++cp)
			if (*cp == '/')
				filename = cp + 1;
		db_printf("[%04x%s] %s", i, i >= trace_index ? "*" : "",
		       trace_buffer[i].funcname);
		if (show_extra)
			db_printf("(%s:%05d):\n\t", filename,
				  trace_buffer[i].lineno);
		else
			db_printf(":  ");
		db_printf(trace_buffer[i].fmt, trace_buffer[i].tag1,
			  trace_buffer[i].tag2, trace_buffer[i].tag3,
			  trace_buffer[i].tag4);
		db_printf("\n");
	}
}


int
matches(pattern, target)
char	*pattern;
char	*target;
{
	char	*cp, *cp1, *cp2;

	for (cp = target; *cp; ++cp) {
		for (cp2 = pattern, cp1 = cp; *cp2 && *cp1; ++cp2, ++cp1)
			if (*cp2 != *cp1)
				break;
		if (!*cp2)
			return 1;
	}
	return 0;
}


char	parse_tr_buffer[100] = "KMSG";

parse_tr(index, range)
unsigned long	index;
unsigned long	range;
{
	int		i;
	char		*filename, *cp;
	char		*string = parse_tr_buffer;

	if (index == 0) {
		index = trace_index - (TRACE_WINDOW-4);
		range = TRACE_WINDOW;
	}
	if (index + range > TRACE_MAX)
		range = TRACE_MAX - index;
	for (i = index; i < index + range; ++i) {
		if (trace_buffer[i].file == (char *) 0 ||
		    trace_buffer[i].funcname == (char *) 0 ||
		    trace_buffer[i].lineno == 0 ||
		    trace_buffer[i].fmt == 0) {
			db_printf("[%04x%s]\n", i,
				  i >= trace_index ? "*" : "");
			continue;
		}
		if (!matches(string, trace_buffer[i].fmt))
			continue;
		for (cp = trace_buffer[i].file; *cp; ++cp)
			if (*cp == '/')
				filename = cp + 1;
		db_printf("[%04x%s] %s", i, i >= trace_index ? "*" : "",
		       trace_buffer[i].funcname);
		db_printf(":  ");
		db_printf(trace_buffer[i].fmt, trace_buffer[i].tag1,
			  trace_buffer[i].tag2, trace_buffer[i].tag3,
			  trace_buffer[i].tag4);
		db_printf("\n");
	}
}

#endif	/* TRACE_BUFFER */
