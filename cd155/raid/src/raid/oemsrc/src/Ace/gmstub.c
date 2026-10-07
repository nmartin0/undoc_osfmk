/*
 * 
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 


#include <stdio.h>
#include <curses.h>
#include <fcntl.h>
#include "acurses.h"
#include "scsidefs.h"

#define	ms2a	"mode2a.dat"
#define	ms2b0	"mode2b0.dat"
#define	ms2b1	"mode2b1.dat"
#define	ms2b2	"mode2b2.dat"
#define	ms2b3	"mode2b3.dat"

int get_page2a	( u_char *Buffer )
{
	int		i;
	u_char		c;
	FILE		*sfd;

	if ((sfd = fopen (ms2a, "a+b")) == NULL) {
		mvprintw ( 23,36,"fopen: %s No such file or directory", ms2a );
		refresh ();
		getch ();
		move ( 23, 36 );
		clrtoeol ();
		refresh ();
		return ( ERR );
	}


	for (i = 0; i < sizeof (physical_array_page_t) && ((c = fgetc (sfd)) != EOF); ++i)
		*Buffer++ = c;
	fclose ( sfd );
	return ( OK );
}

int get_page2b	( int lun, u_char *Buffer )
{
	FILE		*sfd;
	char		*sfp;
	int		i;
	u_char		c;

	switch ( lun ) {
	case 0:
		sfp = ( char *) ms2b0;
		break;

	case 1:
		sfp = ms2b1;
		break;

	case 2:
		sfp = ms2b2;
		break;

	case 3:
		sfp = ms2b3;
		break;

	default:
		return ( ERR );
	}

	if ((sfd = fopen ( sfp, "a+b")) == NULL) {
		mvprintw ( 23,36,"fopen: %s - No such file or directory", sfp );
		refresh ();
		getch ();
		move ( 23, 36 );
		clrtoeol ();
		refresh ();
		return ( ERR );
	}
	for (i = 0; i < sizeof (logical_array_page_t) && ((c = fgetc (sfd)) != EOF); ++i)
		*Buffer++ = c;
	fclose ( sfd );
	return ( OK );
}

int Do_Inquiry ( int lun )
{
	if ( lun < 4 )
		return ( OK );
	else
		return ( ERR );
}

u_int32 get_capacity ( int lun )
{
	switch ( lun ) {
		case 0:
			return ( ( u_int32 ) 0x111111L );
		case 1:
			return ( ( u_int32 ) 0x222222L );
		case 2:
			return ( ( u_int32 ) 0x333333L );
		case 3:
			return ( ( u_int32 ) 0x444444L );
		default:
			break;
	}
	return ( ( u_int32) 0x00 );
}
