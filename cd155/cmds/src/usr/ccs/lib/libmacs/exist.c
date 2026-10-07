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
 *		Copyright (c) 1990 San Diego Supercomputer Center.
 *		All rights reserved.  The SDSC software License Agreement
 *		specifies the terms and conditions for redistribution.
 *
 * File:	exist.c
 *
 * Abstract:	This file contains the query functions of account database
 *		and of password file
 *****************************************************************************/
#ifdef LINT
static char	sccs_id[] = "@(#)exist.c	11.3 12/20/93";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <nx/nxacct.h>

#include "mac_def.h"
#include "agdb.h"
#include "misc.h"

static int	valid_acct __((char *));
static int	valid_user __((char *));

/*===========================================================================*
 * Function:	acct_exist_ok
 *
 * Abstract:	This function checks if the account exists in the account 
 *		database.
 *
 * Arguments: 	input - account name of id string.
 *
 * Return value:
 *		>=0	If the acount name or id exists
 *		-1	If account does not exist
 *
 * Note:
 *===========================================================================*/
int acct_exist_ok(input)
  char	*input;
{
  int	accts;

  accts = valid_acct(input);
  if (accts < 0) {
    printf("\nInvalid account: %s  Re-enter please.\n", input);
    return(-1);

  } else {
    return(accts);
  }
}

/*===========================================================================*
 * Function:    accts_exist_ok
 *
 * Abstract:    This function checks if each account name or id in the given
 *		list of strings exists in the account file.  
 *
 * Arguments:
 *	accts - list of user account strings
 *
 * Return value:
 *	>0	If all the acount name or id exists
 *	0	If one of the account does not exist
 *
 * Notes:
 *	Account names are replaced with account ids as the list is scanned.
 *===========================================================================*/
int accts_exist_ok(accts)
  char **accts;
{
  int			i;
  int			j;
  char			buf[32];
  int			agid;

  for (i = 0; accts[i] != NULL; i++) {
again:
    agid =  valid_acct(accts[i]);
    if (agid < 0) {
      printf("\nInvalid account: %s  Re-enter please.\n", accts[i]);
      return(0);

    } else {
      sprintf(buf, "%d", agid);

      /* check for duplication */
      for (j = 0; j < i; j++) {
        if (strcmp(accts[j], buf) == 0) {
          free(accts[i]);
          for (j = i; accts[j] != NULL; j++) {
            accts[j] = accts[j+1];
          }
          goto again;
        }
      }

      free(accts[i]);
      accts[i] = strdup(buf);
    }
  }
  return(i);
}

/*===========================================================================*
 * Function:    users_exist_ok
 *
 * Abstract:    This function checks if each user name or id in the given
 *		array of strings exists in the passwd file.  
 *
 * Arguments:
 *	users - list of user strings
 *
 * Return value:
 *	>0	If all the user exists
 *	0	If one of the users does not exist
 *
 * Notes:
 *	User names are replaced with user ids as the list is scanned.
 *===========================================================================*/
int users_exist_ok(users)
  char **users;
{
  int			i;
  int			j;
  char			buf[32];
  int			uid;

  for (i = 0; users[i] != NULL; i++) {
again:
    uid =  valid_user(users[i]);
    if (uid < 0) {
      printf("\nInvalid user: %s  Re-enter please.\n", users[i]);
      return(0);

    } else {
      sprintf(buf, "%d", uid);

      /* check for duplication */
      for (j = 0; j < i; j++) {
        if (strcmp(users[j], buf) == 0) {
          free(users[i]);
          for (j = i; users[j] != NULL; j++) {
            users[j] = users[j+1];
          }
          goto again;
        }
      }

      free(users[i]);
      users[i] = strdup(buf);
    }
  }
  return(i);
}

/*===========================================================================*
 * Function:	accts_users_exist_ok
 *
 * Abstract:    This function checks if each user in the given user list
 *		exists in the passwd file, also make sure that each user
 *		is a member of each every account in the account list
 *
 * Arguments:
 *	users - list of user name or id
 *	accts - list of account name or id
 *
 * Return value:
 *	If all the user exists, it will return the number of valid users.
 *	If one of the users does not exist, it will return 0.
 *
 * Notes:
 *	User names are replaced with user ids as the user list is scaned.
 *	Account names are replaced with account ids as the account list 
 *	is scaned.
 *===========================================================================*/
int accts_users_exist_ok(users, accts)
  char	**users;
  char	**accts;
{
  int		i;
  int		j;
  int		k;
  int		agids[MAX_AGIDS];
  int		match;
  int		nagid;
  int		agid;

  if (!users_exist_ok(users)) {
    return(0);
  }

  for (i = 0; users[i] != NULL; i++) {
    nagid = getagidfromuid(agids, atoi(users[i]));
    if (nagid == 0) {
      printf("\nUser %s, not in account file.  Re-enter please.\n", users[i]);
      return(0);
    }

    /* if all accounts skip */
    if ((int) accts <= 0) continue;

    for (k = 0; accts[k] != NULL; k++) {
      agid = atoi(accts[k]);
      for (j = 0, match = 0; j < nagid; j++) {
        if (agids[j] == agid) {
          match = 1;
          break;
        }
      }
      if (!match) {
        printf("\nUser %s is not in account %s.  Re-enter, please.\n",
               users[i], accts[k]);
        return (0);
      }
    }
  }
  return (i);
}

/*****************************************************************************
 * Internal
 *****************************************************************************/

/*===========================================================================*
 * Function:	valid_acct
 *
 * Abstract:	This function checks if the account exists in the account 
 *		database.
 *
 * Arguments:
 *	input - account name or id string.
 *
 * Return value:
 *		>=0	the account id, If the acount name or id exists
 *		-1	If account does not exist
 *
 * Note:
 *===========================================================================*/
static int valid_acct(input)
  char	*input;
{
  struct nxacct		*nxstuff;

  if (isdigit(input[0])) {
    /* is account id */
    if (!acct_id_syntex_ok(input)) {
      return(-1);
    }

    if (nx_getaid(atoi(input)) == NULL) {
      return(-1);
    }
    return(atoi(input));

  } else if (isalpha(input[0])) {
    /* is account name */
    if (!acct_name_syntex_ok(input)) {
      return(-1);
    }

    nxstuff = nx_getanam(input);
    if (nxstuff == NULL) {
      return(-1);
    }
    return(nxstuff->acct_id);

  } else {
    /* is unknown */
    return(-1);
  }
}

/*===========================================================================*
 * Function:	valid_user
 *
 * Abstract:	This function checks if the user exists in the passwd file.
 *
 * Arguments:
 *	input - user name or id string.
 *
 * Return value:
 *		>=0	the user id, if the user name or id exists
 *		-1	user does not exist
 *
 * Note:
 *===========================================================================*/
static int valid_user(input)
  char	*input;
{
  struct passwd		*pwdstuff;

  if (isdigit(input[0])) {
    /* is account id */
    if (!user_id_syntex_ok(input)) {
      return(-1);
    }

    if (getpwuid(atoi(input)) == NULL) {
      return(-1);
    }
    return(atoi(input));

  } else if (isalpha(input[0])) {
    /* is account name */
    if (!user_name_syntex_ok(input)) {
      return(-1);
    }

    pwdstuff = getpwnam(input);
    if (pwdstuff == NULL) {
      return(-1);
    }
    return(pwdstuff->pw_uid);

  } else {
    /* is unknown */
    return(-1);
  }
}

