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
/*
 * Copyright (c) 1988-1990 SecureWare, Inc.  All Rights Reserved.
 */

#ident "@(#)data.c	3.2 10:04:14 6/13/90 SecureWare"
/*
 * Based on:
 *   "@(#)data.c	2.4.1.2 17:08:05 12/28/89 SecureWare"
 */

/*LINTLIBRARY*/


/*
 * This file contains initialized global data for use in libprot.a .
 */

#include <sys/secdefines.h>

#if SEC_BASE

#include <sys/types.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>


struct namepair secclass[] = {
	"d",			AUTH_D,
	"c1",			AUTH_C1,
	"c2",			AUTH_C2,
	"b1",			AUTH_B1,
	"b2",			AUTH_B2,
	"b3",			AUTH_B3,
	"a1",			AUTH_A1,
	(char *) 0,		0
};



struct namepair sys_priv[] = {
	"suspendaudit",		SEC_SUSPEND_AUDIT,
	"configaudit",		SEC_CONFIG_AUDIT,
	"writeaudit",		SEC_WRITE_AUDIT,
	"execsuid",		SEC_EXECSUID,
	"chmodsugid",		SEC_CHMODSUGID,
	"chown",		SEC_CHOWN,
	"acct",			SEC_ACCT,
	"limit",		SEC_LIMIT,
	"lock",			SEC_LOCK,
	"linkdir",		SEC_LINKDIR,
	"mknod",		SEC_MKNOD,
	"mount",		SEC_MOUNT,
	"sysattr",		SEC_SYSATTR,
	"setprocident",		SEC_SETPROCIDENT,
	"chroot",		SEC_CHROOT,
	"debug",		SEC_DEBUG,
	"shutdown",		SEC_SHUTDOWN,
	"filesys",		SEC_FILESYS,
	"remote",		SEC_REMOTE,
	"kill",			SEC_KILL,
	"owner",		SEC_OWNER,
	"chpriv",		SEC_CHPRIV,
	"allowdacaccess",	SEC_ALLOWDACACCESS,
	"sucompat",		SEC_SUCOMPAT,
	"supropagate",		SEC_SUPROPAGATE,
#if SEC_MAC
	"downgrade",		SEC_DOWNGRADE,
	"writeupclearance",	SEC_WRITEUPCLEARANCE,
	"writeupsyshi",		SEC_WRITEUPSYSHI,
	"multileveldir",	SEC_MULTILEVELDIR,
	"allowmacaccess",	SEC_ALLOWMACACCESS,
#endif
#if SEC_ILB
	"allowilbaccess",	SEC_ALLOWILBACCESS,
	"ilnofloat",		SEC_ILNOFLOAT,
#endif
#if SEC_NCAV
	"allowncavaccess",	SEC_ALLOWNCAVACCESS,
#endif
	(char *) 0,		0
};

struct namepair *cmd_priv = (struct namepair *) 0;

struct namepair audit_mask[] = {
	"noevent",		ET_NOEVENT,
	"boot/down",		ET_BOOT_DOWN,
	"login",		ET_LOGIN,
	"process",		ET_PROCESS,
	"ob_available",		ET_OBJECT_AVAIL,
	"ob_map",		ET_OBJECT_MAP,
	"ob_modify",		ET_OBJECT_MOD,
	"ob_unavailable",	ET_OBJECT_UNAV,
	"ob_create",		ET_OBJECT_CREAT,
	"ob_delete",		ET_OBJECT_DEL,
	"dac_chg",		ET_DAC_CHANGE,
	"access_denial",	ET_ACCESS_DENIAL,
	"sysadm",		ET_SYS_ADMIN,
	"insuff_priv",		ET_INSUFF_PRIV,
	"rsc_denial",		ET_RES_DENIAL,
	"ipc",			ET_IPC,
	"proc_mod",		ET_PROCESS_MOD,
	"audit",		ET_AUDIT,
	"subsystem",		ET_SUBSYSTEM,
	"privilege",		ET_PRIVILEGE,
	"authorization",	ET_AUTHORIZATION,
	"seclevel",		ET_SEC_LEVEL,
	"window",		ET_WINDOW_SYSTEM,
	(char *) 0,		0
};


struct namepair auth_dev_type[] = {
{ "printer", AUTH_DEV_PRINTER },
{ "terminal", AUTH_DEV_TERMINAL },
{ "tape", AUTH_DEV_TAPE },
{ "remote", AUTH_DEV_REMOTE },
{ (char *) 0, 0 }
};


#if SEC_ARCH

struct namepair auth_dev_assign[] = {
#if SEC_MAC
 { "single",  AUTH_DEV_SINGLE },
 { "multi",   AUTH_DEV_MULTI },
#endif
 { "label",   AUTH_DEV_LABEL },
 { "nolabel", AUTH_DEV_NOLABEL },
 { "import",  AUTH_DEV_IMPORT },
 { "export",  AUTH_DEV_EXPORT },
#if SEC_ILB
 { "singleil",  AUTH_DEV_ILSINGLE },
 { "multiil",   AUTH_DEV_ILMULTI },
#endif
#if SEC_NCAV
 { "singlencav",  AUTH_DEV_NCAVSINGLE },
 { "multincav",   AUTH_DEV_NCAVMULTI },
#endif
 { (char *) 0, 0 }
};

#endif /* SEC_ARCH */

#endif /* SEC_BASE */
