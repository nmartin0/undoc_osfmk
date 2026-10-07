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
 
/*++ mgr_main.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qmgr/mgr_main.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS manager program main module.
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.8 $ $Date: 1995/02/24 23:31:22 $ $State: Exp $)
 * $Log: mgr_main.c,v $
 * Revision 1.8  1995/02/24  23:31:22  kremenek
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
 * Revision 1.7  1994/11/19  01:35:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/11/02  01:07:51  mwan
 * 1.2 mods
 *
 * Revision 1.5  1993/07/13  18:22:30  mwan
 * T11 - fixed PTS 5012
 *
 * Revision 1.2  1992/10/09  22:29:58  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:28:28  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:28  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:02:43  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)mgr_main.c	1.3 (mgr_main.c OSF/1 NQS2.0 GJK) 7/7/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#define	DEFAULT_HELPLINES 23	/* Default number of help page lines to */
				/* display at a time */
#include <stdio.h>
#include "nqs.h"
#if	UNICOS
#include <fcntl.h>
#include <sys/param.h>
#include <sys/jtab.h>
#else
#if	SGI | SYS52 | UTS | OSF
#include <fcntl.h>
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#include "nqsmgr.h"		/* Token types */
#include "nqsmgrhelp.h"		/* Offsets in help file */
#include "signal.h"		/* Signal handling */
#ifndef	QMGR_HELPFILE		/* Define the Qmgr help file path */
#define	QMGR_HELPFILE	"/usr/lib/nqs/qmgr.hlp"
#endif
#include "nqsvars.h"		/* NQS global vars */

