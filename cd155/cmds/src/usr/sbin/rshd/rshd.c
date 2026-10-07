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
 
/* @(#)rshd.c	3.2 09:38:47 6/18/90 SecureWare */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: rshd.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:15:18 $";
#endif
/*
static char sccsid[] = "@(#)rshd.c	1.13  com/sockcmd/simpled,3.1,9008C 2/21/90 16:53:55";
 */
/* 
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1983, 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/*
#ifndef lint
char copyright[] =
"Copyright (c) 1983, 1988, 1989 The Regents of the University of California.\n\
 All rights reserved.\n";
#endif

#ifndef lint
static char sccsid[] = "rshd.c      5.34 (Berkeley) 6/29/90";
#endif  not lint */

/*
 * remote shell server:
 *	[port]\0
 *	remuser\0
 *	locuser\0
 *	command\0
 *	data
 */
#include <sys/secdefines.h>
#if SEC_BASE
#undef CMU
#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>
#if SEC_MAC
#include <mandatory.h>
#endif
#endif
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <stdio.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <netdb.h>
#include <sys/syslog.h>
#include <stdarg.h>
#include <ctype.h>
#include "pathnames.h"

#include <nl_types.h>
#include <locale.h>
#include "rshd_msg.h" 
#define MSGSTR(Num,Str) catgets(catd,MS_RSHD,Num,Str)
nl_catd catd;           /* message catalog file descriptor */

int	errno;
int	keepalive = 1;
int	check_all = 0;
char	*index(), *rindex(), *strncat(), *getenv(), *malloc();

int sent_null;

#define OPTIONS "aln"

/*ARGSUSED*/
main(argc, argv)
	int argc;
	char **argv;
{
	extern int opterr, optind;
	extern int _check_rhosts_file;
	struct linger linger;
	int ch, on = 1, fromlen;
	struct sockaddr_in from;

#if SEC_BASE
	set_auth_parameters(argc, argv);
#endif
	setlocale(LC_ALL, "");
	catd = NLcatopen(MF_RSHD, 0);

	openlog("rshd", LOG_PID | LOG_ODELAY, LOG_DAEMON);

	opterr = 0;
	while ((ch = getopt(argc, argv, OPTIONS)) != EOF)
		switch((char)ch) {
		case 'a':
			check_all = 1;
			break;
		case 'l':
			_check_rhosts_file = 0;
			break;
		case 'n':
			keepalive = 0;
			break;
		case '?':
		default:
			syslog(LOG_ERR, MSGSTR(USAGE, "usage: rshd [-lns]"));
			break;
		}

	argc -= optind;
	argv += optind;

	fromlen = sizeof (from);
	if (getpeername(0, &from, &fromlen) < 0) {
		fprintf(stderr, MSGSTR( PEERNMERR, "rshd: %s: "), argv[0]);
		perror(MSGSTR(PEERNM, "getpeername")); /*MSG*/
		_exit(1);
	}
	if (keepalive &&
	    setsockopt(0, SOL_SOCKET, SO_KEEPALIVE, (char *)&on,
	    sizeof(on)) < 0)
		syslog(LOG_WARNING, MSGSTR(SETSOCKOPT, "setsockopt (SO_KEEPALIVE): %m")); /*MSG*/
	linger.l_onoff = 1;
	linger.l_linger = 60;			/* XXX */
	if (setsockopt(0, SOL_SOCKET, SO_LINGER, (char *)&linger,
	    sizeof (linger)) < 0)
		syslog(LOG_WARNING, MSGSTR(SETSOCKLING, "setsockopt (SO_LINGER): %m")); /*MSG*/
	doit(&from);
}

#if SEC_BASE
char	logname[24] = "LOGNAME=";
#endif
char    username[20] = "USER=";
char    homedir[64] = "HOME=";
char    shell[64] = "SHELL=";
char    *envinit[] =
#if SEC_BASE
	    {homedir, shell, _PATH_DEFPATH, logname, username, 0};
