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
 *	tioerr.h 11.1 94/03/22 16:49:03
 */
static char	tioerrh_ver[] = "@(#) sourcefile tioerr.h 11.1 94/03/22 16:49:03";

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

/* header file containing err literals */


#define ERR_PRN(s) fprintf(tape_io_p->logdev, \
                      tapeio_glob.err_fmtstr , __FILE__, __LINE__, s )

#define PRN_LOC(s) fprintf(tape_io_p->logdev, \
                      tapeio_glob.loc_fmtstr, __FILE__, __LINE__, s )

#define   SPIDER              1  /* change to 0 to not compile spider code */

#define   SPIDER_ALL          -1
#define   SPIDER_OFF           0

#define   SPIDER_RESERVED     0x00000001
#define   SPIDER_ENTER_EXIT   0x00000002
#define   SPIDER_TAPEIO_INIT  0x00000004
#define   SPIDER_TREAD        0x00000008
#define   SPIDER_CHK_IRD      0x00000010
#define   SPIDER_TWRITE       0x00000020
#define   SPIDER_TAPEIO_EXIT  0x00000040
#define   SPIDER_SYNC_TIO     0x00000080
#define   SPIDER_TGET_BLKID   0x00000100
#define   SPIDER_TLOCATE      0x00000200
/*                              ^
   -----------------------------^ don't use this bit (negative) not allowed */


typedef struct ERR_INFO ERR_INFO;

struct ERR_INFO {
  int   err_code;
  char  *err_str;
} ;

extern ERR_INFO err_info[];



/*--------------------------------------------------
 * Errors for tape_io_init()
 --------------------------------------------------*/

#define   ERR_BLOCKSIZE                   0
#define   ERR_REC_FM                      1
#define   ERR_INVALID_BLK_ATTR            2
#define   ERR_NON_INTEGRAL_REC_SIZE       3
#define   ERR_FIXED_RECLEN                4
#define   ERR_INVALID_NON_BUFFERED_MODE   5
#define   ERR_VAR_RECLEN                  6
#define   ERR_RESERVED7                   7
#define   ERR_INVALID_NUM_BUFS            8
#define   ERR_MEMORY_ALLOC_ERROR          9
#define   ERR_UNDEF_REC_SIZE              10

/*--------------------------------------------------
 * Errors for tread()
 --------------------------------------------------*/

#define   ERR_INVALID_RD_BUF_LEN          11
#define   ERR_BWD_LENGTH                  12
#define   ERR_FIX_BLK_LEN                 13
#define   ERR_BAD_REC_FORMAT              14
#define   ERR_RDW_LENGTH                  15
#define   ERR_RESERVED16                  16
#define   ERR_NOT_OPEN_FOR_READ           17
#define   ERR_ATTEMPT_RD_PAST_EOF         18
#define   ERR_TRUNC_BLK                   19
#define   ERR_INVALID_SDW_STATE           20

/*--------------------------------------------------
 * Errors for twrite()
 --------------------------------------------------*/

#define   ERR_INVALID_WRT_BUF_LEN          21
#define   ERR_CWRITE                       22
#define   ERR_TIODONE                      23
#define   ERR_IWRITE                       24
#define   ERR_NOT_OPEN_FOR_WRITE           25
#define   ERR_ZERO_LEN_WRITE               26
#define   ERR_ATTEMPT_WRT_AFTER_EOF        27
#define   ERR_INVALID_BLOCK_COND           28

/*--------------------------------------------------
 * Errors for tget_blk_id()
 --------------------------------------------------*/

#define   ERR_TGET_DS_NOT_OPEN             29
#define   ERR_RD_TAPE_POS                  30
#define   ERR_ATTEMPT_TGET_BLKID_EOF       31



/*--------------------------------------------------
 * Errors for tlocate_blk()
 --------------------------------------------------*/

#define   ERR_TLOC_NOT_OPEN_RD             32
#define   ERR_LOC_TAPE_ID                  33
#define   ERR_ATTEMPT_TLOC_EOF             34


/*--------------------------------------------------
 * Generic Errors
 --------------------------------------------------*/

#define   ERR_EXPECT_EOM                   35
#define   ERR_IREAD                        36
#define   ERR_INVALID_PARAM                37
#define   ERR_INVALID_HANDLE               38
#define   ERR_CFS_IOCTL                    39

/*
 * Turned off the following for port from iPSC to Paragon
 */
#define	SYNC	0
#define	TEOM	0
#define	TWAIT	0
#define	TDONE	0
#define	CLREOF	0
#define	RELTAPE	0
#define	TESTRDY	0
