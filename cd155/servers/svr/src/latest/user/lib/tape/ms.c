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
 
/*--------------------------------------------------------------------
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	ms.c 11.1 94/03/22 16:49:31
 */
static char	ms_ver[] = "@(#) sourcefile ms.c 11.1 94/03/22 16:49:31";

/*
 *
 *-----------------------------------------------------------------*/ 
#define	CFS 0

#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include "mtio.h"

#if	CFS
#include "tape.h" 
#endif	/* CFS */

#include "type.h"
#include "tapeio.h"

#include "tape3480.h"
#include "protos.h"
#include <string.h>
#include "defines.h"
#include <fcntl.h>
#include <time.h>
#include "tioproto.h"


  struct mtop s;
  int e = 0;
  char path[129];
  char readbuf[81], wrtbuf[80], getsbuf[129], yesno[2];
  char *varptr, *parmptr;

  int  count=0, i=0, rc=0, num=0, force=0, handle=0, length=0;
  int    fsm=0, eventnum=0, rtnnum=0;
  VOL1 vol1;
  REC1 rec1;
  REC2 rec2;
  struct tm  *timeptr;
  time_t      secsnow; 
	char	*program;			/* program name */
  char volser_buf[25];
  char nameaddr_buf[25];

/**/

/*--------------------------------------------------
 * Event/State Table
 --------------------------------------------------*/
int estab[5][16] ={
/*                   0    1   2    3    4    5    6    7    8    9    10   11   12   13   14   15  */
/*                 xxxx xxxV xxFx xxFV xNxx xNxV xNFx xNFV Pxxx PxxV PxFx PxFV PNxx PNxV PNFx PNFV */
/* event indx      ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- */
/* volser   00 */ { 04,  01,  13,  01,  17,  01,  22,  01,  28,  01,  33,  01,  35,  01,  24,  01},
/* forcertn 01 */ { 05,  07,  01,  01,  18,  20,  01,  01,  29,  31,  01,  01,  36,  25,  01,  01},
/* nameaddr 02 */ { 06,   8,  14,  15,  01,  01,  01,  01,  30,  32,  34,  26,  01,  01,  01,  01},
/* pathrtn  03 */ { 10,  11,  12,  16,  19,  21,  23,  27,  03,  03,  03,  03,  03,  03,  03,  03},
/* eop      04 */ { 02,  37,  02,  37,  02,  37,  02,  37,  02,  37,  02,  37,  02,  37,  02,  37}
}; 
void Usage(void);
int (error1) ();
int (error2) ();
int (error3) ();
int (error4) ();
int (volser) ();
int (nameaddr) ();
int (forcertn) ();
int (pathrtn) ();
int (process) ();
/*--------------------------------------------------
 * Structure containing next state and routine to call
 --------------------------------------------------*/
typedef struct {
  int  nextst;
  int (*routine)();
} STATERTN;

STATERTN rtntab[38] = 
{
/* 00   */  0,  error1,      /* Incorrect number of parms       */
/* 01   */  0,  error2,      /* Ambiguous parameter - xxxx      */    
/* 02   */  0,  error3,      /* VOLSER must be specified        */    
/* 03   */  0,  error4,      /* Invalid operand                 */    
/* 04   */  1,  volser,      /*                                 */   
/* 05   */  2,  forcertn,    /*                                 */   
/* 06   */  4,  nameaddr,    /*                                 */   
/* 07   */  3,  forcertn,    /*                                 */   
/* 08   */  5,  nameaddr,    /*                                 */   
/* 09   */  5,  forcertn,    /*                                 */   
/* 10   */  8,  pathrtn,     /*                                 */   
/* 11   */  9,  pathrtn,     /*                                 */   
/* 12   */ 10,  pathrtn,     /*                                 */   
/* 13   */  3,  volser,      /*                                 */   
/* 14   */  6,  nameaddr,    /*                                 */   
/* 15   */  7,  nameaddr,    /*                                 */   
/* 16   */ 11,  pathrtn,     /*                                 */   
/* 17   */  5,  volser,      /*                                 */   
/* 18   */  6,  forcertn,    /*                                 */   
/* 19   */ 12,  pathrtn,     /*                                 */   
/* 20   */  7,  forcertn,    /*                                 */   
/* 21   */ 13,  pathrtn,     /*                                 */   
/* 22   */  7,  volser,      /*                                 */   
/* 23   */ 14,  pathrtn,     /*                                 */   
/* 24   */ 15,  volser,      /*                                 */   
/* 25   */ 15,  forcertn,    /*                                 */   
/* 26   */ 15,  nameaddr,    /*                                 */   
/* 27   */ 15,  pathrtn,     /*                                 */   
/* 28   */  9,  volser,      /*                                 */   
/* 29   */ 10,  forcertn,    /*                                 */   
/* 30   */ 12,  nameaddr,    /*                                 */   
/* 31   */ 11,  forcertn,    /*                                 */   
/* 32   */ 13,  nameaddr,    /*                                 */   
/* 33   */ 11,  volser,      /*                                 */   
/* 34   */ 14,  nameaddr,    /*                                 */   
/* 35   */ 13,  volser,      /*                                 */   
/* 36   */ 14,  forcertn,    /*                                 */   
/* 37   */ 99,  process      /*                               */   
};                                                              
/**/

	int ac;
	char **av;
