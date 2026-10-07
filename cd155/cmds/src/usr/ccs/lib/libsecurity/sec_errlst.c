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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
/*
 * Copyright (c) 1990, SecureWare, Inc.
 *   All rights reserved.
 *
 * Security error message list.
 */

#ident "@(#)sec_errlst.c	1.1 22:33:11 5/28/90 SecureWare"

#include "libsecurity.h"

char *sys_secerrlist[] = {
    /*  0 */	"Security error 0",
    /*  1 */	"MAC configuration file error",
    /*  2 */	"Operation fails on wildcard tag",
    /*  3 */	"ACL configuration file error",
    /*  4 */	"Subject has wildcard sensitivity label",
    /*  5 */	"Directory has wildcard sensitivity label",
    /*  6 */	"Subject and object sensitivity labels not equal",
    /*  7 */	"Privilege required for operation",
    /*  8 */	"No decision available from daemon",
    /*  9 */	"Subject sensitivity label does not dominate",
    /* 10 */	"Object sensitivity label does not dominate",
    /* 11 */	"Setting sensitivity but clearance not set",
    /* 12 */	"Setting sensitivity label to strictly dominate clearance",
    /* 13 */	"Cannot change other MAC labels if sensitivity label not set",
    /* 14 */	"Cannot raise clearance",
    /* 15 */	"Violation of sensitivity/information label relationship",
    /* 16 */	"Violation of increasing tree",
    /* 17 */	"Need write access for operation",
    /* 18 */	"Must be owner of object",
    /* 19 */	"Operation invalid on multilevel directory",
    /* 20 */	"Operation invalid on non-empty directory",
    /* 21 */	"Cannot combine information labels",
    /* 22 */	"Not extended filesystem format",
    /* 23 */	"File is not regular file",
    /* 24 */	"Cannot set file attributes",
    /* 25 */	"Cannot map internal representation to tag",
    /* 26 */	"Nationality caveat configuration file error",
    /* 27 */	"Directory is not multilevel",
    /* 28 */    "Operation invalid on extended format filesystem"
};

#ifdef MSG
int sys_secerrnums[] = {
    /*  0 */	MSESEC_NOERROR,
    /*  1 */	MSESEC_MAC_CONFIG_FAILURE,
    /*  2 */	MSESEC_WILDCARD_TAG,
    /*  3 */	MSESEC_ACL_CONFIG_FAILURE,
    /*  4 */	MSESEC_WILD_SUBJ_SL,
    /*  5 */	MSESEC_WILD_DIR_SL,
    /*  6 */	MSESEC_MAC_NOT_EQUAL,
    /*  7 */	MSESEC_MAC_NEED_PRIVILEGE,
    /*  8 */	MSESEC_NO_DECISION,
    /*  9 */	MSESEC_MAC_SDOM,
    /* 10 */	MSESEC_MAC_ODOM,
    /* 11 */	MSESEC_MAC_NO_CLEARANCE,
    /* 12 */	MSESEC_MAC_DOM_CLEARANCE,
    /* 13 */	MSESEC_MAC_NO_SL,
    /* 14 */	MSESEC_MAC_UP_CLEARANCE,
    /* 15 */	MSESEC_MAC_IL_RELATION,
    /* 16 */	MSESEC_MAC_FS_RELATION,
    /* 17 */	MSESEC_MAC_NO_WRITE,
    /* 18 */	MSESEC_NOT_OWNER,
    /* 19 */	MSESEC_IS_MLD,
    /* 20 */	MSESEC_DIR_NOT_EMPTY,
    /* 21 */	MSESEC_MAC_IL_NOCOMB,
    /* 22 */	MSESEC_UNEXT_FS,
    /* 23 */	MSESEC_NOT_REG,
    /* 24 */	MSESEC_SETATTR_FAIL,
    /* 25 */	MSESEC_INVALID_IR,
    /* 26 */	MSESEC_NCAV_CONFIG_FAILURE,
    /* 27 */	MSESEC_NOT_MLD,
    /* 28 */    MSESEC_EXT_FS
};
#endif

int sys_nsecerr = sizeof(sys_secerrlist) / sizeof(sys_secerrlist[0]);
