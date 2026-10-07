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
 
/*++ mgr_scan.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qmgr/mgr_scan.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS manager program command scanning module.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 01:35:46 $ $State: Exp $)
 * $Log: mgr_scan.c,v $
 * Revision 1.3  1994/11/19  01:35:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:30:13  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:28:28  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:34  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:03:21  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)mgr_scan.c	1.3 (mgr_scan.c OSF/1 NQS2.0 GJK) 6/12/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include "nqs.h"			/* NQS types and definitions */
#include "nqsmgr.h"			/* MAX_LINESIZE and token types */

/*
 *	The parameter:  MAX_LITSIZE  can be configured as necessary.
 *
 *	WARNING:
 *		MAX_LITSIZE MUST be declared to be >= MAX_PATHNAME
 *		and MAX_SERVERNAME.
 */
#define	MAX_LITSIZE	511		/* Maximum length of a literal */
					/* string */

/*
 *	Import machine-id from mgr_main.c
 */
extern mid_t Local_mid;			/* Machine-id of local host */

/*
 *	External functions.
 */
extern void closegrdb();		/* Close group database */
extern void closepwdb();		/* Close account/password database */
extern char *destdev();			/* Return pointer to device name */
					/* portion of a remote device */
					/* specification */
extern char *destqueue();		/* Return pointer to queue name */
					/* portion of a remote queue */
					/* specification */
extern void errormessage();		/* Show parse/syntax error message */
extern struct group *fetchgrnam();	/* Fetch group entry by name */
extern struct passwd *fetchpwnam();	/* Fetch password entry by name */
extern void free();			/* Free heap space */
extern char *malloc();			/* Allocate heap space */
extern int reqspec();			/* Analyze a request-id */
extern int machspec();			/* Return a machine specification */
#if	BSD42 | BSD43 | ULTRIX
extern char *index();			/* Search for char in string */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern char *strchr();			/* Search for char in string */
#else
BAD SYSTEM TYPE
#endif
#endif
extern char *strcpy();			/* String copy */
extern int strlen();			/* String length */


struct cmd_struct {
	struct cmd_struct *more;	/* Ptr to continuation cmd lines */
	char cmd [MAX_LINESIZE+1];	/* Varying length, null terminated */
};					/* single cmd line */

struct set_struct {			/* Enqueued set entity */
	struct set_struct *next;	/* Next queued set element */
	mid_t machine_id;		/* Machine-id specification */
	long uid_gid;			/* Uid OR Gid (-1 if invalid) */
	short mgr_privileges;		/* Manager privilege bits */
	char text [MAX_LITSIZE+1];	/* Null terminated token literal */
};					/* implemented as a varying length */
					/* character array */

static char cmdchar = '\0';		/* Current scan character */
static short wantpath = 0;		/* Non-zero if path mode enabled */
static short wantallinparens = 0;	/* Non-zero: get everything in parens */
					/* into a single literal token */
static short eoc_returned = 1;		/* Non-zero if T_EOC returned for */
					/* command */
static struct cmd_struct *cmd;		/* Current command */
static int scanpos;			/* Current scan position in *curr */
static struct cmd_struct *curr;		/* Current line of cmd being scanned */
static int t_scanpos;			/* Token scan position in the */
static struct cmd_struct *t_curr;	/* command line *t_curr */
static char t_literal [MAX_LITSIZE+1];	/* Null terminated token literal */
static long t_uint;			/* Token unsigned integer value */
static struct set_struct *set_head=NULL;/* Ptr to set queue head */
static struct set_struct *set_tail=NULL;/* Ptr to set queue tail */

static mid_t deqd_mid;				/* Dequeued machine-id */
static long deqd_uid_gid;			/* Dequeued uid or gid */
static short deqd_priv;				/* Dequeue privilege mask */

/*** scan
 *
 *
 *	int scan():
 *	Return the type of the next token in the command.
 *
 *		Possible values are:
 *	[Normal Mode]
 *			T_EOC		( end of command seen)
 *			T_COMMA
 *			T_NEG
 *			T_EQUALS
 *			T_LPAREN
 *			T_RPAREN
 *			T_UINT		( value returned in "t_uint")
 *			T_LITERAL	( pointer returned in "t_literal")
 *			T_ABORT
 *
 *	["wantallin parens" or "wantpath"]
 *			T_EOC
 *			T_LITERAL
 *			T_ABORT
 *
 *	Note that a leading digit in the string indicates an integer, terminated
 *	by white space in the string. Its value is returned in "t_uint".
 *
 *	Any string between a pound sign (#) token and End-of-command is regarded
 *	as COMMENT and is ignored when no special mode is in effect.
 *
 */
int scan()
{
	struct cmd_struct *getcmdline();	/* Get command line fn */
	void new_command();			/* Drop old command and get */
						/* a new one */
	char nextchar();			/* Next character function */
	void scan_error();			/* Display scan error mssg */

	int t_litlen;				/* Literal length */
	register char ch;			/* Scan character */

	ch = cmdchar;				/* Get current scan character */
	if (ch == '\0') {			/* At the end of the current */
						/* command */
		if (eoc_returned) {		/* Previous token returned was*/
						/* T_EOC, so get a new command*/
			eoc_returned = 0;	/* Clear T_EOC flag */
			new_command();		/* Discard the old command */
			cmd = getcmdline();	/* Get command */
			if (cmd != (struct cmd_struct *) 0) {
						/* Initialize scan */
				curr = cmd;	/* Current cmd line */
				scanpos = -1;	/* Current scan position */
				ch = nextchar();/* Load current character */
			}
			else return (T_EOF);	/* No more commands */
		}
		else {				/* We are at the end of the */
						/* command, but have not */
						/* previously returned T_EOC */
			t_scanpos = scanpos;	/* Update scan position */
			eoc_returned = 1;	/* Set T_EOC returned flag */
			return (T_EOC);		/* Inform caller */
		}
	}
	/*
	 *  Return the next token in the command stream.
	 */
	while (isspace (ch)) ch = nextchar();	/* Scan whitespace */
	t_scanpos = scanpos;			/* Save token position */
	t_curr = curr;
	if (wantallinparens) {
		/*
		 *  We are looking for a server, a fixed shell strategy
		 *  shell, a cpu time limit, or a quota limit.
		 *  We will exit this mode upon reaching
		 *  the end of the command, or upon reaching
		 *  the first ")", whichever comes first.
		 */
		t_litlen = 0;			/* No chars in literal */
		/*
		 * MAX_SERVERNAME is really only a limit on servers
		 * and fixed shell strategy shells.
		 * The limit on cpu limits and quota limits is
		 * MAX_LITSIZE.  However, given MAX_SERVERNAME > 20,
		 * this way does no harm.
		 */
		while (ch && ch != ')' && t_litlen < MAX_SERVERNAME) {
			t_literal [t_litlen++] = ch;
			ch = nextchar();
		}
		wantallinparens = 0;	/* Exiting wantallinparens mode */
		if (ch && ch != ')') {
			errormessage (EM_INPARENSTOOLON);	/* too long */
			return (T_ABORT);	
		}
		if (t_litlen == 0) {
			errormessage (EM_INPARENSEXP);	/* something */
			return (T_ABORT);		/* was expected */
		}
		t_literal [t_litlen] = '\0';	/* Add terminating null */
		return (T_LITERAL);
	}
	if (wantpath) {
		/*
		 *  We need a file path.  We will exit this mode upon
		 *  reaching the first whitespace character, or upon
		 *  reaching the end of the command, whichever comes
		 *  first.
		 */
		t_litlen = 0;			/* No characters in literal */
		while (ch && !isspace (ch) && t_litlen < MAX_PATHNAME) {
			t_literal [t_litlen++] = ch;
			ch = nextchar();
		}
		wantpath = 0;			/* Exit wantpath mode */
		if (ch && !isspace (ch)) {
			errormessage (EM_PATSPETOOLON);	/* Path spec */
			return (T_ABORT);		/* too long */
		}
		if (t_litlen == 0) {
			errormessage (EM_PATSPEEXP);	/* Path spec */
			return (T_ABORT);		/* expected */
		}
		t_literal [t_litlen] = '\0';	/* Add terminaing null */
		return (T_LITERAL);
	}
	/*
	 *  No special modes are in effect.
	 */
	switch (ch) {				/* Identify token */
	case '\0':
		eoc_returned = 1;		/* T_EOC returned */
		return (T_EOC);			/* End of command seen */
	case ',':
		nextchar();			/* Step past , */
		return (T_COMMA);		/* , seen */
	case '-':
		nextchar();			/* Step past - */
		return (T_NEG);			/* - seen */
	case '=':
		nextchar();			/* Step past = */
		return (T_EQUALS);		/* = seen */
	case '(':
		nextchar();			/* Step past ( */
		return (T_LPAREN);		/* ( seen */
	case ')':
		nextchar();			/* Step past ) */
		return (T_RPAREN);		/* ) seen */
	case '#':
		while (ch==nextchar() != '\0'); /* Step over */
		eoc_returned = 1;		/* Comment   */
		return (T_EOC);			
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		t_uint = 0;			/* Unsigned integer */
		while (isdigit (ch)) {
			t_uint *= 10;
			t_uint += ch - '0';
			ch = nextchar();	/* Get next char */
		}
		if (ch != ' ' && ch) {
			scan_error ("Number terminator must be a space.");
			return (T_ABORT);
		}
		return (T_UINT);		/* Unsigned integer seen */
	default:				/* Literal found */
		t_litlen = 0;			/* No chars in literal */
		while (ch && !isspace (ch) && ch != ',' && ch != '=' &&
		       ch != '(' && ch != ')' && t_litlen < MAX_LITSIZE) {
			t_literal [t_litlen++] = ch;
			ch = nextchar();
		}
		if (ch && !isspace (ch) && ch != ',' && ch != '=' &&
		    ch != '(' && ch != ')') {
			scan_error ("Literal string is too long.");
			return (T_ABORT);
		}
		t_literal [t_litlen] = '\0';	/* Add terminating null */
		return (T_LITERAL);		/* Literal seen */
	}
}



