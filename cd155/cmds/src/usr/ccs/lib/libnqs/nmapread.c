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
 
/*++ nmapread.c -
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/nmapread.c,v $
 *
 * DESCRIPTION:
 *	Nmapread.c:  Routines to read nmap database
 *
 * STANDARDS VIOLATIONS:
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:27:23 $ $State: Exp $)
 * $Log: nmapread.c,v $
 * Revision 1.4  1994/11/19  02:27:23  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  01:24:18  mwan
 * 1.2 mods
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:58  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:59:59  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nmapread.c	1.2 (nmapread.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include "nmapcommon.h"


/*
 *
 *
 *	int nmap_get_uid (from_mid, from_uid, &local_uid)
 *
 *	Map the user-id: from_uid from the machine with mid: from_mid
 *	to the local user-id: local_uid.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	   1: (NMAP_DEFMAP) if no mapping existed for the specified
 *	      machine and user-id, but a default user-id mapping
 *	      was enabled for the specified machine.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to access the mapping database.
 *	  -4: (NMAP_ENOMAP) if no mapping existed for the specified
 *	      machine and user-id.
 */
int nmap_get_uid (from_mid, from_uid, local_uid)
mid_t from_mid;
uid_t from_uid;
uid_t *local_uid;
{
	struct xidparm parm;
	register int res;
 
	parm.u.u.remote_uid = from_uid;
	res = xidop (from_mid, &parm, GET_UID);
	if (res >= 0) *local_uid = parm.u.u.local_uid;
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (res);
}


/*
 *
 *
 *	int nmap_get_gid (from_mid, from_gid, &local_gid)
 *
 *	Map the user-id: from_gid from the machine with mid: from_mid
 *	to the local user-id: local_gid.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	   1: (NMAP_DEFMAP) if no mapping existed for the specified
 *	      machine and group-id, but a default group-id mapping
 *	      was enabled for the specified machine.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to access the mapping database.
 *	  -4: (NMAP_ENOMAP) if no mapping existed for the specified
 *	      machine and group-id.
 */
int nmap_get_gid (from_mid, from_gid, local_gid)
mid_t from_mid;
gid_t from_gid;
gid_t *local_gid;
{
	struct xidparm parm;
	register int res;
 
	parm.u.g.remote_gid = from_gid;
	res = xidop (from_mid, &parm, GET_GID);
	if (res >= 0) *local_gid = parm.u.g.local_gid;
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (res);
}


/*
 *
 *
 *	int nmap_get_mid (&hostent, &mid)
 *
 *	Determine the machine-id (mid) of the system identified by
 *	the specified hostent structure.
 *
 *	NOTE:	If the passed address of the hostent structure is NULL,
 *		then the machine-id the local machine upon which the
 *		calling process is running is determined by this
 *		function call.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to access the mapping database.
 *	  -4: (NMAP_ENOMAP) if no mapping existed for the specified
 *	      hostent structure.
 */
int nmap_get_mid (hostent, mid)
struct hostent *hostent;
mid_t *mid;
{
	extern int gethostname();		/* Get local host name */
	extern struct hostent *gethostbyname();	/* Get host by name */

	char hostname [MAX_NAMELENGTH+1];	/* Local host name */
	register int status;
	register int alias;

	if (hostent == (struct hostent *) 0) {
		/*
		 *  No hostent structure was specified.  The caller
		 *  is asking for the machine-id of the local machine.
		 */
		if (gethostname (hostname, MAX_NAMELENGTH) == -1) {
			/*
			 *  We were unable to determine the name of the
			 *  local machine!
			 */
			return (NMAP_EUNEXPECT);/* We were surprised */
		}
		hostname [MAX_NAMELENGTH]='\0';	/* Null terminate for sure */
		if ((hostent = gethostbyname(hostname))==(struct hostent *)0) {
			return(NMAP_EUNEXPECT);	/* Surprised again! */
		}
	}
	alias = 0;				/* First alias in list */
	status = namop (hostent->h_name, mid, GET_MID);
	if (hostent->h_aliases != (char **)0) {
		while (status == NMAP_ENOMAP && hostent->h_aliases[alias] != (char *)0){
			/*
			 *  While no mapping, check aliases for a mapping
			 *  until we run out of aliases.
			 */
			status = namop (hostent->h_aliases [alias++], mid, GET_MID);
		}
	}
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (status);	/* Return result */
}


