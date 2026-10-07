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
/*
 * COMPONENT_NAME: (CMDCALC) calculators
 *
 * FUNCTIONS: bc 
 *
 * ORIGINS: 3 26 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1990
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * bc.y   1.12  com/cmd/calc,3.1,9021 2/2/90 17:00:02";
 */

/*
 *
 *  NAME: bc [-c][-l][file ...]
 *                                                                     
 *  FUNCTION:  Provides an interpreter for a arbitrary-precision arithmetic
 *	      language.
 * 	OPTIONS:
 *	-c    Compiles file, but does not invoke dc.
 *       -l    includes a library of math functions.
 *
 *  COMMENTS:  This is a yacc file.  It should first be given to the
 *	      yacc command, which will produce an LR(1) parsing
 *	      program called y.tab.c.  This file must then be compiled
 *	      with the cc command.
 *             The yacc grammer file is divided into three sections,
 *	      each of which is delimited by %%.  These sections are:
 *		   1)  Declarations:  Defines the variables which are
 *		       used in the rules. 
 *		   2)  rules:  Contains strings and expressions to be
 *		       matched in the file to yylex(provided in this
 *		       program), and C commands to execute when a match
 *		       is made. 
 *		   3)  Programs:  Allows the user to define his/her 
 *                      own subroutines.
 *
 */

/*  "%{" and "%}" are used to enclose global variables. */
%{
	static int *getout();
	static int bundle( int, ... );
%}

/*  The following declarations give the associativity and precedence of
 *  the operators.  The later the operators appear, the higher precedence
 *  they have.  For example, "+" is left-associative and is of lower 
 *  precedence than "*". */ 
%right '='
%left '+' '-'
%left '*' '/' '%'
%right '^'
%left UMINUS

%term LETTER DIGIT SQRT LENGTH _IF  FFF EQ
%term _WHILE _FOR NE LE GE INCR DECR
%term _RETURN _BREAK _DEFINE BASE OBASE SCALE
%term EQPL EQMI EQMUL EQDIV EQREM EQEXP
%term _AUTO DOT
%term QSTR

%{
#include <stdarg.h>
#include <stdio.h>

#include <locale.h>
#include <nl_types.h>
nl_catd catd;                   /* global catalog descriptor */
#include "bc_msg.h"
#define MSGSTR(C,D)             catgets(catd, MS_BC, C, D)
FILE *in;
char cary[1000], *cp = { cary };
char string[1000], *str = {string};
int crs = '0';
int rcrs = '0';  /* reset crs */
int bindx = 0;
int lev = 0;
int ln;
char *ss;
int bstack[10] = { 0 };
char *numb[15] = {
  " 0", " 1", " 2", " 3", " 4", " 5",
  " 6", " 7", " 8", " 9", " 10", " 11",
  " 12", " 13", " 14" };
int *pre, *post;
%}

/*  The Rules section. */
/*  This section is made up of grammer rules and actions.  The actions are
 *  performed each time the parser recognizes the rule in the input 
 *  stream.  For example, in the first rule below, start is a non-terminal
 *  name and "start stat tail" are rules.  If one of these rules are
 *  recognized, then the action "output( $2 )" is performed, where "$2"
 *  represents the value returned by the second rule("stat"). */
%%
start	: 
	|  start stat tail
		= output( $2 );
	|  start def dargs ')' '{' dlist slist '}'
		={	bundle( 6,pre, $7, post ,"0",numb[lev],"Q");
			conout( $$, $2 );
			rcrs = crs;
			output( "" );
			lev = bindx = 0;
			}
	;

dlist	:  tail
	| dlist _AUTO dlets tail
	;

