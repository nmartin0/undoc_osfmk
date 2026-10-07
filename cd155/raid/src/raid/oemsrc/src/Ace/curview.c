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
  Name:		$RCSfile: curview.c,v $
  Version:	$Id: curview.c,v 1.2.4.1 1995/06/11 22:19:43 kat Exp $
  Title:	Current View
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:43 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272

  COPYRIGHT 1992, NCR Corporation

  Description:	This module contains the routines to setup the current
		lun and drive screen structures.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: curview.c,v $
 * Revision 1.2.4.1  1995/06/11  22:19:43  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:20  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:45  richardg
 * Initial revision
 *
 * Revision 1.4  1992/07/13  18:57:57  root
 * sdpr u-142 Changed drive status mask from 0x07 to 0x0f. - bem
 *
 * Revision 1.3  1992/05/26  15:16:29  root
 * sdpr u-120 - Several changes as a result of user validation testing.
 * Arror key logic added.  Logic changed to keep same selection after screen
 * update due to manual input or timeout.
 *
 * Revision 1.2  1992/05/15  15:49:14  root
 * sdpr u-119 Used lun_blk_sz in capacity calculation - bem
 *
 * Revision 1.1  1992/03/18  14:00:21  bmyers
 * Initial revision
 *
 */

/******************************************************************************
 ***				    INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "stddefs.h"
#include "acurses.h"
#include <curses.h>
/******************************************************************************
 ***			      EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern grptbl_t		grptbl;
extern int		current_lun;
extern int		current_drive;
extern int		current_view;
extern int		current_group;
extern int		last_view;
extern line_ent_t	W2[];
extern line_ent_t	W4[];
extern line_ent_t	W7[];
extern line_ent_t	W8[];
extern void		paint_window ( line_ent_t *, int );
/******************************************************************************
 ***			      VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
static char		grptmp[20];
static char		luntmp[20];
static char		statmp[20];
static char		rdltmp[20];
static char		devtmp[20];
static char		spctmp[20];
static int		highlight_drive = 0;
static int		highlight_lun = 0;
/******************************************************************************
 ***			      EXTERNAL PROCEDURES			    ***
 *****************************************************************************/
/******************************************************************************
 ***			      INTERNAL PROCEDURES			    ***
 *****************************************************************************/
/*
static void	 	update_drive_view ( void );
static void		update_lun_view ( void );
*/
static int		get_first_drive ( void );
static void		set_lun_highlights ( void );
static void		set_drv_highlights ( void );
static void	 	init_view ( void );
static void		fill_line_info ( grp_entry_t *, int, int, int );
/******************************************************************************
 ***			       ENTRY DEFINITIONS			    ***
 *****************************************************************************/
void			highlight_next_lun ( int );
void			highlight_next_drive ( int, int );
void			highlight_next_drive_in_grp ( void );
void 			highlight_drv_to_add ( void );
void			setup_current_view ( void );
void		 	setup_drive_view ( void );
void			setup_lun_view ( void );
int			grp_containing_drv ( int );
int			lun_containing_drv ( int );

/*========================================================================*/
void setup_current_view ( void )
/*========================================================================*/
{
	if ( current_view == DRIVE_VIEW ) {
		W4[0].bold = ON;
		W8[0].bold = OFF;
	} else {
		W4[0].bold = OFF;
		W8[0].bold = ON;
	}
	init_view ();
	paint_window ( W2, W2LINES );
	paint_window ( W4, W4LINES );
	paint_window ( W7, W7LINES );
	paint_window ( W8, W8LINES );
}

/*========================================================================*/
void setup_drive_view ( void )
/*========================================================================*/
{
	int		index;

	line_ent_t	*d = &W2[0];
	u_char 		*p = ( u_char * ) &grptbl.pg2a;

	for ( index = 0; index < MAX_DRIVES; index++ ) {
		sprintf ( d->data, "%2x", *p & 0xf );
		d->status = *p;
		d->grp = grp_containing_drv ( index );
		d->lun = lun_containing_drv ( index );
		++p;
		++d;
	}
}

