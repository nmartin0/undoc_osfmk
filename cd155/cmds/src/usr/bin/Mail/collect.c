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
static char rcsid[] = "@(#)$RCSfile: collect.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:18:06 $";
#endif
/* 
 * COMPONENT_NAME: CMDMAILX collect.c
 * 
 * FUNCTIONS: MSGSTR, addto, collcont, collect, collhupsig, 
 *            collintsig, collrub, exwrite, forward, intack, mesedit, 
 *            mespipe, psig, transmit 
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
 *	collect.c    5.2 (Berkeley) 6/21/85
 */

/*
 * Mail -- a mail program
 *
 * Collect input from standard input, handling
 * ~ escapes.
 */

#include "rcv.h"
#include <sys/stat.h>

#include "Mail_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_MAILX,n,s) 

/*
 * Read a message from standard output and return a read file to it
 * or NULL on error.
 */

/*
 * The following hokiness with global variables is so that on
 * receipt of an interrupt signal, the partial message can be salted
 * away on dead.letter.  The output file must be available to flush,
 * and the input to read.  Several open files could be saved all through
 * Mail if stdio allowed simultaneous read/write access.
 */

char *tilde[] = {
"Control Commands:",
"   <EOT>           Send message (Ctrl-D on many terminals).",
"   ~q              Quit editor without sending message.",
"   ~p              Display the contents of the message buffer.",
"   ~x              Exit editor without saving or sending message.",
"   ~: <mcmd>       Run a mailbox command, <mcmd>.",
"Add to Heading:",
"   ~h              Add to lists for To:, Subject:, Cc: and Bcc:.",
"   ~t <addrlist>   Add user addresses in <addrlist> to the To: list.",
"   ~s <subject>    Set the Subject: line to the string specified by <subject>.",
"   ~c <addrlist>   Add user addresses in <addrlist> to Cc: (copy to) list.",
"   ~b <addrlist>   Add user addresses in <addrlist> to Bcc: (blind copy) list.",
"Add to Message:",
"   ~{a,A}          Insert the Autograph string, denoted by environment ",
"                   variable sign (or Sign for ~A), into the message.",
"   ~d              Append the contents of dead.letter to the message.",
"   ~i <string>     Insert the value of <string> into the text of the message.",
"   ~r <filename>   Append the contents of <filename> to the message.",
"   ~m <numlist>    Append/indent the contents of message numbers <numlist>.",
"   ~<! <command>   Append the standard output of <command> to the message.",
"Change Message:",
"   ~e              Edit the message using an external editor (default is e).",
"   ~v              Edit the message using an external editor (default is vi).",
"   ~w <filename>   Write the message to <filename>.",
"   ~! <command>    Start a shell, run <command>, and return to the editor.",
"   ~| <command>    Pipe the message to standard input of <command>; REPLACE",
"                   the message with the standard output from that command.",
"================ Mail Editor Commands  (continue on next line) ================",
NULL
};

static	void	(*savesig)();		/* Previous SIGINT value */
static	void	(*savehup)();		/* Previous SIGHUP value */
#ifdef SIGCONT
static	void	(*savecont)();		/* Previous SIGCONT value */
#endif SIGCONT
extern	void intack();
void	collcont();
static	FILE	*newi;			/* File for saving away */
static	FILE	*newo;			/* Output side of same */
int	hf;				/* Ignore interrups */
int	hadintr;			/* Have seen one SIGINT so far */

jmp_buf	*Jmpbuf = (jmp_buf *) 0;	/* global ptr for signal handler */

FILE *
collect(hp)
	struct header *hp;
{
	FILE *ibuf, *fbuf, *obuf;
	int lc, cc, escape, collhup, eof;
	void collrub(), intack();
	register int c, t;
	char linebuf[LINESIZE], *cp;
	extern char tempMail[];
	int notify();
	extern collintsig(), collhupsig();
	char getsub;
	int hcount;
	jmp_buf	coljmp;			/* To get back to work */
	char tempstr[LINESIZE];

