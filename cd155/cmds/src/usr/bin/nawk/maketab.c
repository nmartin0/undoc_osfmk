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
 * $Log: maketab.c,v $
 * Revision 1.2  1994/11/19  01:31:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/18  14:47:05  shala
 * Files for new version of awk command.
 *
 * Revision 1.4  90/12/06  14:10:13  devrcs
 * 	Fixed up comment headers.
 * 	[90/12/03  13:31:47  gm]
 * 
 * Revision 1.3  90/09/13  12:11:35  devrcs
 * 	Cleanup, RCS header, NLS, MSG
 * 	[90/08/29  09:59:28  knight]
 * 
 * Revision 1.2  90/07/05  23:25:56  devrcs
 * 	Populate from AIX sources
 * 	[90/06/29  21:40:51  lehotsky]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: maketab.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:31:32 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) maketab.c
 *
 * FUNCTIONS: main
 *
 * ORIGINS: 3, 10, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984, 19885, 1986, 1987 AT&T
 * 	All Rights Reserved
 * 
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.
 * 
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 *
 * maketab.c	1.3  com/cmd/edit/awk,3.1,9021 10/6/89 15:57:32
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

struct xx
{	int token;
	char *name;
	char *pname;
} proc[] = {
	{ PROGRAM, "program", NULL },
	{ BOR, "boolop", " || " },
	{ AND, "boolop", " && " },
	{ NOT, "boolop", " !" },
	{ NE, "relop", " != " },
	{ EQ, "relop", " == " },
	{ LE, "relop", " <= " },
	{ LT, "relop", " < " },
	{ GE, "relop", " >= " },
	{ GT, "relop", " > " },
	{ ARRAY, "array", NULL },
	{ INDIRECT, "indirect", "$(" },
	{ SUBSTR, "substr", "substr" },
	{ SUB, "sub", "sub" },
	{ GSUB, "gsub", "gsub" },
	{ INDEX, "sindex", "sindex" },
	{ SPRINTF, "asprintf", "sprintf " },
	{ ADD, "arith", " + " },
	{ MINUS, "arith", " - " },
	{ MULT, "arith", " * " },
	{ DIVIDE, "arith", " / " },
	{ MOD, "arith", " % " },
	{ UMINUS, "arith", " -" },
	{ POWER, "arith", " **" },
	{ PREINCR, "incrdecr", "++" },
	{ POSTINCR, "incrdecr", "++" },
	{ PREDECR, "incrdecr", "--" },
	{ POSTDECR, "incrdecr", "--" },
	{ CAT, "cat", " " },
	{ PASTAT, "pastat", NULL },
	{ PASTAT2, "dopa2", NULL },
	{ MATCH, "matchop", " ~ " },
	{ NOTMATCH, "matchop", " !~ " },
	{ MATCHFCN, "matchop", "matchop" },
	{ INTEST, "intest", "intest" },
	{ PRINTF, "aprintf", "printf" },
	{ PRINT, "print", "print" },
	{ CLOSE, "closefile", "closefile" },
	{ DELETE, "delete", "delete" },
	{ SPLIT, "split", "split" },
	{ ASSIGN, "assign", " = " },
	{ ADDEQ, "assign", " += " },
	{ SUBEQ, "assign", " -= " },
	{ MULTEQ, "assign", " *= " },
	{ DIVEQ, "assign", " /= " },
	{ MODEQ, "assign", " %= " },
	{ POWEQ, "assign", " ^= " },
	{ CONDEXPR, "condexpr", " ?: " },
	{ IF, "ifstat", "if(" },
	{ WHILE, "whilestat", "while(" },
	{ FOR, "forstat", "for(" },
	{ DO, "dostat", "do" },
	{ IN, "instat", "instat" },
	{ NEXT, "jump", "next" },
	{ EXIT, "jump", "exit" },
	{ BREAK, "jump", "break" },
	{ CONTINUE, "jump", "continue" },
	{ RETURN, "jump", "ret" },
	{ BLTIN, "bltin", "bltin" },
	{ CALL, "call", "call" },
	{ ARG, "arg", "arg" },
	{ VARNF, "getnf", "NF" },
	{ GETLINE, "getline", "getline" },
	{ 0, "", "" }
};

#define SIZE	LASTTOKEN - FIRSTTOKEN + 1
char *table[SIZE];
char *names[SIZE];

main()
{
	struct xx *p;
	int i, tok;
	char c;
	FILE *fp;
	char buf[100], name[100], def[100];

	printf("#include \"awk.h\"\n");
	printf("#include \"awk.g.h\"\n\n");
	printf("Cell *nullproc();\n");
	for (i = SIZE; --i >= 0; )
		names[i] = "";
	for (p=proc; p->token!=0; p++)
		if (p == proc || strcmp(p->name, (p-1)->name))
			printf("extern Cell *%s();\n", p->name);

	if ((fp = fopen("awk.g.h", "r")) == NULL) {
		fprintf(stderr, "maketab can't open awk.g.h!");
		exit(1);
	}
	printf("static uuchar *printname[%d] = {\n", SIZE);
	i = 0;
	while (fgets(buf, sizeof buf, fp) != NULL) {
		sscanf(buf, "%1c %s %s %d", &c, def, name, &tok);
		if (c != '#')	/* not #define */
			continue;
		if (tok < FIRSTTOKEN || tok > LASTTOKEN) {
			fprintf(stderr, "maketab funny token %d %s\n", tok, buf);
			exit(1);
		}
		names[tok-FIRSTTOKEN] = (char *) malloc(strlen(name)+1);
		strcpy(names[tok-FIRSTTOKEN], name);
		
		printf("\t%c(uuchar *) \"%s\"\t/* %d */\n", ((i==0)?' ':','), name, tok);
		i++;
	}
	printf("};\n\n");

	for (p=proc; p->token!=0; p++)
		table[p->token-FIRSTTOKEN] = p->name;
	printf("\nCell *(*proctab[%d])() = {\n", SIZE);
	for (i=0; i<SIZE; i++) {
	  	char c = (i==0)?' ':',';

		if (table[i]==0)
			printf("\t%cnullproc\t/* %s */\n", c, names[i]);
		else
			printf("\t%c%s\t/* %s */\n", c, table[i], names[i]);
	}
	printf("};\n\n");

	printf("uuchar *tokname(n)\n");	/* print a tokname() function */
	printf("{\n");
	printf("	static char buf[100];\n\n");
	printf("	if (n < FIRSTTOKEN || n > LASTTOKEN) {\n");
	printf("		sprintf(buf, \"token %%d\", n);\n");
	printf("		return (uuchar *)buf;\n");
	printf("	}\n");
	printf("	return printname[n-257];\n");
	printf("}\n");
	exit(0);
}
