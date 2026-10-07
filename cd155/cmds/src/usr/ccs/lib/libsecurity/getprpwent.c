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
 * OSF/1 Release 1.0.1
 */
/*
 * Copyright (c) 1988-90 SecureWare, Inc.  All rights reserved.
 */

#ident "@(#)getprpwent.c	6.2 09:14:24 2/26/91 SecureWare"
/*
 * Based on:
 *   "@(#)getprpwent.c	2.10.2.1 18:17:53 12/29/89 SecureWare"
 */

/*LINTLIBRARY*/


/*
 * This file contains a set of routines used to make programs
 * more secure.  Specifically, this particular file contains
 * routines to implement a protected password scheme.  The
 * routines parallel those of the getpwent(3) routines for
 * the Protected Password database.
 */

#include <sys/secdefines.h>
#include "libsecurity.h"

#if SEC_BASE /*{*/

#include <sys/types.h>
#include <stdio.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>
#include <pwd.h>

#if SEC_NCAV
#include <ncav.h>
#endif

#define	FIELDSPERLINE	4
#define min(a,b) (a)<(b)?(a):(b)

static int last_uid_found = -1;
static struct pr_passwd *pr_passwd = (struct pr_passwd *) 0;

extern struct passwd *getpwent();
extern char *strcpy();


/*
 * Read the next entry of the Protected Password database.  If there is an
 * error or there are no more entries, return 0.
 */
struct pr_passwd *
getprpwent()
{
	int found = 0;
	register struct passwd *p;
	register struct pr_passwd *status = (struct pr_passwd *) 0;

	check_auth_parameters();

	if ((last_uid_found < 0) ||
	    (pw_idtoname(last_uid_found) == (char *) 0))
		setpwent();

	while (!found && ((p = getpwent()) != (struct passwd *) 0))  {
		status = getprpwnam(p->pw_name);
		found = (status != (struct pr_passwd *) 0);
		if (found)
			last_uid_found = p->pw_uid;
	}

	return status;
}


/*
 * Matches exact login name provided and returns the associated
 * entry from the Protected Password database.
 */
struct pr_passwd *
getprpwnam(nam)
	register char *nam;
{
	register struct pr_passwd *status;

	check_auth_parameters();

	status = (struct pr_passwd *) 0;

	setprpwent();

	if (agetuser(nam) == 1)  {
		status = pr_passwd;
		if (read_pw_fields(&status->ufld, &status->uflg) == -1)
			status = (struct pr_passwd *) 0;
		else {
			setprdfent();
			if (agetdefault() != 1 ||
			    read_pw_fields(&status->sfld, &status->sflg) == -1)
				status = (struct pr_passwd *) 0;
		}
	}
	else
		status = (struct pr_passwd *) 0;

	return status;
}


/*
 * Matches exact login UID provided and returns the associated
 * entry from the Protected Password database.
 */
struct pr_passwd *
getprpwuid(uid)
	register int uid;
{
	register char *name;

	check_auth_parameters();

	name = pw_idtoname (uid);
	if (name == (char *) 0)
		return ((struct pr_passwd *) 0);
	else
		return (getprpwnam (name));
}


/*
 * Reset the position of the Protected Password database so that the
 * next time getprpwent() is invoked, it will return the first entry
 * in the database.  Since the database works off of the uid value,
 * this involves resetting the uid value associated with the previous
 * entry.
 */
void
setprpwent()
{
	check_auth_parameters();

	last_uid_found = -1;

	if (pr_passwd == (struct pr_passwd *) 0) {
		pr_passwd = (struct pr_passwd *)
		  malloc (sizeof (*pr_passwd));
		if (pr_passwd == (struct pr_passwd *) 0) {
			endprpwent();
		}
	}
}

/*
 * "Close" the Protected Password database.  Since the database works off
 * of the uid value, this involves resetting the uid value associated with
 * the previous entry.  Also note that there is no need to close a file since
 * in the Protected Password database, there is a file for each entry that
 * is closed immediately after the entry is retrieved.
 */
void
endprpwent()
{
	check_auth_parameters();

	last_uid_found = -1;
	end_authcap (OT_PRPWD);
}


