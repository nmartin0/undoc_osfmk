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
 * Copyright (c) 1988-1990 SecureWare, Inc.  All Rights Reserved.
 */

#ident "@(#)getdvagent.c	6.5 14:55:29 3/5/91 SecureWare"
/*
 * Based on:
 *   "@(#)getdvagent.c	2.8.1.3 23:14:41 1/9/90 SecureWare"
 */

/*LINTLIBRARY*/


/*
 * This file contains * routines to implement a device assignment database.
 * The routines parallel those of the getpwent(3) routines for
 * the Device Assignment database.
 */

#include <sys/secdefines.h>
#include "libsecurity.h"

#if SEC_BASE /*{*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>

#if SEC_NCAV
#include <ncav.h>
#endif

/* static memory to hold components of the device assignment database */

static struct dev_asg *dev_asg = (struct dev_asg *) 0;
static char **old_dev_list;
static char **old_user_list;
#if SEC_MAC
/* device-specific buffers */
static mand_ir_t *min_sl_buffer;
static mand_ir_t *max_sl_buffer;
static mand_ir_t *cur_sl_buffer;
/* system default buffers */
static mand_ir_t *d_min_sl_buffer;
static mand_ir_t *d_max_sl_buffer;
static mand_ir_t *d_cur_sl_buffer;
#endif /* SEC_MAC */

#if SEC_ILB
/* device-specific buffer */
static ilb_ir_t *cur_il_buffer;
/* system default buffer */
static ilb_ir_t *d_cur_il_buffer;
#endif /* SEC_ILB */

#if SEC_NCAV
/* device-specific buffer */
static ncav_ir_t *min_ncav_buffer;
static ncav_ir_t *max_ncav_buffer;
static ncav_ir_t *cur_ncav_buffer;
/* system default buffer */
static ncav_ir_t *d_min_ncav_buffer;
static ncav_ir_t *d_max_ncav_buffer;
static ncav_ir_t *d_cur_ncav_buffer;
#endif /* SEC_NCAV */

static long filepos = 0L;
static FILE *fp = (FILE *) 0;


static int store_dv_fields();
static int read_dv_fields();
static int match_name();
static int parse_dv_field();

extern int strncmp();
extern char *malloc();
extern char *strrchr();
extern char *strchr();
extern char *fgets();
extern char *strcpy();


/*
 * Read the next entry of the File Control database.  If there is an
 * error or there are no more entries, return 0.
 */
struct dev_asg *
getdvagent()
{
	struct dev_asg *status = (struct dev_asg *) 0;
	char *buf;

	check_auth_parameters();

	if (fp == (FILE *) 0)
		setdvagent();

	buf = agetdvag (&filepos, fp, (char *) 0);
	if (buf && read_dv_fields(&dev_asg->ufld, &dev_asg->uflg, buf, 0) == 0)
	{
		setprdfent();
		buf = agetdefault_buf();
		if (buf &&
		    read_dv_fields(&dev_asg->sfld, &dev_asg->sflg, buf, 1) == 0)
				status = dev_asg;
	}

	return status;
}

/* match a particular name in the device assignment database.
 * if there is an error or there are no more entries, return 0.
 */

struct dev_asg *
getdvagnam (nam)
char *nam;
{
	struct dev_asg *status = (struct dev_asg *) 0;
	char *buf;

	check_auth_parameters();

	setdvagent();

	buf = agetdvag (&filepos, fp, nam);
	if (buf && read_dv_fields(&dev_asg->ufld, &dev_asg->uflg, buf, 0) == 0)
	{
		setprdfent();
		buf = agetdefault_buf();
		if (buf &&
		    read_dv_fields(&dev_asg->sfld, &dev_asg->sflg, buf, 1) == 0)
				status = dev_asg;
	}

	return status;
}

/*
 * Reset the position of the Device Assignment database so that the
 * next time getdvagent() is invoked, it will return the first entry
 * in the database.
 */