/*
 *
 *
 *	struct hostent *nmap_get_ent (mid):
 *	Return a hostent structure for the specified machine-id.
 *
 *	Returns:
 *		A pointer to a hostent structure for the specified
 *		machine-id if successful; otherwise NULL is returned.
 */
struct hostent *nmap_get_ent (mid)
mid_t mid;			/* Where to return the machine-id */
{
	extern struct hostent *gethostbyname();	/* Get host by name */
	char *nmap_get_nam();	/* Get principal name for mid */

	char *hostname;		/* Principal name for machine-id */

	if ((hostname = nmap_get_nam (mid)) == (char *) 0) {
		return ((struct hostent *) 0);
	}
	return (gethostbyname (hostname));
}


/*
 *
 *
 *	char *nmap_get_nam (mid)
 *
 *	Return a pointer to the null terminated character string
 *	containing the principal host name for the specified machine-id.
 *	Return NULL for all error conditions (including no mapping for
 *	"mid").
 *
 *	WARNING:  The name is placed in a static data area which is
 *		  overwritten with each call.
 */
char *nmap_get_nam (mid)
mid_t mid;
{

	static char name [MAX_NAMELENGTH+1];	/* Name space */
	struct rawblock block;
	register int i;

	if (open_machine (mid, O_RDONLY) != NMAP_SUCCESS) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return ((char *) 0);	/* NMAP_EUNEXPECT or NMAP_ENOPRIV */
	}
	/*
	 *  Seek to beginning of file if not already there.
	 */
	if (LSEEK (machine_file, 0L, 0) == -1) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return ((char *) 0);	/* NMAP_EUNEXPECT */
	}
	/*
	 *  Get the principal name of the machine from the first
	 *  block of the uid/gid mapping file for the target machine.
	 */
	if (READ (machine_file, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
		return ((char *) 0);
	}
	i = 0;
	while (block.u.mhdr.name [i] != '\0' && i < MAX_NAMELENGTH) {
		name [i] = block.u.mhdr.name [i];
		i++;
	}
	name [i] = '\0';	/* Null terminate */
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (name);
}


/*
 *
 *
 *	int nmap_get_defuid (from_mid, &defuid)
 *
 *
 *	Get the default user-id mapping for the machine with
 *	mid: from_mid to the default user-id: defuid.
 *
 *	Returns:
 *	   0: (NMAP_SUCCESS) if successful.
 *	  -1: (NMAP_EUNEXPECT) if an unanticipated error occurred in
 *	      the mapping software.
 *	  -2: (NMAP_ENOPRIV) if the caller did not have sufficient
 *	      privilege to update the mapping database.
 *	  -4: (NMAP_ENOMAP) if default uid doesn't exist for this mid
 *	  -5: (NMAP_ENOMID) if no such machine as specified by
 *	      the machine-id (mid) exists.
 */
int nmap_get_defuid (from_mid, defuid)
mid_t from_mid;
uid_t *defuid;
{

	struct rawblock block;
	int status;

	if ((status = open_machine (from_mid, O_RDONLY)) != NMAP_SUCCESS) {
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
	if ( block.u.mhdr.defuid_enabled !=  1 )
		return (NMAP_ENOMAP);

	*defuid = block.u.mhdr.defuid;		/* Store default */
	nmap_ctl (NMAP_OPNOFD, nfds);
			/* Relinquish file descriptors if necessary */
	return (NMAP_SUCCESS);	/* Return success */
}
