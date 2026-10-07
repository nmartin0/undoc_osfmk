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
 * execle( path,arg0,....argn,(char *)0, envp ) 
 *      char    *path;
 *      char    *arg0...*argn
 *      char    **envp;
 *
 * Collect variable number of args into a stack-based (char *argv[]) structure.
 * Pass new *argv[] off to execve().
 *
 */
#include <varargs.h>

#define MAXARGS  256

execle(va_alist)
va_dcl
{
	register int	arg_index = 0;
	va_list		ap;
	char		*file;
	char		*args[MAXARGS];
	char		**envp;

	va_start(ap,foo);

	file = va_arg(ap, char *);

        /*
         * collect varargs into argv[], make sure we do NOT overflow!
         */
	while( args[arg_index++] = va_arg(ap, char *) ) {
                if ( arg_index >= MAXARGS )
                        return;
        }

	envp = va_arg(ap, char **);

	va_end(ap);

	execve(file, args, envp);
}