	noreset++;
	ibuf = obuf = NULL;
	if (value("ignore") != NOSTR)
		hf = 1;
	else
		hf = 0;
	hadintr = 0;
	if ((savesig = signal(SIGINT, SIG_IGN)) != SIG_IGN)
		signal(SIGINT, hf ? intack : collrub),
		    sigblock(sigmask(SIGINT));
	if ((savehup = signal(SIGHUP, SIG_IGN)) != SIG_IGN)
		signal(SIGHUP, collrub), sigblock(sigmask(SIGHUP));
#ifdef SIGCONT
	savecont = signal(SIGCONT, collcont);
#endif
	newi = NULL;
	newo = NULL;
	if ((obuf = fopen(tempMail, "w")) == NULL) {
		perror(tempMail);
		goto err;
	}
	newo = obuf;
	if ((ibuf = fopen(tempMail, "r")) == NULL) {
		perror(tempMail);
		newo = NULL;
		fclose(obuf);
		goto err;
	}
	newi = ibuf;
	remove(tempMail);

	/*
	 * If we are going to prompt for a subject,
	 * refrain from printing a newline after
	 * the headers (since some people mind).
	 */

	t = GTO|GSUBJECT|GCC|GNL;
	getsub = 0;
	if (sflag != NOSTR && hp->h_subject == NOSTR)
		hp->h_subject=sflag;
	if (intty && sflag == NOSTR && hp->h_subject == NOSTR &&
	    (value("ask") || value("asksub")) )
		t &= ~GNL, getsub++;
	if (hp->h_seq != 0) {
		puthead(hp, stdout, t);
		fflush(stdout);
	}
	escape = ESCAPE;
	if ((cp = value("escape")) != NOSTR)
		escape = *cp;
	eof = 0;
	for (;;) {
		int omask = sigblock(0) &~ (sigmask(SIGINT)|sigmask(SIGHUP));

		setjmp(coljmp);
		Jmpbuf = (jmp_buf *) coljmp;  /* ptr for signal handler */
		sigsetmask(omask);
		fflush(stdout);
		if (getsub) {
			grabh(hp, GSUBJECT);
			getsub = 0;
			continue;
		}
		if (readline(stdin, linebuf) <= 0) {
			if (intty && value("ignoreeof") != NOSTR) {
				if (++eof > 35)
					break;
				printf(MSGSTR(TERM, "Use \".\" to terminate letter\n"), escape); /*MSG*/
				continue;
			}
			break;
		}
		eof = 0;
		hadintr = 0;
		if (intty && equal(".", linebuf) &&
		    (value("dot") != NOSTR || value("ignoreeof") != NOSTR))
			break;
		if (linebuf[0] != escape || rflag != NOSTR) {
			if ((t = putline(obuf, linebuf)) < 0)
				goto err;
			continue;
		}
		c = linebuf[1];
		switch (c) {
		default:
			/*
			 * On double escape, just send the single one.
			 * Otherwise, it's an error.
			 */

			if (c == escape) {
				if (putline(obuf, &linebuf[1]) < 0)
					goto err;
				else
					break;
			}
			printf(MSGSTR(NOESC, "Unknown tilde escape.\n")); /*MSG*/
			break;

		case 'C':
			/*
			 * Dump core.
			 */

			core();
			break;

		case '!':
			/*
			 * Shell escape, send the balance of the
			 * line to sh -c.
			 */

			shell(&linebuf[2]);
			break;

		case ':':
		case '_':
			/*
			 * Escape to command mode, but be nice!
			 */

			execute(&linebuf[2], 1);
			printf(MSGSTR(CONT, "(continue)\n")); /*MSG*/
			break;

		case '.':
			/*
			 * Simulate end of file on input.
			 */
			goto eofl;

		case 'q':
		case 'Q':
			/*
			 * Force a quit of sending mail.
			 * Act like an interrupt happened.
			 */

			hadintr++;
			collrub(SIGINT);
			exit(1);

		case 'h':
			/*
			 * Grab a bunch of headers.
			 */
			if (!intty || !outtty) {
				printf(MSGSTR(NOCANDO, "~h: no can do!?\n")); /*MSG*/
				break;
			}
			grabh(hp, GTO|GSUBJECT|GCC|GBCC);
			printf(MSGSTR(CONT, "(continue)\n")); /*MSG*/
			break;

		case 't':
			/*
			 * Add to the To list.
			 */

			hp->h_to = addto(hp->h_to, &linebuf[2]);
			hp->h_seq++;
			break;

		case 's':
			/*
			 * Set the Subject list.
			 */

			cp = &linebuf[2];
			while (any(*cp, " \t"))
				cp++;
			hp->h_subject = savestr(cp);
			strcpy (tempstr, hp->h_subject);
			NLflatstr (tempstr, hp->h_subject, LINESIZE);
			hp->h_seq++;
			break;

		case 'c':
			/*
			 * Add to the CC list.
			 */

			hp->h_cc = addto(hp->h_cc, &linebuf[2]);
			hp->h_seq++;
			break;

		case 'b':
			/*
			 * Add stuff to blind carbon copies list.
			 */
			hp->h_bcc = addto(hp->h_bcc, &linebuf[2]);
			hp->h_seq++;
			break;

                case 'A':
                        /*
                         * Insert the autograph string defined by
                         * the environment variable Sign.
                         */

                        cp = value("Sign");
			if (cp) {
                        	printf("%s\n",cp);
                        	if ((t = putline(obuf, cp)) < 0)
                        	    goto err;
			}
                        break;

                case 'a':
                        /*
                         * Insert the autograph string defined by
                         * the environment variable sign.
                         */

                        cp = value("sign");
			if (cp) {
                            printf("%s\n",cp);
                            if ((t = putline(obuf, cp)) < 0)
                                goto err;
			}
                        break;

                case 'i':
                        /*
                         * Insert the contents of the given environment
                         * variable.
                         */
                        cp = value(&linebuf[3]);
                        printf("%s\n", cp);
                        if ((t == putline(obuf, cp)) < 0)
                            goto err;
                        break;

                case 'x':
                        /*
                         * Force a quit of sending mail.
                         * Act like an interrupt happened.
                         * Don't put copy of msg in dead.letter.
                         */

                        hadintr++;
                        assign("nosave", "");
                        collrub(SIGINT);
                        vfree("nosave");
                        exit(1);

		case 'd':
			copy(deadletter, &linebuf[2]);
			/* fall into . . . */

		case 'r':
		case '<':
			/*
			 * Invoke a file:
			 * If shell escape, insert standard output in
			 * to message (XPG3).
			 * Search for the file name,
			 * then open it and copy the contents to obuf.
			 */

			cp = &linebuf[2];
			if (*cp == '!') {
				readshell(obuf, &linebuf[3]);
				break;
			}
			while (any(*cp, " \t"))
				cp++;
			if (*cp == '\0') {
				printf(MSGSTR(WHAT, "Interpolate what file?\n")); /*MSG*/
				break;
			}
			cp = expand(cp);
			if (cp == NOSTR)
				break;
			if (isdir(cp)) {
				printf(MSGSTR(DIR, "%s: directory\n"), cp); /*MSG*/
				break;
			}
			if ((fbuf = fopen(cp, "r")) == NULL) {
				perror(cp);
				break;
			}
			printf("\"%s\" ", cp);
			fflush(stdout);
			lc = 0;
			cc = 0;
			while (readline(fbuf, linebuf) > 0) {
				lc++;
				if ((t = putline(obuf, linebuf)) < 0) {
					fclose(fbuf);
					goto err;
				}
				cc += t;
			}
			fclose(fbuf);
			printf("%d/%d\n", lc, cc);
			break;

		case 'w':
			/*
			 * Write the message on a file.
			 */

			cp = &linebuf[2];
			while (any(*cp, " \t"))
				cp++;
			if (*cp == '\0') {
				fprintf(stderr, MSGSTR(SAYWHAT, "Write what file!?\n")); /*MSG*/
				break;
			}
			if ((cp = expand(cp)) == NOSTR)
				break;
			fflush(obuf);
			rewind(ibuf);
			exwrite(cp, ibuf, 1);
			break;

		case 'm':
		case 'f':
			/*
			 * Interpolate the named messages, if we
			 * are in receiving mail mode.  Does the
			 * standard list processing garbage.
			 * If ~f is given, we don't shift over.
			 */

			if (!rcvmode) {
				printf(MSGSTR(NOTOSEND, "No messages to send from!?!\n")); /*MSG*/
				break;
			}
			cp = &linebuf[2];
			while (any(*cp, " \t"))
				cp++;
			if (forward(cp, obuf, c) < 0)
				goto err;
			printf(MSGSTR(CONT, "(continue)\n")); /*MSG*/
			break;

		case '?':
			for (hcount = 0; tilde[hcount]; hcount++)
			    puts(MSGSTR(THELP+hcount, tilde[hcount]));
			break;

		case 'p':
			/*
			 * Print out the current state of the
			 * message without altering anything.
			 */

			fflush(obuf);
			rewind(ibuf);
			printf(MSGSTR(CONTAINS, "-------\nMessage contains:\n")); /*MSG*/
			puthead(hp, stdout, GTO|GSUBJECT|GCC|GBCC|GNL);
			t = getc(ibuf);
			while (t != EOF) {
				putchar(t);
				t = getc(ibuf);
			}
			printf(MSGSTR(CONT, "(continue)\n")); /*MSG*/
			break;

		case '^':
		case '|':
			/*
			 * Pipe message through command.
			 * Collect output as new message.
			 */

			obuf = mespipe(ibuf, obuf, &linebuf[2]);
			newo = obuf;
			ibuf = newi;
			newi = ibuf;
			printf(MSGSTR(CONT, "(continue)\n")); /*MSG*/
			break;

		case 'v':
		case 'e':
			/*
			 * Edit the current message.
			 * 'e' means to use EDITOR
			 * 'v' means to use VISUAL
			 */

			if ((obuf = mesedit(ibuf, obuf, c)) == NULL)
				goto err;
			newo = obuf;
			ibuf = newi;
			printf(MSGSTR(CONT, "(continue)\n")); /*MSG*/
			break;
		}
	}
eofl:
	fclose(obuf);
	rewind(ibuf);
	if (intty && value("askcc") != NOSTR)
		grabh(hp, GCC);
	else if (intty) {
		printf(MSGSTR(EOT, "EOT\n")); /*MSG*/
		fflush(stdout);
	}
	signal(SIGINT, savesig);
	signal(SIGHUP, savehup);
#ifdef SIGCONT
	signal(SIGCONT, savecont);
#endif
	sigsetmask(0);
	Jmpbuf = (jmp_buf *) 0;  /* clear ptr for signal handler */
	noreset = 0;
	return(ibuf);

err:
	if (ibuf != NULL)
		fclose(ibuf);
	if (obuf != NULL)
		fclose(obuf);
	signal(SIGINT, savesig);
	signal(SIGHUP, savehup);
#ifdef SIGCONT
	signal(SIGCONT, savecont);
#endif
	sigsetmask(0);
	Jmpbuf = (jmp_buf *) 0;  /* clear ptr for signal handler */
	noreset = 0;
	return(NULL);
}

