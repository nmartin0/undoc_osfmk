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
static char rcsid[] = "@(#)$RCSfile: expr.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:40 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 *
 * expr.c  1.7  com/cmd/sh,3.1,9021 1/16/90 14:01:52";
 */

/*
 * NAME:	expr
 *
 * FUNCTION:	expr - evaluate arguments as an expression
 *
 * SYNOPSIS:	expr expression ...
 *
 */

/*
 * operator constants, most self-explanatory
 */
#define	A_STRING	258
#define	NOARG		259
#define	OR		260
#define	AND		261
#define	EQ		262
#define	LT		263
#define	GT		264
#define	GEQ		265
#define	LEQ		266
#define	NEQ		267
#define	ADD		268
#define	SUBT		269
#define	MULT		270
#define	DIV		271
#define	REM		272
#define	MCH		273
#define	MATCH		274
#define	SUBSTR		275
#define	LENGTH		276
#define	INDEX		277

#define	ESIZE		(256*5)		/* expression buffer size	*/
#define	EQL(x,y)	(strcmp(x, y) == 0)	/* string compare	*/

/*
 * macros for regexp
 */
#define	INIT		register char *sp = instring;
#define	GETC()		((unsigned char)*sp++)
#define	PEEKC()		((unsigned char)*sp)
#define	UNGETC(c)	(--sp)
#define	RETURN(c)	return
#define	ERROR(c)	errxx()
void errxx();

/*
 * regexp pattern for numbers...
 */
#define	NUM_PATTERN	"-\\{0,1\\}[0-9]*$"

/*
 * includes
 */
#include	<stdio.h>	/* for NULL	*/
#include	<string.h>
#include	<stdlib.h>
#include	<locale.h>

# include  "expr_msg.h"
nl_catd	catd;
# define   MSGSTR(Num, Str) catgets(catd, MS_EXPR, Num, Str)

/*
 * regexp includes
 */
#include	<NLchar.h>
#include	<NLregexp.h>

/*
 * local functions
 */
char *ltoa(), *rel(), *strsave(), *arith(), *conj();
char *substr(), *length(), *expr_index(), *match();
int yylex(), ematch();
void yyerror();

/*
 * globals
 */
char	**Av;			/* global 'argv'		*/
int	Ac;			/* global 'argc'		*/
int	Argi;			/* index of current argv	*/
int	paren;			/* found a paren?		*/

char Mstring[128];		/* string matched within ()	*/

/*
 * operators
 */
char *operator[] = {
	"|", "&", "+", "-", "*", "/", "%", ":",
	"=", "==", "<", "<=", ">", ">=", "!=",
	"match", "substr", "length", "index", "\0" };

/*
 * operator values
 */
int op[] = { 
	OR, AND, ADD,  SUBT, MULT, DIV, REM, MCH,
	EQ, EQ, LT, LEQ, GT, GEQ, NEQ,
	MATCH, SUBSTR, LENGTH, INDEX };

/*
 * operator precedence table
 */
int pri[] = { 1,2,3,3,3,3,3,3,4,4,5,5,5,6,7,8,9,9,9 };

/*
 * NAME:	yylex
 *
 * FUNCTION:	yylex - determine next command line token type
 *
 * NOTES:	Yylex looks at the next command line token and returns
 *		a value indicating its type.
 *
 * RETURN VALUE DESCRIPTION:	NOARG if no more command line args exist,
 *		an operator constant if the token is an operator, '(' or ')'
 *		if the token is a parenthesis, else A_STRING indicating
 *		an operand.
 */

int
yylex()
{
	register char *p;
	register int i;

	/*
	 * any args left?
	 */
	if(Argi >= Ac)
		return NOARG;

	p = Av[Argi];

	/*
	 * paren?
	 */
	if((*p == '(' || *p == ')') && p[1] == '\0' )
		return (int)*p;

	/*
	 * operator?
	 */
	for(i = 0; *operator[i]; ++i)
		if(EQL(operator[i], p))
			return op[i];

	/*
	 * must be an operand
	 */

	return A_STRING;
}

/*
 * NAME:	rel
 *
 * FUNCTION:	rel - evaluate relational operators (=, !=, >, <, <=, >=)
 *
 * NOTES:	Rel evaluates a relational operator and 2 operands.
 *		If the operands are numerical, they are compared
 *		mathematically, else they are compared as strings
 *		using strcmp().
 *
 * RETURN VALUE DESCRIPTION: "1" if the expression evaluates to true, else "0"
 */

