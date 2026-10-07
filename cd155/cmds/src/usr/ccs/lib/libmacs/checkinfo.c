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
 * File:	checkinfo.c
 *
 * Abstract:	These routines are called by mac_ui() to get and validate user
 *		input and generate option arguments to reflect user input
 *		request and return the number of options.
 *
 *		If user skips input for account or user specification and
 *		 default value is permited, no option argument will be
 *		 generated and 0 is returned;
 *
 *		If skip means default, an option argument will be generated
 *		 using the default value and 1 is returned;
 *
 *		If skip means no_change, no option argument will be
 *		 generated and 0 is returned.
 *
 *		If user entered q or Q, these calls return -1;
 *		If there is a system error, these calls return -2;
 *		otherwise they return the number of arguments generated;
 *
 *		The only exception is userpercent() which returns the
 *		 number of arguments + OVERSIGN to signal caller for
 *		 skipping the next item, which should be TIME INPUT.
 *
 *****************************************************************************/
#ifdef LINT
static char     sccs_id[] = "%W% %H%";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "conf.h"
#include "mac_def.h"
#include "misc.h"
#include "filename.h"

/*===========================================================================*
 * Function:	acct_name_and_id
 *
 * Abstract:	This function asks the user to specify a account name,
 *		account id, and account description.  It's called only for
 *		adding a new account.
 *
 * Arguments:	argp -	pointer to the next argument to be filled
 *		skip_keep -	1	skip input means no change
 *				0	skip input takes default values
 *
 * Return value:
 *		>=0	number of arguments filled
 *		-1	user want to quit
 *		-2	error
 *
 * Notes:
 *	The account's existence in account database are not checked
 *	for even if the account is in account database, it may not be
 *	in the MACD database, and the account should still be allowed 
 *	to add into the MACD database.
 *===========================================================================*/
/* ARGSUSED */
int acct_name_and_id(argp, skip_keep)
  char	**argp;
  int	skip_keep;
{
  int		in_len;
  char		*name;
  char		*id;
  char		*desc;
  extern void free();

  /*-------------------------------------------------------------------------*
   * Get account name, id, and description.
   *-------------------------------------------------------------------------*/
  name = get_aname();
  if ((int)name <= 0) {
    return((int)name);
  }

  id = get_aid();
  if ((int)id <= 0) {
    free(name);
    return((int)id);
  }

  desc = get_adesc();
  if ((int)desc <= 0) {
    free(name);
    free(id);
    return((int)desc);
  }

  /*-------------------------------------------------------------------------*
   * Convert the strings to a command line argument.
   *-------------------------------------------------------------------------*/
  /* "-%c" name ',' id ',' desc NULL */
  in_len = strlen(name) + strlen(id) + strlen(desc) + 5;

  *argp = (char *)malloc(in_len);
  if (*argp == NULL) {
    free(name);
    free(id);
    free(desc);
    printf("\nMemory allocation in acct_name_and_id() failed\n");
    return(-2);
  }

  sprintf(*argp, "-%c%s,%s,%s", op_code[NEW_ACCT_IDX], name, id, desc);
  free(name);
  free(id);
  free(desc);
  return(1);
}

/*===========================================================================*
 * Function:	accts_name_or_id
 *
 * Abstract:	This function asks the user to specify multiple accounts using
 *		names or ids.  It's called to list/set/update/delete accounts.
 *
 * Arguments:	argp -	pointer to the next argument to be filled
 *		skip_keep -	1	skip input means no change
 *				0	skip input takes default values
 *
 * Return value:
 *		>=0	number of arguments filled
 *		-1	user want to quit
 *		-2	error
 *
 * Notes:
 *	The accounts must be exist in account database.
 *	Skiped input will defaulted to all accounts, but it's not allowed under
 *	delete.
 *===========================================================================*/
