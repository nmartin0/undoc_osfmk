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
static char rcsid[] = "@(#)$RCSfile: fcatgetmsg.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:37 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 

/*
 * COMPONENT_NAME: LIBCMSG
 *
 * FUNCTIONS: fcatgetmsg
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
 *
 * sccsid[] = "fcatgetmsg.c      1.9  com/lib/c/msg,3.1,9021 3/29/90 17:33:19";
 */

/*                                                                   
 * EXTERNAL PROCEDURES CALLED: 	_do_open, _do_read_msg
 */


/*
 * NAME: fcatgetmsg
 *                                                                    
 * FUNCTION: Gets a message from a catalog and copies it into a buffer.
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *
 *	Executes under a process.
 *
 * NOTES: In the case that nl_types.h is included in a program and
 *	a program is compiled with -DCAT_MACRO, macro catgetmsg() is
 *	used, and macro catgetmsg() calls fcatgetmsg().
 *	
 * RETURN VALUE DESCRIPTION: Returns a pointer to the buffer on success or
 *	a pointer to a NULL string on failure.
 *
 */  

#define _CAT_MACRO
#include "catio.h"

/*______________________________________________________________________
	The ifdef's around the function name are to provide a 'bailout'
	for the macro version if it runs into trouble.
  ______________________________________________________________________*/

char *fcatgetmsg(nl_catd catd, int setno,int msgno,char *buf,int buflen)

	/*---- catd: the catd to be read from ----*/
 	/*---- setno: the set number of the message ----*/
	/*---- msgno: the message number to be read ----*/
	/*---- buf: the buffer to store the message ----*/
	/*---- buflen: the maximum length of the message ----*/


{
	int errno_save = errno;
	char *_do_read_msg();
	char *m;

	buf[0] = NULL;

	if (catd == NULL || catd == CATD_ERR) 
		RETURN(buf);
	if (!catd->_fd)
		catd = _do_open(catd);
	if (catd == CATD_ERR)
		RETURN(buf);
	if (catd->_mem) {	/*---- for mapped files ----*/
		if (setno <= catd->_hd->_setmax) {
			if (msgno < catd->_set[setno]._n_msgs) {
				if (catd->_set[setno]._mp[msgno]._offset) {
					NLstrncpy(buf, catd->_mem +
                                   catd->_set[setno]._mp[msgno]._offset,buflen);
					RETURN(buf);
				}
			}
		}
		RETURN(buf);
	}
	else {	/*---- for unmapped files ----*/
		m = _do_read_msg(catd, setno,msgno);
		if (m != NULL)
			strcpy(buf, m);
		RETURN(buf);
	}
}