char *
rel(oper, r1, r2)
int oper;			/* operator	*/
register char *r1; 		/* operand 1	*/
register char *r2; 		/* operand 2	*/
{
	register long i;

	/*
	 * compare numerically or as strings?
	 */
	if(ematch(r1, NUM_PATTERN) && ematch(r2, NUM_PATTERN))
		i = atol(r1) - atol(r2);
	else
		i = (long) strcmp(r1, r2);

	/*
	 * evaluate
	 */
	switch(oper) {
	case EQ: 		/* equal			*/
		i = i==0; 
		break;
	case GT: 		/* greater than			*/
		i = i>0; 
		break;
	case GEQ: 		/* greater than or equal	*/
		i = i>=0; 
		break;
	case LT: 		/* less than			*/
		i = i<0; 
		break;
	case LEQ: 		/* less than or equal		*/
		i = i<=0; 
		break;
	case NEQ: 		/* not equal			*/
		i = i!=0; 
		break;
	}

	return i ? "1": "0";
}

/*
 * NAME:	arith
 *
 * FUNCTION:	arith - evaluate an arithmetic operator
 *
 * NOTES:	Arith evaluates an arithmetic operator (+, -, *, /, %).
 *		Both operands must be numbric.
 *
 * RETURN VALUE DESCRIPTION: string containing the answer
 */

char *
arith(oper, r1, r2)
int oper;		/* operator	*/
char *r1;		/* operand 1	*/
char *r2; 		/* operand 2	*/
{
	register long i1, i2;

	/*
	 * make sure both operands are numeric
	 */
	if(!(ematch(r1, NUM_PATTERN) && ematch(r2, NUM_PATTERN)))
		yyerror(MSGSTR(NANARG,"non-numeric argument"));

	/*
	 * convert
	 */
	i1 = atol(r1);
	i2 = atol(r2);

	/*
	 * evaluate
	 */
	switch(oper) {
	case ADD: 			/* add		*/
		i1 = i1 + i2; 
		break;
	case SUBT: 			/* subtract	*/
		i1 = i1 - i2; 
		break;
	case MULT: 			/* multiply	*/
		i1 = i1 * i2; 
		break;
	case DIV: 			/* divide	*/
		if(i2 == 0)
		  yyerror(MSGSTR(INVALID, "invalid expression"));
		else
		  i1 = i1 / i2; 
		break;
	case REM: 			/* remainder	*/
		if(i2 == 0)
		  yyerror(MSGSTR(INVALID, "invalid expression"));
		else
		  i1 = i1 % i2; 
		break;
	}

	/*
	 * convert back to ascii and return
	 */
	return (strsave(ltoa(i1)));
}

/*
 * NAME:	conj
 *
 * FUNCTION:	conj - handle and/or operators
 *
 * NOTES:	Conj evaluates and/or operators (&,|).
 *
 * RETURN VALUE DESCRIPTION: result of evaluation
 */

char *
conj(oper, r1, r2)
int oper;
char *r1;
char *r2; 
{
	register char *rv;

	switch(oper) {

	case OR:		/* or (|)	*/
		/*
		 * return r1 if it's not 0 or NULL, else
		 * return r2 if it's not 0 or NULL, else
		 * return "0"
		 */
		if(EQL(r1, "0") || EQL(r1, ""))
			if(EQL(r2, "0") || EQL(r2, ""))
				rv = "0";
			else
				rv = r2;
		else
			rv = r1;
		break;
	case AND:		/* and (&)	*/
		/*
		 * return "0" if either r1 or r2 is 0 or null, else
		 * return r1
		 */
		if(EQL(r1, "0") || EQL(r1, "") ||
		   EQL(r2, "0") || EQL(r2, ""))
			rv = "0";
		else
			rv = r1;
		break;
	}

	return rv;
}

/*
 * NAME:	substr
 *
 * FUNCTION:	substr - implement substring operation
 *
 * NOTES:	Substr implements the substring operation.  It
 *		requires 3 operators:  a string, a starting index,
 *		and a width.  Example:    expr substr "Hi Mom" 1 3
 *		prints "Hi ".
 *
 * RETURN VALUE DESCRIPTION:	the substring
 */

char *
substr(v, s, w)
char *v;		/* string		*/
char *s;		/* starting index	*/
char *w; 		/* width		*/
{
	register long si, wi;
	register char *res, *rv;

	/*
	 * convert start and width to numbers
	 */
	si = atol(s);
	wi = atol(w);

	/*
	 * find starting location using start
	 */
	while(--si)
		if(*v)
			++v;

	/*
	 * and save it
	 */
	res = v;

	/*
	 * find ending location using width
	 */
	while(wi--)
		if(*v)
			++v;

	/*
	 * allocate space and return value
	 */
	if ((rv = malloc((size_t) (v - res + 1))) == NULL)
		yyerror(MSGSTR(MALLOC, "malloc error"));

	(void) strncpy(rv, res, (size_t)(v - res));
	rv[v - res] = 0;

	return rv;
}

/*
 * NAME:	length
 *
 * FUNCTION:	length - compute string length of a string
 *
 * NOTES:	Length computes the string length of a string and returns
 *		it.
 *
 * RETURN VALUE DESCRIPTION: string containing string length of input string
 */