int accts_name_or_id(argp, skip_keep)
  char	**argp;
  int	skip_keep;
{
  int		i;
  int		in_len;
  char		**accts;
  char		*ptr;

  /*-------------------------------------------------------------------------*
   * Get list of accounts
   *-------------------------------------------------------------------------*/
  accts = get_accts(skip_keep);
  if ((int)accts <= 0) {
    return((int)accts);
  }

  /*-------------------------------------------------------------------------*
   * Convert the strings to a command line argument.
   *-------------------------------------------------------------------------*/
  in_len = 4;
  for (i = 0; accts[i] != NULL; i++) {
    in_len += strlen(accts[i]) + 1;
  }

  *argp = (char *)malloc(in_len);
  if (*argp == NULL) {
    printf("\nMemory allocation in accts_name_or_id() failed\n");
    return(-2);
  }

  sprintf(*argp, "-%c", op_code[ACCTS_NAME_OR_ID_IDX]);
  ptr = (*argp) + 2;

  for (i = 0; accts[i] != NULL; i++) {
    sprintf(ptr, "%s,", accts[i]);
    ptr += strlen(accts[i]) + 1;
  }
  *(ptr-1) = '\0';	/* over write last comma with a NULL char */

  frarray(accts);
  return(1);
}

/*==========================================================================*
 * Function:	accts_new_users_name_or_id
 *
 * Abstract:	This function asks the user to input accounts name or id, and
 *		then user names or ids.  It's called only for adding users to
 *		accounts.
 *
 * Arguments:	argp -	pointer to the next argument to be filled
 *		skip_keep -	1	skip input means no change
 *				0	skip input takes default values
 *
 * Return value:
 *		>=0	number of arguments filled
 *		-1	user want to quit
 *		-2	error
 *
 * Notes:
 * All accounts specified must exist in account database, but the users' 
 * existence under the accounts are not checked for even if the users
 * are already members of the accounts in account database, they may
 * not be in the accounts in MACD database, and they should be allowed
 * to be added into the MACD database.
 *===========================================================================*/
/*ARGSUSED*/
int accts_new_users_name_or_id(argp, skip_keep)
  char	**argp;
  int	skip_keep;
{
  int		i;
  int		argc;
  int		in_len;
  char		**accts;
  char		**users;
  char		*ptr;
  extern void free();

  argc	= 0;
  /*-------------------------------------------------------------------------*
   * Get accounts
   *-------------------------------------------------------------------------*/
  accts = get_accts(SKIP_DEFAULT);
  if ((int)accts < 0) {
    return((int)accts);

  } else if ((int)accts > 0) {
    /*-----------------------------------------------------------------------*
     * Convert the strings to a command line argument.
     *-----------------------------------------------------------------------*/
    in_len = 4;				/* "-%c" id, id, ... "," NULL */
    for (i = 0; accts[i] != NULL; i++) {
      in_len += strlen(accts[i]) + 1;
    }

    *argp = (char *)malloc(in_len);
    if (*argp == NULL) {
      printf("\nMemory allocation in accts_new_users_name_or_id() failed\n");
      return(-2);
    }

    sprintf(*argp, "-%c", op_code[ACCTS_NAME_OR_ID_IDX]);
    ptr = (*argp) + 2;

    for (i = 0; accts[i] != NULL; i++) {
      sprintf(ptr, "%s,", accts[i]);
      ptr += strlen(accts[i]) + 1;
    }
    *(ptr-1) = '\0';		/* over write last comma with a NULL char */

    frarray(accts);
    argc++;

  } else {
    ;	/* All accounts */
  }

  /*-------------------------------------------------------------------------*
   * Get users
   *-------------------------------------------------------------------------*/
  users = get_new_users();
  if ((int)users < 0) {
    if (argc == 1) {
      free(*argp);
    }
    return((int)users);

  } else if ((int)users > 0) {

    /*-----------------------------------------------------------------------*
     * Convert the strings to a command line argument.
     *-----------------------------------------------------------------------*/
    in_len = 4;		/* "-%c" (id, id, ... ",") NULL */
    for (i = 0; users[i] != NULL; i++) {
      in_len += strlen(users[i]) + 1;
    }

    *(argp + argc) = (char *)malloc(in_len);
    if (*(argp + argc) == NULL) {
      printf("\nMemory allocation in accts_new_users_name_or_id() failed\n");
      return(-2);
    }

    sprintf(*(argp+argc), "-%c", op_code[USERS_LOGIN_OR_ID_IDX]);
    ptr = (*(argp+argc)) + 2;

    for (i = 0; users[i] != NULL; i++) {
      sprintf(ptr, "%s,", users[i]);
      ptr += strlen(users[i]) + 1;
    }
    *(ptr-1) = '\0';		/* over write last comma with a NULL char */

    frarray(users);
    argc++;

  } else {
    ;	/* All users */
  }

  return(argc);
}