extern void bufstdout();	/* Block buffer stdout */
extern void exiting();		/* Called on exit */
extern char *get_literal();	/* Get literal string token value */
extern uid_t getuid();		/* Get real user-id */
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
#else
#if	BSD42 | BSD43
extern int interconn();		/* Make up for the lack of named pipes */
#else
BAD SYSTEM TYPE
#endif
#endif
extern int isatty();		/* Return 1 if fd is a tty */
extern int localmid();		/* Get local machine-id */
extern int matchkeyword();	/* Try to match a keyword */
extern void new_command();	/* Drop current command and get a new one */
extern int nqspriv();		/* Get NQS privileges */
extern struct confd *opendb();	/* Open database file */
extern int scan();		/* Scan a command line token */
extern void scan_error();	/* Display command error */
extern int strcmp();		/* String comparison */
extern void v_aboque();		/* ABort Queue */
extern void v_adddes();		/* ADd DEStination */
extern void v_adddev();		/* ADd DEVice */
extern void v_addfor();		/* ADd Form */
extern void v_addgid();		/* ADd Groups */
extern void v_addman();		/* ADd Manager */
extern void v_addque();		/* ADd Queues to complex */
extern void v_adduid();		/* ADd Users */
extern void v_crebatque();	/* Create Batch_queue */
extern void v_crecom();		/* Create Complex */
extern void v_credev();		/* Create DEVICE */
extern void v_credevque();	/* Create DEVICE_queue */
extern void v_crepipque();	/* Create Pipe_queue */
extern void v_delcom();		/* DElete Complex */
extern void v_deldes();		/* DElete DEStination */
extern void v_deldev();		/* DElete DEVice */
extern void v_delfor();		/* DElete Form */
extern void v_delgid();		/* DElete Groups */
extern void v_delman();		/* DElete Manager */
extern void v_delque();		/* DElete Queue */
extern void v_delreq();		/* DElete Request */
extern void v_deluid();		/* DElete Users */
extern void v_disdev();		/* DIsable Device */
extern void v_disque();		/* DIsable Queue */
extern void v_enadev();		/* ENable Device */
extern void v_enaque();		/* ENable Queue */
extern void v_exi();		/* EXit */
extern void v_locdae();		/* Lock */
extern void v_modreq();		/* MODify Request */
extern void v_movque();		/* MOVe Queue */
extern void v_movreq();		/* MOVe Request */
extern void v_purque();		/* Purge Queue */
extern void v_remque();		/* Remove Queue from complex */
extern void v_setcomrunlim();	/* SEt COMplex Run_limit */
extern void v_setdeb();		/* SEt DEBug */
extern void v_setdefbatque();	/* SEt DEFault Batch_request Queue */
extern void v_setdefbatpri();	/* SEt DEFault Batch_request Priority */
extern void v_setdefdestim();	/* SEt DEFault DEStination_retry Time */
extern void v_setdefdeswai();	/* SEt DEFault DEStination_retry Wait */
extern void v_setdefdevpri();	/* SEt DEFault DEVice_request Priority */
extern void v_setdefprifor();	/* SEt DEFault Print_request Forms */
extern void v_setdefprique();	/* SEt DEFault Print_request Queue */
extern void v_setdes();		/* SEt DEStination */
extern void v_setdev();		/* SEt DEVICE */
extern void v_setdevser();	/* SEt DEVICE_server */
extern void v_setfor();		/* SEt Forms */
extern void v_setgblbatlim();	/* SEt Global batch_limit */
extern void v_setliftim();	/* SEt LIfetime */
extern void v_setlogfil();	/* SEt LOg_file */
extern void v_setmai();		/* SEt MAIl */
extern void v_setman();		/* SEt MANager */
extern void v_setmaxcop();	/* SEt MAXimum Copies */
extern void v_setmaxoperet();	/* SEt MAXimum Open_retries */
extern void v_setmaxprisiz();	/* SEt MAXimum Print_size */
extern void v_setndfbatque();	/* SEt NO_Default Batch_request Queue */
extern void v_setndfprifor();	/* SEt NO_Default Print_request Forms */
extern void v_setndfprique();	/* SEt NO_Default Print_request Queue */
extern void v_setnetcli();	/* SEt NEtwork Client */
extern void v_setnetdae();	/* SEt NEtwork Daemon */
extern void v_setnetser();	/* SEt NEtwork Server */
extern void v_setnnedae();	/* SEt NO_Network_daemon */
extern void v_setnoacc();	/* SEt NO_Access */
extern void v_setopewai();	/* SEt Open_wait */
extern void v_setpipcli();	/* SEt PIpe_client */
extern void v_setppcore();	/* SEt CORefile_limit */
extern void v_setppcput();	/* SEt PER_Process Cpu_limit */
extern void v_setppdata();	/* SEt DAta_limit */
extern void v_setppmem();	/* SEt PER_Process Memory_limit */
extern void v_setppnice();	/* SEt NIce_value_limit */
extern void v_setpppfile();	/* SEt PER_Process Permfile_limt */
extern void v_setppstack();	/* SEt STack_limit */
extern void v_setpptfile();	/* SEt PER_Process Tempfile_limit*/
extern void v_setppwork();	/* SEt Working_set_limit */
extern void v_setprcput();	/* SEt PER_Request Cpu_limit */
extern void v_setpri();		/* SEt PRiority */
extern void v_setprmem();	/* SEt PER_Request Memory_limit */
#ifdef SDSC
extern void v_setprncpus();	/* SEt PER_Request Ncpus */
extern void v_setnodegrp();	/* SEt node_group value */
extern void v_setsoftulim();	/* SEt soft user limit */
extern void v_sethardulim();	/* SEt hard user limit */
#endif
extern void v_setprpfile();	/* SEt PER_Request Permfile_limit */
extern void v_setprtfile();	/* SEt PER_Request Tempfile_limit */
extern void v_setrunlim();	/* SEt Run_limit */
#ifdef SDSC
extern void v_setQueuedLimit(); /* SEt Queue_request_limit */
#endif
extern void v_setshsfix();	/* SEt SHell_strategy FIxed */
extern void v_setshsfre();	/* SEt SHell_strategy FRee */
extern void v_setshslog();	/* SEt SHell_strategy Login */
extern void v_setunracc();	/* SEt UNrestricted_access */
extern void v_shoall();		/* SHOw All */
extern void v_shodev();		/* SHOw Device */
extern void v_shofor();		/* SHOw Forms */
extern void v_sholim();		/* SHOw LImits_supported */
extern void v_sholongque();	/* SHOw LOng Queue */
extern void v_shoman();		/* SHOw Managers */
extern void v_shopar();		/* SHOw Parameters */
extern void v_shoque();		/* SHOw Queue */
extern void v_shutdown();	/* SHUtdown */
extern void v_staque();		/* STArt Queue */
extern void v_stoque();		/* STOp Queue */
extern void v_unldae();		/* Unlock */

