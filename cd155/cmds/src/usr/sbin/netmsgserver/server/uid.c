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
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * uid.c
 *
 *
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: uid.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:12:29 $";
#endif
/*
 * Generates locally unique identifiers.
 */


#define DEBUGOFF 1

#include <pthread.h>
#include <sys/errno.h>
#include <mach/boolean.h>
#include <sys/time.h>
#include <sys/types.h>

#include "netmsg.h"
#include "uid.h"

extern long	random();
extern char	*initstate();

#define STATE_SIZE	256
static char		random_state[STATE_SIZE];



/*
 * uid_init
 *
 * Initialises random number generator in order to produce unique identifiers.
 *
 */
EXPORT boolean_t uid_init()
BEGIN("uid_init")
    struct timeval tp;
    struct timezone tzp;

    (void)gettimeofday(&tp, &tzp);

    (void)initstate((unsigned int)tp.tv_usec, (char *)random_state, STATE_SIZE);
    
    RETURN(TRUE);
END


/*
 * uid_get_new_uid
 *
 * Returns a new unique identifier from the random number generator.
 *
 */
EXPORT long uid_get_new_uid()
BEGIN("uid_get_new_uid")
    long new_uid;
    new_uid = (long)random();
    RETURN(new_uid);
END
