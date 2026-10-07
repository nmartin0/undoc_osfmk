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
static char rcsid[] = "@(#)$RCSfile: crt0msg.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:05 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: start
 *
 * ORIGINS: 3 10 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1990
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * crt0msg.c  1.1  com/lib/c/gen,3.1,9013 1/18/90 10:20:02
 */

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>             /* for error messages */
#include <mon.h>               /* for _PROF_IS_P and _PROF_IS_PG */

#define MSGIDP "mcrt0"
#define MSGIDPG "gcrt0"

#if defined(NLS) || defined(KJI)
#define FPRINTF NLfprintf
#else
#define FPRINTF fprintf
#endif /* NLS or KJI */

#ifdef MSG
#include "libc_msg.h"
#define MSG01 NLgetamsg(MF_LIBC, MS_LIBC, M_CRT0, \
                               "%s: Profiling setup call to %s failed. rc: %d\n")
#else
#define MSG01 "%s: Profiling setup call to %s failed. rc: %d\n"
#endif /* MSG */

/*
 * NAME: crt0msg
 *
 * FUNCTION:
 *      Routine to output error message for crt0.s
 *
 *
 * EXECUTION ENVIRONMENT:
 *
 *      Called by mcrt0/gcrt0 if error return from monstartup.
 *
 * (NOTES:)
 *
 *      Written in C to properly use system message facilities.
 *
 *
 * (RECOVERY OPERATION:) None
 *
 * (DATA STRUCTURES:)
 *
 * RETURNS: void
 */

void crt0msg( int prof,              /* +1 is gcrt0 -1 is mcrt0      */
              int rc                 /* rc for message */
              )
{
        if ( prof == _PROF_TYPE_IS_P ) {
                FPRINTF(stderr, MSG01,MSGIDP,"monstartup", rc ) ;
                }
        if ( prof == _PROF_TYPE_IS_PG ) {
                FPRINTF(stderr, MSG01,MSGIDPG,"monstartup", rc ) ;
                }
        return;

} /* end of function start */
