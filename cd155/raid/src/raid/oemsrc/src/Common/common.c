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
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		common.c
 Title:		Common Functions
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:29:55 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This function provides these entry points:  debug, zalloc,
		debug_display_cdb, debug_display_sense, set_bit, bit_set,
		print_title.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "stddefs.h"
#include "dau_err.h"
#include "dac_raidlv.h"

/******************************************************************************
 ***		             VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
#ifdef DEBUG_ON
u_int debug_on = TRUE;
#else
#ifdef 	PARAGONdebug
u_int debug_on = TRUE;
#else
u_int debug_on = FALSE;
#endif
#endif

#ifdef NO_IO
u_int no_io_on = TRUE;
#else
u_int no_io_on = FALSE;
#endif

/******************************************************************************
 ***			     INTERNAL PROCEDURES			    ***
 *****************************************************************************/

/* format_projrel() [internal] function; used by print_title (below).	     */
/* Find and format the numeric values in PROJREL (defined in stddefs.h) from */
/* a string of the form: 01000002+ into a string of the form: 1.00.00.02+    */

char *
format_projrel( projrel, strbuf )
char *projrel;
char *strbuf;
{
	char *findptr, *bufptr;
	int digitsout = 0;

	bufptr = strbuf;
	if ( (findptr = strstr(projrel, "DAC Utils ")) != NULL ) {
		findptr += strlen("DAC Utils ");
		while ( *findptr ) {
			if ( *findptr >= '0' && *findptr <= '9' ) {
				*bufptr++ = *findptr;
				digitsout++;
				if ( digitsout % 2 == 0 )
					*bufptr++ = '.';
			}
			else if ( *findptr == '+' )
				if ( *(bufptr-1) == '.' ) {
					*--bufptr = '+';
					bufptr++;
					*bufptr++ = '.';
				}
				else {
					*bufptr++ = '+';
					*bufptr++ = '.';
				}
			findptr++;
		}
		if ( *(bufptr-1) == '.' )
			--bufptr;
		*bufptr = '\0';
		if ( *strbuf == '0' )
			strbuf++;
		return( strbuf );
	}
	else
		return( NULL );
}

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
/* debug: print an error message and then return or exit */

void 
debug (char *fmt, ...)
{
	va_list args;

	if ( debug_on )
	{
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		fprintf(stderr, "\n");
		fflush(stderr);
		va_end(args);
	}
	return;
}

void
debug_display_cdb( p )
CDB_t *p;
{
	int s;

	s = debug_on;
	debug_on = s | no_io_on;
	debug("		cdb.Byte = %x %x %x %x %x %x %x %x %x %x\n",
	    p->Byte[0], p->Byte[1], p->Byte[2], p->Byte[3], p->Byte[4],
	    p->Byte[5], p->Byte[6], p->Byte[7], p->Byte[8], p->Byte[9]);
	debug("		cdb.Buffer = %x\n", p->Buffer);
	debug("		cdb.BufferLength = %x\n", p->BufferLength);
	debug_on = s;
}

void
debug_display_sense( p )
SCSI_Sense_Data_t *p;
{
	int i;
	char *c;

	debug("Sense Data:  Valid = %d, Sense Key = %d\n		", 
	    p->Valid, p->Sense_Key);
	c = (char *) p;
	for (i=0; i<25; i++) {
		debug(" %x ",c[i]);
		if ( (!(i%10)) & (i>0) )
			debug("\n		");
	}
	debug("\n");
}

char *
zalloc( amount )
int amount;
{
	char *p;

	if ( ( p = (char *) calloc( 1, amount ) ) != NULL )
		return( p );
	else
		error( NO_MEMORY, OUT_OF_MEMORY_MSG );
}

void
set_bit( word, bit )
Bits_In_Short_t *word;
u_int bit;
{
	switch ( bit ) {
		case 0:  word->b0 = 1; break;
		case 1:  word->b1 = 1; break;
		case 2:  word->b2 = 1; break;
		case 3:  word->b3 = 1; break;
		case 4:  word->b4 = 1; break;
		case 5:  word->b5 = 1; break;
		case 6:  word->b6 = 1; break;
		case 7:  word->b7 = 1; break;
		case 8:  word->b8 = 1; break;
		case 9:  word->b9 = 1; break;
		case 10: word->b10 = 1; break;
		case 11: word->b11 = 1; break;
		case 12: word->b12 = 1; break;
		case 13: word->b13 = 1; break;
		case 14: word->b14 = 1; break;
		case 15: word->b15 = 1; break;
	}
}

int
bit_set( w, bit )
u_short w;
u_int bit;
{
	int bit_set;
	Bits_In_Short_t *word = (Bits_In_Short_t *) &w;

	switch ( bit ) {
		case 0:  bit_set = word->b0; break;
		case 1:  bit_set = word->b1; break;
		case 2:  bit_set = word->b2; break;
		case 3:  bit_set = word->b3; break;
		case 4:  bit_set = word->b4; break;
		case 5:  bit_set = word->b5; break;
		case 6:  bit_set = word->b6; break;
		case 7:  bit_set = word->b7; break;
		case 8:  bit_set = word->b8; break;
		case 9:  bit_set = word->b9; break;
		case 10: bit_set = word->b10; break;
		case 11: bit_set = word->b11; break;
		case 12: bit_set = word->b12; break;
		case 13: bit_set = word->b13; break;
		case 14: bit_set = word->b14; break;
		case 15: bit_set = word->b15; break;
	}
	return( bit_set );
}

#define	space_over( n )	{int i; for (i=0; i<n; i++) printf(" ");}

void
print_title( Interactive, title )
int Interactive;
char *title;
{
	char version[40], verstr[20];

	if ( Interactive ) {
		strcpy( version, "Version " );
		strcat( version, format_projrel( PROJREL, verstr ) );
		printf( "\n" );
		space_over( ( 60 - (int) strlen( title ) ) / 2 );
		printf( "%s\n", title );
		space_over( ( 60 - (int) strlen( version ) ) / 2 );
		printf( "%s\n\n", version );
	}
}

#undef space_over

/*
 *  Functions return values without worrying about possible allignment problems.
 *  Useful when accessing 486 or 680[2-4]0 data with 88k.
 */

unsigned long
cs_swap_4(l)
	register unsigned char *l;
{
	register unsigned long r;

	r = (unsigned long) l[3];
	r |= l[2] << 8;
	r |= l[1] << 16;
	r |= l[0] << 24;
	return(r);
}

unsigned short
cs_swap_2(s)
	register unsigned char *s;
{
	register unsigned short r;

	r = (unsigned short) s[1];
	r |= s[0] << 8;
	return(r);
}

swap_bytes(x)
	unsigned short x;
{
	return(((x<<8)&0xff00)|((x>>8)&0x00ff));
}

int raid_level( hw_addr )
hw_zipcode hw_addr;
{
	DAC_RAIDLV *dp;
	long retval, raid_level;

	dp = new_DAC_RAIDLV( NULL, hw_addr );
	if ( dp->read( dp, (u_long *) &raid_level ) )
		retval = -1;
	else
		retval = raid_level;
	dp->destroy( dp );
	return( retval );
}
