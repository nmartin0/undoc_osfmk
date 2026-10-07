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
 
/* @(#)fasthalt.c	5.1 15:42:06 8/16/90 SecureWare */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */

#include	<sys/secdefines.h>
#if SEC_BASE
#include	<sys/security.h>
#include	<prot.h>
#endif

#include	<sys/types.h>
#include        <sys/param.h>
#include	<errno.h>
#include	<stdio.h>
#include 	<locale.h>
#include 	"fasthalt_msg.h" 

nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_FASTHALT,n,s)
#ifdef SEC_BASE
#define MSGSTR_SEC(n,s) catgets(catd,MS_FASTHALT_SEC,n,s)
#endif

#define	FAILURE	-1
#define SH	"/sbin/sh"

#define MAXFILE PATH_MAX /* Maximum pathname length for "/fastboot" file */

static char ststfile[MAXFILE+1] = "/fastboot" ;	/* Name of the current */
static char command[MAXFILE+1] = "/sbin/halt " ; /* "start-stop" like file.*/

main(argc, argv)
	int argc;
	char **argv;
{
	int fd;
        extern uid_t geteuid();
#if SEC_BASE
	extern priv_t *privvec();
	privvec_t saveprivs;
#endif

	(void) setlocale( LC_ALL, "" );
	catd = catopen(MF_FASTHALT,0);

#ifdef SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();
	if (!authorized_user("sysadmin")) {
		fprintf(stderr, MSGSTR_SEC(AUTH,
			"%s: need sysadmin authorization\n"),
			command_name);
		exit(1);
	}
	if (!forceprivs(privvec(SEC_ALLOWDACACCESS,
#if SEC_MAC
				SEC_ALLOWMACACCESS,
#endif
#if SEC_ILB
				SEC_ILNOFLOAT,
#endif
#if SEC_NCAV
				SEC_ALLOWNCAVACCESS,
#endif
				-1), saveprivs)) {
		fprintf(stderr, MSGSTR_SEC(PRIV,
			"%s: insufficient privileges\n"), command_name);
		exit(1);
	}
#else /* SEC_BASE */

 if (geteuid()) {
                fprintf(stderr, MSGSTR(NO_ROOT, "NOT super-user\n"));
                exit(1);
        }
#endif /* SEC_BASE */
	argc--, argv++;

	if(!strcmp(*argv,"-n") || !strcmp(*argv, "-y") || 
		!strcmp(*argv,"-q") || !strcmp(*argv, "-l"))
		;
	else {
		 fprintf(stderr, MSGSTR(USAGE, "usage: fasthalt [-y][-q][-l][-n]\n"));
                        exit(1);
                }

	if (argc > 0)
		strcat(command,*argv);

	if ((fd = creat(&ststfile[0],0644)) != FAILURE)
		close(fd);
#if SEC_BASE
	seteffprivs(saveprivs, (priv_t *) 0);
#endif
	execl(SH,"INITSH","-c", command, 0);
}