/*** deq_mid
 *
 *
 *	mid_t deq_mid():
 *	Return the value of the most recently dequeued machine-id.
 */
mid_t deq_mid()
{
	return (deqd_mid);
}


/*** deq_priv
 *
 *
 *	int deq_priv():
 *	Return the value of the most recently dequeued privilege mask.
 */
int deq_priv()
{
	return (deqd_priv);
}


/*** deq_set
 *
 *
 *	char *deq_set():
 *
 *	Dequeue the next text entity, and optional machine-id or privilege
 *	mask from the queued entity set.
 *
 *	Returns:
 *		A pointer to the dequeued text if successful.  Otherwise
 *		NULL is returned to indicate that there aren't any more
 *		entites in the queued set.
 *
 *		After a call to deq_set(), the corresponding value of
 *		the associated machine-id or privilege mask can be
 *		gotten by the respective calls of:
 *
 *			deq_mid()	AND
 *			deq_priv().
 */
char *deq_set ()
{
	static char text [MAX_LITSIZE+1];	/* Space to store dequeued */
						/* literal */
	register struct set_struct *next;

	if (set_head == (struct set_struct *) 0) {
		return ((char *) 0);		/* No more queued literals */
	}
	next = set_head->next;
	deqd_mid = set_head->machine_id;
	deqd_uid_gid = set_head->uid_gid;
	deqd_priv = set_head->mgr_privileges;
	strcpy (text, set_head->text);
	free ((char *) set_head);
	set_head = next;
	return (text);
}


/*** deq_qual_type
 *
 *
 *	long deq_qual_type():
 *
 *	Return the value of the most recently dequeued qualifier type.
 *	
 */
long deq_qual_type()
{
	return (deqd_uid_gid);
}


/*** deq_uid_gid
 *
 *
 *	long deq_uid_gid():
 *
 *	Return the value of the most recently dequeued account-name
 *	user-id.  (Note: this function returns -1 if the account-name
 *	specified a non-existent account).
 */
long deq_uid_gid()
{
	return (deqd_uid_gid);
}


/*** get_literal
 *
 *
 *	char *get_literal():
 *	Return pointer to the current literal string.
 */
char *get_literal()
{
	return (t_literal);
}


/*** matchkeyword
 *
 *
 *	int matchkeyword():
 *
 *	Test the first argument string to see if it matches the second
 *	keyword argument string.  Both strings must be null-terminated.
 *	The comparison is case-insensitive.  However, the first N
 *	uppercase characters of testmatch determine the minimal abbrev-
 *	iation that will be recognized as matching the keyword.
 *
 *	Returns:
 *		1: If the match was successful;
 *		0: If no match at all;
 *	       -1: If an ambiguous match was made.  In this case,
 *		   the test argument string: maybekeyword was a
 *		   prefix of the minimal abbreviation as specified
 *		   in testmatch.
 */
int matchkeyword (maybekeyword, testmatch)
char *maybekeyword;
char *testmatch;
{
	register char ch1;
	register char ch2;

	while ((ch1 = *maybekeyword++) != '\0') {
		ch2 = *testmatch++;
		if (isupper (ch1)) ch1 = tolower (ch1);
		if (isupper (ch2)) ch2 = tolower (ch2);
		if (ch1 != ch2) return (0);	/* No match */
	}
	if (*testmatch == '_') {
		testmatch--;
		if (isupper (*testmatch)) {
			return (-1);		/* Ambiguous keyword */
		}
		return (1);			/* Success; we have a match */
	}
	if (isupper (*testmatch)) return (-1);	/* Ambiguous keyword */
	return (1);				/* Success; we have a match */
}


/*** new_command
 *
 *
 *	void new_command():
 *	Discard the current command and get a new one.
 */
void new_command()
{
	struct cmd_struct *dropcmd ();	/* Drop current command */
	
	register struct set_struct *next;

	cmd = dropcmd (cmd);		/* Initialize for new command */
	while (set_head != (struct set_struct *) 0) {
					/* Delete the current set */
		next = set_head->next;	/* Get next element */
		free ((char *)set_head);/* Free the current set element */
		set_head = next;	/* Get the next one */
	}
	set_tail = (struct set_struct *) 0;
}


/*** scan_allinparens
 *
 *
 *	char *scan_allinparens():
 *
 *	Scan characters up to a right parenthesis into a single literal token.
 *	This is used for servers, fixed shell strategy shells,
 *	cpu time limits, and quota limits.
 *
 *	Returns:
 *		A pointer to the literal token if successful.  Otherwise
 *		a NULL pointer is returned to indicate that the contents
 *		were too long, were invalid, or were not present in
 *		the command stream.  In the failure case, an error
 *		message will have already been displayed.
 */
char *scan_allinparens ()
{
	register int token_type;		/* Scan token type */

	if ((token_type = scan()) == T_ABORT) return ((char *) 0);
	if (token_type != T_LPAREN) {
		errormessage (EM_LEFPAREXP);	/* "(" expected */
		return ((char *) 0);
	}
	wantallinparens = 1;			/* Set wantallinparens mode */
	if ((token_type = scan()) == T_ABORT) return ((char *) 0);
	/*
	 *  Token type is guaranteed to be a literal.
	 */
	if ((token_type = scan()) == T_ABORT) return ((char *) 0);
	if (token_type != T_RPAREN) {
		errormessage (EM_RIGPAREXP);	/* ")" expected */
		return ((char *) 0);
	}
	return (t_literal);
}


/*** scan_aname
 *
 *
 *	char *scan_aname():
 *	Scan a LOCAL account name.
 *
 *	Returns:
 *		A pointer to the account name if successful.  Otherwise
 *		a NULL pointer is returned to indicate that the account
 *		name was too long, or was not present in the command
 *		stream.  In the failure case, an error message will have
 *		already been displayed.
 */
char *scan_aname ()
{
	register int token_type;	/* Scan token type */

	if ((token_type = scan()) == T_ABORT) return ((char *) 0);
	if (token_type != T_LITERAL) {
		errormessage (EM_ACCNAMEXP);
		return ((char *) 0);
	}
	if (strlen (t_literal) > MAX_ACCOUNTNAME) {
		errormessage (EM_ACCNAMTOOLON);
		return ((char *) 0);
	}
	return (t_literal);
}


/*** scan_dename
 *
 *
 *	char *scan_dename():
 *	Scan a device name or equals sign '='.
 *
 *	Returns:
 *		A pointer to the device name or equals sign character
 *		('=') if successful.  Otherwise a NULL pointer is
 *		returned to indicate that the device name was too long,
 *		was invalid, or no device name or '=' was present in
 *		the command stream.  In the failure case, an error
 *		message will have already been displayed.
 */