char *
length(s)
char *s; 	/* input string	*/
{
	/*
	 * compute length, convert, malloc and return
	 */
	return (strsave(ltoa((long) strlen(s))));
}

/*
 * NAME:	expr_index
 *
 * FUNCTION:	expr_index - find part of one string in another
 *
 * NOTES:	Index looks in one string for any character
 *		that matches the other.
 *
 * RETURN VALUE DESCRIPTION: if found, a string containing the index of the
 *		char found in the first string, else "0"
 */

char *
expr_index(s, t)
char *s;		/* string to look at	*/
char *t; 		/* string to look for	*/
{
	register int i, j;

	for(i = 0; s[i] ; ++i)
		for(j = 0; t[j] ; ++j)
			if(s[i]==t[j])
				return (strsave(ltoa((long) ++i)));

	return "0";
}

/*
 * NAME:	match
 *
 * FUNCTION:	match - implement match ("match" or ":") function
 *
 * NOTES:	Match implements the 'match' function.  This function
 *		is called by using either of the match operators.  E.g.
 *		"expr match string expr" or "expr string : expr".
 *
 * RETURN VALUE DESCRIPTION:	either the number of chars matched, or
 *		a portion of 'string' (if "\(" and "\)" are used in expr)
 */

char *
match(s, p)
char *s;
char *p;
{
	int ematch_val;
	register char *rv;

	/*
	 * call ematch to do the actual function
	 */
	ematch_val = ematch(s, p);

	/*
	 * if there were parens, get the matching string (nbra is defined
	 * and set in regexp.h)...
	 */
	if(nbra)
		rv = strsave(Mstring);
	/*
	 * else just get the number of chars matched ...
	 */
	else
		rv = strsave(ltoa((long) ematch_val));

	return rv;
}

/*
 * NAME:	ematch
 *
 * FUNCTION:	ematch - do the actual expression match
 *
 * NOTES:	Ematch does the actual matching of expression and
 *		string, using the regexp() package.
 *
 * DATA STRUCTURES:	Mstring is filled in with the matching expression
 *		within parens.
 *
 * RETURN VALUE DESCRIPTION:	number of chars matched
 */

int
ematch(s, p)
char *s;
register char *p;
{
	register int num;
	static char expbuf[ESIZE];

	/*
	 * compile the re ...
	 */
	(void) compile(p, expbuf,
			&expbuf[ESIZE], 0);

	/*
	 * make sure there weren't too many parens
	 */
	if(nbra > 1)
		yyerror(MSGSTR(PARANS1,"Too many '\\('s"));

	/*
	 * do the match
	 */
	if(advance(s, expbuf)) {
		/*
		 * any parens?
		 */
		if(nbra == 1) {
			/*
			 * get starting and ending locations of parens
			 * (braslist and braelist are set/used in
			 * regexp.h)
			 */
			p = (char *) braslist[0];
			num = braelist[0] - p;

			/*
			 * be sure we can fit it into Mstring
			 */
			if (num >= sizeof(Mstring) || num < 0) 
				yyerror(MSGSTR(PARANS2,"Paren problem"));

			/*
			 * copy it into Mstring
			 */
			(void) strncpy(Mstring, p, (size_t)num);
			Mstring[num] = '\0';
		}

		/*
		 * return number of chars matched
		 */
		return(loc2 - s);
	}

	/*
	 * no match
	 */
	return(0);
}

/*
 * NAME:	errxx
 *
 * FUNCTION:	errxx - error function for regexp
 *
 * NOTES:	Errxx prints a regular expression error for regexp.
 *
 * RETURN VALUE DESCRIPTION:	none
 */

void
errxx()
{
	yyerror(MSGSTR(RE_ERROR, "RE error"));
}

/*
 * NAME:	yyerror
 *
 * FUNCTION:	yyerror - print an error and exit
 *
 * NOTES:	Yyerror prints an error and exits with value of 2.
 *
 * RETURN VALUE DESCRIPTION:	none
 */

void
yyerror(s)
char *s;
{
#define	WRS(string)	(void) write(2, string, (unsigned) strlen(string))

	WRS("expr: ");
	WRS(s);
	WRS("\n");

	exit(2);

	/* NOTREACHED */
}

/*
 * NAME:	ltoa
 *
 * FUNCTION:	ltoa - long to ascii
 *
 * NOTES:	Ltoa formats a long integer into a string and returns
 *		it.  The return value is static.
 *
 * RETURN VALUE DESCRIPTION:	the formatted string
 */

char *
ltoa(l)
long l;
{
	static char str[20];	/* must be big enuf to hold a long... */

	(void) sprintf(str, "%ld", l);

	return str;
}

