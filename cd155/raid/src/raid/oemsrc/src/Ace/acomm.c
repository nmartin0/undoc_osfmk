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
 ***				 IDENTIFICATION				    ***
 ******************************************************************************
  Name:		$RCSfile: acomm.c,v $
  Version:	$Id: acomm.c,v 1.2.4.1 1995/06/11 22:19:30 kat Exp $
  Title:	ACE Common Routines
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:30 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	

  COPYRIGHT 1992, NCR Corporation

  Description:	This module contains several common routines used by 
		the ace utility.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: acomm.c,v $
 * Revision 1.2.4.1  1995/06/11  22:19:30  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:04  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:40  richardg
 * Initial revision
 *
 * Revision 1.7  1992/07/13  18:55:10  root
 * sdpr 140 Added work_on & work_off functions. - bem
 *
 * Revision 1.6  1992/04/30  19:14:10  root
 *  Added . (period) to maximum lun message. no sdpr - so sue me. - bem
 *
 * Revision 1.5  1992/04/29  12:44:45  root
 * sdpr u-109  Changed printing of title to bottom right of screen - bem
 *
 * Revision 1.4  1992/04/06  14:15:17  root
 * sdpr #76 - get_current_lun_status returning wrong status. fixed - bem
 *
 * Revision 1.3  1992/03/26  13:30:57  root
 * Changed next_nonexistant_lun logic to fill holes if lun's deleted
 * out of order.
 *
 * Revision 1.2  1992/03/19  23:39:47  bmyers
 * Inhibited printing of internal version number to screen.
 *
 * Revision 1.1  1992/03/18  14:00:15  bmyers
 * Initial revision
 *
 */

/******************************************************************************
 ***				    INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "stddefs.h"
#include "dau_err.h"
#include "acurses.h"
#include <curses.h>
/******************************************************************************
  ***			      VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
static char	ibuf[80];
extern int	current_lun;
extern int	current_group;

#ifdef DEBUG_ON
u_int debug_on = TRUE;
#else
u_int debug_on = FALSE;
#endif

#ifdef NO_IO
u_int no_io_on = TRUE;
#else
u_int no_io_on = FALSE;
#endif

/******************************************************************************
 ***			      EXTERNAL PROCEDURES			    ***
 *****************************************************************************/
extern void		popclr ( void );
extern line_ent_t	W7[];
extern grptbl_t		grptbl;
/******************************************************************************
 ***			      EXTERNAL PROCEDURES			    ***
 *****************************************************************************/
extern void	popbox ( int, int, int, int, char * );
/******************************************************************************
 ***			       ENTRY DEFINITIONS			    ***
 *****************************************************************************/
int		get_reply ( void );
void		pop_illegal_value ( void );
void		pop_illegal_group ( void );
void		pop_no_entry_error ( void );
void		pop_max_lun_msg ( void );
void		work_on ( void );
void		work_off ( void );
int		get_hl_lun_nbr ( void );
int		get_next_nonexistant_lun ( void );
char		*get_current_lun_status ( void );
unsigned int	get_two_bytes ( u_char * );
void		put_two_bytes ( u_char *, int );
u_int32 	get_four_bytes ( u_char * );
void		put_four_bytes ( u_char *, u_int32 );
static char	* format_projrel ( char *, char * );

/*==========================================================================*/
int get_hl_lun_nbr ( void )
/*==========================================================================*/
{
	int		i;

	for ( i = 0; i < W7LINES; i++ )
		if ( W7[i].bold == ON )
			break;
	return ( W7[i].lun );
}

/*==========================================================================*/
int get_next_nonexistant_lun ( void )
/*==========================================================================*/
{
	int		i, grp, lun;
	grp_entry_t	*g = grptbl.group;
	int		ltbl[8];

	for ( i = 0; i < MAX_LUNS; i++ )
		ltbl[i] = -1;
	for ( grp = 0; grp <= grptbl.group_count; grp++, g++ )
		for ( lun = 0; lun < g->nbr_luns_in_grp; lun++ )
			if ( g->lun_table[lun].real == TRUE )
				ltbl[g->lun_table[lun].lun_nbr] = 1;
	for ( i = 0; i < MAX_LUNS; i++ ) {
		if ( ltbl[i] == -1 )
			break;
	}
	return ( i );
}

/*==========================================================================*/
char *get_current_lun_status ( void )
/*==========================================================================*/
{
	int		i;

	for ( i = 0; i < W7LINES; i++ )
		if ( W7[i].bold == ON )
			break;

	switch ( W7[i].status  & 0x07) {
		case OPTIMAL:
			sprintf ( ibuf, "Optimal" );
			break;
		case DEGRADED:
			sprintf ( ibuf, "Degraded" );
			break;
		case RECONSTLUN:
			sprintf ( ibuf, "Reconstruct" );
			break;
		case DEAD:
			sprintf ( ibuf, "Dead" );
			break;
		default:
			sprintf ( ibuf, "Unknown" );
	}
	return ( ibuf );
}

