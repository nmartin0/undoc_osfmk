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
 
/*++ mapuser.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/mapuser.c,v $
 *
 * DESCRIPTION:
 *
 *	Given a user's identity on another host, return
 *	a pointer to that user's password file entry on this host.
 *
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	May 9, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1995/03/01 22:34:52 $ $State: Exp $)
 * $Log: mapuser.c,v $
 * Revision 1.4  1995/03/01  22:34:52  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 12468
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnqs/mapuser.c
 *
 * Revision 1.3  1994/11/19  02:27:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:16  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:10  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:51:24  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)mapuser.c	1.3 (mapuser.c OSF/1 NQS2.0 GJK) 7/14/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <pwd.h>
#include NMAP				/* Mid_t (all OS's) */
					/* Uid_t and gid_t (if not BSD43) */

#if OSF
#include "sys_types_OSF.h"      /* in OSF sys_types_OSF.h */
#else
#include <sys/types.h>
#endif

#include <sys/stat.h>

#if	NETWORKED
#if	STREAM_SOCKETS

extern struct passwd *fetchpwnam();	/* NQS version of getpwnam() */
extern struct passwd *fetchpwuid();	/* NQS version of getpwuid() */
extern int nmap_get_uid();		/* Get uid on this machine */
extern char *strcat();			/* Concatenate strings */
#if	BSD42 | BSD43 | ULTRIX
extern char *index();			/* Locate character in string */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern char *strchr();			/* Locate character in string */
#else
BAD SYSTEM TYPE
#endif
#endif
extern int strcmp();			/* String comparison */
extern char *strcpy();			/* String copy */
extern char *strtok();			
extern int  Debug;

#define MAX_ACCOUNTNAME	15

/*** mapuser
 *
 *
 *	struct passwd *mapuser: Given a user's identity on another host,
 *	return a pointer to that user's password file entry on this host.
 *
 */
struct passwd *mapuser (fromuid, fromusername, frommid, fromhostname, mapbyuid)
uid_t fromuid;				/* Client's uid on remote machine */
char *fromusername;			/* Client username on remote machine */
mid_t frommid;				/* Nmap machine id of remote machine */
char *fromhostname;			/* PRINCIPAL name of remote machine */
short mapbyuid;				/* Boolean */
{
	uid_t touid;			/* Mapped user-id */
	char dotrhostspath [256];	/* Users .rhost pathname */
	register struct passwd *passwd;	/* Password entry for account */
	static char toname[MAX_ACCOUNTNAME];
	short local_name_exists;


	
	/*
	 *	remove if mapbyuid according to Intergraph.
	 *	Validation will occur by username and userid 
	 *
	 * if (mapbyuid) {
	 */

	/*
	 *  The selected mapping algorithm operates by user-id.
	 */
	switch (nmap_get_uid (frommid, fromuid, &touid)) {
		case NMAP_SUCCESS:
		case NMAP_DEFMAP:
			return (fetchpwuid (touid));
	}

	/*	Remove according to Intergraph
	 *	return ((struct passwd *) 0);
	 */

	/*
	 *  The selected mapping algorithm operates by name.
	 */
	local_name_exists = (passwd = fetchpwnam (fromusername)) 
		!= (struct passwd *) 0; 
		/*
		 *  The remote user has no account on this
		 *  machine (or the account names are different).
		 *  then local_name_exists is true.
		return ((struct passwd *) 0);
		 */
	if (fromuid != 0 && strcmp (fromusername, "root")) {
		/*
		 *  We are not mapping root.
		 *  Examine the /etc/hosts.equiv file for machine
		 *  equivalency.
		 */
		if (access ("/etc/hosts.equiv", fromhostname,
			   (struct passwd *) 0)) {
			return (passwd);	/* Access allowed */
		}
	}
	/*
	 *  Determine if the user's account specifically allows access.
	 */
	if (local_name_exists) {
		strcpy (dotrhostspath, passwd->pw_dir);
		strcat (dotrhostspath, "/.rhosts");
		if (access (dotrhostspath, fromhostname, passwd)) {
			return (passwd);		/* Access allowed */
		}
	}

	/* 
	 * Determine if /etc/hosts.nqs has a mapping
	 */
	switch(map_user_name(fromhostname, fromusername, toname)) {
	case 0:

		if (Debug > 0)
			fprintf(stderr,"I$map_user_name: %s@%s -> %s \n",
				fromusername,fromhostname,toname);
		if ((passwd = fetchpwnam(toname)) != (struct passwd *) 0)
			return (passwd);
		break;
	case -1:
		if (Debug > 0)
			fprintf(stderr,"D$map_user_name: machine not found \n");
		break;
	case -2:
		if (Debug > 0)
			fprintf(stderr,"D$map_user_name: user not found \n");
		break;
	case -3:
		if (Debug >0)
			fprintf(stderr,
			     "D$map_user_name: cannot open /etc/hosts.nqs\n");

		break;
	}
	return ((struct passwd *) 0);
 }



