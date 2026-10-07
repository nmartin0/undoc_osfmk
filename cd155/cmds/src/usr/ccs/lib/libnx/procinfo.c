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
 
/*
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Id: procinfo.c,v 1.3 1994/11/19 02:32:25 mtm Exp $
 *
 */

/* procinfo.c -
 * History:
 * $Log: procinfo.c,v $
 * Revision 1.3  1994/11/19  02:32:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/12  17:20:22  carbajal
 * Added Intel header and RCS id strings
 *
*/

#include	<stdio.h>
#include	<sys/table.h>
#include	<sys/user.h>
#include	<sys/types.h>
#include	<unistd.h>


/*	get_proc_info	
 *
 *	Description:
 *		get the process table information for the passed in
 *		pid. This works by getting process table entries NPROC
 *		at a time. The entries are then search until the pid
 *		in the process table matches the passed in pid. Entries
 *		with the ZOMBIE flag set are skipped over.
 *
 *	Parameters: 
 *		pid	process id we are looking for
 *		myproc  pointer to process table structure 
 *
 *	Returns:
 *		TRUE if pid was found
 *		FALSE otherwise
 *
 *	Side Effects:
 *		none
*/
#define		NPROC	10
int get_proc_info(int pid,struct tbl_procinfo *myproc)
{
	struct tbl_procinfo	proc[NPROC];
	register int		i,j;
	int			ok=0,nproc;

	/* see how many entries are in the table */
        nproc = table(TBL_PROCINFO, 0, (char *)0, 32767, 0);

	/* look through all the entries, grabbing NPROC entries
	 * per call.
	*/
        for (i=0; i < nproc; i += NPROC) {
                j = nproc - i;
                if (j > NPROC)
                        j = NPROC;
                j = table(TBL_PROCINFO, i, (char *)proc, NPROC, sizeof(proc[0]));
                for (j = j-1; j >= 0; j--) {
                        if (proc[j].pi_status == PI_ZOMBIE)
                                continue;
			if (proc[j].pi_pid == pid){
				bcopy((char *)&proc[j],(char *)myproc,
						sizeof(proc[0]));
				ok = TRUE;
				break;	
			}
		}
	}
	return(ok);
}

