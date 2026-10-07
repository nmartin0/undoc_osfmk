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
 
/*++ mgr_show.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qmgr/mgr_show.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS manager show command execution module.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	March 30, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 01:35:47 $ $State: Exp $)
 * $Log: mgr_show.c,v $
 * Revision 1.4  1994/11/19  01:35:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  01:08:07  mwan
 * 1.2 mods
 *
 * Revision 1.2  1992/10/09  22:30:18  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:28:28  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:37  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:03:41  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)mgr_show.c	1.3 (mgr_show.c OSF/1 NQS2.0 GJK) 6/12/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include "nqs.h"			/* NQS types and definitions */
#include "nqsxvars.h"			/* NQS external vars */
#if	UNICOS
#include <nlist.h>
#include <sys/param.h>
#include <sys/jtab.h>
extern	struct	jtab	jtab[NJOB];	/* job table */
#endif

/*
 *	Miscellaneous functions.
 */
extern void closepwdb();		/* Close account/password database */
extern int daepres();			/* Determine if the local NQS */
					/* daemon is present */
extern void endgrpnam();		/* Discard gid/groupname cache */
extern void endmacnam();		/* Discard machine-name cache */
extern void endusenam();		/* Discard uid/username cache */
extern char *getmacnam();		/* Get machine-name for mid */
extern char *getusenam();		/* Get username for uid */
extern void ldparam();			/* Load general NQS operating params */
extern struct gendescr *nextdb();	/* Next allocated database descr */
extern long nextseqno();		/* Next NQS request seq# */
extern int scan_end();			/* Scan end of command */
extern int scan_opaname();		/* Scan optional local account name */
					/* (account name or EOL) */
extern int scan_opdev();		/* Scan an optional device name */
					/* (device name or EOL) */
extern int scan_opqueue();		/* Scan an optional queue name */
					/* (queue name or EOL) */
extern void seekdb();			/* Seek on database file */
extern int shoalldev();			/* Show all devices */
extern int shoalllim();			/* Show all limits */
extern int shoallque();			/* Show all queues */
extern int shodbyname();		/* Show a device given a string */
extern int shoqbyname();		/* Show a queue given a string */


extern mid_t Local_mid;			/* Machine-id of local host */
extern int Mgr_priv;			/* Privilege bits for current */
					/* user of Qmgr as imported from */
					/* mgr_main.c */


/*** v_shoall
 *
 *
 *	void v_shoall():
 *	SHOw All command.
 */
