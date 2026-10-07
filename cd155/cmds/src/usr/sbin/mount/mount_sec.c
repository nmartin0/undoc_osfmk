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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: mount_sec.c,v $
 * Revision 1.3  1994/11/19  03:09:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/12  22:14:07  shala
 * New version to support maj, min and node numbers.
 *
 * Revision 1.3  90/10/07  22:21:18  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  15:29:30  gm]
 * 
 * Revision 1.2  90/07/27  11:29:16  devrcs
 * 	Security hooks from SecureWare.
 * 	[90/07/19  21:36:43  seiden]
 * 
 * 	Initial version from SecureWare.
 * 	[90/07/13  01:23:07  seiden]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1988-1990 SecureWare, Inc.  All rights reserved.
 */

#ident "@(#)mount_sec.c	3.2 10:03:35 6/24/90 SecureWare"
/*
 * Based on:	@(#)mount_sec.c	2.16.3.3 11:08:16 2/3/90
 */

/*
 * This file contains routines added to the mount command.
 */

#include <sys/secdefines.h>

#if SEC_BASE
#include <sys/security.h>
#include <prot.h>

#if SEC_ARCH
#include <sys/secpolicy.h>
#endif
#if SEC_MAC
#include <mandatory.h>
#endif
#if SEC_ACL
#include <acl.h>
#endif
#if SEC_NCAV
#include <ncav.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <errno.h>

#ifndef _OSF_SOURCE
#if defined(AUX) || defined(BSD) 
#include <mntent.h>
#else
#define MOUNTED "/etc/mnttab"
#endif
#endif

#if SEC_ARCH

static int	have_labels = 0;
static attr_t	attrs[SEC_TAG_COUNT];

#if SEC_ACL
static char	*acl_label = (char *) 0;
#define	MNTOPT_ACL	"acl"
#define MNTOPT_WILDCARD	"WILDCARD"
#endif

#if SEC_MAC
static char	*mand_label = (char *) 0;
#define	MNTOPT_MAC	"mac"
#endif

#if SEC_ILB
static char	*ilb_label = (char *) 0;
#define	MNTOPT_ILB	"ilb"
#endif

#if SEC_NCAV
static char	*ncav_label = (char *) 0;
#define	MNTOPT_NCAV	"ncav"
#endif

#endif /* SEC_ARCH */

extern priv_t	*privvec();

extern char	*strchr(), *strdup();
#ifdef AUX
extern FILE	*setmntent();
#endif

/*
 * Initialize the attribute array to send to the lmount() call prior to
 * setting the individual attributes.
 */
void
mount_init(argc, argv)
	int argc;
	char **argv;
{
#ifndef _OSF_SOURCE
	privvec_t saveprivs;
#endif

	set_auth_parameters(argc, argv);
	initprivs();

#ifndef _OSF_SOURCE
	/* create the mount table file if it does not exist */

	if (access(MOUNTED, 0) != -1)
		return;

	if (forceprivs(privvec(SEC_ALLOWDACACCESS, SEC_OWNER, SEC_CHOWN,
#if SEC_MAC
				SEC_ALLOWMACACCESS,
#endif
#if SEC_ILB
				SEC_ILNOFLOAT,
#endif
#if SEC_NCAV
				SEC_ALLOWNCAVACCESS,
#endif
				-1), saveprivs)) {
		fprintf(stderr, "%s: insufficient privileges\n", command_name);
		exit(1);
	}

	if (create_file_securely(MOUNTED, AUTH_VERBOSE, "mount table") !=
			CFS_GOOD_RETURN) {
		fprintf(stderr, "mount: can not create mount table\n");
		exit(1);
	}

	seteffprivs(saveprivs, (priv_t *) 0);
#endif /* !_OSF_SOURCE */
}

/*
 * Process SecureWare flags that provide global security attributes
 * for unlabeled filesystems.
 */
void
mount_optflag(c, label)
	int	c;
	char	*label;
{
	switch (c) {
#if SEC_ACL
	    case 'A':
		have_labels = 1;
		acl_label = strdup(label);
		break;
#endif
#if SEC_MAC
	    case 'S':
		have_labels = 1;
		mand_label = strdup(label);
		break;
#endif
#if SEC_ILB
	case 'I':
		have_labels = 1;
		ilb_label = strdup(label);
		break;
#endif
#if SEC_NCAV
	case 'C':
		have_labels = 1;
		ncav_label = strdup(label);
		break;
#endif
	}
}

/*
 * Clear out any labels that have been accumulated.  This function is
 * called before processing each filesystem when multiple filesystems
 * are being mounted (as with mount -a or mountall).
 */
