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

#include "cfgmgr.h"
#include "cm.h"
#include "cm_msgstr.h"

#ifdef MSG
#define MSGSTR(Num, Str) 	catgets(catd, MS_CFGMGR, Num, Str)
#else
#define MSGSTR(Num, Str) 	Str
#endif

/*
 *	CMGR:	MSG conversion
 */

char *
cm_msg( int msg_id )
{
#ifdef MSG
	static nl_catd	catd;
#endif
	extern char *	cmgr_msgstr[];
	static int	maxerr;
	static int	initialized = FALSE;

	if (initialized == FALSE) {
		initialized = TRUE;
		for ( maxerr=0; cmgr_msgstr[maxerr] != NULL; maxerr++)
		;
#ifdef MSG
        	catd = catopen(MF_CFGMGR, MS_CFGMGR);
#endif
	}

	if (msg_id < 0 || msg_id >= maxerr)
		msg_id = MSG_MAX;

	return(MSGSTR(msg_id, cmgr_msgstr[msg_id]));
}