/*
 * Place an entry into the Protected Password database under the given
 * name.  Lock the entire Authentication database for this operation.
 * When done, the Protected Password database entry is closed.
 */
int
putprpwnam(nam, p)
	register char *nam;
	register struct pr_passwd *p;
{
	register FILE *file;
	register char *pathname;
	register int status;
	register int cfs_status;
	char *temppathname;
	char *oldpathname;

	check_auth_parameters();

	status = 0;

	pathname = find_auth_file(nam, OT_PRPWD);

	if (!make_transition_files(pathname, &temppathname, &oldpathname))  {
		endprpwent();
		return (0);
	}

	cfs_status = create_file_securely(temppathname, AUTH_VERBOSE,
			     MSGSTR(GETPRPWENT_1, "make new Protected Password entry"));
	if (cfs_status != CFS_GOOD_RETURN) {
		endprpwent();
		return(0);
	}

	file = fopen(temppathname, "w");
	if (file == (FILE *) 0)  {
		unlink(temppathname);
		free(temppathname);
		free(oldpathname);
	}
	else  {
		if (p->uflg.fg_name)  {
			status = store_pw_fields(file, nam, &p->ufld, &p->uflg);
			status = (fclose(file) == 0) && status;
			if (status)
				status = replace_file(temppathname, pathname,
					      oldpathname);
			else {
				(void) unlink (temppathname);
				free (temppathname);
				free (oldpathname);
			}
		}
		else  {
			status = (fclose(file) == 0);
			(void) unlink(temppathname);
			status = (unlink(pathname) == 0) && status;
			free(temppathname);
			free(oldpathname);
		}
	}

	free(pathname);

	endprpwent();

	return status;
}

/*
 * Read the fields for a Protected Password entry.  They are read
 * from the authcap entry currently loaded.  This routine must be
 * called twice for a full Protected Password entry, one for user
 * fields/flags and one for system default fields/flags.
 * returns 0 on success, -1 on failure.
 */
