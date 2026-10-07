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
 * $Log: b.c,v $
 * Revision 1.2  1994/11/19  01:31:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/18  14:46:50  shala
 * Files for new version of awk command.
 *
 * Revision 2.6  90/12/06  14:10:01  devrcs
 * 	Fixed up comment headers.
 * 	[90/12/03  13:31:20  gm]
 * 
 * Revision 2.5  90/10/07  16:46:33  devrcs
 * 	fix to increment the pointer correctly in match function.
 * 	[90/10/03  14:00:42  kumi]
 * 
 * Revision 2.4  90/09/13  12:11:24  devrcs
 * 	Cleanup, RCS header, NLS, MSG
 * 	[90/08/29  09:58:16  knight]
 * 
 * Revision 2.3  90/07/05  23:25:43  devrcs
 * 	Use AIX source
 * 	[90/06/29  21:49:35  lehotsky]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: b.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:31:30 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) b.c
 *
 * FUNCTION: alnum, alpha, alt, aprint, cclenter, cfoll, cgoto, colval, concat,
 * digit, first, follow, freefa, freetr, jalpha, jdigit, jhira, jkanji, jkata,
 * jparen, jpunct, jspace, jxdigit, lower, makedfa, makeinit, match, member,
 * mkdfa, myvoidfunc, nematch, overflo, penter, pmatch, primary, punct, regexp,
 * relex, reparse, space, unary, upper, and xdigit
 *
 * ORIGINS: 3,10, 27
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
 *      All Rights Reserved
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.
 *
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 *
 * b.c  1.10  com/cmd/edit/awk,3.1,9021 4/2/90 15:24:26
 */

/*
Copyright (c) 1984, 19885, 1986, 1987 AT&T
        All Rights Reserved

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.

The copyright notice above does not evidence any
actual or intended publication of such source code.
*/

/*
   Due to the potentially large number of discrete values that can
   satisfy a range (n-n) or charclass ([:jkanji:]), they are kept
   in an intermediate form. The highest "compacted" value is NLCOLMAX,
   and the highest collation value is less than that (although in
   theory a JLS table could assign higher values...), we use NLCOLMAX
   for JLS and 1000 (the max. collation value in NLS is ~900).
   A value TOPVAL+CCL indicates that the following data are in
   characters are the start and end of a range, and TOPVAL+CHAR that
   class (the values are in awk.h, and the calculation done by lex, see
   awk.lx.l). The array of characters created in cgoto is the
   character values of the collation elements bracketed by the two
   values.

   These modifications (and this comment) were lifted from those of "old awk".
 */


#include "awk.h"
#include <stdio.h>
#include "awk.g.h"

#define HAT     (NCHARS-1)      /* matches ^ in regular expr */
                                /* NCHARS is 2**n */
#define MAXLIN 	1024

#ifdef KJI
#define TOPVAL  NLCOLMAX+1
#else
#define TOPVAL  1000
#endif

#define type(v)         (v)->nobj
#define left(v)         (v)->narg[0]
#define right(v)        (v)->narg[1]
#define parent(v)       (v)->nnext

#define LEAF    case CCL: case NCCL: case CHAR: case DOT: case FINAL: case ALL:
#define UNARY   case STAR: case PLUS: case QUEST:

/* encoding in tree Nodes:
        leaf (CCL, NCCL, CHAR, DOT, FINAL, ALL):
                left is index, right contains value or pointer to value
        unary (STAR, PLUS, QUEST): left is child, right is null
        binary (CAT, OR): left and right are children
        parent contains pointer to parent
*/

/*
        CCL:    character class
        NCCL:   complemented character class
        CHAR:   character
        DOT:    any character
        FINAL:
        ALL:
        STAR:   star -- zero or more
        PLUS:   plus -- one or more
        QUEST:  question mark -- zero or one
        OR:             patern alternation
        CAT:    pattern catenation
*/


int     chars[MAXLIN];
int     setvec[MAXLIN];
int     tmpset[MAXLIN];
Node    *point[MAXLIN];         /* penter sets this.  where's it used? */

int     rtok;           /* next token in current re */
int     rlxval;
uuchar  *rlxstr;
uuchar  *prestr;        /* current position in current re */
uuchar  *lastre;        /* origin of last re */

static  int setcnt;
static  int poscnt;

uuchar  *patbeg;        /* holds RSTART */
int     patlen;                 /* holds RLENGTH */

