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
/* @(#)w_secure.c	6.3 17:38:17 3/11/91 SecureWare */
/*
 * Copyright (c) 1988-90 SecureWare, Inc.  All rights reserved.
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: w_secure.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:47:18 $";
#endif
/*
 * Rewritten from:
 *   "@(#)ps_secure.c	2.17.1.1 17:15:13 1/4/90 SecureWare, Inc."
 */

/*
 * This file contains those routines that are added to the w command.
 * If the system supports mandatory access control, filtering must be
 * done on target processes to remove those the process does not
 * dominate if the user is not authorized for allowmacaccess.
 */

#include <sys/secdefines.h>

#if SEC_MAC /*{*/

#include <stdio.h>

#include <sys/security.h>
#include <sys/secpolicy.h>
#include <sys/audit.h>
#include <mandatory.h>
#include <prot.h>

#ifdef KJI
#include <NLchar.h>
#endif

#include <nl_types.h>
#include "w_msg.h"
nl_catd	catd;
#define MSGSTR_SEC(num,str)	catgets(catd,MS_W_SEC,num,str)

static int has_auth = 0;	/* checks for macquery command authorization */

/*
 * Return TRUE if process dominates the target process.
 * Attempt a getlabel(2) call on the target process's sensitivity label.
 */

w_proc_dominate(pid)
	pid_t	pid;
{
	static attr_t	attr;
	static mand_ir_t *mand_ir = (mand_ir_t *) 0;
	static int first_time = 1;
	obj_t	obj;

	if (first_time) {
		mand_ir = mand_alloc_ir();
		if (mand_ir == (mand_ir_t *) 0) {
			fprintf(stderr, MSGSTR_SEC(MACFAILED,
				"%s: MAC initialization failed\n"),
				command_name);
			exit(1);
		}
		attr.ir_length = mand_bytes();
		attr.ir = (char *) mand_ir;
		has_auth = authorized_user("macquery");
		first_time = 0;
	}
	if (has_auth)
		return 1;

	obj.o_pid = pid;

	/*
	 * Attempt to retrieve the target process's label.
	 * This will only succeed if we dominate the target process.
	 */
	return getlabel(mand_config.policy, MAND_SUBJ_SL_TAG, &attr,
			OT_PROCESS, &obj) == 0;
}
/*
 * Privilege-bracket the call to stat(2) by forcing the allowmacaccess
 * privilege if caller has the macquery command authorization (checked in
 * w_proc_dominate())  This is needed for the stat to be successful if the
 * tty's SL dominates that of the process.
 */
int
w_can_view(ttybuf, statbuf)
	char *ttybuf;	
	struct stat *statbuf;
{
	privvec_t saveprivs;
	int ret_val;

	if (has_auth && forceprivs(privvec(SEC_ALLOWMACACCESS, -1), 
			saveprivs)) {
		fprintf(stderr, MSGSTR_SEC(PRIV,
			"%s: insufficient privileges\n"), command_name);
		exit(1);
	}
	ret_val = stat(ttybuf, statbuf);
	if (has_auth)
		seteffprivs(saveprivs, (priv_t *) 0);
	return ret_val;
}
#endif /*} SEC_MAC */
