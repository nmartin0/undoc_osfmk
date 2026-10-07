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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: creadir.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:03 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: creadir
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * creadir.c	1.9  com/lib/c/gen,3.1,8943 10/19/89 16:57:40
 */

#include <sys/types.h>				/* for uid_t / gid_t	*/
#include <sys/stat.h>				/* for struct stat	*/
#include <sys/errno.h>				/* errno defines	*/

extern int chown();				/* system calls		*/
extern int errno;				/* system calls error #	*/

/*
 * NAME:	creadir
 *                                                                    
 * FUNCTION:	create a directory, specifying owner, group, modes
 *                                                                    
 * NOTES:	Searches for the directory given in 'dir', and if not
 *		there, creates it with 'owner', group' and 'mode'.
 *
 * RETURN VALUE DESCRIPTION:	0 if we succeed or if the directory
 *		already exists, else -1
 */  

int creadir(char *path, uid_t owner, gid_t group, int mode)
{
	int old_umask;			/* saved umask			*/
	struct stat path_info;		/* stat buf			*/

	/* stat to see if directory already exists */
	if(stat(path,&path_info) < 0)
	{
		/* does errno indicate a non-existent file? */
		if( errno == ENOENT )
		{
			/* umask(0) so that 'mode' is absolute */
			old_umask = umask((mode_t)0);

			/* attempt to make the directory */
			if(mkdir(path, (mode_t)mode) < 0)
				return(-1);

			/* attempt to chown it over */
			if(chown(path, owner, group) < 0)
				return(-1);

			/* reset the umask back to the original value */
			(void) umask((mode_t)old_umask);
		}

		else
			/* strange errno... */
			return(-1);
	}

	return(0);
}
