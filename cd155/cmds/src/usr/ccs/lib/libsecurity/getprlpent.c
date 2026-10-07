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
 * Copyright (c) 1988-90 SecureWare, Inc.
 *   All rights reserved
 */

#ident "@(#)getprlpent.c	2.1 16:17:51 4/20/90 SecureWare"
/*
 * Based on:
 *   "@(#)getprlpent.c	2.7 17:03:10 10/20/89 SecureWare"
 */

/*LINTLIBRARY*/


/*
 * This file contains a set of routines used to make programs
 * more secure.  Specifically, this particular file contains
 * routines to implement a printer control scheme.  The
 * routines parallel those of the getpwent(3) routines for
 * the Printer Control database.
 */

#include <sys/secdefines.h>
#include "libsecurity.h"

#if SEC_MAC || SEC_ILB /*{*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>

#define	FIELDSPERLINE	4

static struct pr_lp *pr_lp = (struct pr_lp *) 0;
static long filepos = 0L;
static FILE *fp = (FILE *) 0;


extern int strncmp();
extern char *storebool();

static void read_lp_fields();
static int store_lp_fields();

/*
 * Read the next entry of the Printer Control database.  If there is an
 * error or there are no more entries, return 0.
 */
struct pr_lp *
getprlpent()
{
	register struct pr_lp *status;

	check_auth_parameters();

	if (fp == (FILE *) 0)
		setprlpent();

	if (agetlp(&filepos, fp, (char *) 0) == 1)  {
		status = pr_lp;
		read_lp_fields(&status->ufld, &status->uflg);
	}
	else
		status = (struct pr_lp *) 0;

	return status;
}


/*
 * Matches exact printer name provided (the relative name from /dev) and
 * returns the associated entry from the Printer Control database.
 */
struct pr_lp *
getprlpnam(nam)
	register char *nam;
{
	register struct pr_lp *status;

	check_auth_parameters();

	setprlpent();

	if (agetlp (&filepos, fp, nam) == 1) {
		status = pr_lp;
		read_lp_fields (&status->ufld, &status->uflg);
	}
	else
		status = (struct pr_lp *) 0;
	
	return status;
}


/*
 * Reset the position of the Printer Control database so that the
 * next time getprlpent() is invoked, it will return the first entry
 * in the database.
 */
void
setprlpent()
{
	static time_t modify_time;
	struct stat sb;
	char *filename;
	int ret;

	check_auth_parameters();

	if (fp == (FILE *) 0) {
		open_auth_file((char *) 0, OT_LP_CNTL, &fp);
		if (fp != (FILE *) 0) {
			fstat (fileno(fp), &sb);
			modify_time = sb.st_mtime;
		}
	} else {
		filename = find_auth_file ((char *) 0, OT_LP_CNTL);
		ret = stat (filename, &sb);
		if (ret != 0 || sb.st_mtime > modify_time) {
			(void) fclose (fp);
			open_auth_file((char *) 0, OT_LP_CNTL, &fp);
			if (fp != (FILE *) 0) {
				fstat (fileno(fp), &sb);
				modify_time = sb.st_mtime;
			}
		}
		free (filename);
	}
	filepos = 0L;
	if (pr_lp == (struct pr_lp *) 0) {
		pr_lp = (struct pr_lp *)
		  malloc (sizeof (*pr_lp));
		if (pr_lp == (struct pr_lp *) 0) {
			endprlpent();
		}
	}
}


/*
 * Close the file(s) related the to the Printer Control database.
 */
void
endprlpent()
{
	check_auth_parameters();

	if (fp != (FILE *) 0)  {
		(void) fclose(fp);
		fp = (FILE *) 0;
	}
	filepos = 0L;
	end_authcap (OT_LP_CNTL);
}


/*
 * Place an entry into the Printer Control database under the given
 * tty name.  Replace an existing entry if the names compare or add
 * this entry at the end.  (The entry is deleted if the fg_name
 * is 0.)  Lock the entire Authentication database for this operation.
 * When done, the Printer Control database is closed.
 */
