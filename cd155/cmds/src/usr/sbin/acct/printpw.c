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
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: printpw.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:21 $";
#endif

/*
 * Print the password entries. Access to the password data base
 * is through the getpwent() routine.
 * So yellow pages or secure systems must have a modifies getpwent()
 * routine in libc.
 */


#include <stdio.h>
#include <pwd.h>

#include <locale.h>
#include "acct_msg.h"
nl_catd catd;
#define MSGSTR(Num, Str) catgets(catd, MS_ACCT, Num, Str)

main(argc, argv)
int argc;
char *argv[];
{
	char c;
	struct passwd *pw;
	int uflg = 0;	/* flag for uid entry */
	int gflg = 0;	/* flag for gid entry */
	int cflg = 0;	/* flag for comment entry */
	int dflg = 0;	/* flag for directory entry */
	int sflg = 0;	/* flag for shell entry */
	int aflg = 0;	/* flag for all entries */

	(void) setlocale (LC_ALL,"");
	catd = catopen(MF_ACCT,0);

	while((c = getopt(argc, argv, "acdgpsu")) != EOF)
        switch(c) {
                case 'a':
                        aflg++;
                        continue;
                case 'c':
                        cflg++;
                        continue;
                case 'd':
                        dflg++;
                        continue;
                case 'g':
                        gflg++;
                        continue;
                case 's':
                        sflg++;
                        continue;
                case 'u':
                        uflg++;
                        continue;
		case '?':
			fprintf(stderr, 
			    MSGSTR(PWUSAGE, "Usage: %s [-acdgsu]\n"), argv[0]);
			exit(1);
        }

	/* rewind password database */
	(void)setpwent();

	while ((pw = getpwent()) != (struct passwd *)0) {
		printf("%s", pw->pw_name);

		if (uflg || aflg)
			printf(":%d", pw->pw_uid);
		if (gflg || aflg)
			printf(":%d", pw->pw_gid);
		if (cflg || aflg)
			printf(":%s", pw->pw_comment);
		if (dflg || aflg)
			printf(":%s", pw->pw_dir);
		if (sflg || aflg)
			printf(":%s", pw->pw_shell);

		printf("\n");
	}

	/* close password database */
	(void)endpwent();
	return(0);
}
