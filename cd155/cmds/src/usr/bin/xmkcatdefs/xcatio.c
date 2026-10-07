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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
static char sccsid[] = "@(#)xcatio.c	1.4  com/cmd/msg,3.1,8943 9/20/89 10:58:47";
/*
 * COMPONENT_NAME: CMDMSG
 *
 * FUNCTIONS: descopen, descgets, descclose,descset, descerrck
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#include <stdio.h>

/*
 * EXTERNAL PROCEDURES CALLED: None
 */

FILE *descfil;

/*
 * NAME: descopen
 *
 * FUNCTION: Perform message utility input operations.
 *
 * EXECUTION ENVIRONMENT:
 *  	User mode.
 *
 * NOTES: The routines in this file are used to provide a mechanism by which
 *        the routines in gencat can get the next input character without
 *  	  worrying about details such as if the input is coming from a file
 *  	  or standard input, etc.
 *
 * RETURNS: 0 - successful open
 *         -1 - open failure
 */


/*   Open a catalog descriptor file and save file descriptor          */ 

descopen(filnam) 
			/*
			  filnam - pointer to message catalog name
			*/
  char *filnam;
{
	if ((descfil = fopen(filnam,"r")) == 0) 
		return (-1);
	
return (0);
}

/*
 * NAME: descgets
 *
 * FUNCTION: Reads message catalog.
 *
 * EXECUTION ENVIRONMENT:
 *  	User mode.
 *
 * NOTES: Read the next line from the open message catalog descriptor file.
 *
 * RETURNS: Pointer to message buffer -- scccessful
 *          NULL pointer -- error or end-of-file
 */


char *
descgets(buff, bufsize)
			/*
			  buff - pointer to message buffer
			  bufsize - size of message buffer in bytes
			*/
  char *buff;
  int bufsize;
{
	char *str;

	str = fgets(buff, bufsize, descfil);
	buff[bufsize-1] = '\0';        /* terminate in case length exceeded */
	return (str);
}

/*
 * NAME: descclose
 *
 * FUNCTION: Close message catalog
 *
 * EXECUTION ENVIRONMENT:
 *  	User mode.
 *
 * NOTES: Close the message catalog descriptor file.
 *
 * RETURNS: None
 */


descclose()
{
	fclose(descfil);
	descfil = 0;
}

/*
 * NAME: descset
 *
 * FUNCTION: Establish message catalog file descriptor
 *
 * EXECUTION ENVIRONMENT:
 *  	User mode.
 *
 * NOTES: Sets the file descriptor for message catalog access.
 *
 * RETURNS: None
 */


descset(infil)
  FILE *infil;
{
	descfil = infil;
}

/*
 * NAME: descerrck
 *
 * FUNCTION: Check I/O stream status
 *
 * EXECUTION ENVIRONMENT:
 *  	User mode.
 *
 * RETURNS: 0 - no error encountered.
 *         -1 - error encountered.
 */

descerrck()
{
	if (ferror(descfil))
		return(-1);
	else    return(0);
}
