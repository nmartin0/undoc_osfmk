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
static char rcsid[] = "@(#)$RCSfile: cm_startup_mode.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:31 $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "cfgmgr.h"
#include "cm.h"

/*
 *	CFGMGR:	Enter start up mode.
 */
void
startup_mode()
{
        cm_log_t	log;

	/*
	 *	Set up log for replies to syslog
	 */
	log.log_type 	= CM_LOG_SYSLOG;
	log.log_fd 	= 1;
	log.log_domain 	= AF_UNSPEC;
	if (CMGR.dflg)
		log.log_lvl	= LOG_DEBUG;
	else if (CMGR.vflg)
		log.log_lvl	= LOG_INFO;
	else
		log.log_lvl	= LOG_ERR;

	cfgmgr_log(LOG_INFO, cm_msg(MSG_ENTER_AUTOMODE), CMGR.progname);

	startup_config_auto_list(&log);

	cfgmgr_log(LOG_INFO, cm_msg(MSG_LEAVE_AUTOMODE), CMGR.progname);
	return;
}


/*
 * Get each AUTOMATIC subsystems from configuration database
 */
int
startup_config_auto_list( cm_log_t * logp )
{
	AFILE_t		afd;
	ENT_t		entry;
	ATTR_t		attr;
	char *		val;
	int		rc;
	int		cnt_success;
	int		cnt_failure;

	/*
	 *	Open database
	 */
        if (rc=dbfile_open_dflt(&afd)) {
		cfgmgr_log(LOG_ERR, "%s: %s\n", CMGR.database, cm_msg(rc));
		return(0);
	}

	if (rc=dbent_lookup(afd, AUTOENTRY, &entry)) {
		cfgmgr_log(LOG_INFO, "%s: %s\n", AUTOENTRY, cm_msg(rc));
		return(0);
	}

	if ((attr=AFgetatr(entry, AUTO_DYNAMIC)) == NULL) {
		cfgmgr_log(LOG_INFO, "%s: %s: %s\n", AUTOENTRY,
			AUTO_DYNAMIC, cm_msg(DBATR_ENOENT));
		return(0);
	}

	cnt_success = 0;
	cnt_failure = 0;
	while ( (val=AFnxtval(attr)) != NULL ) {

		if (strcmp(val, KEYWORD_NONE) == 0)
			continue;

		/*
		 *	Load and Configure subsystem
		 */
		rc = method_call(logp, val, CM_OP_LOAD|CM_OP_CONFIGURE);
		if (rc == 0) {
			cnt_success++;
		} else if (rc != METHOD_EFAIL) {
			cfgmgr_log(LOG_ERR, "%s: %s\n", val, rc);
			cnt_failure++;
		}
	}

	dbfile_close(afd);

	if (cnt_success || cnt_failure)
		cfgmgr_log(LOG_INFO, cm_msg(MSG_AUTOCONFIGURED),
			CMGR.progname, cnt_success, cnt_failure);
	else
		cfgmgr_log(LOG_INFO, cm_msg(MSG_NOAUTOMATIC),
			CMGR.progname);

	return(0);
}