mid_t Local_mid;			/* Machine-id of local machine */
char *Nqsmgr_prefix = "NQS manager";	/* NQS manager message prefix */
int Mgr_priv;				/* NQS manager privilege bits */
int Help_lines;				/* Number of lines to display */
					/* at a time for help pages */
int scpflag = 0;			/* UNICOS SCP flag */
#if	UNICOS
struct	jtab	jtab[NJOB];	/* Job table */
#endif

/*
 *
 *	Command parsing tables and help file.
 */
static FILE *Helpfile;		/* NQS qmgr help file */
static struct cmd_struct {
	char *vmod;		/* Command verb or modifier */
	long help_offset;	/* Offset in help file of help info */
	void (*cf)();		/* Do command function */
} cset [] = {
	{ "ABort", HELP_ABOQUE, (void (*)()) 0 },
	{ "Queue", HELP_ABOQUE, v_aboque },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "ADd", HELP_ADD, (void (*)()) 0 },
	{ "DEStination", HELP_ADDDES, v_adddes },
	{ "DEVice", HELP_ADDDEV, v_adddev },
	{ "Forms", HELP_ADDFOR, v_addfor },
	{ "Groups", HELP_ADDGID, v_addgid },
	{ "Managers", HELP_ADDMAN, v_addman },
	{ "Queues", HELP_ADDQUE, v_addque },
	{ "Users", HELP_ADDUID, v_adduid },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "Create", HELP_CRE, (void (*)()) 0 },
	{ "Batch_queue", HELP_CREBATQUE, v_crebatque },
	{ "Complex", HELP_CRECOM, v_crecom },
	{ "DEVICE", HELP_CREDEV, v_credev },
	{ "DEVICE_queue", HELP_CREDEVQUE, v_credevque },
	{ "Pipe_queue", HELP_CREPIPQUE, v_crepipque },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "DElete", HELP_DEL, (void (*)()) 0 },
	{ "Complex", HELP_DELCOM, v_delcom },
	{ "DEStination", HELP_DELDES, v_deldes },
	{ "DEVice", HELP_DELDEV, v_deldev },
	{ "Forms", HELP_DELFOR, v_delfor },
	{ "Groups", HELP_DELGID, v_delgid },
	{ "Managers", HELP_DELMAN, v_delman },
	{ "Queue", HELP_DELQUE, v_delque },
#ifndef SDSC
	{ "Request", HELP_DELREQ, v_delreq },
#endif
	{ "Users", HELP_DELUID, v_deluid },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "DIsable", HELP_DIS, (void (*)()) 0 },
	{ "Device", HELP_DISDEV, v_disdev },
	{ "Queue", HELP_DISQUE, v_disque },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "ENable", HELP_ENA, (void (*)()) 0 },
	{ "Device", HELP_ENADEV, v_enadev },
	{ "Queue", HELP_ENAQUE, v_enaque },
	{ (char *) 0, 0L, (void (*)()) 0 },
#ifdef SDSC
	{ "QUIT", HELP_EXI, v_exi },
#endif
	{ "EXit", HELP_EXI, v_exi },
	{ "Lock", HELP_LOCDAE, (void (*)()) 0 },
	{ "Local_daemon", HELP_LOCDAE, v_locdae },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "MODify", HELP_MODREQ, (void (*)()) 0 },
	{ "Request", HELP_MODREQ, v_modreq },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "MOVe", HELP_MOV, (void (*)()) 0 },
	{ "Queue", HELP_MOVQUE, v_movque },
	{ "Request", HELP_MOVREQ, v_movreq },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "Purge", HELP_PURQUE, (void (*)()) 0 },
	{ "Queue", HELP_PURQUE, v_purque },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "Remove", HELP_REMQUE, (void (*)()) 0 },
#ifdef SDSC
	{ "Queues", HELP_REMQUE, v_remque },
#else
	{ "Queue", HELP_REMQUE, v_remque },
