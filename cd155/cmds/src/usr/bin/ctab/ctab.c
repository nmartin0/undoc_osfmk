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
static char rcsid[] = "@(#)$RCSfile: ctab.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:22:00 $";
#endif
/*
 * CMDNLS: ctab
 *
 * ORIGINS: 27, 10
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * ctab.c	1.23  com/cmd/nls,3.1,9021 5/9/90 17:02:09
 */                                                                   

#include <stdio.h>
#ifndef _BLD
#include <locale.h>	/* for call to setlocale */
#include <unistd.h>
#else
#ifndef	_WCHAR_T
#define _WCHAR_T
typedef unsigned short wchar_t;
#endif
#endif

#ifdef _BLD
#define NCstrlen	BLD_NCstrlen
#endif

#ifdef MSG
#include "ctab_msg.h"
#endif

#include <NLctype.h>

/*
 * NOTE!!!  The following defines for character classification must EXACTLY
 *	    match those in <ctype.h>.  They are defined here for cross-system
 *	    development - the ctab utility needs to have the knowledge of
 *	    the character classifications for an OSF target system, without
 *	    regard for these definitions in the host system's ctype.h.
 */
#undef	_U
#undef	_L
#undef	_N
#undef	_S
#undef	_P
#undef	_C
#undef	_B
#undef	_X
#undef	_A
#undef	_G
#define _U         01
#define _L         02
#define _N         04
#define _S        010
#define _P        020
#define _C        040
#define _B       0100
#define _X       0200
#define _A       0400
#define _G      01000

#ifdef _BLD
#ifdef KJI
#include <sys/jctype0.h>
#include <sys/jctype1.h>
#endif
#endif

#define LINE 256    /* input line length */
#define FIELDS 4    /* number of valid fields in input file */
#define FIRST 1     /* first field */
#define SECOND 2    /* second field */
#define THIRD 3     /* third field */
#define FOURTH 4     /* fourth field */

#define IGNORE 1
#define SEP 2
#define TRANS 3
#define REPEAT 4
#define COMMENT 5
#define XVS    6

/* Error codes */
#define ENOFILE         1       /* No file given */
#define EOIFILE         2       /* Error opening input file, no such file */
#define EOOFILE         3       /* Error opening output file */
#define EWOFILE         4       /* Error writing to output file */
#define ECLOSEI         5       /* Can't close input file */
#define ECLOSEO         6       /* Can't close output file */
#define ENOSEEK         7       /* Can't seek in output file */
#define ENOCHAR         8       /* No subject character */
#define ECASECONV       9       /* More than one char given for case field */
#define EEXTRAC         10      /* Extraneous character in subject field */
#define EREPEAT         11      /* Unrecognized repeat or trans character */
#define ETRANS          12      /* Must be single character for translation */
#define EOPTION         13      /* Unknown option */
#define EBLOPT          14      /* No value given for blank option */
#define ESEPOPT         15      /* No value given for sep option */
#define ETRANSOPT       16      /* No value given for trans option */
#define EREPOPT         17      /* No value given for repeat option */
#define ECOMOPT         18      /* No value given for comment option */
#define EONEOPT         19      /* Only one character allowed as option value */
#define EINVFIELD       20      /* Invalid field given to proc_char() */
#define ETHIRDEXTRA     21      /* Extraneous characters in third field */
#define EINVCASE        22      /* Third field must be one of */
                                /* uUlLcCsSaApPxGgXnN- */
#define EMULTC          23      /* Multiple entry of same character */
#define EINVTRANS       24      /* Illegal translation */
#define ECOLLATE        25      /* Base character's collate sequence not given*/
#define EOCTAL          26      /* Invalid octal escape sequence */
#define ENOTRANSC       27      /* No translate sequence given */
#define EINVTHIRD       28      /* Invalid case specifier in third field */
#define EINVFOURTH      29      /* Fourth field no longer supported */
#define EINVZERO        30      /* \000 only allowed first... */
#define EINVCHAR	31      /* Invalid character encountered */

#define IGNORECHAR '@'   /* '@' is the default ignore in collation char */
#define SEPCHAR ':'      /* ':' is the default separator of fields */
#define TRANSCHAR '|'    /* '|' is the default translate character */
#define REPEATCHAR '^'   /* '^' is the default same as last line char */
#define COMMENTCHAR '#'  /* '#' is the default comment character */

#define CTBLSIZE	258
#ifdef KJI
#define TBLSIZE		NLCOLMAX+1
#else
#define TBLSIZE 	NLCHARMAX+1
#endif

/* select JLS */
#ifdef KJI
#define NCflatchr(x)	(x)
#define codetype	257
#define cur_max		2
#define cur_min		1
#define dsp_width	0
#else
#define codetype	1
#define cur_max		1
#define cur_min		1
#define dsp_width	1
#endif

/* valid code point test */
#define isvalid(i)      NCisNLchar(i)

static char *ctabFout = "ctab.out";
static char *ctabFin = "ctab.in";

struct Newcoldesc {
	union {
		wchar_t *strptr;
		short offset;
	} str;
	short strlen;
	union {
		wchar_t *strptr;
		short offset;
	} rep;
	short replen;
	short cd_cval;
	short cd_cuniq;
	short trans;
};
/*
 *  Elements in coldesc contain four fields: 
 *	cd_stroff	the offset from the start of the lc_strings array 
 *			of a collating string,
 *	cd_repoff	the offset from the start of the lc_strings array
 *			of a replacement string
 *	cd_cval		a collate value
 *	cd_cuniq	a coluniq value
 *
 *  The fields are interpreted as follows:
 *      cd_stroff  cd_repoff  cd_cval   cd_cuniq
 *      ---------  ---------  -------   --------
 *       "from"      "to"       0        uniq      n-to-n mapping
 *       "from"        0       coll      uniq      multi-char coll symbol
 *         0           0       coll      uniq      default for char
 *     
 *  IF the "cd_stroff" field is non-zero and the cd_cval is zero,
 *  then the entry defines a many-to-many replacement mapping.
 *  If the input matches the "cd_stroff" string, then it is  
 *  replaced by the replacement string. The input pointer is modified
 *  to point past string to be replaced, the replacement pointer is
 *  set to the replacement string, and -1 is returned.
 *  
 *  If the "repoff" field in zero, and "stroff" is non-zero, then 
 *  the entry defines a multi-character collating element (e.g. Spanish
 *  'ch'); input pointer is set past element, replacement pointer is
 *  set to point to unique value, and collation value is returned.
 *  
 *  If "cd_stroff" and "cd_repoff" are zero, then the entry defines the
 *  collating (and coluniq) value for the character itself; the replacement
 *  pointer is set to point to coluniq value, and collation value is
 *  returned.
 */

struct Newcoldesc user_col[TBLSIZE];
coldesc_t coldesc;

wchar_t	caseconv[CTBLSIZE];
unsigned short ctype[CTBLSIZE];
/*  Note that the collate table must be signed, to allow for negative values.
 */
short	collate[TBLSIZE];
wchar_t coluniq[TBLSIZE];

loc_t NLloc;
col_t NLcol;
ctype_t NLctype;
tim_t NLtim;
num_t NLnum;
mon_t NLmon;
msg_t NLmsg;
map_t NLmap;


wchar_t subj_case, lowc, comment, sep, repeatc, ignc, trans, lasteqc;
int flatc, multc, sequence, unique, colindex, is_alpha, linenum;
int ackstrlen, firstofeqv;
int fourth = 0;
int xvs = 0;
int startcval;
wchar_t *get_string(), *nexttok();
wchar_t *subj_char, *o_char, *p_char;
FILE *fopen(), *ifile, *ofile;
char *msgtab[] =
{
	"",                                               /*  0 */
	"No filename given.",                             /*  1 */
	"No such file: ",                                 /*  2 */
	"Can't open output file for write.",              /*  3 */
	"Can't Write to output file.",                    /*  4 */
	"Can't close input file.",                        /*  5 */
	"Can't close output file.",                       /*  6 */
	"Can't seek in output file.",                     /*  7 */
	"Must specify subject character.",		  /*  8 */
	"Only one character is permitted for case.",      /*  9 */
	"Extraneous characters in subject field.",        /* 10 */
	"Unrecognized repeat or trans character.",        /* 11 */
	"Subject must be single character for trans.",    /* 12 */
	"Unknown option: ",                               /* 13 */
        "No character supplied for blank option.",        /* 14 */
	"No character supplied for sep option.",          /* 15 */
	"No character supplied for trans option.",        /* 16 */
	"No character supplied for repeat option.",       /* 17 */
	"No character supplied for comment option.",      /* 18 */
	"Only one character is allowed in option: ",      /* 19 */
	"Internal error, invalid field.",                 /* 20 */
	"Extraneous characters in third field.",          /* 21 */
	"Third field must be one of lLuUcCsSaApPxGgXnN-", /* 22 */
	"Multiple entries of the same character.",        /* 23 */
	"Illegal translation.",                           /* 24 */
	"Collate sequence needed for:",                   /* 25 */
	"Invalid octal escape sequence.",                 /* 26 */
	"No translate characters given.",                 /* 27 */
	"Invalid case specifier in third field.",         /* 28 */
	"Warning: Fourth field no longer supported.",     /* 29 */
	"Character \\000 only allowed first.",            /* 30 */
	"Invalid character encountered in input.",        /* 31 */
	0
};

