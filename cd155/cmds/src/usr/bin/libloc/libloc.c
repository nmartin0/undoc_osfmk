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
static char rcsid[] = "@(#)$RCSfile: libloc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:29:13 $";
#endif
/*
 * LIBCLOC: libloc command
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */                                                                   

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#ifndef _BLD
#include <locale.h>
#else
#ifndef _WCHAR_T
#define _WCHAR_T
typedef unsigned short wchar_t;
#endif /* _WCHAR_T */
#endif /* _BLD */
#include <sys/localedef.h>
#ifndef _BLD
#include <sys/limits.h>
#else
#ifndef PATH_MAX
#define PATH_MAX        1024
#endif
#ifndef CHAR_MAX
#define CHAR_MAX        127
#endif
#endif /* _BLD */

#define NUMELEM 10			/* # of bytes written out each line */

#define	strlen_t(a)	(a) ? strlen(a) : -1

/* libloc is a utility which reads a collating file and environment file for a
 * set of locales and outputs C source code which mirrors the information
 * in these files.  This C source code is then compiled and linked
 * into libc.  It initializes the list of known environments used by setlocale.
 * 
 * The syntax of libloc is:
 *    libloc [-r | -w] localename ctabfile enfile [localename ctabfile enfile ...]
 * 
 * and the C source is written to stdout.  Error messages are written
 * to stderr.  The -r flag indicates that the read-only csect directive is to
 * be inserted into the assembler source file.  The -w flag indicates the a 
 * read-write csect directive is to be inserted into the assembler source
 * file.  (no longer supported)
 * 
 * The shared library work was motivated by the requirement to resolve
 * certain performance problems in setlocale() and other functions that read
 * and write the locale specific information.
 * 
 * Basically, the old version of setlocale() was required to open and read
 * files which resulted in slower than desired operation.  The solution that
 * is being implemented is to put the locale information in a read-only text
 * segment of the shared library.  The plan is to make available all
 * supported locales in the shared library.
 */

loc_t 	*loc_tptr;
int	get_en_info();
static void reloc_ctab();
static void reloc_ctype();
static char *get_line();
static unsigned long swapshort();
static unsigned long swaplong();
char	*localename;		/* from arg list */
int	swapflag;
char	prevlocalename[PATH_MAX] = "NULL";

main(argc, argv)
int argc;
char *argv[];
{
	char	*ctabfile;		/* from arg list */
	char	*enfile;		/* from arg list */
	int	csectflg = 0;		/* 1==>read-only 2==>read-write */

	csectflg = 0;	/* indicate no csect */
	swapflag = 0;	/* indicate no swapping */
	while (argc > 1) {
		if (strcmp(argv[1], "-r") == 0) {
			if (csectflg != 0) {
				fprintf(stderr,
				      "libloc: -r or -w already specified\n");
				exit(1);
			}
			csectflg = 1;	/* indicate read-only csect */
			argc--;
			argv++;
			continue;
		}
		if (strcmp(argv[1], "-w") == 0) {
			if (csectflg != 0) {
				fprintf(stderr,
				      "libloc: -r or -w already specified\n");
				exit(1);
			}
			csectflg = 2;	/* indicate read-write csect */
			argc--;
			argv++;
			continue;
		}
		if (strcmp(argv[1], "-s") == 0) {
			swapflag = 1;
			argc--;
			argv++;
			continue;
		}
		break;
	}

	if ((argc % 3) != 1) {
	   fprintf(stderr,
		   "usage:libloc [-s] [-r|-w] localename ctabfile enfile ...\n");
	   exit(1);
	}

	printf("#include <sys/types.h>\n");
	printf("#include <sys/localedef.h>\n");
	printf("#include \"setlocale.h\"\n");

	while (argc >= 4) {

		localename = argv[1];
		ctabfile = argv[2];
		enfile = argv[3];
		argv += 3;
		argc -= 3;

		do_locale(localename, ctabfile, enfile, argc == 1);
	}

	exit(0);
}	/* end of program */


do_locale(localename, ctabfile, enfile, last_pass)