#endif
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "SEt", HELP_SET, (void (*)()) 0 },
	{ "COMplex", HELP_SETCOMRUNLIM, (void (*)()) 0 },
	{ "Run_limit", HELP_SETCOMRUNLIM, v_setcomrunlim },
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt */
	{ "CORefile_limit", HELP_SETCOR, v_setppcore },
	{ "DAta_limit", HELP_SETDAT, v_setppdata },
	{ "DEBug", HELP_SETDEB, v_setdeb },
	{ "DEFault", HELP_SETDEF, (void (*)()) 0 },
	{ "Batch_request", HELP_SETDEFBAT, (void (*)()) 0 },
	{ "Priority", HELP_SETDEFBATPRI, v_setdefbatpri },
	{ "Queue", HELP_SETDEFBATQUE, v_setdefbatque },
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt DEFault */
	{ "DEStination_retry", HELP_SETDEFDES, (void (*)()) 0 },
	{ "Time", HELP_SETDEFDESTIM, v_setdefdestim },
	{ "Wait", HELP_SETDEFDESWAI, v_setdefdeswai },
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt DEFault */
	{ "DEVice_request", HELP_SETDEFDEVPRI, (void (*)()) 0 },
	{ "Priority", HELP_SETDEFDEVPRI, v_setdefdevpri },
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt DEFault */
	{ "Print_request", HELP_SETDEFPRI, (void (*)()) 0 },
	{ "Forms", HELP_SETDEFPRIFOR, v_setdefprifor },
	{ "Queue", HELP_SETDEFPRIQUE, v_setdefprique },
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt DEFault */
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt */
	{ "DEStination", HELP_SETDES, v_setdes },
	{ "DEVICE", HELP_SETDEV, v_setdev },
	{ "DEVICE_server", HELP_SETDEVSER, v_setdevser },
	{ "Forms", HELP_SETFOR, v_setfor },
	{ "Global", HELP_SETGBLBATLIM, (void (*)()) 0 },
	{ "Batch_limit", HELP_SETGBLBATLIM, v_setgblbatlim },
	{ (char *) 0, 0L, (void (*)()) 0 },     /* SEt */
	{ "LIfetime", HELP_SETLIFTIM, v_setliftim },
	{ "LOg_file", HELP_SETLOGFIL, v_setlogfil },
	{ "MAIl", HELP_SETMAI, v_setmai },
	{ "MANagers", HELP_SETMAN, v_setman },
#ifdef SDSC
	{ "SOFTUlimit", HELP_SETSOFTULIM, v_setsoftulim },
	{ "HARDUlimit", HELP_SETHARDULIM, v_sethardulim },
#endif
	{ "MAXimum", HELP_SETMAX, (void (*)()) 0 },
	{ "Copies", HELP_SETMAXCOP, v_setmaxcop },
	{ "Open_retries", HELP_SETMAXOPERET, v_setmaxoperet },
	{ "Print_size", HELP_SETMAXPRISIZ, v_setmaxprisiz },
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt */
	{ "NEtwork", HELP_SETNET, (void (*)()) 0 },
	{ "Client", HELP_SETNETCLI, v_setnetcli },
	{ "Daemon", HELP_SETNETDAE, v_setnetdae },
	{ "Server", HELP_SETNETSER, v_setnetser },
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt */
	{ "NIce_value_limit", HELP_SETNIC, v_setppnice },
#ifdef SDSC
	{ "NODE_Group", HELP_SETNODEGRP, v_setnodegrp },
#endif
	{ "NIce_value_limit", HELP_SETNIC, v_setppnice },
	{ "NO_Default", HELP_SETNDF, (void (*)()) 0 },
	{ "Batch_request", HELP_SETNDFBATQUE, (void (*)()) 0 },
	{ "Queue", HELP_SETNDFBATQUE, v_setndfbatque },
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt NO_Default */
	{ "Print_request", HELP_SETNDFPRI, (void (*)()) 0 },
	{ "Forms", HELP_SETNDFPRIFOR, v_setndfprifor },
	{ "Queue", HELP_SETNDFPRIQUE, v_setndfprique },
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt NO_Default */
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt */
	{ "NO_Access", HELP_SETNOACC, v_setnoacc },
	{ "NO_Network_daemon", HELP_SETNNEDAE, v_setnnedae },
	{ "Open_wait", HELP_SETOPEWAI, v_setopewai },
	{ "PER_Process", HELP_SETPERPRO, (void (*)()) 0 },
	{ "Cpu_limit", HELP_SETPERPROCPUT, v_setppcput },
	{ "Memory_limit", HELP_SETPERPROMEM, v_setppmem },
	{ "Permfile_limit", HELP_SETPERPROPER, v_setpppfile },
	{ "Tempfile_limit", HELP_SETPERPROTEM, v_setpptfile },
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt */
	{ "PER_Request", HELP_SETPERREQ, (void (*)()) 0 },
	{ "Cpu_limit", HELP_SETPERREQCPUT, v_setprcput },
