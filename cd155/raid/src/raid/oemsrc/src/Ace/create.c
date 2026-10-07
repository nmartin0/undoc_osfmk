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
  Name:		$RCSfile: create.c,v $
  Version:	$Id: create.c,v 1.2.4.1 1995/06/11 22:19:41 kat Exp $
  Title:	Create A LUN
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:41 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272

  COPYRIGHT 1992, NCR Corporation

  Description:	This module solicits the information needed to create a LUN.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: create.c,v $
 * Revision 1.2.4.1  1995/06/11  22:19:41  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:18  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:45  richardg
 * Initial revision
 *
 * Revision 1.9  1992/06/23  19:04:31  root
 * sdpr u-131 - Allowed 3,4 or 5 drive raid 5. - bem
 *
 * Revision 1.8  1992/05/26  15:14:10  root
 * sdpr u-120 - Several changes as a result of user validation testing.
 *
 * Revision 1.7  1992/05/04  18:56:11  root
 * sdpr u-112 Made default block size 1024 for 2+1 RAID 3 and 2048 for 4+1 RAID 3.
 *
 * Revision 1.6  1992/04/29  13:12:49  root
 * sdpr u-110 set default reconstruction amount to 256 blocks - bem
 *
 * Revision 1.5  1992/04/24  17:41:15  root
 * sdpr u-107 - Changed default segment size to 512 - bem
 *
 * Revision 1.4  1992/03/25  17:22:47  root
 * Set default segment zero size to zero.
 *
 * Revision 1.3  1992/03/21  20:00:59  bmyers
 * Added stricter error checking on drive entries for RAID 0,1,3, and 5.
 *
 * Revision 1.2  1992/03/19  23:42:13  bmyers
 * Made error checking on drive entrys more robust. (ie RAID level sensitive )
 *
 * Revision 1.1  1992/03/18  14:00:20  bmyers
 * Initial revision
 *
 */

/******************************************************************************
 ***				    INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "acurses.h"
#include "scsidefs.h"
#include "stddefs.h"
#include "dau_err.h"
#include <curses.h>
/******************************************************************************
  ***			      VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
int			drvlst[11];
extern int		current_lun;
extern int		current_group;
extern grptbl_t 	grptbl;
extern line_ent_t	W7[];
/******************************************************************************
 ***			      EXTERNAL PROCEDURES			    ***
 *****************************************************************************/
extern void	 	popbox ( int, int, int, int, char * );
extern void	 	popclr ( void );
extern int		get_reply ( void );
extern void		pop_illegal_value ( void );
extern void		pop_illegal_group ( void );
extern void		pop_illegal_nbr_drvs ( void );
extern void		pop_no_entry_error ( void );
extern void		pop_max_lun_msg ( void );
extern int		get_next_nonexistant_lun ( void );
extern int		modify_lun ( int );
extern void		put_two_bytes ( u_char *, int );
extern void		put_four_bytes ( u_char *, u_int32 );
extern int		grp_containing_drv ( int );
extern logical_array_page_t page2b;
/******************************************************************************
 ***			      INTERNAL PROCEDURES			    ***
 *****************************************************************************/
static int		pop_create_form ( int );
static int		pop_predefine_form ( void );
static int		check_duplicate ( int, int );
static int		check_valid_nbr_drvs ( int, int );
static int		check_legal_chan ( int, int, int );
static void		fill_page2b_info ( int, int );
static void		display_chid_form ( int );
static int		check_legal_grp ( void );
static void		copy_drv_str ( int, int );
/******************************************************************************
 ***			       ENTRY DEFINITIONS			    ***
 *****************************************************************************/
int 		create_lun ( void );

/*==========================================================================*/
int create_lun ( void )
/*==========================================================================*/
{
	int	rdlvl, grp, lun;

	if ( ( grp = check_legal_grp () ) == ERR )
		return ( NONE );

	if ( get_next_nonexistant_lun () >= MAX_LUNS ) {
		pop_max_lun_msg ();
		return ( NONE );
	}

	if ( grp == 0 ) {
		if ( ( rdlvl = pop_predefine_form () ) == QUIT )
			return ( NONE );

		if ( ( pop_create_form ( rdlvl ) ) == QUIT )
			return ( NONE );
	} else {
		lun = grptbl.group[grp].lun_table[0].lun_nbr;
		if ( ( get_page2b ( lun ) ) == ERR ) {
			ace_error ( SCSI_CONDITION, MODE_SENSE_ERROR );
			return ( NONE );
		}
		put_four_bytes ( page2b.LUN_Number_of_Blocks, ( u_int32 ) 0 );
	}

	if ( ( modify_lun ( FROM_CREATE ) ) == NONE )
		return ( NONE );

	return ( ALL );
}

