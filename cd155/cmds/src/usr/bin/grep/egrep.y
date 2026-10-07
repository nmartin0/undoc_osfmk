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
/* @(#)$RCSfile: egrep.y,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:25:50 $ */
/*
 * COMPONENT_NAME: (CMDSCAN) commands that scan files
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
 * egrep.y	1.18  com/cmd/scan,3.1,9021 5/8/90 13:37:50 
 */
/*
 * egrep -- print lines containing (or not containing) a regular expression
 *
 *      status returns:
 *              0 - ok, and some matches
 *              1 - ok, but no matches
 *              2 - some error
 */

/* for 3.1, tokens are started at 16129 rather than the YACC default of 257
   If size of largest compacted character or largest allocated collation
   value ever goes beyond this number then this number must be changed
   (or else change YACC!!)

   1. Size of gotofn array varies depending on NLS or JLS. The maximum
      possible value for NLS is ~ 900; so we give it 1000. For JLS,
      it probably will not exceed 12000, but could, in theory, be
      up to 32K. 16128 will be used...

   2. Because ranges must be expressed in collating values and all other
      in character values,  new array (gotofn1) is implemented.
      Positions in this is in colval rather than in charval. If there
      are any values in this array, it will be searched before the
      standard one. This may cause some incompatibilities with previous
      precedence ordering.

   3. Character classes are supported within brackets.

   4. The intermediate format in the "chars" array is changed. Ranges
      are identified by the CCL token, followed by the two endpoints.
      Multi-char ranges (a-d-f-z) are broken up in their components.
      Character classes are identified by the CHAR token, followed by
      the index into the istab array. Both these formats are expanded
      to the normal format when moved to gotofn and gotofn1.

 */
%token CHAR 16129
%token DOT 16130
%token CCL 16131
%token NCCL 16132
%token OR  16133
%token CAT 16134
%token STAR 16135
%token PLUS 16136
%token QUEST 16137
%left OR
%left CHAR DOT CCL NCCL '('
%left CAT
%left STAR PLUS QUEST

