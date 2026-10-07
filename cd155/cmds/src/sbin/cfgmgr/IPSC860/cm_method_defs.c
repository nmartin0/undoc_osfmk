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
 * OSF/1 Release 1.0.1
 *
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: cm_method_defs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:36 $";
#endif


#include <stdio.h>
#include <sys/types.h>

#include "../cm_method.h"


extern int STREAMS_method();
extern int FILSYS_method();
extern int XTI_method();
extern int INET_method();
extern int XNS_method();


extern cmgr_cmfunc_t   cmgr_cmfunc[];
cmgr_cmfunc_t   cmgr_cmfunc[] = {

#ifdef STREAMS_METHOD
		{"streams",	STREAMS_method},
#endif

#ifdef FILSYS_METHOD
		{"filsys",	FILSYS_method},
#endif

#ifdef XTI_METHOD
        	{"xti",		XTI_method},
#endif

#ifdef INET_METHOD
        	{"inet",	INET_method},
#endif

#ifdef XNS_METHOD
        	{"xns",		XNS_method},
#endif
	        {NULL,		NULL}
};