stat	:  e 
		={ bundle(2, $1, "ps." ); }
	| 
		={ bundle(1, "" ); }
	|  QSTR
		={ bundle(3,"[",$1,"]P");}
	|  LETTER '=' e
		={ bundle(3, $3, "s", $1 ); }
	|  LETTER '[' e ']' '=' e
		={ bundle(4, $6, $3, ":", geta($1)); }
	|  LETTER EQOP e
		={ bundle(6, "l", $1, $3, $2, "s", $1 ); }
	|  LETTER '[' e ']' EQOP e
		={ bundle(8,$3, ";", geta($1), $6, $5, $3, ":", geta($1));}
	|  _BREAK
		={ bundle(2, numb[lev-bstack[bindx-1]], "Q" ); }
	|  _RETURN '(' e ')'
		= bundle(4, $3, post, numb[lev], "Q" );
	|  _RETURN '(' ')'
		= bundle(4, "0", post, numb[lev], "Q" );
	| _RETURN
		= bundle(4,"0",post,numb[lev],"Q");
	| SCALE '=' e
		= bundle(2, $3, "k");
	| SCALE EQOP e
		= bundle(4,"K",$3,$2,"k");
	| BASE '=' e
		= bundle(2,$3, "i");
	| BASE EQOP e
		= bundle(4,"I",$3,$2,"i");
	| OBASE '=' e
		= bundle(2,$3,"o");
	| OBASE EQOP e
		= bundle(4,"O",$3,$2,"o");
	|  '{' slist '}'
		={ $$ = $2; }
	|  FFF
		={ bundle(1,"fY"); }
	|  error
		={ bundle(1,"c"); }
	|  _IF CRS BLEV '(' re ')' stat
		={	conout( $7, $2 );
			bundle(3, $5, $2, " " );
			}
	|  _WHILE CRS '(' re ')' stat BLEV
		={	bundle(3, $6, $4, $2 );
			conout( $$, $2 );
			bundle(3, $4, $2, " " );
			}
	|  fprefix CRS re ';' e ')' stat BLEV
		={	bundle(5, $7, $5, "s.", $3, $2 );
			conout( $$, $2 );
			bundle(5, $1, "s.", $3, $2, " " );
			}
	|  '~' LETTER '=' e
		={	bundle(3,$4,"S",$2); }
	;

EQOP	:  EQPL
		={ $$ = (int)"+"; }
	|  EQMI
		={ $$ = (int)"-"; }
	|  EQMUL
		={ $$ = (int)"*"; }
	|  EQDIV
		={ $$ = (int)"/"; }
	|  EQREM
		={ $$ = (int)"%"; }
	|  EQEXP
		={ $$ = (int)"^"; }
	;

fprefix	:  _FOR '(' e ';'
		={ $$ = $3; }
	;

BLEV	:
		={ --bindx; }
	;

slist	:  stat
	|  slist tail stat
		={ bundle(2, $1, $3 ); }
	;

tail	:  '\n'
		={ln++;}
	|  ';'
	;

re	:  e EQ e
		= bundle(3, $1, $3, "=" );
	|  e '<' e
		= bundle(3, $1, $3, ">" );
	|  e '>' e
		= bundle(3, $1, $3, "<" );
	|  e NE e
		= bundle(3, $1, $3, "!=" );
	|  e GE e
		= bundle(3, $1, $3, "!>" );
	|  e LE e
		= bundle(3, $1, $3, "!<" );
	|  e
		= bundle(2, $1, " 0!=" );
	;

