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
static char rcsid[] = "@(#)$RCSfile: sysconfig.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:33 $";
#endif


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <locale.h>

#include "cm.h"
#include "cm_cmdpkt.h"


/*
 *      Common Global data
 */
typedef struct {
	char *		progname;
	int		dflg;
	int		vflg;
	int		sflg;
	int		command;
	char *		name;
	char *		opts;

} sysconfig_common_t;
static sysconfig_common_t	CD;


/*
 *      usage
 */
void
usage()
{
        fprintf(stderr, cm_msg(MSG_SYSCONFIG), CD.progname);
        exit(2);
}

void
options(argc, argv)
        int     argc;
        char ** argv;
{
        register int    c;
        extern char *   optarg;
        extern int      optind;

        CD.progname = strrchr(argv[0],'/') ? strrchr(argv[0],'/') + 1: argv[0];
	CD.vflg 	= FALSE;
	CD.dflg 	= FALSE;
	CD.sflg 	= FALSE;
	CD.command 	= CFGMGR_NOSPEC;
	CD.name 	= "";
	CD.opts 	= "";

        if (getuid() && geteuid()) {
		fprintf(stderr, "%s: %s\n", CD.progname,
			cm_msg(MSG_MUSTBEROOT));
                exit(1);
        }

        while ((c = getopt(argc, argv, "cuqdvsf:o:")) != EOF) {
                switch (c) {
		/*
		 *	Configuration Command
		 */
                case 'c':
			if (CD.command != CFGMGR_NOSPEC)
				usage();
                        CD.command = CFGMGR_CONFIG;
			break;
                case 'u':
			if (CD.command != CFGMGR_NOSPEC)
				usage();
                        CD.command = CFGMGR_UNCONFIG;
			break;
                case 'q':
			if (CD.command != CFGMGR_NOSPEC)
				usage();
                        CD.command = CFGMGR_QUERY;
			break;
		/*
		 *	Configuration Loader Flags
		 */
                case 'l':
			break;
		/*
		 *	Configuration Passthru Options
		 */
                case 'o':
                        CD.opts = optarg;
			break;
		/*
		 *	Misc
		 */
                case 'd':
                        CD.dflg = TRUE;
			break;
                case 'v':
                        CD.vflg = TRUE;
			break;
                case 's':
                        CD.sflg = TRUE;
			break;
                case '?':
                        usage();
			break;
                default:
                        usage();
			break;
                }
        }
        if ((optind +1) != argc)
		usage();

	CD.name = argv[optind];

	if (CD.command == CFGMGR_NOSPEC)
		usage();
	return;
}


/*
 *
 */
void
make_pkt( cmgr_cmdpkt_t * cmdpkt )
{
	cmdpkt->cmdpkt_op = CD.command;

	if (CD.dflg)
		cmdpkt->cmdpkt_loglvl = LOG_DEBUG;
	else if (CD.vflg)
		cmdpkt->cmdpkt_loglvl = LOG_INFO;
	else
		cmdpkt->cmdpkt_loglvl = LOG_ERR;
		

	strncpy(cmdpkt->cmdpkt_name, CD.name, sizeof(cmdpkt->cmdpkt_name)-1);

	strncpy(cmdpkt->cmdpkt_opts, CD.opts, sizeof(cmdpkt->cmdpkt_opts)-1);

	return;
}


/*
 *
 */
int
open_unix_socket( char * name )
{
	int 	fdunix;
	struct sockaddr_un sun;

	fdunix = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fdunix < 0) {
		if (CD.dflg)
		    	fprintf(stderr, cm_msg(MSG_SOCKET), CD.progname,
				strerror(errno));
		return(-1);
	}

	sun.sun_family = AF_UNIX;
        strncpy(sun.sun_path, name, sizeof(sun.sun_path));
	if (connect(fdunix, &sun, sizeof(sun)) < 0) {
		if (CD.dflg)
		    	fprintf(stderr, cm_msg(MSG_CONNECT), CD.progname,
				strerror(errno));
		(void) close(fdunix);
		return(-1);
	}
	return(fdunix);
}


/*
 *
 */
int
request_send(int domain, int fd, cmgr_cmdpkt_t * cmdpkt)
{
	int	rc;

	/*
	 *	Send comand and object name
	 */
	switch (domain) {
	case AF_UNIX:
		if ((rc=write(fd, (char *)cmdpkt, PKTSZ)) != PKTSZ) {
			if (CD.dflg)
				fprintf(stderr, cm_msg(MSG_WRITE), CD.progname,
					strerror(errno));
			(void) close(fd);
			rc = -1;
		}
		break;
	 default:
		rc = -1;
		break;
	}
	return(rc);
}



/*
 *
 */
request_service( int domain, int fd )
{
	char	buf[1024];
	int	buflen;

        /*
         *      Read replies from socket/write to user, until EOF
         */
	switch (domain) {

	case AF_UNIX:
		while ((buflen = read(fd, buf, sizeof(buf))) > 0) {
			if (CD.sflg)
				continue;
			fwrite(buf, 1, buflen, stdout);
		}
		(void) close(fd);
		break;
	 default:
		break;
	}
}



/*
 *
 */
main(argc, argv)
	int	argc;
	char **	argv;
{
	cmgr_cmdpkt_t 	cmdpkt;
	int		domain = AF_UNIX;
	int		fd;

        setlocale(LC_ALL, "");

	options(argc, argv);

	make_pkt(&cmdpkt);

	if ((fd=open_unix_socket(LOCALSOCKNAME)) < 0) {
		fprintf(stderr, cm_msg(MSG_CFGMGR_NOTAVAIL), CD.progname);
		exit(1);
	}

	if (request_send(domain, fd, &cmdpkt) < 0) {
		fprintf(stderr, cm_msg(MSG_CFGMGR_NOSEND), CD.progname);
	    	exit(1);
	}
	request_service(domain, fd);

	exit(0);
}

