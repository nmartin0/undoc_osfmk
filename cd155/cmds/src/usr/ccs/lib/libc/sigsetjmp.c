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
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: sigsetjmp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:20 $";
#endif
#include <setjmp.h>

/*
 * int sigsetjmp( env, savemask )
 *
 *      sigjmp_buf      env
 *      int             savemask
 *
 * if savemask is != 0 then save signal context in sigjmp_buf
 * otherwise don't save signal state.
 */

int sigsetjmp( sigjmp_buf env, int savemask )
{
	return( ( savemask ? setjmp( env ) : _setjmp( env ) ) );
}

#ifdef siglongjmp
#undef siglongjmp
#endif

void
siglongjmp(sigjmp_buf env, int val)
{
	if (env[_JBLEN-1])
		longjmp(env, val);
	else
		_longjmp(env, val);
}