int
read_pw_fields(fld, flg)
	register struct pr_field *fld;
	register struct pr_flag *flg;
{
	long numres;
	int flgres;
	char *strres;
	char namelist[1000];

	extern void loadnamepair();

	check_auth_parameters();

	(void) strncpy ((char *) fld, "", sizeof (*fld));
	(void) strncpy ((char *) flg, "", sizeof (*flg));

	/* set up for command authorizations.
	 * The user must call this routine or authorized_user()
	 * before using the ST_MAX_CPRIV constant.
	 */
	if (build_cmd_priv() == -1)
		return(-1);

	strres = fld->fd_name;
	strres = agetstr(AUTH_U_NAME, &strres);
	if (strres != (char *) 0)
		flg->fg_name = 1;

	if(agtnum(AUTH_U_ID,&numres) == 0) {
		flg->fg_uid = 1;
		fld->fd_uid = numres;
	}

	strres = fld->fd_encrypt;
	strres = agetstr(AUTH_U_PWD, &strres);
	if (strres != (char *) 0)
		flg->fg_encrypt = 1;

	strres = fld->fd_owner;
	strres = agetstr(AUTH_U_OWNER, &strres);
	if (strres != (char *) 0)
		flg->fg_owner = 1;

	if(agtnum(AUTH_U_PRIORITY,&numres) == 0) {
		flg->fg_nice = 1;
		fld->fd_nice = numres;
	}

	strres = namelist;
	strres = agetstr(AUTH_U_CMDPRIV, &strres);
	if (strres != (char *) 0)  {
		flg->fg_cprivs = 1;
		loadnamepair(fld->fd_cprivs, ST_MAX_CPRIV, strres,
			     cmd_priv, AUTH_CMDPRIV, OT_PRPWD, fld->fd_name);
	}

	strres = namelist;
	strres = agetstr(AUTH_U_SYSPRIV, &strres);
	if (strres != (char *) 0)  {
		flg->fg_sprivs = 1;
		loadnamepair(fld->fd_sprivs, SEC_MAX_SPRIV, strres,
			     sys_priv, AUTH_SYSPRIV, OT_PRPWD, fld->fd_name);
	}

#if SEC_NCAV
	flg->fg_nat_citizen = 0;
	strres = namelist;
	strres = agetstr(AUTH_U_NATCITIZEN, &strres);
	if (strres != (char *) 0)  {
		ncav_ir_t	*ncav_ir_ptr;

		ncav_ir_ptr = ncav_er_to_ir(strres);
		if (ncav_ir_ptr != (ncav_ir_t *) 0) {
			fld->fd_nat_citizen =
				(ncav_ir_t *) malloc(sizeof(ncav_ir_t));
			if (fld->fd_nat_citizen != (ncav_ir_t *) 0) {
				memcpy(fld->fd_nat_citizen, ncav_ir_ptr,
				       sizeof(ncav_ir_t));
				flg->fg_nat_citizen = 1;
			}
		}
	}

	flg->fg_nat_caveats = 0;
	strres = namelist;
	strres = agetstr(AUTH_U_NATCAVEATS, &strres);
	if (strres != (char *) 0)  {
		ncav_ir_t	*ncav_ir_ptr;

		ncav_ir_ptr = ncav_er_to_ir(strres);
		if (ncav_ir_ptr != (ncav_ir_t *) 0) {
			fld->fd_nat_caveats =
				(ncav_ir_t *) malloc(sizeof(ncav_ir_t));
			if (fld->fd_nat_caveats != (ncav_ir_t *) 0) {
				memcpy(fld->fd_nat_caveats, ncav_ir_ptr,
				       sizeof(ncav_ir_t));
				flg->fg_nat_caveats = 1;
			}
		}
	}

#endif

	strres = namelist;
	strres = agetstr(AUTH_U_BASEPRIV, &strres);
	if (strres != (char *) 0)  {
		flg->fg_bprivs = 1;
		loadnamepair(fld->fd_bprivs, SEC_MAX_SPRIV, strres,
			     sys_priv, AUTH_BASEPRIV, OT_PRPWD, fld->fd_name);
	}

	strres = namelist;
	strres = agetstr(AUTH_U_AUDITCNTL, &strres);
	if (strres != (char *) 0)  {
		flg->fg_auditcntl = 1;
		loadnamepair(fld->fd_auditcntl, AUTH_MAX_AUDITMASK,
			     strres, audit_mask, AUTH_AUDITMASK, OT_PRPWD,
			     fld->fd_name);
	}

	strres = namelist;
	strres = agetstr(AUTH_U_AUDITDISP, &strres);
	if (strres != (char *) 0)  {
		flg->fg_auditdisp = 1;
		loadnamepair(fld->fd_auditdisp, AUTH_MAX_AUDITMASK,
			     strres, audit_mask, AUTH_AUDITMASK, OT_PRPWD,				     fld->fd_name);
	}


	if(agtnum(AUTH_U_MINCHG,&numres) == 0) {
		flg->fg_min = 1;
		fld->fd_min = (time_t) numres;
	}

	if(agtnum(AUTH_U_MAXLEN,&numres) == 0) {
		flg->fg_maxlen = 1;
		fld->fd_maxlen = numres;
	}

	if(agtnum(AUTH_U_EXP,&numres) == 0) {
		flg->fg_expire = 1;
		fld->fd_expire = (time_t) numres;
	}

	if(agtnum(AUTH_U_LIFE,&numres) == 0) {
		flg->fg_lifetime = 1;
		fld->fd_lifetime = (time_t) numres;
	}

	if(agttime(AUTH_U_SUCCHG,&numres) == 0) {
		flg->fg_schange = 1;
		fld->fd_schange = (time_t) numres;
	}

	if(agtnum(AUTH_U_UNSUCCHG,&numres) == 0) {
		flg->fg_uchange = 1;
		fld->fd_uchange = (time_t) numres;
	}

	flgres = agetflag(AUTH_U_PICKPWD);
	if (flgres != -1)  {
		flg->fg_pick_pwd = 1;
		fld->fd_pick_pwd = flgres;
	}

	flgres = agetflag(AUTH_U_GENPWD);
	if (flgres != -1)  {
		flg->fg_gen_pwd = 1;
		fld->fd_gen_pwd = flgres;
	}

	flgres = agetflag(AUTH_U_RESTRICT);
	if (flgres != -1)  {
		flg->fg_restrict = 1;
		fld->fd_restrict = flgres;
	}

	flgres = agetflag(AUTH_U_NULLPW);
	if (flgres != -1) {
		flg->fg_nullpw = 1;
		fld->fd_nullpw = flgres;
	}

	numres = agetuid(AUTH_U_PWCHANGER);
	if (numres != -1) {
		flg->fg_pwchanger = 1;
		fld->fd_pwchanger = (uid_t) numres;
	}


	flgres = agetflag(AUTH_U_GENCHARS);
	if (flgres != -1)  {
		flg->fg_gen_chars = 1;
		fld->fd_gen_chars = flgres;
	}

	flgres = agetflag(AUTH_U_GENLETTERS);
	if (flgres != -1)  {
		flg->fg_gen_letters = 1;
		fld->fd_gen_letters = flgres;
	}

	strres = fld->fd_tod;
	strres = agetstr(AUTH_U_TOD, &strres);
	if (strres != (char *) 0)
		flg->fg_tod = 1;



#if SEC_MAC
	strres = namelist;
	strres = agetstr(AUTH_U_CLEARANCE, &strres);
	if (strres != (char *) 0)  {
		mand_ir_t *clearance;

		clearance = clearance_er_to_ir(strres);
		if (clearance != (mand_ir_t *) 0)  {
			mand_copy_ir(clearance, &fld->fd_clearance);
			flg->fg_clearance = 1;
			mand_free_ir(clearance);
		}
	}
#endif



	if(agttime(AUTH_U_SUCLOG,&numres) == 0) {
		flg->fg_slogin = 1;
		fld->fd_slogin = (time_t) numres;
	}

	strres = namelist;
	strres = agetstr(AUTH_U_SUCTTY, &strres);
	if (strres != (char *) 0) {
		flg->fg_suctty = 1;
		strncpy (fld->fd_suctty, strres, sizeof(fld->fd_suctty));
	}

	if(agtnum(AUTH_U_UNSUCLOG,&numres) == 0) {
		flg->fg_ulogin = 1;
		fld->fd_ulogin = (time_t) numres;
	}

	strres = namelist;
	strres = agetstr(AUTH_U_UNSUCTTY, &strres);
	if (strres != (char *) 0) {
		flg->fg_unsuctty = 1;
		strncpy (fld->fd_unsuctty, strres, sizeof(fld->fd_unsuctty));
	}

	if(agtnum(AUTH_U_NUMUNSUCLOG,&numres) == 0) {
		flg->fg_nlogins = 1;
		fld->fd_nlogins = (short) numres;
	}

	if(agtnum(AUTH_U_MAXTRIES,&numres) == 0) {
		flg->fg_max_tries = 1;
		fld->fd_max_tries = (short) numres;
	}

	flgres = agetflag(AUTH_U_RETIRED);
	if (flgres != -1)  {
		flg->fg_retired = 1;
		fld->fd_retired = flgres;
	}

	flgres = agetflag(AUTH_U_LOCK);
	if (flgres != -1)  {
		flg->fg_lock = 1;
		fld->fd_lock = flgres;
	}

	return(0);
}