/*========================================================================*/
void setup_lun_view ( void )
/*========================================================================*/
{
	int		i, grp, lun;
	int		line = 0;
	u_int32		cap;
	grp_entry_t	*g = grptbl.group;
	line_ent_t	*p = &W7[0];

	for ( i = 0; i < W7LINES; i++ ) {
		sprintf (&p[i].data[0], \
			"                                            " );
		p[i].grp = -1;
	}

	if ( g->nbr_drvs_in_grp != 0 ) {
		sprintf (&p[line].data[0], \
			" 0   -      -        -     spare    unknown " );
		p[line].grp = 0;
		line++;
	}
	++g;
	for ( grp = 1; grp <= grptbl.group_count; grp++, g++ ) {
		for ( lun = 0; lun < g->nbr_luns_in_grp; lun++ )	{
			sprintf ( grptmp, "%2x   ", grp );
			fill_line_info ( g, grp, lun, line );	
#ifdef PARAGON860 /* fix overflow problem for capacity > 2gb */
			cap = ( (double) g->lun_table[lun].capacity / (double)  1048576)
			  * g->lun_table[lun].block_size;
#else
			cap = ( g->lun_table[lun].capacity *
			   g->lun_table[lun].block_size ) / ( u_int32 ) 1048576;
#endif
#ifdef PARAGON860 /* trim the trailing space at the end of capacity */
			sprintf ( spctmp, "%5d", ( u_int32 ) cap );
#else
			sprintf ( spctmp, "%5d  ", ( u_int32 ) cap );
#endif
			if ( g->lun_table[lun].real == TRUE )
				sprintf (&p[line].data[0], "%s%s%s%s%s%s ", grptmp,
					luntmp, statmp, rdltmp, devtmp, spctmp );
			else 
				sprintf ( &p[line].data[0], 
			"%s-      -        -       -      %s ",grptmp, spctmp );

			p[line].grp = grp;
			line++;
		}
	}
}

/*========================================================================*/
static void init_view ( void )
/*========================================================================*/
{
	int	i, index, id, ch;

	if ( current_view == last_view )
		return;

	for ( i = 0; i < W2LINES; i++ )
		W2[i].bold = OFF;

	for ( i = 0; i < W7LINES; i++ )
		W7[i].bold = OFF;

	if ( current_view == DRIVE_VIEW ) {
		index = get_first_drive ();
		W2[index].bold = ON;
		current_group = W2[index].grp;
		id = index / 5;
		ch = index % 5;
		current_drive = ( ch << 4 ) + id;
		set_lun_highlights ();
		current_lun = -1;
	} else {
		W7[0].bold = ON;
		current_group = W7[0].grp;
		current_lun = 0;
		set_drv_highlights ();
		current_drive = -1;
	}
}

/*========================================================================*/
static int get_first_drive ( void )
/*========================================================================*/
{
	int		index;
	u_char 		*p = ( u_char * ) &grptbl.pg2a;

	for ( index = 0; index < MAX_DRIVES; index++ ) {
		if ( ( grp_containing_drv ( index ) ) != -1 )
			break;
	}
	return ( index );
}

/*========================================================================*/
void highlight_next_drive ( int direction, int amount )
/*========================================================================*/
{
	int		id, ch, index;
	u_char 		*p = ( u_char * ) &grptbl.pg2a;

	ch = ( current_drive >> 4 ) & 0x07;
	id = current_drive & 0x07;
	index = ( id * 5 ) + ch;
	W2[index].bold = OFF;
	if ( direction == TRUE ) {
		while ( ( index += amount ) < MAX_DRIVES ) {
			if ( ( grp_containing_drv ( index ) ) != -1 )
				break;
		}
		if ( index >= MAX_DRIVES ) {
			for ( index %= MAX_DRIVES; index < MAX_DRIVES; index += amount ) {
				if ( ( grp_containing_drv ( index ) ) != -1 )
					break;
			}
		}
	} else {
		while ( ( index -= amount ) >= 0 ) {
			if ( ( grp_containing_drv ( index ) ) != -1 )
				break;
		}
		if ( index < 0 ) {
			for ( index += MAX_DRIVES; index >= 0; index -= amount) {
				if ( ( grp_containing_drv ( index ) ) != -1 )
					break;
			}
		}
	}
	W2[index].bold = ON;
	current_group = W2[index].grp;
	id = index / 5;
	ch = index % 5;
	current_drive = ( ch << 4 ) + id;
	set_lun_highlights ();
}

/*========================================================================*/
void highlight_drv_to_add ( void )
/*========================================================================*/
{
	int		id, ch, index, input;

	ch = ( current_drive >> 4 ) & 0x07;
	id = current_drive & 0x07;
	index = ( id * 5 ) + ch;
	W2[index].bold = OFF;
	W2[0].bold = ON;
	paint_window ( W2, W2LINES );
	current_drive = 0;
	while ( TRUE ) {
		input = getch ();
		if ( input == '\n' )
			break;
		if ( input != ' ' )
			continue;
		for ( id = 0; id < MAX_DRIVES; id++ )
			W2[id].bold = OFF;
		ch = ( current_drive >> 4 ) & 0x07;
		id = current_drive & 0x07;
		index = ( id * 5 ) + ch;
		if (++index >= MAX_DRIVES )
			index = 0;
		W2[index].bold = ON;
		current_group = W2[index].grp;
		id = index / 5;
		ch = index % 5;
		current_drive = ( ch << 4 ) + id;
		paint_window ( W2, W2LINES );
	}
}

