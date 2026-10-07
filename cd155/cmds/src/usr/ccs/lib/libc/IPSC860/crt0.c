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
 *
 * $Id: crt0.c,v 1.21 1994/11/19 02:10:49 mtm Exp $
 *
 * HISTORY
 * $Log: crt0.c,v $
 * Revision 1.21  1994/11/19  02:10:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.20  1994/06/17  00:12:01  raya
 * Added declaration of __debug_info__ symbol for use in IPD.
 *
 * Revision 1.19  1993/08/04  20:49:59  stans
 *    Removed '_NX_present' as this was a BAD idea. Turns out pthread pgms
 *    must initialize the proxy process and all the targets of rforkmulti().
 *    libpthreads.a now has its own versions of rfork() and rforkmulti()
 *    which do the correct re-initialization.
 *
 * Revision 1.18  1993/08/03  22:47:03  stans
 *   Added "we are a parallel pgm" boolean '_NX_present' so pthreads will not
 *   initialize here but after the 'rforkmulti()' call. Problem was the pthreads
 *   library would initialize before the rforkmulti() and hence be invalid at
 *   the destination node. pthread_init() is written to be called ONCE. Defer
 *   init until we reach the destination node.
 *
 * Revision 1.17  1993/06/29  14:32:52  shala
 * Force getwd() to be defined with the user's program. This is temporary
 * solution for IPD to be able to find the path to the files under debug
 * which reside in the current directory. The real fix will be a new
 * system call which can be used by IPD to get the info. That is plan
 * for after R1.1.
 *
 * Revision 1.16  1993/05/19  17:42:08  stans
 *    Support the micro-kernel generated cthreads package.
 *
 * Revision 1.15  1993/04/19  21:26:09  stans
 * Support Mach 3.0 C threads initialization detection.
 *
 * Revision 1.14  1993/04/05  21:56:24  shala
 * Put the label asm("eprol:") under MCRT0 where it belonged. Now when
 * a bt (traceback) from gdb is done, it ends  with crt0_startup()
 * instead of 'eprol'. Fixed by stans.
 *
 * Revision 1.13  1993/01/28  00:03:56  dleslie
 * Removed include of version.h.
 *
 * Revision 1.12  1993/01/05  17:30:41  shala
 * Included version.h file.
 *
 * Revision 1.11  1993/01/05  16:53:57  shala
 * Added version.
 *
 * Revision 1.10  1992/10/13  17:39:34  shala
 * Use start as the Entry point.
 *
 * Revision 1.9  1992/06/10  16:58:04  cameron
 * Changed setup again, this time always call nx_setup().
 *
 * Revision 1.8  92/06/09  19:34:45  stans
 * Support the parallel application initialization function pointer
 * "nx_setup_routine". When linked without "icc -nx" then nx_setup_routine is
 * set to zero (ala BSS declaration). When linked with "icc -nx" then
 * nx_setup_routine is initialized to nx_setup() and the parallel initialization
 * routines are called.:w
 * 
 * Revision 1.7  92/06/09  10:49:02  stans
 * typo in ID macro name...
 * 
 * Revision 1.6  92/06/09  10:27:57  stans
 * added ID string to file comments plus will generate ident compatible
 * ID string.
 * 
 */
char _crt0_id[]="$Id: crt0.c,v 1.21 1994/11/19 02:10:49 mtm Exp $";

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

extern int exit();
extern char *getwd();

/* Used by IPD debugger */
unsigned __debug_info__ = 0;

void	(*mach_init_routine)();
int	(*_cthread_init_routine)();
void	(*_cthread_exit_routine)();
void	(*_pthread_init_routine)();
void	(*_pthread_exit_routine)();

#if NOT_YET
int _ldr_crt0_request = 1;
int _ldr_present;
void (*_ldr_jump_func)();
#endif

#ifdef	MCRT0
int	crt0msg();
extern	unsigned char	eprol;
#endif

extern	unsigned char	etext;

dummy_start()
{
        asm("   start:: br      __crt0_start            ");
        asm("   nop                             ");
}

static dummy_getwd() {}

_crt0_start( argc, argv, envp )
	int argc;
	char *argv[];
	char *envp[];
{
	__denormal();

	environ	= envp;

	if ( !getwd ) dummy_getwd();

	/* if (libmach.a is linked in) then initialize our Mach interfaces */
	if (mach_init_routine)
		(*mach_init_routine)();

#ifdef MCRT0
asm("_eprol:");
#endif

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
	if ( monstartup(&eprol, &etext) )
		crt0msg( -1, 1 );
#endif MCRT0

	/* cthreads and pthreads are mutually exclusive */
	if (_cthread_init_routine) {
		int	newsp;

		/* Get new stack and align on 16 byte boundary.
		 * the cthreads package is assumed to be the micro-kernel
		 * generated version.
		 */
		newsp = (*_cthread_init_routine)() & ~0xf;
		if (newsp)
			_set_cthread_stack(newsp);
	}
	else {
		/*
		 * Initialize the pthread library?
		 */
		if ( _pthread_init_routine )
			(*_pthread_init_routine)();
	}

	errno = 0;

#ifdef NOT_YET
        if (_ldr_present) {
                void (*entry_pt)();

		/*
		 * Call nx_setup() to do all the NX specific startup
		 * wierdness in one place. See libnx/nx_setup.c
		 */
		nx_setup(&argc, argv, environ);

                entry_pt = (void ((*)()))main(argc, argv, environ );
                (*_ldr_jump_func)(entry_pt, argc);

                /*NOTREACHED*/
        }
#endif
	/*
	 * Call nx_setup() to do all the NX specific startup
	 * wierdness in one place. See libnx/nx_setup.c
	 */
	nx_setup(&argc, argv, environ);

	argc = main( argc, argv, environ );

	/* cthreads and pthreads are mutually exclusive */
	if (_cthread_exit_routine)  
		(*_cthread_exit_routine)( argc );
	else {
		/*
		 * perform any pthread exit work?
		 */
		 if ( _pthread_exit_routine )  
			(*_pthread_exit_routine)( argc );
	}

	exit( argc );
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

/*
 *  Switch to the stack which was passed back from cthread_init().
 */

_FAKE(){
asm("__set_cthread_stack::");
asm("bri        r1");
asm("mov r16, sp");
}
