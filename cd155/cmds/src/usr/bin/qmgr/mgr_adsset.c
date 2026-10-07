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
 
/*++ mgr_adsset.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qmgr/mgr_adsset.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS manager "add set", "delete set", and "set set" execution module.
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 15, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1994/11/19 01:35:32 $ $State: Exp $)
 * $Log: mgr_adsset.c,v $
 * Revision 1.5  1994/11/19  01:35:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/11/02  01:07:42  mwan
 * 1.2 mods
 *
 * Revision 1.2  1992/10/09  22:29:52  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:28:28  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:24  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:02:09  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"			/* NQS types and definitions */
#include "nqsmgr.h"			/* Token types and error messages */
#include "transactcc.h"			/* Transaction completion codes */

/*
 *	Declaration of transaction modules.
 */
extern long addfor();			/* Add a form to the forms set */
extern long addnqsman();		/* Add an NQS manager */
extern long addqcomque();		/* Add a queue to a queue complex*/
extern long addquedes();		/* Add a queue destination */
extern long addquedev();		/* Add a queue/device mapping */
extern long addquegid();		/* Add queue access for a gid */
extern long addqueuid();		/* Add queue access for a uid */
extern long delfor();			/* Delete a form from the forms set */
extern long delnqsman();		/* Delete an NQS manager */
extern long delquedes();		/* Delete a queue destination */
extern long delquedev();		/* Delete a queue/device mapping */
extern long delquegid();		/* Delete queue access for a gid */
extern long delqueuid();		/* Delete queue access for a uid */
extern long remqcomque();		/* Remove queue from a queue complex */
extern long setfor();			/* Set the forms set to the single */
					/* specified form */
extern long setnqsman();		/* Set the NQS managers set to */
					/* the single specified manager */
extern long setquedes();		/* Set the queue destination set */
					/* for the specified queue to the */
					/* single specified destination */
extern long setquedev();		/* Set the queue/device mapping set */
					/* for the specified device queue */
					/* to the single specified device */

/*
 *	Miscellaneous functions.
 */
extern char *deq_set();			/* Dequeue text */
extern mid_t deq_mid();			/* Dequeue machine-id */
extern int deq_priv();			/* Dequeue privilege mask */
extern long deq_uid_gid();		/* Dequeue user-id or group-id */
extern char *destqueue();		/* Return pointer to queue name for */
					/* a remote queue specification */
extern void diagnose();			/* Diagnose completion code */
extern void show_failed_prefix();	/* Show TCML_FAILURE prefix */


/*** adsdes_set
 *
 *
 *	void adsdes_set():
 *
 *	Add/delete/set queue destinations set procedure.  The set of
 *	destinations to add/delete/set is defined by the scan_destset()
 *	function which is used to parse a destination set specification.
 */
void adsdes_set (quename, opcode)
char *quename;				/* Corresponding queue name */
int opcode;				/* Operation code = ADD_OP, */
{					/* DEL_OP, or SET_OP. */

	void adsdes_problem();		/* Problem during add/delete/set */
					/* queue destinations. */

	long comcode;			/* Completion code */
	register char *destq;		/* Destination queue name */
	register mid_t destmid;		/* Destination machine-id */
	register short exitflag;	/* Exit flag */
	register short opcode2;		/* Internal opcode */
	short mssgflag;			/* BOOLEAN TRUE if a message has */
					/* been delivered to the user */

	/*
	 *  Loop to add/delete/set queue destinations.
	 */
	mssgflag = 0;
	exitflag = 0;
	opcode2 = opcode;
	while ((destq = deq_set()) != NULL && !exitflag) {
		/*
		 *  Perform the operation for pair.
		 */
		destmid = deq_mid();	/* Get destination machine-id */
		switch (opcode2) {
		case ADD_OP:
			comcode = addquedes (quename, destqueue (destq),
					     destmid);
			break;
		case DEL_OP:
			comcode = delquedes (quename, destqueue (destq),
					     destmid);
			break;
		case SET_OP:
			comcode = setquedes (quename, destqueue (destq),
					     destmid);
			opcode2 = ADD_OP;
			break;
		}
		switch (comcode) {
		case TCML_INTERNERR:
		case TCML_NOESTABLSH:
		case TCML_NOLOCALDAE:
		case TCML_PROTOFAIL:
			adsdes_problem (opcode, quename, destq);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		case TCML_COMPLETE:
			break;
		case TCML_ALREADEXI:
			adsdes_problem (opcode, quename, destq);
			diagnose (comcode);
			mssgflag = 1;
			break;
		case TCML_INSUFFMEM:
		case TCML_MAXQDESTS:
			adsdes_problem (opcode, quename, destq);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		case TCML_NOSUCHDES:
			adsdes_problem (opcode, quename, destq);
			diagnose (comcode);
			mssgflag = 1;
			break;
		case TCML_NOSUCHQUE:
			adsdes_problem (opcode, quename, destq);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		case TCML_SELREFDES:
			adsdes_problem (opcode, quename, destq);
			diagnose (comcode);
			mssgflag = 1;
			break;
		case TCML_WROQUETYP:
			adsdes_problem (opcode, quename, destq);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		default:
			adsdes_problem (opcode, quename, destq);
			diagnose (TCML_UNDEFINED);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
		}
	}
	if (!mssgflag) diagnose (TCML_COMPLETE);
}


