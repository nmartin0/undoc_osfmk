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
  Name:		$RCSfile: modify.c,v $
  Version:	$Id: modify.c,v 1.2.4.1 1995/06/11 22:19:59 kat Exp $
  Title:	Modify LUN Parameters
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:59 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272

  COPYRIGHT 1992, NCR Corporation

  Description:	This module allows the user to view and/or modify the page 2b
		information.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: modify.c,v $
 * Revision 1.2.4.1  1995/06/11  22:19:59  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:36  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:52  richardg
 * Initial revision
 *
 * Revision 1.17  1992/07/13  18:53:36  root
 * sdpr u-139 Added libio call to IO_NoErrorLogging in get_capacity function. - bem
 *
 * Revision 1.16  1992/06/16  18:02:36  root
 * u-127 - fixed downsizing capacity bug. - bem
 *
 * Revision 1.15  1992/05/26  15:18:52  root
 * sdpr u-120 - Several changes as a result of user validation testing.
 * Down arrow key allowed on param mod screen.  Values moved closer to text.
 *
 * Revision 1.14  1992/05/15  15:47:25  root
 * sdpr u-119 Used LUN_Block_Size in capacity calculation - bem
 *
 * Revision 1.13  1992/05/07  14:17:05  root
 * sdpr u-117  Set upper limit to 0x8000 ( 32768 dec ) - bem
 *
 * Revision 1.12  1992/05/04  18:59:19  root
 * sdpr 115 Still more input checking to get rid of garbage if illegal chars
 * entered.  Also redisplayed current value if error on input.
 *
 * Revision 1.11  1992/04/24  17:40:23  root
 * sdpr u-097 fixed bug introduced by rev 1.10 - bem
 *
 * Revision 1.10  1992/04/21  22:08:40  root
 * sdpr u-097 Corrected validation on Logical Unit size entry.
 *
 * Revision 1.9  1992/04/21  21:24:40  root
 * sdpr u-096 Displaying data at wrong column through curses.
 *
 * Revision 1.8  1992/04/10  20:35:17  root
 * sdpr # u-079 Changed logic to put the lun number being created in page2b
 * and issuing the mode-select to lun 0.
 *
 * Revision 1.7  1992/04/06  14:17:59  root
 * sdpr #75 - Added illegal group check to modify_lun function - bem
 *
 * Revision 1.6  1992/03/23  23:10:40  bmyers
 * Added Exit_Status setting for sysadm
 *
 * Revision 1.5  1992/03/21  20:02:11  bmyers
 * Fixed display problem with 10 drive drive list.
 *
 * Revision 1.4  1992/03/20  22:09:25  bmyers
 * Fixed bug when 'q' entered at data OK question.
 *
 * Revision 1.3  1992/03/20  17:32:07  bmyers
 * Changed locking scheme for exclusive access.
 *
 * Revision 1.2  1992/03/19  23:37:01  bmyers
 * Removed code to display and modify flags.
 * Added logic to issue format command only if certain fields were changed.
 *
 * Revision 1.1  1992/03/18  14:00:26  bmyers
 * Initial revision
 *
 */

/******************************************************************************
 ***				    INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "acurses.h"
#include "scsidefs.h"
#include "dau_err.h"
#include "stddefs.h"
#include <curses.h>
/******************************************************************************
  ***			     	 DEFINES				    ***
 *****************************************************************************/
#define A_CAPACITY_ALL		"all"
#define A_NONDESTRUCTIVE	1
#define A_DESTRUCTIVE		2
/******************************************************************************
  ***			      VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
extern int		Exit_Status;
extern int		drvlst[10];
extern int		current_lun;
extern int		current_group;
extern grptbl_t 	grptbl;
static unsigned int	ival;
static u_int32		original_capacity;
static u_int32		lval;
static char		buf[80];
static int		modify_flag;
static logical_array_page_t current2b;
/******************************************************************************
 ***			      EXTERNAL PROCEDURES			    ***
 *****************************************************************************/
extern void 		popbox ( int, int, int, int, char * );
extern void 		popclr ( void );
extern int		get_reply ( void );
extern void		pop_illegal_value ( void );
extern void		pop_illegal_group ( void );
extern void		pop_no_entry_error ( void );
extern char		*get_current_lun_status ( void );
extern int		get_page2b ( int);
extern int		get_hl_lun_nbr ( void );
extern int		get_next_nonexistant_lun ( void );
extern int		format_lun ( int, int );
extern unsigned int	get_two_bytes ( u_char * );
extern u_int32		get_four_bytes ( u_char * );
extern void		put_two_bytes ( u_char *, int );
extern void		put_four_bytes ( u_char *, u_int32 );
extern logical_array_page_t page2b;