e	:  e '+' e
		= bundle(3, $1, $3, "+" );
	|  e '-' e
		= bundle(3, $1, $3, "-" );
	| '-' e		%prec UMINUS
		= bundle(3, " 0", $2, "-" );
	|  e '*' e
		= bundle(3, $1, $3, "*" );
	|  e '/' e
		= bundle(3, $1, $3, "/" );
	|  e '%' e
		= bundle(3, $1, $3, "%" );
	|  e '^' e
		= bundle(3, $1, $3, "^" );
	|  LETTER '[' e ']'
		={ bundle(3,$3, ";", geta($1)); }
	|  LETTER INCR
		= bundle(4, "l", $1, "d1+s", $1 );
	|  INCR LETTER
		= bundle(4, "l", $2, "1+ds", $2 );
	|  DECR LETTER
		= bundle(4, "l", $2, "1-ds", $2 );
	|  LETTER DECR
		= bundle(4, "l", $1, "d1-s", $1 );
	| LETTER '[' e ']' INCR
		= bundle(7,$3,";",geta($1),"d1+",$3,":",geta($1));
	| INCR LETTER '[' e ']'
		= bundle(7,$4,";",geta($2),"1+d",$4,":",geta($2));
	| LETTER '[' e ']' DECR
		= bundle(7,$3,";",geta($1),"d1-",$3,":",geta($1));
	| DECR LETTER '[' e ']'
		= bundle(7,$4,";",geta($2),"1-d",$4,":",geta($2));
	| SCALE INCR
		= bundle(1,"Kd1+k");
	| INCR SCALE
		= bundle(1,"K1+dk");
	| SCALE DECR
		= bundle(1,"Kd1-k");
	| DECR SCALE
		= bundle(1,"K1-dk");
	| BASE INCR
		= bundle(1,"Id1+i");
	| INCR BASE
		= bundle(1,"I1+di");
	| BASE DECR
		= bundle(1,"Id1-i");
	| DECR BASE
		= bundle(1,"I1-di");
	| OBASE INCR
		= bundle(1,"Od1+o");
	| INCR OBASE
		= bundle(1,"O1+do");
	| OBASE DECR
		= bundle(1,"Od1-o");
	| DECR OBASE
		= bundle(1,"O1-do");
	|  LETTER '(' cargs ')'
		= bundle(4, $3, "l", getf($1), "x" );
	|  LETTER '(' ')'
		= bundle(3, "l", getf($1), "x" );
	|  cons
		={ bundle(2, " ", $1 ); }
	|  DOT cons
		={ bundle(2, " .", $2 ); }
	|  cons DOT cons
		={ bundle(4, " ", $1, ".", $3 ); }
	|  cons DOT
		={ bundle(3, " ", $1, "." ); }
	|  DOT
		={ $$ = (int)"l."; }
	|  LETTER
		= { bundle(2, "l", $1 ); }
	|  LETTER '=' e
		={ bundle(3, $3, "ds", $1 ); }
	|  LETTER EQOP e	%prec '='
		={ bundle(6, "l", $1, $3, $2, "ds", $1 ); }
	| LETTER '[' e ']' '=' e
		= { bundle(5,$6,"d",$3,":",geta($1)); }
	| LETTER '[' e ']' EQOP e
		= { bundle(9,$3,";",geta($1),$6,$5,"d",$3,":",geta($1)); }
	| LENGTH '(' e ')'
		= bundle(2,$3,"Z");
	| SCALE '(' e ')'
		= bundle(2,$3,"X");	/* must be before '(' e ')' */
	|  '(' e ')'
		= { $$ = $2; }
	|  '?'
		={ bundle(1, "?" ); }
	|  SQRT '(' e ')'
		={ bundle(2, $3, "v" ); }
	| '~' LETTER
		={ bundle(2,"L",$2); }
	| SCALE '=' e
		= bundle(2,$3,"dk");
	| SCALE EQOP e		%prec '='
		= bundle(4,"K",$3,$2,"dk");
	| BASE '=' e
		= bundle(2,$3,"di");
	| BASE EQOP e		%prec '='
		= bundle(4,"I",$3,$2,"di");
	| OBASE '=' e
		= bundle(2,$3,"do");
	| OBASE EQOP e		%prec '='
		= bundle(4,"O",$3,$2,"do");
	| SCALE
		= bundle(1,"K");
	| BASE
		= bundle(1,"I");
	| OBASE
		= bundle(1,"O");
	;

cargs	:  eora
	|  cargs ',' eora
		= bundle(2, $1, $3 );
	;
eora:	  e
	| LETTER '[' ']'
		=bundle(2,"l",geta($1));
	;

cons	:  constant
		={ *cp++ = '\0'; }

constant:
	  '_'
		={ $$ = (int)cp; *cp++ = '_'; }
	|  DIGIT
		={ $$ = (int)cp; *cp++ = $1; }
	|  constant DIGIT
		={ *cp++ = $2; }
	;

CRS	:
		={ $$ = (int)cp; *cp++ = crs++; *cp++ = '\0';
			if(crs == '[')crs+=3;
			if(crs == 'a')crs='{';
			if(crs >= 0241) {
			    yyerror(MSGSTR(TOOBIG,
			       "bc: program too big"));
				getout();
			}
			bstack[bindx++] = lev++; }
	;

def	:  _DEFINE LETTER '('
		={	$$ = (int)getf($2);
			pre = (int *)"";
			post = (int *)"";
			lev = 1;
			bstack[bindx=0] = 0;
			}
	;

dargs	:
	|  lora
		={ pp( $1 ); }
	|  dargs ',' lora
		={ pp( $3 ); }
	;