mount_resetopts()
{
	have_labels = 0;
#if SEC_ACL
	if (acl_label) {
		free(acl_label);
		acl_label = NULL;
	}
#endif
#if SEC_MAC
	if (mand_label) {
		free(mand_label);
		mand_label = NULL;
	}
#endif
#if SEC_ILB
	if (ilb_label) {
		free(ilb_label);
		ilb_label = NULL;
	}
#endif
#if SEC_NCAV
	if (ncav_label) {
		free(ncav_label);
		ncav_label = NULL;
	}
#endif
}

/*
 * Parse label options as they appear in the filesystem table (/etc/fstab
 * or /etc/checklist).  In this format, no imbedded blanks, tabs or commas
 * are allowed within an individual label option. This means that most
 * labels specified in this format will have to be given as synonyms.
 * This restriction allows us to avoid having to rewrite vendor's library
 * functions that parse filesystem table entries.  To ensure the precedence
 * command-line specified options over those given in the file, we ignore
 * options containing label strings for policies for which we already have
 * label strings.
 */
void
mount_getopts(optstr)
	char	*optstr;
{
	register char	*cp, *nextcp, *ecp;

	for (cp = optstr; cp && *cp; cp = nextcp) {
		if (nextcp = strchr(cp, ','))
			*nextcp++ = '\0';
		if (ecp = strchr(cp, '=')) {
			*ecp++ = '\0';
#if SEC_ACL
			if (acl_label == NULL && strcmp(cp, MNTOPT_ACL) == 0) {
				have_labels = 1;
				acl_label = strdup(ecp);
			}
#endif
#if SEC_MAC
			if (mand_label == NULL && strcmp(cp, MNTOPT_MAC) == 0) {
				have_labels = 1;
				mand_label = strdup(ecp);
			}
#endif
#if SEC_ILB
			if (ilb_label == NULL && strcmp(cp, MNTOPT_ILB) == 0) {
				have_labels = 1;
				ilb_label = strdup(ecp);
			}
#endif
#if SEC_ILB
			if (ncav_label == NULL && strcmp(cp, MNTOPT_NCAV)==0) {
				have_labels = 1;
				ncav_label = strdup(ecp);
			}
#endif
			ecp[-1] = '=';
		}
		if (nextcp)
			nextcp[-1] = ',';
	}
}

/*
 * Determine if the user is authorized to run mount.
 * The mount authorization is always required, and if
 * global attributes are supplied, isso authorization
 * is also checked.
 */
mount_checkauth()
{
	/*
	 * Check the required authorizations.
	 */

	if (!authorized_user("mount")) {
		fprintf(stderr, "mount: need mount authorization.\n");
		exit(2);
	}

	if (have_labels && !authorized_user("isso")) {
		fprintf(stderr, "mount: need isso authorization.\n");
		exit(2);
	}
}

/*
 * Set up the array of security attributes for the filesystem
 * that is about to be mounted.
 */
void
mount_setattrs()
{
	register char	*ir;
	register int	which;
	int		length;

	if (!have_labels)
		return;

	for (which = 0; which < SEC_TAG_COUNT; ++which)  {
		attrs[which].code = 0;
		attrs[which].ir_length = 0L;
		attrs[which].ir = (char *) 0;
	}

#if SEC_ACL
	if (acl_label) {
		if (acl_init()) {
			fprintf(stderr, "mount: can't init for ACLs\n");
			exit(2);
		}
		which = acl_config.first_obj_tag;
		if (strcmp(acl_label, MNTOPT_WILDCARD) == 0) {
			attrs[which].ir = NULL;
			attrs[which].ir_length = 0;
			attrs[which].code = SEC_WILDCARD_TAG;
		} else {
			if ((ir = (char *) acl_er_to_ir(acl_label, &length))
			  == NULL) {
				fprintf(stderr, "Bad ACL: %s\n", acl_label);
				exit(2);
			}
			attrs[which].ir = ir;
			attrs[which].ir_length = length * sizeof(acle_t);
			attrs[which].code = SEC_ACTUAL_TAG;
		}
	}
#endif
#if SEC_MAC
	if (mand_label) {
		if (mand_init()) {
			fprintf(stderr, "mount: can't init for MAC labels\n");
			exit(2);
		}
		if ((ir = (char *) mand_er_to_ir(mand_label)) == NULL) {
			fprintf(stderr, "Bad sensitivity label: %s\n",
				mand_label);
			exit(2);
		}
		which = mand_config.first_obj_tag;
		attrs[which].ir = ir;
		attrs[which].ir_length = mand_bytes();
		attrs[which].code = SEC_ACTUAL_TAG;
	}
#endif
#if SEC_ILB
	if (ilb_label) {
		if (mand_init()) {
			fprintf(stderr, "mount: can't init for ILB labels\n");
			exit(2);
		}
		if ((ir = (char *) ilb_er_to_ir(ilb_label)) == NULL) {
			fprintf(stderr, "Bad information label: %s\n",
				ilb_label);
			exit(2);
		}
		which = mand_config.first_obj_tag + 1;
		attrs[which].ir = ir;
		attrs[which].ir_length = ilb_bytes();
		attrs[which].code = SEC_ACTUAL_TAG;
	}
#endif
#if SEC_NCAV
	if (ncav_label) {
		if (ncav_init()) {
			fprintf(stderr, "mount: can't init for NCAV labels\n");
			exit(2);
		}
		if ((ir = (char *) ncav_er_to_ir(ncav_label)) == NULL) {
			fprintf(stderr, "Bad caveat set: %s\n", ncav_label);
			exit(2);
		}
		which = ncav_config.first_obj_tag;
		attrs[which].ir = ir;
		attrs[which].ir_length = sizeof(ncav_ir_t);
		attrs[which].code = SEC_ACTUAL_TAG;
	}
#endif
}