/*==========================================================================*/
int get_reply ( void )
/*==========================================================================*/
{
	int reply;

	refresh ();
	echo ();
	reply = getch ();
	refresh ();
	noecho ();
	return ( reply );
}

/*==========================================================================*/
void pop_illegal_value ( void )
/*==========================================================================*/
{
	popbox ( 8, 21, 16, 57, "< ERROR >" );
	mvaddstr ( 11, 25, "An illegal value was entered." );
	mvaddstr ( 13, 25, "Press any key to continue." );
	refresh ();
	getch ();
	popclr ();
}

/*==========================================================================*/
void pop_illegal_nbr_drvs ( void )
/*==========================================================================*/
{
	popbox ( 8, 17, 17, 64, "< ERROR >" );
	mvaddstr ( 11, 21, "An illegal number of drives was entered" );
	mvaddstr ( 12, 21, "for the RAID level you have selected." );
	mvaddstr ( 14, 21, "Press any key to continue." );
	refresh ();
	getch ();
	popclr ();
}

/*==========================================================================*/
void pop_illegal_group ( void )
/*==========================================================================*/
{
	popbox ( 8, 21, 17, 60, "< ERROR >" );
	mvaddstr ( 11, 25, "An illegal group was selected" );
	mvaddstr ( 12, 25, "for the action specified." );
	mvaddstr ( 14, 25, "Press any key to continue." );
	refresh ();
	getch ();
	popclr ();
}

/*==========================================================================*/
void pop_max_lun_msg ( void )
/*==========================================================================*/
{
	popbox ( 8, 15, 17, 65, "< ERROR >" );
	mvaddstr ( 11, 20, "The maximum number of logical units has" );
	mvaddstr ( 12, 20, "been reached.  No more may be created." );
	mvaddstr ( 14, 20, "Press any key to continue." );
	refresh ();
	getch ();
	popclr ();
}

/*==========================================================================*/
void pop_no_entry_error ( void )
/*==========================================================================*/
{
	popbox ( 8, 21, 17, 57, "< ERROR >" );
	mvaddstr ( 11, 25, "No value was entered where" );
	mvaddstr ( 12, 25, "one is required." );
	mvaddstr ( 14, 25, "Press any key to continue." );
	refresh ();
	getch ();
	popclr ();
}

/*==========================================================================*/
unsigned int get_two_bytes ( u_char *data )
/*==========================================================================*/
{
	int		i, rval;

	rval = 0;
	for ( i = 0; i < 2; i++ ) {
		rval <<= 8;
		rval += *data++;
	}
	return ( rval );
}

/*==========================================================================*/
void put_two_bytes ( u_char *data, int val )
/*==========================================================================*/
{
	data[0] = ( u_char ) ( val >> 8 ) & 0xff;
	data[1] = ( u_char ) val & 0xff;
}

/*==========================================================================*/
u_int32 get_four_bytes ( u_char *data )
/*==========================================================================*/
{
	int		i;
	u_int32		rval;

	rval = 0;
	for ( i = 0; i < 4; i++ ) {
		rval <<= 8;
		rval += *data++;
	}
	return ( ( u_int32 ) rval );
}

/*==========================================================================*/
void put_four_bytes ( u_char *data, u_int32 val )
/*==========================================================================*/
{
	data[0] = ( u_char ) ( val >> 24 ) & 0xff;
	data[1] = ( u_char ) ( val >> 16 ) & 0xff;
	data[2] = ( u_char ) ( val >> 8 ) & 0xff;
	data[3] = ( u_char ) val & 0xff;
}

/*==========================================================================*/
void 
debug ( char *fmt, ... )
/*==========================================================================*/
{
	va_list args;

	if ( debug_on )
	{
		va_start( args, fmt );
		vfprintf( stderr, fmt, args );
		fprintf( stderr, "\n" );
		fflush( stderr );
		va_end( args );
	}
	return;
}

/*==========================================================================*/
void debug_display_cdb ( p )
CDB_t *p;
/*==========================================================================*/
{
	int s;

	s = debug_on;
	debug_on = s | no_io_on;
	debug ("		cdb.Byte = %x %x %x %x %x %x %x %x %x %x",
	    p->Byte[0], p->Byte[1], p->Byte[2], p->Byte[3], p->Byte[4],
	    p->Byte[5], p->Byte[6], p->Byte[7], p->Byte[8], p->Byte[9] );
	debug ( "		cdb.Buffer = %x", p->Buffer );
	debug ( "		cdb.BufferLength = %x", p->BufferLength );
	debug_on = s;
}