/*
 * NAME:	expres
 *
 * FUNCTION:	expres
 *
 * NOTES:
 *
 * RETURN VALUE DESCRIPTION:
 */

char *
expres(prior, par)
int prior;
int par; 
{
	static int noarg = 0;
	int ylex, temp, op1;
	char *r1, *ra, *rb, *rc;

	/*
	 * retrieve next token and check validity
	 */
	if ((ylex = yylex()) >= NOARG && ylex < MATCH)
		yyerror(MSGSTR(SYNTAX, "syntax error"));

	/*
	 * string?
	 */
	if (ylex == A_STRING) {
		r1 = Av[Argi++];
		temp = Argi;
	}

	/*
	 * left paren?
	 */
	else if (ylex == '(') {
		paren++;
		Argi++;
		/*
		 * get embedded expression
		 */
		r1 = expres(0, Argi);
		Argi--;
	}

lop:
	if ((ylex = yylex()) > NOARG && ylex < MATCH) {
		op1 = ylex;
		Argi++;

		if (pri[op1-OR] <= prior ) 
			return r1;

		else {
			switch(op1) {
			case OR:
			case AND:
				r1 = conj(op1,r1,expres(pri[op1-OR],0));
				break;
			case EQ:
			case LT:
			case GT:
			case LEQ:
			case GEQ:
			case NEQ:
				r1=rel(op1,r1,expres(pri[op1-OR],0));
				break;
			case ADD:
			case SUBT:
			case MULT:
			case DIV:
			case REM:
				r1=arith(op1,r1,expres(pri[op1-OR],0));
				break;
			case MCH:
				r1=match(r1,expres(pri[op1-OR],0));
				break;
			}
			if(noarg == 1) {
				return r1;
			}
			Argi--;
			goto lop;
		}
	}

	/*
	 * right paren?
	 */
	if((ylex = yylex()) == ')') {
		if(par == Argi)
			yyerror(MSGSTR(SYNTAX,"syntax error"));

		if(par != 0) {
			paren--;
			Argi++;
		}

		Argi++;
		return r1;
	}

	if((ylex = yylex()) > MCH && ylex <= INDEX) {
		if (Argi == temp)
			return r1;

		op1 = ylex;
		Argi++;

		switch(op1) {
		case SUBSTR: 
			rc = expres(pri[op1-OR],0);
		case MATCH:
		case INDEX: 
			rb = expres(pri[op1-OR],0);
		case LENGTH: 
			ra = expres(pri[op1-OR],0);
		}

		switch(op1) {
		case MATCH: 
			r1 = match(rb,ra); 
			break;
		case INDEX: 
			r1 = expr_index(rb,ra); 
			break;
		case SUBSTR: 
			r1 = substr(rc,rb,ra); 
			break;
		case LENGTH: 
			r1 = length(ra); 
			break;
		}

		if(noarg == 1)
			return r1;

		Argi--;
		goto lop;
	}

	if ((ylex = yylex()) == NOARG)
		noarg = 1;

	return r1;
}

/*
 * NAME:	strsave
 *
 * FUNCTION:	strsave - save a string somewhere
 *
 * NOTES:	Strsave allocates memory for a string.  The new memory
 *		is returned.  Inspired by K&R, pg 103.
 *
 * RETURN VALUE DESCRIPTION:	NULL if no memory could be allocated,
 *		else a pointer to the memory containing the new string.
 */

char *
strsave(string)
char *string;
{
	register char *new;

	if ((new = (char *) malloc((size_t) (strlen(string) + 1))) == NULL)
		yyerror(MSGSTR(MALLOC, "malloc error"));

	return (strcpy(new, string));
}

/*
 * NAME:	main
 *
 * FUNCTION:	main - main routine
 *
 * NOTES:	Main inits globals, calls expres() to evaluate the
 *		command line, prints out the value, and exits with
 *		either a true (0) or false (1) value.
 *
 * RETURN VALUE DESCRIPTION:	0 if the expression is true, else 1
 */

int
main(argc, argv)
int argc;
char **argv; 
{
	char	*buf;

	(void) setlocale (LC_ALL, "");

	/*
	 * initialization
	 */

#ifdef MSG
	catd = catopen(MF_EXPR, 0);
#endif

	Ac = argc;
	Argi = 1;
	paren = 0;
	Av = argv;

	/* 
	 * evaluate expression on the command line
	 */
	buf = expres(0, 1);

	/*
	 * check syntax error
	 */
	if(Ac != Argi || paren != 0) {
		yyerror(MSGSTR(SYNTAX,"syntax error"));
	}

	/*
	 * write result
	 */
	(void) write(1, buf, (unsigned) strlen(buf));
	(void) write(1, "\n", 1);

	/*
	 * exit true or false depending on result
	 */
	exit(EQL(buf, "0") || buf[0] == '\0' ? 1 : 0);
	/* NOTREACHED */
}
