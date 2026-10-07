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
static char rcsid[] = "@(#)$RCSfile: dc.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 01:22:16 $";
#endif
/*
 * COMPONENT_NAME: (CMDCALC) calculators
 *
 * FUNCTIONS: dc
 *
 * ORIGINS: 3 26 27
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * dc.c        1.9  com/cmd/calc,3.1,9021 1/26/90 13:35:21";
 */

/*
 *
 *  NAME: dc [ file ]
 *                                                                     
 *  FUNCTION: 
 *     The dc command is  an arbitrary precision arithmetic cal-
 *     culator.  dc takes its input  from file or standard input
 *     until it  reads an  end-of-file character.  It  writes to
 *     standard output.   It operates  on decimal  integers, but
 *     you may specify an input  base, output base, and a number
 *     of fractional digits to  be maintained.  dc is structured
 *     overall as a stacking, reverse Polish, calculator.
 * 
 *  OPTIONS:
 *	file	Specifies the input file.
 */                                                                    
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <NLchar.h>
#include <locale.h>
#include <nl_types.h>
#include "dc_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_DC,Num,Str)
#include "dc.h"


/*
 *  NAME:  main
 *
 *  FUNCTION:  This is the main-line procedure.
 *
 *  RETURN VALUE: none
 *
 */
main(argc,argv)
int argc;
char *argv[];
{
        (void) setlocale(LC_ALL, "");
	catd = catopen(MF_DC,0);
	init(argc,argv);
	commnds();
}


/*
 *  NAME:  commnds
 *
 *  FUNCTION:  Contains a huge SWITCH statement within a WHILE loop which
 *		contains the various commands which dc uses.
 *
 *  RETURN VALUE: none
 *
 */