/*** adsdes_problem
 *
 *
 *	void adsdes_problem():
 *	Indicate problem adding/deleting/setting queue destination.
 */
static void adsdes_problem (opcode, quename, destname)
int opcode;			/* Opcode = ADD_OP, DEL_OP, SET_OP */
char *quename;			/* Queue name */
char *destname;			/* Destination */
{
	show_failed_prefix();
	switch (opcode) {
	case ADD_OP:
		printf ("Problem adding queue destination:\n");
		show_failed_prefix();
		printf ("%s -> %s.\n", quename, destname);
		break;
	case DEL_OP:
		printf ("Problem deleting queue destination:\n");
		show_failed_prefix();
		printf ("%s -> %s.\n", quename, destname);
		break;
	case SET_OP:
		printf ("Problem setting queue destination:\n");
		show_failed_prefix();
		printf ("%s -> %s.\n", quename, destname);
		break;
	}
}


/*** adsdev_set
 *
 *
 *	void adsdev_set():
 *
 *	Add/delete/set queue to device mappings procedure.  The set
 *	of devices to add/delete/set is defined by the scan_devset()
 *	function which is used to parse a device set specification.
 */
void adsdev_set (quename, opcode)
char *quename;				/* Corresponding queue name */
int opcode;				/* Operation code = ADD_OP, */
{					/* DEL_OP, or SET_OP. */

	void adsdev_problem();		/* Problem during add/delete/set */
					/* queue/device mappings. */

	long comcode;			/* Completion code */
	register char *devname;		/* Device name */
	register short exitflag;	/* Exit flag */
	register opcode2;		/* Internal opcode */
	short mssgflag;			/* BOOLEAN TRUE if a message has */
					/* been delivered to the user */

	/*
	 *  Loop to add/delete/set queue to device mappings.
	 */
	mssgflag = 0;
	exitflag = 0;
	opcode2 = opcode;
	while ((devname = deq_set()) != NULL && !exitflag) {
		/*
		 *  Perform the operation.
		 */
		switch (opcode2) {
		case ADD_OP:
			comcode = addquedev (quename, devname);
			break;
		case DEL_OP:
			comcode = delquedev (quename, devname);
			break;
		case SET_OP:
			comcode = setquedev (quename, devname);
			opcode2 = ADD_OP;
			break;
		}
		switch (comcode) {
		case TCML_INTERNERR:
		case TCML_NOESTABLSH:
		case TCML_NOLOCALDAE:
		case TCML_PROTOFAIL:
			adsdev_problem (opcode, quename, devname);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		case TCML_COMPLETE:
			break;
		case TCML_ALREADEXI:
			adsdev_problem (opcode, quename, devname);
			diagnose (comcode);
			mssgflag = 1;
			break;
		case TCML_INSUFFMEM:
			adsdev_problem (opcode, quename, devname);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		case TCML_NOSUCHDEV:
		case TCML_NOSUCHMAP:
			adsdev_problem (opcode, quename, devname);
			diagnose (comcode);
			mssgflag = 1;
			break;
		case TCML_NOSUCHQUE:
		case TCML_WROQUETYP:
			adsdev_problem (opcode, quename, devname);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		default:
			adsdev_problem (opcode, quename, devname);
			diagnose (TCML_UNDEFINED);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
		}
	}
	if (!mssgflag) diagnose (TCML_COMPLETE);
}


