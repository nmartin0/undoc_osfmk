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
 * $Log: cubestart.c,v $
 * Revision 1.5  1994/11/18  20:51:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:43:31  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:34:17  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:34:10  dleslie
 * First R1_0 release
 *
 * Revision 2.1.1.2.1.1  92/02/18  19:06:26  jeffreyh
 * 	Fixes from intel.
 * 	[92/02/18            jeffreyh]
 * 
 * Revision 2.2  91/12/10  16:32:32  jsb
 * 	New files from Intel
 * 	[91/12/10  16:12:51  jsb]
 * 
 */

#include <mach/boolean.h>

#if	i386
#define	BOOT_CPU_STARTUP_NAME	"BOOT_386_STARTUP_NAME"
#define	BOOT_CPU_EMULATOR_NAME	"BOOT_386_EMULATOR_NAME"
#define	BOOT_CPU_NO_SERVER	"BOOT_386_NO_SERVER"
#define	BOOT_CPU_REMOTE_CONS	"BOOT_386_REMOTE_CONS"
#endif	i386
#if	i860
#define	BOOT_CPU_STARTUP_NAME	"BOOT_860_STARTUP_NAME"
#define	BOOT_CPU_EMULATOR_NAME	"BOOT_860_EMULATOR_NAME"
#define	BOOT_CPU_NO_SERVER	"BOOT_860_NO_SERVER"
#define	BOOT_CPU_REMOTE_CONS	"BOOT_860_REMOTE_CONS"
#endif	i860

extern char	*startup_name, *emulator_name;
extern char	*getbootenv();


boolean_t ipsc_bootstrap_askname()
{
	char		*server, *emul;

	if (getbootenv(BOOT_CPU_NO_SERVER) || getbootenv("BOOT_NO_SERVER")) {
		return FALSE;
	}

	if ((server = getbootenv(BOOT_CPU_STARTUP_NAME)) != 0) {
		startup_name = server;
	} else if ((server = getbootenv("BOOT_STARTUP_NAME")) != 0) {
		startup_name = server;
	}

	if ((emul = getbootenv(BOOT_CPU_EMULATOR_NAME)) != 0) {
		emulator_name = emul;
	} else if ((emul = getbootenv("BOOT_EMULATOR_NAME")) != 0) {
		emulator_name = emul;
	}

	printf("Server name is %s.\n", startup_name);
	printf("Emulator name is %s.\n", emulator_name);

	return TRUE;

}


char *ipsc_remote_console()
{
	char	*name;

	if ((name = getbootenv(BOOT_CPU_REMOTE_CONS)) != (char *) 0) {
		return name;
	}
	if ((name = getbootenv("BOOT_REMOTE_CONS")) != (char *) 0) {
		return name;
	}

	return (char *) 0;
}
