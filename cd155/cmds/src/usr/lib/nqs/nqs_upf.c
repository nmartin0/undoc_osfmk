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
 
/*++ nqs_upf.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_upf.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS forms set update module.
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
 * $Log: nqs_upf.c,v $
 * Revision 1.3  1994/11/19  02:53:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:26:37  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:44  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:09:49  hender
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
extern int strcmp();		/* String comparison */
extern void udb_addfor();	/* Add form to the NQS forms database */
extern void udb_delfor();	/* Delete form from the NQS forms */
				/* database */


/*** upf_addfor
 *
 *
 *	long upf_addfor():
 *	Add a form to the local NQS forms set.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_ALREADEXI:	if the specified form name is already
 *				present in the NQS forms set.
 */
long upf_addfor (newform)
char *newform;				/* Name of new forms */
{
	if (upf_valfor (newform)) {
		/*
		 *  The specified form is already present in the forms
		 *  set.
		 */
		return (TCML_ALREADEXI);
	}
	udb_addfor (newform);		/* Add the form to the forms set */
	return (TCML_COMPLETE);
}


/*** upf_delfor
 *
 *
 *	long upf_delfor():
 *	Delete a form from the local NQS forms set.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHFORM: if the specified form did not exist
 *				 in the NQS forms set.
 */
long upf_delfor (form)
char *form;				/* Name of forms to delete */
{
	if (upf_valfor (form)) {
		/*
		 *  The specified form to delete is present in the forms
		 *  set.
		 */
		udb_delfor (form);	/* Delete the form from the forms */
					/* set */
		return (TCML_COMPLETE);
	}
	return (TCML_NOSUCHFORM);	/* No such form */
}


/*** upf_valfor
 *
 *
 *	upf_valfor():
 *
 *	Return non-zero if the specified form is defined in the
 *	local NQS forms set.  Otherwise return 0.
 */
int upf_valfor (formname)
register char *formname;		/* Forms name to be validated */
{
	register struct gendescr *descr;

	seekdbb (Formsfile, 0L);	/* Seek to the beginning of the NQS */
					/* forms list file */
	/*
	 *  Search for the entry to be added, to see if it is already
	 *  present.
	 */
	descr = nextdb (Formsfile);
	while (descr != (struct gendescr *) 0) {
		if (strcmp (descr->v.form.forms, formname) == 0) {
			/*
			 *  The forms exists.
			 */
			return (1);	/* Form is valid */
		}
		else descr = nextdb (Formsfile);
	}
	return (0);			/* Form is not valid */
}