/*
 *      This is the default table for characters not specified in
 *      the current ctab input file.  The PC932 code page used by
 *      the KJ system for hex values 80-FF.
 */

unsigned short type[] = {
/*       0        1        2        3        4        5        6        7  */
/* 0*/  _C,      _C,      _C,      _C,      _C,      _C,      _C,      _C,
/* 10*/ _C,      _S|_C,   _S|_C,   _S|_C,   _S|_C,   _S|_C,   _C,      _C,
/* 20*/ _C,      _C,      _C,      _C,      _C,      _C,      _C,      _C,
/* 30*/ _C,      _C,      _C,      _C,      _C,      _C,      _C,      _C,
/* 40*/ _S|_B,   _P,      _P,      _P,      _P,      _P,      _P,      _P,   
/* 50*/ _P,      _P,      _P,      _P,      _P,      _P,      _P,      _P,   
/* 60*/ _N|_X,   _N|_X,   _N|_X,   _N|_X,   _N|_X,   _N|_X,   _N|_X,   _N|_X,
/* 70*/ _N|_X,   _N|_X,   _P,      _P,      _P,      _P,      _P,      _P,
/*100*/ _P,      _A|_U|_X,_A|_U|_X,_A|_U|_X,_A|_U|_X,_A|_U|_X,_A|_U|_X,_A|_U,
/*110*/ _A|_U,   _A|_U,   _A|_U,   _A|_U,   _A|_U,   _A|_U,   _A|_U,   _A|_U,
/*120*/ _A|_U,   _A|_U,   _A|_U,   _A|_U,   _A|_U,   _A|_U,   _A|_U,   _A|_U,
/*130*/ _A|_U,   _A|_U,   _A|_U,   _P,      _P,      _P,      _P,      _P,
/*140*/ _P,      _A|_L|_X,_A|_L|_X,_A|_L|_X,_A|_L|_X,_A|_L|_X,_A|_L|_X,_A|_L,
/*150*/ _A|_L,   _A|_L,   _A|_L,   _A|_L,   _A|_L,   _A|_L,   _A|_L,   _A|_L,
/*160*/ _A|_L,   _A|_L,   _A|_L,   _A|_L,   _A|_L,   _A|_L,   _A|_L,   _A|_L,
/*170*/ _A|_L,   _A|_L,   _A|_L,   _P,      _P,      _P,      _P,      _C,
#ifdef KJI
/*200*/  0,       0,       0,       0,       0,       0,       0,       0,
/*210*/  0,       0,       0,       0,       0,       0,       0,       0,
/*220*/  0,       0,       0,       0,       0,       0,       0,       0,
/*230*/  0,       0,       0,       0,       0,       0,       0,       0,
/*240*/ _G,      _P,      _P,      _P,      _P,      _P,      _G,      _G,
/*250*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*260*/ _G ,     _G ,     _G ,     _G ,     _G ,     _G ,     _G ,     _G ,
/*270*/ _G ,     _G ,     _G,      _G,      _G,      _G,      _G,      _G,
/*300*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*310*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*320*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*330*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*340*/  0,       0,       0,       0,       0,       0,       0,       0,
/*350*/  0,       0,       0,       0,       0,       0,       0,       0,
/*360*/  0,       0,       0,       0,       0,       0,       0,       0,
/*370*/  0,       0,       0,       0,       0,       0,       0,       0,
#else
/*200*/  0,       0,       0,       0,       0,       0,       0,       0,
/*210*/  0,       0,       0,       0,       0,       0,       0,       0,
/*220*/  0,       0,       0,       0,       0,       0,       0,       0,
/*230*/  0,       0,       0,       0,       0,       0,       0,       0,
/*240*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*250*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*260*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*270*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*300*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*310*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*320*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*330*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*340*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*350*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*360*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
/*370*/ _G,      _G,      _G,      _G,      _G,      _G,      _G,      _G,
#endif
/*400*/  0
};

static unsigned long swapshort();
static unsigned long swaplong();
int	swapflag;

/*-----------------------------main()------------------------------*/
/*
 *      Process the options and call the major procedures to complete
 *      the task.
 */
main(argc, argv)
{
	int c, errflg = 0;
	extern int optind;
	extern char *optarg;


#ifndef _BLD /* if not for the build environment */
	setlocale(LC_ALL, "");
#endif

	swapflag = 0;
	while ((c = getopt(argc, argv, "i:o:s")) != EOF)
	{
	    switch(c)
	    {
		case 'i':
		    ctabFin = optarg;
		    break;
		case 'o':
		    ctabFout = optarg;
		    break;
		case 's':
		    swapflag++;
		case '?':
		    errflg++;
	    }
	}
	if (errflg)
	{
#ifdef MSG
	    fprintf(stderr,NLgetamsg(MF_CTAB, MS_CTAB, M_USAGE,
		    "usage: ctab [-i infile] [-o outfile] \n"));
#else
	    fprintf(stderr,"usage: ctab [-i infile] [-o outfile] \n");
#endif
	    exit(2);
	}

	init();
	proc_file();
	check_coldesc();
	proc_default();
	finishup();
	exit(0);
}

/*-----------------------------init()------------------------------*/
init()
{
	int i;

	if ((ifile = fopen(ctabFin,"r")) == NULL)
	    err(EOIFILE, ctabFin);
	if ((ofile = fopen(ctabFout, "w")) == NULL)
	    err(EOOFILE, "");
	repeatc = REPEATCHAR;
	trans = TRANSCHAR;
	sep = SEPCHAR;
	comment = COMMENTCHAR;
	ignc = IGNORECHAR;
	lowc = TBLSIZE;
	lasteqc = TBLSIZE;
	colindex = 1;
	linenum = 1;
	is_alpha = 0;
	ackstrlen = 0;

	/*
	 * Lowest possible collation value is set to 0x101.
         */
	startcval = 257;

	sequence = startcval;
	unique = startcval;

	/*
	 * Initialize the ctype ASCII part.
	 */
	for (i=0; i< (sizeof(type)/sizeof(short)) ; i++)
	    ctype[i+1] = type[i];
	/*
	 * Initialize the caseconv table
	 */
	for (i=0; i < 256; i++)
	{
		if ((i >= 'A') && (i <= 'Z'))
			caseconv[i+1] = i+32;
		else if ((i >= 'a') && (i <= 'z'))
			caseconv[i+1] = i-32;
		     else caseconv[i+1] = 0;
	}
}

/*-----------------------------proc_file()-------------------------*/
proc_file()
{

	unsigned char buff[LINE];

	while (fgets((char *)buff, LINE, ifile) != NULL)
	{
	    proc_line(buff);
	    linenum++;
	}
	linenum = -1;
}

/*-----------------------------proc_line()-------------------------*/
/*
 *      First convert all escaped octal values to their char values,
 *      then convert the whole line to wchar_t's.  Then check to see
 *      if it is an options line, if so process it, else remove the
 *      white space and break it up into individual fields for proc_char()
 *      to handle.
 *      Also updates the collate value, as the user specified line sequence
 *      determines that.
 */

proc_line(s) 
unsigned char *s;	/* input buffer */
{
	register wchar_t *dest, *src;

	wchar_t t[LINE];	/* copy of input buffer except that all escaped
			 * octal values have been converted to chars.
			 */

	wchar_t *sptr;	/* temp pointer to input buffer */
	wchar_t *tptr;	/* pointer to t buffer */
	unsigned char ct;	/* pointer to t buffer */

	wchar_t buf[LINE], buf1[LINE], field_buf[LINE], c, *s1;
	wchar_t tok[LINE], *ostr;
	int i, len, skip;
	wchar_t num;		/* value of number after '\' */

	for (i = 0; i < LINE; i++)
		buf[i] = buf1[i] = field_buf[i] = tok[i] = t[i] = 0;

	subj_char = o_char = NULL;
	multc = 0;
	/* convert input string to NLchar */
	i = NCdecstr(s, buf, strlen(s)+1);

	s1 = buf;
	subj_case = 0;

	/* process 'option' token if any.  nexttok() expects NLchar string */
	ostr = nexttok(s1, tok, &len, &skip);
	if (ostr && cmpstr("option", ostr, 6)) 
	{
		options(s1+6);
		return;
	}

	rm_white_sp(buf, buf1);	/* buf1 points at NLchar str w/o white space */
	if (*buf1 == '\0')
		return;		/* nothing in the current line */


	src = buf1;		/* point at NLchar string */
	for (i = 1; i <= FIELDS; i++) 
	{
		dest = field_buf;

		/* This while loop copies the contents of the current field
		 * to a buffer for processing by proc_char.
		 */
		while ((c = *src++) != sep && c != '\0' && c!= '\n') 
		{
			if (c == '\\')
			{
				*dest++ = c;
				*dest++ = *src++;
			}
			else 
				*dest++ = c;
		}

		/* 
		 * Proc_char expects a null terminated field.
		 */
		*dest = '\0';

		/* process field i of line */
		proc_char(field_buf, i);
	}
	sequence++;
}