#define NFA     20      /* cache this many dynamic fa's */
fa      *fatab[NFA];
int     nfatab  = 0;    /* entries in fatab */
fa      *mkdfa();
uuchar *cclenter();

#ifdef _NO_PROTO
wchar_t colval();
#else
wchar_t colval(const wchar_t ch);
#endif

#ifdef NOGOTOTAB
	static int cgoto __(( fa *f, int s, int c, uuchar **t));
#else
	static int cgoto __(( fa *f, int s, int c));
#endif

/*
Once upon a time the "is*" functions were macros, so these had to be
"real" function bodies.  Now, they are real functions, so we can put pointers
to them in the array below.
*/

#define alpha isalpha
#define upper isupper
#define lower islower
#define digit isdigit
#define alnum isalnum
#define space isspace
#define aprint isprint
#define punct ispunct
#define xdigit isxdigit
#define cntrl iscntrl
#define graph isgraph

#ifdef KJI
#define jalpha isjalpha
#define jdigit isjdigit
#define jspace isjspace
#define jpunct isjpunct
#define jparen isjparen
#define jkanji isjkanji
#define jhira isjhira
#define jkata isjkata
#define jxdigit isjxdigit
#endif

struct isarray {
        char *istr;
        uuchar istoken; /* was char */
        int (*isafunc)();
} istab[] = {
        {":alpha:]",   ALPHA,   alpha   },
        {":upper:]",   UPPER,   upper   },
        {":lower:]",   LOWER,   lower   },
        {":digit:]",   DIGIT,   digit   },
        {":alnum:]",   ALNUM,   alnum   },
        {":space:]",   SPACE,   space   },
        {":print:]",   APRINT,  aprint  },
        {":punct:]",   PUNCT,   punct   },
        {":xdigit:]",  XDIGIT,  xdigit  },
        {":cntrl:]",   CNTRL,   cntrl   },
        {":graph:]",   GRAPH,   graph  }
#ifdef KJI
                                         ,
        {":jalpha:]",  JALPHA,  jalpha  },
        {":jdigit:]",  JDIGIT,  jdigit  },
        {":jspace:]",  JSPACE,  jspace  },
        {":jpunct:]",  JPUNCT,  jpunct  },
        {":jparen:]",  JPAREN,  jparen  },
        {":jkanji:]",  JKANJI,  jkanji  },
        {":jhira:]",   JHIRA,   jhira   },
        {":jkata:]",   JKATA,   jkata   },
        {":jxdigit:]", JXDIGIT, jxdigit }
#endif

};

#define NISTAB (sizeof(istab) / sizeof(struct isarray))
struct isarray *sp;
int ilen;


/*
The original algorithm below was intended to do lru replacement
of dynamic dfa's, but fatab[i]-use was never decremented, so
the algorithm was just "least used".

An additional complication was that of the least used, the first array
element found was replaced.
Consider the case in which the code's most likely to be invoked: lots
of dfa's each used once.  The table fills up, makedfa replaces element 0;
The next element appears, it replaces element zero again, etc.
If at this point we begin alternating patterns, it produces worst-case behavior.
*/

fa *makedfa(s, anchor)  /* returns dfa for reg expr s */
        uuchar *s;
        int anchor;
{
        int i, use, nuse;
        fa *pfa;

        nuse = -1;
        if (compile_time)       /* a constant for sure */
                return mkdfa(s, anchor);
        for (i = 0; i < nfatab; i++)    /* is it there already? */
                if (fatab[i]->anchor == anchor && strcmp(fatab[i]->restr,s) == 0) {
                        fatab[i]->use++;
                        nuse = i;
                } else
                        fatab[i]->use--;
        if (nuse >= 0)
                return fatab[nuse];

        pfa = mkdfa(s, anchor);
        if (nfatab < NFA) {     /* room for another */
                fatab[nfatab] = pfa;
                fatab[nfatab]->use = 1;
                nfatab++;
                return pfa;
        }
        use = fatab[0]->use;    /* replace least-recently used */
        nuse = 0;
        for (i = 1; i < nfatab; i++)
                if (fatab[i]->use < use) {
                        use = fatab[i]->use;
                        nuse = i;
                }
        freefa(fatab[nuse]);
        fatab[nuse] = pfa;
        pfa->use = 1;
        return pfa;
}