/*
 * Perform the actual mount system call after raising the required
 * privileges.  If global labels have been specified, use the lmount
 * call, otherwise use the standard mount call.  We use generic
 * arguments to avoid vendor-specific ordering differences.  If the
 * vendor's mount system call requires more or fewer than 4 arguments
 * this must be changed.
 */
mount_domount(a1, a2, a3, a4)
	long	a1, a2, a3, a4;
{
	int	ret;
	priv_t	*raise;
	privvec_t saveprivs;
	extern int sec_errno;

	if (have_labels)
		raise = privvec(SEC_MOUNT, SEC_REMOTE,
#if SEC_ACL
					SEC_ALLOWDACACCESS,
#endif
#if SEC_MAC
					SEC_ALLOWMACACCESS, SEC_MULTILEVELDIR,
#endif
#if SEC_ILB
					SEC_ALLOWILBACCESS,
#endif
#if SEC_NCAV
					SEC_ALLOWNCAVACCESS,
#endif
					-1);
	else
		raise = privvec(SEC_MOUNT, SEC_REMOTE,
#if SEC_MAC
					SEC_MULTILEVELDIR,
#endif
					-1);
	if (forceprivs(raise, saveprivs)) {
		fprintf(stderr, "mount: insufficient privilege\n");
		exit(2);
	}
	disablepriv(SEC_SUSPEND_AUDIT);

        sec_errno = 0;
	ret = lmount(a1, a2, a3, a4, have_labels ? attrs : (attr_t *) 0);

	seteffprivs(saveprivs, (priv_t *) 0);
	return ret;
}

#ifdef AUX
/*
 * open the mount table for update
 */
FILE *
mount_setmntent (file, flags)
char *file;
char *flags;
{
	FILE *ret;

	forcepriv(SEC_ALLOWMACACCESS);
	ret = setmntent(file, flags);
	disablepriv(SEC_ALLOWMACACCESS);
	return(ret);
}

/*
 * open the file system device
 */
mount_opendev (dev, flag)
char *dev;
int flag;
{
	int ret;

	forcepriv(SEC_ALLOWMACACCESS);
	ret = open(dev,flag);
	disablepriv(SEC_ALLOWMACACCESS);
	return(ret);
}
#endif /* AUX */

#ifdef SYSV_3
void
mount_info(fsname, roflag, ro, special, directory)
	char *fsname;
	int roflag;
	int ro;
	char *special;
	char *directory;
{
	printf("mount ");
#if SEC_ACL
	if (acl_label != (char *) 0)
		printf("-A %s ", acl_label);
#endif
#if SEC_MAC
	if (mand_label != (char *) 0)
		printf("-S %s ", mand_label);
#endif
#if SEC_ILB
	if (ilb_label != (char *) 0)
		printf("-I %s ", ilb_label);
#endif
#if SEC_NCAV
	if (ncav_label != (char *) 0)
		printf("-C %s ", ncav_label);
#endif
	printf("-f %s %s%s %s\n", fsname, roflag & ro? "-r " : "",
		special, directory);
}


/*
 * If the fsname is `-', that is filler used by mountall for no fsname.
 */
int
mount_no_fsname(fsname)
	char *fsname;
{
	return ((fsname == (char *) 0) ||
		(strcmp(fsname, "-") == 0));
}
#endif /* SYSV_3 */
#endif SEC_BASE
