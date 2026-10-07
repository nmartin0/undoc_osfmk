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
 * $Log: rtc.c,v $
 * Revision 1.5  1994/11/18  20:51:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:43:37  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:34:23  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:34:18  dleslie
 * First R1_0 release
 *
 * Revision 2.3.1.2  92/09/15  17:21:13  jeffreyh
 * 	Removed atoi
 * 	[92/09/10            stans@ssd.intel.com]
 * 
 * Revision 2.3.1.1  92/05/27  00:45:34  jeffreyh
 * 	Support bootenv BOOT_TIME", # of seconds from epoch. includes atoi().
 * 	[stans@ssd.intel.com]
 * 
 * Revision 2.3  91/06/18  20:52:52  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  19:05:32  jsb]
 * 
 * Revision 2.2  90/12/04  14:50:50  jsb
 * 	First checkin.
 * 	[90/12/03  21:57:09  jsb]
 * 
 */

#include <sys/types.h>
#include <sys/time.h>

/*
 * Since ipsc does NOT have any hardware which remembers the time-of-day, we
 * get the approx. time from bootcube in the form of a boot magic variable
 * "BOOT_TIME". BOOT_TIME is a decimal integer representing the number of
 * seconds since the epoch (output from time(0) function).
 *
 * inputs:
 *	tp	pointer to time_t value
 *
 * outputs:
 *
 *	none.
 *
 * Side Effects:
 *	*tp is written with the # of seconds since the epoch as derrived from
 *	the bootmagic var "BOOT_TIME".
 *
 */ 
void
readtodc( tp )
	time_t	*tp;
{
	char		*now;
	int		t;
	extern char	* getbootenv();

	/* reason to be here? */
	if ( tp == (time_t *)0 )
		return;

	if ( (now=getbootenv("BOOT_TIME")) != (char *)0 )
		*tp = atoi( now );
}

void
writetodc()
{
}