fa *mkdfa(s, anchor)    /* does the real work of making a dfa */
        uuchar *s;
        int anchor;     /* anchor = 1 for anchored matches, else 0 */
{
        Node *p, *p1, *reparse();
        fa *f;

        p = reparse(s);
        p1 = op2(CAT, op2(STAR, op2(ALL, NIL, NIL), NIL), p);
                /* put ALL STAR in front of reg.  exp. */
        p1 = op2(CAT, p1, op2(FINAL, NIL, NIL));
                /* put FINAL after reg.  exp. */

        poscnt = 0;
        penter(p1);     /* enter parent pointers and leaf indices */
        if (dbg){
                printf ("f = |%u|,sizeof(fa) = |%d|, poscnt*sizeof(rrow) = |%d|\n",f,sizeof(fa),poscnt*sizeof(rrow));
                fflush(stdout);
                }
        if ((f = (fa *) Calloc(1, sizeof(fa) + poscnt*sizeof(rrow))) == NULL)
                overflo(MSGSTR(BNORMFA, "no room for fa"));
        f->accept = poscnt-1;   /* penter has computed number of positions in re */
        cfoll(f, p1);   /* set up follow sets */
        freetr(p1);
        if ((f->posns[0] = (int *) Calloc(1, *(f->re[0].lfollow)*sizeof(int))) == NULL)
                        overflo(MSGSTR(BSPCMKDFA, "out of space in makedfa"));
        if ((f->posns[1] = (int *) Calloc(1, sizeof(int))) == NULL)
                overflo(MSGSTR(BSPCMKDFA, "out of space in makedfa"));
        *f->posns[1] = 0;
        f->initstat = makeinit(f, anchor);
        f->reset = 0;
        f->anchor = anchor;
        f->restr = tostring(s);
        return f;
}

int makeinit(f, anchor)
        fa *f;
        int anchor;
{
        register i, k;

        f->curstat = 2;
        f->out[2] = 0;
        k = *(f->re[0].lfollow);
        xfree(f->posns[2]);
        if ((f->posns[2] = (int *) Calloc(1, (k+1)*sizeof(int))) == NULL)
                overflo(MSGSTR(BSPCMKINIT, "out of space in makeinit"));
        for (i=0; i<=k; i++) {
                (f->posns[2])[i] = (f->re[0].lfollow)[i];
        }
        if ((f->posns[2])[1] == f->accept)
                f->out[2] = 1;
#ifndef NOGOTOTAB
        for (i=0; i<NCHARS; i++)
                f->gototab[2][i] = 0;
#endif
        f->curstat = cgoto(f, 2, HAT, (uuchar **)0);
        if (anchor) {
                *f->posns[2] = k-1;     /* leave out position 0 */
                for (i=0; i<k; i++) {
                        (f->posns[0])[i] = (f->posns[2])[i];
                }

                f->out[0] = f->out[2];
                if (f->curstat != 2)
                        --(*f->posns[f->curstat]);
        }
        return f->curstat;
}

penter(p)       /* set up parent pointers and leaf indices */
        Node *p;
{
        switch(type(p)) {
        LEAF
                left(p) = (Node *) poscnt;
                point[poscnt++] = p;
                break;
        UNARY
                penter(left(p));
                parent(left(p)) = p;
                break;
        case CAT:
        case OR:
                penter(left(p));
                penter(right(p));
                parent(left(p)) = p;
                parent(right(p)) = p;
                break;
        default:
                ERROR MSGSTR(BUNKTYPPNTR, "unknown type %d in penter\n"), type(p) FATAL;
                break;
        }
}

freetr(p)       /* free parse tree */
        Node *p;
{
        switch (type(p)) {
        LEAF
                xfree(p);
                break;
        UNARY
                freetr(left(p));
                xfree(p);
                break;
        case CAT:
        case OR:
                freetr(left(p));
                freetr(right(p));
                xfree(p);
                break;
        default:
                ERROR MSGSTR(BUNKTYPFRTR, "unknown type %d in freetr"), type(p) FATAL;
                break;
        }
}

overflo(s)
        uuchar *s;
{
        ERROR MSGSTR(BRE2BIG, "regular expression too big: %s"), s FATAL;
}

