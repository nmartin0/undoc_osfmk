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
 
/*++ machspec.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/machspec.c,v $
 *
 * DESCRIPTION:
 *
 *	Determine the machine-id of the explicit or implied machine
 *	specification as given in the text parameter.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:10 $ $State: Exp $)
 * $Log: machspec.c,v $
 * Revision 1.3  1994/11/19  02:27:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:06  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:06  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:51:08  hender
 * Sterling version 4/22/87
 * 
 *
 */


#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)machspec.c	1.2 (machspec.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include NETDB				/* Network database declarations */
#include NMAP				/* Network mapping codes */

extern struct hostent *gethostbyname();	/* Get hostent structure for name */
extern int localmid();			/* Get machine-id of local host */
extern int nmap_get_mid();		/* Get machine-id for name */

/*** machspec
 *
 *
 *	int machspec():
 *
 *	Determine the machine-id of the explicit or implied machine
 *	specification as given in the text parameter.
 *
 *	The form of an explicit machine specification is:
 *
 *		@machine-name	OR
 *		@[integer#]
 *
 *	where the "machine-name" form represents the principal name
 *	or alias assigned to a particular machine, and the "[integer#]"
 *	form is supported so that explicit machine-ids can be specified
 *	in place of a name or alias.
 *
 *	Returns:
 *		0: if successful, in which case the machine-id
 *		   parameter is successfully updated as
 *		   appropriate.
 *
 *	       -1: if an invalid machine-specification syntax
 *		   is encountered.
 *
 *	       -2: if the explicit or implied machine-specification
 *		   is not recognized by the local system (NMAP_ENOMAP).
 *
 *	       -3: if the Network Mapping Procedures (NMAP_)
 *		   deny access to the caller (NMAP_ENOPRIV).
 *
 *	       -4: if some other general NMAP_ error occurs.
 */
int machspec (text, machine_id)
register char *text;			/* Literal text */
mid_t *machine_id;			/* Pointer of where to return */
					/* the proper machine-id */
{
	register mid_t mid_spec;	/* Machine-id specification */
	struct hostent *ent;		/* Host table entry structure */

	while (*text && *text != '@') {
		text++;			/* Look for possible machine- */
	}				/* specification */
	if (*text == '\0') {
		/*
		 *  No machine-specification was given.  The
		 *  local machine is assumed.
		 */
		return (localmid (machine_id));
	}
	/*
	 *  A machine-name specification appears.
	 */
	text++;				/* Step past machine-specification */
					/* introducer character */
	if (*text == '\0') {
		/*
		 *  Missing machine-specification.
		 */
		return (-1);		/* Bad syntax */
	}
	/*
	 *  Determine the machine-id of the destination machine.
	 */
	if (*text == '[') {
		/*
		 *  Could be an explicit machine specification by machine-id.
		 */
		text++;
		mid_spec = 0;
		while (*text >= '0' && *text <= '9') {
			mid_spec *= 10;
			mid_spec += *text - '0';
			*text++;
		}
		if (*text == ']') {
			/*
			 *	[ <digit-sequence> ]
			 */
			if (*++text) {		/* Invalid machine-id spec */
				return (-1);	/* Bad syntax */
			}
			*machine_id = mid_spec;
			return (0);		/* Success */
		}
		return (-1);			/* Invalid syntax */
	}
	/*
	 *  The destination host has been specified as a name.
	 */
	if ((ent = gethostbyname (text)) == (struct hostent *) 0) {
		return (-2);			/* This machine is not */
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