/*
 * Non destructively interrogate the value of the given signal.
 */

void
(*psig(int n))()
{
	void (*wassig)();

	wassig = signal(n, SIG_IGN);
	signal(n, wassig);
	return(wassig);
}

/*
 * Write a file, ex-like if f set.
 */

exwrite(name, ibuf, f)
	char name[];
	FILE *ibuf;
{
	register FILE *of;
	register int c;
	long cc;
	int lc;
	struct stat junk;

	if (f) {
		printf("\"%s\" ", name);
		fflush(stdout);
	}
	if (stat(name, &junk) >= 0 && (junk.st_mode & S_IFMT) == S_IFREG) {
		if (!f)
			fprintf(stderr, "%s: ", name);
		fprintf(stderr, MSGSTR(EXISTS, "File exists\n"), name); /*MSG*/
		return(-1);
	}
	if ((of = fopen(name, "w")) == NULL) {
		perror(NOSTR);
		return(-1);
	}
	lc = 0;
	cc = 0;
	while ((c = getc(ibuf)) != EOF) {
		cc++;
		if (c == '\n')
			lc++;
		putc(c, of);
		if (ferror(of)) {
			perror(name);
			fclose(of);
			return(-1);
		}
	}
	fclose(of);
	printf("%d/%ld\n", lc, cc);
	fflush(stdout);
	return(0);
}

