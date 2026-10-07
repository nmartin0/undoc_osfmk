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
static char rcsid[] = "@(#)$RCSfile: NLcatgets.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:25 $";
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
 * FUNCTIONS: NLcatgets
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
 * sccsid[] = "NLcatgets.c       1.11  com/lib/c/msg,3.1,9021 3/29/90 17:29:39";
 */

/*                                                                   
 * EXTERNAL PROCEDURES CALLED:	_do_open, _do_read_msg
 */


/*
 * NAME: NLcatgets
 *                                                                    
 * FUNCTION: Gets a string message from a catalog.
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *
 *	NLcatgets executes under a process.
 *
 * RETURNS: Returns a pointer to the string if the retrieval
 *	is successful. Otherwise, it returns the 'def' string
 *	if the catalog was not opened or it returns a pointer to a 
 *	null string if the set or message number was out of bounds.
 */  

#include "catio.h"

/*______________________________________________________________________
	The ifdef on the function name are used so that if macros are used,
	it has a 'bailout' for difficult situations.
  ______________________________________________________________________*/


#ifndef _CAT_MACRO
char *NLcatgets(nl_catd catd,int setno,int msgno, char *def)
#else 			
char *NLfcatgets(nl_catd catd,int setno,int msgno, char *def)
#endif 	

{
	int errno_save = errno;
	char *_do_read_msg();
	char            *m;

	if (catd == NULL || catd == CATD_ERR || catd->_count == 0)
		RETURN(def);
	if (!catd->_fd)
		catd = _do_open(catd);
	if (catd == CATD_ERR)
		RETURN(def);
	if (catd->_mem) {
		if (setno <= catd->_hd->_setmax) {
			if (msgno < catd->_set[setno]._n_msgs) {
				if (catd->_set[setno]._mp[msgno]._offset) {
					RETURN(catd->_mem + 
                                        catd->_set[setno]._mp[msgno]._offset);
				}
			}
		}
		RETURN(def);
	}
	else {
		m = _do_read_msg(catd,setno,msgno);
		if (m == NULL)
			RETURN(def);
		else
		 	RETURN(m);
	}
}