main(acorig, avorig)
  int acorig;
  char **avorig;
{
  ac = acorig;
  av = avorig;
  memset (&vol1, 0x00, sizeof(vol1));
  memset (&rec1, 0x00, sizeof(rec1));
  memset (&rec2, 0x00, sizeof(rec2));
  time(&secsnow);
  timeptr = localtime(&secsnow);

  /*--------------------------------------------------------------------
   *  Deal with command line arguments.  Set defaults.
   *--------------------------------------------------------------------*/ 
  force = 0;
  memset(&nameaddr_buf[0],' ', (sizeof(nameaddr_buf)-1) );
  memset(&nameaddr_buf[(sizeof(nameaddr_buf)-1)], '\0', 1);
  strcpy(path, "/cfs/ctape0");

	program = *av;					/* First argument is program name . . . */
	av++;
	ac--;

  while (fsm != 99) {
    if (*av == NULL) {          /* VOLSER is next */
      eventnum = 4;
    }
    else if (strncmp(*av, "-", 1) == 0) { 
      if (strncmp(*av, "-v", 2) == 0) {              /* VOLSER */
        eventnum = 0; 
      }
      else if (strncmp(*av, "-f", 2) == 0) {         /* Force */
        eventnum = 1; 
      }
      else if (strncmp(*av, "-n", 2) == 0) {         /* Nameaddr */
        eventnum = 2; 
      }
      else {
        printf("Unknown parameter - %s\n", *av);
        exit(1);
      }
    }
    else {                                           /* Path */
      eventnum = 3;
    }

    rtnnum = estab[eventnum][fsm];     /* index into next state/routine table*/
    rc =     rtntab[rtnnum].routine(); /* call routine                    */
    fsm =    rtntab[rtnnum].nextst;
    ac--;
    av++;
  }  
  return(0);
}  
/**/
/*--------------------------------------------------
 * Volser
 *-------------------------------------------------*/
int volser()
{
  if (strlen(*av) != 2)  {          /* No space in between -v and volser */
    parmptr=*av;
    parmptr+=2;
    strcpy(volser_buf, parmptr);
	}
  else {                            /* next parm should be volser */
    ac--;
    av++;
    if (*av == NULL) { 
      fprintf(stderr, "VOLSER must be specified\n");
      Usage();
      exit(1);
    }
    if (strncmp(*av, "-", 1) == 0) { /* Shouldn't be */
      fprintf(stderr, "Ambiguous parameter - %s: \n", *av);
      Usage();
      exit(1);
    }
    else {
      strcpy(volser_buf, *av);
    }
	}

  if ( strlen(volser_buf) < sizeof(vol1.volser) ) {
    memset(&volser_buf[strlen(volser_buf)],' ', 
      ( (sizeof(vol1.volser))-(strlen(volser_buf)) ) );
    memset(&volser_buf[(sizeof(vol1.volser)-1)], '\0', 1);
  }
  else {
    fprintf(stderr, "%s: Length of VOLSER argument too long.  Truncating ...\n", program);
    memset(&volser_buf[(sizeof(vol1.volser)-1)], '\0', 1);
  }
  length = strlen(volser_buf);
  for (i=0; i < length; i++) {
    volser_buf[i] = toupper(volser_buf[i]);
  }
  return(0);
}  
/**/
/*--------------------------------------------------
 * Nameaddr
 *-------------------------------------------------*/
