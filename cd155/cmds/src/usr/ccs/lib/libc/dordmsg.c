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
static char rcsid[] = "@(#)$RCSfile: dordmsg.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:19 $";
#endif

/*
 * COMPONENT_NAME: LIBCMSG
 *
 * FUNCTIONS: _do_read_msg
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
 * sccsid[] = "dordmsg.c  1.6  com/lib/c/msg,3.1,9021 3/29/90 17:33:48";
 */


#include "catio.h"
/*                                                                   
 * EXTERNAL PROCEDURES CALLED: fread, fseek
 */


/*
 * NAME: _do_read_msg
 *                                                                    
 * FUNCTION: Reads a message from an unmapped file and places the text in a 
 *	static buffer.
 *
 * EXECUTION ENVIRONMENT:
 *	Executes under a process.
 *
 * RETURNS: NULL if the set or message is bad and if the read fails.
 *	Pointer to message if success.
 *
 */  

char *_do_read_msg(nl_catd catd,int setno,int msgno) 

	/*---- catd: the catd of the catalog to be read from ----*/
	/*---- setno: the set number of the message ----*/
	/*---- msgno: the msgno of the message ----*/

{
        nl_catd catd1;          /*--- catd for different process ----*/
	char *_read_msg();

	if (getpid() == catd->_pid)
		return (_read_msg (catd, setno, msgno));
	else {
        	if ((catd1 = catopen(catd->_name, 0)) != CATD_ERR)
           		return (_read_msg(catd1,setno,msgno));
        	else
           		return(NULL);
	}
}
	


char *_read_msg(nl_catd catd,int setno,int msgno)

{ 
        struct _msgptr *mpt;    /*--- temporary pointer to the message
                                      array for speed.  ----*/
        char *mt;               /*--- temporary pointer to the message text
                                      to for speed.  ----*/
     
	if (setno <= catd->_hd->_setmax) {
		if (msgno < catd->_set[setno]._n_msgs) {
			mpt = &catd->_set[setno]._mp[msgno];
			if (mpt->_offset) {
				if (!catd->_set[setno]._msgtxt[msgno]) {
					if (!(mt = catd->_set[setno]._msgtxt[msgno] = 
					     (char *)malloc(mpt->_msglen + 1)))
				 	     return(NULL);
					fseek(catd->_fd,(long)mpt->_offset,0); 
					if (fread((void *)mt,(size_t)(mpt->_msglen+1),(size_t)1, catd->_fd) != 1) 
						return(NULL);
					return(mt);
				}
				else {
					return(catd->_set[setno]._msgtxt[msgno]);
				}
			}
		}
	}
	return(NULL);
}