/*** adsdev_problem
 *
 *
 *	void adsdev_problem():
 *	Indicate problem adding/deleting/setting queue/device mapping.
 */
static void adsdev_problem (opcode, quename, devname)
int opcode;			/* Opcode = ADD_OP, DEL_OP, SET_OP */
char *quename;			/* Queue name */
char *devname;			/* Device name */
{
	show_failed_prefix();
	switch (opcode) {
	case ADD_OP:
		printf ("Problem adding queue->device mapping:\n");
		show_failed_prefix();
		printf ("%s -> %s.\n", quename, devname);
		break;
	case DEL_OP:
		printf ("Problem deleting queue->device mapping:\n");
		show_failed_prefix();
		printf ("%s -> %s.\n", quename, devname);
		break;
	case SET_OP:
		printf ("Problem setting queue->device mapping:\n");
		show_failed_prefix();
		printf ("%s -> %s.\n", quename, devname);
		break;
	}
}


/*** adsfor_set
 *
 *
 *	void adsfor_set():
 *
 *	Add/delete/set forms set.  The set of forms to add/delete/set
 *	is defined by the scan_forset() function which is used to
 *	scan/parse a forms set specification.
 */
void adsfor_set (opcode)
int opcode;				/* Operation code = ADD_OP, */
{					/* DEL_OP, or SET_OP. */

	void adsfor_problem();		/* Problem during add/delete/set */
					/* of forms set */

	long comcode;			/* Completion code */
	register char *formname;	/* Form name */
	register short exitflag;	/* Exit flag */
	register opcode2;		/* Internal opcode */
	short mssgflag;			/* BOOLEAN TRUE if a message has */
					/* been delivered to the user */

	/*
	 *  Loop to add/delete/set forms set.
	 */
	mssgflag = 0;
	exitflag = 0;
	opcode2 = opcode;
	while ((formname = deq_set()) != NULL && !exitflag) {
		/*
		 *  Perform the operation.
		 */
		switch (opcode2) {
		case ADD_OP:
			comcode = addfor (formname);
			break;
		case DEL_OP:
			comcode = delfor (formname);
			break;
		case SET_OP:
			comcode = setfor (formname);
			opcode2 = ADD_OP;
			break;
		}
		switch (comcode) {
		case TCML_INTERNERR:
		case TCML_NOESTABLSH:
		case TCML_NOLOCALDAE:
		case TCML_PROTOFAIL:
			adsfor_problem (opcode, formname);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		case TCML_COMPLETE:
			break;
		case TCML_ALREADEXI:
			adsfor_problem (opcode, formname);
			diagnose (comcode);
			mssgflag = 1;
			break;
		case TCML_NOSUCHFORM:
			adsfor_problem (opcode, formname);
			diagnose (comcode);
			mssgflag = 1;
			break;
		default:
			adsfor_problem (opcode, formname);
			diagnose (TCML_UNDEFINED);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
		}
	}
	if (!mssgflag) diagnose (TCML_COMPLETE);
}


/*** adsfor_problem
 *
 *
 *	void adsfor_problem():
 *	Indicate problem adding/deleting/setting forms set.
 */
static void adsfor_problem (opcode, formname)
int opcode;			/* Opcode = ADD_OP, DEL_OP, SET_OP */
char *formname;			/* Forms name */
{
	show_failed_prefix();
	switch (opcode) {
	case ADD_OP:
		printf ("Problem adding form: %s to forms set.\n", formname);
		break;
	case DEL_OP:
		printf ("Problem deleting form: %s from forms set.\n",
			formname);
		break;
	case SET_OP:
		printf ("Problem setting forms set to include: %s.\n",
			formname);
		break;
	}
}


