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
static char rcsid[] = "@(#)$RCSfile: cmd1.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:17:57 $";
#endif
/* 
 * COMPONENT_NAME: CMDMAILX cmd1.c
 * 
 * FUNCTIONS: MSGSTR, More, Type, brokpipe, folders, from, headers, 
 *            local, mboxit, more, pcmdlist, pdot, print, printhead, 
 *            screensize, scroll, stouch, top, type, type1 
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
 *	cmd1.c       5.3 (Berkeley) 9/15/85
 */

#include "rcv.h"
#include <sys/stat.h>

#include "Mail_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_MAILX,n,s) 

/*
 * Mail -- a mail program
 *
 * User commands.
 */

/*
 * Print the current active headings.
 * Don't change dot if invoker didn't give an argument.
 */

static int screen;
void brokpipe();

headers(msgvec)
	int *msgvec;
{
	register int n, mesg, flag;
	register struct message *mp;
	int size;

	size = screensize();
	n = msgvec[0];
	if (n != 0)
		screen = (n-1)/size;
	if (screen < 0)
		screen = 0;
	mp = &message[screen * size];
	if (mp >= &message[msgCount])
		mp = &message[msgCount - size];
	if (mp < &message[0])
		mp = &message[0];
	flag = 0;
	mesg = mp - &message[0];
	if (dot != &message[n-1])
		dot = mp;
	for (; mp < &message[msgCount]; mp++) {
		mesg++;
		if (mp->m_flag & MDELETED)
			continue;
		if (flag++ >= size)
			break;
		printhead(mesg);
		sreset();
	}
	if (flag == 0) {
		printf(MSGSTR(NOMAIL, "No more mail.\n")); /*MSG*/
		return(1);
	}
	return(0);
}

/*
 * Set the list of alternate names for out host.
 */
local(namelist)
	char **namelist;
{
	register int c;
	register char **ap, **ap2, *cp;

	c = argcount(namelist) + 1;
	if (c == 1) {
		if (localnames == 0)
			return(0);
		for (ap = localnames; *ap; ap++)
			printf("%s ", *ap);
		printf("\n");
		return(0);
	}
	if (localnames != 0)
		cfree((char *) localnames);
	localnames = (char **) calloc(c, sizeof (char *));
	for (ap = namelist, ap2 = localnames; *ap; ap++, ap2++) {
		cp = (char *) calloc(strlen(*ap) + 1, sizeof (char));
		strcpy(cp, *ap);
		*ap2 = cp;
	}
	*ap2 = 0;
	return(0);
}

/*
 * Scroll to the next/previous screen
 */

scroll(arg)
	char arg[];
{
	register int s, size;
	int cur[1];

	cur[0] = 0;
	size = screensize();
	s = screen;
	switch (*arg) {
	case 0:
	case '+':
		s++;
		if (s * size > msgCount) {
			printf(MSGSTR(LAST, "On last screenful of messages\n")); /*MSG*/
			return(0);
		}
		screen = s;
		break;

	case '-':
		if (--s < 0) {
			printf(MSGSTR(FIRST, "On first screenful of messages\n")); /*MSG*/
			return(0);
		}
		screen = s;
		break;

	default:
		printf(MSGSTR(NOSCMD, "Unrecognized scrolling command \"%s\"\n"), arg); /*MSG*/
		return(1);
	}
	return(headers(cur));
}

/*
 * Compute what the screen size should be.
 * We use the following algorithm:
 *	If user specifies with screen option, use that.
 *	If baud rate < 1200, use  5
 *	If baud rate = 1200, use 10
 *	If baud rate > 1200, use 20
 */
screensize()
{
	register char *cp;
	register int s;
#ifdef	TIOCGWINSZ
	struct winsize ws;
#endif

	if ((cp = value("screen")) != NOSTR) {
		s = atoi(cp);
		if (s > 0)
			return(s);
	}
	if (baud < B1200)
		s = 5;
	else if (baud == B1200)
		s = 10;
#ifdef	TIOCGWINSZ
	else if (ioctl(fileno(stdout), TIOCGWINSZ, &ws) == 0 && ws.ws_row != 0)
		s = ws.ws_row - 4;
#endif
	else
		s = 20;
	return(s);
}

/*
 * Print out the headlines for each message
 * in the passed message list.
 */

from(msgvec)
	int *msgvec;
{
	register int *ip;

	for (ip = msgvec; *ip != NULL; ip++) {
		printhead(*ip);
		sreset();
	}
	if (--ip >= msgvec)
		dot = &message[*ip - 1];
	return(0);
}

/*
 * Print out the header of a specific message.
 * This is a slight improvement to the standard one.
 */