commnds(){
	register int c;

	int c1;	/* Holds the second byte read of a two-byte */
		/* NLS character. */
	NLchar nlc;	/* Used as the index into the symbol table. */

	register struct blk *p,*q;
	long l;
	int sign;
	struct blk **ptr,*s,*t;
	struct sym *sp;
	int sk,sk1,sk2;
	int n,d;

/*
 *  The following define statements were created in an effort to remove
 *  the goto's which appeared in the original code.  In all cases, the
 *  code which appears here originally appeared under a goto header, 
 *  and this header was in turn called throughout the code.
 */
#define CASEDIV() \
	if(dscale() != 0)continue; \
	binop('/'); \
	if(irem != 0)release(irem); \
	release(rem)

#define SEMPTY() \
	error(MSGSTR(M_STOV,"symbol table overflow\n"))

#define EXECUTE() \
	p = pop();  \
	EMPTY;  \
	if((readptr != &readstk[0]) && (*readptr != 0)){  \
		if((*readptr)->rd == (*readptr)->wt)  \
			release(*readptr);  \
		else{  \
			if(readptr++ == &readstk[RDSKSZ]){  \
				error(MSGSTR(EM_NSTDPT,"nesting depth\n"));  \
			}  \
		}  \
	}  \
	else readptr++;  \
	*readptr = p;  \
	if(p != 0)rewind(p);  \
	else if((c = readc()) != '\n')unreadc(c)

#define DDONE() \
	if(remsign<0)chsign(divd); \
	if(divr != ddivr)release(divr); \
	rem = divd

#define EDONE() \
	release(p);  \
	release(e)




	/*  This while statement takes the place of a goto statement.
	 *  It is being used to retain the integrity of the program. */
	while(1){
		if(((c = readc())>='0' && c <= '9')|| (c>='A' && c <='F') || c == '.'){
			unreadc(c);
			p = readin();
			pushp(p);
			continue;
		}

		switch(c){
		case ' ':
		case '\n':
		case 0377:
		case EOF:
			continue;
		/*  Prints out various statistics on the status of the stack. */
		case 'Y':
			if(stkptr == &stack[0]) {
				printf(MSGSTR(EMPTSTK,"empty stack\n"));
				continue;
			}
			sdump("stk",*stkptr);
			NLprintf(MSGSTR(HEADMOR,
			    "all %ld rel %ld headmor %ld\n"),all,rel,headmor);
			printf(MSGSTR(NBYTES,"nbytes %ld\n"),nbytes);
			continue;
		/*  The underscore indicates unary minus. */
		case '_':
			p = readin();
			savk = sunputc(p);
			chsign(p);
			sputc(p,savk);
			pushp(p);
			continue;
		/*  subtraction */
		case '-':
			subt();
			continue;
		/*  Either unary minus or the addition operator. */
		case '+':
			if(eqk() != 0)continue;
			binop('+');
			continue;
		/*  multiplication */
		case '*':
			arg1 = pop();
			EMPTY;
			arg2 = pop();
			EMPTYR(arg1);
			sk1 = sunputc(arg1);
			sk2 = sunputc(arg2);
			binop('*');
			p = pop();
			sunputc(p);
			savk = sk1+sk2;
			if(savk>k && savk>sk1 && savk>sk2){
				sk = sk1;
				if(sk<sk2)sk = sk2;
				if(sk<k)sk = k;
				p = removc(p,savk-sk);
				savk = sk;
			}
			sputc(p,savk);
			pushp(p);
			continue;
		/*  division */
		case '/':  
			CASEDIV();
			continue;
		/*  remainder */
		case '%':
			if(dscale() != 0)continue;
			binop('/');
			p = pop();
			release(p);
			if(irem == 0){
				sputc(rem,skr+k);
				pushp(rem);
				continue;
			}
			p = add0(rem,skd-(skr+k));
			q = add(p,irem);
			release(p);
			release(irem);
			sputc(q,skd);
			pushp(q);
			continue;
		/*  Replaces the top element on the stack by its square root. */
		case 'v':
			p = pop();
			EMPTY;
			savk = sunputc(p);
			if(length(p) == 0){
				sputc(p,savk);
				pushp(p);
				continue;
			}
			if((c = sbackc(p))<0){
				error(
				MSGSTR(EM_IMNUM,"sqrt of neg number\n"));/*MSG*/
			}
			if(k<savk)n = savk;
			else{
				n = k*2-savk;
				savk = k;
			}
			arg1 = add0(p,n);
			arg2 = sqrt(arg1);
			sputc(arg2,savk);
			pushp(arg2);
			continue;
		/*  exponenciation */
		case '^':
			neg = 0;
			arg1 = pop();
			EMPTY;
			if(sunputc(arg1) != 0)error(
				MSGSTR(EM_NOTINT,"exp not an integer\n"));/*MSG*/
			arg2 = pop();
			EMPTYR(arg1);
			if(sfbeg(arg1) == 0 && sbackc(arg1)<0){
				neg++;
				chsign(arg1);
			}
			if(length(arg1)>=3){
				error(
				MSGSTR(EM_2BIG,"exp too big\n"));/*MSG*/
			}
			savk = sunputc(arg2);
			p = exp(arg2,arg1);
			release(arg2);
			rewind(arg1);
			c = sgetc(arg1);
			if(c == EOF)c = 0;
			else if(sfeof(arg1) == 0)
				c = sgetc(arg1)*100 + c;
			d = c*savk;
			release(arg1);
			if(neg == 0){
				if(k>=savk)n = k;
				else n = savk;
				if(n<d){
					q = removc(p,d-n);
					sputc(q,n);
					pushp(q);
				}
				else {
					sputc(p,d);
					pushp(p);
				}
			}
			else {
				sputc(p,d);
				pushp(p);
			}
			if(neg == 0)continue;
			p = pop();
			q = salloc(2);
			sputc(q,1);
			sputc(q,0);
			pushp(q);
			pushp(p);
			CASEDIV();
			continue;
		/*  Pushes the numb of elements in the stack onto the stack. */
		case 'z':
			p = salloc(2);
			n = stkptr - stkbeg;
			if(n >= 100){
				sputc(p,n/100);
				n %= 100;
			}
			sputc(p,n);
			sputc(p,0);
			pushp(p);
			continue;
		/*  Replaces the top number in the stack with the number of 
		 *  digits in that number. */
		case 'Z':
			p = pop();
			EMPTY;
			n = (length(p)-1)<<1;
			fsfile(p);
			sbackc(p);
			if(sfbeg(p) == 0){
				if((c = sbackc(p))<0){
					n -= 2;
					if(sfbeg(p) == 1)n += 1;
					else {
						if((c = sbackc(p)) == 0)n += 1;
						else if(c > 90)n -= 1;
					}
				}
				else if(c < 10) n -= 1;
			}
			release(p);
			q = salloc(1);
			if(n >= 100){
				sputc(q,n%100);
				n /= 100;
			}
			sputc(q,n);
			sputc(q,0);
			pushp(q);
			continue;
		/*  Pops the top value on the stack and uses that value as the
		 *  number radix for further input. */
		case 'i':
			p = pop();
			EMPTY;
			p = scalint(p);
			release(inbas);
			inbas = p;
			continue;
		/*  Pushes the input base on the top of the stack. */		
		case 'I':
			p = copy(inbas,length(inbas)+1);
			sputc(p,0);
			pushp(p);
			continue;
		/*  Pops the top value on the stack and uses that value as
		 *  the number radix for further output. */
		case 'o':
			p = pop();
			EMPTY;
			p = scalint(p);
			sign = 0;
			n = length(p);
			q = copy(p,n);
			fsfile(q);
			l = c = sbackc(q);
			if(n != 1){
				if(c<0){
					sign = 1;
					chsign(q);
					n = length(q);
					fsfile(q);
					l = c = sbackc(q);
				}
				if(n != 1){
					while(sfbeg(q) == 0)l = l*100+sbackc(q);
				}
			}
			logo = log2(l);
			obase = l;
			release(basptr);
			if(sign == 1)obase = -l;
			basptr = p;
			outdit = bigot;
			if(n == 1 && sign == 0){
				if(c <= 16){
					outdit = hexot;
					fw = 1;
					fw1 = 0;
					ll = 70;
					release(q);
					continue;
				}
			}
			n = 0;
			if(sign == 1)n++;
			p = salloc(1);
			sputc(p,-1);
			t = add(p,q);
			n += length(t)*2;
			fsfile(t);
			if((c = sbackc(t))>9)n++;
			release(t);
			release(q);
			release(p);
			fw = n;
			fw1 = n-1;
			ll = 70;
			if(fw>=ll)continue;
			ll = (70/fw)*fw;
			continue;
		/*  Pushes the output base on the top of the stack. */
		case 'O':
			p = copy(basptr,length(basptr)+1);
			sputc(p,0);
			pushp(p);
			continue;
		/*  Puts the bracketed string onto the top of the stack. */
		case '[':
			n = 0;
			p = salloc(0);
			while(1){
				if((c = readc()) == ']'){
					if(n == 0)break;
					n--;
				}
				sputc(p,c);
				if(c == '[')n++;
			}
			pushp(p);
			continue;
		/*  Pops the top of the stack and uses that value as a 
		 *  nonnegative scale factor. */
		case 'k':
			p = pop();
			EMPTY;
			p = scalint(p);
			if(length(p)>1){
				error(
				MSGSTR(EM_SC2BIG,"scale too big\n"));/*MSG*/
			}
			rewind(p);
			k = sfeof(p)?0:sgetc(p);
			release(scalptr);
			scalptr = p;
			continue;
		case 'K':
			p = copy(scalptr,length(scalptr)+1);
			sputc(p,0);
			pushp(p);
			continue;
		/*  Replaces the number on the top of the stack with its
		 *  scale factor. */
		case 'X':
			p = pop();
			EMPTY;
			fsfile(p);
			n = sbackc(p);
			release(p);
			p = salloc(2);
			sputc(p,n);
			sputc(p,0);
			pushp(p);
			continue;
		/*  Pops the top value on the stack and the string execution
		 *  level by that value. */
		case 'Q':
			p = pop();
			EMPTY;
			if(length(p)>2){
				error(
				MSGSTR(EM_QWHO,"Q?\n"));/*MSG*/
			}
			rewind(p);
			if((c =  sgetc(p))<0){
				error(
				MSGSTR(EM_NEGQ,"neg Q\n"));/*MSG*/
			}
			release(p);
			while(c-- > 0){
				if(readptr == &readstk[0]){
					error(
					MSGSTR(EM_RS,"readstk?\n"));/*MSG*/
				}
				if(*readptr != 0)release(*readptr);
				readptr--;
			}
			continue;
		/*  Exits the program. */
		case 'q':
			if(readptr <= &readstk[1])exit(0);
			if(*readptr != 0)release(*readptr);
			readptr--;
			if(*readptr != 0)release(*readptr);
			readptr--;
			continue;
		/*  Displays all values on the stack. */
		case 'f':
			if(stkptr == &stack[0])
				printf(MSGSTR(EMPTSTK,"empty stack\n"));
			else {
				for(ptr = stkptr; ptr > &stack[0];){
					print(*ptr--);
				}
			}
			continue;
		/*  Displays the top value on the stack. */
		case 'p':
			if(stkptr == &stack[0])
				printf(MSGSTR(EMPTSTK,"empty stack\n"));
			else{
				print(*stkptr);
			}
			continue;
		/*  Interprets the top of the stack as a string, removes
		 *  it and displays it. */
		case 'P':
			p = pop();
			EMPTY;
			sputc(p,0);
			printf("%s",p->beg);
			release(p);
			continue;
		/*  Duplicates the top of the stack. */
		case 'd':
			if(stkptr == &stack[0]){
				printf(MSGSTR(EMPTSTK,"empty stack\n"));
				continue;
			}
			q = *stkptr;
			n = length(q);
			p = copy(*stkptr,n);
			pushp(p);
			continue;
		/*  Cleans the stack.  Dc pops all values on the stack. */
		case 'c':
			while(stkerr == 0){
				p = pop();
				if(stkerr == 0)release(p);
			}
			continue;
		/*  Treats the character following S as a stack(ex: "x").
		 *  It pops the top off the main stack and pushes that 
		 *  value onto stack "x". */
		case 'S':
			if(stkptr == &stack[0]){
				error(
				MSGSTR(EM_SAVE,"save: args\n"));/*MSG*/
			}
			c = readc() & 0377;

			/* Test for a shift byte. */
			if (NCisshift(c)) {

			    /* Read the second byte of the two-byte NLS */
			    /* character. */
			    c1 = readc() & 0377;
			} /* end if */

			/* Convert to an NLchar. */
			_NCdec2(c,c1,nlc);
#ifndef KJI
			nlc = _NCmap(nlc);
#endif

			/* Index into the symbol table on an NLchar, nlc. */
			sptr = stable[nlc];
			sp = stable[nlc] = sfree;
			sfree = sfree->next;
			if(sfree == 0) {
				SEMPTY();
				break;
			}
			sp->next = sptr;
			p = pop();
			EMPTY;
			if(nlc >= ARRAYST){
				q = copy(p,PTRSZ);
				for(n = 0;n < PTRSZ-1;n++)sputc(q,0);
				release(p);
				p = q;
			}
			sp->val = p;
			continue;
		/*  Pops the top of the stack and stores it in a register
		 *  named "x"(the charcter following "s").  */
		case 's':
			if(stkptr == &stack[0]){
				error(
				MSGSTR(EM_SARGS,"save:args\n"));/*MSG*/
			}
			c = readc() & 0377;

			/* Test for a shift byte. */
			if (NCisshift(c)) {

			    /* Read the second byte of the two-byte NLS */
			    /* character. */
			    c1 = readc() & 0377;
			} /* end if */

			/* Convert to an NLchar. */
			_NCdec2(c,c1,nlc);
#ifndef KJI
			nlc = _NCmap(nlc);
#endif

			/* Index into the symbol table on an NLchar, nlc. */
			sptr = stable[nlc];
			if(sptr != 0){
				p = sptr->val;
				if(nlc >= ARRAYST){
					rewind(p);
					while(sfeof(p) == 0)release(getwd(p));
				}
				release(p);
			}
			else{
				sptr = stable[nlc] = sfree;
				sfree = sfree->next;
				if(sfree == 0) {
					SEMPTY();
					break;
				}
				sptr->next = 0;
			}
			p = pop();
			sptr->val = p;
			continue;
		/*  Pushes the value in register "x"(the character following
		 *  "l") onto the stack. */
		case 'l':
			load();
			continue;
		/*  Treats "x"(the character following "L") as a stack and pops
		 *  its top value onto the main stack. */
		case 'L':
			c = readc() & 0377;


	 		/* Test for a shift byte. */
			if (NCisshift(c)) {

	    		    /* Read the second byte of the two-byte NLS */
	    		    /* character. */
	    		    c1 = readc() & 0377;
			} /* end if */

			/* Convert to an NLchar. */
			_NCdec2(c,c1,nlc);
#ifndef KJI
			nlc = _NCmap(nlc);
#endif

			/* Index into the symbol table on an NLchar, nlc. */
			sptr = stable[nlc];
			if(sptr == 0){
				error(
				MSGSTR(EM_LWHO,"L?\n"));/*MSG*/
			}
			stable[nlc] = sptr->next;
			sptr->next = sfree;
			sfree = sptr;
			p = sptr->val;
			if(nlc >= ARRAYST){

				rewind(p);
				while(sfeof(p) == 0){
					q = getwd(p);
					if(q != 0)release(q);
				}
			}
			pushp(p);
			continue;
		/*  Used for array operations. */
		case ':':
			p = pop();
			EMPTY;
			q = scalint(p);
			fsfile(q);
			c = 0;
			if((sfbeg(q) == 0) && ((c = sbackc(q))<0)){
				error(
				MSGSTR(EM_NEGIND,"neg index\n"));/*MSG*/
			}
			if(length(q)>2){
				error(
				MSGSTR(EM_IND2BIG,"index too big\n"));/*MSG*/
			}
			if(sfbeg(q) == 0)c = c*100+sbackc(q);
			if(c >= MAXIND){
				error(
				MSGSTR(EM_IND2BIG,"index too big\n"));/*MSG*/
			}
			release(q);
			n = readc() & 0377;
			sptr = stable[n];
			if(sptr == 0){
				sptr = stable[n] = sfree;
				sfree = sfree->next;
				if(sfree == 0) {
					SEMPTY();
					break;
				}
				sptr->next = 0;
				p = salloc((c+PTRSZ)*PTRSZ);
				zero(p);
			}
			else{
				p = sptr->val;
				if(length(p)-PTRSZ < c*PTRSZ){
					q = copy(p,(c+PTRSZ)*PTRSZ);
					release(p);
					p = q;
				}
			}
			seekc(p,c*PTRSZ);
			q = lookwd(p);
			if (q!=NULL) release(q);
			s = pop();
			EMPTY;
			salterwd(p,s);
			sptr->val = p;
			continue;
		/*  Used for array operations. */
		case ';':
			p = pop();
			EMPTY;
			q = scalint(p);
			fsfile(q);
			c = 0;
			if((sfbeg(q) == 0) && ((c = sbackc(q))<0)){
				error(
				MSGSTR(EM_NEGIND,"neg index\n"));/*MSG*/
			}
			if(length(q)>2){
				error(
				MSGSTR(EM_IND2BIG,"index too big\n"));/*MSG*/
			}
			if(sfbeg(q) == 0)c = c*100+sbackc(q);
			if(c >= MAXIND){
				error(
				MSGSTR(EM_IND2BIG,"index too big\n"));/*MSG*/
			}
			release(q);
			n = readc() & 0377;
			sptr = stable[n];
			if(sptr != 0){
				p = sptr->val;
				if(length(p)-PTRSZ >= c*PTRSZ){
					seekc(p,c*PTRSZ);
					s = getwd(p);
					if(s != 0){
						q = copy(s,length(s));
						pushp(q);
						continue;
					}
				}
			}
			q = salloc(PTRSZ);
			putwd(q, (struct blk *)0);
			pushp(q);
			continue;
		/*  Treats the top element of the stack as a character string
		 *  and executes it as a string of dc commands. */
		case 'x':
			EXECUTE();
			continue;
		/*  Gets and runs a line of input. */
		case '?':
			if(++readptr == &readstk[RDSKSZ]){
				error(
				MSGSTR(EM_NSTDPT,"nesting depth\n"));/*MSG*/
			}
			*readptr = 0;
			fsave = curfile;
			curfile = stdin;
			while((c = readc()) == '!')command();
			p = salloc(0);
			sputc(p,c);
			while((c = readc()) != '\n'){
				sputc(p,c);
				if(c == '\\')sputc(p,readc());
			}
			curfile = fsave;
			*readptr = p;
			continue;
		/*  Interprets the rest of the line as an AIX command. */
		case '!':
			if(command() == 1) { EXECUTE(); }
			continue;
		/*  Pops the top two elements of the stack and compares them. */
		case '<':
		case '>':
		case '=':
			if(cond(c) == 1) { EXECUTE(); }
			continue;
		default:

			/* Was a shift byte read, i.e. the first */
			/* byte, of a two-byte NLS character? */
			if (NCisshift(c)) {
			    printf("%o ",c);
			   
			    /* Read in the second byte making up the */
			    /* NLS character. */
			    c = readc() & 0377;
			} /* end if */
			printf(
			MSGSTR(UNIMPL,"%o is unimplemented\n")/*MSG*/
			,c);
		}
	}
}

