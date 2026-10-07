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
static char rcsid[] = "@(#)$RCSfile: binmail.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:19:44 $";
#endif
/* 
 * COMPONENT_NAME: CMDMAILX binmail.c
 * 
 * FUNCTIONS: MSGSTR, Mbellmail, areforwarding, cat, copyback, 
 *            copylet, copymt, delete, done, getarg, isfrom, legal, 
 *            lock, notifybiff, printmail, savdead, send_copy, sendmail, 
 *            sendrmt, setsig, system, unlock 
 *
 * ORIGINS: 3  10  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 *	binmail.c	1.14  com/cmd/mailx,3.1,9013 3/14/90 13:16:00
 * 	binmail.c	5.1 17:50:33 8/15/90 SecureWare 
 */

/*
 * binmail: Sends messages to system users and displays 
 *           messages from system users.
 *
 *	Mail to a remote machine will normally use the uux command.
 *	If available, it may be better to send mail via nusend
 *	or usend, although delivery is not as reliable as uux.
 *	Mail may be compiled to take advantage
 *	of these other networks by adding:
 *		#define USE_NUSEND  for nusend
 *	and
 *		#define USE_USEND   for usend.
 *
 *	NOTE:  If either or both defines are specified, that network
 *	will be tried before uux.
 *
 *	If the '-I' command line option (undocumented) is given,
 *	remote mail is passed to the 'INmail' mailing system for delivery
 *	(via 'iscrmail', 'sendmail', and 'qftp').
 *	DO NOT USE the '-I' option if INmail is not installed;
 *	your mail will not be delivered.
 */

#include	<sys/secdefines.h>
#if SEC_BASE
#include	<sys/security.h>
#endif

#include	<stdio.h>
#include	<locale.h>
#include	<sys/syslog.h>
#include	<sys/types.h>
#include	<sys/errno.h>
#include	<signal.h>
#include	<pwd.h>
#include	<time.h>
#include	<utmp.h>
#include	<string.h>
#include	<sys/stat.h>
#include	<setjmp.h>
#include	<sys/utsname.h>
#include	<sys/param.h>
#include 	<dirent.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>

/*              include file for message texts          */
#include "binmail_msg.h" 
nl_catd  scmc_catd;   /* Cat descriptor for scmc conversion */
#define MSGSTR(Num,Str) catgets(scmc_catd,MS_binmail,Num,Str)

#define LOCKON	1	/* lock set */
#define LOCKOFF	0	/* lock released */
#define C_NOACCESS	0	/* no access to file */
#define CHILD	0
#define SAME	0

#define CERROR		-1
#define CSUCCESS	0

#define TRUE	1
#define FALSE	0

#define E_FLGE	1	/* exit flge error */
#define E_FILE	2	/* exit file error */
#define E_SPACE	3	/* exit no space */
/*
 * copylet flags
 */
#define	REMOTE		1		/* remote mail, add rmtmsg */
#define ORDINARY	2
#define ZAP		3		/* zap header and trailing empty line */
#define FORWARD		4

#define	LSIZE		256		/* maximum size of a line */
#define	MAXLET		300		/* maximum number of letters */
#define FROMLEVELS	20		/* maxium number of forwards */
#define MAXFILENAME	128

#ifndef	MFMODE
#define	MFMODE		0600		/* create mode for mailbox files */
#endif

#define A_OK		0		/* return value for access */
#define A_EXIST		0		/* access check for existence */
#define A_WRITE		2		/* access check for write permission */
#define A_READ		4		/* access check for read permission */
#undef 	sleep

static char BELL[] = "binmail";

#define NAME_MAX  255

struct	let	{
	long	adr;
	char	change;
} let[MAXLET];

struct	passwd	*getpwent();
struct	utsname utsn;

FILE	*tmpf, *malf;

char	lettmp[] = "/tmp/maXXXXX";
char	from[] = "From ";
#define	TO MSGSTR(M_MSG_51,"To: ")
static char *maildir = "/usr/spool/mail/";  /* default mailbox directory */
char	recip[256];

/*
 * fowrding address
 */
char	*mailfile;
char	maillock[] = ".lock";
char	dead[] = "/dead.letter";
char	rmtbuf[] = "/tmp/marXXXXXX";
#define RMTMSG	MSGSTR(M_MSG_52," remote from %s\n")
#define FORWMSG	MSGSTR(M_MSG_53," forwarded by %s\n")
char	frwrd[] = "Forward to ";
#define	nospace MSGSTR(M_MSG_55,"%s: no space for temp file\n")
char	*thissys;
char	mbox[] = "/mbox";
char	curlock[50] ={ '\0' };
char	line[LSIZE];
char	resp[LSIZE];
char	*hmbox;
char	*hmdead;
char	*home;
char	*my_name;
char	*getlogin();
char	*malloc();
char	lfil[50];
char	*ctime();
char	*getenv();
char	*strrchr();
char	*mktemp();

int	error;
int	fromlevel = 0;
int	nlet	= 0;
int	locked;
int	changed;
int	forward;
int	delete();
int	flgf = 0;
int	flgp = 0;
int	flge = 0;
int	INflag = 0;	/* use INmail for remote mail delivery */
int	delflg = 1;
int	toflg = 0;
int	savdead();
int	(*saveint)();
int	(*setsig())();

long	ftell();
long	iop;

jmp_buf	sjbuf;

#ifdef USE_NUSEND
jmp_buf nusendfail;
int	nusendjmp = FALSE;
#endif

#ifdef USE_USEND
jmp_buf usendfail;
int	usendjmp = FALSE;
#endif

unsigned umsave;

static char emalloc[] = "%s: can't malloc\n";
static char bl_chown[] = "%s: error changing ownership of file";