/*
 * Edit the message being collected on ibuf and obuf.
 * Write the message out onto some poorly-named temp file
 * and point an editor at it.
 *
 * On return, make the edit file the new temp file.
 */

FILE *
mesedit(ibuf, obuf, c)
	FILE *ibuf, *obuf;
{
	int pid, s;
	FILE *fbuf;
	register int t;
	void (*sig)(), (*scont)();
	void signull();
	struct stat sbuf;
	extern char tempMail[], tempEdit[];
	register char *edit;

	sig = signal(SIGINT, SIG_IGN);
#ifdef SIGCONT
	scont = signal(SIGCONT, signull);
#endif SIGCONT
	if (stat(tempEdit, &sbuf) >= 0) {
		printf(MSGSTR(FEXISTS, "%s: file exists\n"), tempEdit); /*MSG*/
		goto out;
	}
	close(creat(tempEdit, 0600));
	if ((fbuf = fopen(tempEdit, "w")) == NULL) {
		perror(tempEdit);
		goto out;
	}
	fflush(obuf);
	rewind(ibuf);
	t = getc(ibuf);
	while (t != EOF) {
		putc(t, fbuf);
		t = getc(ibuf);
	}
	fflush(fbuf);
	if (ferror(fbuf)) {
		perror(tempEdit);
		remove(tempEdit);
		goto fix;
	}
	fclose(fbuf);
	if ((edit = value(c == 'e' ? "EDITOR" : "VISUAL")) == NOSTR)
		edit = c == 'e' ? EDITOR : VISUAL;
	pid = vfork();
	if (pid == 0) {
		sigchild();
		if (sig != SIG_IGN)
			signal(SIGINT, SIG_DFL);
		execlp(edit, edit, tempEdit, 0);
		perror(edit);
		_exit(1);
	}
	if (pid == -1) {
		perror("fork");
		remove(tempEdit);
		goto out;
	}
	while (wait(&s) != pid)
		;
	if ((s & 0377) != 0) {
		printf(MSGSTR(FERROR, "Fatal error in \"%s\"\n"), edit); /*MSG*/
		remove(tempEdit);
		goto out;
	}

	/*
	 * Now switch to new file.
	 */

	if ((fbuf = fopen(tempEdit, "a")) == NULL) {
		perror(tempEdit);
		remove(tempEdit);
		goto out;
	}
	if ((ibuf = fopen(tempEdit, "r")) == NULL) {
		perror(tempEdit);
		fclose(fbuf);
		remove(tempEdit);
		goto out;
	}
	remove(tempEdit);
	fclose(obuf);
	fclose(newi);
	obuf = fbuf;
	goto out;
fix:
	perror(tempEdit);
out:
#ifdef SIGCONT
	signal(SIGCONT, scont);
#endif SIGCONT
	signal(SIGINT, sig);
	newi = ibuf;
	return(obuf);
}

