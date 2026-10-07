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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: NLyesno.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:15 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NLyesno, scan
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * NLyesno.c	1.7  com/lib/c/nls,3.1,9013 2/13/90 16:00:25
 */

/* No includes */

/*
 * EXTERNAL PROCEDURES CALLED:
 */

extern char *NLgetenv();        /* Obtain environment information */
static int   scan();            /* Scan for match of user's response */


/*
 * NAME: NLyesno
 *
 * FUNCTION: Determine "yes" or "no" response to prompts.
 *
 * (EXECUTION ENVIRONMENT:)
 *
 *      Environment-Specific aspects, such as -
 *      Preemptable        : Yes
 *      VMM Critical Region: No
 *      Runs on Fixed Stack: No
 *      May Page Fault     : Yes
 *      May Backtrack      : Yes
 *
 * (NOTES:)
 *
 *      Compare a string s against the substrings given in values
 *      of YESSTR and NOSTR.  If s matches an element of YESSTR,
 *      return 1 (true).  Otherwise, if s matches an element of
 *      NOSTR, return 0 (false).  Otherwise, return -1 as an
 *      error indication (which normally indicates that the
 *      calling code should prompt again for a proper response).
 *      Note that a null response is indicated in YESSTR or NOSTR
 *      by a leading or trailing colon or by two adjacent colons.
 *      The string s is considered a null response if it's first
 *      element is either '\0' or '\n'.
 *
 * (DATA STRUCTURES:) Effects on global data structures -- none.
 *
 * RETURN VALUE DESCRIPTION: 1 for YES response
 *                           0 for NO response
 *                          -1 for error condition
 */



int
NLyesno (s)
				 /*
				    s is the pointer to the
				    response given by the user.
				 */
register char *s;
{
    register char *response;
    register int len = strlen (s);

    /* Try to match affirmative. */
    if (!(response = NLgetenv ("YESSTR")))
	response = "y:yes:Y:YES";
    if (scan (s, response, len))
	return (1);

    /* Try to match negative. */
    if (!(response = NLgetenv ("NOSTR")))
	response = "n:no:N:NO";
    if (scan (s, response, len))
	return (0);

    return (-1);
}

/*
 * NAME: scan
 *
 * FUNCTION: Scan for match of user response against environment strings.
 *
 * (NOTES:) Internal subroutine
 *
 * (DATA STRUCTURES:) Effects on global data structures -- none.
 *
 * RETURN VALUE DESCRIPTION:    0 - no match found
 *                              1 - match found
 */

static
scan (str0, str1, len)
				/*
				  str0 - pointer to user's response
				  str1 - pointer to environment data
				  len  - length of user's response
				*/
register char *str0, *str1;
register int   len;
{
    register int   r;
    register char *strend;
             char  str2[257];
             char *str3;

    str3 = (char *)0;
    strend = str1 + strlen (str1);
    /*
     * Copy the environment data string to a local copy.
     * If the string length is greater than the local storage,
     * allocale enough storage for the copy.
     * Set str1 and strend to point to the copy.
     */
    if (str1 < strend) {
        if (strend - str1 > 256) {
            if ((str3 = (char *)malloc(strend - str1)) == 0)
                return (0);
            else {
                strcpy(str3, str1);
                str1 = str3;
            }
        }
        else {
            strcpy(str2, str1);
            str1 = str2;
        }
        strend = str1 + strlen (str1);
    }
    if (*str0 == '\n')
	len = 0;
    do {
	/*
	 * Find the next colon.
	 */
	r = strcspn (str1, ":");
	/*
	 * If the colon is escaped, then copy the rest of the string
	 * left one position so that the colon overwrites the backslash.
	 * Continue looking for a non-escaped colon.
	 * When it is found, then r is the count of characters from the
	 * start of the field to the non-escaped colon.
	 * str1 and strend point to the start and end of the field.
	 */
        while (str1 < strend) {
	    if ((*(str1 + r) == ':') &&
	        (*(str1 + r - 1) == '\\' )) {
		strcpy(str1+r-1, str1+r);
		--strend;
		r += strcspn (str1+r, ":"); 
	    }
	    else
		break;
	}
	/*
	 * Compare exactly the number of characters indicated by r.
	 */
	if ((r == len) && (strncmp (str0, str1, len) == 0)) {
	    if (str3 != (char *)0)
		free(str3);
	    return (1);
	}
	str1 += r + 1;
    } while (str1 < strend);
    if (str3 != (char *)0)
	free(str3);
    return (0);
}