/* this notifies the "biff" server (comsat) that mail has arrived
   for the username */

static notifybiff(msg)
char *msg;
{

static struct sockaddr_in addr;
static int f = -1;
struct hostent *hp;
struct servent *sp;


	/* get address of biff server only once per invokation */

	if (addr.sin_family == 0) {  /* first pass: get service address */


		if (! (hp = gethostbyname("localhost")))
		    syslog(LOG_MAIL | LOG_WARNING, MSGSTR(M_MSG_66,
			"localhost name is not defined\n"));
		else {

		    if (! (sp = getservbyname("biff", "udp")))
			syslog(LOG_MAIL | LOG_INFO, MSGSTR(M_MSG_67,
			    "biff service is not defined in /etc/services\n"));
		    else {  /* got it ok */
			addr.sin_family = hp->h_addrtype;
			bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
			addr.sin_port = sp->s_port;

		    }
		}
	}

	/* get socket (if first pass) and send message */

	if (addr.sin_family) {

		if (f < 0 && ((f = socket(AF_INET, SOCK_DGRAM, 0)) == -1))
			syslog(LOG_MAIL | LOG_ERR, MSGSTR(M_MSG_68,
			    "error getting biff socket: %m\n"));
		else {

		    if (sendto(f, msg, strlen(msg)+1, 0, &addr,
			sizeof (addr)) == -1)
			    syslog(LOG_MAIL | LOG_ERR, MSGSTR(M_MSG_69,
				"error sending message to biff: %m\n"));
		}
	}
}


/*
 *	mail [ + ] [ -irpqet ]  [ -f file ]
 *	mail [ -t ] persons 
 *	rmail [ -t ] persons 
 */
main(argc, argv)
char	**argv;
{
	register i;
	register char *pnew;

	(void ) setlocale(LC_ALL,"");
	scmc_catd = catopen(MF_BINMAIL,0);
#if SEC_BASE
	mail_init(argc, argv);
#endif
#if SEC_BASE
	mail_init(argc, argv);
#endif
	umsave = umask(7);
	setbuf(stdout, malloc(BUFSIZ));
	uname(&utsn);
	thissys = utsn.nodename;
	pnew = strchr(thissys,'.');
	if (pnew != NULL)
		*pnew = '\0';
#if SEC_BASE
	mail_identity(&my_name, &home);
#else
	/* 
	 * Use environment variables
	 * logname is used for from
	 */
	if(((home = getenv("HOME")) == NULL) || (strlen(home) == 0))
		home = ".";
	if (((my_name = getenv("LOGNAME")) == NULL) || (strlen(my_name) == 0))
		my_name = getlogin();
	if ((my_name == NULL) || (strlen(my_name) == 0))
		my_name = getpwuid(geteuid())->pw_name;
#endif
	/*
	 * Catch signals for cleanup
	 */
	if(setjmp(sjbuf)) 
		done();
        for (i=SIGINT; i<SIGCHLD; i++)
		setsig(i, delete);
	setsig(SIGHUP, done);
	/*
	 * Make temporary file for letter
	 */
	mktemp(lettmp);
	unlink(lettmp);
	if((tmpf = fopen(lettmp, "w")) == NULL){
		fprintf(stderr,  MSGSTR(M_MSG_0, 
			"%s: can't open %s for writing\n"),BELL,lettmp);
		error = E_FILE;
		done();
	}
	/*
	 * Rmail always invoked to send mail
	 */
	if (argv[0][0] != 'r' &&	
		(argc == 1 || argv[1][0] == '-' || argv[1][0] == '+'))
		printmail(argc, argv);
	else {
		/* check for the special case of rmail -t */
		if (argv[0][0] == 'r' && (strcmp (argv[1],"-t") == 0))
		{
			toflg = 1;
			argv++;
			argc--;
		}
		sendmail(argc, argv);
	}
	done();
}

/*
 * Signal reset
 * signals that are not being ignored will be 
 * caught by function f
 *	i	-> signal number
 *	f	-> signal routine
 * return
 *	rc	-> former signal
 */
int (*setsig(i, f))()
int	i;
void	(*f)();
{
	register int (*rc)();

	if((rc = (int (*)()) signal(i,SIG_IGN))!= (int (*)()) SIG_IGN)
		signal(i, f);
	return(rc);
}

/*
 * Print mail entries
 *	argc	-> argument count
 *	argv	-> arguments
 */
