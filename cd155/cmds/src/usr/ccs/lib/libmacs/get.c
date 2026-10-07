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
 * File:        get.c
 *
 * Abstract:	This file contains functions that accept and check for
 *		various input items for MACS utilities.
 *****************************************************************************/
#ifdef LINT
static char     sccs_id[] = "%W% %H%";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "mac_def.h"
#include "misc.h"

/*===========================================================================*
 * Function:    get_acct
 *
 * Abstract:    This function solicit user input for an account name or id
 *		from stdin and validates it.  This function
 *		is only called for transfer allocation between accounts.
 *
 * Arguments: from_to -	item index which indicate the input item is an 
 *			from_account or to_account name or id
 *
 * Return value:
 *	>=0	If the input is a valid account name or id
 *	-1	user wants quit
 *	-2	error
 *
 * Notes:
 *===========================================================================*/
int get_acct(from_to)
   int from_to;
{
   int		in_len;
   char		input[MAX_ACCT_NAME_LEN];
   int		acct;

l_acct:		/* retry */

   /* output prompt */
   if (from_to == FROM_ACCT_IDX) {
      printf("From Account Name or ID: ");
   } else if (from_to == TO_ACCT_IDX) {
      printf("To Account Name or ID: ");
   } else {
      printf("Account Name or ID: ");
   }

   /* get input from user */
   if (get_line(input, MAX_ACCT_NAME_LEN) != 0) {
      printf("\nInput too long.  Re-enter, please.\n");
      goto l_acct;
   }

   in_len = strlen(input);
   if (in_len == 0) {
      printf("\nMust input an account name or ID, q or Q, please.\n");
      goto l_acct;
   }

   if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
     return(-1);
   }

   acct = acct_exist_ok(input);
   if (acct < 0) {
      goto l_acct;
   }

   return(acct);
}

/*===========================================================================*
 * Function:    get_accts
 *
 * Abstract:    This function solicit user input for a list of account names
 *		or ids from stdin and validates each
 *
 * Arguments:
 *	skip_keep - 1	skip input takes default value
 *		    0	skip input means no change
 *
 * Return value:
 *		>0	pointer to account array
 *		0	the input is default
 *		-1	user wants to quit
 *		-2	error
 *
 * Notes:
 *	The caller should free the list when it is done using it.
 *===========================================================================*/
char **get_accts(skip_keep)
  int skip_keep;
{
  int		in_len;
  char		input[MAX_ACCT_LIST_LEN];
  char		**accts;
  extern int	countagids();

l_start:			/* retry */
  /* output prompt */
  if (!skip_keep) {
     printf("Account Names or IDs (default is all): ");
  } else {
     printf("Account Names or IDs: ");
  }

  /* get user input */
  if (get_line(input, MAX_ACCT_LIST_LEN) != 0) {
     printf("\nInput too long.  Re-enter, please.\n");
     goto l_start;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    if (skip_keep) {
      printf("\nMust enter account name or ID, q or Q, please\n");
      goto l_start;

    } else {
      if (countagids()) {
        printf("\nDefault to all accounts.\n");
        return(NULL);

      } else {
        printf("\nNo account exists in database.\n");
        return((char **)-1);
      }
    }
  }

  if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
    return((char **)-1);
  }

  if ((accts = mkarray(input)) == NULL) {
     printf("\nInvalid input.  Account name or ID, please.\n");
     goto l_start;
  }

  if (!accts_exist_ok(accts)) {
    frarray(accts);
    goto l_start;
  }
  return(accts);
}

/*===========================================================================*
 * Function:    get_adesc
 *
 * Abstract:    This function solicit user input for an account description 
 *		and checks if it is a valid description.
 *
 * Arguments:
 *	None.
 *
 * Return value:
 *		>0	valid pointer to the description
 *		-1	user wants to quit
 *		-2	error
 *
 * Notes:
 *	The string returned should be freed.
 *===========================================================================*/
char *get_adesc()
{
  int	in_len;
  char	input[MAX_ACCT_DESC_LEN];
  char	*ptr;

l_desc:
  printf("Account Description: ");

  if (get_line(input, MAX_ACCT_DESC_LEN) != 0) {
    printf("\nInput too long.  Re-enter, please.\n");
    goto l_desc;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    printf("\nMust input an account description, q or Q, please.\n");
    goto l_desc;
  }

  if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
    return((char *)-1);
  }

  if (!acct_desc_syntex_ok(input)) {
    goto l_desc;
  }

  ptr = strdup(input);
  if (ptr == NULL) {
    fprintf(stderr, "\nError memory allocation in get_adesc() failed\n");
    return((char *)-2);
  }
  return(ptr);
}

