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
 
/*++ mgr_cmd.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qmgr/mgr_cmd.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS manager command execution module.
 *
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
 * REVISION HISTORY: ($Revision: 1.7 $ $Date: 1995/02/24 23:31:11 $ $State: Exp
$)
 * $Log: mgr_cmd.c,v $
 * Revision 1.7  1995/02/24  23:31:11  kremenek
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 5134
 *  Testing: Developer testing
 *  Module(s):	 ./cmds_libs/src/usr/bin/qmgr/mgr_cmd.c
 * 		./cmds_libs/src/usr/bin/qmgr/mgr_main.c
 * 		./cmds_libs/src/usr/bin/qmgr/mgr_packet.c
 * 		./cmds_libs/src/usr/bin/qmgr/qmgr.hlp
 * 		./cmds_libs/src/usr/ccs/lib/libnqs/listq.c
 * 		./cmds_libs/src/usr/include/nqs/nqspacket.h
 * 		./cmds_libs/src/usr/include/nqs/nqsstruct.h
 * 		./cmds_libs/src/usr/lib/nqs/macs_sched.c
 * 		./cmds_libs/src/usr/lib/nqs/nqs_ldconf.c
 * 		./cmds_libs/src/usr/lib/nqs/nqs_main.c
 * 		./cmds_libs/src/usr/lib/nqs/nqs_nsq.c
 * 		./cmds_libs/src/usr/lib/nqs/nqs_upq.
 *
 * Revision 1.6  1994/12/08  00:16:27  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 3801
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/bin/qmgr/mgr_cmd.c
 * 		cmds_libs/src/usr/bin/qmgr/qmgr.hlp
 *
 * Revision 1.5  1994/11/19  01:35:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/11/02  01:07:46  mwan
 * 1.2 mods
 *
 * Revision 1.2  1992/10/09  22:29:55  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:28:28  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:26  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:02:16  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include <pwd.h>			/* Password file definitions */
#include "nqs.h"			/* NQS types and definitions */
#include "nqsxvars.h"			/* NQS external vars */
#include "nqsmgr.h"			/* Token types and error messages */
#include "transactcc.h"			/* Transaction completion codes */

/*
 *	Declaration of transaction modules.
 */
extern long aboque();			/* Abort queue */
extern long crebatque();		/* Create a new batch queue */
extern long crecom();			/* Create queue complex */
extern long credev();			/* Create a new device */
extern long credevque();		/* Create a new device queue */
extern long crepipque();		/* Create a new pipe queue */
extern long delcom();                   /* Delete queue complex */
extern long deldev();			/* Delete a device */
extern long delreq();			/* Delete a req */
extern long delque();			/* Delete a queue */
extern long disdev();			/* Disable a device */
extern long disque();			/* Disable a queue */
extern long enadev();			/* Enable a device */
extern long enaque();			/* Enable a queue */
extern long locdae();			/* Lock NQS daemon in memory */
extern long modreq();			/* Modify a req */
extern long movque();			/* Move a queue */
extern long nqsshutdn();		/* Shutdown the NQS daemon */
extern long purque();			/* Purge a queue */
extern long setbatpri();		/* Set default priority for */
					/* batch reqs */
extern long setbatque();		/* Set the default batch queue */
extern long setcomrun();		/* Set the queue complex run limit */
extern long setdeblev();		/* Set the debug message level */
extern long setdestim();		/* Set the default destination */
					/* retry state tolerance time */
extern long setdeswai();		/* Set the default destination */
					/* wait retry time */
extern long setdevfor();		/* Set forms in device */
extern long setdevpri();		/* Set default priority for */
					/* device reqs */
extern long setdevser();		/* Set server for device */
extern long setlife();			/* Set lifetime of all NQS reqs */
					/* residing in a pipe queue */
extern long setlogfil();		/* Set the NQS log file */
extern long setmaxcop();		/* Set max copies for an NQS */
					/* device req */
extern long setmaxopr();		/* Set max failed device open */
					/* retry limit */
extern long setmaxpsz();		/* Set max size of a print req */
extern long setndfbat();		/* Set no default batch queue */
extern long setndffor();		/* Set no default print form */
extern long setndfpri();		/* Set no default print queue */
extern long setnetcli();		/* Set network client */
extern long setnetdae();		/* Set network daemon */
extern long setnetser();		/* Set network server */
extern long setnnedae();		/* Set no network daemon */
extern long setnoacc();			/* Set queue access to none */
extern long setnoponl();		/* Clear pipeonly attribute for queue */
extern long setnqsmai();		/* Change the NQS mail account */
extern long setopewai();		/* Set time to wait between failed */
					/* device open operations */
extern long setpipcli();		/* Set the pipe client for the */
					/* specified pipe queue */
extern long setpiponl();		/* Set pipeonly attribute for queue */
extern long setppcore();		/* Set per-process core file */
					/* size limit for a queue */
extern long setppcput();		/* Set per-process cpu time limit */
					/* for a queue */
extern long setppdata();		/* Set per-process data segment */
					/* size limit for a queue */
extern long setppmem();			/* Set per-process memory size */
					/* limit for a queue */
extern long setppnice();		/* Set nice value for a queue */
extern long setpppfile();		/* Set per-process permanent file */
					/* size limit for a queue */
extern long setppstack();		/* Set per-process stack size */
					/* limit for a queue */
extern long setpptfile();		/* Set per-process temporary file */
					/* size limit for a queue */
extern long setppwork();		/* Set working set size limit */
					/* for a queue */
extern long setprcput();		/* Set per-request cpu time limit */
					/* for a queue */
extern long setprifor();		/* Set the default print-req forms */
extern long setprique();		/* Set the default print-req queue */
extern long setprmem();			/* Set per-request memory size limit */
					/* for a queue */
extern long setprpfile();		/* Set per-request permanent file */
					/* size limit for a queue */
extern long setprtfile();		/* Set per-request temporary file */
					/* size limit for a queue */
extern long setquepri();		/* Set the priority of the */
					/* specified queue */
extern long setquerun();		/* Set the maximum number of reqs */
					/* allowed to run in a batch or */
					/* pipe queue */
#ifdef SDSC
extern long setquequed();               /* Set the maximum number of reqs */
                                        /* allowed to be in a running and */
                                        /* queued state in a batch queue */
#endif
extern long setshsfix();		/* Set a fixed shell strategy */
extern long setshsfre();		/* Set a free shell strategy */
extern long setshslog();		/* Set a login shell strategy */
extern long setunracc();		/* Set queue access to unrestricted */
extern long staque();			/* Start the specified queue */
extern long stoque();			/* Stop the specified queue */
extern long unldae();			/* Unlock the NQS daemon from */
					/* memory, making it vulnerable */
					/* to swapping */
/*
 *	Miscellaneous functions.
 */
extern void adsdes_set();		/* Add/delete/set queue destinations */
extern void adsdev_set();		/* Add/delete/set queue to device */
					/* mappings */
extern void adsfor_set();		/* Add/delete/set forms set */
extern void adsgid_set();		/* Add/delete/set group id set */
extern void adsmgr_set();		/* Add/delete/set manager set */
extern void adsque_set();		/* Add/remove/set queue to complex */
extern void adsuid_set();		/* Add/delete/set user id set */
extern void closepwdb();		/* Close account/password database */
extern char *deq_set();			/* Dequeue text */
extern void diagnose();			/* Diagnose completion code */
extern void errormessage();		/* Report error message */
extern void exiting();			/* Relinquish connection to the */
					/* local NQS daemon */
extern struct passwd *fetchpwnam();	/* Fetch password entry for name */
extern void fsizedb();			/* Update database file size */
extern void modreq_set();		/* Modify a request parameter set */
extern void movreq_set();		/* Move a request set */
extern char *scan_allinparens();	/* Scan into a single literal token */
					/* everything up to a right paren */
