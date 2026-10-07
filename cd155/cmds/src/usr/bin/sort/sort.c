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
 * OSF/1 Release 1.0.3
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: sort.c,v $ $Revision: 1.4 $ (OSF) $Date: 1995/02/10 17:56:29 $";
#endif
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: sort
 *
 * ORIGINS: 3, 27
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
 * sort.c	1.24  com/cmd/files,3.1,9021 5/7/90 16:13:38
 *
 */

/*   fixed for peachtree enhancement, MAXMEM, TREEZ and cmpa()     */
/*   please refer to H.Stuettgen and A.Schuur  id: HJS-S           */

/*   This version of the sort for OSF/1 (from AIX 3.1) uses strxfrm and strcoll.
 *   To increase performance in the NLS case, the sort is not done
 *   in wchar_t's, but in char format. For a 'plain' sort, i.e.
 *   without any keys or flags, the compare is done using strcoll.
 *   If keys are specified, or folding/dictionary/printable flags,
 *   keys are extracted and converted to strxfrm format and prepended
 *   to the record. String compares are then done using strcmp.
 *   On final pass, the keys are stripped before records are written
 *   to output file.
 *   Max. record size is set to 20K. For keyed operations, the max.
 *   size is 3 times this (60K incl. strxfrm format keys).
 *   In checksort and merge operations, keys are not prepended but
 *   compared "in situ"; actual record max length is 20K.
 *   The "-A" option uses old-fashioned AT&T algorithms; the performance
 *   is about 10 times the collating sort. Use it when you can!
 */
/*
 * NAME: sort
 * FUNCTION: Sorts or merges files
 * FLAGS
 *  -A     Sorts on a byte-by-byte basis using ASCII character values.
 *  -b     Ignores leading blanks, spaces, and tabs
 *  -c     Checks that the input is sorted according to the ordering rules
 *  -d     Sorts in dictionary order.
 *  -f     Merges uppercase and lowercases letters.
 *  -i     Sorts only by character in the ASCII range octal 040 - 0176
 *  -m     Merges only, the input is already sorted
 *  -n     Sorts any initial numeric strings
 *  -o fl  Directs output to fl instead of stdout
 *  -r     reverses the order of the specified sort
 *  -t ch  Sets field separator character to char.
 *  -u     Suppresses all but one in each set of equal lines.
 *  -T dir Places all the tempory files that are created in the directory "dir"
 */
#include <NLctype.h>
#include <stdio.h>
#include <locale.h>
#define ISDIGIT isdigit
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <values.h>
#include <sys/resource.h>	/* for getrlimit() */
#include <nl_types.h>
#include "sort_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_SORT,Num,Str)
#define N       16
#define	C	20
#define NF	10
#define MTHRESH  8  /* threshhold for doing median of 3 qksort selection */
#define TREEZ  512  /* no less than N and best if power of 2   HJS-S-C   */
#define LINE  1024
#define SORT_LINE_MAX  20480

/*
 * Memory administration
 *
 * Using a lot of memory is great when sorting a lot of data.
 * Using a megabyte to sort the output of `who' loses big.
 * MAXMEM, MINMEM and DEFMEM define the absolute maximum,
 * minimum and default memory requirements.  Administrators
 * can override any or all of these via defines at compile time.
 * Users can override the amount allocated (within the limits
 * of MAXMEM and MINMEM) on the command line.
 */

#ifndef	MAXMEM
#define MAXMEM  4194304 /* 4  Megabyte maximum */      /*   HJS-S-C   */
#endif

#ifndef	MINMEM
#define	MINMEM	  16384	/* 16K minimum */
#endif

#ifndef	DEFMEM
#define DEFMEM    262144      /* start 256kb HJS-S-C  */
#endif


#define ASC 	0
#define NUM	1
#define XSTR	2
#define XNUM    3


#define	blank(c) ((c)==' ' || (c)=='\t')

int     Aflag;       /* used to signal the -A option ... i.e. byte compare */
int	posflag;	/* used to signal the +pos option */
char    nlcp[2];        /* wchar_t pair represented as chars */

FILE	*os;
char	*dirtry[] = {"/var/tmp", "/usr/tmp", "/tmp", '\0'};
char	**dirs;
char	file1[100];
char	*file = file1;
char	*filep;
#define NAMEOHD 12 /* sizeof("/stm00000aa") */
int	nfiles;
int	*lspace;
int	*maxbrk;
unsigned tryfor;
unsigned alloc;
char bufin[BUFSIZ], bufout[BUFSIZ];	/* Use setbuf's to avoid malloc calls.
					** malloc seems to get heartburn
					** when brk returns storage.
					*/
char tbuf[SORT_LINE_MAX];		/* buffers for strxfrm use */
char ebuf[SORT_LINE_MAX];
char xbuf[SORT_LINE_MAX*3+1];		/* assume strxfrm < 3x data */
char *te = tbuf + SORT_LINE_MAX-1;
char *ee = ebuf + SORT_LINE_MAX-1;
char *xe = xbuf + (SORT_LINE_MAX*3);	

int	maxrec;

int 	mflg;
int	nway;
int	cflg;
int	uflg;
int	outflag = 0;
char	*outfil;
int 	unsafeout;	/*kludge to assure -m -o works*/
int 	eargc;
char	**eargv;
struct btree {
    char *rp;
    int  rn;
} tree[TREEZ], *treep[TREEZ];
int	blkcnt[TREEZ];
long	wasfirst = 0, notfirst = 0;
int	bonus;
char	**blkcur[TREEZ];
wchar_t tabchar;
struct lconv *loc;
wchar_t dec;
wchar_t	decmon;
unsigned char zero[256];
				/* The following tables are not used  by the
				 * normal sort. However, the "-A" option uses
				 * them...
				 */
unsigned char	fold[256] = {		/* table folds ASCII lowers to uppers */
	0000,0001,0002,0003,0004,0005,0006,0007,
	0010,0011,0012,0013,0014,0015,0016,0017,
	0020,0021,0022,0023,0024,0025,0026,0027,
	0030,0031,0032,0033,0034,0035,0036,0037,
	0040,0041,0042,0043,0044,0045,0046,0047,
	0050,0051,0052,0053,0054,0055,0056,0057,
	0060,0061,0062,0063,0064,0065,0066,0067,
	0070,0071,0072,0073,0074,0075,0076,0077,
	0100,0101,0102,0103,0104,0105,0106,0107,
	0110,0111,0112,0113,0114,0115,0116,0117,
	0120,0121,0122,0123,0124,0125,0126,0127,
	0130,0131,0132,0133,0134,0135,0136,0137,
	0140,0101,0102,0103,0104,0105,0106,0107,
	0110,0111,0112,0113,0114,0115,0116,0117,
	0120,0121,0122,0123,0124,0125,0126,0127,
	0130,0131,0132,0173,0174,0175,0176,0177,
	0200,0201,0202,0203,0204,0205,0206,0207,
	0210,0211,0212,0213,0214,0215,0216,0217,
	0220,0221,0222,0223,0224,0225,0226,0227,
	0230,0231,0232,0233,0234,0235,0236,0237,
	0240,0241,0242,0243,0244,0245,0246,0247,
	0250,0251,0252,0253,0254,0255,0256,0257,
	0260,0261,0262,0263,0264,0265,0266,0267,
	0270,0271,0272,0273,0274,0275,0276,0277,
	0300,0301,0302,0303,0304,0305,0306,0307,
	0310,0311,0312,0313,0314,0315,0316,0317,
	0320,0321,0322,0323,0324,0325,0326,0327,
	0330,0331,0332,0333,0334,0335,0336,0337,
	0340,0341,0342,0343,0344,0345,0346,0347,
	0350,0351,0352,0353,0354,0355,0356,0357,
	0360,0361,0362,0363,0364,0365,0366,0367,
	0370,0371,0372,0373,0374,0375,0376,0377
};
unsigned char nofold[256] = {
	0000,0001,0002,0003,0004,0005,0006,0007,
	0010,0011,0012,0013,0014,0015,0016,0017,
	0020,0021,0022,0023,0024,0025,0026,0027,
	0030,0031,0032,0033,0034,0035,0036,0037,
	0040,0041,0042,0043,0044,0045,0046,0047,
	0050,0051,0052,0053,0054,0055,0056,0057,
	0060,0061,0062,0063,0064,0065,0066,0067,
	0070,0071,0072,0073,0074,0075,0076,0077,
	0100,0101,0102,0103,0104,0105,0106,0107,
	0110,0111,0112,0113,0114,0115,0116,0117,
	0120,0121,0122,0123,0124,0125,0126,0127,
	0130,0131,0132,0133,0134,0135,0136,0137,
	0140,0141,0142,0143,0144,0145,0146,0147,
	0150,0151,0152,0153,0154,0155,0156,0157,
	0160,0161,0162,0163,0164,0165,0166,0167,
	0170,0171,0172,0173,0174,0175,0176,0177,
	0200,0201,0202,0203,0204,0205,0206,0207,
	0210,0211,0212,0213,0214,0215,0216,0217,
	0220,0221,0222,0223,0224,0225,0226,0227,
	0230,0231,0232,0233,0234,0235,0236,0237,
	0240,0241,0242,0243,0244,0245,0246,0247,
	0250,0251,0252,0253,0254,0255,0256,0257,
	0260,0261,0262,0263,0264,0265,0266,0267,
	0270,0271,0272,0273,0274,0275,0276,0277,
	0300,0301,0302,0303,0304,0305,0306,0307,
	0310,0311,0312,0313,0314,0315,0316,0317,
	0320,0321,0322,0323,0324,0325,0326,0327,
	0330,0331,0332,0333,0334,0335,0336,0337,
	0340,0341,0342,0343,0344,0345,0346,0347,
	0350,0351,0352,0353,0354,0355,0356,0357,
	0360,0361,0362,0363,0364,0365,0366,0367,
	0370,0371,0372,0373,0374,0375,0376,0377
};

