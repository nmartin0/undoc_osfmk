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
 * (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
 */
/*
 * OSF/1 Release 1.0.4
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: dspcat.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:30:59 $";
#endif
/*
 * COMPONENT_NAME: CMDMSG
 *
 * FUNCTIONS: Main, make_msg, unpack
 *
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989.
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * dspcat.c	1.9  com/cmd/msg,3.1,9013 12/21/89 13:43:13
 */
 
#include <stdio.h>
#include <locale.h>
#include "catio.h"
#include "msgfac_msg.h" 
#define die(s)		puts(s), exit(1)

void	make_msg(), 	/*---- prints ._msg format messages ----*/
	unpack();	/*--- reformats string into ._msg (gencat) format ----*/
/*                                                                   
 * EXTERNAL PROCEDURES CALLED: 	standard library functions
 */


/*
 * NAME: main
 *                                                                    
 * FUNCTION: Decides what it has been asked to do (from the input arguments)
 *           and does it. (It either prints out a single message, a set, or the
 * 	     whole catalog.)
 *
 * EXECUTION ENVIRONMENT:
 *    	User mode.
 *                                                                    
 * RETURNS:   exit's(0);
 *
 */  



main(int argc, char *argv[]) 

{
	int 	set = ERR,	/*---- set to be printed ----*/
		msg = ERR,	/*---- msg to be printed ----*/
		msgout = FALSE;	/*---- TRUE for ._msg style output format ----*/
	int 	i,
		j;
	nl_catd catd;

	setlocale (LC_ALL,"");
	if (argc >= 2) {
		if (!memcmp(argv[1],"-g",2))  {
			msgout = TRUE;
			argc--;
			argv++;
		}
	}

	if (argc == 2)
		;
	else if (argc == 3) 
		sscanf(argv[2],"%d",&set);
	else if (argc == 4) {
		if (msgout)
			die( NLgetamsg(MF_MSGFAC, MS_DSPCAT, M_NO_ID, "No message id allowed with -g option.") );
		sscanf(argv[2],"%d",&set);
		sscanf(argv[3],"%d",&msg);
	}
	else {	/*---- Too many or too few arguments ----*/
		die( NLgetamsg(MF_MSGFAC, MS_DSPCAT, M_USAGE, "Usage:  dspcat [-g] catname [set#] [msg#]") );
	}

	if ((catd = catopen(argv[1], 0)) == CATD_ERR) {	
                /*---- open the cat ----*/
		printf( NLgetamsg(MF_MSGFAC, MS_DSPCAT, M_CAT_NO_OPEN, 
                        "Unable to open specified catalog (%s)\n") ,argv[1]);
		exit(1);
	}

	if (msgout) {	/*---- ._msg style output format ----*/
		make_msg(catd,set);
		exit(0);
	}

/*______________________________________________________________________
	Standard dspcat style output
  ______________________________________________________________________*/

	if (argc == 4) {	/*---- both set and message specified ----*/
		fputs(catgets(catd,set,msg,""),stdout);
	}
	else if (argc == 3) {	/*---- just the set ----*/
		for (i = 0 ; i < catd->_set[set]._n_msgs ; i++) {
			if (catd->_set[set]._mp[i]._offset)
				printf("%s\n",catgets(catd,set,i,""));
		}
	}
	else if (argc == 2) {	/*---- print the whole catalog ----*/
		for (j = 0 ; j <= catd->_setmax ; j++) {
			for (i = 0 ; i < catd->_set[j]._n_msgs ; i++) {
				if (catd->_set[j]._mp[i]._offset)
					printf("%d : %d %s\n",j,i,
                                               catgets(catd,j,i,""));
			}
		}
	}
	exit(0);
	return(1);
}




/*
 * NAME: make_msg
 *
 * FUNCTION: 	Makes message and prints its output with a ._msg style format 
 *		(i.e. suitable for input to gencat).
 *
 * EXECUTION ENVIRONMENT:
 *    	User mode.
 *                                                                    
 * RETURNS: 	void
 */

void make_msg(catd, set) 
nl_catd catd;
int set;

	/*---- catd: catalog descriptor ----*/
	/*---- set: set number (ERR for all sets) ----*/

{
	int 	i,	/*---- Misc counter(s) used for loops ----*/
		j,	/*---- Misc counter(s) used for loops ----*/
		setmin,	/*---- minimum set to be printed ----*/
		setmax; /*---- maximum set to be printed ----*/

	char 	buffer[NL_TEXTMAX * 2];
  			/*---- buffer to store unpacked message in  ----*/

	if (set == ERR) {
		setmin = 1;
		setmax = catd->_setmax;
	}
	else 
		setmin = setmax = set;

	for (j = setmin ; j <= setmax ; j++) {

		printf("\n$delset %d\n",j);	/* header info for each set */
		printf("$set %d\n",j);
		printf("$quote \"\n\n");

		for (i = 0 ; i < catd->_set[j]._n_msgs ; i++) {
			if (catd->_set[j]._mp[i]._offset) {
				unpack(buffer,catgets(catd,j,i,""));	
				printf("%d\t\"%s\"\n",i,buffer);
			}
		}
	}
} 



/*
 * NAME: unpack
 *
 * FUNCTION: unpack a text string into a format suitable for gencat.   
 *
 * EXECUTION ENVIRONMENT:
 *    	User mode.
 *                                                                    
 * RETURNS: void
 */

void unpack (t, ss) 
char *t; char *ss;

	/*---- t: Target buffer ----*/
	/*---- ss: Source buffer ----*/

{
	unsigned char *s=(unsigned char *)ss;

	while (*s) {	/*---- Until end of string ----*/
		if (*s == '"') {	/*---- escape embedded quotes ----*/
			s++;
			*t++ = '\\';
			*t++ = '"';
		}
		else if (*s == '\\') {	/*--- escape embedded backslash ---*/
			s++;
			strcpy(t, "\\\\");
			t += 2;
		}
		else if (*s >= ' ') {	/*---- Normal copy ----*/
			*t++ = *s++;
		}
		else if (*s == '\n') {	/*---- unpack control chars ----*/
			s++;
			strcpy(t,"\\n");
			t += 2;
			*t++ = '\\';	/* Display new lines with \n\ format */
			*t++ = '\n';
		}
		else if (*s == '\t') {
			s++;
			strcpy(t,"\\t");
			t += 2;
		}
		else if (*s == '\v') {
			s++;
			strcpy(t,"\\v");
			t += 2;
		}
		else if (*s == '\b') {
			s++;
			strcpy(t,"\\b");
			t += 2;
		}
		else if (*s == '\r') {
			s++;
			strcpy(t,"\\r");
			t += 2;
		}
		else if (*s == '\f') {
			s++;
			strcpy(t,"\\f");
			t += 2;
		}
		else {			/*---- catch all for octal chars ----*/
			t += sprintf(t,"\\%03o",*s);
			s++;
		}
	}
	*t = '\0';
}

