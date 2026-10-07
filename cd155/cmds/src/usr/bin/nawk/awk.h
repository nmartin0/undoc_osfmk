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
 * $Log: awk.h,v $
 * Revision 1.2  1994/11/19  01:31:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/18  14:46:37  shala
 * Files for new version of awk command.
 *
 * Revision 1.4  90/12/06  14:09:55  devrcs
 * 	Fixed up comment headers.
 * 	[90/12/03  13:31:04  gm]
 * 
 * Revision 1.3  90/09/13  12:11:18  devrcs
 * 	Conditionalized OSF1 specific code under MAKETAB.
 * 	[90/09/07  07:45:00  gm]
 * 
 * 	Cleanup, RCS header, NLS, MSG
 * 	[90/08/29  09:57:13  knight]
 * 
 * Revision 1.2  90/07/05  23:25:35  devrcs
 * 	Populate from AIX sources
 * 	[90/06/29  21:41:34  lehotsky]
 * 
 * $EndLog$
 */
/*
 * @(#)$RCSfile: awk.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:31:26 $
 */
/*
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * awk.h       1.8  com/cmd/edit/awk,3.1,9021 4/2/90 15:24:09
 */

/*
Copyright (c) 1984, 19885, 1986, 1987 AT&T
        All Rights Reserved

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.

The copyright notice above does not evidence any
actual or intended publication of such source code.
*/

#ifdef MAKETAB
#include <ctype.h>
#else
#include <NLctype.h>
#endif

#define NOGOTOTAB

typedef double  Awkfloat;
/* uchar used by AT&T is part of types.h on present platform    */
typedef unsigned char uuchar;

#ifdef MYALLOC
        extern void *Malloc(), *Calloc();
#else
#       define  Malloc  malloc
#       define  Calloc  calloc
#       define  Free    free
#endif

#define xfree(a)        { if ((a) != NULL) { Free(a); a = NULL; } }

#ifdef  DEBUG
#       define  dprintf if(dbg)printf
#else
        void myvoidfunc();
#       define  dprintf myvoidfunc
#endif

extern  char    errbuf[200];
#define ERROR   sprintf(errbuf,
#define FATAL   ), error(1, errbuf)
#define WARNING ), error(0, errbuf)
#define SYNTAX  ), yyerror(errbuf)

extern int      compile_time;   /* 1 if compiling, 0 if running */

#define RECSIZE (3 * 1024)      /* sets limit on records, fields, etc., etc. */

extern uuchar   **FS;
extern uuchar   **RS;
extern uuchar   **ORS;
extern uuchar   **OFS;
extern uuchar   **OFMT;
extern Awkfloat *NR;
extern Awkfloat *FNR;
extern Awkfloat *NF;
extern uuchar   **FILENAME;
extern uuchar   **SUBSEP;
extern Awkfloat *RSTART;
extern Awkfloat *RLENGTH;

extern uuchar   *record;
extern int      dbg;
extern int      lineno;
extern int      errorflag;
extern int      donefld;        /* 1 if record broken into fields */
extern int      donerec;        /* 1 if record is valid (no fld has changed) */

#define CBUFLEN 400
extern uuchar   cbuf[CBUFLEN];  /* miscellaneous character collection */

extern  uuchar  *patbeg;        /* for RSTART */
extern  int     patlen;                 /* for RLENGTH */

/* Cell:  all information about a variable or constant */

typedef struct Cell {
        uuchar  ctype;          /* OCELL, OBOOL, OJUMP, etc. */
        uuchar  csub;           /* CCON, CTEMP, CFLD, etc. */
        uuchar  *nval;          /* name, for variables only */
        uuchar  *sval;          /* string value */
        Awkfloat fval;          /* value as number */
        unsigned tval;          /* type info: STR|NUM|ARR|FCN|FLD|CON|DONTFREE */
        struct Cell *cnext;     /* ptr to next if chained */
} Cell;

typedef struct {                /* symbol table array */
        int     nelem;          /* elements in table right now */
        int     size;           /* size of tab */
        Cell    **tab;          /* hash table pointers */
} Array;

#define NSYMTAB 50      /* initial size of a symbol table */
extern Array    *symtab, *makesymtab();
extern Cell     *setsymtab(), *lookup();

extern Cell     *recloc;        /* location of input record */
extern Cell     *nrloc;         /* NR */
extern Cell     *fnrloc;        /* FNR */
extern Cell     *nfloc;         /* NF */
extern Cell     *rstartloc;     /* RSTART */
extern Cell     *rlengthloc;    /* RLENGTH */

/* Cell.tval values: */
#define NUM     01      /* number value is valid */
#define STR     02      /* string value is valid */
#define DONTFREE 04     /* string space is not freeable */
#define CON     010     /* this is a constant */
#define ARR     020     /* this is an array */
#define FCN     040     /* this is a function name */
#define FLD     0100    /* this is a field $1, $2, ... */
#define REC     0200    /* this is $0 */

#define freeable(p)     (!((p)->tval & DONTFREE))

