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
 *
 *              Copyright (c) 1990 San Diego Supercomputer Center.
 *              All rights reserved.  The SDSC software License Agreement
 *              specifies the terms and conditions for redistribution.
 *
 * File:        dump.c
 *
 * Abstract:	print contents of various data structures for debugging
 *
 *****************************************************************************/
#ifdef LINT
static char     sccs_id[] = "@(#)dump.c	2.4 10/15/92";
#endif

#include <stdio.h>

/*===========================================================================*
 * Function:    
 *
 * Abstract:    This function dumps a buffer in HEX format
 *
 * Arguments:
 *
 * Return value:
 *
 * Notes:
 *		THIS FUNCTION IS DANGERIOUS its casting a (char *) to a (int *)
 *===========================================================================*/
void dump_buf(buf, length) 
   char		*buf;
   int		length;
{
   int	i;
   int	j;
   int	*intptr;

   for (i = length / sizeof(int) + 1, intptr = (int *)buf; i; ) {
      fprintf(stderr,"        ");
      for (j = 0; j < 6 && i; i--, intptr++, j++) {
         fprintf(stderr,"%X  ",*intptr);
      }
      fprintf(stderr,"\n");
   }
}