dlets	:  lora
		={ tp($1); }
	|  dlets ',' lora
		={ tp($3); }
	;
lora	:  LETTER
	|  LETTER '[' ']'
		={ $$ = (int)geta($1); }
	;

/*  The Program section. */
/*  This section contains the C language programs which perform the functions
 *  used by the actions in the rules section. */
%%
# define error 256

int peekc = -1;
int sargc;
int ifile;
char **sargv;

char funtab[52] = {
	01,0,02,0,03,0,04,0,05,0,06,0,07,0,010,0,011,0,012,0,013,0,014,0,015,0,016,0,017,0,
	020,0,021,0,022,0,023,0,024,0,025,0,026,0,027,0,030,0,031,0,032,0 };
char atab[52] = {
	0241,0,0242,0,0243,0,0244,0,0245,0,0246,0,0247,0,0250,0,0251,0,0252,0,0253,0,
	0254,0,0255,0,0256,0,0257,0,0260,0,0261,0,0262,0,0263,0,0264,0,0265,0,0266,0,
	0267,0,0270,0,0271,0,0272,0};
char *letr[26] = {
  "a","b","c","d","e","f","g","h","i","j",
  "k","l","m","n","o","p","q","r","s","t",
  "u","v","w","x","y","z" } ;
char *dot = { "." };


/*
 *  NAME:  yylex 
 *
 *  FUNCTION:  This is the lexical analyzer.  It reads the input
 *	       stream and sends tokens (with values, if required)
 *	       to the parser that yacc generates. 
 *
 *  RETURN VALUE:  Returns an integer (called a token number) which 
 * 		   represents the kind of token that was read.
 *
 */
yylex(){
	int c, ch;

    while(1) {
	c = getch();
	peekc = -1;
	while( c == ' ' || c == '\t' ) c = getch();
	if(c == '\\'){
		getch();
		break;
	}
	if( c<= 'z' && c >= 'a' ) {
		/* look ahead for reserved words */
		peekc = getch();
		if( peekc >= 'a' && peekc <= 'z' ){ /* must be reserved word */
			if( c=='i' && peekc=='f' ){ c=_IF; }
			else if( c=='w' && peekc=='h' ){ c=_WHILE; }
			else if( c=='f' && peekc=='o' ){ c=_FOR; }
			else if( c=='s' && peekc=='q' ){ c=SQRT; }
			else if( c=='r' && peekc=='e' ){ c=_RETURN; }
			else if( c=='b' && peekc=='r' ){ c=_BREAK; }
			else if( c=='d' && peekc=='e' ){ c=_DEFINE; }
		 	else if( c=='s' && peekc=='c' ){ c= SCALE; }
			else if( c=='b' && peekc=='a' ){ c=BASE; }
			else if( c=='i' && peekc == 'b'){ c=BASE; }
			else if( c=='o' && peekc=='b' ){ c=OBASE; }
			else if( c=='d' && peekc=='i' ){ c=FFF; }
			else if( c=='a' && peekc=='u' ){ c=_AUTO; }
			else if( c == 'l' && peekc=='e'){ c=LENGTH; }
			else if( c == 'q' && peekc == 'u'){getout();}
			/* could not be found */
			else return( error );
			/* skip over rest of word. */
			peekc = -1;
			while( (ch = getch()) >= 'a' && ch <= 'z' );
			peekc = ch;
			return( c );
		}

		/* usual case; single letter */
		yylval = (int)letr[c-'a'];
		return( LETTER );
	}
	if( c>= '0' && c <= '9' || c>= 'A' && c<= 'F' ){
		yylval = c;
		return( DIGIT );
	}
	/* Parse out the operators. */
	switch( c ){
	case '.':	return( DOT );
	case '=':
		switch( peekc = getch() ){
			case '=': c=EQ; peekc = -1; return(c);
			case '+': c=EQPL; peekc = -1; return(c);
			case '-': c=EQMI; peekc = -1; return(c);
			case '*': c=EQMUL; peekc = -1; return(c);
			case '/': c=EQDIV; peekc = -1; return(c);
			case '%': c=EQREM; peekc = -1; return(c);
			case '^': c=EQEXP; peekc = -1; return(c);
			default:   return( '=' );
		}
	case '+':	return( cpeek( '+', INCR, '+' ) );
	case '-':	return( cpeek( '-', DECR, '-' ) );
	case '<':	return( cpeek( '=', LE, '<' ) );
	case '>':	return( cpeek( '=', GE, '>' ) );
	case '!':	return( cpeek( '=', NE, '!' ) );
	case '/':
		if((peekc = getch()) == '*'){
			peekc = -1;
			while((getch() != '*') || ((peekc = getch()) != '/'));
			peekc = -1;
			break;
		}
		else return(c);
	case '"':	
		 /* If a string is encountered, then read it in until the
		  * second set of double quotes is found.  */
		 yylval = (int)str;
		 while((c=getch()) != '"') {
			*str++ = c;
			if(str >= &string[999]) {
			   yyerror(MSGSTR(NOSTRSPC,
			  	 "bc: string space exceeded"));
			    getout();
		        }
	         }
	         *str++ = '\0';
	         return(QSTR);
	default:	 return( c );
	}
    }
}



