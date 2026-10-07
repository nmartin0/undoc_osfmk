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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_getopt.c,v 1.5 1994/11/19 02:31:39 mtm Exp $
 * History:
 * $Log: nx_getopt.c,v $
 * Revision 1.5  1994/11/19  02:31:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1992/11/13  19:49:00  shala
 * Return value was pointing to an item on the stack. Corrected by
 * returning a pointer to argv. This corrects invalid arg problem with
 * mkpart.
 *
 * Revision 1.3  1992/11/09  21:21:32  carbajal
 * Copy opts to saveopts and pass saveopts to strchk. This function
 * modifies the passed in string and hence are options list was being
 * trashed.
 *
 *
 */

#include <stdio.h>
#include <strings.h>
#define ERR(s, c)	if(nx_opterr){\
	(void) fputs(argv[0], stderr);\
	(void) fputs(s, stderr);\
	(void) fputs(c, stderr);\
	(void) fputc('\n', stderr);}

extern int strcmp();
extern char *strchr();
extern char *strchk();
extern char *strtok();
 
#define SAVEPT  savept

int	nx_opterr = 1;
int	nx_optind = 1;
char	*nx_optarg;

char*
nx_getopt(argc, argv, opts)
int	argc;
char	**argv, *opts;
{
	static int sp = 1;
	register char* c;
	char *cp;
        char *cp2,saveopts[256];
        char error = '0';
	char    *optopt;

	if(sp == 1) {
		if(nx_optind >= argc ||
		   argv[nx_optind][0] != '-' || argv[nx_optind][1] == '\0')
			return(NULL);
		} else if(strcmp(argv[nx_optind], "--") == NULL) {
			nx_optind++;
			return(NULL);
		}
	optopt = c = argv[nx_optind];
        c++;


	/* Make a copy of opts to prevent it from being trashed by
	 * strtok in strchk
	*/
        strcpy(saveopts,opts);

	if((cp=strchk(saveopts, c)) == NULL) {
		ERR(": illegal option -- ", c);
		if(argv[nx_optind][++sp] == '\0') {
			nx_optind++;
			sp = 1;
		}
		return(&error);
	}
        cp2 = cp;
	if((cp2 = strchr(cp,':')) != NULL) {
		if(argv[nx_optind+1] != NULL) {
                        nx_optind++;
			nx_optarg = argv[nx_optind];
                        nx_optind++;
                }
		else if(++nx_optind >= argc) {
			ERR(": option requires an argument -- ", c);
			sp = 1;
			return(&error);
		} else {
			nx_optarg = argv[nx_optind++];
			if (*nx_optarg == '-') {
			    ERR(": option requires an argument -- ", c);
			    sp = 1;
			    return(&error);
			}
		}
		sp = 1;
	} else {
	        sp = 1;
	        nx_optind++;
		nx_optarg = NULL;
	}
        if(cp2 != NULL)
             *cp2='\0';
        
	return(c);
}

/**
char*
strchk(s1,s2)
char *s1;
char *s2;
{
  char *s3;
  char tok[] = "," ;

  s3 = strtok(s1, tok );
  while (s3 != NULL) {
    if(strstr(s3,s2) != NULL)
             return(s3);
    s3 = strtok(NULL, tok);
  }
 return(NULL);
}
**/





char*
strchk(s1,s2)
char *s1;
char *s2;
{
  char *s3;

  s3 = strtok(s1, "," );
  while (s3 != NULL) {
    if(strstr(s3,s2) != NULL)
             return(s3);
    s3 = strtok(NULL, ",");
  }
/*** Should never get here ***/
 return(NULL);
}

