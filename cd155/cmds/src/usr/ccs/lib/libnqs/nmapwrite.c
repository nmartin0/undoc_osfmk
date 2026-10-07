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
 
/*++ nmapwrite.c -
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/nmapwrite.c,v $
 *
 * DESCRIPTION:
 *	Nmapwrite.c:  Routines to modify nmap database
 *
 * STANDARDS VIOLATIONS:
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:27:25 $ $State: Exp $)
 * $Log: nmapwrite.c,v $
 * Revision 1.4  1994/11/19  02:27:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  01:24:23  mwan
 * 1.2 mods
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:00  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:59:19  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nmapcommon.h"


/*
 *
 *
 *	int nmap_add_uid (from_mid, from_uid, local_uid)
 *
 *	Add the mapping for user-id: from_uid from the machine with
 *	mid: from_mid to the local user-id: local_uid.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -3: (NMAP_ECONFLICT) if a mapping existed for the specified
 *	      machine and user-id which differed from the mapping
 *	      that was specified.  The mapping database remains
 *	      unchanged.
 *	  -5: (NMAP_ENOMID) if no such machine as specified by
 *	      the machine-id (mid) exists.
 */
int nmap_add_uid (from_mid, from_uid, local_uid)
mid_t from_mid;
uid_t from_uid;
uid_t local_uid;
{
	register int res;
	struct xidparm parm;

	parm.u.u.remote_uid = from_uid;
	parm.u.u.local_uid = local_uid;
	res = xidop (from_mid, &parm, ADD_UID);
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (res);
}


/*
 *
 *
 *	int nmap_add_gid (from_mid, from_gid, local_gid)
 *
 *	Add the mapping for the user-id: from_gid from the machine with
 *	mid: from_mid to the local user-id: local_gid.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -3: (NMAP_ECONFLICT) if a mapping existed for the specified
 *	      machine and group-id which differed from the mapping
 *	      that was specified.  The mapping database remains
 *	      unchanged.
 *	  -5: (NMAP_ENOMID) if no such machine as specified by
 *	      the machine-id (mid) exists.
 */
int nmap_add_gid (from_mid, from_gid, local_gid)
mid_t from_mid;
gid_t from_gid;
gid_t local_gid;
{
	register int res;
	struct xidparm parm;
 
	parm.u.g.remote_gid = from_gid;
	parm.u.g.local_gid = local_gid;
	res = xidop (from_mid, &parm, ADD_GID);
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (res);
}


/*
 *
 *
 *	int nmap_set_defuid (from_mid, defuid)
 *
 *	Enable and set the default user-id mapping for the machine with
 *	mid: from_mid to the default user-id: defuid.  Any default
 *	user-id mapping already enabled is REPLACED by the new default
 *	mapping.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -5: (NMAP_ENOMID) if no such machine as specified by
 *	      the machine-id (mid) exists.
 */