/*===========================================================================*
 * Function:	accts_users_name_or_id
 *
 * Abstract:	This function asks the user to specify account names or ids,
 *		and then user names or ids.  The given users must be in all
 *		the accounts named.
 *		It's called for list/change/delete users from their accounts.
 *
 * Arguments:	argp -	pointer to the next argument to be filled
 *		skip_keep -	1	skip input means no change
 *				0	skip input takes default values
 *
 * Return value:
 *		>=0	number of arguments filled
 *		-1	user want to quit
 *		-2	error
 *
 * Notes:
 *	Skip input will defaulted to all.  But default on both
 *	accounts and users is not allowed under change or delete
 *===========================================================================*/
int accts_users_name_or_id(argp, skip_keep)
  char	**argp;
  int	skip_keep;
{
  int		i;
  int		argc;
  int		in_len;
  char		**accts;
  char		**users;
  char		*ptr;
  extern void free();

  argc = 0;
  /*-------------------------------------------------------------------------*
   * Get accounts
   *-------------------------------------------------------------------------*/
  accts = get_accts(SKIP_DEFAULT);
  if ((int)accts < 0) {
    return((int)accts);

  } else if ((int)accts > 0) {
    /*-----------------------------------------------------------------------*
     * Convert the strings to a command line argument.
     *-----------------------------------------------------------------------*/
    in_len = 4;				/* "-%c" id, id, ... "," NULL */
    for (i = 0; accts[i] != NULL; i++) {
      in_len += strlen(accts[i]) + 1;
    }

    *argp = (char *)malloc(in_len);
    if (*argp == NULL) {
      printf("\nMemory allocation in accts_users_name_or_id() failed\n");
      return(-2);
    }

    sprintf(*argp, "-%c", op_code[ACCTS_NAME_OR_ID_IDX]);
    ptr = (*argp) + 2;

    for (i = 0; accts[i] != NULL; i++) {
      sprintf(ptr, "%s,", accts[i]);
      ptr += strlen(accts[i]) + 1;
    }
    *(ptr-1) = '\0';		/* over write last comma with a NULL char */
    argc++;

  } else {
    ;		/* All accounts */
  }

  /*-------------------------------------------------------------------------*
   * Get users
   *-------------------------------------------------------------------------*/
  if (!skip_keep || argc) {
    users = get_users(accts, SKIP_DEFAULT);
  } else {
    users = get_users(accts, skip_keep);
  }

  if ((int)accts > 0) {
    frarray(accts);
  }

  if ((int)users < 0) {
    if (argc < 0) {
      free(*argp);
    }
    return((int)users);

  } else if ((int)users > 0) {
    /*-----------------------------------------------------------------------*
     * Convert the strings to a command line argument.
     *-----------------------------------------------------------------------*/
    in_len = 4;
    for (i = 0; users[i] != NULL; i++) {
      in_len += strlen(users[i]) + 1;
    }

    *(argp+argc) = (char *)malloc(in_len);
    if (*(argp+argc) == NULL) {
      printf("Memory allocation in accts_users_name_or_id() failed\n");
      return(-2);
    }
    sprintf(*(argp+argc), "-%c", op_code[USERS_LOGIN_OR_ID_IDX]);
    ptr = (*(argp+argc)) + 2;

    for (i = 0; users[i] != NULL; i++) {
      sprintf(ptr, "%s,", users[i]);
      ptr += strlen(users[i]) + 1;
    }
    *(ptr-1) = '\0';
    frarray(users);
    argc++;

  } else {
    ;	/* All users */
  }
  return(argc);
}

