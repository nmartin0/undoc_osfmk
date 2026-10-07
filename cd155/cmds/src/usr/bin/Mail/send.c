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
static char rcsid[] = "@(#)$RCSfile: send.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:18:26 $";
#endif
/* 
 * COMPONENT_NAME: CMDMAILX send.c
 * 
 * FUNCTIONS: MSGSTR, fixhead, fmt, headerp, infix, mail, mail1, 
 *            puthead, savemail, send, sendmail, statusput 
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
 *	send.c       5.2 (Berkeley) 6/21/85
 */

#include "rcv.h"
#include <sys/wait.h>
#include <ctype.h>
#include <sys/stat.h>

#include "Mail_msg.h" 
extern nl_catd catd;
extern short file_recip;
extern char  *recip_name;
#define MSGSTR(n,s) catgets(catd,MS_MAILX,n,s) 

/*
 * Mail -- a mail program
 *
 * Mail to others.
 */

/*
 * Send message described by the passed pointer to the
 * passed output buffer.  Return -1 on error, but normally
 * the number of lines written.  Adjust the status: field
 * if need be.  If doign is set, suppress ignored header fields.
 */
send(mailp, obuf, doign)
	struct message *mailp;
	FILE *obuf;
{
	register struct message *mp;
	register int t;
	long c;
	FILE *ibuf;
	char line[LINESIZE], field[BUFSIZ];
	int lc, ishead, infld, fline, dostat;
	char *cp, *cp2;

	mp = mailp;
	ibuf = setinput(mp);
	c = mp->m_size;
	ishead = 1;
	dostat = 1;
	infld = 0;
	fline = 1;
	lc = 0;
	while (c > 0L) {
		fgets(line, LINESIZE, ibuf);
		c -= (long) strlen(line);
		lc++;
		if (ishead) {
			/* 
			 * First line is the From line, so no headers
			 * there to worry about
			 */
			if (fline) {
				fline = 0;
				goto writeit;
			}
			/*
			 * If line is blank, we've reached end of
			 * headers, so force out status: field
			 * and note that we are no longer in header
			 * fields
			 */
			if (line[0] == '\n') {
				if (dostat) {
					statusput(mailp, obuf, doign);
					dostat = 0;
				}
				ishead = 0;
				goto writeit;
			}
			/*
			 * If this line is a continuation (via space or tab)
			 * of a previous header field, just echo it
			 * (unless the field should be ignored).
			 */
			if (infld && (isspace(line[0]) || line[0] == '\t')) {
				if (doign && isign(field)) continue;
				goto writeit;
			}
			infld = 0;
			/*
			 * If we are no longer looking at real
			 * header lines, force out status:
			 * This happens in uucp style mail where
			 * there are no headers at all.
			 */
			if (!headerp(line)) {
				if (dostat) {
					statusput(mailp, obuf, doign);
					dostat = 0;
				}
				putc('\n', obuf);
				ishead = 0;
				goto writeit;
			}
			infld++;
			/*
			 * Pick up the header field.
			 * If it is an ignored field and
			 * we care about such things, skip it.
			 */
			cp = line;
			cp2 = field;
			while (*cp && *cp != ':' && !isspace(*cp))
				*cp2++ = *cp++;
			*cp2 = 0;
			if (doign && isign(field))
				continue;
			/*
			 * If the field is "status," go compute and print the
			 * real Status: field
			 */
			if (icequal(field, "status")) {
				if (dostat) {
					statusput(mailp, obuf, doign);
					dostat = 0;
				}
				continue;
			}
		}
writeit:
		fputs(line, obuf);
		if (ferror(obuf))
			return(-1);
	}
	if (ferror(obuf))
		return(-1);
	if (ishead && (mailp->m_flag & MSTATUS))
		printf(MSGSTR(BADSTATUS, "failed to fix up status field\n")); /*MSG*/
	return(lc);
}

/*
 * Test if the passed line is a header line, RFC 733 style.
 */
headerp(line)
	register char *line;
{
	register char *cp = line;

	while (*cp && !isspace(*cp) && *cp != ':')
		cp++;
	while (*cp && isspace(*cp))
		cp++;
	return(*cp == ':');
}

