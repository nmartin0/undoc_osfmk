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
 * File:        syntax.c
 *
 * Abstract:	The file contains syntax-checking routines used by
 *		MACS utilities user interfaces
 *****************************************************************************/
#ifdef LINT
static char     sccs_id[] = "@(#)syntax.c	1.6 10/15/92";
#endif

#include <stdio.h>
#include <ctype.h>

/*===========================================================================*
 * Function:	iscsym
 *
 * Abstract:    This function returns true if the character is valid in a name.
 *
 * Arguments:
 *	c - character
 *
 * Return value:
 *	If the character is valid, it will return 1;
 *	If the character is not valid, it will return 0;
 *
 * Notes:
 *===========================================================================*/
static int iscsym(c)
  char c;
{
  return(isalpha(c) || isdigit(c) || (c == '_'));
}

/*===========================================================================*
 * Function:    acct_desc_syntex_ok
 *
 * Abstract:    This function returns true if the input string contains no
 *		illegal characters.
 *
 * Arguments:
 *	input - input string.
 *
 * Return value:
 *	If the string is valid, it will return 1;
 *	If the string is not valid, it will return 0;
 *
 * Notes:
 *===========================================================================*/
int acct_desc_syntex_ok(input)
  char	*input;
{
  int i;

  if (input == NULL || input[0] == '\0') {
    printf("\nMissing account description.\n");
    return(0);
  }

  for (i = 0; input[i] !='\0'; i++) {
    if (!isprint(input[i])) {
      printf("\nInvalid account description %s.\n", input);
      printf("Only printable characters are allowed.\n");
      return(0); 
    }
  }
  return(1);
}

/*===========================================================================*
 * Function:    acct_id_syntex_ok
 *
 * Abstract:    This function returns true if the input string contains no
 *		illegal characters.
 *
 * Arguments:
 *	input - input string.
 *
 * Return value:
 *	If the string is valid, it will return 1;
 *	If the string is not valid, it will return 0;
 *
 * Notes:
 *===========================================================================*/
int acct_id_syntex_ok(input)
  char *input;
{
  int i;

  if (input == NULL || input[0] == '\0') {
    printf("\nMissing account id.\n");
    return(0);
  }
  for (i = 0; input[i] != '\0'; i++) {
    if (!isdigit(input[i])) {
      printf("\nInvalid account id.  Integer please\n");
      return(0); 
    }
  }
  return(1);
}

/*===========================================================================*
 * Function:	acct_name_syntex_ok
 *
 * Abstract:    This function returns true if the input string contains no
 *		illegal characters.
 *
 * Arguments:
 *	input - input string.
 *
 * Return value:
 *	If the string is valid, it will return 1;
 *	If the string is not valid, it will return 0;
 *
 * Notes:
 *===========================================================================*/
int acct_name_syntex_ok(input)
  char *input;
{
  int i;

  if (input == NULL || input[0] == '\0') {
    printf("\nMissing account name.\n");
    return(0);
  }

  if (!isalpha(input[0])) {
    printf("\nInvalid account name %s.\n", input);
    printf("The leading character must be alphabetical.\n");
    return(0);
  }

  for (i = 1; input[i] != '\0'; i++) {
    if (!iscsym(input[i])) {
      printf("\nInvalid account name %s.\n", input);
      printf("Names consist of alphanumeric characters and _, separated by commas.\n");
      return(0);
    }
  }
  return(1);
}


/*===========================================================================*
 * Function:    user_id_syntex_ok
 *
 * Abstract:    This function returns true if the input string contains no
 *		illegal characters.
 *
 * Arguments:
 *	input - input string.
 *
 * Return value:
 *	If the string is valid, it will return 1;
 *	If the string is not valid, it will return 0;
 *
 * Notes:
 *===========================================================================*/
int user_id_syntex_ok(input)
  char	*input;
{
  int i;

  for (i = 0; input[i] != '\0'; i++) {
    if (!isdigit(input[i])) {
      printf("\nInvalid user id.  Integer please\n");
      return(0);
    }
  }
  return(1);
}

/*===========================================================================*
 * Function:    user_name_syntex_ok
 *
 * Abstract:    This function returns true if the input string contains no
 *		illegal characters.
 *
 * Arguments:
 *	input - input string.
 *
 * Return value:
 *	If the string is valid, it will return 1;
 *	If the string is not valid, it will return 0;
 *
 * Notes:
 *===========================================================================*/
int user_name_syntex_ok(input)
  char *input;
{
  int i;

  if (input == NULL || input[0] == '\0') {
    printf("\nMissing account description.\n");
    return(0);
  }
  if (!isalpha(input[0])) {
    printf("\nInvalid user name.  Alphabetic leading charater, please.\n");
    return(0);
  }

  for (i = 1; input[i] !='\0'; i++) {
    if (!iscsym(input[i])) {
      printf("\nInvalid user name %s.\n", input);
      printf("\nNames consist of alphanumeric characters and _, separated by commas.\n");
      return(0);
    }
  }
  return(1);
}


/*===========================================================================*
 * Function:    percent_syntex_ok
 *
 * Abstract:    This function returns true if the input string contains no
 *		illegal characters.
 *
 * Arguments:
 *	input - input string.
 *
 * Return value:
 *	If the string is valid, it will return 1;
 *	If the string is not valid, it will return 0;
 *
 * Notes:
 *===========================================================================*/
int percent_syntex_ok(input)
  char *input;
{
  int	i;
  int	points;

  points = 0;
  for (i = 0; input[i] !='\0'; i++) {
   if (input[i] == '.' && points == 0) {
      points = 1;

    } else if (!isdigit(input[i])) {
      return(0);
    }
  }
  return(1);
}