/*===========================================================================*
 * Function:	acct_users_name_or_id
 *
 * Abstract:	This function asks the user to specify two accounts name or id.
 *		It's called only for transfering time from one account to
 *		another.
 *
 * Arguments:	argp -	pointer to the next argument to be filled
 *		skip_keep -	1	skip input means no change
 *				0	skip input takes default values
 *
 * Return value:
 *		>=0	number of arguments filled
 *		-1	user want to quit
 *		-2	error
 *
 * Notes:
 *===========================================================================*/
/*ARGSUSED*/
int two_acct_name_or_id(argp, skip_keep)
  char	**argp;
  int	skip_keep;
{
  int		from_acct;
  int		to_acct;
  char		buff[80];
  extern void free();

  from_acct = get_acct(FROM_ACCT_IDX);
  if (from_acct < 0) {
    return(from_acct);
  }
  sprintf(buff, "-%c%d", op_code[FROM_ACCT_IDX], from_acct);

  *argp = strdup(buff);
  if (*argp == NULL) {
    printf("Memory allocation in two_acct_name_or_id() failed\n");
    return(-2);
  }

again:
  to_acct = get_acct(TO_ACCT_IDX);
  if (to_acct < 0) {
    free(*argp);
    return(to_acct);
  }
  if (from_acct == to_acct) {
    printf("Must be a different account than previous agid, re-enter please.\n");
    goto again;
  }

  sprintf(buff, "-%c%d", op_code[TO_ACCT_IDX], to_acct);

  *(argp + 1) = strdup(buff);
  if (*(argp+1) == NULL) {
    printf("Memory allocation in two_acct_name_or_id() failed\n");
    return(-2);
  }
  return(2);
}

/*===========================================================================*
 * Function:	acctweight
 *
 * Abstract:	This function asks user to specify weight flag.
 *		It's called for adding a new account or for changing existing
 *		accounts' characteristics.
 *
 * Arguments:	argp -	pointer to the next argument to be filled
 *		skip_keep -	1	skip input means no change
 *				0	skip input takes default values
 *
 * Return value:
 *		>=0	number of arguments filled
 *		-1	user want to quit
 *		-2	error
 *
 * Notes:
 * Skip input will default under the creation of a new account, but will
 * mean unchanged when modify accounts info.
 *===========================================================================*/
int acctweight(argp, skip_keep)
  char	**argp;
  int	skip_keep;
{
  int			in_len;
  char			input[MAX_ACCT_WT_LEN];

l_wt:
  if (!skip_keep) {
    printf("Weight Flag (1/0, default %s): ", DEF_WEIGHT);
  } else {
    printf("Weight Flag (1/0, skip for unchanged): ");
  }

  if (get_line(input, MAX_ACCT_WT_LEN) != 0) {
    printf("\nInput too long.  Re-enter, please\n");
    goto l_wt;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    if (skip_keep) {
      return(0);
    } else {
      printf("Weight defaulted to %s\n", DEF_WEIGHT);

      *argp = (char *)malloc(strlen(DEF_WEIGHT)+3);
      if (*argp == NULL) {
        printf("Memory allocation in acctweight() failed\n");
        return(-2);
      }
      sprintf(*argp, "-%c%s", op_code[ACCT_WEIGHT_IDX], DEF_WEIGHT);
      return(1);
    }
  }

  if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
    return(-1);
  }

  if (in_len != 1 || (input[0] != '0' && input[0] != '1')) {
    printf("\nInvalid input.  1, 0, q or Q, please\n");
    goto l_wt;
  }

  *argp = (char *)malloc(in_len+3);
  if (*argp == NULL) {
    printf("Memory allocation in acctweight() failed\n");
    return(-2);
  }
  sprintf(*argp, "-%c%s", op_code[ACCT_WEIGHT_IDX], input);
  return(1);
}