/*-----------------------------cmpstr()----------------------------*/
/*
 *      This routine converts an ascii string into NLchar string and
 *      then compares it with the second string.
 */
cmpstr(s, s1, len)
char *s;
wchar_t *s1;
int len;
{
	wchar_t buffer[LINE], *s2;
	int i, l;

	l = NCdecstr(s, buffer, len+1);
	s2 = buffer;
	for (i = 0; i < l; i++)
	{
	    if (!(*s2++ == *s1++))
		return(0);
	}
	if (*s1)
	    return(0);
	else
	    return(1);
}

/*-----------------------------rm_white_sp()-----------------------*/
/*
 *    White spaces are tabs and spaces.  This routine removes them,
 *    but keeps the escaped ones along with the backslash.  It also
 *    gets rid of comments and blank lines.
 */
rm_white_sp(in, out)
register wchar_t *in, *out;
{
	wchar_t c, lastchar = (wchar_t)0;

	while (*in)
	{
	    while ((c = *in++) != '\t' && c!=' ' && c!=comment && c!= '\n' && c!='\0')
		lastchar = *out++ = c;
	    if (lastchar != '\\' && (c == comment || c == '\n'))
		break;
	    if (lastchar == '\\')    /* copy the white space */
		*out++ = c;          /* if it was escaped */
	}
	*out = '\0';
}

/*-----------------------------proc_default()----------------------*/
/*
 *      Fill the rest of the code points and assign them collating
 *      values.  Those with code points below the lowest supplied by
 *      the user comes first, all others go after the user supplied
 *      code points.
 *
 */  

proc_default()
{
	int i, pos, posf;

	sequence = unique;

	/*
	 * We compute the lowest value (not including ignores) input by
         * the user.  This is so that when we process the whole
	 * range of code points not supplied by the user, we can
	 * determine where to collate those that come before the lowest
	 * character value supplied by the user.
	 */

	for (lowc = 0; lowc < 65534; lowc++)
	{
	    if (isvalid(lowc))
	    {
		if (coluniq[_NCmap(lowc)+1] > 1)
			break;
	    }
	}
	if (lowc == 0)
		pos = lowc;
	else	pos = ((lowc + (lowc/256) + 0x100) & 0xff00);
					/* number of chars before user-
					supplied list begins, rounded up
					to nearest multiple of 256 */

	/* 
	 * Here we increment all assigned collation values (except
	 * ignores) with the adjusted "lowc" value.
         */

	for (i = 0; i < 65534; i++)
	{
	    if (isvalid(i))
	    {
		if (collate[_NCmap(i)+1])
		{
			if (collate[_NCmap(i)+1] > 1)
			{
				collate[_NCmap(i)+1] += pos;
				coluniq[_NCmap(i)+1] += pos;
			}
			else 	if (coluniq[_NCmap(i)+1] != 1)
					coluniq[_NCmap(i)+1] += pos;
		}
	    }
	}
	for (i = 0; i < (colindex - 1); i++)
	{
	    if (user_col[i].cd_cval > 1)
		user_col[i].cd_cval += pos;
	    if (user_col[i].cd_cuniq > 1)
		user_col[i].cd_cuniq += pos;
	    if (user_col[i].cd_cval == 1) {
			user_col[i].cd_cval = 0;
			user_col[i].cd_cuniq = 0;
	    }
	}

	/*
	 * The following code walks through the arrays and assigns values
	 * to all unassigned characters. If lower than "lowc", they are
         * given values between starting value (which is 0x101) and lowc.
         * If they are higher than "lowc", ther are given values higher
         * than the highest assigned value.
	 * The assigned values are also checked and adjusted to avoid any 
	 * collation value containing a zero byte.
	 */

	sequence += pos;
	unique = sequence;
	pos = startcval;
	for (i = 0; i < 65534; i++)
	{
	    if (i < lowc)			/* if less than lowest val */
	    {					/* assigned by user */
		if (isvalid(i))                 /* a macro to check that */
		{                               /* this is a valid code point */
		    if (coluniq[_NCmap(i)+1] == 1) 
		    {
 		    	if (collate[_NCmap(i)+1] == 1)
 		    		collate[_NCmap(i)+1] = 0;
 		    	coluniq[_NCmap(i)+1] = 0;
 		    }
 		    else
 		    {
 		    	if (!(pos & 0377))	/* if value cont. zero byte */
 			    pos++;		/* increment w 1 */
 		    	coluniq[_NCmap(i)+1] = pos;
 		    	collate[_NCmap(i)+1] = pos++;
 		    }
		}
	    }
	    else
	    {
		if (isvalid(i))
		{
		    if (collate[_NCmap(i)+1] == 0)
		    {
			if (!(unique & 0377))
			{
			    unique++;
			    sequence++;
			}
			coluniq[_NCmap(i)+1] = unique++;
			collate[_NCmap(i)+1] = sequence++;
		    }
		    else
			if (coluniq[_NCmap(i)+1] == 1) 
			{
			    if (collate[_NCmap(i)+1] == 1)
			    	    collate[_NCmap(i)+1] = 0;
			    coluniq[_NCmap(i)+1] = 0;
 			}
		}
	    }
	}
}

/*-----------------------------finishup()--------------------------*/
/*
 *      This routine writes out the internal structure to the output
 *      file specified by the user.  It first writes out the multi-character
 *      strings at a location after the NLcoldesc structures.  So it seeks
 *      there base on the number of NLcoldesc structures allocated during
 *      input processing.
 *
 */  