int
putprlpnam(nam, p)
	register char *nam;
	register struct pr_lp *p;
{
	register struct pr_lp *others;
	register FILE *tempfile;
	register int replaced;
	register int status;
	register char *pathname;
	register int cfs_status ;
	char *temppathname;
	char *oldpathname;

	check_auth_parameters();

	status = 0;
	replaced = 0;

	pathname = find_auth_file(nam, OT_LP_CNTL);

	if (!make_transition_files(pathname, &temppathname, &oldpathname))  {
		endprlpent();
		return (0);
	}

	cfs_status = create_file_securely(temppathname, AUTH_VERBOSE,
			     MSGSTR(GETPRLPENT_1, "make new Printer Control database"));
	if (cfs_status != CFS_GOOD_RETURN) {
		endprlpent() ;
		return(0) ;
	}

	/* now file is locked.  Reference the current database */

	if (fp != (FILE *) 0)
		(void) fclose (fp);
	fp = fopen (pathname, "r");
	filepos = 0L;
	
	tempfile = fopen(temppathname, "w");

	if (tempfile == (FILE *) 0)  {
		(void) unlink(temppathname);
		free(temppathname);
		free(oldpathname);
	}
	else  {
		status = 1;
		while (status &&
		   agetlp(&filepos, fp, (char *) 0) == 1)  {
			others = pr_lp;
			read_lp_fields(&others->ufld, &others->uflg);
			if (!others->uflg.fg_name)
				status = store_lp_fields(tempfile, "",
						&others->ufld, &others->uflg);
			else if (strncmp(nam, others->ufld.fd_name,
				    sizeof(others->ufld.fd_name)) == 0)  {
				if (p->uflg.fg_name)
					status = store_lp_fields(tempfile, nam,
							&p->ufld, &p->uflg);
				replaced = 1;
			}
			else
				status = store_lp_fields(tempfile,
						others->ufld.fd_name,
						&others->ufld, &others->uflg);
		}

		if (status && !replaced && p->uflg.fg_name)
			status = store_lp_fields(tempfile, nam, &p->ufld,
						 &p->uflg);

		status = (fclose(tempfile) == 0) && status;


		if (status)
			status = replace_file(temppathname, pathname,
				oldpathname);
		else {
			(void) unlink(temppathname);
			free(temppathname);
			free(oldpathname);
		}

	}

	free(pathname);

	endprlpent();

	return status;
}


/*
 * Read the fields for a Printer Control entry.  They are read
 * from the authcap entry currently loaded.
 */
static void
read_lp_fields(fld, flg)
	register struct l_field *fld;
	register struct l_flag *flg;
{
	long numres;
	register int flgres;
	char *strres;

	check_auth_parameters();

	(void) strncpy ((char *) fld, "", sizeof (*fld));
	(void) strncpy ((char *) flg, "", sizeof (*flg));


	strres = fld->fd_name;
	strres = agetstr(AUTH_L_LPNAME, &strres);
	if (strres != (char *) 0)
		flg->fg_name = 1;

	strres = fld->fd_initseq;
	strres = agetstr(AUTH_L_INITSEQ, &strres);
	if (strres != (char *) 0)
		flg->fg_initseq = 1;

	strres = fld->fd_termseq;
	strres = agetstr(AUTH_L_TERMSEQ, &strres);
	if (strres != (char *) 0)
		flg->fg_termseq = 1;

	strres = fld->fd_emph;
	strres = agetstr(AUTH_L_EMPH, &strres);
	if (strres != (char *) 0)
		flg->fg_emph = 1;

	strres = fld->fd_deemph;
	strres = agetstr(AUTH_L_DEEMPH, &strres);
	if (strres != (char *) 0)
		flg->fg_deemph = 1;

	strres = fld->fd_chrs;
	strres = agetstr(AUTH_L_CHRS, &strres);
	if (strres != (char *) 0)
		flg->fg_chrs = 1;

	if (agtnum(AUTH_L_CHRSLEN, &numres) == 0) {
		flg->fg_chrslen = 1;
		fld->fd_chrslen = (ushort) numres;
	}

	strres = fld->fd_escs;
	strres = agetstr(AUTH_L_ESCS, &strres);
	if (strres != (char *) 0)
		flg->fg_escs = 1;

	if (agtnum(AUTH_L_ESCSLEN, &numres) == 0) {
		flg->fg_escslen = 1;
		fld->fd_escslen = (ushort) numres;
	}

	if (agtnum(AUTH_L_LINELEN, &numres) == 0) {
		flg->fg_linelen = 1;
		fld->fd_linelen = numres;
	}

	if (agtnum(AUTH_L_PAGELEN, &numres) == 0) {
		flg->fg_pagelen = 1;
		fld->fd_pagelen = numres;
	}

	flgres = agetflag(AUTH_L_TRUNCLINE);
	if (flgres != -1)  {
		flg->fg_truncline = 1;
		fld->fd_truncline = flgres;
	}
}


