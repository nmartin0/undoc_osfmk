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
static char rcsid[] = "@(#)$RCSfile: method_s5fs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:46 $";
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysconfig.h>

#include "cm.h"


/*
 *      Export Static Method Entry Point
 */
extern int		SVFS_method();

/*
 *      Local BSS
 */
const static char 	s5fs_name[] = "Method s5fs";
int			s5fs_configured;	
int			s5fs_loaded;
kmod_id_t		s5fs_id;


#define	reterr(x)	do { \
			cm_log(logp, LOG_ERR, "%s: %s", s5fs_name, cm_msg(x)); \
			return(-1);	\
			} while(0)
/*
 *
 *	Name:		s5fs_method()
 *	Description:	System V file system Configuration Method 
 *	Returns:	Zero 		On success.
 *			Non-zero	On failure.
 *
 */

int
SVFS_method( cm_log_t * logp, ENT_t entry, cm_op_t op )
{
	int	rc;

	switch (op) {

	case CM_OP_LOAD:
		if (s5fs_loaded)
			reterr(KMOD_EALREADY);
		if ((rc=cm_kls_load(entry, &s5fs_id)) != 0)
			reterr(rc);
		s5fs_loaded = 1;
		break;

	case CM_OP_UNLOAD:
		if (!s5fs_loaded)
			reterr(KMOD_EEXIST);
		if ( s5fs_configured )
			reterr(KMOD_EBUSY);
		if ((rc=cm_kls_unload(s5fs_id)) != 0)
			reterr(rc);
		s5fs_id = LDR_NULL_MODULE;
		s5fs_loaded = 0;
		break;

	case CM_OP_CONFIGURE:
		if (!s5fs_loaded)
			reterr(KMOD_EEXIST);
		if (s5fs_configured)
			reterr(KMOD_EBUSY);
		if ((rc=s5fs_method_configure(logp, entry, s5fs_id)) != 0)
			reterr(rc);
		s5fs_configured = 1;
		break;

	case CM_OP_UNCONFIGURE:
		if (!s5fs_loaded)
			reterr(KMOD_EEXIST);
		if (!s5fs_configured)
			reterr(KMOD_ECEXIST);
		if ((rc=s5fs_method_unconfigure(logp, s5fs_id)) != 0)
			reterr(rc);
		s5fs_configured = 0;
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
s5fs_method_configure( cm_log_t * logp, ENT_t entry, kmod_id_t s5fs_id )
{
	int	rc;

	cm_log(logp, LOG_DEBUG, "%s: Configuring s5fs", s5fs_name);

        if ((rc=cm_kls_call(s5fs_id, SYSCONFIG_CONFIGURE, NULL, 0, 
		NULL, 0)) != 0) {
		return(rc);
	}
	cm_log(logp, LOG_DEBUG, "%s: Configured s5fs", s5fs_name);
	return(0);
}


/*
 *
 */
int
s5fs_method_unconfigure( cm_log_t * logp, kmod_id_t s5fs_id )
{
	int	rc;

        if ((rc=cm_kls_call(s5fs_id, SYSCONFIG_UNCONFIGURE, NULL, 0, 
		NULL, 0)) != 0) {
		return(rc);
	}
	cm_log(logp, LOG_DEBUG, "%s: Deconfigured s5fs", s5fs_name);
	return(0);
}