/*===========================================================================*
 * Function:	acctkill
 *
 * Abstract:	This function ask user to specify kill flag value.
 *		It's called for creating new account or for changing account
 *		information.
 *
 * Arguments:	argp -	pointer to the next argument to be filled
 *		skip_keep -	1	skip input means no change
 *				0	skip input takes default values
 *
 * Return value:
 *		>=0	number of arguments filled
 *		-1	user want to quit
 *		-2	error
 *
 * Notes:
 *===========================================================================*/
int acctkill(argp, skip_keep)
  char	**argp;
  int	skip_keep;
{
  int		in_len;
  char		input[MAX_ACCT_KILL_LEN];

l_kill:
  if (!skip_keep) {
    printf("No_kill Flag (1/0, default %s): ", DEF_KILL);
  } else {
    printf("No_kill Flag (1/0, skip for unchanged): ");
  }

  if (get_line(input, MAX_ACCT_KILL_LEN) != 0) {
    printf("\nInput too long.  Re-enter, please\n");
    goto l_kill;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    if (skip_keep) {
      return(0);
    } else {
      printf("No_kill flag defaulted to %s\n", DEF_KILL);

      *argp = (char *)malloc(strlen(DEF_KILL)+3);
      if (*argp == NULL) {
        printf("Memory allocation in acctkill() failed\n");
        return(-2);
      }
      sprintf(*argp, "-%c%s", op_code[ACCT_KILL_IDX], DEF_KILL);
      return(1);
    }
  }

  if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
    return(-1);
  }

  if (in_len != 1 || (input[0] != '0' && input[0] != '1')) {
    printf("\nInvalid input.  1, 0, q or Q, please\n");
    goto l_kill;
  }

  *argp = (char *)malloc(in_len+3);
  if (*argp == NULL) {
    printf("Memory allocation in acctkill() failed\n");
    return(-2);
  }
  sprintf(*argp, "-%c%s", op_code[ACCT_KILL_IDX], input);
  return(1);
}

/*===========================================================================*
 * Function:	acctlock
 *
 * Abstract:	This function asks the user to specify lock flag value.
 *		It's called for creating a new account or for changing
 *		accounts' information
 *
 * Arguments:	argp -	pointer to the next argument to be filled
 *		skip_keep -	1	skip input means no change
 *				0	skip input takes default values
 *
 * Return value:
 *		>=0	number of arguments filled
 *		-1	user want to quit
 *		-2	error
 *
 * Notes:
 *===========================================================================*/
int acctlock(argp, skip_keep)
  char	**argp;
  int	skip_keep;
{
  int		in_len;
  char		input[MAX_ACCT_LOCK_LEN];

l_lock:
  if (!skip_keep)
    printf("Lock Flag (1/0, default %s): ", DEF_LOCK);
  else
    printf("Lock Flag (1/0, skip for unchanged): ");

  if (get_line(input, MAX_ACCT_LOCK_LEN) != 0) {
    printf("\nInput too long.  Re-enter, please\n");
    goto l_lock;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    if (skip_keep) {
      return(0);
    } else {
      printf("Lock flag defaulted to %s\n", DEF_LOCK);

      *argp = (char *)malloc(strlen(DEF_LOCK)+3);
      if (*argp == NULL) {
        printf("Memory allocation in acctlock() failed\n");
        return(-2);
      }
      sprintf(*argp, "-%c%s", op_code[ACCT_LOCK_IDX], DEF_LOCK);
      return(1);
    }
  }

  if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
    return(-1);
  }

  if (in_len != 1 || (input[0] != '0' && input[0] != '1')) {
    printf("\nInvalid input.  1, 0, q or Q, please\n");
    goto l_lock;
  }

  *argp = (char *)malloc(in_len+3);
  if (*argp == NULL) {
    printf("Memory allocation in acctlock() failed\n");
    return(-2);
  }
  sprintf(*argp, "-%c%s", op_code[ACCT_LOCK_IDX], input);
  return(1);
}