/*
 * Store the user fields and flags associated with a Printer Control
 * entry.  This routine outputs to the actual file.  It returns 1 if there
 * is no error and 0 if an error occurred in writing.
 */
static int
store_lp_fields(f, name, fd, fg)
	register FILE *f;
	register char *name;
	register struct l_field *fd;
	register struct l_flag *fg;
{
	register int fields = 1;
	int error;

	check_auth_parameters();

	error = (fflush(f) != 0);

	if (!error && (name != (char *) 0))  {
		error = fprintf(f, "%s:", name) == EOF;
		fields = pr_newline(f, fields, &error);
	}

	if (!error && fg->fg_name)  {
		error = fprintf(f, "%s=%s:", AUTH_L_LPNAME, fd->fd_name) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_initseq)  {
		if (fields > 1)
			fields = pr_newline(f, FIELDSPERLINE, &error);
		error = fprintf(f, "%s=%s:", AUTH_L_INITSEQ,
				fd->fd_initseq) == EOF;
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}
	if (!error && fg->fg_termseq)  {
		if (fields > 1)
			fields = pr_newline(f, FIELDSPERLINE, &error);
		error = fprintf(f, "%s=%s:", AUTH_L_TERMSEQ,
				fd->fd_termseq) == EOF;
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}
	if (!error && fg->fg_chrs)  {
		if (fields > 1)
			fields = pr_newline(f, FIELDSPERLINE, &error);
		error = fprintf(f, "%s=%s:", AUTH_L_CHRS, fd->fd_chrs) == EOF;
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}
	if (!error && fg->fg_escs)  {
		if (fields > 1)
			fields = pr_newline(f, FIELDSPERLINE, &error);
		error = fprintf(f, "%s=%s:", AUTH_L_ESCS, fd->fd_escs) == EOF;
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}
	if (!error && fg->fg_emph)  {
		error = fprintf(f, "%s=%s:", AUTH_L_EMPH, fd->fd_emph) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_deemph)  {
		error = fprintf(f, "%s=%s:", AUTH_L_DEEMPH,
				fd->fd_deemph) == EOF;
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}
	if (!error && fg->fg_chrslen)  {
		error = fprintf(f, "%s#%u:", AUTH_L_CHRSLEN,
				(uint) fd->fd_chrslen) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_escslen)  {
		error = fprintf(f, "%s#%u:", AUTH_L_ESCSLEN,
				(uint) fd->fd_escslen) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_linelen)  {
		error = fprintf(f, "%s#%u:", AUTH_L_LINELEN,
				(uint) fd->fd_linelen) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_pagelen)  {
		error = fprintf(f, "%s#%u:", AUTH_L_PAGELEN,
				(uint) fd->fd_pagelen) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_truncline)  {
		error = fprintf(f, "%s%s:", AUTH_L_TRUNCLINE,
				storebool(fd->fd_truncline)) == EOF;
		fields = pr_newline(f, fields, &error);
	}

	if (!error && (name != (char *) 0))
		error = fprintf(f, "%s:\n", AUTH_CHKENT) == EOF;

	error = (fflush(f) != 0) || error;

	return !error;
}
#endif /*} SEC_MAC || SEC_ILB */
