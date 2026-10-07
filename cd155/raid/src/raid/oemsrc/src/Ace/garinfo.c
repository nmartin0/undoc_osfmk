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
  Name:		$RCSfile: garinfo.c,v $
  Version:	$Id: garinfo.c,v 1.2.4.1 1995/06/11 22:19:48 kat Exp $
  Title:	Get Array Info
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:48 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272


  COPYRIGHT 1992, NCR Corporation

  Description:	This module gathers the needed array info from mode sense,
		read capacity, etc and fills the appropriate 'ace'
		structures.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: garinfo.c,v $
 * Revision 1.2.4.1  1995/06/11  22:19:48  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:29  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:48  richardg
 * Initial revision
 *
 * Revision 1.5  1992/07/13  18:52:15  root
 * sdpr u-139 Added libio call to IO_NoErrorLogging in check_for_extra_space
 * function. - bem
 *
 * Revision 1.4  1992/05/15  15:50:19  root
 * sdpr u-119 Added lun_blk_sz save to fill_lun_info - bem
 *
 * Revision 1.3  1992/04/10  20:34:01  root
 * sdpr #u-79 Changed logic so lun is created by putting the lun number in
 * page2b and issuing the mode-select to lun 0.
 *
 * Revision 1.2  1992/03/20  17:11:10  bmyers
 * Added locking parameter to mode_select call.
 *
 * Revision 1.1  1992/03/18  14:00:23  bmyers
 * Initial revision
 *
 */
/******************************************************************************
 ***				    INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "acedefs.h"
#include "scsidefs.h"
#include <curses.h>
/******************************************************************************
 ***			      EXTERNAL REFERENCES			    ***
 *****************************************************************************/
physical_array_page_t	page2a;
logical_array_page_t	page2b;
SCSI_Inquiry_Data_t	InquiryData;
int			get_page2b ( int );
extern grptbl_t         grptbl;
extern void		setup_lun_view ( void );
extern void		setup_drive_view ( void );
extern u_int32 		get_capacity ( int );
extern u_int32		get_four_bytes ( u_char * );
/******************************************************************************
 ***			      VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
static int	lun_count;
static int	list[MAX_LUNS + 1];
/******************************************************************************
 ***			      INTERNAL PROCEDURES			    ***
 *****************************************************************************/
static void	get_lun_info ( int );
static void	fill_pg2a_drv_status ( void );
static void	check_for_extra_space ( int );
static void	fill_lun_entry ( int, int, int );
static int	get_drive_status ( int );
static void     fill_spare_list ( void );
static int 	get_drv_entry ( void );
static int	check_for_new_group ( int );
static void	copy_drv_entry ( int, int, int );
static int	do_inquiry ( int );
static int	get_page2a ( void );
static drv_ref_t tmplist[MAX_DRIVES];

/*=======================================================================*/
void get_array_info ( int update_needed )
/*=======================================================================*/
{
	int     lun, i, grp;
	int	list_length;
	int	*lptr = list;

	if ( update_needed == ALL ) {
		memset ( &grptbl, 0, ( size_t ) sizeof ( grptbl_t ) );
		if ( ( get_page2a () ) == OK ) {
			fill_pg2a_drv_status ();
			fill_spare_list ();
		}
		for ( lun = 0; lun < MAX_LUNS; lun++ ) 
			get_lun_info ( lun );

		for ( grp = 1; grp <= grptbl.group_count; grp++ )
			check_for_extra_space ( grp );
	}
	setup_lun_view ();
	setup_drive_view ();
}

/*=======================================================================*/
static void get_lun_info ( int lun )
/*=======================================================================*/
{
	int	nbr_drvs, grp;
	int 	exists = TRUE;

	if ( ( do_inquiry ( lun ) ) == OK ) {
		if ( ( get_page2b ( lun ) ) == OK ) {
			nbr_drvs = get_drv_entry ( );
			if ( ( check_for_new_group ( nbr_drvs ) ) == TRUE )
				grptbl.group_count++;
			if ( ( grp = check_lun_in_previous_grp () ) == -1 ) {
				grp = grptbl.group_count;
				exists = FALSE;
			}
			fill_lun_entry ( grp, lun, TRUE );
			if ( exists == FALSE )
				copy_drv_entry ( grptbl.group_count,
						lun, nbr_drvs );
		}
	}
}