/*
 * Pipe the message through the command.
 * Old message is on stdin of command;
 * New message collected from stdout.
 * Sh -c must return 0 to accept the new message.
 */

FILE *
mespipe(ibuf, obuf, cmd)
	FILE *ibuf, *obuf;
	char cmd[];
{
	register FILE *ni, *no;
	int pid, s;
	void (*savesig)();
	char *Shell;
	extern char tempEdit[];

	newi = ibuf;
	if ((no = fopen(tempEdit, "w")) == NULL) {
		perror(tempEdit);
		return(obuf);
	}
	if ((ni = fopen(tempEdit, "r")) == NULL) {
		perror(tempEdit);
		fclose(no);
		remove(tempEdit);
		return(obuf);
	}
	remove(tempEdit);
	savesig = signal(SIGINT, SIG_IGN);
	fflush(obuf);
	rewind(ibuf);
	if ((Shell = value("SHELL")) == NULL)
		Shell = "/bin/sh";
	if ((pid = vfork()) == -1) {
		perror("fork");
		goto err;
	}
	if (pid == 0) {
		/*
		 * stdin = current message.
		 * stdout = new message.
		 */

		sigchild();
		close(0);
		dup(fileno(ibuf));
		close(1);
		dup(fileno(no));
		for (s = 4; s < 15; s++)
			close(s);
		execl(Shell, Shell, "-c", cmd, 0);
		perror(Shell);
		_exit(1);
	}
	while (wait(&s) != pid)
		;
	if (s != 0 || pid == -1) {
		fprintf(stderr, MSGSTR(FAILED, "\"%s\" failed!?\n"), cmd); /*MSG*/
		goto err;
	}
	if (fsize(ni) == 0) {
		fprintf(stderr, MSGSTR(NOBYTES, "No bytes from \"%s\" !?\n"), cmd); /*MSG*/
		goto err;
	}

	/*
	 * Take new files.
	 */

	newi = ni;
	fclose(ibuf);
	fclose(obuf);
	signal(SIGINT, savesig);
	return(no);

err:
	fclose(no);
	fclose(ni);
	signal(SIGINT, savesig);
	return(obuf);
}

