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
/* Copyright (c) 1988-90 SecureWare, Inc.
 *   All rights reserved
 *
 * Authentication Database support routines.
 */

#ident "@(#)authcap.c	5.2 15:56:33 8/17/90 SecureWare"
/* Based on:
 *   "@(#)authcap.c	2.10.3.1 17:35:48 1/4/90 SecureWare"
 */

/*LINTLIBRARY*/

#include <sys/secdefines.h>

#if SEC_BASE /*{*/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>

#define AUTH_MAX_CAPBUFSIZ AUTH_CAPBUFSIZ /* maximum size buffers can grow */
#define AUTH_INIT_CAPBUFSIZ 128		  /* initial size of buffers */

/* Default database pointers are kept in this file for packaging. */

static FILE *default_fp;
static long default_filepos;

/*
 * Root of the authentication database and templates for the files
 * containing the descriptions of the system files, commands, and
 * defaults
 */
#define	AUTHCAPDIR	"/tcb/files/auth"
#define	TEMPLATE	"/etc/auth/%s/%s"
#define	FILES		"files"
#define	TTYS		"ttys"
#define	DEFAULT		"default"
#define DEVASG		"devassign"
#if SEC_MAC || SEC_ILB
#define LPS		"lps"
#endif
#define SUBSYS		"subsystems"

/*
 * authcap - routines for dealing with the authentication capability data base
 *
 * BUG:		Should use a "last" pointer in abuf, so that searching
 *		for capabilities alphabetically would not be a n**2/2
 *		process when large numbers of capabilities are given.
 *
 * Essentially all the work here is scanning and decoding escapes
 * in string capabilities.
 */

static	char *abuf;
static char *default_name = "system";

/* pointers to heap space buffers allocated during processing */

static char *user_buf = (char *) 0;
static int  user_len = 0;
static char *file_buf = (char *) 0;
static int   file_len = 0;
static char *tty_buf = (char *) 0;
static int   tty_len = 0;
static char *default_buf = (char *) 0;
static int   default_len = 0;
static char *devasg_buf = (char *) 0;
static int   devasg_len = 0;
#if SEC_MAC
static char *lp_buf = (char *) 0;
static int   lp_len = 0;
#endif

static int match_name();
static int match_file();
static char	*askip();
static char	*adecode();
static	int agetent();
long adecodenum();

extern char *malloc();
extern char *realloc();
extern char *strrchr();
extern char *strchr();
extern char *strcpy();
extern char *strcat();
extern long lseek();



/*
 * Get an entry for user name from the user's authentication file.
 */
int
agetuser(user)
	char *user;
{
	int ent_ret;
	FILE *fp;

	open_auth_file(user, OT_PRPWD, &fp);
	if (fp == (FILE *) 0)
		ent_ret = -1;
	else {
		ent_ret = agetent(&user_buf, &user_len, user, OT_PRPWD,
				  (long *) 0, fp);
		(void) fclose(fp);
	}

	return ent_ret;
}


/*
 * Get an entry for file name.
 */
char *
agetfile(filepos, fp, nam)
	long *filepos;
	FILE *fp;
	char *nam;
{
	if (agetent(&file_buf, &file_len, nam, OT_FILE_CNTL, filepos, fp) == 1)
		return file_buf;
	return (char *) 0;
}


/*
 * Get an entry for tty name.  Assumes tty resides in /dev .
 */
int
agettty(filepos, fp, name)
	long *filepos;
	FILE *fp;
	char *name;
{
	return agetent(&tty_buf, &tty_len, name, OT_TERM_CNTL, filepos, fp);
}


/*
 * Get an entry for system default.
 */
int
agetdefault()
{
	if (default_fp == (FILE *) 0)
		setprdfent();

	return agetent(&default_buf, &default_len, (char *) 0, OT_DFLT_CNTL,
			&default_filepos, default_fp);
}


/*
 * Get an entry for system default, and return the buffer with the entry
 */
char *
agetdefault_buf()
{
	if (agetdefault() == 1)
		return (default_buf);
	return ((char *) 0);
}


/*
 * Get an entry for device name.
 */
char *
agetdvag(filepos, fp, nam)
	long *filepos;
	FILE *fp;
	char *nam;
{
	if (agetent(&devasg_buf, &devasg_len, nam, OT_DEV_ASG,
			filepos, fp) == 1)
		return (devasg_buf);

	return ((char *) 0);
}