/*
 *  NAME:  div 
 *
 *  FUNCTION:  Pops the top two elements of the stack, divides them and 
 *		places the result back on the stack. 
 *
 *  RETURN VALUE:
 *
 */
struct blk *
div(ddivd,ddivr)
struct blk *ddivd,*ddivr;
{
	int divsign,remsign,offset,divcarry;
	int carry, dig,magic,d,dd;
	long c,td,cc;
	struct blk *ps;
	register struct blk *p,*divd,*divr;
	int temp;

	rem = 0;
	p = salloc(0);
	/*  If the divider is zero, then place it back on the stack. */
	if(length(ddivr) == 0){
		pushp(ddivr);
		printf(MSGSTR(DIVZERO,"divide by 0\n"));/*MSG*/
		return((struct blk *) 1);
	}
	divsign = remsign = 0;
	divr = ddivr;
	fsfile(divr);
	if((temp=sbackc(divr)) == -1){
		divr = copy(ddivr,length(ddivr));
		chsign(divr);
		divsign = ~divsign;
	}
	divd = copy(ddivd,length(ddivd));
	fsfile(divd);
	if(sfbeg(divd) == 0 && sbackc(divd) == -1){
		chsign(divd);
		divsign = ~divsign;
		remsign = ~remsign;
	}
	offset = length(divd) - length(divr);
	if(offset < 0) {
		DDONE();
		return(p);
	}
	seekc(p,offset+1);
	sputc(divd,0);
	magic = 0;
	fsfile(divr);
	c = sbackc(divr);
	if(c<10)magic++;
	c = c*100 + (sfbeg(divr)?0:sbackc(divr));
	if(magic>0){
		c = (c*100 +(sfbeg(divr)?0:sbackc(divr)))*2;
		c /= 25;
	}
	while(offset >= 0){
		fsfile(divd);
		td = sbackc(divd)*100;
		dd = sfbeg(divd)?0:sbackc(divd);
		td = (td+dd)*100;
		dd = sfbeg(divd)?0:sbackc(divd);
		td = td+dd;
		cc = c;
		if(offset == 0)td += 1;
		else cc += 1;
		if(magic != 0)td = td<<3;
		dig = td/cc;
		rewind(divr);
		rewind(divxyz);
		carry = 0;
		while(sfeof(divr) == 0){
			d = sgetc(divr)*dig+carry;
			carry = d / 100;
			salterc(divxyz,d%100);
		}
		salterc(divxyz,carry);
		rewind(divxyz);
		seekc(divd,offset);
		carry = 0;
		while(sfeof(divd) == 0){
			d = slookc(divd);
			d = d-(sfeof(divxyz)?0:sgetc(divxyz))-carry;
			carry = 0;
			if(d < 0){
				d += 100;
				carry = 1;
			}
			salterc(divd,d);
		}
		divcarry = carry;
		sbackc(p);
		salterc(p,dig);
		sbackc(p);
		if(--offset >= 0)divd->wt--;
	}
	if(divcarry != 0){
		salterc(p,dig-1);
		salterc(divd,-1);
		ps = add(divr,divd);
		release(divd);
		divd = ps;
	}

