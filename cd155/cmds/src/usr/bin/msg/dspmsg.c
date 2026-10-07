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
 * OSF/1 Release 1.0.3
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: dspmsg.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:31:02 $";
#endif
/*
 * COMPONENT_NAME: CMDMSG
 *
 * FUNCTIONS: main, pars_args
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * dspmsg.c	1.9  com/cmd/msg,3.1,9013 12/29/89 10:16:21
 */


/*                                                                   
 * EXTERNAL PROCEDURES CALLED: standard library functions
 */


#include <stdio.h>
#include <locale.h>
#include "catio.h"
#include "msgfac_msg.h"

#define isanumber(c) (c >= '0' && c <= '9') ? 1 : 0
#define toanumber(c) (c - '0')
#define NOT_SET -1
#define TRUE 	1
#define FALSE   0

struct arguments {
	int	set,
		msg,
		argmax;
	char	*catname,
		*def,
		**args;
};

	/*-- subroutine used to parse the input arguments ---*/
void parse_args(int argc, char *argv[], struct arguments *args);




/*
 * NAME: main
 *                                                                    
 * FUNCTION: 	Extract a message string from a catalog. Perform printf
 * 		style substitutions and print it out.
 * 
 * EXECUTION ENVIRONMENT:
 *  	User mode.
 *	                                                                    
 * RETURNS: 	Exit with 0, except when: the format string 
 *		is invalid.
 */  

int main(int argc,char *argv[]) 

	/* argc: Number of arguments */
	/* argv: argument vector */

{
	struct arguments   args;   /* place to store the parsed arguments*/
	char 	*message;	   /* place to store message */
	char 	*p;		   /* pointer to current pos within message */
	int 	idx, 		   /* current argument to be printed */
		reorder = NOT_SET; /* Reordering  (TRUE, FALSE, NOT_SET) */

	setlocale (LC_ALL,"");
	if (argc < 3) {
		die(NLgetamsg(MF_MSGFAC,MS_DSPMSG,M_DSPMSG, "Usage: dspmsg [-s setno] <catname> <msgno> ['default' arg ... ]"));
	}

/*______________________________________________________________________
	Parse the input arguments int the args structure.
  ______________________________________________________________________*/

	parse_args(argc,argv,&args);

/*______________________________________________________________________
	get the message out of the catalog.
  ______________________________________________________________________*/

	message = NLgetamsg(args.catname,args.set,args.msg,args.def);

/*______________________________________________________________________

	print out the message making the appropriate sub's for
	the parameters.  Reorder the parameters if necessary.
	Do not use mixed reordering!!!
  ______________________________________________________________________*/

	for (p = message , idx = 0 ; *p ; p++ )  {
		if (*p == '\\') {	/* process the '\' codes */
			switch (*++p) {
				case 'n': {
					putc('\n',stdout);
					break;
				}
				case 't': {
					putc('\t',stdout);
					break;
				}
				case 'b': {
					putc('\b',stdout);
					break;
				}
				case 'r': {
					putc('\r',stdout);
					break;
				}
				default: 
					putc(*p,stdout);

			}
		}
		else if (*p == '%') {
                /* do the %s printf style substitution */

			if (*++p == '%')  {
				putc(*p,stdout);
				continue;
			}
			if (isanumber(*p)) {
			/* do the reordering if appropriate */

				if (reorder == FALSE)
					exit(1);
				/* do not allow mixing of reorder types */

				for (idx = 0 ; isanumber(*p) ; p++) {
					idx += idx * 10 + toanumber(*p);
				}
				idx--;
				if (*p++ != '$') {
					exit (1);
				}
				if (*p != 's')
					exit (1);
				reorder = TRUE;
			}
			else {
				if (*p != 's') {
					exit(1);
				}
				if (reorder == TRUE)	
					exit(1);
				/* do not allow mixing of reorder types */

				reorder = FALSE;	
			}
			if (idx >= args.argmax)
				exit(1);
			fwrite(args.args[idx],strlen(args.args[idx]),1,stdout);
			idx++;				
		}
		else {
			putc(*p,stdout);
		}
	}
	exit(0);
	return(1);
}



/*
 * NAME: parse_args
 *
 * FUNCTION: Sets up the args-> data structure for main().
 *
 * EXECUTION ENVIRONMENT:
 *  	User mode.
 * 
 * RETURNS: void
 */

void parse_args(int argc, char *argv[], struct arguments *args) 

	/* argc: The number or arguments */
	/* argv: The input argument vector */
	/* args: The output argument structure */

{

	args->args = NULL;
	args->def = "";
	args->argmax = 0;
	args->set = 1;
	argv++ ; 
	argc--;				/* Skip the program name */
	if (!strcmp(*argv,"-s")) {	/* check for a set number */
		if (argc < 4) 		/* check for sufficient arguements */
		die(NLgetamsg(MF_MSGFAC,MS_DSPMSG,M_DSPMSG, "Usage: dspmsg [-s setno] <catname> <msgno> ['default' arg ... ]"));
		argv++; 
		argc--;				/* skip past the '-s' */
		sscanf(*argv,"%d",&args->set);	/* get the real set number */
		argv++; 
		argc--;				/* skip past the set number */
	}
	args->catname = *argv++;		/* get the cat name */
	argc--;
	if (!strcmp(*argv,"-s")) {		/* check for a set number */
		if (argc < 3) 		/* check for sufficient arguements */
		die(NLgetamsg(MF_MSGFAC,MS_DSPMSG,M_DSPMSG, "Usage: dspmsg [-s setno] <catname> <msgno> ['default' arg ... ]"));

		argv++; 
		argc--;				/* skip past the '-s' */
		sscanf(*argv,"%d",&args->set);	/* get the real set number */
		argv++; 
		argc--;				/* skip past the set number */
	}
	sscanf(*argv++,"%d",&args->msg);	/* scan the message number */
	argc--;
	if (argc) {				/* check for the arg count 
       						   for a default string */
		args->def= *argv++;
		argc--;
	}
	if (argc)  {
		args->args = argv;
		args->argmax = argc;
	}
}
