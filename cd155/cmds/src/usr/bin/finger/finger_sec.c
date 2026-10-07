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
/*
 * Copyright (c) 1990 SecureWare, Inc.  All Rights Reserved.
 */


#if SEC_BASE
#ident "@(#)finger_sec.c	4.1 14:43:47 7/13/90 SecureWare"

#include <sys/secdefines.h>
#include <sys/security.h>
#include <prot.h>

#if SEC_NET_TTY
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>

static void decode_inet_addr();
#endif

/*
 * Get the last login information for the specified user.
 */
finger_findwhen(user, tty, ttymax, host, hostmax, logintime)
	char	*user, *tty, *host;
	int	ttymax, hostmax;
	time_t	*logintime;
{
	struct pr_passwd	*pw;
	char			path[32];

	pw = getprpwnam(user);
	
	if (pw && pw->uflg.fg_slogin)
		*logintime = pw->ufld.fd_slogin;
	else
		*logintime = (time_t) 0;

	if (pw && pw->uflg.fg_suctty) {
#if SEC_NET_TTY
		/*
		 * See if the tty name corresponds to an existing device
		 * node.  If not, try to interpret it as a network address.
		 */
		memset(path, '\0', sizeof path);
		strcpy(path, "/dev/");
		strncat(path, pw->ufld.fd_suctty, sizeof pw->ufld.fd_suctty);
		if (access(path, 0) < 0) {
			decode_inet_addr(pw->ufld.fd_suctty, host, hostmax);
			strncpy(tty, "a pseudo tty", ttymax);
		} else
#endif
		{
			host[0] = '\0';
			if (ttymax > sizeof pw->ufld.fd_suctty)
				ttymax = sizeof pw->ufld.fd_suctty;
			strncpy(tty, pw->ufld.fd_suctty, ttymax);
		}
		tty[ttymax] = '\0';
	} else
		host[0] = tty[0] = '\0';
}

#if SEC_NET_TTY
/*
 * Decode a hexadecimal internet address into a hostname.
 */
static void
decode_inet_addr(addr, host, hostmax)
	char	*addr, *host;
	int	hostmax;
{
	register struct hostent	*hp;
	register char	*cp;
	unsigned long	inaddr = 0;
	char		buf[9];

	host[0] = '\0';

	strncpy(buf, addr, sizeof buf);
	buf[8] = '\0';

	for (cp = buf; *cp && isxdigit(*cp); ++cp) {
		inaddr <<= 4;
		if (isdigit(*cp))
			inaddr += *cp - '0';
		else if (isupper(*cp))
			inaddr += *cp - 'A' + 10;
		else
			inaddr += *cp - 'a' + 10;
	}
	inaddr = htonl(inaddr);

	if (cp != &buf[8])
		return;

	hp = gethostbyaddr((char *) &inaddr, sizeof inaddr, AF_INET);
	if (hp == (struct hostent *) 0)
		strncpy(host, inet_ntoa(inaddr), hostmax);
	else
		strncpy(host, hp->h_name, hostmax);
	host[hostmax] = '\0';
}
#endif /* SEC_NET_TTY */
#endif /* SEC_BASE */