#else
            {homedir, shell, _PATH_DEFPATH, username, 0};
#endif
char    **environ;

doit(fromp)
	struct sockaddr_in *fromp;
{
	char cmdbuf[NCARGS+1], *cp;
	char locuser[16], remuser[16];
	struct passwd *pwd;
	int s;
	struct hostent *hp;
	char *hostname, *errorstr = NULL, *errorhost;
        u_short port;
	int pv[2], pid, cc;
	int nfd;
	fd_set ready, readfrom;
	char buf[BUFSIZ], sig;
	int one = 1;
	char remotehost[2 * MAXHOSTNAMELEN + 1];
#if SEC_BASE
	struct pr_passwd *ppr;
	static privvec_t nosprivs;
	mask_t		*sysprivs;
	mask_t		*baseprivs;
#if SEC_MAC
	mand_ir_t	*clearance, *slevel;
#endif
#endif /* SEC_BASE */

	(void) signal(SIGINT, SIG_DFL);
	(void) signal(SIGQUIT, SIG_DFL);
	(void) signal(SIGTERM, SIG_DFL);
#ifdef DEBUG
	{ int t = open(_PATH_TTY, 2);
	  if (t >= 0) {
		ioctl(t, TIOCNOTTY, (char *)0);
		(void) close(t);
	  }
	}
#endif DEBUG
	fromp->sin_port = ntohs((u_short)fromp->sin_port);
	if (fromp->sin_family != AF_INET) {
		syslog(LOG_ERR, MSGSTR(FRMADDRERR, "malformed \"from\" address (af %d)\n"), fromp->sin_family); /*MSG*/
		exit(1);
	}
#ifdef IP_OPTIONS
      {
	u_char optbuf[BUFSIZ/3], *cp;
	char lbuf[BUFSIZ], *lp;
	int optsize = sizeof(optbuf), ipproto;
	struct protoent *ip;

	if ((ip = getprotobyname("ip")) != NULL)
		ipproto = ip->p_proto;
	else
		ipproto = IPPROTO_IP;
	if (!getsockopt(0, ipproto, IP_OPTIONS, (char *)optbuf, &optsize) &&
	    optsize != 0) {
		lp = lbuf;
		for (cp = optbuf; optsize > 0; cp++, optsize--, lp += 3)
			sprintf(lp, " %2.2x", *cp);
		syslog(LOG_NOTICE,MSGSTR(CONN_IP, "Connection from %s received using IP options (ignored):%s"), inet_ntoa(fromp->sin_addr), lbuf);
		if (setsockopt(0, ipproto, IP_OPTIONS,
		    (char *)NULL, &optsize) != 0) {
			syslog(LOG_ERR, MSGSTR(SETSCKIP, "setsockopt IP_OPTIONS NULL: %m"));
			exit(1);
		}
	}
      }
#endif

	if (fromp->sin_port >= IPPORT_RESERVED ||
	    fromp->sin_port < IPPORT_RESERVED/2) {
		syslog(LOG_NOTICE, MSGSTR(ILLEGAL_PORT, "Connection from %s on illegal port"),
			inet_ntoa(fromp->sin_addr));
		exit(1);
	}

	(void) alarm(60);
	port = 0;
	for (;;) {
		char c;
		if ((cc = read(0, &c, 1)) != 1) {
			if (cc < 0)
				syslog(LOG_ERR, MSGSTR(RDERR, "read: %m")); /*MSG*/
			shutdown(0, 1+1);
			exit(1);
		}
		if (c == 0)
			break;
		port = port * 10 + c - '0';
	}

