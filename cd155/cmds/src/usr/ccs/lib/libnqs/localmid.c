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
 
/*++ localmid.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/localmid.c,v $
 *
 * DESCRIPTION:
 *
 *	Return the machine-id of the local host.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 11, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:08 $ $State: Exp $)
 * $Log: localmid.c,v $
 * Revision 1.3  1994/11/19  02:27:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:59  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:03  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:50:53  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include NETDB				/* Network database declarations */
#include NMAP				/* Network mapping codes */

extern int nmap_get_mid();		/* Get machine-id for name */

/*** localmid
 *
 *
 *	int localmid():
 *	Return the machine-id of the local host.
 *
 *	Returns:
 *		0: if successful, in which case the machine_id
 *		   parameter is properly set.
 *
 *	       -2: if the local machine-id is not defined in the
 *		   local host's mapping tables (NMAP_ENOMAP).
 *
 *	       -3: if the Network Mapping Procedures (NMAP_)
 *		   deny access to the caller (NMAP_ENOPRIV).
 *
 *	       -4: if some other general NMAP_ error occurs.
 */
int localmid (machine_id)
mid_t *machine_id;                 	/* Pointer of where to return */
                                        /* the proper machine-id */
{
	static short local_is_known = 0;/* Boolean non-zero if the */
					/* local machine-id is stored */
					/* in local_mid. */
        static mid_t local_mid;            /* Machine-id of local machine */
	if (local_is_known) {
		/*
		 *  The local machine-id is known from a previous
		 *  call.
		 */
		*machine_id = local_mid;
		return (0);		/* Success */
	}
	switch (nmap_get_mid ((struct hostent *) 0, machine_id)) {
	case NMAP_SUCCESS:		/* Successfully got local machine-id */
		local_mid = *machine_id;
		local_is_known = 1;
		return (0);
	case NMAP_ENOMAP:		/* What?  No local mid defined! */
		return (-2);
	case NMAP_ENOPRIV:		/* No privilege */
		return (-3);
	}
	return (-4);			/* General NMAP_ error */
}