#if SEC_MAC
/*
 * Get an entry for lp name.
 */
int
agetlp(filepos, fp, name)
	long *filepos;
	FILE *fp;
	char *name;
{
	return agetent(&lp_buf, &lp_len, name, OT_LP_CNTL, filepos, fp);
}
#endif


/*
 * Get a database entry from one of the authcap-format files.
 * Routine understands escapes and removes newlines
 * Returns -1 if the file it needs does not exist, 0 if the entry cannot be
 * found and 1 if the entry was found.
 */
static int
agetent(bpp, blenp, name, type, pos, fp)
	char **bpp;
	int  *blenp;
	char *name;
	int type;
	long *pos;
	FILE *fp;
{
	register char *bp = *bpp;
	register int cnt = 0;
	register char *cp;
	register long entry_end;
	register int buf_length = *blenp;
	int c;
	int found_entry;
	char ibuf[BUFSIZ];
	int new_size;
	unsigned long cp_offset;
	char *nbp;


	if (pos == (long *) 0)
		entry_end = 0L;
	else
		entry_end = *pos;

	if (fp == (FILE *) 0)
		return (-1);

	if (ftell (fp) != entry_end)
		fseek (fp, entry_end, 0);

	if (bp == (char *) 0) {
		bp = malloc (AUTH_INIT_CAPBUFSIZ);
		if (bp == (char *) 0) {
			entry_end = lseek (fileno(fp), 0L, 2);
			return (0);
		}
		*bpp = bp;
		*blenp = AUTH_INIT_CAPBUFSIZ;
		buf_length = AUTH_INIT_CAPBUFSIZ;
	}

	abuf = bp;

	for (;;)  {

		cp = bp;
		*cp = '\0';

		/* gather up the entry into the buffer. */
		for (;;) {
			if (fgets (ibuf, sizeof (ibuf), fp) == NULL)
				return (0);
			cnt = strlen(ibuf) ;
			if (cnt <= 0)
				return(0) ;
			entry_end += cnt;
			if (ibuf[0] == '#')
				continue;
			cnt -= 1;	/* drop newline */
			c = ibuf[cnt - 1];
			if (c == '\\')
				cnt--;

			/* grow the buffer if it's too big, up to max allowed */
			new_size = cnt + strlen (bp) + 1;
			cp_offset = (unsigned long) cp - (unsigned long) bp;
			if (new_size > AUTH_MAX_CAPBUFSIZ) {
				entry_end = lseek (fileno(fp), 0L, 2);
				break;
			} else if (new_size > buf_length) {
				nbp = realloc (bp, new_size);
				if (nbp == (char *) 0) {
					entry_end = lseek (fileno(fp), 0L, 2);
					break;
				}
				/* reset to point to the new buffer. */
				*bpp = nbp;
				bp = nbp;
				abuf = bp;
				buf_length = new_size;
				*blenp = buf_length;
				cp = (char *) ((unsigned long) bp + cp_offset);
			}
			strncpy (cp, ibuf, cnt);
			cp += cnt;
			*cp = '\0';
			
			if (c != '\\')
				break;
		}

		/*
		 * The real work for the match.
		 */
		if ( name == (char *) 0 ||
		    (type == OT_PRPWD && match_name(name)) ||
		    (type == OT_FILE_CNTL && match_file(name)) ||
		    (type == OT_TERM_CNTL && match_name(name)) ||
#if SEC_MAC
		    (type == OT_LP_CNTL &&   match_name (name))  ||
#endif
		    (type == OT_DEV_ASG &&   match_name (name))) {

			/*
			 * As a sanity check when reading an entry,
			 * make sure the entry has a AUTH_CHKENT flag in
			 * it.  We place AUTH_CHKENT at the end when
			 * creating the entry s.t. when reading the
			 * entry later, we know the entry is complete.
			 */
			if (agetflag(AUTH_CHKENT) == 1)  {
				found_entry = 1;
				if (pos != (long *) 0)
					*pos = entry_end;
			}
			else  {
				if (pos != (long *) 0)
					*pos = lseek (fileno(fp), 0L, 2);
				found_entry = 0;
			}

			return found_entry;
		}
	}
}


