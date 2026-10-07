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
 * Copyright (c) 1988-1990 SecureWare, Inc.  All rights reserved.
 */

#ident "@(#)umount_sec.c	3.3 14:24:10 6/22/90 SecureWare"
/*
 * Based on:	@(#)umount_sec.c	2.10.1.1 11:08:47 2/3/90
 */

#include <sys/secdefines.h>
#include <sys/types.h>
#include <sys/security.h>
#include <sys/audit.h>
#include <stdio.h>
#include <errno.h>
#include <prot.h>
#include <string.h>

#ifdef AUX
#include <mntent.h>
#define NEW_MNTTAB	"/etc/mnttab-t"
#define OLD_MNTTAB	"/etc/mnttab-o"
#endif

#if SEC_BASE
extern priv_t *privvec();

/*
 * Check user's authorization to unmount filesystems.
 */
void
umount_checkauth()
{
	if (!authorized_user("mount"))  {
		fprintf(stderr, "%s: need mount authorization\n",
			command_name);
		exit(1);
	}
}


/*
 * Perform the unmount system call after raising the required privileges.
 */
#ifdef _OSF_SOURCE
umount_do_umount(path, flags)
	char	*path;
	int	flags;
#else
umount_do_umount(path)
	char	*path;
#endif
{
	int	ret;
	privvec_t	saveprivs;

	if (forceprivs(privvec(SEC_MOUNT,
#ifdef SEC_MAC
			       SEC_MULTILEVELDIR, 
#endif
			       -1), saveprivs))
	{
		fprintf(stderr, "%s: insufficient privileges\n", command_name);
		exit(1);
	}
	disablepriv(SEC_SUSPEND_AUDIT);

#ifdef _OSF_SOURCE
	ret = umount(path, flags);
#else
#ifdef AUX
	ret = unmount(path);
#else
	ret = umount(path);
#endif
#endif

	seteffprivs(saveprivs, (priv_t *) 0);
	return ret;
}

#ifndef _OSF_SOURCE
/*
 * allow rewriting of mount table regardless of invoker's sensitivity
 * level or discretionary identity.
 */

FILE *
umount_new_mnttab(file, mode)
	char	*file;
	char	*mode;
{
	int	ret;
	FILE	*fp;
	privvec_t	saveprivs;

	if (forceprivs(privvec(SEC_OWNER, SEC_CHOWN, SEC_ALLOWDACACCESS,
#if SEC_MAC
				SEC_ALLOWMACACCESS,
#endif
#if SEC_NCAV
				SEC_ALLOWNCAVACCESS,
#endif
				-1, saveprivs))) {
		fprintf(stderr, "%s: insufficient privileges\n", command_name);
		exit(1);
	}

	ret = create_file_securely(NEW_MNTTAB, AUTH_VERBOSE,
			"create temporary file for rewrite of mount table");
	if (ret != CFS_GOOD_RETURN) {
		fprintf (stderr, "Cannot create %s\n", NEW_MNTTAB);
		exit(2);
	}
	/* ensuing setmntent should refer to OUR temporary file */
	strcpy(file, NEW_MNTTAB);
	fp = fopen(NEW_MNTTAB, mode);
	seteffprivs(saveprivs, (priv_t *) 0);
	return(fp);
}

/*
 * open the mount table for writing, regardless of invoker's sensitivity
 * level or discretionary identity.
 */

FILE *
umount_setmntent(file, mode)
	char	*file;
	char	*mode;
{
	FILE	*fp;
	privvec_t	saveprivs;

	if (forceprivs(privvec(SEC_ALLOWDACACCESS,
#if SEC_MAC
				SEC_ALLOWMACACCESS,
#endif
#if SEC_NCAV
				SEC_ALLOWNCAVACCESS,
#endif
				-1, saveprivs))) {
		fprintf(stderr, "%s: insufficient privileges\n", command_name);
		exit(1);
	}

	fp = setmntent(file, mode);
	seteffprivs(saveprivs, (priv_t *) 0);
	return(fp);
}

/* 
 * replace the old mount table with the new one just built.
 * use the rename call, if it exists on the base system.
 */
umount_rename(nfile, ofile)
	char	*nfile;
	char	*ofile;
{
	int	save_error = 0, ret = 0;
	char	*oldfile, *tempfile;
	extern int	errno;
	privvec_t	saveprivs;

	if (forceprivs(privvec(SEC_ALLOWDACACCESS,
#if SEC_MAC
				SEC_ALLOWMACACCESS,
#endif
#if SEC_NCAV
				SEC_ALLOWNCAVACCESS,
#endif
				-1, saveprivs))) {
		fprintf(stderr, "%s: insufficient privileges\n", command_name);
		exit(1);
	}
	
#ifdef AUX
	ret = rename(NEW_MNTTAB, ofile);
#else
	oldfile = strdup(OLD_MNTTAB);
	tempfile = strdup(NEW_MNTTAB);
	if (!replace_file(tempfile, ofile, oldfile)) {
		errno = EACCES;
		ret = -1;
	}
#endif
	seteffprivs(saveprivs, (priv_t *) 0);
	return(ret);
}
#endif /* !_OSF_SOURCE */
#endif SEC_BASE