	rewind(p);
	divcarry = 0;
	while(sfeof(p) == 0){
		d = slookc(p)+divcarry;
		divcarry = 0;
		if(d >= 100){
			d -= 100;
			divcarry = 1;
		}
		salterc(p,d);
	}
	if(divcarry != 0)salterc(p,divcarry);
	fsfile(p);
	while(sfbeg(p) == 0){
		if((temp=sbackc(p)) == 0)truncate(p);
		else break;
	}
	if(divsign < 0)chsign(p);
	fsfile(divd);
	while(sfbeg(divd) == 0){
		if((temp=sbackc(divd)) == 0)truncate(divd);
		else break;
	}
	DDONE();
	return(p);
}



/*
 *  NAME:  dscale 
 *
 *  FUNCTION:  This function is used by the appropriate division functions
 *		to determine the scale.
 *
 *  RETURN VALUE:  0)  everything was OK.
 *
 */
dscale(){
	register struct blk *dd,*dr;
	register struct blk *r;
	int c;

	dr = pop();
	EMPTYS;
	dd = pop();
	EMPTYSR(dr);
	fsfile(dd);
	skd = sunputc(dd);
	fsfile(dr);
	skr = sunputc(dr);
	if(sfbeg(dr) == 1 || (sfbeg(dr) == 0 && sbackc(dr) == 0)){
		sputc(dr,skr);
		pushp(dr);
		errorrt(MSGSTR(DIVZERO,"divide by 0\n"));/*MSG*/
	}
	c = k-skd+skr;
	if(c < 0)r = removr(dd,-c);
	else {
		r = add0(dd,c);
		irem = 0;
	}
	arg1 = r;
	arg2 = dr;
	savk = k;
	return(0);
}



/*
 *  NAME:  removr
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
struct blk *
removr(p,n)
struct blk *p;
{
	int nn;
	register struct blk *q,*s,*r;

	/* Sets the pointer p back to the beginning. */
	rewind(p);
	nn = (n+1)/2;
	q = salloc(nn);
	while(n>1){
		sputc(q,sgetc(p));
		n -= 2;
	}
	r = salloc(2);
	while(sfeof(p) == 0)sputc(r,sgetc(p));
	release(p);
	if(n == 1){
		if(tenptr == NULL){
        		tenptr = salloc(1);
        		sputc(tenptr,10);
		}
		s = div(r,tenptr);
		release(r);
		rewind(rem);
		if(sfeof(rem) == 0)sputc(q,sgetc(rem));
		release(rem);
		irem = q;
		return(s);
	}
	irem = q;
	return(r);
}



/*
 *  NAME:  sqrt
 *
 *  FUNCTION:  Calculates the square root of the top element on the stack.
 *
 *  RETURN VALUE:  The square root.
 *
 */
struct blk *
sqrt(p)
struct blk *p;
{
	struct blk *t;
	struct blk *r,*q,*s;
	int c,n,nn;

	n = length(p);
	fsfile(p);
	c = sbackc(p);
	if((n&1) != 1)c = c*100+(sfbeg(p)?0:sbackc(p));
	n = (n+1)>>1;
	r = salloc(n);
	zero(r);
	seekc(r,n);
	nn=1;
	while((c -= nn)>=0)nn+=2;
	c=(nn+1)>>1;
	fsfile(r);
	sbackc(r);
	if(c>=100){
		c -= 100;
		salterc(r,c);
		sputc(r,1);
	}
	else salterc(r,c);
	while(1){
		q = div(p,r);
		s = add(q,r);
		release(q);
		release(rem);
		q = div(s,sqtemp);
		release(s);
		release(rem);
		s = copy(r,length(r));
		chsign(s);
		t = add(s,q);
		release(s);
		fsfile(t);
		nn = sfbeg(t)?0:sbackc(t);
		if(nn>=0)break;
		release(r);
		release(t);
		r = q;
	}
	release(t);
	release(q);
	release(p);
	return(r);
}


/*
 *  NAME:  exp
 *
 *  FUNCTION:  Uses the top two elements on the stack to determine the 
 *		exponent, and then places that value back on the stack.
 *
 *  RETURN VALUE:  The exponent.
 *
 */
struct blk *
exp(base,ex)
struct blk *base,*ex;
{
	register struct blk *r,*e,*p;
	struct blk *e1,*t,*cp;
	int temp,c,n;
	r = salloc(1);
	sputc(r,1);
	p = copy(base,length(base));
	e = copy(ex,length(ex));
	fsfile(e);
	if(sfbeg(e) != 0) {
		EDONE();
		return(r);
	}
	temp=0;
	c = sbackc(e);
	if(c<0){
		temp++;
		chsign(e);
	}
	while(length(e) != 0){
		e1=div(e,sqtemp);
		release(e);
		e = e1;
		n = length(rem);
		release(rem);
		if(n != 0){
			e1=mult(p,r);
			release(r);
			r = e1;
		}
		t = copy(p,length(p));
		cp = mult(p,t);
		release(p);
		release(t);
		p = cp;
	}
	if(temp != 0){
		if((c = length(base)) == 0){
			EDONE();
			return(r);
		}
		if(c>1)create(r);
		else{
			rewind(base);
			if((c = sgetc(base))<=1){
				create(r);
				sputc(r,c);
			}
			else create(r);
		}
	}
	EDONE();
	return(r);
}



/*
 *  NAME:  init
 *
 *  FUNCTION:  This is the first function which is executed.  It sets up
 *		the input file, makes sure it can create a directory,
 *		and initializes some variables.
 *
 *  RETURN VALUE: none
 *
 */
