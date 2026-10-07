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
/* @(#)error.h	1.7  com/cmd/prog/common,3.1,9013 1/18/90 11:03:04 */
/*
 * COMPONENT_NAME: (CMDPROG) error.h
 *
 * FUNCTIONS: TOOLSTR, cerror, uerror, warning, werror                       
 *
 * ORIGINS: 27 03 09 32 00 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Changes for ANSI C were developed by HCR Corporation for IBM
 * Corporation under terms of a work made for hire contract.
 */

# ifndef EXIT
# define EXIT exit
# endif

#ifdef MSG
#include "ctools_msg.h"
#define TOOLSTR(Num, Str) NLcatgets(catd, MS_CTOOLS, Num, Str)
nl_catd catd;
#else
#define TOOLSTR(Num, Str) Str
#endif


int nerrors = 0;  /* number of errors */

/* -------------------- uerror -------------------- */

/*VARARGS1*/
uerror( mode, s, a, b ) char *s; {
	if( mode ){
#if	defined (LINT) || defined (CFLOW)
		if( !ifname || !strcmp(pfname, ifname) )
			fprintf( stdout, TOOLSTR(M_MSG_199, "\"%s\", line %d: error: "), pfname, lineno );
		else
			fprintf( stdout, TOOLSTR(M_MSG_277, "\"%s\", line %d (\"%s\"): error: "), ifname, lineno, pfname );
		fprintf( stdout, s, a, b );
		fprintf( stdout, "\n" );
#else
		fprintf( stderr, TOOLSTR(M_MSG_288, "%s, line %d: error: "), ftitle, lineno );
		fprintf( stderr, s, a, b );
		fprintf( stderr, "\n" );
#endif
		if( ++nerrors > 30 )
			cerror(TOOLSTR(M_MSG_257, "too many errors"));
	} else {
#ifdef WANSI
		warning( WANSI, s, a, b );
#else
		warning( 1, s, a, b );
#endif
	}
}

/* -------------------- werror -------------------- */

/*VARARGS1*/
werror( mode, s, a, b ) char *s; {
	if( mode ){
#if	defined (LINT) || defined (CFLOW)
		if( !ifname || !strcmp(pfname, ifname) )
			fprintf( stdout, TOOLSTR(M_MSG_199, "\"%s\", line %d: error: "), pfname, lineno );
		else
			fprintf( stdout, TOOLSTR(M_MSG_277, "\"%s\", line %d (\"%s\"): error: "), ifname, lineno, pfname );
		fprintf( stdout, s, a, b );
		fprintf( stdout, "\n" );
#else
		fprintf( stderr, TOOLSTR(M_MSG_288, "%s, line %d: error: "), ftitle, lineno );
		fprintf( stderr, s, a, b );
		fprintf( stderr, "\n" );
#endif
	} else {
#ifdef WANSI
		warning( WANSI, s, a, b );
#else
		warning( 1, s, a, b );
#endif
	}
}

/* -------------------- cerror -------------------- */
/* Nov 7, 1992  (Stephen G. Delahunty)
   The routine 'cerror' is also used by the trap handler to store the failure
   reason in errno and return a (-1) to the calling process. By using the same
   function name here, this routine gets called immediately following the trap.
   I have renamed this function to "MYcerror" and have included a 'define' in the
   header file 'mfile1.h'. This is only a temporary fix. The real fix should be
   to rename the 'cerror' function in the trap handler because this progam will
   bite us (or someone in the field) in the future.
*/
/*VARARGS1*/
MYcerror( s, a, b, c ) char *s; {
	/*
	** Compiler error: die
	*/
	if( nerrors && nerrors <= 30 ){
		/* give the compiler the benefit of the doubt */
		fprintf( stderr, TOOLSTR(M_MSG_288, "%s, line %d: error: "), ftitle, lineno );
		fprintf( stderr, TOOLSTR(M_MSG_289,
			"cannot recover from earlier errors: goodbye!\n" ));
	} else {
#if	defined (LINT) || defined (LINTP2)
		fprintf( stderr, TOOLSTR(M_MSG_290, "%s, line %d: lint error: " ), ftitle, lineno);
#else
#if defined (CFLOW) || defined (CFLOW2)
		fprintf( stderr, TOOLSTR(M_MSG_267, "%s, line %d: cflow error: " ), ftitle, lineno);
#else
#if defined (CXREF)
		fprintf( stderr, TOOLSTR(M_MSG_268, "%s, line %d: cxref error: " ), ftitle, lineno);
#else
		fprintf( stderr, TOOLSTR(M_MSG_291, "%s, line %d: compiler error: " ), ftitle, lineno);
#endif
#endif
#endif
		fprintf( stderr, s, a, b, c );
		fprintf( stderr, "\n" );
	}
#ifdef FORT
	{	extern short debugflag;
		if (debugflag)
			abort();
	}
#endif /*FORT*/
	EXIT( 2 );
}

/* -------------------- warning -------------------- */

/*VARARGS1*/
warning( mode, s, a, b ) int mode; char *s; {
	if( mode ){
#if	defined (LINT) || defined (CFLOW)
		if( !ifname || !strcmp(pfname, ifname) )
			fprintf( stdout, TOOLSTR(M_MSG_292, "\"%s\", line %d: warning: "), pfname, lineno );
		else
			fprintf( stdout, TOOLSTR(M_MSG_293, "\"%s\", line %d (\"%s\"): warning: "), ifname, lineno, pfname );
		fprintf( stdout, s, a, b );
		fprintf( stdout, "\n" );
#else
		fprintf( stderr, TOOLSTR(M_MSG_294, "%s, line %d: warning: "), ftitle, lineno );
		fprintf( stderr, s, a, b );
		fprintf( stderr, "\n" );
#endif
	}
}