printmail(argc, argv)
char	**argv;
{
	register int c;
	int	okerr = 0, flgd = 0, flg, i, j, print, aret, stret, goerr = 0;
	char	*p, *getarg();
	char	frwrdbuf[256];
	struct	stat stbuf;
	extern	char *optarg;
	extern int optind;

	/*
	 * Print in reverse order
	 */
	if (argc > 1 && argv[1][0] == '+') {
		forward = 1;
		argc--;
		argv++;
	}
	while((c = getopt(argc, argv, "d:f:rpqietI")) != EOF)
		switch(c) {

		/*
		 * set system mailbox directory
		 */
		case 'd':
			flgd = 1;
			if (! (maildir = malloc((i = strlen(optarg)) + 2))) {
				fprintf(stderr,MSGSTR(M_MSG_33, emalloc),BELL);
				error = E_FILE;
				done();
			}
			strcpy(maildir, optarg);
			/* make sure it ends with '/' */
			if (maildir[i - 1] != '/')
				strcat(maildir, "/");
			break;
		/*
		 * use file input for mail
		 */
		case 'f':
			flgf = 1;
			mailfile = optarg;
			break;
		/* 
		 * print without prompting
		 */
		case 'p':
			flgp++;
		/* 
		 * terminate on deletes
		 */
		case 'q':
			delflg = 0;
		case 'i':
			break;

		/* 
		 * print by first in, first out order
		 */
		case 'r':
			forward = 1;
			break;
		/*
		 * do  not print mail
		 */
		case 'e':
			flge = 1;
			break;

		case 't':
			toflg = 1;
			break;
		 /*
		  * use 'INmail' to deliver remote mail.
		  */
		case 'I':
			INflag = 1;
			break;
		/*
		 * bad option
		 */
		case '?':
			goerr++;
	}
	if(goerr || (flgd && flgf)) {
		fprintf(stderr,  MSGSTR(M_MSG_1,
		    "usage: %s [-erpqt] [-d directory | -f file] [persons]\n"),
		    BELL);
		error = E_FILE;
		done();

	}
	if (i = argc - optind) {  /* users given on cmd line: we are sending */
		sendmail(i + 1, argv + optind - 1);
		done();
	}
#if SEC_MAC
	if (!flge && !mail_check_proc_level()) {
		fprintf(stderr, "%s: log in at your clearance to read mail\n",
			BELL);
		error = E_FILE;
		done();
	}
#endif

	/*
	 * create working directory mbox name
	 */
	if((hmbox = malloc(strlen(home) + strlen(mbox) + 1)) == NULL){
		fprintf(stderr,MSGSTR(M_MSG_33, emalloc),BELL);
		error = E_FILE;
		return;
	}
	cat(hmbox, home, mbox);
	/* check for MAIL env var if -d and -f not given */
	if (flgf || (! flgd && ((mailfile = getenv("MAIL")) && *mailfile))) {
		/* set up maildir from mailfile */
		if (! (maildir = malloc(strlen(mailfile)))) {
			fprintf(stderr,MSGSTR(M_MSG_33, emalloc),BELL);
			error = E_FILE;
			return;
		}
		if (p = strrchr(strcpy(maildir, mailfile), '/'))
		    *p = '\0';  /* truncate at last '/' */

	} else {  /* set mailfile from given or default dir, and username */
		if (! (mailfile = malloc(strlen(maildir) + strlen(my_name)
		    + 1))) {
			fprintf(stderr,MSGSTR(M_MSG_33, emalloc),BELL);
			error = E_FILE;
			return;
		}
		cat(mailfile, maildir, my_name);
	}
	/*
	 * Check accessibility of mail file
	 */
	if (access(mailfile, A_READ) || ! (malf = fopen(mailfile, "r"))) {
		if (errno == ENOENT && ! flgf)  /* ok if it's not there */
			++okerr;
		else {  /* real error */
			fprintf(stderr,MSGSTR(M_MSG_5,
				"%s: cannot open %s: "), BELL, mailfile);
			perror("");
			error = E_FILE;
			return;
		}
	}
	if (okerr || (! stat(mailfile, &stbuf) && stbuf.st_size == 0)) {
		/* also ok if it's empty */
		if (!flge)
			printf(MSGSTR(M_MSG_6,"No mail.\n") );
		error = E_FLGE;
		return;
	}

	lock(mailfile);
	/*
	 * See if mail is to be forwarded to 
	 * another system
	 */
	if(areforwarding(mailfile)) {
		if(flge) {
			unlock();
			error = E_FLGE;
			return;
		}
		printf(MSGSTR(M_MSG_7, "Your mail is being forwarded to ") );
		fseek(malf, (long)(sizeof(frwrd) - 1), 0);
		fgets(frwrdbuf, sizeof(frwrdbuf), malf);
		printf("%s", frwrdbuf);
		if(getc(malf) != EOF)
			printf(MSGSTR(M_MSG_9,
			    "and your mailbox contains extra stuff\n") );
		unlock();
		return;
	}
	if(flge) {
		unlock();
		return;
	}
#if SEC_MAC
	if (!mail_check_file(fileno(malf))) {
		fprintf(stderr, "%s: mailbox (%s) is not at your clearance\n",
			BELL, mailfile);
		error = E_FILE;
		done();
	}
#endif
	/*
	 * copy mail to temp file and mark each
	 * letter in the let array
	 */
	copymt(malf, tmpf);
	fclose(malf);
	fclose(tmpf);
	unlock();
	if((tmpf = fopen(lettmp, "r")) == NULL) {
	        fprintf(stderr,MSGSTR(M_MSG_10,
			"%s: can't open %s\n"),BELL,lettmp);
		error = E_FILE;
		return;
	}
	changed = 0;
	print = 1;
	for (i = 0; i < nlet; ) {

		/*
		 * reverse order ?
		 */
		j = forward ? i : nlet - i - 1;
		if( setjmp(sjbuf) == 0 && print != 0 )
				copylet(j, stdout, ORDINARY);
		
		/*
		 * print only
		 */
		if(flgp) {
			i++;
			continue;
		}
		/*
		 * Interactive
		 */
		setjmp(sjbuf);
		printf("? ");
		fflush(stdout);
		if (fgets(resp, sizeof(resp), stdin) == NULL)
			break;
		print = 1;
		switch (resp[0]) {

		default:
			printf(MSGSTR(M_MSG_12,"usage\n") );

		/*
		 * help
		 */
		case '?':
			print = 0;
			printf(MSGSTR(M_MSG_56,"q\t\tquit\n"));
			printf(MSGSTR(M_MSG_57,
				"x\t\texit without changing mail\n"));
			printf(MSGSTR(M_MSG_58,"p\t\tprint\n"));
			printf(MSGSTR(M_MSG_59,
				"s [file]\tsave (default mbox)\n"));
			printf(MSGSTR(M_MSG_60,
				"w [file]\tsame without header\n"));
			printf(MSGSTR(M_MSG_61,"-\t\tprint previous\n"));
			printf(MSGSTR(M_MSG_62,"d\t\tdelete\n"));
			printf(MSGSTR(M_MSG_63,"+\t\tnext (no delete)\n"));
			printf(MSGSTR(M_MSG_64,"m [user]\tmail to user\n"));
			printf(MSGSTR(M_MSG_65,"! cmd\t\texecute cmd\n"));
			break;
		/*
		 * skip entry
		 */
		case '+':

		case 'n':
		case '\n':
			i++;
		case 'p':
			break;
		case 'x':
			changed = 0;
		case 'q':
			goto donep;
		/*
		 * Previous entry
		 */
		case '^':
		case '-':
			if (--i < 0)
				i = 0;
			break;
		/*
		 * Save in file without header
		 */
		case 'y':
		case 'w':
		/*
		 * Save mail with header
		 */
		case 's':
			if (resp[1] == '\n' || resp[1] == '\0')
				cat(resp+1, hmbox, "");
			else if(resp[1] != ' ') {
				printf(MSGSTR(M_MSG_14,"invalid command\n"));
				print = 0;
				continue;
			}
			umask(umsave);
#if SEC_MAC
			/* Revert to original privileges */
			mail_revert_privs();
#endif
			flg = 0;
			for (p = resp+1; (p = getarg(lfil, p)) != NULL; ) {
				if((aret=legal(lfil)))
					malf = fopen(lfil, "a");
				if ((malf == NULL) || (aret == 0)) {
					fprintf(stderr,  MSGSTR(M_MSG_15,
					  "%s: cannot append to %s\n")
						,BELL,lfil);
					flg++;
					continue;
				}
				if (aret==2 &&
					chown(lfil, getuid(), getgid())) {
					    sprintf(frwrdbuf, MSGSTR(BL_CHOWN,
						bl_chown), BELL);
					    perror(frwrdbuf);
					    flg++;
					    continue;
				}
				if (copylet(j, malf, resp[0]=='w'? ZAP: ORDINARY) == FALSE) {
					fprintf(stderr,MSGSTR(M_MSG_16,
					  "%s: cannot save mail\n"),BELL);
				}
				fclose(malf);
			}
#if SEC_MAC
			/* Restore raised privileges */
			mail_revert_privs();
#endif
			umask(7);
			if (flg)
				print = 0;
			else {
				let[j].change = 'd';
				changed++;
				i++;
			}
			break;
		/*
		 * Mail letter to someone else
		 */
		case 'm':
			if (resp[1] == '\n' || resp[1] == '\0') {
				i++;
				continue;
			}
			if (resp[1] != ' ') {
				printf(MSGSTR(M_MSG_17,"invalid command\n") );
				print = 0;
				continue;
			}
			flg = 0;
			for (p = resp+1; (p = getarg(lfil, p)) != NULL; )
				if (sendrmt(j, lfil) == FALSE)
					flg++;
			if (flg)
				print = 0;
			else {
				let[j].change = 'd';
				changed++;
				i++;
			}
			break;
		/*
		 * Escape to shell
		 */
		case '!':
			system(resp+1);
			printf("!\n");
			print = 0;
			break;
		/*
		 * Delete an entry
		 */
		case 'd':
			let[j].change = 'd';
			changed++;
			i++;
			if (resp[1] == 'q')
				goto donep;
			break;
		}
	}
	/*
	 * Copy updated mail file back
	 */
   donep:
	if (changed)
		copyback();
}