char *localename;
char *ctabfile;
char *enfile;
int last_pass;
{
	struct 	stat sbuf;
	int 	fsize;
	loc_t	*locptr;
	col_t	*colptr;
	ctype_t	*ctypeptr;
	mon_t	*monptr;
	num_t	*numptr;
	tim_t	*timptr;
	msg_t	*msgptr;
	map_t	*mapptr;
	loc_t	NLloc;

	locptr = &NLloc;

	bzero((char *)locptr, sizeof(loc_t));

	if (stat(ctabfile, &sbuf) == -1) { 
		fprintf(stderr, "libloc: no such file %s\n", ctabfile);
		exit(1);
	}
	fsize = sbuf.st_size;


	if (get_ctab_info(ctabfile, fsize ) == -1){
		fprintf(stderr, "libloc: unable to read ctab data\n");
		exit(1);
	}
	locptr->lc_mag0   	= loc_tptr->lc_mag0;
	locptr->lc_mag1   	= loc_tptr->lc_mag1;
	locptr->lc_version   	= swapshort(loc_tptr->lc_version);
	locptr->lc_code_type   	= swapshort(loc_tptr->lc_code_type);
	locptr->lc_length  	= swapshort(loc_tptr->lc_length);
	locptr->lc_coltbl 	= loc_tptr->lc_coltbl;
	locptr->lc_chrtbl 	= loc_tptr->lc_chrtbl;

	if (stat(enfile, &sbuf) == -1) { 
		fprintf(stderr, "libloc: no such file %s\n", enfile);
		exit(1);
	}
	fsize = sbuf.st_size;

	if (get_en_info(locptr, localename, enfile, fsize) == -1) {
		fprintf(stderr, "libloc: unable to read .en data\n");
		exit(1);
	}

	/* The rest of the this program is devoted to writing the contents
	 * of the locale.  The contents of localedef.h are provided here in
	 * the form of comments.  Each struc is treated separately.  After
	 * the comment, the struct is written out.
	 *
	 * Notes:
	 *
	 * Any struct member which is a pointer is either set to NULL
	 * or is initialized to the value of the offset from the start of 
	 * the loc_t struct.
	 *
	 * A .align is added in front of every struct explicitly to align the
	 * structure to start at a 4-byte boundary.  In addition, each call
	 * to pbyte writes a .align.
	 *
	 * Be careful to ensure that what is written agrees with the current
	 * contents of localedef.h.  Be sure to update the comments in this
	 * module and what gets written.
	 */

/* Assume these have already been included */

#ifdef COMMENT
typedef struct collation_table {                /* LC_COLLATE */
	short    lc_version;                    /* 1 for now */
	short	 lc_length;			/* length of this table */
	char     *lc_locale_name;		/* pointer to locale name */
	int      len_collate;
	short    *lc_collate;                   /* ptr to coll tbl */
	int      len_coluniq;
	short  *lc_coluniq;                   /* ptr to 2nd wt tbl */
	int      len_coldesc;
	coldesc_t *lc_coldesc;                  /* ptr to coldesc */
	int	 len_strings;
	wchar_t	 *lc_strings;			/* coldesc strings */
	int	high_cvalue;
} col_t;
#endif

	colptr = locptr->lc_coltbl;

	printf("static const char %s_locname[] = \"%s\";\n\n", localename, localename);

	pshort("collate", colptr->lc_collate, colptr->len_collate);
	pshort("coluniq", colptr->lc_coluniq, colptr->len_coluniq);
	pcoldesc("coldesc", colptr->lc_coldesc, colptr->len_coldesc);
	pwchar("strings", colptr->lc_strings, colptr->len_strings);

	printf("static const col_t %s_col_t = {\n", localename);
	printf("\t(short)%d,\n", swapshort(colptr->lc_version));
	printf("\t(short)%d,\n", swapshort(colptr->lc_length));
	printf("\t(char *)%s_locname,\n", localename);
	printf("\t(int)%d,\n", swaplong(colptr->len_collate));
	printf("\t(short *)%s_collate,\n", localename);
	printf("\t(int)%d,\n", swaplong(colptr->len_coluniq));
	printf("\t(short *)%s_coluniq,\n", localename);
	printf("\t(int)%d,\n", swaplong(colptr->len_coldesc));
	printf("\t(coldesc_t *)%s_coldesc,\n", localename);
	printf("\t(long)%d,\n", swaplong(colptr->len_strings));
	printf("\t(wchar_t *)%s_strings,\n", localename);
	printf("\t(long)%d\n", swaplong(colptr->high_cvalue));
	printf("};\n\n");

#ifdef COMMENT
typedef struct char_classification_table {      /* LC_CTYPE */
	short      lc_version; 		/* version 1 */
	short	   lc_length;  		/* length of this table */
	short      lc_code_type; 	/* 0 for now */
	short      mb_cur_max;   	/* 2 for now */
	short      mb_cur_min;   	/* 1 for now */
	short      lc_dsp_width;                  
	char       *lc_locale_name; 	/* pointer to locale name */
	int        len_caseconv;   	/* table length */
	wchar_t    *lc_caseconv;   	/* ptr to tbl */
	int        len_ctype;   	/* table length */
	unsigned short *lc_ctype;   	/* old table */
} ctype_t;
#endif

	ctypeptr = locptr->lc_chrtbl;

	pwchar("caseconv", ctypeptr->lc_caseconv, ctypeptr->len_caseconv);
	pushort("ctype", ctypeptr->lc_ctype, ctypeptr->len_ctype);

	printf("static const ctype_t %s_ctype_t = {\n", localename);
	printf("\t(short)%d,\n", swapshort(ctypeptr->lc_version));
	printf("\t(short)%d,\n", swapshort(ctypeptr->lc_length));
	printf("\t(short)%d,\n", swapshort(ctypeptr->lc_code_type));
	printf("\t(short)%d,\n", swapshort(ctypeptr->mb_cur_max));
	printf("\t(short)%d,\n", swapshort(ctypeptr->mb_cur_min));
	printf("\t(short)%d,\n", swapshort(ctypeptr->lc_dsp_width));
	printf("\t(char *)%s_locname,\n", localename);
	printf("\t(int)%d,\n", swaplong(ctypeptr->len_caseconv));
	printf("\t(wchar_t *)%s_caseconv,\n", localename);
	printf("\t(long)%d,\n", swaplong(ctypeptr->len_ctype));
	printf("\t(unsigned short *)%s_ctype\n", localename);
	printf("};\n\n");

#ifdef COMMENT
typedef	struct lc_monetary_table {
	short  	lc_version;
	short 	lc_length;		/* length of this table */
	char   	*lc_locale_name;	/* pointer to locale name */
	char 	*int_curr_symbol;	/* international currency symbol*/
	char 	*currency_symbol;	/* national currency symbol	*/
	char 	*mon_decimal_point;	/* currency decimal point	*/
	char 	*mon_thousands_sep;	/* currency thousands separator*/
	char 	*mon_grouping;		/* currency digits grouping	*/
	char 	*positive_sign;		/* currency plus sign		*/
	char 	*negative_sign;		/* currency minus sign		*/
	char 	int_frac_digits;	/* internat currency fract digits*/
	char 	frac_digits;		/* currency fractional digits	*/
	char 	p_cs_precedes;		/* currency plus location	*/
	char 	p_sep_by_space;		/* currency plus space ind.	*/
	char 	n_cs_precedes;		/* currency minus location	*/
	char 	n_sep_by_space;		/* currency minus space ind.	*/
	char 	p_sign_posn;		/* currency plus position	*/
	char 	n_sign_posn;		/* currency minus position	*/
} mon_t;
#endif

	monptr = locptr->lc_montbl;

	pbyte("mon_int_curr_symbol", monptr->int_curr_symbol,
			strlen_t(monptr->int_curr_symbol) + 1);
	pbyte("mon_currency_symbol", monptr->currency_symbol,
			strlen_t(monptr->currency_symbol) + 1);
	pbyte("mon_decimal_point", monptr->mon_decimal_point,
			strlen_t(monptr->mon_decimal_point) + 1);
	pbyte("mon_thousands_sep", monptr->mon_thousands_sep,
			strlen_t(monptr->mon_thousands_sep) + 1);
	pbyte("mon_grouping", monptr->mon_grouping,
			strlen_t(monptr->mon_grouping) + 1);
	pbyte("mon_positive_sign", monptr->positive_sign,
			strlen_t(monptr->positive_sign) + 1);
	pbyte("mon_negative_sign", monptr->negative_sign,
			strlen_t(monptr->negative_sign) + 1);

	printf("static const mon_t %s_mon_t = {\n", localename);
	printf("\t(short)%d,\n", monptr->lc_version);
	printf("\t(short)%d,\n", monptr->lc_length);
	printf("\t(char *)%s_locname,\n", localename);
	printf("\t(char *)%s_mon_int_curr_symbol,\n", localename);
	printf("\t(char *)%s_mon_currency_symbol,\n", localename);
	printf("\t(char *)%s_mon_decimal_point,\n", localename);
	printf("\t(char *)%s_mon_thousands_sep,\n", localename);
	printf("\t(char *)%s_mon_grouping,\n", localename);
	printf("\t(char *)%s_mon_positive_sign,\n", localename);
	printf("\t(char *)%s_mon_negative_sign,\n", localename);
	printf("\t0x%02x,\n", monptr->int_frac_digits);
	printf("\t0x%02x,\n", monptr->frac_digits);
	printf("\t0x%02x,\n", monptr->p_cs_precedes);
	printf("\t0x%02x,\n", monptr->p_sep_by_space);
	printf("\t0x%02x,\n", monptr->n_cs_precedes);
	printf("\t0x%02x,\n", monptr->n_sep_by_space);
	printf("\t0x%02x,\n", monptr->p_sign_posn);
	printf("\t0x%02x\n", monptr->n_sign_posn);
	printf("};\n\n");

#ifdef COMMENT
typedef struct numeric_table {                  /* LC_NUMERIC */
	short	lc_version;
	short	lc_length;		/* length of this table */
	char    *lc_locale_name;	/* pointer to locale name */
	char 	*decimal_point;
	char 	*thousands_sep;
	char	*grouping;
} num_t;
#endif

	numptr = locptr->lc_numtbl;

	pbyte("num_decimal_point", numptr->decimal_point,
			strlen_t(numptr->decimal_point) + 1);
	pbyte("num_thousands_sep", numptr->thousands_sep,
			strlen_t(numptr->thousands_sep) + 1);
	pbyte("num_grouping", numptr->grouping,
			strlen_t(numptr->grouping) + 1);

	printf("static const num_t %s_num_t = {\n", localename);
	printf("\t(short)%d,\n", numptr->lc_version);
	printf("\t(short)%d,\n", numptr->lc_length);
	printf("\t(char *)%s_locname,\n", localename);
	printf("\t(char *)%s_num_decimal_point,\n", localename);
	printf("\t(char *)%s_num_thousands_sep,\n", localename);
	printf("\t(char *)%s_num_grouping\n", localename);
	printf("};\n\n");

#ifdef COMMENT
typedef struct lc_time_table{
	short   lc_version;
	short	lc_length;	 /* length of this table */
	char    *lc_locale_name; /* pointer to locale name */
	char    *t_fmt;         /* NLTIME; date %X descriptor */
	char    *d_fmt;         /* NLDATE; date %x descriptor */
	char    *nlldate;       /* NLLDATE  long form         */
	char    *d_t_fmt;       /* NLDATIM, date %c descriptor */
	char    *abday;         /* NLSDAY; date %a descriptor */
	char    *day;           /* NLLDAY; date %A descriptor */
	char    *abmon;         /* NLSMONTH; date %b descriptor */
	char    *mon;           /* NLLMONTH; date %B descriptor */
	char    *misc;          /* NLTMISC  at;each;every;on;through  */
	char    *tstrs;         /* NLTSTRS */
	char    *tunits;        /* NLTUNITS */
	char	*year;		/* Name of the year and the starting time */
	char    *am_pm;         /* am and pm */
} tim_t;
#endif

	timptr = locptr->lc_timtbl;

	pbyte("t_fmt", timptr->t_fmt, strlen_t(timptr->t_fmt) + 1);
	pbyte("d_fmt", timptr->d_fmt, strlen_t(timptr->d_fmt) + 1);
	pbyte("nlldate", timptr->nlldate, strlen_t(timptr->nlldate) + 1);
	pbyte("d_t_fmt", timptr->d_t_fmt, strlen_t(timptr->d_t_fmt) + 1);
	pbyte("abday", timptr->abday, strlen_t(timptr->abday) + 1);
	pbyte("day", timptr->day, strlen_t(timptr->day) + 1);
	pbyte("abmon", timptr->abmon, strlen_t(timptr->abmon) + 1);
	pbyte("mon", timptr->mon, strlen_t(timptr->mon) + 1);
	pbyte("misc", timptr->misc, strlen_t(timptr->misc) + 1);
	pbyte("tstrs", timptr->tstrs, strlen_t(timptr->tstrs) + 1);
	pbyte("tunits", timptr->tunits, strlen_t(timptr->tunits) + 1);
	pbyte("year", timptr->year, strlen_t(timptr->year) + 1);
	pbyte("am_pm", timptr->am_pm, strlen_t(timptr->am_pm) + 1);

	printf("static const tim_t %s_tim_t = {\n", localename);
	printf("\t(short)%d,\n", timptr->lc_version);
	printf("\t(short)%d,\n", timptr->lc_length);
	printf("\t(char *)%s_locname,\n", localename);
	printf("\t(char *)%s_t_fmt,\n", localename);
	printf("\t(char *)%s_d_fmt,\n", localename);
	printf("\t(char *)%s_nlldate,\n", localename);
	printf("\t(char *)%s_d_t_fmt,\n", localename);
	printf("\t(char *)%s_abday,\n", localename);
	printf("\t(char *)%s_day,\n", localename);
	printf("\t(char *)%s_abmon,\n", localename);
	printf("\t(char *)%s_mon,\n", localename);
	printf("\t(char *)%s_misc,\n", localename);
	printf("\t(char *)%s_tstrs,\n", localename);
	printf("\t(char *)%s_tunits,\n", localename);
	printf("\t(char *)%s_year,\n", localename);
	printf("\t(char *)%s_am_pm\n", localename);
	printf("};\n\n");

#ifdef COMMENT
typedef struct lc_messages_table {
	short	lc_version;
	short	lc_length;		/* length of this table */
	char    *lc_locale_name;	/* pointer to locale name */
	char 	*messages;		/* Message Catalog name */
	char 	*yes_string;		/* Response string for affirmation */
	char 	*no_string;		/* Response string for negation */
} msg_t;
#endif

	msgptr = locptr->lc_msgtbl;

	pbyte("messages", msgptr->messages, strlen_t(msgptr->messages) + 1);
	pbyte("yes_string", msgptr->yes_string,
			strlen_t(msgptr->yes_string) + 1);
	pbyte("no_string", msgptr->no_string, strlen_t(msgptr->no_string) + 1);

	printf("static const msg_t %s_msg_t = {\n", localename);
	printf("\t(short)%d,\n", msgptr->lc_version);
	printf("\t(short)%d,\n", msgptr->lc_length);
	printf("\t(char *)%s_locname,\n", localename);
	printf("\t(char *)%s_messages,\n", localename);
	printf("\t(char *)%s_yes_string,\n", localename);
	printf("\t(char *)%s_no_string\n", localename);
	printf("};\n\n");

#ifdef COMMENT
typedef struct wchar_mapping_table {            /* used for wchar_t map */
	short    lc_version;
	short	 lc_length;			/* length of this table */
	char     *lc_identifier;
} map_t;
#endif

	mapptr = locptr->lc_maptbl;

	printf("static const char %s_msg_identifier[] = \"\";\n\n", localename);

	printf("static const map_t %s_map_t = {\n", localename);
	printf("\t(short)%d,\n", mapptr->lc_version);
	printf("\t(short)%d,\n", mapptr->lc_length);
	printf("\t(char *)%s_msg_identifier\n", localename);
	printf("};\n\n");

#ifdef COMMENT
typedef struct localeinfo_table {           
	char     lc_mag0, lc_mag1;      /* magic numbers... */
	short    lc_version;            /* identifier */
	short    lc_code_type;                 
	short	 lc_length;		/* length of this table */
	col_t    *lc_coltbl;		/* LC_COLLATE */
	ctype_t  *lc_chrtbl;		/* LC_CTYPE */
	mon_t    *lc_montbl;		/* LC_MONETARY */
	num_t    *lc_numtbl;		/* LC_NUMERIC */
	tim_t    *lc_timtbl;		/* LC_TIME */
	msg_t    *lc_msgtbl;		/* LC_MESSAGES */
  	map_t    *lc_maptbl;		/* For code page work later */
} loc_t;
#endif

	printf("static const env_t %s_env = {\n", localename);
	printf("\t\"%s\",\n", localename);
	printf("\t\"%s\",\n", localename);
	printf("\tNULL,\n");
	printf("\t(env_t *)%s,\n", prevlocalename);
	printf("\t{\n");
	printf("\t\t%d,\n\t\t%d,\n", locptr->lc_mag0, locptr->lc_mag1);
	printf("\t\t(short)%d,\n", locptr->lc_version);
	printf("\t\t(short)%d,\n", locptr->lc_code_type);
	printf("\t\t(short)%d,\n", locptr->lc_length);
	printf("\t\t(col_t *)&%s_col_t,\n", localename);
	printf("\t\t(ctype_t *)&%s_ctype_t,\n", localename);
	printf("\t\t(mon_t *)&%s_mon_t,\n", localename);
	printf("\t\t(num_t *)&%s_num_t,\n", localename);
	printf("\t\t(tim_t *)&%s_tim_t,\n", localename);
	printf("\t\t(msg_t *)&%s_msg_t,\n", localename);
	printf("\t\t(map_t *)&%s_map_t\n", localename);
	printf("\t}\n");
	printf("};\n\n");

	sprintf(prevlocalename, "&%s_env", localename);

	if (last_pass) {

		printf("env_t	*_envp = (env_t *)%s;\n\n", prevlocalename);

		printf("static loc_t	cur_loc = {\n");
		printf("\t%d,\n\t%d,\n", locptr->lc_mag0, locptr->lc_mag1);
		printf("\t(short)%d,\n", locptr->lc_version);
		printf("\t(short)%d,\n", locptr->lc_code_type);
		printf("\t(short)%d,\n", locptr->lc_length);
		printf("\t(col_t *)&%s_col_t,\n", localename);
		printf("\t(ctype_t *)&%s_ctype_t,\n", localename);
		printf("\t(mon_t *)&%s_mon_t,\n", localename);
		printf("\t(num_t *)&%s_num_t,\n", localename);
		printf("\t(tim_t *)&%s_tim_t,\n", localename);
		printf("\t(msg_t *)&%s_msg_t,\n", localename);
		printf("\t(map_t *)&%s_map_t\n", localename);
		printf("};\n\n");

		printf("loc_t		*_locp = &cur_loc;\n\n");
	}
}