void v_shoall()
{
	void shofor();			/* Show forms */
	void shoman();			/* Show managers */
	void shopar();			/* Show general operating parameters */

	long flags;			/* Display flags */
	short daepresent;		/* Boolean non-zero if the local */
					/* NQS daemon is present and running */

	if (scan_end() == -1) return;
	/*
	 *  Determine if the local NQS daemon is still present and running,
	 *  and set the daepresent boolean flag accordingly.
	 */
#if	BSD42 | BSD43 | ULTRIX
	daepresent = daepres (Queuefile);
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	daepresent = daepres();
#else
BAD SYSTEM TYPE
#endif
#endif
	flags  = (SHO_R_ALLUID | SHO_R_SHORT);
	flags |= (SHO_RS_EXIT | SHO_RS_RUN | SHO_RS_STAGE | SHO_RS_QUEUED |
		  SHO_PIPE | SHO_BATCH |SHO_DEVICE | SHO_QUE |
		  SHO_RS_WAIT | SHO_RS_HOLD | SHO_RS_ARRIVE);
	printf ("Queues:\n\n");
	/*
	 *  We must seek to the start of all of the database files
	 *  used by shoallque(), because this Qmgr program (or other
	 *  Qmgr programs), may have just changed the NQS configuration.
	 *
	 *  Furthermore, we use seekdb() INSTEAD of seekdbb(), since
	 *  we MUST do an unbuffered seek to get the latest copy of
	 *  the first block of each of the requisite database files.
	 */
	seekdb (Queuefile, 0L);		/* Seek (unbuffered) to file start */
	seekdb (Qmapfile, 0L);		/* Seek (unbuffered) to file start */
	seekdb (Pipeqfile, 0L);		/* Seek (unbuffered) to file start */
	seekdb (Qcomplexfile, 0L);	/* Seek (unbuffered) to file start */
#if UNICOS
	/*
	 *  Read job table from kernel memory.
	 */
	{
		register int fd;
		struct nlist nl[2];

		nl[0].n_name = "jtab";
		nl[1].n_name = "";
		fd = open("/dev/kmem", 0);
		if (fd < 0 ||
		    nlist( "/unicos", nl ) < 0 ||
	    	    lseek( fd, nl[0].n_value, 0 ) < 0 ||
	    	    read( fd, (char*)jtab, sizeof(jtab) ) != sizeof(jtab)) {
			fprintf(stderr, "Unable to read job table.\n");
		}
		close(fd);
	}
#endif
	/* display queues */
	list(Queuefile, Qcomplexfile,NULL,flags,NULL,Qmapfile,Pipeqfile);
	printf ("Devices:\n\n");
	/*
	 *  We must seek to the start of the device definition
	 *  file used by shoalldev(), since this Qmgr program (or
	 *  other Qmgr programs), may have just changed the NQS
	 *  configuration.
	 *
	 *  Furthermore, we use seekdb() INSTEAD of seekdbb(), since
	 *  we MUST do an unbuffered seek to get the latest copy of
	 *  the first block of the device definition file.
	 */
	seekdb (Devicefile, 0L);	/* Seek (unbuffered) to file start */
	shoalldev (Devicefile);
	printf ("Managers:\n");
	shoman();
	printf ("Forms:\n");
	shofor();
	printf ("NQS operating parameters:\n");
	shopar();
	printf ("Limits supported:\n\n");
	shoalllim ((struct confd *) 0, 0);	/* Don't show shell strategy */
}


/*** v_shodev
 *
 *
 *	void v_shodev():
 *	SHOw Device command.
 */
void v_shodev()
{
	char devname [MAX_DEVNAME + 1 + MAX_MACHINENAME + 1];

	/*
	 *  We must seek to the start of the device definition
	 *  file used by shoalldev() and shodbyname(), since this
	 *  Qmgr program (or other Qmgr programs), may have just
	 *  changed the NQS configuration.
	 *
	 *  Furthermore, we use seekdb() INSTEAD of seekdbb(), since
	 *  we MUST do an unbuffered seek to get the latest copy of
	 *  the first block of the device definition file.
	 */
	seekdb (Devicefile, 0L);	/* Seek (unbuffered) to file start */
	if (scan_opdev (devname) == -1) return;	/* Scan optional device */
	if (devname [0] == '\0') {
		shoalldev (Devicefile);
	}
	else {
		if (scan_end() == -1) return;
		shodbyname (Devicefile, devname);
	}
	endmacnam();			/* Discard machine-name cache */
	endusenam();			/* Flush username cache and */
}					/* close password file opened */
					/* by shodev() */


/*** v_shofor
 *
 *
 *	void v_shofor():
 *	SHOw Forms command.
 */
void v_shofor()
{
	void shofor();			/* Show forms */

	if (scan_end() == -1) return;
	shofor();
}


/*** shofor
 *
 *
 *	void shofor():
 *	Show the forms set.
 */
static void shofor()
{
	register struct gendescr *descr;
	register int col;

	/*
	 *  We must seek to the start of all the forms definition
	 *  file, since this Qmgr program (or other Qmgr programs),
	 *  may have just changed the NQS configuration.
	 *
	 *  Furthermore, we use seekdb() INSTEAD of seekdbb(), since
	 *  we MUST do an unbuffered seek to get the latest copy of
	 *  the first block of the forms definition file.
	 */
	seekdb (Formsfile, 0L);		/* Seek (unbuffered) to file start */
	putchar ('\n');
	descr = nextdb (Formsfile);
	col = 3;
	while (descr != (struct gendescr *) 0) {
		col = putname (descr->v.form.forms, MAX_FORMNAME, col);
		descr = nextdb (Formsfile);
	}
	if (col != 3) putchar ('\n');
	putchar ('\n');
}