/*
 * copy temp or whatever back to mailbox directory
 */
copyback()
{
	register i, n, c;
	int new = 0, aret;
	struct stat stbuf;

	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	lock(mailfile);
	stat(mailfile, &stbuf);
	/*
	 * Has new mail arrived?
	 */
	if (stbuf.st_size != let[nlet].adr) {
		if((malf = fopen(mailfile, "r")) == NULL) {
			fprintf(stderr,MSGSTR(M_MSG_19, 
			   "%s: can't re-read %s\n"),BELL,mailfile);
			error = E_FILE;
			done();
		}
		fseek(malf, let[nlet].adr, 0);
		fclose(tmpf);
		if((tmpf = fopen(lettmp, "a")) == NULL) {
			fprintf(stderr,MSGSTR(M_MSG_20,
				"%s: can't re-write %s\n"),BELL,mailfile);
			error = E_FILE;
			done();
		}

		/*
		 * append new mail
		 * assume only one
		 * new letter
		 */
		while ((c = fgetc(malf)) != EOF)
			if (fputc(c, tmpf) == EOF) {
				fclose(malf);
				fclose(tmpf);
				fprintf(stderr, nospace,BELL);
				error = E_SPACE;
				done();
			}
		fclose(malf);
		fclose(tmpf);
		if((tmpf = fopen(lettmp, "r")) == NULL) {
			fprintf(stderr,MSGSTR(M_MSG_21,
			  "%s: can't re-read %s\n"),BELL,lettmp);
			error = E_FILE;
			done();
		}
		if(nlet == (MAXLET-2)){
			fprintf(stderr,MSGSTR(M_MSG_22,
				"copyback: Too many letters\n") );
			error = E_SPACE;
			done();
		}
		let[++nlet].adr = stbuf.st_size;
		new = 1;
	}
	/*
	 * Copy mail back to mail file
	 */
	if((aret=access(mailfile, A_WRITE)) == A_OK)
		malf = fopen(mailfile, "w");
	if ((malf == NULL) || (aret == CERROR)) {
		fprintf(stderr,MSGSTR(M_MSG_23, 
			"%s: can't rewrite %s\n"),BELL,mailfile);
		error = E_FILE;
		done();
	}
	n = 0;
	for (i = 0; i < nlet; i++)
		if (let[i].change != 'd') {
			if (copylet(i, malf, ORDINARY) == FALSE) {
				fprintf(stderr,MSGSTR(M_MSG_24,
				  "%s: cannot copy mail back\n"),BELL);
			}
			n++;
		}
	fclose(malf);


	if (new && !flgf)
		printf(MSGSTR(M_MSG_25,"new mail arrived\n") );
	unlock();
}

