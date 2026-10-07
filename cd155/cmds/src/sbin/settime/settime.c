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
 * settime
 *
 * a hack to propagate a timeval & timezone structure to a node
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/errno.h>

int getopt(int, char **argv, char *optstring);
extern char *optarg;
extern int optind;

main( int argc, char **argv )
{
  int c;
  int debug = 0;
  int dflag = 0;
  int mflag = 0;
  int errflag = 0;

  struct timeval tp;
  struct timezone tzp;

  tzp.tz_dsttime = 0;
  tzp.tz_minuteswest = 0;

  /*
   * Read in arguments in case this program is so broken that it
   * still can't set timezones correctly :-(
   */
  while ((c = getopt(argc, argv, "d:m:v")) != -1)
    switch( c ) {
    case 'd':
      dflag++;
      if (sscanf(optarg, "%d", &tzp.tz_dsttime) != 1)
	{
	  (void)fprintf(stderr, "Couldn't find DST value.\n");
	  errflag++;
	}
      break;

    case 'm':
      mflag++;
      if (sscanf(optarg, "%d", &tzp.tz_minuteswest) != 1)
	{
	  (void)fprintf(stderr, "Couldn't find minutes value.\n");
	  errflag++;
	}
      break;

    case 'v':
      debug++;
      break;

    default:
      errflag++;
    }

  if ( errflag )
    {
      (void)fprintf(stderr, "usage: timezone [-dmv] [nodes]\n");
      (void)fprintf(stderr, "-d # = type of DST (from <sys/time.h>)\n");
      (void)fprintf(stderr, "-m # = minutes west of GMT\n");
      (void)fprintf(stderr, "-v   = verbose flag\n");
      exit ( -1 );
    }

  /*
   * skip setting the values if arguments given in command line
   */
  if (( dflag == 0 ) && ( mflag == 0 ))
    {
      /*
       * tzset() does the actual parsing of TZ for us
       */
      (void)tzset();

      if ( debug )
	{
	  (void)printf("tzname   = %s/%s\n", tzname[0], tzname[1]); 
	  (void)printf("timezone = %d\n", timezone);
	  (void)printf("daylight = %d\n", daylight);
	}
  
      /*
       * tzset() returns the # of seconds, not minutes, from GMT
       */
      tzp.tz_minuteswest = timezone / 60;

      /*
       * Only attempt to set tzp.tz_dsttime if daylight is set
       */
      if ( daylight != 0 )
	{
	  /*
	   * data is from cmds/osf/src/usr/ccs/lib/libc/ctime.c
	   */
	  switch( tzp.tz_minuteswest )
	    {
	    case -1*60:		/* Middle European */
	      tzp.tz_dsttime = DST_MET;
	      break;

	    case -2*60:		/* Eastern European */
	      tzp.tz_dsttime = DST_EET;
	      break;

	    case 4*60:		/* Atlantic */
	      tzp.tz_dsttime = DST_CAN;
	      break;

	    case 5*60:		/* Eastern */
	    case 6*60:		/* Central */
	    case 7*60:		/* Mountain */
	    case 8*60:		/* Pacific */
	      tzp.tz_dsttime = DST_USA;
	      break;

	    case 0*60:		/* Western European */
	      tzp.tz_dsttime = DST_WET;
	      break;

	    case -10*60:	/* Aust: Eastern */
	    case -10*60+30:	/* Aust: Central */
	      tzp.tz_dsttime = DST_AUST;
	      break;

	    case -8*60:		/* Aust: Western */
	    default:
	      tzp.tz_dsttime = DST_NONE;
	    }
	}
    }

  if ( debug )
    {
      (void)printf("tz_minuteswest   = %d\n", tzp.tz_minuteswest);
      (void)printf("tz_dsttime       = %d\n", tzp.tz_dsttime);
    }

  if (gettimeofday( &tp, 0 ) == -1)
    {
      perror("gettimeofday");
      exit( -1 );
    }

  if (settimeofday( &tp, &tzp) == -1)
    {
      perror("settimeofday");
      exit( -1 );
    }

  return( 0 );
}
