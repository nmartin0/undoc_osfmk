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
 *	tioproto.h 11.1 94/03/22 16:49:04
 */
static char	tioprotoh_ver[] = "@(#) sourcefile tioproto.h 11.1 94/03/22 16:49:04";

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

/************************************************
 *  tioproto.h
 *
 *    Function Prototypes for blocking/deblocking
 ************************************************/

/*--------------------------------------------------
 *  User interface routines - Exposed EPS routines
 --------------------------------------------------*/

int twrite(int handle, char *buffer, int length);
int tread(int handle, char *buffer, int length);
int tlocate_blk(int handle, int block_id);
int tget_blk_id(int handle);


/*--------------------------------------------------
 *  Internal function calls used by the tape
 *  library interface programs.
 --------------------------------------------------*/

int   check_iread(LVCB *lvcb_p, TAPE_IO *tape_io_p);
void  cleareof(int fd);
int   ird_status(LVCB *lvcb_p, TAPE_IO *tape_io_p, int iread_rv);
int   issue_iread(int fildes, TAPE_IO *tape_io_p, char *buffer, int length);
void  log_time(FILE *logdev);
void  prn_tiostats(LVCB *lvcb_p, TAPE_IO *tape_io_p);
int   set_spider(int spider_val);
int   set_tapedev(int handle, int tapedev, int no_unld);
int   sync_tio(LVCB *lvcb_p, TAPE_IO *tape_io_p);
int   tape_io_init(int handle);
int   tape_io_exit(int handle);
int   tiodone(int iod);             /* int iod is really a (_iod_t *) */
void  tio_log_init(LVCB *lvcb_p);
int   tiowait(int iod);             /* int iod is really a (_iod_t *) */
int   tisbot(int fd);
int   tiseof(int fd);
int   tiseom(int fd);


/*--------------------------------------------------
 *  External function called to inform the label
 *  processing code that the following condition
 *  occurred:
 *
 *  TREAD: A tapemark was encountered.
 *
 *  TWRITE: Logical end-of-media encountered.
 --------------------------------------------------*/

int   eod(int handle);


