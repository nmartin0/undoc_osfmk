/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	tgetblk_.c 11.1 94/03/22 16:50:15
 */
static char	tgetblk__ver[] = "@(#) sourcefile tgetblk_.c 11.1 94/03/22 16:50:15";

/*
 *
 *  ******************  ENGINEERING CHANGE HISTORY ***********************
 *
 *    Date      Engineer                  Description
 *
 *  2/25/92     G. Haycox       Module creation release to BETA
 *
 *
 */

/* tgetblk_.c
 *
 * INPUT:   handle: pointer to the handle from the TALLOCTAPE
 *
 * OUTPUT:  positive number: Current Block ID
 *
 *          negative number: indicating an error
 *
 * Description:
 *
 *      FORTRAN I/F Routine for tgetblk():
 *
 * */


int tget_blk_id_(int *handle) {

    return ( tget_blk_id(*handle) );

}


