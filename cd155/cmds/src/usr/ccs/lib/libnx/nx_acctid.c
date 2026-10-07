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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_acctid.c,v 1.5 1994/11/19 02:31:38 mtm Exp $
 *
 * HISTORY
 * $Log: nx_acctid.c,v $
 * Revision 1.5  1994/11/19  02:31:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/07/01  00:48:11  jkearns
 * Merge from R1.2
 *
 * Revision 1.2.2.1  1994/04/27  18:32:18  jkearns
 * This PTS gives the system administrator the capability to set up default
 * MACS accounts for users, and for the user to be able to change accounts
 * by setting an environment variable, ACCOUNT.
 *
 *  Reviewer: Scott Hahn
 *  Risk: Medium
 *  Benefit or PTS #: 8428
 *  Testing: Combined development/evaluation testing.
 *  Module(s): usr/ccs/lib/libnx: nx_acctid.c nx_initve.c
 *             usr/sbin/allocator/init_appl.c
 *
 * Revision 1.2  1993/10/22  21:15:16  jkearns
 * Change syscall to use the defined names.
 *
 * Revision 1.1  1992/10/01  17:53:41  rkl
 * Initial revision
 *
 *
 */
#include <nx/nxacct.h>
#include <syscall.h>
#include "fcntl.h"
#include <sys/file.h>
#include <pwd.h>
#include <nx/defines.h>


nx_setacctid(id)
acct_t	id;
{
	return (syscall(SYS_nx_setacctid, id));
}

int
nx_getacctid() {
#define BOL	0	/* Beginning of line */
#define ACCT	1	/* Reading account name */
#define COLONFND 2	/* Colon found */
#define NAMES	3	/* Parsing list of names */
#define INVALID	4	/* Rest of line considered invalid */
#define SRCHCOL	5	/* Searching for colon */
#define SRCHNAM	6	/* Searching for first name */
#define MATCH	7	/* Match found */
#define SRCHNNAME 8	/* Searching for next name */

  int i=0, bytes=0, state=BOL, namelen=0, count=0,
	acct_count=0, match_count=0, bol=0, exitflag=0, c=0;
  char *acctname, *tmpacctname;
  char username[20];
  uid_t	uid;
  char buff[512];
  char *buffptr;
  char account[129];
  struct nxacct	*acct;
  struct passwd *tmppw;
  int fd;

  if ((acctname = (char *) getenv("ACCOUNT")) != NULL) {
	tmpacctname=acctname;
	while ((c = (int) *tmpacctname) != '\0') {
		if (isdigit(c) == 0)
			break;
		tmpacctname++;
	}
	if (c == '\0') 
		return(atoi(acctname));	/* Account ID specified */

	acct = nx_getanam(acctname);		/* Account name specified */
        if (acct == NULL) {
                return(-1);   /* No such account in etc/nxaccount */
        } else {
                return(acct->acct_id);
	}
  }
	
  uid = getuid();
  tmppw = (struct passwd *) getpwuid(uid);
  strcpy(username, tmppw->pw_name);
  namelen=strlen(username);

  if ( (fd=open("/etc/nx/nx_dflt_accts", O_RDONLY )  ) == -1 ) 
  	return(0);

  memset(buff, '\0', sizeof(buff));
  memset(account, '\0', sizeof(account));
  buffptr=(char *) buff;
  while ( (bytes=read(fd, buffptr, 512)) > 0) {
	for ( i=0; i<bytes && exitflag==0; i++, count++ ) {
		
	if ( ((buff[i] >= 'a') && (buff[i] <= 'z')) ||
		((buff[i] >= 'A') && (buff[i] <= 'Z')) ||
		((buff[i] >= '0') && (buff[i] <= '9')) ||
		buff[i] == '_') {
		switch ( state ) {
			case BOL: {
				account[0]=buff[i];
				acct_count=1;
				state=ACCT;
				break;
			}
			case ACCT: {
				account[acct_count]=buff[i];
				acct_count++;
				break;
			}
			case SRCHCOL : {
				state=INVALID;
				break;
			}
			case COLONFND :
			case SRCHNAM :
			case NAMES: {
				if ( buff[i] == username[match_count] ) {
					/* a match */
					match_count++;
					if ( match_count == namelen) {
						state=MATCH;
						break;
					}
					else {
						state=NAMES;
						break;
					}
				}
				else {
					match_count=0;
					state=SRCHNNAME;
					break;
				}
			}
			case MATCH:{
				match_count=0;
				state=SRCHNNAME;
				break;
			}
			default: break;
		}	
	}

	else if (buff[i] == '\n') {
		switch(state) {
			case MATCH: {
				exitflag=1;
				break;
			}
			default: { 
				acct_count=0;
				memset(account, '\0', sizeof(account));
				match_count=0;
				state=BOL;
				break;
			}
		}
	}         

	else if (buff[i] == ':') {
		switch ( state ) {
			case ACCT: {
				state=COLONFND;
				account[acct_count]='\0';
				match_count=0;
				break;
			}
			case SRCHCOL: {
				state=COLONFND;
				match_count=0;
				break;
			}
			default: {
				state=INVALID; 
				break; 
			}
		}
	}

	else if ((buff[i] == '\t') || (buff[i] == ' ')) {
		switch(state) {
			case NAMES:
			case SRCHNNAME: {
				state=INVALID;
				break;
			}
			case ACCT: {
				state=SRCHCOL;
				account[acct_count]='\0';
				break;
			}
			case COLONFND:  {
				state=SRCHNAM;
				break;
			}
			case MATCH:  {
				exitflag=1;
				break;
			}
			default: break;
		}
	}

	else if (buff[i] == ',') {
		switch(state) {
			case NAMES: 
			case SRCHNNAME: {
				match_count=0;
				state=COLONFND;
				break;
			}
			case MATCH: {
				exitflag=1;
				break;
			}
			default: {
				state=INVALID; 
				break; 
			}
		}
	}
	else state=INVALID;
	}
  memset(buff, '\0', sizeof(buff));
  }

  close(fd);
  if ( exitflag == 1 ) {
	acct = nx_getanam(account);
        if (acct == NULL) {
                return(-1);  /* No such account in etc/nxaccount */
        } else {
                return(acct->acct_id);
	}
  }
  else
	return(0);
}
