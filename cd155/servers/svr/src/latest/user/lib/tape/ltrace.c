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
 *	ltrace.c 11.1 94/03/22 16:49:30
 */
static char	ltrace_ver[] = "@(#) sourcefile ltrace.c 11.1 94/03/22 16:49:30";

/*
 *
 */

#include "stdio.h"
#include <stdarg.h>

#include "type.h"
#include "tapeio.h"
#include <mtio.h>
#include <tape.h>
#include <sys/types.h>
#include "tape3480.h"
#include "extern.h"
#include "string.h"
#include "protos.h"
#include "defines.h"
#include "time.h"
#include <stdio.h>
#include <fcntl.h>
#include "tioproto.h"


void
ltrace(int format, ...) {
  va_list ap;
  va_start(ap, format);


  if (format == 1) {
   /*
    * LTRACE(format, "message",  __FILE__, __LINE__)
    */
    char *message = va_arg(ap, char *);
    LVCB *lvcbptr = va_arg(ap, LVCB *);
    char *file    = va_arg(ap, char *);
    int   line    = va_arg(ap, int);

    if (lvcbptr->debug_msgs != 1) {
      return;
    }
    printf("%s.%d: %s\n",  file, line, message); 

    va_end(ap);
  }


  if (format == 2) {
   /*
    * LTRACE(format, "message", lvcbptr, __FILE__, __LINE__)
    */
    char *message = va_arg(ap, char *);
    LVCB *lvcbptr = va_arg(ap, LVCB *);
    char *file    = va_arg(ap, char *);
    int   line    = va_arg(ap, int);

    if (lvcbptr->debug_msgs != 1) {
      return;
    }
    printf("\n%s.%d: %s\n",  file, line, message); 
    /*-------------------------------------------------
     * LVCB
     --------------------------------------------------*/
/*LVCB:
   xxxxxxxxxxxxxxxxxxxxxxxxxxxxx    path
   0000 0000 0000 0000 0000 0000    cvsqno, ldsqno, read, dsfsm, volfsm, eod
   0000 0000 0000 0000 0000 0000    feov, fd, tindx, loc, writn, tapedev
      0 0000 0000 0000 0000 0000    expire, yy, dd, volcnt, exlst, dbgnounld
   0000 0000 0000 0000              blkmode, blkcnt, buffmd, tapeblk_bufs
*/
printf("LVCB:\n   %s\n", lvcbptr->path);
printf("    %04d %04d %04d %04d %04d %04d    cvsqno, ldsqno, read, dsfsm, volfsm, eod\n",
lvcbptr->cur_volseqno, lvcbptr->last_dsseqno, lvcbptr->read, lvcbptr->dsfsm, 
lvcbptr->dsfsm,lvcbptr->eodhit);

printf("    %04d %04d %04d %04d %04d %04d    feov, fd, tindx, loc, writn, tapedev\n",
lvcbptr->feov, lvcbptr->fildes, lvcbptr->tindex, lvcbptr->located, 
lvcbptr->written, lvcbptr->tapedev);

printf("       %c %04d %04d %04d %04d %04d    createc, yy, dd, volcnt, exlst, dbgnounld\n",
lvcbptr->expirec, lvcbptr->expireyy, lvcbptr->expiredd, lvcbptr->volcount, 
lvcbptr->exlst, lvcbptr->debug_nounload);

printf("    %04d %04d %04d %04d              blkmode, blkcnt, buffmd, tapeblk_bufs\n",
lvcbptr->block_mode, lvcbptr->blockcnt, lvcbptr->buffered_mode, lvcbptr->num_tapeblk_bufs);


/*VOL1:
       volser: xxxxxxxxxxx        vtoc: xxxxxxxxxxx    nameaddr: xxxxxxxxxxx
    reserved2: xxxxxxxxxxxxxxxxxxxx
    reserved3: xxxxxxxxxxxxxxxxxxxxxxxxxxxxx
*/

printf("  VOL1:\n    volser: %-11s        vtoc: %-11s    nameaddr: %-11s\n", 
lvcbptr->vol1.volser, lvcbptr->vol1.vtoc, lvcbptr->vol1.nameaddr);
printf("   reserved2: %-20s\n", lvcbptr->vol1.reservd2);
printf("   reserved3: %-29s\n", lvcbptr->vol1.reservd3);

/*REC1:
       dsname: xxxxxxxxxxxxxxxxx
     sys_code: xxxxxxxxxxxxx
     volseqno: 0000    dsseqno: 0000       dsserno: xxxxxx   version: xx     
      labelid: xxxx   reserved: xxxxxxxx     genno: xxxx     blockcnt: 0000   
     create_c: x     create_yr: 0000       create_dy: 0000
     expire_c: x     expire_yr: 0000       expire_dy: 0000  
*/
printf("  REC1:\n"); 
printf("      dsname: %-17s\n", lvcbptr->rec1.dsname);
printf("    sys_code: %-13s\n", lvcbptr->rec1.system_code);
printf("    volseqno: %04d    dsseqno: %04d       dsserno: %-6s   version: %-2s\n", 
lvcbptr->rec1.volseqno, lvcbptr->rec1.dsseqno, lvcbptr->rec1.dsserno, lvcbptr->rec1.version);
printf("     labelid: %c%c%c%c   reserved: %-7s      genno: %-4s    blockcnt: %04d\n",
lvcbptr->rec1.labelid[0], lvcbptr->rec1.labelid[1], lvcbptr->rec1.labelid[2],
lvcbptr->rec1.labelno, lvcbptr->rec1.reserved, lvcbptr->rec1.genno, 
lvcbptr->rec1.blockcnt);
printf("    create_c: %c     create_yr: %04d     create_dy: %04d\n", 
lvcbptr->rec1.createc, lvcbptr->rec1.createyy, lvcbptr->rec1.createdd); 
printf("    expire_c: %c     expire_yr: %04d     expire_dy: %04d\n", 
lvcbptr->rec1.expirec, lvcbptr->rec1.expireyy, lvcbptr->rec1.expiredd); 


/*REC2:
      labelid: xxxx   blocklen: 0000        reclen: 0000    position: 0000   
     rec_tech: xx        recfm: x          control: x       reserved: x   
     blk_attr: x      checkpnt: x        reserved2: xxxxxxxxx
     jbstepid: xxxxxxxxxxxxxxxx
    reserved3: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  
*/
printf("  REC2:\n");
printf("     labelid: %c%c%c%c   blocklen: %04d        reclen: %04d    position: %04d\n",
lvcbptr->rec2.labelid[0], lvcbptr->rec2.labelid[1], lvcbptr->rec2.labelid[2],
lvcbptr->rec2.labelno, lvcbptr->rec2.blocklen, lvcbptr->rec2.reclen, 
lvcbptr->rec2.position);
printf("    rec_tech: %-2s        recfm: %c          control: %c       reserved: %c\n",
lvcbptr->rec2.rec_tech, lvcbptr->rec2.recfm, lvcbptr->rec2.control, 
lvcbptr->rec2.reserved);
printf("    blk_attr: %c      checkpnt: %c        reserved2: %-8s\n", 
lvcbptr->rec2.blkattr, lvcbptr->rec2.checkpnt, lvcbptr->rec2.reservd2);
printf("    jbstepid: %-18s\n", lvcbptr->rec2.jbstepid);
printf("   reserved3: %-32s\n\n", lvcbptr->rec2.reservd3);


    va_end(ap);
  }
  if (format == 3) {
   /*
    * LTRACE(format, "message", lvcbptr, dcbptr, __FILE__, __LINE__)
    */
    char *message = va_arg(ap, char *);
    LVCB *lvcbptr = va_arg(ap, LVCB *);
    DCB  *dcbptr  = va_arg(ap, DCB*);
    char *file    = va_arg(ap, char *);
    int   line    = va_arg(ap, int);

    if (lvcbptr->debug_msgs != 1) {
      return;
    }
    printf("\n%s.%d: %s\n",  file, line, message); 

    /*DCB:
      xxxxxxxxxxxxxxxxx                dsname
      0000 0000 0000 0000 0000    0    volsqno, dssqno, read,  expireyy, dd, c
      0000 0000 0000 0000 0000    0    reclen, blknum, blklen, bufmd, blkmd, recf
      0000 xxxxxxxxxxxxx x             numtblk, system_code, blkattr
    */
    printf("DCB:\n    %17s                dsname\n", dcbptr->dsname);
    printf("    %04d %04d %04d %04d %04d %4c    volsqno, dssqno, read,  expireyy, dd, c\n",
    dcbptr->volseqno, dcbptr->dsseqno, dcbptr->read,  dcbptr->expireyy, 
    dcbptr->expiredd, dcbptr->expirec);

    printf("    %04d %04d %04d %04d %04d %4c    reclen, blknum, blklen, bufmd, blkmd, recf\n",
    dcbptr->reclen, dcbptr->blocknum, dcbptr->blocklen, dcbptr->buffered_mode, 
    dcbptr->block_mode, dcbptr->recfm);

    printf("    %04d %4c %13s          numtblk, blkattr, system_code\n\n",
    dcbptr->num_tapeblk_bufs, dcbptr->blkattr, dcbptr->system_code);

    }
}
      
