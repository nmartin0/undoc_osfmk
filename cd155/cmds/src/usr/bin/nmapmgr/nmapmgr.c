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
 
/*++ nmapmgr.c - Network Queueing System substitute Network Mapping manager
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/nmapmgr/nmapmgr.c,v $
 *
 * DESCRIPTION:
 *
 *	Provide a very poor version of the real NPSN network mapping
 *	manager program for non-NPSN systems.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *
 * REVISION HISTORY: ($Revision: 1.2 $ $Date: 1994/11/19 01:32:06 $ $State: Exp $)
 * $Log: nmapmgr.c,v $
 * Revision 1.2  1994/11/19  01:32:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/09/24  19:25:00  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:57  root
 * Version 2.0 Source
 * 
 * 
 * Revision 2.3  87/08/25  16:56:13  hender
 * uid_t gid_t now defined in /usr/netinclude/sys/types.h (4.3)
 * 
 * Revision 2.2  87/04/22  14:59:50  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nmapmgr.c	1.2 (nmapmgr.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <ctype.h>
#include NETDB
#if	BSD43 | UTS | SGI
#include <sys/types.h>			/* Uid_t, gid_t */
#else
#if	BSD42 | SYS52 | ULTRIX | UNICOS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
#include NMAP				/* Mid_t (all OS's) */
					/* Uid_t and gid_t (if not BSD43) */

#define	MAX_LINESIZ	255
#define	MAX_CMD		31

extern void nmap_ctl ();
extern int nmap_get_uid ();
extern int nmap_get_gid ();
extern int nmap_get_mid ();
extern char *nmap_get_nam ();
extern int nmap_add_uid ();
extern int nmap_add_gid ();
extern int nmap_set_defuid ();
extern int nmap_set_defgid ();
extern int nmap_add_nam ();
extern int nmap_add_mid ();
extern int nmap_chg_mid ();
extern int nmap_del_uid ();
extern int nmap_del_gid ();
extern int nmap_del_defuid ();
extern int nmap_del_defgid ();
extern int nmap_del_nam ();
extern int nmap_del_mid ();
extern int nmap_create ();


/*** main
 *
 *	main (): The main routine of nmapmgr.
 *
 *
 */
main ()
{
	char cmdbuf [MAX_LINESIZ];

	umask (0);			/* CRITICAL */
	for (;;) {
		printf ("NMAPMGR>: ");
		if (fgets (cmdbuf, MAX_LINESIZ, stdin) == NULL) {
			putchar ('\n');
			if (feof (stdin)) exit (0);
			printf ("NMAPMGR error reading command input.\n");
			exit (1);
		}
		if (cmdbuf [strlen (cmdbuf)-1] != '\n') {
			printf ("NMAPMGR command line too long.\n");
			exit (1);
		}
		cmdbuf [strlen (cmdbuf)-1] = '\0';	/* Zap '\n' */
		docmd (cmdbuf);
	}
}


/*** docmd
 *
 *	static docmd (): Do command.
 *
 *
 */