pshort(label, ptr, len)
char *label;		/* null terminated label */
unsigned short *ptr;	/* ptr to data to be copied to byte format */
int len;		/* number of bytes to read and copy to output */
{
	int	i;	/* value counter for current line */
	int	cnt;	/* # bytes actually copied this line */
	int	sav;	/* copy of input parameter */

	len = swaplong(len);
	len = len / sizeof(unsigned short);
	sav = len;

	printf("\nstatic const short %s_%s[] = {", localename, label);

	if(len == 0)  {
		printf("\n\t0x0");
	}
	else  {
	    while(len > 0) {

		if(len >= NUMELEM)
			cnt = NUMELEM;
		else
			cnt = len;
		len -= cnt;

		for(i=0; i < cnt; i++) {
			if(i)  {
				/* not first field */
				if((i == (cnt - 1)) && (len == 0))  {
					/* last element */
					/* printf(" 0x%02x, 0x0", *ptr++); */
					printf(" %d, 0x0", swapshort(*ptr++));
				}
				else  {
					/* not last element */
					/* printf(" 0x%02x,", *ptr++); */
					printf(" %d,", swapshort(*ptr++));
				}
			}
			else  {
				/* first field */
				if((i == (cnt - 1)) && (len == 0))  {
					/* last element */
					/* printf("\n\t0x%02x, 0x0", *ptr++); */
					printf("\n\t%d, 0x0", swapshort(*ptr++));
				}
				else  {
					/* not last element */
					/* printf("\n\t0x%02x,", *ptr++); */
					printf("\n\t%d,", swapshort(*ptr++));
				}
			}
		}
	    }

	}

	printf("\n};\n\n");
}

