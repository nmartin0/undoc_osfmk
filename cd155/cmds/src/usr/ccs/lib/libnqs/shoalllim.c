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
 
/*++ shoalllim.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/shoalllim.c,v $
 *
 * DESCRIPTION:
 *
 *	Show information about 
 *	1) all limits on the local machine and
 *	2) the shell strategy on the local machine,
 *	in both cases as selected by the display criteria flags.
 *
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	January 13, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:53 $ $State: Exp $)
 * $Log: shoalllim.c,v $
 * Revision 1.3  1994/11/19  02:27:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:19:02  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:01  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:55:48  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"

extern struct gendescr *nextdb();	/* Return next allocated entry */
					/* from a database file */

/*** shoalllim
 *
 *
 *	int shoalllim():
 *	Show status information on all local limits
 *	and the local shell strategy.
 *
 *	WARNING:
 *		It is assumed that the current position of
 *		the parameter definitions file is 0.
 *
 *	Returns:
 *		Always returns 0. Shoalllim always produces output.
 *
 */
int shoalllim (file, flags)
register struct confd *file;		/* NQS parameters file  or NULL */
register long flags;			/* Display flags */
{
    register struct gendescr *descr;	/* pointer into param file */
    int found;		/* boolean: found the correct param descriptor? */

    if (VALID_LIMITS == 0) {
	printf ("  No limits are supported for this machine.");
    }
    else {
	if (VALID_LIMITS & LIM_PRDRIVES) {
	    printf ("  Per-req. tape drive limit (-l6)\n");
	}
	if (VALID_LIMITS & LIM_PPCORE) {
	    printf ("  Core file size limit (-lc)\n");
	}
	if (VALID_LIMITS & LIM_PPDATA) {
	    printf ("  Data segment size limit (-ld)\n");
	}
	if (VALID_LIMITS & LIM_PPPFILE) {
	    printf ("  Per-process permanent file ");
	    printf ("size limit (-lf)\n");
	}
	if (VALID_LIMITS & LIM_PRPFILE) {
	    printf ("  Per-request permanent file ");
	    printf ("size limit (-lF)\n");
	}
	if (VALID_LIMITS & LIM_PPMEM) {
	    printf ("  Per-process memory size limit (-lm)\n");
	}
	if (VALID_LIMITS & LIM_PRMEM) {
	    printf ("  Per-request memory size limit (-lM)\n");
	}
	if (VALID_LIMITS & LIM_PPNICE) {
	    printf ("  Nice value (-ln)\n");
	}
	if (VALID_LIMITS & LIM_PRNCPUS) {
	    printf ("  Per-request # of cpus limit (-lP)\n");
	}
	if (VALID_LIMITS & LIM_PPQFILE) {
	    printf ("  Per-process quick file ");
	    printf ("size limit (-lq)\n");
	}
	if (VALID_LIMITS & LIM_PRQFILE) {
	    printf ("  Per-request quick file ");
	    printf ("size limit (-lQ)\n");
	}
	if (VALID_LIMITS & LIM_PPSTACK) {
	    printf ("  Stack segment size limit (-ls)\n");
	}
	if (VALID_LIMITS & LIM_PPCPUT) {
	    printf ("  Per-process cpu time limit (-lt)\n");
	}
	if (VALID_LIMITS & LIM_PRCPUT) {
	    printf ("  Per-request cpu time limit (-lT)\n");
	}
	if (VALID_LIMITS & LIM_PPTFILE) {
	    printf ("  Per-process temporary file ");
	    printf ("size limit (-lv)\n");
	}
	if (VALID_LIMITS & LIM_PRTFILE) {
	    printf ("  Per-request temporary file ");
	    printf ("size limit (-lV)\n");
	}
	if (VALID_LIMITS & LIM_PPWORK) {
	    printf ("  Working set limit (-lw)\n");
	}
    }
    putchar ('\n');
    if (flags & SHO_SHS) {		/* Show shell strategy */
	descr = nextdb (file);
	found = 0;
	while (descr != (struct gendescr *)0 && !found) {
	    if (descr->v.par.paramtype == PRM_GENPARAMS) {
		found = 1;
		printf ("  Shell strategy = ");
		switch (descr->v.par.v.genparams.shell_strategy) {
		case SHSTRAT_FREE:
		    printf ("FREE\n");
		    break;
		case SHSTRAT_LOGIN:
		    printf ("LOGIN\n");
		    break;
		case SHSTRAT_FIXED:
		    printf ("FIXED: %s\n",
			    descr->v.par.v.genparams.fixed_shell);
		    break;
		}
	    }
	    descr = nextdb (file);	/* Get the next param */
	}
	if (found == 0) {
	    printf ("  No shell strategy.\n");	
	}
	putchar ('\n');
    }
    return (0);
}
