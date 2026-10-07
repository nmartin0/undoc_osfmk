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
 * Copyright (c) 1988, 1989 SecureWare, Inc.  All rights reserved.
 *
 * This Module contains Proprietary Information of SecureWare, Inc. and
 * should be treated as Confidential.
 */

#ident "@(#)security.c	2.1 16:18:22 4/20/90 SecureWare"
/*
 * Based on:
#ident "@(#)security.c	2.6 09:13:51 11/1/89 SecureWare, Inc."

/*LINTLIBRARY*/

#include <sys/secdefines.h>

#if SEC_BASE /*{*/

/*
 * This file contains a set of routines used to make programs
 * more secure.  Specifically, it contains system all interface
 * routines for security.
 */

#include <sys/types.h>

#include <sys/security.h>
#include <sys/audit.h>
#if SEC_ARCH
#include <sys/secpolicy.h>
#endif
#include <sys/auditsysc.h>
#include <prot.h>

extern int syscall();

/*
 * Adjunct error number that allows analysis of security call failures.
 * The value of this external is set from the second byte of rval1.
 * The low order bits are used for the UNIX error number;
 * SEC_ERRNO_MASK is a mask of where in the errno field the sec_errno
 * value is placed.  SEC_ERRNO_SHIFT tells how far over to shift
 * the sec_errno value to get it right-justified.  Be careful if the
 * mask extends to the highest bit of errno and the machine sign extends
 * on a right shift.
 */

int sec_errno;
extern int errno;

#define SET_SEC_ERRNO()	{\
	sec_errno = (((long) errno) & SEC_ERRNO_MASK) >> SEC_ERRNO_SHIFT; \
	errno = ((long) errno) & ~SEC_ERRNO_MASK; \
	}
/*
 * the security system call entry point, defined in sys/auditsysc.h.
 */

#define SECURITY AUDIT_SECURITY

/*
 * The routines herein are user library stubs for the
 * security() sysent entry point.  The first argument to security() is
 * the system call code of the call and the remaining arguments
 * are call-specific.  Specify additional arguments as 0.
 *
 * Note that some return actual values (getluid(), getpriv())
 * while the others return 0 for success.  All return -1 for failure.
 */


/*
 * Force all open files in the system to the filename provided to become
 * invalid.  On the next read/write/ioctl access, send a signal to the
 * effected process and return an error.
 */
int
stopio(filename)
	char *filename;
{
	register int ret;

	ret = syscall(SECURITY, SEC_STOPIO, (ulong) filename,
		0L, 0L, 0L, 0L, 0L);
	if (ret != -1)
		ret = 0;
	else
		SET_SEC_ERRNO();

	return ret;
}


/*
 * Get the system's notion of the login UID associated with the process.
 * If it has not been set yet, return an error.
 */
int
getluid()
{
	register int ret;

	ret = syscall(SECURITY, SEC_GETLUID, 0L, 0L, 0L, 0L, 0L, 0L);

	if (ret == -1)
		SET_SEC_ERRNO();

	return ret;
}


/*
 * Set the system's notion of the login UID for this process and all its
 * descendants.  Once set, it cannot be reset.
 */
int
setluid(uid)
	ushort uid;
{
	register int ret;

	ret = syscall(SECURITY, SEC_SETLUID,
		(ulong) uid, 0L, 0L, 0L, 0L, 0L);
	if (ret != -1)
		ret = 0;
	else
		SET_SEC_ERRNO();

	return ret;
}


/*
 * Get the system privileges for this process and store them in the supplied
 * buffer.  The size in bytes of the number of privileges is returned.
 * Privtype specifies either the maximum, base or effective privileges.
 */
int
getpriv(privtype, vec)
	int privtype;
	priv_t *vec;
{
	obj_t objid;

	objid.o_pid = 0;

	return statprivsys(privtype, vec, OT_PROCESS, &objid);
}


/*
 * Reset the system privileges for this process.  They cannot be more
 * than was already set.  Privtype specifies either the maximum,
 * base or effective privileges.
 */
int
setpriv(privtype, vec)
	int privtype;
	priv_t *vec;
{
	obj_t objid;

	objid.o_pid = 0;

	return chprivsys(privtype, vec, OT_PROCESS, &objid);

}

#if SEC_PRIV
/*
 * Retrieve the specified privilege set for a file.
 * Privtype is one of SEC_POTENTIAL_PRIV or SEC_GRANTED_PRIV
 */

int
statpriv(file, privtype, vec)
	char *file;
	int privtype;
	priv_t *vec;
{
	obj_t objid;

	objid.o_file = file;

	return statprivsys(privtype, vec, OT_REGULAR, &objid);
}

/*
 * Set the privilege set for a file.
 * Privtype is one of SEC_POTENTIAL_PRIV or SEC_GRANTED_PRIV
 */

chpriv(file, privtype, vec)
	char *file;
	int privtype;
	priv_t *vec;
{
	obj_t objid;

	objid.o_file = file;

	return chprivsys(privtype, vec, OT_REGULAR, &objid);
}
#endif

/*
 * Obtain the privileges associated with an object, whether the object be
 * a process, file or IPC entity.
 */
int
statprivsys(privtype, vec, objtype, objid)
	int privtype;
	priv_t *vec;
	int objtype;
	obj_t *objid;
{
	register int ret;

	ret = syscall(SECURITY, SEC_STATPRIV, privtype, (ulong) vec,
		       objtype, objid, 0L, 0L);

	if (ret == -1)
		SET_SEC_ERRNO();

	return ret;
}


/*
 * Change the privileges associated with an object, whether the object be
 * a process, file or IPC entity.
 */
