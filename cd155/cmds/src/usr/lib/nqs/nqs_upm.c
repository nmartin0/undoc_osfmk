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
 
/*++ nqs_upm.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_upm.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS manager set update module.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	June 16, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:27 $ $State: Exp $)
 * $Log: nqs_upm.c,v $
 * Revision 1.3  1994/11/19  02:53:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:26:39  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:45  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:09:53  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"
#include "nqsxvars.h"
#include "transactcc.h"		/* Transaction completion codes */

extern struct gendescr *nextdb();
				/* Return next allocated entry */
				/* from a database file */
extern void seekdbb();		/* Buffered seek on a database file */
extern void udb_addnqsman();	/* Add NQS manager to NQS database */
extern void udb_delnqsman();	/* Delete an NQS manager from the NQS */
				/* database */


/*** upm_addnqsman
 *
 *
 *	long upm_addnqsman():
 *	Add an NQS manager account to the NQS manager set.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_ALREADEXI:	if the specified account is already
 *				present in the NQS manager set.
 */
long upm_addnqsman (account_uid, account_mid, privilege_bits)
uid_t account_uid;			/* NQS manager account user-id */
mid_t account_mid;			/* NQS manager account machine-id */
int privilege_bits;			/* Privilege bits */
{
	if (validmgr (account_uid, account_mid)) {
		/*
		 *  The specified account to add to the NQS manager
		 *  set is already present in the set.
		 */
		return (TCML_ALREADEXI);
	}
	udb_addnqsman (account_uid, account_mid, privilege_bits);
	return (TCML_COMPLETE);		/* Add the account to the */
					/* NQS manager set */
}


/*** upm_delnqsman
 *
 *
 *	long upm_delnqsman():
 *	Delete an NQS manager account from the NQS manager set.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_NOSUCHMAN:	if the specified manager account was not
 *				present at the correct privilege level
 *				in the NQS manager set.
 *		TCML_ROOTINDEL:	if an attempt is made to delete "root"
 *				from the NQS manager set.
 */
long upm_delnqsman (account_uid, account_mid, privilege_bits)
uid_t account_uid;			/* NQS manager account user-id */
mid_t account_mid;			/* NQS manager account machine-id */
int privilege_bits;			/* Privilege bits */
{
	if (account_uid == 0 && account_mid == Locmid) {
		/*
		 *  The "root" account can never be deleted from the NQS
		 *  manager set.
		 */
		return (TCML_ROOTINDEL);
	}
	if (validmgr (account_uid, account_mid) == privilege_bits) {
		/*
		 *  The specified account to delete from the NQS manager
		 *  set is present at the specified privilege level.
		 */
		udb_delnqsman (account_uid, account_mid);
		return (TCML_COMPLETE);	/* Delete the account from the */
					/* NQS manager set */
	}
	return (TCML_NOSUCHMAN);	/* No such manager account */
}


/*** validmgr
 *
 *
 *	validmgr():
 *
 *	Return the (non-zero) privilege bits if the specified
 *	manager account is defined in the NQS manager set.
 *	Otherwise return 0.
 */
static int validmgr (account_uid, account_mid)
uid_t account_uid;			/* Manager account user-id */
mid_t account_mid;			/* Manager account machine-id */
{
	register struct gendescr *descr;

	seekdbb (Mgrfile, 0L);		/* Seek to the beginning of the NQS */
					/* manager list file */
	/*
	 *  Search for the entry to be added, to see if it is already
	 *  present.
	 */
	descr = nextdb (Mgrfile);
	while (descr != (struct gendescr *) 0) {
		if (descr->v.mgr.manager_uid == account_uid &&
		    descr->v.mgr.manager_mid == account_mid) {
			/*
			 *  He or she is a manager.
			 */
			return (descr->v.mgr.privileges);
		}
		else descr = nextdb (Mgrfile);
	}
	return (0);			/* He or she is not a manager. */
}
