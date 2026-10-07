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
 
/*****************************************************************************
 *              Copyright (c) 1990 San Diego Supercomputer Center.
 *              All rights reserved.  The SDSC software License Agreement
 *              specifies the terms and conditions for redistribution.
 *
 * File:        maclist.c
 *
 * Abstract:	This file contain the main driver for the MACS
 *		utility - maclist.
 *****************************************************************************/

#ifdef LINT
static char     sccs_id[] = "%W% %H%";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <nx/nxacct.h>
#include <pwd.h>
#include "mac_def.h"
#include "filename.h"

/* These, and the arrays that are dimensioned by them, will be removed */
/* when an idpairs structure is implemented in these programs */

#define INSIZE 4096
extern mac_list();  /* from maclib.a  */
extern macinit();   /* from maclib.a */
extern macexit();   /* from maclib.a */
extern is_macwatch();   /* from maclib.a */

#define USAGE	"\nUsage: maclist [ -A | -U | [-a <accounts>] [-u <users>] ]\nMultiple <accounts> or <users> must be separated by commas (no spaces allowed)\n"

int getoptlst();

int nusers;
int rval=0;
int accts[MAX_AGIDS];
int users[MAX_UIDS];
int *pusers;
int *paccts;

extern int	getopt(int, char **, char *);
extern char	*optarg;
extern int	optind;
extern int	opterr;

/*===========================================================================*
 * Function:    main
 *
 * Abstract:    This function validates command-line options, connects to
 *		MACD and issue requests for information on all accounts and
 *		users specified in the command-line options and prints them
 *		to the stdout.
 *
 * Arguments:	argc -	argument count
 *		argv -	argument list
 *
 * Return value:  None
 *
 * Notes:
 *===========================================================================*/
