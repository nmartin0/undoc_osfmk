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
static char rcsid[] = "@(#)$RCSfile: catgets.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:50 $";
#endif

/*
 * COMPONENT_NAME: LIBCMSG
 *
 * FUNCTIONS: catgets
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
 * catgets.c 1.12  com/lib/c/msg,3.1,9021 3/29/90 17:32:10
 */


#include "catio.h"

#ifdef  _THREAD_SAFE
#include "rec_mutex.h"
extern  struct rec_mutex _catalog_rmutex;
#endif
/*                                                                   
 * EXTERNAL PROCEDURES CALLED: 	_do_open, _do_read_msg
 */


/*
 * NAME: catgets
 *                                                                    
 * FUNCTION: Gets a pointer to a message from a message catalog.
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *
 *	Catgets executes under a process.
 *
 * NOTES: In the case that nl_types.h is included in a program and
 *	a program is compiled with -D_CAT_MACRO, macro catgets() is
 *	used, and macro catgets() calls fcatgets().
 *	
 * RETURNS: Returns a pointer to the message on success.
 *	If the catd is invalid, the default string is returned.
 *	If the message or set number is invalid, a null string is returned.
 *
 */  

/*	
	This may be defined out because it is replaced by a macro.
*/

/*______________________________________________________________________
	The ifdef's around the function name are to provide a 'bailout'
	function in case the macro version runs into trouble.
  ______________________________________________________________________*/

#ifndef _CAT_MACRO
char *catgets(nl_catd catd,int setno,int msgno,char *def)
#else
char *fcatgets(nl_catd catd,int setno,int msgno,char *def)
#endif

	/*---- catd: the catd to get the message from ----*/
	/*---- setno: the set number of the message ----*/
	/*---- msgno: the message number of the message ----*/
	/*---- def: the default string to be returned ----*/

{
	int errno_save;
	char *_do_read_msg();
	char *m;

#ifdef  _THREAD_SAFE
#undef  RETURN
#define RETURN(s) \
        return(rec_mutex_unlock(&_catalog_rmutex), seterrno(errno_save), s)

        errno_save = geterrno();
#else
        errno_save = errno;
#endif
        if (catd == NULL || catd == CATD_ERR)
                return(def);
#ifdef  _THREAD_SAFE
        rec_mutex_lock(&_catalog_rmutex);
#endif
	if (!catd->_fd) {
		catd = _do_open(catd);
		if (catd == CATD_ERR) 
			RETURN(def);
	} else if (!_catisopen(catd))
		RETURN(def);

	if (catd->_mem) {	/*----  for mapped files ----*/
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
	else {	/*---- for unmapped files ----*/
		m = _do_read_msg(catd,setno,msgno);
                if(m == NULL)
                        RETURN(def);
                else
                        RETURN(m);
	}
}
