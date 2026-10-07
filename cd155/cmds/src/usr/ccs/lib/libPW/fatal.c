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
static char sccsid[] = "@(#)fatal.c	1.7  com/lib/PW,3.1,9008 1/18/90 10:33:32";
/*
 * COMPONENT_NAME: (LIBPW) Programmers Workbench Library
 *
 * FUNCTIONS: fatal
 *
 * ORIGINS: 3 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 */

# include	"sys/types.h"
# include	"macros.h"
# include	"fatal.h"

#ifdef MSG
#include "pw_msg.h"
nl_catd catd;
#define MSGSTR(Num, Str) NLcatgets(catd, MS_PW, Num, Str)
#else
#define MSGSTR(Num, Str) Str
#endif

#ifndef NLS
#define NLfprintf fprintf
#endif

/*
	General purpose error handler.
	Typically, low level subroutines which detect error conditions
	(an open or create routine, for example) return the
	value of calling fatal with an appropriate error message string.
	E.g.,	return(fatal("can't do it"));
	Higher level routines control the execution of fatal
	via the global word Fflags.
	The macros FSAVE and FRSTR in <fatal.h> can be used by higher
	level subroutines to save and restore the Fflags word.
 
	The argument to fatal is a pointer to an error message string.
	The action of this routine is driven completely from
	the "Fflags" global word (see <fatal.h>).
	The following discusses the interpretation of the various bits
	of Fflags.
 
	The FTLMSG bit controls the writing of the error
	message on file descriptor 2.  The message is preceded
	by the string "ERROR: ", unless the global character pointer
	"Ffile" is non-zero, in which case the message is preceded
	by the string "ERROR [<Ffile>]: ".  A newline is written
	after the user supplied message.
 
	If the FTLCLN bit is on, clean_up is called with an
	argument of 0 (see clean.c).
 
	If the FTLFUNC bit is on, the function pointed to by the global
	function pointer "Ffunc" is called with the user supplied
	error message pointer as argument.
	(This feature can be used to log error messages).
 
	The FTLACT bits determine how fatal should return.
	If the FTLJMP bit is on longjmp(Fjmp,1) is
	called (Fjmp is declared in macros.h and must be set elsewhere
	via setjmp).
 
	If the FTLEXIT bit is on the value of userexit(1) is
	passed as an argument to exit(II)
	(see userexit.c).
 
	If none of the FTLACT bits are on
	(the default value for Fflags is 0), the global word
	"Fvalue" (initialized to -1) is returned.
 
	If all fatal globals have their default values, fatal simply
	returns -1.
*/

int	Fcnt;
int	Fflags;
char	*Ffile;
int	Fvalue = -1;
int	(*Ffunc)();
jmp_buf Fjmp;


fatal(msg)
char *msg;
{
	++Fcnt;
	if (Fflags & FTLMSG) {
#ifdef MSG
		catd = NLcatopen(MF_PW, 0);
#endif
		NLfprintf(stderr, MSGSTR(ERROR, "ERROR"));
		if (Ffile) {
			write(2, " [", 2);
			write(2,Ffile,length(Ffile));
			write(2, "]", 1);
		}
		write(2, ": ", 2);
		write(2,msg,length(msg));
		write(2, "\n", 1);
	}
	if (Fflags & FTLCLN)
		clean_up(0);
	if (Fflags & FTLFUNC)
		(*Ffunc)(msg);
	switch (Fflags & FTLACT) {
	case FTLJMP:
		longjmp(Fjmp,1);
	case FTLEXIT:
		exit(userexit(1));
	}
	return(Fvalue);
}