cfoll(f, v)     /* enter follow set of each leaf of vertex v into lfollow[leaf] */
        fa *f;
        register Node *v;
{
        register int i;
        register int *p;

        switch(type(v)) {
        LEAF
                f->re[(int) left(v)].ltype = type(v);
                f->re[(int) left(v)].lval = (int) right(v);
                for (i=0; i<=f->accept; i++)
                        setvec[i] = 0;
                setcnt = 0;
                follow(v);      /* computes setvec and setcnt */
                if ((p = (int *) Calloc(1, (setcnt+1)*sizeof(int))) == NULL)
                        overflo(MSGSTR(BFOLSETOFLW, "follow set overflow"));
                f->re[(int) left(v)].lfollow = p;
                *p = setcnt;
                for (i = f->accept; i >= 0; i--)
                        if (setvec[i] == 1) *++p = i;
                break;
        UNARY
                cfoll(f,left(v));
                break;
        case CAT:
        case OR:
                cfoll(f,left(v));
                cfoll(f,right(v));
                break;
        default:
                ERROR MSGSTR(BUNKTYPCFOIL, "unknown type %d in cfoll"), type(v) FATAL;
        }
}

first(p)                /* collects initially active leaves of p into setvec */
        register Node *p;       /* returns 0 or 1 depending on whether p matches empty string */
{
        register int b;

        switch(type(p)) {
        LEAF
                if (setvec[(int) left(p)] != 1) {
                        setvec[(int) left(p)] = 1;
                        setcnt++;
                }
                if (type(p) == CCL && (*(int *) right(p)) == '\0')
                        return(0);              /* empty CCL */
                else return(1);
        case PLUS:
                if (first(left(p)) == 0) return(0);
                return(1);
        case STAR:
        case QUEST:
                first(left(p));
                return(0);
        case CAT:
                if (first(left(p)) == 0 && first(right(p)) == 0) return(0);
                return(1);
        case OR:
                b = first(right(p));            /* insure both left and right get done */
                if (first(left(p)) == 0 || b == 0) return(0);
                return(1);
        }
        ERROR MSGSTR(BUNKTYPFRST, "unknown type %d in first\n"), type(p) FATAL;
        return(-1);
}

follow(v)
        Node *v;                /* collects leaves that can follow v into setvec */
{
        Node *p;

        if (type(v) == FINAL)
                return;
        p = parent(v);
        switch (type(p)) {
        case STAR:
        case PLUS:
                first(v);
                follow(p);
                return;

        case OR:
        case QUEST:
                follow(p);
                return;

        case CAT:
                if (v == left(p)) {     /* v is left child of p */
                        if (first(right(p)) == 0) {
                                follow(p);
                                return;
                        }
                }
                else            /* v is right child */
                        follow(p);
                return;
        }
}

static int
member(c, s, t) /* is c in s? */
int c;
register *s;
uuchar **t;
{
        register int d;
        uuchar *u;
        short cv, cu;
        u = *t;
        while (*s) {
                switch (*s) {
                case TOPVAL+CCL:
                       cu = NCcoluniq(c);
                       cv = (((cv = NCcollate(c)) < 0) &&
                                _NLxcolu(cv, &u, (wchar_t *)0, &cu));
                        if ((cu >= s[1]) && (cu <= s[2])) {
                                *t = u;
                                return(1);
                        }
                        s += 3;
                        break;
                case TOPVAL+CHAR:
                        d = _NCunmap(c);
                        if ((NCisNLchar(d)) && (*istab[s[1]].isafunc)(d))
                                return(1);
                        s += 2;
                        break;
                default:
                        if (c == *s++)
                                return(1);

                }
        }
        return(0);
}

match(f, p)                                     /* 1 iff fa "f" matches string "p" */
        register fa *f;
        register uuchar *p;
{
        register int s, ns;
        register int pch;
        uuchar *q;

        s = f->reset?makeinit(f,0):f->initstat;
        if (f->out[s])
                return(1);
        do {
                pch = *p;
                if (NCisshift(*p)) {
                        pch = _NCmap(NCdechr(p));
                        p++;
                dprintf("two-byte ");
                }
                dprintf("mapped character in match: 0x%x\n", pch);

#ifndef NOGOTOTAB
                if (ns=f->gototab[s][pch])
                        s=ns;
                else    s=cgoto(f, s, pch);
#else
                q = ++p;
                s=cgoto(f, s, pch, &q);
                p = q;
#endif

                if (f->out[s])
                        return(1);
        } while (pch != 0);
        return(0);
}