/*** adsgid_set
 *
 *
 *	void adsgid_set():
 *
 *	Add/delete/set gidset.  The set of gids (group id's) to
 *	add/delete/set is defined by the scan_groupset() function
 *	which is used to scan/parse a gid set specification.
 *	Setting is currently impossible.
 */
void adsgid_set (quename, opcode)
char *quename;				/* Queue name */
int opcode;				/* Operation code = ADD_OP or DEL_OP */
{

	void adsgid_problem();		/* Problem during add/delete/set */
					/* of gid set */

	long comcode;			/* Completion code */
	register short exitflag;	/* Exit flag */
	char *group_name;		/* Group as specified by qmgr user */
	long gid;			/* Group-id */
	short mssgflag;			/* BOOLEAN TRUE if a message has */
					/* been delivered to the user */

	/*
	 *  Loop to add/delete/set gid set.
	 */
	mssgflag = 0;
	exitflag = 0;
	while ((group_name = deq_set()) != NULL && !exitflag) {
		/*
		 *  Perform the operation.
		 */
		gid = deq_uid_gid(); /* Get user-id */
		if (gid != -1) {
			/*
			 *  The group exists, as far as we can tell....
			 */
			switch (opcode) {
			case ADD_OP:
				comcode = addquegid (quename, (gid_t) gid);
				break;
			case DEL_OP:
				comcode = delquegid (quename, (gid_t) gid);
				break;
			}
		}
		else comcode = TCML_NOSUCHGRP;	/* No such group */
		switch (comcode) {
		case TCML_INTERNERR:
		case TCML_NOESTABLSH:
		case TCML_NOLOCALDAE:
		case TCML_PROTOFAIL:
			adsgid_problem (opcode, quename, group_name);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		case TCML_COMPLETE:
			break;
		case TCML_ALREADACC:
		case TCML_NOACCNOW:
		case TCML_NOSUCHGRP:
		case TCML_NOSUCHQUE:
		case TCML_UNRESTR:
			adsgid_problem (opcode, quename, group_name);
			diagnose (comcode);
			mssgflag = 1;
			break;
		default:
			adsgid_problem (opcode, quename, group_name);
			diagnose (TCML_UNDEFINED);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
		}
	}
	if (!mssgflag) diagnose (TCML_COMPLETE);
}


/*** adsgid_problem
 *
 *
 *	void adsgid_problem():
 *	Indicate problem adding/deleting/setting gid set.
 */
static void adsgid_problem (opcode, quename, group_name)
int opcode;			/* Opcode = ADD_OP or DEL_OP*/
char *quename;				/* Queue name */
char *group_name;			/* Group as specified by qmgr user */
{
	show_failed_prefix();
	switch (opcode) {
	case ADD_OP:
		printf ("Problem adding group: %s; queue = %s.\n",
			group_name, quename);
		break;
	case DEL_OP:
		printf ("Problem deleting group: %s; queue = %s.\n",
			group_name, quename);
		break;
	}
}


/*** adsmgr_set
 *
 *
 *	void adsmgr_set():
 *
 *	Add/delete/set manager set.  The set of manager accounts to
 *	add/delete/set is defined by the scan_mgrset() function which
 *	is used to scan/parse a manager account set specification.
 */
