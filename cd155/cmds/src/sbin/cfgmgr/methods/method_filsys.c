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
static char rcsid[] = "@(#)$RCSfile: method_filsys.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:41 $";
#endif

#include <errno.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysconfig.h>
#include <sys/mount.h>

#include "cm.h"

#define	MAXFSNAMSZ	32
typedef struct {
	char 		fs_name[MAXFSNAMSZ +1];
	uint		fs_loaded;
	uint		fs_configured;
	kmod_id_t	fs_id;
	filesys_config_t fs_outadm;
} fs_mod_t;

/*
 *      Local BSS
 */
#define		MAXFILESYS	20
fs_mod_t	fs_mod_list[MAXFILESYS];


/*
 *
 *	Name:		FILSYS_method()
 *	Description:	Generic OSF/1 FILSYS Configuration Method
 *	Returns:	Zero 		On success.
 *			Non-zero	On failure.
 *
 */
int
FILSYS_method( cm_log_t * logp, ENT_t entry, cm_op_t op, cm_op_t * rop )
{
	int		rc;
	fs_mod_t *	fs_mod;

	if (rc=FILSYS_lookup_fs_mod(entry, &fs_mod)) {
		METHOD_LOG(LOG_ERR, rc);
		return(-1);
	}

	rc = 0;
	if (op & CM_OP_LOAD) {
		rc = FILSYS_method_load(logp, entry, fs_mod);
		if (rc == 0) {
			*rop = CM_OP_LOAD;
			METHOD_LOG(LOG_INFO, MSG_LOADED);
		} else {
			METHOD_LOG(LOG_INFO, rc);
		}
	}

	if (op & CM_OP_CONFIGURE) {
		rc = FILSYS_method_configure(logp, entry, fs_mod);
		if (rc == 0) {
			*rop = CM_OP_CONFIGURE;
			METHOD_LOG(LOG_INFO, MSG_CONFIGURED);
		} else {
			METHOD_LOG(LOG_INFO, rc);
		}
	}

	if (op & CM_OP_UNCONFIGURE) {
		rc = FILSYS_method_unconfigure(logp, entry,fs_mod);
		if (rc == 0) {
			*rop = CM_OP_UNCONFIGURE;
			METHOD_LOG(LOG_INFO, MSG_UNCONFIGURED);
		} else {
			METHOD_LOG(LOG_INFO, rc);
		}
	}

	if (op & CM_OP_UNLOAD) {
		rc = FILSYS_method_unload(logp, entry, fs_mod);
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
FILSYS_method_load( cm_log_t * logp, ENT_t entry, fs_mod_t * fs_mod )
{
	int	rc;

	if (fs_mod->fs_loaded)
		return(KMOD_LOAD_L_EBUSY);
#ifndef TEST
	if ((rc=cm_kls_load(entry, &fs_mod->fs_id)) != 0)
		return(rc);
#endif
	fs_mod->fs_loaded = 1;
	return(0);
}


/*
 *
 */
int
FILSYS_method_unload( cm_log_t * logp, ENT_t entry, fs_mod_t * fs_mod )
{
	int	rc;

	if (!fs_mod->fs_loaded)
		return(KMOD_UNLOAD_L_EEXIST);
	if (fs_mod->fs_configured)
		return(KMOD_UNLOAD_C_EBUSY);
#ifndef TEST
	if ((rc=cm_kls_unload(fs_mod->fs_id)) != 0)
		return(rc);
#endif
	fs_mod->fs_id = LDR_NULL_MODULE;
	fs_mod->fs_loaded = 0;
}


/*
 *
 */
int
FILSYS_method_configure( cm_log_t * logp, ENT_t entry, fs_mod_t * fs_mod )
{
	int	rc;

	if (!fs_mod->fs_loaded)
		return(KMOD_CONFIG_L_EEXIST);
	if (fs_mod->fs_configured)
		return(KMOD_CONFIG_C_EBUSY);
#ifndef TEST
        if ((rc=cm_kls_call(fs_mod->fs_id, SYSCONFIG_CONFIGURE,
	NULL, 0, &(fs_mod->fs_outadm), sizeof(filesys_config_t))) != 0) {
		if (rc == KLDR_EFAIL)
			cm_log(logp, LOG_ERR, "%s: %s\n", AFentname(entry),
				strerror(errno));
		return(rc);
	}
#endif
	FILSYS_prtcfg(logp, entry, fs_mod, "configured");
	fs_mod->fs_configured = 1;
	return(0);
}


/*
 *
 */
int
FILSYS_method_unconfigure( cm_log_t * logp, ENT_t entry, fs_mod_t * fs_mod )
{
	int	rc;

	if (!fs_mod->fs_loaded)
		return(KMOD_UNCONFIG_L_EEXIST);
	if (!fs_mod->fs_configured)
		return(KMOD_UNCONFIG_C_EEXIST);
#ifndef TEST
        if ((rc=cm_kls_call(fs_mod->fs_id, SYSCONFIG_UNCONFIGURE,
		NULL, 0, NULL, 0)) != 0) {
		if (rc == KLDR_EFAIL)
			cm_log(logp, LOG_ERR, "%s: %s\n", AFentname(entry),
				strerror(errno));
		return(rc);
	}
#endif
	FILSYS_prtcfg(logp, entry, fs_mod, "deconfigured");
	fs_mod->fs_configured = 0;
	return(0);
}


/*
 *
 */
int
FILSYS_prtcfg(cm_log_t * logp, ENT_t entry, fs_mod_t * fs_mod, char * string)
{
	if (fs_mod->fs_outadm.fc_version == OSF_FILESYS_CONFIG_10) {
		cm_log(logp, LOG_ERR, "%s: %s as type %d\n", 
				AFentname(entry), string,
				fs_mod->fs_outadm.fc_type);
	} else {
		cm_log(logp, LOG_ERR, "%s: %s\n", AFentname(entry), string);
	}
}

int
FILSYS_lookup_fs_mod( ENT_t entry, fs_mod_t ** p)
{
	char *	entname;
	int 	rc;
	int	i;

	if ((entname=AFentname(entry)) == NULL
		|| (i=strlen(entname)) > MAXFSNAMSZ || i < 1)
		return(KMOD_ENOENT);


	for(i=0; i < MAXFILESYS; i++) {
		if (fs_mod_list[i].fs_name[0] == '\0')
			continue;
		if (!strcmp(fs_mod_list[i].fs_name, entname)) {
			*p = &fs_mod_list[i];
			return(0);
		}
	}
	for(i=0; i < MAXFILESYS; i++) {
		if (fs_mod_list[i].fs_name[0] == '\0') {
			strncpy(fs_mod_list[i].fs_name, entname, MAXFSNAMSZ);
			fs_mod_list[i].fs_loaded = 0;
			fs_mod_list[i].fs_configured = 0;
			fs_mod_list[i].fs_id = LDR_NULL_MODULE;
			bzero((char *)&fs_mod_list[i].fs_outadm,
				(sizeof(filesys_config_t)));
			*p = &fs_mod_list[i];
			return(0);
		}
	}
	return(KMOD_ENOMEM);
}