extern char *scan_aname();		/* Scan a LOCAL account name */
extern int scan_destset();		/* Scan a destination set */
extern int scan_devset();		/* Scan a device set */
extern char *scan_dename();		/* Scan a device name or '=' */
extern char *scan_dfname();		/* Scan a device full-name */
extern char *scan_dname();		/* Scan a device name */
extern int scan_end();			/* Scan end of command */
extern int scan_equals();		/* Scan an "=" character token */
extern void scan_error();		/* Scan error */
extern char *scan_fname();		/* Scan a form name */
extern int scan_forset();		/* Scan a forms set as the last */
					/* parse construct in a command line */
extern int scan_groupset();		/* Scan a group set */
extern int scan_int();			/* Scan an integer value */
extern char *scan_logname();		/* Scan a logfile path name */
extern int scan_mgrset();		/* Scan an NQS manager account set */
					/* as the last parse construct in a */
					/* command line */
extern int scan_opaname();		/* Scan optional local account name */
					/* (account name or EOL) */
extern int scan_opdev();		/* Scan an optional device name */
					/* (device name or EOL) */
extern int scan_opqueue();		/* Scan an optional queue name */
					/* (queue name or EOL) */
extern int scan_opwait();		/* Scan an optional wait time */
					/* as the LAST token in a command */
					/* line */
extern char *scan_qcomname();		/* Scan a queue complex name */
extern char *scan_qname();		/* Scan a queue name */
extern int scan_qset();			/* Scan a queue set */
extern int scan_qualifier();		/* Scan a qualifier */
extern int scan_reqset();		/* Scan a request set */
extern int scan_reqidname();		/* Scan a request name */
extern int scan_reqmodset();		/* Scan a request modifier set */
extern int scan_userset();		/* Scan a user set */
extern int scancpulim();		/* Parse a cpu time limit */
extern int scanquolim();		/* Parse a quota limit */
extern int scan_string();		/* Scan a literal string */
extern char *strcpy();			/* String copy */


extern int Mgr_priv;			/* Privilege bits for current */
					/* user of Qmgr as imported from */
					/* mgr_main.c */

/*** v_aboque
 *
 *
 *	void v_aboque():
 *	ABort Queue command.
 */
void v_aboque()
{
	char quename [MAX_QUEUENAME+1];	/* Name of queue to be aborted */
	char *cp;			/* Pointer to queue name */
	int wait_time;			/* Abort SIGKILL wait time */
	long long_int;

	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);				/* Save queue name */
	if (scan_opwait (&long_int, (long) ABORT_WAIT, 0L, 600L,
			 EM_INTWAITIMEXP, EM_WAITIMOUTOFBOU) == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	wait_time = long_int;
	diagnose (aboque (quename, wait_time));
}


/*** v_adddes
 *
 *
 *	void v_adddes():
 *	ADd DEStination command.
 */