/*** v_sholim
 *
 *
 *	void v_sholim():
 *	SHOw LImits_supported command.
 */
void v_sholim()
{
	if (scan_end() == -1) return;
	putchar ('\n');
	shoalllim ((struct confd *) 0, 0);	/* Don't show shell strategy */
}


/*** v_sholongque
 *
 *
 *	void v_sholongque():
 *	SHOw LOng Queue command.
 */
void v_sholongque()
{
	char quename [MAX_QUEUENAME+1+MAX_MACHINENAME+1]; /* Name of queue */
	long flags;			/* Display flags */
	short daepresent;		/* Boolean non-zero if the local */
					/* NQS daemon is present and running */
	long whomuid;			/* Uid of account name */

	flags = (SHO_PIPE | SHO_BATCH | SHO_DEVICE | SHO_FULL
		 | SHO_RS_RUN | SHO_RS_EXIT | SHO_RS_QUEUED
		 | SHO_RS_HOLD | SHO_QUE | SHO_R_ALLUID );

	if (scan_opqueue (quename) == -1) return;
	/*
	 *  We must seek to the start of all of the database files
	 *  used by shoallque() or shoqbyname(), because this Qmgr
	 *  program (or other Qmgr programs), may have just changed
	 *  the NQS configuration.
	 *
	 *  Furthermore, we use seekdb() INSTEAD of seekdbb(), since
	 *  we MUST do an unbuffered seek to get the latest copy of
	 *  the first block of each of the requisite database files.
	 *  ( NOT !!! valid CERN change-> CHANGED as a seekdb() will cause an error the next time
	 *  this command is used in the same session of qmgr. <- NOT !!! valid CERN change)
	 */
	seekdb (Queuefile, 0L); /*  CERN Boissat begin */
	seekdb (Qcomplexfile, 0L);
	seekdb (Qmapfile, 0L);		/* Seek (unbuffered) to file start */
	seekdb (Pipeqfile, 0L);	/* Seek (unbuffered) to file start */
				/*  CERN Boissat end */

#if UNICOS
	/*
	 *  Read job table from kernel memory.
	 */
	read_j_table();
#endif

	if (quename [0] == '\0') {
		/*
		 *  All queues are to be shown.
		 *  Whomuid contains garbage, OK because SHO_R_ALLUID is on.
		 *  The same is true below if whomuid == -1.
		 */
		list(Queuefile, Qcomplexfile,NULL,flags,NULL,
			Qmapfile,Pipeqfile);
	}
	else {
		if (scan_opaname (&whomuid) == -1) return;
		if (whomuid != -1) {
			if (scan_end() == -1) return;
			flags &= ~SHO_R_ALLUID;
		}
		list(Queuefile, Qcomplexfile,quename,flags,NULL,
			Qmapfile,Pipeqfile);
	}
	endgrpnam();		/* Flush groupname cache and close */
				/* group file opened by a callee */
	endmacnam();		/* Discard machine-name cache */
	endusenam();		/* Flush username cache and close */
}				/* password file opened by a callee */


/*** v_shoman
 *
 *
 *	void v_shoman():
 *	SHOw Managers command.
 */
void v_shoman()
{
	void shoman();			/* Show managers */

	if (scan_end() == -1) return;
	shoman();
}


/*** shoman
 *
 *
 *	void shoman():
 *	Show the manager set.
 */
