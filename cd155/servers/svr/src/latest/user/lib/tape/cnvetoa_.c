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
 *	cnvetoa_.c 11.1 94/03/22 16:50:06
 */
static char	cnvetoa__ver[] = "@(#) sourcefile cnvetoa_.c 11.1 94/03/22 16:50:06";

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
 * cnvetoa_.c
 *
 * Description:
 *
 *  This module contains the FORTRAN I/F Routines for
 *  the following character conversion routines:
 *
 *      etoa()
 *      strnetoa()
 *      set_etoa_trans()
 *      reset_etoa_trans()
 *
 *
 --------------------------------------------------*/


/*--------------------------------------------------
 * etoa_()
 *
 --------------------------------------------------*/

etoa_(char *ebcdic, char *ascii) {

    *ascii = etoa(*ebcdic);
}


/*--------------------------------------------------
 * strnetoa_()
 *
 --------------------------------------------------*/

int strnetoa_(char *ebcdic_p, char *ascii_p, int *length) {

    return ( strnetoa(ebcdic_p, ascii_p, *length) );
}



/*--------------------------------------------------
 * set_etoa_trans_()
 *
 --------------------------------------------------*/

char *set_etoa_trans_(char *new_table_p) {

    return ( set_etoa_trans(new_table_p) );
}

/*--------------------------------------------------
 * reset_etoa_trans_()
 *
 --------------------------------------------------*/

char *reset_etoa_trans_(char **new_table_p) {

    return ( set_etoa_trans(*new_table_p) );
}