/*
 * copy mail (f1) to temp (f2)
 */
copymt(f1, f2)	
register FILE *f1, *f2;
{
	long nextadr;

	nlet = nextadr = 0;
	let[0].adr = 0;
	while (fgets(line, sizeof(line), f1) != NULL) {

		/*
		 * bug nlet should be checked
		 */
		if(line[0] == from[0])
			if (isfrom(line)){
				if(nlet >= (MAXLET-2)){
					fclose(f1);
					fclose(f2);
					fprintf(stderr, MSGSTR(M_MSG_26,
					  	"copymt: Too many letters\n") );
					error = E_SPACE;
					done();
				}
				let[nlet++].adr = nextadr;
			}
		nextadr += strlen(line);
		if (fputs(line, f2) == EOF) {
			fclose(f1);
			fclose(f2);
			fprintf(stderr, nospace,BELL);
			error = E_SPACE;
			done();
		}
	}

	/*
	 * last plus 1
	 */
	let[nlet].adr = nextadr;
}

/*
 * check to see if mail is being forwarded
 *	s	-> mail file
 * returns
 *	TRUE	-> forwarding
 *	FALSE	-> local
 */
areforwarding(s)
char *s;
{
	register char *p;
	register int c;
	FILE *fd;
	char fbuf[256];
	if((fd = fopen(s, "r")) == NULL) {

	/* NLS uglyness:  mail allows forwarding without
	   requiring the user of the forwarding mechanism to
	   actually have a login on the local machine.  
	   To support this AND flattened names we read
	   the directory and flatten all the file names
	   looking for a match.  If there is more than
	   one match return FALSE with a message to stderr.
	*/

	   
		DIR *pdir;
		char filename[NAME_MAX+1];
		char flatuser[NAME_MAX+1];
		char flatfl[NAME_MAX+1];
		char mtch[NAME_MAX+1];
		char mbuf[256];
		char *ds;
		struct dirent *dentry;
		int i, j=0;
		p = strrchr(s,'/');
#ifndef KJI
        	NLflatstr (++p, flatuser, NAME_MAX+1);
#endif
		if ( strcmp(p, flatuser) != SAME ) {
			return(FALSE);
		} 
		if ((pdir = opendir(maildir)) == NULL) {
			return(FALSE);
		}
		while ((dentry = readdir(pdir)) != NULL) {
			if (dentry->d_fileno == 0)
				continue;
			for (i = 0, ds = dentry->d_name; i < NAME_MAX; i++)
				if ((filename[i] = *ds++) == '\0')
					break;
			filename[NAME_MAX] = '\0';
#ifndef KJI
        		NLflatstr (filename, flatfl, NAME_MAX+1);
#endif
			if ( strcmp(p, flatfl) == SAME ) {
				strcpy(mtch, filename);
				j++;
			} 
			
		}
		closedir(pdir);
		switch (j) {
			case 0:
				return(FALSE);
				break;
			case 1:
				strcpy(mbuf,maildir);
				strcat(mbuf,mtch);
				if (! (fd = fopen(mbuf, "r"))) {
					perror(mbuf);
					error = E_FILE;
					done();
				}
				break;
			default:
				fprintf(stderr,MSGSTR(M_MSG_27,
					"%s: flattened name %s ambiguous\n")
						,BELL,p);
				return(FALSE);
				break;
			
		}
	}
			
			
	fbuf[0] = '\0';
	fread(fbuf, sizeof(frwrd) - 1, 1, fd);
	if(strncmp(fbuf, frwrd, sizeof(frwrd) - 1) == SAME) {
		for(p = recip; (c = getc(fd)) != EOF && c != '\n';)
			if(c != ' ') 
			*p++ = c;
		*p = '\0';
		fclose(fd);
		return(TRUE);
	}
	fclose(fd);
	return(FALSE);
}

/*
 * copy letter 
 *	ln	-> index into: letter table
 *	f	-> file descrptor to copy file to
 *	type	-> copy type
 */
copylet(ln, f, type) 
register FILE *f;
{
	register int i;
	register char *s;
	char	buf[512], lastc, ch;
	int	k, n, j;
	int	num;

	fseek(tmpf, let[ln].adr, 0);
	k = let[ln+1].adr - let[ln].adr;
	if(k)
		k--;
	for(i=0;i<k;){
		s = buf;
		num = ((k-i) > sizeof(buf))?sizeof(buf):(k-i);
		if((n = read(tmpf->_file, buf, num)) < 0){
		}
		lastc = buf[n-1];
		if(i == 0){
			for(j=0;j<n;j++,s++){
				if(*s == '\n')
					break;
			}
			if(type != ZAP)
				if(write(f->_file,buf,j) == CERROR){
				return(FALSE);
				}
			i += j+1;
			n -= j+1;
			ch = *s++;
			switch(type){
			case REMOTE:
				fprintf(f, RMTMSG, thissys);
				break;
			case ORDINARY:
				fprintf(f, "%c", ch);
				break;
			case FORWARD:
				fprintf(f, FORWMSG, my_name);
				break;
			}
			fflush(f);
		}
		if(write(f->_file, s, n) != n){
			return(FALSE);
		}
		i += n;
	}
	if(type != ZAP || lastc != '\n'){
		read(tmpf->_file, buf, 1);
		write(f->_file, buf, 1);
	}
	return(TRUE);
}

