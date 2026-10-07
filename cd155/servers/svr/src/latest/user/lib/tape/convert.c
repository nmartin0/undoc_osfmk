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
 *	convert.c 11.1 94/03/22 16:49:19
 */
static char	convert_ver[] = "@(#) sourcefile convert.c 11.1 94/03/22 16:49:19";

/*
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <type.h>
#include "tapeio.h"
#include "tape3480.h"
#include "extern.h"
#include "protos.h"
#include "tioproto.h"
#include <string.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include "defines.h"

/*
extern void time();
*/

/*
 * Convert VOL1 from IBM format
 */
vol1fibm(VOL1 *vol1, char *bufptr)
{
  stretoa(&bufptr[0],  &vol1->labelid[0],   3);    
  stretoa(&bufptr[3],  &vol1->labelno,      1);    
  stretoa(&bufptr[4],  &vol1->volser[0],    6);     
  stretoa(&bufptr[10], &vol1->reserved,     1);  
  stretoa(&bufptr[11], &vol1->vtoc[0],     10);     
  stretoa(&bufptr[21], &vol1->reservd2[0], 20); 
  stretoa(&bufptr[41], &vol1->nameaddr[0], 10); 
  stretoa(&bufptr[51], &vol1->reservd3[0], 29); 
  return(0);
}

/*
 * Convert VOL1 to IBM format
 */
vol1tibm(VOL1 *vol1, char *bufptr)
{
  stratoe( &vol1->labelid[0],   &bufptr[0]);    
  stratoe( &vol1->labelno,      &bufptr[3]);    
  stratoe( &vol1->volser[0],    &bufptr[4]);     
  stratoe( &vol1->reserved,     &bufptr[10]);  
  stratoe( &vol1->vtoc[0],      &bufptr[11]);     
  stratoe( &vol1->reservd2[0],  &bufptr[21]); 
  stratoe( &vol1->nameaddr[0],  &bufptr[41]); 
  stratoe( &vol1->reservd3[0],  &bufptr[51]); 
  return(0);
}

/*
 * Convert REC1 from IBM format
 */
int
rec1fibm(rec1, bufptr)
	REC1 *rec1;
	char *bufptr;
{
  stretoa(&bufptr[0],  &rec1->labelid[0],      3); 
  stretoa(&bufptr[3],  &rec1->labelno,         1);  
  stretoa(&bufptr[4],  &rec1->dsname[0],      17); 
  stretoa(&bufptr[21], &rec1->dsserno[0],      6);      
  ztoi(   &bufptr[27],          &rec1->volseqno,     4);     
  ztoi(   &bufptr[31],          &rec1->dsseqno,      4);      
  stretoa(&bufptr[35],  &rec1->genno[0],        4);        
  stretoa(&bufptr[39],  &rec1->version[0],      2);      
  stretoa(&bufptr[41],  &rec1->createc,         1);      
  ztoi(   &bufptr[42],          &rec1->createyy,     2);     
  ztoi(   &bufptr[44],          &rec1->createdd,     3);     
  stretoa(&bufptr[47],  &rec1->expirec,         1);      
  ztoi(   &bufptr[48],          &rec1->expireyy,     2);     
  ztoi(   &bufptr[50],          &rec1->expiredd,     3);     
  ztoi(   &bufptr[53],          &rec1->security,     1);     
  ztoi(   &bufptr[54],          &rec1->blockcnt,     6);     
  stretoa(&bufptr[60],  &rec1->system_code[0], 13); 
  stretoa(&bufptr[73],  &rec1->reserved[0],     7);     
  return(0);
}                                              

/*
 * Convert REC1 to IBM format
 */