/*==========================================================================*/
static int pop_predefine_form ( void )
/*==========================================================================*/
{
	int		rval, input;

	popbox ( 0, 34, 14, 78, "< PRE-DEFINED CONFIGURATIONS >");

	while ( TRUE ) {
		mvaddstr ( 3, 37, "Enter RAID level choice, or enter" );
		mvaddstr ( 4, 37, "\"q\" to quit.        " );
		mvaddstr ( 7, 37, "0 - RAID Level  0" );
		mvaddstr ( 8, 37, "1 - RAID Level  1" );
		mvaddstr ( 9, 37, "3 - RAID Level  3" );
		mvaddstr ( 10, 37, "5 - RAID Level  5" );
		move ( 4, 51 );
		input = get_reply ();
		switch ( input ) {
			case 'q':
			case 'Q':
				rval = QUIT;
				break;
			case '0':
			case '1':
			case '3':
			case '5':
				rval = input - 0x30;
				break;
			default:
				pop_illegal_value ( );
				rval = CONTINUE;
		}
		if ( rval != CONTINUE )
			break;
	}
	popclr ();
	return ( rval );
}

/*==========================================================================*/
static int pop_create_form ( int rdlvl )
/*==========================================================================*/
{
	int		drv_cnt, i, ch, id, index, chid, ans, rval;
	char		*buf;

#ifdef PARAGON860 /* shorten the popup box */
	popbox ( 0, 34, 20, 78, "< CREATE LUN >" );
#else
	popbox ( 0, 34, 24, 78, "< CREATE LUN >" );
#endif

	while ( TRUE ) {
		display_chid_form ( rdlvl );
		for ( i = 0; i < 11; i++ )
			drvlst[i] = 0xff;
#ifdef PARAGON860 /* restrict drv_cnt to 5 */
		for ( drv_cnt = 0; drv_cnt < 5; drv_cnt++ ) {
#else
		for ( drv_cnt = 0; drv_cnt < 10; drv_cnt++ ) {
#endif
			while ( TRUE ) {
				move ( 6 + drv_cnt, 56 );
				ch = get_reply ();
				if ( ch == 'q' || ch == 'Q' ) {
					rval = QUIT;
					popclr ();
					return ( rval );
				}
				if ( ch == '\n' ) {
					if ( drv_cnt == 0 ) {
						pop_no_entry_error ();
						continue;
					} else
						break;
				}
#ifdef PARAGON860 /* display the character entered */
				addch(ch);
#endif
				ch -= 0x30;
				if ( ( check_legal_chan ( ch, drv_cnt, rdlvl ) ) == ERR )
					continue;
				move ( 6 + drv_cnt, 65 );
				id = get_reply ();
				if ( id == 'q' || id == 'Q' ) {
					rval = QUIT;
					popclr ();
					return ( rval );
				}
#ifdef PARAGON860 /* display the character entered */
				addch(id);
#endif
				id -= 0x30;
				index = ( id * 5 ) + ( ch - 1 );
				if ( ( grp_containing_drv ( index ) ) != 0 ) {
					pop_illegal_value ();
					continue;
				}
				chid = ( ch << 4 ) + id;
				if ( ( check_duplicate ( drv_cnt, chid ) ) == ERR )
					continue;
				drvlst[drv_cnt] = chid;
				break;
			}
			if ( ch == '\n' )
				break;
		}
		if ( ( check_valid_nbr_drvs ( drv_cnt, rdlvl ) ) == ERR )
			continue;

		for ( i = 0; i < 5; i++ )
#ifdef PARAGON860 /* adjust for the shorter box */
			mvaddstr ( i + 13, 39,
				"                                     " );
		do {
			mvaddstr ( 13, 39, "Is the data OK? (y/n/q)  " );
			move ( 13, 63 );
#else
			mvaddstr ( i + 18, 39,
				"                                     " );
		do {
			mvaddstr ( 18, 39, "Is the data OK? (y/n/q)  " );
			move ( 18, 63 );
#endif
			switch ( ans = get_reply () ) {
				case 'n':
				case 'N':
					rval = CONTINUE;
					break;	
				case 'y':
				case 'Y':
					rval = OK;
					break;
				case 'q':
				case 'Q':
					rval = QUIT;
					break;
				default:
					rval = ERR;
					continue;
			}
		} while ( rval != QUIT && rval != OK && rval != CONTINUE );
		if ( rval != CONTINUE )
			break;
	}
	popclr ();
	fill_page2b_info ( rdlvl, drv_cnt );
	copy_drv_str ( drv_cnt, rdlvl );
	return ( rval );
}

/*==========================================================================*/
static void copy_drv_str ( int drv_cnt, int rdlvl )
/*==========================================================================*/
{
	int		i, j, k;
	u_char 		*p = page2b.Configuration_Table;

	memset ( p, 0, sizeof ( page2b.Configuration_Table ) );

	/* If RAID level 1, split the entries into
	   two lists for the configuration table.

	   We want to mirror the first entry with the second,
	   the third with the fourth, and so on.		*/ 

	if ( rdlvl == 1 ) {
		j = ( drv_cnt / 2 ) + 1;
		for ( i = k = 0; i < drv_cnt; i += 2 ) {
			p[k++] = drvlst[i];
			p[j++] = drvlst[1 + i];
		}
	} else {
		for ( i = k = 0; drvlst[i] != 0xff; i++ ) {
			p[k++] = drvlst[i];
			if ( i == 4 )
				++k;		/* start second list */
		}
	}
}

/*==========================================================================*/
static int check_duplicate ( int entry, int chid )
/*==========================================================================*/
{
	int		i;

	for ( i = 0; i < entry; i++ ) {
		if ( drvlst[i] == chid ) {
			pop_illegal_value ();
			return ( ERR );
		}
	}
	return ( OK );
}

/*==========================================================================*/
static int check_legal_chan ( int ch, int count, int rdlvl )
/*==========================================================================*/
{
	int	i;
	int	rval = OK;

	if ( ch < 1 || ch > 5 )
		rval = ERR;

	/* assure that the RAID 3 channel numbers
	   are in ascending order */

	if ( ( rdlvl == 3 ) && ( ch != count + 1 ) )
		rval = ERR;

	/* assure that mirrored pairs in RAID 1
	   are not on the same channel */

	if ( ( rdlvl == 1 ) && ( count % 2 ) ) {
		if ( ( drvlst[count -1] & 0xf0 ) >> 4 == ch )
			rval = ERR;
	}

	/* assure that no drives in RAID 5
	   are on the same channel  */

	if ( ( rdlvl == 5 ) && count > 0 ) {
		for ( i = 0; i < count; i++ ) {
			if ( ( drvlst[i] & 0xf0 ) >> 4 == ch ) {
				rval = ERR;
				break;
			}
		}
	}

	/* assure that no drives in RAID 0 in the
	   same list are on the same channel */

	if ( ( rdlvl == 0 ) && count > 0 ) {
		if ( count < 5 ) {
			for ( i = 0; i < count; i++ ) {
				if ( ( drvlst[i] & 0xf0 ) >> 4 == ch ) {
					rval = ERR;
					break;
				}
			}
		}
		if ( count > 5 ) {
			for ( i = 0; i < ( count % 5 ); i++ ) {
				if ( ( drvlst[5 + i] & 0xf0 ) >> 4 == ch ) {
					rval = ERR;
					break;
				}
			}
		}
	}

	if ( rval == ERR )	
		pop_illegal_value ();

	return ( rval );
}

/*==========================================================================*/
static int check_valid_nbr_drvs ( int drv_cnt, int rdlvl )
/*==========================================================================*/
{
	int	rval = OK;

	/* for RAID 3 and 5, assure that
	   exactly 3 or 5 drives were entered. */

	if ( rdlvl == 3 ) {
		if ( drv_cnt != 3 && drv_cnt != 5 )
			rval = ERR;
	}

	if ( rdlvl == 5 ) {
		if ( drv_cnt < 3 || drv_cnt > 5 )
			rval = ERR;
	}

	/* for RAID 1, assure that an even
	   number of drives was entered. */

	if ( rdlvl == 1 ) {
		if ( ( drv_cnt % 2 ) != 0 )
			rval = ERR;
	}

	if ( rval == ERR )
		pop_illegal_nbr_drvs ();

	return ( rval );
}

/*==========================================================================*/
static void fill_page2b_info ( int rdlvl, int drv_cnt )
 /*==========================================================================*/
{
	int 		i;
	u_int32		lval;

	memset ( &page2b, 0, sizeof ( logical_array_page_t ) );

	put_two_bytes ( page2b.Header.BlockDescriptorLength, 8 );
	page2b.Page_Code = 0x2b;
	page2b.Parameters_Savable = 1;
	page2b.Page_Length = 130;
	page2b.RAID_Level = rdlvl;
	page2b.LUN_Type = 0;
	if ( rdlvl == 3 ) {
		if ( drv_cnt == 3 )
			lval = 1024;
		else
			lval = 2048;
		}
	else
		lval = 512;
	put_four_bytes ( page2b.LUN_Block_Size, lval );
	put_four_bytes ( page2b.LUN_Number_of_Blocks, ( u_int32 ) 0 );
	if ( rdlvl == 3 )
		lval = 1;
	else
		lval = 512;				/* sdpr #u-107 was 64 */
	put_four_bytes ( page2b.LUN_Segment_Size, lval );
	page2b.Segment_Zero_Size = 0;
	page2b.Auto_Detect_Replace_Disabled = OFF;
	page2b.AEN_Polling_Enabled = ON;
	page2b.Parity_Verification_Enabled = ON;
	page2b.Wr_Parity_Verification_Enabled = ON;
	page2b.Reconstruction_Frequency = 1;
	put_two_bytes ( page2b.Reconstruction_Amount, 256 ); /* sdpr u-110 */
}

/*==========================================================================*/
static void display_chid_form ( int rdlvl )
/*==========================================================================*/
{
	mvprintw ( 2, 46, "RAID Level  [ %d ]", rdlvl );
	mvaddstr ( 4, 37, "Drive" );
	mvaddstr ( 6, 39, "1      Channel =     ID =  " );
	mvaddstr ( 7, 39, "2      Channel =     ID =  " );
	mvaddstr ( 8, 39, "3      Channel =     ID =  " );
	mvaddstr ( 9, 39, "4      Channel =     ID =  " );
	mvaddstr ( 10, 39, "5      Channel =     ID =  " );
#ifdef PARAGON860 /* adjust for shorter box */
	mvaddstr ( 13, 39, "Enter the channel and ID number." );
	mvaddstr ( 15, 39, "Press <Enter> at \"Channel\" to end" );
	mvaddstr ( 16, 39, "drive selection, or press \"q\" to" );
	mvaddstr ( 17, 39, "quit." );
#else
	mvaddstr ( 11, 39, "6      Channel =     ID =  " );
	mvaddstr ( 12, 39, "7      Channel =     ID =  " );
	mvaddstr ( 13, 39, "8      Channel =     ID =  " );
	mvaddstr ( 14, 39, "9      Channel =     ID =  " );
	mvaddstr ( 15, 38, "10      Channel =     ID =  " );
	mvaddstr ( 18, 39, "Enter the channel and ID number." );
	mvaddstr ( 20, 39, "Press <Enter> at \"Channel\" to end" );
	mvaddstr ( 21, 39, "drive selection, or press \"q\" to" );
	mvaddstr ( 22, 39, "quit." );
#endif
}

/*==========================================================================*/
static int check_legal_grp ( void )
/*==========================================================================*/
{
	int		i;

	for ( i = 0; i < W7LINES; i++ ) {
		if (W7[i].bold == ON )
			break;
	}

	if ( W7[i].grp != 0 && W7[i].lun != -1 ) {
		pop_illegal_group ();
		return ( ERR );
	}
	return ( W7[i].grp );
}
