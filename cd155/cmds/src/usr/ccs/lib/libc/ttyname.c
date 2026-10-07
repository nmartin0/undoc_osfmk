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
static char rcsid[] = "@(#)$RCSfile: ttyname.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:17 $";
#endif

/*
 * ttyname.c	5.2 (Berkeley) 3/9/86
 */

/*
 * ttyname(f): return "/dev/ttyXX" which the the name of the
 * tty belonging to file f.
 *  NULL if it is not a tty
 */

#define	NULL	0
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/stat.h>
#ifdef _THREAD_SAFE
#include <errno.h>
#endif

static	char	dev[]	= "/dev/";
char	*strcpy();
char	*strcat();

static struct ttymap {
	int tm_rdev;
	char tm_base;
	char tm_count;
} ttymap[] = {
	-1,	'v',	1,
	-1,	'p',	5,
	-1,	'P',	5,
};

static char *
#ifdef _THREAD_SAFE
fastttyname(dev_t rdev, char *ttytemp, int len)
#else
fastttyname(dev_t rdev)
#endif
{
	struct stat stb;
	register int i, n, rmin, rmaj = major(rdev);
	register struct ttymap *tm;
#ifndef _THREAD_SAFE
	static char ttytemp[32] = "/dev/tty??";
#else

	strcpy(ttytemp, "/dev/tty??");
#endif

	for (i = 0; i < (sizeof(ttymap)/sizeof(ttymap[0])); i++) {
		tm = &ttymap[i];
		if (tm->tm_base == '\0')
			continue;
		if (tm->tm_rdev < 0) {
			ttytemp[8] = tm->tm_base;
			ttytemp[9] = '0';
			if (stat(ttytemp, &stb) < 0) {
				tm->tm_base = '\0';
				continue;
			}
			tm->tm_rdev = stb.st_rdev;
			n = tm->tm_count;
			tm->tm_count = 0;
			do {
				tm->tm_count += 16;
				if (--n == 0)
					break;
				ttytemp[8]++;
			} while (stat(ttytemp, &stb) >= 0);
		}
		if (major(tm->tm_rdev) == rmaj) {
			rmin = minor(rdev);
			if (rmin < minor(tm->tm_rdev) ||
			    rmin >= minor(tm->tm_rdev) + tm->tm_count)
				continue;
			rmin -= minor(tm->tm_rdev);
			ttytemp[8] = tm->tm_base+((rmin>>4)&0xf);
			ttytemp[9] = (rmin&0xf) < 10 ?
				     (rmin&0xf) + '0' :
				     (rmin&0xf) - 10 + 'a';
			if (stat(ttytemp, &stb) >= 0 && stb.st_rdev == rdev)
				return(ttytemp);
		}
	}
	return(NULL);
}

#ifdef _THREAD_SAFE
#define	FAIL	-1
#else
#define	FAIL	NULL
#endif

#ifdef _THREAD_SAFE
int
ttyname_r(int f, char *rbuf, int len)
#else
char *
ttyname(int f)
#endif
{
	struct stat fsb;
	struct stat tsb;
	register struct dirent *db;
	register DIR *df;
	char *p;
#ifndef _THREAD_SAFE
	static char rbuf[32];
#else

	if ((rbuf == NULL) || (len < 32)) {
		seterrno(EINVAL);
		return(FAIL);
	}
#endif

	if (isatty(f)==0)
		return(FAIL);
	if (fstat(f, &fsb) < 0)
		return(FAIL);
	if ((fsb.st_mode&S_IFMT) != S_IFCHR)
		return(FAIL);

#ifdef _THREAD_SAFE
	if ((p = fastttyname(fsb.st_rdev, rbuf, len)) != NULL)
		return(0);
#else
	if ((p = fastttyname(fsb.st_rdev)) != NULL)
		return(p);
#endif

	if ((df = opendir(dev)) == NULL)
		return(FAIL);

	while ((db = readdir(df)) != NULL) {
		if (db->d_fileno != fsb.st_ino)
			continue;
		strcpy(rbuf, dev);
		strcat(rbuf, db->d_name);
		if (stat(rbuf, &tsb) < 0)
			continue;
		if (tsb.st_dev == fsb.st_dev && tsb.st_ino == fsb.st_ino) {
			closedir(df);
#ifdef _THREAD_SAFE
			return(0);
#else
			return(rbuf);
#endif
		}
	}
	closedir(df);
#ifdef _THREAD_SAFE
	seterrno(ENOENT);
#endif
	return(FAIL);
}
