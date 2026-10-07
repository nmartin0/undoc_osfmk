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
 *	converr.h 11.1 94/03/22 16:49:09
 */
static char	converrh_ver[] = "@(#) sourcefile converr.h 11.1 94/03/22 16:49:09";

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

/* header file containing error literals */



typedef struct CERR_INFO CERR_INFO;

struct CERR_INFO {
  int   err_code;
  char  *err_str;
} ;

extern CERR_INFO cerr_info[];



/*--------------------------------------------------
 * Index into the Error code/sting array
 * for the data conversion errors.
 --------------------------------------------------*/

#define   ERR_INVALID_ZONED_LEN          0
#define   ERR_INVALID_ZONED_NUM          1
#define   ERR_INVALID_SIGN               2
#define   ERR_INVALID_PACKED_LEN         3
#define   ERR_INVALID_PACKED_NUM         4
#define   ERR_OVERFLOW_ZONE_LEN          5
#define   ERR_OVERFLOW_PACKED_LEN        6
#define   ERR_INVALID_ZONE_DIGIT         7

#define   ERR_LOSS_OF_PRECISION          8
#define   ERR_UNDERFLOW                  9
#define   ERR_OVERFLOW                   10

