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
 
/*++ ldparam.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/ldparam.c,v $
 *
 * DESCRIPTION:
 *
 *	Load the general NQS operating parameters.  This module is used
 *	by the main NQS daemon via ../src/nqs_ldconf.c  and by the Qmgr
 *	program via ../src/mgr_cmd.c.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 17, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:27:02 $ $State: Exp $)
 * $Log: ldparam.c,v $
 * Revision 1.4  1994/11/19  02:27:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  01:23:40  mwan
 * 1.2 mods
 *
 * Revision 1.2  1992/10/09  20:16:52  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:56  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:50:44  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"
#include "nqsxvars.h"			/* External global variables */


/*
 *	External refs:
 */
extern struct gendescr *nextdb();	/* Get next allocated descriptor from */
					/* an NQS database file */
extern void seekdb();			/* Unbuffered seek on database file */
extern char *strcpy();			/* String copy function */
extern long telldb();			/* Return position of last read */
					/* record in database file */

/*** ldparam
 *
 *
 *	void ldparam():
 *	Load the general NQS operating parameters.
 */
void ldparam ()
{
	register struct gendescr *gendescr;	/* Database descriptor */

	/*
	 *  Read and process the general NQS operating parameters
	 *  from the NQS parameter file.
	 */
	Debug = 0;				/* Set up defaults */
	Defbatpri = MAX_RPRIORITY / 2;
	Defbatque [0] = '\0';
	Defdesrettim = 3600L * 24 * 3;		/* 3 days of patience */
	Defdesretwai = 300;			/* 5-minute dest retry waits */
	Defdevpri = MAX_RPRIORITY / 2;
	Defnetrettim = 3600L * 24 * 3;		/* 3 days of patience */
	Defnetretwai = 300;			/* 5-minute net retry waits */
	Defprifor [0] = '\0';
	Defprique [0] = '\0';
	Fixed_shell [0] = '\0';			/* No fixed shell by default */
	Lifetime = 86400 * 7;			/* 1-week pipe queue lifetime */
	Logfilename [0] = '\0';			/* No default logfile */
	/* TAC Intergraph gave logfile default strcpy(Logfilename,"/usr/lib/nqs
		/logfile"); 	*/
	Maxcopies = 2;
	Maxextrequests = MAX_EXTREQUESTS;
	Maxgblacclimit = MAX_GBLACCLIMIT;
	Maxgblbatlimit = MAX_GBLBATLIMIT;
	Maxgblnetlimit = MAX_GBLNETLIMIT;
	Maxgblpiplimit = MAX_GBLPIPLIMIT;
#ifdef SDSC
	Maxsoftulimit = MAX_SOFTULIMIT;
	Maxhardulimit = MAX_HARDULIMIT;
#endif
	Maxoperet = 2;				/* TAC Interggraph up to 180 */
	Maxprint = 1000000;			/*TAC Intgrph up to 10000000 */
	Mail_uid = 0;				/* Default mail acct is root */
#if	NETWORKED
#ifndef	NQS_LIBEXE
NQS_LIBEXE must be defined!
#else
	sprintf (Netdaemon, "%s/netdaemon", NQS_LIBEXE);
						/* Default network daemon */
	sprintf (Netserver, "%s/netserver", NQS_LIBEXE);
						/* Default network server */
	sprintf (Netclient, "%s/netclient", NQS_LIBEXE);
						/* Default emptier of */
						/* network queues */
#endif
#else
	Netdaemon [0] = '\0';			/* No network daemon */
						/* configured by default */
	Netserver [0] = '\0';			/* No network server */
						/* configured by default */
	Netclient [0] = '\0';			/* No network client */
						/* configured by default */
#endif
	Opewai = 5;				/* TAC Intrgrph changed to 10 */
	Plockdae = 0;				/* Not locked in mem by def */
	Shell_strategy = SHSTRAT_FREE;		/* Free shell-choice */
	Termsignal = TERMSIG_DEFAULT;		/* Default request termsig */
	Udbgenparams = -1;			/* Offset of these three */
	Udbnetprocs = -1;			/* database records is */
	Udblogfile = -1;			/* not known */
	gendescr = nextdb (Paramfile);
	while (gendescr != (struct gendescr *) 0 &&
	       gendescr->v.par.paramtype != PRM_NETPEER) {
	    /*
	     *  This entry defines the general NQS operating parameters
	     *  record, the logfile record, or the network server definition
	     *  record.  These three (3) records ALWAYS appear as the first
 	     *  three records in the parameters file.
	     */
	    switch (gendescr->v.par.paramtype) {
	    case PRM_GENPARAMS:
		Udbgenparams = telldb (Paramfile);
		Mail_uid = gendescr->v.par.v.genparams.mail_uid;
		Maxgblacclimit = gendescr->v.par.v.genparams.maxgblacclimit;
		Maxgblbatlimit = gendescr->v.par.v.genparams.maxgblbatlimit;
		Maxgblnetlimit = gendescr->v.par.v.genparams.maxgblnetlimit;
		Maxgblpiplimit = gendescr->v.par.v.genparams.maxgblpiplimit;
#ifdef SDSC
		Maxsoftulimit = gendescr->v.par.v.genparams.maxsoftulimit;
		Maxhardulimit = gendescr->v.par.v.genparams.maxhardulimit;
#endif
		Plockdae = gendescr->v.par.v.genparams.plockdae;
		Shell_strategy = gendescr->v.par.v.genparams.shell_strategy;
		Termsignal = gendescr->v.par.v.genparams.termsignal;
		Debug = gendescr->v.par.v.genparams.debug;
		Defbatpri = gendescr->v.par.v.genparams.defbatpri;
		Defdevpri = gendescr->v.par.v.genparams.defdevpri;
		Maxcopies = gendescr->v.par.v.genparams.maxcopies;
		Maxextrequests = gendescr->v.par.v.genparams.maxextrequests;
		Maxoperet = gendescr->v.par.v.genparams.maxoperet;
		Maxprint = gendescr->v.par.v.genparams.maxprint;
		Opewai = gendescr->v.par.v.genparams.opewai;
		Defdesrettim = gendescr->v.par.v.genparams.defdesrettim;
		Defdesretwai = gendescr->v.par.v.genparams.defdesretwai;
		Defnetrettim = gendescr->v.par.v.genparams.defnetrettim;
		Defnetretwai = gendescr->v.par.v.genparams.defnetretwai;
		Lifetime = gendescr->v.par.v.genparams.lifetime;
		strcpy (Defprifor, gendescr->v.par.v.genparams.defprifor);
		strcpy (Defbatque, gendescr->v.par.v.genparams.defbatque);
		strcpy (Defprique, gendescr->v.par.v.genparams.defprique);
		strcpy (Fixed_shell, gendescr->v.par.v.genparams.fixed_shell);
		strcpy (Netdaemon, gendescr->v.par.v.genparams.netdaemon);
		break;
	    case PRM_LOGFILE:
		Udblogfile = telldb (Paramfile);
		strcpy (Logfilename, gendescr->v.par.v.logfile);
		break;
	    case PRM_NETPROCS:
		Udbnetprocs = telldb (Paramfile);
		strcpy (Netserver, gendescr->v.par.v.netprocs.netserver);
		strcpy (Netclient, gendescr->v.par.v.netprocs.netclient);
		break;
	    }
	    gendescr = nextdb (Paramfile);
	}
}
