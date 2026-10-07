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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: temp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:18:28 $";
#endif
/* 
 * COMPONENT_NAME: CMDMAILX temp.c
 * 
 * FUNCTIONS: MSGSTR, tinit 
 *
 * ORIGINS: 10  26  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	temp.c       5.2 (Berkeley) 6/21/85
 */

#include "rcv.h"

#include "Mail_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_MAILX,n,s) 

/*
 * Mail -- a mail program
 *
 * Give names to all the temporary files that we will need.
 */

#define FILE_LEN (20)  /* long enough to hold "/tmp/Rs" + pid */
char	tempMail[FILE_LEN];
char	tempQuit[FILE_LEN];
char	tempEdit[FILE_LEN];
char	tempSet[FILE_LEN];
char	tempResid[FILE_LEN];
char	tempMesg[FILE_LEN];

tinit()
{
	register char *cp, *cp2;
	char uname[PATHSIZE];
	char flatuname[PATHSIZE];
	register int err = 0;
	register pid_t pid;

	pid = getpid();
	sprintf(tempMail, "/tmp/Rs%05d", pid);
	sprintf(tempResid, "/tmp/Rq%05d", pid);
	sprintf(tempQuit, "/tmp/Rm%05d", pid);
	sprintf(tempEdit, "/tmp/Re%05d", pid);
	sprintf(tempSet, "/tmp/Rx%05d", pid);
	sprintf(tempMesg, "/tmp/Rx%05d", pid);

	if (strlen(myname) != 0) {
		uid = getuserid(myname);
		if (uid == -1) {
			printf(MSGSTR(NOUSER, "\"%s\" is not a user of this system\n"), myname); /*MSG*/
			exit(1);
		}
	}
	else {
		uid = getuid() & UIDMASK;
		if (username(uid, uname) < 0) {
			copy("ubluit", myname);
			err++;
			if (rcvmode) {
				printf(MSGSTR(WHOAREYOU, "Who are you!?\n")); /*MSG*/
				exit(1);
			}
		}
		else
			copy(uname, myname);
	}
	cp = value("HOME");
	if (cp == NOSTR)
		cp = ".";
	copy(cp, homedir);
	cp = copy(homedir, mbox);
        if ((cp2 = value("MBOX")) == NOSTR)
                copy("/mbox", cp);
        else {
                copy("/", cp);
                cp++;
                copy(cp2, cp);
        }
	cp = copy(homedir, mailrc);
	copy("/.mailrc", cp);
	cp = copy(homedir, deadletter);
        if ((cp2 = value("DEAD")) == NOSTR)
                copy("/dead.letter", cp);
        else {
                copy("/", cp);
                cp++;
                copy(cp2, cp);
        }
}