finishup()
{
	int i, j, len;
	short offset;
	wchar_t *ptr, *optr, *xptr;
	long pointer, spointer;
	long s_loc, s_chr, s_col, s_mon, s_num, s_tim, s_msg, s_map;
	long s_caseconv, s_ctype;
	long s_collate, s_coluniq, s_eqvmap, s_coldesc, s_strings;
	long size;

	/* determine string offsets for coldesc and pack strings for writing */
	optr = ptr = (wchar_t *) malloc (sizeof(wchar_t) * (ackstrlen));
	offset = 1;
	ptr += offset;
	for (i = 0; i < (colindex - 1); i++)
	{
	    if (user_col[i].trans) 
	    {				/* len is in shorts! */
	    	len = user_col[i].strlen + 1;
		xptr = user_col[i].str.strptr;
		for (j=0; j < len; j++)
			*ptr++ = *xptr++;
	    	user_col[i].str.offset = offset;
	    	offset += len;  
	    	len = user_col[i].replen + 1;
		xptr = user_col[i].rep.strptr;
		for (j=0; j < len; j++)
			*ptr++ = *xptr++;
	    	user_col[i].rep.offset = offset;
	    	offset += len;  
		continue;
	    }
	    if (user_col[i].strlen == 1 && !user_col[i].cd_cval)
		continue;
	    len = user_col[i].strlen + 1;
	    xptr = user_col[i].str.strptr;
	    for (j=0; j < len; j++)
			*ptr++ = *xptr++;
	    user_col[i].str.offset = offset;
	    offset += len;  
	}		

	/* determine length of tables */
	/* Roundup to the nearest int boundary */
	s_loc = ((sizeof(NLloc)   + sizeof(int)-1)/sizeof(int)) * sizeof(int);
	s_chr = ((sizeof(NLctype) + sizeof(int)-1)/sizeof(int)) * sizeof(int);
	s_col = ((sizeof(NLcol)   + sizeof(int)-1)/sizeof(int)) * sizeof(int);
	s_mon = ((sizeof(NLmon)   + sizeof(int)-1)/sizeof(int)) * sizeof(int);
	s_num = ((sizeof(NLnum)   + sizeof(int)-1)/sizeof(int)) * sizeof(int);
	s_tim = ((sizeof(NLtim)   + sizeof(int)-1)/sizeof(int)) * sizeof(int);
	s_msg = ((sizeof(NLmsg)   + sizeof(int)-1)/sizeof(int)) * sizeof(int);
	s_map = ((sizeof(NLmap)   + sizeof(int)-1)/sizeof(int)) * sizeof(int);

	s_caseconv = ((sizeof(caseconv) + sizeof(int)-1)/sizeof(int)) * 
                     sizeof(int);

	s_ctype = ((sizeof(ctype)  + sizeof(int)-1)/sizeof(int)) * sizeof(int);
	s_collate = ((sizeof(collate) + sizeof(int)-1)/sizeof(int)) * sizeof(int);
	s_coluniq = ((sizeof(coluniq) + sizeof(int)-1)/sizeof(int)) * sizeof(int);
	s_coldesc = (colindex - 1) * sizeof(coldesc_t);
	s_strings = (ptr - optr) * sizeof(wchar_t);

	NLloc.lc_mag0 = NLCTMAG0;
	NLloc.lc_mag1 = NLCTMAG1;
	NLloc.lc_version = swapshort(1);
	NLloc.lc_length = swapshort(s_loc);

	pointer = 0;		/* offset from beginning of file */

	/* character classification table */
	pointer += s_loc;	/* now points at chr class info in file */
	NLloc.lc_chrtbl = (ctype_t *) swaplong(pointer);
	pointer += s_chr;	/* prepare for next setting of pointer */
	NLctype.lc_version = swapshort(1);
	NLctype.lc_length = swapshort(s_chr);	/* length of ctype_t */
	NLctype.lc_code_type = swapshort(codetype);
	NLctype.mb_cur_max = swapshort(cur_max);
	NLctype.mb_cur_min = swapshort(cur_min);
	NLctype.lc_dsp_width = swapshort(dsp_width);
	NLctype.lc_locale_name = (char *) 0;
	NLctype.len_caseconv = swaplong(sizeof(caseconv));
	NLctype.lc_caseconv = (wchar_t *) swaplong(pointer);
	pointer += s_caseconv;
	NLctype.len_ctype = swaplong(sizeof(ctype));
	NLctype.lc_ctype = (unsigned short *) swaplong(pointer);
	pointer += s_ctype;

	/* collation table */
	NLloc.lc_coltbl = (col_t *) swaplong(pointer);
	pointer += s_col;
	NLcol.lc_version = swapshort(1);
	NLcol.lc_length = swapshort(sizeof(col_t));
	NLcol.lc_locale_name = (char *) 0;
	NLcol.len_collate = swaplong(sizeof(collate));
	NLcol.lc_collate = (short *) swaplong(pointer);
	pointer += s_collate;
	NLcol.len_coluniq = swaplong(sizeof(coluniq));
	NLcol.lc_coluniq = (short *) swaplong(pointer);
	pointer += s_coluniq;
	NLcol.len_coldesc = swaplong(s_coldesc);
	NLcol.high_cvalue = swaplong(unique);

	if (s_coldesc) {
		NLcol.lc_coldesc = (coldesc_t *) swaplong(pointer);
		pointer += s_coldesc;
	}
	else
		NLcol.lc_coldesc = (coldesc_t *) 0;

	NLcol.len_strings = swaplong(s_strings); 
	if (s_strings)
		NLcol.lc_strings = (wchar_t *) swaplong(pointer); 
	else 
		NLcol.lc_strings = NULL;

	pointer += s_strings;

	/* monetary table */
	NLloc.lc_montbl = (mon_t *) swaplong(pointer);
	pointer += s_mon;
	NLmon.lc_version = swapshort(1);
	NLmon.lc_length = swapshort(sizeof(mon_t));
	NLmon.lc_locale_name = (char *) 0;
	NLmon.int_curr_symbol = (char *) 0;
	NLmon.currency_symbol = (char *) 0;
	NLmon.mon_decimal_point = (char *) 0;
	NLmon.mon_thousands_sep = (char *) 0;
	NLmon.mon_grouping = (char *) 0;
	NLmon.positive_sign = (char *) 0;
	NLmon.negative_sign = (char *) 0;
	NLmon.int_frac_digits = 0;
	NLmon.frac_digits = 0;
	NLmon.p_cs_precedes = 0;
	NLmon.p_sep_by_space = 0;
	NLmon.n_cs_precedes = 0;
	NLmon.n_sep_by_space = 0;
	NLmon.p_sign_posn = 0;
	NLmon.n_sign_posn = 0;

	/* numeric table */
	NLloc.lc_numtbl = (num_t *) swaplong(pointer);
	pointer += s_num;
	NLnum.lc_version = swapshort(1);
	NLnum.lc_length = swapshort(sizeof(num_t));
	NLnum.lc_locale_name = (char *) 0;
	NLnum.decimal_point = (char *) 0;
	NLnum.thousands_sep = (char *) 0;
	NLnum.grouping = (char *) 0;

	/* time table */
	NLloc.lc_timtbl = (tim_t *) swaplong(pointer);
	pointer += s_tim;
	NLtim.lc_version = swapshort(1);
	NLtim.lc_length = swapshort(sizeof(tim_t));
	NLtim.lc_locale_name = (char *) 0;
	NLtim.t_fmt = (char *) 0;
	NLtim.d_fmt = (char *) 0;
	NLtim.nlldate = (char *) 0;
	NLtim.d_t_fmt = (char *) 0;
	NLtim.abday = (char *) 0;
	NLtim.day = (char *) 0;
	NLtim.abmon = (char *) 0;
	NLtim.mon = (char *) 0;
	NLtim.misc = (char *) 0;
	NLtim.tstrs = (char *) 0;
	NLtim.tunits = (char *) 0;
	NLtim.year = (char *) 0;
	NLtim.am_pm = (char *) 0;

	/* message table */
	NLloc.lc_msgtbl = (msg_t *) swaplong(pointer);
	pointer += s_msg;
	NLmsg.lc_version = swapshort(1);
	NLmsg.lc_length = swapshort(sizeof(msg_t));
	NLmsg.lc_locale_name = (char *) 0;
	NLmsg.messages = (char *) 0;
	NLmsg.yes_string = (char *) 0;
	NLmsg.no_string = (char *) 0;

	/* wchar_t mapping table */
	NLloc.lc_maptbl = (map_t *) swaplong(pointer);
	pointer += s_map;
	NLmap.lc_version = swapshort(1);
	NLmap.lc_length = swapshort(sizeof(map_t));
	NLmap.lc_identifier = 0;

	if (swapflag) {
		for (i = 0; i < CTBLSIZE; i++)
			caseconv[i] = swapshort(caseconv[i]);
		for (i = 0; i < CTBLSIZE; i++)
			ctype[i] = swapshort(ctype[i]);
		for (i = 0; i < TBLSIZE; i++)
			collate[i] = swapshort(collate[i]);
		for (i = 0; i < TBLSIZE; i++)
			coluniq[i] = swapshort(coluniq[i]);
	}

	if (fwrite((char *)&NLloc, s_loc, 1, ofile) != 1)
	    err(EWOFILE, "");
	if (fwrite((char *)&NLctype, s_chr, 1, ofile) != 1)
	    err(EWOFILE, "");
	if (fwrite((char *)caseconv, s_caseconv, 1, ofile) != 1)
	    err(EWOFILE, "");
	if (fwrite((char *)ctype, s_ctype, 1, ofile) != 1)
	    err(EWOFILE, "");
	if (fwrite((char *)&NLcol, s_col, 1, ofile) != 1)
	    err(EWOFILE, "");
	if (fwrite((char *)collate, s_collate, 1, ofile) != 1)
	    err(EWOFILE, "");
	if (fwrite((char *)coluniq, s_coluniq, 1, ofile) != 1)
	    err(EWOFILE, "");
	for (i = 0; i < (colindex -1); i++)
	{
	    coldesc.cd_cval = swapshort(user_col[i].cd_cval);
	    coldesc.cd_cuniq = swapshort(user_col[i].cd_cuniq);
	    if (user_col[i].strlen == 1 && !user_col[i].trans)
	    {
		coldesc.cd_stroff = 0;
		coldesc.cd_repoff = 0;
	    } else {
	    	coldesc.cd_stroff = swapshort(user_col[i].str.offset);
	    	coldesc.cd_repoff = swapshort(user_col[i].rep.offset);
	    }
	    if (fwrite((char *)&coldesc, sizeof(coldesc_t), 1, ofile) != 1)
		    err(EWOFILE, "");
	} 

	if (s_strings > 0)
		if (fwrite((char *)optr, s_strings, 1, ofile) != 1)
			err(EWOFILE, "");
	if (fwrite((char *)&NLmon, s_mon, 1, ofile) != 1)
	    err(EWOFILE, "");
	if (fwrite((char *)&NLnum, s_num, 1, ofile) != 1)
	    err(EWOFILE, "");
	if (fwrite((char *)&NLtim, s_tim, 1, ofile) != 1)
	    err(EWOFILE, "");
	if (fwrite((char *)&NLmsg, s_msg, 1, ofile) != 1)
	    err(EWOFILE, "");
	if (fwrite((char *)&NLmap, s_map, 1, ofile) != 1)
	    err(EWOFILE, "");

	if (fclose(ifile) == EOF)
	    err(ECLOSEI, ctabFin);
	if (fclose(ofile) == EOF)
	    err(ECLOSEO, ctabFout);
}

/*-----------------------------proc_char()-------------------------*/
/*
 *    This routine is passed a field, null terminated, without the
 *    colon separator and processes the information provided.
 */