/*
 * check for "from" string
 *	lp	-> string to be checked
 * returns
 *	TRUE	-> match
 *	FALSE	-> no match
 */
isfrom(lp)
register char *lp;

{
	register char *p;

	for (p = from; *p; )
		if (*lp++ != *p++)
			return(FALSE);
	return(TRUE);
}

/*
 * Send mail
 *	argc	-> argument count
 *	argv	-> argument list
 */
sendmail(argc, argv)
char **argv;
{
	int	aret;
	char	**args;
	int	fromflg = 0;
	char	*asctime();
	struct	tm *bp, *localtime();

#define NLDATEFMT "%a %sD %sT %z %Y"
#define NLDATESIZE 27
	char	NLdate[NLDATESIZE];

	extern char *tzname[];

	/*
	 * Format time
	 */
	time(&iop);
	bp = localtime(&iop);
	NLstrtime(NLdate, NLDATESIZE, NLDATEFMT, bp);
	fprintf(tmpf, "%s%s %s\n", from, my_name, NLdate);
	/*
	 * Copy to list in mail entry?
	 */
	if (toflg == 1 && argc > 1) {
		aret = argc;
		args = argv;
		fprintf(tmpf,"%s ",TO);
		while(--aret > 0)
			fprintf(tmpf,"%s ",*++args);
		fputs("\n",tmpf);
	}
	iop = ftell(tmpf);
	flgf = 1;
	/*
	 * Read mail message
	 */
	saveint = setsig(SIGINT, savdead);
	fromlevel = 0;
	while (fgets(line, sizeof(line), stdin) != NULL) {
		if (line[0] == '.' && line[1] == '\n')
			break;
		/*
		 * If "from" string is present prepend with
		 * a ">" so it is no longer interpreted as
		 * the last system fowarding the mail.
		 */
		if(line[0] == from[0])
			if (isfrom(line))
				fputs(">", tmpf);
		/*
		 * Find out how many "from" lines
		 */
		if (fromflg == 0 && (strncmp(line, "From", 4) == SAME || strncmp(line, ">From", 5) == SAME))
			fromlevel++;
		else
			fromflg = 1;
		if (fputs(line, tmpf) == EOF) {
			fclose(tmpf);
			fprintf(stderr, nospace,BELL);
			error = E_SPACE;
			return;
		}
		flgf = 0;
	}
	setsig(SIGINT, saveint);
	fputs("\n", tmpf);
	/*
	 * In order to use some of the subroutines that
	 * are used to read mail, the let array must be set up
	 */
	nlet = 1;
	let[0].adr = 0;
	let[1].adr = ftell(tmpf);
	fclose(tmpf);
	if (flgf)
		return;
	if((tmpf = fopen(lettmp, "r")) == NULL) {
		fprintf(stderr,MSGSTR(M_MSG_32,
			"%s: cannot reopen %s for reading\n"),BELL,lettmp);
		error = E_FILE;
		return;
	}
	/*
	 * Send a copy of the letter to the specified users
	 */
	if (error == 0)
		while (--argc > 0)
			if (send_copy(0, *++argv, 0) == FALSE) 
				error++;
	/*
	 * Save a copy of the letter in dead.letter if
	 * any letters can't be sent
	 */
	if (error) {
		/*
		 * Try to create dead letter in current directory
		 * or in home directory
		 */
		umask(umsave);
		if((aret=legal(&dead[1])))
			malf = fopen(&dead[1], "w");
		if ((malf == NULL) || (aret == 0)) {

			/*
			 * try to create in $HOME
			 */
			if((hmdead = malloc(strlen(home) + strlen(dead) + 1)) == NULL) {
				fprintf(stderr,MSGSTR(M_MSG_33, emalloc),BELL);
				goto out;
			}
			cat(hmdead, home, dead);
			if((aret=legal(hmdead)))
				malf = fopen(hmdead, "w");
			if ((malf == NULL) || (aret == 0)) {
				fprintf(stderr,MSGSTR(M_MSG_34,
				  "%s: cannot create %s\n"),BELL,&dead[1]);
			out:
				fclose(tmpf);
				error = E_FILE;
				umask(7);
				return;
			}  else {
				chmod(hmdead, 0600);
                                chown(hmdead, getuid(), getgid());
				printf(MSGSTR(M_MSG_35, "Mail saved in %s\n"),
					hmdead);
			}
		} else {
			chmod(&dead[1], 0600);
                        chown(&dead[1], getuid(), getgid());
			printf(MSGSTR(M_MSG_36,"Mail saved in %s\n"),&dead[1]);
		}
		/*
		 * Copy letter into dead letter box
		 */
		umask(7);
		if (copylet(0, malf, ZAP) == FALSE) {
			fprintf(stderr,MSGSTR(M_MSG_37,
				"%s: cannot save in dead letter\n"),BELL);
		}
		fclose(malf);
	}
	fclose(tmpf);
}

savdead()
{
	setsig(SIGINT, saveint);
	error++;
}

/*
 * send mail to remote system taking fowarding into account
 *	n	-> index into mail table
 *	name	-> mail destination
 * returns
 *	TRUE	-> sent mail
 *	FALSE	-> can't send mail
 */
