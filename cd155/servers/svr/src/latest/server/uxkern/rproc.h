/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * HISTORY
 * $Log: rproc.h,v $
 * Revision 1.4  1994/11/18  20:49:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:43:55  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:05:29  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:56:00  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:44:34  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:55:10  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1992/07/28  19:53:07  rabii
 * 	Initial chack-in of file containing rproc related structures and
 * 	functions definitions. (rabii)
 *
 */

#ifndef _RPROC_H_
#define _RPROC_H_

node_t	get_rproc_node();
#ifndef REMOTE_PROC
any_t                   init_default_pager();
#else   /* REMOTE_PROC */
any_t                   init_default_pager_rproc();
#endif  /* REMOTE_PROC */

struct rproc_list_entry {
	node_t				node;
	struct rproc_list_entry		*next;
};
#endif /* _RPROC_H_ */