printhead(mesg)
{
	struct message *mp;
	FILE *ibuf;
	char headline[LINESIZE], wcount[LINESIZE], *subjline, dispc, curind;
	char pbuf[BUFSIZ];
	int s;
	struct headline hl;
	register char *cp;
	char flatline[LINESIZE];

	mp = &message[mesg-1];
	ibuf = setinput(mp);
	readline(ibuf, headline);
	subjline = hfield("subject", mp);
	if (subjline == NOSTR)
		subjline = hfield("subj", mp);

	/*
	 * Bletch!
	 */

	if (subjline != NOSTR && strlen(subjline) > 27)
		subjline[28] = '\0';
	curind = dot == mp ? '>' : ' ';
	dispc = ' ';
	if (mp->m_flag & MSAVED)
		dispc = '*';
	if (mp->m_flag & MPRESERVE)
		dispc = 'P';
	if ((mp->m_flag & (MREAD|MNEW)) == MNEW)
		dispc = 'N';
	if ((mp->m_flag & (MREAD|MNEW)) == 0)
		dispc = 'U';
	if (mp->m_flag & MBOX)
		dispc = 'M';
	parse(headline, &hl, pbuf);
	sprintf(wcount, "%d/%ld", mp->m_lines, mp->m_size);
	s = strlen(wcount);
	cp = wcount + s;
	while (s < 7)
		s++, *cp++ = ' ';
	*cp = '\0';
	if (subjline != NOSTR) {
		NLflatstr (subjline, flatline, LINESIZE);
                if ((value("showto") != NOSTR)  &&
                    (strcmp(hl.l_from, myname) == 0) )
                    printf("%c%c%3d To %-19.19s  %16.16s %8s \"%s\"\n", curind,
                           dispc, mesg, nameto(mp), hl.l_date, wcount,
                           flatline);
                else
                    printf("%c%c%3d %-16.16s  %16.16s %8s \"%s\"\n", curind,
                       dispc, mesg, nameof(mp, 0), hl.l_date, wcount, flatline);
        }
        else {
                if ((value("showto") != NOSTR)  &&
                    (strcmp(hl.l_from, myname) == 0) )
                    printf("%c%c%3d To %-19.19s  %16.16s %8s \"%s\"\n", curind,
                           dispc, mesg, nameto(mp), hl.l_date, wcount,
                           flatline);
                else
                    printf("%c%c%3d %-16.16s  %16.16s %8s\n", curind, dispc,
                       mesg, nameof(mp, 0), hl.l_date, wcount);
        }
}

/*
 * Print out the value of dot.
 */

pdot()
{
	printf("%d\n", dot - &message[0] + 1);
	return(0);
}

/*
 * Print out all the possible commands.
 */

pcmdlist()
{
	register struct cmd *cp;
	register int cc;
	extern struct cmd cmdtab[];

	printf(MSGSTR(CMDS, "Commands are:\n")); /*MSG*/
	for (cc = 0, cp = cmdtab; cp->c_name != NULL; cp++) {
		cc += strlen(cp->c_name) + 2;
		if (cc > 72) {
			printf("\n");
			cc = strlen(cp->c_name) + 2;
		}
		if ((cp+1)->c_name != NOSTR)
			printf("%s, ", cp->c_name);
		else
			printf("%s\n", cp->c_name);
	}
	return(0);
}

/*
 * Paginate messages, honor ignored fields.
 */
more(msgvec)
	int *msgvec;
{
	return (type1(msgvec, 1, 1));
}

/*
 * Paginate messages, even printing ignored fields.
 */
More(msgvec)
	int *msgvec;
{

	return (type1(msgvec, 0, 1));
}

/*
 * Type out messages, honor ignored fields.
 */
type(msgvec)
	int *msgvec;
{

	return(type1(msgvec, 1, 0));
}

/*
 * Type out messages, even printing ignored fields.
 */
Type(msgvec)
	int *msgvec;
{

	return(type1(msgvec, 0, 0));
}

/*
 * Type out the messages requested.
 */
jmp_buf	pipestop;

