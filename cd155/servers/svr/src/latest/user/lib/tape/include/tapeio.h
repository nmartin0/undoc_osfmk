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
 *	tapeio.h 11.1 94/03/22 16:49:01
 */
static char	tapeioh_ver[] = "@(#) sourcefile tapeio.h 11.1 94/03/22 16:49:01";

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

/* TAPEIO.H:
 *
 * This include file contains definitions for
 *  the record formats for the IBM tape
 *
 *
*/

typedef struct BDW BDW;

struct BDW{

    bit16   blk_len;    /* needs to be swapped (IBM) */
    bit16   reserved;

};


typedef struct RDW RDW;

struct RDW{

    bit16   rec_len;        /* needs to be swapped (IBM) */
    bit16   reserved;

};


typedef struct REC_HDR REC_HDR;

struct REC_HDR{

    RDW     rdw;        /* record descriptor word */
    ubit8   rec_data;   /* 1st byte of the record data */
};




/* **********>>>> Always keep the BLK_BUF_HDR on 32 bit boundaries */

typedef struct BLK_BUF_HDR BLK_BUF_HDR;

struct BLK_BUF_HDR {

    BLK_BUF_HDR    *next;   /* pointer to the next block buffer */
    int             mid;    /* message id, iread or iwrite operations */
    int             status; /* status of data buffer, FULL, EMPTY, etc.. */
    int             length; /* number of bytes read */
} ;

/* **********>>>> Always keep the BLK_BUF_HDR on 32 bit boundaries */


/* Buffer Header Status Conditions */


#define CMD_COMPLETE    0x00

#define EMPTY           0x00
#define FULL            0x01


typedef struct VAR_BLK_BUF VAR_BLK_BUF;

struct VAR_BLK_BUF{

    BLK_BUF_HDR     blk_buf_hdr;    /* Tape block buffer header */
    BDW             bdw;            /* block descriptor word */
    RDW             rdw;            /* record descriptor word */
    ubit8           rec_data;       /* 1st byte of record data */

};

typedef struct BLK_BUF BLK_BUF;

struct BLK_BUF{

    BLK_BUF_HDR     blk_buf_hdr;    /* Tape block buffer header */
    ubit8           data;           /* first byte of the tape block data */

};


typedef struct POSITION POSITION;

/* this structure is defined to perform a union type overlay,
 * see the IPS for READ_TAPE_POSITION for the actual structure
 * definition
 */

struct POSITION{

    char  flags;
    char  partition;
    char  reserved_1;
    char  reserved_2;
    unsigned int   first_block;
    unsigned int   last_block;
    unsigned int   blocks_in_buffer;
    unsigned int   number_of_bytes;
};



typedef struct TAPE_IO TAPE_IO;

struct TAPE_IO{

    void           *dmem_p;         /* pointer to memory allocated by malloc */
    BLK_BUF_HDR    *head;           /* points to the current free buffer */
    BLK_BUF_HDR    *tail;           /* points to the current full buffer */
    int             phy_blk_num;    /* physical block number */
    char           *blk_proc_p;     /* block process pointer */
    int             block_id;       /* block id saved on get_blkid() */
    int             num_records;    /* number of records processed */
    int             oper_blked;     /* iread or iwrite blocked, waiting for buffer */
    int             oper_busy;      /* iread or iwrite still busy */
    int             num_ireads;     /* the number of iread commands issued */
    int             num_iwrites;    /* the number of iwrite commands issued */
    int             num_cwrites;    /* the number of cwrite commands issued */
    int             eod;            /* EOD encountered, phy pos on opposite side of TM */
    int             eof;            /* EOF header has been read */
    int             trunc_blk;      /* Flag to indicate a truncated block has been read */
    FILE           *logdev;         /* LOGDEV: FILE pointer to the log device */
    bit16           blk_len;        /* block length */
    bit16           record_format;  /* record format; UNDEFINED when in block_mode */

};

/* Literal Definitions for record format */

#define   REC_FORMAT  0x0F
#define   UNDEFINED   0x00
#define   FIXED       0x01
#define   VARIABLE    0x02

#define   BLK_ATTR              0xF0
#define   UNBLOCKED_RECORDS     0x00
#define   BLOCKED_RECORDS       0x10
#define   SPAN_OR_STD_RECORDS   0x20
#define   SPANNED_REC           0x20
#define   STANDARD_REC          0x20
#define   BLK_S_RECORDS         0x30

/* Literal Definitions for spanned records */

#define   SDW_IDLE              0x00
#define   SDW_START             0x01
#define   SDW_MID               0x03
#define   SDW_END               0x02



#define     NEW_VOL     1   /* return from eod() at volume switching */

typedef struct TAPEIO_GLOB TAPEIO_GLOB;

struct TAPEIO_GLOB {

    int     spider;
    int     read_cnt;
    char   *err_fmtstr;
    char   *loc_fmtstr;

};

extern TAPEIO_GLOB  tapeio_glob;
extern int          errno;



