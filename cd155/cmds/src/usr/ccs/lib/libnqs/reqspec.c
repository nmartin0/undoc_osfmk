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
 
/*++ reqspec.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/reqspec.c,v $
 *
 * DESCRIPTION:
 *
 *	Return the sequence number, and explicit or implied machine
 *	specification as given in the request-id text parameter.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:41 $ $State: Exp $)
 * $Log: reqspec.c,v $
 * Revision 1.3  1994/11/19  02:27:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:32  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:46  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:54:25  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)reqspec.c	1.2 (reqspec.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include NETDB				/* Network database header file; */
					/* Proto/Makefile will tell us */
					/* where to find netdb.h */
#include NMAP				/* Network mapping codes */

#if OSF
#include        "core_dump.h"
#endif

extern struct hostent *gethostbyname();	/* Get hostent structure for name */
extern int localmid();			/* Get machine-id of local host */
extern int nmap_get_mid();		/* Get machine-id for name */

/*** reqspec
 *
 *
 *	int reqspec():
 *
 *	Return the sequence number, and explicit or implied machine
 *	specification as given in the request-id text parameter.
 *
 *	A request-id is always of the form:
 *
 *		<seqno>		OR
 *		<seqno>.<machine-specification>
 *
 *	If no <machine-specification> is given, then the request is
 *	assumed to have been submitted from the local machine.
 *
 *	The form of an explicit machine specification (when present
 *	in a request-id specification), is:
 *
 *		.machine-name	OR
 *		.[integer#]
 *
 *	where the "machine-name" form represents the principal name
 *	or alias for the machine from whence the request was submitted,
 *	and the "[integer#]" form is supported so that explicit
 *	machine-ids can be specified in place of a name or alias.
 *
 *	Returns:
 *		0: if successful, in which case the orig_seqno,
 *		   and machine-id parameters are properly updated.
 *
 *	       -1: if an invalid request-id syntax is encountered.
 *
 *	       -2: if the explicit or implied machine-specification
 *		   is not recognized by the local system (NMAP_ENOMAP).
 *
 *	       -3: if the Network Mapping Procedures (NMAP_)
 *		   deny access to the caller (NMAP_ENOPRIV).
 *
 *	       -4: if some other general NMAP_ error occurs.
 */
int reqspec (text, orig_seqno, machine_id)
register char *text;			/* Literal text */
long *orig_seqno;			/* Req sequence number */
mid_t *machine_id;			/* Pointer of where to return */
					/* the proper machine-id */
{
	register mid_t mid_spec;	/* Machine-id specification */
	register long seqno;		/* Sequence# of request */
	struct hostent *ent;		/* Host table entry structure */

	seqno = 0;
	while (*text >= '0' && *text <= '9') {
		seqno *= 10;
		seqno += *text - '0';
		text++;			/* Advance to the next character. */
	}
	if (*text == '\0') {
		/*
		 *  No explicit machine-specification is present.
		 *  We assume the local machine in this case.
		 */
		*orig_seqno = seqno;
		return (localmid (machine_id));
	}
	else if (*text != '.') {
		/*
		 *  Invalid request-id syntax.
		 */
		return (-1);		/* Report bad request-id syntax */
	}
	/*
 	 *  A machine-specification is present.
	 */
	text++;				/* Scan past '.' */
	if (*text == '\0') {
		/*
		 *  Missing machine-specification.
		 */
		return (-1);		/* Bad request-id syntax */
	}
	/*
	 *  Determine the machine-id of the machine specification
	 *  as present in the request-id text.
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
			*orig_seqno = seqno;	/* Return request sequence# */
			*machine_id = mid_spec;
			return (0);		/* Success */
		}
		return (-1);			/* Invalid syntax */
	}
	/*
	 *  The machine-specification portion of the request-id has been
	 *  specified as a name.
	 */
	if ((ent = gethostbyname (text)) == (struct hostent *) 0) {
		return (-2);			/* This machine is not */
						/* known to us */
	}
	switch (nmap_get_mid (ent, machine_id)) {
	case NMAP_SUCCESS:		/* Successfully got local machine-id */
		*orig_seqno = seqno;	/* Return request sequence# */
		return (0);
	case NMAP_ENOMAP:		/* What?  No local mid defined! */
		return (-2);
	case NMAP_ENOPRIV:		/* No privilege */
		return (-3);
	}
	return (-4);			/* General NMAP_ error */
}