/*
 * Match_name deals with name matching.  The first field of the authcap
 * entry is a sequence of names separated by |'s, so we compare
 * against each such name.  The normal : terminator after the last
 * name (before the first field) stops us.
 */
static int
match_name(np)
	char *np;
{
	register char *Np, *Bp;

	Bp = abuf;
	if (*Bp == '#')
		return(0);

	for (;;) {
		for (Np = np; *Np && *Bp == *Np; Bp++, Np++)
			continue;
		if (*Np == 0 && (*Bp == '|' || *Bp == ':' || *Bp == 0))
			return (1);
		while (*Bp && *Bp != ':' && *Bp != '|')
			Bp++;
		if (*Bp == 0 || *Bp == ':')
			return (0);
		Bp++;
	}
}


/*
 * A name will match its exact equivalent in the database or it will
 * match a last wild card component.  Thus /etc/abc matches database
 * entries /etc/abc and /etc/* .  Note that the wild card entries must
 * appear in the database after all the real names for which it is
 * equivalent.
 */
static int
match_file(np)
	register char *np;
{
	register char *Np, *Bp; 
	register int match;
	register char *wild_card;
	register char *last_slash;

	Bp = abuf;
	if (*Bp == '#')
		return(0);

	/* find the prefix match */
	for (Np = np; *Np && *Bp == *Np; Bp++, Np++)
		;

	/* check the case that the entire name was matched */
	if (*Np == '\0')
		if (*Bp == '|' || *Bp == ':')
			return (1);

	/* check the wildcard case */
	if (*Bp == '*' && *Np != '\0' && strchr (Np, '/') == (char *) 0)
		return (1);

	/* neither matched, return failure */
	return (0);
}

/*
 * Skip to the next field.  Notice that this is very dumb, not
 * knowing about \: escapes or any such.  If necessary, :'s can be put
 * into the authcap file in octal.
 */
static char *
askip(bp)
	register char *bp;
{

	while (*bp && *bp != ':')
		bp++;
	if (*bp == ':')
		bp++;
	return bp;
}

/*
 * Return a time value.
 * If the value is "now", the first time the database is read the time is
 * taken from the system time.
 */

int
agttime(id,val)
	char *id;
	int *val;
{
	register char *bp = abuf;
	register int idlen;

	for (;;) {
		bp = askip(bp);
		if (*bp == '\0')
			return (-1);
		idlen = strlen(id);
		if (strncmp(id, bp, idlen) != 0)
			continue;
		bp += idlen;
		if (*bp == '@')
			return(-1);
		if (*bp != '#')
			continue;
		bp++;
		if (strncmp(bp, AUTH_TIME_NOW, strlen(AUTH_TIME_NOW)) == 0)
			*val=time((long *) 0);
		else
			*val=adecodenum(bp);
		return(0);
	}
}


/*
 * Return the (numeric) option id in val.
 * Note that this may be -1.
 * Returns 0 if field is present, else -1.
 * Numeric options look like
 *	mt#5
 * i.e. the option string is separated from the numeric value by
 * a # character.  If the option is not found we return -1.
 * Note that we handle octal numbers beginning with 0 and hex
 * digits beginning with 0x.
 */
int
agtnum(id,val)
	char *id;
	long *val;
{
	register char *bp = abuf;
	register int idlen;

	for (;;) {
		bp = askip(bp);
		if (*bp == '\0')
			return (-1);
		idlen = strlen(id);
		if (strncmp(id, bp, idlen) != 0)
			continue;
		bp += idlen;
		if (*bp == '@')
			return(-1);
		if (*bp != '#')
			continue;
		bp++;
		*val=adecodenum(bp);
		return(0);
	}
}

/*
 * Decode a number according to C conventions.
 */
long
adecodenum(bp)
	register char	*bp;
{
	register int	base, sign = 1;
	register long	i ;

	if (*bp == '-') {
		sign = -1;
		bp++;
	}
	if (*bp == '0') {
		bp++;
		if (*bp == 'x' || *bp == 'X')  {
			base = 16;
			bp++;
		} else
			base = 8;
	} else
		base = 10;

	i = 0;
	while (isxdigit(*bp))
		if (isdigit(*bp)) {
			i *= base;
			i += *bp++ - '0';
		} else {
			i *= base;
			if (isupper(*bp))
				i += *bp - 'A' + 10;
			else
				i += *bp - 'a' + 10;
			bp++;
		}

	return sign * i;
}

