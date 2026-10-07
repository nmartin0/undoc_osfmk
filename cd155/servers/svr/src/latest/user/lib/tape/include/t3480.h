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
 
/*----------------------------------------------------------------------
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	t3480.h 11.1 94/03/22 16:48:58
 */
static char	t3480h_ver[] = "@(#) sourcefile t3480.h 11.1 94/03/22 16:48:58";

/*
 *
 *----------------------------------------------------------------------*/ 

/*----------------------------------------------------------------------
 * User Exit Parameter List
 *----------------------------------------------------------------------*/ 
typedef struct  {
  int  volseqno;     /* Volume Sequence Number                     */
  char volser[7];
  char dsname[18];   /* Data Set Name                              */
  int  dsseqno;      /* Data Set Sequence Number                   */
  char bufferhd[4];  /* UHLn or UTLn                               */
  char buffer[76];   /* User (Input/Output) (Hdr/Trailer) Label data*/
} EXITPARM;

/*----------------------------------------------------------------------
 * Format of Volume Serial Number Items
 *----------------------------------------------------------------------*/ 
typedef struct {
  char serialno[7];
} VOLSER;

/*----------------------------------------------------------------------
 * Volume Control Block.  Used in calls to tapealloc.
 *----------------------------------------------------------------------*/ 
typedef struct {
VOLSER  *vollist;     /* List of Volume Serial Number(s) to be used */
int    (*exlst) ();   /* User Exit Routine Addresses                */
} VCB;


/*----------------------------------------------------------------------
 * Data set control Block.  Used in calls to tdsopen.
 *----------------------------------------------------------------------*/ 
typedef struct {
  /*--------------------------------------------------------------------
   * REC1 fields
   *--------------------------------------------------------------------*/ 
  char dsname[18];   /* Data Set Name                              */
  int  volseqno;     /* Volume Sequence Number                     */
  int  dsseqno;      /* Data Set Sequence Number                   */
  char expirec;      /* Expiration Date - century (blank=19, 0=20  */
  int  expireyy;     /* Expiration Date - year (91, 92, etc)       */
  int  expiredd;     /* Expiration Date - day of year              */
  char system_code[14]; /* System Code                                */
  /*--------------------------------------------------------------------
   * REC2 Fields
   *--------------------------------------------------------------------*/ 
  char recfm;        /* Record Format                              */
  int  blocklen;     /* Block Length                               */
  int  reclen;       /* Record Length                              */
  char blkattr;
  /*--------------------------------------------------------------------
   * options 
   *--------------------------------------------------------------------*/ 
  int  blocknum;     /* Block ID Number                            */
  int  read;         /* Opened for read (write if zero)            */
  int  buffered_mode;/* Use buffering on I/O node                  */
  int  block_mode;   /* Read/Write Blocks at a time                */
  int  num_tapeblk_bufs;
} DCB;

#define INPUT_HEADER_LABEL    2
#define INPUT_TRAILER_LABEL   3
#define OUTPUT_HEADER_LABEL   4
#define OUTPUT_TRAILER_LABEL  5
#define EOV_USEREXIT          6
#define WAITING_FOR_OPEN      7
#define NEXT_VOLSER           8
#define OVERWRITE_EXPIRE      9


/*----------------------------------------------------------------------
 * Errors
 *----------------------------------------------------------------------*/ 
#define TERR_HDRLEN_NOT80                   - 3
#define TERR_VOL1_NOT_FOUND                 - 4
#define TERR_REC1_NOT_FOUND                 - 5
#define TERR_INVALID_INPUT_TAPE_SCRATCH     - 6
#define TERR_ALREADY_TDSOPEND               - 7
#define TERR_INVALID_DSSEQNO                - 8
#define TERR_BLOCKNUM_NOT_SUPPORTED         - 9
#define TERR_NO_VOLSER                      -10
#define TERR_COULD_NOT_RESERVE_TAPE         -11
#define TERR_INVALID_VOLUME                 -12
#define TERR_EXPIRATION_DATE                -13
#define TERR_FSF_FAILED                     -14
#define TERR_DSSEQNO_NOT_FOUND              -15
#define TERR_INVALID_VOLSEQNO               -16
#define TERR_MEM_ALLOC                      -17
#define TERR_INVALID_DSNAME_SPECIFIED       -18
#define TERR_INVALID_DSNAME_LEN             -19
#define TERR_INVALID_DSNAME_QUALLEN         -20
#define TERR_INVALID_DSNAME_FRSTQUAL        -21
#define TERR_INVALID_DSNAME_CHAR            -22
#define TERR_CANNOT_APPEND                  -23
#define TERR_CWRITE_FAILED                  -24
#define TERR_CLOSE_FAILED                   -25
#define TERR_RELEASE_TAPE_FAILED            -26
#define TERR_BLKCNTS_DONT_MATCH             -27
#define TERR_NO_DATASET_OPENED              -28
#define TERR_INV_HANDLE                     -29
#define TERR_NOTM_USRLABELS                 -30
#define TERR_NOT_START_OF_DATASET           -31
#define TERR_MAX_VOLS_EXCEEDED              -32
#define TERR_NO_NEXT_VOLSER                 -33
#define TERR_VOLSEQNO_ORDER                 -34
#define TERR_INVALID_BLOCKNUM               -35
#define TERR_BLOCKNUM_ON_WRITE              -36
#define TERR_CANNOT_OPEN                    -37
#define TINFO_EOF_ENCOUNTERED               -38