/*===========================================================================*
 * Function:	useraccess
 *
 * Abstract:	This function asks the user to specify users' access permission.
 *		It's called for adding users in accounts or for modifying
 *		users' accounts information
 *
 * Arguments:	argp -	pointer to the next argument to be filled
 *		skip_keep -	1	skip input means no change
 *				0	skip input takes default values
 *
 * Return value:
 *		>=0	number of arguments filled
 *		-1	user want to quit
 *		-2	error
 *
 * Notes:
 *===========================================================================*/
int useraccess(argp, skip_keep)
  char	**argp;
  int	skip_keep;
{
  int		i;
  int		in_len;
  char		input[MAX_USER_ACCESS_LEN];

l_acc:
  if (!skip_keep)
    printf("User Permission (M,T,U, default %s): ", DEF_ACCESS);
  else
    printf("User Permission (+/-/,M,T,U, skip for unchanged): ");

  if (get_line(input, MAX_USER_ACCESS_LEN) != 0) {
    printf("\nInput too long.  Re-enter, please\n");
    goto l_acc;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    if (skip_keep) {
      return(0);
    } else {
      printf("Permission defaulted to %s\n", DEF_ACCESS);

      *argp = (char *)malloc(strlen(DEF_ACCESS)+3);
      if (*argp == NULL) {
        printf("Memory allocation in useraccess() failed\n");
        return(-2);
      }
      sprintf(*argp, "-%c%s", op_code[USER_ACCESS_IDX], DEF_ACCESS);
      return(1);
    }
  }

  if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
    return(-1);
  }

  if (input[0] == '+' || input[0] == '-') {
    if (!skip_keep || in_len <= 1) {
      printf("\nInvalid input.  Re-enter, please\n");
      goto l_acc;
    }
    i=1;
  } else i=0;

  for (; i<in_len; i++) {
    if (input[i] == 'u' || input[i] == 'm' || input[i] == 't') {
      input[i] -= 32;    /* capitalize */
      continue;
    }
    if (input[i] == 'U' || input[i] == 'M' || input[i] == 'T')
      continue;
    printf("\nInvalid input.  Re-enter, please\n");
    goto l_acc;
  }

  *argp = (char *)malloc(in_len+3);
  if (*argp == NULL) {
    printf("Memory allocation in useraccess() failed\n");
    return(-2);
  }
  sprintf(*argp, "-%c%s", op_code[USER_ACCESS_IDX], input);
  return(1);
}

/*===========================================================================*
 * Function:	userpercent
 *
 * Abstract:	This function asks the user to specify user allocation in terms
 *		of percentage of total account allocation.
 *		It's called for adding a user to accounts, for modifying users'
 *		accounts information and for transfering allocation between
 *		accounts.
 *
 * Arguments:	argp -	pointer to the next argument to be filled
 *		skip_keep -	1	skip input means no change
 *				0	skip input takes default values
 *
 * Return value:
 *		OVERSIGN+1	one argument contain percentage is returned
 *				and allocminutes() call should be skipped
 *		0	no argument filled
 *		-1	user want to quit
 *		-2	error
 *
 * Notes:
 *===========================================================================*/