	(void) alarm(0);
	if (port != 0) {
		int lport = IPPORT_RESERVED - 1;
		s = rresvport(&lport);
		if (s < 0) {
			syslog(LOG_ERR, MSGSTR(STDERR, "can't get stderr port: %m")); /*MSG*/
			exit(1);
		}
		if (port >= IPPORT_RESERVED) {
			syslog(LOG_ERR, MSGSTR(NO2PORT, "2nd port not reserved\n")); /*MSG*/
			exit(1);
		}
		fromp->sin_port = htons((u_short)port);
		if (connect(s, fromp, sizeof (*fromp)) < 0) {
			syslog(LOG_INFO, MSGSTR(NO2CONN, "connect second port: %m")); /*MSG*/
			exit(1);
		}
	}
#ifdef notdef
        /* from inetd, socket is already on 0, 1, 2 */
	dup2(f, 0);
	dup2(f, 1);
	dup2(f, 2);
#endif
	hp = gethostbyaddr((char *)&fromp->sin_addr, sizeof (struct in_addr),
		fromp->sin_family);
	if (hp) {
		/*
		 * If name returned by gethostbyaddr is in our domain,
		 * attempt to verify that we haven't been fooled by someone
		 * in a remote net; look up the name and check that this
		 * address corresponds to the name.
		 */
		hostname = hp->h_name;
		if (check_all || local_domain(hp->h_name)) {
			strncpy(remotehost, hp->h_name, sizeof(remotehost) - 1);
			remotehost[sizeof(remotehost) - 1] = 0;
			errorhost = remotehost;
			hp = gethostbyname(remotehost);
			if (hp == NULL) {
				syslog(LOG_INFO,
				    MSGSTR(NOADDR, "Couldn't look up address for %s"),
				    remotehost);
				error(MSGSTR(NOADDR2, "Couldn't look up address for your host"));
				 hostname = inet_ntoa(fromp->sin_addr);
			} else for (; ; hp->h_addr_list++) {
				if (hp->h_addr_list[0] == NULL) {
					syslog(LOG_NOTICE,MSGSTR(HOSTADDR1,
					"Host addr %s not listed for host %s"),
					    inet_ntoa(fromp->sin_addr),
					    hp->h_name);
					error(MSGSTR(HOSTADDR2, "Host address mismatch"));
                                        errorstr =
                                            "Host address mismatch for %s\n";
                                        hostname = inet_ntoa(fromp->sin_addr);
                                        break;
				}
				if (!bcmp(hp->h_addr_list[0],
				    (caddr_t)&fromp->sin_addr,
				    sizeof(fromp->sin_addr))) {
					hostname = hp->h_name;
					break;
				}
			}
		}
	} else 
		errorhost = hostname = inet_ntoa(fromp->sin_addr);

	getstr(remuser, sizeof(remuser), "remuser");
	getstr(locuser, sizeof(locuser), "locuser");
	getstr(cmdbuf, sizeof(cmdbuf), "command");
	setpwent();
	pwd = getpwnam(locuser);
	/* 
	 * send magic null over in case we have to
	 * print out error messages
	 */
	(void) write(2, "\0", 1);
	sent_null = 1;
	if (pwd == NULL) {
		error(MSGSTR(LOGINERR, "Login incorrect.\n")); /*MSG*/
		exit(1);
	}
	if (chdir(pwd->pw_dir) < 0) {
		(void) chdir("/");
#ifdef notdef
		error(MSGSTR(NODIR, "No remote directory.\n")); /*MSG*/
		exit(1);
#endif
	}

	if (pwd->pw_passwd != 0 && *pwd->pw_passwd != '\0' &&
	    ruserok(hostname, pwd->pw_uid == 0, remuser, locuser) < 0) {
		error(MSGSTR(NOPERMIT, "Permission denied.\n")); /*MSG*/
		exit(1);
	}