sendrmt(n, name)
register char *name;
{
# define NSCCONS	"/usr/nsc/cons/"
	register char *p;
	register local;
	FILE *rmf, *popen();
	char rsys[64], cmd[200];
	char remote[30];

	/*
	 * assume mail is for remote
	 * look for bang to confirm that
	 * assumption
	 */
	local = 0;
	while (*name=='!')
		name++;
	for(p=rsys; *name!='!'; *p++ = *name++)
		if (*name=='\0') {
			local++;
			break;
		}
	*p = '\0';
	if ((!local && *name=='\0') || (local && *rsys=='\0')) {
		fprintf(stderr,MSGSTR(M_MSG_38,"null name\n") );
		return(FALSE);
	}
	if (local)
		sprintf(cmd, "binmail %s", rsys);
	if (strcmp(thissys, rsys) == SAME) {
		local++;
		sprintf(cmd, "binmail %s", name+1);
	}

#if SEC_BASE
	if (!local) {
		fprintf(stderr, "%s: no remote mail on trusted systems\n",
			BELL);
		return FALSE;
	}
#else /* !SEC_BASE */
	 /* If we've been invoked with the '-I' option,
	  * use the INmail system to send remote mail.
	  */
	if ( ! local  &&  INflag)
		sprintf(cmd, "iscrmail \"%s!%s\"", rsys, name+1);

	/*
	 * send local mail or remote via uux
	 */
	else if (!local) {
		if (fromlevel > FROMLEVELS)
			return(FALSE);

#ifdef USE_NUSEND
		/*
		 * If mail can't be sent over NSC network
		 * use uucp.
		 */
		if (setjmp(nusendfail) == 0) {
			nusendjmp = TRUE;
			sprintf(remote, "%s%s", NSCCONS, rsys);
			if (access(remote, A_EXIST) != CERROR) {
				/*
				 * Send mail over NSC network
				 */
				sprintf(cmd, "nusend -d %s -s -e -!'rmail %s' - 2>/dev/null",
					rsys, name);
#ifdef B_DEBUG
printf("%s\n", cmd);
#endif
				if ((rmf=popen(cmd, "w")) != NULL) {
					copylet(n, rmf, local? FORWARD: REMOTE);
					if (pclose(rmf) == 0) {
						nusendjmp = FALSE;
						return(TRUE);
					}
				}
			}
		}
		nusendjmp = FALSE;
#endif

#ifdef USE_USEND
		if (setjmp(usendfail) == 0) {
			usendjmp = TRUE;
			sprintf(cmd, "usend -s -d%s -uNoLogin -!'rmail %s' - 2>/dev/null",
				rsys, name);
#ifdef B_DEBUG
printf("%s\n", cmd);
#endif
			if ((rmf=popen(cmd, "w")) != NULL) {
				copylet(n, rmf, local? FORWARD: REMOTE);
				if (pclose(rmf) == 0) {
					usendjmp = FALSE;
					return(TRUE);
				}
			}
		}
		usendjmp = FALSE;
#endif

		/*
		 * Use uux to send mail
		 */
		if (strchr(name+1, '!'))
			sprintf(cmd, "uux - %s!rmail \\(%s\\)", rsys, name+1);
		else
			sprintf(cmd, "uux - %s!rmail %s", rsys, name+1);
	}
#endif /* !SEC_BASE */
#ifdef B_DEBUG
printf("%s\n", cmd);
#endif
	/*
	 * copy letter to pipe
	 */
	if ((rmf=popen(cmd, "w")) == NULL)
		return(FALSE);
	if (copylet(n, rmf, local? FORWARD: REMOTE) == FALSE) {
		fprintf(stderr,MSGSTR(M_MSG_42,
			"%s: cannot pipe to mail command\n"),BELL);
		pclose(rmf);
		return(FALSE);
	}

	/*
	 * check status
	 */
	return(pclose(rmf)==0 ? TRUE : FALSE);
}

/*
 * send letter n to name
 *	n	-> letter number
 *	name	-> mail destination
 *	level	-> depth of recursion for forwarding
 * returns
 *	TRUE	-> mail sent
 *	FALSE	-> can't send mail
 */
send_copy(n, name, level)
int	n;
char	*name;
{
	register char *p;
	char	file[MAXFILENAME];
	struct	passwd	*pwd, *getpwnam();
	char buf[128];

	if(level > 20) {
		fprintf(stderr,MSGSTR(M_MSG_43, "unbounded forwarding\n") );
		return(FALSE);
	}
	if (strcmp(name, "-") == SAME)
		return(TRUE);
	/*
	 * See if mail is to be fowarded
	 */
	for(p=name; *p!='!' &&*p!='\0'; p++)
		;
	if (*p == '!')
		return(sendrmt(n, name));
	/*
	 * See if user has specified that mail is to be fowarded
	 */
	cat(file, maildir, name);
	if(areforwarding(file))
		return(send_copy(n, recip, level+1));
	/*
	 * see if user exists on this system
	 */
	setpwent();	
	if((pwd = getpwnam(name)) == NULL){
		fprintf(stderr,MSGSTR(M_MSG_44,"%s: can't send to %s\n"),
			BELL,name);
		return(FALSE);
	}
	cat(file, maildir, pwd->pw_name);
	lock(file);
	/*
	 * If mail file does not exist create it
	 * with the correct uid and gid
	 */
#if SEC_BASE
	if (!mail_check_mailbox(pwd->pw_name, pwd->pw_uid, file)) {
		unlock();
		return FALSE;
	}
#else /* !SEC_BASE */
	if(access(file, A_EXIST) == CERROR) {
		umask(0);
		close(creat(file, MFMODE));
		umask(7);
		if (chown(file, pwd->pw_uid, pwd->pw_gid)) {
		    sprintf(buf, MSGSTR(BL_CHOWN, bl_chown), BELL);
		    perror(buf);
		    unlink(file);
		    unlock();
		    return(FALSE);
		}
	}
#endif /* !SEC_BASE */
	/*
	 * Append letter to mail box
	 */
	if((malf = fopen(file, "a")) == NULL){
		fprintf(stderr,MSGSTR(M_MSG_45, "%s: cannot append to %s\n"),
			BELL,file);
		unlock();
		return(FALSE);
	}

	/* this is to compensate for the open() behavior that
	   causes the file ptr to == 0 even on open for append,
	   as per POSIX specs (?) */
	
	fseek(malf, 0L, SEEK_END);

	/* set up msg for biff server telling it the username and the offset
	   into the mailbox file for this message */

	sprintf(buf, "%s@%d\n", name, ftell(malf)); 

	if (copylet(n, malf, ORDINARY) == FALSE) {
		fprintf(stderr,MSGSTR(M_MSG_46,"%s: cannot append to %s\n"),
			BELL,file);
		unlock();
		return(FALSE);
	}
	fclose(malf);
	unlock();
	notifybiff(buf);  /* send notify message to biff server */
	return(TRUE);
}