int userpercent(argp, skip_keep)
  char	**argp;
  int	skip_keep;
{
  int		i, k;
  int		j;
  int		in_len;
  char		input[MAX_USER_PCNT_LEN];

l_pcnt:
  if (!skip_keep || skip_keep == SKIP_TRANSFER)
    printf("Percentage of Account Allocation (0.0 - 100.0 or skip): ");
  else
    printf("%c of Account Allocation (+/-, 0.0 - 100.0, skip for unchanged): ", '%');

  if (get_line(input, MAX_USER_PCNT_LEN) != 0) {
    printf("\nInput too long.  Re-enter, please\n");
    goto l_pcnt;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    return(0);
  }

  if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
    return(-1);
  }

  if (input[0] == '+' || input[0] == '-') {
    if (!skip_keep || skip_keep == SKIP_TRANSFER || in_len <= 1) {
      printf("\nInvalid input.  Re-enter, please\n");
      goto l_pcnt;
    }
    i = k = 1;
 
  } else {
    i = k = 0;
  }

  for (j = 0; i < in_len; i++) {
    if (input[i] == '.' && j == 0) {
      j++;

    } else if (!isdigit(input[i])) {
      printf("\nInvalid input.  0.0 - 100.0, q or Q, please\n");
      goto l_pcnt;
    }
  }

  if (atof(input+k) > 100.0) {
    printf("\nInput out of range.  0.0 - 100.0, q or Q, please\n");
    goto l_pcnt;
  }

  *argp = (char *)malloc(in_len+3);
  if (*argp == NULL) {
    printf("Memory allocation in userpercent() failed\n");
    return(-2);
  }
  sprintf(*argp, "-%c%s", op_code[USER_PERCENT_IDX], input);
  return(OVERSIGN+1);
}

/*===========================================================================*
 * Function:	allocminutes
 *
 * Abstract:	This function ask user to specify users' allocation in minutes.
 * 		It's called for adding users or accounts, for modifying
 * 		users' or accounts' information and for transfering users'
 * 		allocation within an account
 *
 * Arguments:	argp -	pointer to the next argument to be filled
 *		skip_keep -	1	skip input means no change
 *				0	skip input takes default values
 *
 * Return value:
 *		>=0	number of arguments filled
 *		-1	user want to quit
 *		-2	error
 *
 * Notes:
 *===========================================================================*/
int allocminutes(argp, skip_keep)
  char	**argp;
  int	skip_keep;
{
  int		i;
  int		in_len;
  char		input[MAX_MINUTES_LEN];

l_time:
  if (!skip_keep)
    printf("Allocation node-minutes (default %s, ~ unlimited): ", DEF_MINUTES);
  else if (skip_keep == SKIP_TRANSFER)
    printf("Allocation node-minutes (integer): ");
  else
    printf("Allocation node-minutes (+/-/~,integer, skip for unchanged): ");

  if (get_line(input, MAX_MINUTES_LEN) != 0) {
    printf("\nInput too long.  Re-enter, please\n");
    goto l_time;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    if (skip_keep) {
      if (skip_keep == SKIP_TRANSFER) {
	printf("\nInput required, or enter q or Q to quit\n");
	goto l_time;
      }
      return(0);
    } else {
      printf("Time defaulted to %s minutes\n", DEF_MINUTES);
      *argp = (char *)malloc(MAX_MINUTES_LEN+3);
      if (*argp == NULL) {
        printf("Memory allocation in allocminutes() failed\n");
        return(-2);
      }
      sprintf(*argp, "-%c%s", op_code[ALLOC_MINUTES_IDX], DEF_MINUTES);
      return(1);
    }
  }

  if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
    return(-1);
  }

  if (in_len == 1 && input[0] == '~') {
    *argp = (char *)malloc(in_len+3);
    if (*argp == NULL) {
      printf("Memory allocation in allocminutes() failed\n");
      return(-2);
    }
    sprintf(*argp, "-%c", op_code[UNLIMIT_IDX]);
    return(1);
  }

  if (input[0] == '+' || input[0] == '-') {
    if (!skip_keep || skip_keep == SKIP_TRANSFER || in_len <= 1) {
      printf("\nInvalid input.  Re-enter, please\n");
      goto l_time;
    }
    i = 1;

  } else {
    i = 0;
  }

  for (; i < in_len; i++) {
    if (!isdigit(input[i])) {
      printf("\nInvalid input.  Re-enter, please\n");
      goto l_time;
    }
  }
  *argp = (char *)malloc(in_len+3);
  if (*argp == NULL) {
    printf("Memory allocation in allocminutes() failed\n");
    return(-2);
  }
  sprintf(*argp, "-%c%s", op_code[ALLOC_MINUTES_IDX], input);
  return(1);
}