/******************************************************************************
 ***			      INTERNAL PROCEDURES			    ***
 *****************************************************************************/
static void		display_drvlst ( void );
static void		display_modify_form ( int, int, int );
static u_int32		*edit_ulong_value ( int );
static int		accept_form_input ( int );
static void		edit_form ( void );
static void		get_lun_blk_size ( void );
static void		get_lun_nbr_blks ( void );
static void		get_segment_size ( void );
static void		get_segment_zero_size ( void );
static void		get_recon_freq ( void );
static void		get_recon_amt ( void );
static void		get_current_values ( void );
static void		check_for_change ( void );
static u_int32		get_capacity ( int );
/******************************************************************************
 ***			       ENTRY DEFINITIONS			    ***
 *****************************************************************************/
int 		modify_lun ( int );

/*==========================================================================*/
int modify_lun ( int flag )
/*==========================================================================*/
{
	int			rval, ans, lock, lun_save;
	logical_array_page_t	*p = &page2b;
	int			create = FALSE;
	int			lun = get_hl_lun_nbr ();
	int			first_time = TRUE;

	modify_flag = NONE;

	if ( lun == -1 || current_group == 0 ) {
		if ( lun == -1 && flag == FROM_MAINMENU ) {
			pop_illegal_group ();
			return ( NONE );
		}
		create = TRUE;
		lun = get_next_nonexistant_lun ();
	} else {
		if ( ( get_page2b ( lun ) ) == ERR ) {
			ace_error ( SCSI_CONDITION, MODE_SENSE_ERROR );
			return ( ALL );
		}
	}

#ifdef ASCII
	popbox ( 0, 0, 23, 78, "< MODIFY/DISPLAY PARAMETERS >" );
#else
	popbox ( 0, 0, 24, 78, "< MODIFY/DISPLAY PARAMETERS >" );
#endif

	while ( TRUE ) {
		display_modify_form ( lun, create, first_time );
		first_time = FALSE;
		move ( 21, 37 );
		ans = get_reply ();
		if ( ans == 'y' || ans == 'Y' ) {
			rval = OK;
			break;
		}
		if ( ans == 'q' || ans == 'Q' ) {
			rval = NONE;
			break;
		}
		if ( ans != 'n' && ans != 'N' )
			continue;

		edit_form ();
	}
	popclr ();
	if ( ( rval == OK && modify_flag != NONE ) ||
			( rval == OK && flag == FROM_CREATE ) ) {
		if ( flag == FROM_MAINMENU ) {
			if ( modify_flag == A_NONDESTRUCTIVE ) {
				p->Action_Status = MODIFY_LUN;
				lock = FALSE;
			} else {
				if ( pop_data_loss ( "Modification" ) == QUIT )
					return ( NONE );
				p->Action_Status = REPLACE_LUN;
				lock = TRUE;
				flag = FROM_CREATE;
			}
		} else {
			p->Action_Status = ADD_LUN;
			p->LUN_Type = 01;
			lock = FALSE;
			p->Lun_Number = lun;		/* LUN must be in     */
			lun_save = lun;			/* page 2b for NOVELL */
 			lun = 0;			/* Issue mode sense   */
			modify_flag = A_DESTRUCTIVE;	/* to LUN 0           */
		}
	
		rval = mode_select ( lun, p, sizeof ( logical_array_page_t ),
				PAGE_FORMAT_SCSI_2, SAVE_PARAMETERS, lock );

		if ( create == TRUE )			/* retrieve lun num   */
			lun = lun_save;			/* we just created    */

		if ( rval == LOCK_ERROR )
			return ( ALL );

		if ( rval ) {
			ace_error ( SCSI_CONDITION, MODE_SELECT_ERROR );
			return ( rval );
		}

		if ( p->Action_Status == ADD_LUN )
			Exit_Status = 1;

		if ( modify_flag == A_DESTRUCTIVE )
			rval = format_lun ( lun, flag );
	}
	return ( rval );
}

/*==========================================================================*/
static void edit_form ( void )
/*==========================================================================*/
{
	get_current_values ();

	if ( page2b.RAID_Level == 3 )		/* RAID level 5 block        */
		get_lun_blk_size ();		/* must be 512               */

	get_lun_nbr_blks ();

	if ( page2b.RAID_Level != 3 ) {		/* RAID level 3 segment      */
		get_segment_size ();		/* size and segment zero     */
		get_segment_zero_size ();	/* size are not changeable   */
	}
	get_recon_freq ();
	get_recon_amt ();

	check_for_change ();
}