%{
#include <stdio.h>
#include <locale.h>
#include <NLctype.h>

#define MAXPOS 4000
#define MAXLIN 1000
#ifndef  KJI
#define NSTATES 256
#define NCHARS 256
#define NTOP 1000
#else
#define NSTATES 256
#define NCHARS NLCOLMAX+NLCHARMAX
#define NTOP 16128
#endif
#define FINAL -1
#include <nl_types.h>
#include "egrep_msg.h"
nl_catd catd;
#define MSGSTR(n,s)     catgets(catd,MS_EGREP,n,s)
int nchars;
char gotofn[NSTATES][NTOP];
char gotofn1[NSTATES][NTOP];
wchar_t symbol[NTOP];
wchar_t colsym[NTOP];
int colis[NSTATES];
int state[NSTATES];
char out[NSTATES];
int line = 1;
int name[MAXLIN];
int left[MAXLIN];
int right[MAXLIN];
int parent[MAXLIN];
int foll[MAXLIN];
int positions[MAXPOS];
NLchar chars[MAXLIN];
int nxtpos;
int nxtchar = 0;
int tmpstat[MAXLIN];
int initstat[MAXLIN];
int xstate;
int count;
int icount;
unsigned char *input;
#define         BSIZE   512
long    lnum;           /* current line number.                        */
int     bflag;          /* print block number of matches               */
int     cflag;          /* print total line count of matches           */
int     fflag;          /* get pattern from a file.                    */
int     lflag;          /* print name of file once then quit searching */
int     nflag;          /* print line number of each match             */
int     hflag = 1;      /* don't print file name on grep of multi files*/
int 	iflag;		/* ignore case				       */
int     sflag;          /* silent mode. Don't print anything but errors*/
int     vflag;          /* everything but.  The NOT case.              */
int     eflag;          /* expression follows                          */
int     nfile;          /* Number of files in the argument list.       */
extern  char *optarg;
extern  int optind;
FILE    *expfile;
int     blkno;          /* current offset block in the file.           */
long    tln;            /* total number of lines that had a match      */
int     nsucc;          /* return code.                                */
char 	lcasebuf[BSIZE*2+1];	/* buffer with original line */
char	buf[BSIZE*2+1];	/* buffer with possibly changed line */
static void lower();	

#ifdef _NO_PROTO
wchar_t colval();
#else
wchar_t colval(const wchar_t ch);
int _NLxcolu(int desc, char **src, wchar_t **xstr, wchar_t *uval);
#endif

/* As the "is" functions aren't functions, but macros, we cannot put    */
/* the "function" in the array below; thus another layer of indirection */

ALPHA(c) {return(NCisalpha(c));}
UPPER(c) {return(NCisupper(c));}
LOWER(c) {return(NCislower(c));}
DIGIT(c) {return(NCisdigit(c));}
ALNUM(c) {return(NCisalnum(c));}
SPACE(c) {return(NCisspace(c));}
PRINT(c) {return(NCisprint(c));}
PUNCT(c) {return(NCispunct(c));}
XDIGIT(c) {return(NCisxdigit(c));}
CNTRL(c) {return(NCiscntrl(c));}
GRAPH(c) {return(NCisgraph(c));}
#ifdef KJI
JALPHA(c) {return(isjalpha(c));}
JDIGIT(c) {return(isjdigit(c));}
JSPACE(c) {return(isjspace(c));}
JPUNCT(c) {return(isjpunct(c));}
JPAREN(c) {return(isjparen(c));}
JKANJI(c) {return(isjkanji(c));}
JHIRA(c) {return(isjhira(c));}
JKATA(c) {return(isjkata(c));}
JXDIGIT(c) {return(isjxdigit(c));}
#endif
struct isarray {
        char *isstr;
        int (*isfunc)();
} istab[] = {
        { "[:alpha:]", ALPHA },
        { "[:upper:]", UPPER },
        { "[:lower:]", LOWER },
        { "[:digit:]", DIGIT },
        { "[:alnum:]", ALNUM },
        { "[:space:]", SPACE },
        { "[:print:]", PRINT },
        { "[:punct:]", PUNCT },
        { "[:xdigit:]", XDIGIT },
        { "[:cntrl:]", CNTRL },
        { "[:graph:]", GRAPH }


#ifdef KJI
                                ,
        { "[:jalpha:]", JALPHA },
        { "[:jdigit:]", JDIGIT },
        { "[:jspace:]", JSPACE },
        { "[:jpunct:]", JPUNCT },
        { "[:jparen:]", JPAREN },
        { "[:jkanji:]", JKANJI },
        { "[:jhira:]", JHIRA },
        { "[:jkata:]", JKATA },
        { "[:jxdigit:]", JXDIGIT }
#endif

#define NISTAB (sizeof(istab) / sizeof(struct isarray))
};
int     ix;

int     f;
%}

%%
s:      t
                ={ unary(FINAL, $1);
                  line--;
                }
        ;
t:      b r
                ={ $$ = node(CAT, $1, $2); }
        | OR b r OR
                ={ $$ = node(CAT, $2, $3); }
        | OR b r
                ={ $$ = node(CAT, $2, $3); }
        | b r OR
                ={ $$ = node(CAT, $1, $2); }
        ;
b:
                ={ $$ = enter(DOT);
                   $$ = unary(STAR, $$); }
        ;
r:      CHAR
                ={ $$ = enter($1); }
        | DOT
                ={ $$ = enter(DOT); }
        | CCL
                ={ $$ = cclenter(CCL); }
        | NCCL
                ={ $$ = cclenter(NCCL); }
        ;

