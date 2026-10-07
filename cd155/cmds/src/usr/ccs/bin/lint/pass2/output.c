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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: output.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:53:53 $";
#endif
/*
 * COMPONENT_NAME: (CMDPROG) Programming Utilites
 *
 * FUNCTIONS: LERROR, PrintSymbol, tprint
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
 * output.c	1.4  com/cmd/prog/lint/pass2,3.1,9013 12/22/89 14:31:48";
 */

#include "lint_msg.h"
#define         MSGSTR(Num, Str) catgets(catd, MS_LINT, Num, Str)
nl_catd catd;

#include "mfile1.h"
#include "lint2.h"

/*
** Lint pass2 standard error message.
*/
LERROR(mode, s, p, use)
	int mode, use;
	char *s;
	SMTAB *p;
{
	char *pn, *in;
	short l;

	if (mode) {
		switch (use) {
		case CRUSE:
			pn = curPFname;
			in = curIFname;
			l = curRLine;
			break;

		case CDUSE:
			pn = curPFname;
			in = curIFname;
			l = curDLine;
			break;

		case DUSE:
		case RUSE:
		default:
			pn = p->rd[use].pfname;
			in = p->rd[use].ifname;
			l = p->rd[use].line;
			break;
		}
		if (!strcmp(pn, in) || !in)
			printf(MSGSTR(M_MSG_263, "\"%s\", line %d: warning: "), pn, l);
		else
			printf(MSGSTR(M_MSG_264, "\"%s\", line %d (\"%s\"): warning: "),
				in, l, pn);
		printf(s, p->sname);
		printf("\n");
	}
}

#ifdef	DEBUG
/*
** Debug mode symbolic diagnostic.
*/
PrintSymbol(s, p)
	char *s;
	SMTAB *p;
{
	register MBTAB *m;
	static char done = 0;

	if (!done) {
		printf("DPF (DIF, DL)/RPF (RIF, RL): Symbol <Type> Usage\n");
		printf("\tMember <Type> Tagname...\n");
		printf("------------------------------------------\n");
		++done;
	}

	/* Print message, if there is one. */
	if (*s)
		printf("%s\n", s);

	/* Print symbol information. */
	if (!strcmp(p->dpf, p->dif))
		printf("%s (%d)/", p->dpf, p->dl);
	else
		printf("%s (%s, %d)/", p->dpf, p->dif, p->dl);
	if (!strcmp(p->rpf, p->rif))
		printf("%s (%d): %s <", p->rpf, p->rl, p->sname);
	else
		printf("%s (%s, %d): %s <", p->rpf, p->rif, p->rl, p->sname);
	tprint(p->type); printf("> 0%o\n", p->usage);

	/* Print each member, if any. */
	if (p->nmbrs) {
		m = p->mbrs;
		while (m) {
			printf("\t%s <", m->mname);
			tprint(m->type); printf("> %s\n", m->tagname);
			m = m->next;
		}
	}
}

/*
** Output a description of the type t.  This function must remain
** consistent with the ordering in pcc/m_ind/mfile1.h .  The same
** function exists in m_ind/treewalk.h .
*/
tprint(t)
	TPTR t;
{
	register PPTR p;
	TWORD bt;
	static char *tnames[NBTYPES] = {
		"null",
		"ellipsis",
		"farg",
		"moety",
		"signed",
		"undef",
		"void",
		"char",
		"schar",
		"short",
		"int",
		"long",
		"float",
		"double",
		"ldouble",
		"strty",
		"unionty",
		"enumty",
		"uchar",
		"ushort",
		"unsigned",
		"ulong"
	};

	for( ;; t = DECREF(t) ){

		if( ISCONST(t) ) printf( "const " );
		if( ISVOLATILE(t) ) printf( "volatile " );

		if( ISPTR(t) ) printf( "PTR " );
 		else if( ISFTN(t) ){
 			printf( "FTN (" );
			if( ( p = t->ftn_parm ) != PNIL ){
				for( ;; ){
					tprint( p->type );
					if( ( p = p->next ) == PNIL ) break;
					printf( ", " );
				}
 			}
			printf( ") " );
 		}
		else if( ISARY(t) ) printf( "ARY[%.0d] ", t->ary_size );
		else {
			if( ISTSIGNED(t) ) printf( "<signed> " );
			if( HASCONST(t) ) printf( "<HASCONST> " );
			if( HASVOLATILE(t) ) printf( "<HASVOLATILE> " );
			printf( tnames[bt = TOPTYPE(t)] );
			printf( "(0%o)", t->typ_size );
			return;
		}
	}
}
#endif