pmatch(f, p)                    /* pattern match.  set posbeg to beginning of match */
        register fa *f;
        register uuchar *p;
{
        register s, ns;
        uuchar *q, *r;
        register int qch;
        int i, k;

        s = f->reset?makeinit(f,1):f->initstat;
        patbeg = p;
        patlen = -1;
        do {
                q = p;
                do {
                        if (f->out[s])          /* final state */
                                patlen = q-p;
                        qch = *q;
                        if (NCisshift(*q)) {
                                qch = _NCmap(NCdechr(q));
                                q++;
                        }
#ifndef NOGOTOTAB
                        if (ns=f->gototab[s][qch])
                                s=ns;
                        else    s=cgoto(f, s, qch);
#else
                        r = ++q;
                        s=cgoto(f, s, qch, &r);
                        q = r;
#endif

                        if (s==1)       /* no transition */
                                if (patlen >= 0) {
                                        patbeg = p;
                                        return(1);
                                }
                                else
                                        goto nextin;    /* no match */
                } while (qch != 0);
                if (f->out[s])
                        patlen = q-p-1; /* don't count $ */
                if (patlen >= 0) {
                        patbeg = p;
                        return(1);
                }
        nextin:
                s = 2;
                if (f->reset) {
                        for (i=2; i<=f->curstat; i++)
                                Free(f->posns[i]);
                        k = *f->posns[0];
                        if ((f->posns[2] = (int *) Calloc(1, (k+1)*sizeof(int))) == NULL)
                                overflo(MSGSTR(BSPCPMTCH, "out of space in pmatch"));
                        for (i=0; i<=k; i++)
                                (f->posns[2])[i] = (f->posns[0])[i];
                        f->initstat = f->curstat = 2;
                        f->out[2] = f->out[0];
#ifndef NOGOTOTAB
                        for (i=0; i<NCHARS; i++)
                                f->gototab[2][i] = 0;
#endif
                }
        } while (*(p += (NCisshift(*p)? 2 : 1)) != 0);
        return (0);
}

nematch(f, p)                           /* non-empty match, near dup of pmatch() */
        register fa *f;
        register uuchar *p;
{
        register int s, ns;
        uuchar *q, *r;
        register int qch;
        int i, k;

        s = f->reset?makeinit(f,1):f->initstat;
        patlen = -1;
        while (*p) {
                q = p;
                do {
                        qch = *q;
                        if (NCisshift(*q)) {
                                qch = _NCmap(NCdechr(q));
                                q++;
                        }
                        if (f->out[s])          /* final state */
                                patlen = q-p;
#ifndef NOGOTOTAB
                        if (ns=f->gototab[s][qch])
                                s=ns;
                        else    s=cgoto(f,s,qch);
#else
                        r = ++q;
                        s=cgoto(f,s,qch,&q);
                        q = r;
#endif

                        if (s==1)       /* no transition */
                                if (patlen > 0) {
                                        patbeg = p;
                                        return(1);
                                }
                                else
                                        goto nnextin;   /* no nonempty match */
                } while (qch != 0);
                if (f->out[s])
                        patlen = q-p-1; /* don't count $ */
                if (patlen > 0 ) {
                        patbeg = p;
                        return(1);
                }
        nnextin:
                s = 2;
                if (f->reset) {
                        for (i=2; i<=f->curstat; i++)
                                Free(f->posns[i]);
                        k = *f->posns[0];
                        if ((f->posns[2] = (int *) Calloc(1, (k+1)*sizeof(int))) == NULL)
                                overflo(MSGSTR(BNOSTATSPC, "out of state space"));
                        for (i=0; i<=k; i++)
                                (f->posns[2])[i] = (f->posns[0])[i];
                        f->initstat = f->curstat = 2;
                        f->out[2] = f->out[0];
#ifndef NOGOTOTAB
                        for (i=0; i<NCHARS; i++)
                                f->gototab[2][i] = 0;
#endif
                }
        p += (NCisshift(*p)? 2 : 1);
        }
        return (0);
}

Node *regexp(), *primary(), *concat(), *alt(), *unary();

Node *reparse(p)
        uuchar *p;
{
        /* parses regular expression pointed to by p */
        /* uses relex() to scan regular expression */
        Node *np;

        dprintf("reparse <%s>\n", p);
        lastre = prestr = p;    /* prestr points to string to be parsed */
        rtok = relex();
        if (rtok == '\0')
                ERROR MSGSTR(BEMPTYRE, "empty regular expression") FATAL;
        np = regexp();
        if (rtok == '\0')
                return(np);
        else
                ERROR MSGSTR(BSNTXRE, "syntax error in regular expression %s at %s"), lastre, prestr FATAL;
}

Node *regexp()
{
        return (alt(concat(primary())));
}

