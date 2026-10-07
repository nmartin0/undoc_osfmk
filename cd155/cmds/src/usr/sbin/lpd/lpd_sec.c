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
 
#if SEC_BASE
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: lpd_sec.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:08:22 $";
#endif
/*
 * Copyright (c) 1990 SecureWare, Inc.  All Rights Reserved.
 */

#ident "@(#)lpd_sec.c	4.1 15:53:38 7/19/90 SecureWare"

#include "lp.h"

#if SEC_MAC
#include <sys/security.h>
#include <mandatory.h>
#define MSGSTR_SEC(n,s) catgets(catd,MS_PRINTER_SEC,n,s)

extern char	*sys_secerrlist[];
extern int	sec_errno;
#endif

uid_t	lp_uid;
gid_t	lp_gid;

/*
 * lpd_initialize
 *
 * Called upon startup by lpd.  Verify that the process's luid is not
 * already set, then set it as well as the uid and gid to "lp".
 */

lpd_initialize()
{
	errno = 0;
	(void) getluid();
	if (errno == 0) {
		fprintf(stderr, MSGSTR_SEC(LPDSEC1, "lpd: must be run from init\n"));
		exit(1);
	}

	lp_uid = pw_nametoid("lp");
	if (lp_gid == (gid_t) -1) {
		fprintf(stderr, MSGSTR_SEC(GROUPUNDEF, "%s: group id \"lp\" is not defined\n"), "lpd");
		exit(1);
	}

	lp_gid = gr_nametoid("lp");
	if (lp_uid == (uid_t) -1) {
		fprintf(stderr, MSGSTR_SEC(USERUNDEF, "%s: user id \"lp\" is not defined\n"), "lpd");
		exit(1);
	}

	setluid(lp_uid);
	setgid(lp_gid);
	setuid(lp_uid);
}

/*
 * Set the discretionary attributes of the UNIX domain socket through which
 * the daemon receives requests.  This hook should be called after the
 * bind() system call and before the listen() system call.
 */
lpd_setsockattr(path)
	char	*path;
{
	if (chown(path, lp_uid, lp_gid) == -1) {
		syslog(LOG_ERR, MSGSTR_SEC(LPDSEC4, "%s: cannot change owner/group: %m"), path);
		exit(1);
	}
	if (chmod(path, 0660) == -1) {
		syslog(LOG_ERR, MSGSTR_SEC(LPDSEC5, "%s: cannot change mode: %m"), path);
		exit(1);
	}
#if SEC_MAC
	if (chslabel(path, (mand_ir_t *) 0) == -1) {
		syslog(LOG_ERR, MSGSTR_SEC(LPDSEC6, "%s: cannot set MAC label: %s"), path,
			sys_secerrlist[sec_errno]);
		exit(1);
	}
#endif
}

/*
 * lpd_checkclient
 *
 * Verify that the process at the other end of a UNIX domain socket is
 * privileged.  The socket descriptor is passed as an argument.
 */
lpd_checkclient(fd)
{
	int	priv;

	if (ioctl(fd, SIOCGPEERPRIV, &priv) == -1 || priv == 0) {
		syslog(LOG_ERR, MSGSTR_SEC(LPDSEC7, "lpd: ignoring untrusted UNIX domain client"));
		exit(1);
	}
}

#endif /* SEC_BASE */
