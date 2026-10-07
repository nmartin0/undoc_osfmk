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
static char rcsid[] = "@(#)$RCSfile: cronsub.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:05:06 $";
#endif
/*
 * COMPONENT_NAME:  (CMDOPER) - cronsub.c
 *
 * ORIGIN: IBM
 *
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1988
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 * 
 * cronsub.c   1.7  com/cmd/oper/cron,3.1,9021 4/18/90 20:04:29
 * cronsub.c	4.1 10:17:10 7/12/90 SecureWare 
 */                                                                   

#include <sys/secdefines.h>

#include <sys/types.h>
#include <sys/param.h>        /* for BSIZE needed in dirent.h */
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include "cron.h"

#include "cron_msg.h"
nl_catd catd;
#define	MSGSTR(Num,Str) catgets(catd,MS_CRON,Num,Str)

#define ATRMDEL		"at: only jobs belonging to user : %s may be deleted\n"
#define ATCANTCD	"at: can't change directory to the at directory\n"
#define CRCANTCD   "crontab: can't change directory to the crontab directory\n"
#define BADOPEN		"crontab: can't open your crontab file.\n"
#define CRNOREADDIR	"crontab: can't read the crontabs directory\n"
#define ATNOREADDIR	"at: can't read the atjobs directory\n"
#define BADSTAT		"crontab: stat failed\n"
#define BADUNLINK	"crontab: unlink of crontab file failed\n"
#define BADATUNLINK	"at: unlink of atjob file failed\n"
#define BADSCANDIR	"at: scandir of /var/spool/cron/atjobs failed\n"
#define BADJOBOPEN	"at: unable to read your at job.\n"
#define NONEXIST	"at: Job name %s does not exist.\n"
#define NOTOWNER	"at: You must be the owner of job %s.\n"

struct dirent *dp;
DIR *dir;
char *getfilenam();
int list_aj();
int list_cj();
int remove_cj();
int remove_aj();

extern char *NLgetenv(char *);

/*
 * NAME: rm_at_jobs
 *                                                                    
 * FUNCTION: remove at jobs for a user or the administrator
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *                                                                   
 *	This function will find the specified job and delete it.
 *                                                                   
 * RETURNS: 0 - successful or 1 - failure
 */  


