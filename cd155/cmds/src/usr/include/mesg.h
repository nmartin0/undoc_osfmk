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
/* @(#)mesg.h 1.11  com/inc,3.1,9013 2/28/90 09:08:25 */

/*
 * COMPONENT_NAME: INC
 *
 * FUNCTIONS: mesg.h
 *
 * ORIGINS: 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * (Copyright statements and/or associated legends of other
 * companies whose code appears in any part of this module must
 * be copied here.)
 */

/*                                                                   
 * EXTERNAL PROCEDURES CALLED: NLfcatgets, fcatgets, fcatgetmsg
 */

#ifndef _MESG_H_
#define _MESG_H_

#include <nl_types.h>
#include <limits.h>

#define CAT_MAGIC 	505
#define CATD_ERR 	((nl_catd) -1)
#define NL_MAXOPEN	10

nl_catd _do_open();

#ifdef _CAT_MACRO
#define NLcatgets(catd,setno,msgno,def) \
(((catd == NULL || catd == CATD_ERR || !catd->_mem)) ? NLfcatgets(catd,setno,msgno,def) : \
	(setno > catd->_setmax) ? def : \
	        (msgno >= catd->_set[setno]._n_msgs) ? def : \
		    	(!catd->_set[setno]._mp[msgno]._offset) ? def: \
				catd->_mem + catd->_set[setno]._mp[msgno]._offset)

#define catgets(catd,setno,msgno,def) \
(((catd == NULL || catd == CATD_ERR || !catd->_mem)) ? fcatgets(catd,setno,msgno,def) : \
	(setno > catd->_setmax) ? "" : \
	        (msgno >= catd->_set[setno]._n_msgs) ? "" : \
		    	(!catd->_set[setno]._mp[msgno]._offset) ? "": \
				catd->_mem + catd->_set[setno]._mp[msgno]._offset)

#define catgetmsg(catd,setno,msgno,buf,buflen) \
((catd == NULL || catd == CATD_ERR) ? "" : \
	(!catd->_mem) ? 	fcatgetmsg(catd,setno,msgno,buf,buflen) : \
			(setno > catd->_hd->_setmax) ? "" :\
			        (msgno >= catd->_set[setno]._n_msgs) ? "" : \
				   (!catd->_set[setno]._mp[msgno]._offset) ? "" : \
					(strncpy(buf, \
						(char *) (catd->_mem + catd->_set[setno]._mp[msgno]._offset),\
						buflen) , \
					buf))

#endif  /* _CAT_MACRO */

struct _message {
	unsigned short 	_set,
			_msg;
	char 		*_text;
	unsigned	_old;
};

struct _header {
	int 		_magic;
	unsigned short	_n_sets,
			_setmax;
	char 		_filler[20];
};
struct _catset {
	unsigned short 	_setno,
			_n_msgs;
	struct _msgptr 	*_mp;
	char	**_msgtxt;
};

#include <stdio.h>

struct catalog_descriptor {
	char		*_mem;
	char		*_name;
	FILE 		*_fd;
	struct _header 	*_hd;
	struct _catset 	*_set;
	int		_setmax;
	int		_count;
	int		_pid;
};


struct _msgptr {
	unsigned short 	_msgno,
			_msglen;
	unsigned long	_offset;
};
#endif  /* _MESG_H_ */