/*===========================================================================*
 * Function:    get_aid
 *
 * Abstract:    This function solicit user input for an account id number 
 *		and checks if it is a valid acount number.
 *
 * Arguments:
 *	None.
 *
 * Return value:
 *		>0	valid pointer to the account id
 *		-1	user wants to quit
 *		-2	error
 *
 * Notes:
 *	The string returned should be freed.
 *===========================================================================*/
char *get_aid()
{
  int	in_len;
  char	input[MAX_ACCT_ID_LEN];
  char	*ptr;

l_aid:
  printf("Account ID: ");

  if (get_line(input, MAX_ACCT_ID_LEN) != 0) {
    printf("\nInput too long.  Re-enter, please\n");
    goto l_aid;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    printf("\nMust input an account ID, q or Q, please.\n");
    goto l_aid;
  }

  if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
    return((char *)-1);
  }

  if (!acct_id_syntex_ok(input)) {
    goto l_aid;
  }

  ptr = strdup(input);
  if (ptr == NULL) {
    fprintf(stderr, "\nError memory allocation in get_aid() failed\n");
    return((char *)-2);
  }
  return(ptr);
}

/*===========================================================================*
 * Function:    get_aname
 *
 * Abstract:    This function gets an account name from stdin and validates it.
 *
 * Arguments:
 *	None.
 *
 * Return value:
 *		>0	valid pointer to the account name
 *		-1	user wants to quit
 *		-2	error 
 *
 * Notes:
 *	The string returned should be freed.
 *===========================================================================*/
char *get_aname()
{
  int	in_len;
  char	input[MAX_ACCT_NAME_LEN];
  char	*ptr;

l_acct:
  printf("Account Name: ");

  if (get_line(input, MAX_ACCT_NAME_LEN) != 0) {
    printf("\nInput too long.  Re-enter, please.\n");
    goto l_acct;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    printf("\nMust input an account name, q or Q, please.\n");
    goto l_acct;
  }

  if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
    return((char *)-1);
  }

  if (!acct_name_syntex_ok(input)) {
    goto l_acct;
  }

  ptr = strdup(input);
  if (ptr == NULL) {
    fprintf(stderr, "\nError memory allocation in get_aname() failed\n");
    return((char *)-2);
  }
  return(ptr);
}

/*===========================================================================*
 * Function:    get_line
 *
 * Abstract:    This function gets a line of input into given buffer
 *		it stores only up to the specified length
 *
 * Arguments:
 *	s - buffer
 *	len - size of buffer
 *
 * Return value:
 *		0 	successful
 *		-1 	error;
 *
 * Notes:
 *===========================================================================*/
int get_line (s, len)
  char	*s;
  int	len;
{
  int i;
  int c;

  i = 0;
  while ((c = getchar()) != EOF) {
    if (c == 10) {
      if (i < len) {
        s[i] = '\0';
        return(0);

      } else {
        s[len-1] = '\0';
        return(-1);
      }
    }
    if (i == 0 && c == ' ') continue;
    if (i < len - 1) s[i] = (char)c;
    i++;
  }
  s[i] = '\0';
  return(0);
}

/*===========================================================================*
 * Function:	get_menu_item
 *
 * Abstract:	This function prompts the user for a menu selection.
 *
 * Arguments:
 *	max - largest menu number
 *
 * Return value:
 *		>=0	menu item number
 *		EOF	if end of file reached or (item == max)
 *
 * Notes:
 *===========================================================================*/
int get_menu_item(max)
  int	max;
{
  int		c;
  int		state;
  int		num;

  printf("Enter menu item number: ");
  state = 0;
  for (;;) {
    c = getchar();
    if (c == EOF) {
      return(c);

    } else {
      switch (state) {
      case 0:					/* find first digit */
        if (isdigit(c)) {
          num = c - '0';
          state = 1;

        } else if ((char)c == '\n') {
          printf("\nInvalid menu item number, re-enter please\n");
          printf("Enter menu item number: ");
          state = 0;

        } else if ((char)c != ' ') {
          printf("\nInvalid menu item number, re-enter please\n");
          printf("Enter menu item number: ");
          state = 2;
        }
        break;

      case 1:					/* NUM, wait for newline */
        if (isdigit(c)) {
          num = num * 10 + c - '0';

        } else if ((char)c == '\n') {
          if (num > max) {
            printf("\nInvalid menu item number, re-enter please\n");
            printf("Enter menu item number: ");

            state = 2;

          } else if (num == max) {
            return(EOF);

          } else {
            return(num);
          }

        } else {
          printf("\nInvalid menu item number, re-enter please\n");
          printf("Enter menu item number: ");

          state = 2;
        }
        break;

      case 2:					/* RESET, wait for newline */
        if ((char)c == '\n') {
          num = 0;
          state = 0;
        }
        break;

      default:
        abort();
        break;
      }
    }
  }
}