proc_char(s, field)
register wchar_t *s;
int field;
{
	register wchar_t *charptr;
	wchar_t xc;
	int i, len, skip, negi;
        unsigned long cl;
	wchar_t *ptr;

	charptr = s;
	switch(field)
	{
	/*
	 * The first field contains the subject character (subj_char).
	 * It is potentially a multi-character string, so we call
	 * get_string to malloc some memory space to store it and
	 * returns the address to be stored in subj_char.  charptr is the
	 * pointer that keeps track of position within the field.
	 */
	    case FIRST:
		subj_char = get_string(charptr, &len, &skip);
		charptr = charptr + len + skip;
		if (subj_char == NULL)
			err(ENOCHAR, "");       /* didn't get a subject char */
		if ((subj_char[0] == '\0') && (unique != startcval))
			err(EINVZERO, "");
		
	/*
	 * check if a special character was given, i.e. repeat character
	 * or translation character.
	 */
		if (!(unique & 0377))		/* ensure not containing any */
		    unique++;			/* zero bytes */
		i = get_a_char(charptr, &xc);
		charptr += i;
		if (!i)
		{
		    firstofeqv = 1;		/* "first of eqv. class" */
		    p_char = subj_char;		/* save ptr to char */
		    sequence = unique;
		    if (len > 1)
		    {
			multc = 1;
			colldesc(subj_char, len, unique++, sequence);
		    }
		    else
			setcollate(subj_char, unique++, sequence);
		    break;
		} else
		{
		    if (subj_char[0] == '\0')
			err(EEXTRAC, "");
		    if (xc == repeatc)		/* this is in eqv.class */
		    {
			if (firstofeqv)		/* is this the second? */
			{
						/* increment unique value
						for 1st in equ.class by 1 */
			    if (NCstrlen(p_char) > 1)
			    {
				negi = collate[_NCmap(p_char[0])+1];
				ptr = user_col[-negi - 1].str.strptr;
				while (p_char[0] == ptr[0])
				{
				    if (compare(p_char, ptr))
				    {
					user_col[-negi - 1].cd_cuniq = unique++;
					break;
				    }
				    negi--;
				    ptr = user_col[-negi - 1].str.strptr;
				}
			    } 	
			    else
			    {
				coluniq[_NCmap(p_char[0])+1] = unique++;
			    }
			    
			}
			firstofeqv = 0;		/* turn off until single char */
			--sequence;		/* reset sequence */
			if (len > 1)
			{
			    multc = 1;
			    colldesc(subj_char, len, unique++, sequence);
			}
			else
			    setcollate(subj_char, unique++, sequence);
		    } else
			if (xc == ignc)		/* ignore char for collation */
			{
			    --sequence;		/* reset sequence */
			    if (len > 1)
			    {
			        multc = 1;
			        colldesc(subj_char, len, 1, 1);
			    }
			    else
			        setcollate(subj_char, 1, 1);
			
			}
			else
			{
			    if (xc == trans)	/* 1-to-n mapping */
			    {
		    	    	sequence = unique;
			    	if (len > 1)
				    err(ETRANS, "");
			    	o_char = get_string(charptr, &len, &skip);
			    	if (!len)
				    err(ENOTRANSC, "");
			    	charptr = charptr + len + skip;
			    	set_trans(subj_char, o_char, 1, len);
			    } else
			    	    err(EEXTRAC, "");
			}
		}
	    /*
	     * Check if there are extraneous characters left over, if so
	     * then error condition.
	     */
		if (get_a_char(charptr, &xc))
		    err(EEXTRAC, "");
		break;

	case SECOND:
	    if (multc)
		break;
	/*
	 * This field provides the upper or lower case conversion
	 * character.  Since it is to be stored as a single character
	 * in the NLloc table, it will be an error condition if the user
	 * provides multiple characters in this field.
	 * The variable is_alpha is used to signal that we did get a character
	 * so that when we are processing the third field, we can set
	 * the appropriate constants.
	 */
	    i = get_a_char(charptr, &xc);
	    charptr += i;
	    if (i)
	    {
		subj_case = xc;
		is_alpha = 1;
		if (get_a_char(charptr, &xc))
		    err(ECASECONV, "");
	    } else      /* null field, thus default to subject character */
	    {
		is_alpha = 0;
		if (!caseconv[subj_char[0]+1])
			caseconv[subj_char[0]+1] = subj_char[0];
	    }
	    break;

	case THIRD:
	    if (multc)
		break;
	/*
	 * This field gets an indication of the character classification.
	 * The types supported are l: lower case, u: upper case, c: control,
         * s: space, a: alphabetic, p: punctuation, x: hex digit, 
         * n: numeric digit, or -:none (no type).
	 * If the user does not specify the third field, then
         * the default C locale classification will be used.  If the
         * character is not in the C locale, then it will be marked as
         * punctuation.
	 */
	    i = get_a_char(charptr, &xc);
            /*
             * If at least one type is specified, clear the default type
             * value.
             */
	    if (i)
	    {
                ctype[subj_char[0]+1] = 0;
	        do /* while (i = get_a_char(charptr, &xc)) */
	        {
                    charptr+=i;
		    switch(xc) {
                        /* upper case */
		        case 'u':
		        case 'U':
			    if ((ctype[_NCmap(subj_char[0])+1] & _C) ||
				 (ctype[_NCmap(subj_char[0])+1] & _N) ||
				  (ctype[_NCmap(subj_char[0])+1] & _P) ||
				   (ctype[_NCmap(subj_char[0])+1] & _S)) 
				err(EINVTHIRD);
			    if (!xvs && (subj_char[0] < 128) && 
			    		(!(type[_NCmap(subj_char[0])] & _U)))
				err(EINVTHIRD);
			    ctype[_NCmap(subj_char[0])+1] |= _U;
			    ctype[_NCmap(subj_char[0])+1] |= _A;
			    if (is_alpha == 1) 
			        caseconv[_NCmap(subj_char[0])+1] = subj_case;
			    break;
		        case 'l':
		        case 'L':
			    if ((ctype[_NCmap(subj_char[0])+1] & _C) ||
				 (ctype[_NCmap(subj_char[0])+1] & _N) ||
				  (ctype[_NCmap(subj_char[0])+1] & _P) ||
				   (ctype[_NCmap(subj_char[0])+1] & _S)) 
				err(EINVTHIRD);
			    if (!xvs && (subj_char[0] < 128) && 
			    		(!(type[_NCmap(subj_char[0])] & _L)))
				err(EINVTHIRD);
			    ctype[_NCmap(subj_char[0])+1] |= _L;
			    ctype[_NCmap(subj_char[0])+1] |= _A;
			    if (is_alpha == 1)
		 	        caseconv[_NCmap(subj_char[0])+1] = subj_case;
			    break;
		        case 'c':
		        case 'C':
			    if (!xvs && (subj_char[0] < 128) && 
			    		(!(type[_NCmap(subj_char[0])] & _C)))
				err(EINVTHIRD);
			    if ((ctype[_NCmap(subj_char[0])+1] & _U) ||
				 (ctype[_NCmap(subj_char[0])+1] & _L) ||
				  (ctype[_NCmap(subj_char[0])+1] & _A) ||
				   (ctype[_NCmap(subj_char[0])+1] & _G))
				err(EINVTHIRD);
			    ctype[_NCmap(subj_char[0])+1] |= _C;
			    break;
		        case 's':
		        case 'S':
			    if ((ctype[_NCmap(subj_char[0])+1] & _U) ||
				 (ctype[_NCmap(subj_char[0])+1] & _L) ||
				  (ctype[_NCmap(subj_char[0])+1] & _A) ||
				   (ctype[_NCmap(subj_char[0])+1] & _N)) 
				err(EINVTHIRD);
			    if (!xvs && (subj_char[0] < 128) && 
			    		(!(type[_NCmap(subj_char[0])] & _S)))
				err(EINVTHIRD);
			    ctype[_NCmap(subj_char[0])+1] |= _S;
			    break;
		        case 'a':
		        case 'A':
			    if ((ctype[_NCmap(subj_char[0])+1] & _C) ||
				 (ctype[_NCmap(subj_char[0])+1] & _N) ||
				  (ctype[_NCmap(subj_char[0])+1] & _P) ||
				   (ctype[_NCmap(subj_char[0])+1] & _S)) 
				err(EINVTHIRD);
			    if (!xvs && (subj_char[0] < 128) && 
			    		(!(type[_NCmap(subj_char[0])] & _S)))
				err(EINVTHIRD);
			    ctype[_NCmap(subj_char[0])+1] |= _A;
			    break;
		        case 'p':
		        case 'P':
			    if ((ctype[_NCmap(subj_char[0])+1] & _N) ||
				 (ctype[_NCmap(subj_char[0])+1] & _A) ||
				  (ctype[_NCmap(subj_char[0])+1] & _U) ||
				   (ctype[_NCmap(subj_char[0])+1] & _L))
				err(EINVTHIRD);
			    if (!xvs && (subj_char[0] < 128) && 
			    		(!(type[_NCmap(subj_char[0])] & _P)))
				err(EINVTHIRD);
			    ctype[_NCmap(subj_char[0])+1] |= _P;
			    break;
		        case 'x':
		        case 'X':
			    if (!xvs && (subj_char[0] < 128) && 
			    		(!(type[_NCmap(subj_char[0])] & _X)))
				err(EINVTHIRD);
			    ctype[_NCmap(subj_char[0])+1] |= _X;
			    break;
		        case 'n':
		        case 'N':
			    if (!xvs && (subj_char[0] < 128) && 
			    		(!(type[_NCmap(subj_char[0])] & _N)))
				err(EINVTHIRD);
			    ctype[_NCmap(subj_char[0])+1] |= _N;
			    break;
		        case 'b':
		        case 'B':
			    if (!xvs)
				err(EINVTHIRD);
			    ctype[_NCmap(subj_char[0])+1] |= _B;
			    break;
		        case 'G':
		        case 'g':
			    if (ctype[_NCmap(subj_char[0])+1] & _C) 
				err(EINVTHIRD);
			    ctype[_NCmap(subj_char[0])+1] |= _G;
			    break;
		        case '-':
			    ctype[_NCmap(subj_char[0])+1] = 0;
			    break;
		
		        default:
			    err(EINVCASE, "");
			    break;
		    }
	        } while (i = get_a_char(charptr, &xc));
	    }
            /*
             * If the second field is specified, but the third is not
             * upper case or lower case, then error.
             * If the third field is not upper or lower case, then
             * the case conversion character should be itself, not
             * a default from the C locale.
             */
            if (!(ctype[_NCmap(subj_char[0])+1] & (_U | _L)))
	    {
                if (is_alpha)
		    err(EINVTHIRD, "");
                else
		    caseconv[_NCmap(subj_char[0])+1] = subj_char[0];
	    }
            
            /*
             * Validate the combinations of character classification
             * specified.
             */
            cl = ctype[_NCmap(subj_char[0])+1];
            if ((cl & _U) & (_C | _N | _P | _S))
                err(EINVTHIRD);
            if ((cl & _L) & (_C | _N | _P | _S))
                err(EINVTHIRD);
            if ((cl & _A) & (_C | _N | _P | _S))
                err(EINVTHIRD);
            if ((cl & _N) & (_U | _L | _A | _C | _P | _S))
                err(EINVTHIRD);
            if ((cl & _S) & (_U | _L | _A | _N | _P))
                err(EINVTHIRD);
            if ((cl & _C) & (_U | _L | _A | _N | _P| _G))
                err(EINVTHIRD);
            if ((cl & _P) & (_U | _L | _A | _C | _N | _S))
                err(EINVTHIRD);

	    break;

	case FOURTH:
	    	i = get_a_char(charptr, &xc);
		if (i && fourth == 0) {
			warn(EINVFOURTH, "");
			fourth++;
		}
		break;
	default:
		err(EINVFIELD, "");
	}
}