init(argc,argv)
int argc;
char *argv[];
{
	register struct sym *sp;
	struct stat statbuf;

	/* When an interrupt signal is found, execute onintr. */
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, (void (*)(int))onintr);
	setbuf(stdout,(char *)NULL);
	svargc = --argc;
	svargv = argv;
	while(svargc>0 && svargv[1][0] == '-'){
		switch(svargv[1][1]){
		default:
			dbg=1;
		}
		svargc--;
		svargv++;
	}
	ifile=1;
	if(svargc<=0)curfile = stdin;
	else if((curfile = fopen(svargv[1],"r")) == NULL){
		printf(
		MSGSTR(COF,"can't open file %s\n")/*MSG*/
		,svargv[1]);
		exit(1);
		}
	else {
		stat(svargv[1], &statbuf);
		if((statbuf.st_mode & S_IFMT) == S_IFDIR){
		printf(
		MSGSTR(CBD,"%s cannot be a directory\n")/*MSG*/
		, svargv[1]);
		exit(1);
		}
	}
	dummy = (char *)malloc(1);
	scalptr = salloc(1);
	sputc(scalptr,0);
	basptr = salloc(1);
	sputc(basptr,10);
	obase=10;
	log10=log2(10L);
	ll=70;
	fw=1;
	fw1=0;
	tenptr = salloc(1);
	sputc(tenptr,10);
	obase=10;
	inbas = salloc(1);
	sputc(inbas,10);
	sqtemp = salloc(1);
	sputc(sqtemp,2);
	chptr = salloc(0);
	strptr = salloc(0);
	divxyz = salloc(0);
	stkbeg = stkptr = &stack[0];
	stkend = &stack[STKSZ];
	stkerr = 0;
	readptr = &readstk[0];
	k=0;
	sp = sptr = &symlst[0];
	while(sptr < &symlst[TBLSZ]){
		sptr->next = ++sp;
		sptr++;
	}
	sptr->next=0;
	sfree = &symlst[0];
	return;
}



/*
 *  NAME:  onintr
 *
 *  FUNCTION:  This function is called when an interrupt is encountered
 *		in the init procedure.  It will resolve the interrupt, 
 *		assign stdin as the current file, and call the commands
 *		procedure.
 *
 *  RETURN VALUE: none
 *
 */
onintr(void){
	signal(SIGINT, (void (*)(int))onintr);
	while(readptr != &readstk[0]){
		if(*readptr != 0){release(*readptr);}
		readptr--;
	}
	curfile = stdin;
	commnds();
}



/*
 *  NAME:  pushp
 *
 *  FUNCTION:  Pushes an element onto the stack.
 *
 *  RETURN VALUE:  none
 *
 */
pushp(p)
struct blk *p;
{
	if(stkptr + 1 == stkend){
		printf(
		MSGSTR(OOSS,"out of stack space\n"));/*MSG*/
		return;
	}
	stkerr=0;
	*++stkptr = p;
	return;
}



/*
 *  NAME:  pop
 *
 *  FUNCTION:  Pops an element off the stack.
 *
 *  RETURN VALUE:  0)  Stack empty.
 *
 */
struct blk *
pop(){
	if(stkptr == stack){
		stkerr=1;
		return(0);
	}
	return(*stkptr--);
}



/*
 *  NAME:  readin
 *
 *  FUNCTION:  Reads in the data from the input file and does some 	
 *		rudimentary error checking before passing the value 
 *		back to the calling routine.
 *
 *  RETURN VALUE:  The valid input.
 *
 */
struct blk *
readin(){
	register struct blk *p,*q;
	int dp,dpct;
	register int c;

	dp = dpct=0;
	p = salloc(0);
	while(1){
		c = readc();
		switch(c){
		case '.':
			if(dp != 0){
                                printf(
				MSGSTR(TPI,"2 decimal pts. - 2nd ignored.\n"));
				break;
			}
			dp++;
			continue;
		case '\\':
			readc();
			continue;
		default:
			if(c >= 'A' && c <= 'F') c = c - 'A' + 10;
			else {
				if(c >= '0' && c <= '9')c -= '0';
				else goto gotnum;
			}
			if(dp != 0){
				if(dpct >= 99) continue;
				dpct++;
			}
			create(chptr);
			if(c != 0)sputc(chptr,c);
			q = mult(p,inbas);
			release(p);
			p = add(chptr,q);
			release(q);
		}
	}
gotnum:
	unreadc(c);  
	if(dp == 0){ 
		sputc(p,0);  
		return(p);  
	}  
	else{  
		q = scale(p,dpct);  
		return(q);  
	}  
}



/*
 *  NAME:  add0
 *
 *  FUNCTION:  Returns a pointer to struct.
 *
 *  RETURN VALUE:  A pointer to struct.
 *
 */
struct blk *
add0(p,ct)
int ct;
struct blk *p;
{
		/* returns pointer to struct with ct 0's & p */
	register struct blk *q,*t;

	q = salloc(length(p)+(ct+1)/2);
	while(ct>1){
		sputc(q,0);
		ct -= 2;
	}
	rewind(p);
	while(sfeof(p) == 0){
		sputc(q,sgetc(p));
	}
	release(p);
	if(ct == 1){
		if(tenptr == NULL){
        		tenptr = salloc(1);
        		sputc(tenptr,10);
		}
		t = mult(tenptr,q);
		release(q);
		return(t);
	}
	return(q);
}



/*
 *  NAME:  mult
 *
 *  FUNCTION:  Pops the top two elements on the stack, multiplies them 
		and places the answer back on the top of the stack. 
 *
 *  RETURN VALUE:
 *
 */
struct blk *
mult(p,q)
struct blk *p,*q;
{
	register struct blk *mp,*mq,*mr;
	int sign,offset,carry;
	int cq,cp,mt,mcr;
	int temp;

	offset = sign = 0;
	fsfile(p);
	mp = p;
	if(sfbeg(p) == 0){
		if((temp=sbackc(p)) < 0){
			mp = copy(p,length(p));
			chsign(mp);
			sign = ~sign;
		}
	}
	fsfile(q);
	mq = q;
	if(sfbeg(q) == 0){
		if((temp=sbackc(q)) < 0){
			mq = copy(q,length(q));
			chsign(mq);
			sign = ~sign;
		}
	}
	mr = salloc(length(mp)+length(mq));
	zero(mr);
	rewind(mq);
	while(sfeof(mq) == 0){
		cq = sgetc(mq);
		rewind(mp);
		rewind(mr);
		mr->rd += offset;
		carry=0;
		while(sfeof(mp) == 0){
			cp = sgetc(mp);
			mcr = sfeof(mr)?0:slookc(mr);
			mt = cp*cq + carry + mcr;
			carry = mt/100;
			salterc(mr,mt%100);
		}
		offset++;
		if(carry != 0){
			mcr = sfeof(mr)?0:slookc(mr);
			salterc(mr,mcr+carry);
		}
	}
	if(sign < 0){
		chsign(mr);
	}
	if(mp != p)release(mp);
	if(mq != q)release(mq);
	return(mr);
}



/*
 *  NAME:  chsign
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
chsign(p)
struct blk *p;
{
	register int carry;
	register int ct;

	carry=0;
	rewind(p);
	while(sfeof(p) == 0){
		ct=100-slookc(p)-carry;
		carry=1;
		if(ct>=100){
			ct -= 100;
			carry=0;
		}
		salterc(p,ct);
	}
	if(carry != 0){
		sputc(p,-1);
		fsfile(p);
		sbackc(p);
		ct = sbackc(p);
		if(ct == 99){
			truncate(p);
			sputc(p,-1);
		}
	}
	else{
		fsfile(p);
		ct = sbackc(p);
		if(ct == 0)truncate(p);
	}
	return;
}


/*
 *  NAME:  readc
 *
 *  FUNCTION:  Reads characters in from either stdin or from a specified 	
		file.  
 *
 *  RETURN VALUE:  The character read in.
 *
 */
readc(){
	while(1) {
		if((readptr != &readstk[0]) && (*readptr != 0)){
			if(sfeof(*readptr) == 0)
				return(lastchar = sgetc(*readptr));
			release(*readptr);
			readptr--;
		}
		else {
			lastchar = getc(curfile);
			if(lastchar != EOF)return(lastchar);
			if(readptr != &readptr[0]){
				readptr--;
				if(*readptr == 0)curfile = stdin;
			}
			else {
				if(curfile != stdin){
					fclose(curfile);
					curfile = stdin;
				}
				else exit(0);
			}
		}
	}
}