void adsmgr_set (opcode)
int opcode;				/* Operation code = ADD_OP, */
{					/* DEL_OP, or SET_OP. */

	void adsmgr_problem();		/* Problem during add/delete/set */
					/* of manager set */

	long comcode;			/* Completion code */
	register short exitflag;	/* Exit flag */
	register opcode2;		/* Internal opcode */
	char *mgr_name;			/* Manager account name */
	mid_t mgr_mid;			/* Manager machine-id */
	long mgr_uid;			/* Manager user-id */
	short privileges;		/* Privileges mask */
	short mssgflag;			/* BOOLEAN TRUE if a message has */
					/* been delivered to the user */

	/*
	 *  Loop to add/delete/set manager set.
	 */
	mssgflag = 0;
	exitflag = 0;
	if (opcode == SET_OP) {
		setnqsman();		/* Reduce set to only "root" */
		opcode2 = ADD_OP;	/* SET becomes ADD */
	}
	else opcode2 = opcode;
	while ((mgr_name = deq_set()) != NULL && !exitflag) {
		/*
		 *  Perform the operation.
		 */
		mgr_mid = deq_mid();	/* Get manager machine-id */
		mgr_uid = deq_uid_gid(); /* Get manager user-id */
		privileges = deq_priv();/* Get manager privilege mask */
		if (mgr_uid != -1) {
			/*
			 *  The account exists, as far as we can tell....
			 */
#ifdef SDSC
		    if (mgr_uid == 0) {		/* root */
			comcode = TCML_COMPLETE;
		    } else {
#endif
			switch (opcode2) {
			case ADD_OP:
				comcode = addnqsman ((uid_t) mgr_uid, mgr_mid,
						     privileges);
				break;
			case DEL_OP:
				comcode = delnqsman ((uid_t) mgr_uid, mgr_mid,
						     privileges);
				break;
			}
#ifdef SDSC
		    }
#endif
		}
		else comcode = TCML_NOSUCHACC;	/* No such account */
		switch (comcode) {
		case TCML_INTERNERR:
		case TCML_NOESTABLSH:
		case TCML_NOLOCALDAE:
		case TCML_PROTOFAIL:
			adsmgr_problem (opcode, mgr_name);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		case TCML_COMPLETE:
			break;
		case TCML_ALREADEXI:
		case TCML_NOSUCHACC:
		case TCML_NOSUCHMAN:
		case TCML_ROOTINDEL:
			adsmgr_problem (opcode, mgr_name);
			diagnose (comcode);
			mssgflag = 1;
			break;
		default:
			adsmgr_problem (opcode, mgr_name);
			diagnose (TCML_UNDEFINED);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
		}
	}
	if (!mssgflag) diagnose (TCML_COMPLETE);
}


/*** adsmgr_problem
 *
 *
 *	void adsmgr_problem():
 *	Indicate problem adding/deleting/setting manager set.
 */
static void adsmgr_problem (opcode, mgr_name)
int opcode;			/* Opcode = ADD_OP, DEL_OP, SET_OP */
char *mgr_name;			/* Manager account name */
{
	show_failed_prefix();
	switch (opcode) {
	case ADD_OP:
		printf ("Problem adding account: %s to manager set.\n",
			mgr_name);
		break;
	case DEL_OP:
		printf ("Problem deleting account: %s from manager set.\n",
			mgr_name);
		break;
	case SET_OP:
		printf ("Problem setting manager set to include: %s.\n",
			mgr_name);
		break;
	}
}


/*** adsque_set
 *
 *
 *	void adsque_set():
 *
 *	Add/remove/set queue set.  The set of scheduler queues to
 *	add/remove/set is defined by the scan_queset() function
 *	which is used to scan/parse a queue set specification.
 *	Setting is currently impossible.
 */
void adsque_set (qcom_name, opcode)
char *qcom_name;			/* Queue complex name */
int opcode;				/* Operation code = ADD_OP or DEL_OP */
{

	void adsque_problem();		/* Problem during add/remove/set */
					/* of queue set */

	long comcode;			/* Completion code */
	register short exitflag;	/* Exit flag */
	char *que_name;			/* Queue complex name */
	short mssgflag;			/* BOOLEAN TRUE if a message has */
					/* been delivered to the user */

	/*
	 *  Loop to add/remove/set queue set.
	 */
	mssgflag = 0;
	exitflag = 0;
	while ((que_name = deq_set()) != NULL && !exitflag) {
		/*
		 *  Perform the operation.
		 */
		switch (opcode) {
		case ADD_OP:
			comcode = addqcomque (que_name, qcom_name);
			break;
		case DEL_OP:
			comcode = remqcomque (que_name, qcom_name);
			break;
		}
		switch (comcode) {
		case TCML_INSUFFMEM:
		case TCML_INTERNERR:
		case TCML_NOESTABLSH:
		case TCML_NOLOCALDAE:
		case TCML_PROTOFAIL:
			adsque_problem (opcode, que_name, qcom_name);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		case TCML_COMPLETE:
			break;
		case TCML_ALREADEXI:
		case TCML_ALREADCOM:	/* CERN Boissat */
		case TCML_NOSUCHCOM:
		case TCML_NOSUCHQUE:
		case TCML_TOOMANCOM:    /* CERN Boissat */
		case TCML_WROQUETYP:
			adsque_problem (opcode, que_name, qcom_name);
			diagnose (comcode);
			mssgflag = 1;
			break;
		default:
			adsque_problem (opcode, que_name, qcom_name);
			diagnose (TCML_UNDEFINED);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
		}
	}
	if (!mssgflag) diagnose (TCML_COMPLETE);
}