rec1tibm(REC1 *rec1, char *bufptr)
{
  stratoe( &rec1->labelid[0],     &bufptr[0]  ); 
  stratoe( &rec1->labelno,        &bufptr[3]  );  
  stratoe( &rec1->dsname[0],      &bufptr[4]  ); 
  stratoe( &rec1->dsserno[0],     &bufptr[21] );      
  itoz(     rec1->volseqno,       &bufptr[27], 4 );     
  itoz(     rec1->dsseqno,        &bufptr[31], 4 );      
  stratoe( &rec1->genno[0],       &bufptr[35] );        
  stratoe( &rec1->version[0],     &bufptr[39] );      
  stratoe( &rec1->createc,        &bufptr[41] );      
  itoz(     rec1->createyy,       &bufptr[42], 2 );     
  itoz(     rec1->createdd,       &bufptr[44], 3 );     
  stratoe( &rec1->expirec,        &bufptr[47] );      
  itoz(     rec1->expireyy,       &bufptr[48], 2 );     
  itoz(     rec1->expiredd,       &bufptr[50], 3 );     
  itoz(     rec1->security,       &bufptr[53], 1 );     
  itoz(     rec1->blockcnt,       &bufptr[54], 6 );     
  stratoe( &rec1->system_code[0], &bufptr[60] ); 
  stratoe( &rec1->reserved[0],    &bufptr[73] );     
  return(0);
}


/*
 * Convert REC2 from IBM format
 */
rec2fibm(REC2 *rec2, char *bufptr)
{                                                  
  stretoa(&bufptr[0],    &rec2->labelid[0],     3);
  stretoa(&bufptr[3],    &rec2->labelno,        1);
  stretoa(&bufptr[4],    &rec2->recfm,          1);
  ztoi   (&bufptr[5],    &rec2->blocklen,       5);
  ztoi   (&bufptr[10],   &rec2->reclen,         5);
  stretoa(&bufptr[15],   &rec2->density,        1);
  ztoi   (&bufptr[16],   &rec2->position,       1);
  stretoa(&bufptr[17],   &rec2->jbstepid[0],   17);
  stretoa(&bufptr[34],   &rec2->rec_tech[0],    2);
  stretoa(&bufptr[36],   &rec2->control,        1);
  stretoa(&bufptr[37],   &rec2->reserved,       1);
  stretoa(&bufptr[38],   &rec2->blkattr,        1);
  stretoa(&bufptr[39],   &rec2->reservd2[0],    8);
  stretoa(&bufptr[47],   &rec2->checkpnt,       1);
  stretoa(&bufptr[48],   &rec2->reservd3[0],   32);
  return(0);
}


/*
 * Convert REC2 to IBM format
 */
rec2tibm(REC2 *rec2, char *bufptr)
{
  stratoe( &rec2->labelid[0],   &bufptr[0] );
  stratoe( &rec2->labelno,      &bufptr[3] ); 
  stratoe( &rec2->recfm,        &bufptr[4] ); 
  itoz   (  rec2->blocklen,     &bufptr[5] , 5); 
  itoz   (  rec2->reclen,       &bufptr[10], 5); 
  stratoe( &rec2->density,      &bufptr[15]); 
  itoz   (  rec2->position,     &bufptr[16], 1); 
  stratoe( &rec2->jbstepid[0],  &bufptr[17]); 
  stratoe( &rec2->rec_tech[0],  &bufptr[34]); 
  stratoe( &rec2->control,      &bufptr[36]); 
  stratoe( &rec2->reserved,     &bufptr[37]); 
  stratoe( &rec2->blkattr,      &bufptr[38]); 
  stratoe( &rec2->reservd2[0],  &bufptr[39]); 
  stratoe( &rec2->checkpnt,     &bufptr[47]); 
  stratoe( &rec2->reservd3[0],  &bufptr[48]); 
  return(0);
}



/*
 * 
 * Generate LVCB.REC1 and LVCB.REC2 for output (Headers-from-DCB)
 * 
 */