/*-----------------------------get_string()------------------------*/
/*
 *	Parses the input string returning up to the next repeat character,
 *	translation character, or NULL.
 *
 *      This routine allocates some memory to store the newly obtained
 *      string.  It returns that address.  It also does some backslash
 *      processing.  If a backslash is encountered, it is stripped, but the
 *      next character is retained in the string.  But in that situation
 *      we increment the skip counter so that in the routine that called
 *      this one, we can do the proper positioning of the pointer for
 *      subsequent scanning of the rest of the line.
 *
 *      Note also that in this version, no mutli-byte characters are
 *      allowed....
 */  

wchar_t *
get_string(s, len, skip)
wchar_t	*s;	/* pointer to input string */
int	*len;	/* ptr to length of returned string in bytes */
int	*skip;	/* extra bytes to skip resulting from '\' */
{
	wchar_t buf[LINE];	/* buffer containing string to be returned */
	wchar_t c1;		/* next character from input string */
	wchar_t *s1;		/* pointer to characters in buf */
	wchar_t *ptr;		/* pointer to characters in allocated buffer */
	wchar_t *ptrsave;	/* returned pointer to allocated buffer */

	wchar_t num;
	*len = *skip = 0;	/* initialize return lengths */

	if (*s == '\0')		/* return null if input string is null */
		return(NULL);

	s1 = buf;		/* compose return string in buf */

	while ((c1 = *s++) != repeatc && c1 != ignc && c1!=trans && c1!='\0') {
		if (c1 == '\\') {
			if(*s >= '0' && *s <= '7' ){
				num = 0;
				while( *s >= '0' && *s <= '7') {
					num =  (num<<3)| (*s - '0');
					s++;
					(*skip)++;
				}
				if(isvalid(num) && (num < 256)) {
					*s1++ = num;		
					(*len)++;
				} else	err(EOCTAL,"");
			}else{
				c1 = *s;
				if (NCisNLchar(c1)) {
				    	*s1++ = *s++; /* copy char after '\' */
				    	(*skip)++;    /* for the slash */
					(*len)++;
				} else	err(EINVCHAR, "");
			}
		} else {
			if (NCisNLchar(c1)) {
				*s1++ = c1;
				(*len)++;	/* only one character */
			} else	err(EINVCHAR, "");
			
		}
	}

	*s1 = '\0';		/* null terminate the returned string */
	if (*len == 0)
		return(NULL);

	/* allocate buffer to contain returned string */
	ptr = (wchar_t *) malloc (sizeof(wchar_t) * (*len + 1));

	ackstrlen += (*len+1);	/* bump size for finishup */
	s1 = buf;		/* point at string to be returned */
	ptrsave = ptr;		/* save pointer to allocated buffer */
	while (*ptr++ = *s1++) 	/* copy returned string to alloc'd buffer */
		;
	return(ptrsave);	/* return allocated buffer pointer */
}

/*-----------------------------get_a_char()------------------------*/
/*
 *      A simple routine that just returns one character.  If that
 *      character was preceded by a backslash, then we discard the
 *      backslash.
 */
get_a_char(s, xc)
register wchar_t *s;	/* pointer to input string */
register wchar_t *xc;	/* pointer to buffer to contain returned character */
{
	wchar_t num;
	int	nc;

	if (*s == '\0')    /* a null field */
	{
	    *xc = '\0';
	    return(0);
	}
	if (*s == '\\')
	{
		nc = 2;
		s++;
		if(*s >= '0' && *s <= '7' )
		{
			num = 0;
			while( *s >= '0' && *s <= '7') {
				num =  (num<<3)| (*s - '0');
				nc++;
				s++;
			}
			nc--;
			if(isvalid(num) && (num < 256))
				*xc = num;		
			else
				err(EOCTAL,"");
		}
		else
			*xc = *s;
		return(nc);
	}
	else
	{
	    *xc = *s;
	    return(1);
	}
}

/*-----------------------------colldesc()--------------------------*/
/*
 *      This routine handles multi-character stings.  First thing
 *      we need to check is whether we have allocated some structures
 *      that began with the same character, if so then we compare the
 *      strings until we get a match, which is an error condition, because
 *      it means that the user has supplied the string twice, else if
 *      there was no match, then we want to insert this string into
 *      a spot that is just before the single character's spot within
 *      the array.
 *      If the string and the first character is completely new, then
 *      we allocates two structures, one for the string, and one for
 *      the first character.
 *
 */  