unsigned char	nonprint[256] = {
	1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

unsigned char	dict[256] = {
	1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};



struct	field {
	unsigned char *code;
	unsigned char *ignore;
	int fcmp;
	int rflg;
	int bflg[2];
	int m[2];
	int n[2];
}	fields[NF];
struct field proto = {
	nofold,
	zero,
	ASC,
	1,
	0,0,
	0,-1,
	0,0
};
struct field xfields[NF];
int	nfields;
int 	error = 1;
int	cmpset = 0;
int	pos1flag;	/* pos1flag: nonzero iff a "+pos1" option has been
			 * seen but no corresponding "-pos2" option.
			 */

#ifdef _NO_PROTO
int	NLstrlen();
int	NLstrcmp();
int	cmp(), cmpa();
int	Acmp(), Acmpa();
int	term();
char	*skip();
char 	*eol();
char	*setfil();
#else
void 	sort(void);
void 	msort(char **a, char **b);
void 	insert(struct btree **tp, int n);
void 	merge(int a, int b);
void 	cline(register char *tp, register char *fp);
int	rline(register FILE *iop, register char *s);
void 	wline(char *s);
void 	checksort(void);
void 	disorder(char *s, char *t);
void 	newfile(void);
char 	*setfil(register int i);
void 	oldfile(void);
void 	safeoutfil(void);
void 	cant(char *f);
void 	diag(char *s, char *t);
void	newoptfield(char *s,int spos,int ink, struct field *p);
void	Usage(void);
void 	term(void);
int	cmp(char *a, char *b);
int	cmpa(register char *pa, register char *pb);
int	Acmp(char *i, char *j);
int	Acmpa(register char *pa, register char *pb);
char 	*skip( register char *p, struct field *fp, int j);
char 	*eol(register char *p);
void 	copyproto(void);
void 	tblinit(void);
void 	initree(void);
int 	cmpsave(register int n);
int 	number(register char **ppa);
void 	qksort(char **a, char **l);
void 	rderror(char *s);
void 	wterror(char *s);
int	fgetrec(char *s, int n, FILE *stream, char *f);
void 	fldtowa(char *p, char *l, char *b, unsigned char *ignore, unsigned char *code);
unsigned grow_core(unsigned size, unsigned cursize);
char 	*xfrmtos(char *s);
#endif

int	(*compare)() = cmpa;

extern int optind;
extern int optopt;
extern char *optarg;
extern int opterr;

#ifdef _NO_PROTO
main(argc, argv)
int  argc;
char *argv[];
#else
main(int argc, char *argv[])
#endif
{
	register a;
	char *arg;
	struct field *p, *q;
	int i;
	int c;		/* option character */
	int kopt;	/* -k option has been seen.*/
	int badopt;	/* at least one command line syntax error */
	char **oargv;	/* oargv,oartc,oargi: for checking obsolete -o location */
	int oargc,oargi,skipargs;
	int signedalloc;
	struct rlimit rlp;	/* for getrlimit() */
	char *sbrk();
	char *cloc;

	setlocale(LC_ALL,"");
	cloc = setlocale(LC_COLLATE,NULL);
	if (!strcmp(cloc,"C")) {
		Aflag = 1;
		compare = Acmpa;
	}


	/* close any file descriptors that may have been */
	/* left open -- we may need them all		*/
	for (i = 3; i < 3 + N; i++)
		(void) close(i);
	catd = catopen(MF_SORT,0);
	loc = localeconv();
	dec = loc->decimal_point[0];
	decmon = loc->mon_decimal_point[0];

	copyproto();
	initree();
	eargv = argv;
	tryfor = DEFMEM;
	nfields = 0;
	pos1flag = 0;
	kopt = 0;
	badopt = 0;
	uflg = 0;
	maxrec = 0;
	outfil = (char *)NULL;
			/* Command parsing: follow POSIX guidelines; allow 
			 * widely-used POSIX "obsolescent" sort key options;
			 * and allow -y option with optional argument.
			 */
	do {
	  p = &fields[nfields];
		/* Five-stage processing of each potential option character:
		 * 1,2. If it is the first character of an obsolescent option
		 *    that does not follow getopt() conventions, process it
		 *    manually and update getopt() pointers to next possible
		 *    option character.
		 * 3. If it is -t and nothing follows it then set tabchar to
		 *    space. getopt() deals with all other cases.
		 * 4. If it is -y and is either (the last command line
		 *    argument) or (followed in the next command line
		 *    argument by something that does not begin with a digit)
		 *    then (treat it as non-POSIX -y with omitted argument).
		 * 5. Otherwise process it through getopt() for normal 
		 *    option processing.
		 */
	  if(optind >= argc)
		c = EOF;
	  else {
		arg = argv[optind];
		optarg = arg + 1;
		c = arg[0];
	  };
	  if ( c == '+') {	
		/* Part 1 of 5: Obsolescent +pos1 option */
		newoptfield(arg+1,0,0,p);
		optarg = argv[optind++];
	  } else if (c == '-' && strlen(arg) > 1
			 && (ISDIGIT(arg[1]) || arg[1]=='.' )) { 	
		/* Part 2 of 5: Obsolescent -pos2 option */
		newoptfield(arg+1,1,0,p);
		optarg = argv[optind++];
	  } else if (c == '-' && arg[1] == 't' && strlen(arg) == 2) {
		/* part 3 of 5: -t option on its own */
		tabchar = ' ';
		optarg = argv[optind++];
	  } else if (c == '-' && strlen(arg) == 2 && arg[1] == 'y'
			&& (argv[optind+1] == NULL
			|| !ISDIGIT(argv[optind+1][0])) ) {
		/* Part 4 of 5: -y with optional Kilobytes argument omitted */
	   	tryfor = MAXMEM;
	   	optarg = argv[optind++];
	   	c = arg[1];
	  } else {
		/* Part 5 of 5: Normal POSIX command syntax option */
	   	c = getopt(argc,argv,"bcdfik:mno:rut:y:z:AT:");
	    switch(c){
	/* Operation modification options */
	    case 'c':
		cflg = 1;
		break;;
	    case 'm':
		mflg = 1;
		cmpset = 0;
		break;
	    case 'o':
		outfil = optarg;
		break;
	    case 't':
		if (strlen(optarg) == 1)
			tabchar = optarg[0];
		else badopt++;
		break;
	    case 'u':
		uflg = 1;
		break;
	    case 'y':
		/* -y with omitted argument handled separately above */
		/* Check for -y argument = valid integer */
		tryfor = (unsigned int) number(&optarg);
		if (tryfor == 0)
			badopt++;
		else
			tryfor *= 1024;
		
		/* Limit -y request to range [MINMEM,MAXMEM] , default=DEFMEM */
		tryfor = (tryfor<MINMEM ? MINMEM : 
				(tryfor > MAXMEM ? MAXMEM :
					(tryfor==0 ? DEFMEM : tryfor)));
		break;
	    case 'z':
		maxrec = number(&optarg);
		if (maxrec == 0)
			badopt++;
		break;
	    case 'A':
		Aflag = 1;
		cloc = setlocale(LC_ALL, "C");
		compare = Acmpa;
		break;
	    case 'T':
		if (optarg[0] != '\0') {
			if ((NLstrlen(optarg) + NAMEOHD) > sizeof(file1)) {
				diag(MSGSTR(PATH, "path name too long: ")
					, optarg);
				exit(1);
			}
			else dirtry[0] = optarg;
		}
		break;
	/* Field modification options, must precede field spec option if preceded by '-' */
	    case 'b':
	    case 'd':
	    case 'f':
	    case 'i':
	    case 'n':
	    case 'r':
	/* Per POSIX 1003.2/D11(4.58.3,11630f): options -b,-d,-f,-i,-n, and -r
	 * must precede option -k although b,d,f,i,n, and r may appear within a
	 * -k option as a type in a keydef.  See (4.58.10,11855-11861).
	 */
		if (kopt) badopt++;
		else {
			switch(c) {
	  		case 'd':
				p->ignore = dict;
				break;
	  		case 'f':
				p->code = fold;
				break;
	  		case 'i':
				p->ignore = nonprint;
				break;
	  		case 'n':
				p->fcmp = NUM;
				break;
			case 'b':
				if (nfields==0) p->bflg[0]++;
				else p->bflg[1-pos1flag]++;
				break;
	  		case 'r':
				p->rflg = -1;
				break;
			default:
				break;
			}
		}
		break;
	/* Sort key options */
	    case 'k':
		kopt++;
		newoptfield(optarg,0,1,p);
		break;
	    case EOF:
		break;
	    default:
		badopt++;
		break;	
	    }; /*end switch(c)*/
	  }; /* end POSIX syntax option */
	  /* Update choice of comparison routine: if the option qualifies
	   * sort, use a sort routine that allows for field processing and
	   * processed sort keys prepended to the record. (All options
	   * except  -A, -T, -o, -u, -y, and  -z  qualify sort somehow.)
	   */
	  if ( (strchr("ATouyz",(int)c) == NULL ) && c!=EOF) {
		if(Aflag)
			compare = Acmp;
		else
			compare = cmp;
		if (!mflg)
			cmpset = 1;
	  }
	} /* end do c = ... */
	while (c != EOF);

	/* Set up input file names for pre-getopt() parameter processing: 
	 * eargc = number of input Files; eargv[0..eargc-1] = array of Filenames.
	 */
	eargv = &argv[optind];
	eargc = argc - optind;

	/* Check for -o option in the file operands (POSIX obsolescent requirement) */
	for (oargv=eargv,oargc=eargc; oargc>0; oargv++,oargc--) {
		if (strncmp(*oargv,"-o",2)==0) {
			/* Found option beginning with  -o,
			 * get -o argument as output file name.
			 */
			if (strlen(*oargv) == 2 && oargc > 1) {
				/* -o with Filename in following argv[] string */
				skipargs = 2;
				outfil = *(oargv+1);
			} else if (strlen(*oargv) > 2) {
				/* -oFilename  all in one argv[] string */
				skipargs = 1;
				outfil = (*oargv)+2;
			} /* else continue; No file specified, take  -o  as last input file. */
			/* Delete -o and argument from file names list */
			eargc -= skipargs;
			oargc -= skipargs;
			for (oargi=0;oargi<oargc;oargi++)
				oargv[oargi]=oargv[oargi+skipargs];
			oargv++;
		}
	}

	if (badopt) {
		Usage();
	}

	q = &fields[0];
	for(a=1; a<=nfields; a++) {
		p = &fields[a];
		if(p->code != proto.code) continue;
		if(p->ignore != proto.ignore) continue;
		if(p->fcmp != proto.fcmp) continue;
		if(p->rflg != proto.rflg) continue;
		if(p->bflg[0] != proto.bflg[0]) continue;
		if(p->bflg[1] != proto.bflg[1]) continue;
		p->code = q->code;
		p->ignore = q->ignore;
		p->fcmp = q->fcmp;
		p->rflg = q->rflg;
		p->bflg[0] = p->bflg[1] = q->bflg[0];
	}
	if(eargc == 0)
		eargv[eargc++] = "-";
	if(cflg && eargc>1) {
		diag(MSGSTR(CHECK,"can check only 1 file"), "");
		exit(1);
	}

	safeoutfil();

	lspace = (int *) sbrk(0);
	/*
	 * Get the maximum possible brk value (for BSD systems)
	 * maxbrk is an (int *), so we do the addition with char pointers
	 */
        {
                char *p = (char *)lspace;

                getrlimit(RLIMIT_DATA, &rlp);
                p += rlp.rlim_max;
                maxbrk = (int *)p;
        }

	if (!mflg && !cflg) {
		if ((alloc=grow_core(tryfor,(unsigned) 0)) == 0) {
			diag(MSGSTR(ALLOC,"allocation error before sort"), "");
			exit(1);
		}
	} else {
		if ( (alloc = grow_core(MAXMEM,(unsigned) 0)) == 0 ) {
			diag(MSGSTR(MALLOC,"allocation error before merge"), "");
			exit(1);
		}
	}

	a = -1;
	for(dirs=dirtry; *dirs; dirs++) {
		(void) sprintf(filep=file1, "%s/stm%.5uaa", *dirs, getpid());
		while (*filep)
			filep++;
		filep -= 2;
		if ( (a=creat(file, 0600)) >=0)
			break;
	}
	if(a < 0) {
		diag(MSGSTR(LOCATE,"can't locate temp"), "");
		exit(1);
	}
	(void) close(a);
	(void) unlink(file);
#ifdef _NO_PROTO
	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
		(void) signal(SIGHUP, (void *)term);
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		(void) signal(SIGINT, (void *)term);
	(void) signal(SIGPIPE, (void *)term);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
		(void) signal(SIGTERM, (void *)term);
#else
	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
		(void) signal(SIGHUP, (void (*)(int))term);
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		(void) signal(SIGINT, (void (*)(int))term);
	(void) signal(SIGPIPE, (void (*)(int))term);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
		(void) signal(SIGTERM, (void (*)(int))term);
#endif
	nfiles = eargc;
	if(!mflg && !cflg) {
		sort();
		if (ferror(stdin))
			rderror(MSGSTR(STDIN,"stdin"));
		(void) fclose(stdin);
	}

	if (maxrec == 0)  maxrec = SORT_LINE_MAX;
	alloc = (N + 1) * maxrec + N * BUFSIZ;
	for (nway = N; nway >= 2; --nway) {
		if (alloc < (maxbrk - lspace) * sizeof(int *))
			break;
		alloc -= maxrec + BUFSIZ;
	}
	if (nway < 2 || brk((char *)lspace + alloc) != 0) {
		diag(MSGSTR(MALLOC,"allocation error before merge"), "");
		term();
	}

	if (cflg)   checksort();

	wasfirst = notfirst = 0;
	a = mflg || cflg ? 0 : eargc;
	if ((i = nfiles - a) > nway) {	/* Do leftovers early */
		if ((i %= (nway - 1)) == 0)
			i = nway - 1;
		if (i != 1)  {
			newfile();
			setbuf(os, bufout);
			merge(a, a+i);
			a += i;
		}
	}
	for(; a+nway<nfiles || unsafeout&&a<eargc; a=i) {
		i = a+nway;
		if(i>=nfiles)
			i = nfiles;
		newfile();
		setbuf(os, bufout);
		merge(a, i);
	}
	if(a != nfiles) {
		oldfile();
		setbuf(os, bufout);
		merge(a, nfiles);
	}
	error = 0;
	term();
	exit(0);
}

/*
 * NAME: sort
 * FUNCTION: setup the buffers for the sort and call msort routine
 */
#ifdef _NO_PROTO
sort()
#else
void 
sort(void)
#endif
{
	register char *cp;
	register char **lp, **ep;
	char *keep, *ekeep, **mp, **lmp;
	FILE *iop;
	int n;
	int done, i, first;
	char *f;

	/*
	** Records are read in from the front of the buffer area.
	** Pointers to the records are allocated from the back of the buffer.
	** If a partially read record exhausts the buffer, it is saved and
	** then copied to the start of the buffer for processing with the
	** next coreload.
	*/
	first = 1;
	done = 0;
	keep = NULL;
	ekeep = NULL;
	i = 0;
	ep = (char **) (((char *) lspace) + alloc);
	if ((f=setfil(i++)) == NULL) /* open first file */
		iop = stdin;
	else if ((iop=fopen(f,"r")) == NULL)
		cant(f);
	setbuf(iop,bufin);
	do {
		lp = ep - 1;
		cp = (char *) lspace;
		*lp-- = cp; /* move record from previous coreload */
		if (keep && ekeep)
			for(; keep < ekeep; *cp++ = *keep++);
		while ((char *)lp - cp > 1) {
			n = fgetrec(cp,(char *) lp - cp, iop, f);
			if (n == 0) {
				if (ferror(iop))
					rderror((char *)iop);

				if (keep != 0 )
					/* The kept record was at
					   the EOF.  Let the code
					   below handle it.       */;
				else
				if (i < eargc) {
					if ((f=setfil(i++)) == NULL)
						iop = stdin;
					else if ((iop=fopen(f,"r")) == NULL )
						cant(f);
					setbuf(iop,bufin);
					continue;
				}
				else {
					done++;
					break;
				}
			}
			cp += n-1;
			if ( *cp == '\n') {
				cp += 2;
				if ( cp - *(lp+1) > maxrec )
					maxrec = cp - *(lp+1);
				*lp-- = cp;
				keep = 0;
			}
			else 
			if ( cp + 2 < (char *) lp ) {
				if (NCisshift(*cp)) 
					cp--;
				/* the last record of the input */
				/* file is missing a NEWLINE    */
				if(f == NULL) diag(MSGSTR(NEWLINE,
				  "warning: missing NEWLINE added at EOF"), "");
				else diag(MSGSTR(NEWLINE2,
				  "warning: missing NEWLINE added at end of input file ")
						, f);
				*++cp = '\n';
				*++cp = '\0';
				*lp-- = ++cp;
				keep = 0;
			}
			else {  /* the buffer is full */
				keep = *(lp+1);
				ekeep = ++cp;
			}
			if ((char *)lp - cp <= 2 && first == 1) {
				/* full buffer */
				tryfor = alloc;
				tryfor = grow_core(tryfor,alloc);
				if (tryfor == 0)
					/* could not grow */
					first = 0;
				else { /* move pointers */
					lmp = ep + 
					   (tryfor/sizeof(char **) - 1);
					for ( mp = ep - 1; mp > lp;)
						*lmp-- = *mp--;
					ep += tryfor/sizeof(char **);
					lp += tryfor/sizeof(char **);
					alloc += tryfor;
				}
			}
		}
		if (keep != 0 && *(lp+1) == (char *) lspace) {
			fprintf(stderr,MSGSTR(FATAL,"fatal: record too large %d\n"),LINE);
			term();
		}
		first = 0;
		lp += 2;
		if(done == 0 || nfiles != eargc)
			newfile();
		else
			oldfile();
		setbuf(os, bufout);
		msort(lp, ep);
		if (ferror(os))
			wterror(MSGSTR(SORT,"sorting"));
		(void) fclose(os);
	} while(done == 0);
}


/*
 * NAME: msort
 * FUNCTION: setup the merge sort and call qksort to do the actual sorting
 */
#ifdef _NO_PROTO
msort(a, b)
char **a, **b;
#else
void 
msort(char **a, char **b)
#endif
{
	register struct btree **tp;
	register int i, j, n;
	char *save;

	i = (b - a);
	if (i < 1)
		return;
	else if (i == 1) {
		wline(*a);
		return;
	}
	else if (i >= TREEZ)
		n = TREEZ; /* number of blocks of records */
	else n = i;

	/* break into n sorted subgroups of approximately equal size */
	tp = &(treep[0]);
	j = 0;
	do {
		(*tp++)->rn = j;
		b = a + (blkcnt[j] = i / n);
		qksort(a, b);
		blkcur[j] = a = b;
		i -= blkcnt[j++];
	} while (--n > 0);
	n = j;

	/* make a sorted binary tree using the first record in each group */
	for (i = 0; i < n;) {
		(*--tp)->rp = *(--blkcur[--j]);
		insert(tp, ++i);
	}
	wasfirst = notfirst = 0;
	bonus = cmpsave(n);


	j = uflg;
	tp = &(treep[0]);
	while (n > 0)  {
		wline((*tp)->rp);
		if (j) save = (*tp)->rp;

		/* Get another record and insert.  Bypass repeats if uflg */

		do {
			char *s;

			i = (*tp)->rn;
			/* strip leading strxfrm data */
			s = xfrmtos(*(blkcur[i]-1));
			if (j) while((blkcnt[i] > 1) &&
					(*s == '\0')) {
				--blkcnt[i];
				--blkcur[i];
				/* strip leading strxfrm data */
				s = xfrmtos(*(blkcur[i]-1));
			} 
			if (--blkcnt[i] > 0) {
				(*tp)->rp = *(--blkcur[i]);
				insert(tp, n);
			} else {
				if (--n <= 0) break;
				bonus = cmpsave(n);
				tp++;
			}
		} while (j && (*compare)((*tp)->rp, save) == 0);
	}
}

/* Insert the element at tp[0] into its proper place in the array of size n */
/* Pretty much Algorith B from 6.2.1 of Knuth, Sorting and Searching */
/* Special case for data that appears to be in correct order */

#ifdef _NO_PROTO
insert(tp, n)
struct btree **tp;
int n;
#else
void
insert(struct btree **tp, int n)
#endif
{
    register struct btree **lop, **hip, **midp;
    register int c;
    struct btree *hold;

    midp = lop = tp;
    hip = lop++ + (n - 1);
    if ((wasfirst > notfirst) && (n > 2) &&
	((*compare)((*tp)->rp, (*lop)->rp) >= 0)) {
	wasfirst += bonus;
	return;
    }
    while ((c = hip - lop) >= 0) { /* leave midp at the one tp is in front of */
	midp = lop + c / 2;
	if ((c = (*compare)((*tp)->rp, (*midp)->rp)) == 0) 
		if (Aflag) break; /* match */
	if (c <= 0) lop = ++midp;   /* c < 0 => tp > midp */
	else       hip = midp - 1; /* c > 0 => tp < midp */
    }
    c = midp - tp;
    if (--c > 0) { /* number of moves to get tp just before midp */
	hip = tp;
	lop = hip++;
	hold = *lop;
	do *lop++ = *hip++; while (--c > 0);
	*lop = hold;
	notfirst++;
    } else wasfirst += bonus;
}


/*
 * NAME: merge
 * FUNCTION: merge sorted files together
 */
#ifdef _NO_PROTO
merge(a, b)
int a, b;
#else
void
merge(int a, int b)
#endif
{
	FILE *tfile[N];
	char *buffer = (char *) lspace;
	char	*save;
	char *iobuf;
	register int nf;		/* number of merge files */
	register struct btree **tp;
	register int i, j;
	char	*f;

	save = (char *) lspace + (nway * maxrec);
	iobuf = save + maxrec;
	tp = &(treep[0]);
	for (nf=0, i=a; i < b; i++)  {
		f = setfil(i);
		if (f == 0)
			tfile[nf] = stdin;
		else if ((tfile[nf] = fopen(f, "r")) == NULL)
			cant(f);
		(*tp)->rp = buffer + (nf * maxrec);
		(*tp)->rn = nf;
		setbuf(tfile[nf], iobuf);
		iobuf += BUFSIZ;
		if (rline(tfile[nf], (*tp)->rp)==0) {
			nf++;
			tp++;
		} else {
			if(ferror(tfile[nf]))
				rderror(f);
			(void) fclose(tfile[nf]);
		}
	}


	/* make a sorted btree from the first record of each file */
	for (--tp, i = 1; i++ < nf;) insert(--tp, i);

	bonus = cmpsave(nf);
	tp = &(treep[0]);
	j = uflg;

	/* If -u option specified, suppress character-code comparison of
	 * records with equal collation values based on sort options.
	 */

	while (nf > 0) {
		wline((*tp)->rp);
		if (j) cline(save, (*tp)->rp);

		/* Get another record and insert.  Bypass repeats if uflg */

		do {
			i = (*tp)->rn;
			if (rline(tfile[i], (*tp)->rp)) {
				if (ferror(tfile[i]))
					rderror(setfil(i+a));
				(void) fclose(tfile[i]);
				if (--nf <= 0) break;
				++tp;
				bonus = cmpsave(nf);
			} else insert(tp, nf);
		} while (j && (*compare)((*tp)->rp, save) == 0 );
	}

	for (i=a; i < b; i++) {
		if (i >= eargc)
			(void) unlink(setfil(i));
	}
	if (ferror(os))
		wterror("merging");
	(void) fclose(os);
}

/*
 * NAME: cline
 * FUNCTION: copy line
 */
#ifdef _NO_PROTO
cline(tp, fp)
register char *tp, *fp;
#else
void
cline(register char *tp, register char *fp)
#endif
{
	while ((*tp++ = *fp++) != '\n');
}

/*
 * NAME: rline
 * FUNCTION: read line
 *           Because the lines may be read form temporary work files,
 *           we must check the state of the sort. For merge and cksort
 *           operations, (and plain sorts), we read as usual. For keyed
 *	     sorts, the key area (which may contain zero bytes) is first
 *	     read, then the remainder (text portion) of the record.
 */
#ifdef _NO_PROTO
rline(iop, s)
register FILE *iop;
register char *s;
#else
int
rline(register FILE *iop, register char *s)
#endif
{
	register int n;
	int rlen, maxlen;

	maxlen = maxrec -1;
	if (!mflg && !cflg && (cmpset == 1)) {
		maxlen = maxrec-3;
		fread(s, 1, 2, iop);
		rlen = ((s[0] << 8) | (s[1]));
		rlen -= 2;
		if (rlen > maxlen)
			rlen = maxlen;
		if (fread(s, 1, rlen, iop) < rlen)
			return(1);
		maxlen -= rlen;
		s += rlen;
	}
	if (fgets(s,maxlen,iop) == NULL )
		n = 0;
	else
		n = strlen(s);
	if ( n == 0 )
		return(1);
	s += n - 1;
	if ( *s == '\n' )
		return(0);
	if ( n < maxlen) {
		diag(MSGSTR(NEWLINE3,"warning: missing NEWLINE at EOF added"),"");
		*++s = '\n';
		return(0);
	}
	else {
		fprintf(stderr,MSGSTR(TOOLONG,"fatal: line too long %d\n"),LINE);
		term();
	}
	return(0);
}

/*
 * NAME: wline
 * FUNCTION: write line
 */
#ifdef _NO_PROTO
wline(s)
char *s;
#else
void
wline(char *s)
#endif
{
	s = xfrmtos(s);
	(void) fputs(s,os);
}

/*
 * NAME: checksort
 * FUCNTION: has file already been sorted.
 */
#ifdef _NO_PROTO
checksort()
#else
void
checksort(void)
#endif
{
	char *lines[2];
	register char **s;
	char *f;
	register int i, j, r;
	register FILE *iop;

	s = &(lines[0]);
	f = setfil(0);
	if (f == 0)
		iop = stdin;
	else if ((iop = fopen(f, "r")) == NULL)
		cant(f);
	setbuf(iop, bufin);

	i = 0;   j = 1;
	s[0] = (char *) lspace;
	s[1] = s[0] + maxrec;
	if ( rline(iop, s[0]) ) {
		if (ferror(iop)) {
			rderror(f);
		}
		(void) fclose(iop);
		exit(0);
	}
	while ( !rline(iop, s[j]) )  {
		r = (*compare)(s[i], s[j]);
		if (r < 0)
			disorder(MSGSTR(DISORDER,"disorder: "), s[j]);
		if (r == 0 && uflg)
			disorder(MSGSTR(NUNIQUE,"not unique: "), s[j]);
		r = i;  i = j; j = r;
	}
	if (ferror(iop))
		rderror(f);
	(void) fclose(iop);
	exit(0);
}

/*
 * NAME: disorder
 * FUNCTION: added NULL character to the end of the string t and print error message
 */
#ifdef _NO_PROTO
disorder(s, t)
char *s; 
char *t;
#else
void
disorder(char *s, char *t)
#endif
{
	register char *u;
	for(u=t; *u!='\n';u++) ;
	*u = 0;
	diag(s, t);
	term();
}

/*
 * NAME: newfile
 * FUNCTION: open file for writting
 */
#ifdef _NO_PROTO
newfile()
#else
void
newfile(void)
#endif
{
	register char *f;

	f = setfil(nfiles);
	if((os=fopen(f, "w")) == NULL) {
		diag(MSGSTR(CREATE,"can't create "), f);
		term();
	}
	nfiles++;
	outflag = 0;
}

/*
 * NAME: setfil
 * FUNCTION: set up unique temp file name
 */
char *
#ifdef _NO_PROTO
setfil(i)
register int i;
#else
setfil(register int i)
#endif
{
	if(i < eargc)
		if(eargv[i][0] == '-' && eargv[i][1] == '\0')
			return(0);
		else
			return(eargv[i]);
	i -= eargc;
	filep[0] = i/26 + 'a';
	filep[1] = i%26 + 'a';
	return(file);
}

/*
 * NAME: oldfile
 * FUNCTION: open output file or set os to stdout
 */
#ifdef _NO_PROTO
oldfile()
#else
void
oldfile(void)
#endif
{
	if(outfil) {
		if((os=fopen(outfil, "w")) == NULL) {
			diag(MSGSTR(CREATE,"can't create "), outfil);
			term();
		}
	} else
		os = stdout;
	outflag = 1;	/* set to mark output file... */
}

/*
 * NAME: safeoutfil
 * FUNCTION: check the output file is it safe to use as an output file
 */
#ifdef _NO_PROTO
safeoutfil()
#else
void
safeoutfil(void)
#endif
{
	register int i;
	struct stat ostat, istat;

	if(!mflg||outfil==0)
		return;
	if(stat(outfil, &ostat)==-1)
		return;
	if ((i = eargc - N) < 0) i = 0;	/*-N is suff., not nec. */
	for (; i < eargc; i++) {
		if(stat(eargv[i], &istat)==-1)
			continue;
		if(ostat.st_dev==istat.st_dev&&
		   ostat.st_ino==istat.st_ino)
			unsafeout++;
	}
	return;
}

/*
 * NAME: cant
 * FUNCTION: print error message when unable to open a file
 */
#ifdef _NO_PROTO
cant(f)
char *f;
#else
void
cant(char *f)
#endif
{
	diag(MSGSTR(OPEN,"can't open "), f);
	term();
}

/*
 * NAME:
 * FUNCTION: print error message
 */
#ifdef _NO_PROTO
diag(s, t)
char *s, *t;
#else
void
diag(char *s, char *t)
#endif
{
	register FILE *iop;

	iop = stderr;
	(void) fputs("sort: ", iop);
	(void) fputs(s, iop);
	(void) fputs(t, iop);
	(void) fputs("\n", iop);
}

/*
 * NAME: term
 * FUNCTION: clean up and exit 
 */
#ifdef _NO_PROTO
term()
#else
void
term(void)
#endif
{
	register i;

	(void) signal(SIGINT, SIG_IGN);
	(void) signal(SIGHUP, SIG_IGN);
	(void) signal(SIGTERM, SIG_IGN);
	if(nfiles == eargc)
		nfiles++;
	for(i=eargc; i<=nfiles; i++) {	/*<= in case of interrupt*/
		(void) unlink(setfil(i));	/*with nfiles not updated*/
	}
	exit(error);
}

/*
 * NAME: cmp
 * FUNCTION: compare two strings
 * 		This routine is substantially expanded and changed to be
 *		able to handle the prefixed form of the data. If the compare
 *		code is either XNUM or XSTR, then the field is prepended to
 *		the record; otherwise, the old method for finding the key
 *		and compare it holds.
 *		Note that, for string compares, if the caller specified either
 *		folding or dictionary sort, then the fields are moved (and
 *		either compacted or folded) to work areas, where they are
 *		compared. If neither folding or dictionary order is speci-
 *		fied, the compare takes place in situ.
 */
int
#ifdef _NO_PROTO
cmp(a, b)
char *a, *b;
#else
cmp(char *a, char *b)
#endif
{
	register char *pa, *pb;
	char *la, *lb;
	char sava, savb;
	char *ipa, *ipb, *jpa, *jpb;
	unsigned char *code, *ignore;
	register int sa;
	int sb;
	int i, j;
	int k;
	struct field *fp;

	for(k = nfields>0; k<=nfields; k++) {
		fp = &fields[k];
		pa = a;
		pb = b;
		if ( (fp->fcmp == ASC) || (fp->fcmp == NUM) ) {
			if(k >= 0) {	/* keys are in original place */
				la = skip(pa, fp, 1);
				pa = skip(pa, fp, 0);
				lb = skip(pb, fp, 1);
				pb = skip(pb, fp, 0);
			} else {
				la = eol(pa);
				lb = eol(pb);
			}
		} else  {		/* keys are prepended to record */
			pa += 2;
			pb += 2;
			for (i = 0; i < k; i++) {
				pa += ((pa[0] << 8) | (pa[1]));
				pb += ((pb[0] << 8) | (pb[1]));
			}
			la = pa + ((pa[0] << 8) | (pa[1]));
			lb = pb + ((pb[0] << 8) | (pb[1]));
			pa += 2;
			pb += 2;
		}
		if( (fp->fcmp == NUM) || (fp->fcmp == XNUM) ) {
			sa = sb = fp->rflg;
			if(*pa == '-') {
				pa++;
				sa = -sa;
			}
			if(*pb == '-') {
				pb++;
				sb = -sb;
			}
			for(ipa = pa; ipa<la&& ISDIGIT(*ipa); ipa++) ;
			for(ipb = pb; ipb<lb&& ISDIGIT(*ipb); ipb++) ;
			jpa = ipa;
			jpb = ipb;
			i = 0;
			if(sa==sb)
				while(ipa > pa && ipb > pb)
					if(j = *--ipb - *--ipa)
						i = j;
			while(ipa > pa)
				if(*--ipa != '0')
					return(-sa);
			while(ipb > pb)
				if(*--ipb != '0')
					return(sb);
			if(i) return(i*sa);
			if(*(pa=jpa) == dec || *(pa=jpa) == decmon)
				pa++;
			if(*(pb=jpb) == dec || *(pb=jpb) == decmon)
				pb++;
			if(sa==sb)
				while(pa<la && ISDIGIT(*pa)
				   && pb<lb && ISDIGIT(*pb))
					if(i = *pb++ - *pa++)
						return(i*sa);
			while(pa<la && ISDIGIT(*pa))
				if(*pa++ != '0')
					return(-sa);
			while(pb<lb && ISDIGIT(*pb))
				if(*pb++ != '0')
					return(sb);
			continue;
		}
		if (fp->fcmp == XSTR) {		/* 'key' is prepended */
			sa = strcmp(pb, pa);
			if (sa == 0)
				continue;		
			return(sa*fp->rflg);
		}
		if (fp->fcmp == ASC) {		/* record is not prepended */
			code = fp->code;
			ignore = fp->ignore;
			if ( (ignore == zero) && (code == nofold) ) {
				sava = *la;
				*la = '\0';
				savb = *lb;
				*lb = '\0';
				sa = strcoll(pb, pa);
				*la = sava;
				*lb = savb;
			} else {		/* have to use work areas */
				ipa = tbuf;
				fldtowa(pa, la, ipa, ignore, code);
				ipb = ebuf;
				fldtowa(pb, lb, ipb, ignore, code);
				sa = strcoll(ipb, ipa);
			}
			if ( sa == 0 )
				continue;
			return(sa*fp->rflg);
		}
	}
	if(uflg)
		return(0);
	for(k = nfields>0; k<=nfields; k++) {
		fp = &fields[k];
		if ( (fp->fcmp == XNUM) || (fp->fcmp == XSTR) ) {
			pa = a;
			pb = b;
			pa += ((pa[0] << 8) | (pa[1]));
			pb += ((pb[0] << 8) | (pb[1]));
			return(cmpa(pa, pb));
		}
	}
	return(cmpa(a, b));
}

/*   this routine was recoded in 370 assembler as part of the peachtree
 *   activities.                HJS-S
 *
 * NAME: cmpa
 * FUNCTION: compare two strings
 *			This routine is modified to use strcoll.
 */
int
#ifdef _NO_PROTO
cmpa(pa, pb)
register char *pa, *pb;
#else
cmpa(register char *pa, register char *pb)
#endif
{
	register int alen, blen; 
 	register int r,i,j;
 	NLchar nlpa[BUFSIZ],*pnlpa = &nlpa[0],nlpb[BUFSIZ], *pnlpb = &nlpb[0];

	alen = eol(pa) - pa;
	blen = eol(pb) - pb;
	pa[alen] = '\0';
	pb[blen] = '\0';
	r = strcoll(pb, pa) * fields[0].rflg;
/* P37708 */
	/* for translated characters that have the same primary and unique
	** collation value, check the NLchar value
	*/
 	if (r == 0) {
 	    /* compare each characters NLchar value */
 	    i = (alen < blen) ? alen : blen;
 	    NCdecstr(pa,pnlpa,alen+1);
 	    NCdecstr(pb,pnlpb,blen+1); 
 	    j=0;
	    do {
 		r = nlpb[j] - nlpa[j];
 		j++;
 	    } while (--i > 0 && r == 0) ;
 	}
/* P37708 end */
	pa[alen] = '\n';
	pb[blen] = '\n';
	return (r);
}

/*
 * NAME: Acmp
 * FUNCTION: compare two strings
 *			This is the oldfashioned AT&T code; assuming the
 *			native collating sequence. It is activated using
 *			the -A flag.
 */
int
#ifdef _NO_PROTO
Acmp(i, j)
char *i, *j;
#else
Acmp(char *i, char *j)
#endif
{
	register char *pa, *pb;
	register unsigned char *ignore;
	unsigned char *code;
	char *la, *lb;
	char *ipa, *ipb, *jpa, *jpb;
	register int sa;
	int sb;
	int a, b;
	int k;
	struct field *fp;

	for(k = nfields>0; k<=nfields; k++) {
		fp = &fields[k];
		pa = i;
		pb = j;
		if(k >= 0) {
			la = skip(pa, fp, 1);
			pa = skip(pa, fp, 0);
			lb = skip(pb, fp, 1);
			pb = skip(pb, fp, 0);
		} else {
			la = eol(pa);
			lb = eol(pb);
		}
		if(fp->fcmp==NUM) {
			sa = sb = fp->rflg;
			if(*pa == '-') {
				pa++;
				sa = -sa;
			}
			if(*pb == '-') {
				pb++;
				sb = -sb;
			}
			for(ipa = pa; ipa<la&& ISDIGIT(*ipa); ipa++) ;
			for(ipb = pb; ipb<lb&& ISDIGIT(*ipb); ipb++) ;
			jpa = ipa;
			jpb = ipb;
			a = 0;
			if(sa==sb)
				while(ipa > pa && ipb > pb)
					if(b = *--ipb - *--ipa)
						a = b;
			while(ipa > pa)
				if(*--ipa != '0')
					return(-sa);
			while(ipb > pb)
				if(*--ipb != '0')
					return(sb);
			if(a) return(a*sa);
			if(*(pa=jpa) == dec || *(pa=jpa) == decmon)
				pa++;
			if(*(pb=jpb) == dec || *(pb=jpb) == decmon)
				pb++;
			if(sa==sb)
				while(pa<la && ISDIGIT(*pa)
				   && pb<lb && ISDIGIT(*pb))
					if(a = *pb++ - *pa++)
						return(a*sa);
			while(pa<la && ISDIGIT(*pa))
				if(*pa++ != '0')
					return(-sa);
			while(pb<lb && ISDIGIT(*pb))
				if(*pb++ != '0')
					return(sb);
			continue;
		}
		code = fp->code;
		ignore = fp->ignore;
loop: 
		while(ignore[*pa])
			pa++;
		while(ignore[*pb])
			pb++;
		if(pa>=la || *pa=='\n')
			if(pb<lb && *pb!='\n')
				return(fp->rflg);
			else continue;
		if(pb>=lb || *pb=='\n')
			return(-fp->rflg);

		sa = code[*pb++] - code[*pa++];

		if(sa == 0)
			goto loop;
		return(sa*fp->rflg);
	}
	if(uflg)
		return(0);
	return(Acmpa(i, j));
}
/*
 * NAME: Acmpa
 * FUNCTION: compare two strings
 *			This is the oldfashioned AT&T code, assuming that
 *			collation is according to the native code order.
 *			It is activated using the -A flag.
 */
int
#ifdef _NO_PROTO
Acmpa(pa, pb)
register char *pa, *pb;
#else
Acmpa(register char *pa, register char *pb)
#endif
{

	while(*pa == *pb++)
		if(*pa++ == '\n')
			return(0);
	return(
		*pa == '\n' ? fields[0].rflg:
		*--pb == '\n' ?-fields[0].rflg:
		*pb > *pa   ? fields[0].rflg:
		-fields[0].rflg
	);
}

/*
 * NAME: skip
 * FUNCTION: skip a field
 */

char *
#ifdef _NO_PROTO
skip(p, fp, j)
register char *p;
struct field *fp;
int j;
#else
skip(register char *p, struct field *fp, int j)
#endif
{
	register i, index;
	register wchar_t tbc;
	unsigned char *pptr;

	if( (i=fp->m[j]) < 0)
		return((char *)eol(p));
	if (tbc = tabchar)
		while (--i >= 0) {
			while(NCdechr(p) != tbc)
				if(*p != '\n')
					p += NLchrlen(p);
				else    return(p);
			if (i > 0 || j == 0)
				p += NLchrlen(p);
		}
	else	while (--i >= 0) {
			while(blank(*p))
				p++;
			while(!blank(*p))
				if(*p != '\n')
					p += NLchrlen(p);
				else    return(p);
		}
	if(fp->bflg[j] && (posflag | (fp->fcmp == NUM))) {
		if (j == 1 && fp->m[j] > 0)
			p += NLchrlen(p);
		while(blank(*p))
			p += NLchrlen(p);
	}
 	/* skip characters within the field */
	i = fp->n[j];
	while((i-- > 0) && (*p != '\n'))

 		if (( index = NCcollate(*p++)) < 0 ) {
			pptr = (unsigned char *)p;
 			index = _NLxcol(index,&pptr,0);
			p = (char *)pptr;
		}

	return(p);
}

/*
 * NAME: eol
 * FUNCTION: find the end of the line
 */
char *
#ifdef _NO_PROTO
eol(p)
register char *p;
#else
eol(register char *p)
#endif
{
	if ((p = (char *)strchr(p, '\n')) == NULL) {
		diag(MSGSTR(NULLS,
			"cannot process data file (check for null chars)"),"");
		exit(1);
	} else  return((char *)p);
}

/*
 * NAME: copyproto
 * FUNCTION: copy the prototype for the sort fields
 */
#ifdef _NO_PROTO
copyproto()
#else
void
copyproto(void)
#endif
{
	register i;
	register int *p, *q;

	p = (int *)&proto;
	q = (int *)&fields[nfields];
	for(i=0; i<sizeof(proto)/sizeof(*p); i++)
		*q++ = *p++;
}

/*
 * NAME: initree
 * FUNCTION: initialize the binary search tree
 */
#ifdef _NO_PROTO
int initree()
#else
void 
initree(void)
#endif
{
	register struct btree **tpp, *tp;
	register int i;

	for (tp = &(tree[0]), tpp = &(treep[0]), i = TREEZ; --i >= 0;)
	    *tpp++ = tp++;
}

#ifdef _NO_PROTO
int cmpsave(n)
register int n;
#else
int 
cmpsave(register int n)
#endif
{
	register int award;

	if (n < 2) return (0);
	for (n++, award = 0; (n >>= 1) > 0; award++);
	return (award);
}

/*
 * NAME: newoptfield
 * FUNCTION: Process field specification option string.
 */ 
#ifdef _NO_PROTO
newoptfield(s, spos, ink, p)
char	*s;
int	spos;
int	ink;
struct	field	*p;
#else
void
newoptfield(char *s,int spos,int ink, struct field *p)
#endif
{
  /* Entry: ink == 1 => s points to start of "-k" option value string
   *    ink == 0 && spos == 0 => s points to start of "+" pos1 string
   *    ink == 0 && spos == 1 => s points to start of "-" pos2 string
   *    nfields = number of prior field spec options
   *    pos1flag = 0 if no previous call to newoptfield(), or if previous call
   *	to newoptfield() was for "-m.n" or "-k...,m.n"; 
   *    = 1 if previous call to newoptfield() was for "+m.n" or "-km.n".
   * Only single-byte portable character set characters are accepted as option
   * values for +Pos1, -Pos2, and -kOptlist options.
   */
  char *sc;
  int rpart;	/* 0 if processing "+m.n", 1 if "-m.n" part (obsolescent)*/
	/* 0 if processing "-km.n" part, 1 if processing ",m.n" part (POSIX -k)*/
  int state;	/* 0 => no part of "m.n" processed */
  		/* 1 => "m" of "m.n" has been processed */
		/* 2 => "." of "m.n" has been processed */
  int numval;

  if ( ink || !spos || !pos1flag ) {   
	/* Either starting "-k", or starting "+m.n", or starting
	 * "-m.n" for which there was no corresponding "+m.n" .
	 */
	if(++nfields>=NF) {
		diag(MSGSTR(KEYS,"too many keys"), "");
		exit(1);
	}
	if(!spos || (ink && *s!=','))
		posflag++;
	copyproto();
  }
  p = &fields[nfields];
  state = 0;
  rpart = (!ink && spos);

  for (sc=optarg;*sc!='\0';sc++)
  {
	switch(*sc){
	case '.':
		if (state++ == 0) { /* Omitted "m" of "m.n" defaults to 0 */
		  p->m[rpart] = 0;
		  state++;
		}
		if (state > 2) { /* Too many "." */
		  Usage();
		}
		break;
	case '-':
		if (ink || !rpart++ ) {
		  Usage();
		}
		else
		  state = 0;
		break;
	case ',':	/* found -k...,m.n */
		if (!ink || rpart++ ) {
		  Usage();
		}
		else
		  state = 0;
		break;
	case 'b':
		p->bflg[rpart]++;
		break;
	case 'd':
		p->ignore = dict;
		break;
	case 'f':
		p->code = fold;
		break;
	case 'i':
		p->ignore = nonprint;
		break;
	case 'n':
		p->fcmp = NUM;
		break;
	case 'r':
		p->rflg = -1;
		break;
	default:
		if(ISDIGIT(*sc)) {
		  numval = number(&sc);
		  if((ink?numval-(state==2?0:1):numval)<0) { 
			Usage();
		  }
		  if(ink) {    /* Convert -k measurements to +pos1 -pos2 measurements.
				* Per POSIX 1003.2/D11(4.58.7,11785-11789),
				* -ka.b,c.d  =	if d==0 then +(a-1).(b-1) -c.d
				*			else +(a-1).(b-1) -(c-1).d
				*/
		    if(rpart)
		      if(state && numval>0)
		        p->m[rpart]--;
		      else;
		    else
		      numval--;
		  };
		  if (!state++){
			p->m[rpart] = numval;
		  } else {
			p->n[rpart] = numval;
		  }
		} else {
			 Usage();
		}
		break;
	} /* switch(*sc) */
  } /* for (sc=... */
  pos1flag = !rpart;
}

/*
 * NAME: Usage
 * FUNCTION: Display Usage message and exit >0
 */
#ifdef _NO_PROTO
Usage()
#else
void
Usage(void)
#endif
{
  fprintf(stderr,MSGSTR(USAGE,
"Usage:  sort\t[-Abcdfimnru] [-T Directory] [-tCharacter] [-y kilobytes] [-o File]\n\
\t\t[-k Keydefinition]... [[+Position1][-Position2]]... [File]...\n"));
  exit(1);
}

/*
 * NAME: number
 * FUNCTION: convert a string of digits into a number
 */
#ifdef _NO_PROTO
number(ppa)
register char **ppa;
#else
int 
number(register char **ppa)
#endif
{
	register int n;
	register char *pa;

	pa = *ppa;
	n = 0;
	while(ISDIGIT(*pa)) {
		n = n*10 + *pa - '0';
		*ppa = pa++;
	}
	return(n);
}

#define qsexc(p,q) t= *p;*p= *q;*q=t
#define qstexc(p,q,r) t= *p;*p= *r;*r= *q;*q=t

/*
 * NAME: qksort
 * FUNCTION: sort the binary tree
 */
#ifdef _NO_PROTO
qksort(a, l)
char **a, **l;
#else
void 
qksort(char **a, char **l)
#endif
{
	register char **i, **j;
	register char **lp, **hp;
	char **k;
	char *t;
	int c, delta;
	unsigned n;


start:
	if((n=l-a) <= 1)
		return;

	n /= 2;
	if (n >= MTHRESH) {
		lp = a + n;
		i = lp - 1;
		j = lp + 1;
		delta = 0;
		c = (*compare)(*lp, *i);
		if (c < 0) --delta;
		else if (c > 0) ++delta;
		c = (*compare)(*lp, *j);
		if (c < 0) --delta;
		else if (c > 0) ++delta;
		if ((delta /= 2) && (c = (*compare)(*i, *j)))
		    if (c > 0) n -= delta;
		    else       n += delta;
	}
	hp = lp = a+n;
	i = a;
	j = l-1;


	for(;;) {
		if(i < lp) {
			if((c = (*compare)(*i, *lp)) == 0) {
				--lp;
				qsexc(i, lp);
				continue;
			}
			if(c < 0) {
				++i;
				continue;
			}
		}

loop:
		if(j > hp) {
			if((c = (*compare)(*hp, *j)) == 0) {
				++hp;
				qsexc(hp, j);
				goto loop;
			}
			if(c > 0) {
				if(i == lp) {
					++hp;
					qstexc(i, hp, j);
					i = ++lp;
					goto loop;
				}
				qsexc(i, j);
				--j;
				++i;
				continue;
			}
			--j;
			goto loop;
		}


		if(i == lp) {
			if(uflg)
				for(k=lp; k<hp;) **k++ = '\0';
			if(lp-a >= l-hp) {
				qksort(hp+1, l);
				l = lp;
			} else {
				qksort(a, lp);
				a = hp+1;
			}
			goto start;
		}


		--lp;
		qstexc(j, lp, i);
		j = --hp;
	}
}

/*
 * NAME: rderror
 * FUNCTION: print read error
 */
#ifdef _NO_PROTO
rderror(s)
char *s;
#else
void 
rderror(char *s)
#endif
{
	diag(MSGSTR(EREAD,"read error on "), s == 0 ? MSGSTR(STDIN,"stdin") : s);
	term();
}

/*
 * NAME: wterror
 * FUNCTION: print write error
 */
#ifdef _NO_PROTO
wterror(s)
char *s;
#else
void 
wterror(char *s)
#endif
{
	diag(MSGSTR(EWRITE,"write error while "), s);
	term();
}

/*
 * NAME: grow_core
 * FUNCTION: get more memory
 */
#ifdef _NO_PROTO
grow_core(size,cursize)
	unsigned size, cursize;
#else
unsigned 
grow_core(unsigned size, unsigned cursize)
#endif
{
	unsigned newsize;
	unsigned long longnewsize;

	longnewsize = (unsigned long) size + (unsigned long) cursize;
	if (longnewsize < MINMEM)
		longnewsize = MINMEM;
	else
	if (longnewsize > MAXMEM)
		longnewsize = MAXMEM;
	newsize = (unsigned) longnewsize;
	for (; ((char *)lspace+newsize) <= (char *)lspace; newsize >>= 1);
	if (longnewsize > (unsigned long) (maxbrk - lspace) * (unsigned long) sizeof(int *))
		newsize = (maxbrk - lspace) * sizeof(int *);
	if (newsize <= cursize)
		return(0);
	if ( brk((char *) lspace + newsize) != 0)
		return(0);
	return(newsize - cursize);
}

/*
 * NAME: fgetrec
 * FUNCTION: get a char string from the stream and, if field sorting or
 *           other special functions (e.g., folding) is needed, retrieve
 *	     sort fields, converting string fields to strxfrm format, and
 *	     prepend sort key(s) to the record. If possible, the record
 *	     is built in the sort's work area; if that is not possible
 *	     (not enough space) it is built in a work area and moved.
 *           The key area is structured as follows:
 *		Bytes 0-1:		key length (offset to data)
 *		Bytes 2-3:		length flag field 0
 *		Bytes 4-i:		key field 0
 *		Bytes i+1-i+2:		length flag field 1
 *		Bytes i-3...		key field 1 ...
 *	     Subsequent fields follow in same format.	
 */
int
#ifdef _NO_PROTO
fgetrec(s,n,stream,f)
char *s;
int n;
FILE *stream;
char *f;
#else
fgetrec(char *s, int n, FILE *stream, char *f)
#endif
{
	char  *p;
	char  *l;
	unsigned char *ignore;
	unsigned char *code;
	char *tb;
	char *eb;
	char *yb;
	char *xb;
	static char *xbb;

	static int recl = 0;
	int k;
	struct field *fp;

	char savechar;
	int i, j, rc;
	if ( (cmpset == 0) || (Aflag == 1) ) {
		recl = 0;
		if (fgets(s, n, stream) == NULL)
			return (0);
		if ((rc = strlen(s)) == 0)
			return (0);
		else	return (rc);
	}

			/* The following code will prepend the record
			   with an encoded version of the sort keys.
			 */
	if (recl != 0) {
		if (recl < n) {
			memcpy(s, xbb, recl+1);
			j = --recl;
			recl = 0;
			return(j);
		} else {
			memcpy(s, xbb, n);
			xbb += n;
			recl -= n;
			return(n);
		}
	}
	tb = tbuf;
	if (fgets(tb, SORT_LINE_MAX-1, stream) == NULL) {
		rc = 0;
	}
	else	rc = strlen(tb);

	if (rc == 0)
		return (rc);
	
	te = &(tb[strlen(tb)-1]);
	if (*te == '\n') {
		te += 2;
		rc++;
	} 
	else {	
		if (rc < (SORT_LINE_MAX-1)) {
				/* the last record of the input */
				/* file is missing a NEWLINE    */
			if(f == NULL) diag(MSGSTR(NEWLINE,
			  "warning: missing NEWLINE added at EOF"), "");
			else diag(MSGSTR(NEWLINE2,
			  "warning: missing NEWLINE added at end of input file ")
					, f);
			*++te = '\n';
			*++te = '\0';
		}
		else {
			fprintf(stderr,MSGSTR(TOOLONG,
					"fatal: line too long %d\n"),SORT_LINE_MAX);
			term();
		}
	}
	eb = ebuf;
	if ((rc * 5) < n) {
		xb = xbb = s;
		xe = s + n - 1;
	} else {
		xb = xbb = xbuf;
		xe = xbuf + (sizeof(xbuf) -1);
	}
	xb += 2;		/* save space for length */
	if (nfields > 0) {
		*xb++ = '\0';
		*xb++ = '\2';
	}
	for(k = nfields>0; k<=nfields; k++) {
		fp = &fields[k];
		p = tb;
		if(k >= 0) {
			l = skip(p, fp, 1);
			p = skip(p, fp, 0);
		} else {
			l = eol(p);
		}
		if((fp->fcmp==NUM) || (fp->fcmp==XNUM)) {
			fp->fcmp=XNUM;
			j = l - p + 3;
			*xb++ = (j >> 8);
			*xb++ = (j & 0xff);
			if ((l-p) > (xe - xb - 1)) {
				if (xbb == s) {
					j = xb - xbb;
					memcpy(xbuf, xbb, j);
					xb = xbb = xbuf;
					xb += j;
					xe = xbuf + (sizeof(xbuf) -1);
				}
				else {  fprintf(stderr,MSGSTR(TOOLONG,
					"fatal: line too long %d\n"),SORT_LINE_MAX);
					term();
				}
			}
			memcpy(xb, p, (l-p));
			xb += (l-p);
			*xb++ = '\0';
			continue;
		}

		fp->fcmp=XSTR;
		eb = ebuf;
		yb = xb;
		xb += 2;
		code = fp->code;
		ignore = fp->ignore;
		if ( (ignore != dict) && (ignore != nonprint) && 
							(code != fold) ) {
			savechar = l[0];
			*l = '\0';
			j = strxfrm(xb, p, (xe - xb - 1));
			if (j  > (xe - xb - 1)) {
				if (xbb == s) {
					i = xb - xbb;
					memcpy(xbuf, xbb, i);
					yb = xb = xbb = xbuf;
					yb += i-2;
					xb += i;
					xe = xbuf + (sizeof(xbuf) -1);
					j = strxfrm(xb, p, (xe - xb - 1));
				}
			}
			if (j  < (xe - xb - 1)) {
				yb[0] = ((j+3) >> 8);
				yb[1] = ((j+3) & 0xff);
				xb += ++j;
				l[0] = savechar;
			}	
			else { fprintf(stderr,MSGSTR(TOOLONG,
					"fatal: line too long %d\n"),SORT_LINE_MAX);
				term();
			}
		} else {
			fldtowa(p, l, eb, ignore, code);
			j = strxfrm(xb, ebuf, (xe - xb - 1));
			if (j  > (xe - xb - 1)) {
				if (xbb == s) {
					i = xb - xbb;
					memcpy(xbuf, xbb, i);
					yb = xb = xbb = xbuf;
					yb += i-2;
					xb += i;
					xe = xbuf + (sizeof(xbuf) -1);
					j = strxfrm(xb, ebuf, (xe - xb - 1));
				}
			}
			if (j  < (xe - xb - 1)) {
				j = strxfrm(xb, ebuf, (xe - xb - 1));
				xb += ++j;
				j += 2;
				*yb++ = (j >> 8);
				*yb = (j & 0xff);
			} else { 
				fprintf(stderr,MSGSTR(TOOLONG,
					"fatal: line too long %d\n"),SORT_LINE_MAX);
				term();
			}
		}
	}
	recl = xb - xbb;
	xbb[0] = (recl >>  8);
	xbb[1] = (recl & 0xff);
	if (rc > (xe - xb - 1)) {
		if (xbb == s) {
			i = xb - xbb;
			memcpy(xbuf, xbb, i);
			xb = xbb = xbuf;
			xb += i;
			xe = xbuf + (sizeof(xbuf) -1);
		}
	}
	if (rc < (xe - xb - 1)) 
		memcpy(xb, tb, rc+1);
	else { 
		fprintf(stderr,MSGSTR(TOOLONG,
			"fatal: line too long %d\n"),SORT_LINE_MAX);
		term();
	}
	recl += rc;
	if (recl < n) {
		if (s != xbb)
			memcpy(s, xbb, recl+1);
		j = --recl;
		recl = 0;
		return(j);
	}
	else {
		memcpy(s, xbb, n);
		s[n] = '\0';
		xbb += n;
		recl -= n;
		return(n);
	}

}
/*
 * NAME: fldtowa
 * FUNCTION: copies input string between (p) and (l) to work area (b),
 *	     removing non-sorting characters (i.e. non-dictionary and/or
 *	     non-printing) and optionally folding into lowercase.
 */
#ifdef _NO_PROTO
fldtowa(p,l,b,ignore,code)
char *p;
char *l;
char *b;
unsigned char *ignore;
unsigned char *code;
#else
void 
fldtowa(char *p, char *l, char *b, unsigned char *ignore, unsigned char *code)
#endif
{
	wchar_t wc;
	char *sb;
	int j;

	sb = b;
	while (p < l) {
	     if (ignore == dict) {
		   while( !(NCisalnum(NCdechr(p)) || NCisspace(NCdechr(p))) )
			p += NLchrlen(p);
	     }
	     if (ignore == nonprint) {
		   while( !(NCisprint(NCdechr(p)) || *p == '\t' || *p == '\n') )
			p += NLchrlen(p);
	     }
	     if (code == fold) {
			wc = (wchar_t)NCtoupper(NCdechr(p));
			j = NCenc(&wc, b);
			b += j;
			p += j;
	     }		
	     else {
		if (NCisshift(*p))
			*b++ = *p++;
		*b++ = *p++;
	     }
	}
	*b = '\0';

 	/* if an input string contains only non-sorting characters, treat it 
	   as a null string.						     */
	if (NLstrcmp(sb, "\n") == 0)
		*sb = '\0';
} 

/*
 * NAME: xfrmtos 
 * FUNCTION: convert transformed strxfrm data to untransformed data. 
 * function will strip leading strxfrm data.
 */
char *
xfrmtos(char *s)
{
	size_t rlen;

	if (cmpset == 1) {
		if (!outflag) {
			if (Aflag)
				rlen = strlen(s);
			else
				rlen = ((s[0] << 8) | (s[1]));
			fwrite(s, 1, rlen, os);
		}
		if (!Aflag)
			s += ((s[0] << 8) | (s[1]));
	} 
	return (s);
}