pushort(label, ptr, len)
char *label;		/* null terminated label */
unsigned short *ptr;	/* ptr to data to be copied to byte format */
int len;		/* number of bytes to read and copy to output */
{
	int	i;	/* value counter for current line */
	int	cnt;	/* # bytes actually copied this line */
	int	sav;	/* copy of input parameter */

	len = swaplong(len);
	len = len / sizeof(unsigned short);
	sav = len;

	printf("\nstatic const unsigned short %s_%s[] = {", localename, label);

	if(len == 0)  {
		printf("\n\t0x0");
	}
	else  {
	    while(len > 0) {

		if(len >= NUMELEM)
			cnt = NUMELEM;
		else
			cnt = len;
		len -= cnt;

		for(i=0; i < cnt; i++) {
			if(i)  {
				/* not first field */
				if((i == (cnt - 1)) && (len == 0))  {
					/* last element */
					/* printf(" 0x%02x, 0x0", *ptr++); */
					printf(" %d, 0x0", swapshort(*ptr++));
				}
				else  {
					/* not last element */
					/* printf(" 0x%02x,", *ptr++); */
					printf(" %d,", swapshort(*ptr++));
				}
			}
			else  {
				/* first field */
				if((i == (cnt - 1)) && (len == 0))  {
					/* last element */
					/* printf("\n\t0x%02x, 0x0", *ptr++); */
					printf("\n\t%d, 0x0", swapshort(*ptr++));
				}
				else  {
					/* not last element */
					/* printf("\n\t0x%02x,", *ptr++); */
					printf("\n\t%d,", swapshort(*ptr++));
				}
			}
		}
	    }

	}

	printf("\n};\n\n");
}

