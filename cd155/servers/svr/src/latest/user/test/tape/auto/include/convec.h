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
 *	convec.h 11.1 94/03/22 16:49:06
 */
static char	convech_ver[] = "@(#) sourcefile convec.h 11.1 94/03/22 16:49:06";

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

/*--------------------------------------------------
 * Errors for data conversion routines
 --------------------------------------------------*/


#define   CERR_INVALID_ZONED_LEN             -10000
#define   CERR_INVALID_ZONED_NUM             -10001
#define   CERR_INVALID_SIGN                  -10002
#define   CERR_INVALID_PACKED_LEN            -10003
#define   CERR_INVALID_PACKED_NUM            -10004
#define   CERR_OVERFLOW_ZONE_LEN             -10005
#define   CERR_OVERFLOW_PACKED_LEN           -10006
#define   CERR_INVALID_ZONE_DIGIT            -10007


/*--------------------------------------------------
 * Generic Errors
 --------------------------------------------------*/

#define   CERR_LOSS_OF_PRECISION             -10100
#define   CERR_UNDERFLOW                     -10101
#define   CERR_OVERFLOW                      -10102


