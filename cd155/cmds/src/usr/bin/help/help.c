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
static char rcsid[] = "@(#)$RCSfile: help.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:26:03 $";
#endif

/*
 * COMPONENT_NAME: (CMDMAN) commands that allow users to read online
 * documentation 
 *
 * FUNCTIONS: none 
 *
 * ORIGINS: 27 
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * help.c	1.2  com/cmd/man,3.1,9021 9/14/89 06:22:46
 */

/*
 * help -- provides information for the new user. 
 *
 * format -- help 
 * 	It presents a one-page display of information for the new user.
 */

#include <stdio.h>
#include <locale.h>

#include "help_msg.h" 
nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_HELP,n,s) 

#define msg_HELP \
"\n\
The commands: \n\
    man -k keyword	lists commands relevant to a keyword \n\
    man command		prints out the manual pages for a command \n\
are helpful; other basic commands are: \n\
    cat			- concatenates files (and just prints them out) \n\
    vi			- text editor \n\
    finger		- user information lookup program \n\
    ls			- lists contents of directory \n\
    mail		- sends and receives mail \n\
    passwd		- changes login password \n\
    sccshelp		- views information on the Source Code Control System \n\
    tset		- sets terminal modes \n\
    who			- who is on the system \n\
    write		- writes to another user \n\
You could find programs about mail by the command: 	man -k mail \n\
and print out the man command documentation via:	man mail \n\
You can log out by typing \"exit\". \n\n"


/*
 *  main
 */
main()
{
	(void ) setlocale(LC_ALL,"");
	catd = catopen(MF_HELP,0);

	printf(MSGSTR(HELP, msg_HELP)); 
	exit(0);
}