pcoldesc(label, ptr, len)
char *label;		/* null terminated label */
coldesc_t *ptr;		/* ptr to data to be copied to byte format */
int len;		/* number of bytes to read and copy to output */
{
	int	i;	/* value counter for current line */
	int	cnt;	/* # bytes actually copied this line */
	int	sav;	/* copy of input parameter */

	len = swaplong(len);
	sav = len;

	printf("\nstatic const coldesc_t %s_%s[] = {", localename, label);

	if(len == 0)  {
		printf("\n\t0x0");
	}
	else  {
	    while(len > 0) {

		if(len >= NUMELEM)
			cnt = NUMELEM;
		else
			cnt = len;
		len -= cnt;

		for(i=0; i < cnt; i++) {
			if(i)  {
				/* not first field */
				if((i == (cnt - 1)) && (len == 0))  {
					/* last element */
					/* printf(" 0x%02x, 0x0", *ptr++); */
					printf(" %d, 0x0", swapshort(*ptr++));
				}
				else  {
					/* not last element */
					/* printf(" 0x%02x,", *ptr++); */
					printf(" %d,", swapshort(*ptr++));
				}
			}
			else  {
				/* first field */
				if((i == (cnt - 1)) && (len == 0))  {
					/* last element */
					/* printf("\n\t0x%02x, 0x0", *ptr++); */
					printf("\n\t%d, 0x0", swapshort(*ptr++));
				}
				else  {
					/* not last element */
					/* printf("\n\t0x%02x,", *ptr++); */
					printf("\n\t%d,", swapshort(*ptr++));
				}
			}
		}
	    }

	}

	printf("\n};\n\n");
}

