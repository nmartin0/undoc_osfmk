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
 * Copyright (c) 1988-90 SecureWare, Inc.
 *   All rights reserved
 */

#ident "@(#)getprfient.c	6.3 09:14:20 2/26/91 SecureWare"
/*
 * Based on:
 *   "@(#)getprfient.c	2.7.1.2 23:14:49 1/9/90 SecureWare, Inc."
 */

/*LINTLIBRARY*/


/*
 * This file contains a set of routines used to make programs
 * more secure.  Specifically, this particular file contains
 * routines to implement a file integrity scheme.  The
 * routines parallel those of the getpwent(3) routines for
 * the File Control database.
 */

#include <sys/secdefines.h>
#include "libsecurity.h"

#if SEC_BASE /*{*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <stdio.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>

#if SEC_MAC
#include <mandatory.h>
static mand_ir_t	*mand_ir = (mand_ir_t *) 0;
#endif

#if SEC_ACL_SWARE
#include <acl.h>
#endif

#if SEC_NCAV
#include <ncav.h>

static ncav_ir_t	*ncav_ir = (ncav_ir_t *) 0;
#endif

static struct pr_file *pr_file = (struct pr_file *) 0;
static long filepos = 0L;
static FILE *fp = (FILE *) 0;

extern char *malloc();
extern char *strrchr();
extern char *strcpy();
extern char *strchr();

static int read_fi_fields();
static int parse_fi_field();
static int store_fi_fields();

/*
 * Read the next entry of the File Control database.  If there is an
 * error or there are no more entries, return 0.
 */
struct pr_file *
getprfient()
{
	register char	*bp;

	if (fp == (FILE *) 0)
		setprfient();

	while (!feof(fp)) {
		bp = agetfile(&filepos, fp, (char *) 0);
		if (bp &&
		    read_fi_fields(&pr_file->ufld, &pr_file->uflg, bp) == 0)
			return pr_file;
	}

	return (struct pr_file *) 0;
}


/*
 * Matches exact command name provided or a wild-card and returns the associated
 * entry from the Command Control database.
 */
struct pr_file *
getprfinam(nam)
	char *nam;
{
	char *bp;

	setprfient();

	bp = agetfile(&filepos, fp, nam);
	if (bp && read_fi_fields(&pr_file->ufld, &pr_file->uflg, bp) == 0)
		return pr_file;

	return (struct pr_file *) 0;
}

/*
 * Reset the position of the File Control database so that the
 * next time getprfient() is invoked, it will return the first entry
 * in the database.
 */
void
setprfient()
{
	static time_t modify_time;
	struct stat sb;
	char *filename;
	int ret;

	if (fp == (FILE *) 0) {
		open_auth_file((char *) 0, OT_FILE_CNTL, &fp);
		if (fp != (FILE *) 0) {
			fstat (fileno(fp), &sb);
			modify_time = sb.st_mtime;
		}
	} else {
		filename = find_auth_file ((char *) 0, OT_FILE_CNTL);
		ret = stat (filename, &sb);
		if (ret != 0 || sb.st_mtime > modify_time) {
			(void) fclose (fp);
			open_auth_file((char *) 0, OT_FILE_CNTL, &fp);
			if (fp != (FILE *) 0) {
				fstat (fileno(fp), &sb);
				modify_time = sb.st_mtime;
			}
		}
		free (filename);
	}
	filepos = 0L;
	if (pr_file == (struct pr_file *) 0) {
		pr_file = (struct pr_file *) malloc (sizeof (*pr_file));
		if (pr_file == (struct pr_file *) 0) {
			endprfient();
		}
	}

#if SEC_MAC
	mand_init();

	if (mand_ir == (mand_ir_t *) 0) {
		mand_ir = mand_alloc_ir();
		if (mand_ir == (mand_ir_t *) 0) {
			endprfient();
		}
	}
#endif
#if SEC_NCAV
	ncav_init();

	if (ncav_ir == (ncav_ir_t *) 0) {
		ncav_ir = ncav_alloc_ir();
		if (ncav_ir == (ncav_ir_t *) 0) {
			endprfient();
		}
	}
#endif

}

/*
 * Close the file(s) related the to the File Control database.
 */
