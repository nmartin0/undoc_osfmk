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
 
/*++ machpath.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/machpath.c,v $
 *
 * DESCRIPTION:
 *
 *	Determine the machine-id of the explicit or implied machine
 *	specification in the given path specification:
 *
 *		path_name			OR
 *		machine_name:path_name
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:09 $ $State: Exp $)
 * $Log: machpath.c,v $
 * Revision 1.3  1994/11/19  02:27:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:04  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:05  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:51:01  hender
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
extern char *index();			/* Find character in string */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern char *strchr();			/* Find character in string */
#else
BAD SYSTEM TYPE
#endif
#endif

/*** machpath
 *
 *
 *	int machpath():
 *
 *	Determine the machine-id of the explicit or implied machine
 *	specification in the given path specification.
 *
 *		path_name			OR
 *		machine_name:path_name
 *
 *	Returns:
 *		1: if successful, and an explicit machine-name
 *		   appeared in the pathname specification.
 *		   The machine-id parameter is appropriately
 *		   updated.
 *
 *		0: if successful, but no explicit machine-name
 *		   appeared in the pathname specification (the
 *		   local machine is implicitly assumed).  The
 *		   machine-id parameter is appropriately
 *		   updated.
 *
 *	       -1: if a null machine-name was specified in the
 *		   path-name:
 *
 *			:path_name
 *
 *	       -2: if the explicit or implied machine-specification
 *		   is not recognized by the local system (NMAP_ENOMAP).
 *
 *	       -3: if the Network Mapping Procedures (NMAP_)
 *		   deny access to the caller (NMAP_ENOPRIV).
 *
 *	       -4: if some other general NMAP_ error occurs.
 */
int machpath (path_name, machine_id)
register char *path_name;		/* Path-name to be inspected */
mid_t *machine_id;			/* Pointer of where to return */
					/* the proper machine-id */
{
	register struct hostent *ent;	/* Host table entry structure */
	register char *end_machine;	/* End of machine-name */

#if	BSD42 | BSD43 | ULTRIX
	if ((end_machine = index (path_name, ':')) == (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	if ((end_machine = strchr (path_name, ':')) == (char *) 0) {
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
	if (path_name == end_machine) {
		/*
		 *  Missing machine-specification.
		 */
		return (-1);		/* Bad syntax */
	}
	/*
	 *  Determine the machine-id of the destination machine.
	 */
	*end_machine = '\0';		/* Temporarily zap ':' */
	ent = gethostbyname (path_name);/* Lookup machine-name */
	*end_machine = ':';		/* Restore ':' */
	if (ent == (struct hostent *) 0) {
		return (-2);		/* This machine is not */
					/* known to us */
	}
	switch (nmap_get_mid (ent, machine_id)) {
	case NMAP_SUCCESS:		/* Successfully got local machine-id */
		return (1);		/* for explicitly named machine */
	case NMAP_ENOMAP:		/* What?  No local mid defined! */
		return (-2);
	case NMAP_ENOPRIV:		/* No privilege */
		return (-3);
	}
	return (-4);			/* General NMAP_ error */
}