/*
 * Get a user ID valued field.  Look up the user identifier in
 * the password database (/etc/passwd) and return the user ID
 */

agetuid(id)
	char *id;
{
	register char *bp = abuf;
	register char *bpe;
	register int idlen;
	int user_id;

	check_auth_parameters();

	for (;;) {
		bp = askip(bp);
		if (*bp == '\0')
			return (-1);
		idlen = strlen(id);
		if (strncmp(id, bp, idlen) != 0)
			continue;
		bp += idlen;
		if (*bp != '=')
			continue;
		bp++;
		bpe = strchr (bp, ':');
		if (bpe == (char *) 0)
			continue;
		*bpe = '\0';
		user_id = pw_nametoid (bp);
		*bpe = ':';
		return (user_id);
	}
}

/*
 * Get a group ID valued field.  Look up the group identifier in
 * the group database (/etc/group) and return the group ID
 */

agetgid(id)
	char *id;
{
	register char *bp = abuf;
	register char *bpe;
	register int idlen;
	int group_id;

	check_auth_parameters();

	for (;;) {
		bp = askip(bp);
		if (*bp == '\0')
			return (-1);
		idlen = strlen(id);
		if (strncmp(id, bp, idlen) != 0)
			continue;
		bp += idlen;
		if (*bp != '=')
			continue;
		bp++;
		bpe = strchr (bp, ':');
		if (bpe == (char *) 0)
			continue;
		*bpe = '\0';
		group_id = gr_nametoid (bp);
		*bpe = ':';
		return (group_id);
	}
}

/*
 * Handle a flag option.
 * Flag options are given "naked", i.e. followed by a : or the end
 * of the buffer.  Return 1 if we find the option, 0 if it is present
 * and designated false, or -1 if it is not given.
 */
int
agetflag(id)
	char *id;
{
	register char *bp = abuf;
	register int idlen;

	check_auth_parameters();

	for (;;) {
		bp = askip(bp);
		if (!*bp)
			return (-1);
		idlen = strlen(id);
		if (strncmp(id, bp, idlen) == 0)  {
			bp += idlen;
			if (!*bp || *bp == ':')
				return (1);
			else if (*bp == '@')
				return(0);
		}
	}
}

/*
 * Get a string valued option.
 * These are given as
 *	cp=ps,whodo
 * Much decoding is done on the strings, and the strings are
 * placed in area, which is a ref parameter which is updated.
 * No checking on area overflow.
 */
char *
agetstr(id, area)
	char *id;
	char **area;
{
	register char *bp = abuf;
	register int idlen;

	check_auth_parameters();

	for (;;) {
		bp = askip(bp);
		if (!*bp)
			return (char *) (0);
		idlen = strlen(id);
		if (strncmp(id, bp, idlen) == 0)  {
			bp += idlen;
			if (*bp == '@')
				return((char *) 0);
			if (*bp != '=')
				continue;
			bp++;
			return (adecode(bp, area));
		}
	}
}

/*
 * Adecode does the grunt work to decode the
 * string capability escapes.
 */
static char *
adecode(str, area)
	register char *str;
	char **area;
{
	register char *cp;
	register int c;
	register char *dp;
	int i;

	cp = *area;
	while ((c = *str++) && c != ':') {
		switch (c) {

		case '^':
			c = *str++ & 037;
			break;

		case '\\':
			dp = "E\033^^\\\\::n\nr\rt\tb\bf\f";
			c = *str++;
nextc:
			if (*dp++ == c) {
				c = *dp++;
				break;
			}
			dp++;
			if (*dp)
				goto nextc;
			if (isdigit(c)) {
				c -= '0', i = 2;
				do
					c <<= 3, c |= *str++ - '0';
				while (--i && isdigit(*str));
			}
			break;
		}
		*cp++ = c;
	}
	*cp++ = 0;
	str = *area;
	*area = cp;
	return (str);
}


/* parse a comma-separated string list */