/*
 *  NAME:  unreadc
 *
 *  FUNCTION:  Puts c back into the file or removes it from the stack. 
 *
 *  RETURN VALUE:  none
 *
 */
unreadc(c)
char c;
{
	if((readptr != &readstk[0]) && (*readptr != 0)){
		sungetc(*readptr,c);
	}
	else ungetc((int)c,curfile);
	return;
}



/*
 *  NAME:  binop
 *
 *  FUNCTION:  Based on the value passed in, this function calls the correct
 *		binary operation procedure.
 *
 *  RETURN VALUE: none
 *
 */
binop(c)
char c;
{
	register struct blk *r;

	switch(c){
	case '+':
		r = add(arg1,arg2);
		break;
	case '*':
		r = mult(arg1,arg2);
		break;
	case '/':
		r = div(arg1,arg2);
		break;
	}
	release(arg1);
	release(arg2);
	sputc(r,savk);
	pushp(r);
	return;
}



/*
 *  NAME:  print
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
print(hptr)
struct blk *hptr;
{
	int sc;
	register struct blk *p,*q,*dec;
	int dig,dout,ct;
	int temp;

	rewind(hptr);
	while(sfeof(hptr) == 0){
		if((temp=sgetc(hptr)) > 99){
			rewind(hptr);
			while(sfeof(hptr) == 0){
				printf("%c",sgetc(hptr));
			}
			printf("\n");
			return;
		}
	}
	fsfile(hptr);
	sc = sbackc(hptr);
	if(sfbeg(hptr) != 0){
		printf("0\n");
		return;
	}
	count = ll;
	p = copy(hptr,length(hptr));
	sunputc(p);
	fsfile(p);
	if((temp=sbackc(p)) < 0){
		chsign(p);
		OUTC('-');
	}
	if((obase == 0) || (obase == -1)){
		oneot(p,sc,'d');
		return;
	}
	if(obase == 1){
		oneot(p,sc,'1');
		return;
	}
	if(obase == 10){
		tenot(p,sc);
		return;
	}
	create(strptr);
	dig = log10*sc;
	dout = ((dig/10) + dig) /logo;
	dec = getdec(p,sc);
	p = removc(p,sc);
	while(length(p) != 0){
		q = div(p,basptr);
		release(p);
		p = q;
		(*outdit)(rem,0);
	}
	release(p);
	fsfile(strptr);
	while(sfbeg(strptr) == 0)OUTC(sbackc(strptr));
	if(sc == 0){
		release(dec);
		printf("\n");
		return;
	}
	create(strptr);
	OUTC('.');
	ct=0;
	do{
		q = mult(basptr,dec);
		release(dec);
		dec = getdec(q,sc);
		p = removc(q,sc);
		(*outdit)(p,1);
	}while(++ct < dout);
	release(dec);
	rewind(strptr);
	while(sfeof(strptr) == 0)OUTC(sgetc(strptr));
	printf("\n");
	return;
}



/*
 *  NAME:  getdec
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
struct blk *
getdec(p,sc)
struct blk *p;
{
	int cc;
	register struct blk *q,*t,*s;

	rewind(p);
	if(length(p)*2 < sc){
		q = copy(p,length(p));
		return(q);
	}
	q = salloc(length(p));
	while(sc >= 1){
		sputc(q,sgetc(p));
		sc -= 2;
	}
	if(sc != 0){
		if(tenptr == NULL){
        		tenptr = salloc(1);
        		sputc(tenptr,10);
		}
		t = mult(q,tenptr);
		s = salloc(cc = length(q));
		release(q);
		rewind(t);
		while(cc-- > 0)sputc(s,sgetc(t));
		sputc(s,0);
		release(t);
		if(tenptr == NULL){
        		tenptr = salloc(1);
        		sputc(tenptr,10);
		}
		t = div(s,tenptr);
		release(s);
		release(rem);
		return(t);
	}
	return(q);
}


/*
 *  NAME:  tenot
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
tenot(p,sc)
struct blk *p;
{
	register int c,f;

	fsfile(p);
	f=0;
	while((sfbeg(p) == 0) && ((p->rd-p->beg-1)*2 >= sc)){
		c = sbackc(p);
		if((c<10) && (f == 1))printf("0%d",c);
		else printf("%d",c);
		f=1;
		TEST2;
	}
	if(sc == 0){
		printf("\n");
		release(p);
		return;
	}
	if((p->rd-p->beg)*2 > sc){
		c = sbackc(p);
		printf("%d.",c/10);
		TEST2;
		OUTC(c%10 +'0');
		sc--;
	}
	else {
		OUTC('.');
	}
	if(sc > (p->rd-p->beg)*2){
		while(sc>(p->rd-p->beg)*2){
			OUTC('0');
			sc--;
		}
	}
	while(sc > 1){
		c = sbackc(p);
		if(c<10)printf("0%d",c);
		else printf("%d",c);
		sc -= 2;
		TEST2;
	}
	if(sc == 1){
		OUTC(sbackc(p)/10 +'0');
	}
	printf("\n");
	release(p);
	return;
}



/*
 *  NAME:  oneot
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
oneot(p,sc,ch)
struct blk *p;
char ch;
{
	register struct blk *q;

	q = removc(p,sc);
	create(strptr);
	sputc(strptr,-1);
	while(length(q)>0){
		p = add(strptr,q);
		release(q);
		q = p;
		OUTC(ch);
	}
	release(q);
	printf("\n");
	return;
}



/*
 *  NAME:  hexot
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
hexot(p,flg)
struct blk *p;
{
	register int c;
	rewind(p);
	if(sfeof(p) != 0){
		sputc(strptr,'0');
		release(p);
		return;
	}
	c = sgetc(p);
	release(p);
	if(c >= 16){
		printf( MSGSTR(HDO16,"hex digit > 16"));/*MSG*/
		return;
	}
	sputc(strptr,c<10?c+'0':c-10+'A');
	return;
}



