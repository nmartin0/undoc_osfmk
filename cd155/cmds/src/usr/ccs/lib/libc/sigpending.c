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
 * OSF/1 Release 1.0
 */
/* sigpending -- library stub to transform calling sequence */

/*
 * user program calls:
 *     success = sigpending(sigset_t *mask);
 *
 * kernel-side interface:
 *     sigset_t omask = sigpending();
 *
 */

#include <sys/signal.h>
#include <sys/param.h>
#include <errno.h>

extern sigset_t _sigpending();

int
sigpending(mask)
sigset_t        *mask;
{
        errno = 0;
        *mask = _sigpending();
        if (errno != 0) {
                return (-1);
        }
        else
                return (0);
}

