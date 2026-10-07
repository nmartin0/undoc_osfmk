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
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#include <sys/param.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <sys/stat.h>

#ifdef NLS
#include <locale.h>
#endif

#ifdef  MSG
#include <nl_types.h>
#include "groups_msg.h"
nl_catd catd;
#define MSGSTR(num,str) NLcatgets(catd,MS_groups,num,str)  /*MSG*/
#else
#define MSGSTR(num,str) str
#endif

#define GROUP_FILE	"/etc/group"

extern struct passwd *getpwnam();   /* get passwd struct for user */
extern struct group *getgrent(); /* get the next group struct in the file */

int	groups[NGROUPS];

/*
 * NAME: groups [user]
 *                                                                    
 * FUNCTION: Write to standard out user group membership.
 *           Default is current user.
 */  
main(argc, argv)
	int argc;
	char *argv[];
{
	int ngroups, i;
	char *sep = "";
	struct group *gr;
	struct stat sbuf;

#ifdef NLS
	(void ) setlocale (LC_ALL,"");   /* set up table for current lang */
#endif

#ifdef  MSG
	catd = NLcatopen(MF_GROUPS, 0);
#endif

	if(stat(GROUP_FILE, &sbuf) < 0) {
		fprintf(stderr,MSGSTR(M_NO_GRP_FILE, "groups: %s file must be created\n"), GROUP_FILE);
		exit(1);
		}

	if(sbuf.st_size == 0)
		fprintf(stderr,MSGSTR(M_BAD_SIZE, "groups: %s bad size\n"), GROUP_FILE);

	if (argc > 1)
		showgroups(argv[1]);
	else {
	ngroups = getgroups(NGROUPS, groups); /* get list of current */
                                                /* user's group membership */
	for (i = 0; i < ngroups; i++) {
		gr = getgrgid(groups[i]);             /* get data on group */
		if (gr == NULL)
			printf("%s%d", sep, groups[i]);
		else
			printf("%s%s", sep, gr->gr_name);
		sep = " ";
		}
	printf("\n");
	}
exit(0);	/* normal exit code */
}

/*
 * NAME: showgroup
 *                                                                    
 * FUNCTION: Display group membership for user.
 */  
showgroups(user)
	char *user;
{
	struct group *gr;
	struct passwd *pw;
	char **cp;
	char *sep = "";
		
	if ((pw = getpwnam(user)) == NULL) {       /* get data on user */
		fprintf(stderr,MSGSTR(M_MSG_3, "No such user\n"));
		exit(1);
	}
	 while (gr = getgrent()) {         /* search group file for user */
		if (pw->pw_gid == gr->gr_gid) {
			printf("%s%s", sep, gr->gr_name);
			sep = " ";
			continue;
		}	
		for (cp = gr->gr_mem; cp && *cp; cp++)
			if (strcmp(*cp, user) == 0) {
				printf("%s%s", sep, gr->gr_name);
				sep = " ";
				break;
			}
	}
	printf("\n");
	return;
}
