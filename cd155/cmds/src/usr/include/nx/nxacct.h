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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/nxacct.h,v 1.3 1994/11/19 02:44:49 mtm Exp $
 *
 * HISTORY
 * $Log: nxacct.h,v $
 * Revision 1.3  1994/11/19  02:44:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/01  17:56:27  rkl
 * Changed external declarations for nx_setaent() and nx_endaent()
 * to void to match library fucntions.
 *
 * Revision 1.1  1992/10/01  17:31:59  rkl
 * Initial revision
 *
 *
 */
#include <sys/types.h>

typedef gid_t	acct_t;

struct nxacct {
	char	*acct_name;     /* name of the group */
	char	*acct_passwd;   /* encrypted password of the group */
	acct_t	acct_id;        /* numerical group ID */
	char	**acct_mem;     /* null-terminated array of pointers
				   to the individual member names */
};

extern	struct nxacct*	nx_getaent();
extern	int		nx_getaent_r();
extern	struct nxacct*	nx_getaid();
extern	int		nx_getaid_r();
extern	struct nxacct*	nx_getanam();
extern	int		nx_getanam_r();
extern	void		nx_setaent();
extern	int		nx_setaent_r();
extern	void		nx_endaent();
extern	int		nx_endaent_r();
extern	struct nxacct*	nx_fgetgrent();
extern	int		nx_fgetgrent_r();