int
chprivsys(privtype, vec, objtype, objid)
	int privtype;
	priv_t *vec;
	int objtype;
	obj_t *objid;
{
	register int ret;

	ret = syscall(SECURITY, SEC_CHPRIVSYS, privtype, (ulong) vec,
		      objtype, objid, 0L, 0L);
	if (ret != -1)
		ret = 0;
	else
		SET_SEC_ERRNO();

	return ret;
}


#if SEC_ARCH
/*
 * Obtain the effective access for the object, considering ALL security
 * policies and UNIX constraints.
 */
int
eaccess(file, mode)
	char *file;
	int mode;
{
	register int ret;

	ret = syscall(SECURITY, SEC_EACCESS, file, mode, 0L, 0L, 0L);
	if (ret != -1)
		ret = 0;
	else
		SET_SEC_ERRNO();

	return ret;
}
#endif

#if SEC_MAC
/*
 * Make a multilevel directory.
 */
int
mkmultdir(file, mode)
	char *file;
	int mode;
{
	register int ret;

	ret = syscall(SECURITY, SEC_MKMULTDIR, file, 0L, 0L, 0L, 0L, 0L);
	if (ret != -1)
		ret = 0;
	else
		SET_SEC_ERRNO();

	return ret;
}


/*
 * Remove a multilevel directory.
 */
int
rmmultdir(file, mode)
	char *file;
	int mode;
{
	register int ret;

	ret = syscall(SECURITY, SEC_RMMULTDIR, file, 0L, 0L, 0L, 0L, 0L);
	if (ret != -1)
		ret = 0;
	else
		SET_SEC_ERRNO();

	return ret;
}


/*
 * Query a file to see if it is a multilevel directory.  It returns 1
 * if it is a multilevel directory, 0 if not, and -1 for errors.
 */
int
ismultdir(file, mode)
	char *file;
	int mode;
{
	register int ret;

	ret = syscall(SECURITY, SEC_ISMULTDIR, file, 0L, 0L, 0L, 0L, 0L);

	if (ret == -1)
		SET_SEC_ERRNO();

	return ret;
}
#endif

#if SEC_CHOTS
/*
 * Make a two person rule file.
 */

int
mk2person(file)
	char *file;
{
	register int ret;

	ret = syscall(SECURITY, SEC_MK2PERSON, file, 0L, 0L, 0L, 0L, 0L);
	if (ret != -1)
		ret = 0;
	else
		SET_SEC_ERRNO();

	return ret;
}


/*
 * Remove the two person rule flag from a file.
 */

int
rm2person(file)
	char *file;
{
	register int ret;

	ret = syscall(SECURITY, SEC_RM2PERSON, file, 0L, 0L, 0L, 0L);
	if (ret != -1)
		ret = 0;
	else
		SET_SEC_ERRNO();

	return ret;
}


/*
 * Query a file to see if it is a two person rule file.  It returns 1
 * if it is a two person rule file, 0 if not, and -1 for errors.
 */

int
is2person(file)
	char *file;
{
	register int ret;

	ret = syscall(SECURITY, SEC_IS2PERSON, file, 0L, 0L, 0L, 0L);
	if(ret == -1)
		SET_SEC_ERRNO();

	return ret;
}
#endif


#if SEC_WIS
/*
 * Set the process's session ID
 */

int
setsession(sid)
	int sid;
{
	register int ret;

	ret = syscall(SECURITY, SEC_SETSESSION, sid, 0L, 0L, 0L, 0L);
	if (ret == -1)
		SET_SEC_ERRNO();

	return ret;
}


/*
 * Send a signal to all processes in a session.  Returns 0 if one or more
 * processes were signalled, -1 with errno set if no process was signalled
 * or an error was detected.
 */

int
killsession(sid, sig)
	int sid, sig;
{
	register int ret;

	ret = syscall(SECURITY, SEC_KILLSESSION, sid, sig, 0L, 0L, 0L);
	if (ret == -1)
		SET_SEC_ERRNO();

	return ret;
}
#endif


#if SEC_ARCH

/*
 * Get a security label from one of the policies.
 */
int
getlabel(policy,tag_offset,attr,objtype,objid)
int policy, tag_offset, objtype;
attr_t *attr;
obj_t *objid;
{
	register int ret;

	ret = syscall(SECURITY, SEC_GETLABEL, policy, tag_offset,
			attr, objtype, objid);
	if (ret != -1)
		ret = 0;
	else
		SET_SEC_ERRNO();

	return ret;
}


/*
 * Set a security label from one of the policies.
 */
int
setlabel(policy,tag_offset,attr,objtype,objid)
int policy, tag_offset, objtype;
attr_t *attr;
obj_t *objid;
{
	register int ret;

	ret = syscall(SECURITY, SEC_SETLABEL, policy, tag_offset,
			attr, objtype, objid);
	if (ret != -1)
		ret = 0;
	else
		SET_SEC_ERRNO();

	return ret;
}


/*
 * Label mount an insecure file system type, assigning all files therein to
 * single values (fixed by the labels) for the security policies.
 */
int
lmount(special, dir, mflag, fstyp, attrs)
	char *special;
	char *dir;
	int mflag;
	int fstyp;
	attr_t *attrs;
{
	register int ret;

	ret = syscall(SECURITY, SEC_LMOUNT,
		special, dir, mflag, fstyp, attrs, 0L);
	if (ret != -1)
		ret = 0;
	else
		SET_SEC_ERRNO();

	return ret;
}

/*
 * Return TRUE if the file system supports labels and/or privileges
 */
int
islabeledfs(file)
	char *file;
{
	register int ret;

	ret = syscall(SECURITY, SEC_ISLABELEDFS, file, 0L, 0L, 0L, 0L, 0L);

	if (ret == -1)
		SET_SEC_ERRNO();

	return ret;
}
#endif /* SEC_ARCH */
#endif /*} SEC_BASE */