colldesc(string, len, unq, seq)
wchar_t *string; 
int len, unq, seq; 
{
	int negi, i;
	wchar_t *ptr;


	if ((negi = collate[_NCmap(string[0])+1]) < 0)  /* seen already */
	{
	    ptr = user_col[-negi - 1].str.strptr;
	    while (ptr != NULL && string[0] == ptr[0])
	     					/* continue search is first */
	                                        /* char is the same or we */
            {					/* find trans entry */
		if (compare(string, ptr))
		     err(EMULTC, "");
		negi--;
		if (user_col[-negi - 2].trans == 1)
		    break;
		ptr = user_col[-negi - 1].str.strptr;
	    }
	    /*
	     * We found the spot here.  So we need to copy everything
	     * from here to the end of the array over by one spot so
	     * we can insert the new one.
	     */
	    negi++;
	    for (i = (colindex - 1); i >= -negi; i--)
	    {
		user_col[i].str.strptr = user_col[i - 1].str.strptr;
		user_col[i].strlen = user_col[i - 1].strlen;
		user_col[i].rep.strptr = user_col[i - 1].rep.strptr;
		user_col[i].replen = user_col[i - 1].replen;
		user_col[i].cd_cval = user_col[i - 1].cd_cval;
		user_col[i].cd_cuniq = user_col[i - 1].cd_cuniq;
		user_col[i].trans = user_col[i - 1].trans;
	    }
	    user_col[-negi - 1].str.strptr = string;
	    user_col[-negi - 1].strlen = len;
	    user_col[-negi - 1].rep.strptr = 0;
	    user_col[-negi - 1].replen = 0;
	    user_col[-negi - 1].cd_cval = seq;
	    user_col[-negi - 1].cd_cuniq = unq;
	    user_col[-negi - 1].trans = 0;
	    colindex++;	/* indicate one more user-defined entry */
	    /*
	     * Have to adjust the collate values as well, since we
	     * moved them down the array.
	     */
	    if (negi == collate[_NCmap(string[0])+1])
		negi--;

	    for (i = 0; i < 256; i++)
	    {
		if (isvalid(i))
		{
			if (collate[_NCmap(i)+1] <= negi) {
				(collate[_NCmap(i)+1])--;
				(coluniq[_NCmap(i)+1])--;
			}
		}
	    }
	}
	else
	    if (negi >= 0)
	/*
	 * If negi == 0 then new string, so get two structures and assign
	 * the sequence to the multi-char string.  We don't yet know what the
	 * sequence will be for the first character, that will be
	 * taken care of by setcollate() when we see that single
	 * character by itself in the input file.  If we never see
	 * that single character by itself, then the routine check_coldesc()
	 * will pick that up as an error condition.
	 * If negi was greater than 0, then we saw the single character
	 * first and we need to get another structure for the multi-char
	 * string.
	 */
	    {
		int tmp, tmp1;

		if (negi > 0) {
		    tmp = collate[_NCmap(string[0])+1];
		    tmp1 = coluniq[_NCmap(string[0])+1];
		}
		collate[_NCmap(string[0])+1] = -colindex;
		user_col[colindex - 1].str.strptr = string;
		user_col[colindex - 1].strlen = len;
		user_col[colindex - 1].rep.strptr = 0;
		user_col[colindex - 1].replen = 0;
		user_col[colindex - 1].cd_cval = seq;
		user_col[colindex - 1].cd_cuniq = unq;
		user_col[colindex - 1].trans = 0;
		colindex++;
		ptr = (wchar_t *)malloc(sizeof(wchar_t) * 2);
		ackstrlen += 2;
		user_col[colindex - 1].str.strptr = ptr;
		*ptr++ = string[0];
		*ptr = '\0';
		user_col[colindex - 1].strlen = 1;
		user_col[colindex - 1].rep.strptr = 0;
		user_col[colindex - 1].replen = 0;
		if (negi > 0) {
		    user_col[colindex - 1].cd_cval = tmp;
		    user_col[colindex - 1].cd_cuniq = tmp1;
		} else {
		    user_col[colindex - 1].cd_cval = 0;
		    user_col[colindex - 1].cd_cuniq = 0;
		}
		user_col[colindex - 1].trans = 0;
		colindex++;
	    }
}

/*-----------------------------check_coldesc()---------------------*/
/*
 *      This routine is called when the user file has been completely
 *      processed.  Its duty is to check that for multiple character
 *      strings supplied by the user, the first character of that string
 *      is also provided in the file, so there will be a default collating
 *      sequence for that character as well as collating sequence for the
 *      multi-character string.
 *
 */  

check_coldesc()
{
	int i, l;
	unsigned char c[10];

/*
 *      Loop through the array of structures that stores the multi-character
 *      string and their first character.  At this time, the single character
 *      is still stored in memory and being pointed to, this is just a
 *      convenience so that we may use the str.strptr field of the union
 *      to test for the end of the array.  In the routine where this structure
 *      is written out to file, the str.strptr field(cd_stroff in the final
 *      structure) for the single character will be zeroed, to indicate
 *      first character of the sequence.
 */
	i = 0;
	while (user_col[i].str.strptr != NULL)
	{
	    if (user_col[i].trans == 0 && user_col[i].cd_cval == 0)
	    {
#ifndef _BLD
		l = NCencstr(user_col[i].str.strptr, c, 2);
#endif
		err(ECOLLATE, c);
	    }
	    ++i;
	}
}


/*-----------------------------compare()---------------------------*/
/*
 *      Simply compares two NLchar strings for absolute equality.
 */
compare(s1, s2)
wchar_t *s1, *s2;
{
	while (*s1)
	{
	    if (*s1 == *s2)
		s1++, s2++;
	    else
		return(0);
	}
	if (*s2 == (int)NULL)
	    return(1);
	else return(0);
}

/*-----------------------------set_trans()-------------------------*/
/*
 *      Equate the subject character to the string to be translated to.
 *      So we allocate one coldesc structure and fill in the necessary
 *      information.
 */
set_trans(from, to, flen, len)
wchar_t *from, *to;
int flen, len;
{
	wchar_t *tmp;
	int i;
	int elem = 0;


	tmp = to;
	while (*tmp)                    /* if any of the character to be */
	{                               /* translated to is the same as the */
	    if (*from == *tmp++)        /* subject character, then infinite */
		err(EINVTRANS, "");             /* loop will result */
	}
	for (i = 0; i < (colindex - 1); i++) {
	    if (user_col[i].trans != 1)
	    {
		if (compare(user_col[i].str.strptr, to))
		    elem = 1;
		continue;
	    }
	    tmp = user_col[i].rep.strptr;
	    while (*tmp)		/* verify that source char is not */
	    {				/* target for another translation */ 
		if (*from == *tmp++)
		    err(EINVTRANS, "");
            }
	    tmp = to;
	    while (*tmp && !elem)	/* verify that target is not the */
	    {   			/* source of another translation */
		if (*user_col[i].str.strptr == *tmp++)
		    err(EINVTRANS, "");
            }
        }
	if ((i = collate[_NCmap(from[0]) +1]) > 0)
	    err(EMULTC, "");
        if (i < 0) 
	{
	    i = -i -1;
	    tmp = user_col[i].str.strptr;
	    while (from[0] == tmp[0])
	    {
		if (!compare(from, tmp))
	        	tmp = user_col[++i].str.strptr;
		else	
		{
		    if (user_col[i].trans == 0 && user_col[i].cd_cval != 0)
			err(EMULTC, "");
		    else
		    {
			coluniq[_NCmap(from[0])+1] = unique;
			user_col[i].str.strptr = from;
			user_col[i].rep.strptr = to;
			user_col[i].strlen = flen;
			user_col[i].replen = len;
			user_col[i].cd_cval = 0;
			user_col[i].cd_cuniq = unique++;
			user_col[i].trans = 1;
			return;
		    }
		}
            }
        }

	coluniq[_NCmap(from[0])+1] = unique;
	collate[_NCmap(from[0])+1] = -colindex;
	user_col[colindex - 1].str.strptr = from;
	user_col[colindex - 1].rep.strptr = to;
	user_col[colindex - 1].strlen = flen;
	user_col[colindex - 1].replen = len;
	user_col[colindex - 1].cd_cval = 0;
	user_col[colindex - 1].cd_cuniq = unique++;
	user_col[colindex - 1].trans = 1;
	colindex++;
}

/*-----------------------------setcollate()------------------------*/
/*
 *      This routine sets the unique as well as the common collate
 *      sequence for a single character. (multi-character string is
 *      handled by colldesc().
 *
 */  

setcollate(character, unq, seq)
wchar_t *character;
int unq;
int seq;
{
	int i1;

	coluniq[_NCmap(character[0])+1] = unq;
	/*
	 * If the character was the start of a multi-character string
	 * then its collate value in NLloc would have been set to a negative
	 * number, if so, then we want to find it in the array of
	 * coldesc structures.  By convention, it will always be the
	 * last one of the series of strings that started with this character.
	 */
	if ((i1 = collate[_NCmap(character[0])+1]) < 0)
	{
	    while (character[0] == user_col[-i1 - 1].str.strptr[0])
		i1--;
	    i1++;
	    if (user_col[-i1 - 1].trans == 1)
		err(EMULTC, "");
	    user_col[-i1 - 1].cd_cval = seq;
	    user_col[-i1 - 1].cd_cuniq = coluniq[_NCmap(character[0])+1];
	}
	else
	/*
	 * If we haven't seen it before, then set the collate value.
	 */
	    if (i1 == 0)
	    {
		collate[_NCmap(character[0])+1] = seq;
	    }
	    else
		err(EMULTC, "");        /* same char entered multiple times */
}

/*-----------------------------options()---------------------------*/
/*
 *      This routine process the option line.
 */