#ifdef SDSC
        { "Ncpus", HELP_SETPERREQNCPUS, v_setprncpus },
#endif
	{ "Memory_limit", HELP_SETPERREQMEM, v_setprmem },
	{ "Permfile_limit", HELP_SETPERREQPER, v_setprpfile },
	{ "Tempfile_limit", HELP_SETPERREQTEM, v_setprtfile },
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt */
	{ "PIpe_client", HELP_SETPIPCLI, v_setpipcli },
	{ "PRiority", HELP_SETPRI, v_setpri },
	{ "Run_limit", HELP_SETRUNLIM, v_setrunlim },
#ifdef SDSC
        { "Queue_request_limit", HELP_SETQUEDLIM, v_setQueuedLimit },
#endif
	{ "SHell_strategy", HELP_SETSHS, (void (*)()) 0 },
	{ "FIxed", HELP_SETSHSFIX, v_setshsfix },
	{ "FRee", HELP_SETSHSFRE, v_setshsfre },
	{ "Login", HELP_SETSHSLOG, v_setshslog },
	{ (char *) 0, 0L, (void (*)()) 0 },	/* SEt */
	{ "STack_limit", HELP_SETSTA, v_setppstack },
	{ "UNrestricted_access", HELP_SETUNRACC, v_setunracc },
	{ "Working_set_limit", HELP_SETWOR, v_setppwork },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "SHOw", HELP_SHO, (void (*)()) 0 },
	{ "All", HELP_SHOALL, v_shoall },
	{ "Devices", HELP_SHODEV, v_shodev },
	{ "Forms", HELP_SHOFOR, v_shofor },
	{ "LImits_supported", HELP_SHOLIM, v_sholim },
	{ "LOng", HELP_SHOLONGQUE, (void (*)()) 0 },
	{ "Queues", HELP_SHOLONGQUE, v_sholongque },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "Managers", HELP_SHOMAN, v_shoman },
	{ "Parameters", HELP_SHOPAR, v_shopar },
	{ "Queues", HELP_SHOQUE, v_shoque },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "SHUtdown", HELP_SHUTDOWN, v_shutdown },
	{ "STArt", HELP_STAQUE, (void (*)()) 0 },
	{ "Queue", HELP_STAQUE, v_staque },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "STOp", HELP_STOQUE, (void (*)()) 0 },
	{ "Queue", HELP_STOQUE, v_stoque },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ "Unlock", HELP_UNLDAE, NULL },
	{ "Local_daemon", HELP_UNLDAE, v_unldae },
	{ (char *) 0, 0L, (void (*)()) 0 },
	{ (char *) 0, 0L, (void (*)()) 0 }
};


/*** main
 *
 *
 *	void main():
 *	NQS manager main function.
 */
