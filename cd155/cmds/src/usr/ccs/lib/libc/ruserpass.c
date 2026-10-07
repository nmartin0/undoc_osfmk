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
static char rcsid[] = "@(#)$RCSfile: ruserpass.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:55 $";
#endif
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * ruserpass.c	5.5 (Berkeley) 6/27/88
 */


#include <stdio.h>
#include <utmp.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

char	*renvlook(), *malloc(), *index(), *getenv(), *getpass(), *getlogin();
struct	utmp *getutmp();
static	FILE *cfile;
static	renv(), rnetrc(), token();

ruserpass(host, aname, apass)
	char *host, **aname, **apass;
{

	renv(host, aname, apass);
	if (*aname == 0 || *apass == 0)
		rnetrc(host, aname, apass);
	if (*aname == 0) {
		char *myname = getlogin();
		*aname = malloc(16);
		printf("Name (%s:%s): ", host, myname);
		fflush(stdout);
		if (read(2, *aname, (unsigned)16) <= 0)
			exit(1);
		if ((*aname)[0] == '\n')
			*aname = myname;
		else
			if (index(*aname, '\n'))
				*index(*aname, '\n') = 0;
	}
	if (*aname && *apass == 0) {
		printf("Password (%s:%s): ", host, *aname);
		fflush(stdout);
		*apass = getpass("");
	}
}

static
renv(host, aname, apass)
	char *host, **aname, **apass;
{
	register char *cp;
	char *stemp, fgetlogin, *comma;

	cp = renvlook(host);
	if (cp == NULL)
		return;
	if (!isalpha(cp[0]))
		return;
	comma = index(cp, ',');
	if (comma == 0)
		return;
	if (*aname == 0) {
		*aname = malloc(comma - cp + 1);
		strncpy(*aname, cp, (size_t)(comma - cp));
	} else
		if (strncmp(*aname, cp, (size_t)(comma - cp)))
			return;
	comma++;
	cp = malloc(strlen(comma)+1);
	strcpy(cp, comma);
	*apass = malloc(16);
	mkpwclear(cp, host[0], *apass);
}

static
char *
renvlook(host)
	char *host;
{
	register char *cp, **env;
	extern char **environ;

	env = environ;
	for (env = environ; *env != NULL; env++)
		if (!strncmp(*env, "MACH", (size_t)4)) {
			cp = index(*env, '=');
			if (cp == 0)
				continue;
			if (strncmp(*env+4, host, (size_t)(cp-(*env+4))))
				continue;
			return (cp+1);
		}
	return (NULL);
}

#define	DEFAULT	1
#define	LOGIN	2
#define	PASSWD	3
#define	NOTIFY	4
#define	WRITE	5
#define	YES	6
#define	NO	7
#define	COMMAND	8
#define	FORCE	9
#define	ID	10
#define	MACHINE	11

static char tokval[100];

static struct toktab {
	char *tokstr;
	int tval;
} toktab[]= {
	"default",	DEFAULT,
	"login",	LOGIN,
	"password",	PASSWD,
	"notify",	NOTIFY,
	"write",	WRITE,
	"yes",		YES,
	"y",		YES,
	"no",		NO,
	"n",		NO,
	"command",	COMMAND,
	"force",	FORCE,
	"machine",	MACHINE,
	0,		0
};

static
rnetrc(host, aname, apass)
	char *host, **aname, **apass;
{
	char *hdir, buf[BUFSIZ];
	int t;
	struct stat stb;
	extern int errno;

	hdir = getenv("HOME");
	if (hdir == NULL)
		hdir = ".";
	(void)sprintf(buf, "%s/.netrc", hdir);
	cfile = fopen(buf, "r");
	if (cfile == NULL) {
		if (errno != ENOENT)
			perror(buf);
		return;
	}
next:
	while ((t = token())) switch(t) {

	case DEFAULT:
		(void) token();
		continue;

	case MACHINE:
		if (token() != ID || strcasecmp(host, tokval))
			continue;
		while ((t = token()) && t != MACHINE) switch(t) {

		case LOGIN:
			if (token())
				if (*aname == 0) { 
					*aname = malloc(strlen(tokval) + 1);
					strcpy(*aname, tokval);
				} else {
					if (strcmp(*aname, tokval))
						goto next;
				}
			break;
		case PASSWD:
			if (fstat(fileno(cfile), &stb) >= 0
			    && (stb.st_mode & 077) != 0) {
	fprintf(stderr, "Error - .netrc file not correct mode.\n");
	fprintf(stderr, "Remove password or correct mode.\n");
				exit(1);
			}
			if (token() && *apass == 0) {
				*apass = malloc(strlen(tokval) + 1);
				strcpy(*apass, tokval);
			}
			break;
		case COMMAND:
		case NOTIFY:
		case WRITE:
		case FORCE:
			(void) token();
			break;
		default:
	fprintf(stderr, "Unknown .netrc option %s\n", tokval);
			break;
		}
		goto done;
	}
done:
	fclose(cfile);
}

