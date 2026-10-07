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
static char rcsid[] = "@(#)$RCSfile: edit.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:18:11 $";
#endif
/* 
 * COMPONENT_NAME: CMDMAILX edit.c
 * 
 * FUNCTIONS: MSGSTR, edit1, editor, visual 
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
 *	edit.c       5.2 (Berkeley) 6/21/85
 */

#include "rcv.h"
#include <stdio.h>
#include <sys/stat.h>

#include "Mail_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_MAILX,n,s) 

/*
 * Mail -- a mail program
 *
 * Perform message editing functions.
 */

/*
 * Edit a message list.
 */

editor(msgvec)
	int *msgvec;
{
	char *edname;

	if ((edname = value("EDITOR")) == NOSTR)
		edname = EDITOR;
	return(edit1(msgvec, edname));
}

/*
 * Invoke the visual editor on a message list.
 */

visual(msgvec)
	int *msgvec;
{
	char *edname;

	if ((edname = value("VISUAL")) == NOSTR)
		edname = VISUAL;
	return(edit1(msgvec, edname));
}

/*
 * Edit a message by writing the message into a funnily-named file
 * (which should not exist) and forking an editor on it.
 * We get the editor from the stuff above.
 */

edit1(msgvec, ed)
	int *msgvec;
	char *ed;
{
	register char *cp, *cp2;
	register int c;
	int *ip, pid, mesg, lines;
	long ms;
	void (*sigint)(), (*sigquit)();
	FILE *ibuf, *obuf;
	char edname[40];
	struct message *mp;
	extern char tempEdit[];
	off_t fsize(), size;
	struct stat statb;
	long modtime;

	/*
	 * Set signals; locate editor.
	 */

	sigint = signal(SIGINT, SIG_IGN);
	sigquit = signal(SIGQUIT, SIG_IGN);

	/*
	 * Deal with each message to be edited . . .
	 */

	for (ip = msgvec; *ip && ip-msgvec < msgCount; ip++) {
		mesg = *ip;
		mp = &message[mesg-1];
		mp->m_flag |= MODIFY;

		/*
		 * Make up a name for the edit file of the
		 * form "/tmp/Messagen.pid" and make sure it doesn't
		 * already exist.
		 */

		sprintf(edname, "/tmp/Message%u.%x", mesg, getpid());
		if (!access(edname, 2)) {
			printf(MSGSTR(FEXISTS, "%s: file exists\n"), edname); /*MSG*/
			goto out;
		}

		/*
		 * Copy the message into the edit file.
		 */

		close(creat(edname, 0600));
		if ((obuf = fopen(edname, "w")) == NULL) {
			perror(edname);
			goto out;
		}
		if (send(mp, obuf, 0) < 0) {
			perror(edname);
			fclose(obuf);
			remove(edname);
			goto out;
		}
		fflush(obuf);
		if (ferror(obuf)) {
			remove(edname);
			fclose(obuf);
			goto out;
		}
		fclose(obuf);

		/*
		 * If we are in read only mode, make the
		 * temporary message file readonly as well.
		 */

		if (readonly)
			chmod(edname, 0400);

		/*
		 * Fork/execl the editor on the edit file.
		 */

		if (stat(edname, &statb) < 0)
			modtime = 0;
		modtime = statb.st_mtime;
		pid = vfork();
		if (pid == -1) {
			perror("fork");
			remove(edname);
			goto out;
		}
		if (pid == 0) {
			sigchild();
			if (sigint != SIG_IGN)
				signal(SIGINT, SIG_DFL);
			if (sigquit != SIG_IGN)
				signal(SIGQUIT, SIG_DFL);
			execl(ed, ed, edname, 0);
			perror(ed);
			_exit(1);
		}
		while (wait(&mesg) != pid)
			;

		/*
		 * If in read only mode, just remove the editor
		 * temporary and return.
		 */

		if (readonly) {
			remove(edname);
			continue;
		}

		/*
		 * Now copy the message to the end of the
		 * temp file if it was changed; else continue
		 * with the next message.
		 */

		if (stat(edname, &statb) < 0) {
			perror(edname);
			goto out;
		}
		if (modtime == statb.st_mtime) {  /* no changes made */
			remove(edname);
			continue;  /* with next message in list */
		}
		if ((ibuf = fopen(edname, "r")) == NULL) {
			perror(edname);
			remove(edname);
			goto out;
		}
		remove(edname);
		fseek(otf, (long) 0, 2);
		size = fsize(otf);
		mp->m_block = blockof(size);
		mp->m_offset = offsetof(size);
		ms = 0L;
		lines = 0;
		while ((c = getc(ibuf)) != EOF) {
			if (c == '\n')
				lines++;
			putc(c, otf);
			if (ferror(otf))
				break;
			ms++;
		}
		mp->m_size = ms;
		mp->m_lines = lines;
		if (ferror(otf))
			perror("/tmp");
		fclose(ibuf);
	}

	/*
	 * Restore signals and return.
	 */

out:
	signal(SIGINT, sigint);
	signal(SIGQUIT, sigquit);
}
