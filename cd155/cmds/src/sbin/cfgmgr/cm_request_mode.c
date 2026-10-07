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
static char rcsid[] = "@(#)$RCSfile: cm_request_mode.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:29 $";
#endif

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <strings.h>
#include <syslog.h>
#include <AFdefs.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>

#ifdef CFG_INET
#include <netdb.h>
#include <netinet/in.h>
#endif

#include "cfgmgr.h"
#include "cm.h"
#include "cm_cmdpkt.h"

int
open_unix_socket( char * name )
{
	int			fdunix;
	struct sockaddr_un 	sun;

        (void) unlink(name);
        fdunix = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fdunix < 0) {
		cfgmgr_log(LOG_WARNING, "%s: %s: %s\n",
			CMGR.progname, "socket", strerror(errno));
                return(-1);
        }

        sun.sun_family = AF_UNIX;
        strncpy(sun.sun_path, name, sizeof(sun.sun_path));
        if (bind(fdunix, &sun, sizeof(sun)) < 0) {
		cfgmgr_log(LOG_WARNING, "%s: %s: %s\n",
			CMGR.progname, "bind", strerror(errno));
		close(fdunix);
		(void) unlink(sun.sun_path);
                return(-1);
        }

	if (chmod(sun.sun_path, 0600) < 0) {
		cfgmgr_log(LOG_WARNING, "%s: %s: %s\n",
			CMGR.progname, "chmod", strerror(errno));
		close(fdunix);
		(void) unlink(sun.sun_path);
                return(-1);
	}

	if (listen(fdunix, 5) < 0) {
		cfgmgr_log(LOG_WARNING, "%s: %s: %s\n",
			CMGR.progname, "listen", strerror(errno));
		close(fdunix);
		(void) unlink(sun.sun_path);
                return(-1);
	}
	return(fdunix);
}

#ifdef CFG_INET
int
open_inet_socket( char * name )
{
	int			fdinet;
	struct sockaddr_in	sin;
	struct servent *	sp;

	fdinet = socket(AF_INET, SOCK_STREAM, 0);
	if (fdinet < 0) {
		cfgmgr_log(LOG_WARNING, "%s: %s: %s\n",
			CMGR.progname, "socket", strerror(errno));
		return(-1);
	}

	sp = getservbyname(name, "tcp");
	if (sp == NULL) {
		cfgmgr_log(LOG_WARNING, "%s: %s: %s/tcp %s\n", CMGR.progname,
			"getservbyname", name, "unknown service");
		close(fdinet);
		return(-1);
	}

	sin.sin_family = AF_INET;
	sin.sin_port = sp->s_port;
	if (bind(fdinet, &sin, sizeof(sin), 0) < 0) {
		cfgmgr_log(LOG_WARNING, "%s: %s: %s\n",
			CMGR.progname, "bind", strerror(errno));
		close(fdinet);
		return(-1);
	}
	if (listen(fdinet, 5) < 0) {
		cfgmgr_log(LOG_WARNING, "%s: %s: %s\n",
			CMGR.progname, "listen", strerror(errno));
		close(fdunix);
                return(-1);
	}
	return(fdinet);
}
#endif


int
request_receive(int domain, int fd, cmgr_cmdpkt_t * cmdpkt)
{

	switch (domain) {

	case AF_UNIX:
		if (read(fd, (char *)cmdpkt, PKTSZ) != PKTSZ) {
			cfgmgr_log(LOG_ERR,  "%s: %s: %s\n",
				CMGR.progname, "read", strerror(errno));
			return(-1);

		}
		break;

#ifdef CFG_INET
	case AF_INET:
		if (recv(fd, (char *)cmdpkt, PKTSZ, 0) != PKTSZ) {
			cfgmgr_log(LOG_ERR,  "%s: %s: %s\n",
				CMGR.progname, "recv", strerror(errno));
			return(-1);
		}
		break;
#endif

	default:
		cfgmgr_log(LOG_ERR,  "%s: %s (%d)\n", CMGR.progname,
				"invalid domain", domain);
		return(-1);
	}

	return(0);
}

/*
 * Dispatch command request
 */