/*==========================================================================*/
static void get_current_values ( void )
/*==========================================================================*/
{
	/* make a copy of page 2b so we can check it
	   later to see if it has been changed.        */

	memcpy ( &current2b, &page2b, sizeof ( logical_array_page_t ) );
}


/*==========================================================================*/
static void check_for_change ( void )
/*==========================================================================*/
{
	logical_array_page_t	*p = &page2b;
	logical_array_page_t	*c = &current2b;
	int			cur, new;
	u_int32			lcur, lnew;

	cur = get_two_bytes ( c->Reconstruction_Amount );
	new = get_two_bytes ( p->Reconstruction_Amount );
	if ( cur != new )
		modify_flag = A_NONDESTRUCTIVE;
	
	if ( c->Reconstruction_Frequency != p->Reconstruction_Frequency )
		modify_flag = A_NONDESTRUCTIVE;

	lcur = get_four_bytes ( c->LUN_Block_Size );
	lnew = get_four_bytes ( p->LUN_Block_Size );
	if ( lcur != lnew )
		modify_flag = A_DESTRUCTIVE;

	lcur = get_four_bytes ( c->LUN_Number_of_Blocks );
	lnew = get_four_bytes ( p->LUN_Number_of_Blocks );
	if ( lcur != lnew )
		modify_flag = A_DESTRUCTIVE;

	lcur = get_four_bytes ( c->LUN_Segment_Size );
	lnew = get_four_bytes ( p->LUN_Segment_Size );
	if ( lcur != lnew )
		modify_flag = A_DESTRUCTIVE;

	if ( c->Segment_Zero_Size != p->Segment_Zero_Size )
		modify_flag = A_DESTRUCTIVE;
}

/*==========================================================================*/
static u_int32 *edit_ulong_value ( int row )
/*==========================================================================*/
{
	int		rcnt;

	rcnt = accept_form_input ( row );
	lval = 0;
	if ( rcnt == 0 )
		return ( NULL );
	sscanf ( buf, "%ld", ( u_int32 *) &lval );
	return ( ( u_int32 * ) &lval );
}

/*==========================================================================*/
static int accept_form_input ( int row )
/*==========================================================================*/
{
	int		c, i;

	for ( i = 0; i < 80; i++ )
	    buf[i] = 0;
	move ( row, MCOL - 8 );
	refresh ();
	i = 0;
	noecho ();
	keypad ( stdscr, TRUE );
	flushinp ();
	while ( TRUE ) {
		c = getch ();
		if ( c == '\n' || c == KEY_DOWN )
			break;
		if ( i >= 8 && c != '\b' ) {
			beep ();
			continue;
		}
		if ( c == '\b' ) {
			if ( i > 0 )
				i--;
			else
				beep ();
			buf[i] = 0;
		} else {
			if ( c < 0x30  || c > 0x7f ) {
				beep ();
				continue;
			}
			buf[i++] = ( char ) c;
		}
		mvprintw ( row, MPCOL, "%8s", buf );
		move ( row, MCOL - 8 );
		refresh ();
	}
	echo ();
	refresh ();
	return ( i );
}

/*==========================================================================*/
static void display_drvlst ( void )
/*==========================================================================*/
{
	int		i, ch, id;
	grp_entry_t	*g = &grptbl.group[current_group];

	if ( current_group == 0 ) {
		for ( i = 0; drvlst[i] != 0xff; i++ ) {
			ch = drvlst[i] >> 4;
			id = drvlst[i] & 0x07;
			printw ( " (%x,%x)" , ch, id );
		}
	} else {
		for ( i = 0; i < g->nbr_drvs_in_grp; i++ ) {
			ch = ( g->drive_list[i].drive ) >> 4;
			id = g->drive_list[i].drive & 0x07;
			printw ( "(%x,%x) " , ch, id );
		}
	}
}