/*
 * Output a reasonable looking status field.
 * But if "status" is ignored and doign, forget it.
 */
statusput(mp, obuf, doign)
	register struct message *mp;
	register FILE *obuf;
{
	char statout[3];

	if (doign && isign("status"))
		return;
	if ((mp->m_flag & (MNEW|MREAD)) == MNEW)
		return;
	if (mp->m_flag & MREAD)
		strcpy(statout, "R");
	else
		strcpy(statout, "");
	if ((mp->m_flag & MNEW) == 0)
		strcat(statout, "O");
	fprintf(obuf, MSGSTR(STATUS, "Status: %s\n"), statout); /*MSG*/
}


/*
 * Interface between the argument list and the mail1 routine
 * which does all the dirty work.
 */

mail(people)
	char **people;
{
	register char *cp2;
	register int s;
	char *buf, **ap;
	struct header head;

	for (s = 0, ap = people; *ap != (char *) 0; ap++)
		s += strlen(*ap) + 1;
	buf = salloc(s+1);
	cp2 = buf;
	for (ap = people; *ap != (char *) 0; ap++) {
		cp2 = copy(*ap, cp2);
		*cp2++ = ' ';
	}
	if (cp2 != buf)
		cp2--;
	*cp2 = '\0';
	head.h_to = buf;
	head.h_subject = NOSTR;
	head.h_cc = NOSTR;
	head.h_bcc = NOSTR;
	head.h_seq = 0;
	mail1(&head, 0, 0);
	return(0);
}


/*
 * Send mail to a bunch of user names.  The interface is through
 * the mail routine below.
 */

sendmail(str)
	char *str;
{
	register char **ap;
	char *bufp;
	register int t;
	struct header head;

	if (blankline(str))
		head.h_to = NOSTR;
	else
		head.h_to = str;
	head.h_subject = NOSTR;
	head.h_cc = NOSTR;
	head.h_bcc = NOSTR;
	head.h_seq = 0;
	mail1(&head, 0, 0);
	return(0);
}

/*
 * Mail a message on standard input to the people indicated
 * in the passed header.  (Internal interface).
 */

