// 
// $Copyright
// Copyright 1993, 1994, 1995  Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * Enable PFS emulator-based vnode caching for the specified PFS filesystem.
 * Call is valid one-time only per UNIX process. Subsequent calls return an
 * error. Caching for ONLY one PFS filesystem is supported.
 *
 * int pfs_host_init( char *pfs_filesystem )
 *
 * inputs:
 *	pfs_filesystem	PFS filesystem mount-point pathname (e.g., "/pfs").
 *
 * outputs:
 *	 0 == success, emulator vnode cache initialized
 *	!0 == error code.
 */
        .file   "pfs_host_init.s"

#include "SYS.h"

RSYSCALL(pfs_host_init)
