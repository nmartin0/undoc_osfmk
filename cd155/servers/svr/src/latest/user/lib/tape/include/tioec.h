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
 *	tioec.h 11.1 94/03/22 16:49:02
 */
static char	tioech_ver[] = "@(#) sourcefile tioec.h 11.1 94/03/22 16:49:02";

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
 * Errors for tape_io_init()
 --------------------------------------------------*/

#define   TERR_BLOCKSIZE                   -1000
#define   TERR_REC_FM                      -1001
#define   TERR_INVALID_BLK_ATTR            -1002
#define   TERR_NON_INTEGRAL_REC_SIZE       -1003
#define   TERR_FIXED_RECLEN                -1004
#define   TERR_INVALID_NON_BUFFERED_MODE   -1005
#define   TERR_VAR_RECLEN                  -1006
#define   TERR_RESERVED7                   -1007
#define   TERR_INVALID_NUM_BUFS            -1008
#define   TERR_MEMORY_ALLOC_ERROR          -1009
#define   TERR_UNDEF_REC_SIZE              -1010

/*--------------------------------------------------
 * Errors for tread()
 --------------------------------------------------*/

#define   TERR_INVALID_RD_BUF_LEN          -1100
#define   TERR_BWD_LENGTH                  -1101
#define   TERR_FIX_BLK_LEN                 -1102
#define   TERR_BAD_REC_FORMAT              -1103
#define   TERR_RDW_LENGTH                  -1104
#define   TERR_RESERVED16                  -1105
#define   TERR_NOT_OPEN_FOR_READ           -1106
#define   TERR_ATTEMPT_RD_PAST_EOF         -1107
#define   TERR_TRUNC_BLK                   -1108
#define   TERR_INVALID_SDW_STATE           -1109

/*--------------------------------------------------
 * Errors for twrite()
 --------------------------------------------------*/

#define   TERR_INVALID_WRT_BUF_LEN          -1200
#define   TERR_CWRITE                       -1201
#define   TERR_TIODONE                      -1202
#define   TERR_IWRITE                       -1203
#define   TERR_NOT_OPEN_FOR_WRITE           -1204
#define   TERR_ZERO_LEN_WRITE               -1205
#define   TERR_ATTEMPT_WRT_AFTER_EOF        -1206
#define   TERR_INVALID_BLOCK_COND           -1207

/*--------------------------------------------------
 * Errors for tget_blk_id()
 --------------------------------------------------*/

#define   TERR_TGET_DS_NOT_OPEN             -1300
#define   TERR_RD_TAPE_POS                  -1301
#define   TERR_ATTEMPT_TGET_BLKID_EOF       -1302


/*--------------------------------------------------
 * Errors for tlocate_blk()
 --------------------------------------------------*/

#define   TERR_TLOC_NOT_OPEN_RD             -1400
#define   TERR_LOC_TAPE_ID                  -1401
#define   TERR_ATTEMPT_TLOC_EOF             -1402


/*--------------------------------------------------
 * Generic Errors
 --------------------------------------------------*/

#define   TERR_EXPECT_EOM                   -2000
#define   TERR_IREAD                        -2001
#define   TERR_INVALID_PARAM                -2002
#define   TERR_INVALID_HANDLE               -2003
#define   TERR_CFS_IOCTL                    -2004