void
setdvagent()
{
	static time_t modify_time;
	struct stat sb;
	char *filename;
	int ret;

	check_auth_parameters();

	if (fp == (FILE *) 0) {
		open_auth_file((char *) 0, OT_DEV_ASG, &fp);
		if (fp != (FILE *) 0) {
			fstat (fileno(fp), &sb);
			modify_time = sb.st_mtime;
		}
	} else {
		filename = find_auth_file ((char *) 0, OT_DEV_ASG);
		ret = stat (filename, &sb);
		if (ret != 0 || sb.st_mtime > modify_time) {
			(void) fclose (fp);
			open_auth_file((char *) 0, OT_DEV_ASG, &fp);
			if (fp != (FILE *) 0) {
				fstat (fileno(fp), &sb);
				modify_time = sb.st_mtime;
			}
		}
		free (filename);
	}
	filepos = 0L;
	if (dev_asg == (struct dev_asg *) 0) {
		dev_asg = (struct dev_asg *) calloc(sizeof (*dev_asg), 1);
		if (dev_asg == (struct dev_asg *) 0) {
			enddvagent();
		}
	}
}

/*
 * Close the file related the to the Device Assignment database.
 */
void
enddvagent()
{
	check_auth_parameters();

	if (fp != (FILE *) 0)  {
		(void) fclose(fp);
		fp = (FILE *) 0;
	}
	filepos = 0L;
	if (dev_asg != (struct dev_asg *) 0) {
		free ((char *) dev_asg);
		dev_asg = (struct dev_asg *) 0;
#if SEC_MAC
		if (min_sl_buffer) {
			mand_free_ir(min_sl_buffer);
			min_sl_buffer = (mand_ir_t *) 0;
		}
		if (d_min_sl_buffer) {
			mand_free_ir(d_min_sl_buffer);
			d_min_sl_buffer = (mand_ir_t *) 0;
		}
		if (max_sl_buffer) {
			mand_free_ir(max_sl_buffer);
			max_sl_buffer = (mand_ir_t *) 0;
		}
		if (d_max_sl_buffer) {
			mand_free_ir(d_max_sl_buffer);
			d_max_sl_buffer = (mand_ir_t *) 0;
		}
		if (cur_sl_buffer) {
			mand_free_ir(cur_sl_buffer);
			cur_sl_buffer = (mand_ir_t *) 0;
		}
		if (d_cur_sl_buffer) {
			mand_free_ir(d_cur_sl_buffer);
			d_cur_sl_buffer = (mand_ir_t *) 0;
		}
#endif /* SEC_MAC */
#if SEC_ILB
		if (cur_il_buffer) {
			ilb_free_ir(cur_il_buffer);
			cur_il_buffer = (ilb_ir_t *) 0;
		}
		if (d_cur_il_buffer) {
			ilb_free_ir(d_cur_il_buffer);
			d_cur_il_buffer = (ilb_ir_t *) 0;
		}
#endif /* SEC_ILB */
		if (old_dev_list) {
			free((char *) old_dev_list);
			old_dev_list = (char **) 0;
		}
		if (old_user_list) {
			free((char *) old_user_list);
			old_user_list = (char **) 0;
		}
#if SEC_NCAV
		if (max_ncav_buffer) {
			free((char *) max_ncav_buffer);
			max_ncav_buffer = (ncav_ir_t *) 0;
		}
		if (d_max_ncav_buffer) {
			free((char *) d_max_ncav_buffer);
			d_max_ncav_buffer = (ncav_ir_t *) 0;
		}
		if (min_ncav_buffer) {
			free((char *) min_ncav_buffer);
			min_ncav_buffer = (ncav_ir_t *) 0;
		}
		if (d_min_ncav_buffer) {
			free((char *) d_min_ncav_buffer);
			d_min_ncav_buffer = (ncav_ir_t *) 0;
		}
		if (cur_ncav_buffer) {
			free((char *) cur_ncav_buffer);
			cur_ncav_buffer = (ncav_ir_t *) 0;
		}
		if (d_cur_ncav_buffer) {
			free((char *) cur_ncav_buffer);
			d_cur_ncav_buffer = (ncav_ir_t *) 0;
		}
#endif /* SEC_NCAV */
	}
	end_authcap (OT_DEV_ASG);
}

/*
 * Place an entry into the Device Assignment database under the given
 * file name.  Replace an existing entry if the names compare or add
 * this entry at the end.  (The entry is deleted if the fg_devname
 * is 0.)  Lock the entire Authentication database for this operation.
 * When done, the File Control database is closed.
 */