int main(argc, argv)
int		argc;
char		**argv;
{
    int		aflg	= 0;
    int		uflg	= 0;
    int		uallflg	= 0;
    int		aallflg	= 0;
    int		cpuflg	= 0;
    int		remflg	= 0;
    int		i, j, k;
    int		c;
    int		optargc;
    char		**optargv;
    int		naccts = 0;
    uid_t		user_id;
    struct passwd	*pwent;
    struct nxacct	*nxaent;
    extern struct passwd	*getpwuid(), *getpwnam();

    char		*malloc_hack;

    malloc_hack = malloc(1024);

    /* verify that MACS is in "macwatch" mode */
    is_macwatch();

    /* validate the user who is runing the utility */
    user_id = getuid();
    if (user_id < 0) {
        (void) fprintf(stderr, "\nFailed in finding your user id, abort\n");
       exit(1);
    }
    if ((pwent = getpwuid (user_id)) == NULL) {
       (void) fprintf(stderr, "\nYou have an invalid user-id %d, abort\n", 
		user_id);
       exit(1);
    }
    

/*-------------------------------------------------------------------------*
 * Parse and check command line args
 *-------------------------------------------------------------------------*/
    while ((c = getopt(argc, argv, "crUAa:u:")) != -1) {
        switch(c) {
        case '?': exit(1);
        case ':': exit(1);
        case 'c':
            if (remflg >= 1) {
                (void) fprintf(stderr, 
		    "\n-c option cannot be used with -r option, abort\n");
                exit(1);
            }
            cpuflg++;
            break;

        case 'r':
            if (cpuflg >= 1) {
                (void) fprintf(stderr, 
		    "\n-r option cannot be used with -c option, abort\n");
                exit(1);
            }
            remflg++;
            break;

        case 'U':
            if (aallflg || aflg || uflg) {
                (void) fprintf(stderr,
		    "\n-U option cannot be used with -A or -a or -u options, abort\n");
		(void) fprintf(stderr, USAGE);
	        exit(1);
            }
            /* Get all accounts the user can see.  Root can see all and
	     non-root can only see accounts s/he belongs */
            if (user_id) {    /* non-root */
                if ((naccts = getagidfromuid(accts, user_id)) < 0) {
                    (void) fprintf(stderr,
                        "\nFailed in getting account access list for user %d (%s), abort\n",
                        user_id, pwent->pw_name);
                    exit(1);
                } else if (!naccts) {
                    (void) fprintf(stderr,
                        "\nNo account for user %d (%s), abort\n",
                        user_id, pwent->pw_name);
                    exit(1);
                }
            }
            else if ((naccts = getagidfromuid(accts, -1)) < 0) {
                (void) fprintf(stderr, "\n%s is empty, abort\n", 
			NXACCOUNT_FNAME);
                exit(1);
            }
            uallflg = 1;
            break;

        case 'A':
            if (uallflg || aflg || uflg) {
                (void) fprintf(stderr,
		    "\n-A option cannot be used with -U or -a or -u options, abort\n");
		(void) fprintf(stderr, USAGE);
                exit(1);
            }
            if (user_id) {    /* non-root */
                if ((naccts = getagidfromuid(accts, user_id)) < 0) {
                    (void) fprintf(stderr,
                        "\nFailed in getting account access list for user %d (%s), abort\n",
                        user_id, pwent->pw_name);
                    exit(1);
                } else if (!naccts) {
                    (void) fprintf(stderr,
                        "\nNo account is accessible by user %d (%s), abort\n",
                        user_id, pwent->pw_name);
                    exit(1);
                }
            }
            else if ((naccts = getagidfromuid(accts, -1)) < 0) {
                (void) fprintf(stderr, "\n%s is empty, abort\n", 
			NXACCOUNT_FNAME);
                exit(1);
            }
            aallflg = 1;
            break;

      case 'a':
            if (uallflg || aallflg) {
                (void) fprintf(stderr,
		    "\n-a option cannot be used with -A or -U options, abort\n");
		(void) fprintf(stderr, USAGE);
                exit(1);
        
	    }
            if ((optargc = getoptlst(optarg, &optargv)) <= 0) {
                (void) printf("\nFailed in parsing account list\n\n");
		(void) fprintf(stderr, USAGE);
                exit(1);
            }
            for (i = 0, j = 0, rval = 0; i < optargc; i++) {
                /* validate the accounts with /etc/nxaccount */
                if (optargv[i][0] <= '9' && optargv[i][0] >= '0') {
                    if ((nxaent = nx_getaid(atoi(&optargv[i][0]))) == NULL) {
                        (void) fprintf(stderr, 
			    "\nInvalid account %s, skipped\n", &optargv[i][0]);
              
			continue;
                    }
                } else {
                    if ((nxaent = nx_getanam(&optargv[i][0])) == NULL) {
                        (void) fprintf(stderr, 
			    "\nInvalid account %s, skipped\n", &optargv[i][0]);
                        continue;
                    }
                }
                rval++;
                /* validate permission to see */
                if (user_id) {  
		    /* non-root, only can see accounts if is a member */
		    if ((pwent = getpwuid (user_id)) == NULL) {
		       (void) fprintf(stderr, "\nYou have an invalid user-id %d, abort\n", 
				user_id);
		       exit(1);
		    }
                    for (k=0; nxaent->acct_mem[k]!=NULL; k++) {
                        if (strcmp(pwent->pw_name, nxaent->acct_mem[k]) == 0) {
                            accts[j++] = nxaent->acct_id;
                            break;
                        }
		    }
                    if (nxaent->acct_mem[k] == NULL) {
                        (void) fprintf(stderr, 
				"\nPermission denied for listing account %s, skipped\n",
			optargv[i]);
			continue;
		    }
                }
                else {  /* root, can see all */
                    accts[j++] = nxaent->acct_id;
                }
            }
            if (!rval) {
                (void) fprintf(stderr, 
			"\nNo valid accounts specified, abort\n");
                exit(1);
            }
            if ((naccts = j) == 0) {
                (void) fprintf(stderr, 
		    "\nPermission denied on all specified accounts, abort\n");
                exit(1);
            }
            aflg++;
            break;

        case 'u':
            if (uallflg || aallflg) {
                (void) fprintf(stderr,
		    "\n-u option cannot be used with -A or -U options, abort\n");
		(void) fprintf(stderr, USAGE);
                exit(1);
            }
            if ((optargc = getoptlst(optarg, &optargv)) <= 0) {
                (void) fprintf(stderr, 
		    "\nFailed in parsing user list, abort\n");
		(void) fprintf(stderr, USAGE);
                exit(1);
            }
            for (i = 0, j = 0, rval = 0; i < optargc; i++) {
	    /* validate the users with password file */
                if (optargv[i][0] <= '9' && optargv[i][0] >= '0') {
                    if ((pwent = getpwuid((uid_t) atoi(&optargv[i][0]))) == NULL) {
                        (void) fprintf(stderr, "\nInvalid user %s, skipped\n", &optargv[i][0]);
                        continue;
                    }
                } else if ((pwent = getpwnam(&optargv[i][0])) == NULL) {
                    (void) fprintf(stderr, 
			"\nInvalid user %s, skipped\n", &optargv[i][0]);
                    continue;
                }
                users[j++] = pwent->pw_uid;
                nusers++;
            }
            if (nusers  == 0) {
                (void) fprintf(stderr, "\nNo valid users specified, abort\n");
                exit(1);
            }
            uflg++;
            break;
        default:
	    (void) fprintf(stderr, "\nInternal error!  Unexpected value returned by getopt()\n");
	    exit(2);
        }
    }

    /* no non-option arguments are allowed */
    if(optind < argc) {
	(void) fprintf(stderr, "\nUnrecognized argument \"%s\"\n", argv[optind]);
	(void) fprintf(stderr, USAGE);
	exit(1);
    }

    /* -A account listing only */
    if (aallflg) {
        if ((rval = macinit()) == -1) {
            (void) fprintf(stderr,
                "\nFailed in initiating communication with MACD\n");
            exit(1);
        }
        rval = mac_list(accts, users, naccts, 0, cpuflg, remflg);
        macexit();
    }

    /* -U or -a and no -u */
    if (uallflg || (aflg && !uflg)) {
        if ((rval = macinit()) == -1) {
            (void) fprintf(stderr,
                "\nFailed in initiating communication with MACD\n");
            exit(1);
        }
	for (i = 0, paccts = accts; i<naccts; i++, paccts++) {
	    if((nusers = getuidfromagid(users, accts[i])) < 0) {
		(void) fprintf(stderr,
		    "\nFailed in getting user access list for account %d, abort\n",
		    accts[i]);
		exit(1);
	    }
            if ((rval = mac_list(paccts, users, 1, nusers, cpuflg, remflg)) 
			== -1) {
		(void) fprintf(stderr,"\nFailed in completing list function\n");
        	macexit();
	    }
	}
        macexit();
    }

    /* no argument */
    if (argc == 1 || (!uallflg && !aallflg && !aflg && !uflg)) {
        users[0] = getuid();
        nusers = 1;
        accts[0] = nx_getacctid();
        if (accts[0] < 0) {
            (void) fprintf(stderr, 
		"\nYour default account is invalid, abort\n");
            exit(1);
        }
        naccts = 1;
	/* validate the account with /etc/nxaccount */
	if ((nxaent = nx_getaid(accts[0])) == NULL) {
	    (void) fprintf(stderr, 
		    "\nDefault account %d does not exist, abort\n", accts[0]);
	    exit(1);
	}
	/* validate permission to see */
	if (user_id) {  
	    /* non-root, only can see accounts if is a member */
	    for (k=0; nxaent->acct_mem[k]!=NULL; k++) {
		if (strcmp(pwent->pw_name, nxaent->acct_mem[k]) == 0) {
		    break;
		}
	    }
	    if (nxaent->acct_mem[k] == NULL) {
		(void) fprintf(stderr, 
			 "\nPermission denied for listing default account %d, abort\n",
			accts[0]);
			exit(1);
	    }
	}
    }

    /* -u only */
    else if (uflg && !aflg) {
    int tmp_accts[MAX_AGIDS], accts_permit[MAX_AGIDS], naccts_permit;
    if (user_id) { /* non-root */
	    if ((naccts_permit = getagidfromuid(accts_permit, user_id)) < 0) {
	        (void) fprintf(stderr,
		    "\nFailed in getting account access list for you (uid=%d), abort\n",
		    user_id);
	        exit(1);
	    }
	    if (naccts_permit == 0) {
                (void) fprintf(stderr, "Permission denied\n");
	        exit(1);
	    }
	}
        for (i=0; i<nusers; i++) { 
	    /* get all accounts for each user in list */
            if ((rval = getagidfromuid(tmp_accts, users[i])) < 0) {
                (void) fprintf(stderr,
                    "\nFailed in getting account access list for uid=%d, skipped\n",
                    users[i]);
		for (j=i; j>nusers-1; j++) users[j] = users[j+1];
		users[nusers--] = 0;
		i--;
                continue;
            } else if (!rval) {
                (void) fprintf(stderr,
                    "\nNo account for user %d (%s), skipped\n",
                    users[i], pwent->pw_name);
		for (j=i; j>nusers-1; j++) users[j] = users[j+1];
		users[nusers--] = 0;
		i--;
                continue;
            }
	    /* add new accounts to account list */
	    for (j=0; j<rval; j++) {
		for (k=0; k<naccts; k++)
		    if (accts[k] == tmp_accts[j]) break;
	        if (k >= naccts) {
		    if (user_id == 0) accts[naccts++] = tmp_accts[j];
		    else for (k=0; k<naccts_permit; k++)
		        if (accts_permit[k] == tmp_accts[j])
			    accts[naccts++] = tmp_accts[j];
		}
	    }
        } 
	if (naccts == 0) {
	    (void) fprintf(stderr, "\nPermission denied\n");
	    exit(1);
	}
    } 

    /* no argument or -u or -a -u */
    if ((rval=macinit())==-1) {
        (void) fprintf(stderr,
            "\nFailed in initiating communication with MACD, abort\n");
        exit(1);
    }
    for (i = 0, paccts = accts; i<naccts; i++, paccts++) {
	/* validate the account with /etc/nxaccount */
	if ((nxaent = nx_getaid(*paccts)) == NULL) {
	    (void) fprintf(stderr, 
		    "\nAccount %d does not exist, skipping\n", *paccts);
	    continue;
	}
	/* validate that users exist in account */
        for (j = 0, pusers = users; j<nusers; j++, pusers++) {
	    if ((pwent = getpwuid (*pusers)) == NULL) {
	        (void) fprintf(stderr, "\nUser ID=%d does not exist, skipping\n", 
		        *pusers);
		continue;
	    }
	    if(aflg) {
		/* if user requested both a specific account and specific
		   users (both -u and -a), issue a warning message if one
		   of the requested users does not exist in one of the
		   requested accounts */
	        for (k=0; nxaent->acct_mem[k]!=NULL; k++) {
	            if (strcmp(pwent->pw_name, nxaent->acct_mem[k]) == 0) {
		        break;
		    }
	        }
	        if (nxaent->acct_mem[k] == NULL) {
		    (void) fprintf(stderr, 
			     "\nUser %s does not exist in account %s, skipping\n",
			    pwent->pw_name, nxaent->acct_name);
			    continue;
	        }
	    }
	}
        if ((rval = mac_list(paccts, users, 1, nusers, cpuflg, remflg)) == -1) {
            (void) fprintf(stderr,"\nFailed in completing list function\n");
            macexit();
        }
    }
    macexit();
    return(0);
}

/******************************************************************************
* Function getoptlst() returns a list of pointers to tokens.  The tokens are
* pointed to by optarg on entry and the tokens are separated by commas.
*******************************************************************************/
#define MAXTOKS 50

int getoptlst(ignore, optv)
    char	*ignore;
    char	***optv;
{
    char *newlist, *token;
    int ntoks;
    char *tokptr[MAXTOKS];
    extern char *optarg;

    /*
     * Duplicate the memory and get tokens until we can't get no more
     */

    newlist = strdup(optarg);
    for (ntoks = 0; ntoks < MAXTOKS; ntoks++) {
      token = strtok(((ntoks == 0) ? optarg : NULL), ",\n");
      if (token == NULL) break;
      tokptr[ntoks] = strdup(token);
    }

    /*
     * Now, copy the data and free the memory -
     * don't bother with moving the strings
     */

    *optv = (char **) malloc(ntoks*sizeof(char *));
    memcpy(*optv, tokptr, ntoks * sizeof(char *));
    free(newlist);
    return(ntoks);
}