/*========================================================================*/
void highlight_next_drive_in_grp ( void )
/*========================================================================*/
{
	int		id, ch, index;
	u_char 		*p = ( u_char * ) &grptbl.pg2a;

	ch = ( current_drive >> 4 ) & 0x07;
	id = current_drive & 0x07;
	index = ( id * 5 ) + ch;
	W2[index].bold = OFF;
	while ( ++index < MAX_DRIVES ) {
		if ( ( grp_containing_drv ( index ) ) == current_group )
			break;
	}
	if ( index == MAX_DRIVES ) {
		for ( index = 0; index < MAX_DRIVES; index++ ) {
			if ( ( grp_containing_drv ( index ) ) == current_group )
				break;
		}
	}
	W2[index].bold = ON;
	id = index / 5;
	ch = index % 5;
	current_drive = ( ch << 4 ) + id;
	set_lun_highlights ();
}

/*========================================================================*/
void highlight_next_lun ( int direction )
/*========================================================================*/
{
	int		cnt;

	cnt = grptbl.lun_count;
	if ( grptbl.group[0].nbr_drvs_in_grp != 0 )
		++cnt;
	W7[current_lun].bold = OFF;
	if ( direction == TRUE ) {
		if ( ++current_lun == cnt )
			current_lun = 0;
	} else {
		if ( --current_lun < 0 )
			current_lun = cnt - 1;
	}              
	W7[current_lun].bold = ON;
	current_group = W7[current_lun].grp;
	set_drv_highlights ();
}

/*========================================================================*/
void set_lun_highlights ( void )
/*========================================================================*/
{
	int	i, cnt, grp, offset, ch, id;

	ch = ( current_drive >> 4 ) & 0x07;
	id = current_drive & 0x07;
	offset =  ( id * 5 ) + ch;
	grp = W2[offset].grp;
	cnt = grptbl.lun_count;
	if ( grptbl.group[0].nbr_drvs_in_grp != 0 )
		++cnt;
	for ( i = 0; i <= cnt; i++ ) {
		if ( grp == W7[i].grp )
			W7[i].bold = ON;
		else	
			W7[i].bold = OFF;
	}
}

/*========================================================================*/
void set_drv_highlights ( void )
/*========================================================================*/
{
	int	i, grp;

	grp = W7[current_lun].grp;
	for ( i = 0; i < MAX_DRIVES; i++ ) {
		if ( grp == W2[i].grp )
			W2[i].bold = ON;
		else	
			W2[i].bold = OFF;
	}
}

/*========================================================================*/
int grp_containing_drv ( int drive )
/*========================================================================*/
{
/*
	entry param 'drive' is an index derived from the highlighted drive's
	position on the screen drive matrix. It must be converted into channel
	and id for this routine.
*/
 
	int		grp, drv, chid, cnt;
	grp_entry_t	*g = &grptbl.group[0];

	cnt = grptbl.group_count;
	if ( grptbl.group[0].nbr_drvs_in_grp != 0 )
		++cnt;
	chid = ( ( ( drive % 5 ) + 1 ) << 4 ) + ( drive / 5 );
	for ( grp = 0; grp <= cnt; grp++ ) {
		for ( drv = 0; drv < g[grp].nbr_drvs_in_grp; drv++ ) {
			if ( g[grp].drive_list[drv].drive == chid )
				return ( grp );
		}
	}
	return ( -1 );
}

/*========================================================================*/
int lun_containing_drv ( int index )
/*========================================================================*/
{
	int		grp;
	grp_entry_t	*g = &grptbl.group[0];

	grp = grp_containing_drv ( index );
	if ( grp == 0 || grp == -1 )
		return ( -1 );
	return ( g[grp].lun_table[0].lun_nbr );
}

/*========================================================================*/
static void fill_line_info ( grp_entry_t *g, int grp, int lun, int line )
/*========================================================================*/
{
	line_ent_t	*p = &W7[0];

#ifdef PARAGON860 /* trim trail space to align with titles */
	sprintf ( luntmp, "%x  ", g->lun_table[lun].lun_nbr );
#else
	sprintf ( luntmp, "%x   ", g->lun_table[lun].lun_nbr );
#endif
	p[line].lun = g->lun_table[lun].lun_nbr;
	p[line].status = g->lun_table[lun].status & 0x07;
	switch ( g->lun_table[lun].status  & 0x07) {
		case OPTIMAL:
			sprintf ( statmp, WLUNSTAT_0 );
			break;
		case DEGRADED:
			sprintf ( statmp, WLUNSTAT_1 );
			break;
		case RECONSTLUN:
			sprintf ( statmp, WLUNSTAT_2 );
			break;
		case DEAD:
			sprintf ( statmp, WLUNSTAT_4 );
			break;
		default:
			sprintf ( statmp, WUNKNOWN );
	}
	sprintf ( rdltmp, "%x  ", g->lun_table[lun].raid_lvl );
	get_system_dev_name ( devtmp, g->lun_table[lun].lun_nbr );
}