char *scan_dename ()
{
	register int token_type;	/* Scan token type */

	if ((token_type = scan()) == T_ABORT) return ((char *) 0);
	if (token_type != T_LITERAL) {
		if (token_type == T_EQUALS) {
			t_literal [0] = '=';
			t_literal [1] = '\0';
			return (t_literal);
		}
		errormessage (EM_DEVNAMOREQUEXP);
		return ((char *) 0);
	}
	if (strlen (t_literal) > MAX_DEVNAME) {
		errormessage (EM_DEVNAMTOOLON);
		return ((char *) 0);
	}
#if	BSD42 | BSD43 | ULTRIX
	if (index (t_literal, '@') != (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	if (strchr (t_literal, '@') != (char *) 0) {
#else
BAD SYSTEM TYPE
#endif
#endif
		errormessage (EM_INVDEVNAMSPE);
		return ((char *) 0);
	}
	return (t_literal);
}


/*** scan_destset
 *
 *
 *	int scan_destset():
 *
 *	Scan a set of queue destinations in one of the following forms:
 *
 *		<destination>		OR
 *		( <destination>, .... )
 *
 *	The set of scanned destinations is queued via the enqueue_set()
 *	function, and can be read-out in order of appearance via the
 *	dequeue_set() function.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if an error occurred.
 */
int scan_destset ()
{
	mid_t machine_id;		/* Machine-id for destination */
	register int token;

	if ((token = scan()) == T_ABORT) return (-1);
	if (token == T_LPAREN) {
		do {
			if ((token = scan()) == T_ABORT) return (-1);
			if (token != T_LITERAL) {
				errormessage (EM_DESNAMEXP);
				return (-1);	/* was expected */
			}
			switch (machspec (t_literal, &machine_id)) {
			case -1:
				errormessage (EM_INVDESSYN);
				return (-1);
			case -2:
				errormessage (EM_NOSUCMAC);
				return (-1);
			case -3:
				errormessage (EM_INSNMAPRI);
				return (-1);
			case -4:
				errormessage (EM_GENNMAERR);
				return (-1);
			}
			if (strlen (destqueue (t_literal)) > MAX_QUEUENAME) {
				errormessage (EM_DESNAMTOOLON);
				return (-1);	/* is too long */
			}
			if (enqueue_set (t_literal, machine_id, 0, 0) == -1) {
				/*
				 *  Insufficient memory.  Message has
				 *  been displayed.
				 */
				return (-1);
			}
		} while ((token = scan()) == T_COMMA);
		if (token == T_ABORT) return (-1);
		if (token != T_RPAREN) {
			errormessage (EM_RIGPARORCOMEXP);
			return (-1);
		}
	}
	else if (token != T_LITERAL) {
		errormessage (EM_DESNAMORLEFPAREXP);
		return (-1);
	}
	else {
		switch (machspec (t_literal, &machine_id)) {
		case -1:
			errormessage (EM_INVDESSYN);
			return (-1);
		case -2:
			errormessage (EM_NOSUCMAC);
			return (-1);
		case -3:
			errormessage (EM_INSNMAPRI);
			return (-1);
		case -4:
			errormessage (EM_GENNMAERR);
			return (-1);
		}
		if (strlen (destqueue (t_literal)) > MAX_QUEUENAME) {
			errormessage (EM_DESNAMTOOLON);
			return (-1);	/* is too long */
		}
		if (enqueue_set (t_literal, machine_id, 0, 0) == -1) {
			return (-1);
		}
	}
	return (0);
}


/*** scan_devset
 *
 *
 *	int scan_devset():
 *	Scan a device set in one of the following forms:
 *
 *		<device-name>		OR
 *		( <device-name>, .... )
 *
 *	The set of scanned devices is queued via the enqueue_set()
 *	function, and can be read-out in order of appearance via the
 *	dequeue_set() function.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if an error occurred.
 */
int scan_devset ()
{
	register int token;

	if ((token = scan()) == T_ABORT) return (-1);
	if (token == T_LPAREN) {
		do {
			if ((token = scan()) == T_ABORT) return (-1);
			if (token != T_LITERAL) {
				errormessage (EM_DEVNAMEXP);
				return (-1);
			}
			if (strlen (t_literal) > MAX_DEVNAME) {
				errormessage (EM_DEVNAMTOOLON);
				return (-1);
			}
#if	BSD42 | BSD43 | ULTRIX
			if (index (t_literal, '@') != (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
			if (strchr (t_literal, '@') != (char *) 0) {
#else
BAD SYSTEM TYPE
#endif
#endif
				errormessage (EM_INVDEVNAMSPE);
				return (-1);
			}
			if (enqueue_set (t_literal, (mid_t) 0, 0, 0) == -1) {
				/*
				 *  If there is not sufficient heap space,
				 *  then we wind up here, with a message
				 *  having already been displayed....
				 */
				return (-1);
			}
		} while ((token = scan()) == T_COMMA);
		if (token == T_ABORT) return (-1);
		if (token != T_RPAREN) {
			errormessage (EM_RIGPARORCOMEXP);
			return (-1);
		}
	}
	else if (token != T_LITERAL) {
		errormessage (EM_DEVNAMORLEFPAREXP);
		return (-1);
	}
	else {
		if (strlen (t_literal) > MAX_DEVNAME) {
			errormessage (EM_DEVNAMTOOLON);
			return (-1);
		}
#if	BSD42 | BSD43 | ULTRIX
		if (index (t_literal, '@') != (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
		if (strchr (t_literal, '@') != (char *) 0) {
#else
BAD SYSTEM TYPE
#endif
#endif
			errormessage (EM_INVDEVNAMSPE);
			return (-1);
		}
		if (enqueue_set (t_literal, (mid_t) 0, 0, 0) == -1) {
			return (-1);
		}
	}
	return (0);
}


/*** scan_dfname
 *
 *
 *	char *scan_dfname():
 *	Scan a device-fullname.
 *
 *	Returns:
 *		A pointer to the fullname if successful.  Otherwise
 *		a NULL pointer is returned to indicate that the full-
 *		name was too long, was, invalid, or was not present
 *		in the command stream.  In the failure case, an error
 *		message will have already been displayed.
 */
char *scan_dfname ()
{
	register int token_type;	/* Scan token type */

	if ((token_type = scan()) == T_ABORT) return ((char *) 0);
	if (token_type != T_LITERAL) {
		errormessage (EM_DEVFULNAMEXP);
		return ((char *) 0);
	}
	if (strlen (t_literal) > MAX_PATHNAME) {
		errormessage (EM_DEVFULNAMTOOLON);
		return ((char *) 0);
	}
#if	BSD42 | BSD43 | ULTRIX
	if (index (t_literal, '@') != (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	if (strchr (t_literal, '@') != (char *) 0) {
#else
BAD SYSTEM TYPE
#endif
#endif
		errormessage (EM_INVDEVFULNAMSPE);
		return ((char *) 0);
	}
	return (t_literal);
}


/*** scan_dname
 *
 *
 *	char *scan_dname():
 *	Scan a device name.
 *
 *	Returns:
 *		A pointer to the device name if successful.  Otherwise
 *		a NULL pointer is returned to indicate that the device
 *		name was too long, was invalid, or was not present in
 *		the command stream.  In the failure case, an error
 *		message will have already been displayed.
 */
char *scan_dname ()
{
	register int token_type;	/* Scan token type */

	if ((token_type = scan()) == T_ABORT) return ((char *) 0);
	if (token_type != T_LITERAL) {
		errormessage (EM_DEVNAMEXP);
		return ((char *) 0);
	}
	if (strlen (t_literal) > MAX_DEVNAME) {
		errormessage (EM_DEVNAMTOOLON);
		return ((char *) 0);
	}
#if	BSD42 | BSD43 | ULTRIX
	if (index (t_literal, '@') != (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	if (strchr (t_literal, '@') != (char *) 0) {
#else
BAD SYSTEM TYPE
#endif
#endif
		errormessage (EM_INVDEVNAMSPE);
		return ((char *) 0);
	}
	return (t_literal);
}


/*** scan_end
 *
 *
 *	int scan_end():
 *	Scan end of command.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if there are more characters/tokens in the command,
 *		   in which case an appropriate error message is
 *		   displayed.
 */
int scan_end ()
{
	register int token;

	if ((token = scan()) == T_ABORT) return (-1);
	if (token != T_EOC) {
		errormessage (EM_UNECHAPASENDOFVALCOM);
		return (-1);
	}
	return (0);			/* Success */
}


/*** scan_equals
 *
 *
 *	int scan_equals():
 *	Scan an equals sign character: '='.
 *
 *	Returns:
 *		0: if successful.
 *	       -1: if unsuccessful, in which case the appropriate
 *		   error message will have already been displayed.
 */
int scan_equals ()
{
	register int token_type;	/* Scan token type */

	if ((token_type = scan()) == T_ABORT) return (-1);
	if (token_type != T_EQUALS) {
		errormessage (EM_EQUEXP);
		return (-1);
	}
	return (0);
}


/*** scan_error
 *
 *
 *	void scan_error():
 *	Display a command scan error message diagnostic.
 */
void scan_error (text)
char *text;				/* Diagnostic message text */
{
	void new_command();		/* Get new command */

	register struct cmd_struct *curr;
	register int i;

	curr = cmd;
	printf ("Syntax/scan error:\n");
	while (curr != (struct cmd_struct *) 0) {
		fputs (curr->cmd, stdout);
		if (curr->more != (struct cmd_struct *) 0) putchar ('\\');
		putchar ('\n');
		if (curr == t_curr) {
			/*
			 *  This is the line with the diagnostic.
			 */
			i = t_scanpos;
			while (i--) putchar (' ');
			putchar ('^');
			putchar ('\n');
			fputs (text, stdout);
			putchar ('\n');
		}
		curr = curr->more;
	}
	new_command();			/* Drop the bad command */
}					/* and get a new one */


/*** scan_fname
 *
 *
 *	char *scan_fname():
 *	Scan a forms-name.
 *
 *	Returns:
 *		A pointer to the forms-name if successful.  Otherwise
 *		a NULL pointer is returned to indicate that the forms
 *		name was too long, or was not present in the command
 *		stream.  In the failure case, an error message will
 *		have already been displayed.
 */
char *scan_fname ()
{
	register int token_type;	/* Scan token type */

	if ((token_type = scan()) == T_ABORT) return ((char *) 0);
	if (token_type != T_LITERAL) {
		errormessage (EM_FORNAMEXP);
		return ((char *) 0);
	}
	if (strlen (t_literal) > MAX_FORMNAME) {
		errormessage (EM_FORNAMTOOLON);
		return ((char *) 0);
	}
	return (t_literal);
}


/*** scan_forset
 *
 *
 *	int scan_forset():
 *	Scan a forms set as the last parse construct in a command:
 *
 *		<form-name> ...
 *
 *	The set of scanned forms is queued via the enqueue_set()
 *	function, and can be read-out in order of appearance via the
 *	dequeue_set() function.
 *
 *	Returns:
 *		0: if successful in scanning one or more forms-names;
 *	       -1: if an error occurred;
 *	       -2: if the first token was an equals sign.
 *	 	   In this case, scan_forset() does not consume
 *		   the rest of the line.  This ugliness makes
 *		   v_setfor() easier to write.
 */
int scan_forset ()
{
	register int token;

	if ((token = scan()) == T_ABORT) return (-1);
	if (token == T_EQUALS) return (-2);
	do {
		if (token != T_LITERAL) {
			errormessage (EM_FORNAMEXP);
			return (-1);
		}
		if (strlen (t_literal) > MAX_FORMNAME) {
			errormessage (EM_FORNAMTOOLON);
			return (-1);
		}
		if (enqueue_set (t_literal, (mid_t) 0, 0, 0) == -1) {
			/*
			 *  If there is not sufficient heap space,
			 *  then we wind up here, with a message
			 *  having already been displayed....
			 */
			return (-1);
		}
	} while ((token = scan()) == T_LITERAL);
	if (token == T_ABORT) return (-1);
	if (token == T_EOC) return (0);		/* Successful completion */
	errormessage (EM_FORNAMEXP);		/* Form name expected */
	return (-1);
}


/*** scan_groupset
 *
 *
 *	int scan_groupset():
 *	Scan a group set in one of the following forms:
 *
 *		<group-specifier>		OR
 *		( <group-specifier>, .... )
 *
 *	A group-specifier is either a group-name, or the sequence [gid].
 *	The set of scanned groups is queued via the enqueue_set()
 *	function, and can be read-out in order of appearance via the
 *	dequeue_set() function.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if an error occurred.
 */
int scan_groupset ()
{
	register int token;			/* Parse token type */
	short group_file_open;			/* Boolean */
	long gid;
	mid_t mid;				/* Machine id */

	group_file_open = 0;
	if ((token = scan()) == T_ABORT) return (-1);
	if (token == T_LPAREN) {
		do {
			if ((token = scan()) == T_ABORT) {
				if (group_file_open) closegrdb ();
				return (-1);
			}
			if (token != T_LITERAL) {
				errormessage (EM_GRPSPEEXP);
				if (group_file_open) closegrdb ();
				return (-1);
			}
			switch (group_analyze (t_literal, &gid, &mid)) {
			case -1:
				if (group_file_open) closegrdb ();
				return (-1);
			case 0:
				break;
			case 1:
				group_file_open = 1;
				break;
			}
			if (enqueue_set (t_literal, (mid_t) 0, gid, 0)
				== -1) {
				/*
				 *  If there is not sufficient heap
				 *  space, then we wind up here,
				 *  with a message having already
				 *  been displayed....
				 */
				if (group_file_open) closegrdb ();
				return (-1);
			}
		} while ((token = scan()) == T_COMMA);
		if (token == T_ABORT) {
			if (group_file_open) closegrdb ();
			return (-1);
		}
		if (token != T_RPAREN) {
			errormessage (EM_RIGPARORCOMEXP);
			if (group_file_open) closegrdb ();
			return (-1);
		}
	}
	else if (token != T_LITERAL) {
		errormessage (EM_GRPSPEORLEFPAREXP);
		return (-1);
	}
	else {
		switch (group_analyze (t_literal, &gid, &mid)) {
		case -1:
			return (-1);
		case 0:
			break;
		case 1:
			group_file_open = 1;
			break;
		}
		if (enqueue_set (t_literal, (mid_t) 0, gid, 0) == -1) {
			if (group_file_open) closegrdb ();
			return (-1);
		}
	}
	if (group_file_open) closegrdb ();
	return (0);
}


/*** scan_int
 *
 *
 *	int scan_int():
 *	Scan an integer token.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if an integer was not the next token in the command
 *		   input stream, or the integer value was out of bounds,
 *		   in which case an appropriate error message is displayed.
 */
int scan_int (ptr_to_long, low_bound, high_bound, int_expected, bounds_error)
long *ptr_to_long;			/* Pointer to long integer */
long low_bound;				/* Low integer value bound */
long high_bound;			/* High integer value bound */
int int_expected;			/* Integer value expected msg# */
int bounds_error;			/* Integer bounds error msg# */
{
	switch (scan ()) {
	case T_ABORT:
		return (-1);
	case T_NEG:
		switch (scan ()) {
		case T_ABORT:
			return (-1);
		case T_UINT:
			if (-(t_uint) < low_bound || -(t_uint) > high_bound) {
				errormessage (bounds_error);
				return (-1);
			}
			*ptr_to_long = (-(t_uint));
			return (0);		/* Successful completion */
		default:
			errormessage (int_expected);	/* Integer expected */
			return (-1);
		}
	case T_UINT:
		if (t_uint < low_bound || t_uint > high_bound) {
			errormessage (bounds_error);
			return (-1);
		}
		*ptr_to_long = t_uint;		/* Set the scanned integer */
		return (0);			/* Successful completion */
	default:
		errormessage (int_expected);	/* Integer expected */
		return (-1);
	}
}


/*** scan_logname
 *
 *
 *	char *scan_logname():
 *	Scan a logfile-name specification.
 *
 *	Returns:
 *		A pointer to the path name if successful.  Otherwise
 *		a NULL pointer is returned to indicate that the log-
 *		file path name was too long, or was not present in
 *		the command stream.  In the failure case, an error
 *		message will have already been displayed.
 */
char *scan_logname ()
{
	wantpath = 1;			/* Want a path */
	if (scan() == T_ABORT) return ((char *) 0);
	/*
	 *  Token type is guaranteed to be a literal.
	 */
	return (t_literal);
}


/*** scan_mgrset
 *
 *
 *	int scan_mgrset():
 *
 *	Scan an NQS manager account set as the last parse construct
 *	in a command.
 *
 *		<account-name>:{m,o} ...
 *
 *	The form of an account name in this context is:
 *
 *		<account-name>		OR
 *		<account-name>@<machine-name>
 *
 *	The set of scanned managers is queued via the enqueue_set()
 *	function, and can be read-out in order of appearance via the
 *	dequeue_set() function.  The machine-id for each of the manager
 *	accounts so specified can be gotten by the deq_mid() function.
 *	The user-id for the accounts can be gotten by the deq_uid()
 *	function.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if an error occurred.
 */
int scan_mgrset ()
{
	register int token;		/* Parse token */
	register char *cp;		/* Character copy pointer */
	register short privileges;	/* Account privileges */
	short accpwd_db_open;		/* Boolean password file is */
					/* open flag */
	long uid;			/* User-id for account */
	mid_t mid;			/* Machine-id for account */

	accpwd_db_open = 0;
	if ((token = scan()) == T_ABORT) return (-1);
	do {
		if (token != T_LITERAL || t_literal [0] == ':') {
			errormessage (EM_ACCSPEEXP);
			if (accpwd_db_open) closepwdb();
			return (-1);
		}
#if	BSD42 | BSD43 | ULTRIX
		cp = index (t_literal, ':');
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
		cp = strchr (t_literal, ':');
#else
BAD SYSTEM TYPE
#endif
#endif
		if (cp == (char *) 0) {
			/*
			 *  No privilege level specified.
			 */
			errormessage (EM_NOMANOPEPRISPE);
			if (accpwd_db_open) closepwdb();
			return (-1);
		}
		else {
			/*
			 *  Privilege level specified.
			 */
			*cp++ = '\0';	/* Scan past introducer */
					/* terminating the account spec */
			if (*cp == 'm' || *cp == 'M' ||
			    *cp == 'o' || *cp == 'O') {
				/*
				 *  Valid privilege specifier so far...
				 */
				privileges = QMGR_OPER_PRIV;
				if (*cp == 'm' || *cp == 'M') {
					privileges |= QMGR_MGR_PRIV;
				}
				if (*++cp != '\0') {
					/*
					 *  Invalid privilege specification.
					 */
					errormessage (EM_INVMANPRISYN);
					if (accpwd_db_open) closepwdb();
					return (-1);
				}
				switch (acct_analyze (t_literal, &uid, &mid)) {
				case -1:
					if (accpwd_db_open) closepwdb();
					return (-1);
				case 0:
					break;
				case 1:
					accpwd_db_open = 1;
					break;
				}
				if (enqueue_set (t_literal, mid, uid,
						 privileges) == -1) {
					/*
					 *  There was not sufficient
					 *  heap space to queue the
					 *  set entry.  A message has
					 *  already been displayed.
					 */
					if (accpwd_db_open) {
						closepwdb();
					}
					return (-1);
				}
			}
			else {
				errormessage (EM_INVPRICLASPE);
				if (accpwd_db_open) closepwdb();
				return (-1);
			}
		}
	} while ((token = scan()) == T_LITERAL);
	if (token == T_ABORT) {
		if (accpwd_db_open) closepwdb();
		return (-1);
	}
	if (token == T_EOC) {			/* Successful completion */
		if (accpwd_db_open) closepwdb();
		return (0);
	}
	errormessage (EM_ACCSPEEXP);		/* Account specifier expected */
	if (accpwd_db_open) closepwdb();
	return (-1);
}


/*** scan_opaname
 *
 *
 *	int scan_opaname():
 *
 *	Scan an optional local account name. Possibilities:
 *
 *	1) <account-name> <unknown>
 *	2) end of command
 *
 *	Returns:
 *		0: if successful, assigning uid into whomuid;
 *		0: if no account was specified, assigning -1 into whomuid;
 *	       -1: if an error occurred.
 */
int scan_opaname (ptr_to_long)
long *ptr_to_long;
{
	register int token;		/* Parse token */
	long uid;			/* User-id for account */
	mid_t mid;			/* Machine-id for account */

	if ((token = scan()) == T_ABORT) return (-1);
	if (token == T_EOC) {
		*ptr_to_long = -1;	/* This is why ptr_to_long is signed */
		return (0);
	}
	if (token != T_LITERAL) {
		errormessage (EM_ACCNAMEXP);
		return (-1);
	}
	switch (acct_analyze (t_literal, &uid, &mid)) {
	case -1:			/* Syntax error */
		return (-1);
	case 0:				/* [123] format ok */
		break;
	case 1:
		break;
	}
	if (uid == -1) {		/* No such account */
		errormessage (EM_ACCNAMEXP);
		return (-1);
	}
	if (mid != Local_mid) {	
		errormessage (EM_INVACCSPESYN);
		return (-1);
	}
	closepwdb();			/* Close the password file, opened */
					/* by a callee */
	*ptr_to_long = uid;
	return (0);
}


/*** scan_opdev
 *
 *
 *	int scan_opdev():
 *
 *	Scan an optional device.  Possibilities:
 *
 *	1) <device-name> <unknown>
 *	2) end of command
 *
 *	Returns:
 *		0: if successful, placing the device name in devname;
 *	       -1: if an error occurred.
 */
int scan_opdev(devname)
char *devname;
{
	register int token;

	if ((token = scan()) == T_ABORT) return (-1);
	if (token == T_EOC) {			/* End of command seen */
		devname [0] = '\0';
		return (0);
	}
	if (token != T_LITERAL) {
		errormessage (EM_DEVNAMEXP);
		return (-1);
	}
	if (strlen (destdev (t_literal)) > MAX_DEVNAME) {
		errormessage (EM_DEVNAMTOOLON);
		return (-1);
	}
	strcpy (devname, t_literal);	/* Copy full name; "device@machine" */
	return (0);
}


/*** scan_opqueue
 *
 *
 *	int scan_opqueue():
 *
 *	Scan an optional queue. Possibilities:
 *
 *	1) <queue-name> <unknown>
 *	2) end of command
 *
 *	Returns:
 *		0: if successful, placing the queue name in quename;
 *	       -1: if an error occurred.
 */
int scan_opqueue (quename)
char *quename;
{
	register int token;

	if ((token = scan()) == T_ABORT) return (-1);
	if (token == T_EOC) {			/* End of command seen */
		quename [0] = '\0';
		return (0);
	}
	if (token != T_LITERAL) {
		errormessage (EM_QUENAMEXP);
		return (-1);
	}
	if (strlen (destqueue (t_literal)) > MAX_QUEUENAME) {
		errormessage (EM_QUENAMTOOLON);
		return (-1);
	}
	strcpy (quename, t_literal);	/* Copy full name; "queue@machine" */
	return (0);
}


/*** scan_opwait
 *
 *
 *	int scan_opwait():
 *
 *	Scan an optional integer wait-time specification as the last
 *	token in the command.
 *
 *	Returns:
 *		0: if successful, or the next input token was T_EOC,
 *		   in which case the specified default value is used.
 *
 *	       -1: if there was another token in the command input stream
 *		   that was not an integer, or an integer was scanned
 *		   that was not the last token of the command, or the
 *		   scanned integer value was out of bounds, in which
 *		   case an appropriate error message is displayed.
 */
int scan_opwait (ptr_to_long, default_value, low_bound, high_bound,
		 int_expected, bounds_error)
long *ptr_to_long;			/* Pointer to long integer */
long default_value;			/* Default integer value */
long low_bound;				/* Low integer value bound */
long high_bound;			/* High integer value bound */
int int_expected;			/* Integer value expected msg# */
int bounds_error;			/* Integer bounds error msg# */
{
	register int token;

	if ((token = scan()) == T_ABORT) return (-1);
	if (token == T_EOC) {
		*ptr_to_long = default_value;
		return (0);		/* Success by default */
	}
	if (token != T_UINT) {
		errormessage (int_expected);	/* Integer expected */
		return (-1);
	}
	if (t_uint < low_bound || t_uint > high_bound) {
		errormessage (bounds_error);	/* Out of bounds */
		return (-1);
	}
	if ((token = scan()) == T_ABORT) return (-1);
	if (token == T_EOC) {
		*ptr_to_long = t_uint;	/* Set the scanned integer */
		return (0);		/* Successful completion */
	}
	errormessage (EM_UNECHAPASENDOFVALCOM);
	return (-1);
}


/*** scan_qcomname
 *
 *
 *	char *scan_qcomname():
 *	Scan a queue complex name.
 *
 *	Returns:
 *		A pointer to the queue complex name if successful.
 *		Otherwise a NULL pointer is returned to indicate that 
 *		the queue complex name was too long, was invalid, or
 *              was not present in the command stream.
 *		In the failure case, an error message will have
 *		already been displayed.
 */
char *scan_qcomname ()
{
	register int token_type;	/* Scan token type */

	if ((token_type = scan_string()) == T_ABORT) return ((char *) 0);
	if (token_type != T_LITERAL) {
		errormessage (EM_QCOMNAMEXP);
		return ((char *) 0);
	}
	if (strlen (t_literal) > MAX_QCOMPLXNAME) {
		errormessage (EM_QCOMNAMTOOLON);
		return ((char *) 0);
	}
#if	BSD42 | BSD43 | ULTRIX
	if (index (t_literal, '@') != (char *) 0 || isdigit (t_literal[0])) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	if (strchr (t_literal, '@') != (char *) 0 || isdigit (t_literal[0])) {
#else
BAD SYSTEM TYPE
#endif
#endif
		errormessage (EM_INVQCOMNAMSPE);
		return ((char *) 0);
	}
	return (t_literal);
}


/*** scan_qname
 *
 *
 *	char *scan_qname():
 *	Scan a queue name.
 *
 *	Returns:
 *		A pointer to the queue name if successful.  Otherwise
 *		a NULL pointer is returned to indicate that the queue
 *		name was too long, was invalid, or was not present in
 *		the command stream.  In the failure case, an error
 *		message will have already been displayed.
 */
char *scan_qname ()
{
	register int token_type;	/* Scan token type */

	if ((token_type = scan_string()) == T_ABORT) return ((char *) 0);
	if (token_type != T_LITERAL) {
		errormessage (EM_QUENAMEXP);
		return ((char *) 0);
	}
	if (strlen (t_literal) > MAX_QUEUENAME) {
		errormessage (EM_QUENAMTOOLON);
		return ((char *) 0);
	}
#if	BSD42 | BSD43 | ULTRIX
	if (index (t_literal, '@') != (char *) 0 || isdigit (t_literal[0])) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	if (strchr (t_literal, '@') != (char *) 0 || isdigit (t_literal[0])) {
#else
BAD SYSTEM TYPE
#endif
#endif
		errormessage (EM_INVQUENAMSPE);
		return ((char *) 0);
	}
	return (t_literal);
}


/*** scan_qualifier
 *
 *
 *	int scan_qualifier():
 *	Scan for a qualifier in the specified qualifier set.
 *
 *	Returns:
 *		-1: if the next input token was not T_EOC, and the
 *		    token was not one of the specified qualifiers,
 *		    was an ambiguous qualifier match, or was not a
 *		    qualifier at all, or the token was of type T_ABORT.
 *		 0: if the current token is T_EOC.
 *	    [1..n]: if the next token was a qualifier, and matched the
 *		    n-th qualifier as specified in the qualifier set.
 */
int scan_qualifier (qualif_set)
register char *qualif_set[];		/* Set of valid qualifiers */
{
	register int token_type;	/* Scan token type */
	register int qualifier;		/* qualifier index */

	if ((token_type = scan()) == T_ABORT) return (-1);
	if (token_type == T_EOC) return (0);
	if (token_type != T_LITERAL) {
		/*
		 *  A qualifier keyword was expected.
		 */
		errormessage (EM_KEYEXP);
		return (-1);
	}
	/*
	 *  A keyword was scanned.
	 */
	qualifier = 0;
	do {
		qualifier++;
		switch (matchkeyword (t_literal, *qualif_set)) {
		case -1:		/* Ambiguous match */
			errormessage (EM_AMBQUA);
			return (-1);
		case  0:
			break;		/* No match */
		case  1:
			return (qualifier);
		}
	} while (*++qualif_set != (char *) 0);
	errormessage (EM_INVQUASPE);	/* Invalid qualifier specified */
	return (-1);
}


/*** scan_queset
 *
 *
 *	int scan_queset():
 *	Scan a queue set of the following form:
 *
 *		( <queue-specifier>, .... )
 *
 *	A queue-specifier is a scheduler queue-name. The set of
 *	scanned queue names is queued via the enqueue_set() function,
 *	and can be read-out in order of appearance via the
 *	dequeue_set() function.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if an error occurred.
 */
int scan_queset ()
{
	register int token;

	if ((token = scan_string()) == T_ABORT) return (-1);
	if (token == T_LPAREN) {
		do {
			if ((token = scan_string()) == T_ABORT) return (-1);
			if (token != T_LITERAL) {
				errormessage (EM_QUENAMEXP);
				return (-1);
			}
			if (strlen (t_literal) > MAX_QUEUENAME) {
				errormessage (EM_QUENAMTOOLON);
				return (-1);
			}
#if	BSD42 | BSD43 | ULTRIX
			if (index (t_literal, '@') != (char *) 0
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
			if (strchr (t_literal, '@') != (char *) 0
#else
BAD SYSTEM TYPE
#endif
#endif
					|| isdigit (t_literal[0])) {
				errormessage (EM_INVQUENAMSPE);
				return (-1);
			}
			if (enqueue_set (t_literal, (mid_t) 0, 0, 0) == -1) {
				/*
				 *  If there is not sufficient heap space,
				 *  then we wind up here, with a message
				 *  having already been displayed....
				 */
				return (-1);
			}
		} while ((token = scan()) == T_COMMA);
		if (token == T_ABORT) return (-1);
		if (token != T_RPAREN) {
			errormessage (EM_RIGPARORCOMEXP);
			return (-1);
		}
	}
	else {
		if (token == T_LITERAL) {	/* CERN Boissat begin */
			if (strlen (t_literal) > MAX_QUEUENAME) {
				errormessage (EM_QUENAMTOOLON);
				return(-1);
			}
#if BSD42 | BSD43 | ULTRIX
			if (index (t_literal, '@') != (char *) 0
#else
#if SGI | SYS52 | UNICOS | UTS | OSF
			if (strchr (t_literal, '@') != (char *) 0
#else
BAD SYSTEM TYPE
#endif
#endif

			|| isdigit (t_literal[0])) {
				errormessage ( EM_INVQUENAMSPE);
			}
			if (enqueue_set (t_literal, (mid_t) 0, 0, 0) == -1) {
			/*
			 * If there is not sufficent heap space,
			 * then we wind up here, with a message
			 * having already been diplsayed....
			 */
				return (-1);
			}
		}	
		else {
			errormessage (EM_QUENAMEXP);
		}					/* CERN Boissat end */

		errormessage (EM_LEFPAREXP);
		return (-1);
	}
	return (0);
}


/*** scan_reqidname
 *
 *
 *	int scan_reqidname():
 *	Scan a request identifier in one of the following forms:
 *
 *		<sequence-no>		OR
 *		<sequence-no>.<hostname>
 *
 *	Returns:
 *		0:	if successful, assigning req_seqno and req_mid;
 *		-1:	if an error occurred. In this case a message
 *			will have already been displayed.
 */
int scan_reqidname (req_seqno, req_mid)
long *req_seqno;			/* Request sequence  number */
mid_t *req_mid;				/* Request machine identifier */
{
	register int token_type;	/* Scan token type */

	if ((token_type = scan_string()) == T_ABORT) return (NULL);
	if (token_type != T_LITERAL) {
		errormessage (EM_REQIDEXP);
		return (NULL);		/* Request identifier expected */
		}
	if (strlen (t_literal) > MAX_REQNAME) {
		errormessage (EM_REQIDTOOLON);
		return (NULL);		/* Request identifier too long */
		}	
	switch (reqspec (t_literal,req_seqno,req_mid)) {
	case  0:			/* Successful */
		return (0);
	case -1:			/* Invalid syntax */
		errormessage (EM_INVREQIDSYN);
		return (-1);
	case -2:			/* Unknown machine identifier */
		errormessage (EM_NOSUCMAC);
		return (-1);
	case -3:			/* Network mapping db inaccessible */
		errormessage (EM_NETMAPDBINACC);
		return (-1);
	case -4:			/* Network mapping DB error */
		errormessage (EM_NETMAPDBERR);
		return (-1);
	}
}


/*** scan_reqmodset
 *
 *
 *	int scan_reqmodset()
 *
 *	Scan a set of request modifiers as the last construct in a command
 *	
 *	The form of a request modifier is:
 *	
 *		Nice_limit = <nice value>	OR
 *		RTime_limit = <Tlimit>		OR
 *		RMemory_limit = <Mlimit>
 *		
 *	The set of parameter pairs is queued via the enqueue_set() function
 *	and can be read out in order of appearance via the dequeue_set()
 *	function. The parameter type for each pair so specified can be
 *	obtained by the deq_type() function.
 *
 *	Returns:
 *		 0 - if successful;
 *		-1 - if an error occurred. In this case, an error message 
 *			will already have been displayed.
 */
int scan_reqmodset()
{
	static char *qualifier_set [] = {
		"Nice_limit",
		"RTime_limit",
		"RMemory_limit",
		NULL
	};

	int qual_token;
	/*
	 *	Boolean variables
	 */
	register short nice_seen;		/* Nice_limit seen */
	register short cput_seen;		/* RTime_limit seen */
	register short mem_seen;		/* RMemory_limit seen */
	/*
	 *	No attributes seen yet
	 */
	nice_seen = 0;
	cput_seen = 0;
	mem_seen = 0;

	/*
	 *	Get key-word/value pairs
	 */
	while ((qual_token = scan_qualifier (qualifier_set)) > 0) {
		/*
		 *  A valid qualifier keyword has been recognized.
		 */
		if (scan_equals() == -1) return (-1);
		if (scan_string() != T_LITERAL) {
			errormessage (EM_KEYVALEXP);
			return (-1);
		}
		/*
		 *	Analyze parameter value 
		 */
		switch (qual_token) {
		case 1:			/* Nice Value */
			if (nice_seen) {
				errormessage (EM_KEYDUP);
				return (-1);
			}
			nice_seen = 1;
			if (enqueue_set (t_literal, (mid_t)0, (long)qual_token,
 				0) == -1) return (-1);
			break;
		case 2:			/* CPU time limit */
			if (cput_seen) {
				errormessage (EM_KEYDUP);
				return (-1);
			}
			cput_seen = 1;
			if (enqueue_set (t_literal, (mid_t)0, (long)qual_token,
 				0) == -1) return (-1);
			break;
		case 3:			/* Memory Limit */	
			if (mem_seen) {
				errormessage (EM_KEYDUP);
				return (-1);
			}
			mem_seen = 1;
			if (enqueue_set (t_literal, (mid_t)0, (long)qual_token,
 				0) == -1) return (-1);
			break;
		}	
	}
	if (qual_token == -1) return (-1);	/* Qualifier bad */

	else {				/* End of command encountered */

		if ((nice_seen + cput_seen + mem_seen) == 0) {
			errormessage (EM_KEYEXP);
			return (-1);
		}
	return (0);
	}
}


/*** scan_reqset
 *
 *
 *	int scan_reqset():
 *	Scan a request set in one of the following forms:
 *
 *		<request-ident>		OR
 *		(<request-ident>, .... )
 *
 *	The set of scanned requests is queued via the enqueue_set()
 *	function, and can be read-out in order of appearance via the
 *	dequeue_set() function.
 *
 *	Returns:
 *		 0: 	if successful;
 *		-1:	if an error occurred. In this case, an error
 *			message will already have been displayed.	
 */
int scan_reqset ()
{
	long req_seqno;			/* Request sequence number */
	mid_t req_mid;			/* Request machine identifier */
	register int token;

	if ((token = scan_string()) == T_ABORT) return (-1);
	if (token == T_LPAREN) {
		do {
			if ((token = scan_string()) == T_ABORT) return (-1);
			if (token != T_LITERAL) {
				errormessage (EM_REQIDEXP);
				return (-1);
			}
			switch (reqspec (t_literal, &req_seqno, &req_mid)) {
			case 0:
			if (enqueue_set (t_literal, req_mid, req_seqno, 0)
				 == -1) {
				/*
				 *  If there is not sufficient heap space,
				 *  then we wind up here, with a message
				 *  having already been displayed....
				 */
				return (-1);
				}
				break;	
			case -1:			/* Invalid syntax */
				errormessage (EM_INVREQIDSYN);
			return (-1);
			case -2:		/* Unknown machine identifier */
				errormessage (EM_NOSUCMAC);
				return (-1);
			case -3:	/* Network mapping db inaccessible */
				errormessage (EM_NETMAPDBINACC);
				return (-1);
			case -4:		/* Network mapping DB error */
				errormessage (EM_NETMAPDBERR);
				return (-1);
			}
		} while ((token = scan_string()) == T_COMMA);
		if (token == T_ABORT) return (-1);
		if (token != T_RPAREN) {
			errormessage (EM_RIGPARORCOMEXP);
			return (-1);
		}
	}
	else if (token != T_LITERAL) {
		errormessage (EM_REQIDORLEFPAREXP);
		return (-1);
	}
	else {
		switch (reqspec (t_literal, &req_seqno, &req_mid)) {
		case 0:
			if (enqueue_set (t_literal, req_mid, req_seqno, 0)
				 == -1) return (-1);
			break;
		case -1:			/* Invalid syntax */
			errormessage (EM_INVREQIDSYN);
			return (-1);
		case -2:			/* Unknown machine identifier */
			errormessage (EM_NOSUCMAC);
			return (-1);
		case -3:		/* Network mapping db inaccessible */
			errormessage (EM_NETMAPDBINACC);
			return (-1);
		case -4:			/* Network mapping DB error */
			errormessage (EM_NETMAPDBERR);
			return (-1);
		}
	}
	return (0);
}


/*** scan_string
 *
 *
 *	int scan_string():
 *
 *	Return the type of the next token in the command.
 *
 *		Possible values are:	
 *			T_EOC		( end of command seen)
 *			T_COMMA
 *			T_NEG
 *			T_EQUALS
 *			T_LPAREN
 *			T_RPAREN
 *			T_LITERAL	( pointer returned in "t_literal")
 *			T_ABORT
 *
 *	N.B. No attempt is made at integer evaluation. Digits are regarded
 *	as (part of) a literal string.	
 *
 *	A literal is terminated by a space/comma/equals/left parens/right 
 *	parens or an end-of-command.
 *
 *	Any string between a pound sign (#) token and End-of-command is regarded
 *	as COMMENT and is ignored when no special mode is in effect.
 *
 */
int scan_string()
{
	struct cmd_struct *getcmdline();	/* Get command line fn */
	void new_command();			/* Drop old command and get */
						/* a new one */
	char nextchar();			/* Next character function */
	void scan_error();			/* Display scan error mssg */

	int t_litlen;				/* Literal length */
	register char ch;			/* Scan character */

	ch = cmdchar;				/* Get current scan character */
	if (ch == '\0') {			/* At the end of the current */
						/* command */
		if (eoc_returned) {		/* Previous token returned was*/
						/* T_EOC, so get a new command*/
			eoc_returned = 0;	/* Clear T_EOC flag */
			new_command();		/* Discard the old command */
			cmd = getcmdline();	/* Get command */
			if (cmd != (struct cmd_struct *) 0) {
						/* Initialize scan */
				curr = cmd;	/* Current cmd line */
				scanpos = -1;	/* Current scan position */
				ch = nextchar();/* Load current character */
			}
			else return (T_EOF);	/* No more commands */
		}
		else {				/* We are at the end of the */
						/* command, but have not */
						/* previously returned T_EOC */
			t_scanpos = scanpos;	/* Update scan position */
			eoc_returned = 1;	/* Set T_EOC returned flag */
			return (T_EOC);		/* Inform caller */
		}
	}
	/*
	 *  Return the next token in the command stream.
	 */
	while (isspace (ch)) ch = nextchar();	/* Scan whitespace */
	t_scanpos = scanpos;			/* Save token position */
	t_curr = curr;
	switch (ch) {				/* Identify token */
	case '\0':
		eoc_returned = 1;		/* T_EOC returned */
		return (T_EOC);			/* End of command seen */
	case ',':
		nextchar();			/* Step past , */
		return (T_COMMA);		/* , seen */
	case '=':
		nextchar();			/* Step past = */
		return (T_EQUALS);		/* = seen */
	case '(':
		nextchar();			/* Step past ( */
		return (T_LPAREN);		/* ( seen */
	case ')':
		nextchar();			/* Step past ) */
		return (T_RPAREN);		/* ) seen */
	case '#':
		while (ch==nextchar() != '\0'); /* Step over */
		eoc_returned = 1;		/* Comment   */
		return (T_EOC);			
	default:				/* Literal found */
		t_litlen = 0;			/* No chars in literal */
		while (ch && !isspace (ch) && ch != ',' && ch != '=' &&
		       ch != '(' && ch != ')' && t_litlen < MAX_LITSIZE) {
			t_literal [t_litlen++] = ch;
			ch = nextchar();
		}
		if (ch && !isspace (ch) && ch != ',' && ch != '=' &&
		    ch != '(' && ch != ')') {
			scan_error ("Literal string is too long.");
			return (T_ABORT);
		}
		t_literal [t_litlen] = '\0';	/* Add terminating null */
		return (T_LITERAL);		/* Literal seen */
	}
}


/*** scan_userset
 *
 *
 *	int scan_userset():
 *	Scan a user set in one of the following forms:
 *
 *		<user-specifier>		OR
 *		( <user-specifier>, .... )
 *
 *	A user-specifier is either a user-name, or the sequence [uid].
 *	The set of scanned users is queued via the enqueue_set()
 *	function, and can be read-out in order of appearance via the
 *	dequeue_set() function.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if an error occurred.
 */
int scan_userset ()
{
	register int token;			/* Parse token type */
	short passwd_file_open;			/* Boolean */
	long uid;
	mid_t mid;				/* Machine id */

	passwd_file_open = 0;
	if ((token = scan()) == T_ABORT) return (-1);
	if (token == T_LPAREN) {
		do {
			if ((token = scan()) == T_ABORT) {
				if (passwd_file_open) closepwdb ();
				return (-1);
			}
			if (token != T_LITERAL) {
				errormessage (EM_ACCSPEEXP);
				if (passwd_file_open) closepwdb ();
				return (-1);
			}
			switch (acct_analyze (t_literal, &uid, &mid)) {
			case -1:
				if (passwd_file_open) closepwdb ();
				return (-1);
			case 0:
				break;
			case 1:
				passwd_file_open = 1;
				break;
			}
			if (enqueue_set (t_literal, (mid_t) 0, uid, 0)
				== -1) {
				/*
				 *  If there is not sufficient heap
				 *  space, then we wind up here,
				 *  with a message having already
				 *  been displayed....
				 */
				if (passwd_file_open) closepwdb ();
				return (-1);
			}
		} while ((token = scan()) == T_COMMA);
		if (token == T_ABORT) {
			if (passwd_file_open) closepwdb ();
			return (-1);
		}
		if (token != T_RPAREN) {
			errormessage (EM_RIGPARORCOMEXP);
			if (passwd_file_open) closepwdb ();
			return (-1);
		}
	}
	else if (token != T_LITERAL) {
		errormessage (EM_ACCSPEORLEFPAREXP);
		return (-1);
	}
	else {
		switch (acct_analyze (t_literal, &uid, &mid)) {
		case -1:
			return (-1);
		case 0:
			break;
		case 1:
			passwd_file_open = 1;
			break;
		}
		if (enqueue_set (t_literal, (mid_t) 0, uid, 0) == -1) {
			if (passwd_file_open) closepwdb ();
			return (-1);
		}
	}
	if (passwd_file_open) closepwdb ();
	return (0);
}


/*** acct_analyze
 *
 *
 *	acct_analyze():
 *
 *	Analyze an account specification, returning the user-id, and
 *	machine-id of the specified account.
 *
 *	The syntax of an account specification is as follows:
 *
 *		<account-name>			OR
 *		<account-name>@<machine-name>
 *
 *	The syntax of an <account-name> is:
 *
 *		<simple-account-name>		OR
 *		[ <decimal-digit-seq-account-user-id> ]
 *
 *	The syntax of a <machine-name> is:
 *
 *		<simple-name-of-machine>	OR
 *		[ <decimal-digit-seq-machine-id> ]
 *
 *	Returns:
 *		1: if successful (local password file opened).
 *		0: if successful (local password file not
 *		   opened).
 *	       -1: if a syntax error is encountered, an error occurs
 *		   getting the machine-id of the account, or a remote
 *		   account is specified by name, and not by numeric
 *		   user-id.  In such cases, an error message is displayed
 *		   (local password file not opened).
 *
 */
static int acct_analyze (account_spec, ptrtouid, ptrtomid)
char *account_spec;		/* Account specification */
long *ptrtouid;			/* Return uid for account, -1 if non- */
				/* numeric account-name does not exist. */
				/* Undefined if syntax error found */
mid_t *ptrtomid;		/* Return machine-id for account, */
				/* undefined, if the named machine */
				/* does not exist. */
{
	register char *account_scan;	/* Account name scanning */
	register long i;		/* Number scanning */
	register struct passwd *passwd;	/* Pointer to password file entry */
	short must_be_local;		/* Account must be local */

	account_scan = account_spec;
	if (*account_scan == '[') {
		/*
		 *  A numeric user-id specification is given.
		 */
		account_scan++;		/* Scan past '[' */
		if (isdigit (*account_scan)) {
			i = 0;
			do {
				i *= 10;
				i += *account_scan++ - '0';
			} while (isdigit (*account_scan));
			if (*account_scan++ != ']') {
				errormessage (EM_INVACCSPESYN);
				return (-1);
			}
			*ptrtouid = i;
			/*
			 * It is OK if this uid does not appear in the
			 * password file. This is an escape hatch to
			 * allow the deletion from NQS of references
			 * to nonexistent accounts.
			 */
			must_be_local = 0;	/* Account does not have */
		}				/* to be local */
		else {
			errormessage (EM_INVACCSPESYN);
			return (-1);
		}
	}
	else if (*account_scan == '@') {	/* Missing account name */
		errormessage (EM_ACCSPEEXP);	/* Account specifier expected */
		return (-1);
	}
	else {
		/*
		 *  We have an account name to scan, and must translate
		 *  it to the local user-id.
		 */
		do {
			account_scan++;
		} while (*account_scan && *account_scan != '@');
		if (*account_scan) {
			*account_scan = '\0';	/* Temporarily null term */
			passwd = fetchpwnam (account_spec);
			*account_scan = '@';	/* Put back '@' */
		}
		else passwd = fetchpwnam (account_spec);
		if (passwd == (struct passwd *) 0) *ptrtouid = -1;
							/* No such local */
							/* account name */
		else *ptrtouid = passwd->pw_uid;	/* Store local uid */
		must_be_local = 1;		/* Account is required to */
	}					/* be local */
	/*
	 *  Analyze the account specification for the machine upon which
	 *  the specified account resides.
	 */
	switch (machspec (account_spec, ptrtomid)) {
	case -1:
		errormessage (EM_INVACCSPESYN);
		return (-1);
	case -2:
		errormessage (EM_NOSUCMAC);
		return (-1);
	case -3:
		errormessage (EM_INSNMAPRI);
		return (-1);
	case -4:
		errormessage (EM_GENNMAERR);
		return (-1);
	}
	if (*ptrtomid != Local_mid && must_be_local) {
		/*
		 *  Another machine besides the local host has the
		 *  specified account, where the account was not
		 *  specified as an explicit user-id.  We do not
		 *  presently support the operation of inspecting
		 *  remote password files....
		 */
		errormessage (EM_REMACCCANTBESPEBYNAM);
		return (-1);
	}
	return (must_be_local);		/* 0 if local password file not */
}					/* inspected.  Otherwise 1. */


/*** dropcmd
 *
 *
 *	struct cmd_struct *dropcmd():
 *	Drop buffered command lines.  The current character is set to '\0'.
 *
 *	Returns:
 *		NULL: This function always returns NULL.
 */
static struct cmd_struct *dropcmd (cmd)
register struct cmd_struct *cmd;
{
	register struct cmd_struct *next;

	while (cmd != (struct cmd_struct *) 0) {
		next = cmd->more;
		free ((char *) cmd);
		cmd = next;
	}
	cmdchar = '\0';		/* Current char is undefined */
	return ((struct cmd_struct *) 0);
}


/*** enqueue_set
 *
 *
 *	int enqueue_set ():
 *
 *	Queue a set element.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if insufficient heap space exists to preform the
 *		   operation (in which case an error message will be
 *		   displayed).
 */
static int enqueue_set (text, machine_id, uid_gid, privileges)
char *text;					/* Text to be queued */
mid_t machine_id;				/* Machine-id to be queued */
int uid_gid;					/* Uid OR Gid to be queued */
short privileges;				/* Manager privilege bits */
{
	void scan_error();
	register struct set_struct *element;
	register unsigned size;

	size = sizeof (struct set_struct) - MAX_LITSIZE + strlen (text);
	if ((element = (struct set_struct *)
			malloc (size)) == (struct set_struct *) 0) {
		/*
		 *  Insufficient heap space.
		 */
		scan_error ("Insufficient heap space to parse command.");
		return (-1);
	}
	element->next = (struct set_struct *) 0;/* NULL next pointer */
	element->machine_id = machine_id;	/* Save machine-id */
	element->uid_gid = uid_gid;		/* Uid OR Gid */
	element->mgr_privileges = privileges;	/* Save privileges */
	strcpy (element->text, text);		/* Save text */
	if (set_tail == (struct set_struct *) 0) set_head = element;
	else set_tail->next = element;		/* Add to the end of */
	set_tail = element;			/* the set */
	return (0);
}


/*** getcmdline
 *
 *
 *	struct cmd_struct *getcmdline():
 *	Get a command line from stdin.
 *
 *	Returns:
 *		A pointer to the possibly multi-line command received
 *		from stdin (if successful).  Otherwise NULL is returned
 *		to indicate that EOF has been reached on Stdin.
 */
static struct cmd_struct *getcmdline()
{
	struct cmd_struct *cmd;			/* Cmd lines */
	struct cmd_struct *new;			/* New cmd line */
	struct cmd_struct *next;		/* Next cmd line */
	char buffer [MAX_LINESIZE + 1];		/* Line buffer */
	short line_size;			/* Current line size */
	short exit;				/* Exit flag */
	int ch;					/* Input character */

	wantpath = 0;				/* Not looking for file path */
	wantallinparens = 0;			/* Not looking for server, */
						/* cpu limit, or quota limit */
	exit = 0;				/* Exit flag */
	cmd = (struct cmd_struct *) 0;		/* No command lines yet */
	while (!feof (stdin) && !exit) {
		if (cmd == (struct cmd_struct *) 0) printf ("Mgr: ");
		else printf ("Mgr:_");
		fflush (stdout);		/* Make prompt visible since */
						/* block buffering is in */
						/* effect (see mgr_main.c) */
		line_size = 0;			/* No chars in buffer */
		while (line_size < MAX_LINESIZE && ((ch = getchar()) != '\n') &&
		       ch != EOF) {
			if (ch < 0) ch = -ch;		/* Tremendous */
			if (ch > 127) ch %= 127;	/* paranoia */
			if (ch == '\t') ch == ' ';	/* Tabs become spaces */
			if (ch >= ' ') {
				/*
				 *  We do NOT take control characters as
				 *  significant, and in particular -- we
				 *  do NOT store any trailing newline
				 *  character marking the end of the line....
				 */
				buffer [line_size++] = ch;
			}
		}
		if (ch != '\n' && ch != EOF) {
			/*
			 *  Line is too long.
			 */
			do {			/* Scan remaining line text */
				ch = getchar();
			} while (ch != EOF && ch != '\n');
			printf ("Command line too long.  Command ignored.\n");
			cmd = dropcmd (cmd);	/* Discard buffered cmd lines */
		}
		else if (line_size) {		/* A valid cmd line so far */
			if (buffer [line_size-1] != '\\') {
				/*
				 *  The command is not to be continued.
				 */
				exit = 1;
			}
			else line_size--;	/* Discard continuation char */
			buffer [line_size] = '\0';	/* Trailing null */
			new = (struct cmd_struct *)
			       malloc (sizeof (struct cmd_struct) -
				       MAX_LINESIZE + line_size);
			if (new == (struct cmd_struct *) 0) {
				printf ("Insufficient memory to buffer ");
				printf ("command.  Command discarded.\n");
				cmd = dropcmd (cmd);	/* Drop command */
			}
			else {
				new->more = (struct cmd_struct *) 0;
				strcpy (new->cmd, buffer);
				if (cmd == (struct cmd_struct *) 0) cmd = new;
				else next->more = new;
				next = new;
			}
		}
		else if (cmd != (struct cmd_struct *) 0) {
			exit = 1;	/* No text on this line, but some */
					/* text has been typed.... */
		}
	}
	return (cmd);		/* Return received (possibly multi-line) cmd */
}


/*** group_analyze
 *
 *
 *	group_analyze():
 *
 *	Analyze a group specification, returning the group-id
 *	of the specified group.
 *
 *	The syntax of a group specification is as follows:
 *
 *		<simple-group-name>			OR
 *		[ <decimal-digit-seq-group-id> ]
 *
 *	Returns:
 *		1: if successful (local group file opened).
 *		0: if successful (local group file not opened).
 *	       -1: if a syntax error is encountered
 *		   (local group file not opened).
 *
 */
static int group_analyze (group_spec, ptrtogid)
char *group_spec;		/* Group specification */
long *ptrtogid;			/* Return gid for account, -1 if non- */
				/* numeric group-name does not exist. */
				/* Undefined if syntax error found */
{
	register long i;		/* Number scanning */
	register struct group *groupp;	/* Pointer to group file entry */

	if (*group_spec == '[') {
		/*
		 *  A numeric group-id specification is given.
		 */
		group_spec++;		/* Scan past '[' */
		if (isdigit (*group_spec)) {
			i = 0;
			do {
				i *= 10;
				i += *group_spec++ - '0';
			} while (isdigit (*group_spec));
			if (*group_spec++ != ']') {
				errormessage (EM_INVGRPSPESYN);
				return (-1);
			}
			*ptrtogid = i;
			/*
			 * It is OK if this gid does not appear in the
			 * group file. This is an escape hatch to
			 * allow the deletion from NQS of references
			 * to nonexistent groups.
			 */
			return (0);
		}
		else {
			errormessage (EM_INVGRPSPESYN);
			return (-1);
		}
	}
	else {
		/*
		 *  We have an group name to scan, and must translate
		 *  it to the group-id.
		 */
		groupp = fetchgrnam (group_spec);
		if (groupp == (struct group *) 0) {	/* No such group */
			*ptrtogid = -1;
		}
		else *ptrtogid = groupp->gr_gid;
		return (1);
	}
}


/*** nextchar
 *
 *
 *	char nextchar():
 *	Returns:
 *		The next character of the command stream (if it
 *		exists).  Otherwise '\0' is returned.
 *
 *	WARNING:
 *		Additional calls to this function when the '\0' character
 *		has been returned for the same command will fail.
 */
static char nextchar ()
{
	scanpos++;				/* Increment scan position */
	while (curr != (struct cmd_struct *) 0 &&
	       curr->cmd [scanpos] == '\0') {
		if (curr->more == (struct cmd_struct *) 0) {
						/* Do not advance past the */
			cmdchar = '\0';		/* end of the last line */
			return ('\0');		/* End of command */
		}
		curr = curr->more;		/* Get the next line */
		scanpos = 0;			/* Beginning of line */
	}
	cmdchar = curr->cmd [scanpos];		/* Have a character */
	return (cmdchar);
}