Awkfloat setfval(), getfval();
uuchar  *setsval(), *getsval();
uuchar  *tostring(), *tokname(), *qstring();
double  log(), sqrt(), exp(), atof();

/* function types */
#define FLENGTH 1
#define FSQRT   2
#define FEXP    3
#define FLOG    4
#define FINT    5
#define FSYSTEM 6
#define FRAND   7
#define FSRAND  8
#define FSIN    9
#define FCOS    10
#define FATAN   11
#define BLENGTH 12

/* Node:  parse tree is made of nodes, with Cells at bottom */

typedef struct Node {
        int     ntype;
        struct  Node *nnext;            /* parent (back pointer) */
        int     lineno;
        int     nobj;                                   /* type: CCL, NCCL, etc. */
        struct Node *narg[1];   /* variable: actual size set by calling malloc */
} Node;

#define NIL     ((Node *) 0)

extern Node     *winner;
extern Node     *nullstat;                      /* ? appears unused */
extern Node     *nullnode;

/* ctypes */
#define OCELL   1
#define OBOOL   2
#define OJUMP   3

/* Cell subtypes: csub */
#define CCOPY   6
#define CCON    5
#define CTEMP   4
#define CNAME   3
#define CVAR    2
#define CFLD    1

/* bool subtypes */
#define BTRUE   11
#define BFALSE  12

/* jump subtypes */
#define JEXIT   21
#define JNEXT   22
#define JBREAK  23
#define JCONT   24
#define JRET    25

/* node types */
#define NVALUE  1
#define NSTAT   2
#define NEXPR   3
#define NFIELD  4

extern  Cell    *(*proctab[])();
extern  Cell    *nullproc();
extern  Cell    *relop();
extern  int     pairstack[], paircnt;
extern  Cell    *fieldadr();

extern  Node    *stat1(), *stat2(), *stat3(), *stat4(), *pa2stat();
extern  Node    *op1(), *op2(), *op3(), *op4();
extern  Node    *linkum(), *valtonode(), *rectonode(), *exptostat();
extern  Node    *makearr();

#define notlegal(n)     (n <= FIRSTTOKEN || n >= LASTTOKEN || proctab[n-FIRSTTOKEN] == nullproc)
#define isvalue(n)      ((n)->ntype == NVALUE)
#define isexpr(n)       ((n)->ntype == NEXPR)
#define isjump(n)       ((n)->ctype == OJUMP)
#define isexit(n)       ((n)->csub == JEXIT)
#define isbreak(n)      ((n)->csub == JBREAK)
#define iscont(n)       ((n)->csub == JCONT)
#define isnext(n)       ((n)->csub == JNEXT)
#define isret(n)        ((n)->csub == JRET)
#define isstr(n)        ((n)->tval & STR)
#define isnum(n)        ((n)->tval & NUM)
#define isarr(n)        ((n)->tval & ARR)
#define isfunc(n)       ((n)->tval & FCN)
#define istrue(n)       ((n)->csub == BTRUE)
#define istemp(n)       ((n)->csub == CTEMP)

#ifdef KJI
#define NCHARS (NLCOLMAX +1)            /* 16384 */
#define NSTATES 256
#else
#define NCHARS (NLCHARMAX +1)		/* was 128+1 in distributed version */ 
#define NSTATES 256			/* was 16 in distributed version */
#endif

typedef struct rrow {
        int     ltype;
        int     lval;
        int     *lfollow;
} rrow;

typedef struct fa {
        uuchar  *restr;                /* original regular expression string */
        int     anchor;                                   /* anchored match? */
        int     use;       /* how recently used? (for cacheing dynamic fa's) */
#ifndef NOGOTOTAB
        uuchar  gototab[NSTATES][NCHARS];    /* 4 meg apiece in KJI version! */
#endif
        int     *posns[NSTATES];
        uuchar  out[NSTATES];           /* 1 if accept state (adjoins FINAL) */
        int     initstat;                              /* return of makeinit */
        int     curstat;
        int     accept;                                 /* position of FINAL */
        int     reset;                                    /* states flushed? */
        struct  rrow re[1];          /* one per state, starting with state 2 */
} fa;

extern  fa      *makedfa();

#ifdef MAKETAB
#define MSGSTR(n,s)     s
#else
#include        "awk_msg.h"
nl_catd 		catd;
#define MSGSTR(n,s)     catgets(catd,MS_AWK,n,s)
#endif

# define ALPHA 1                /* used by lx and b  for regexp */
# define UPPER 2
# define LOWER 3
# define DIGIT 4
# define ALNUM 5
# define SPACE 6
# define APRINT 7
# define PUNCT 8
# define XDIGIT 9
# define CNTRL 10
# define GRAPH 11

#ifdef KJI
# define JALPHA 12
# define JDIGIT 13
# define JSPACE 14
# define JPUNCT 15
# define JPAREN 16
# define JKANJI 17
# define JHIRA 18
# define JKATA 19
# define JXDIGIT 20
#endif