r:      r OR r
                ={ $$ = node(OR, $1, $3); }
        | r r %prec CAT
                ={ $$ = node(CAT, $1, $2); }
        | r STAR
                ={ $$ = unary(STAR, $1); }
        | r PLUS
                ={ $$ = unary(PLUS, $1); }
        | r QUEST
                ={ $$ = unary(QUEST, $1); }
        | '(' r ')'
                ={ $$ = $2; }
        | error
        ;

%%
yyerror(s) {
        fprintf(stderr, "egrep: %s\n", s);
        exit(2);
}

yylex() {
        extern int yylval;
        int cclcnt, x;
        int last = 0;
        register int c, d;
        switch(c = nextch()) {
            case '$':
            case '^': c = '\n';
                goto defchar;
            case '|': return (OR);
            case '*': return (STAR);
            case '+': return (PLUS);
            case '?': return (QUEST);
            case '(': return (c);
            case ')': return (c);
            case '.': return (DOT);
            case '\0': return (0);
            case '\n': return (OR);
            case '[':
               x = CCL;
               cclcnt = 0;
               count = nxtchar++;
               if ((c = nextsym()) == '^') {
                       x = NCCL;
                       c = nextsym();
               }
               do {
                 if (c == '\0') synerror();
                 if (c == '-' && cclcnt > 0 && chars[nxtchar-1] != 0) {
                      if ((d = nextsym()) != 0) {
                          if ((last == CHAR) || (d == CHAR))
                               synerror();

                          c = _NCunmap(chars[nxtchar-1]);
                          while (c < d) {
                              if (nxtchar>= MAXLIN) overflo();
                              ++c;
                              chars[nxtchar++] = _NCmap(c);
                              cclcnt++;
                           }
                           last = CCL;
                           continue;
                       }
                  }
                  if (c == CHAR) {
                      if (last == CCL) synerror();
                      chars[nxtchar++] = CHAR;
                      cclcnt++;
                      chars[nxtchar++] = ix;
                      cclcnt++;
                      last = CHAR;
                      continue;
                  }

                  if (nxtchar >= MAXLIN) overflo();
                  chars[nxtchar++] = _NCmap(c);
                  cclcnt++;
               } while ((c = nextsym()) != ']');
               chars[count] = cclcnt;
               return (x);
            case '\\':
                if ((c = nextch()) == '\0') synerror();
            defchar:
            default:
                yylval = _NCmap(c);
                last = 0;
                return (CHAR);
        }
}

#define IFBUFLEN 16

int
nextsym()
{
        register int c, d;
        static char ifbuf[IFBUFLEN];
        static char *ib = ifbuf;
        static char *eb = &ifbuf[IFBUFLEN-2];
        if      (*ib != '\0')
                        return(c = *ib++);
        else    if ((c = nextch()) != '[')
                        return (c);

        switch (c = nextch()) {
        case ':':
                ib = ifbuf;
                *ib++ = '[';
                do {
                   if ((c == '\0') || (c == '\n'))
                                synerror();
                   *ib++ = c;
                } while (((c = nextch()) != ']') &&
                        (c != '-') && (c != '[') &&
                        (ib < eb));
                *ib++ = c;
                *ib = '\0';
                for (ix = 0; ix < NISTAB; ix++)
                        if((strcmp(ifbuf,istab[ix].isstr))==0)
                                return(CHAR);
                if (c == ']') {
                        unnextch(c);
                        *--ib = '\0';
                }
                ib = ifbuf;
                return(c = *ib++);
        default:
                unnextch(c);
                return('[');
        }

}
int
nextch() {
        register int c;
        register int c1;
        if (fflag) {
#ifdef KJI
                if ((c = getwc(expfile)) == EOF) return 0;
#else
                if ((c = getc(expfile)) == EOF) return(0);
#endif
#ifndef KJI
                if (NCisshift (c)) {
                        if ((c1 = getc(expfile)) == EOF) return(0);
                        if (_NCdec2 (c, c1, c) == 1) ungetc (c1, expfile);
                }
#endif
        }
        else {
                c = *input++;
                if (NCisshift (c)) {
                        c1 = *input++;
                        if (_NCdec2 (c, c1, c) == 1) input--;
                }
        }
	if (iflag)	/* ignore case */
		c = NCtolower(c);
        return(c);
}
unnextch(c) int c; {
        if (fflag)
                ungetwc(c, expfile);
        else {
                if (c > 0xff)
                        input -= 2;
                else
                        input--;
        }
}