void
endprfient()
{
	if (fp != (FILE *) 0)  {
		(void) fclose(fp);
		fp = (FILE *) 0;
	}
	filepos = 0L;
#if SEC_MAC
	if (mand_ir != (mand_ir_t *) 0)
		mand_free_ir(mand_ir);
#endif
	end_authcap (OT_FILE_CNTL);
}


/*
 * Place an entry into the File Control database under the given
 * file name.  Replace an existing entry if the names compare or add
 * this entry at the end.  (The entry is deleted if the fg_devname
 * is 0.)  Lock the entire Authentication database for this operation.
 * When done, the File Control database is closed.
 */
int
putprfinam(nam, p)
	register char *nam;
	register struct pr_file *p;
{
	register FILE *tempfile;
	register int replaced;
	register int status;
	register int cfs_status;
	register char *pathname;
	char *temppathname;
	char *oldpathname;
	char *bp;
	char filebuf[512];

	status = 0;
	replaced = 0;

	setprfient();

	pathname = find_auth_file(nam, OT_FILE_CNTL);

	if (!make_transition_files(pathname, &temppathname, &oldpathname))  {
		endprfient();
		free(pathname);
		return (0);
	}

	cfs_status = create_file_securely(temppathname, AUTH_VERBOSE,
			     MSGSTR(GETPRFIENT_1, "make new File Control database"));
	if (cfs_status != CFS_GOOD_RETURN) {
		endprfient();
		free(pathname);
		return(0);
	}

	/* now file is locked.  Reference the current database */

	tempfile = fopen(temppathname, "w");
	if (tempfile == (FILE *) 0)  {
		unlink(temppathname);
		free(temppathname);
		free(oldpathname);
	}
	else  {
		status = 1;
		rewind(fp);
		while (status && fgets(filebuf, sizeof filebuf, fp)) {
			if (strcmp(nam, filebuf) == 0 &&
			    filebuf[strlen(nam)] == ':') {
				(void) agetfile(&filepos, fp, (char *) 0);
				status = store_fi_fields(tempfile,
						nam, &p->ufld, &p->uflg);
				replaced = 1;
			} else {
				do status = fputs(filebuf, tempfile) != NULL;
				while (status &&
					filebuf[strlen(filebuf)-2] == '\\' &&
					fgets(filebuf, sizeof filebuf, fp));
				filepos = ftell(fp);
			}
		}

		if (status && !replaced)
			status = store_fi_fields(tempfile, nam, &p->ufld,
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

	endprfient();

	return status;
}


static char *file_table[] = {
	AUTH_F_OWNER,
	AUTH_F_GROUP,
	AUTH_F_MODE,
	AUTH_F_TYPE,
#if SEC_MAC
	AUTH_F_SLEVEL,
#endif
#if SEC_ACL_SWARE
	AUTH_F_ACL,
#endif
#if SEC_NCAV
	AUTH_F_NCAV,
#endif
#if SEC_PRIV
	AUTH_F_PPRIVS,
	AUTH_F_GPRIVS,
#endif
};

enum {
	OwnerOffset = 0,
	GroupOffset,
	ModeOffset,
	TypeOffset
#if SEC_MAC
      , SLevelOffset
#endif
#if SEC_ACL_SWARE
      , ACLOffset
#endif
#if SEC_NCAV
      , NCAVOffset
#endif
#if SEC_PRIV
      , PPrivOffset,
	GPrivOffset
#endif
};

#define	F_OWNER		(int) OwnerOffset
#define	F_GROUP		(int) GroupOffset
#define	F_MODE		(int) ModeOffset
#define	F_TYPE		(int) TypeOffset
#if SEC_MAC
#define	F_SLEVEL	(int) SLevelOffset
#endif
#if SEC_ACL_SWARE
#define	F_ACL		(int) ACLOffset
#endif
#if SEC_NCAV
#define F_NCAV		(int) NCAVOffset
#endif
#if SEC_PRIV
#define	F_PPRIVS	(int) PPrivOffset
#define	F_GPRIVS	(int) GPrivOffset
#endif

#define	FILE_TABLE_SIZE	(sizeof file_table / sizeof file_table[0])


/*
 * Read the fields for a File Control entry.  They are read
 * from the authcap entry currently loaded.  This routine must be
 * called twice for a full Protected Password entry, one for specific
 * file fields/flags and one for system default fields/flags.
 */
static int
read_fi_fields(fld, flg, cp)
	register struct f_field *fld;
	register struct f_flag *flg;
	register char *cp;
{
	register int	i, label_len;
	char	*end_field, *end_entry, savec;

	(void) strncpy ((char *) fld, "", sizeof (*fld));
	(void) strncpy ((char *) flg, "", sizeof (*flg));

	/* Find end of entry.  agetent ensures that it ends in :AUTH_CHKENT: */
	end_entry = cp + strlen(cp) - sizeof AUTH_CHKENT;

	/* Get the name field */
	if (*cp == ':') {
		fld->fd_name = (char *) 0;
		flg->fg_name = 0;
		++cp;
	} else {
		end_field = strchr(cp, ':');
		if (end_field == (char *) 0)
			return 1;
		*end_field = '\0';
		fld->fd_name = cp;
		flg->fg_name = 1;
		cp = end_field + 1;
	}

	/* Get the remaining fields */
	while (cp < end_entry) {
		end_field = strchr(cp, ':');
		if (end_field == (char *) 0)
			return 1;
		*end_field = '\0';
		label_len = strcspn(cp, "=@#");
		savec = cp[label_len];
		cp[label_len] = '\0';
		for (i = 0; i < FILE_TABLE_SIZE; ++i)
			if (strcmp(cp, file_table[i]) == 0) {
				cp[label_len] = savec;
				cp += label_len;
				if (parse_fi_field(cp, i, fld, flg))
					return 1;
				break;
			}
		cp = end_field + 1;
		while (*cp == ':')
			++cp;
	}
	return 0;
}

static int
parse_fi_field(field, index, fld, flg)
	register char		*field;
	int			index;
	register struct f_field	*fld;
	register struct f_flag	*flg;
{
	int	id;

	switch (index) {
	case F_OWNER:
		if (*field != '=' || (id = pw_nametoid(&field[1])) == -1)
			return 1;
		fld->fd_uid = id;
		flg->fg_uid = 1;
		break;

	case F_GROUP:
		if (*field != '=' || (id = gr_nametoid(&field[1])) == -1)
			return 1;
		fld->fd_gid = id;
		flg->fg_gid = 1;
		break;

	case F_MODE:
		if (*field != '#')
			return 1;
		fld->fd_mode = adecodenum(&field[1]);
		flg->fg_mode = 1;
		break;
		
	case F_TYPE:
		if (*field != '=')
			return 1;
		fld->fd_type[0] = field[1];
		flg->fg_type = 1;
		break;

#if SEC_MAC
	case F_SLEVEL:
		if (*field != '=')
			return 1;
		if (strcmp(&field[1], AUTH_F_WILD) == 0)
			fld->fd_slevel = (mand_ir_t *) 0;
		else if (strcmp(&field[1], AUTH_F_SYSLO) == 0) {
			memcpy(mand_ir, mand_syslo, mand_bytes());
			fld->fd_slevel = mand_ir;
		} else if (strcmp(&field[1], AUTH_F_SYSHI) == 0) {
			memcpy(mand_ir, mand_syshi, mand_bytes());
			fld->fd_slevel = mand_ir;
		} else
			return 1;
		flg->fg_slevel = 1;
		break;
#endif

#if SEC_NCAV
	case F_NCAV:
		if (*field != '=')
			return 1;
		if (strcmp(&field[1], AUTH_F_WILD) == 0)
			fld->fd_ncav = (ncav_ir_t *) 0;
		else if (strcmp(&field[1], AUTH_F_ALL) == 0) {
			*ncav_ir = *ncav_max_ir;
			fld->fd_ncav = ncav_ir;
		} else
			return 1;
		flg->fg_ncav = 1;
		break;
#endif

#if SEC_ACL_SWARE
	case F_ACL:
		if (*field != '=')
			return 1;
		if (strcmp(&field[1], AUTH_F_WILD) == 0) {
			fld->fd_acl = ACL_DELETE;
			fld->fd_acllen = 0;
		} else {
			fld->fd_acl = acl_er_to_ir(&field[1], &fld->fd_acllen);
			if (fld->fd_acl == (acle_t *) 0)
				return 1;
		}
		flg->fg_acl = 1;
		break;
#endif

#if SEC_PRIV
	case F_PPRIVS:
		if (*field != '=')
			return 1;
		loadnamepair(fld->fd_pprivs, SEC_MAX_SPRIV, &field[1],
				sys_priv, AUTH_PPRIVS, OT_FILE_CNTL,
				fld->fd_name);
		flg->fg_pprivs = 1;
		break;

	case F_GPRIVS:
		if (*field != '=')
			return 1;
		loadnamepair(fld->fd_gprivs, SEC_MAX_SPRIV, &field[1],
				sys_priv, AUTH_GPRIVS, OT_FILE_CNTL,
				fld->fd_name);
		flg->fg_gprivs = 1;
		break;
#endif
	}

	return 0;
}


/*
 * Store the file specific fields and flags associated with a File Control
 * entry.  This routine outputs to the actual file.  It returns 1 if there
 * is no error and 0 if an error occurred in writing.
 */
static int
store_fi_fields(f, name, fd, fg)
	register FILE *f;
	register char *name;
	register struct f_field *fd;
	register struct f_flag *fg;
{
	register int fields = 1;
	int error;
	char *uname;
	char *gname;

	error = (fflush(f) != 0);

	if (!error) {
		error = fprintf(f, "%s:", name) == EOF;
		fields = pr_newline(f, fields, &error);
	}

	if (!error && fg->fg_uid)  {
		uname = pw_idtoname (fd->fd_uid);
		error = uname == (char *) 0 ||
			fprintf(f, "%s=%s:", AUTH_F_OWNER, uname) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_gid)  {
		gname = gr_idtoname (fd->fd_gid);
		error = gname == (char *) 0 ||
			fprintf(f, "%s=%s:", AUTH_F_GROUP, gname) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_mode)  {
		error = fprintf(f, "%s#0%o:", AUTH_F_MODE,
				(uint) fd->fd_mode) == EOF;
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_type)  {
		error = fprintf(f, "%s=%s:", AUTH_F_TYPE, fd->fd_type) == EOF;
		fields = pr_newline(f, fields, &error);
	}
#if SEC_MAC
	if (!error && fg->fg_slevel)  {
		char *mander;

		if (fd->fd_slevel == (mand_ir_t *) 0)
			mander = AUTH_F_WILD;
		else if (memcmp(fd->fd_slevel, mand_syslo, mand_bytes()) == 0)
			mander = AUTH_F_SYSLO;
		else if (memcmp(fd->fd_slevel, mand_syshi, mand_bytes()) == 0)
			mander = AUTH_F_SYSHI;
		else
			mander = (char *) 0;
		if (mander) {
			error = fprintf(f, "%s=%s:", AUTH_F_SLEVEL,
					mander) == EOF;
			fields = pr_newline(f, fields, &error);
		} else
			error = 1;
	}
#endif
#if SEC_NCAV
	if (!error && fg->fg_ncav)  {
		char *ncaver;

		if (fd->fd_ncav == (ncav_ir_t *) 0)
			ncaver = AUTH_F_WILD;
		else if (*fd->fd_ncav == *ncav_max_ir)
			ncaver = AUTH_F_ALL;
		else
			ncaver = (char *) 0;
		if (ncaver) {
			error = fprintf(f, "%s=%s:", AUTH_F_NCAV,
					ncaver) == EOF;
			fields = pr_newline(f, fields, &error);
		} else
			error = 1;
	}
#endif
#if SEC_ACL_SWARE
	if (!error && fg->fg_acl)  {
		char *acler;

		if (fd->fd_acl == ACL_DELETE)
			acler = AUTH_F_WILD;
		else
			acler = acl_ir_to_er(fd->fd_acl, fd->fd_acllen);
		if (acler) {
			error = fprintf(f, "%s=%s:", AUTH_F_ACL, acler) == EOF;
			fields = pr_newline(f, fields, &error);
		}
		else
			error = 1;
	}
#endif
#if SEC_PRIV
	if (!error && fg->fg_pprivs)  {
		fields = pr_newline(f, fields, &error);
	}
	if (!error && fg->fg_gprivs)  {
		fields = pr_newline(f, fields, &error);
	}
#endif

	if (!error && (name != (char *) 0))
		error = fprintf(f, "%s:\n", AUTH_CHKENT) == EOF;

	error = (fflush(f) != 0) || error;

	return !error;
}
#endif /*} SEC_BASE */