static void shoman()
{
	register struct gendescr *descr;

	/*
	 *  We must seek to the start of all the manager definition
	 *  file, since this Qmgr program (or other Qmgr programs),
	 *  may have just changed the NQS configuration.
	 *
	 *  Furthermore, we use seekdb() INSTEAD of seekdbb(), since
	 *  we MUST do an unbuffered seek to get the latest copy of
	 *  the first block of the manager definition file.
	 */
	seekdb (Mgrfile, 0L);		/* Seek (unbuffered) to file start */
	putchar ('\n');
	descr = nextdb (Mgrfile);
	while (descr != (struct gendescr *) 0) {
		if (descr->v.mgr.manager_mid != Local_mid) {
			/*
			 *  The manager account is on a remote machine.
			 */
			printf ("  [%1d]@", (int) descr->v.mgr.manager_uid);
			fputs (getmacnam (descr->v.mgr.manager_mid), stdout);
		}
		else {
			/*
			 *  The manager account is on the local machine.
			 */
			printf ("  %s", getusenam (descr->v.mgr.manager_uid));
		}
		if (descr->v.mgr.privileges & QMGR_MGR_PRIV) {
			fputs (":m\n", stdout);
		}
		else fputs (":o\n", stdout);
		descr = nextdb (Mgrfile);
	}
	endmacnam();			/* Discard machine-name cache */
	endusenam();			/* Discard username cache and */
	putchar ('\n');			/* close the password file */
}


/*** putname
 *
 *
 *	int putname():
 *	Display a name in formatted columns width+2 fields wide.
 */
static int putname (name, width, col)
char *name;				/* Name to be displayed */
int width;				/* Maximum name length */
int col;				/* Current column: [1..n] */
{
	register int i;

	printf ("  %s", name);
	width += 2;
	if ((col += width) + width > 79) {
		putchar ('\n');
		col = 3;
	}
	else {
		i = width - strlen (name);
		while (i--) putchar (' ');
	}
	return (col);
}


/*** v_shopar
 *
 *
 *	void v_shopar():
 *	SHOw Parameters command.
 */
void v_shopar()
{
	void shopar();			/* Show general NQS operating */
					/* parameters */

	if (scan_end() == -1) return;
	shopar();
}


/*** shopar
 *
 *
 *	void shopar():
 *	Show general NQS operating parameters.
 */
static void shopar()
{
	/*
	 *  We must seek to the start of all the general parameters
	 *  file, since this Qmgr program (or other Qmgr programs),
	 *  may have just changed the NQS configuration.
	 *
	 *  Furthermore, we use seekdb() INSTEAD of seekdbb(), since
	 *  we MUST do an unbuffered seek to get the latest copy of
	 *  the first block of the general parameters file.
	 */
	seekdb (Paramfile, 0L);		/* Seek (unbuffered) to file start */
	ldparam();			/* Load general NQS operating */
					/* parameters. */
	putchar ('\n');
	printf ("  Debug level =  %1d\n", Debug);
	printf ("  Default batch_request priority = %1d\n", Defbatpri);
	printf ("  Default batch_request queue = ");
	if (Defbatque [0] == '\0') printf ("NONE\n");
	else printf ("%s\n", Defbatque);
	printf ("  Default destination_retry time = %1ld hours\n",
	       (long) Defdesrettim / 3600);
	printf ("  Default destination_retry wait = %1ld minutes\n",
	       (long) Defdesretwai / 60);
	printf ("  Default device_request priority = %1d\n", Defdevpri);
	if (Defprifor[0] == '\0') printf ("  No default print forms\n");
	else printf ("  Default print forms = %s\n", Defprifor);
	printf ("  Default print queue = ");
	if (Defprique [0] == '\0') printf ("NONE\n");
	else printf ("%s\n", Defprique);
	printf ("  Global batch run limit = %1d\n", Maxgblbatlimit);
	printf ("  Global pipe limit = %1d\n", Maxgblpiplimit);
	printf ("  (Pipe queue request) Lifetime = %1ld hours\n",
		Lifetime / 3600);
	printf ("  Log_file = ");
	if (Logfilename [0] == '\0') printf ("/dev/null");
	else fputs (Logfilename, stdout);
	putchar ('\n');
	printf ("  Mail account = %s\n", getusenam (Mail_uid));
	printf ("  Maximum number of print copies = %1d\n", Maxcopies);
	printf ("  Maximum failed device open retry limit = %1d\n", Maxoperet);
	printf ("  Maximum print file size = %1ld bytes\n", Maxprint);
	printf ("  Netdaemon = ");
	if (Netdaemon [0] == '\0') printf ("NONE\n");
	else printf ("%s\n", Netdaemon);
	printf ("  Netclient = ");
	if (Netclient [0] == '\0') printf ("NONE\n");
	else printf ("%s\n", Netclient);
	printf ("  Netserver = ");
	if (Netserver [0] == '\0') printf ("NONE\n");
	else printf ("%s\n", Netserver);
	printf ("  (Failed device) Open_wait time = %1d seconds\n", Opewai);
	if (Plockdae) printf ("  NQS daemon is LOCKED in memory\n");
	else printf ("  NQS daemon is not locked in memory\n");
	Seqno_user = nextseqno();	/* Next request seq# */
	printf ("  Next available sequence number = %1ld\n", Seqno_user);
	printf ("  Batch request shell choice strategy = ");
	if (Shell_strategy == SHSTRAT_FREE) printf ("FREE\n");
	else {
		if (Shell_strategy == SHSTRAT_LOGIN) printf ("LOGIN\n");
		else {
			printf ("FIXED: %s\n", Fixed_shell);
		}
	}
#ifdef SDSC
	printf ("  Soft user_limit = %d\n", Maxsoftulimit);
	printf ("  Hard user_limit = %d\n", Maxhardulimit);
#endif
	putchar ('\n');
	endusenam();			/* Discard username cache */
}