int
putdvagnam(nam, p)
	register char *nam;
	register struct dev_asg *p;
{
	register FILE *tempfile;
	register int replaced;
	register int status;
	register char *pathname;
	register int cfs_status;
	char *temppathname;
	char *oldpathname;
	char filebuf[256];
	char continuation;

	check_auth_parameters();

	status = 0;
	replaced = 0;

	setdvagent();

	pathname = find_auth_file(nam, OT_DEV_ASG);

	if (!make_transition_files(pathname, &temppathname, &oldpathname))  {
		enddvagent();
		return (0);
	}

	cfs_status = create_file_securely(temppathname, AUTH_VERBOSE,
			     MSGSTR(GETDVAGENT_1, "make new Device Assignment database"));
	if (cfs_status != CFS_GOOD_RETURN) {
		enddvagent();
		return(0);
	}

	/* now file is locked.  Reference the current database */

	tempfile = fopen(temppathname, "w");
	if (tempfile == (FILE *) 0)  {
		free(temppathname);
		free(oldpathname);
	}
	else  {
		status = 1;
		fseek (fp, 0L, 0);
		while (status &&
		   (fgets (filebuf, sizeof (filebuf), fp) != (char *) 0)) {
			if (match_name (nam, filebuf) == 0) {
				fseek (fp, filepos, 0);
				(void) agetdvag (&filepos, fp, nam);
				if(p->uflg.fg_name)
					status = store_dv_fields (tempfile, nam,
					 &p->ufld, &p->uflg);
				replaced = 1;
			} else do {
				status = fputs (filebuf, tempfile) != NULL;
				continuation =
				  (filebuf[strlen(filebuf) - 2] == '\\');
				if (continuation)
					fgets (filebuf, sizeof (filebuf), fp);
				filepos = ftell (fp);
			} while (continuation && status);
		}
					
		if (status && !replaced)
			if(p->uflg.fg_name)
			     status = store_dv_fields(tempfile, nam, &p->ufld,
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

	enddvagent();

	return status;
}

/* match the name entry in the file -- be careful about prefixes */

static int
match_name (name, buffer)
char *name;
char *buffer;
{
	char *cp;
	int count;

	cp = strchr (buffer, ':');
	if (cp == (char *) 0)
		return (1);
	count = cp - buffer;
	if (strncmp (buffer, name, count) == 0 && name[count] == 0)
		return (0);
	return (1);
}

/*
 * allocate a new device assignment database entry, and copy an existing
 * entry into it.
 * For alignment, the unaligned strings are at the end of the dynamicall
 * allocated area, while the aligned structures are immediately after the
 * device assignment database entry.  During calculations, str_count
 * maintains the total string size, while align_count keeps the size
 * of the aligned object area.
 */

struct dev_asg *
copydvagent(dv)
register struct dev_asg *dv;
{
	register struct dev_asg *rdv;
	register char *cp;
	char *ocp;
	int i;
	int str_count = 0;
	char *str_cp;
	int align_count = 0;
	char *align_cp;
	int ndevs = 0;
	int nusers = 0;

	if (dv == (struct dev_asg *) 0)
		return (dv);

	/* count up the total string requirements for the new structure
	 */
	if (dv->uflg.fg_devs) {
		for (i = 0; cp = dv->ufld.fd_devs[i]; i++)
			str_count += strlen(cp) + 1;
		ndevs = i + 1;
		align_count += ndevs * sizeof(char *);
	}

	if (dv->uflg.fg_users) {
		for (i = 0; cp = dv->ufld.fd_users[i]; i++)
			str_count += strlen(cp) + 1;
		nusers = i + 1;
		align_count += nusers * sizeof(char *);
	}

	if (dv->uflg.fg_name)
		str_count += strlen(dv->ufld.fd_name) + 1;

#if SEC_NCAV
	align_count += (dv->uflg.fg_max_nat_caveats +
			dv->uflg.fg_min_nat_caveats +
			dv->uflg.fg_cur_nat_caveats +
			dv->sflg.fg_max_nat_caveats +
			dv->sflg.fg_min_nat_caveats +
			dv->sflg.fg_cur_nat_caveats) * sizeof(ncav_ir_t);
#endif

#if SEC_MAC
	align_count += (dv->uflg.fg_min_sl +
			dv->uflg.fg_max_sl +
			dv->uflg.fg_cur_sl +
			dv->sflg.fg_min_sl +
			dv->sflg.fg_max_sl +
			dv->sflg.fg_cur_sl) * mand_bytes();
#endif

#if SEC_ILB
	align_count += (dv->uflg.fg_cur_il +
			dv->sflg.fg_cur_il) * ilb_bytes();
#endif

	/* allocate a new structure and copy strings into it */

	rdv = (struct dev_asg *)
		calloc(sizeof *rdv + str_count + align_count, 1);
	if (rdv == (struct dev_asg *) 0) {
		return ((struct dev_asg *) 0);
	}

	/* set "aligned items" and "unaligned strings" pointers */

	align_cp = (char *) ((unsigned long) rdv + sizeof(struct dev_asg));
	str_cp = (char *) ((unsigned long) align_cp + align_count);
	*rdv = *dv;

	if (dv->uflg.fg_devs) {
		rdv->ufld.fd_devs = (char **) align_cp;
		align_cp += ndevs * sizeof(char *);
		for (i = 0; ocp = dv->ufld.fd_devs[i]; i++) {
			rdv->ufld.fd_devs[i] = str_cp;
			strcpy(str_cp, ocp);
			str_cp += strlen(str_cp) + 1;
		}
		rdv->ufld.fd_devs[i] = (char *) 0;
	}

	if (dv->uflg.fg_users) {
		rdv->ufld.fd_users = (char **) align_cp;
		align_cp += nusers * sizeof(char *);
		for (i = 0; ocp = dv->ufld.fd_users[i]; i++) {
			rdv->ufld.fd_users[i] = str_cp;
			strcpy(str_cp, ocp);
			str_cp += strlen(str_cp) + 1;
		}
		rdv->ufld.fd_users[i] = (char *) 0;
	}

	if (dv->uflg.fg_name) {
		rdv->ufld.fd_name = str_cp;
		strcpy(str_cp, dv->ufld.fd_name);
		str_cp += strlen(str_cp) + 1;
	}

/* macro to copy aligned data to the new structure */

#define COPY_DATA(flg, fld, flg_val, fld_val, type, len) \
	if (dv->flg.flg_val) { \
		rdv->fld.fld_val = (type *) align_cp; \
		align_cp += len; \
		memcpy((char *)rdv->fld.fld_val, \
			(char *)dv->fld.fld_val, len); \
	}

#if SEC_NCAV
	COPY_DATA(uflg, ufld, fg_max_nat_caveats, fd_max_nat_caveats,
	  ncav_ir_t, sizeof ncav_ir_t);
	COPY_DATA(sflg, sfld, fg_max_nat_caveats, fd_max_nat_caveats,
	  ncav_ir_t, sizeof ncav_ir_t);
	COPY_DATA(uflg, ufld, fg_min_nat_caveats, fd_min_nat_caveats,
	  ncav_ir_t, sizeof ncav_ir_t);
	COPY_DATA(sflg, sfld, fg_min_nat_caveats, fd_min_nat_caveats,
	  ncav_ir_t, sizeof ncav_ir_t);
	COPY_DATA(uflg, ufld, fg_cur_nat_caveats, fd_cur_nat_caveats,
	  ncav_ir_t, sizeof ncav_ir_t);
	COPY_DATA(sflg, sfld, fg_cur_nat_caveats, fd_cur_nat_caveats,
	  ncav_ir_t, sizeof ncav_ir_t);
#endif

#if SEC_MAC
	COPY_DATA(uflg, ufld, fg_min_sl, fd_min_sl, mand_ir_t, mand_bytes());
	COPY_DATA(sflg, sfld, fg_min_sl, fd_min_sl, mand_ir_t, mand_bytes());
	COPY_DATA(uflg, ufld, fg_max_sl, fd_max_sl, mand_ir_t, mand_bytes());
	COPY_DATA(sflg, sfld, fg_max_sl, fd_max_sl, mand_ir_t, mand_bytes());
	COPY_DATA(uflg, ufld, fg_cur_sl, fd_cur_sl, mand_ir_t, mand_bytes());
	COPY_DATA(sflg, sfld, fg_cur_sl, fd_cur_sl, mand_ir_t, mand_bytes());
#endif

#if SEC_ILB
	COPY_DATA(uflg, ufld, fg_cur_il, fd_cur_il, ilb_ir_t, ilb_bytes());
	COPY_DATA(sflg, sfld, fg_cur_il, fd_cur_il, ilb_ir_t, ilb_bytes());
#endif

	return (rdv);
}

/* This enumerated type determines the order in the parsing table */

enum {
	DeviceOffset = 0,
	TypeOffset,
	UsersOffset
#if SEC_MAC
      , MaxSLOffset,
	MinSLOffset,
	CurSLOffset
#endif
#if SEC_ILB
      , CurILOffset
#endif
      , AssignOffset
#if SEC_NCAV
      , MaxNatOffset,
	MinNatOffset,
	CurNatOffset
#endif
	};

static char *dev_table[] = {
	AUTH_V_DEVICES,
	AUTH_V_TYPE,
	AUTH_V_USERS,
#if SEC_MAC
	AUTH_V_MAX_SL,
	AUTH_V_MIN_SL,
	AUTH_V_CUR_SL,
#endif
#if SEC_ILB
	AUTH_V_CUR_IL,
#endif
	AUTH_V_ASSIGN,
#if SEC_NCAV
	AUTH_V_MAX_NAT_CAVEATS,
	AUTH_V_MIN_NAT_CAVEATS,
	AUTH_V_CUR_NAT_CAVEATS,
#endif

};

/* offsets into the above array */

#define DEVICES		(int) DeviceOffset
#define TYPE		(int) TypeOffset
#define	USERS		(int) UsersOffset
#if SEC_MAC
#define MAX_SL		(int) MaxSLOffset
#define MIN_SL		(int) MinSLOffset
#define CUR_SL		(int) CurSLOffset
#endif
#if SEC_ILB
#define CUR_IL		(int) CurILOffset
#endif
#define ASSIGN		(int) AssignOffset
#if SEC_NCAV
#define MAX_NAT_CAVEATS	(int) MaxNatOffset
#define MIN_NAT_CAVEATS	(int) MinNatOffset
#define CUR_NAT_CAVEATS	(int) CurNatOffset
#endif

#define DEV_TABLE_SIZE (sizeof (dev_table) / sizeof (dev_table[0]))

static int
read_dv_fields (fld, flg, cp, is_default)
struct dev_field *fld;
struct dev_flag  *flg;
register char *cp;
int is_default;		/* is this the system default entry? */
{
	int i;
	char matchbuf[32];
	char *colon;
	int str_size;
	char *end_entry;

	/* zero out the structures passed */
	memset((char *) fld, '\0', sizeof (*fld));
	memset((char *) flg, '\0', sizeof (*flg));

	/* we know from agetent that the entry ends in :AUTH_CHKENT:
	 * this expression puts us on the first character of AUTH_CHKENT.
	 */
	end_entry = cp + strlen (cp) - sizeof (AUTH_CHKENT);
	/* store the name field */
	if (*cp == ':') {
		fld->fd_name = (char *) 0;
		flg->fg_name = 0;
	} else {
		colon = strchr (cp, ':');
		if (colon == (char *) 0)
			return (1);
		*colon = '\0';
		fld->fd_name = cp;
		flg->fg_name = 1;
		cp = colon + 1;
	}
	/* store the other fields */
	while (cp < end_entry) {
		colon = strchr (cp, ':');
		if (colon == (char *) 0)
			return (1);
		*colon = '\0';
		str_size = strcspn (cp, "=#@");
		strncpy (matchbuf, cp, str_size);
		matchbuf[str_size] = '\0';
		cp += str_size;
		/* match against one of the field names */
		for (i = 0; i < DEV_TABLE_SIZE; i++)
			if (strcmp (matchbuf, dev_table[i]) == 0) {
				if (parse_dv_field (cp, i, fld, flg,
							is_default))
					return (1);
				break;
			}
		/* ignore non-matches, and just skip over them */
		cp = colon + 1;
		while (*cp == ':')
			cp++;
	}
	return (0);
}

/* load the fields into the structure dev_asg.
 * return 0 if OK, 1 if error.
 */

static int
parse_dv_field (field, index, fld, flg, is_default)
char *field;
int index;
struct dev_field *fld;
struct dev_flag  *flg;
int is_default;		/* is this the default entry? */
{
	char **list;
	char **user_list;
	int ret = 0;
#if SEC_MAC
	mand_ir_t *sl;
#endif
#if SEC_ILB
	ilb_ir_t *il;
#endif
#if SEC_NCAV
	ncav_ir_t *ncav;
#endif

	/* all are required to have an '=' sign */
	if (*field != '=')
		return (1);
	field++;

	switch (index) {
	case DEVICES:
		/* system default device list not recognized */

		if (is_default)
			break;

		if (old_dev_list != (char **) 0)
			free ((char *) old_dev_list);
		list = agetstrlist (field);
		if (list != (char **) 0) {
			fld->fd_devs = list;
			flg->fg_devs = 1;
		} else
			ret = 1;
		old_dev_list = list;
		break;

	case USERS:
		/* system default user list not recognized */

		if (is_default)
			break;

		if (old_user_list != (char **) 0)
			free ((char *) old_user_list);
		user_list = agetstrlist(field);
		if (user_list != (char **) 0) {
			fld->fd_users = user_list;
			flg->fg_users = 1;
		} else
			ret = 1;
		old_user_list = user_list;
		break;

#if SEC_MAC
	case MAX_SL:

		/* copy into the existing max sl buffer, if already allocated */

		sl = mand_er_to_ir(field);
		if (sl != (mand_ir_t *) 0) {
			if (is_default)
				if (d_max_sl_buffer == (mand_ir_t *) 0)
					d_max_sl_buffer = sl;
				else {
					mand_copy_ir(sl, d_max_sl_buffer);
					mand_free_ir(sl);
				}
			else
				if (max_sl_buffer == (mand_ir_t *) 0)
					max_sl_buffer = sl;
				else {
					mand_copy_ir(sl, max_sl_buffer);
					mand_free_ir(sl);
				}
			fld->fd_max_sl = is_default ?
						d_max_sl_buffer : max_sl_buffer;
			flg->fg_max_sl = 1;
		}
		break;

	case MIN_SL:
		sl = mand_er_to_ir(field);
		if (sl != (mand_ir_t *) 0) {
			if (is_default)
				if (d_min_sl_buffer == (mand_ir_t *) 0)
					d_min_sl_buffer = sl;
				else {
					mand_copy_ir(sl, d_min_sl_buffer);
					mand_free_ir(sl);
				}
			else
				if (min_sl_buffer == (mand_ir_t *) 0)
					min_sl_buffer = sl;
				else {
					mand_copy_ir(sl, min_sl_buffer);
					mand_free_ir(sl);
				}
			fld->fd_min_sl = is_default ?
						d_min_sl_buffer : min_sl_buffer;
			flg->fg_min_sl = 1;
		}
		break;

	case CUR_SL:
		sl = mand_er_to_ir(field);
		if (sl != (mand_ir_t *) 0) {
			if (is_default)
				if (d_cur_sl_buffer == (mand_ir_t *) 0)
					d_cur_sl_buffer = sl;
				else {
					mand_copy_ir(sl, d_cur_sl_buffer);
					mand_free_ir(sl);
				}
			else
				if (cur_sl_buffer == (mand_ir_t *) 0)
					cur_sl_buffer = sl;
				else {
					mand_copy_ir(sl, cur_sl_buffer);
					mand_free_ir(sl);
				}
			fld->fd_cur_sl = is_default ?
						d_cur_sl_buffer : cur_sl_buffer;
			flg->fg_cur_sl = 1;
		}
		break;
#endif

	case ASSIGN:
		loadnamepair (fld->fd_assign, AUTH_MAX_DEV_ASSIGN,
		  field, auth_dev_assign, AUTH_DEV_ASSIGN, OT_DEV_ASG,
		  fld->fd_name);
		flg->fg_assign = 1;
		break;
#if SEC_ILB
	case CUR_IL:
		il = ilb_er_to_ir(field);
		if (il != (ilb_ir_t *) 0) {
			if (is_default)
				if (d_cur_il_buffer == (ilb_ir_t *) 0)
					d_cur_il_buffer = il;
				else {
					ilb_copy_ir(il, d_cur_il_buffer);
					ilb_free_ir(il);
				}
			else
				if (cur_il_buffer == (ilb_ir_t *) 0)
					cur_il_buffer = il;
				else {
					ilb_copy_ir(il, cur_il_buffer);
					ilb_free_ir(il);
				}
			fld->fd_cur_il = is_default ?
						d_cur_il_buffer : cur_il_buffer;
			flg->fg_cur_il = 1;
		}
		break;

#endif
	case TYPE:
		loadnamepair (fld->fd_type, AUTH_MAX_DEV_TYPE,
		  field, auth_dev_type, AUTH_DEV_TYPE, OT_DEV_ASG,
		  fld->fd_name);
		flg->fg_type = 1;
		break;
#if SEC_NCAV
	case MAX_NAT_CAVEATS:

		/*
		 * copy the mapped field into the statically allocated buffer
		 * Note that ncav_er_to_ir() returns a pointer to statically
		 * allocated memory (no need to free).
		 */

		ncav = ncav_er_to_ir(field);
		if (ncav != (ncav_ir_t *) 0) {
			if (is_default) {
				if (d_max_ncav_buffer == (ncav_ir_t *) 0) {
					d_max_ncav_buffer = (ncav_ir_t *)
					  malloc(sizeof(ncav_ir_t));
					if (d_max_ncav_buffer == (ncav_ir_t *) 0)
						break;
				}
				memcpy((char *) d_max_ncav_buffer,
				       (char *) ncav,
				       sizeof(ncav_ir_t));
			} else {
				if (max_ncav_buffer == (ncav_ir_t *) 0) {
					max_ncav_buffer = (ncav_ir_t *)
					  malloc(sizeof(ncav_ir_t));
					if (max_ncav_buffer == (ncav_ir_t *) 0)
						break;
				}
				memcpy((char *) max_ncav_buffer,
				       (char *) ncav,
				       sizeof(ncav_ir_t));
			}

			fld->fd_max_nat_caveats = is_default ?
					d_max_ncav_buffer : max_ncav_buffer;
			flg->fg_max_nat_caveats = 1;
		}
		break;

	case MIN_NAT_CAVEATS:

		ncav = ncav_er_to_ir(field);
		if (ncav != (ncav_ir_t *) 0) {
			if (is_default) {
				if (d_min_ncav_buffer == (ncav_ir_t *) 0) {
					d_min_ncav_buffer = (ncav_ir_t *)
					  malloc(sizeof(ncav_ir_t));
					if (d_min_ncav_buffer == (ncav_ir_t *) 0)
						break;
				}
				memcpy((char *) d_min_ncav_buffer,
				       (char *) ncav,
				       sizeof(ncav_ir_t));
			} else {
				if (min_ncav_buffer == (ncav_ir_t *) 0) {
					min_ncav_buffer = (ncav_ir_t *)
					  malloc(sizeof(ncav_ir_t));
					if (min_ncav_buffer == (ncav_ir_t *) 0)
						break;
				}
				memcpy((char *) min_ncav_buffer,
				       (char *) ncav,
				       sizeof(ncav_ir_t));
			}

			fld->fd_min_nat_caveats = is_default ?
					d_min_ncav_buffer : min_ncav_buffer;
			flg->fg_min_nat_caveats = 1;
		}
		break;

	case CUR_NAT_CAVEATS:

		ncav = ncav_er_to_ir(field);
		if (ncav != (ncav_ir_t *) 0) {
			if (is_default) {
				if (d_cur_ncav_buffer == (ncav_ir_t *) 0) {
					d_cur_ncav_buffer = (ncav_ir_t *)
					  malloc(sizeof(ncav_ir_t));
					if (d_cur_ncav_buffer == (ncav_ir_t *) 0)
						break;
				}
				memcpy((char *) d_cur_ncav_buffer,
				       (char *) ncav,
				       sizeof(ncav_ir_t));
			} else {
				if (cur_ncav_buffer == (ncav_ir_t *) 0) {
					cur_ncav_buffer = (ncav_ir_t *)
					  malloc(sizeof(ncav_ir_t));
					if (cur_ncav_buffer == (ncav_ir_t *) 0)
						break;
				}
				memcpy((char *) cur_ncav_buffer,
				       (char *) ncav,
				       sizeof(ncav_ir_t));
			}

			fld->fd_cur_nat_caveats = is_default ?
					d_cur_ncav_buffer : cur_ncav_buffer;
			flg->fg_cur_nat_caveats = 1;
		}
		break;
#endif
	}
	return (ret);
}

static int
store_dv_fields (f, name, fld, flg)
FILE *f;
char *name;
register struct dev_field *fld;
register struct dev_flag *flg;
{
	int error;
	char *s;
	int i;
	int fields = 1;
	char	*namelist;
#if SEC_MAC || SEC_ILB
	char	*level_er;
#endif

	error = (fflush(f) != 0);

	if (!error && (name != (char *) 0))  {
		error = fprintf(f, "%s:", name) == EOF;
		fields = pr_newline(f, fields, &error);
	}

	if (!error && flg->fg_devs) {
		error = fprintf (f, "%s=", AUTH_V_DEVICES) == EOF;
		for (i = 0; s = fld->fd_devs[i]; i++) {
			if (i > 0)
				error = error || putc (',', f) == EOF;
			error = error || fputs (s, f) == EOF;
		}
		error = error || putc (':', f) == EOF;
		fields = pr_newline(f, fields, &error);
	}

	if (!error && flg->fg_users) {
		error = fprintf (f, "%s=", AUTH_V_USERS) == EOF;
		for (i = 0; s = fld->fd_users[i]; i++) {
			if (i > 0)
				error = error || putc (',', f) == EOF;
			error = error || fputs (s, f) == EOF;
		}
		error = fprintf(f, ":\\\n\t:") == EOF;
	}

#if SEC_MAC
	if (!error && flg->fg_max_sl)  {
		level_er = mand_ir_to_er(fld->fd_max_sl);
		if (level_er) {
			error = fprintf(f, "%s=%s:\\\n\t:", AUTH_V_MAX_SL,
					level_er) == EOF;
			fields = pr_newline(f, fields, &error);
		}
		else
			error = 1;
	}

	if (!error && flg->fg_min_sl)  {
		level_er = mand_ir_to_er(fld->fd_min_sl);
		if (level_er) {
			error = fprintf(f, "%s=%s:\\\n\t:", AUTH_V_MIN_SL,
					level_er) == EOF;
			fields = pr_newline(f, fields, &error);
		}
		else
			error = 1;
	}

	if (!error && flg->fg_cur_sl)  {
		level_er = mand_ir_to_er(fld->fd_cur_sl);
		if (level_er) {
			error = fprintf(f, "%s=%s:\\\n\t:", AUTH_V_CUR_SL,
					level_er) == EOF;
			fields = pr_newline(f, fields, &error);
		}
		else
			error = 1;
	}
#endif

	if (!error && flg->fg_assign) {
		namelist = storenamepair(fld->fd_assign, AUTH_MAX_DEV_ASSIGN,
					 auth_dev_assign, AUTH_DEV_ASSIGN);
		error = fprintf(f, "%s=%s:", AUTH_V_ASSIGN, namelist) == EOF;
		free(namelist);
	}
#if SEC_ILB
	if (!error && flg->fg_cur_il)  {
		level_er = ilb_ir_to_er(fld->fd_cur_il);
		if (level_er) {
			error = fprintf(f, "%s=%s:\\\n\t:", AUTH_V_CUR_IL,
					level_er) == EOF;
			fields = pr_newline(f, fields, &error);
		}
		else
			error = 1;
	}
#endif

#if SEC_NCAV

	if (!error && flg->fg_max_nat_caveats)  {
		namelist = ncav_ir_to_er(fld->fd_max_nat_caveats);
		if (namelist) {
			error = fprintf(f, "%s=%s:", AUTH_V_MAX_NAT_CAVEATS,
					namelist) == EOF;
			fields = pr_newline(f, fields, &error);
		}
		else
			error = 1;
	}

	if (!error && flg->fg_min_nat_caveats)  {
		namelist = ncav_ir_to_er(fld->fd_min_nat_caveats);
		if (namelist) {
			error = fprintf(f, "%s=%s:", AUTH_V_MIN_NAT_CAVEATS,
					namelist) == EOF;
			fields = pr_newline(f, fields, &error);
		}
		else
			error = 1;
	}

	if (!error && flg->fg_cur_nat_caveats)  {
		namelist = ncav_ir_to_er(fld->fd_cur_nat_caveats);
		if (namelist) {
			error = fprintf(f, "%s=%s:", AUTH_V_CUR_NAT_CAVEATS,
					namelist) == EOF;
			fields = pr_newline(f, fields, &error);
		}
		else
			error = 1;
	}

#endif

	if (!error && flg->fg_type)  {
		namelist = storenamepair(fld->fd_type, AUTH_MAX_DEV_TYPE,
					 auth_dev_type, AUTH_DEV_TYPE);
		error = fprintf(f, "%s=%s:", AUTH_V_TYPE, namelist) == EOF;
		free(namelist);
		fields = pr_newline(f, fields, &error);
	}

	if (!error)
		error = fprintf(f, "%s:\n", AUTH_CHKENT) == EOF;

	error = (fflush(f) != 0) || error;

	return !error;

}

#endif /*} SEC_BASE */
