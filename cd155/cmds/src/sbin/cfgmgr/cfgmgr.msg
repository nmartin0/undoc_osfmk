$ 
$ $Copyright
$ Copyright 1993, 1994, 1995  Intel Corporation
$ INTEL CONFIDENTIAL
$ The technical data and computer software contained herein are subject
$ to the copyright notices; trademarks; and use and disclosure
$ restrictions identified in the file located in /etc/copyright on
$ this system.
$ Copyright$
$ 
 
$
$ (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
$ ALL RIGHTS RESERVED
$
$
$ OSF/1 Release 1.0

$ @(#)$RCSfile: cfgmgr.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:13 $

$quote 	"
$set MS_CFGMGR

$ 	Message strings used by cfgmgr
$
MSG_EXITING		"exiting"
MSG_RESETING		"reseting"
MSG_ALREADYRUNNING	"configuration manager already running"
MSG_MUSTBEROOT		"must be run as super-user"

MSG_CFGMGR	"usage: %s [-c database] [-R#] [-A#] [-d]\n"
MSG_SYSCONFIGDB	"usage: %s [-c database] [-f file] [-on|-off] -adlup name\n"
MSG_SYSCONFIG	"usage: %s [-v] [-s] -c|u|q subsystem\n"

MSG_SOCKET		"%s: socket: %s\n"
MSG_CONNECT		"%s: connect: %s\n"
MSG_WRITE		"%s: write: %s\n"
MSG_CFGMGR_NOTAVAIL	"%s: Configuration manager is not available.\n"
MSG_CFGMGR_NOSEND	"%s: Unable to send configuration manager request\n"

MSG_AF_ERRCATR		"%s: Stanza entry attribute list too long.\n"
MSG_AF_ERRCBUF		"%s: Stanza entry too long.\n"
MSG_AF_SYNTAX		"%s: Stanza entry syntax error encountered.\n"

MSG_AF_RENAME		"%s: Unable to update file from %s: %s\n"
MSG_AF_AUTOALREADY	"%s: Already in automatic list\n"
MSG_AF_AUTONOTALREADY	"%s: Not in automatic list\n"
MSG_AF_ENOEXIST		"%s: Entry not found in %s\n"
MSG_AF_EEXIST		"%s: Entry already exists in %s\n"
MSG_AF_INTERNAL		"%s: Internal error\n"
MSG_AF_DUP		"%s: Duplicate entries found\n"


$ 	Message strings used by cfgmgr in startup mode and request mode
$
MSG_ENTER_AUTOMODE	"%s: entering automatic configuration mode\n"
MSG_LEAVE_AUTOMODE	"%s: exiting automatic configuration mode\n"
MSG_AUTOMATIC		"%s: configuring automatic subsystem\n"
MSG_AUTOCONFIGURED	"%s: %d subsystems configured successfully, %d failed\n"
MSG_NOAUTOMATIC		"%s: no automatic subsystems configured\n"
MSG_DETACHING		"%s: detaching from parent session\n"
MSG_ENTER_REQUESTMODE	"%s: entering command request mode\n"
MSG_REQUESTING		"%s: configuring requested subsystem\n"
MSG_NOUNIXSOCK		"cannot open local AF_UNIX domain socket"
MSG_NOINETSOCK		"cannot open remote AF_INET domain socket"
MSG_NOSOCKS		"no communications sockets open"


$
$	Message strings used by cfgmgr during configuration
MSG_LOADING		"loading subsystem"
MSG_UNLOADING		"unloading subsystem"
MSG_CONFIGURING		"configuring subsystem"
MSG_UNCONFIGURING	"unconfiguring subsystem"
MSG_QUERYING		"querying subsystem"
MSG_CALLING		"calling subsystem configuration entry point"

MSG_LOADED		"successfully loaded subsystem"
MSG_UNLOADED		"successfully unloaded subsystem"
MSG_CONFIGURED		"successfully configured subsystem"
MSG_UNCONFIGURED	"successfully unconfigured subsystem"
MSG_QUERIED		"successfully queried subsystem"
MSG_CALLED		"successfully called subsystem configuration entry"


$ 	Errors returned by cfgmgr kernel loader server functions:
$		cm_kls_attach()
$		cm_kls_detach()
$		cm_kls_load()
$		cm_kls_unload()
$		cm_kls_call()
$
KLDR_EATTACH		"cannot attach to kernel load server"
KLDR_ELOAD		"kernel loader server can not load subsystem module"
KLDR_EUNLOAD		"kernel loader server can not unload subsystem module"
KLDR_ENOTKMOD		"subsystem module is not a dynamic subsystem"
KLDR_NOENT		"subsystem module missing entry point"
KLDR_EFAIL		"subsystem module configuration operation failed"


$ 	Errors returned by cfgmgr database manager functions:
$		dbfile_open_dflt()
$		dbfile_open()
$		dbfile_rewind()
$		dbfile_close()
$		dbent_lookup()
$		dbent_next()
$		dbattr_mkminor()
$		dbattr_mksfile()
$
DBFILE_ENOENT		"cannot open configuration database"
DBENT_ENOENT		"cannot find entry"
DBATR_ENOENT		"cannot find attribute"
DBPARSE_ENOMEM		"no more memory for parsing device names"
DBPARSE_EINVAL		"invalid syntax encountered in parsing string"
DBPARSE_E2BIG		"too many elements in parse string"
DBPARSE_ENOMATCH	"device name and minor number elements are not equal"


$ 	Errors returned by cfgmgr special device routines:
$		cm_make_nods()
$		cm_destroy_nods()
$		cm_destroy_sfile()
$
MKNODS_ENOENT		"no such device directory"
MKNODS_EINVAL		"invalid device file arguments"
MKNODS_ENOMEM		"not enough memory available to create device files"
MKNODS_EPERM		"cannot create device files subdirectory"
MKNODS_MAKING		"%s: creating device special files:\n"
MKNODS_REMOVING		"%s: removing device special files:\n"
MKNODS_CREATED		"%s: %-32.32s  %-3d/%-3d  created\n"
MKNODS_ECREATE		"%s: %-32.32s  %-3d/%-3d  failed to create\n"
MKNODS_REMOVED		"%s: %-32.32s removed\n"
MKNODS_EREMOVED		"%s: %-32.32s failed to remove\n"



$ 	Errors returned by cfgmgr method routines:
$		method_call()
$
METHOD_ELOAD		"failed to load subsystem configuration method"
METHOD_EUNLOAD		"failed to unload subsystem configuration method"
METHOD_EATTACH		"failed to attach to user space loader to load method"
METHOD_ENOENT		"failed to locate subsystem configuration method"
METHOD_EENTRY		"subsystem configuration method missing entry point"
METHOD_EFAIL		"subsystem configuration method failed"
METHOD_EINVAL		"unknown configuration method operation"
METHOD_ENOMEM 		"not enough memory to create method"


$ 	Errors returned by Configuration Methods.
$
KMOD_LOAD_L_EBUSY	"kernel subsystem is already loaded"

KMOD_UNLOAD_L_EEXIST	"kernel subsystem module is not currently loaded"
KMOD_UNLOAD_C_EBUSY	"kernel subsystem is currently configured"

KMOD_CONFIG_L_EEXIST	"kernel subsystem module is not currently loaded"
KMOD_CONFIG_C_EBUSY	"kernel subsystem is already configured"

KMOD_UNCONFIG_L_EEXIST 	"kernel subsystem module is not currently loaded"
KMOD_UNCONFIG_C_EEXIST	"kernel subsystem is not currently configured"

KMOD_EBUSY		"kernel subsystem is currently configured"
KMOD_EFAIL		"kernel subsystem configuration operation failed"
KMOD_EINVAL		"unknown kernel subsystem configuration operation"
KMOD_ENOMEM		"not enough memory available to perform operation"
KMOD_ENODEV		"no such device"
KMOD_ENOENT		"invalid entry for this subsystem type"

MSG_MAX			"message index exceeds table size"