/*
 * Store the user fields and flags associated with a Protected Password
 * entry.  This routine outputs to the actual file.  It returns 1 if there
 * is no error and 0 if an error occurred in writing.
 */
int
store_pw_fields(f, name, fd, fg)
	register FILE *f;
	register char *name;
	register struct pr_field *fd;
	register struct pr_flag *fg;
{
	register int fields = 1;
	register char *namelist;
	int error;
	char *uname;

	check_auth_parameters();

	/* In all cases, set up for command authorizations.
	 * The user will call this routine or
	 * authorized_user before using the ST_MAX_CPRIV variable.
	 */
	build_cmd_priv();

	error = (fflush(f) != 0);

	if (!error && (name != (char *) 0))  {
		error = fprintf(f, "%s:", name) == EOF;
		fields = pr_newline(f, fields, &error);
	}

	if (!error && fg->fg_name)  {
		error = fprintf(f, "%s=%*s:", AUTH_U_NAME,
			min(strlen(fd->fd_name), sizeof(fd->fd_name)),
			fd->fd_name) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_uid)  {
		error = fprintf(f, "%s#%u:", AUTH_U_ID,
				(uint) fd->fd_uid) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_encrypt)  {
		if (fields > 1)
			fields = pr_newline(f, FIELDSPERLINE, &error);
		error = fprintf(f, "%s=%*s:", AUTH_U_PWD,
			min(strlen(fd->fd_encrypt), sizeof(fd->fd_encrypt)),
			fd->fd_encrypt) == EOF;
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}
	if (!error && fg->fg_owner)  {
		error = fprintf(f, "%s=%*s:", AUTH_U_OWNER,
			min(strlen(fd->fd_owner), sizeof(fd->fd_owner)),
			fd->fd_owner) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_nice)  {
		error = fprintf(f, "%s#%d:", AUTH_U_PRIORITY,
				fd->fd_nice) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_cprivs)  {
		namelist = storenamepair(fd->fd_cprivs, ST_MAX_CPRIV,
					 cmd_priv, AUTH_CMDPRIV);
		error = fprintf(f, "%s=%s:", AUTH_U_CMDPRIV, namelist) == EOF;
		free(namelist);
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_sprivs)  {
		if (fields > 1)
			fields = pr_newline(f, FIELDSPERLINE, &error);
		namelist =
			storenamepair(fd->fd_sprivs, SEC_MAX_SPRIV, sys_priv,
				      AUTH_SYSPRIV);
		error = fprintf(f, "%s=%s:", AUTH_U_SYSPRIV, namelist) == EOF;
		free(namelist);
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}

#if defined(NCAV)

	if (!error && fg->fg_nat_citizen)  {
		if (fields > 1)
			fields = pr_newline(f, FIELDSPERLINE, &error);
		namelist = ncav_ir_to_er(fd->fd_nat_citizen);
		if (namelist)
			error = fprintf(f, "%s=%s:", AUTH_U_NATCITIZEN,
					namelist) == EOF;
		else
			error = 1;
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}

	if (!error )  {
		if (!fg->fg_nat_caveats)  {
			ncav_init();
			fg->fg_nat_caveats = 1;
			memcpy(fd->fd_nat_caveats, ncav_max_ir,
			       sizeof( ncav_ir_t) );
		}
		if (fields > 1)
			fields = pr_newline(f, FIELDSPERLINE, &error);
		namelist = ncav_ir_to_er(fd->fd_nat_caveats);
		if (namelist)
			error = fprintf(f, "%s=%s:", AUTH_U_NATCAVEATS,
					namelist) == EOF;
		else
			error = 1;
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}

#endif

	if (!error && fg->fg_bprivs)  {
		if (fields > 1)
			fields = pr_newline(f, FIELDSPERLINE, &error);
		namelist =
			storenamepair(fd->fd_bprivs, SEC_MAX_SPRIV, sys_priv,
				      AUTH_BASEPRIV);
		error = fprintf(f, "%s=%s:", AUTH_U_BASEPRIV, namelist) == EOF;
		free(namelist);
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}

	if (!error && fg->fg_auditcntl)  {
		if (fields > 1)
			fields = pr_newline(f, FIELDSPERLINE, &error);
		namelist = storenamepair(fd->fd_auditcntl, AUTH_MAX_AUDITMASK,
					 audit_mask, AUTH_AUDITMASK);
		error = fprintf(f, "%s=%s:", AUTH_U_AUDITCNTL, namelist) == EOF;
		free(namelist);
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}
	if (!error && fg->fg_auditdisp)  {
		if (fields > 1)
			fields = pr_newline(f, FIELDSPERLINE, &error);
		namelist = storenamepair(fd->fd_auditdisp, AUTH_MAX_AUDITMASK,
					 audit_mask, AUTH_AUDITMASK);
		error = fprintf(f, "%s=%s:", AUTH_U_AUDITDISP, namelist) == EOF;
		free(namelist);
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}

	if (!error && fg->fg_min)  {
		error = fprintf(f, "%s#%lu:", AUTH_U_MINCHG,
				(ulong) fd->fd_min) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_maxlen)  {
		error = fprintf(f, "%s#%u:", AUTH_U_MAXLEN,
				(uint) fd->fd_maxlen) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_expire)  {
		error = fprintf(f, "%s#%lu:", AUTH_U_EXP,
				(ulong) fd->fd_expire) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_lifetime)  {
		error = fprintf(f, "%s#%lu:", AUTH_U_LIFE,
				(ulong) fd->fd_lifetime) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_schange)  {
		error = fprintf(f, "%s#%lu:", AUTH_U_SUCCHG,
				(ulong) fd->fd_schange) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_uchange)  {
		error = fprintf(f, "%s#%lu:", AUTH_U_UNSUCCHG,
				(ulong) fd->fd_uchange) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_pick_pwd)  {
		error = fprintf(f, "%s%s:", AUTH_U_PICKPWD,
				storebool(fd->fd_pick_pwd)) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_gen_pwd)  {
		error = fprintf(f, "%s%s:", AUTH_U_GENPWD,
				storebool(fd->fd_gen_pwd)) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_restrict)  {
		error = fprintf(f, "%s%s:", AUTH_U_RESTRICT,
			storebool(fd->fd_restrict)) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_nullpw) {
		error = fprintf(f, "%s%s:", AUTH_U_NULLPW,
			storebool(fd->fd_nullpw)) == EOF;
		fields = pr_newline(f, fields, &error);
	}

	if (!error && fg->fg_pwchanger) {
		if (fg->fg_uid && fg->fg_name && fd->fd_pwchanger == fd->fd_uid)
			uname = fd->fd_name;
		else
			uname = pw_idtoname(fd->fd_pwchanger);
		error = uname == (char *) 0 ||
			fprintf(f, "%s=%s:", AUTH_U_PWCHANGER, uname) == EOF;
		fields = pr_newline(f, fields, &error);
	}


	if (!error && fg->fg_gen_chars)  {
		error = fprintf(f, "%s%s:", AUTH_U_GENCHARS,
			storebool(fd->fd_gen_chars)) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_gen_letters)  {
		error = fprintf(f, "%s%s:", AUTH_U_GENLETTERS,
			storebool(fd->fd_gen_letters)) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_tod)  {
		error = fprintf(f, "%s=%s:", AUTH_U_TOD, fd->fd_tod) == EOF;
		fields = pr_newline(f, fields, &error);
	}

#if SEC_MAC
	if (!error && fg->fg_clearance)  {
		char *clearance_er;

		if (fields > 1)
			fields = pr_newline(f, FIELDSPERLINE, &error);
		clearance_er = clearance_ir_to_er(&fd->fd_clearance);
		if (clearance_er)
			error = fprintf(f, "%s=%s:", AUTH_U_CLEARANCE,
					clearance_er) == EOF;
		else
			error = 1;
		fields = pr_newline(f, FIELDSPERLINE, &error);
	}
#endif

	if (!error && fg->fg_slogin)  {
		error = fprintf(f, "%s#%lu:", AUTH_U_SUCLOG,
				(ulong) fd->fd_slogin) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_suctty) {
		error = fprintf(f, "%s=%s:", AUTH_U_SUCTTY,
				fd->fd_suctty) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_ulogin)  {
		error = fprintf(f, "%s#%lu:", AUTH_U_UNSUCLOG,
				(ulong) fd->fd_ulogin) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_unsuctty) {
		error = fprintf(f, "%s=%s:", AUTH_U_UNSUCTTY,
				fd->fd_unsuctty) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_nlogins)  {
		error = fprintf(f, "%s#%u:", AUTH_U_NUMUNSUCLOG,
				(uint) fd->fd_nlogins) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_max_tries)  {
		error = fprintf(f, "%s#%u:", AUTH_U_MAXTRIES,
				(uint) fd->fd_max_tries) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_retired)  {
		error = fprintf(f, "%s%s:", AUTH_U_RETIRED,
				storebool(fd->fd_retired)) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_lock)  {
		error = fprintf(f, "%s%s:", AUTH_U_LOCK,
				storebool(fd->fd_lock)) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && (name != (char *) 0))
		error = fprintf(f, "%s:\n", AUTH_CHKENT) == EOF;

	error = (fflush(f) != 0) || error;

	return !error;
}

#endif /*} SEC_BASE */
