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
 * File:        misc.c
 *
 * Abstract:	These are miscellaneous routines used by macadmin routines
 *****************************************************************************/
#ifdef LINT
static char     sccs_id[] = "@(#)misc.c	2.7 10/15/92";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <time.h>

#include "misc.h"

#define TIMESTRING (128)

#ifdef DEBUG
int isalpha (c)
  char c;
{
	if (c>='A' && c<='Z') return(1);
	if (c>='a' && c<='z') return(1);
	return(0);
}

int isdigit(c)
  char c;
{
	if (c>='0' && c<='9') return(1);
	return(0);
}

int isprint(c)
  char c;
{
	if (c < 32 || c > 126) return(0);
	return(1);
}
#endif

/*===========================================================================*
 * Function:    mkarray
 *
 * Abstract:    This function packs an array with items seperated by commas
 *		in the input character string.
 *
 * Arguments:	s -	a character string contain comma-seperated items
 *
 * Return value:
 *		>NULL	valid character-string array
 *		NULL	error
 *
 * Notes:
 *===========================================================================*/
char **mkarray(s)
  char *s;
{
  int		i;
  int		slen;		/* length of input string */
  int		ncomma = 0;	/* number of commas in input string */

  char		*ptr;
  char		*wptr;
  char		**array;
  extern void free();

  if (s == NULL) {
    return(NULL);
  }

  slen = strlen(s);
  if (slen == 0) {
    return (NULL);
  }

  /* count commas */
  for (ptr = s; *ptr != '\0'; ptr++) {
    if (*ptr == ',') ncomma++;
  }

  array = (char **)malloc((ncomma + 2) * sizeof(char *));
  if (array == NULL) {
    fprintf(stderr, "Memory allocation (malloc()) fail\n");
    return(NULL);
  }

  /*------------------------------------------------------------------------*
   * Break the comma seperated strings into individual strings.
   * i - number of words found
   * ptr - points to end of word
   * wptr - points to start of word
   *------------------------------------------------------------------------*/
  i = 0;
  for (ptr = s, wptr = s, slen = 0; *ptr != '\0'; ptr++, slen++) {
    if (*ptr == ',') {
      /* end of a word */
      if (slen > 0) {
        array[i] = (char *)malloc(slen+1);
        if (array[i] == NULL) {
          frarray(array);
          printf("Memory allocation (malloc()) fail\n");
          return(NULL);
        }
        memcpy(array[i], wptr, slen);
        array[i][slen] = '\0';
        i++;
      }
      slen = -1;	/* set len two -1 so it will be zero next time */
      wptr = ptr + 1;
    }
  }

  /* copy the last string */
  if (slen > 0) {
    array[i] = (char *)malloc(slen+1);
    if (array[i] == NULL) {
        frarray(array);
        printf("Memory allocation (malloc()) fail\n");
        return(NULL);
    }
    memcpy(array[i], wptr, slen);
    array[i][slen] = '\0';
    i++;
  }

  /* if there was an error clean up */
  if (i <= 0) {
    free(array);
    return(NULL);
  }

  array[i] = NULL;
  return(array);
}

/*===========================================================================*
 * Function:    frarray
 *
 * Abstract:    This function frees an array returned by mkarray().
 *
 * Arguments:	array -	the array to be freed.
 *
 * Return value: None
 *
 * Notes:
 *===========================================================================*/
void frarray(array)
  char **array;
{
  int i;
  extern void free();

  for (i = 0; array[i] != NULL; i++) {
    free(array[i]);
  }
  free(array);
}

/*===========================================================================*
 * Function:	timstr
 *
 * Abstract:	This function returns the current time in 26-charater string
 *		format without new-line character. 
 *
 * Arguments:	event_time
 *
 * Return value:
 *		>NULL	pointer the the result string
 *		NULL	error
 *
 * Notes:
 *===========================================================================*/
char *timstr(event_time)
time_t event_time;
{
  time_t	now;
  static char	timestring[TIMESTRING];

  /*
   * Get the formatted time string - strip off trailing '\n'
   */

  if (event_time <= 0) now = time(NULL);
  else now = event_time;
  sprintf(timestring, "(%d) %s", now, ctime(&now));
  timestring[strlen(timestring)-1] = '\0';
  return(timestring);
}