	if (pwd->pw_uid && !access(_PATH_NOLOGIN, F_OK)) {
		error(MSGSTR(LOGINDISABLE, "Logins currently disabled.\n"));
		exit(1);
	}

#if SEC_BASE
	ppr = getprpwnam(locuser);
	if (ppr == (struct pr_passwd *) 0) {
		error(MSGSTR(NOPRPWENT, "No protected password entry.\n"));
		exit(1);
	}
	if (ppr->uflg.fg_name == 0 || ppr->uflg.fg_uid == 0 ||
	    ppr->ufld.fd_uid != pwd->pw_uid ||
	    strcmp(ppr->ufld.fd_name, pwd->pw_name) != 0) {
		error(MSGSTR(PRPWSYNC, "Protected password data out of sync.\n"));
		exit(1);
	}
	if (locked_out(ppr)) {
		error(MSGSTR(DISABLED, "Account is disabled.\n"));
		exit(1);
	}
	if (ppr->uflg.fg_retired && ppr->ufld.fd_retired) {
		error(MSGSTR(RETIRED, "Account has been retired.\n"));
		exit(1);
	}
	if (time_lock(ppr)) {
		error(MSGSTR(UNAVAIL, "Account is unavailable at this time.\n"));
		exit(1);
	}
	(void) setluid(pwd->pw_uid);
#if SEC_MAC
	if (ppr->uflg.fg_clearance)
		clearance = &ppr->ufld.fd_clearance;
	else if (ppr->sflg.fg_clearance)
		clearance = &ppr->sfld.fd_clearance;
	else
		clearance = mand_syslo;
#if SEC_SHW
	slevel = mand_syshi;
#else
	slevel = mand_syslo;
#endif
	if (setclrnce(clearance) == -1) {
		error(MSGSTR(CLEARANCE, "Can't set process clearance.\n"));
		exit(1);
	}
	if (setslabel(slevel) == -1) {
		error(MSGSTR(SETSL, "Can't set process sensitivity label.\n"));
		exit(1);
	}
#if SEC_ILB
	if (setilabel(mand_syslo) == -1) {
		error(MSGSTR(SETIL, "Can't set process information label.\n"));
		exit(1);
	}
#endif /* SEC_ILB */
#endif /* SEC_MAC */
#endif /* SEC_BASE */

