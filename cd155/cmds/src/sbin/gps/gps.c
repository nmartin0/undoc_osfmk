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
 
#include <string.h>

#define MAX_PS_ARGS 1024

main( argc, argv )
int  argc;
char **argv;
{
int  i;
char ps[MAX_PS_ARGS];

  strcpy( ps, "ps " );
  for ( i = 1; i < argc; ++i ) {
       strcat( ps, argv[i] );
       strcat( ps, " " );
  }
  if ( join_root_part() < 0 ) {
     perror( "join_root_part()" );
  }
  system( ps );
}