/*=======================================================================*/
static void fill_pg2a_drv_status ( void )
/*=======================================================================*/
{
	int	id, ch;
	u_char	*des = ( u_char * ) &grptbl.pg2a;
	u_char	*src = ( u_char * ) &page2a.Drive_Status;

	for ( id = 0; id < 8 ; id++, src += 10 )
		for ( ch = 1; ch <= 5; ch++ )
			*des++ = *src++;
 }

/*=======================================================================*/
static void fill_spare_list ( void )
/*=======================================================================*/
{
	int		i, chan, id;
	int		drives = 0;
	drv_ref_t *	d = ( drv_ref_t * ) &grptbl.group[0].drive_list[0];
	u_char		*dptr = ( u_char * ) &page2a.Drive_Status;

	for ( id = 0; id < MAX_SCSI_ID; id++, dptr+= 10 ) {
		for ( chan = 1; chan <= MAX_CHANNEL; chan++ ) {
			if ( *dptr++ == SPARE ) {
				d->drive = ( chan << 4 ) | id;
				d->group = 0;
				d->status = SPARE;
				++d;
				drives++;
			}
		}
	}
	grptbl.group[0].nbr_drvs_in_grp = drives;
}

/*=======================================================================*/
static void check_for_extra_space ( int grp )
/*=======================================================================*/
{
	int		stat, lun;
	grp_entry_t	*g = &grptbl.group[grp];
	logical_array_page_t	*p = &page2b;

#ifdef UV4
	IO_NoErrorLogging ( TRUE );
#endif
	if ( g->nbr_luns_in_grp == 0 || g->lun_table[0].type != SUB_LUN )
		return;

/*  get page2b info for LUN using this drive group */

	lun = g->lun_table[0].lun_nbr;

	stat = mode_sense ( lun, p, sizeof ( logical_array_page_t ), 
		PAGE_CONTROL_CURRENT_VALUES, LOGICAL_ARRAY_PG_CODE );

	if ( stat != OK )
		return;

	lun = get_next_nonexistant_lun ();

	if ( lun >= MAX_LUNS )
		return;		/* stop if were out of lun's */

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
			fill_lun_entry ( grp, lun, FALSE );

		/* Now, make the LUN disappear  by deleting it */

		p->Action_Status = DELETE_LUN;

		stat = mode_select ( lun, p, sizeof ( logical_array_page_t ),
			PAGE_FORMAT_SCSI_2, SAVE_PARAMETERS, FALSE );
	}
#ifdef UV4
	IO_NoErrorLogging ( FALSE );
#endif
}


/*=======================================================================*/
static void fill_lun_entry ( int grp_cnt, int lun, int real )
/*=======================================================================*/
{
	grp_entry_t *g = &grptbl.group[grp_cnt];
	lun_entry_t *l = &grptbl.group[grp_cnt].lun_table[g->nbr_luns_in_grp];
	logical_array_page_t *lap = &page2b;

	grptbl.group[grp_cnt].lun_list[g->nbr_luns_in_grp] = lun;
	g->nbr_luns_in_grp++;
	grptbl.lun_count++;
	if ( real ) {
		g->nbr_real_luns_in_grp++;
		grptbl.real_lun_count++;
		l->lun_nbr = lun;
	} else
		l->lun_nbr = -1;
	l->type = lap->LUN_Type;
	l->real = real;
	l->status = lap->Action_Status;
	l->raid_lvl = lap->RAID_Level;
	l->block_size = ( u_int32 ) get_four_bytes ( lap->LUN_Block_Size );
	l->capacity = ( u_int32 ) get_four_bytes ( lap->LUN_Number_of_Blocks );
}

