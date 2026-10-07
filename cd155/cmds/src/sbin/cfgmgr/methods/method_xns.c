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
static char rcsid[] = "@(#)$RCSfile: method_xns.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:47 $";
#endif

#include <errno.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysconfig.h>
#include <netns/ns_config.h>

#include "cm.h"

/*
 *      Local BSS
 */
int			XNS_configured;
int			XNS_loaded;
kmod_id_t		XNS_id;


/*
 *
 *	Name:		XNS_method()
 *	Description:	Inet Configuration Method
 *	Returns:	Zero 		On success.
 *			Non-zero	On failure.
 *
 */
int
XNS_method( cm_log_t * logp, ENT_t entry, cm_op_t op, cm_op_t * rop )
{
	int	rc;

	rc = 0;
	if (op & CM_OP_LOAD) {
		rc = XNS_method_load(logp, entry);
		if (rc == 0) {
			*rop = CM_OP_LOAD;
			METHOD_LOG(LOG_INFO, MSG_LOADED);
		} else {
			METHOD_LOG(LOG_INFO, rc);
		}
	}

	if (op & CM_OP_CONFIGURE) {
		rc = XNS_method_configure(logp, entry);
		if (rc == 0) {
			*rop = CM_OP_CONFIGURE;
			METHOD_LOG(LOG_INFO, MSG_CONFIGURED);
		} else {
			METHOD_LOG(LOG_INFO, rc);
		}
	}

	if (op & CM_OP_UNCONFIGURE) {
		rc = XNS_method_unconfigure(logp, entry);
		if (rc == 0) {
			*rop = CM_OP_UNCONFIGURE;
			METHOD_LOG(LOG_INFO, MSG_UNCONFIGURED);
		} else {
			METHOD_LOG(LOG_INFO, rc);
		}
	}

	if (op & CM_OP_UNLOAD) {
		rc = XNS_method_unload(logp, entry);
		if (rc == 0) {
			*rop = CM_OP_UNLOAD;
			METHOD_LOG(LOG_INFO, MSG_UNLOADED);
		} else {
			METHOD_LOG(LOG_INFO, rc);
		}
	}
	return(rc == 0 ? 0 : -1);
}


/*
 *
 */
int
XNS_method_load( cm_log_t * logp, ENT_t entry )
{
	int	rc;

	if (XNS_loaded)
		return(KMOD_LOAD_L_EBUSY);
	if ((rc=cm_kls_load(entry, &XNS_id)) != 0)
		return(rc);
	XNS_loaded = 1;
	return(0);
}


/*
 *
 */
int
XNS_method_unload( cm_log_t * logp, ENT_t entry )
{
	int	rc;

	if (!XNS_loaded)
		return(KMOD_UNLOAD_L_EEXIST);
	if (XNS_configured)
		return(KMOD_UNLOAD_C_EBUSY);
	if ((rc=cm_kls_unload(XNS_id)) != 0)
		return(rc);
	XNS_id = LDR_NULL_MODULE;
	XNS_loaded = 0;
	return(0);
}


/*
 *
 */
int
XNS_method_configure( cm_log_t * logp, ENT_t entry )
{
	int	rc;

	if (!XNS_loaded)
		return(KMOD_CONFIG_L_EEXIST);
	if (XNS_configured)
		return(KMOD_CONFIG_C_EBUSY);
        if ((rc=cm_kls_call(XNS_id, SYSCONFIG_CONFIGURE, NULL, 0,
		NULL, 0)) != 0)
		return(rc);
	XNS_configured = 1;
	return(0);
}


/*
 *
 */
int
XNS_method_unconfigure( cm_log_t * logp, ENT_t entry )
{
	int	rc;

	if (!XNS_loaded)
		return(KMOD_UNCONFIG_L_EEXIST);
	if (!XNS_configured)
		return(KMOD_UNCONFIG_C_EEXIST);
        if ((rc=cm_kls_call(XNS_id, SYSCONFIG_UNCONFIGURE, NULL, 0,
		NULL, 0)) != 0) {
		return(rc);
	}
	XNS_configured = 0;
	return(0);
}