static docmd (cmdbuf)
char *cmdbuf;
{
	long getint ();

	struct hostent hostent;
	char *arg [MAX_CMD+1];
	int tokens;
	char *ch;
	int res;
	int nfds;
	mid_t from_mid;
	uid_t from_uid;
	gid_t from_gid;
	mid_t to_mid;
	uid_t to_uid;
	gid_t to_gid;
	mid_t mid;
	uid_t defuid;
	gid_t defgid;

	/*
 	 *  Break command into tokens.
	 */
	for (tokens = 0; tokens <= MAX_CMD; tokens++) arg [tokens] = NULL;
	tokens = 0;
	ch = cmdbuf;
	while (*ch && tokens < MAX_CMD) {
		while (*ch && isspace (*ch)) ch++;
		if (*ch) {
			arg [tokens++] = ch;	/* Save ptr to arg */
			while (*ch && !(isspace (*ch))) ch++;
			if (*ch) *ch++ = '\0';	/* Null terminate */
		}
	}
	/* Bail out if no data */
	if (tokens == 0)
		return;
	/*
	 *  Perform the command.
	 */
	if (strequ (arg [0], "Add")) {
		if (tokens == 1) printf ("Missing verb modifier.\n");
		else if (strequ (arg [1], "Uid")) {
			if (tokens >= 5) {
				if ((from_mid = getint (arg [2])) >= 0 &&
				    (from_uid = getint (arg [3])) >= 0 &&
				    (to_uid = getint (arg [4])) >= 0) {
					showret (nmap_add_uid (from_mid,
							       from_uid,
							       to_uid));
				}
			}
			else printf ("Missing arguments.\n");
		}
		else if (strequ (arg [1], "Gid")) {
			if (tokens >= 5) {
				if ((from_mid = getint (arg [2])) >= 0 &&
				    (from_gid = getint (arg [3])) >= 0 &&
				    (to_gid = getint (arg [4])) >= 0) {
					showret (nmap_add_gid (from_mid,
							       from_gid,
							       to_gid));
				}
			}
			else printf ("Missing arguments.\n");
		}
		else if (strequ (arg [1], "Name")) {
			if (tokens >= 4) {
				if ((to_mid = getint (arg [3])) >= 0) {
					showret (nmap_add_nam (arg [2],to_mid));
				}
			}
			else printf ("Missing arguments.\n");
		}
		else if (strequ (arg [1], "Mid")) {
			if (tokens >= 4) {
				if ((mid = getint (arg [2])) >= 0) {
					showret (nmap_add_mid (mid, arg [3]));
				}
			}
			else printf ("Missing arguments.\n");
		}
		else printf ("Unrecognized command verb modifier.\n");
	}
	else if (strequ (arg [0], "CHange")) {
		if (tokens == 1) printf ("Missing verb modifier.\n");
		else if (strequ (arg [1], "Name")) {
			if (tokens >= 4) {
				if ((mid = getint (arg [2])) >= 0) {
					showret (nmap_chg_mid (mid, arg [3]));
				}
			}
			else printf ("Missing arguments.\n");
		}
		else printf ("Unrecognized command verb modifier.\n");
	}
	else if (strequ (arg [0], "CReate")) {
		showret (nmap_create());
	}
	else if (strequ (arg [0], "Delete")) {
		if (tokens == 1) printf ("Missing verb modifier.\n");
		else if (strequ (arg [1], "Uid")) {
			if (tokens >= 4) {
				if ((from_mid = getint (arg [2])) >= 0 &&
				    (from_uid = getint (arg [3])) >= 0) {
					showret (nmap_del_uid (from_mid,
							       from_uid));
				}
			}
			else printf ("Missing arguments.\n");
		}
		else if (strequ (arg [1], "Gid")) {
			if (tokens >= 4) {
				if ((from_mid = getint (arg [2])) >= 0 &&
				    (from_gid = getint (arg [3])) >= 0) {
					showret (nmap_del_gid (from_mid,
							       from_gid));
				}
			}
			else printf ("Missing arguments.\n");
		}
		else if (strequ (arg [1], "DEFUid")) {
			if (tokens >= 3) {
				if ((from_mid = getint (arg [2])) >= 0) {
					showret (nmap_del_defuid (from_mid));
				}
			}
			else printf ("Missing <mid>.\n");
		}
		else if (strequ (arg [1], "DEFGid")) {
			if (tokens >= 3) {
				if ((from_mid = getint (arg [2])) >= 0) {
					showret (nmap_del_defgid (from_mid));
				}
			}
			else printf ("Missing <mid>.\n");
		}
		else if (strequ (arg [1], "Name")) {
			if (tokens >= 3) {
				showret (nmap_del_nam (arg [2]));
			}
			else printf ("Missing <name>.\n");
		}
		else if (strequ (arg [1], "Mid")) {
			if (tokens >= 3) {
				if ((mid = getint (arg [2])) >= 0) {
					showret (nmap_del_mid (mid));
				}
			}
			else printf ("Missing <mid>.\n");
		}
		else printf ("Unrecognized command verb modifier.\n");
	}
	else if (strequ (arg [0], "Exit")) exit (0);
	else if (strequ (arg [0], "Get")) {
		if (tokens == 1) printf ("Missing verb modifier.\n");
		else if (strequ (arg [1], "Uid")) {
			if (tokens >= 4) {
				if ((from_mid = getint (arg [2])) >= 0 &&
				    (from_uid = getint (arg [3])) >= 0) {
					res = nmap_get_uid (from_mid, from_uid,
							    &to_uid);
					showret (res);
					if (res >= NMAP_SUCCESS) {
						printf ("Mapped uid = %d.\n",
							to_uid);
					}
				}
			}
			else printf ("Missing arguments.\n");
		}
		else if (strequ (arg [1], "Gid")) {
			if (tokens >= 4) {
				if ((from_mid = getint (arg [2])) >= 0 &&
				    (from_gid = getint (arg [3])) >= 0) {
					res = nmap_get_gid (from_mid, from_gid,
							    &to_gid);
					showret (res);
					if (res >= NMAP_SUCCESS) {
						printf ("Mapped gid = %d.\n",
							to_gid);
					}
				}
			}
			else printf ("Missing arguments.\n");
		}
		else if (strequ (arg [1], "Mid")) {
			if (tokens >= 3) {
				hostent.h_name = arg [2];
				hostent.h_aliases = arg+3;
				res = nmap_get_mid (&hostent, &mid);
				showret (res);
				if (res == NMAP_SUCCESS) {
					printf ("Mid = %d.\n", mid);
				}
			}
			else printf ("Missing <name(s)>.\n");
		}
		else if (strequ (arg [1], "Name")) {
			if (tokens >= 3) {
				if ((mid = getint (arg [2])) >= 0) {
					arg [0] = nmap_get_nam (mid);
					if (arg [0] == NULL) {
						printf ("No such mid.\n");
					}
					else printf ("Name = %s.\n", arg[0]);
				}
			}
			else printf ("Missing <mid>.\n");
		}
		else printf ("Unrecognized command verb modifier.\n");
	}
	else if (strequ (arg [0], "Help")) {
		putchar ('\n');
		printf ("Commands:\n");
		printf ("  Add Uid <from_mid> <from_uid> <to_uid>\n");
		printf ("  Add Gid <from_mid> <from_gid> <to_gid>\n");
		printf ("  Add Name <name> <to_mid>\n");
		printf ("  Add Mid <mid> <principal-name>\n");
		printf ("  CHange Name <mid> <new-name>\n");
		printf ("  CReate\n");
		printf ("  Delete Uid <from_mid> <from_uid>\n");
		printf ("  Delete Gid <from_mid> <from_gid>\n");
		printf ("  Delete DEFUid <from_mid>\n");
		printf ("  Delete DEFGid <from_mid>\n");
		printf ("  Delete Name <name>\n");
		printf ("  Delete Mid <mid>\n");
		printf ("  Exit\n");
		printf ("  Get Uid <from_mid> <from_uid>\n");
		printf ("  Get Gid <from_mid> <from_gid>\n");
		printf ("  Get Mid <name>\n");
		printf ("  Get Name <mid>\n");
		printf ("  Help\n");
		printf ("  Quit\n");
		printf ("  Set Nfds <#-of-file-descrs>\n");
		printf ("  Set DEFUid <from_mid> <defuid>\n");
		printf ("  Set DEFGid <from_mid> <defgid>\n");
		printf (" ^D (to exit nmapmgr.\n");
	}
	else if (strequ (arg [0], "Quit")) exit (0);
	else if (strequ (arg [0], "Set")) {
		if (tokens == 1) printf ("Missing verb modifier.\n");
		else if (strequ (arg [1], "Nfds")) {
			if (tokens >= 3) {
				if ((nfds = getint (arg [2])) >= 0) {
					nmap_ctl (NMAP_OPNOFD, nfds);
				}
			}
			else printf ("Missing <#-of-file-descrs>.\n");
		}
		else if (strequ (arg [1], "DEFUid")) {
			if (tokens >= 4) {
				if ((from_mid = getint (arg [2])) >= 0 &&
				    (defuid = getint (arg [3])) >= 0) {
					showret (nmap_set_defuid (from_mid,
							          defuid));
				}
			}
			else printf ("Missing arguments.\n");
		}
		else if (strequ (arg [1], "DEFGid")) {
			if (tokens >= 4) {
				if ((from_mid = getint (arg [2])) >= 0 &&
				    (defgid = getint (arg [3])) >= 0) {
					showret (nmap_set_defgid (from_mid,
								  defgid));
				}
			}
			else printf ("Missing arguments.\n");
		}
		else printf ("Unrecognized command verb modifier.\n");
	}
	else if (tokens >= 1) printf ("Unrecognized command verb: %s\n",
				      arg [0]);
}