/*** access
 *
 *
 *	int access():
 *
 *	Return non-zero if the specified Berkeley style access file
 *	grants access to the specified remote account.
 *
 *	If the password entry parameter:  passwd  is non-null, then
 *	the access equivalency file being scanned MUST contain lines
 *	of the form:
 *
 *		machinename username
 *
 *	and any such access equivalency file MUST be owned by the
 *	account identified by the given password entry.
 */
static int access (accessfname, fromhostname, passwd)
char *accessfname;			/* Access file name */
char *fromhostname;			/* Name of client machine */
register struct passwd *passwd;		/* Password file entry */
{
	struct stat statbuf;		/* Stat() buffer */
	char accesshost [256];		/* Access file line buffer */
	register FILE *accessfile;	/* Access file */
	register char *cp;
	register char *user;

	accessfile = fopen (accessfname, "r");
	if (passwd != (struct passwd *) 0) {
		 
	         if (accessfile == (FILE *) 0) { 
		   /*
		    * sharon eisner gillett
		    * thinking machines corp.
		    * sharon@think.com
                    * fix for .rhosts file couldn't be opened (doesn't exists)
		    */
                   return(0);
		 }

		/*
		 *  Verify ownership of the .rhosts file, and ensure
		 *  that (on Berkeley systems), that the file is not
		 *  a symbolic link.
		 */
		if (fstat (fileno (accessfile), &statbuf) == -1) return (0);
#if	SGI | SYS52 | UNICOS | UTS | OSF
		if (statbuf.st_uid != passwd->pw_uid) return (0);
#else
#if	BSD42 | BSD43 | ULTRIX
		if (statbuf.st_uid != passwd->pw_uid ||
		   (statbuf.st_mode & S_IFMT) == S_IFLNK) {
			return (0);
		}
#else
BAD SYSTEM TYPE
#endif
#endif
	}
	
	if (accessfile != (FILE *) 0) {
		/*
		 *  The access file was successfully opened.
		 */
		while (fgets (accesshost, sizeof (accesshost),
			      accessfile) != (char *) 0) {
			/*
			 *  We have an access file line to interpret.
			 */
#if	BSD42 | BSD43 | ULTRIX
			cp = index (accesshost, '\n');
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
			cp = strchr (accesshost, '\n');
#else
BAD SYSTEM TYPE
#endif
#endif
			if (cp != (char *) 0) *cp = '\0';
			if (passwd != (struct passwd *) 0) {
				/*
				 *  We are inspecting a user account
				 *  .rhost file.  A username field is
				 *  required.
				 */
#if	BSD42 | BSD43 | ULTRIX
				user = index (accesshost, ' ');
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
				user = strchr (accesshost, ' ');
#else
BAD SYSTEM TYPE
#endif
#endif
				if (user != (char *) 0) {
					/*
					 *  A username field is present
					 *  (as required).
					 */
					*user++ = '\0';
					if (!strcmp (accesshost, fromhostname)&&
					    !strcmp (user, passwd->pw_name)) {
						return (1);
					}
				}
			}
			else if (!strcmp (accesshost, fromhostname)) {
				/*
				 *  Client account is at an equivalent host.
				 */
				return (1);
			}
		}
	}
	return (0);			/* No access */
}


 /*** map_user_name
  *
  *
  *
  *    int map_user_name()
  *
  *    Return non-zero if the /etc/hosts.nqs file
  *    grants access to the specified remote account
  */
 int map_user_name (machine, user, toname)
 char *machine;                                /* remote machine name */
 char *user;                           /* remote user name */
 char *toname;                         /* mapped user name */
 {
       FILE *accessfile;               /* hosts.nqs file */
       static char buffer[BUFSIZ];     /* hosts.nqs line buffer */
       char *cp;                       /* current line */
       char *paren;                    /* paren found */
       short list;                     /* true if a list of names */
       int found = 0;                  /* true if name found */

       accessfile = fopen ("/etc/hosts.nqs", "r");
       if (accessfile == (FILE *) 0) return(-3);
       while (fgets(buffer,sizeof(buffer),accessfile)) {
               cp = strchr(buffer, '\n');
               if (cp) *cp = '\0';
               cp = strtok(buffer," \t");
               switch (*cp) {
               case '(':       /* this is a list of machines */
                       cp++;
                       do {
                               paren = strchr(cp,')');
                               if (paren) *paren = '\0';
                               if (strcmp(machine, cp) == 0) {
                                       found++;
                               }
                               if (!paren) cp = strtok((char *)0," \t");
                       } while (!paren);
                       if (found && finduser(user, toname)==0)
                               return(0);
                       break;
               case '*':
                       if (finduser(user, toname)==0)
                               return(0);
                       break;
               case '#':
               case '\n':

               case '\0':
                       break;
               default:
                       if (strcmp(machine, cp) == 0) {
                               if (finduser(user, toname)==0)
                                       return(0);
                       }
                       break;
               }
       }
       return (-1);    /* not found */
 }


 /*** finduser
  *
  *
  *    int finduser();
  *
  *    Returns the mapped user in toname if the user is found
  *    in the current line
  *
  *    Returns
  *     0:     if the current line grants access
  *    -2:     if the user is not found
  */
 int finduser(user, toname)
 char *user, *toname;
 {
       char *cp, *paren, *equal;
       int found = 0;
       int userlist = 0;

       while(cp = strtok((char *)0, " \t")) {
               userlist++;
               switch (*cp) {
               case '(':       /* is this a list of users? */
                       cp++;
                       do {
                               paren = strchr(cp,')');
                               if (paren) *paren = '\0';
                               if (strcmp(user, cp) == 0) found++;
                               if (!paren) cp = strtok((char *)0," \t");
                       } while (cp && !paren);
                       if (found) {
                               equal = strchr(paren+1,'=');
                               if (equal)

                                       strcpy(toname,equal+1);
                               else
                                       strcpy(toname,user);
                               return(0);
                       }
                       break;
               case '*':
                       if (*(cp+1) == '=')
                               strcpy(toname,cp+2);
                       else
                               strcpy(toname,user);
                       return(0);
                       break;
               default:
                       equal = strchr(cp,'=');
                       if (equal) *equal = '\0';
                       if (strcmp(user, cp) == 0) {
                               if (equal)
                                       strcpy(toname,equal+1);
                               else
                                       strcpy(toname,user);
                               return(0);
                       }
                       break;
               }
       }
       if (!found && !userlist) {
               strcpy(toname,user);
               return(0);
       }
       else
               return(-2);
}
#else
BAD NETWORK TYPE
#endif	STREAM_SOCKETS
#endif	NETWORKED


