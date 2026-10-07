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
static char rcsid[] = "@(#)$RCSfile: env.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:23:12 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 27
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
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 * 
 * env.c	1.9  com/cmd/sh,3.1,9008 1/12/90 10:26:50
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "env_msg.h"

extern	char **environ;

char	*nvmatch(); 
int	printvar();
void	addname(), print(), usage();

#define NENV	250		/* max # of env vars we handle	*/
char	*newenv[NENV];		/* our new environment		*/
char	*nullp = NULL;		/* a null environment		*/

/*
 * NAME:	env, printenv
 *
 * SYNTAX:	env [-] [name] [name=value] ... [command arg...]
 *		printenv [name]
 *
 * FUNCTION:	Both env and printenv can be used to print out the
 *		current environment or a particular environment
 *		variable.  If called without any arguments, both
 *		simply print out all environment variables.  If
 *		called with 1 argument, and that argument matches
 *		an environment variable name, the environment
 *		variable name and its value is printed.
 *
 *		Env can be used to set environment variables, then
 *		execute a command with the new environment.  Arguments
 *		of the form "name=value" are added to the current
 *		environment before "command" is executed.  If "-" is
 *		specified, env ignores the current environment and
 *		builds a new environment from scratch using the
 *		"name=value" argument.
 *
 * RETURN VALUE DESCRIPTION:	this program returns 0 if no errors
 *		occurred, else it returns 1
 */

int
main(argc, argv, envp)
register int argc;
register char **argv;
char **envp;
{
	register char *progname;

	(void) setlocale (LC_ALL, "");

	/*
	 * get the program name minus the path
	 */
	if ((progname = strrchr(argv[0], '/')) != NULL)
		progname++;
	else
		progname = argv[0];

	argc--;
	argv++;

	/*
	 * check for the "-" argument
	 */
	if (argc > 0 && strcmp(*argv, "-") == 0) 
	{
		/*
		 * can't call printenv with "-"
		 */
		if (strcmp(progname, "printenv") == 0)
		{
			usage();	
			return(1);
		}
		else
		{
			/* start with a null environment */
			envp = &nullp;
			argc--;
			argv++;
		}
	}

	if (strcmp(progname,"printenv") == 0)
	{
		/*
		 * printenv specific ...
		 */
		if (argc == 0)
			/*
			 * just print the environment if no arguments given
			 */
			print();

		else if (argc == 1)                         
		{
			/*
			 * print out a particular value
			 */
			if (!printvar(argv[0]))
				return(1);
		}

		else
		{
			/*
			 * can only run with 1 or no arguments
			 */
			usage();
			return(1);
		}
	}

	else
	{		
		/*
		 * env specific ...
		 */

		/*
		 * put current environment into new environment
		 */
		for (; *envp != NULL; envp++)
			if (strchr(*envp, '=') != NULL)
				addname(*envp);

		/*
		 * add "name=value" arguments
		 */
		while (*argv != NULL && strchr(*argv, '=') != NULL)
			addname(*argv++);

		environ = newenv;	/* reset environment */

		if (*argv == NULL)
			/*
			 * just print out environment if no arguments given
			 */
			print();

		/*
		 * if there's only one argument left, try to print it.
		 * if it's not in our environment, execute it
		 */
		else if (*(argv + 1) != NULL || !printvar(*argv))
		{
			/*
			 * execute a command
			 */
			(void) execvp(*argv, argv);

			perror(*argv);
			return(1);
		}
	}

	return(0);
}

/*
 * NAME:  addname
 *
 * FUNCTION:  adds a variable and its value to the environment. 
 */

void
addname(arg)
register char *arg;
{
	register char **p;

	for (p = newenv; *p != NULL; p++)	/* search for variable */
	{
		if (p >= &newenv[NENV-1])	/* environment full... */
		{
			(void) fputs(NLgetamsg(MF_ENV, MS_ENV, OVERFLOW,
				"too many values in environment\n"), stderr);
			print();
			exit(1);
		}

		if (nvmatch(arg, *p) != NULL)	/* match? */
			break;
	}

	*p = arg;		/* add it */
}

/*
 * NAME:  print
 *
 * FUNCTION: prints out the entire environment
 *	     (variable names and their value).
 *	     
 */

void
print()
{
	register char **e;

	for (e = environ; *e != NULL; e++)
		(void) puts(*e);
}

/*
 *	s1 is either name, or name=value
 *	s2 is name=value
 *	if names match, return value of s2, else NULL
 */

char *
nvmatch(s1, s2)
register char *s1;
register char *s2;
{

	while (*s1 == *s2++)
		if (*s1++ == '=')
			return(s2);

	if (*s1 == '\0' && *(s2-1) == '=')
		return(s2);

	return(NULL);
}

/*
 * NAME: printvar
 * 
 * FUNCTION:  searches for the value of name and then it prints out the value.
 *
 * RETURN: returns 1 if we found and printed "name=value", else 0
 */

int
printvar(name)
register char *name;
{
	register char **e, *value;

	for (e = environ; *e != NULL; e++)	/* search enviroment */
		if ((value = nvmatch(name, *e)) != NULL)
		{
			printf ("%s\n", value);
			return(1);
		}

	return(0);
}

/*
 * NAME:  usage
 *
 * FUNCTION: display the correct usage of this command.
 *	     (only called for printenv!)
 */

void
usage()
{
	(void) fputs(NLgetamsg(MF_ENV, MS_ENV, USAGE,
			"Usage: printenv [name]\n"), stderr);
}