wchar_t
#ifdef _NO_PROTO
colval(ch)
wchar_t ch;
#else
colval(const wchar_t ch)
#endif
{
        char ifbuf[16];
        char *ib;
        int cv;
        wchar_t uv;

        ib = ifbuf;
        if (ch > 256)
                *ib++ = (ch >> 8);
        *ib++ = (ch & 0xff);
        *ib = '\0';
        uv = NCcoluniq(ch);
        cv = (((cv = NCcollate(ch)) < 0) &&
                                (_NLxcolu(cv, &ib, (wchar_t **)0, &uv)));
        return (uv);
}


synerror() {
        fprintf(stderr, MSGSTR(SYNERR,"egrep: syntax error\n"));        /*MSG*/
        exit(2);
}

enter(x) int x; {
        if(line >= MAXLIN) overflo();
        name[line] = x;
        left[line] = 0;
        right[line] = 0;
        return(line++);
}

cclenter(x) int x; {
        register int linno;
        linno = enter(x);
        right[linno] = count;
        return (linno);
}

node(x, l, r) {
        if(line >= MAXLIN) overflo();
        name[line] = x;
        left[line] = l;
        right[line] = r;
        parent[l] = line;
        parent[r] = line;
        return(line++);
}

unary(x, d) {
        if(line >= MAXLIN) overflo();
        name[line] = x;
        left[line] = d;
        right[line] = 0;
        parent[d] = line;
        return(line++);
}
/*
 * NAME: ovrflow
 *
 * FUNCTION: print error and exit
 *
 * RETURN VALUE:        none (exit)
 */

overflo() {
        fprintf(stderr,
        MSGSTR(NOSPACE,"egrep: regular expression requires too much space\n")); /*MSG*/
        exit(2);
}

