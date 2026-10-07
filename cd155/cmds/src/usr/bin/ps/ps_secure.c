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
 * Copyright (c) 1988-90 SecureWare, Inc.  All rights reserved.
 */

#ident "@(#)ps_secure.c	3.3 15:04:52 6/20/90 SecureWare"
/*
 * Rewritten from:
 *   "@(#)ps_secure.c	2.17.1.1 17:15:13 1/4/90 SecureWare, Inc."
 */

/*
 * This file is part of a library to make commands more secure.
 * This file contains those routines that are added to the
 * ps command.
 * If the system supports a ps_data file, hooks are required to create
 * it with the proper attributes.
 * If the system supports mandatory access control, filtering must be
 * done on target processes to remove those the process does not
 * dominate if the user is not authorized for allowmacaccess.
 */

#include <sys/secdefines.h>

#if SEC_BASE
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>
#if SEC_MAC
#include <sys/secpolicy.h>
#include <mandatory.h>
#endif

extern priv_t *privvec();

#ifndef _OSF_SOURCE
/*
 * Open the ps data file securely and return the file descriptor of the
 * new file, set for writing.
 */
int
ps_open_file_securely(file)
	char *file;
{
	int cfs_status;
	privvec_t saveprivs;

	/* Save the effective privileges already in effect */

	if (forceprivs(privvec(SEC_ALLOWDACACCESS, SEC_OWNER, SEC_CHOWN,
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
		fprintf(stderr, "%s: insufficient privileges\n", command_name);
		exit(1);
	}
	unlink(file);
	cfs_status = create_file_securely(file, AUTH_VERBOSE,
			     "store static operating system parameters");
	if (cfs_status == CFS_GOOD_RETURN) {
		enter_quiet_zone();
		cfs_status = open(file, O_WRONLY);
	} else
		cfs_status = -1;
#if SEC_ILB
	/*
	 * Leave ilnofloat raised to avoid potential problems when
	 * the ps_data file is written.
	 */
	if (cfs_status >= 0)
		ADDBIT(saveprivs, SEC_ILNOFLOAT);
#endif
	seteffprivs(saveprivs, (priv_t *) 0);
	return(cfs_status);
}


/*
 * Restore the signals that were set to SIG_IGN when the data file
 * was opened so that the file data would not be interrupted.
 */
void
ps_cleanup_new_file()
{
	exit_quiet_zone();
}
#endif

#if SEC_MAC /*{*/

/*
 * Return TRUE if process dominates the target process.
 * Attempt a getlabel(2) call on the target process's sensitivity label.
 */

ps_proc_dominate(pid)
	int	pid;
{
	static attr_t	attr;
	static mand_ir_t *mand_ir = (mand_ir_t *) 0;
	static int has_auth;
	static int first_time = 1;
	obj_t	obj;

	if (first_time) {
		mand_ir = mand_alloc_ir();
		if (mand_ir == (mand_ir_t *) 0) {
			fprintf(stderr, "%s: MAC initialization failed\n",
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
#endif /*} SEC_MAC */
#endif SEC_BASE
