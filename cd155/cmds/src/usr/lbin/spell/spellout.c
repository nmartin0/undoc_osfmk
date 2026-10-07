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
static char rcsid[] = "@(#)$RCSfile: spellout.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:48:21 $";
#endif
/*
 * COMPONENT_NAME: (CMDTEXT) Text Formatting Services
 *
 * FUNCTIONS:
 *
 * ORIGINS: 10,13,26,27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 */
/*  spellout.c	1.2  com/bsd.d/spell.d,3.1,8950 11/18/89 17:41:32";  */

/*
 * spellout - display words [not] in spelling list 
 *
 * spellout [-d] hlist < wordlist > (mis)matchlist 
 */

#include "spell_msg.h"
#define MSGSTR(n,s) NLgetamsg(MF_SPELL,MS_SPELL,n,s)


#include <stdio.h>
#include <locale.h>

#define	WORDLENGTH	30	/* NW in BSD spellout (spell.h) */

main (argc, argv)
    int             argc;
    char          **argv;
{
    char            word[WORDLENGTH];
    int             dflag = 0;

    setlocale (LC_ALL, "");
    if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'd')
    {
	dflag = (argc == argc);
	argc--;
	argv++;
    }
    if (argc <= 1)
    {
	fprintf (stderr, MSGSTR (ARGCNT, "spellout: arg count\n"));	/* MSG */
	exit (1);
    }
    if (!prime (argc, argv))
    {
	fprintf (stderr, MSGSTR (CANTINIT, "spellout: cannot initialize hash table\n"));	/* MSG */
	exit (1);
    }
    /*
     * Display Truth table -d flag | 0  1 --+----- hashlook 0 | 1  0 1 | 0  1 
     */
    while (fgets (word, sizeof (word), stdin))
    {
	register int    x;
	if (word[x = strlen (word) - 1] == '\n')
	    word[x] = NULL;
	if (dflag == hashlook (word))
	    puts (word);
    }
}