static
token()
{
	char *cp;
	int c;
	struct toktab *t;

	if (feof(cfile))
		return (0);
	while ((c = getc(cfile)) != EOF &&
	    (c == '\n' || c == '\t' || c == ' ' || c == ','))
		continue;
	if (c == EOF)
		return (0);
	cp = tokval;
	if (c == '"') {
		while ((c = getc(cfile)) != EOF && c != '"') {
			if (c == '\\')
				c = getc(cfile);
			*cp++ = c;
		}
	} else {
		*cp++ = c;
		while ((c = getc(cfile)) != EOF
		    && c != '\n' && c != '\t' && c != ' ' && c != ',') {
			if (c == '\\')
				c = getc(cfile);
			*cp++ = c;
		}
	}
	*cp = 0;
	if (tokval[0] == 0)
		return (0);
	for (t = toktab; t->tokstr; t++)
		if (!strcmp(t->tokstr, tokval))
			return (t->tval);
	return (ID);
}
/* rest is nbs.c stolen from berknet */

/*
 * The contents of this portion of this file have been replaced with
 * dummy routines.  Replace with the real thing if you have access to it.
 */
static
char *nbsencrypt(str, key, result)
char *str, *key, *result;
{
	return(result+strlen(key)+strlen(str));
}

static
char *nbsdecrypt(cpt, key, result)
char *cpt, *key, *result;
{
	return(result+strlen(key)+strlen(cpt));
}

/*
 * End of dummy routines
 */
/*
	getutmp()
	return a pointer to the system utmp structure associated with
	terminal sttyname, e.g. "/dev/tty3"
	Is version independent-- will work on v6 systems
	return NULL if error
*/
static
struct utmp *getutmp(sttyname)
char *sttyname;
{
	static struct utmp utmpstr;
	FILE *fdutmp;

	if(sttyname == NULL || sttyname[0] == 0)return(NULL);

	fdutmp = fopen("/etc/utmp","r");
	if(fdutmp == NULL)return(NULL);

	while(fread((void *)&utmpstr, (size_t)1, (size_t)sizeof(utmpstr), fdutmp) == sizeof utmpstr)
		if(strcmp(utmpstr.ut_line,sttyname+5) == 0){
			fclose(fdutmp);
			return(&utmpstr);
		}
	fclose(fdutmp);
	return(NULL);
}

static
sreverse(sto, sfrom)
	register char *sto, *sfrom;
{
	register int i;

	i = strlen(sfrom);
	while (i >= 0)
		*sto++ = sfrom[i--];
}

static
char *mkenvkey(mch)
	char mch;
{
	static char skey[40];
	register struct utmp *putmp;
	char stemp[40], stemp1[40], sttyname[30];
	register char *sk,*p;

	if (isatty(2))
		strcpy(sttyname,ttyname(2));
	else if (isatty(0))
		strcpy(sttyname,ttyname(0));
	else if (isatty(1))
		strcpy(sttyname,ttyname(1));
	else
		return (NULL);
	putmp = getutmp(sttyname);
	if (putmp == NULL)
		return (NULL);
	sk = skey;
	p = putmp->ut_line;
	while (*p)
		*sk++ = *p++;
	*sk++ = mch;
	(void)sprintf(stemp, "%ld", putmp->ut_time);
	sreverse(stemp1, stemp);
	p = stemp1;
	while (*p)
		*sk++ = *p++;
	*sk = 0;
	return (skey);
}

mkpwunclear(spasswd,mch,sencpasswd)
	char mch, *spasswd, *sencpasswd;
{
	register char *skey;

	if (spasswd[0] == 0) {
		sencpasswd[0] = 0;
		return;
	}
	skey = mkenvkey(mch);
	if (skey == NULL) {
		fprintf(stderr, "Can't make key\n");
		exit(1);
	}
	nbsencrypt(spasswd, skey, sencpasswd);
}

mkpwclear(sencpasswd,mch,spasswd)
	char mch, *spasswd, *sencpasswd;
{
	register char *skey;

	if (sencpasswd[0] == 0) {
		spasswd[0] = 0;
		return;
	}
	skey = mkenvkey(mch);
	if (skey == NULL) {
		fprintf(stderr, "Can't make key\n");
		exit(1);
	}
	nbsdecrypt(sencpasswd, skey, spasswd);
}