options(s)
wchar_t *s;
{
	wchar_t token[LINE], *cptr;
	char c[LINE];
	int length, skip;

	while(cptr = nexttok(s, token, &length, &skip))
	{
	    s += skip;
	    switch(convert(cptr))
	    {
		case SEP:
		    if ((cptr = nexttok(s, token, &length, &skip))== NULL)
			err(ESEPOPT, "");
		    s += skip;
		    if (cptr[1] != '\0')
		    {
#ifndef _BLD
			length = NCencstr(cptr, c, length+1);
#endif
			err(EONEOPT, c);
		    }
		    sep = cptr[0];
		    break;
		case TRANS:
		    if ((cptr = nexttok(s, token, &length, &skip))== NULL)
			err(ETRANSOPT, "");
		    s += skip;
		    if (cptr[1] != '\0')
		    {
#ifndef _BLD
			length = NCencstr(cptr, c, length+1);
#endif
			err(EONEOPT, c);
		    }
		    trans = cptr[0];
		    break;
		case REPEAT:
		    if ((cptr = nexttok(s, token, &length, &skip))== NULL)
			err(EREPOPT, "");
		    s += skip;
		    if (cptr[1] != '\0')
		    {
#ifndef _BLD
			length = NCencstr(cptr, c, length+1);
#endif
			err(EONEOPT, c);
		    }
		    repeatc = cptr[0];
		    break;
		case COMMENT:
		    if ((cptr = nexttok(s, token, &length, &skip))== NULL)
			err(ECOMOPT, "");
		    s += skip;
		    if (cptr[1] != '\0')
		    {
#ifndef _BLD
			length = NCencstr(cptr, c, length+1);
#endif
			err(EONEOPT, c);
		    }
		    comment = cptr[0];
		    break;
		case IGNORE:
		    if ((cptr = nexttok(s, token, &length, &skip))== NULL)
			err(EBLOPT, "");
		    s += skip;
		    if (cptr[1] != '\0')
		    {
#ifndef _BLD
			length = NCencstr(cptr, c, length+1);
#endif
			err(EONEOPT, c);
		    }
		    ignc = cptr[0];
		    break;
		case XVS:
		    xvs++;
		    break;
		default:
#ifndef _BLD
		    length = NCencstr(cptr, c, length+1);
#endif
		    err(EOPTION, c);
	    }
	}
}

/*-----------------------------convert()---------------------------*/
/*
 *      This routine converts the input wchar_t string into a number
 *      that the option routine will understand.
 *
 */  

convert(string) 
wchar_t *string; 
{
	switch(string[0])
	{
	    case 's':
		if (cmpstr("sep", string, 3))
		    return(SEP);
		else
		    return(0);
	    case 't':
		if (cmpstr("trans", string, 5))
		    return(TRANS);
		else
		    return(0);
	    case 'r':
		if (cmpstr("repeat", string, 6))
		    return(REPEAT);
		else
		    return(0);
	    case 'c':
		if (cmpstr("comment", string, 7))
		    return(COMMENT);
		else
		    return(0);
	    case 'i':
		if (cmpstr("ignore", string, 6))
		    return(IGNORE);
	    case 'N':
		if (cmpstr("NON-ASCII", string, 9))
		    return(XVS);
	    default:
		return(0);
	}
}

/*-----------------------------nexttok()---------------------------*/
/*
 *      This routine simply returns the next token.
 */
wchar_t *
nexttok(s, token, length, skip)
wchar_t *s, *token;
int *length, *skip;
{
	wchar_t c, *s1, *s2, *tmp;
	int gotone;

	*length = *skip = 0;
	gotone = 0;
	s1 = s;
	tmp = s2 = token;
	while (*s == ' ' || *s == '\t')    /* skip over white spaces */
	    ++s;
	while ((c = *token++ = *s++) != ' ' && c!='\t' && c!='\n' && c!='\0')
	    ++gotone;
	*(--token) = '\0';
	*skip = gotone ? (s - s1) : 0;
	while (*tmp++)
	    (*length)++;
	return (*length ? s2 : NULL);
}


/*-----------------------------_BLD -------------------------------*/
/* The following routines are included here for the build environment */

#ifdef _BLD
NCisNLchar(c)
int c;
{
#ifdef KJI
	if ( (unsigned int) c < 0x80)
		return (1);
	if ( (_jctype1_[_jctype0_[(unsigned short)(c) >> 8]] [(c) & 0xff]) != 0)
		return (1);
	else	return (0);
#else
	return ( (unsigned int) c < 256 );
#endif
}

NCisshift(c)
int c;
{
#ifdef KJI
	return( (_jctype0_[(c) & 0xff]) > 1 );
#else 
	return( 0 );
#endif
}
#endif	/* end of ifdef _BLD */


/*-----------------------------err()-------------------------------*/
/*
 * Prints message to stderr.  The message is made up of two parts.  The
 * first is a string determined by message number err_no.  The second is 
 * a string determined by pointer s.  s may not refer to something which 
 * should be translated.
 */
err(err_no, s)
int err_no;
char *s;
{

#ifdef MSG
	nl_catd catd;
	char format[NL_TEXTMAX];

	/* Attempt to open catalog and get message.  If either fails, then
	 * compose default message.
	 */

	if (linenum == -1) {
		if (((catd = catopen (MF_CTAB, 0)) == (nl_catd) -1) ||
	    	   (catgetmsg(catd,MS_CTAB,M_POST,format,NL_TEXTMAX))!=format)
		     	  strcpy(format, "Error in post checking: %s %s\n");
		   (void)fprintf(stderr, format, 
			  catgets (catd, MS_CTAB, err_no, msgtab[err_no]), s);
	} else {
		if (((catd = catopen (MF_CTAB, 0)) == (nl_catd) -1) ||
	           (catgetmsg(catd,MS_CTAB,M_OTHER,format,NL_TEXTMAX))!=format) 
	    		  strcpy (format, "Error on line %d: %s %s\n");
		   (void)fprintf(stderr, format, linenum,
			  catgets (catd, MS_CTAB, err_no, msgtab[err_no]), s);
	}
#else
	if (linenum == -1)
		(void)fprintf(stderr, "Error in post checking: %s %s\n", 
			msgtab[err_no], s);
	else	(void)fprintf(stderr, "Error on line %d: %s %s\n", linenum,
			msgtab[err_no], s);
#endif
	exit(1);
}
/*-----------------------------warn()-------------------------------*/
/*
 * Prints message to stderr.  The message is made up of two parts.  The
 * first is a string determined by message number err_no.  The second is 
 * a string determined by pointer s.  s may not refer to something which 
 * should be translated.
 */
warn(err_no, s)
int err_no;
char *s;
{
#ifdef MSG
	nl_catd catd;
	char format[NL_TEXTMAX];

	/* Attempt to open catalog and get message.  If either fails, then
	 * compose default message.
	 */
	if (((catd = catopen (MF_CTAB, 0)) == (nl_catd) -1) ||
	    (catgetmsg (catd, MS_CTAB, M_OTHER, format, NL_TEXTMAX)) != format)
	    strcpy (format, "Error on line %d: %s %s\n");

	(void)fprintf(stderr, format, linenum,
			catgets (catd, MS_CTAB, err_no, msgtab[err_no]), s);
#else
	(void)fprintf(stderr, "Error on line %d: %s %s\n", linenum,
			msgtab[err_no], s);
#endif
	return;
}

#ifdef _BLD
/*
 * NAME: NCstrlen
 *
 * FUNCTION: Counts the number of NLchars in the string pointed to by s before
 *      the terminating null character.  The string must be of type NLchar.
 *
 * RETURN VALUE DESCRIPTION: An integer, the number of NLchars in s before the
 *      terminating null character.
 */
int
#ifdef _NO_PROTO
NCstrlen(s)
register NLchar *s;
#else /* _NO_PROTO */
NCstrlen(register NLchar *s)
#endif /* _NO_PROTO */
{
        register NLchar *s0 = s + 1;

        while (*s++ != 0)
                ;
        return (s - s0);
}
#endif /* _BLD */


#ifdef NCdec
#undef NCdec
#endif
int
#ifdef _NO_PROTO
NCdec(c, x)
unsigned char *c;
wchar_t *x;
#else
NCdec(unsigned char *c, wchar_t *x)
#endif
{
        if (NCisshift(c[0])) {                          /* multi-byte? */
                 *x = ( (c[0] << 8) | (c[1] & 0xff) );  /* yes, collect */
                 return (2);                            /* and return */
        } else { *x = c[0];
                 return (1);
        }
}


/*
 * Convert a string of chars to wchar_ts; return length of string produced.
 */

int
NCdecstr(c, nlc, len)
unsigned char *c;       /* char string */
wchar_t *nlc;   /* wchar_t string */
int len;       /* the length of char */
{
        unsigned char lc;      /* temporary storage for c */
        wchar_t *onlc = nlc;    /* the next wchar_t position */

        /*  Always NUL-terminate output string, if any; but never count
         *  NUL as part of length.
         */
        while (0 <= --len) {
                lc = *c;
                c += NCdec(c, nlc);  /* decode char to wchar_t */
                if (!lc)
                        break;
                ++nlc;
        }
        if (onlc < nlc && len < 0)
                *--nlc = 0;
        return (nlc - onlc);
}

static unsigned long	/* XXX "long" (and &xff's) needed for optimality */
swapshort(w)
	register unsigned long w;
{
	if (!swapflag) return w;
	return ((w << 8) | ((w >> 8) & 0xff)) & 0xffff;
}

static unsigned long
swaplong(l)
	register unsigned long l;
{
	if (!swapflag) return l;
	return (l << 24) | (l >> 24) | ((l & 0xff00) << 8) | ((l >> 8) & 0xff00);
}