/*
 *  NAME:  bigot
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
bigot(p,flg)
struct blk *p;
{
	register struct blk *t,*q;
	register int l;
	int neg;
	int temp;

	if(flg == 1)t = salloc(0);
	else{
		t = strptr;
		l = length(strptr)+fw-1;
	}
	neg=0;
	if(length(p) != 0){
		fsfile(p);
		if((temp=sbackc(p)) < 0){
			neg=1;
			chsign(p);
		}
		while(length(p) != 0){
			if(tenptr == NULL){
        			tenptr = salloc(1);
        			sputc(tenptr,10);
			}
			q = div(p,tenptr);
			release(p);
			p = q;
			rewind(rem);
			sputc(t,sfeof(rem)?'0':sgetc(rem)+'0');
			release(rem);
		}
	}
	release(p);
	if(flg == 1){
		l = fw1-length(t);
		if(neg != 0){
			l--;
			sputc(strptr,'-');
		}
		fsfile(t);
		while(l-- > 0)sputc(strptr,'0');
		while(sfbeg(t) == 0)sputc(strptr,sbackc(t));
		release(t);
	}
	else{
		l -= length(strptr);
		while(l-- > 0)sputc(strptr,'0');
		if(neg != 0){
			sunputc(strptr);
			sputc(strptr,'-');
		}
	}
	sputc(strptr,' ');
	return;
}



/*
 *  NAME:  add
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
struct blk *
add(a1,a2)
struct blk *a1,*a2;
{
	register struct blk *p;
	register int carry,n;
	int size;
	int c,n1,n2;

	size = length(a1)>length(a2)?length(a1):length(a2);
	p = salloc(size);
	rewind(a1);
	rewind(a2);
	carry=0;
	while(--size >= 0){
		n1 = sfeof(a1)?0:sgetc(a1);
		n2 = sfeof(a2)?0:sgetc(a2);
		n = n1 + n2 + carry;
		if(n>=100){
			carry=1;
			n -= 100;
		}
		else if(n<0){
			carry = -1;
			n += 100;
		}
		else carry = 0;
		sputc(p,n);
	}
	if(carry != 0)sputc(p,carry);
	fsfile(p);
	if(sfbeg(p) == 0){
		while(sfbeg(p) == 0 && (c = sbackc(p)) == 0);
		if(c != 0)salterc(p,c);
		truncate(p);
	}
	fsfile(p);
	if(sfbeg(p) == 0 && sbackc(p) == -1){
		while((c = sbackc(p)) == 99){
			if(c == EOF)break;
		}
		sgetc(p);
		salterc(p,-1);
		truncate(p);
	}
	return(p);
}



/*
 *  NAME:  eqk
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
eqk(){
	register struct blk *p,*q;
	register int skp;
	int skq;

	p = pop();
	EMPTYS;
	q = pop();
	EMPTYSR(p);
	skp = sunputc(p);
	skq = sunputc(q);
	if(skp == skq){
		arg1=p;
		arg2=q;
		savk = skp;
		return(0);
	}
	else if(skp < skq){
		savk = skq;
		p = add0(p,skq-skp);
	}
	else {
		savk = skp;
		q = add0(q,skp-skq);
	}
	arg1=p;
	arg2=q;
	return(0);
}



/*
 *  NAME:  removc
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
struct blk *
removc(p,n)
struct blk *p;
{
	register struct blk *q,*r;

	rewind(p);
	while(n>1){
		sgetc(p);
		n -= 2;
	}
	q = salloc(2);
	while(sfeof(p) == 0)sputc(q,sgetc(p));
	if(n == 1){
		if(tenptr == NULL){
        		tenptr = salloc(1);
        		sputc(tenptr,10);
		}
		r = div(q,tenptr);
		release(q);
		release(rem);
		q = r;
	}
	release(p);
	return(q);
}



/*
 *  NAME:  scalint
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
struct blk *
scalint(p)
struct blk *p;
{
	register int n;
	n = sunputc(p);
	p = removc(p,n);
	return(p);
}



/*
 *  NAME:  scale
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
struct blk *
scale(p,n)
struct blk *p;
{
	register struct blk *q,*s,*t;

	t = add0(p,n);
	q = salloc(1);
	sputc(q,n);
	s = exp(inbas,q);
	release(q);
	q = div(t,s);
	release(t);
	release(s);
	release(rem);
	sputc(q,n);
	return(q);
}



/*
 *  NAME:  subt
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
subt(){
	arg1=pop();
	EMPTYS;
	savk = sunputc(arg1);
	chsign(arg1);
	sputc(arg1,savk);
	pushp(arg1);
	if(eqk() != 0)return(1);
	binop('+');
	return(0);
}



/*
 *  NAME:  command
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
command(){
	int c;
	char line[100],*sl;
	register (*savint)(int),pid,rpid;
	int retcode;

	switch(c = readc()){
	case '<':
		return(cond(NL));
	case '>':
		return(cond(NG));
	case '=':
		return(cond(NE));
	default:
		sl = line;
		*sl++ = c;
		while((c = readc()) != '\n')*sl++ = c;
		*sl = 0;
		if((pid = fork()) == 0){
			execl("/bin/sh","sh","-c",line,0);
			exit(0100);
		}
		savint = (int (*)(int))signal(SIGINT, SIG_IGN);
		while((rpid = wait(&retcode)) != pid && rpid != -1);
		signal(SIGINT, (void (*)(int))savint);
		printf("!\n");
		return(0);
	}
}



/*
 *  NAME:  cond
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
cond(c)
char c;
{
	register struct blk *p;
	register int cc;

	if(subt() != 0)return(1);
	p = pop();
	sunputc(p);
	if(length(p) == 0){
		release(p);
		if(c == '<' || c == '>' || c == NE){
			/* Eat up the first byte. */
			cc = readc() & 0377;

			/* Test for a shift byte. */
			if (NCisshift(cc)) {

			    /* Eat up the second byte of the two-byte NLS */
			    /* character. */
			    readc();
			} /* end if */
			return(0);
		}
		load();
		return(1);
	}
	else {
		if(c == '='){
			release(p);

			/* Eat up the first byte. */
			cc = readc() & 0377;

			/* Test for a shift byte. */
			if (NCisshift(cc)) {
			    /* Eat up the second byte of the two-byte NLS */
			    /* character. */
			    readc();
			} /* end if */
			return(0);
		}
	}
	if(c == NE){
		release(p);
		load();
		return(1);
	}
	fsfile(p);
	cc = sbackc(p);
	release(p);
	if((cc<0 && (c == '<' || c == NG)) ||
		(cc >0) && (c == '>' || c == NL)){

		/* Eat up first byte. */
		cc = readc() & 0377;

		/* Test for a shift byte. */
		if (NCisshift(cc)) {

		    /* Eat up the second byte of the two-byte NLS */
		    /* character. */
		    readc();
		} /* end if */
		return(0);
	}
	load();
	return(1);
}



/*
 *  NAME:  load
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
load(){
	register int c;
	int c1;	/* Holds the second byte read of a two-byte */
		/* NLS character. */
	NLchar nlc;	/* Used as the index into the symbol table. */
	register struct blk *p,*q;
	struct blk *t,*s;
	c = readc() & 0377;


	/* Test for a shift byte. */
	if (NCisshift(c)) {

	    /* Read the second byte of the two-byte NLS */
	    /* character. */
	    c1 = readc() & 0377;
	} /* end if */

	/* Convert to an NLchar. */
	_NCdec2(c,c1,nlc);
#ifndef KJI
	nlc = _NCmap(nlc);
#endif

	/* Index into the symbol table on an NLchar, nlc. */
	sptr = stable[nlc];
	if(sptr != 0){
		p = sptr->val;
		if(nlc >= ARRAYST){
			q = salloc(length(p));
			rewind(p);
			while(sfeof(p) == 0){
				s = getwd(p);
				if(s == 0){putwd(q, (struct blk *)NULL);}
				else{
					t = copy(s,length(s));
					putwd(q,t);
				}
			}
			pushp(q);
		}
		else{
			q = copy(p,length(p));
			pushp(q);
		}
	}
	else{
		q = salloc(1);
		sputc(q,0);
		pushp(q);
	}
	return;
}



/*
 *  NAME:  log2
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
log2(n)
long n;
{
	register int i;

	if(n == 0)return(0);
	i=31;
	if(n<0)return(i);
	while((n= n<<1) >0)i--;
	return(--i);
}



/*
 *  NAME:  salloc
 *
 *  FUNCTION:  Allocates space on the stack.
 *
 *  RETURN VALUE:  A pointer to the top of the stack.
 *
 */
struct blk *
salloc(size)
int size;
{
	register struct blk *hdr;
	register char *ptr;
	all++;
	nbytes += size;
	if (size == 0) 
		ptr = 0;
	else {
		ptr = (char *)malloc((unsigned)size);
		if(ptr == 0){
			garbage("salloc");
			if((ptr = (char *)malloc((unsigned)size)) == 0)
				ospace("salloc");
		}
 	}
	if((hdr = hfree) == 0)hdr = morehd();
	hfree = (struct blk *)hdr->rd;
	hdr->rd = hdr->wt = hdr->beg = ptr;
	hdr->last = ptr+size;
	return(hdr);
}