/*
 *  NAME:  cpeek
 *
 *  FUNCTION:  This function is used to parse operators which are made up
 *		of more than one character.  For example, a call to this
 *		function might be "cpeek( "=", LE, "<" )".  When this call
 *		is made, it has already been determined that a "<" sign
 *		has been found.  This function says that if the next 
 *		character is an equal sign, then the operator is "<=".
 *		If the next character is not an equal sign, the the 
 *		operator is simply "<". 
 *
 *  RETURN VALUE:  yes)  The next character read is the same as the first
 *				arguement. 
 *		   no)   The next character read is not the same as the    
 *				first arguement.
 *
 */
cpeek( c, yes, no ){
	if( (peekc=getch()) != c ) return( no );
	else {
		peekc = -1;
		return( yes );
	}
}

/*
 *  NAME:  getch  
 *
 *  FUNCTION: Reads in a character. 
 *
 *  RETURN VALUE:  The character read in.
 *
 */
getch(){
	int ch;
	int cont=1;

	while(cont) {
		ch = (peekc < 0) ? getc(in) : peekc;
		peekc = -1;
		if(ch != EOF)return(ch);
		if(++ifile > sargc){
			if(ifile >= sargc+2)getout();
			in = stdin;
			ln = 0;
		}
		else {
			fclose(in);
			if((in = fopen(sargv[ifile],"r")) != NULL) {
				ln = 0;
				ss = sargv[ifile];
			}
			else {
				yyerror(MSGSTR(NOINFILE,
					"bc: cannot open input file"));
				cont = 0;
			}
		}
	}
}

/*
 *  NAME:  bundle
 *
 *  FUNCTION:  Stores the arguements into the bundling space. 
 *
 *  RETURN VALUE:  The location in the bundling space where the args 
 *			are stored.
 *
 */
# define b_sp_max 3000
int b_space [ b_sp_max ];
int * b_sp_nxt = { b_space };

int	bdebug = 0;
static int
bundle(int i,...){
  	va_list ap;

	int *q;

  	va_start(ap, i);

	q = b_sp_nxt;
	if( bdebug ) printf(MSGSTR(BUND, "bundle %d elements at %x\n"),i,  q );
	while(i-- > 0){
		if( b_sp_nxt >= & b_space[b_sp_max] )
			yyerror(MSGSTR(NOBUNDLE,
			   "bc: bundling space exceeded"));
		* b_sp_nxt++ = va_arg(ap, int);
	}
	* b_sp_nxt++ = 0;
	yyval = (int)q;
	return( (int)q );
}

/*
 *  NAME:  routput
 *
 *  FUNCTION:  This is a recursive function.  It prints the contents of the
 *		bundling space from "p" until the end.
 *
 *  RETURN VALUE:  none
 *
 */
routput(p) int *p; {
	if( bdebug ) printf(MSGSTR(ROUT, "routput(%o)\n"), p );
	if( p >= &b_space[0] && p < &b_space[b_sp_max]){
		/* part of a bundle */
		while( *p != 0 ) routput( *p++ );
	}
	else printf("%s",p );	 /* character string */
}

/*
 *  NAME:  output
 *
 *  FUNCTION:  Prints out the contents of the budling space from "*p" on.
 *
 *  RETURN VALUE: none
 *
 */