/*** strequ
 *
 *	int strequ (str1, str2)
 *	char *str1;
 *	char *str2;
 *
 *	Return 1 if the two null-terminated strings are equal.
 *	Otherwise return 0.  This comparison however uses uppercase
 *	characters in str2 to denote acceptable abbreviations.
 *	Thus:
 *
 *		strequ ("defg", "DEFGid") = 1
 *
 *	while:
 *
 *		strequ ("def", "DEFGid") = 0
 */
static int strequ (str1, str2)
char *str1;
char *str2;
{
	register char ch1;
	register char ch2;

	while (ch1 = *str1++) {
		ch2 = *str2++;
		if (isupper (ch1)) ch1 += 'a' - 'A';
		if (isupper (ch2)) ch2 += 'a' - 'A';
		if (ch1 != ch2) return (0);	/* Strings not equal */
	}
	if (isupper (*str2)) return (0);/* Strings not equal */
	return (1);			/* Strings equal */
}
 

/*** getint
 *
 *	long getint (ptr)
 *	char *ptr;
 *
 *	Return integer value of unsigned sequence of decimal ASCII
 *	digits referenced by ptr.  Print-out an error message and
 *	return a -1 if no sequence of decimal digits exists.
 */
static long getint (ptr)
char *ptr;
{
	register int i;

	if (isdigit (*ptr)) {
		i = 0;
		while (isdigit (*ptr)) {
			i *= 10;
			i += *ptr++ - '0';
		}
		if (i < 0) {
			printf ("Integer overflow.\n");
			return (-1);
		}
		return (i);
	}
	else {
		printf ("Unsigned integer number expected.\n");
		return (-1);
	}
}


/*** showret
 *
 *	showret (code)
 *	int code;
 *
 *	Identify the return code from the NMAP library module.
 */
static showret (code)
register int code;
{
	switch (code) {
	case NMAP_SUCCESS:
		printf ("NMAP_SUCCESS:  Successful completion.\n");
		break;
	case NMAP_DEFMAP:
		printf ("NMAP_DEFMAP:  Successful completion using ");
		printf ("default mapping.\n");
		break;
	case NMAP_EUNEXPECT:
		printf ("NMAP_EUNEXPECT:  Fatal error in mapping software.\n");
		break;
	case NMAP_ENOPRIV:
		printf ("NMAP_ENOPRIV:  No privilege for operation.\n");
		break;
	case NMAP_ECONFLICT:
		printf ("NMAP_ECONFLICT:  Already exists.\n");
		break;
	case NMAP_ENOMAP:
		printf ("NMAP_ENOMAP:  No such mapping.\n");
		break;
	case NMAP_ENOMID:
		printf ("NMAP_ENOMID:  No such machine.\n");
		break;
	case NMAP_EBADNAME:
		printf ("NMAP_EBADNAME:  Null name or name too long.\n");
		break;
	default:
		printf ("Invalid NMAP return code.\n");
		break;
	}
}