/*===========================================================================*
 * Function:	get_users
 *
 * Abstract:    This function gets user input and validates that the users
 *		exists, and they are in the named accounts.
 *
 * Arguments:
 *	accts - list of accounts users must be in.
 *
 * Return value:
 *		>0	pointer to an array of user ids
 *		-1	user wants quit
 *		-2	error
 *
 * Notes:
 *	The caller should free the list when it is done using it.
 *===========================================================================*/
char **get_users(accts, skip_keep)
  char	**accts;
  int	skip_keep;
{
  int	in_len;
  char	input[MAX_USER_LIST_LEN];
  char	**users;

l_users:
  if (!skip_keep) {
    printf("User Names or IDs (Default is all): ");
  } else {
    printf("User Names or IDs: ");
  }

  if (get_line(input, MAX_USER_LIST_LEN) != 0) {
    printf("\nInput too long.  Re-enter, please.\n");
    goto l_users;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    if (!skip_keep) {
       printf("\nDefault to all users.\n");
      return((char **)0);
    }
    printf("\nMust enter user name or ID, q or Q please.\n");
    goto l_users;
  }

  if (in_len == 1 && (input[0] == 'q' || input[0] == 'Q')) {
    return((char **)-1);
  }

  if ((users = mkarray(input)) == NULL) {
     printf("\nInvalid input.  User name or ID, please.\n");
    goto l_users;
  }

  if (!accts_users_exist_ok(users, accts)) {
    frarray(users);
    goto l_users;
  }

  return(users);
}

/*===========================================================================*
 * Function:	get_yes_no
 *
 * Abstract:	This function prompts the users for a yes, no answer.
 *
 * Arguments:
 *	msg - a prompt message
 *
 * Return value:
 *	0 - NO
 *	1 - YES
 *	EOF - if end of file reached
 *
 * Notes:
 *===========================================================================*/
int get_yes_no(msg)
  char	*msg;
{
  int		c;
  int		state;

  printf(msg);					/* print prompt */
  state = 0;
  for (;;) {
    c = getchar();
    if (c == EOF) {
      return(c);

    } else {
      switch (state) {
      case 0:					/* find first letter */
        if ((char)c == 'n' || (char)c == 'N') {
          state = 1;

        } else if ((char)c == 'y' || (char)c == 'Y') {
          state = 2;

        } else if ((char)c == '\n') {
          printf("\nInput must be y or n, please.\n");
          printf(msg);
          state = 0;

        } else if ((char)c != ' ') {
          printf("\nInput must be y or n, please.\n");
          printf(msg);
          state = 3;
        }
        break;

      case 1:					/* NO, wait for newline */
        if ((char)c == '\n') {
          return(0);
        }
        break;

      case 2:					/* YES, wait for newline */
        if ((char)c == '\n') {
          return(1);
        }
        break;

      case 3:					/* RESET, wait for newline */
        if ((char)c == '\n') {
          state = 0;
        }
        break;

      default:
        abort();
        break;
      }
    }
  }
}


/*===========================================================================*
 * Function:	get_new_users
 *
 * Abstract:	This function solicit user input a list of users (names or
 *		ids), and then validates that those users exist in password
 *		file.
 *
 * Arguments:
 *	None.
 *
 * Return value:
 *		>0	pointer to an array of user ids
 *		-1	user wants quit
 *		-2	error
 *
 * Notes:
 *	The caller should free the list when it is done using it.
 *===========================================================================*/
char **get_new_users()
{
  int		in_len;
  char		input[MAX_USER_LIST_LEN];
  char		**users;

l_users:
  printf("User Names or IDs: ");

  if (get_line(input, MAX_USER_LIST_LEN) != 0) {
    printf("\nInput too long.  Re-enter, please.\n");
    goto l_users;
  }

  in_len = strlen(input);
  if (in_len == 0) {
    printf("\nMust enter user name or ID, q or Q please.\n");
    goto l_users;
  }

  if (in_len == 1 && (input[0]=='q' || input[0]=='Q')) {
    return((char **)-1);
  }

  if ((users = mkarray(input)) == NULL) {
    printf("\nInvalid input.  User name or ID, please.\n");
    goto l_users;
  }

  if (!users_exist_ok(users)) {
    frarray(users);
    goto l_users;
  }

  return(users);
}