/*==========================================================================*/
void get_lun_blk_size ( void )
/*==========================================================================*/
{
	u_int32		*lvp;

	attron ( A_REVERSE );
	mvprintw ( MR10, MPCOL, "%8ld", ( u_int32 ) get_four_bytes
						   ( page2b.LUN_Block_Size ) );
	refresh ();
	for ( ; ; ) {
		if ( ( lvp = edit_ulong_value ( MR10 ) ) != NULL ) {
			if ( page2b.RAID_Level == 3 ) { 
			       if ( *lvp != 1024 && *lvp != 2048 ) {
					pop_illegal_value ();
					mvprintw ( MR10, MPCOL, "%8ld",
						( u_int32 ) get_four_bytes
						   ( page2b.LUN_Block_Size ) );
					refresh ();
					continue;
				}
			}
			if ( page2b.RAID_Level == 5 ) {
				if ( *lvp != 512 ) {
					pop_illegal_value ();
					mvprintw ( MR10, MPCOL, "%8ld",
						( u_int32 ) get_four_bytes
						   ( page2b.LUN_Block_Size ) );
					refresh ();
					continue;
				}
			}
			put_four_bytes ( page2b.LUN_Block_Size, *lvp );
		}
		break;
	}
	attroff ( A_REVERSE );
	mvprintw ( MR10, MPCOL, "%8ld", ( u_int32 ) get_four_bytes
						   ( page2b.LUN_Block_Size ) );
}

/*==========================================================================*/
static void get_lun_nbr_blks ( void )
/*==========================================================================*/
{
	u_int32		*lvp, capacity, lun_blk_sz;

	lun_blk_sz = get_four_bytes ( page2b.LUN_Block_Size );

#ifdef PARAGON860 /* resolve overflow errors */
	capacity = get_four_bytes ( page2b.LUN_Number_of_Blocks );
	capacity = (double) capacity / (1048576 / (double) lun_blk_sz);
#else
	capacity = get_four_bytes ( page2b.LUN_Number_of_Blocks );
	capacity *= ( u_int32 ) lun_blk_sz;
	capacity /= ( u_int32 ) 1048576;
#endif
	attron ( A_REVERSE );
	if ( capacity == 0 )
		mvprintw ( MR11, MPCOL, "%8s", A_CAPACITY_ALL );
	else
		mvprintw ( MR11, MPCOL, "%8ld", ( u_int32 ) capacity );
	refresh ();
	for ( ; ; ) {
		if ( ( lvp = edit_ulong_value ( MR11 ) ) != NULL ) {
			if ( buf[0] > 0x39 ) {
				if ( ( strcmp ( buf, "all" ) ) != 0 ) {
					pop_illegal_value ();
#ifdef PARAGON860 /* resolve overflow errors */
	capacity = get_four_bytes ( page2b.LUN_Number_of_Blocks );
	capacity = (double) capacity / (1048576 / (double) lun_blk_sz);
#else
					capacity = get_four_bytes
						( page2b.LUN_Number_of_Blocks );
					capacity *= ( u_int32 ) lun_blk_sz;
					capacity /= ( u_int32 ) 1048576;
#endif
					if ( capacity == 0 )
						mvprintw ( MR11, MPCOL, "%8s",
							A_CAPACITY_ALL );
					else
						mvprintw ( MR11, MPCOL, "%8ld",
							( u_int32 ) capacity );
					refresh ();
					continue;
				}
			}

#ifdef PARAGON860 /* resolve overflow errors */
			lval =  ((double)lval * (double)1048576 ) / lun_blk_sz;
#else
			lval = ( lval * 1048576 ) / lun_blk_sz;
#endif
			if ( lval > original_capacity ) {
				pop_illegal_value ();
#ifdef PARAGON860 /* resolve overflow errors */
	capacity = get_four_bytes ( page2b.LUN_Number_of_Blocks );
	capacity = (double) capacity / (1048576 / (double) lun_blk_sz);
#else
				capacity = get_four_bytes
					( page2b.LUN_Number_of_Blocks );
				capacity *= ( u_int32 ) lun_blk_sz;
				capacity /= ( u_int32 ) 1048576;
#endif
				if ( capacity == 0 )
					mvprintw ( MR11, MPCOL, "%8s",
						A_CAPACITY_ALL );
				else
					mvprintw ( MR11, MPCOL, "%8ld",
						( u_int32 ) capacity );
				refresh ();
				continue;
			}
			put_four_bytes ( page2b.LUN_Number_of_Blocks, *lvp );
		}
	break;
	}
	attroff ( A_REVERSE );
#ifdef PARAGON860 /* resolve overflow errors */
	capacity = get_four_bytes ( page2b.LUN_Number_of_Blocks );
	capacity = (double) capacity / (1048576 / (double) lun_blk_sz);
#else
	capacity = get_four_bytes ( page2b.LUN_Number_of_Blocks );
	capacity *= ( u_int32 ) lun_blk_sz;
	capacity /= ( u_int32 ) 1048576;
#endif
	if ( capacity == 0 )
		mvprintw ( MR11, MPCOL, "%8s", A_CAPACITY_ALL );
	else
		mvprintw ( MR11, MPCOL, "%8ld", ( u_int32 ) capacity );
}