cfoll(v) {
        register int i;
        if (left[v] == 0) {
                count = 0;
                for (i=1; i<=line; i++) tmpstat[i] = 0;
                follow(v);
                add(foll, v);
        }
        else if (right[v] == 0) cfoll(left[v]);
        else {
                cfoll(left[v]);
                cfoll(right[v]);
        }
}
cgotofn()
{
        register int c, i, k;
        int n, s;
        int l, m, o;
        int j, nc, pc, pos;
        int curpos, num;
        int number, newpos;
        count = 0;
        for (n=3; n<=line; n++) tmpstat[n] = 0;
        if (cstate(line-1)==0) {
                tmpstat[line] = 1;
                count++;
                out[0] = 1;
        }
        for (n=3; n<=line; n++) initstat[n] = tmpstat[n];
        count--;                /*leave out position 1 */
        icount = count;
        tmpstat[1] = 0;
        add(state, 0);
        n = 0;
        for (s=0; s<=n; s++)  {
                if (out[s] == 1) continue;
                for (i=0; i<nchars; i++) {
                                symbol[i] = 0;
                                colsym[i] = 0;
                                }
                num = positions[state[s]];
                count = icount;
                for (i=3; i<=line; i++) tmpstat[i] = initstat[i];
                pos = state[s] + 1;
                for (i=0; i<num; i++) {
                        curpos = positions[pos];
                        if ((c = name[curpos]) >= 0) {
                                switch(c)  {
                                case DOT:
                                        for (k=0; k<nchars; k++) {
                                                if (k!=colval('\n'))
                                                        symbol[k] = 1;
                                        };
                                        break;
                                case CCL:
                                        nc = chars[right[curpos]];
                                        pc = right[curpos] + 1;
                                        for (k = 0; k < nc; k++) {
                                            switch(chars[pc]) {
                                            case CCL:
                                                 l = colval(_NCunmap(chars[pc+1]));
                                                 m = colval(_NCunmap(chars[pc+2]));
                                                 while (l <=  m)
                                                        colsym[l++] = 1;
                                                 k += 2;
                                                 pc += 3;
                                                 break;
                                            case CHAR:
                                                 pc++;
                                                 for(j=0; j<NCHARS; j++) {
                                                 l = _NCunmap(j);
                                                 if ((NCisNLchar(l) &&
                                                 (*istab[chars[pc]].isfunc)(l)))
                                                   symbol[j] = 1;
                                                 }
                                                 pc++;
                                                 k++;
                                                 break;
                                            default:
                                                 symbol[chars[pc++]] = 1;
                                                 break;
                                            }
                                        }
                                        break;
                                case NCCL:
                                        nc = chars[right[curpos]];
                                        for (j = 0; j < NCHARS; j++) {
                                                if (!NCisNLchar(j))
                                                        continue;
                                                else o = colval(j);
                                            pc = right[curpos] + 1;
                                            for (k = 0; k < nc; k++) {
                                              switch(chars[pc]) {
                                              case CCL:
                                                   k += 2;
                                                   l = colval(_NCunmap(chars[pc+1]));
                                                   m = colval(_NCunmap(chars[pc+2]));
                                                   if ((l <= o) && (o <= m)) {
                                                        pc +=3;
                                                        goto cont;
                                                   } else pc +=3;
                                                   break;
                                              case CHAR:
                                                   pc++;
                                                   if ((*istab[chars[pc]].isfunc)(_NCunmap(j)))
                                                        goto cont;
                                                   break;
                                              default:
                                                  if (j==chars[pc++]) {
                                                                goto cont;
                                                        } else  pc++;
                                                  break;
                                              }
                                            }
                                                if (o != colval('\n')) {
                                                        symbol[j] = 1;
                                                        colsym[o] = 1;
                                                }
                                                cont:;
                                        }
                                        break;
                                break;
                                default:
                                        if (c < nchars) symbol[c] = 1;
                /*                      else printf(MSGSTR(FUNNY,"something's funny\n"));       /*MSG*/
                                }
                        }
                        pos++;
                }
                for (c=0; c<nchars; c++) {
                        if ((symbol[c] == 1) || (colsym[c] == 1))
                                {
                                if (colsym[c] == 1) colis[s] = 1;
                                count = icount;
                                for (i=3;i <= line; i++) tmpstat[i]=initstat[i];
                                pos = state[s] + 1;
                                for (i=0; i<num; i++) {
                                    curpos = positions[pos];
                                    if ((k = name[curpos]) >= 0)
                                        if (
                                               (k == c)
                                            || (k == DOT)
                                            || (k == CCL &&
                                                member(c, right[curpos], 1))
                                            || (k == NCCL &&
                                                member(c, right[curpos], 0))
                                           ) {
                                                number=positions[foll[curpos]];
                                                newpos = foll[curpos] + 1;
                                                for (k=0; k<number; k++) {
                                                    if (tmpstat[positions[newpos]] != 1) {
                                                        tmpstat[positions[newpos]] = 1;
                                                        count++;
                                                    }
                                                newpos++;
                                                }
                                             }
                                        pos++;
                                } /* end nextstate */
                                if (notin(n)) {
                                        if (n >= NSTATES) overflo();
                                        add(state, ++n);
                                        if (tmpstat[line] == 1) out[n] = 1;
                                        if (symbol[c] == 1)
                                                gotofn[s][c] = n;
                                        if (colsym[c] == 1)
                                                gotofn1[s][c] = n;
                                }
                                else {
                                        if (symbol[c] == 1)
                                                gotofn[s][c] = xstate;
                                        if (colsym[c] == 1)
                                                gotofn1[s][c] = xstate;
                                }
                        }
                }
        }
}