void
hdrsfdcb(int handle, DCB *dcb)
{
	LVCB	*lvcbptr;

	struct	tm	*timeptr;
	time_t		timer; 

	lvcbptr = tapemain.lvcblist[handle];

   /*-------------------------------------------------------------------
    * Generate HDR1 defaults
    *-------------------------------------------------------------------*/ 
	timer = 8 * 60 * 60;
	timeptr = localtime(&timer);

  strcpy(lvcbptr->rec1.labelid,   "HDR");
         lvcbptr->rec1.labelno =  '1';
  strcpy(lvcbptr->rec1.genno,     "    ");
  strcpy(lvcbptr->rec1.version,   "  ");


	if (timeptr->tm_year < 99) {
		lvcbptr->rec1.createc = ' ';
	}
	else {                                           
		lvcbptr->rec1.createc = '1';
	}
         lvcbptr->rec1.createyy = timeptr->tm_year % 100;
         lvcbptr->rec1.createdd = timeptr->tm_yday + 1;
         lvcbptr->rec1.security = 0;
         lvcbptr->rec1.blockcnt = 0;
  memset(lvcbptr->rec1.system_code, ' ', sizeof(lvcbptr->rec1.system_code));
  memset(&lvcbptr->rec1.system_code[sizeof(lvcbptr->rec1.system_code)-1],  0x00, 1);
  memset(lvcbptr->rec1.reserved, ' ', sizeof(lvcbptr->rec1.reserved));
  memset(&lvcbptr->rec1.reserved[sizeof(lvcbptr->rec1.reserved)-1],  0x00, 1);
  strcpy(lvcbptr->rec1.dsserno,   lvcbptr->vollist[lvcbptr->cur_volseqno].serialno);

   /*-------------------------------------------------------------------
    * Build HDR1 from DCB 
    *-------------------------------------------------------------------*/ 
  strcpy(lvcbptr->rec1.dsname,    dcb->dsname);
         lvcbptr->rec1.volseqno = dcb->volseqno;
         lvcbptr->rec1.dsseqno  = dcb->dsseqno;
      if(lvcbptr->rec1.dsseqno == 0) {
          lvcbptr->rec1.dsseqno  = 1;
      }
         lvcbptr->rec1.expirec =  dcb->expirec;
         lvcbptr->rec1.expireyy = dcb->expireyy;
         lvcbptr->rec1.expiredd = dcb->expiredd;
  strcpy(lvcbptr->rec1.system_code, dcb->system_code);

         lvcbptr->expirec =  dcb->expirec;
         lvcbptr->expireyy = dcb->expireyy;
         lvcbptr->expiredd = dcb->expiredd;

   /*-------------------------------------------------------------------
    * Generate HDR2 defaults
    *-------------------------------------------------------------------*/ 
  strcpy(lvcbptr->rec2.labelid,   "HDR");
         lvcbptr->rec2.labelno  = '2';
         lvcbptr->rec2.density  = ' ';
         lvcbptr->rec2.position = 0;
  memset(lvcbptr->rec2.jbstepid, ' ', sizeof(lvcbptr->rec2.jbstepid));
  memset(&lvcbptr->rec2.jbstepid[sizeof(lvcbptr->rec2.jbstepid)-1],  0x00, 1);
  strcpy(lvcbptr->rec2.rec_tech,  "  ");
         lvcbptr->rec2.control  = ' ';
         lvcbptr->rec2.reserved  = ' ';
         lvcbptr->rec2.checkpnt  = ' ';
  memset(lvcbptr->rec2.reservd2,  ' ', sizeof(lvcbptr->rec2.reservd2));
  memset(&lvcbptr->rec2.reservd2[sizeof(lvcbptr->rec2.reservd2)-1],  0x00, 1);
  memset(lvcbptr->rec2.reservd3,  ' ', sizeof(lvcbptr->rec2.reservd3));
  memset(&lvcbptr->rec2.reservd3[sizeof(lvcbptr->rec2.reservd3)-1],  0x00, 1);

   /*-------------------------------------------------------------------
    * build HDR2 from DCB 
    *-------------------------------------------------------------------*/ 
         lvcbptr->rec2.recfm    = dcb->recfm;
         lvcbptr->rec2.blocklen = dcb->blocklen;
         lvcbptr->rec2.reclen =   dcb->reclen;
         lvcbptr->rec2.blkattr  = dcb->blkattr;
}