mail1(hp, savemsg, filename)
	struct header *hp;
	int    savemsg;
	char   *filename;
{
	register char *cp;
	int pid, i, s, p, gotcha;
	char **namelist, *deliver, *ccp;
	struct name *to, *np;
	struct stat sbuf;
	FILE *mtf, *postage;
	int remote = rflag != NOSTR || rmail;
	char **t;
	char buf[BUFSIZ];
	char fold_dir[BUFSIZ];


	/*
	 * Collect user's mail from standard input.
	 * Get the result as mtf.
	 */

	pid = -1;
	if ((mtf = collect(hp)) == NULL)
		return(-1);
	hp->h_seq = 1;
	if (hp->h_subject == NOSTR)
		hp->h_subject = sflag;

	/*
	 * Now, take the user names from the combined
	 * to and cc lists and do all the alias
	 * processing.
	 */

	senderr = 0;
	to = usermap(cat(extract(hp->h_bcc, GBCC),
	    cat(extract(hp->h_to, GTO), extract(hp->h_cc, GCC))));
	if (to == NIL) {
		printf(MSGSTR(NORECIPS, "No recipients specified\n")); /*MSG*/
		goto topdog;
	}

	/*
	 * Look through the recipient list for names with /'s
	 * in them which we write to as files directly.
	 */

	to = outof(to, mtf, hp);
	rewind(mtf);
	to = verify(to);
	if (senderr && !remote) {
topdog:

		if (fsize(mtf) != 0) {
			remove(deadletter);
			exwrite(deadletter, mtf, 1);
			rewind(mtf);
		}
	}
	for (gotcha = 0, np = to; np != NIL; np = np->n_flink)
		if ((np->n_type & GDEL) == 0) {
			gotcha++;
			break;
		}
	if (!gotcha)
		goto out;
	to = elide(to);
	mechk(to);
	if (count(to) > 1)
		hp->h_seq++;
	if (hp->h_seq > 0 && !remote) {
		fixhead(hp, to);
		if (fsize(mtf) == 0)
		    if (hp->h_subject == NOSTR)
			printf(MSGSTR(NOINFO, "No message, no subject; hope that's ok\n")); /*MSG*/
		    else
			printf(MSGSTR(NOTEXT, "Null message body; hope that's ok\n")); /*MSG*/
		if ((mtf = infix(hp, mtf)) == NULL) {
			fprintf(stderr, MSGSTR(LOST, ". . . message lost, sorry.\n")); /*MSG*/
			return(-1);
		}
	}
	namelist = unpack(to);
	if (debug) {
		printf("Recipients of message:\n");
		for (t = namelist; *t != NOSTR; t++)
			printf(" \"%s\"", *t);
		printf("\n");
		fflush(stdout);
		return;
	}

        /*
         * If followup was the original caller then save a copy of the reply
         * message in a file named after the person being replied to.
         */

        if (savemsg == TRUE) {
            sprintf(buf, "%s/%s", homedir, filename);
            cp=buf;
            savemail(expand(cp), hp, mtf);
        }
        else if ((ccp = value("record")) != NOSTR || file_recip == TRUE) {
                if (file_recip == TRUE)
                        ccp = recip_name;

                /*
                 * If the record environment variable contains a full
                 * pathname, then store the msg in there instead of
                 * in the user's home directory.
                 */

                if (ccp[0] == '/') {
                    cp = ccp;
                }
                else {
                    if ((value("outfolder") != NOSTR) &&
                        (getfold(fold_dir) == 0))
                        sprintf(buf, "%s/%s", fold_dir, ccp);
                    else
                        sprintf(buf,"%s/%s",homedir,ccp);
                    cp=buf;
                }
                savemail(expand(cp), hp, mtf);
        }

	/*
	 * Wait, to absorb a potential zombie, then
	 * fork, set up the temporary mail file as standard
	 * input for "mail" and exec with the user list we generated
	 * far above. Return the process id to caller in case he
	 * wants to await the completion of mail.
	 */

	while (wait3((union wait *)&s, WNOHANG, (struct rusage *)0) > 0)
		;
	rewind(mtf);
	pid = fork();
	if (pid == -1) {
		perror("fork");
		remove(deadletter);
		exwrite(deadletter, mtf, 1);
		goto out;
	}
	if (pid == 0) {
		sigchild();
#ifdef SIGTSTP
		if (remote == 0) {
			signal(SIGTSTP, SIG_IGN);
			signal(SIGTTIN, SIG_IGN);
			signal(SIGTTOU, SIG_IGN);
		}
#endif
		for (i = SIGHUP; i <= SIGQUIT; i++)
			signal(i, SIG_IGN);
		if (!stat(POSTAGE, &sbuf))
			if ((postage = fopen(POSTAGE, "a")) != NULL) {
				fprintf(postage, "%s %d %d\n", myname,
				    count(to), fsize(mtf));
				fclose(postage);
			}
		s = fileno(mtf);
		for (i = 3; i < 15; i++)
			if (i != s)
				close(i);
		close(0);
		dup(s);
		close(s);
#ifdef CC
		submit(getpid());
#endif CC
#ifdef SENDMAIL
		deliver = getenv ("sendmail");
		if (deliver == NOSTR)
			deliver = getenv ("SENDMAIL");
		if (deliver == NOSTR)
			deliver = value("sendmail");
		if (deliver == NOSTR)
			deliver = SENDMAIL;
		execv (deliver, namelist);
		perror(deliver);
#endif SENDMAIL
		execv(MAIL, namelist);
		perror(MAIL);
		exit(1);
	}

out:
	if (remote || (value("verbose") != NOSTR) || 
	    (value("sendwait") != NOSTR)) {
		p = waitpid(pid, &s, 0);
		if (s != 0)
			senderr++;
		pid = 0;
	}
	fclose(mtf);
	return(pid);
}

/*
 * Fix the header by glopping all of the expanded names from
 * the distribution list into the appropriate fields.
 * If there are any ARPA net recipients in the message,
 * we must insert commas, alas.
 */

fixhead(hp, tolist)
	struct header *hp;
	struct name *tolist;
{
	register struct name *nlist;
	register int f;
	register struct name *np;

	for (f = 0, np = tolist; np != NIL; np = np->n_flink)
		if (any('@', np->n_name)) {
			f |= GCOMMA;
			break;
		}