pwchar(label, ptr, len)
char *label;		/* null terminated label */
wchar_t *ptr;		/* ptr to data to be copied to byte format */
int len;		/* number of bytes to read and copy to output */
{
	int	i;	/* value counter for current line */
	int	cnt;	/* # bytes actually copied this line */
	int	sav;	/* copy of input parameter */

	len = swaplong(len);
	len = len / sizeof(wchar_t);
	sav = len;

	printf("\nstatic const wchar_t %s_%s[] = {", localename, label);

	if(len == 0)  {
		printf("\n\t0x0");
	}
	else  {
	    while(len > 0) {

		if(len >= NUMELEM)
			cnt = NUMELEM;
		else
			cnt = len;
		len -= cnt;

		for(i=0; i < cnt; i++) {
			if(i)  {
				/* not first field */
				if((i == (cnt - 1)) && (len == 0))  {
					/* last element */
					/* printf(" 0x%02x, 0x0", *ptr++); */
					printf(" %d, 0x0", swapshort(*ptr++));
				}
				else  {
					/* not last element */
					/* printf(" 0x%02x,", *ptr++); */
					printf(" %d,", swapshort(*ptr++));
				}
			}
			else  {
				/* first field */
				if((i == (cnt - 1)) && (len == 0))  {
					/* last element */
					/* printf("\n\t0x%02x, 0x0", *ptr++); */
					printf("\n\t%d, 0x0", swapshort(*ptr++));
				}
				else  {
					/* not last element */
					/* printf("\n\t0x%02x,", *ptr++); */
					printf("\n\t%d,", swapshort(*ptr++));
				}
			}
		}
	    }

	}

	printf("\n};\n\n");
}

pbyte(label, ptr, len)
char *label;		/* null terminated label */
unsigned char *ptr;	/* ptr to data to be copied to byte format */
int len;		/* number of bytes to read and copy to output */
{
	int	i;	/* value counter for current line */
	int	cnt;	/* # bytes actually copied this line */
	int	sav;	/* copy of input parameter */

	sav = len;

	printf("\nstatic const unsigned char %s_%s[] = {", localename, label);

	if(len == 0)  {
		printf("\n\t0x0");
	}
	else  {
	    while(len > 0) {

		if(len >= NUMELEM)
			cnt = NUMELEM;
		else
			cnt = len;
		len -= cnt;

		for(i=0; i < cnt; i++) {
			if(i)  {
				/* not first field */
				if((i == (cnt - 1)) && (len == 0))  {
					/* last element */
					/* printf(" 0x%02x, 0x0", *ptr++); */
					printf(" %d, 0x0", *ptr++);
				}
				else  {
					/* not last element */
					/* printf(" 0x%02x,", *ptr++); */
					printf(" %d,", *ptr++);
				}
			}
			else  {
				/* first field */
				if((i == (cnt - 1)) && (len == 0))  {
					/* last element */
					/* printf("\n\t0x%02x, 0x0", *ptr++); */
					printf("\n\t%d, 0x0", *ptr++);
				}
				else  {
					/* not last element */
					/* printf("\n\t0x%02x,", *ptr++); */
					printf("\n\t%d,", *ptr++);
				}
			}
		}
	    }

	}

	printf("\n};\n\n");
}

int
get_ctab_info(locfile, locfsize)
char *locfile;	/* name of ctab binary file */
int locfsize;	/* size of ctab binary file in bytes */
{
	int	cfile;		/* file descriptor for ctab binary file */

        if ((cfile = open(locfile, O_RDONLY, 0)) < 0)
                return(-1);

        /*
        ** Allocate space for the ctab table.
        */

        if( (loc_tptr = (loc_t *)malloc((unsigned)locfsize)) == NULL){
                close(cfile);
                return(-1);
        }
        if( read(cfile, loc_tptr, locfsize)  != locfsize) {
                (void)close(cfile);
                free(loc_tptr);
                return(-1);
        }
        (void)close(cfile);
        if( loc_tptr->lc_mag0 != NLCTMAG0 || loc_tptr->lc_mag1 != NLCTMAG1 ) {
                free(loc_tptr);
                return (-1);
        }

	reloc_ctab(loc_tptr);
	reloc_ctype(loc_tptr);
	return(0);
}

static
void reloc_ctab(ctab)
loc_t *ctab;
{
	col_t *coltbl;

	coltbl = (col_t *)((char *)ctab + (long)swaplong(ctab->lc_coltbl));
	ctab->lc_coltbl = (col_t *)coltbl;

	if (coltbl->len_collate) {
		coltbl->lc_collate = (short *)
			((char *)ctab + (long)swaplong(coltbl->lc_collate));
	}
	if (coltbl->len_coluniq) {
		coltbl->lc_coluniq = (short *)
			((char *)ctab + (long)swaplong(coltbl->lc_coluniq));
	}
	if (coltbl->len_coldesc) {
		coltbl->lc_coldesc = (coldesc_t *)
			((char *)ctab + (long)swaplong(coltbl->lc_coldesc));
	}
	if (coltbl->len_strings) {
		coltbl->lc_strings = (wchar_t *)
			((char *)ctab + (long)swaplong(coltbl->lc_strings));
	}
}

static
void reloc_ctype(ctab)
loc_t *ctab;
{
	ctype_t	*chrtbl;

	chrtbl = (ctype_t *)((char *)ctab + (long)swaplong(ctab->lc_chrtbl));
	ctab->lc_chrtbl = (ctype_t *)chrtbl;

	if (chrtbl->len_caseconv) {
		chrtbl->lc_caseconv = (wchar_t *)( (char *)ctab + 
					(long)swaplong(chrtbl->lc_caseconv));
	}
	if (chrtbl->len_ctype) {
		chrtbl->lc_ctype = (unsigned short *)( (char *)ctab + 
					(long)swaplong(chrtbl->lc_ctype));
	}
}

