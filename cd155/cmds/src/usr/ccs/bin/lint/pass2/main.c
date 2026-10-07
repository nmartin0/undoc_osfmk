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
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: main.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:53:53 $";
#endif
/*
 * COMPONENT_NAME: (CMDPROG) Programming Utilites
 *
 * FUNCTIONS: LintPass2, main
 *
 * ORIGINS: 00 03 10 27 32
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * main.c	1.6  com/cmd/prog/lint/pass2,3.1,9049base 10/10/90 16:58:26 
 */
#include "lint_msg.h"
#define         MSGSTR(Num, Str) catgets(catd, MS_LINT, Num, Str)
nl_catd catd;

#include "mfile1.h"
#include "error.h"
#include "lint2.h"
#include <locale.h>

main(argc, argv)
int argc;
register char *argv[];
{
	register int i, c;
	register char *cp;
	register int r;
	int fdef = 0;

	setlocale(LC_ALL, "");
	catd = catopen(MF_LINT, 0);

	for( i = 0; i <= MXDBGFLG; i++ ){
		devdebug[i] = 0;
		warnlevel[i] = 0;
	}

	for( i=1; i<argc; ++i ){
		if( *(cp=argv[i]) == '-' )
		while( *++cp ){
		    switch( *cp ){
		    case 'p': /* extreme portability */
			    pflag++;
			    break;

		    case 'X': /* debug toggles */
			    debug++;
			    break;

		    case 'w': /* toggle warning level */
			    while( *++cp )
				{
				switch(*cp)
				{
				case 'A':
  
				/*if( *cp == 'A' ) */
					for( r = 0; r <= MXDBGFLG; r++ )
						warnlevel[r] = !warnlevel[r];
					break;
				case 'l':
					warnlevel[*cp] = 0;
					break;
				case 'u':
					warnlevel[*cp] = 0;
					break;
				case 'D':
					warnlevel[*cp] = 0;
					break;
				case 'R':
					warnlevel[*cp] = 0;
				 	break;
				case 'h':
					warnlevel[*cp] = 0;
					break;
				default:
					
				/*else */if( ( *cp >= 'a' && *cp <= 'z' ) ||
						( *cp >= 'A' && *cp <= 'Z' ) )
					warnlevel[*cp] = !warnlevel[*cp];
					}
				}
			    --cp;
			    break;

		    case 'M': /* HCR development toggles */
			    while( *++cp )
				if( ( *cp >= 'a' && *cp <= 'z') ||
						( *cp >= 'A' && *cp <= 'Z' ) )
					devdebug[*cp] = !devdebug[*cp];
			    --cp;
			    break;
		    }
		}

		else {
			/* First-time initializations. */
			if (fdef == 0) {
				/* check for ANSI mode */
				if ( devdebug[ANSI_MODE] ) {
					/* set all other ANSI flags on */
					devdebug[ANSI_PARSE]	=
						!devdebug[ANSI_PARSE];
					devdebug[COMPATIBLE]	=
						!devdebug[COMPATIBLE];
					devdebug[PROMOTION]	=
						!devdebug[PROMOTION];
					devdebug[REFDEF]	=
						!devdebug[REFDEF];
					devdebug[SCOPING]	=
						!devdebug[SCOPING];
					devdebug[STRUCTBUG]	=
						!devdebug[STRUCTBUG];
					devdebug[TYPING]	=
						!devdebug[TYPING];
				}
				InitTypes();
				fdef = 1;
				curSym = &theSym;
			}
			fname = argv[i];
			OpenFile();
			LintPass2();
			CloseFile();
		}
	}
	CheckSymbols();
}

LintPass2()
{
	int stat;

	/* Read and process all symbol records. */
	while (1) {
		switch (InHeader()) {
		case LINTEOF:
			/* EOF delimiter. */
			if (markerEOF)
				return;
			markerEOF = 1;
			continue;

		case LINTADD:
			/* Append to function usage, symbol should exist. */
			InUsage();
			prevSym = LookupSymbol(curSym, LOOK);
			if (prevSym)
				AddFtnUsage(prevSym, curSym);
			else
				cerror(MSGSTR(M_MSG_260,
					"i/o sequence error on file %s"),
					curPFname);

			break;

		case LINTSYM:
			/* Fetch symbol. */
			InSymbol();
			prevSym = LookupSymbol(curSym, LOOK);

			/* Cross-check old symbols, otherwise
			 * insert new symbols. */
			if (prevSym) {
				switch (stat = CheckSymbol()) {
				case STORE:
					(void) LookupSymbol(curSym, stat);
					break;
				case CHANGE:
					ChangeSymbol(prevSym, curSym);
					break;
				case REJECT:
					/* Remember bad function calls. */
					if (ISFTN(curSym->type))
						FtnRefSymbol(prevSym, curSym);
#ifdef	DEBUG
					if (debug)
						PrintSymbol("REJECTING",
							curSym);
#endif
					break;
				default:
					cerror(MSGSTR(M_MSG_261,
			"unknown action for symbol in file %s"), curPFname);
				}
			}
			else
				(void) LookupSymbol(curSym, STORE);
			break;

		default:
			cerror(MSGSTR(M_MSG_262,
				"unknown record directive in file %s"),
				curPFname);
		}
	}
}