main ()
{
	static char *ambverb = "Ambiguous command verb.";
	static char *invverb = "Invalid command verb.";

	void cleanup();			/* Cleanup exit function */
	void help();			/* Help function */
	void scan_modifiers();		/* Scan verb modifiers */

	int size;			/* Generic descr size checking */
	int residual;			/* Generic descr size checking */
	int state;			/* State */
	int tokentype;			/* Scanned token type */
	short helpseen;			/* Help command prefix seen */

	signal (SIGINT, cleanup);	/* Catch common signals */
	signal (SIGQUIT, cleanup);	/* and exit */
	signal (SIGHUP, cleanup);
	signal (SIGTERM, cleanup);
	signal (SIGPIPE, cleanup);	/* For broken stream socket */
					/* connection to a remote machine */
	/*
	 *  Set the default number of lines to display at a time,
	 *  when showing a help page to the user.
	 */
	Help_lines = DEFAULT_HELPLINES;	/* Set default */
	/*
	 *  Determine the machine-id of the local machine.
	 */
	if (localmid (&Local_mid) != 0) {
		fprintf (stderr, "%s(FATAL): Unable to determine machine-id ",
			 Nqsmgr_prefix);
		fprintf (stderr, "of local host.\n");
		exit (1);
	}
	/*
	 *  Determine the NQS privileges (or lack thereof), of the
	 *  invoking user.
	 */
	Mgr_priv = nqspriv (getuid(), Local_mid, Local_mid);
	/*
	 *  Check to see that a single instance of any NQS database
	 *  structure in the queue or device descriptor file will fit
	 *  within a single block of size ATOMICBLKSIZ.  This constraint
	 *  must be satisfied so that updates to the NQS status and
	 *  configuration database files will happen atomically.
	 */
	size = sizeof (struct gendescr);
	residual = size % sizeof (ALIGNTYPE);
	if (residual) size += sizeof (ALIGNTYPE) - residual;
	if (size > ATOMICBLKSIZ) {
		fprintf (stderr, "%s(FATAL): Configuration database ",
			 Nqsmgr_prefix);
		fprintf (stderr, "descriptor size exceeds ATOMICBLKSIZ.\n");
		exit(2);
	}
	/*
	 *  Change directory to the NQS "root" directory.
	 */
	if (chdir (Nqs_root) == -1) {
		fprintf (stderr, "%s(FATAL): Unable to chdir() to %s.\n",
			 Nqsmgr_prefix, Nqs_root);
		exit(3);
	}
	/*
	 *  Open the network queue descriptor file.
	 */
	if ((Netqueuefile = opendb (Nqs_netqueues, O_RDONLY)) == NULL) {
		/*
		 *  We could not open the network queue descriptor file.
		 */
		fprintf (stderr, "%s(FATAL): Unable to open the ",
			 Nqsmgr_prefix);
		fprintf (stderr, "network queue descriptor file.\n");
		exit(4);
	}
	/*
	 *  Open the non-network queue descriptor file.
	 */
	if ((Queuefile = opendb (Nqs_queues, O_RDONLY)) == NULL) {
		/*
		 *  We could not open the non-network queue descriptor file.
		 */
		fprintf (stderr, "%s(FATAL): Unable to open the ",
			 Nqsmgr_prefix);
		fprintf (stderr, "non-network queue descriptor file.\n");
		exit(5);
	}
	/*
	 *  Open the device descriptor file.
	 */
	if ((Devicefile = opendb (Nqs_devices, O_RDONLY)) == NULL) {
		/*
		 *  We could not open the device descriptor file.
		 */
		fprintf (stderr, "%s(FATAL): Unable to open the ",
			 Nqsmgr_prefix);
		fprintf (stderr, "device descriptor file.\n");
		exit(6);
	}
	/*
	 *  Open the queue/device/destination mapping descriptor file.
	 */
	if ((Qmapfile = opendb (Nqs_qmaps, O_RDONLY)) == NULL) {
		/*
		 *  We could not open the queue/device/destination mapping
		 *  descriptor file.
		 */
		fprintf (stderr, "%s(FATAL): Unable to open the ",
			 Nqsmgr_prefix);
		fprintf (stderr, "queue/device/destination mapping file.\n");
		exit(7);
	}
	/*
	 *  Open the pipe queue destination descriptor file.
	 */
	if ((Pipeqfile = opendb (Nqs_pipeto, O_RDONLY)) == NULL) {
		/*
		 *  We could not open the pipe queue destination
		 *  descriptor file.
		 */
		fprintf (stderr, "%s(FATAL): Unable to open the ",
			 Nqsmgr_prefix);
		fprintf (stderr, "pipe queue destination mapping file.\n");
		exit(8);
	}
	/*
	 *  Open the general parameters file.
	 */
	if ((Paramfile = opendb (Nqs_params, O_RDONLY)) == NULL) {
		/*
		 *  We could not open the general parameters file.
		 */
		fprintf (stderr, "%s(FATAL): Unable to open the ",
			 Nqsmgr_prefix);
		fprintf (stderr, "general parameters file.\n");
		exit(9);
	}
	/*
	 *  Open the queue complex descriptor file.
	 */
	if ((Qcomplexfile = opendb (Nqs_qcomplex, O_RDONLY)) == NULL) {
		/*
		 *  We could not open the queue complex descriptor file.
		 */
		fprintf (stderr, "%s(FATAL): Unable to open the ",
			 Nqsmgr_prefix);
		fprintf (stderr, "queue complex descriptor file.\n");
		exit(9);
	}
	/*
	 *  Open the NQS managers file.
	 */
	if ((Mgrfile = opendb (Nqs_mgracct, O_RDONLY)) == NULL) {
		/*
		 *  We could not open the NQS manager access list file.
		 */
		fprintf (stderr, "%s(FATAL): Unable to open the ",
			 Nqsmgr_prefix);
		fprintf (stderr, "NQS manager access list file.\n");
		exit(10);
	}
	/*
	 *  Open the NQS forms file.
	 */
	if ((Formsfile = opendb (Nqs_forms, O_RDONLY)) == NULL) {
		/*
		 *  We could not open the NQS forms list file.
		 */
		fprintf (stderr, "%s(FATAL): Unable to open the ",
			 Nqsmgr_prefix);
		fprintf (stderr, "NQS forms file.\n");
		exit(11);
	}
	/*
	 *  Open the NQS qmgr help file.
	 */
	if ((Helpfile = fopen (QMGR_HELPFILE, "r")) == NULL) {
		/*
		 *  Unable to open the NQS help file.
		 */
		fprintf (stderr, "%s(FATAL): Unable to open the ",
			 Nqsmgr_prefix);
		fprintf (stderr, "NQS Qmgr help file.\n");
		exit (12);
	}
	/*
	 *  Get a pipe to the local daemon.
	 *  On systems with named pipes, we get a pipe to the local
	 *  daemon automatically the first time we call inter().
	 */
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
#else
#if	BSD42 | BSD43
	if (interconn () < 0) {
		fprintf (stderr, "%s(FATAL): Unable to get ", Nqsmgr_prefix);
		fprintf (stderr, "a pipe to the local daemon.\n");
		exit (13);
	}
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  Block buffer stdout for maximum efficiency.
	 *  This will be particularly important in the networked version
	 *  of Qmgr....
	 */
	bufstdout();
	/*
	 *  Loop forever to process commands until exit or EOF.
	 */
	for (;;) {
		do {
			new_command();		/* Get a new command */
			tokentype = scan();	/* Get type of first token */
		} while (tokentype == T_EOC || tokentype == T_ABORT);
		if (tokentype == T_EOF) {	/* Exit, no more commands */
			putchar ('\n');
			exiting();		/* Relinquish any connection */
						/* to the local NQS daemon */
			fflush (stdout);	/* Flush all output */
			fflush (stderr);
			exit (0);
		}
		if (tokentype != T_LITERAL) {
			scan_error (invverb);	/* Invalid command verb */
			continue;		/* Get a new command */
		}
		/*
		 *  The first token in the command line is a character
		 *  string literal, which may be a valid NQS manager
		 *  program command verb.
		 */
		helpseen = 0;			/* No help prefix seen */
		if (matchkeyword (get_literal(), "Help") == 1 ||
		    strcmp (get_literal(), "?") == 0) {
			/*
			 *  Scan next token to get command verb.
			 */
			if ((tokentype = scan()) == T_EOC) {
				help (HELP);	/* Help */
				continue;
			}
			else if (tokentype == T_ABORT) continue;
			else if (tokentype != T_LITERAL) {
				scan_error (invverb);	/* Invalid command */
				continue;
			}
			if (matchkeyword (get_literal(), "Help") == 1 ||
			    strcmp (get_literal(), "?") == 0) {
				help (HELP);
				continue;
			}
			helpseen = 1;		/* Help command prefix seen */
		}
		/*
		 *  The current get_literal() token under inspection needs
		 *  to be a valid NQS manager program command verb.  It is
		 *  already known that the text of this token is not
		 *  equivalent to "help", or any abbreviation thereof.
		 */
		state = 0;		/* Verb table scan state is 0 */
		while (state >= 0) {
			switch (matchkeyword (get_literal(),
				cset [state].vmod)){
			case  1:			 /* Verb matched */
				if (cset [state].cf == NULL) {
					scan_modifiers (state, helpseen);
				}
				else if (helpseen) {
					help (cset [state].help_offset);
				}
				else (*cset [state].cf)();
				state = -1;		/* Exit loop */
				break;
			case  0:			/* No verb match */
				state = search_state (state);
				if (cset [state].vmod == NULL) {
					scan_error (invverb);
					state = -1;	/* Exit loop */
				}
				break;
			case -1:			/* Ambiguous match */
				scan_error (ambverb);	/* Show error */
				state = -1;		/* Exit loop */
				break;
			}
		}
	}
}