static int 
parse_entries(locptr, ptr, locale)
loc_t	*locptr;	/* pointer to locale data */
char	*ptr;		/* pointer to beginning of memory copy of .en file */
char	*locale;	/* used only for setting locale_name */
{
	int	val;	/* sscanf needs integer for destination value */
	char 	*line,	/* Pointer to the beginning of a line */
		*locale_name,
		*lhsval,
		*rhsval,
		*bufp,
	 	*delimiter; 

	mon_t	*monptr;
	num_t	*numptr;
	tim_t	*timptr;
	msg_t	*msgptr;
	map_t	*mapptr;
	
	locale_name = (char *)malloc(PATH_MAX);
	strcpy(locale_name, locale);

	/*
	** Parse the info in the malloced buffer to create a sorted
	** list of locale info (32 fields).
	*/

	monptr = (mon_t *)malloc(sizeof(mon_t));
	locptr->lc_montbl = monptr;
	monptr->lc_locale_name = locale_name; 

	numptr = (num_t *)malloc(sizeof(num_t) );
	locptr->lc_numtbl = numptr;
	numptr->lc_locale_name = locale_name; 

	timptr = (tim_t *)malloc(sizeof(tim_t) );
	locptr->lc_timtbl= timptr; 
	timptr->lc_locale_name = locale_name; 

	msgptr = (msg_t *)malloc(sizeof(msg_t) );
	locptr->lc_msgtbl = msgptr;
	msgptr->lc_locale_name = locale_name; 

	mapptr = (map_t *)malloc(sizeof(map_t));
	locptr->lc_maptbl = mapptr;
	
	bufp = (char *)ptr;
	for(;;) {
		/*
		**	get_line() returns a NULL ptr on end of buffer.
		**	It returns a line containing LHS=RHS pair with a 
		**	NULL termination. The bufp points to the beginning
		**	of the next line each time it is called.
		*/ 
		if( (line = (char *)get_line(&bufp) ) == NULL ) 
			break ;		/* No more info */

		/*
		** Obtain the LHS and RHS separately. 
		** Error check and return if no occurrence of '=' in the line.
		*/
		if(!(rhsval = (char *)strpbrk(line, "=") ))
			continue;
		/*
		** Break at any occurrence of ' ', '\t' or '='.
		*/
		delimiter = (char *)strpbrk(line, " \t=");
		*delimiter = '\0';
		/*
		** Look past '=' sign and white space 
		*/
		++rhsval;
		rhsval += strspn(rhsval, " \t");
		/*
		** The lhsval is a string that can be mapped into an index 
		** (integer) value.
		** Save the rhsval in the localeinfo[] structure at the place 
		** corresponding to the index value.
		*/
		/* Map the lhsval (a string) to index (an integer) in 
		** the range 0 to 31) 
		*/
		lhsval = line;
		/* 
		** Search for a match of the lhs value (string)
		** so that the env structure can point to the rhs value.
		*/
	
	/* LC_MONETARY */
		if(strcmp(lhsval, "INT_CUR_SYM")==0){
			monptr->int_curr_symbol= rhsval;
			continue;
		}
		if(strcmp(lhsval, "CUR_SYM")==0){
			monptr->currency_symbol = rhsval;
			continue;
		}
		if(strcmp(lhsval, "MON_DEC_PNT")==0){
			monptr->mon_decimal_point= rhsval;
			continue;
		}
		if(strcmp(lhsval, "MON_THOUS")==0){
			monptr->mon_thousands_sep= rhsval;
			continue;
		}
		if(strcmp(lhsval, "MON_GRP")==0){
 			monptr->mon_grouping = rhsval;
			continue;
		}
		if(strcmp(lhsval, "POS_SGN")==0){
			monptr->positive_sign= rhsval;
			continue;
		}
		if(strcmp(lhsval, "NEG_SGN")==0){
			monptr->negative_sign = rhsval;
			continue;
		}
		if(strcmp(lhsval, "INT_FRAC")==0){
			if(strcmp(rhsval, "CHAR_MAX")==0)
				val =  CHAR_MAX;
			else
				sscanf(rhsval, "%d", &val);
			monptr->int_frac_digits = val;
			continue;
		}
		if(strcmp(lhsval, "FRAC_DIG")==0){
			if(strcmp(rhsval, "CHAR_MAX")==0)
				val =  CHAR_MAX;
			else
				sscanf(rhsval, "%d", &val);
			monptr->frac_digits = val;
			continue;
		}
		if(strcmp(lhsval, "P_CS_PRE")==0){
			if(strcmp(rhsval, "CHAR_MAX")==0)
				val =  CHAR_MAX;
			else
				sscanf(rhsval, "%d", &val);
			monptr->p_cs_precedes=val;
			continue;
		}
		if(strcmp(lhsval, "P_SEP_SP")==0){
			if(strcmp(rhsval, "CHAR_MAX")==0)
				val =  CHAR_MAX;
			else
				sscanf(rhsval, "%d", &val);
			monptr->p_sep_by_space = val;
			continue;
		}
		if(strcmp(lhsval, "N_CS_PRE")==0){
			if(strcmp(rhsval, "CHAR_MAX")==0)
				val =  CHAR_MAX;
			else
				sscanf(rhsval, "%d", &val);
			monptr->n_cs_precedes = val;
			continue;
		}
		if(strcmp(lhsval, "N_SEP_SP")==0){
			if(strcmp(rhsval, "CHAR_MAX")==0)
				val =  CHAR_MAX;
			else
				sscanf(rhsval, "%d", &val);
			monptr->n_sep_by_space = val;
			continue;
		}
		if(strcmp(lhsval, "P_SGN_POS")==0){
			if(strcmp(rhsval, "CHAR_MAX")==0)
				val =  CHAR_MAX;
			else
				sscanf(rhsval, "%d", &val);
			monptr->p_sign_posn = val;
			continue;
		}
		if(strcmp(lhsval, "N_SGN_POS")==0){
			if(strcmp(rhsval, "CHAR_MAX")==0)
				val =  CHAR_MAX;
			else
				sscanf(rhsval, "%d", &val);
			monptr->n_sign_posn = val;
			continue;
		}
	/* LC_NUMERIC */
		if(strcmp(lhsval, "DEC_PNT")==0){
			numptr->decimal_point = rhsval;
			continue;
		}
		if(strcmp(lhsval, "THOUS_SEP")==0){
			numptr->thousands_sep = rhsval;
			continue;
		}
		if(strcmp(lhsval, "GROUPING")==0){
			numptr->grouping = rhsval;
			continue;
		}
	/* LC_TIME */
		if(strcmp(lhsval,"NLTIME")==0){
			timptr->t_fmt = rhsval;
			continue;
		}
		if(strcmp(lhsval,"NLDATE" ) == 0){
	    		timptr->d_fmt = rhsval;  
			continue;
		}
		if(strcmp(lhsval,"NLLDATE")==0){
	    		timptr->nlldate = rhsval;
			continue;
		}
		if(strcmp(lhsval,"NLDATIM")==0){
	    		timptr->d_t_fmt = rhsval;
			continue;
		}
		if(strcmp(lhsval,"NLSDAY")==0){
	    		timptr->abday = rhsval;
			continue;
		}
		if(strcmp(lhsval,"NLLDAY")==0){
	    		timptr->day = rhsval; 
			continue;
		}
		if(strcmp(lhsval,"NLSMONTH")==0){
	    		timptr->abmon = rhsval; 
			continue;
		}
		if(strcmp(lhsval,"NLLMONTH")==0){
	    		timptr->mon = rhsval; 
			continue;
		}
		if(strcmp(lhsval,"NLTMISC")==0){
			timptr->misc = rhsval; 
			continue;
		}
		if(strcmp(lhsval,"AMPMSTR" )==0){
			timptr->am_pm = rhsval; 
			continue;
		}
		if(strcmp(lhsval,"NLTSTRS")==0){
			timptr->tstrs = rhsval;
			continue;
		}
		if(strcmp(lhsval,"NLTUNITS" )==0){
			timptr->tunits = rhsval; 
			continue;
		}
		if(strcmp(lhsval,"NLYEAR" )==0){
			timptr->year = rhsval;
			continue;
		}
	/* LC_MESSAGES */
		if(strcmp(lhsval,"MESSAGES")==0){
			msgptr->messages = rhsval ;	
			continue;
		}
		if(strcmp(lhsval,"YESSTR")==0){
			msgptr->yes_string = rhsval;	
			continue;
		}
		if(strcmp(lhsval, "NOSTR")==0){
			msgptr->no_string = rhsval;	
			continue;
		}
	}; /* end of for loop */
	return(0);	/* indicate no error */
}