Node *primary()
{
        Node *np;
        int c;
        uuchar tmp[2];

        switch (rtok) {
        case SHIFT_CHAR:
                tmp[0] = rlxval;
                rtok = relex();
                tmp[1] = rlxval;
                c = NCdechr(tmp);
                c = _NCmap(c);
                dprintf("primary: two-byte character 0x%x\n", c);
                np = op2(CHAR, NIL, (Node *) c);
                rtok = relex();
                return (unary(np));
        case CHAR:
                np = op2(CHAR, NIL, (Node *) rlxval);
                rtok = relex();
                return (unary(np));
        case ALL:
                rtok = relex();
                return (unary(op2(ALL, NIL, NIL)));
        case DOT:
                rtok = relex();
                return (unary(op2(DOT, NIL, NIL)));
        case CCL:
                np = op2(CCL, NIL, cclenter(rlxstr));
                rtok = relex();
                return (unary(np));
        case NCCL:
                np = op2(NCCL, NIL, cclenter(rlxstr));
                rtok = relex();
                return (unary(np));
        case '^':
                rtok = relex();
                return (unary(op2(CHAR, NIL, (Node *) HAT)));
        case '$':
                rtok = relex();
                return (unary(op2(CHAR, NIL, NIL)));
        case '(':
                rtok = relex();
                if (rtok == ')') {      /* special pleading for () */
                        rtok = relex();
                        return unary(op2(CCL, NIL, tostring("")));
                }
                np = regexp();
                if (rtok == ')') {
                        rtok = relex();
                        return (unary(np));
                }
                else
                        ERROR MSGSTR(BSNTXRE, "syntax error in regular expression %s at %s"), lastre, prestr FATAL;
        default:
                ERROR MSGSTR(BILLPRMRYRE, "illegal primary in regular expression %s at %s"), lastre, prestr FATAL;
        }
}

Node *concat(np)
        Node *np;
{
        switch (rtok) {
        case CHAR: case DOT: case ALL: case CCL: case NCCL: case '$': case '(':
        case SHIFT_CHAR:
                return (concat(op2(CAT, np, primary())));
        default:
                return (np);
        }
}

Node *alt(np)
        Node *np;
{
        if (rtok == OR) {
                rtok = relex();
                return (alt(op2(OR, np, concat(primary()))));
        }
        return (np);
}

Node *unary(np)
        Node *np;
{
        switch (rtok) {
        case STAR:
                rtok = relex();
                return (unary(op2(STAR, np, NIL)));
        case PLUS:
                rtok = relex();
                return (unary(op2(PLUS, np, NIL)));
        case QUEST:
                rtok = relex();
                return (unary(op2(QUEST, np, NIL)));
        default:
                return (np);
        }
}

/*
        Return token type.
        If CHAR, set rlxval to character, if CCL || NCCL, set rlxstr to string
*/
relex()         /* lexical analyzer for reparse */
{
        register int c;
        register int i;
        uuchar cbuf[150];       /* ? overrides cbuf declared in awk.h */
        int clen, cflag;

        switch (c = *prestr++) {
        case '|': return OR;
        case '*': return STAR;
        case '+': return PLUS;
        case '?': return QUEST;
        case '.': return DOT;
        case '\0': prestr--; return '\0';
        case '^':
        case '$':
        case '(':
        case ')':
                return c;
        case '\\':
                if ((c = *prestr++) == 't')
                        c = '\t';
                else if (c == 'n')
                        c = '\n';
                else if (c == 'f')
                        c = '\f';
                else if (c == 'r')
                        c = '\r';
                else if (c == 'b')
                        c = '\b';
                else if (c == '\\')
                        c = '\\';
                else if (isdigit(c)) {
                        int n = c - '0';
                        if (isdigit((int)*prestr)) {
                                n = 8 * n + *prestr++ - '0';
                                if (isdigit((int)*prestr))
                                        n = 8 * n + *prestr++ - '0';
                        }
                        c = n;
                } /* else it's now in c */
        default:
                rlxval = c;
                if (NCisshift((uuchar) c))
                        return SHIFT_CHAR;
                return CHAR;
        case '[':
                clen = 0;
                if (*prestr == '^') {
                        cflag = 1;
                        prestr++;
                }
                else
                        cflag = 0;

                for (;;) {
                        if ((c = *prestr++) == '\\') {
                                cbuf[clen++] = '\\';
                                if ((c = *prestr++) == '\0')
                                        ERROR MSGSTR(BNONTERMCHCL, "nonterminated character class %s"), lastre FATAL;
                                cbuf[clen++] = c;
                        } else if (c == ']') {
                                cbuf[clen] = 0;
                                rlxstr = tostring(cbuf);
                                if (cflag == 0)
                                        return CCL;
                                else
                                        return NCCL;
                        } else if (c == '\n') {
                                ERROR MSGSTR(BNLINCHRCL, "newline in character class %s..."), lastre FATAL;
                        } else if (c == '\0') {
                                ERROR MSGSTR(BNONTERMCHCL, "nonterminated character class %s"), lastre FATAL;
                        } else if (c == '[') {
                                for (i = 0; i < NISTAB; i++) {
                                        sp = &istab[i];
                                        ilen = strlen(sp->istr);
                                        if((strncmp(prestr, sp->istr, ilen))==0) {
                                                cbuf[clen++] = 255; /* problem with size */
                                                c = sp->istoken;
                                                prestr += ilen; /* this may be wrong */
                                                break;
                                        }
                                }
                                cbuf[clen++] = c;
                        } else
                                cbuf[clen++] = c;
                }
        }
}
static int
#ifdef NOGOTOTAB
cgoto(f, s, c, t)
#else
cgoto(f, s, c)
#endif
        fa *f;
        int s, c;