	if (port) {
		if (pipe(pv) < 0) {
			error(MSGSTR(PIPERR, "Can't make pipe.\n")); /*MSG*/
			exit(1);
		}
		pid = fork();
		if (pid == -1)  {
			error(MSGSTR(RETRY, "Try again.\n")); /*MSG*/
			exit(1);
		}
		if (pid) {
			(void) close(0); (void) close(1); (void) close(2);
			(void) close(pv[1]);
			FD_ZERO(&readfrom);
			FD_SET(s, &readfrom);
			FD_SET(pv[0], &readfrom);
			if (pv[0] > s)
                                nfd = pv[0];
                        else
                                nfd = s;
			ioctl(pv[0], FIONBIO, (char *)&one);
			/* should set s nbio! */
			nfd++;
			do {
				ready = readfrom;
				if (select(nfd, &ready, (fd_set *)0,
				    (fd_set *)0, (struct timeval *)0) < 0)
					break;
				if (FD_ISSET(s, &ready)) {
					int	ret;
					if ((ret = read(s, &sig, 1)) <= 0)
						FD_CLR(s, &readfrom);
					else
						killpg(pid, sig);
				}
				if (FD_ISSET(pv[0], &ready)) {
					errno = 0;
					cc = read(pv[0], buf, sizeof (buf));
					if (cc <= 0) {
						shutdown(s, 1+1);
						FD_CLR(pv[0], &readfrom);
					} else
						(void) write(s, buf, cc);
				}
			} while (FD_ISSET(s, &readfrom) ||
			    FD_ISSET(pv[0], &readfrom));
			exit(0);
		}
		setsid();
		(void) close(s); (void) close(pv[0]);
		dup2(pv[1], 2);
		close(pv[1]);
	}
	if (*pwd->pw_shell == '\0')
		pwd->pw_shell = _PATH_BSHELL;
	if (setlogin(pwd->pw_name) < 0)
                syslog(LOG_ERR, "setlogin() failed: %m");
	(void) setgid((gid_t)pwd->pw_gid);
	initgroups(pwd->pw_name, pwd->pw_gid);
	(void) setuid((uid_t)pwd->pw_uid);
#if SEC_BASE
	audit_login(ppr, pwd, "remsh", (char *) 0, ES_LOGIN_REMOTE);
	audit_adjust_mask(ppr);
	if (ppr->uflg.fg_sprivs)
		sysprivs = ppr->ufld.fd_sprivs;
	else if (ppr->sflg.fg_sprivs)
		sysprivs = ppr->sfld.fd_sprivs;
	else
		sysprivs = nosprivs;
	if (ppr->uflg.fg_bprivs)
		baseprivs = ppr->ufld.fd_bprivs;
	else if (ppr->sflg.fg_bprivs)
		baseprivs = ppr->sfld.fd_bprivs;
	else 
		baseprivs = nosprivs;
	setpriv(SEC_MAXIMUM_PRIV, sysprivs);
	setpriv(SEC_BASE_PRIV, baseprivs);
	endprpwent();
	endprdfent();
#endif /* SEC_BASE */
        environ = envinit;
	strncat(homedir, pwd->pw_dir, sizeof(homedir)-6);
        strncat(shell, pwd->pw_shell, sizeof(shell)-7);
#if SEC_BASE
	strncat(logname, pwd->pw_name, sizeof(logname)-9);
#endif
        strncat(username, pwd->pw_name, sizeof(username)-6);
        cp = rindex(pwd->pw_shell, '/');
        if (cp)
                cp++;
        else
                cp = pwd->pw_shell;
      	endpwent();
	if (pwd->pw_uid == 0) {
                syslog(LOG_INFO|LOG_AUTH, MSGSTR(ROOT_SHELL, 
			"ROOT shell from %s@%s, comm: %s\n"),
                	remuser, hostname, cmdbuf);
	}
        execl(pwd->pw_shell, cp, "-c", cmdbuf, 0);
	sprintf(buf,"rshd, %s",pwd->pw_shell);
	perror(buf);
	exit(1);
}

/*
 * Report error to client.
 * Note: can't be used until second socket has connected
 * to client, or older clients will hang waiting
 * for that connection first.
 */
/*VARARGS1*/
int
error(char *fmt, ...)
{
	va_list ap;
        char buf[BUFSIZ], *bp = buf;

	va_start(ap, fmt);
	(void) vsprintf(bp, fmt, ap);
        (void) write(2, buf, strlen(buf));
        va_end(ap);
}

getstr(buf, cnt, err)
	char *buf;
	int cnt;
	char *err;
{
	char c;

	do {
		if (read(0, &c, 1) != 1)
			exit(1);
		*buf++ = c;
		if (--cnt == 0) {
			error(MSGSTR(TOOLONG, "%s too long\n"), err); /*MSG*/
			exit(1);
		}
	} while (c != 0);
}

/*
 * Check whether host h is in our local domain,
 * as determined by the part of the name following
 * the first '.' in its name and in ours.
 * If either name is unqualified (contains no '.'),
 * assume that the host is local, as it will be
 * interpreted as such.
 */
local_domain(h)
	char *h;
{
	char localhost[MAXHOSTNAMELEN];
	char *p1, *p2, *topdomain();

	localhost[0] = 0;
        (void) gethostname(localhost, sizeof(localhost));
        p1 = topdomain(localhost);
        p2 = topdomain(h);
	if (p1 == NULL || p2 == NULL || !strcasecmp(p1, p2))
		return(1);
	return(0);
}
char *
topdomain(h)
        char *h;
{
        register char *p;
        char *maybe = NULL;
        int dots = 0;

	for (p = h + strlen(h); p >= h; p--) {
                if (*p == '.') {
                        if (++dots == 2)
                                return (p);
                        maybe = p;
                }
        }
	 return (maybe);
}