/*
 * Execute a shell command and insert its standard output
 * into the message.
 */

readshell(obuf, cmd)
	FILE *obuf;
	char cmd[];
{
	void (*savesig)();
	int pid, s;
	char *Shell;
	extern char tempEdit[];

	savesig = signal(SIGINT, SIG_IGN);
	fflush(obuf);
	if ((Shell = value("SHELL")) == NULL)
		Shell = "/bin/sh";
	if ((pid = vfork()) == -1) {
		perror("fork");
		goto err;
	}
	if (pid == 0) {
		/*
		 * stdout = message
		 */

		sigchild();
		close(1);
		dup(fileno(obuf));
		for (s = 4; s < 15; s++)
			close(s);
		execl(Shell, Shell, "-c", cmd, 0);
		perror(Shell);
		_exit(1);
	}
	while (wait(&s) != pid)
		;
	if (s != 0 || pid == -1) {
		fprintf(stderr, MSGSTR(FAILED, "\"%s\" failed!?\n"), cmd); 
		goto err;
	}
	
	signal(SIGINT, savesig);
	return(0);
	
err:
	return(-1);
}

/*
 * Interpolate the named messages into the current
 * message, preceding each line with a tab.
 * Return a count of the number of characters now in
 * the message, or -1 if an error is encountered writing
 * the message temporary.  The flag argument is 'm' if we
 * should shift over and 'f' if not.
 */
forward(ms, obuf, f)
	char ms[];
	FILE *obuf;
{
	register int *msgvec, *ip;
	extern char tempMail[];

	msgvec = (int *) salloc((msgCount+1) * sizeof *msgvec);
	if (msgvec == (int *) NOSTR)
		return(0);
	if (getmsglist(ms, msgvec, 0) < 0)
		return(0);
	if (*msgvec == NULL) {
		*msgvec = first(0, MMNORM);
		if (*msgvec == NULL) {
			printf(MSGSTR(NOAMSGS, "No appropriate messages\n")); /*MSG*/
			return(0);
		}
		msgvec[1] = NULL;
	}
	printf(MSGSTR(INTER, "Interpolating:")); /*MSG*/
	for (ip = msgvec; *ip != NULL; ip++) {
		touch(*ip);
		printf(" %d", *ip);
		if (f == 'm') {
			if (transmit(&message[*ip-1], obuf) < 0L) {
				perror(tempMail);
				return(-1);
			}
		} else
			if (send(&message[*ip-1], obuf, 0) < 0) {
				perror(tempMail);
				return(-1);
			}
	}
	printf("\n");
	return(0);
}