char **
agetstrlist (field)
char *field;
{
	register char *cp = field;
	char **ret;
	int count;
	int i;

	if (*cp == '\0')
		return (char **) 0;
	/* count the number of fields */
	for (count = 2; *cp && (cp = strchr (cp, ',')); cp++)
		count++;
	ret = (char **) calloc (count, sizeof (char *));
	/* set the values of the array */
	cp = field;
	for (i = 0; i < count - 1; i++) {
		ret[i] = cp;
		cp = strchr (cp, ',');
		if (cp)
			*cp++ = '\0';
	}
	ret[i] = (char *) 0;
	return (ret);
}


/*
 * Return the file descriptor for the Authentication database file
 * containing the entry.
 */
void
open_auth_file(name, type, fpp)
	char *name;
	int type;
	FILE **fpp;
{
	register char *pathname;
	register FILE *fp = (FILE *) 0;
	char *tpath, *opath;
	int i;
	struct stat sb;

	pathname = find_auth_file(name, type);
	make_transition_files(pathname, &tpath, &opath);

	if (pathname == (char *) 0)
		fp = (FILE *) 0;
	else  {
		int changed_i = 0;
		time_t orig_mtime = 0;

		for (i = 0; i < AUTH_LOCK_ATTEMPTS; i++) {

			/*
			 * If file exists, check conditions for forced removal
			 */

			if (stat(tpath, &sb) == 0) {
				if (i - changed_i == AUTH_LOCK_ATTEMPTS / 2 &&
				    sb.st_mtime == orig_mtime)
					unlink(tpath);
				/*
				 * If original time not set or changed, reset it
				 */

				if (!orig_mtime || sb.st_mtime != orig_mtime) {
					orig_mtime = sb.st_mtime;
					changed_i = i;
				}
				(void) sleep (AUTH_RETRY_DELAY);
			}
			else {
				fp = fopen(pathname, "r");
				if (fp != (FILE *) 0)
					break;
			}
		}

		/*
		 * To be on the safe side, make sure the database files
		 * are closed across an exec().  This is a safety valve --
		 * such files should be explicitly closed with endpr??ent().
		 */
		if (fp != (FILE *) 0)
			(void) fcntl(fileno (fp), F_SETFD, 1);
		free(pathname);
		free(tpath);
		free(opath);
	}

	*fpp = fp;
}


/*
 * Locate the file associated with the entry name and the database type.
 * return the malloc'd string of the file name.  Depending on the database,
 * the file will contain just the single entry or it will contain the entire
 * database.
 */
char *
find_auth_file(name, type)
	register char *name;
	int type;
{
	register char *pathname;
	register int pathlen;
	char subdirname[sizeof("a/")];

	switch(type)  {
		case OT_PRPWD:
			*subdirname = *name;
			subdirname[1] = '/';
			subdirname[2] = '\0';
			pathlen = strlen(AUTHCAPDIR) + 3 + strlen(subdirname) +
				  strlen(name);
			pathname = malloc(pathlen);

			if (pathname != (char *) 0) {
				(void) strcpy(pathname, AUTHCAPDIR);
				(void) strcat(pathname, "/");
				(void) strcat(pathname, subdirname);
				(void) strcat(pathname, name);
			}
			break;

		case OT_FILE_CNTL:
			pathlen = strlen(TEMPLATE) +
				  strlen(default_name) +
				  strlen(FILES) + 1;
			pathname = malloc(pathlen);

			if (pathname != (char *) 0) 
				(void) sprintf(pathname, TEMPLATE,
					       default_name, FILES);
			break;
		
		case OT_SUBSYS:
			pathlen = sizeof(AUTH_SUBSYSDIR) + 1 + strlen(name);
			pathname = malloc(pathlen);

			if (pathname != (char *) 0)
				(void) sprintf(pathname, "%s/%s",
						AUTH_SUBSYSDIR, name);
			break;

		case OT_TERM_CNTL:
			pathlen = strlen(TEMPLATE) +
				  strlen(default_name) +
				  strlen(TTYS) + 1;
			pathname = malloc(pathlen);

			if (pathname != (char *) 0)
				(void) sprintf(pathname, TEMPLATE,
					       default_name, TTYS);
			break;

		case OT_DFLT_CNTL:
			pathlen = strlen(TEMPLATE) +
				  strlen(default_name) +
				  strlen(DEFAULT) + 1;
			pathname = malloc(pathlen);

			if (pathname != (char *) 0)
				(void) sprintf(pathname, TEMPLATE,
					       default_name,
					       DEFAULT);
			break;
					
		case OT_DEV_ASG:
			pathlen = strlen(TEMPLATE) +
				  strlen(default_name) +
				  strlen(DEVASG) + 1;
			pathname = malloc(pathlen);

			if (pathname != (char *) 0)
				(void) sprintf(pathname, TEMPLATE,
					       default_name,
					       DEVASG);
			break;

#if SEC_MAC
		case OT_LP_CNTL:
			pathlen = strlen(TEMPLATE) +
				  strlen(default_name) +
				  strlen(LPS) + 1;
			pathname = malloc(pathlen);

			if (pathname != (char *) 0)
				(void) sprintf(pathname, TEMPLATE,
					       default_name, LPS);
			break;
#endif

		default:
			pathname = (char *) 0;
	}

	return pathname;
}


