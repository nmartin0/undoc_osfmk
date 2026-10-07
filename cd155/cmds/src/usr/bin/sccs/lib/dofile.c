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
static char rcsid[] = "@(#)$RCSfile: dofile.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:38:11 $";
#endif
/*
 * COMPONENT_NAME: CMDSCCS      Source Code Control System (sccs)
 *
 * FUNCTIONS: do_file
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
 */
/*   dofile.c 1.6 com/cmd/sccs/lib/comobj,3.1,9013 11/8/89 14:24:39"; */


# include	"defines.h"
#ifdef PDA
#include	<sys/types.h>
#include 	<sys/param.h>
#include	<dirent.h>
#else
# include	<dirent.h>
#endif

int	nfiles;
char	had_dir;
char	had_standinp;


do_file(p,func)
register char *p;
int (*func)();
{
	extern char *Ffile;
	char str[FILESIZE];
	char ibuf[FILESIZE];
#ifdef PDA
	DIR *dirdat;
	struct dirent *dirent;
#else
	char	dbuf[BUFSIZ];
	FILE *iop;
	struct dirent dir[2];
#endif

	if (p[0] == '-') {
		had_standinp = 1;
		while (gets(ibuf) != NULL) {
			if (sccsfile(ibuf)) {
				Ffile = ibuf;
				(*func)(ibuf);
				nfiles++;
			}
		}
	}
	else if (exists(p) && (Statbuf.st_mode & S_IFMT) == S_IFDIR) {
		had_dir = 1;
		Ffile = p;
#ifdef PDA
		if ((dirdat = opendir(p)) == NULL)
			return;
		readdir(dirdat);			/* skip "." */
		readdir(dirdat);			/* skip ".." */
		do {
			dirent = readdir(dirdat) ;
			if (dirent != NULL) {
				sprintf(str,"%s/%s",p,dirent->d_name);
				if(sccsfile(str)) {
					Ffile = str;
					(*func)(str);
					nfiles++;
				}
			}
		} while (dirent != NULL);
		closedir(dirdat);
#else
		if((iop = fopen(p,"r")) == NULL)
			return;
		setbuf(iop,dbuf);
		dir[1].d_ino = 0;
		fread((char *)dir,sizeof(dir[0]),1,iop);   /* skip "."  */
		fread((char *)dir,sizeof(dir[0]),1,iop);   /* skip ".."  */
		while(fread((char *)dir,sizeof(dir[0]),1,iop) == 1) {
			if(dir[0].d_ino == 0) continue;
			sprintf(str,"%s/%s",p,dir[0].d_name);
			if(sccsfile(str)) {
				Ffile = str;
				(*func)(str);
				nfiles++;
			}
		}
		fclose(iop);
#endif
	}
	else {
		Ffile = p;
		(*func)(p);
		nfiles++;
	}
}