/*** v_shoque
 *
 *
 *	void v_shoque():
 *	SHOw Queue command.
 */
void v_shoque()
{
	char quename [MAX_QUEUENAME+1+MAX_MACHINENAME+1]; /* Name of queue */
	long flags;			/* Display flags */
	short daepresent;		/* Boolean non-zero if the local */
					/* NQS daemon is present and running */
	long whomuid;			/* Uid of account name */

	flags = (SHO_PIPE | SHO_BATCH | SHO_DEVICE
		 | SHO_RS_RUN | SHO_RS_EXIT | SHO_RS_QUEUED
		 | SHO_RS_HOLD | SHO_QUE | SHO_R_ALLUID );

	if (scan_opqueue (quename) == -1) return;
	/*
	 *  We must seek to the start of all of the database files
	 *  used by shoallque() or shoqbyname(), because this Qmgr
	 *  program (or other Qmgr programs), may have just changed
	 *  the NQS configuration.
	 *
	 *  Furthermore, we use seekdb() INSTEAD of seekdbb(), since
	 *  we MUST do an unbuffered seek to get the latest copy of
	 *  the first block of each of the requisite database files.
	 *  ( NOT !!! valid CERN change-> CHANGED as a seekdb() will cause an error the next time
	 *  this command is used in the same session of qmgr. <- NOT !!! valid CERN change)
	 */
	seekdb (Queuefile, 0L);	/* Seek (unbuffered) to file start CERN Boissat */
	seekdb (Qcomplexfile, 0L);
#if UNICOS
	/* *  Read job table from kernel memory.  */
	read_j_table();
#endif


	
	/* all queues are shown if queuename == NULL) */
	if (quename [0] == '\0') {
		/*
		 * All queues are to be shown.
		 * Whomuid contains garbage, OK because SHO_R_ALLUID is on.
		 * The same is true below if whomuid == -1.
		 */
		list(Queuefile, Qcomplexfile,NULL,flags,NULL,
			Qmapfile,Pipeqfile);
	}
	else {
		if (scan_opaname (&whomuid) == -1) return;
		if (whomuid != -1) {
			if (scan_end() == -1) return;
			flags &= ~SHO_R_ALLUID;
		}
		list(Queuefile, Qcomplexfile,quename,flags,NULL,
			Qmapfile,Pipeqfile);
	}
	printf("\n");
	endgrpnam();		/* Flush groupname cache and close */
				/* group file opened by a callee */
	endmacnam();		/* Discard machine-name cache */
	endusenam();		/* Flush username cache and close */
}				/* password file opened by a callee */
