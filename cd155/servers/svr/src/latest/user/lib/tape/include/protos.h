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
 *	protos.h 11.1 94/03/22 16:48:57
 */
static char	protosh_ver[] = "@(#) sourcefile protos.h 11.1 94/03/22 16:48:57";

/*
 *
 */

int  t3480_interface(int fd, int cnt, int cmd);
int  open_locate_failed(int handle);
int  dbglabel(int handle);
void ltrace(int format, ...);
int  locate_tape_id(int fildes, int bid);
int  open_locate(int handle, DCB *dcb, REC1 *hdr1buf, REC2 *hdr2buf);
int open_load_disp(int handle);
int  eod(int handle);
int  mt_display(char path[], LOADDISP *loaddisp);
int  userexit_input(int handle, int type, char label[5]);
int  userexit_output(int handle, int type);
int  found_input(DCB *dcb, int handle, REC1 *hdr1buf, REC2 *hdr2buf);
int  tiseom(int fildes);
int  userexit(int code, EXITPARM *exitparm);
int  (exlst)(int code, EXITPARM *exitparm);
void cleanup(int handle);
void hdrsfdcb(int handle, DCB *dcb);
int  found_output(int handle, DCB *dcb);
int  rec1fibm(REC1 *rec1, char *bufptr);
int  rec1tibm(REC1 *rec1, char *bufptr);
int  rec2fibm(REC2 *rec2, char *bufptr);
int  rec2tibm(REC2 *rec2, char *bufptr);
int  tdsclose(int handle);
int  tdsopen(int handle, DCB *dcb);
int  tapealloc(VCB *vcb, char *path);
int  tapedealloc(int handle);
int  verify_vol(int handle, VOL1 *vol1buf, REC1 *hdr1buf);
int  vol1fibm(VOL1 *hdr1, char *bufptr);
int  write_to_tape(void);
void usage(void);
#ifdef __TURBOC__
int  _cread(int filedes, char *buffer, int count);
int  twrite(int filedes, char *buffer, int count);
int  _cwrite(int filedes, char *buffer, int count);
int  dbugread(int handle, char *readbuf, int count);
int  dbugwrt(int handle, char *readbuf, int count);
int  load_display(char *msg);
int  mt_bsf(int filedes, int cnt);
int  mt_bsr(int filedes, int cnt);
int  mt_fsf(int fildes, int cnt);
int  mt_locate(int fd, char temp3480,long bid);
int  mt_rew(int filedes, int cnt);
int  mt_weof(int filedes, int cnt);
int  open_quick(int abc);
int  release_tape(char *path);
/*
int  reserve_tape(char *path);
*/
int  tape_io_init (int handle);
int  tape_io_exit (int handle);
int  unload_tape(char *path);
#endif