int nameaddr()
{    
  if (strlen(*av) != 2)  {          /* No space in between -n and nameaddr */
    parmptr=*av;
    parmptr+=2;
    strcpy(nameaddr_buf, parmptr);
	}
  else {                            
    ac--;
    av++;
    if (*av == NULL) { 
      fprintf(stderr, "nameaddr must be specified if -n is specified\n");
      Usage();
      exit(1);
    }
    if (strncmp(*av, "-", 1) == 0) { /* Shouldn't be */
      fprintf(stderr, "Ambiguous parameter - %s: \n", *av);
      Usage();
      exit(1);
    }
    else {
      strcpy(nameaddr_buf, *av);
    }
	}

  if ( strlen(nameaddr_buf) < sizeof(vol1.nameaddr) ) {
    memset(&nameaddr_buf[strlen(nameaddr_buf)],' ', 
      ( (sizeof(vol1.nameaddr))-(strlen(nameaddr_buf)) ) );
    memset(&nameaddr_buf[(sizeof(vol1.nameaddr)-1)], '\0', 1);
  }
  else {
    fprintf(stderr, "%s: Length of NAMEADDR argument too long.  Truncating ...\n", program);
    memset(&nameaddr_buf[(sizeof(vol1.nameaddr)-1)], '\0', 1);
  }
  length = strlen(nameaddr_buf);
  for (i=0; i < length; i++) {
    nameaddr_buf[i] = toupper(nameaddr_buf[i]);
  }
  return(0);
}    
/**/
/*--------------------------------------------------
 * Force
 *-------------------------------------------------*/
int forcertn()
{    
    force = 1;
    return(0);
}
/**/
/*--------------------------------------------------
 * Path
 *-------------------------------------------------*/
int pathrtn()
{    
    strncpy(path, *av, 80);
    return(0);
}
/**/
/*--------------------------------------------------
 * Error1
 *-------------------------------------------------*/
int error1()
{    
      fprintf(stderr, "Incorrect Number of parameters\n");
      Usage();
      exit(1);
}
/*--------------------------------------------------
 * Error2
 *-------------------------------------------------*/
int error2()
{    
      fprintf(stderr, "Ambiguous parameter   %s\n", *av);
      Usage();
      exit(1);
}
/*--------------------------------------------------
 * Error3
 *-------------------------------------------------*/
int error3()
{    
      fprintf(stderr, "VOLSER must be specified\n");
      Usage();
      exit(1);
}
/*--------------------------------------------------
 * Error4
 *-------------------------------------------------*/
int error4()
{    
      fprintf(stderr, "Invalid operand  %s\n", *av);
      Usage();
      exit(1);
}
/*PAGE*/
/*--------------------------------------------------
 * Process MS
 *-------------------------------------------------*/
