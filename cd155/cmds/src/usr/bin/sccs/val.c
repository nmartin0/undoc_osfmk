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
static char rcsid[] = "@(#)$RCSfile: val.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:37:57 $";
#endif
/*
 * COMPONENT_NAME: CMDSCCS      Source Code Control System (sccs)
 *
 * FUNCTIONS: escdodelt, findsid, fredck, initarg, nextarg, process,
 *            report, validate, main
 *
 * ORIGINS: 3, 10, 27
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
 * val.c 1.4 com/cmd/sccs/cmd,3.1,9021 9/15/89 13:38:22";
 */


/************************************************************************/
/*									*/
/*  val -                                                               */
/*  val [-midentifier] [-rSID] [-s] [-ytype] file ...                   */
/*                                                                      */
/************************************************************************/

# include	"defines.h"
# include	"had.h"

# include 	"val_msg.h"
#define MSGSTR(Num, Str) catgets(catd, MS_VAL, Num, Str)

# define	FILARG_ERR	0200	/* no file name given */
# define	UNKDUP_ERR	0100	/* unknown or duplicate keyletter */
# define	CORRUPT_ERR	040	/* corrupt file error code */
# define	FILENAM_ERR	020	/* file name error code */
# define        INVALSID_ERR    010     /* invalid SID error */
# define	NONEXSID_ERR	04	/* non-existent SID error code */
# define	TYPE_ERR	02	/* type arg value error code */
# define	NAME_ERR	01	/* name arg value error code */
# define	TRUE		1
# define	FALSE		0

int	ret_code;	/* prime return code from 'main' program */
int	inline_err;	/* input line error code (from 'process') */
int	infile_err;	/* file error code (from 'validate') */
char    **xargv;        /* command line arguments */
char    **xargvp;       /* ptr into xargv */
char    *linep;         /* ptr into line for command from stdin */
int     silent;         /* set to suppress msgs */

struct packet gpkt;
struct sid sid;

char	had[26];	/* had flag used in 'process' function */
char    *type;          /* ptr to type (-y) value */
char    *name;          /* ptr to name (-m) value */
char    *prline;        /* line to print upon error */
char	line[BUFSIZ];
char    *malloc();      /* function returning ptr to new memory */
char    *fgets();       /* function returning ptr to line read */
char    *strcpy();      /* function returning ptr to copied string */

char    Error[128];
struct  stat Statbuf;


nl_catd catd;

/* This is the main program that determines whether the command line
 * comes from the standard input or read off the original command
 * line.  See VAL(I) for more information.
*/
main(argc,argv)
int argc;
char	*argv[];
{
	Fflags = FTLJMP;

	catd = catopen(MF_SCCS, 0);

	if (argc == 2 && argv[1][0] == '-' && !(argv[1][1])) {
		while (fgets(line,BUFSIZ,stdin))
			if (line[0] != '\n') {
				repl (line,'\n','\0');
				prline = line;
				process();
			}
	}
	else {
		xargv = argv+1;
		prline = NULL;
		process();
	}
	exit(ret_code);
}


/* Perform initialization before calls to nextarg() */
initarg()
{
	static char *linebuf;

	if (xargvp = xargv) return;
	if (linebuf) free(linebuf);
	linep = linebuf = strcpy(malloc(size(line)),line);
}


/* Return a ptr to the next argument on the command line.
 * If the command line was read from stdin, arguments are delimited by
 * blanks and tabs.
 */
char *
nextarg()
{
	register char *p;

	if (xargvp) return *xargvp++;

	p = linep + strspn(linep," \t");    /* skip blanks */
	if (!*p) return(NULL);
	linep = p + strcspn(p, " \t");      /* skip to next blank */
	if (*linep) *linep++ = '\0';
	return(p);
}


/* This function processes a command line.  It
 * determines which keyletter values are present on the command
 * line and assigns the values to the correct storage place.  It
 * then calls validate for each file name on the command line.
*/
process()
{
	register char   *p;
	register int	testklt;
	int	num_files;
	char	c;

	silent = FALSE;
	num_files = inline_err = 0;

	/*
	clear out had flags for each 'line' processed
	*/
	zero(had,sizeof(had));
	/*
	scan for all flags.
	*/
	for (initarg(); p = nextarg();)
		if (*p++ == '-') {
			testklt = TRUE;
			c = *p++;
			switch (c) {
				case 's':
					testklt = 0;
					/*
					turn on 'silent' flag.
					*/
					silent = TRUE;
					break;
				case 'r':
					/*
					check for invalid SID.
					*/
					if (setjmp(Fjmp))
						inline_err |= INVALSID_ERR;
					else {
						extern char *sid_ab();

						chksid(sid_ab(p,&sid),&sid);
						if ((sid.s_rel < MINR) ||
						    (sid.s_rel > MAXR))
							inline_err |= INVALSID_ERR;
					}
					break;
				case 'y':
					type = p;
					break;
				case 'm':
					name = p;
					break;
				default:
					inline_err |= UNKDUP_ERR;
			}
			/*
			use 'had' array and determine if the keyletter
			was given twice.
			*/
			if (had[c - 'a']++ && testklt++)
				inline_err |= UNKDUP_ERR;
		}
		else {
			/*
			assume file name if no '-' preceded argument
			*/
			num_files++;
		}
	/*
	check if any files were named as arguments
	*/
	if (num_files == 0)
		inline_err |= FILARG_ERR;
	/*
	report any errors in command line.
	*/
	report(inline_err);
	/*
	loop through 'validate' for each file on command line.
	*/
	for (initarg(); p = nextarg();)
		if (*p != '-') {
			int validate();

			do_file(p,validate);
		}
}