/*
 * signal catching routine
 * reset signals on quits and interupts
 * exit on other signals
 *	i	-> signal #
 */
delete(i)
register int i;
{
	setsig(i, delete);

#ifdef USE_NUSEND
	if (i == SIGPIPE && nusendjmp == TRUE)
		longjmp(nusendfail, 1);
#endif

#ifdef USE_USEND
	if (i == SIGPIPE && usendjmp == TRUE)
		longjmp(usendfail, 1);
#endif

	if(i>SIGQUIT){
		fprintf(stderr,MSGSTR(M_MSG_47, "%s: error signal %d\n"),
			BELL,i);
	}else
		fprintf(stderr, "\n");
	if(delflg && (i==SIGINT || i==SIGQUIT))
		longjmp(sjbuf, 1);
	done();
}

/*
 * clean up lock files and exit
 */
done()
{
	unlock();
	unlink(lettmp);
	unlink(rmtbuf);
	exit(error);
}

/*
 * create mail lock file
 *	file	-> lock file name
 * returns:
 *	none
 */
lock(file)
char *file;
{
	register int f, i;

	if (locked == LOCKON)
		return;
	cat(curlock, file, maillock);
	for (i=0; i<100; i++) {
		if((f = creat(curlock, C_NOACCESS)) != CERROR){
			close(f);
			locked = LOCKON;
			return;
		} else if (errno != EACCES) {  /* real error */
			fprintf(stderr, MSGSTR(BL_ELOCK,
			    "%s: cannot create lock file %s: "), BELL, curlock);
			perror("");
			error = E_FILE;
			done();
		}
		sleep(10);
	}
	fprintf(stderr,MSGSTR(M_MSG_49,"%s: %s not creatable after %d tries\n")
		,BELL,curlock,i);
	error = E_FILE;
	done();
}

unlock()
{
	if (*curlock != '\0')
		unlink(curlock);
	locked = LOCKOFF;
}

/*
 * concatenate from1 and from2 to to
 *	to	-> destination string
 *	from1	-> source string
 *	from2	-> source string
 * return:
 *	none
 */
cat(to, from1, from2)
register char *to, *from1, *from2;
{

	for (; *from1; )
		*to++ = *from1++;
	for (; *from2; )
		*to++ = *from2++;
	*to = '\0';
}

/*
 * get next token
 *	p	-> string to be searched
 *	s	-> area to return token
 * returns:
 *	p	-> updated string pointer
 *	s	-> token
 *	NULL	-> no token
 */
char *getarg(s, p)	
register char *s, *p;
{
	while (*p == ' ' || *p == '\t')
		p++;
	if (*p == '\n' || *p == '\0')
		return(NULL);
	while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0')
		*s++ = *p++;
	*s = '\0';
	return(p);
}

/*
 * check existence of file
 *	file	-> file to check
 * returns:
 *	0	-> exists unwriteable
 *	1	-> exists writeable
 *	2	-> does not exist
 */
legal(file)
register char *file;
{
	register char *sp;
	char dfile[MAXFILENAME];

	/*
	 * If file does not exist then
	 * try "." if file name has no "/"
	 * For file names that have a "/", try check
	 * for existence of previous directory
	 */
	if(access(file, A_EXIST) == A_OK)
		if(access(file, A_WRITE) == A_OK)
			return(1);
		else	return(0);
	else {
		if((sp=strrchr(file, '/')) == NULL)
			cat(dfile, ".", "");
		else {
			strncpy(dfile, file, sp - file);
			dfile[sp - file] = '\0';
		}
		if(access(dfile, A_WRITE) == CERROR) 
			return(0);
		return(2);
	}
}

/*
 * invok shell to execute command waiting for
 * command to terminate
 *	s	-> command string
 * return:
 *	status	-> command exit status
 */
system(s)
char *s;
{
	register int pid, w;
	int status;
	void (*istat)(), (*qstat)();

	/*
	 * Spawn the shell to execute command, however,
	 * since the mail command runs setgid mode
	 * reset the effective group id to the real
	 * group id so that the command does not
	 * acquire any special privileges
	 */
	if ((pid = fork()) == CHILD) {
		setuid(getuid());
		setgid(getgid());
		execl("/bin/sh", "sh", "-c", s, NULL);
		_exit(127);
	}

	/*
	 * Parent temporarily ignores signals so it 
	 * will remain around for command to finish
	 */
	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	while ((w = wait(&status)) != pid && w != CERROR)
		;
	if (w == CERROR)
		status = CERROR;
	signal(SIGINT, istat);
	signal(SIGQUIT, qstat);
	return(status);
}