/*
 *  NAME:  morehd
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
struct blk *
morehd(){
	register struct blk *h,*kk;
	headmor++;
	nbytes += HEADSZ;
	hfree = h = (struct blk *)malloc(HEADSZ);
	if(hfree == 0){
		garbage("morehd");
		if((hfree = h = (struct blk *)malloc(HEADSZ)) == 0)
			ospace("headers");
	}
	kk = h;
	while(h<hfree+(HEADSZ/BLK))(h++)->rd = (char *)++kk;
	(--h)->rd=0;
	return(hfree);
}
/*
sunputc(hptr)
struct blk *hptr;
{
	hptr->wt--;
	hptr->rd = hptr->wt;
	return(*hptr->wt);
}
*/



/*
 *  NAME:  copy
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
struct blk *
copy(hptr,size)
struct blk *hptr;
int size;
{
	register struct blk *hdr;
	register unsigned sz;
	register char *ptr;

	all++;
	nbytes += size;
	sz = length(hptr);
	if (size == 0)
		ptr = 0;
	else {
		ptr = nalloc(hptr->beg, (unsigned)size);
		if(ptr == 0){
			garbage("copy");
			if((ptr = nalloc(hptr->beg, (unsigned)size)) == NULL){
				printf(MSGSTR(PCOPYSIZ,"copy size %d\n")/*MSG*/
				,size);
				ospace("copy");
			}
		}
	}
	if((hdr = hfree) == 0)hdr = morehd();
	hfree = (struct blk *)hdr->rd;
	hdr->rd = hdr->beg = ptr;
	hdr->last = ptr+size;
	hdr->wt = ptr+sz;
	ptr = hdr->wt;
	while(ptr<hdr->last)*ptr++ = '\0';
	return(hdr);
}



/*
 *  NAME:  sdump
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
sdump(s1,hptr)
char *s1;
struct blk *hptr;
{
	char *p;
	printf(MSGSTR(SDUMP,"%s %o rd %o wt %o beg %o last %o\n"),s1,hptr,hptr->rd,hptr->wt,hptr->beg,hptr->last);
	p = hptr->beg;
	while(p < hptr->wt)printf("%d ",*p++);
	printf("\n");
}



/*
 *  NAME:  seekc
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
seekc(hptr,n)
struct blk *hptr;
{
	register char *nn,*p;

	nn = hptr->beg+n;
	if(nn > hptr->last){
		nbytes += nn - hptr->last;
		free((void *)hptr->beg);
		p = (char *)realloc((void *)hptr->beg, (size_t)n);
		if(p == 0){
			hptr->beg = (char *)realloc(hptr->beg, (unsigned)(hptr->last-hptr->beg));
			garbage("seekc");
			if((p = (char *)realloc(hptr->beg, (unsigned)n)) == 0)
				ospace("seekc");
		}
		hptr->beg = p;
		hptr->wt = hptr->last = hptr->rd = p+n;
		return;
	}
	hptr->rd = nn;
	if(nn>hptr->wt)hptr->wt = nn;
	return;
}



/*
 *  NAME:  salterwd
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
salterwd(hptr,n)
struct wblk *hptr;
struct blk *n;
{
	if(hptr->rdw == hptr->lastw)more(hptr);
	*hptr->rdw++ = n;
	if(hptr->rdw > hptr->wtw)hptr->wtw = hptr->rdw;
	return;
}



/*
 *  NAME:  more
 *
 *  FUNCTION:  Allocates more space.
 *
 *  RETURN VALUE:  none
 *
 */
more(hptr)
struct blk *hptr;
{
	register unsigned size;
	register char *p;

	if((size=(hptr->last-hptr->beg)*2) == 0)size=1;
	nbytes += size/2;
	free((void *)hptr->beg);
	p = (char *)realloc((void *)hptr->beg, (size_t)size);
	if(p == 0){
		hptr->beg = (char *)realloc(hptr->beg, (unsigned)(hptr->last-hptr->beg));
		garbage(MSGSTR(MORE, "more"));
		if((p = (char *)realloc(hptr->beg,size)) == 0)
			ospace(MSGSTR(MORE, "more"));
	}
	hptr->rd = hptr->rd-hptr->beg+p;
	hptr->wt = hptr->wt-hptr->beg+p;
	hptr->beg = p;
	hptr->last = p+size;
	return;
}



/*
 *  NAME:  ospace
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
ospace(s)
char *s;
{
	printf(MSGSTR(OS1,"out of space: %s\n"),s);/*MSG*/
	printf(MSGSTR(HEADMOR,"all %ld rel %ld headmor %ld\n"),all,rel,headmor);
	printf(MSGSTR(NBYTES,"nbytes %ld\n"),nbytes);
	sdump(MSGSTR(STK, "stk"),*stkptr);
	abort();
}



/*
 *  NAME:  garbage
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
garbage(s)
char *s;
{
	int i;
	struct blk *p, *q;
	struct sym *tmps;
	int ct;

	for(i=0;i<TBLSZ;i++){
		tmps = stable[i];
		if(tmps != 0){
			if(i < ARRAYST){
				do {
					p = tmps->val;
					if(((int)p->beg & 01)  != 0){
						printf(MSGSTR(STRING,"string %o\n"),i);
						sdump(MSGSTR(ODDBEG, "odd beg"),p);
					}
					redef(p);
					tmps = tmps->next;
				} while(tmps != 0);
				continue;
			}
			else {
				do {
					p = tmps->val;
					rewind(p);
					ct = 0;
					while((q = getwd(p)) != NULL){
						ct++;
						if(q != 0){
							if(((int)q->beg & 01) != 0){
								printf(MSGSTR(ARR,"array %o elt %d odd\n"),i-ARRAYST,ct);
								sdump("elt",q);
							}
							redef(q);
						}
					}
					tmps = tmps->next;
				} while(tmps != 0);
			}
		}
	}
}



/*
 *  NAME:  redef
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
redef(p)
struct blk *p;
{
	register offset;
	register char *newp;

	if ((int)p->beg&01) {
		printf(MSGSTR(ODD,"odd ptr %o hdr %o\n"),p->beg,p);
		ospace(MSGSTR(REFD, "redef-bad"));
	}
	free((void *)p->beg);
	free((void *)dummy);
	dummy = (char *)malloc((size_t)1);
	if(dummy == NULL)ospace(MSGSTR(DUMMY, "dummy"));
	newp = (char *)realloc(p->beg, (unsigned)(p->last-p->beg));
	if(newp == NULL)ospace(MSGSTR(RED, "redef"));
	offset = newp - p->beg;
	p->beg = newp;
	p->rd += offset;
	p->wt += offset;
	p->last += offset;
}



/*
 *  NAME:  release
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
release(p)
register struct blk *p;
{
	rel++;
	nbytes -= p->last - p->beg;
	p->rd = (char *)hfree;
	hfree = p;
	free((void *)p->beg);
}



/*
 *  NAME:  getwd
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
struct blk *
getwd(p)
struct blk *p;
{
	register struct wblk *wp;

	wp = (struct wblk *)p;
	if (wp->rdw == wp->wtw)
		return(NULL);
	return(*wp->rdw++);
}



/*
 *  NAME:  putwd
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
putwd(p, c)
struct blk *p, *c;
{
	register struct wblk *wp;

	wp = (struct wblk *)p;
	if (wp->wtw == wp->lastw)
		more(p);
	*wp->wtw++ = c;
}



/*
 *  NAME:  lookwd
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
struct blk *
lookwd(p)
struct blk *p;
{
	register struct wblk *wp;

	wp = (struct wblk *)p;
	if (wp->rdw == wp->wtw)
		return(NULL);
	return(*wp->rdw);
}



/*
 *  NAME:  nalloc
 *
 *  FUNCTION:
 *
 *  RETURN VALUE:
 *
 */
char *
nalloc(p,nbytes)
register char *p;
unsigned nbytes;
{
	register char *q, *r;
	q = r = (char *) malloc(nbytes);
	if(q==0)
		return(0);
	while(nbytes--)
		*q++ = *p++;
	return(r);
}