cstate(v) {
        register int b;
        if (left[v] == 0) {
                if (tmpstat[v] != 1) {
                        tmpstat[v] = 1;
                        count++;
                }
                return(1);
        }
        else if (right[v] == 0) {
                if (cstate(left[v]) == 0) return (0);
                else if (name[v] == PLUS) return (1);
                else return (0);
        }
        else if (name[v] == CAT) {
                if (cstate(left[v]) == 0 && cstate(right[v]) == 0) return (0);
                else return (1);
        }
        else { /* name[v] == OR */
                b = cstate(right[v]);
                if (cstate(left[v]) == 0 || b == 0) return (0);
                else return (1);
        }
}


member(symb, set, torf) {
        register int i, j, num, pos;
        num = chars[set];
        pos = set + 1;
        for (i=0; i<num; i++)
                switch(chars[pos]) {
                case CCL:               /* symb value is a colval */
                             if ((colval(chars[pos+1]) <= symb) &&
                                                (colval(chars[pos+2]) >= symb))
                                                return(torf);
                             i += 2;
                             pos += 3;
                             break;
                case CHAR:              /* symb is _NCmap(char) */
                             i++;
                             pos++;
                             if ((*istab[chars[pos++]].isfunc)(_NCunmap(symb)))
                                                return(torf);
                             break;
                default:                /* symb is char (?) */
                             if (symb == chars[pos++]) return (torf);
                             break;
                }
        return (!torf);
}


notin(n) {
        register int i, j, pos;
        for (i=0; i<=n; i++) {
                if (positions[state[i]] == count) {
                        pos = state[i] + 1;
                        for (j=0; j < count; j++)
                                if (tmpstat[positions[pos++]] != 1) goto nxt;
                        xstate = i;
                        return (0);
                }
                nxt: ;
        }
        return (1);
}

add(array, n) int *array; {
        register int i;
        if (nxtpos + count > MAXPOS) overflo();
        array[n] = nxtpos;
        positions[nxtpos++] = count;
        for (i=3; i <= line; i++) {
                if (tmpstat[i] == 1) {
                        positions[nxtpos++] = i;
                }
        }
}

/*
 * NAME: follow
 *
 * FUNCTION: check for a match
 *
 * RETURN VALUE: none
 */

follow(v) int v; {
        int p;
        if (v == line) return;
        p = parent[v];
        switch(name[p]) {
                case STAR:
                case PLUS:      cstate(v);
                                follow(p);
                                return;

                case OR:
                case QUEST:     follow(p);
                                return;

                case CAT:       if (v == left[p]) {
                                        if (cstate(right[p]) == 0) {
                                                follow(p);
                                                return;
                                        }
                                }
                                else follow(p);
                                return;
                case FINAL:     if (tmpstat[line] != 1) {
                                        tmpstat[line] = 1;
                                        count++;
                                }
                                return;
        }
}