void
request_service(int domain, int fd, cmgr_cmdpkt_t * cmd)
{
	cm_log_t	log;
	ENT_t		entry;
	int		rc;

	/*
	 * 	Set up log for replies to requestor
	 */
	log.log_type	= CM_LOG_MSG;
	log.log_fd      = fd;
	log.log_domain  = domain;
	log.log_lvl  	= cmd->cmdpkt_loglvl;

	rc = method_call(&log, cmd->cmdpkt_name, cmd->cmdpkt_op);
	if (rc == 0) {
		return;
	} else if (rc != METHOD_EFAIL) {
		cm_log(&log, LOG_ERR, "%s: %s\n", cmd->cmdpkt_name, cm_msg(rc));
	}
	return;
}



/*
 *	Enter request mode
 */
void
request_mode()
{
	cmgr_cmdpkt_t	cmdpkt;
	fd_set		fdselect;
	int		rc;
	int		fdunix;
#ifdef	CFG_INET
	int		fdinet;
#endif

        cfgmgr_log(LOG_INFO, cm_msg(MSG_ENTER_REQUESTMODE), CMGR.progname);

        FD_ZERO(&fdselect);
	/*
	 *	Open UNIX communication port
	 */
	if ((fdunix = open_unix_socket(LOCALSOCKNAME)) >= 0)
		FD_SET(fdunix, &fdselect);
	else
		cfgmgr_log(LOG_WARNING, "%s: %s\n", CMGR.progname,
	    		cm_msg(MSG_NOUNIXSOCK));

#ifdef CFG_INET
	/*
	 *	Open INET communication port
	 */
	if ((fdinet = open_inet_socket(SERVICENAME)) >= 0)
		FD_SET(fdinet, &fdselect);
	else
		cfgmgr_log(LOG_WARNING, "%s: %s\n", CMGR.progname,
	    		cm_msg(MSG_NOINETSOCK));
#endif

	if (fdselect.fds_bits == 0) {
		cfgmgr_log(LOG_ERR, "%s: %s\n", CMGR.progname,
	    		cm_msg(MSG_NOSOCKS));
		CMGR.exitval = 2;
		cmgr_mainexit();
	}


	/*
	 *	Read (select) communications requests
	 */
	for (;;) {
		int	nfound;
		int	domain;
		int	pid;
		int	sfd;
		int	fromlen;
		fd_set	readfds;
		struct sockaddr_un fromunix;
#ifdef CFG_INET
	    	struct sockaddr_in frominet;
#endif

		readfds = fdselect;
	    	nfound = select(20, &readfds, 0, 0, 0);
	    	if (nfound <= 0) {
			if (nfound < 0 && errno != EINTR) {
				cfgmgr_log(LOG_WARNING, "%s: %s: %s\n",
				CMGR.progname, "select", strerror(errno));
				}
			continue;
	    	}

		/*
		 *	AF_UNIX request
		 */
	    	if (FD_ISSET(fdunix, &readfds)) {
			domain = AF_UNIX;
			fromlen = sizeof(fromunix);
			sfd = accept(fdunix, &fromunix, &fromlen);
	    	}
#ifdef CFG_INET
		/*
		 *	AF_INET request
		 */
	    	else if (FD_ISSET(fdinet, &readfds)) {
			domain = AF_INET;
			fromlen = sizeof(frominet);
			sfd = accept(fdinet, &frominet, &fromlen);
	    	}
#endif
	    	else {
			cfgmgr_log(LOG_ERR, "%s: %s: %s\n",
				CMGR.progname, "select", strerror(ENXIO));
			continue;
	    	}

		/*
		 *	Bad accept
		 */
		if (sfd < 0) {
		    	if (errno != EINTR)
				cfgmgr_log(LOG_WARNING, "%s: %s: %s\n",
				CMGR.progname, "accept", strerror(errno));
		    	continue;
		}

		/*
		 * 	Receive comamnd packet
		 */
		if ((rc = request_receive(domain, sfd, &cmdpkt)) != 0) {
			cfgmgr_log(LOG_WARNING, "%s: %s: %s\n", CMGR.progname,
				"request_receive", strerror(EINVAL));
			close(sfd);
			continue;
		}

		/*
		 * 	Service requested comamnd
		 */
	    	request_service(domain, sfd, &cmdpkt);


		/*
		 * 	Finished with socket request descriptor
		 */
		close(sfd);

	}
	/* NOTREACHED */
}

