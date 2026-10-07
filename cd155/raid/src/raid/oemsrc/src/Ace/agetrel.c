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
 ***				 IDENTIFICATION				    ***
 ******************************************************************************
  Name:		$RCSfile: agetrel.c,v $
  Version:	$Id: agetrel.c,v 1.2.4.1 1995/06/11 22:19:35 kat Exp $
  Title:	Get OS Release
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:35 $
		(last change by: $Author: kat $)
  Programmer:	ach

  Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90

  COPYRIGHT 1992, NCR Corporation

  Description:	Gets the operating system release number.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: agetrel.c,v $
 * Revision 1.2.4.1  1995/06/11  22:19:35  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:11  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:42  richardg
 * Initial revision
 *
 * Revision 1.3  1992/05/29  12:55:11  root
 * sdpr v-119 - Added logic to take care of OS Rel 1.03
 *
 * Revision 1.2  1992/03/21  20:08:31  bmyers
 * Added correct 'cvs' header.
 *
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
		if ( ( rel <= 100 ) || ( rel >= 300 ) ) {
			ace_error(NON_SPECIFIC_ERROR, BAD_OS_REV_LEVEL);
		}
		return ( rel );
	}

	been_thru=TRUE;  /* mark been thru flag true so we wont repeat this */
#ifdef PARAGON860 /* force release level */
	rel = 299;
#else

	/* determine the release number */
	for (i = 0; i < 256; i++)
		rbuf[i] = 0x00;   /* initialize the buffer */

	if ((fd = open ("/etc/.relid", O_RDONLY)) == -1) {
		ace_error(NON_SPECIFIC_ERROR, BAD_RELID_OPEN);	/* could not read  */
		          				  	/* the .relid file */
	}		
	read (fd, rbuf, 128);

	ucp = (char *) strtok (rbuf, " ");		/* tokenize the buffer*/
	while (ucp != NULL) {				/* search for RELEASE */
		if (strncmp (ucp, "RELEASE", 7) == 0)
			break;
		ucp = (char *) strtok (NULL, " ");
	}	
	
	ucp = (char *) strtok (NULL, " ");		/* hopefully the next */
	rel = (( ucp[0] - 0x30) * 1000 ) +		/* token will be the  */
	      (( ucp[1] - 0x30) * 100 ) +		/* number we want */
	      (( ucp[2] - 0x30) * 10 ) +
	      (( ucp[3] - 0x30 ));

	close ( fd );

	if ((rel <= 100) || (rel >= 300)) {
		NULL;					    /*uh-oh we didn't */
		ace_error(NON_SPECIFIC_ERROR, BAD_OS_REV_LEVEL);/*get back rel 1  */
		         			            /*or rel 2        */
	}
#endif
	return(rel);
}