/*==========================================================================*/
void debug_display_sense ( p )
SCSI_Sense_Data_t *p;
/*==========================================================================*/
{
	int i;
	char *c;

	debug ( "Sense Data:  Valid = %d, Sense Key = %d\n		", 
	    p->Valid, p->Sense_Key );
	c = ( char * ) p;
	for ( i=0; i<25; i++ ) {
		debug ( " %x ",c[i] );
		if ( ( ! ( i%10 ) ) & ( i>0 ) )
			debug ( "\n		" );
	}
	debug ( "\n" );
}

/*==========================================================================*/
char * zalloc ( amount )
int amount;
/*==========================================================================*/
{
	char *p;

	if ( ( p = ( char * ) calloc ( 1, amount ) ) != NULL )
		return ( p );
	else
		ace_error ( NO_MEMORY, OUT_OF_MEMORY_MSG );
}

/*==========================================================================*/
void set_bit ( word, bit )
Bits_In_Short_t *word;
u_int bit;
/*==========================================================================*/
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

/*==========================================================================*/
int bit_set ( w, bit )
u_short w;
u_int bit;
/*==========================================================================*/
{
	int bit_set;
	Bits_In_Short_t *word = ( Bits_In_Short_t * ) &w;

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
	return ( bit_set );
}

/*
 *  Functions return values without worrying about possible allignment problems.
 *  Useful when accessing 486 or 680[2-4]0 data with 88k.
 */

/*==========================================================================*/
unsigned long cs_swap_4 ( l )
register unsigned char *l;
/*==========================================================================*/
{
	register unsigned long r;

	r = ( unsigned long ) l[3];
	r |= l[2] << 8;
	r |= l[1] << 16;
	r |= l[0] << 24;
	return ( r );
}

/*==========================================================================*/
unsigned short cs_swap_2 ( s )
register unsigned char *s;
/*==========================================================================*/
{
	register unsigned short r;

	r = ( unsigned short ) s[1];
	r |= s[0] << 8;
	return ( r );
}

/*==========================================================================*/
swap_bytes ( x )
unsigned short x;
/*==========================================================================*/
{
	return( ( ( x<<8 )&0xff00 )|( ( x>>8 )&0x00ff ) );
}

/*==========================================================================*/
void work_on ( void )
/*==========================================================================*/
{
	attron ( A_REVERSE | A_BLINK );
	mvaddstr ( WRKROW, WRKCOL, " Working " );
	attroff ( A_REVERSE | A_BLINK );
#ifdef ASCII
	move ( 23, 79 );
#else
	move ( 24, 79 );
#endif
	refresh ();
}

/*==========================================================================*/
void work_off ( void )
/*==========================================================================*/
{
	mvaddstr ( WRKROW, WRKCOL, "         " );
#ifdef ASCII
	move ( 23, 79 );
#else
	move ( 24, 79 );
#endif
	refresh ();
}

/*==========================================================================*/
void display_title ( void )
/*==========================================================================*/
{
	char version[40], verstr[20];

	strcpy ( version, "Version " );
	strcat ( version, format_projrel ( PROJREL, verstr ) );
#ifdef ASCII
	mvprintw ( 23, 46, "ace - " ); 		/* sdpr u-109 */
#else
	mvprintw ( 24, 46, "ace - " ); 		/* sdpr u-109 */
#endif
	printw ( "%s", version );
}

/*==========================================================================*/
char * format_projrel ( projrel, strbuf )
char *projrel;
char *strbuf;
/*==========================================================================*/
{
/* format_projrel() [internal] function; used by print_title (below).	     */
/* Find and format the numeric values in PROJREL (defined in stddefs.h) from */
/* a string of the form: 01000002+ into a string of the form: 1.00.00.02+    */

	char *findptr, *bufptr;
	int digitsout = 0;

	bufptr = strbuf;
	if ( ( findptr = strstr ( projrel, "DAC Utils " ) ) != NULL ) {
		findptr += strlen ( "DAC Utils " );
		while ( *findptr ) {
			if ( *findptr >= '0' && *findptr <= '9' ) {
				*bufptr++ = *findptr;
				digitsout++;
				if ( digitsout % 2 == 0 )
					*bufptr++ = '.';
				if ( digitsout % 6 == 0 )
					break;
			}
			else if ( *findptr == '+' )
				if ( * ( bufptr-1 ) == '.' ) {
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
		if ( * ( bufptr-1 ) == '.' )
			--bufptr;
		*bufptr = '\0';
		if ( *strbuf == '0' )
			strbuf++;
		return ( strbuf );
	}
	else
		return ( NULL );
}