/* end of processing for the databases.
 * free the static buffers, and the "ac" buf if allocated.
 */

void
end_authcap (type)
int type;
{
	switch (type) {
	case OT_PRPWD:
		if (user_buf != (char *) 0) {
			free (user_buf);
			user_buf = (char *) 0;
			user_len = 0;
		}
		break;
	case OT_FILE_CNTL:
		if (file_buf != (char *) 0) {
			free (file_buf);
			file_buf = (char *) 0;
			file_len = 0;
		}
		break;
	case OT_TERM_CNTL:
		if (tty_buf != (char *) 0) {
			free (tty_buf);
			tty_buf = (char *) 0;
			tty_len = 0;
		}
		break;
	case OT_DFLT_CNTL:
		if (default_buf != (char *) 0) {
			free (default_buf);
			default_buf = (char *) 0;
			default_len = 0;
		}
		break;
	case OT_DEV_ASG:
		if (devasg_buf != (char *) 0) {
			free (devasg_buf);
			devasg_buf = (char *) 0;
			devasg_len = 0;
		}
		break;
#if SEC_MAC
	case OT_LP_CNTL:
		if (lp_buf != (char *) 0) {
			free (lp_buf);
			lp_buf = (char *) 0;
			lp_len = 0;
		}
		break;
#endif
	}
}

/*
 * the default database routines are included here because otherwise
 * the entire Authentication database support software would be brought
 * in every time any user referenced any of the databases.
 */

/* This structure is visible to getprdfent.c for packaging reasons */
struct pr_default *pr_default = (struct pr_default *) 0;

/*
 * Close the file(s) related the to the System Default database.
 */
void
endprdfent()
{
	check_auth_parameters();

	if (default_fp != (FILE *) 0)  {
		(void) fclose(default_fp);
		default_fp = (FILE *) 0;
	}
	default_filepos = 0L;
	end_authcap (OT_DFLT_CNTL);
}


/*
 * Reset the position of the System Default database so that the
 * next time getprdfent() is invoked, it will return the first entry
 * in the database.
 */
void
setprdfent()
{
	static time_t modify_time;
	struct stat sb;
	char *filename;
	int ret;

	if (default_fp == (FILE *) 0) {
		open_auth_file((char *) 0, OT_DFLT_CNTL, &default_fp);
		if (default_fp != (FILE *) 0) {
			fstat (fileno(default_fp), &sb);
			modify_time = sb.st_mtime;
		}
	} else {
		filename = find_auth_file ((char *) 0, OT_DFLT_CNTL);
		ret = stat (filename, &sb);
		if (ret != 0 || sb.st_mtime > modify_time) {
			(void) fclose (default_fp);
			open_auth_file((char *) 0, OT_DFLT_CNTL, &default_fp);
			if (default_fp != (FILE *) 0) {
				fstat (fileno(default_fp), &sb);
				modify_time = sb.st_mtime;
			}
		}
		free (filename);
	}
	default_filepos = 0L;
	if (pr_default == (struct pr_default *) 0) {
		pr_default = (struct pr_default *)
		  malloc (sizeof (*pr_default));
		if (pr_default == (struct pr_default *) 0) {
			endprdfent();
		}
	}
}

/*
 * When writing the default database, need to resynchronize with the
 * current version of the database in case it has been changed since
 * the last time the program has accessed it.
 */

reset_default(pathname)
char *pathname;
{
	if (default_fp != (FILE *) 0)
		(void) fclose (default_fp);
	default_fp = fopen (pathname, "r");
	default_filepos = 0L;
}
#endif /*} SEC_BASE */