type1(msgvec, doign, page)
	int *msgvec;
{
	register *ip;
	register struct message *mp;
	register int mesg;
	register char *cp;
	int c, nlines;
	FILE *ibuf, *obuf;

	obuf = stdout;
	if (setjmp(pipestop)) {
		if (obuf != stdout) {
			pipef = NULL;
			pclose(obuf);
		}
		signal(SIGPIPE, SIG_DFL);
		return(0);
	}
	if (intty && outtty && (page || (cp = value("crt")) != NOSTR)) {
		nlines = 0;
		if (!page) {
			for (ip = msgvec; *ip && ip-msgvec < msgCount; ip++)
				nlines += message[*ip - 1].m_lines;
		}
		if (page || nlines > atoi(cp)) {
			cp = value("PAGER");
			if (cp == NULL || *cp == '\0')
				cp = PAGE;
			obuf = popen(cp, "w");
			if (obuf == NULL) {
				perror(cp);
				obuf = stdout;
			}
			else {
				pipef = obuf;
				signal(SIGPIPE, brokpipe);
			}
		}
	}
	for (ip = msgvec; *ip && ip-msgvec < msgCount; ip++) {
		mesg = *ip;
		touch(mesg);
		mp = &message[mesg-1];
		dot = mp;
		print(mp, obuf, doign);
	}
	if (obuf != stdout) {
		pipef = NULL;
		pclose(obuf);
	}
	signal(SIGPIPE, SIG_DFL);
	return(0);
}

/*
 * Respond to a broken pipe signal --
 * probably caused by using quitting more.
 */

void
brokpipe()
{
	longjmp(pipestop, 1);
}

/*
 * Print the indicated message on standard output.
 */

print(mp, obuf, doign)
	register struct message *mp;
	FILE *obuf;
{

	if (value("quiet") == NOSTR)
		fprintf(obuf, MSGSTR(MESGNUM, "Message %2d:\n"), mp - &message[0] + 1); /*MSG*/
	touch(mp - &message[0] + 1);
	send(mp, obuf, doign);
}

/*
 * Print the top so many lines of each desired message.
 * The number of lines is taken from the variable "toplines"
 * and defaults to 5.
 */

top(msgvec)
	int *msgvec;
{
	register int *ip;
	register struct message *mp;
	register int mesg;
	int c, topl, lines, lineb;
	char *valtop, linebuf[LINESIZE];
	FILE *ibuf;

	topl = 5;
	valtop = value("toplines");
	if (valtop != NOSTR) {
		topl = atoi(valtop);
		if (topl < 0 || topl > 10000)
			topl = 5;
	}
	lineb = 1;
	for (ip = msgvec; *ip && ip-msgvec < msgCount; ip++) {
		mesg = *ip;
		touch(mesg);
		mp = &message[mesg-1];
		dot = mp;
		if (value("quiet") == NOSTR)
			printf(MSGSTR(MESGNUM, "Message %2d:\n"), mesg); /*MSG*/
		ibuf = setinput(mp);
		c = mp->m_lines;
		if (!lineb)
			printf("\n");
		for (lines = 0; lines < c && lines < topl; lines++) {
			if (readline(ibuf, linebuf) <= 0)
				break;
			puts(linebuf);
			lineb = blankline(linebuf);
		}
	}
	return(0);
}

/*
 * Touch all the given messages so that they will
 * get mboxed.
 */

stouch(msgvec)
	int msgvec[];
{
	register int *ip;

	for (ip = msgvec; *ip != 0; ip++) {
		dot = &message[*ip-1];
		dot->m_flag |= MTOUCH;
		dot->m_flag &= ~MPRESERVE;
	}
	return(0);
}

/*
 * Make sure all passed messages get mboxed.
 */

mboxit(msgvec)
	int msgvec[];
{
	register int *ip;

	for (ip = msgvec; *ip != 0; ip++) {
		dot = &message[*ip-1];
		dot->m_flag |= MTOUCH|MBOX;
		dot->m_flag &= ~MPRESERVE;
	}
	return(0);
}

/*
 * List the folders the user currently has.
 */
folders()
{
	char dirname[BUFSIZ], cmd[BUFSIZ];
	int pid, s, e;
	char *lister, *lister_no_arg;

	if (getfold(dirname) < 0) {
		printf(MSGSTR(NOVALUE, "No value set for \"folder\"\n")); /*MSG*/
		return(-1);
	}
	switch ((pid = fork())) {
	case 0:
		sigchild();

                /*
                 * If the environment variable LISTER is set then use the
                 * given command as the lister, along with the given
                 * options.  Otherwise, just use ls.
                 */

                if ((lister = value("LISTER")) == NOSTR) {
                    execlp("ls", "ls", dirname, 0);
                }
                else {
                    lister_no_arg = (char *)malloc(strlen(lister) + 1);
                    sscanf(lister, "%s", lister_no_arg);
                    if (strlen(lister_no_arg) == strlen(lister))
                        execlp(lister_no_arg, lister_no_arg, dirname, 0);
                    else {
                        lister = lister + strlen(lister_no_arg) + 1;
                        execlp(lister_no_arg, lister_no_arg, lister, dirname, 0);
                    }
                }

		_exit(1);

	case -1:
		perror("fork");
		return(-1);

	default:
		while ((e = wait(&s)) != -1 && e != pid)
			;
	}
	return(0);
}
