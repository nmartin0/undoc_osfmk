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
  Name:		$RCSfile: adebug.c,v $
  Version:	$Id: adebug.c,v 1.2.4.1 1995/06/11 22:19:34 kat Exp $
  Title:	'ace' debugger
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:34 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272

  COPYRIGHT 1992, NCR Corporation

  Description:	Debugging routines for lookin at various 'ace' things.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: adebug.c,v $
 * Revision 1.2.4.1  1995/06/11  22:19:34  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:09  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:41  richardg
 * Initial revision
 *
 * Revision 1.2  1992/04/14  15:24:28  root
 * sdpr u-083 Added request sense call and display to help debug problem
 * related to this sdpr - bem.
 *
 * Revision 1.1  1992/03/18  14:00:18  bmyers
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
#include <curses.h>
/******************************************************************************
 ***			      EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern void		popbox ( int, int, int, int, char * );
extern void		popclr ( void );
extern int		get_reply ( void );
/******************************************************************************
 ***			      VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
extern line_ent_t		W2[];
extern line_ent_t		W7[];
extern grptbl_t			grptbl;
extern hw_zipcode_t		hw_addr;
extern SCSI_Inquiry_Data_t	InquiryData;
extern logical_array_page_t	page2a;
extern logical_array_page_t	page2b;
/******************************************************************************
 ***			      INTERNAL PROCEDURES			    ***
 *****************************************************************************/
static void			show_grptbl ( void );
static void			show_bitmap ( void );
static void			show_p2b ( void );
static void			show_p2a ( void );
static void			show_winst ( line_ent_t *, int );
static void			show_grp_p2a ( void );
static void			show_hwzip ( void );
static void			show_inquiry ( void );
static void			show_sense ( void );
static u_char			sbuf[128];
/******************************************************************************
 ***			       ENTRY DEFINITIONS			    ***
 *****************************************************************************/
void				pop_debug ( void );

/*===========================================================================*/
void pop_debug ( void )
/*===========================================================================*/
{
	int	i, answer;

#ifdef ASCII
	popbox ( 0, 0, 23, 79, "DEBUG" );
#else
	popbox ( 0, 0, 24, 79, "DEBUG" );
#endif

	while ( TRUE ) {
		clear ();
		mvaddstr ( 0, 37, "DEBUG" );
		mvaddstr ( 2, 2, "g - display group table" );
		mvaddstr ( 2, 42, "2 - display W2 struct" );
		mvaddstr ( 3, 2, "a - display page2a" );
		mvaddstr ( 3, 42, "7 - display W7 struct" );
		mvaddstr ( 4, 2, "b - display page2b" );
		mvaddstr ( 4, 42, "h - hardware zipcode" );
		mvaddstr ( 5, 2, "m - display page2b disk bit map" );
		mvaddstr ( 5, 42, "i - display inquiry data" );
		mvaddstr ( 6, 2, "p - display grptbl.page2a" );
		mvaddstr ( 6, 42, "r - request sense" );
		mvaddstr ( 7, 2, "q - exit debug" );
		mvaddstr ( 23, 58, " ");
		move ( 23, 58 );
#ifdef PARAGON860 /* menu won't display without a refresh */
		refresh();
#endif
		switch ( answer = getch () ) {
		case 'q':
		case 'Q':
			clear ();
			popclr ();
			return;
		case 'g':
			show_grptbl ();
			break;
		case 'a':
			show_p2a ();
			break;
		case 'p':
			show_grp_p2a ();
			break;
		case 'b':
			show_p2b ();
			break;
		case 'm':
			show_bitmap ();
			break;
		case 'h':
			show_hwzip ();
			break;
		case 'i':
			show_inquiry ();
			break;
		case 'r':
			show_sense ();
			break;
		case '2':
			show_winst ( W2, W2LINES );
			break;
		case '7':
			show_winst ( W7, W7LINES );
			break;
		}
	}
}