	if (debug && f & GCOMMA)
		fprintf(stderr, "Should be inserting commas in recip lists\n");
	hp->h_to = detract(tolist, GTO|f);
	hp->h_cc = detract(tolist, GCC|f);
}

/*
 * Prepend a header in front of the collected stuff
 * and return the new file.
 */

FILE *
infix(hp, fi)
	struct header *hp;
	FILE *fi;
{
	extern char tempMail[];
	register FILE *nfo, *nfi;
	register int c, ck;

	rewind(fi);
	if ((nfo = fopen(tempMail, "w")) == NULL) {
		perror(tempMail);
		return(fi);
	}
	if ((nfi = fopen(tempMail, "r")) == NULL) {
		perror(tempMail);
		fclose(nfo);
		return(fi);
	}
	remove(tempMail);
	puthead(hp, nfo, GTO|GSUBJECT|GCC|GNL);
	c = getc(fi);
	while (c != EOF) {
		ck = putc(c, nfo);
		if (ck == EOF) {
		perror(tempMail);
		fclose(nfo);
		fclose(nfi);
		return(fi);
		}
		c = getc(fi);
	}
	if (ferror(fi)) {
		perror("read");
		return(fi);
	}
	fflush(nfo);
	if (ferror(nfo)) {
		perror(tempMail);
		fclose(nfo);
		fclose(nfi);
		return(fi);
	}
	fclose(nfo);
	fclose(fi);
	rewind(nfi);
	return(nfi);
}

/*
 * Dump the to, subject, cc header on the
 * passed file buffer.
 */

puthead(hp, fo, w)
	struct header *hp;
	FILE *fo;
{
	register int gotcha;
	char templine[LINESIZE];

	gotcha = 0;
	if (hp->h_to != NOSTR && w & GTO)
		fmt("To: ", hp->h_to, fo), gotcha++;
	if (hp->h_subject != NOSTR && w & GSUBJECT)
		{
		strcpy (templine, hp->h_subject);
		NLflatstr (templine, hp->h_subject, LINESIZE);
		fprintf(fo, "Subject: %s\n", hp->h_subject), gotcha++;
		}
	if (hp->h_cc != NOSTR && w & GCC)
		fmt("Cc: ", hp->h_cc, fo), gotcha++;
	if (hp->h_bcc != NOSTR && w & GBCC)
		fmt("Bcc: ", hp->h_bcc, fo), gotcha++;
	if (gotcha && w & GNL)
		putc('\n', fo);
	return(0);
}

/*
 * Format the given text to not exceed 72 characters.
 */

fmt(str, txt, fo)
	register char *str, *txt;
	register FILE *fo;
{
	register int col;
	register char *bg, *bl, *pt, ch;

	col = strlen(str);
	if (col)
		fprintf(fo, "%s", str);
	pt = bg = txt;
	bl = 0;
	while (*bg) {
		pt++;
		if (++col >72) {
			if (!bl) {
				bl = bg;
				while (*bl && !isspace(*bl))
					bl++;
			}
			if (!*bl)
				goto finish;
			ch = *bl;
			*bl = '\0';
			fprintf(fo, "%s\n    ", bg);
			col = 4;
			*bl = ch;
			pt = bg = ++bl;
			bl = 0;
		}
		if (!*pt) {
finish:
			fprintf(fo, "%s\n", bg);
			return;
		}
		if (isspace(*pt))
			bl = pt;
	}
}

/*
 * Save the outgoing mail on the passed file.
 */

savemail(name, hp, fi)
	char name[];
	struct header *hp;
	FILE *fi;
{
	register FILE *fo;
	register int c, ck;
	long now;
	char *n;

	if ((fo = fopen(name, "a")) == NULL) {
		perror(name);
		return(-1);
	}
	time(&now);
	n = rflag;
	if (n == NOSTR)
		n = myname;
	fprintf(fo, "From %s %s", n, ctime(&now));
	rewind(fi);
	for (c = getc(fi); c != EOF; c = getc(fi)) {
		ck = putc(c, fo);
		if (ck == EOF) {
		perror(name);
		break;
		}
	}
	fprintf(fo, "\n");
	fflush(fo);
	if (ferror(fo))
		perror(name);
	fclose(fo);
	return(0);
}