output( p ) int *p; {
	routput( p );
	b_sp_nxt = & b_space[0];
	printf( "\n" );
	fflush(stdout);
	cp = cary;
	crs = rcrs;
}

conout( p, s ) int *p; char *s; {
	printf("[");
	routput( p );
	printf("]s%s\n", s );
	fflush(stdout);
	lev--;
}

/*
 *  NAME:  yyerror
 *
 *  FUNCTION:  Handles errors which occur during parser operation.
 *
 *  RETURN VALUE:  none
 *
 */
yyerror( s ) char *s; {
	char *t, errmsg[256];

	if(ifile > sargc)ss=", teletype";
	sprintf(errmsg, MSGSTR(INVINP, "%s on line %d %s"), s, ln+1, ss );
	t = errmsg+strlen(errmsg)-1;
	if (*t == '\n')
		*t = '\0';
	printf("c[%s]pc\n", errmsg );
	fflush(stdout);
	cp = cary;
	crs = rcrs;
	bindx = 0;
	lev = 0;
	b_sp_nxt = &b_space[0];
	exit();
}

/*
 *  NAME:  pp
 *
 *  FUNCTION: Puts the relevant stuff on pre and post for the letter s.
 *
 *  RETURN VALUE:  none
 *
 */
pp( s ) char *s; {
	bundle(3, "S", s, pre );
	pre = (int *)yyval;
	bundle(4, post, "L", s, "s." );
	post = (int *)yyval;
}

/*
 *  NAME:  tp 
 *
 *  FUNCTION:  Same as pp, but for temps.
 *
 *  RETURN VALUE:  none
 *
 */
tp( s ) char *s; { 
	bundle(3, "0S", s, pre );
	pre = (int *)yyval;
	bundle(4, post, "L", s, "s." );
	post = (int *)yyval;
}

/*
 *  NAME:  yyinit
 *
 *  FUNCTION:  If a valid input file was given in the programs invocation,
 * 		then that file is opened, else stdin is used.
 *
 *  RETURN VALUE:  none
 *
 */
yyinit(argc,argv) int argc; char *argv[];{
	signal( 2, (int(*)())1 );	/* ignore all interrupts */
	sargv=argv;
	sargc= -- argc;
	if(sargc == 0)in=stdin;
	else if((in = fopen(sargv[1],"r")) == NULL)
		yyerror(MSGSTR(NOINFILE,
			"bc: cannot open input file"));
	ifile = 1;
	ln = 0;
	ss = sargv[1];
}


/*
 *  NAME:  getout
 *
 *  FUNCTION:  exits the program.
 *
 *  RETURN VALUE: none 
 *
 */
static int *getout(){
	printf("q");
	fflush(stdout);
	exit();
}

int *
getf(p) char *p;{
	return((int *)&funtab[2*(*p -0141)]);
}

int *
geta(p) char *p;{
	return((int *)&atab[2*(*p - 0141)]);
}


/*
 *  NAME:  main
 *
 *  FUNCTION:  Parses out the options which are used to call bc.
 *
 *  RETURN VALUE:  none
 *
 */
main(argc, argv)
char **argv;
{
	int p[2];


	(void)setlocale(LC_ALL, "");
	catd = catopen(MF_BC,0);
	/* Takes care of any specified options.  These can be either
	 * "bc" for binary calculator or "l" to include a library. */
	if (argc > 1 && *argv[1] == '-') {
		if((argv[1][1] == 'd')||(argv[1][1] == 'c')){
			yyinit(--argc, ++argv);
			yyparse();
			exit();
		}
		if(argv[1][1] != 'l'){
			printf(MSGSTR(USAGE,
			   "usage: bc [-cl] [files...]\n"));
			fflush(stdout);
			exit();
		}
		argv[1] = "/usr/lib/lib.b";
	}
	pipe(p);
	if (fork()==0) {
		close(1);
		dup(p[1]);
		close(p[0]);
		close(p[1]);
		yyinit(argc, argv);
		yyparse();
		exit();
	}
	close(0);
	dup(p[0]);
	close(p[0]);
	close(p[1]);
	execl("/bin/dc", "dc", "-", 0);
	execl("/usr/bin/dc", "dc", "-", 0);
}
