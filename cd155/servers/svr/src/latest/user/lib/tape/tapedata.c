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
 *	tapedata.c 11.1 94/03/22 16:49:37
 */
static char	tapedata_ver[] = "@(#) sourcefile tapedata.c 11.1 94/03/22 16:49:37";

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

/* tapedata.c
 *
 *
 * Description:
 *
 *      This module contains the global data used for
 *      the tape read/write routines
 *
 * */

#include <stdio.h>
#include "tioec.h"

/*--------------------------------------------------
 * Error Code and Error print strings,  tioerr.h
 * contain the literal definitions for the err_code
 * index into this array.
 --------------------------------------------------*/

struct ERR_INFO {
  int   err_code;
  char  *err_str;
} err_info[] = {

/* tape_io_init Errors */

/* 000 */  {TERR_BLOCKSIZE, "Invalid DCB.blocklen"},
/* 001 */  {TERR_REC_FM, "Invalid DCB.recfm"},
/* 002 */  {TERR_INVALID_BLK_ATTR, "Invalid DCB.blkattr"},
/* 003 */  {TERR_NON_INTEGRAL_REC_SIZE, "reclen not multiple of blocklen"},
/* 004 */  {TERR_FIXED_RECLEN, "FIXED: reclen != blocklen"},
/* 005 */  {TERR_INVALID_NON_BUFFERED_MODE, "Invalid record format for Unbuffered mode"},
/* 006 */  {TERR_VAR_RECLEN, "VARIABLE DCB.reclen"},
/* 007 */  {TERR_RESERVED7, ""},
/* 008 */  {TERR_INVALID_NUM_BUFS, "Invalid num_tapeblk_bufs"},
/* 009 */  {TERR_MEMORY_ALLOC_ERROR, "malloc"},
/* 010 */  {TERR_UNDEF_REC_SIZE, "UNDEFINED reclen must be 0"},


/* tread Errors */

/* 011 */  {TERR_INVALID_RD_BUF_LEN, "tread req < reclen"},
/* 012 */  {TERR_BWD_LENGTH, "Invalid BDW length"},
/* 013 */  {TERR_FIX_BLK_LEN, "FIXED: (HDR2)blocklen"},
/* 014 */  {TERR_BAD_REC_FORMAT, "Bad Record Format"},
/* 015 */  {TERR_RDW_LENGTH, "RDW length"},
/* 016 */  {TERR_RESERVED16, ""},
/* 017 */  {TERR_NOT_OPEN_FOR_READ, "Attempt TREAD while NOT Open for read\n"},
/* 018 */  {TERR_ATTEMPT_RD_PAST_EOF, "Attempt TREAD after EOF\n"},
/* 019 */  {TERR_TRUNC_BLK, "Truncated Block not Last in Dataset\n"},
/* 020 */  {TERR_INVALID_SDW_STATE, "Spanned: Invalid SDW state\n"},


/* twrite Errors */

/* 021 */  {TERR_INVALID_WRT_BUF_LEN, "Invalid write buffer length"},
/* 022 */  {TERR_CWRITE, "cwrite"},
/* 023 */  {TERR_TIODONE, "tiodone"},
/* 024 */  {TERR_IWRITE, "iwrite"},
/* 025 */  {TERR_NOT_OPEN_FOR_WRITE, "Attempt TWRITE while NOT Open for write\n"},
/* 026 */  {TERR_ZERO_LEN_WRITE, "Invalid zero length TWRITE\n"},
/* 027 */  {TERR_ATTEMPT_WRT_AFTER_EOF, "Attempt TWRITE after EOF\n"},
/* 028 */  {TERR_INVALID_BLOCK_COND, "Spanned (NOT blocked): NON-Empty buffer\n"},


/* tget_blk_id Errors */

/* 029 */  {TERR_TGET_DS_NOT_OPEN, "Invalid TGET_BLK_ID: Device NOT open\n"},
/* 030 */  {TERR_RD_TAPE_POS, "READ_TAPE_POSITION"},
/* 031 */  {TERR_ATTEMPT_TGET_BLKID_EOF, "Attempt TGET_BLK_ID after EOF\n"},


/* tlocate_blk Errors */

/* 032 */  {TERR_TLOC_NOT_OPEN_RD, "TLOCATE_BLOCK while NOT open for read\n"},
/* 033 */  {TERR_LOC_TAPE_ID, "LOCATE_TAPE_ID"},
/* 034 */  {TERR_ATTEMPT_TLOC_EOF, "Attempt TLOCATE_BLK after EOF\n"},


/* Generic Errors */

/* 035 */  {TERR_EXPECT_EOM, "Expected EOM"},
/* 036 */  {TERR_IREAD, "iread"},
/* 037 */  {TERR_INVALID_PARAM, "Invalid Param"},
/* 038 */  {TERR_INVALID_HANDLE, "Invalid Handle"},
/* 039 */  {TERR_CFS_IOCTL, "CFS_IOCTL"},

};



/*--------------------------------------------------
 * SPIDER: Debug print control (default is OFF)
 *
 --------------------------------------------------*/

struct {

    int     spider;
    int     read_cnt;
    char   *err_fmtstr;
    char   *loc_fmtstr;

} tapeio_glob = {0, 0, "%s #%04d ERROR:  %s", "%s #%04d %s"};




