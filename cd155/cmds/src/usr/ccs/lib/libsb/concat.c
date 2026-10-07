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
/*
 * subroutine for concatenating strings into a buffer
 *
 * char *concat(buf, buflen, ptr1, ptr2, ..., NULL) { char *ep; return(ep); }
 * char *buf, *ptr1, *ptr2, ...;
 * int buflen;
 *
 * "buflen" should be sizeof("buf")
 * "buf" will be terminated by a null byte
 * "concat" will return a pointer to the null byte, if return is non-null
 *
 * concat will return null(0) under any of the following conditions:
 *    1) buf is null
 *    2) buflen <= 0
 *    3) buf was not large enough to hold the contents of all the ptrs.
 */

#include <stdio.h>
#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

char *vconcat();

/*VARARGS2*/
char *
#if __STDC__
concat(char *buf, int buflen, ...)
#else
concat(va_alist)
va_dcl
#endif
{
#if !__STDC__
    char *buf;
    int buflen;
#endif
    va_list ap;
    char *ptr;

#if __STDC__
    va_start(ap, buflen);
#else
    va_start(ap);
    buf = va_arg(ap, char *);
    buflen = va_arg(ap, int);
#endif
    ptr = vconcat(buf, buflen, ap);
    va_end(ap);
    return(ptr);
}

char *
vconcat(buf, buflen, ap)
char *buf;
int buflen;
va_list ap;
{
    register char *arg, *ptr, *ep;

    if (buf == NULL)
	return(NULL);
    if (buflen <= 0)
	return(NULL);
    ptr = buf;
    *ptr = '\0';
    ep = buf + buflen;
    while (ptr != NULL && (arg = va_arg(ap, char *)) != NULL)
	while (*ptr = *arg++)
	    if (++ptr == ep) {
		ptr = NULL;
		break;
	    }
    return(ptr);
}
