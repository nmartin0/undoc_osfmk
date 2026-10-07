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
 *	tlocblk_.c 11.1 94/03/22 16:50:15
 */
static char	tlocblk__ver[] = "@(#) sourcefile tlocblk_.c 11.1 94/03/22 16:50:15";

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

/* tlocblk_.c
 *
 * INPUT:   handle:     pointer to the handle from the TALLOCTAPE
 *          block_id:   pointer to the Block ID variable to position
 *                      the tape to.
 *
 * OUTPUT:               0 : Locate to block successful.
 *
 *          negative number: Indicates an error
 *
 * Description:
 *
 *     FORTRAN I/F Routine for tlocblk():
 *                                      
 * */

int tlocate_blk_(int *handle, int *block_id) {

    return ( tlocate_blk(*handle, *block_id) );

}