/*** cleanup
 *
 *
 *	void cleanup():
 *
 *	Relinquish any connection to the local NQS daemon on receipt
 *	of signal.
 */
static void cleanup (sig)
int sig;				/* Signal received */
{
	signal (sig, SIG_IGN);		/* Ignore multiple signals */
	exiting();			/* Relinquish connection to the */
					/* local NQS daemon */
	fflush (stdout);		/* Flush all output */
	fflush (stderr);
	exit (14);
}


/*** help
 *
 *
 *	void help():
 *	Display help information on a given topic from the help file.
 */
static void help (page)
long page;				/* Offset of help page in help file */
{
	register short ch;		/* Character to write */
	register short lines;		/* Number of lines written */
	register short totty;		/* Boolean TRUE if writing to a tty */

	lines = 0;
	totty = isatty (1);		/* Stdout going to tty? */
	fseek (Helpfile, page, 0);	/* Seek to 1st char of help page */
	while ((ch = getc (Helpfile)) != EOF && ch != '#') {
		putchar (ch);
		if (totty && ch == '\n') {
			lines++;	/* One more line written */
			if (lines == Help_lines) {
				/*
				 *  Stop here, prompting for more
				 *  help.
				 */
				printf ("Press <RETURN> to continue.");
				fflush (stdout);
				while ((ch = getchar()) != EOF && ch != '\n')
					;
				lines = 0;	/* Start a new screen */
			}
		}
	}
}


