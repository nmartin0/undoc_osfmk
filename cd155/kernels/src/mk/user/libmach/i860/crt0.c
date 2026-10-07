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
 * Copyright 1991 by Intel Corporation,
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
 * $Log: crt0.c,v $
 * Revision 1.5  1994/11/18  21:06:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:05:42  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:59:53  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:05:59  dleslie
 * First R1_0 release
 *
 * Revision 2.3.2.2  92/04/08  15:47:09  jeffreyh
 * 	Moved set_cthread_stack here from pre_crt0.o so that crto.o would
 * 	 be usable in a standalone environment.
 * 	[92/04/06            jeffreyh]
 * 
 * Revision 2.3.2.1  92/03/28  10:23:18  jeffreyh
 * 	Fix cthread stack botch
 * 	[92/03/26            cfj]
 * 
 * Revision 2.3  91/12/10  16:33:02  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:31:47  jsb]
 * 
 */ 

#define BE_MACH 0
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University and Alessandro Forin
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */

/*
 *	C start up routine.
 *
 *      Intel SSD i860 entry assumptions: [ Wed Sep 11 13:44:22 PDT 1991 ]
 *
 *      User`s stack (grows down in memory) high memory here.
 *
 *      =================
 *      |     <null>    |
 *      =================
 *      |    envp[1]    |
 *      =================
 *      |    envp[0]    |<--    r18     address of envp[0]
 *      =================
 *      |     <null>    |
 *      =================
 *      |  argv[argc-1] |
 *      =================
 *      |     argv[0]   |<--    r17     address of argv[0]
 *      =================
 *      |      argc     |<--    r16     contents of top-of-stack (argc)
 *      =================
 */

char **environ = (char **)0;
char **_auxv = (char **)0;

int errno; 

#ifdef paranoid
static int fd;
#endif paranoid


#if	MACH
int	(*mach_init_routine)();
int	(*_cthread_init_routine)();
int	(*_cthread_exit_routine)();
int	(*_StrongBox_init_routine)();
extern int exit();
#endif	MACH



extern	unsigned char	etext;
extern	unsigned char	eprol;

_crt0_start( argc, argv, envp )
	int argc;
	char *argv[];
	char *envp[];
{
	environ	= envp;


#if	MACH
	if (mach_init_routine)
		(void) mach_init_routine();
#endif	MACH

asm("_eprol:");

#ifdef paranoid
	/*
	 * The standard I/O library assumes that file descriptors 0, 1, and 2
	 * are open. If one of these descriptors is closed prior to the start 
	 * of the process, I/O gets very confused. To avoid this problem, we
	 * insure that the first three file descriptors are open before calling
	 * main(). Normally this is undefined, as it adds two unnecessary
	 * system calls.
	 */
	do	{
		fd = open("/dev/null", 2);
	} while (fd >= 0 && fd < 3);
	close(fd);
#endif paranoid

#ifdef MCRT0
	monstartup(&eprol, &etext);
#endif MCRT0

	errno = 0;


#if	MACH
	if (_cthread_init_routine){
		int newsp;

		/* Get new stack and align on 16 byte boundary. */
		newsp = (*_cthread_init_routine)() & ~0xf; 
		if (newsp)
			_set_cthread_stack(newsp);
	}
	if (_StrongBox_init_routine)	(*_StrongBox_init_routine)();
	if (_cthread_exit_routine)  
		_cthread_exit_routine(main(argc, argv, environ));
	else
		exit(main(argc, argv, environ));
#else
	exit(main(argc, argv, environ));
#endif
}

#ifdef MCRT0
/*ARGSUSED*/
exit(code)
	register int code;
{
	/* Save profiling info in file */
	monitor(0);
	_cleanup();
	_exit( code );
}
#endif /* MCRT0 */

#ifdef CRT0
/*
 * null mcount and moncontrol,
 * just in case some routine is compiled for profiling
 */
moncontrol(val)
	int val;
{

}

mcount() {}
#endif CRT0

#ifdef MACH
/*
 *  Switch to the stack which was passed back from cthread_init().
 */

_FAKE(){
asm("__set_cthread_stack::");
asm("bri	r1");
asm("mov r16, sp");
}
#endif MACH


