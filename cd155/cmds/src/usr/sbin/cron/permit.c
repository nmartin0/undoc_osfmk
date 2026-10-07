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
static char rcsid[] = "@(#)$RCSfile: permit.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:05:12 $";
#endif
/*
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: permit
 *
 * ORIGINS: 3, 27
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
 *
 */

/*
 * module used for crontab and at
 */                                                                   

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <pwd.h>
#include "cron.h"

struct stat globstat;
#define	exists(file)	(stat(file,&globstat) == 0)
#undef ROOT	/* defined as 0 in cron.h */
#define ROOT	"root"

int per_errno;	/* status info from getuser */


/****************/
char *getuser(uid)
/****************/
int uid;
{
	struct passwd *nptr;

	if ((nptr=getpwuid((uid_t)uid)) == NULL) {
		per_errno=1;
		return(NULL); }
	if ((strcmp(nptr->pw_shell,SHELL)!=0) &&
	    (strcmp(nptr->pw_shell,"")!=0)) {
		per_errno=2;
		/* return NULL if you want crontab and at to abort
		   when the users login shell is not /bin/sh otherwise
		   return pw_name
		*/
		return(nptr->pw_name);
	}
	return(nptr->pw_name);
}


/**********************/
allowed(user,allow,deny)
/**********************/
char *user,*allow,*deny;
{
	if ( exists(allow) ) {
		if ( within(user,allow) ) return(1);
		else return(0); }
	else if ( exists(deny) ) {
		if ( within(user,deny) ) return(0);
		else return(1); }
	else if ( strcmp(user,ROOT)==0 ) return(1);
		else return(0);
}


/************************/
within(username,filename)
/************************/
char *username,*filename;
{
	char line[UNAMESIZE];
	FILE *cap;
	int i;

	if((cap = fopen(filename,"r")) == NULL)
		return(0);
	while ( fgets(line,UNAMESIZE,cap) != NULL ) {
		for ( i=0 ; line[i] != '\0' ; i++ ) {
			if ( isspace(line[i]) ) {
				line[i] = '\0';
				break; }
		}
		if ( strcmp(line,username)==0 ) {
			fclose(cap);
			return(1); }
	}
	fclose(cap);
	return(0);
}
