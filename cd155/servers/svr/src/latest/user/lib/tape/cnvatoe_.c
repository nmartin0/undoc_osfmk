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
 *	cnvatoe_.c 11.1 94/03/22 16:50:05
 */
static char	cnvatoe__ver[] = "@(#) sourcefile cnvatoe_.c 11.1 94/03/22 16:50:05";

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

#include "type.h"
#include "convt.h"
#include "cnvproto.h"


/*--------------------------------------------------
 * cnvatoe_.c
 *
 * Description:
 *
 *  This module contains the FORTRAN I/F Routines
 *  for the following functions:
 *
 *      atoe()
 *      strnatoe()
 *      set_atoe_trans()
 *      reset_atoe_trans()
 *
 --------------------------------------------------*/

/*--------------------------------------------------
 * atoe_()
 *
 --------------------------------------------------*/

atoe_(char *ascii, char *ebcdic) {

     *ebcdic = atoe(*ascii);
}


/*--------------------------------------------------
 * strnatoe_()
 *
 --------------------------------------------------*/

int strnatoe_(char *ascii_p, char *ebcdic_p, int *length) {

    return ( strnatoe(ascii_p, ebcdic_p, *length) );
}

/*--------------------------------------------------
 * set_atoe_trans_()
 *
 --------------------------------------------------*/

char *set_atoe_trans_(char *new_table_p) {

    return (set_atoe_trans( new_table_p) );
}


/*--------------------------------------------------
 * reset_atoe_trans_()
 *
 --------------------------------------------------*/

char *reset_atoe_trans_(char **new_table_p) {

    return (set_atoe_trans( *new_table_p) );
}