int nmap_set_defuid (from_mid, defuid)
mid_t from_mid;
uid_t defuid;
{

	struct rawblock block;
	int status;

	if ((status = open_machine (from_mid, O_RDWR)) != NMAP_SUCCESS) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (status);   /* NMAP_EUNEXPECT, NMAP_ENOMID, or */
				   /* NMAP_ENOPRIV */
	}
	/*
	 *  Seek to beginning of file if not already there.
	 */
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	if (READ (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	block.u.mhdr.defuid_enabled = 1;	/* Enable default uid */
	block.u.mhdr.defuid = defuid;		/* Store default */
	/*
	 *  Now, rewrite updated block.
	 */
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	if (WRITE (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (NMAP_SUCCESS);	/* Return success */
}


/*
 *
 *
 *	int nmap_set_defgid (from_mid, defgid)
 *
 *	Enable and set the default group-id mapping for the machine with
 *	mid: from_mid to the default group-id: defgid.  Any default
 *	group-id mapping already enabled is REPLACED by the new default
 *	mapping.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -5: (NMAP_ENOMID) if no such machine as specified by
 *	      the machine-id (mid) exists.
 */
int nmap_set_defgid (from_mid, defgid)
mid_t from_mid;
gid_t defgid;
{

	struct rawblock block;
	int status;

	if ((status = open_machine (from_mid, O_RDWR)) != NMAP_SUCCESS) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (status);   /* NMAP_EUNEXPECT, NMAP_ENOMID, or */
				   /* NMAP_ENOPRIV */
	}
	/*
	 *  Seek to beginning of file if not already there.
	 */
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	if (READ (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	block.u.mhdr.defgid_enabled = 1;	/* Enable default uid */
	block.u.mhdr.defgid = defgid;		/* Store default */
	/*
	 *  Now, rewrite updated block.
	 */
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	if (WRITE (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (NMAP_SUCCESS);	/* Return success */
}


/*
 *
 *
 *	int nmap_add_nam (from_nam, to_mid)
 *
 *	Add the mapping from the name: from_nam to the machine-id: to_mid:
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -3: (NMAP_ECONFLICT) if a mapping existed for the specified
 *	      machine name which differed from the mapping that was
 *	      specified.  The mapping database remains unchanged.
 *	  -5: (NMAP_ENOMID) if no such machine as specified by
 *	      the machine-id (mid) exists.
 *	  -6: (NMAP_EBADNAME) if a bad "from_nam" was specified.
 *	      Either the name was null, or the name was too long.
 */
int nmap_add_nam (from_nam, to_mid)
char *from_nam;
mid_t to_mid;
{
	register int res;

	res = namop (from_nam, &to_mid, ADD_NAM);
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (res);
}


/*
 *
 *
 *	int nmap_add_mid (mid, name)
 *
 *	Create a new machine with the specified machine-id (mid) and
 *	assign the principal name (name) to this new machine.
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -3: (NMAP_ECONFLICT) if a machine already exists with the
 *	      specified machine-id (mid).
 *	  -6: (NMAP_EBADNAME) if a bad principle name was specified.
 *	      Either the name was null, or the name was too long.
 */
int nmap_add_mid (mid, name)
mid_t mid;
char *name;
{
	struct rawblock newblock;
	register int i;
	int status;
	struct hostent testused;
	mid_t testmid;

	i = strlen (name);
	if (i == 0 || i > MAX_NAMELENGTH) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EBADNAME);
	}
	testused.h_name = name;
	testused.h_aliases = (char **)0;
	if (nmap_get_mid (&testused, &testmid) == NMAP_SUCCESS) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_ECONFLICT);
	}
	if ((status = open_machine (mid, O_RDWR | O_CREAT |
					 O_EXCL)) != NMAP_SUCCESS) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (status);  /* NMAP_EUNEXPECT, NMAP_ENOPRIV, or */
				  /* NMAP_ECONFLICT */
	}
	/*
	 *  Clear the mapping block.
	 */
	for (i=0; i < ATOMICBLKSIZ; i++) newblock.u.bytes [i] = '\0';
	/*
	 *  Store the principle name of the machine in the first block.
	 */
	i = 0;
	for (i = 0; name[i] != '\0'; i++) {
		newblock.u.mhdr.name [i] = name[i];
	}
	newblock.u.mhdr.name [i] = '\0';	/* Null terminate name */
	newblock.u.mhdr.defuid_enabled = 0;	/* No default uid mapping */
	newblock.u.mhdr.defgid_enabled = 0;	/* No default gid mapping */
	newblock.u.mhdr.enatime = 1L;		/* Enable mapping by default */
	/*
 	 *  Write the first block of the machine file.  It is not
	 *  necessary to do an lseek() to the beginning of the file
	 *  here since the O_CREAT flag guaranteed that the file was
	 *  created from scratch.  See the "fine print" on open_machine().
 	 *  This block will be the first block in the machine file.
	 */
	if (WRITE (machine_file, newblock.u.bytes,
		   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	/*
	 *  Write the first User-id mapping block for this machine (the
	 *  block will not contain any mappings).  This block will be
 	 *  the second block in the machine file.
	 */
	for (i=0; i < ATOMICBLKSIZ; i++) {
		newblock.u.bytes[i]='\0';	/* Paranoia */
	}
	newblock.u.umap.next = 0;	/* No next block. */
	newblock.u.umap.n_pairs = 0;	/* No mappings. */
	if (WRITE (machine_file, newblock.u.bytes,
		   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	/*
	 *  Write the first Group-id mapping block for this machine (the
	 *  block will not contain any mappings).  This block will be
	 *  the third block in the machine file.
	 */
	for (i=0; i < ATOMICBLKSIZ; i++) {
		newblock.u.bytes[i]='\0';	/* Paranoia */
	}
	newblock.u.gmap.next = 0;	/* No next block. */
	newblock.u.gmap.n_pairs = 0;	/* No mappings. */
	if (WRITE (machine_file, newblock.u.bytes,
		   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	return (nmap_add_nam (name, mid));	/* Return success */
}


/*
 *
 *
 *	int nmap_chg_mid (mid, name)
 *
 *	Change the principal name of the machine whose machine-id is
 *	mid to the new name of (name).
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -5: (NMAP_ENOMID) if no such machine as specified by
 *	      the machine-id (mid) exists.
 *	  -6: (NMAP_EBADNAME) if a bad principle name was specified.
 *	      Either the name was null, or the name was too long.
 */
int nmap_chg_mid (mid, name)
mid_t mid;
char *name;
{

	struct rawblock block;
	register int i;
	int status;

	i = strlen (name);
	if (i == 0 || i > MAX_NAMELENGTH) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EBADNAME);
	}
	if ((status = open_machine (mid, O_RDWR)) != NMAP_SUCCESS) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (status);   /* NMAP_EUNEXPECT, NMAP_ENOMID, or */
				   /* NMAP_ENOPRIV */
	}
	/*
	 *  Seek to beginning of file if not already there.
	 */
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	if (READ (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	/*
	 *  Store the principle name of the machine in the first block.
	 */
	i = 0;
	while (*name) {
		block.u.mhdr.name [i++] = *name++;
	}
	block.u.mhdr.name [i] = '\0';
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	if (WRITE (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (NMAP_SUCCESS);	/* Return success */
}


/*
 *
 *
 *	int nmap_del_uid (from_mid, from_uid)
 *
 *	Delete the mapping for user-id: from_uid from the machine with
 *	mid: from_mid.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -4: (NMAP_ENOMAP) if the mapping to be deleted did not exist.
 */
int nmap_del_uid (from_mid, from_uid)
mid_t from_mid;
uid_t from_uid;
{
	register int res;
	struct xidparm parm;

	parm.u.u.remote_uid = from_uid;
	res = xidop (from_mid, &parm, DEL_UID);
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (res);
}


/*
 *
 *
 *	int nmap_del_gid (from_mid, from_gid)
 *
 *	Delete the mapping for the user-id: from_gid from the machine with
 *	mid: from_mid.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -4: (NMAP_ENOMAP) if the mapping to be deleted did not exist.
 */
int nmap_del_gid (from_mid, from_gid)
mid_t from_mid;
gid_t from_gid;
{
	register int res;
	struct xidparm parm;
 
	parm.u.g.remote_gid = from_gid;
	res = xidop (from_mid, &parm, DEL_GID);
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (res);
}


/*
 *
 *
 *	int nmap_del_defuid (from_mid)
 *
 *	Delete and disable the default user-id mapping for the machine
 *	with mid: from_mid.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -5: (NMAP_ENOMID): if no such machine as specified by the
 *	      machine-id (from_mid) exists.
 */
int nmap_del_defuid (from_mid)
mid_t from_mid;
{

	struct rawblock block;
	int status;

	if ((status = open_machine (from_mid, O_RDWR)) != NMAP_SUCCESS) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (status);   /* NMAP_EUNEXPECT, NMAP_ENOMID, or */
				   /* NMAP_ENOPRIV */
	}
	/*
	 *  Seek to beginning of file if not already there.
	 */
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	if (READ (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	block.u.mhdr.defuid_enabled = 0;	/* Disable default uid */
	/*
	 *  Now, rewrite updated block.
	 */
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	if (WRITE (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (NMAP_SUCCESS);	/* Return success */
}


/*
 *
 *
 *	int nmap_del_defgid (from_mid)
 *
 *	Delete and disable the default group-id mapping for the machine
 *	with mid: from_mid.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -5: (NMAP_ENOMID): if no such machine as specified by the
 *	      machine-id (from_mid) exists.
 */
int nmap_del_defgid (from_mid)
mid_t from_mid;
{

	struct rawblock block;
	int status;

	if ((status = open_machine (from_mid, O_RDWR)) != NMAP_SUCCESS) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (status);   /* NMAP_EUNEXPECT, NMAP_ENOMID, or */
				   /* NMAP_ENOPRIV */
	}
	/*
	 *  Seek to beginning of file if not already there.
	 */
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	if (READ (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	block.u.mhdr.defgid_enabled = 0;	/* Disable default uid */
	/*
	 *  Now, rewrite updated block.
	 */
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	if (WRITE (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (NMAP_SUCCESS);	/* Return success */
}


/*
 *
 *
 *	int nmap_del_nam (from_nam)
 *
 *	Delete the name-to-mid mapping for the: from_nam.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -4: (NMAP_ENOMAP) if the mapping to be deleted did not exist.
 */
int nmap_del_nam (from_nam)
char *from_nam;
{
	register int res;
	mid_t no_mid;

	res = namop (from_nam, &no_mid, DEL_NAM);
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (res);
}


/*
 *
 *
 *	int nmap_del_mid (mid)
 *
 *	Deletes ALL mappings associated with the machine identified by
 *	"mid" (that is, all uid, gid, and name-to-mid, and mid-to-name
 *	mappings for the specified "mid").
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -5: (NMAP_ENOMID) if no such machine as specified by
 *	      the machine-id (mid) exists.
 */
int nmap_del_mid (mid)
mid_t mid;
{

	char path [MAX_PATHLENGTH+1];
	struct rawblock hashblock;
	struct rawblock mapblock;
	register long address;
	unsigned prevoffset;
	unsigned offset;
	register struct nammap *mmap;
	register int i;			/* Index var */
	register int j;			/* Index var */
	int size;			/* Size of entry */
	register int status;		/* Status return from open_name_map() */

	if ((status = open_name_map (O_RDWR)) != NMAP_SUCCESS) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (status);	/* Return -1: Fatal error */
					/* 	  -2: Insufficient privilege */
	}
	machine_name (mid, path);	/* Get pathname */
	/*
	 *  Path now has the complete pathname of the uid/gid
	 *  mapping file for the specified mid.  Delete the uid/gid
	 *  mappings for the specified machine-id, and the principal
	 *  name for the specified machine.
	 */
	if (unlink (path) == -1) {		/* unlink failed */
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		if (errno == EACCES) return (NMAP_ENOPRIV);
		if (errno == ENOENT) return (NMAP_ENOMAP);
		return (NMAP_EUNEXPECT);
	}
	/*
	 *  Now, delete all name-to-mid mappings for the specified mid.
	 */
	for (i=0; i < HASHNAMBLKS; i++) {
		if (LSEEK (name_mapfile, (long) i * ATOMICBLKSIZ, 0) == -1) {
			nmap_ctl (NMAP_OPNOFD, nfds);
				/* Relinquish file descriptors if necessary */
			return (NMAP_EUNEXPECT);
		}
		if (READ (name_mapfile, hashblock.u.bytes,
			  ATOMICBLKSIZ) != ATOMICBLKSIZ) {
			nmap_ctl (NMAP_OPNOFD, nfds);
				/* Relinquish file descriptors if necessary */
			return (NMAP_EUNEXPECT);
		}
		for (j=0; j < HASHENTSPERBLK; j++) {
			/*
			 * Get address of first block in the hash chain.
			 */
			address = hashblock.u.hash.chain [j];
			while (address != 0L) {
				if (LSEEK (name_mapfile, address, 0)==-1){
					/*
					 *  Relinquish file descriptors
					 *  as necessary.
					 */
					nmap_ctl (NMAP_OPNOFD, nfds);
					return (NMAP_EUNEXPECT);
				}
				if (READ (name_mapfile, mapblock.u.bytes,
					  ATOMICBLKSIZ) != ATOMICBLKSIZ) {
					/*
					 *  Relinquish file descriptors
					 *  as necessary.
					 */
					nmap_ctl (NMAP_OPNOFD, nfds);
					return (NMAP_EUNEXPECT);
				}
				/*
				 *  Skip over nextptr for hash chain link and
				 *  examine this block.
				 */
				offset = ((char *)(&mapblock.u.nhdr.map))
				       - mapblock.u.bytes;
				prevoffset = offset;
				do {
					mmap = (struct nammap *)
					       (mapblock.u.bytes + offset);
					if (mmap->namelen != 0 &&
					    mmap->mid == mid) {
						/*
						 *  We must delete this
						 *  name to mid mapping.
						 */
						mmap->namelen=0;/* Free entry */
						if (prevoffset != offset) {
							/*
							 *  The entry to be
							 *  freed is not the
							 *  first entry in the
							 *  block.
							 */
							offset = prevoffset;
							size = mmap->size;
							mmap=(struct nammap *)
							     (mapblock.u.bytes +
							      offset);
							/*
							 *  Merge with previous
							 *  entry.
							 */
							mmap->size += size;
						}
						if (LSEEK (name_mapfile,
							   address, 0)==-1) {
							/*
							 *  Relinquish file
							 *  descriptors as
							 *  necessary.
							 */
							nmap_ctl (NMAP_OPNOFD,
								  nfds);
							return (NMAP_EUNEXPECT);
						}
						if (WRITE (name_mapfile,
							   mapblock.u.bytes,
							   ATOMICBLKSIZ)
						    != ATOMICBLKSIZ) {
							/*
							 *  Relinquish file
							 *  descriptors as
							 *  necessary.
							 */
							nmap_ctl (NMAP_OPNOFD,
								  nfds);
							return (NMAP_EUNEXPECT);
						}
					}
					/*
					 *  Prepare to access the next entry.
					 */
					prevoffset = offset;
					offset += mmap->size;
				} while (offset < ATOMICBLKSIZ);
				/*
				 *  No name to specified mid mapping was found
				 *  in this block.  Go inspect the succeeding
				 *  block(s) for such a mapping.
				 */
				address = mapblock.u.nhdr.nexthash;
			}
		}
	}
	nmap_ctl (NMAP_OPNOFD, nfds);
				/* Relinquish file descriptors if necessary */
	return (NMAP_SUCCESS);	/* Success */
}


/*
 *
 *
 *	int nmap_create ()
 *
 *	Create the Network mapping database.
 *	No NMAP calls with the exception of nmap_ctl() will work
 *	until the database has been created.
 *
 *	This function returns:
 *	   0: (NMAP_SUCCESS) if the network mapping database was
 *	      created successfully.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error condition has
 *	      occurred in the mapping software.  This error is also
 *	      returned if the size of one of the key NMAP data
 *	      structures exceeds ATOMICBLKSIZ bytes (a absolutely
 *	      disastrous error).
 *	  -2: (NMAP_ENOPRIV) if the caller lacks sufficient privilege
 *	      to create the database.
 *	  -3: (NMAP_ECONFLICT) if the network mapping database already
 *	      exists.
 *
 */
int nmap_create ()
{
	struct rawblock block;
	register int i;
	register int status;

	/*
	 *  Test for bad sizes of things.
	 */
	if (sizeof (struct m_header) > ATOMICBLKSIZ ||
	    sizeof (struct gidmap) > ATOMICBLKSIZ ||
	    sizeof (struct uidmap) > ATOMICBLKSIZ ||
	    sizeof (struct hashblk) > ATOMICBLKSIZ ||
	    sizeof (struct n_header) > ATOMICBLKSIZ) {
		/*
		 *  All of the above MUST fit in one physical disk block!
		 */
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	/*
	 *  Clear the rawblock.
	 */
	for (i=0; i < ATOMICBLKSIZ; i++) block.u.bytes [i] = '\0';
	/*
	 *  Format a block of null hash chain pointers.
	 */
	for (i=0; i < HASHENTSPERBLK; i++) {
		block.u.hash.chain [i] = 0;
	}
	/*
	 *  Format hash table chain for the name-to-mid mappings.
	 */
	if ((status = open_name_map (O_RDWR|O_CREAT|O_EXCL)) != NMAP_SUCCESS) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (status);	/*NMAP_EUNEXPECT, NMAP_ENOPRIV, */
	}				/* or NMAP_ECONFLICT */
	for (i=0; i < HASHNAMBLKS; i++) {
		if (WRITE (name_mapfile, block.u.bytes,
			   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
			nmap_ctl (NMAP_OPNOFD, nfds);
				/* Relinquish file descriptors if necessary */
			return (NMAP_EUNEXPECT);
		}
	}
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (NMAP_SUCCESS);
}


/*
 *
 *
 *	int nmap_dis_mid (from_mid, wakeuptime)
 *
 *	Disables user and group mapping from "from_mid" to the local machine
 *	until "wakeuptime" (standard Unix long) is reached.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -5: (NMAP_ENOMID) if no such machine as specified by
 *	      the machine-id (mid) exists.
 */
int nmap_dis_mid (from_mid, wakeuptime)
mid_t from_mid;
time_t wakeuptime;
{

	struct rawblock block;
	int status;

	if ((status = open_machine (from_mid, O_RDWR)) != NMAP_SUCCESS) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (status);   /* NMAP_EUNEXPECT, NMAP_ENOMID, or */
				   /* NMAP_ENOPRIV */
	}
	/*
	 *  Seek to beginning of file if not already there.
	 */
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	if (READ (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	block.u.mhdr.enatime = wakeuptime;	/* Set up new enable time */
	/*
	 *  Now, rewrite updated block.
	 */
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	if (WRITE (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return (NMAP_EUNEXPECT);
	}
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (NMAP_SUCCESS);	/* Return success */
}


/*
 *
 *
 *	int nmap_ena_mid (from_mid)
 *
 *	Enables user and group mapping from "from_mid" to the local machine
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -5: (NMAP_ENOMID) if no such machine as specified by
 *	      the machine-id (mid) exists.
 */
int nmap_ena_mid (from_mid)
mid_t from_mid;
{

	return (nmap_dis_mid (from_mid, 1L));
}