/*==========================================================================*/
static void get_segment_size ( void ) 
/*==========================================================================*/
{
	u_int32		*lvp;

	attron ( A_REVERSE );
	mvprintw ( MR12, MPCOL, "%8ld", ( u_int32 )
		   get_four_bytes ( page2b.LUN_Segment_Size ) );
	refresh ();
	for ( ; ; ) {
		if ( ( lvp = edit_ulong_value ( MR12 ) ) != NULL ) {
			if ( *lvp < 3 || *lvp > 0xffff ) {
				pop_illegal_value ();
				mvprintw ( MR12, MPCOL, "%8ld", ( u_int32 )
				   get_four_bytes ( page2b.LUN_Segment_Size ) );
				refresh ();
				continue;
			}
			put_four_bytes ( page2b.LUN_Segment_Size, *lvp );
		}
		break;
	}
	attroff ( A_REVERSE );
	mvprintw ( MR12, MPCOL, "%8ld", ( u_int32 )
		   get_four_bytes ( page2b.LUN_Segment_Size ) );
}

/*==========================================================================*/
static void get_segment_zero_size ()
/*==========================================================================*/
{
	u_int32		*lvp;

	attron ( A_REVERSE );
	mvprintw ( MR13, MPCOL, "%8d", ( u_char ) page2b.Segment_Zero_Size );
	refresh ();
	for ( ; ; ) {
		if ( ( lvp = edit_ulong_value ( MR13 ) ) != NULL ) {
			if ( *lvp > 0xff || buf[0] > 0x39 ) {
				pop_illegal_value ();
				mvprintw ( MR13, MPCOL, "%8d", ( u_char )
					page2b.Segment_Zero_Size );
				refresh ();
				continue;
			}
			page2b.Segment_Zero_Size = ( u_char ) lval & 0xff;
		}
		break;
	}
	attroff ( A_REVERSE );
	mvprintw ( MR13, MPCOL, "%8d", ( u_char ) page2b.Segment_Zero_Size );
}