#ifdef NOGOTOTAB
        uuchar **t;
#endif
{
        register int i, j, k;
        register int *p, *q;
        static uuchar nullbyte[] = "";
        uuchar *r;
#ifdef NOGOTOTAB
        if (t != 0)
                r = *t;
        else    r = nullbyte;
#endif
        for (i=0; i<=f->accept; i++)
                setvec[i] = 0;
        setcnt = 0;
        /* compute positions of gototab[s,c] into setvec */
        p = f->posns[s];
        for (i=1; i<=*p; i++) {
                if ((k = f->re[p[i]].ltype) != FINAL) {
                        if (k == CHAR && c == f->re[p[i]].lval
                                || k == DOT && c != 0 && c != HAT
                                || k == ALL && c != 0
#ifdef NOGOTOTAB
                                || k == CCL && member(c, (int *) f->re[p[i]].lval, &r)
                                || k == NCCL && !member(c, (int *) f->re[p[i]].lval, &r) && c != 0 && c != HAT)
#else
                                || k == CCL && member(c, (int *) f->re[p[i]].lval)
                                || k == NCCL && !member(c, (int *) f->re[p[i]].lval) && c != 0 && c != HAT)
#endif
                                {
                                        q = f->re[p[i]].lfollow;
                                        for (j=1; j<=*q; j++) {
                                                if (setvec[q[j]] == 0) {
                                                        setcnt++;
                                                        setvec[q[j]] = 1;
                                                }
                                        }
                                }
                }
        }
        /* determine if setvec is a previous state */
        tmpset[0] = setcnt;
        j = 1;
        for (i = f->accept; i >= 0; i--)
                if (setvec[i]) {
                        tmpset[j++] = i;
                }
        /* tmpset == previous state? */
        for (i=1; i<= f->curstat; i++) {
                p = f->posns[i];
                if ((k = tmpset[0]) != p[0])
                        goto different;
                for (j = 1; j <= k; j++)
                        if (tmpset[j] != p[j])
                                goto different;
                /* setvec is state i */
#ifndef NOGOTOTAB
                f->gototab[s][c] = i;
#else
                if (t != 0)
                        *t = r;
#endif
                return i;
        different:;
        }

        /* add tmpset to current set of states */
        if (f->curstat >= NSTATES-1) {
                f->curstat = 2;
                f->reset = 1;
                for (i=2; i<NSTATES; i++)
                        Free(f->posns[i]);
        }
        else
                ++(f->curstat);
#ifndef NOGOTOTAB
        for (i=0; i<NCHARS; i++)
                f->gototab[f->curstat][i] = 0;
#endif
        if ((p = (int *) Calloc(1, (setcnt+1)*sizeof(int))) == NULL)
                overflo(MSGSTR(BNOSPCCGOTO, "out of space in cgoto"));

        f->posns[f->curstat] = p;
#ifndef NOGOTOTAB
        f->gototab[s][c] = f->curstat;
#endif
        for (i = 0; i <= setcnt; i++)
                p[i] = tmpset[i];
        if (setvec[f->accept])
                f->out[f->curstat] = 1;
        else
                f->out[f->curstat] = 0;
#ifdef NOGOTOTAB
        if (t != 0)
                *t = r;
#endif
        return f->curstat;
}


freefa(f)
        struct fa *f;
{

        register int i;

        if (f == NULL)
                return;
        for (i=0; i<=f->curstat; i++)
                Free(f->posns[i]);
        for (i=0; i<=f->accept; i++)
                Free(f->re[i].lfollow);
        Free(f->restr);
        Free(f);
}