/*===========================================================================*/
static void show_grptbl ( void )
/*===========================================================================*/
{
	int grp, i, ans;
	grp_entry_t *g = &grptbl.group[0];


	for (grp = 0; grp < MAX_GROUPS; grp++ ) {
		clear ();
		mvprintw (0,30,"Group Number %d", grp);
		mvprintw (14,2,"Group Table");
		mvprintw (15,2,"nbr_luns_in_grp       %x",g->nbr_luns_in_grp);
		mvprintw (16,2,"nbr_real_luns_in_grp  %x",g->nbr_real_luns_in_grp);
		mvprintw (17,2,"nbr_drvs_in_grp       %x",g->nbr_drvs_in_grp);
		mvprintw (21,2,"group_count  %x",grptbl.group_count);
		mvprintw (21,22,"lun_count  %x",grptbl.lun_count);
		mvprintw (21,42,"real_lun_count  %x",grptbl.real_lun_count);
		mvprintw (2,2,"LUN Table");
		for (i = 0; i < 4; i++ ) {
			mvprintw (3,2+(i*20),"type       %x",g->lun_table[i].type);
			mvprintw (4,2+(i*20),"real       %x",g->lun_table[i].real);
			mvprintw (5,2+(i*20),"status     %x",g->lun_table[i].status);
			mvprintw (6,2+(i*20),"lun_nmber  %x",g->lun_table[i].lun_nbr);
			mvprintw (7,2+(i*20),"raid_lvl   %x",g->lun_table[i].raid_lvl);
			mvprintw (8,2+(i*20),"capacity   %lx",(u_int32)g->lun_table[i].capacity);
		}
		mvaddstr (10,2,"LUN List");
		move (11, 2);
		for (i = 0; i < g->nbr_luns_in_grp; i++ )
			printw ("%02x ",g->lun_list[i]);
		mvaddstr (14,30,"Drive List");
		move (15,40);
		for (i = 0; i < g->nbr_drvs_in_grp; i++) {
			mvprintw (15,30+(i*10),"bold %02x",g->drive_list[i].bold);
			mvprintw (16,30+(i*10),"stat %02x",g->drive_list[i].status);
			mvprintw (17,30+(i*10),"grp  %02x",g->drive_list[i].group);
			mvprintw (18,30+(i*10),"drv  %02x",g->drive_list[i].drive);
		}
		move ( 23, 78);
		refresh ();
		ans = getch ();
		if ( ans == 'q' )
			break;
		++g;
	}
	clear ();
	refresh ();
}

/*===========================================================================*/
static void show_bitmap ( void )
/*===========================================================================*/
{
	int i;
	u_char *p = &page2b.Disk_Bit_Map[0];
	
	clear ();
	mvaddstr ( 2,2,"Page 2b Disk Bit Map" );
	move ( 4, 2 );
	for ( i = 0; i < 32; i++ ) {
		if ( i && ( i % 16 ) == 0 )
			move ( ( i / 16 ) + 4, 2 );
		printw ("%02x ", (u_char) p[i] );
	}
	move ( 23, 78);
	refresh ();
	getch ();
	refresh ();
}

/*===========================================================================*/
static void show_p2a ( void )
/*===========================================================================*/
{
	int i;
	u_char *p = (u_char *) &page2a;

	clear ();
	mvaddstr ( 2,2,"Page 2a" );
	move ( 4, 2 );
	for ( i = 0; i < sizeof ( physical_array_page_t ); i++ ) {
		if ( i && ( i % 16 ) == 0 )
			move ( ( i / 16 ) + 4, 2 );
		printw ("%02x ", (u_char) p[i] );
	}
	move ( 23, 78);
	refresh ();
	getch ();
	refresh ();
}

/*===========================================================================*/
static void show_grp_p2a ( void )
/*===========================================================================*/
{
	int i;
	u_char *p = (u_char *) &grptbl.pg2a[0];

	clear ();
	mvaddstr ( 2,2,"grptbl.page2a" );
	move ( 4, 2 );
	for ( i = 0; i < 40; i++ ) {
		if ( i && ( i % 5 ) == 0 )
			move ( ( i / 5 ) + 4, 2 );
		printw ("%02x ", (u_char) p[i] );
	}
	move ( 23, 78);
	refresh ();
	getch ();
	refresh ();
}

/*===========================================================================*/
static void show_p2b ( void )
/*===========================================================================*/
{
	int i;
	u_char *p = (u_char *) &page2b;

	clear ();
	mvaddstr ( 2,2,"Page 2b" );
	move ( 4, 2 );
	for ( i = 0; i < sizeof ( logical_array_page_t ); i++ ) {
		if ( i && ( i % 16 ) == 0 )
			move ( ( i / 16 ) + 4, 2 );
		printw ("%02x ", (u_char) p[i] );
	}
	move ( 23, 78);
	refresh ();
	getch ();
	refresh ();
}

