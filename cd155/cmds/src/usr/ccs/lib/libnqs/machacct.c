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
 
/*++ machacct.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/machacct.c,v $
 *
 * DESCRIPTION:
 *
 *	Determine the machine-id of the explicit or implied machine
 *	specification in the given account specification:
 *
 *		account_name			OR
 *		account_name@machine_name
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	November 5, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:08 $ $State: Exp $)
 * $Log: machacct.c,v $
 * Revision 1.3  1994/11/19  02:27:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:01  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:04  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:50:57  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include NETDB				/* Network database */
#include NMAP				/* Network mapping codes */

extern struct hostent *gethostbyname();	/* Get hostent structure for name */
extern int localmid();			/* Get machine-id of local host */
extern int nmap_get_mid();		/* Get machine-id for name */
#if	BSD42 | BSD43 | ULTRIX
extern char *index();			/* Scan for character */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern char *strchr();			/* Scan for character */
#else
BAD SYSTEM TYPE
#endif
#endif

/*** machacct
 *
 *
 *	int machacct():
 *
 *	Determine the machine-id of the explicit or implied machine
 *	specification in the given account specification:
 *
 *		account_name			OR
 *		account_name@machine_name
 *
 *	Returns:
 *		0: if successful, in which case the machine-id
 *		   parameter is successfully updated as
 *		   appropriate.
 *
 *	       -1: if a null machine-name was specified as
 *		   in:
 *
 *			account_name@
 *
 *	       -2: if the explicit or implied machine-specification
 *		   is not recognized by the local system (NMAP_ENOMAP).
 *
 *	       -3: if the Network Mapping Procedures (NMAP_)
 *		   deny access to the caller (NMAP_ENOPRIV).
 *
 *	       -4: if some other general NMAP_ error occurs.
 */
int machacct (account_name, machine_id)
register char *account_name;		/* Account name */
mid_t *machine_id;			/* Pointer of where to return */
					/* the proper machine-id */
{
	register struct hostent *ent;	/* Host table entry structure */

#if	BSD42 | BSD43 | ULTRIX
	if ((account_name = index (account_name, '@')) == (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	if ((account_name = strchr (account_name, '@')) == (char *) 0) {
#else
BAD SYSTEM TYPE
#endif
#endif
		/*
		 *  No machine-specification was given.  The
		 *  local machine is assumed.
		 */
		return (localmid (machine_id));
	}
	/*
	 *  A machine-name specification appears.
	 */
	account_name++;			/* Step past machine-specification */
					/* introducer character */
	if (*account_name == '\0') {
		/*
		 *  Missing machine-specification.
		 */
		return (-1);		/* Bad syntax */
	}
	/*
	 *  Determine the machine-id of the destination machine.
	 */
	if ((ent = gethostbyname (account_name)) == (struct hostent *) 0) {
		return (-2);		/* This machine is not */
					/* known to us */
	}
	switch (nmap_get_mid (ent, machine_id)) {
	case NMAP_SUCCESS:		/* Successfully got local machine-id */
		return (0);
	case NMAP_ENOMAP:		/* What?  No local mid defined! */
		return (-2);
	case NMAP_ENOPRIV:		/* No privilege */
		return (-3);
	}
	return (-4);			/* General NMAP_ error */
}
