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
static char rcsid[] = "@(#)$RCSfile: reader.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:53:55 $";
#endif
/*
 * COMPONENT_NAME: (CMDPROG) Programming Utilites
 *
 * FUNCTIONS: CloseFile, GetName, InHeader, InMembers, InSymbol, InType,
	      InUsage, OpenFile
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
 * reader.c	1.5  com/cmd/prog/lint/pass2,3.1,9013 9/12/89 11:52:04"; 
 */

#include "lint_msg.h"
#define         MSGSTR(Num, Str) catgets(catd, MS_LINT, Num, Str)
nl_catd catd;

#include <stdio.h>
#include "mfile1.h"
#include "lint2.h"

FILE *fp;			/* current file pointer */

#define BIO 1			/* binary i/o selected */

/*
** Header distinguishes file partitions from symbol data.
*/
InHeader()
{
	char iocode;

	/* Read file delimiter record. */
#ifdef	BIO
	if (fread((char *) &iocode, sizeof(char), 1, fp) < 1) {
#else
	if (fscanf(fp, "%d", &iocode) == EOF) {
#endif
		if (markerEOF)
			return ((int) LINTEOF);
		cerror(MSGSTR(M_MSG_265, "unexpected EOF for file %s"),
			curPFname);
	}
	
	/* Handle case where delimiter indicates a new file. */
	if (iocode == LINTBOF) {
		curPFname = StoreSName(GetName(GETMISC));
		/* Read for next record (assumed symbol). */
#ifdef	BIO
		fread((char *) &iocode, sizeof(char), 1, fp);
#else
		fscanf(fp, "%d", &iocode);
#endif
		markerEOF = 0;
	}
	return ((int) iocode);
}

/*
** Read characer string from intermediate file.
** Do name truncation if portability option enabled.
*/
char *
GetName(what)
	int what;
{
	static char buf[BUFSIZ];
	register char *cp = (char *) buf;
	register int c;

#ifdef	BIO
	while ((c = fgetc(fp)) != EOF) {
		*cp++ = c;
		if (c == '\0')
			break;
	}
#else
	fscanf(fp, "%s", cp);
#endif
	/* Six character name truncation to upper case. */
	if (pflag && (what == GETNAME)) {
		register char *ep = (char *) &buf[6];
		for (cp = (char *) buf; *cp && cp < ep; cp++)
			*cp = tolower(*cp);
		*cp = '\0';
	}
	return (buf);
}

/*
** Read function usage symbol record.
*/
InUsage()
{
	curSym->sname = (char *) sbuf;
	strcpy(curSym->sname, GetName(GETNAME));
#ifdef	BIO
	fread((char *) &curSym->usage, sizeof(short), 1, fp);
#else
	fscanf(fp, "%d", &curSym->usage);
#endif
}

/*
** Read symbol record.
*/
InSymbol()
{
	char *s;

	curSym->sname = (char *) sbuf;
	curIFname = (char *) ibuf;
	strcpy(curSym->sname, GetName(GETNAME));

	/* Minimize storing new include filenames. */
	s = GetName(GETMISC);
	if ((prevIFname != NULL) && !strcmp(prevIFname, s))
		curIFname = prevIFname;
	else
		prevIFname = curIFname = StoreSName(s);

#ifdef	BIO
	fread((char *) &curDLine, sizeof(short), 1, fp);
	fread((char *) &curRLine, sizeof(short), 1, fp);
	fread((char *) &curSym->usage, sizeof(short), 1, fp);
#else
	fscanf(fp, "%d", &curDLine);
	fscanf(fp, "%d", &curRLine);
	fscanf(fp, "%d", &curSym->usage);
#endif
	curSym->type = InType();

	/* Read symbol members, if any. */
	curSym->nmbrs = 0;
	curSym->mbrs = 0;
	if (curSym->usage & LINTMBR) {
#ifdef	BIO
		fread((char *) &curSym->nmbrs, sizeof(short), 1, fp);
#else
		fscanf(fp, "%d", &curSym->nmbrs);
#endif
		InMembers();
	}
}

/*
** Get each member of the current symbol.
*/
InMembers()
{
	register MBTAB *m;
	register int i;
	TWORD bt;

	/* Read and link each member. */
	for (i = 0; i < curSym->nmbrs; i++) {
		if (i == 0) {
			curSym->mbrs = m = MBMalloc();
		}
		else {
			m->next = MBMalloc();
			m = m->next;
		}

		m->mname = StoreMName(GetName(GETNAME));
		m->type = InType();
		m->tagname = 0;
		if ((bt = BTYPE(m->type)) == STRTY || bt == UNIONTY ||
			bt == ENUMTY) {
#ifdef	BIO
			short usage;

			fread((char *) &usage, sizeof(short), 1, fp);
#else
			fscanf(fp, "%d", &usage);
#endif
			if (usage & LINTTAG)
				m->tagname = StoreMName(GetName(GETNAME));
		}
		m->next = 0;
	}
}

/*
** Get the type of the current symbol.
*/
TPTR
InType()
{
	register TPTR t;
	register PPTR p;
	TPTR ot;
#ifdef	BIO
	struct tyinfo ty;
#else
	unsigned tnext, info, type, pnext;
#endif

#ifdef	BIO
	fread((char *) &ty, sizeof(struct tyinfo), 1, fp);
	if ((t = FindBType(ty.tword)) == TNIL)
		t = tynalloc(ty.tword);
#else
	fscanf(fp, "%o %o %o", &type, &tnext, &info);
	if ((t = FindBType(type)) == TNIL)
		t = tynalloc(type);
#endif
	ot = t;

#ifdef	BIO
	while (ty.next != TNIL) {
 		if (ISFTN(t)) {
			if (ty.ftn_parm != PNIL) {	/* is a PPTR */
#else
	while (tnext) {
 		if (ISFTN(t)) {
			if (info) {	/* is a PPTR */
#endif
				t->ftn_parm = p = parmalloc();
				do {
#ifdef	BIO
					fread((char *) p,
						sizeof(struct parminfo),1,fp);
					p->type = InType();
					if (p->next == PNIL)
#else
					fscanf(fp, "%o %o", &type, &pnext);
					p->type = InType();
					if (!pnext)
#endif
						break;
					p->next = parmalloc();
				} while (p = p->next);
			}
 		}
		else if (ISARY(t))
#ifdef	BIO
			t->ary_size = ty.ary_size;
#else
			t->ary_size = info;
#endif
#ifdef	BIO
		fread((char *) &ty, sizeof(struct tyinfo), 1, fp);
		if ((t->next = FindBType(ty.tword)) == TNIL)
			t->next = tynalloc(ty.tword);
#else
		fscanf(fp, "%o %o %o", &type, &tnext, &info);
		if ((t->next = FindBType(type)) == TNIL)
			t->next = tynalloc(type);
#endif
		t = t->next;
	}
	return (ot);
}

/*
** Simple file open/close functions.
*/
OpenFile()
{
	if ((fp = fopen(fname, "r")) == NULL) {
		cerror(MSGSTR(M_MSG_266, "can't open file %s\n"), fname);
		exit(1);
	}
	markerEOF = 0;
}

CloseFile()
{
	fclose(fp);
}