/* This function actually does the validation on the named file.
 * It determines whether the file is an SCCS-file or if the file
 * exists.  It also determines if the values given for type, SID,
 * and name match those in the named file.  An error code is returned
 * if any mismatch occurs.  See VAL(I) for more information.
*/
validate(file)
char    *file;
{
	extern char             *auxf(), *sname();
	extern struct idel      *dodelt();
	extern char             *Sflags[];
	register char           *p;
	struct stats            stats;

	infile_err = 0;

	if (setjmp(Fjmp)) {
		infile_err |= gpkt.p_iop? CORRUPT_ERR : FILENAM_ERR;
		goto out;
	}
	sinit(&gpkt,file,2);
	/*
	read delta table checking for errors and/or
	SID.
	*/
	if (!dodelt(&gpkt,&stats,(struct sid *)NULL,0))
		fmterr(&gpkt);

	finduser(&gpkt);
	doflags(&gpkt);
	/*
	check if 'y' flag matched '-y' arg value.
	*/
	if (HADY)
		if (!(p = Sflags[TYPEFLAG - 'a']) || !equal(type,p))
			infile_err |= TYPE_ERR;
	/*
	check if 'm' flag matched '-m' arg value.
	*/
	if (HADM)
		if (!equal(name,
		    (p = Sflags[MODFLAG - 'a'])? p : auxf(sname(file),'g')))
			infile_err |= NAME_ERR;

	if (gpkt.p_line[1] != BUSERTXT || gpkt.p_line[0] != CTLCHAR)
		fmterr(&gpkt);
	flushto(&gpkt,EUSERTXT,1);

	/*
	If a valid sid was specified, determine whether it occurs.
	*/
	if (HADR && !(inline_err & INVALSID_ERR))
		if (!findsid())
			infile_err |= NONEXSID_ERR;

	/*
	read remainder of file so 'readmod'
	can check for corruptness.
	*/
	gpkt.p_chkeof = 1;
	while (readmod(&gpkt))
		;

out:    if (gpkt.p_iop) fclose(gpkt.p_iop);
	report(infile_err);
	ffreeall();
}


/* Determine whether the requested sid exists.
 * If the sid is unambiguous (R.L or R.L.B.S) it must occur as is.
 * If R is given, any R.... is sufficient.
 * If R.L.B is given, any R.L.B.... is sufficient.
 */
findsid()
{
	register struct idel *rdp;
	register int n;

	for (n = maxser(&gpkt); n; n--) {
		rdp = &gpkt.p_idel[n];
		if (rdp->i_sid.s_rel == sid.s_rel)
			if (!sid.s_lev ||
			    sid.s_lev == rdp->i_sid.s_lev &&
			    sid.s_br == rdp->i_sid.s_br &&
			    (!sid.s_seq || sid.s_seq == rdp->i_sid.s_seq))
				return(TRUE);
	}
	return(FALSE);
}


/* This function will report the error that occurred on the command
 * line.  It will print one diagnostic message for each error that
 * was found in the named file.
*/
report(code)
register int	code;
/*register char	*file; */
{
	char	percent;

	ret_code |= code;
	if (!code || silent) return;
	percent = '%';		/* '%' for -m and/or -y messages */
	if (prline) {
		printf("%s\n\n",prline);
		prline = NULL;
	}
	if (code & NAME_ERR)
		printf(MSGSTR(MMSMTCH, "Value specified by -m does not match the value in SCCS file.\n"));  /* MSG */
	if (code & TYPE_ERR)
		printf(MSGSTR(YMSMTCH, "Value specified by -y does not match the text in SCCS file.\n"));  /* MSG */
	if (code & NONEXSID_ERR)
		printf(MSGSTR(SIDNOEXST, 
                  "The specified SID does not exist.\n"));  /* MSG */
	if (code & INVALSID_ERR)
		printf(MSGSTR(SIDINVLD, 
                  "The specified SID is not valid.\n"));  /* MSG */
	if (code & FILENAM_ERR)
		printf(MSGSTR(OPNSCCS,
                  "Cannot open the file.\n"));  /* MSG */
	if (code & CORRUPT_ERR)
		printf(MSGSTR(CRPTSCCS, 
                  "Damaged SCCS file.\n"));  /* MSG */
        if (code & UNKDUP_ERR)
                printf(MSGSTR(DUPKEYLTR, 
                  "The specified flag does not exist.\n"));  /* MSG */
        if (code & FILARG_ERR)
                printf(MSGSTR(MSGFILARG, 
                  "Specify a file name or -.\n"));  /* MSG */
}

/* Null routine to satisfy external reference from dodelt() */
escdodelt()
{
}
#ifdef CASSI
/* NULL routine to satisfy external reference from dodelt() */
fredck()
{
}
#endif
