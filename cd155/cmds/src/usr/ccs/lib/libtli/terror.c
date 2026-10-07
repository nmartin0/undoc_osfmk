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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */

/** Copyright (c) 1989  Mentat Inc.
 ** terror.c 1.3, last change 12/20/89
 **/

#include <tli/common.h>
#include <stdio.h>
#ifdef	XTI
#include <xti.h>
#else
#include <tiuser.h>
#endif
#include <errno.h>

#ifdef XTI

char	* t_errlist[] = {
	"no error specified",
	"incorrect address format",
	"incorrect option format",
	"incorrect permissions",
	"illegal transport fd",
	"could not allocate address",
	"out of state",
	"bad call sequence number",
	"system error",
	"event requires attention",
	"illegal amount of data",
	"buffer not large enough",
	"flow control",
	"no data",
	"no disconnect indication available",
	"no unitdata error indication available",
	"bad flags",
	"no orderly release indication available",
	"primitive not supported",
	"state is in process of changing",
	"unsupported struct-type requested",
	"invalid transport provider name",
	"qlen is zero",
	"address in use"
};

#else

char	* t_errlist[] = {
	"no error specified",
	"insufficient permission",
	"incorrect transport address format",
	"illegal amount of data",
	"file descriptor does not refer to a transport endpoint",
	"incorrect flags",
	"incorrect transport options",
	"incorrect sequence number",
	"buffer size insufficient",
	"cannot send message",
	"an asynchronous event has occurred",
	"transport provider could not allocate an address",
	"no data is available",
	"no orderly release indication is available",
	"function not supported by the transport",
	"primitive issued in the wrong sequence",
	"state is changing",
	"a system error occurred",
	"no unitdata error indication is available",
	"no disconnect indication is available"
};
#endif

int	t_errno;
int	t_nerr = sizeof(t_errlist) / sizeof(t_errlist[0]);

#if defined(_NO_PROTO)
extern	char	*terrmsg();
#else
extern	char	*terrmsg( int errno );
#endif /* defined(_NO_PROTO) */

extern 	int 	sys_nerr;
extern 	char 	*sys_errlist[];
extern	int	errno;

void
t_error (msg)
	char	* msg;
{
	if (!msg)
	{
		msg = "t_error";
	}

	if (t_errno >= 0  &&  t_errno < t_nerr) 
	{
		if (t_errno == TSYSERR)
			fprintf(stderr, "%s: %s, %s\n", msg, t_errlist[t_errno], terrmsg(errno));
		else if( *msg == NULL)
			fprintf(stderr, "%s\n", t_errlist[t_errno]);
		else
			fprintf(stderr, "%s: %s\n", msg, t_errlist[t_errno]);
	} else
		fprintf(stderr, "%s: unknown error %d\n", msg, t_errno);

	return;
}



/*
 * terrmsg()	
 *
 *	Map the value of errno to a text string error message
 *
 * Inputs:
 *	e	= Current value of errno
 *
 * Outputs:
 *	None.
 *
 * Returns:
 *	Pointer to null terminated character string error
 *	message.
 */

static char buf[256];

char *terrmsg( e )
int e;
{
	/*
	 * Clear buffer prior to usage
	 */
	bzero(buf, sizeof(buf));

	/*
	 * Check range of error value
	 */
	if ( (e >= 0) && (e < sys_nerr) )
	{
		sprintf(buf, "%s", sys_errlist[e]);
	}
	else
	{
		sprintf(buf, "Error %d occurred.", e);
	}

	/*
	 * Return pointer to statically defined 
	 * buffer contaning error string.
	 */
	return(buf);
}