int process()
{    
  /*--------------------------------------------------------------------
   * Open the device.
   *--------------------------------------------------------------------*/ 
  for (i=0; (((handle = open(path, O_RDWR)) == -1 ) && (i<3)); i++) {
      }
  if (handle == -1) {
    perror(path);
    printf("Device cannot be opened.\n");
    return(0);
  }
  else printf("\nReading Tape ...");

  /*--------------------------------------------------------------------
   * Read first record.  Verify VOL1.
   *--------------------------------------------------------------------*/ 
  count = read(handle, readbuf, sizeof(readbuf));
  if (count == 80) { 
    stretoa(&readbuf[0],  vol1.labelid,   3);    
    stretoa(&readbuf[3],  &vol1.labelno,   1);    
    stretoa(&readbuf[4],  vol1.volser,    6);     
    stretoa(&readbuf[41], vol1.nameaddr, 10); 
    if ( (strcmp(vol1.labelid, "VOL") != 0) ||
          (vol1.labelno != '1')) {
      printf("\nInvalid VOL1 record found.  Rewriting tape ...");
      rc = write_to_tape();
      return(0);
    }
  }
  else {
    printf("\nInvalid VOL1 record found.  Rewriting tape ...");
    rc = write_to_tape();
    return(0);
  }
  
  /*----------------------------------------------------------------
    * Read second record.  Verify HDR1.
    *----------------------------------------------------------------*/ 
  count = read(handle, readbuf, sizeof(readbuf));
  if (count == 80) {
    stretoa(&readbuf[0],   rec1.labelid,      3); 
    stretoa(&readbuf[3],   &rec1.labelno,      1);  
    stretoa(&readbuf[4],   rec1.dsname,      17); 
    stretoa(&readbuf[47],  &rec1.expirec,      1);      
    ztoi(   &readbuf[48], &rec1.expireyy,     2);     
    ztoi(   &readbuf[50], &rec1.expiredd,     3);     
    stretoa(&readbuf[73], rec1.reserved,     7);     
    if ( (strcmp(rec1.labelid, "HDR") != 0) ||
          (rec1.labelno != '1')) {
      printf("\nInvalid HDR1 record found.  Rewriting tape ...");
      rc = write_to_tape();
      return(0);
    }
  }
  else {
    printf("\nInvalid HDR1 record found.  Rewriting tape ...");
    rc = write_to_tape();
    return(0);
  }

  /*------------------------------------------------------------
    * Read third record.  Verify HDR2.
    *------------------------------------------------------------*/ 
  count = read(handle, readbuf, sizeof(readbuf));
  if (count == 80) {
    stretoa(&readbuf[0],   rec2.labelid,     3);
    stretoa(&readbuf[3],   &rec2.labelno,     1);
    if ( (strcmp(rec2.labelid, "HDR")!= 0) ||
          (rec2.labelno != '2')) {
      printf("\nInvalid HDR2 record found.  Rewriting tape ...");
      rc = write_to_tape();
      return(0);
    }
  }
  else {
    if (count == 0) {
      rc = write_to_tape();
      return(0);
    }
    else {
      printf("\nInvalid HDR2 record found.  Rewriting tape ...");
      rc = write_to_tape();
      return(0);
    }
  }


  /*--------------------------------------------------------
    * Tape is labeled (correctly).  If date has not expired, cannot
    * overwrite.  If date is OK, ask for verification before
    * overwriting.
    *--------------------------------------------------------*/ 
  if (rec1.expirec == ' ')
    i = 0;
  else i = 100 + (100 * atoi(&rec1.expirec));
  if ( ((rec1.expireyy + i) > timeptr->tm_year) ||
      (((rec1.expireyy + i) == timeptr->tm_year) &&
        (rec1.expiredd > timeptr->tm_yday)) ) {
        printf("\nExpiration Date has not expired !");
  }
  else {
    printf("\n\nExisting Headers found on tape:");
  }
  printf("\n  Volume Serial       = %s", vol1.volser);
  printf("\n  Name/Address Code   = %s", vol1.nameaddr);
  printf("\n  First Data Set Name = %s", rec1.dsname);
  if ( force == 1) {
    printf("\n\nForce option specified.  Rewriting tape.");
    rc = write_to_tape();
    return(0);
  }
  else {
    strcpy(yesno, " ");
    while ( (strcmp(yesno, "y") != 0) &&
            (strcmp(yesno, "Y") != 0) &&
            (strcmp(yesno, "n") != 0) &&
            (strcmp(yesno, "N") != 0)   ) {
        printf("\n\nAre you sure you want to erase this tape ? (y/n)");
        gets(&getsbuf[0]);
        if ( strlen(getsbuf) < sizeof(yesno) ) {
          strcpy(&yesno[0], &getsbuf[0]);
        }
       else {
          memcpy(&yesno[0], &getsbuf[0], (sizeof(yesno)-1));
          memset(&yesno[(sizeof(yesno)-1)], '\0', 1);
       }
    }
    if ( (strcmp(yesno, "y") == 0) ||
          (strcmp(yesno, "Y") == 0) ) {
        rc = write_to_tape();
        return(0);
    }
    else {
      /*----------------------------------------------------------
       * Answered "NO"
       *----------------------------------------------------------*/ 
      close(handle);
      printf("\nProcess Aborted\n");
      return(0);
    }
  }
}