/*===========================================================================*/
static void show_winst ( line_ent_t *win, int lines )
/*===========================================================================*/
{
	int	i;

	clear ();

	for ( i = 0; i < lines; i++ ) {
		clear ();
		mvprintw ( 4, 2, "w->lun     %2x", win[i].lun );
		mvprintw ( 5, 2, "w->grp     %2x", win[i].grp );
		mvprintw ( 6, 2, "w->bold    %2x", win[i].bold );
		mvprintw ( 7, 2, "w->status  %2x", win[i].status );
		move ( 23, 79 );
		refresh ();
		if ( getch () == 'q' )
			break;
		refresh ();
	}
}

/*===========================================================================*/
static void show_hwzip ( void )
/*===========================================================================*/
{
	clear ();
	mvprintw ( 2, 2, "IO_Bus              %d", hw_addr.IO_Bus );
	mvprintw ( 3, 2, "SCSI_Controller     %2d", hw_addr.SCSI_Controller );
	mvprintw ( 4, 2, "SCSI_Bus            %2d", hw_addr.SCSI_Bus );
	mvprintw ( 5, 2, "PUN                 %2d", hw_addr.PUN );
	mvprintw ( 6, 2, "LUN                 %2d", hw_addr.LUN );
	mvprintw ( 7, 2, "DAC_SCSI_Bus        %2d", hw_addr.DAC_SCSI_Bus );
	mvprintw ( 8, 2, "DAC_PUN             %2d", hw_addr.DAC_PUN );
	mvprintw ( 9, 2, "DAC_LUN             %2d", hw_addr.DAC_LUN );
	move ( 23, 79 );
	refresh ();
	getch ();
}

/*===========================================================================*/
static void show_inquiry ( void )
/*===========================================================================*/
{
	int			i;
	SCSI_Inquiry_Data_t	*p = &InquiryData;
	u_char			*pt =  ( u_char * ) &InquiryData;

	clear ();
	mvprintw ( 2, 2, "Periph_Qual         %2d", p->Periph_Qualifier );
	mvprintw ( 3, 2, "Device_Type_Qual    %2d", p->Device_Type_Qualifier );
#ifdef PARAGON860 /* delimit the printing of these strings */
	mvprintw ( 4, 2, "Vendor_ID           \"%.8s\"", p->Vendor_ID );
	mvprintw ( 5, 2, "Product_ID          \"%.16s\"", p->Product_ID );
	mvprintw ( 6, 2, "Revision_Level      \"%.4s\"", p->Revision_Level );
#else
	mvprintw ( 4, 2, "Vendor_ID           %s", p->Vendor_ID );
	mvprintw ( 5, 2, "Product_ID          %s", p->Product_ID );
	mvprintw ( 6, 2, "Revision_Level      %s", p->Revision_Level );
#endif
	move ( 8, 2 );
	for ( i = 0; i < sizeof ( SCSI_Inquiry_Data_t ); i++ ) {
		if ( i && ( i % 16 ) == 0 )
			move ( ( i / 16 ) + 8, 2 );
		printw ("%02x ", (u_char) pt[i] );
	}
	move ( 23, 79 );
	refresh ();
	getch ();
}

/*===========================================================================*/
static void show_sense ( void )
/*===========================================================================*/
{
	int			i, lun, status;

	clear ();
	mvaddstr ( 2, 2, "Enter LUN #   " );
#ifdef PARAGON860 /* refresh required to display previous mvaddstr */
	refresh();
#endif
	lun = getch ();
#ifdef PARAGON860 /* echo the lun selected */
	addch(lun);
	refresh();
#endif

	for ( i = 0; i < sizeof ( sbuf ); i++ )
		sbuf[i] = 0;

#ifdef PARAGON860
	status = request_sense ( lun, sbuf, sizeof ( sbuf ), TRUE );
#else
	status = request_sense ( lun, sbuf, sizeof ( sbuf ), FALSE );
#endif

	mvprintw ( 4, 2, "Status %x", status );

	move ( 6, 2 );
	for ( i = 0; i < sizeof ( sbuf ); i++ ) {
		if ( i && ( i % 16 ) == 0 )
			move ( ( i / 16 ) + 6, 2 );
		printw ("%02x ", (u_char) sbuf[i] );
	}
#ifdef PARAGON860 /* refresh required to display data */
	refresh();
#endif

	getch ();
}