/*=======================================================================*/
static int get_drv_entry ( void )
/*=======================================================================*/
{
	int	i, chan, id, bitmap, ch;
	int	drives = 0;
	drv_ref_t *tmp = ( drv_ref_t * ) &tmplist[0];
	u_char *dbm = ( u_char * ) &page2b.Disk_Bit_Map[0];
	u_char *cfgt = ( u_char * ) &page2b.Configuration_Table[0];
	grp_entry_t *g = &grptbl.group[grptbl.group_count];

	memset ( tmp, 0, ( size_t ) sizeof ( drv_ref_t ) );

/*
 *  get drive information from configuration table if present
*/
	for ( i = 0; i < 64; i++ ) {
		if ( cfgt[i] == 0 ) {			/* end of list */
			if ( cfgt[1+i] == 0 )		/* end of data */
				break;
			i++;
		}
		tmp->drive = cfgt[i];
		tmp->group = grptbl.group_count;
		tmp->status = get_drive_status ( cfgt[i] );
		++tmp;
		drives++;
	}

	if ( drives != 0 ) {
		return ( drives );
	}
/*
 *  get drive information from disk bit map if no configuration table
*/
	for ( i = 0; i < 32; i++ ) {
		if ( dbm[i] != 0 ) {
			bitmap = dbm[i];
			id = i / 2;
			for ( ch = 1; ch < 6; ch++ ) {
				if ( bitmap & 1 ) {
					tmp->drive = ( ch << 4 ) | id;
					tmp->group = grptbl.group_count;
					tmp->status = get_drive_status \
							( tmp->drive );
					++tmp;
					drives++;
				}
				bitmap >>= 1;
			}
		}
	}
	return ( drives );
}

/*=======================================================================*/
static int get_drive_status ( int chid )
/*=======================================================================*/
{
	int ch, id;

	ch = ( chid >> 4 ) & 0x07;
	id = chid & 0x07;
	return ( page2a.Drive_Status[ ( id * 15 ) + ( ch - 1 ) ] );
}

/*=======================================================================*/
static int check_for_new_group ( int nbr_drvs )
/*=======================================================================*/
{
	int i, grp, drv, ndrv;
	drv_ref_t *tmp = ( drv_ref_t * ) &tmplist[0];
	int grpcnt = grptbl.group_count;
/*
	if ( grpcnt == 1 )
		return ( TRUE );
*/
	for ( i = 0; i < nbr_drvs; i++ ) {
		for ( grp = 1; grp <= grpcnt; grp++ ) {
			ndrv = grptbl.group[grp].nbr_drvs_in_grp;
			for ( drv = 0; drv < ndrv; drv++ ) {
				if ( grptbl.group[grp].drive_list[drv].drive \
					== tmplist[i].drive )
					return ( FALSE );
			}
		}
	}
	return ( TRUE );
}

/*=======================================================================*/
static void copy_drv_entry ( int grp_cnt, int lun, int nbr_drvs )
/*=======================================================================*/
{
	int	i;
	drv_ref_t *src = ( drv_ref_t * ) &tmplist[0];
	drv_ref_t *des = ( drv_ref_t * ) &grptbl.group[grp_cnt].drive_list[0];

	grptbl.group[grp_cnt].nbr_drvs_in_grp = nbr_drvs;

	memcpy ( des, src, ( sizeof ( drv_ref_t ) * MAX_DRIVES ) );

	for ( i = 0; i < nbr_drvs; i++ )
		grptbl.group[grp_cnt].drive_list[i].group = grp_cnt;
}

/*=======================================================================*/
static int check_lun_in_previous_grp ( void )
/*=======================================================================*/
{
	int		ch, id, index;

	ch = ( tmplist[0].drive >> 4 ) & 0x07;
	id = tmplist[0].drive & 0x07;
	index = ( ( id * 5 ) + ch ) - 1; 
	return ( grp_containing_drv ( index ) );

}

/*=======================================================================*/
static int do_inquiry ( int lun )
/*=======================================================================*/
{
	int 			status;
	SCSI_Inquiry_Data_t 	*p = &InquiryData;

	p->Periph_Qualifier = 0x07;
	status = inquiry ( lun, p, sizeof ( SCSI_Inquiry_Data_t) );

	if ( status == OK )
		if ( p->Periph_Qualifier == 0 )
			return ( OK );
	return ( ERR );
}

/*=======================================================================*/
static int get_page2a ( void )
/*=======================================================================*/
{
	physical_array_page_t	*p = &page2a;

	return ( mode_sense ( 0, p, sizeof ( physical_array_page_t ), 
		PAGE_CONTROL_CURRENT_VALUES, PHYSICAL_ARRAY_PG_CODE ) );
}

/*=======================================================================*/
int get_page2b ( int lun )
/*=======================================================================*/
{
	logical_array_page_t	*p = &page2b;

	return ( mode_sense ( lun, p, sizeof ( logical_array_page_t ), 
		PAGE_CONTROL_CURRENT_VALUES, LOGICAL_ARRAY_PG_CODE ) );
}