/*** scan_modifiers
 *
 *
 *	void scan_modifiers():
 *	A valid command verb has been found which has required modifiers.
 */
static void scan_modifiers (state, helpseen)
register int state;			/* Parse state */
int helpseen;				/* Help verb seen */
{
	static char *invmod = "Invalid command verb modifier.";
	static char *ambmod = "Ambiguous command verb modifier.";
	static char *partcmd = "Incomplete command specified.";

	register int tokentype;

	do {
		tokentype = scan();	/* Get type of next token */
		if (tokentype == T_EOC) {
			/*
			 *  A command verb modifier was expected.
			 */
			if (helpseen) help (cset [state].help_offset);
			else scan_error (partcmd);	/* Partial command */
			state = -1;			/* Exit loop */
		}
		else if (tokentype != T_LITERAL) {
			/*
			 *  A character string literal is needed as a
			 *  command verb modifier, and we did not get
			 *  it.
			 */
			scan_error (invmod);		/* Invalid modifier */
			state = -1;			/* Exit loop */
		}
		else {
			/*
			 *  The current token is a character string
			 *  literal, which may be a valid (and required)
			 *  command verb modifier.
			 */
			state++;	/* Increment state to modifier */
					/* descriptors for command verb */
			do {
				/*
				 *  Loop to identify command verb modifier.
				 */
				switch (matchkeyword (get_literal(),
					cset [state].vmod)) {
				case  1:		/* Modifier matched */
					if (cset [state].cf == NULL) {
						/*
						 *  More required modifiers....
						 */
						scan_modifiers(state, helpseen);
					}
					else if (helpseen) {
						help (cset [state].help_offset);
					}
					else (*cset [state].cf)();
					state = -1;	/* Exit loop */
					break;
				case  0:		/* No match */
					state = search_state (state);
					if (cset [state].vmod == NULL) {
						scan_error (invmod);
						state = -1;	/* Exit loop */
					}
					break;
				case -1:		/* Ambiguous match */
					scan_error (ambmod);	/* Show error */
					state = -1;		/* Exit loop */
					break;
				}
			} while (state >= 0);
		}
	} while (state >= 0);
}


/*** search_state
 *
 *
 *	int search_state():
 *
 *	Adjust verb [modifier] searching state to next verb [modifier]
 *	in command set (cset).
 *
 *	Returns:
 *		The new command state integer.
 */
static int search_state (state)
register int state;
{
	register int modifier_level;

	/*
	 *  Scan past any modifier descriptors for the current command
	 *  verb [modifier] to reach the next command descriptor (state)
	 *  entry.
	 */
	modifier_level = 0;
	do {
		if (cset [state].vmod == NULL) {
			/*
			 *  We have reached the end of one modifier subtree
			 *  within the possible parse tree for the current
			 *  command verb [modifier].
			 */
			modifier_level--;
		}
		else if (cset [state].cf == NULL) {
			/*
			 *  We have reached another modifier subtree in
			 *  the possible parse tree for the current
			 *  command verb [modifier].
			 */
			modifier_level++;
		}
		state++;			/* Next state */
	} while (modifier_level > 0);
	return (state);
}
