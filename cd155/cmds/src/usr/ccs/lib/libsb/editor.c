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
 *  editor  --  fork editor to edit some text file
 *
 *  Usage:
 *	i = editor(file, prompt);
 *	char *file, *prompt;
 *	int i;
 *
 *  The editor() routine is used to fork the user's favorite editor.
 *  There is assumed to be an environment variable named "EDITOR" whose
 *  value is the name of the favored editor.  If the EDITOR parameter is
 *  missing, some default (see DEFAULTED below) is assumed.  The runp()
 *  routine is then used to find this editor on the searchlist specified
 *  by the PATH variable (or the default path).  "file" is the name of
 *  the file to be edited and "prompt" is a string (of any length) which
 *  will be printed in a such a way that the user can see it at least at
 *  the start of the editing session.  editor() returns the value of the
 *  runp() call.
 */

#include <stdio.h>

char *getenv();

#define DEFAULTED "vi"

int
editor(file, prompt)
register char *file, *prompt;
{
	register char *editor;

	if ((editor = getenv("EDITOR")) == NULL)
		editor = DEFAULTED;
	if (*prompt) printf("%s\n", prompt);
	return(runp(editor, editor, file, 0));
}