/*
 * parse .en file and return 0 if no error or -1 if error.
 * The loc_t, mon_t, num_t, tim_t, and msg_t structs are initialized in 
 * memory and contain the correct information from the .en file.
 */
int get_en_info(locptr, locale, locfile, locfsize)
loc_t	*locptr;	/* pointer to loc_t for this locale */
char	*locale;	/* name of locale */
char	*locfile;	/* name of en file containing locale data */
int	locfsize;	/* size of the en file in bytes */
{
	int 	fd;		/* .en file descriptor */
	char	*infoptr;	/* pointer to memory copy of .en file */

	/* open .en file and return NULL if unable to do so */
	if ((fd = open(locfile, O_RDONLY, 0)) < 0)
		return(-1);

	/* malloc space for entire .en file and return with error if unable */
	if( (infoptr = (char *)malloc(locfsize + 1)) == NULL) {
		close(fd);
		return(-1);
	}

	/* read entire .en file into memory and return with error if unable */
	if (read(fd, infoptr, (unsigned)locfsize) != locfsize) {
		close(fd);
		free(infoptr);
		return(-1);
	}
	close (fd);

	/* parse .en file and return 0 if no error or -1 if error */
	return(parse_entries(locptr, infoptr, locale));
}

/*
 * Returns pointer to next string (lhs=rhs pair) in the locale or NULL if none
 * exists.  The string is not necessarily NULL terminated.  The returned
 * pointer points into the .en memory copy and is advanced with each call to 
 * this routine.  Buffers are NOT allocated for each line.
 */
static 
char *get_line(sbufp)
char **sbufp;	/* pointer to string pointer (which is bumped each call) */
{
	char 	*bp, 
		*p, 
		*tp, 
		*newline;

	int 	linegot = 0;

	/*
	**	Validate the buffer pointer before starting the
	**	parse of the buffer to obtain expressions of the
	**	type LHS=RHS.
	*/
	if (sbufp == NULL)
		return(NULL);

	p = (char *)*sbufp;
	
	while (p && !linegot) {
		/* No more lines */
		p += strspn(p, "\n"  ) ;
		if ( *p == NULL )
			break;

		/*
		 * A comment line.  Just continue. Allow for comment
		 * character (#) in the first position only.
		 */
		if (*p == '#') {
			if ( (p = (char *)strchr(p, '\n')) == NULL)
				break;
			p++;
			continue;
		}
	
		bp = (char *)strchr(p, '\n');
		if(bp == NULL){
			p = (char *)NULL; 	
			break;
		}

		/*
		 * Advance buffer pointer past new-line or to null if 
		 * there is no new-line.
		 */
		newline = (char *)p; /* Pointer to the beginning of the line */
		*bp = '\0';
		p = ++bp;
		linegot++;
	} 

	*sbufp = (char *)p; 	/* Point to the next line in the buffer */
	if(linegot)
		return((char *)newline);
	else
		return(NULL);
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