main(argc, argv)
char **argv;
{
        register int c;
        int errflg = 0;

        extern nl_catd catd;
        catd = catopen(MF_EGREP,0);
        (void) setlocale(LC_ALL,"");

                while(( c = getopt(argc, argv, "bshice:f:lnv")) != EOF)
                        switch(c) {

                        case 'b':               /* display block numbers */
                                bflag++;
                                continue;

                        case 'c':               /* display count of matches */
                                cflag++;
                                continue;

                        case 'e':               /* pattern */
                                eflag++;
                                input = (unsigned char*)optarg;
                                continue;

                        case 'f':               /* get pattern from file */
                                fflag++;
                                expfile = fopen(optarg,"r");
                                if (expfile == NULL) {
                                     fprintf(stderr,
                                     MSGSTR(CANTOPEN,"egrep: can't open %s\n"),optarg); /*MSG*/
                                     exit(2);
                                }
                                continue;

                        case 'h':               /* don't print file names */
                                hflag = 0;
                                continue;

			case 'i':		/* ignore case */
				iflag++;
				continue;

                        case 'l':               /* list file once on match */
                                lflag++;
                                continue;

                        case 'n':               /* print line numbers */
                                nflag++;
                                continue;

                        case 's':               /* silent mode */
                                sflag++;
                                continue;

                        case 'v':               /* the NOT case */
                                vflag++;
                                continue;

                        case '?':
                                errflg++;
                }

                argc -= optind;
                if (errflg || ((argc <= 0) && !(fflag || eflag))) {
                        printf(MSGSTR(USGE,"usage: egrep [ -bchilnsv ] [ -e exp ] [ -f file ] [ strings ] [ file ] ...\n")); /*MSG*/
                        exit(2);
                }
                if ( !eflag  && !fflag ) {
                        input = (unsigned char*)argv[optind];
                        optind++;
                        argc--;
        }
                nchars = NTOP;


        yyparse();

        cfoll(line-1);
        cgotofn();
        nfile = argc;
        argv = &argv[optind];
        if (argc<=0) {
                if (lflag) exit(1);
                execute((char *)NULL);
        }
        else
                while ( --argc >= 0 ) {
                        execute(*argv);
                        argv++;
                }
        exit((nsucc == 2) ? 2 : (nsucc == 0));
}

/*
 * NAME: execute
 *
 * FUNCTION: Look for a match line by line for the argument file.
 *
 * RETURN VALUE: none
 */