/*==========================================================================*/
static void get_recon_freq ()
/*==========================================================================*/
{
	u_int32		*lvp;

	attron ( A_REVERSE );
	mvprintw ( MR14, MPCOL, "%8d", page2b.Reconstruction_Frequency );
	refresh ();
	for ( ; ; ) {
		if ( ( lvp = edit_ulong_value ( MR14 ) ) != NULL ) {
#ifdef PARAGON860	/* allow a value of 0 */
			if ( *lvp < 0 || *lvp > 255 ) {
#else
			if ( *lvp < 1 || *lvp > 255 ) {
#endif
				pop_illegal_value ();
				mvprintw ( MR14, MPCOL, "%8d",
					page2b.Reconstruction_Frequency );
				refresh ();
				continue;
			}
			page2b.Reconstruction_Frequency = ( u_char ) lval & 0xff;
		}
		break;
	}
	attroff ( A_REVERSE );
	mvprintw ( MR14, MPCOL, "%8d", page2b.Reconstruction_Frequency );
}

/*==========================================================================*/
static void get_recon_amt ()
/*==========================================================================*/
{
	u_int32		*lvp;
	unsigned int	val;

	val = ( unsigned int ) get_two_bytes ( page2b.Reconstruction_Amount );
	attron ( A_REVERSE );
	mvprintw ( MR15, 42, "%8u", ( unsigned int ) val );
	refresh ();
	for ( ; ; ) {
		if ( ( lvp = edit_ulong_value ( MR15 ) ) != NULL ) {
			if ( *lvp < 1 || *lvp > 0x8000 ) {
				pop_illegal_value ();
				val = ( unsigned int ) get_two_bytes
					( page2b.Reconstruction_Amount );
				mvprintw ( MR15, 42, "%8u",
					( unsigned int ) val );
				refresh ();
				continue;
			}
			ival = ( unsigned int ) lval;
			put_two_bytes ( page2b.Reconstruction_Amount, ival );
		}
		break;
	}
	attroff ( A_REVERSE );
	val = ( unsigned int ) get_two_bytes ( page2b.Reconstruction_Amount );
	mvprintw ( MR15, 42, "%8u", ( unsigned int ) val );
}

/*==========================================================================*/
static void display_modify_form ( int lun, int create, int first_time )
/*==========================================================================*/
{
	u_int32		capacity, lun_blk_sz;
	unsigned int	val;

	mvprintw ( MR2, MSCOL, "LUN %x Status = ", lun );

	if ( create == TRUE )
		printw ( "creating" );
	else 
		printw ( "%s", ( char * ) get_current_lun_status () );
	mvprintw ( MR3, MSCOL, "LUN %x Drives =", lun );
	display_drvlst ();
	mvaddstr ( MR7, MSCOL, "PARAMETER" );
	mvaddstr ( MR7, 45, "VALUE" );
	mvaddstr ( MR9, MSCOL, "RAID Level" );
	mvprintw ( MR9, MCOL - 8, "%x", page2b.RAID_Level );
	mvaddstr ( MR10, MSCOL, "Logical Block Size (bytes)" );
	lun_blk_sz = get_four_bytes ( page2b.LUN_Block_Size );
	mvprintw ( MR10, MPCOL, "%8ld", ( u_int32 ) lun_blk_sz );
	mvaddstr ( MR11, MSCOL, "Logical Unit Size (mb)" );
	capacity = get_four_bytes ( page2b.LUN_Number_of_Blocks );
	if ( first_time == TRUE )
		original_capacity = capacity;
	if ( first_time == TRUE  && create == TRUE ) {
		capacity = get_capacity ( lun );
		original_capacity = capacity;
	}

#ifdef PARAGON860 /* resolve overflow errors */
	capacity = (double) capacity / (double)(1048576 / lun_blk_sz);
#else
	capacity *= ( u_int32 ) lun_blk_sz;
	capacity /= ( u_int32 ) 1048576;
#endif
	if ( capacity == 0 )
		mvprintw ( MR11, MPCOL, "%8s", A_CAPACITY_ALL );
	else
		mvprintw ( MR11, MPCOL, "%8ld", ( u_int32 ) capacity );
	mvaddstr ( MR12, MSCOL, "Segment Size (blocks)" );
	mvprintw ( MR12, MPCOL, "%8ld",
		( u_int32 ) get_four_bytes ( page2b.LUN_Segment_Size ) );
	mvaddstr ( MR13, MSCOL, "Segment Zero Size (blocks)" );
	mvprintw ( MR13, MPCOL, "%8d", ( u_char ) page2b.Segment_Zero_Size );
	mvaddstr ( MR14, MSCOL, "Delay Interval (tenth's of a second)" );
	mvprintw ( MR14, MPCOL + 5,"%3d", page2b.Reconstruction_Frequency );
	mvaddstr ( MR15, MSCOL , "Blocks Per Delay Interval" );
	val = ( unsigned int ) get_two_bytes ( page2b.Reconstruction_Amount );
	mvprintw ( MR15, 42, "%8u", ( unsigned int ) val );
	mvaddstr ( MR21, MSCOL, "Are the values shown OK? (y/n/q)   " );
}

/*=======================================================================*/
static u_int32 get_capacity ( int lun )
/*=======================================================================*/
{
	int		stat;
	logical_array_page_t	*p = &page2b;
	u_int32		rval = 0;

#ifdef UV4
	IO_NoErrorLogging ( TRUE );
#endif

/* make a sub-lun asking for all unused space */

	p->Action_Status = ADD_LUN;
	p->LUN_Type = SUB_LUN;
	p->Lun_Number = lun;		/* must be in page 2b for NOVELL */
	put_four_bytes ( p->LUN_Number_of_Blocks, ( u_int32 ) 0 );

	stat = mode_select ( 0, p, sizeof ( logical_array_page_t ),
		PAGE_FORMAT_SCSI_2, SAVE_PARAMETERS, FALSE );

	if ( stat == OK ) {

	/* now get page2b for this 'new'lun */

		stat = mode_sense ( lun, p, sizeof ( logical_array_page_t ), 
			PAGE_CONTROL_CURRENT_VALUES, LOGICAL_ARRAY_PG_CODE );

		if ( stat == OK )
			rval = get_four_bytes ( p->LUN_Number_of_Blocks );

		/* Now, make the LUN disappear  by deleting it */

		p->Action_Status = DELETE_LUN;

		stat = mode_select ( lun, p, sizeof ( logical_array_page_t ),
			PAGE_FORMAT_SCSI_2, SAVE_PARAMETERS, FALSE );

	}
#ifdef UV4
	IO_NoErrorLogging ( FALSE );
#endif
	return ( rval );
}