uuchar *cclenter(p)
        register uuchar *p;
{
        register int i, c;
        uuchar *op;

        int cend;
        uuchar cclbuf[MAXLIN];  /* this need not be this big */
        int *q;
        register int ix;
        register int *temp;
        int xtype = 0;

        op = p;
        i = 0;
        while ((c = *p++) != 0) {
                if (NCisshift(c)) {
                        p++;
                        continue;
                }
                if (c == '\\') {
                        if ((c = *p++) == 't')
                                c = '\t';
                        else if (c == 'n')
                                c = '\n';
                        else if (c == 'f')
                                c = '\f';
                        else if (c == 'r')
                                c = '\r';
                        else if (c == 'b')
                                c = '\b';
                        else if (c == '\\')
                                c = '\\';
                        else if (isdigit(c)) {
                                int n = c - '0';
                                if (isdigit((int)*p)) {
                                        n = 8 * n + *p++ - '0';
                                        if (isdigit((int)*p))
                                                n = 8 * n + *p++ - '0';
                                }
                                c = n;
                        } /* else */
                                /* c = c; */
                }
                if (i >= MAXLIN-1)
                        overflo("bracket expression too big");
                cclbuf[i++] = c;
        }
        cclbuf[i++] = '\0';

        /* Now do ranges and Kanji conversions */

        p = cclbuf;
        i = 0;
        for (p = cclbuf; (*p != 0) && (i <= MAXLIN); p++) {
                if (NCisshift (*p))
                {
                        c = NCdechr(p);
                        chars[i++] = _NCmap(c);
                        p++;
                } else if (*p == '-' && i > 0 && chars[i-1] != 0 && *(p+1) != '\0') {
                        p++;
                        if ((xtype == CHAR) || (*p == 255))
                                ERROR MSGSTR(BERRINRE, "error in regexp") FATAL;
                        cend = *p;

                        if (NCisshift ((uuchar) cend)) {
                                cend = NCdechr(p);
                                p++;
                        }
                        c = _NCunmap(chars[i - 1]);
                        if (colval((wchar_t)c) <= colval((wchar_t)cend)) {
                                chars[i - 1] = TOPVAL+CCL;
                                chars[i++] = colval((wchar_t)c);
                                chars[i++] = colval((wchar_t)cend);
                                if (i >= MAXLIN) overflo();
                        }
                } else if (*p == 255) {         /* charclass [:name:] */
                        p++;
                        for (ix = 0; ix < NISTAB; ix++) {
                                if (*p == istab[ix].istoken) {
                                        chars[i++] = TOPVAL+CHAR;
                                        chars[i++] = ix;
                                        xtype = CHAR;
                                        break;
                                }
                        }
                        if (ix >= NISTAB)  p++;
                        continue;
                } else
                        chars[i++] = *p;
        }
        if (i > MAXLIN)
                overflo(MSGSTR(BCHRCL2BIG, "character class too big"));
        chars[i++] = 0;

        /* okay, now make something to return */
        temp = (int *)malloc(i * sizeof(int));
        if (temp == NULL)
                overflo(MSGSTR(BSPCTOSTRING, "out of space in tostring on chars"));

        (void)memcpy((char *) temp, (char *) chars, i * sizeof(int));

        if (dbg) {
                printf ("cclenter: in = |%s|, chars = |", op);
                q = chars;
                while (*q)
                {
                        printf (" %d(0x%x), ",*q, *q);
                        q++;
                }
                printf ("|\n");
                printf ("cclenter: in = |%s|, out = |", op);
                q = temp;
                while (*q)
                        printf (" %c, ",*q++);
                printf ("|\n");
                printf ("cclenter: in = |%s|, out (in hex) = |", op);
                q = temp;
                while (*q)
                {
                        printf (" %d(0x%x), ",*q, *q);
                        q++;
                }
                printf ("|\n");
        }
        xfree(op);
        return((uuchar *) temp);
}

#ifndef DEBUG
void myvoidfunc(){}
#endif

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
        short cvalue, uvalue;

        ib = ifbuf;
        if (ch > 256)
                *ib++ = (ch >> 8);
        *ib++ = (ch & 0xff);
        *ib = '\0';
        uvalue = NCcoluniq(ch);
        ((cvalue = NCcollate(ch)) < 0) &&
                        (cvalue = _NLxcolu(cvalue, &ib, (wchar_t *)0, &uvalue));
        return (uvalue);
}
