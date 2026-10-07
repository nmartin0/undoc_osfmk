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
static char rcsid[] = "@(#)$RCSfile: method_nfs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:45 $";
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysconfig.h>

#include "cm.h"


/*
 *      Export Static Method Entry Point
 */
extern int              NFS_method();

/*
 *      Local BSS
 */
const static char 	nfs_name[] = "Method nfs";
int			nfs_configured;	
int			nfs_loaded;
kmod_id_t		nfs_id;

#define	reterr(x)	do { \
			cm_log(logp, LOG_ERR, "%s: %s", nfs_name, cm_msg(x)); \
			return(-1);	\
			} while (0)
/*
 *
 *	Name:		nfs_method()
 *	Description:	NFS Configuration Method 
 *	Returns:	Zero 		On success.
 *			Non-zero	On failure.
 *
 */
int
NFS_method( cm_log_t * logp, ENT_t entry, cm_op_t op ) 
{
	int	rc;

	switch (op) {

	case CM_OP_LOAD:
		if (nfs_loaded)
			reterr(KMOD_EALREADY);
		if ((rc=cm_kls_load(entry, &nfs_id)) != 0)
			reterr(rc);
		nfs_loaded = 1;
		break;

	case CM_OP_UNLOAD:
		if (!nfs_loaded)
			reterr(KMOD_EEXIST);
		if ( nfs_configured )
			reterr(KMOD_EBUSY);
		if ((rc=cm_kls_unload(nfs_id)) != 0)
			reterr(rc);
		nfs_id = LDR_NULL_MODULE;
		nfs_loaded = 0;
		break;

	case CM_OP_CONFIGURE:
		if (!nfs_loaded)
			reterr(KMOD_EEXIST);
		if (nfs_configured)
			reterr(KMOD_EBUSY);
		if ((rc=nfs_method_configure(logp, entry, nfs_id)) != 0)
			reterr(rc);
		nfs_configured = 1;
		break;

	case CM_OP_UNCONFIGURE:
		if (!nfs_loaded)
			reterr(KMOD_EEXIST);
		if (!nfs_configured)
			reterr(KMOD_ECEXIST);
		if ((rc=nfs_method_unconfigure(logp, nfs_id)) != 0)
			reterr(rc);
		nfs_configured = 0;
		break;

	case CM_OP_QUERY:
	default:
		reterr(KMOD_EINVAL);
	}
	return(0);
}


/*
 *
 */
int
nfs_method_configure( cm_log_t * logp, ENT_t entry, kmod_id_t nfs_id )
{
	int	rc;

	cm_log(logp, LOG_DEBUG, "%s: Configuring nfs", nfs_name);

        if ((rc=cm_kls_call(nfs_id, SYSCONFIG_CONFIGURE, NULL, 0, 
		NULL, 0)) != 0) {
		return(rc);
	}
	cm_log(logp, LOG_DEBUG, "%s: Configured nfs", nfs_name);
	return(0);
}


/*
 *
 */
int
nfs_method_unconfigure( cm_log_t * logp, kmod_id_t nfs_id )
{
	int	rc;

        if ((rc=cm_kls_call(nfs_id, SYSCONFIG_UNCONFIGURE, NULL, 0, 
		NULL, 0)) != 0) {
		return(rc);
	}
	cm_log(logp, LOG_DEBUG, "%s: Deconfigured nfs", nfs_name);
	return(0);
}
