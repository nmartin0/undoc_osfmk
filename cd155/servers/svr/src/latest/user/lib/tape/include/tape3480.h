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
 *	tape3480.h 11.1 94/03/22 16:48:59
 */
static char	tape3480h_ver[] = "@(#) sourcefile tape3480.h 11.1 94/03/22 16:48:59";

/*
 *
 */

#define MAX_DEVICES 10
#define MAX_VOLUMES 75

#include "t3480.h"

typedef struct {
  char labelid[4];
  char labelno;
  char volser[7];
  char reserved;
  char vtoc[11];
  char reservd2[21];
  char nameaddr[11];
  char reservd3[30];
} VOL1;

typedef struct {
  char labelid[4];
  char labelno;
  char dsname[18];
  char dsserno[7];
  int  volseqno;
  int  dsseqno;
  char genno[5];
  char version[3];
  char createc;     /* century */
  int  createyy;    /* year    */
  int  createdd;    /* day     */
  char expirec;  /* century */
  int  expireyy;    /* year    */
  int  expiredd;    /* day     */
  int  security;
  int  blockcnt;
  char system_code[14];
  char reserved[8];
} REC1;

typedef struct {
  char labelid[4];
  char labelno;
  char recfm;
  int  blocklen;
  int  reclen;
  char density;
  int  position;
  char jbstepid[18];
  char rec_tech[3];
  char control;
  char reserved;
  char blkattr;
  char reservd2[9];
  char checkpnt;
  char reservd3[33];
} REC2;



/*----------------------------------------------------------------------
 *                      Local VCB - LVCB
 *----------------------------------------------------------------------*/ 
typedef struct {
  int    tindex;          /* file handle returned on TAPEALLOC */
  int    fildes ;         /* file handle returned on UNIX open */
  int     last_dsseqno;   /* DSSEQNO of the current or previous data set */
  char    path[128];      /* path used in TAPEALLOC, e.g., /cfs/9tape  */
  int     cur_volseqno;
  VOL1    vol1;
  REC1    rec1;
  REC2    rec2;
  VOLSER  vollist[MAX_VOLUMES];
  int     volcount;
  int    (*exlst) ();     /* User Exit Routine Addresses                */
  int     blockcnt;       /*                       */
  int     blocknum;       /* Block ID Number                            */
  int     read;           /* Opened for read (write if zero)            */
  int     buffered_mode;  /* Use buffering on I/O node                  */
  int     block_mode;     /* Read/Write Blocks at a time                */
  int     num_tapeblk_bufs;
  TAPE_IO tape_io;
  int     tapedev;        /* 1 = 9-track                                */  
  int     debug_nounload; /* for DEBUG - don't unload on tdealloc   */ 
  char    expirec;        /* Expiration Date - century (blank=19, 0=20  */
  int     expireyy;       /* Expiration Date - year (91, 92, etc)       */
  int     expiredd;       /* Expiration Date - day of year              */
  int     eodhit;         /* Expiration Date - day of year              */
  int     located;        /* a TLOCATE has been issued */
  int     written;        /* At least one TWRITE has been written */
  int     feov;           /* FEOV has been issued.                      */
  int     tdsopen;        /* TDSOPEN in progress                        */
  int     debug_msgs;     /* 1 = Do printfs in DEbug version            */
  int     volfsm;         /* Volume Checking FSM                        */
  int     dsfsm;          /* Data Set Checking FSM                      */
                          
} LVCB;

typedef struct {
  unsigned char    flag;
  char    msg1[8];
  char    msg2[8];
} LOADDISP;



/*
 * Main Tape data structure.  Contains a pointer to a linked list of
 * local VCBs.
 */
typedef struct {
  LVCB  *lvcblist[100]; /* Copy of user's volume control block */
} TAPEMAIN;