/*
 * Send message described by the passed pointer to the
 * passed output buffer.  Insert a tab in front of each
 * line.  Return a count of the characters sent, or -1
 * on error.
 */

long
transmit(mailp, obuf)
	struct message *mailp;
	FILE *obuf;
{
	register struct message *mp;
	register int ch;
	long c, n;
	int bol;
	FILE *ibuf;

	mp = mailp;
	ibuf = setinput(mp);
	c = mp->m_size;
	n = c;
	bol = 1;
	while (c-- > 0L) {
		if (bol) {
			bol = 0;
			putc('\t', obuf);
			n++;
			if (ferror(obuf)) {
				perror("/tmp");
				return(-1L);
			}
		}
		ch = getc(ibuf);
		if (ch == '\n')
			bol++;
		putc(ch, obuf);
		if (ferror(obuf)) {
			perror("/tmp");
			return(-1L);
		}
	}
	return(n);
}

/*
 * Print (continue) when continued after ^Z.
 */
void
collcont(s)
{

	printf(MSGSTR(CONT, "(continue)\n")); /*MSG*/
	fflush(stdout);
}


void collrub(s)
{
	register FILE *dbuf;
	register int c;

	if (s == SIGINT && hadintr == 0) {
		hadintr++;
		fflush(stdout);
		fprintf(stderr, MSGSTR(INTR, "\n(Interrupt -- one more to kill letter)\n")); /*MSG*/
		if (signal(SIGINT, SIG_IGN) != SIG_IGN)
			signal(SIGINT, hf ? intack : collrub),
			    sigblock(sigmask(SIGINT));
		if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
			signal(SIGHUP, collrub), sigblock(sigmask(SIGHUP));
		if (Jmpbuf)
		    longjmp(*Jmpbuf, 1);
		return;
	}
	fclose(newo);
	rewind(newi);
	if (s == SIGINT && value("nosave") != NOSTR || fsize(newi) == 0)
		goto done;
	if ((dbuf = fopen(deadletter, "w")) == NULL)
		goto done;
	chmod(deadletter, 0600);
	while ((c = getc(newi)) != EOF)
		putc(c, dbuf);
	fclose(dbuf);
	fprintf(stderr, MSGSTR(LINTRP, "\n(Last Interrupt -- letter saved in %s)\n"), deadletter); /*MSG*/

done:
	fclose(newi);
	signal(SIGINT, savesig);
	signal(SIGHUP, savehup);
#ifdef SIGCONT
	signal(SIGCONT, savecont);
#endif SIGCONT
	if (rcvmode) {
		if (s == SIGHUP)
			hangup(SIGHUP);
		else
			stop(s);
	}
	else
		exit(1);
}

/*
 * Acknowledge an interrupt signal from the tty by typing an @
 */

void intack(s)
{
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, hf ? intack : collrub),
		    sigblock(sigmask(SIGINT));
	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
		signal(SIGHUP, collrub), sigblock(sigmask(SIGHUP));
	
	puts("@");
	fflush(stdout);
	clearerr(stdin);
	if (Jmpbuf)
	    longjmp(*Jmpbuf, 1);
}

/*
 * Add a string to the end of a header entry field.
 */

char *
addto(hf, news)
	char hf[], news[];
{
	register char *cp, *cp2, *linebuf;
	char flatnews[LINESIZE];

	if (hf == NOSTR)
		hf = "";
	if (*news == '\0')
		return(hf);
	NLflatstr (news, flatnews, LINESIZE);
	linebuf = salloc(strlen(hf) + strlen(flatnews) + 2);
	for (cp = hf; any(*cp, " \t"); cp++)
		;
	for (cp2 = linebuf; *cp;)
		*cp2++ = *cp++;
	*cp2++ = ' ';
	for (cp = flatnews; any(*cp, " \t"); cp++)
		;
	while (*cp != '\0')
		*cp2++ = *cp++;
	*cp2 = '\0';
	return(linebuf);
}