void v_adddes()
{

	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	/*
	 *  Scan the set of specified destinations.
	 */
	if (scan_destset() == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsdes_set (quename, ADD_OP);	/* Add the destinations */
	fsizedb (Qmapfile);		/* Mappings file may have expanded */
	fsizedb (Pipeqfile);		/* Pipe queue destination file */
					/* may have expanded */
}


/*** v_adddev
 *
 *
 *	void v_adddev():
 *	ADd DEVice command.
 */
void v_adddev()
{
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	/*
	 *  Scan the set of specified devices.
	 */
	if (scan_devset() == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsdev_set (quename, ADD_OP);	/* Add the queue/device mappings */
	fsizedb (Qmapfile);		/* Mappings file may have expanded */
}


/*** v_addfor
 *
 *
 *	void v_addfor():
 *	ADd Form command.
 */
void v_addfor()
{
	if (scan_forset() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsfor_set (ADD_OP);		/* Add the forms to the forms set */
	fsizedb (Formsfile);		/* Forms file may have expanded */
}


/*** v_addgid
 *
 *
 *	void v_addgid():
 *	ADd Groups command.
 */
void v_addgid()
{
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	/*
	 *  Scan the set of specified groups.
	 */
	if (scan_groupset() == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsgid_set (quename, ADD_OP);	/* Add the access by group */
}


/*** v_addman
 *
 *
 *	void v_addman():
 *	ADd Manager command.
 */
void v_addman()
{
	if (scan_mgrset() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsmgr_set (ADD_OP);		/* Add the accounts to the mgr set */
	fsizedb (Mgrfile);		/* Manager file may have expanded */
}


/*** v_addque
 *
 *
 *	void v_addque():
 *	ADd Queue to complex command.
 */
void v_addque()
{
	char qcom_name [MAX_QCOMPLXNAME+1];	/* Queue complex name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	/*
	 *  Scan the set of specified queues.
	 */
	if (scan_queset() == -1) return;
	if ((cp = scan_qcomname()) == NULL) return;
	strcpy (qcom_name, cp);		/* Save queue complex name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsque_set (qcom_name, ADD_OP);	/* Add the queues */
	fsizedb (Qmapfile);		/* Mappings file may have expanded */
}


/*** v_adduid
 *
 *
 *	void v_adduid():
 *	ADd Users command.
 */
void v_adduid()
{
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	/*
	 *  Scan the set of specified users.
	 */
	if (scan_userset() == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsuid_set (quename, ADD_OP);	/* Add the access by user */
}


/*** v_crebatque
 *
 *
 *	void v_crebatque():
 *	Create Batch_queue command.
 */
void v_crebatque()
{
	static char *qualifier_set[] = {
		"PIpeonly",
		"PRiority",
		"Run_limit",
		NULL
	};

	long comcode;			/* Completion code */
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	long long_int;			/* Used for integer scanning */
	int priority;			/* Queue priority */
	int runlimit;			/* Queue run-limit */
	short pipeonly;			/* BOOLEAN Pipeonly attr */
	register char *cp;		/* Character pointer */
	register int token;		/* Scan token */
	/*
	 *  Boolean variables:
	 */
	short pr_seen;			/* PRiority seen */
	short rl_seen;			/* Run_limit seen */

	/*
	 *  No attributes seen yet.
	 */
	pr_seen = 0;
	rl_seen = 0;
	pipeonly = 0;
	runlimit = 1;			/* Default run limit */
	/*
	 *  Get the name of the queue to be created.
	 */
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	/*
	 *  Get the queue attributes.
	 */
	while ((token = scan_qualifier (qualifier_set)) > 0) {
		/*
		 *  A qualifier keyword has been recognized.
		 */
		switch (token) {
		case 1:			/* Pipeonly */
			pipeonly = 1;	/* Queue has the pipeonly attr */
			break;
		case 2:			/* Priority */
			if (pr_seen) {
				errormessage (EM_MULPRI);
				return;
			}
			pr_seen = 1;	/* Set the seen flag */
			if (scan_equals() == -1) return;
			if (scan_int (&long_int, 0L,
				     (long) MAX_QPRIORITY,
				      EM_INTPRIEXP,
				      EM_PRIOUTOFBOU) == -1) return;
			/*
			 *  Successful priority value scan.
			 */
			priority = long_int;
			break;
		case 3:			/* Run_limit */
			if (rl_seen) {
				errormessage (EM_MULRUNLIM);
				return;
			}
			rl_seen = 1;	/* Set the seen flag */
			if (scan_equals() == -1) return;
			if (scan_int (&long_int, 1L, (long) MAX_QRUNLIMIT,
				      EM_INTRUNLIMEXP,
				      EM_RUNLIMOUTOFBOU) == -1) {
				return;
			}
			/*
			 *  Good run-limit value.
			 */
			runlimit = long_int;
			break;
		}
	}
	if (token != 0) return;			/* An error occurred */
	/*
	 *  The end of the command has been reached.
	 */
	if (!pr_seen) {
		errormessage (EM_NOPRISPE);	/* No priority specified */
		return;
	}
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	/*
	 *  Create the new queue.
	 */
	if ((comcode = crebatque (quename, priority,
		runlimit, pipeonly)) == TCML_COMPLETE) {
		fsizedb (Queuefile);		/* Queue file size  */
						/* may have changed */
		printf( "Queue %s created.\n", quename);
		/* There is nothing more to add to a BATCH queue */
		diagnose (TCML_COMPLETE);
	}
	else diagnose (comcode);		/* Diagnose failure */
}


/*** v_crecom
 *
 *
 *	void v_crecom():
 *	Create Complex command.
 */
void v_crecom()
{
	char qcom_name [MAX_QCOMPLXNAME+1];	/* Queue complex name */
	char *cp;			/* Pointer to queue name */
#ifdef SDSC
	long retcode;
#endif

	if (scan_equals() == -1) return;
	/*
	 *  Scan the set of specified queues.
	 */
	if (scan_queset() == -1) return;
	if ((cp = scan_qcomname()) == NULL) return;
	strcpy (qcom_name, cp);		/* Save queue complex name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
#ifdef SDSC
	retcode = crecom (qcom_name);	/* Create the queue complex */
	if (retcode == TCML_COMPLETE) {
	    fsizedb (Qmapfile);		/* Mappings file may have expanded */
	    adsque_set (qcom_name, ADD_OP);	/* Add the queues */
	    fsizedb (Qmapfile);		/* Mappings file may have expanded */
	} else {
	    diagnose(retcode);		/* output error */
	}
#else
	diagnose(crecom (qcom_name));	/* Create the queue complex */
	fsizedb (Qmapfile);		/* Mappings file may have expanded */
	adsque_set (qcom_name, ADD_OP);	/* Add the queues */
	fsizedb (Qmapfile);		/* Mappings file may have expanded */
#endif
}


/*** v_credev
 *
 *
 *	void v_credev():
 *	Create DEVICE command.
 */
void v_credev()
{
	static char *qualifier_set[] = {
		"FOrms",
		"FUllname",
		"Server",
		NULL
	};

	char devname [MAX_DEVNAME+1];	/* New device name */
	char formsname [MAX_FORMNAME+1];/* Forms name for device */
	char fullname [MAX_PATHNAME+1];	/* Full device name */
	char srvname [MAX_SERVERNAME+1];/* Server name */
	register char *cp;		/* Character pointer */
	register int token;		/* Scan token */

	/*
	 *  Boolean variables:
	 */
	short fo_seen;			/* FOrms seen */
	short fu_seen;			/* FUllname seen */
	short se_seen;			/* Server seen */

	/*
	 *  No attributes seen yet.
	 */
	fo_seen = 0;
	fu_seen = 0;
	se_seen = 0;
	/*
	 *  Get the name of the device to be created.
	 */
	if ((cp = scan_dname()) == NULL) return;
	strcpy (devname, cp);			/* Save device name */
	/*
	 *  Get the device attributes.
	 */
	while ((token = scan_qualifier (qualifier_set)) > 0) {
		/*
		 *  A qualifier keyword has been recognized.
		 */
		switch (token) {
		case 1:				/* FOrms */
			if (fo_seen) {
				errormessage (EM_MULDEVFOR);
				return;
			}
			fo_seen = 1;		/* Set the seen flag */
			if (scan_equals() == -1) return;
			if ((cp = scan_fname()) == NULL) return;
			strcpy (formsname, cp);	/* Save forms name */
			break;
		case 2:				/* FUllname */
			if (fu_seen) {
				errormessage (EM_MULDEVFULNAM);
				return;
			}
			fu_seen = 1;		/* Set the seen flag */
			if (scan_equals() == -1) return;
			if ((cp = scan_dfname()) == NULL) return;
			strcpy (fullname, cp);	/* Save full device-name */
			break;
		case 3:
			if (se_seen) {
				errormessage (EM_MULSER);
				return;
			}
			se_seen = 1;		/* Set the seen flag */
			if (scan_equals() == -1) return;
			if ((cp = scan_allinparens()) == NULL) return;
			strcpy (srvname, cp);	/* Save server specification */
			break;
		}
	} 
	/*
	 *  The end of the command has been reached.
	 */
	if (!fo_seen) {
		errormessage (EM_NODEVFORSPE);	/* No device forms specified */
		return;
	}
	if (!fu_seen) {
		errormessage(EM_NODEVFULNAMSPE);/* No device fullname */
		return;				/* specified */
	}
	if (!se_seen) {
		errormessage (EM_NOSERSPE);	/* No server specified */
		return;
	}
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	/*
	 *  Create the device.
	 */
	diagnose (credev (devname, formsname, fullname, srvname));
	fsizedb (Devicefile);			/* Device file size */
						/* may have changed */
}


/*** v_credevque
 *
 *
 *	void v_credevque():
 *	Create DEVICE_queue command.
 */
void v_credevque()
{
	static char *qualifier_set[] = {
		"Device",
		"PIpeonly",
		"PRiority",
		NULL
	};

	long comcode;			/* Completion code */
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	long long_int;			/* Used for integer scanning */
	int priority;			/* Queue priority */
	short pipeonly;			/* BOOLEAN Pipeonly attr */
	register char *cp;		/* Character pointer */
	register int token;		/* Scan token */
	/*
	 *  Boolean variables:
	 */
	short de_seen;			/* Device seen */
	short pr_seen;			/* PRiority seen */

	/*
	 *  No attributes seen yet.
	 */
	de_seen = 0;
	pr_seen = 0;
	pipeonly = 0;
	/*
	 *  Get the name of the queue to be created.
	 */
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	/*
	 *  Get the queue attributes.
	 */
	while ((token = scan_qualifier (qualifier_set)) > 0) {
		/*
		 *  A qualifier keyword has been recognized.
		 */
		switch (token) {
		case 1:			/* Device set */
			if (de_seen) {
				errormessage (EM_MULDEVSET);
				return;
			}
			de_seen = 1;	/* Set the seen flag */
			if (scan_equals() == -1) return;
			/*
			 *  Scan the set of specified devices.
			 */
			if (scan_devset() == -1) return;
			break;
		case 2:			/* Pipeonly */
			pipeonly = 1;	/* Queue has the pipeonly attr */
			break;
		case 3:			/* Priority */
			if (pr_seen) {
				errormessage (EM_MULPRI);
				return;
			}
			pr_seen = 1;	/* Set the seen flag */
			if (scan_equals() == -1) return;
			if (scan_int (&long_int, 0L,
				     (long) MAX_QPRIORITY,
				      EM_INTPRIEXP,
				      EM_PRIOUTOFBOU) == -1) return;
			/*
			 *  Successful priority value scan.
			 */
			priority = long_int;
			break;
		}
	}
	if (token != 0) return;			/* An error occurred */
	/*
	 *  The end of the command has been reached.
	 */
	if (!pr_seen) {
		errormessage (EM_NOPRISPE);	/* No priority specified */
		return;
	}
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	/*
	 *  Create the new queue.
	 */
	if ((comcode = credevque (quename, priority,
				  pipeonly)) == TCML_COMPLETE) {
		/*
		 *  We were successful in our efforts to create the
		 *  device queue.  Now add queue/device mappings
		 *  as necessary.
		 */
		fsizedb (Queuefile);		/* Queue file size may have */
						/* gotten bigger */
		printf ("Queue %s created.\n", quename);
		/*
		 *  Add any queue to device mappings as
		 *  specified.
		 */
		adsdev_set (quename, ADD_OP);	/* Add the queue/device */
						/* mappings as appropriate */
		fsizedb (Qmapfile);		/* Mappings file may have */
						/* expanded */
	}
	else diagnose (comcode);		/* Diagnose error */
}


/*** v_crepipque
 *
 *
 *	void v_crepipque():
 *	Create Pipe_queue command.
 */
void v_crepipque()
{
	static char *qualifier_set[] = {
		"Destination",
		"PIpeonly",
		"PRiority",
		"Run_limit",
		"Server",
		NULL
	};

	long comcode;			/* Completion code */
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	long long_int;			/* Used for integer scanning */
	int priority;			/* Queue priority */
	int runlimit;			/* Queue run-limit */
	short pipeonly;			/* BOOLEAN Pipeonly attr */
	char srvname [MAX_SERVERNAME+1];/* Server for queue */
	register char *cp;		/* Character pointer */
	register int token;		/* Scan token */
	/*
	 *  Boolean variables:
	 */
	short de_seen;			/* Destination seen */
	short pr_seen;			/* PRiority seen */
	short rl_seen;			/* Run_limit seen */
	short se_seen;			/* Server seen */

	/*
	 *  No attributes seen yet.
	 */
	de_seen = 0;
	pr_seen = 0;
	rl_seen = 0;
	se_seen = 0;
	pipeonly = 0;
	runlimit = 1;			/* Default run limit */
	/*
	 *  Get the name of the queue to be created.
	 */
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	/*
	 *  Get the queue attributes.
	 */
	while ((token = scan_qualifier (qualifier_set)) > 0) {
		/*
		 *  A qualifier keyword has been recognized.
		 */
		switch (token) {
		case 1:			/* Destination set */
			if (de_seen) {
				errormessage (EM_MULDESSET);
				return;
			}
			de_seen = 1;	/* Set the seen flag */
			if (scan_equals() == -1) return;
			/*
			 *  Scan the set of specified destinations.
			 */
			if (scan_destset() == -1) return;
			break;
		case 2:			/* Pipeonly */
			pipeonly = 1;	/* Queue has the pipeonly attr */
			break;
		case 3:			/* Priority */
			if (pr_seen) {
				errormessage (EM_MULPRI);
				return;
			}
			pr_seen = 1;	/* Set the seen flag */
			if (scan_equals() == -1) return;
			if (scan_int (&long_int, 0L,
				     (long) MAX_QPRIORITY,
				      EM_INTPRIEXP,
				      EM_PRIOUTOFBOU) == -1) return;
			/*
			 *  Successful priority value scan.
			 */
			priority = long_int;
			break;
		case 4:			/* Run_limit */
			if (rl_seen) {
				errormessage (EM_MULRUNLIM);
				return;
			}
			rl_seen = 1;	/* Set the seen flag */
			if (scan_equals() == -1) return;
			if (scan_int (&long_int, 1L, (long) MAX_QRUNLIMIT,
				      EM_INTRUNLIMEXP,
				      EM_RUNLIMOUTOFBOU) == -1) {
				return;
			}
			/*
			 *  Good run-limit value.
			 */
			runlimit = long_int;
			break;
		case 5:			/* Server */
			if (se_seen) {
				errormessage (EM_MULSER);
				return;
			}
			se_seen = 1;	/* Set the seen flag */
			if (scan_equals() == -1) return;
			if ((cp = scan_allinparens()) == NULL) return;
			/*
			 *  A good server specification has been
			 *  scanned.
			 */
			strcpy (srvname, cp);	/* Save server spec */
			break;
		}
	}
	if (token != 0) return;			/* An error occurred */
	/*
	 *  The end of the command has been reached.
	 */
	if (!pr_seen) {
		errormessage (EM_NOPRISPE);	/* No priority specified */
		return;
	}
	if (!se_seen) {
		errormessage (EM_NOSERSPE);	/* No server specified */
		return;
	}
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	/*
	 *  Create the new queue.
	 */
	if ((comcode = crepipque (quename, priority, runlimit,
				  pipeonly, srvname)) == TCML_COMPLETE) {
		/*
		 *  We were successful in our efforts to create the
		 *  pipe queue.  Now add any destinations as required.
		 */
		fsizedb (Queuefile);		/* Queue file size may have */
						/* gotten bigger */
		printf ("Queue %s created.\n", quename);
		adsdes_set (quename, ADD_OP);	/* Add the destinations */
		fsizedb (Qmapfile);		/* Mappings file may have */
						/* expanded */
		fsizedb (Pipeqfile);		/* Pipe queue destination */
						/* file may have expanded */
	}
	else diagnose (comcode);		/* Diagnose error */
}


/*** v_delcom
 *
 *
 *	void v_delcom():
 *	Delete Complex command.
 */
void v_delcom()
{
	char qcom_name [MAX_QCOMPLXNAME+1];	/* Queue complex name */
	char *cp;			/* Pointer to queue name */

	if ((cp = scan_qcomname()) == NULL) return;
	strcpy (qcom_name, cp);		/* Save queue complex name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose(delcom(qcom_name));	/* Delete queue complex */
	fsizedb (Qmapfile);		/* Mappings file may have contracted */
}


/*** v_deldes
 *
 *
 *	void v_deldes():
 *	DElete DEStination command.
 */
void v_deldes()
{
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	/*
	 *  Scan the set of specified destinations.
	 */
	if (scan_destset() == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsdes_set (quename, DEL_OP);	/* Delete the destinations */
}


/*** v_deldev
 *
 *
 *	void v_deldev():
 *	DElete DEVice command.
 */
void v_deldev()
{
	char devname [MAX_DEVNAME+1];	/* Device name */
	char *cp;			/* Pointer to device name, '=', */
					/* or queue name */
	char quename [MAX_QUEUENAME+1];	/* Queue name */

	if ((cp = scan_dename()) == NULL) return;
	if (*cp != '=') {
		/*
		 *  Delete a device.
		 */
		strcpy (devname, cp);	/* Squirrel away the device name */
		if (scan_end() == -1) return;
		if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
			diagnose (TCML_INSUFFPRV);
			return;		/* Manager privileges required */
		}
		diagnose (deldev (devname));
	}
	else {
		/*
		 *  Delete queue/device mappings.
		 *  Scan the set of specified devices to delete
		 *  from the mappings for the named queue at the
		 *  end of the command.
		 *
		 *  Scan the set of specified devices.
		 */
		if (scan_devset() == -1) return;
		if ((cp = scan_qname()) == NULL) return;
		strcpy (quename, cp);		/* Save queue name */
		if (scan_end() == -1) return;
		if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
			diagnose (TCML_INSUFFPRV);
			return;		/* Manager privileges required */
		}
		adsdev_set (quename, DEL_OP);	/* Delete the queue/device */
	}					/* mappings */
}


/*** v_delfor
 *
 *
 *	void v_delfor():
 *	Delete Form command.
 */
void v_delfor()
{
	if (scan_forset() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsfor_set (DEL_OP);		/* Delete the forms from the */
}					/* forms set */


/*** v_delgid
 *
 *
 *	void v_delgid():
 *	DElete Groups command.
 */
void v_delgid()
{
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	/*
	 *  Scan the set of specified groups.
	 */
	if (scan_groupset() == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsgid_set (quename, DEL_OP);	/* Delete the access by group */
}


/*** v_delman
 *
 *
 *	void v_delman():
 *	DElete Manager command.
 */
void v_delman()
{
	if (scan_mgrset() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsmgr_set (DEL_OP);		/* Delete accounts from the mgr set */
}


/*** v_delque
 *
 *
 *	void v_delque():
 *	DElete Queue command.
 */
void v_delque()
{
	char quename [MAX_QUEUENAME+1];	/* Name of queue to be deleted */
	char *cp;			/* Pointer to queue name */

	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (delque (quename));
}


/*** v_delreq
 *
 *
 *	void v_delreq():
 *	DElete Request command.
 */
void v_delreq()
{
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	printf ("DElete Request command is not yet supported through the.\n");
	printf ("Qmgr program.\n");
}


/*** v_deluid
 *
 *
 *	void v_deluid():
 *	DElete Users command.
 */
void v_deluid()
{
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	/*
	 *  Scan the set of specified users.
	 */
	if (scan_userset() == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsuid_set (quename, DEL_OP);	/* Delete the access by user */
}


/*** v_disdev
 *
 *
 *	void v_disdev():
 *	DIsable Device command.
 */
void v_disdev()
{
	char devname [MAX_DEVNAME+1];	/* Name of device to be disabled */
	char *cp;			/* Pointer to device name */

	if ((cp = scan_dname()) == NULL) return;
	strcpy (devname, cp);		/* Save device name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (disdev (devname));
}


/*** v_disque
 *
 *
 *	void v_disque():
 *	DIsable Queue command.
 */
void v_disque()
{
	char quename [MAX_QUEUENAME+1];	/* Name of queue to be disabled */
	char *cp;			/* Pointer to queue name */

	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (disque (quename));
}


/*** v_enadev
 *
 *
 *	void v_enadev():
 *	Enable Device command.
 */
void v_enadev()
{
	char devname [MAX_DEVNAME+1];	/* Name of device to be enabled */
	char *cp;			/* Pointer to device name */

	if ((cp = scan_dname()) == NULL) return;
	strcpy (devname, cp);		/* Save device name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (enadev (devname));
}


/*** v_enaque
 *
 *
 *	void v_enaque():
 *	Enable Queue command.
 */
void v_enaque()
{
	char quename [MAX_QUEUENAME+1];	/* Name of queue to be enabled */
	char *cp;			/* Pointer to queue name */

	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (enaque (quename));
}


/*** v_exi
 *
 *
 *	void v_exi():
 *	Exit command.
 */
void v_exi()
{
	exiting();			/* Relinquish connection to the */
	fflush (stdout);		/* local daemon, flush all */
	fflush (stderr);		/* output buffers, and exit. */
	exit (0);
}


/*** v_locdae
 *
 *
 *	void v_locdae():
 *	Lock command.
 *
 */
void v_locdae()
{
	if (scan_end () == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (locdae ());
}


/*** v_modreq
 *
 *
 *	void v_modreq();
 *	Modify Request command.
 */
void v_modreq()
{
	long req_seqno;				/* Request sequence number */
	mid_t orig_mid;				/* Originating machine ident */

	/*
	 *	Get the request identifier
	 */
	if (scan_reqidname (&req_seqno, &orig_mid) == -1) {
		errormessage (EM_REQIDEXP);
 		return;
		}
	/*
	 *	Scan the set of modifications
	 */
	if (scan_reqmodset() == -1) return;
	
        if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
                diagnose (TCML_INSUFFPRV);
                return;                 /* Operator privileges required */
        }

	modreq_set (req_seqno, (mid_t) orig_mid, (uid_t) 0); /* Mod requests */
}


/*** v_movque
 *
 *
 *	void v_movque();
 *	MOVe Queue command.
 */
void v_movque()
{
	char src_qname [MAX_QUEUENAME+1];	/* Name of source queue */
	char des_qname [MAX_QUEUENAME+1];	/* Name of destn queue */
	char *cp;			/* Pointer to queue name */

	if ((cp = scan_qname()) == NULL) return;
	strcpy (src_qname, cp);		/* Save source queue name */
	if ((cp = scan_qname()) == NULL) return;
	strcpy (des_qname, cp);		/* Save destin queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose(movque(src_qname,des_qname));
}


/*** v_movreq
 *
 *
 *	void v_movreq();
 *	MOVe Request command.
 */
void v_movreq()
{
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
#ifdef SDSC
        /*
         *  Scan the parameter set
         */

        if (scan_equals() == -1) {
           return;
        }
        /*
         *      Scan the set of request identifiers
         */

        if (scan_reqset() == -1) {
           return;
        }
        if ((cp = scan_qname()) == NULL) {
           return;
        }
        strcpy (quename, cp);           /* ** Save queue name ** */
        if (scan_end() == -1) {
           return;
        }
        if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
           diagnose (TCML_INSUFFPRV);
           return; /* ** Operator privileges required ** */
        }
        movreq_set (quename);   /* ** Move the requests to the queue ** */
#else
	printf ("Move Request command is not yet supported through the.\n");
	printf ("Qmgr program.\n");
	return;
#endif
}

 
/*** v_purque
 *
 *
 *	void v_purque():
 *	Purge Queue command.
 */
void v_purque()
{
	char quename [MAX_QUEUENAME+1];	/* Name of queue to be purged */
	char *cp;			/* Pointer to queue name */

	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (purque (quename));
}

 
/*** v_remque
 *
 *
 *	void v_remque():
 *	Remove Queue from complex command.
 */
void v_remque()
{
	char qcom_name [MAX_QCOMPLXNAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	/*
	 *  Scan the set of specified queues.
	 */
	if (scan_queset() == -1) return;
	if ((cp = scan_qcomname()) == NULL) return;
	strcpy (qcom_name, cp);		/* Save queue complex name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsque_set (qcom_name, DEL_OP);	/* Delete the queues */
}


/*** v_setcomrunlim
 *
 *
 *	void v_setcomrunlim();
 *	SEt COMplex Run_limit command.
 */
void v_setcomrunlim()
{
	long run_limit;			/* Run-limit being set */
	char qcom_name [MAX_QCOMPLXNAME+1];	/* Queue complex name */
	char *cp;			/* Pointer to queue complex name */

	if (scan_equals() == -1) return;
	if (scan_int (&run_limit, 1L, (long) MAX_GBLBATLIMIT,
		      EM_INTRUNLIMEXP, EM_RUNLIMOUTOFBOU) == -1) return;
	if ((cp = scan_qcomname()) == NULL) return;
	strcpy (qcom_name, cp);
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (setcomrun (qcom_name, (int) run_limit));
}


/*** v_setdeb
 *
 *
 *	void v_setdeb():
 *	SEt DEBug command.
 */
void v_setdeb()
{
	long debug;			/* Debug level being set */

	if (scan_int (&debug, 0L, 3L, EM_INTDEBVALEXP,
		      EM_DEBVALOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setdeblev ((int) debug));
}


/*** v_setdefbatque
 *
 *
 *	void v_setdefbatque():
 *	SEt DEFault Batch_request Queue command.
 */
void v_setdefbatque()
{
	char quename [MAX_QUEUENAME+1];	/* Name of default queue */
	char *cp;			/* Pointer to queue name */

	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setbatque (quename));
}


/*** v_setdefbatpri
 *
 *
 *	void v_setdefbatpri():
 *	SEt DEFault Batch_request Priority command.
 */
void v_setdefbatpri()
{
	long default_pri;		/* Default priority being set */

	if (scan_int (&default_pri, 0L, (long) MAX_RPRIORITY,
		      EM_INTPRIEXP, EM_PRIOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setbatpri ((int) default_pri));
}


/*** v_setdefdestim
 *
 *
 *	void v_setdefdestim():
 *	SEt DEFault DEStination_retry Time command.
 */
void v_setdefdestim()
{
	long retry_hours;		/* Destination retry time in hours */

	if (scan_int (&retry_hours, 0L, 10000L,
		      EM_INTDESTIMEXP, EM_DESTIMOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setdestim (retry_hours * 3600));
}


/*** v_setdefdeswai
 *
 *
 *	void v_setdefdeswai():
 *	SEt DEFault DEStination_retry Wait command.
 */
void v_setdefdeswai()
{
	long dest_wait;			/* Default destination retry wait */
					/* time in minutes */

	if (scan_int (&dest_wait, 1L, 100000L,
		      EM_INTDESWAIEXP, EM_DESWAIOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setdeswai ((dest_wait * 60L)));
}


/*** v_setdefdevpri
 *
 *
 *	void v_setdefdevpri():
 *	SEt DEFault DEVice_request Priority command.
 */
void v_setdefdevpri()
{
	long default_pri;		/* Default priority being set */

	if (scan_int (&default_pri, 0L, (long) MAX_RPRIORITY,
		      EM_INTPRIEXP, EM_PRIOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setdevpri ((int) default_pri));
}


/*** v_setdefprifor
 *
 *
 *	void v_setdefprifor():
 *	SEt DEFault Print_request Forms command.
 */
void v_setdefprifor()
{
	char form [MAX_FORMNAME+1];	/* Form name */
	char *cp;			/* Pointer to form name */

	if ((cp = scan_fname()) == NULL) return;
	strcpy (form, cp);		/* Save forms name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setprifor (form));
}


/*** v_setdefprique
 *
 *
 *	void v_setdefprique():
 *	SEt DEFault Print_request Queue command.
 */
void v_setdefprique()
{
	char quename [MAX_QUEUENAME+1];	/* Name of default queue */
	char *cp;			/* Pointer to queue name */

	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setprique (quename));
}


/*** v_setdes
 *
 *
 *	void v_setdes():
 *	SEt DEStination command.
 */
void v_setdes()
{
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	/*
	 *  Scan the set of specified destinations.
	 */
	if (scan_destset() == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsdes_set (quename, SET_OP);	/* Set the destinations */
	fsizedb (Qmapfile);		/* Mappings file may have expanded */
	fsizedb (Pipeqfile);		/* Pipe queue destination file */
					/* may have expanded */
}


/*** v_setdev
 *
 *
 *	void v_setdev():
 *	SEt DEVICE command.
 */
void v_setdev()
{
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	/*
	 *  Scan the set of specified devices.
	 */
	if (scan_devset() == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsdev_set (quename, SET_OP);	/* Set the queue/device mappings */
	fsizedb (Qmapfile);		/* Mappings file may have expanded */
}


/*** v_setdevser
 *
 *
 *	void v_setdevser():
 *	SEt DEVICE_server command.
 */
void v_setdevser()
{
	char devname [MAX_DEVNAME+1];	/* Device name */
	char srvname [MAX_SERVERNAME+1];/* Server for device */
	char *cp;			/* Pointer to name */

	if (scan_equals() == -1) return;
	if ((cp = scan_allinparens()) == NULL) return;
	strcpy (srvname, cp);		/* Save server spec */
	if ((cp = scan_dname()) == NULL) return;
	strcpy (devname, cp);		/* Save device name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setdevser (devname, srvname));
}


/*** v_setfor
 *
 *
 *	void v_setfor():
 *	SEt Forms command.
 */
void v_setfor()
{
	char formname [MAX_FORMNAME+1];
	char devname [MAX_DEVNAME+1];
	char *cp;

	switch(scan_forset()) {
	
	case -2:	/* it was "set forms = <forms-name> <device-name>" */
			/* not "set forms <forms-name> ..." */
		
		if ((cp = scan_fname()) == NULL) return;
		strcpy (formname, cp);
		if ((cp = scan_dname()) == NULL) return;
		strcpy (devname, cp);
		if (scan_end() == -1) return;
		if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
			diagnose (TCML_INSUFFPRV);
			return;		/* operator privileges required.*/
		}
		diagnose (setdevfor (devname, formname));
		break;
	
	case -1:			/* parse error */
		return;
	
	case 0:		/* it was "set forms <forms-name> ..." */

		if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
			diagnose (TCML_INSUFFPRV);
			return;		/* Manager privileges required */
		}
		adsfor_set (SET_OP);	/* Set the forms to the forms set */
		fsizedb (Formsfile);	/* Forms file may have expanded */
		
	default:
		return;
	}
}


/*** v_setgblbatlim
 *
 *
 *	void v_setgblbatlim();
 *	SEt Global_batch_limit command.
 */
void v_setgblbatlim()
{
	long run_limit;			/* Run-limit being set */

	if (scan_equals() == -1) return;
	if (scan_int (&run_limit, 0L, (long) MAX_GBLBATLIMIT,
		      EM_INTRUNLIMEXP, EM_RUNLIMOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (setgblbat ( (int) run_limit));
}


#ifdef SDSC
/*** v_setsoftulim
 *
 *
 *	void v_setsoftulim();
 *	SEt the soft user_limit command.
 */
void v_setsoftulim()
{
	long user_limit;			/* user limit being set */

	if (scan_equals() == -1) return;
	if (scan_int (&user_limit, 0L, (long) MAX_SOFTULIMIT,
		      EM_INTULIMEXP, EM_USERLIMOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (setsoftulim ( (int) user_limit));
}

/*** v_sethardulim
 *
 *
 *	void v_sethardulim();
 *	SEt the hard user_limit command.
 */
void v_sethardulim()
{
	long user_limit;			/* user limit being set */

	if (scan_equals() == -1) return;
	if (scan_int (&user_limit, 0L, (long) MAX_HARDULIMIT,
		      EM_INTULIMEXP, EM_USERLIMOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (sethardulim ( (int) user_limit));
}

#endif

/*** v_setliftim
 *
 *
 *	void v_setliftim():
 *	SEt LIfetime command.
 */
void v_setliftim()
{
	long lifetime;			/* Pipe queue req lifetime in hours */

	if (scan_int (&lifetime, 1L, 100000L,
		      EM_INTLIFTIMEXP, EM_LIFTIMOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setlife ((lifetime * 3600L)));
}


/*** v_setlogfil
 *
 *
 *	void v_setlogfil():
 *	SEt LOg_file command.
 */
void v_setlogfil()
{
	char pathname [MAX_PATHNAME+1];	/* Path name of logfile */
	char *cp;			/* Pointer to pathname name */

	if ((cp = scan_logname()) == NULL) return;
	strcpy (pathname, cp);		/* Save pathname */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setlogfil (pathname));
}


/*** v_setmai
 *
 *
 *	void v_setmai():
 *	SEt MAIl command.
 */
void v_setmai()
{
	char acct [MAX_ACCOUNTNAME+1];	/* Account name */
	char *cp;			/* Pointer to queue name */
	register struct passwd *passwd;	/* Password file entry */

	if ((cp = scan_aname()) == NULL) return;
	strcpy (acct, cp);		/* Save account name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	if ((passwd = fetchpwnam (acct)) == NULL) diagnose (TCML_NOSUCHACC);
	else {
		closepwdb();		/* Close account/password database */
		diagnose (setnqsmai ((uid_t) passwd->pw_uid));
	}
}


/*** v_setman
 *
 *
 *	void v_setman():
 *	SEt MANager command.
 */
void v_setman()
{
	if (scan_mgrset() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	adsmgr_set (SET_OP);		/* Set the accounts to the mgr set */
	fsizedb (Mgrfile);		/* Manager file may have expanded */
}


/*** v_setmaxcop
 *
 *
 *	void v_setmaxcop():
 *	SEt MAXimum Copies command.
 */
void v_setmaxcop()
{
	long max_copies;		/* Maximum copies on device req. */

	if (scan_int (&max_copies, 1L, 1000L,
		      EM_INTMAXCOPEXP, EM_MAXCOPOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setmaxcop (max_copies));
}


/*** v_setmaxoperet
 *
 *
 *	void v_setmaxoperet():
 *	SEt MAXimum Open_retries command.
 */
void v_setmaxoperet()
{
	long retry_limit;		/* Failed device open retry limit */

	if (scan_int (&retry_limit, 0L, 1000000L,
		      EM_INTMAXOPERETEXP, EM_MAXOPERETOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setmaxopr (retry_limit));
}


/*** v_setmaxprisiz
 *
 *
 *	void v_setmaxprisiz():
 *	SEt MAXimum Print_size command.
 */
void v_setmaxprisiz()
{
	long max_printsize;		/* Maximum print size */

	if (scan_int (&max_printsize, 1L, 100000000L,
		      EM_INTMAXPRISIZEXP, EM_MAXPRISIZOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setmaxpsz (max_printsize));
}


/*** v_setndfbatque
 *
 *
 *	void v_setndfbatque():
 *	SEt NO_Default Batch_request Queue command.
 */
void v_setndfbatque()
{
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setndfbat());
}


/*** v_setndfprifor
 *
 *
 *	void v_setndfprifor():
 *	SEt NO_Default Print_request Forms command.
 */
void v_setndfprifor()
{
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setndffor());
}


/*** v_setndfprique
 *
 *
 *	void v_setndfprique():
 *	SEt NO_Default Print_request Queue command.
 */
void v_setndfprique()
{
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setndfpri());
}


/*** v_setnetcli
 *
 *
 *	void v_setnetcli();
 * 	SEt NEtwork Client command.
 */
void v_setnetcli()
{
	char srvname[MAX_SERVERNAME+1];
	char *cp;

	if (scan_equals() == -1) return;
	if ((cp = scan_allinparens()) == NULL) return;
	strcpy (srvname, cp);
	if (scan_end() == -1) return;
	if (( Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setnetcli (srvname));
}


/*** v_setnetdae
 *
 *
 *	void v_setnetdae();
 * 	SEt NEtwork Daemon command.
 */
void v_setnetdae()
{
	char srvname[MAX_SERVERNAME+1];
	char *cp;

	if (scan_equals() == -1) return;
	if ((cp = scan_allinparens()) == NULL) return;
	strcpy (srvname, cp);
	if (scan_end() == -1) return;
	if (( Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setnetdae (srvname));
}


/*** v_setnetser
 *
 *
 *	void v_setnetser();
 * 	SEt NEtwork Server command.
 */
void v_setnetser()
{
	char srvname[MAX_SERVERNAME+1];
	char *cp;

	if (scan_equals() == -1) return;
	if ((cp = scan_allinparens()) == NULL) return;
	strcpy (srvname, cp);
	if (scan_end() == -1) return;
	if (( Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setnetser (srvname));
}


/*** v_setnnedae
 *
 *
 *	void v_setnnedae();
 *	SEt NO_network_daemon command.
 */
void v_setnnedae()
{
	if (scan_end () == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setnnedae ());
}


/*** v_setnoacc
 *
 *
 *	void v_setnoacc():
 *	SEt NO_Access command.
 */
void v_setnoacc()
{
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */
	
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end () == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;
	}
	diagnose (setnoqueacc (quename));
}


/*** v_setopewai
 *
 *
 *	void v_setopewai():
 *	SEt Open_wait command.
 */
void v_setopewai()
{
	long retry_wait;		/* Time to wait between failed */
					/* device open retry attempts */

	if (scan_int (&retry_wait, 1L, 1000L,
		      EM_INTOPEWAIEXP, EM_OPEWAIOUTOFBOU) == -1) return;
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setopewai (retry_wait));
}

/*** v_setpipcli
 *
 *
 *	void v_setpipcli():
 *	SEt PIpe_client command.
 */
void v_setpipcli()
{
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char srvname [MAX_SERVERNAME+1];/* The pipe client */
	char *cp;			/* Pointer to name */

	if (scan_equals() == -1) return;
	if ((cp = scan_allinparens()) == NULL) return;
	strcpy (srvname, cp);		/* Save client specification */
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setpipcli (quename, srvname));
}


/*** v_setppcore
 *
 *
 *	void v_setppcore():
 *	SEt Corefile_limit command.
 */
void v_setppcore()
{
	struct quotalimit quotalimit;
	char quename [MAX_QUEUENAME + 1];
	short infinite;

	if (quotapreamble (quename, &quotalimit, &infinite) != -1) {
		diagnose (setppcore (quename, quotalimit.max_quota,
				     quotalimit.max_units, infinite));
	}
}


#ifdef SDSC
/*** v_setQueuedLimit
 *
 *
 *      void v_setQueuedLimit():
 *      SEt Queue_request_limit command.
 */
void v_setQueuedLimit()
{
        struct quotalimit quotalimit;
        char quename [MAX_QUEUENAME + 1];
        short infinite;
 
        if (quotapreamble (quename, &quotalimit, &infinite) != -1) {
                diagnose (setquequed (quename, quotalimit.max_quota,
                                     quotalimit.max_units, infinite));
        }
}
#endif

 
/*** v_setppcput
 *
 *
 *	void v_setppcput():
 *	SEt PER_Process Cpu_limit command.
 */
void v_setppcput()
{
	struct cpulimit cpulimit;
	char quename [MAX_QUEUENAME + 1];
	short infinite;

	if (cpupreamble (quename, &cpulimit, &infinite) != -1) {
		diagnose (setppcput (quename, cpulimit.max_seconds,
				    cpulimit.max_ms, infinite));
	}
}


/*** v_setppdata
 *
 *
 *	void v_setppdata():
 *	SEt DAta_limit command.
 */
void v_setppdata()
{
	struct quotalimit quotalimit;
	char quename [MAX_QUEUENAME + 1];
	short infinite;

	if (quotapreamble (quename, &quotalimit, &infinite) != -1) {
		diagnose (setppdata (quename, quotalimit.max_quota,
				     quotalimit.max_units, infinite));
	}
}


/*** v_setppmem
 *
 *
 *	void v_setppmem():
 *	SEt PER_Process Memory_limit command.
 */
void v_setppmem()
{
	struct quotalimit quotalimit;
	char quename [MAX_QUEUENAME + 1];
	short infinite;

	if (quotapreamble (quename, &quotalimit, &infinite) != -1) {
		diagnose (setppmem (quename, quotalimit.max_quota,
				    quotalimit.max_units, infinite));
	}
}


/*** v_setppnice
 *
 *
 *	void v_setppnice():
 *	SEt NIce_value_limit command.
 */
void v_setppnice()
{
	long nice_value;
	char quename [MAX_QUEUENAME + 1];
	register char *cp;

	if (scan_equals() == -1) return;
	if (scan_int (&nice_value, (long) MIN_REQNICE, (long) MAX_REQNICE,
		EM_INTNICEEXP, EM_NICEOUTOFBOU) == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setppnice (quename, (int) nice_value));
}

#ifdef SDSC
/*** v_setnodegrp
 *
 *
 *	void v_setnodegrp():
 *	SEt NODE_Group command.
 */
void v_setnodegrp()
{
	long nodegrp_value;
	char quename [MAX_QUEUENAME + 1];
	register char *cp;

	if (scan_equals() == -1) return;
	if (scan_int (&nodegrp_value, (long) MIN_NODEGRP, (long) MAX_NODEGRP,
		EM_INTNODEGRPEXP, EM_NODEGRPOUTOFBOU) == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setnodegrp (quename, (int) nodegrp_value));
}
#endif


/*** v_setpppfile
 *
 *
 *	void v_setpppfile():
 *	SEt PER_Process Permfile_limit command.
 */
void v_setpppfile()
{
	struct quotalimit quotalimit;
	char quename [MAX_QUEUENAME + 1];
	short infinite;

	if (quotapreamble (quename, &quotalimit, &infinite) != -1) {
		diagnose (setpppfile (quename, quotalimit.max_quota,
				      quotalimit.max_units, infinite));
	}
}


/*** v_setppstack
 *
 *
 *	void v_setppstack():
 *	SEt STack_limit command.
 */
void v_setppstack()
{
	struct quotalimit quotalimit;
	char quename [MAX_QUEUENAME + 1];
	short infinite;

	if (quotapreamble (quename, &quotalimit, &infinite) != -1) {
		diagnose (setppstack (quename, quotalimit.max_quota,
				      quotalimit.max_units, infinite));
	}
}


/*** v_setpptfile
 *
 *
 *	void v_setpptfile():
 *	SEt PER_Process Tempfile_limit command.
 */
void v_setpptfile()
{
	struct quotalimit quotalimit;
	char quename [MAX_QUEUENAME + 1];
	short infinite;

	if (quotapreamble (quename, &quotalimit, &infinite) != -1) {
		diagnose (setpptfile (quename, quotalimit.max_quota,
				      quotalimit.max_units, infinite));
	}
}


/*** v_setppwork
 *
 *
 *	void v_setppwork():
 *	SEt Working_set_limit command.
 */
void v_setppwork()
{
	struct quotalimit quotalimit;
	char quename [MAX_QUEUENAME + 1];
	short infinite;

	if (quotapreamble (quename, &quotalimit, &infinite) != -1) {
		diagnose (setppwork (quename, quotalimit.max_quota,
				     quotalimit.max_units, infinite));
	}
}


/*** v_setprcput
 *
 *
 *	void v_setprcput():
 *	SEt PER_Request Cpu_limit command.
 */
void v_setprcput()
{
	struct cpulimit cpulimit;
	char quename [MAX_QUEUENAME + 1];
	short infinite;

	if (cpupreamble (quename, &cpulimit, &infinite) != -1) {
		diagnose (setprcput (quename, cpulimit.max_seconds,
				    cpulimit.max_ms, infinite));
	}
}


/*** v_setpri
 *
 *
 *	void v_setpri():
 *	SEt PRiority command.
 */
void v_setpri()
{
	long priority;			/* Priority being set */
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	if (scan_int (&priority, 0L, (long) MAX_QPRIORITY,
		      EM_INTPRIEXP, EM_PRIOUTOFBOU) == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setquepri (quename, (int) priority));
}


/*** v_setprmem
 *
 *
 *	void v_setprmem():
 *	SEt PER_Request Memory_limit command.
 */
void v_setprmem()
{
	struct quotalimit quotalimit;
	char quename [MAX_QUEUENAME + 1];
	short infinite;

	if (quotapreamble (quename, &quotalimit, &infinite) != -1) {
		diagnose (setprmem (quename, quotalimit.max_quota,
				    quotalimit.max_units, infinite));
	}
}

#ifdef SDSC
/*** v_setprncpus
 *
 *
 *	void v_setprncpus ():    ( was [void v_setppnice():] in NQS v.2.0)
 *	SEt PER_Request Ncpus_limit command.
 */
void v_setprncpus ()
{
	long ncpus_value;
	char quename [MAX_QUEUENAME + 1];
	register char *cp;

	if (scan_equals() == -1) return;
	if (scan_int (&ncpus_value, (long) MIN_NCPUS, (long) MAX_NCPUS,
		EM_INTNCPUSEXP, EM_NCPUSOUTOFBOU) == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setprncpus (quename, (int) ncpus_value));
}
#endif


/*** v_setprpfile
 *
 *
 *	void v_setprpfile():
 *	SEt PER_Request Permfile_limit command.
 */
void v_setprpfile()
{
	struct quotalimit quotalimit;
	char quename [MAX_QUEUENAME + 1];
	short infinite;

	if (quotapreamble (quename, &quotalimit, &infinite) != -1) {
		diagnose (setprpfile (quename, quotalimit.max_quota,
				      quotalimit.max_units, infinite));
	}
}


/*** v_setprtfile
 *
 *
 *	void v_setprtfile():
 *	SEt PER_Request Tempfile_limit command.
 */
void v_setprtfile()
{
	struct quotalimit quotalimit;
	char quename [MAX_QUEUENAME + 1];
	short infinite;

	if (quotapreamble (quename, &quotalimit, &infinite) != -1) {
		diagnose (setprtfile (quename, quotalimit.max_quota,
				      quotalimit.max_units, infinite));
	}
}


/*** v_setrunlim
 *
 *
 *	void v_setrunlim():
 *	SEt Run_limit command.
 */
void v_setrunlim()
{
	long run_limit;			/* Run-limit being set */
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */

	if (scan_equals() == -1) return;
	if (scan_int (&run_limit, 1L, (long) MAX_QRUNLIMIT,
		      EM_INTRUNLIMEXP, EM_RUNLIMOUTOFBOU) == -1) return;
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (setquerun (quename, (int) run_limit));
}


/*** v_setshsfix
 *
 *
 *	void v_setshsfix():
 *	SEt SHell_strategy FIxed command.
 */
void v_setshsfix()
{
	char shname [MAX_SERVERNAME+1];
	char *cp;

	if (scan_equals() == -1) return;
	if ((cp = scan_allinparens()) == NULL) return;
	strcpy (shname, cp);
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Manager privileges required */
	}
	diagnose (setshsfix (shname));
}


/*** v_setshsfre
 *
 *
 *	void v_setshsfre():
 *	SEt SHell_strategy FRee command.
 */
void v_setshsfre()
{
	if (scan_end () == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;
	}
	diagnose (setshsfre());
}


/*** v_setshslog
 *
 *
 *	void v_setshslog():
 *	SEt SHell_strategy Login command.
 */
void v_setshslog()
{
	if (scan_end () == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;
	}
	diagnose (setshslog());
}


/*** v_setunracc
 *
 *
 *	void v_setunracc():
 *	SEt UNrestricted_access command.
 */
void v_setunracc()
{
	char quename [MAX_QUEUENAME+1];	/* Queue name */
	char *cp;			/* Pointer to queue name */
	
	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);		/* Save queue name */
	if (scan_end () == -1) return;
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;
	}
	diagnose (setunrqueacc (quename));
}


/*** v_shutdown
 *
 *
 *	void v_shutdown():
 *	SHUtdown command.
 */
void v_shutdown()
{
	int wait_time;			/* Shutdown SIGKILL wait time */
	long long_int;

	if (scan_opwait (&long_int, (long) SHUTDOWN_WAIT, 0L, 600L,
			 EM_INTWAITIMEXP, EM_WAITIMOUTOFBOU) == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	wait_time = long_int;
	diagnose (nqsshutdn (wait_time));
}


/*** v_staque
 *
 *
 *	void v_staque():
 *	STArt Queue command.
 */
void v_staque()
{
	char quename [MAX_QUEUENAME+1];	/* Name of queue to be started */
	char *cp;			/* Pointer to queue name */

	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);				/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (staque (quename));
}


/*** v_stoque
 *
 *
 *	void v_stoque():
 *	STOp Queue command.
 */
void v_stoque()
{
	char quename [MAX_QUEUENAME+1];	/* Name of queue to be stopped */
	char *cp;			/* Pointer to queue name */

	if ((cp = scan_qname()) == NULL) return;
	strcpy (quename, cp);				/* Save queue name */
	if (scan_end() == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (stoque (quename));
}


/*** v_unldae
 *
 *
 *	void v_unldae():
 *	Unlock Local_daemon command.
 */
void v_unldae()
{
	if (scan_end () == -1) return;
	if ((Mgr_priv & QMGR_OPER_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return;			/* Operator privileges required */
	}
	diagnose (unldae ());
}

/*** cpupreamble
 *
 *
 *	int cpupreamble ():
 *
 *	Perform all of the necessary preamble work for a command that
 *	sets a CPU time quota limit.
 *
 *	Returns:
 *		 0: if scanning was successful, and privileges are ok;
 *		-1: otherwise.
 */
static int cpupreamble (quename, cpulimit, infinite)
char quename [MAX_QUEUENAME + 1];
struct cpulimit *cpulimit;
short *infinite;
{
	register char *cp;

	if (scan_equals() == -1) return (-1);
	if ((cp = scan_allinparens()) == (char *) 0) return (-1);
	*infinite = 0;
	if (*cp == 'u') {
		cpulimit->max_seconds = 0;		/* Give these fields */
		cpulimit->max_ms = 0;			/* values that will */
		cpulimit->warn_seconds = 0;		/* pass integrity */
		cpulimit->warn_ms = 0;			/* checks */
		*infinite = 1;
	}
	else {
		switch (scancpulim (cp, cpulimit, 1)) {
		case 0:
			break;
		case -1:
			errormessage (EM_INVCPULIMSYN);
			return (-1);
		case -2:
			errormessage (EM_BADCPULIMVAL);
			return (-1);
		case -3:
			errormessage (EM_CPULIMEXP);
			return (-1);
		}
	}
	if ((cp = scan_qname()) == (char *) 0) return (-1);
	strcpy (quename, cp);
	if (scan_end() == -1) return (-1);
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return (-1);		/* Manager privileges required */
	}
	return (0);			/* Scanning successful, and */
}					/* privileges are sufficient */


/*** quotapreamble
 *
 *
 *	int quotapreamble():
 *
 *	Perform all of the necessary preamble work for a command that
 *	sets a quota limit having nothing to do with "nice" or cpu limits.
 *
 *	Returns:
 *		 0: if scanning was successful, and privileges are ok;
 *		-1: otherwise.
 */
static int quotapreamble (quename, quotalimit, infinite)
char quename [MAX_QUEUENAME + 1];
struct quotalimit *quotalimit;
short *infinite;
{
	register char *cp;

	if (scan_equals() == -1) return (-1);
	if ((cp = scan_allinparens()) == (char *) 0) return (-1);
	*infinite = 0;
	if (*cp == 'u') {
		quotalimit->max_quota = 0;		/* Give these fields */
		quotalimit->max_units = QLM_BYTES;	/* values that will */
		quotalimit->warn_quota = 0;		/* pass integrity */
		quotalimit->warn_units = QLM_BYTES;	/* checks */
		*infinite = 1;
	}
	else {
		switch (scanquolim (cp, quotalimit, 1)) {
		case 0:
			break;
		case -1:
			errormessage (EM_INVQUOLIMSYN);
			return (-1);
		case -2:
			errormessage (EM_BADQUOLIMVAL);
			return (-1);
		case -3:
			errormessage (EM_QUOLIMEXP);
			return (-1);
		}
	}
	if ((cp = scan_qname()) == (char *) 0) return (-1);
	strcpy (quename, cp);
	if (scan_end() == -1) return (-1);
	if ((Mgr_priv & QMGR_MGR_PRIV) == 0) {
		diagnose (TCML_INSUFFPRV);
		return (-1);		/* Manager privileges required */
	}
	return (0);			/* Scanning successful, and */
}					/* privileges are sufficient */
