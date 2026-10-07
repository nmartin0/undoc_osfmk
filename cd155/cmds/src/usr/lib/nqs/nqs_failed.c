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
 
/*++ nqs_failed.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_failed.c,v $
 *
 * DESCRIPTION:
 *
 *	Move the named request control file to the NQS hold directory
 *	for failed requests.
 *
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
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/12/06 22:20:21 $ $State: Exp $)
 * $Log: nqs_failed.c,v $
 * Revision 1.4  1994/12/06  22:20:21  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 7729
 *  Testing: Developer testing, EATs
 *  Module(s): cmds_libs/src/usr/lib/nqs/nqs_failed.c
 *
 * Revision 1.3  1994/11/19  02:52:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:27  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:04  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:05:58  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_failed.c	1.2 (nqs_failed.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include "nqs.h"			/* MAX_PATHNAME definition */
#include "nqsxvars.h"			/* Global vars and directories */
#if	SGI | SYS52 | UTS | UNICOS | OSF
#include "dir.4.2.h"			/* Include directory walking defs */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/dir.h>			/* Include directory walking defs */
#else
BAD SYSTEM TYPE
#endif
#endif

/*
 *	External functions:
 */
extern int is6bitstr();			/* Is 6-bit string boolen fn */
extern void pack6name();		/* Compute name on 6-bit alphabet */
extern int strncmp();			/* String comparison */
extern int strlen();			/* Return string length */

/*** nqs_failed
 *
 *
 *	void nqs_failed():
 *
 *	Move the named request control file from the control directory
 *	to the holding directory for failed requests.
 */
void nqs_failed (orig_seqno, orig_mid)
long orig_seqno;			/* Sequence number for req */
mid_t orig_mid;				/* Machine-id for req */
{
	char ctrlname [15];		/* Name of control file in control */
					/* directory */
	char path [MAX_PATHNAME+1];	/* Control/data subdirectory path */
	char datapath [MAX_PATHNAME+1];	/* Data subdirectory path */
	char holdpath [MAX_PATHNAME+1];	/* Failed directory path */
	DIR *dir;			/* Open queue order dir structure */
	struct direct *dirent;		/* Ptr to directory entry */

	printf ("I$Request being placed in failed directory.\n");
	fflush (stdout);
	pack6name (path, Nqs_control, (int) (orig_seqno % MAX_CTRLSUBDIRS),
		  (char *) 0, (long) orig_seqno, 5, (long) orig_mid, 6, 0, 0);
	pack6name (ctrlname, (char *) 0, -1, (char *) 0, (long) orig_seqno, 5,
		  (long) orig_mid, 6, 0, 0);
	sprintf (holdpath, "%s/%s", Nqs_failed, ctrlname);
	link (path, holdpath);		/* Establish link in failed dir */
	unlink (path);			/* Remove control subdirectory entry */

#ifdef SDSC
        printf ("I$nqs_failed : Control file for the request NQS_%d_%d %s/%s\n",
        orig_mid,orig_seqno,Nqs_root,path);
        printf ("I$nqs_failed : was placed in failed directory %s/%s\n",
        Nqs_root,holdpath);
#endif

	/*
	 *  Traverse the appropriate data subdirectory to grab all data
	 *  files associated with the failed request.
	 */
	pack6name (datapath, Nqs_data, (int) (orig_seqno % MAX_DATASUBDIRS),
		  (char *) 0, 0L, 0, 0L, 0, 0, 0);
	if ((dir = opendir (datapath)) == (DIR *) 0) {
		printf ("E$Unable to open the data dir. in nqs_failed().\n");
		printf ("I$Data files for failed request ");
		printf ("not moved to failed directory.\n");
		fflush (stdout);
		return;
	}
	while ((dirent = readdir (dir)) != (struct direct *) 0) {
		/*
		 *  Determine if this data file is for the request being
		 *  placed on hold.
		 */
		if (strlen (dirent->d_name) == 14 &&
		    is6bitstr (dirent->d_name, 14) &&
		    strncmp (dirent->d_name, ctrlname, 11) == 0) {
			/*
			 *  This data file is for the request being placed
			 *  in the failed directory.
			 */
			sprintf (path, "%s/%s", datapath, dirent->d_name);
			sprintf (holdpath, "%s/%s", Nqs_failed,
				 dirent->d_name);
			link (path, holdpath);	/*Establish link to data file*/
			unlink (path);		/*Remove from data dir */

#ifdef SDSC
                        printf ("I$nqs_failed : Data file for the request NQS_%d_%d %s/%s\n",
                        orig_mid,orig_seqno,Nqs_root,path);
                        printf ("I$nqs_failed : was placed in failed directory %s/%s\n",
                        Nqs_root,holdpath);
                        (void) fflush(stdout);
#endif

		}
	}
	closedir (dir);			/* Close the data file directory */
}
