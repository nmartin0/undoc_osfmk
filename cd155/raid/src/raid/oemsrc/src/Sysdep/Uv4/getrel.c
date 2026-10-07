/*
 * 
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		getrel.c
 Title:		Get Release
 Version:	   
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:35:38 $
 Programmer:	ach
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90


 COPYRIGHT 1991-92, NCR Corp.

 Description:	Gets the operating system release number.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include "getrel.h"

/******************************************************************************
 ***				  DEFINITIONS				    ***
 *****************************************************************************/

char rbuf[256];

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

u_long get_release ()
{
	u_long fd, i;
	static u_long rel, been_thru;
	char *ucp = rbuf;

	if (been_thru) {
		if ((rel <= 100) || (rel >= 300)) {
			error(NON_SPECIFIC_ERROR, BAD_OS_REV_LEVEL);
		}
		return (rel);
	}

	been_thru=TRUE;  /* mark been thru flag true so we wont repeat this */
#ifdef PARAGON860 /* force release level */
	rel = 299;
#else
	/* determine the release number */
	for (i = 0; i < 256; i++)
		rbuf[i] = 0x00;   /* initialize the buffer */

	if ((fd = open ("/etc/.relid", O_RDONLY)) == -1) {
		error(NON_SPECIFIC_ERROR, BAD_RELID_OPEN);/* could not read  */
		          				  /* the .relid file */
	}		
	read (fd, rbuf, 128);

	ucp = (char *) strtok (rbuf, " ");		/* tokenize the buffer*/
	while (ucp != NULL) {				/* search for RELEASE */
		if (strncmp (ucp, "RELEASE", 7) == 0)
			break;
		ucp = (char *) strtok (NULL, " ");
	}	
	
	ucp = (char *) strtok (NULL, " ");	/* hopefully the next */
	rel = ((ucp[0] - 0x30) * 1000) +        /* tokens will be the  */
	      ((ucp[1] - 0x30) * 100)  +	/* numbers we want */
	      ((ucp[2] - 0x30) * 10)  +
	      ((ucp[3] - 0x30));
	close (fd);
	if ((rel <= 100) || (rel >= 300)) {
		     					    /*uh-oh we didn't */
		error(NON_SPECIFIC_ERROR, BAD_OS_REV_LEVEL);/*get back the    */
	}	         			            /*supported rel   */
#endif
	return(rel);
}
