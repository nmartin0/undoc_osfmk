/*
 *    bsd.h
 */
/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */


#ifndef _BSD_H_
#define _BSD_H_

/*
 *  All BSD functions go here.  This includes all functions which should
 *  be included for compatibility, but have been orphaned by POSIX, XOPEN,
 *  ANSI C, or any other standard.
 *  
 *  Use of these functions is discouraged.  Possible alternatives are given
 *  in comments.
 */

#include <standards.h>

_SSD_FUNC_PROTO_BEGIN_

	char * getwd __protomagic((char *));               /* getcwd() */

	void   bcopy __protomagic((char *, char *, int));  /* memcpy() */
	int    bcmp  __protomagic((char *, char *, int));  /* memcmp() */
	void   bzero __protomagic((char *, int));          /* none     */
	int    ffs   __protomagic((int));                  /* none     */

_SSD_FUNC_PROTO_END_

#endif /* _BSD_H_ */

