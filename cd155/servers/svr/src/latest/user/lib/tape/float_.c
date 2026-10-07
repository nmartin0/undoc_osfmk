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
 *	float_.c 11.1 94/03/22 16:50:08
 */
static char	float__ver[] = "@(#) sourcefile float_.c 11.1 94/03/22 16:50:08";

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
 * istoieees_()  ('C' - Istois)
 *
 *  FORTRAN I/F Routine.
 *
 *  Name change from Istois to istoieees because
 *  FORTRAN is not case sensative.
 *
 --------------------------------------------------*/


int istoieees_(float *ibm_float, float *i_float, int *length) {

    return ( Istois(ibm_float, i_float, *length) );

}


/*--------------------------------------------------
 * istoieeed_() - ('C' - Istoid)
 *
 *  FORTRAN I/F Routine.
 *
 *  Name change from Istoid to istoieeed because
 *  FORTRAN is not case sensative.
 *
 --------------------------------------------------*/


void istoieeed_(float *ibm_float, double *i_double, int *length) {

   Istoid(ibm_float, i_double, *length);
}


/*--------------------------------------------------
 * ieeestois_() - ('C' - istoIs)
 *
 *  FORTRAN I/F Routine.
 *
 *  Name change from istoIs to ieeestois because
 *  FORTRAN is not case sensative.
 *
 --------------------------------------------------*/


int ieeestois_(float *i_float, float *ibm_float, int *length) {

    return ( istoIs(i_float, ibm_float, *length) );
}

/* PAGE  */

/*--------------------------------------------------
 * ieeedtois_() - ('C' - idtoIs)
 *
 *  FORTRAN I/F Routine.
 *
 *  Name change from idtoIs to ieeedtois because
 *  FORTRAN is not case sensative.
 *
 --------------------------------------------------*/


int ieeedtois_(double *i_double, float *ibm_float, int *length) {

    return ( idtoIs(i_double, ibm_float, *length) );
}


/*--------------------------------------------------
 * ieeedtoid_() - ('C' - idtoId)
 *
 *  FORTRAN I/F Routine.
 *
 *  Name change from idtoId to ieeedtoid because
 *  FORTRAN is not case sensative.
 *
 --------------------------------------------------*/


int ieeedtoid_(double *i_double, double *ibm_double, int *length) {

    return ( idtoId(i_double, ibm_double, *length) );
}


/*--------------------------------------------------
 * idtoieeed_() - ('C' - Idtoid)
 *
 *  FORTRAN I/F Routine.
 *
 *  Name change from Idtoid to idtoieeed because
 *  FORTRAN is not case sensative.
 *
 --------------------------------------------------*/


int idtoieeed_(double *ibm_double, double *i_double, int *length) {

    return ( Idtoid(ibm_double, i_double, *length) );
}
