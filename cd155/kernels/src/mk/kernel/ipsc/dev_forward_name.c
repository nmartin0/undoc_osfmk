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
 * $Log: dev_forward_name.c,v $
 * Revision 1.5  1994/11/18  20:51:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:43:35  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:34:21  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:34:16  dleslie
 * First R1_0 release
 *
 * Revision 2.7.2.3  92/09/15  18:16:48  jeffreyh
 * 	Delete mose of the device forward table. We now use the server
 * 	for this functionality.
 * 	[92/09/15            jeffreyh]
 * 
 * Revision 2.7.2.2  92/03/28  10:09:38  jeffreyh
 * 	Additions for generic scsi
 * 	[92/03/17            jerrie]
 * 	Additions for OSF_LOCAL case
 * 	[92/03/17            jeffreyh]
 * 
 * Revision 2.7.2.1  92/02/18  19:06:39  jeffreyh
 * 	Changes from Intel for remote console in dev_forward_name
 * 	[92/02/18            jeffreyh]
 * 
 * 	local OSF diferences under OSF_LOCAL
 * 	[92/02/18            sjs]
 * 
 * Revision 2.7  91/12/10  16:32:30  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:33:38  jsb]
 * 
 * Revision 2.6  91/08/03  18:18:42  jsb
 * 	Removed norma conditionals; use conf/file technology instead.
 * 	[91/07/27  18:16:09  jsb]
 * 
 * Revision 2.5  91/06/18  20:52:48  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  19:05:03  jsb]
 * 
 * Revision 2.4  91/06/17  15:46:43  jsb
 * 	Renamed NORMA conditionals. Modified for non-integrated I/O nodes.
 * 	[91/06/17  13:48:13  jsb]
 * 
 * Revision 2.3  91/06/06  17:06:32  jsb
 * 	Add entries to compensate for ux's generation of names like sd0i
 * 	when it should be generating names like sd1a.
 * 	(This is a stopgap, not a real fix.)
 * 	[91/05/13  17:29:24  jsb]
 * 
 * Revision 2.2.1.1  90/12/17  23:09:42  jsb
 * 	Add entries to compensate for ux's generation of names like sd0i
 * 	when it should be generating names like sd1a.
 * 	(This is a stopgap, not a real fix.)
 * 
 * Revision 2.2  90/12/14  11:01:43  jsb
 * 	First checkin.
 * 	[90/12/14  09:37:57  jsb]
 * 
 */

#if	iPSC386
#include <cnp.h>
#include <sd.h>
#include <esp.h>
#include <md.h>
#endif	iPSC386


/*
 * Translate a device name to it's local representation and
 * optionally the node whre the device resides.
 *
 * inputs:
 *	name	pointer to device name (NO "<")
 *	namebuf	pointer to result name buffer
 *	namelen	length of result buffer
 *
 * outputs:
 *	char string pointer to device name for use in device_open() call.
 */

char *
dev_forward_name(name, namebuf, namelen)
	char *name;
	char *namebuf;
	int namelen;
{
        register char			*cp=name;

	extern char			*ipsc_remote_console();

        if ((strcmp(name, "console") == 0) &&
	    ((cp = ipsc_remote_console()) != 0)) {
                strcpy(namebuf, cp);
                return namebuf;
        }

#if     iPSC860 || (iPSC386 && NMD > 0)
        if (strcmp(name, "md0a") == 0) {
		name = "<131>md0a";
	}
#endif  iPSC860 || (iPSC386 && NMD > 0)

	return name;
}
