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
static char rcsid[] = "@(#)$RCSfile: NLgetamsg.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:48 $";
#endif

/*
 * COMPONENT_NAME: LIBCMSG
 *
 * FUNCTIONS: NLgetamsg
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
 * sccsid[] = "NLgetamsg.c  1.13  com/lib/c/msg,3.1,9021 3/29/90 17:31:40";
 */

#include "catio.h"

/*                                                                   
 * EXTERNAL PROCEDURES CALLED: 	_do_open, _do_read_msg
 */


/*
 * NAME: NLgetamsg
 *                                                                    
 * FUNCTION: Opens a catalog, reads a message, and closes the catalog.
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *
 *	NLgetamsg executes under a process.
 *
 * RETURN VALUE DESCRIPTION: Returns a pointer to the default string on failure.
 *	Otherwise, it returns a pointer to the string that is retrieved.
 */  

char *NLgetamsg(char *cat, int setno, int msgno, char* def) 

	 /*---- cat: The name of the catalog to get the message from ----*/
 	 /*---- setno: The message set no. ----*/
	 /*---- msgno: The message no. ----*/
	 /*---- def: The default string ----*/

{
	int errno_save = errno;
	char *_do_read_msg();	/*---- The routine for non-mapped files ----*/
	nl_catd catd;		/*---- catd for the catalog ----*/
	char *mp;		/*---- pointer to the message text ----*/
	char *um;

	catd = catopen(cat, 0);

	if (catd == CATD_ERR) 
		RETURN(def);
	if (catd->_mem) {		/*---- for mapped files ----*/
		if (setno <= catd->_hd->_setmax) {
			if (msgno < catd->_set[setno]._n_msgs) {
				if (catd->_set[setno]._mp[msgno]._offset) {
					if (catd->_set[setno]._msgtxt[msgno]) {
						catd->_count = catd->_count - 1;
						RETURN(catd->_set[setno]._msgtxt[msgno]);
					}
					else {
						mp = catd->_set[setno]._msgtxt[msgno] 
                                                   = (char *)malloc(catd->_set[setno]._mp[msgno]._msglen + 1);
			                        strcpy(mp,catd->_mem + 
                                                       catd->_set[setno]._mp[msgno]._offset);
						catd->_count = catd->_count - 1;
						RETURN(mp);
					}
				}
			}
		}
		catd->_count = catd->_count - 1;
		RETURN(def);
	}
	else {				/*---- Catch for unmapped files ----*/
		um = _do_read_msg(catd,setno,msgno);
		catd->_count = catd->_count - 1;
		if (um == NULL)
			RETURN(def);
		else
		 	RETURN(um);
	}
}
