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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: parse.c,v $
 * Revision 1.2  1994/11/19  01:31:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/18  14:47:11  shala
 * Files for new version of awk command.
 *
 * Revision 2.5  90/12/06  14:10:15  devrcs
 * 	Fixed up comment headers.
 * 	[90/12/03  13:31:54  gm]
 * 
 * Revision 2.4  90/09/13  12:11:38  devrcs
 * 	Cleanup, RCS header, NLS, MSG
 * 	[90/08/29  09:59:51  knight]
 * 
 * Revision 2.3  90/07/05  23:25:59  devrcs
 * 	Use AIX source
 * 	[90/06/29  21:49:55  lehotsky]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: parse.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:31:33 $";
#endif
/*
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * parse.c	1.3  com/cmd/edit/awk,3.1,9021 8/21/89 14:10:41
 */ 

/*
Copyright (c) 1984, 19885, 1986, 1987 AT&T
	All Rights Reserved

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.

The copyright notice above does not evidence any
actual or intended publication of such source code.
*/

#include <stdio.h>
#include "awk.h"
#include "awk.g.h"


Node *nodealloc(n)
{
	register Node *x;
	x = (Node *) Malloc(sizeof(Node) + (n-1)*sizeof(Node *));
	if (x == NULL)
		ERROR MSGSTR(PNODEALLOC, "out of space in nodealloc") FATAL;
	x->nnext = NULL;
	x->lineno = lineno;
	return(x);
}

Node *exptostat(a) Node *a;
{
	a->ntype = NSTAT;
	return(a);
}

Node *node0(a)
{
	register Node *x;
	x = nodealloc(0);
	x->nobj = a;
	return(x);
}

Node *node1(a,b) Node *b;
{
	register Node *x;
	x = nodealloc(1);
	x->nobj = a;
	x->narg[0]=b;
	return(x);
}

Node *node2(a,b,c) Node *b, *c;
{
	register Node *x;
	x = nodealloc(2);
	x->nobj = a;
	x->narg[0] = b;
	x->narg[1] = c;
	return(x);
}

Node *node3(a,b,c,d) Node *b, *c, *d;
{
	register Node *x;
	x = nodealloc(3);
	x->nobj = a;
	x->narg[0] = b;
	x->narg[1] = c;
	x->narg[2] = d;
	return(x);
}

Node *node4(a,b,c,d,e) Node *b, *c, *d, *e;
{
	register Node *x;
	x = nodealloc(4);
	x->nobj = a;
	x->narg[0] = b;
	x->narg[1] = c;
	x->narg[2] = d;
	x->narg[3] = e;
	return(x);
}

Node *stat3(a,b,c,d) Node *b, *c, *d;
{
	register Node *x;
	x = node3(a,b,c,d);
	x->ntype = NSTAT;
	return(x);
}

Node *op2(a,b,c) Node *b, *c;
{
	register Node *x;
	x = node2(a,b,c);
	x->ntype = NEXPR;
	return(x);
}

Node *op1(a,b) Node *b;
{
	register Node *x;
	x = node1(a,b);
	x->ntype = NEXPR;
	return(x);
}

Node *stat1(a,b) Node *b;
{
	register Node *x;
	x = node1(a,b);
	x->ntype = NSTAT;
	return(x);
}

Node *op3(a,b,c,d) Node *b, *c, *d;
{
	register Node *x;
	x = node3(a,b,c,d);
	x->ntype = NEXPR;
	return(x);
}

Node *op4(a,b,c,d,e) Node *b, *c, *d, *e;
{
	register Node *x;
	x = node4(a,b,c,d,e);
	x->ntype = NEXPR;
	return(x);
}

Node *stat2(a,b,c) Node *b, *c;
{
	register Node *x;
	x = node2(a,b,c);
	x->ntype = NSTAT;
	return(x);
}

Node *stat4(a,b,c,d,e) Node *b, *c, *d, *e;
{
	register Node *x;
	x = node4(a,b,c,d,e);
	x->ntype = NSTAT;
	return(x);
}

Node *valtonode(a, b) Cell *a;
{
	register Node *x;

	a->ctype = OCELL;
	a->csub = b;
	x = node1(0, a);
	x->ntype = b == CFLD ? NFIELD : NVALUE;
	return(x);
}

Node *rectonode()
{
	/* return valtonode(lookup("$0", symtab), CFLD); */
	return valtonode(recloc, CFLD);
}

Node *makearr(p) Node *p;
{
	Cell *cp;

	if (isvalue(p)) {
		cp = (Cell *) (p->narg[0]);
		if (isfunc(cp))
			ERROR MSGSTR(PFUNCNOTARR, "%s is a function, not an array"), cp->nval SYNTAX;
		else if (!isarr(cp)) {
			xfree(cp->sval);
			cp->sval = (uuchar *) makesymtab(NSYMTAB);
			cp->tval = ARR;
		}
	}
	return p;
}

Node *pa2stat(a,b,c) Node *a, *b, *c;
{
	register Node *x;
	x = node4(PASTAT2, a, b, c, (Node *) paircnt);
	paircnt++;
	x->ntype = NSTAT;
	return(x);
}

Node *linkum(a,b) Node *a, *b;
{
	register Node *c;

	if (errorflag)	/* don't link things that are wrong */
		return a;
	if (a == NULL) return(b);
	else if (b == NULL) return(a);
	for (c = a; c->nnext != NULL; c = c->nnext)
		;
	c->nnext = b;
	return(a);
}

defn(v, vl, st)	/* turn on FCN bit in definition */
	Cell *v;
	Node *st, *vl;	/* body of function, arglist */
{
	Node *p;
	int n;

	if (isarr(v)) {
		ERROR MSGSTR(PARRANDFUNC, "`%s' is an array name and a function name"), v->nval SYNTAX;
		return;
	}
	v->tval = FCN;
	v->sval = (uuchar *) st;
	n = 0;	/* count arguments */
	for (p = vl; p; p = p->nnext)
		n++;
	v->fval = n;
	dprintf("defining func %s (%d args)\n", v->nval, n);
}

isarg(s)	/* is s in argument list for current function? */
	uuchar *s;
{
	extern Node *arglist;
	Node *p = arglist;
	int n;

	for (n = 0; p != 0; p = p->nnext, n++)
		if (strcmp(((Cell *)(p->narg[0]))->nval, s) == 0)
			return n;
	return -1;
}