rm_at_jobs(flags,user)
int flags;
char *user;
{
	FILE *fp;
	char *name;
	time_t t, num();
	uid_t uid;
	char *ptr;
	struct passwd *pwinfo;
	struct stat buf;

#if SEC_BASE && defined(BUILDING_AT)
	uid = getluid();
#else
	uid = getuid();
#endif
        pwinfo = getpwuid(uid);

        /* permission check */
#if SEC_BASE && defined(BUILDING_AT)
	if (!at_authorized() && (strcmp(user,pwinfo->pw_name))
#else
        if (uid && (strcmp(user,pwinfo->pw_name))
#endif
                && (strcmp(getfilenam(user),pwinfo->pw_name))) {
                        /* don't have permission to remove file(s) */
                fprintf(stderr,MSGSTR(MS_ATRMDEL,ATRMDEL),pwinfo->pw_name);
                exit(1);
        }

        /* permission granted  !!! pass through !!! */
	if (chdir(ATDIR)==-1) 
		return(1);
	if (flags & CRON_USER) {
		if((dir=opendir(ATDIR)) == NULL)
			return(2);
		while ((dp = readdir(dir)) != NULL ) {
			if (!(strcmp(getfilenam(dp->d_name),user)))  {
				if (flags & CRON_PROMPT)
					if (prompt_del(dp->d_name)==1)
						continue;
				if (unlink(dp->d_name) != 0) {
					return(3);
				}
				sendmsg(DELETE,dp->d_name);
				if (!(flags & CRON_QUIET))
					printf(MSGSTR(MS_ATDELTD,
					"at file: %s deleted\n"), dp->d_name);
			}
		}
		closedir(dir);
	}
	else {
		if (user) {
			if (stat(user, &buf))
				return(4);	/* job file does not exist */
#if SEC_BASE && defined(BUILDING_AT)
			if ((uid = getluid()) != buf.st_uid && !at_authorized())
#else
			if (((uid=getuid()) != buf.st_uid) && (uid != ROOT))
#endif
				return(5);	/* they don't own this one... */
 			/* should we prompt for confirmation */
			if ((flags & CRON_PROMPT) && (prompt_del(user)!=0))
				return(0);
			else {
				if (unlink(user) != 0)  {
					return(3);
				}
				sendmsg(DELETE,user);
				if (!(flags & CRON_QUIET))
					printf(MSGSTR(MS_ATDELTD,"at file: %s deleted\n"), user);
			}
		}
		else { /* root is the only one to get this far */
                        if((dir=opendir(ATDIR)) == NULL)
                                return(2);
                        while ((dp = readdir(dir)) != NULL ) {
                                name = getfilenam(dp->d_name);

                                if (*name == '\0')
                                        continue;
                                if (flags & CRON_PROMPT)
                                         if (prompt_del(dp->d_name)!=0)
                                                continue;
                                if (unlink(dp->d_name) != 0)  {
                                        return(3);
                                }
                                sendmsg(DELETE,dp->d_name);
                                if (!(flags & CRON_QUIET))
                                        printf(MSGSTR(MS_ATDELTD,"at file: %s deleted\n"), dp->d_name);
                        }
                        closedir(dir);
		}
	}
}

/*
 * NAME: prompt_del
 *                                                                    
 * FUNCTION: verify file deletion
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *                                                                   
 *	This function will prompt the user to make sure he really wants
 *	to delete the file, user responds yes or no.
 *                                                                   
 * RETURNS: 0 - successful or 1 - failure
 */  


prompt_del(user)
char *user;
{
	register junk;
	char	ans[2];
	char	*ystr;
	int	in;
	int	yflag = 0;

	ystr = NLgetenv("YESSTR");
	if (ystr!=NULL) {
		in = NLstrcspn(ystr, ":");
		ystr = (char *)NLstrncpy(calloc(in+1, 1), ystr, in);
	}
	else { 
		/* if YESSTR is not set anywhere, ystr is set to default "y" */
		ystr = "y";		
		yflag++;
	}

	printf(MSGSTR(MS_DELETE,"delete %s? (%s) "), user, ystr);
        junk = getwchar();
 	ans[0] = (char) junk;
	ans[1] = '\0';
	
	/* swallow rest of line */
	if (junk != '\n' && junk != EOF)
		do
			junk = getwchar();
		while   (junk != '\n' && junk != EOF); 

	/* if EOF, be safe and forget the whole thing */
	if (junk == EOF)
		return(1);

	/* else normal check */
	if (NLyesno(ans) == 1 || ans[0] == '\n') {
		printf(MSGSTR(MS_YESLOW,"yes\n"));
		return(0);
	/* check the default case when YESSTR is not set anywhere */
	} else if (yflag && ans[0] == 'y') {
		printf(MSGSTR(MS_YESLOW,"yes\n"));
		return(0);
	} else {
		printf(MSGSTR(MS_NO,"no\n"));
		return(1);
	}

}

/*
 * NAME: ls_at_jobs
 *                                                                    
 * FUNCTION: list cron jobs for a user or the administrator
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *                                                                   
 *	This function will list all files for a user or administrator in
 *	the /usr/spool/cron/atjobs directory.
 *                                                                   
 * RETURNS: 0 - successful or 1 - failure
 */  

ls_at_jobs(flags, user)
int flags;
char *user;
{
	time_t t, num();
	char *ptr;
	int count=0;
	int i,alphasort();		/* sort jobs by date of execution */
	int numentries;
	struct dirent **queue;		/* the queue itself */
	char *dotptr;

	if (chdir(ATDIR)==-1) 
		return(1); 
	if (flags & CRON_SORT_E) { /* sort in order of submission */
		if ((numentries = scandir(".",&queue,NULL,NULL)) < 0) 
			return(3);
	}
	else {				/* sort in order of execution */
		if ((numentries = scandir(".",&queue,NULL,alphasort)) < 0) 
			return(3);
	}

	for (i=0;numentries>i;i++)  {
		if (!strcmp(queue[i]->d_name,"."))
			continue;
		if (!strcmp(queue[i]->d_name,".."))
			continue;
		/* all valid at job filenames must contain 2 .'s */
		if ((dotptr = (strchr(queue[i]->d_name,'.'))) == NULL)
			continue;
		/* all valid at job filenames must contain 2 .'s */
		if ((strchr(dotptr+1,'.')) == NULL)
			continue;
		if ((user==NULL) || 
		   !(strcmp(getfilenam(queue[i]->d_name),user)))  {
			ptr = strchr(queue[i]->d_name,'.');
			ptr++;
			t = num(&ptr);
			if ((user == NULL) || 
			   (!(strcmp(getfilenam(queue[i]->d_name),user))))
				count++;
			if (flags & CRON_COUNT_JOBS)
				continue;
			printf("%s\t%s",queue[i]->d_name, NLasctime(localtime(&t)));
		}
	}
	if (flags & CRON_COUNT_JOBS)
		printf(MSGSTR(MS_CRONCNT,"%d files in the queue\n"),count);
	return(0);
}

/*
 * NAME: ls_cron_tabs
 *                                                                    
 * FUNCTION: list cron jobs for a user or the administrator
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *                                                                   
 *	This function will find the specified job and delete it.
 *                                                                   
 * RETURNS: 0 - successful or 1 - failure
 */  

ls_cron_tabs(flags,user)
int flags;
char *user;
{
	FILE *fp;
	char line[CTLINESIZE];
	struct stat buf;
	int found=0;

	if (chdir(CRONDIR)==-1) 
		return(1); 
	if((dir=opendir(CRONDIR)) == NULL)
		return(2);
	while ((dp = readdir(dir)) != NULL ) {
		if (!strcmp(dp->d_name,"..") || !strcmp(dp->d_name,"."))
			continue;
		if ((user==NULL) || !(strcmp(dp->d_name,user))) {
			found++;
			if (flags & CRON_VERBOSE) {
				if ((fp = fopen(dp->d_name,"r")) == NULL)
					return(3);
				if (user == NULL)
					fprintf(stdout,MSGSTR(MS_FILENM,"\ncrontab: filename %s\n"), dp->d_name);
				while(fgets(line,CTLINESIZE,fp) != NULL) {
					fputs(line,stdout);
				}
				fclose(fp);
			}
			else {
				if (stat(dp->d_name,&buf))
					return(4);
				else {
			printf(MSGSTR(MS_SUBTIME,"crontab file: %s\tsubmission time: %s\n"),dp->d_name, NLctime(&buf.st_mtime));
				}
			}
		}
	}
	closedir(dir);
	if ((!found) && (user != NULL)) 
		return(3);
	return(0);
}

/*
 * NAME: rm_cron_tabs
 *                                                                    
 * FUNCTION: remove cron jobs for a user or the administrator
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *                                                                   
 *	This function will find the specified job and delete it.
 *	The administrator can delete anyone's job
 *                                                                   
 * RETURNS: 0 - successful or 1 - failure
 */  

rm_cron_tabs(flags,user)
int flags;
char *user;
{

	if (chdir(CRONDIR)==-1) 
		return(1);

	if (unlink(user) != 0) 
		return(2);

	sendmsg(DELETE,user);
	if (!(flags & CRON_QUIET))
		printf(MSGSTR(MS_CRDELETD,"crontab file: %s deleted\n"),user);
	return(0);
}

/*
 * NAME: getfilenam
 *                                                                    
 * FUNCTION: parses the file name to return just the chars before 1st '.'
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *                                                                   
 * RETURNS: 0 - successful or 1 - failure
 */  

static char tmpptr[UNAMESIZE];

char *getfilenam(base)
char *base;
{
	strcpy(tmpptr,base);
	if ((strchr(tmpptr, '.')) != NULL)
		* (strchr(tmpptr, '.')) = '\0';
	return(tmpptr);
}

list_cj(flags,user)
int flags;
char *user;
{
	switch(ls_cron_tabs(flags,user)) {
	   case 1: 
		fprintf(stderr,(MSGSTR(MS_CRCANTCD, CRCANTCD)));
		exit(1); 
	   case 2: 
		fprintf(stderr, (MSGSTR(MS_CRNOREADDIR, CRNOREADDIR)));
		exit(1); 
 	   case 3: 
		fprintf(stderr,(MSGSTR(MS_BADOPEN, BADOPEN)));
		exit(1); 
	   case 4: 
		fprintf(stderr, (MSGSTR(MS_BADSTAT, BADSTAT)));
		exit(1); 
	}
	return(0); 
}

remove_cj(flags,user)
char *user;
{
	switch(rm_cron_tabs(flags,user)) {
	   case 1: 
		fprintf(stderr,(MSGSTR(MS_CRCANTCD, CRCANTCD)));
		exit(1); 
	   case 2: 
	       fprintf(stderr,(MSGSTR(MS_BADUNLINK,BADUNLINK)));
		exit(1); 
	}
	return(0); 
}
		

list_aj(flags,user)
int flags;
char *user;
{
	switch(ls_at_jobs(flags,user)) {
	   case 1: 
		fprintf(stderr,(MSGSTR(MS_ATCANTCD, ATCANTCD)));
		exit(1);
	   case 2: 
		fprintf(stderr,(MSGSTR(MS_ATNOREADDIR, ATNOREADDIR)));
		exit(1);
	   case 3: 
		fprintf(stderr,(MSGSTR(MS_BADSCANDIR, BADSCANDIR)));
		exit(1);
	}
	return(0);
}


remove_aj(flags,user)
int flags;
char *user;
{
	switch(rm_at_jobs(flags,user)) {
	   case 1: 
		fprintf(stderr,(MSGSTR(MS_ATCANTCD, ATCANTCD)));
		exit(1); 
	   case 2: 
	        fprintf(stderr,(MSGSTR(MS_BADJOBOPEN,BADJOBOPEN)));
		exit(1); 
	   case 3: 
	        fprintf(stderr,(MSGSTR(MS_BADATUNLINK,BADATUNLINK)));
		exit(1); 
	   case 4:
		fprintf(stderr, (MSGSTR(MS_NONEXIST, NONEXIST)), user);
		exit(1);
	   case 5:
		fprintf(stderr, (MSGSTR(MS_NOTOWNER, NOTOWNER)), user);
		exit(1);
	}
	return(0); 
}