/*===========================================================================*
 * Function:	maxnodes
 *
 * Abstract:	This function asks user to specify the maximum number of nodes
 *		an account or an user allowed to access.
 *		It's called for creating or modifying accounts or users
 *		information.
 *
 * Arguments:	argp -	pointer to the next argument to be filled
 *		skip_keep -	1	skip input means no change
 *				0	skip input takes default values
 *
 * Return value:
 *		>=0	number of arguments filled
 *		-1	user want to quit
 *		-2	error
 *
 * Notes:
 *===========================================================================*/
int maxnodes(argp, skip_keep)
  char	**argp;
  int	skip_keep;
{
  int		i, total_nodes;
  int		in_len;
  char		input[MAX_NODES_LEN];
  extern void free();

  struct macsconf *conf, *readconf();

  if ((conf = readconf()) == NULL) {
      switch (errno) {
        case ENOENT: (void) printf("Fail in opening %s, assume %d nodes\n",
			MACS_CONF_FILE, DEF_NODES);
                     break;
        case ENOMEM: (void) printf("Fail in malloc() in readconf(), assume %d nodes\n",
			DEF_NODES);
                     break;
        default: (void) printf("Fail in reading %s, assume %d nodes\n",
                        MACS_CONF_FILE, DEF_NODES);
                     break;
      }
    total_nodes = DEF_NODES;
  } else {
    total_nodes = conf->total_nodes;
    (void) free (conf);
    if (total_nodes <= 0) {
      (void) printf("Invalid total_nodes in %s, assume %d nodes\n", 
	MACS_CONF_FILE, DEF_NODES);
      total_nodes = DEF_NODES;
    }
  }

    
l_node:
  if (!skip_keep)
    printf("MAX. Nodes Allowed (0 - %d, default ~ system_limit): ",
           total_nodes);
  else
    printf("MAX. Nodes Allowed (0 - %d, ~, skip for unchanged): ", total_nodes);

  if (get_line(input, MAX_NODES_LEN) != 0) {
    printf("\nInput too long.  Re-enter, please\n");
    goto l_node;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    if (skip_keep) {
      return(0);
    } else {
      printf("Max. nodes defaulted to system limit\n");
      *argp = (char *)malloc(3);
      if (*argp == NULL) {
        printf("Memory allocation in maxnodes() fail\n");
        return(-2);
      }
      sprintf(*argp, "-%c-1", op_code[MAX_NODES_IDX]);
      return(1);
    }
  }

  if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
    return(-1);
  }

  if (in_len == 1 && input[0] == '~') {
    *argp = (char *)malloc(3);
    if (*argp == NULL) {
      printf("Memory allocation in maxnodes() fail\n");
      return(-2);
    }
    sprintf(*argp, "-%c-1", op_code[MAX_NODES_IDX]);
    return(1);
  }


  for (i = 0; i < in_len; i++) {
    if (!isdigit(input[i])) {
      printf("\nInvalid input.  0 - %d, q or Q, please\n", total_nodes);
      goto l_node;
    }
  }
  if (atoi(input) > total_nodes) {
    printf("\nInput out of range.  0 - %d, q or Q, please\n", total_nodes);
    goto l_node;
  }
  *argp = (char *)malloc(in_len+3);
  if (*argp == NULL) {
    printf("Memory allocation in maxnodes() fail\n");
    return(-2);
  }
  sprintf(*argp, "-%c%s", op_code[MAX_NODES_IDX], input);
  return(1);
}

