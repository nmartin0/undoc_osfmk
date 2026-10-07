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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/common_nx_c/execve.c,v 1.3 1994/12/07 17:18:40 wds Exp $
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <nx/defines.h>
#include <errno.h>


/***************************** execve ***********************
 *
 *      Calling Sequence:
 *             execve(file, argv, envp);
 *
 *      Description:
 *		This execve() is specific to the NX library.  It replaces the
 *		execve() call in libc.a  
 *		It performs all the same functions as the libc.a execve(), but
 *		in addition it adds a flag to argv specifying the ptype the
 *		new process should inherit. If this function is called by 
 *		nx_loadve, the flag will already have been added and this 
 *		function will do nothing.
 *		
 *      Parameters:
 *              file       : char pointer, name of file to execute.
 *              argv       : command line arguments
 *              envp       : environment
 *
*/


int
execve(path, argv, envp)
char *path, *argv[], *envp[];

{
	int index, argc, newargc;
	char **newargv, **saveargv;
        char    *ptypestr;      /* ptype in the string format */
        char    special[] = SPECIAL;    /* Special flag for nx_loadve() */
        char    special2[] = SPECIAL2;    /* Special flag for execv() */


	if (_myptype() == -1) { /* No ptype to inherit, just do it! */
        	if(execvenx(path, argv, envp )  == -1 ) {
                	return(-1);
        	}
	} 
        /*

         * Build an argument list with a special flag and
         * the ptype. If special flag already present (from
	 * nx_loadve()), just call with given arguments.
         */
        for ( index = 0; argv[index] != NULL; index++) {
		if (strcmp(argv[index], special) == 0) {;
        		execvenx(path, argv, envp );
			/* execvenx should never return */
               		return(-1);
		}
	}
        argc = index;

       	saveargv = newargv = (char **)malloc( sizeof(char *) * (argc + 3));
       	if ( newargv == NULL ) {
               	errno = ENOMEM;
               	return(-1);
       	}

       	newargc = 0;
       	for ( index = 0; index < argc; index++) {
               	*newargv = argv[index];
               	newargv++;
               	newargc++;
       	}

       	*newargv = special2;
       	newargv++;
       	newargc++;
       	ptypestr = (char *)malloc ( sizeof( int));
       	if ( ptypestr == NULL ) {
               	errno = ENOMEM;
               	return(-1);
       	}
       	sprintf( ptypestr, "%d", _myptype());
       	*newargv = ptypestr;
       	newargv++;
       	newargc++;
       	*newargv = (char *) 0;
       	newargv = saveargv;

        /*
         *  Invalidate ptype (ipd_msg_info pointer in system table).
         */
        _nx_invalidate_ptype();

       	/*
         * Load the executable
         */
        if(execvenx(path, newargv, envp )  == -1 ) {
               	return(-1);
        }
}