/*** adsque_problem
 *
 *
 *	void adsque_problem():
 *	Indicate problem adding/removing/setting queue set.
 */
static void adsque_problem (opcode, que_name, qcom_name)
int opcode;				/* Opcode = ADD_OP or DEL_OP */
char *que_name;				/* Queue name */
char *qcom_name;			/* Queue complex name */
{
	show_failed_prefix();
	switch (opcode) {
	case ADD_OP:
		printf ("Problem adding queue: %s; queue complex = %s.\n",
			que_name, qcom_name);
		break;
	case DEL_OP:
		printf ("Problem removing queue: %s; queue complex = %s.\n",
			que_name, qcom_name);
		break;
	}
}


/*** adsuid_set
 *
 *
 *	void adsuid_set():
 *
 *	Add/delete/set uidset.  The set of uids (user id's) to
 *	add/delete/set is defined by the scan_userset() function
 *	which is used to scan/parse a uid set specification.
 *	Setting is currently impossible.
 */
void adsuid_set (quename, opcode)
char *quename;				/* Queue name */
int opcode;				/* Operation code = ADD_OP or DEL_OP */
{

	void adsuid_problem();		/* Problem during add/delete/set */
					/* of uid set */

	long comcode;			/* Completion code */
	register short exitflag;	/* Exit flag */
	char *user_name;		/* User account name */
	long uid;			/* User-id */
	short mssgflag;			/* BOOLEAN TRUE if a message has */
					/* been delivered to the user */

	/*
	 *  Loop to add/delete/set uid set.
	 */
	mssgflag = 0;
	exitflag = 0;
	while ((user_name = deq_set()) != NULL && !exitflag) {
		/*
		 *  Perform the operation.
		 */
		uid = deq_uid_gid(); /* Get user-id */
		if (uid != -1) {
			/*
			 *  The account exists, as far as we can tell....
			 */
			switch (opcode) {
			case ADD_OP:
				comcode = addqueuid (quename, (uid_t) uid);
				break;
			case DEL_OP:
				comcode = delqueuid (quename, (uid_t) uid);
				break;
			}
		}
		else comcode = TCML_NOSUCHACC;	/* No such account */
		switch (comcode) {
		case TCML_INTERNERR:
		case TCML_NOESTABLSH:
		case TCML_NOLOCALDAE:
		case TCML_PROTOFAIL:
			adsuid_problem (opcode, quename, user_name);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		case TCML_COMPLETE:
			break;
		case TCML_ALREADACC:
		case TCML_NOACCNOW:
		case TCML_NOSUCHACC:
		case TCML_NOSUCHQUE:
		case TCML_UNRESTR:
			adsuid_problem (opcode, quename, user_name);
			diagnose (comcode);
			mssgflag = 1;
			break;
		default:
			adsuid_problem (opcode, quename, user_name);
			diagnose (TCML_UNDEFINED);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
		}
	}
	if (!mssgflag) diagnose (TCML_COMPLETE);
}


/*** adsuid_problem
 *
 *
 *	void adsuid_problem():
 *	Indicate problem adding/deleting/setting uid set.
 */
static void adsuid_problem (opcode, quename, user_name)
int opcode;				/* Opcode = ADD_OP or DEL_OP */
char *quename;				/* Queue name */
char *user_name;			/* Account name */
{
	show_failed_prefix();
	switch (opcode) {
	case ADD_OP:
		printf ("Problem adding user: %s; queue = %s.\n",
			user_name, quename);
		break;
	case DEL_OP:
		printf ("Problem deleting user: %s; queue = %s.\n",
			user_name, quename);
		break;
	}
}