execute(file)
char *file;
{
        register char *p;
        register int cstat;
        register int cstat1;
        int cvalue;
        wchar_t uvalue;
        char *q;
        register int ccount;
        char *nlp;
        int istat;
        int c;
        if (file) {
                if ((f = open(file, 0)) < 0) {
                      fprintf(stderr, MSGSTR(CANTOPEN,"egrep: can't open %s\n"), file); /*MSG*/
                        nsucc = 2;
                        return;
                }
        }
        else f = 0;
        ccount = 0;
        lnum = 1;
        tln = 0;
        blkno = 0;
        p = buf;
        nlp = iflag ? lcasebuf : buf;
        if ((ccount = read(f,p,BSIZE))<=0) goto done;
	if (iflag) {
		p[ccount] = '\0';
		lower(p);
	}
        istat = cstat = gotofn[0][_NCmap('\n')];
        if (out[cstat]) goto found;
        for (;;) {
                if ((ccount == 1) && NCisshift(p[0])) {
                        if (p < &buf[BSIZE*2])
                                p++;
                        else  {
                                buf[0] = *p;
                                p = &buf[1];
                        }
                        if ((ccount = read(f, p, 1)) == 1) {
                                ccount = 2;
                                p--;
                        } else  {
                                p--;
                                ccount = 1;
                        }
                }
                if (_NCdec2 (*p, p[1], c) == 2) {
                   p++;
                   --ccount;
                }
                if (colis[cstat]) {
                        q = p;
                        q++;
                        uvalue = NCcoluniq(c);
                        cvalue = (((cvalue = NCcollate(c)) < 0) &&
                                        (_NLxcolu(cvalue, &q, (wchar_t **)0, &uvalue)));
			/*
			 * cstat -> current state
			 * gotofn1 -> transition function for character class
			 * gotofn -> transition function 
			 * 
			 * If it's not obvious, this is using finite
			 * state machine algorithm. Regretable it's
			 * nondeterministic, so we check for other 
			 * transitions and always transition to the
			 * highest state.  This forces it to be
			 * deterministic (given input, only one
			 * transition).  PTM 40994
			 */

			if ( ( (cstat1 = gotofn[cstat][uvalue] ) || 
				( gotofn1[cstat][_NCmap(c)]) >=
				(gotofn[cstat][_NCmap(c)]) ) ) {
	                        if (cstat1 = gotofn1[cstat][uvalue]) {
       	                        	cstat = cstat1;
                                	p = --q;
                        	} else  cstat = gotofn[cstat][_NCmap(c)];
			} else cstat = gotofn[cstat][_NCmap(c)];
                } else cstat = gotofn[cstat][_NCmap(c)];
                if (out[cstat]) {
                found:  for(;;) {
                                if (*p++ == '\n') {
                                        if (vflag == 0) {
                                succeed: nsucc = (nsucc == 2) ? 2 : 1;
                                        if (cflag) tln++;
                                        else if (sflag)
                                                ;       /* silent mode */
                                        else if (lflag) {
                                                printf("%s\n", file);
                                                close(f);
                                                return;
                                        }
                                        else {
                                                if (nfile>1 && hflag) printf("%s:",file);
                                                if (bflag) printf("%d:", blkno);
                                                if (nflag) printf("%ld:", lnum);
                                                if (iflag ? (p - buf) <= (nlp - lcasebuf) : p <= nlp) {
                                                        while (nlp < (iflag ? &lcasebuf[BSIZE*2] : &buf[BSIZE*2]))
                                                                putchar(*nlp++);
                                                        nlp = iflag ? lcasebuf : buf;
                                                }
                                                while (*nlp != '\n') 
						    putchar(*nlp++);
						putchar('\n');
                                        }
                                }
                                        lnum++;
                                        nlp = iflag ? lcasebuf + (p - buf) : p;
                                        if((out[(cstat=istat)]) == 0) goto brk2;
                                }
                                cfound:
                                if (--ccount <= 0) {
                                        if (p <= &buf[BSIZE]) {
                                                if ((ccount = read(f, p, BSIZE)) <= 0) goto done;
                                        }
                                        else if (p == &buf[BSIZE*2]) {
                                                p = buf;
                                                if ((ccount = read(f, p, BSIZE)) <= 0) goto done;
                                        }
                                        else {
                                                if ((ccount = read(f, p, &buf[BSIZE*2]-p)) <= 0) goto done;
                                        }
					if (iflag) {
						p[ccount] = '\0';
						lower(p);
					}
                                        blkno++;
                                }
                        }
                }
                if (*p++ == '\n') {
                        if (vflag) goto succeed;
                        else {
                                lnum++;
                                nlp = iflag ? lcasebuf + (p - buf) : p;
                                if (out[(cstat=istat)]) goto cfound;
                        }
                }
                brk2:
                if (--ccount <= 0) {
                        if (p <= &buf[BSIZE]) {
                                if ((ccount = read(f, p, BSIZE)) <= 0) break;
                        }
                        else if (p == &buf[BSIZE*2]) {
                                p = buf;
                                if ((ccount = read(f, p, BSIZE)) <= 0) break;
                        }
                        else {
                                if ((ccount = read(f, p, &buf[BSIZE*2] - p)) <= 0) break;
                        }
			if (iflag) {
				p[ccount] = '\0';
				lower(p);
			}
                        blkno++;
                }
        }
done:   close(f);
        if (cflag) {
                if (nfile > 1)
                        printf("%s:", file);
                printf("%ld\n", tln);
        }
}

/*
 * Convert buffer p to lower case, 
 * Save a copy of the original in lcasebuf.
 */
static void
lower(char *p1)
{
	int cnt;
	char twochr[2]={0};
	NLchar tmp;
	char *save = lcasebuf + (p1 - buf);

	while (*p1 != '\0')
	{
		*save++ = twochr[0] = *p1;
		if (NCisshift((int)twochr[0]))
			*save++ = twochr[1] = *(p1+1);
		NCdecode (twochr,&tmp);
		tmp = NCtolower((int) tmp);
		cnt = NCencode (&tmp, twochr);
		*p1++ = twochr[0];
		if (cnt == 2)
			*p1++ = twochr[1];
	}
	*save = '\0';
}