/**/
int
write_to_tape(void)
  {
    printf("\nWriting New Volume Labels ...");

    s.mt_op = MTREW;
    s.mt_count = 1;
    if (ioctl(handle, MTIOCTOP, &s) == -1) {
      perror(path);
    return(-1);
    }

    strcpy(&vol1.volser[0], &volser_buf[0]);
    strcpy(&vol1.nameaddr[0], &nameaddr_buf[0]);

    strcpy(vol1.labelid, "VOL");
    vol1.labelno = '1';
    vol1.reserved = ' ';
    memset(vol1.vtoc, ' ', (sizeof(vol1.vtoc)-1));
    memset(vol1.reservd2, ' ', (sizeof(vol1.reservd2)-1));
    memset(vol1.reservd3, ' ', (sizeof(vol1.reservd3)-1));
    memset(&vol1.vtoc[(sizeof(vol1.vtoc)-1)], '\0', 1);
    memset(&vol1.reservd2[(sizeof(vol1.reservd2)-1)], '\0', 1);
    memset(&vol1.reservd3[(sizeof(vol1.reservd3)-1)], '\0', 1);

    stratoe(vol1.labelid,   &wrtbuf[0]);    
    stratoe(&vol1.labelno,   &wrtbuf[3]);    
    stratoe(vol1.volser,    &wrtbuf[4]);     
    stratoe(&vol1.reserved,  &wrtbuf[10]);  
    stratoe(vol1.vtoc,      &wrtbuf[11]);     
    stratoe(vol1.reservd2,  &wrtbuf[21]); 
    stratoe(vol1.nameaddr,  &wrtbuf[41]); 
    stratoe(vol1.reservd3,  &wrtbuf[51]); 
    rc =   write(handle, wrtbuf, sizeof(wrtbuf));

    /*------------------------------------------------------
      * Only care about Label Id and Label Number -
      * A scratch tape, by definition, is VOL1, HDR1, TM
      * (No HDR2)
      *------------------------------------------------------*/ 
    strcpy(rec1.labelid, "HDR");
    rec1.labelno = '1';
    memset(rec1.dsname, ' ',  (sizeof(rec1.dsname)-1));
    memset(rec1.genno, '0',   (sizeof(rec1.genno)-1));
    memset(rec1.version, '0', (sizeof(rec1.version)-1));
    rec1.expirec = ' ';
    memset(rec1.system_code, ' ', (sizeof(rec1.system_code)-1));
    memset(rec1.reserved, ' ', (sizeof(rec1.reserved)-1));
    memset(&rec1.dsname[(sizeof(rec1.dsname)-1)], '\0', 1);
    memset(&rec1.genno[(sizeof(rec1.genno)-1)], '\0', 1);
    memset(&rec1.version[(sizeof(rec1.version)-1)], '\0', 1);
    memset(&rec1.system_code[(sizeof(rec1.system_code)-1)], '\0', 1);
    memset(&rec1.reserved[(sizeof(rec1.reserved)-1)], '\0', 1);

    if (timeptr->tm_year <= 99)
      rec1.createc = ' ';
    else {                                           
      i = 1;
      uitoa(i, &rec1.createc);
    }

    rec1.createyy = timeptr->tm_year;
    rec1.createdd = timeptr->tm_yday + 1; /* IBM format has the day of the
                                             year from 1-366, UNIX has it
                                             from 0-365                  */
    strcpy(rec1.dsserno, "000000");
    rec1.volseqno = 0;
    rec1.dsseqno =  1;
    rec1.expireyy = 0;    
    rec1.expiredd = 0;    
    rec1.security = 0;
    rec1.blockcnt = 0;


    stratoe( &rec1.labelid[0],     &wrtbuf[0]  ); 
    stratoe( &rec1.labelno,     &wrtbuf[3]  );  
    stratoe( &rec1.dsname[0],      &wrtbuf[4]  ); 
    stratoe( &rec1.dsserno[0],     &wrtbuf[21] );      
    itoz(     rec1.volseqno,       &wrtbuf[27], 4 );     
    itoz(     rec1.dsseqno,        &wrtbuf[31], 4 );      
    stratoe( &rec1.genno[0],       &wrtbuf[35] );        
    stratoe( &rec1.version[0],     &wrtbuf[39] );      
    stratoe( &rec1.createc,     &wrtbuf[41] );      
    itoz(     rec1.createyy,       &wrtbuf[42], 2 );     
    itoz(     rec1.createdd,       &wrtbuf[44], 3 );     
    stratoe( &rec1.expirec,     &wrtbuf[47] );      
    itoz(     rec1.expireyy,       &wrtbuf[48], 2 );     
    itoz(     rec1.expiredd,       &wrtbuf[50], 3 );     
    itoz(     rec1.security,       &wrtbuf[53], 1 );     
    itoz(     rec1.blockcnt,       &wrtbuf[54], 6 );     
    stratoe( &rec1.system_code[0], &wrtbuf[60] ); 
    stratoe( &rec1.reserved[0],    &wrtbuf[73] );     
    rc =   write(handle, wrtbuf, sizeof(wrtbuf));


    s.mt_op = MTWEOF;
    s.mt_count = 1;
    if (ioctl(handle, MTIOCTOP, &s) == -1) {
      perror(path);
    return(-1);
    }

    close(handle);
    printf("\nScratch Tape Creation Successful !\n");
    return(0);

  }

/*PAGE*/
void Usage(void) {
  fprintf(stderr, "Usage: ms -vvolser [-nnameaddr] [-f] [tapedevice]\n");
  fprintf(stderr, "\t-vvolser     Volume serial number to be stored on the tape - REQUIRED\n");
  fprintf(stderr, "\t-nnameaddr   Name/Address information - Optional\n");
  fprintf(stderr, "\t-f           Forces the tape to be overwritten - Optional\n");
  fprintf(stderr, "\ttapedevice   CFS tape device which supports IBM-format tapes - Optional\n");
}
