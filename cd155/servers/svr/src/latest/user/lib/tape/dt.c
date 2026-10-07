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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license agreement
 *  or nondisclosure agreement with Intel Corporation and may not be
 *  copied nor disclosed except in accordance with the terms of that
 *  agreement.
 *
 *	dt.c 11.1 94/03/22 16:49:22
 */
static char	dt_ver[] = "@(#) sourcefile dt.c 11.1 94/03/22 16:49:22";

/*
 */

/*------------------------------------------------------------------
 *  dt.c
 *
 *   Dump an IBM-format tape.
 *
 *  Usage: dt [ -f | -F ] [ -v [size] ] [ -b ] [ tapedevice ]
 *
 *  History:
 *  10-30-91   g t  Created
 *
 *-----------------------------------------------------------------*/
#define TAPEDEV "/cfs/ctape0"
#define COUNT (32 * 1024) 
#define DEBUG 0
#define CFS 0

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include "mtio.h"
#include <sys/types.h>
#include <nodedef.h>
#include <msgtype.h>
#include <pid.h>

#include <cfs.h>
#if	CFS
#include <diskproc.h>
#include <nameproc.h>
#endif	/* CFS */

#include <fio.h>
#include <iod.h>
#include <scsi.h>
#include "type.h"
#include "tapeio.h"

Usage() {
    fprintf(stderr,"Usage: dt [-f] [-v [size]] [-b] [tapedevice]\n");
    fprintf(stderr, "\t-f          formatted headers/labels (default is HEX display)\n");
    fprintf(stderr, "\t-v [size]   verbose display of size bytes of each data block\n");
    fprintf(stderr, "\t            (default size is 0 bytes)\n");
    fprintf(stderr, "\t-b          print hardware blockid of each block\n");
    fprintf(stderr, "\ttapedevice  cfs tape device (default is %s)\n", TAPEDEV);
}

#if	CFS
void cleareof(int fd) {
    fds[fd].fs_pid &= ~FEOF;
}
#endif	/* CFS */


main(ac, av)
    int ac;
    char **av;

{
    char *sizestr,             /* pointers used to parse arguments */
	         *endstr;            
    int  i,j,k;                /* ICV */
    char tape_dev[80];         /* name of tape device */
    int  fd;                   /* fildes for open tape device */
    int  rc;                   /* return code */
    int  num,                  /* number of bytes for this read */
         total,                /* total bytes on this tape */
         subtotal,             /* bytes read since last tape mark */
	 subblock;             /* blocks read since last tape mark */
    unsigned char  *ptr,       /* malloc buffer */
                   *prptr1,    /* char at a time thru buffer */
                   *prptr2;    /* line at a time thru buffer */
    unsigned char asciilabel[80]; /* HEADER or TRAILER in ascii */
    char tm,                 /* number of consecutive tape marks */
                             /* tm=1 indicates tape mode switch */
                             /* tm=2 indicates end of tape */
                             /* tm=3 indicates VOL switch */
         eod,                /* indicates just left DATA state */
         userlab,            /* =1 indicates UHL or UTL to dump */
         errorlab;     /* 1 indicates fatal error on label to dump */
    char *program;           /* program name */

#if	CFS
	MT3480_ID	ps;	/* position structure */
#else
	POSITION	ps;	/* position structure */
#endif	/* CFS */

/* Tape Modes: */
#define HEADER      1
#define DATA        2
#define TRAILER     3

struct state   {       /* State machine for HEADER/TRAILER modes: */
    char *label;
    char data_state;
    char tm_state;
     }    statetab[] = {
/*
state   this label next   next
        should     data    tm
          be       state  state        0x7F is the fatal error state */
/*  0*/ "VOL1",      1,    0x7F,
/*  1*/ "HDR1",      2,    0x7F, /* SPECIAL STATE - double tape mark ok */ 
/*  2*/ "HDR2",      3,    0x7F, /* SPECIAL STATE - scratch tape */
/*  3*/ "UHL1",      4,    12,
/*  4*/ "UHL2",      5,    12,
/*  5*/ "UHL3",      6,    12,
/*  6*/ "UHL4",      7,    12,
/*  7*/ "UHL5",      8,    12,
/*  8*/ "UHL6",      9,    12,
/*  9*/ "UHL7",      10,   12,
/* 10*/ "UHL8",      11,   12,
/* 11*/ "tape mark", 0x7F, 12,
          /* data blocks expected here */
/* 12*/ "EOF1",      13,   0x7F,     /* SPECIAL STATE - "EOV1" ok */
/* 13*/ "EOF2",      14,   0x7F,
/* 14*/ "UTL1",      15,   1,
/* 15*/ "UTL2",      16,   1,
/* 16*/ "UTL3",      17,   1,
/* 17*/ "UTL4",      18,   1,
/* 18*/ "UTL5",      19,   1,
/* 19*/ "UTL6",      20,   1,
/* 20*/ "UTL7",      21,   1,
/* 21*/ "UTL8",      22,   1,
/* 22*/ "tape mark", 0x7F, 1,

/* 23*/ "EOV1",      24,   0x7F,
/* 24*/ "EOV2",      25,   0x7F,
/* 25*/ "UTL1",      26,   0,
/* 26*/ "UTL2",      27,   0,
/* 27*/ "UTL3",      28,   0,
/* 28*/ "UTL4",      29,   0,
/* 29*/ "UTL5",      30,   0,
/* 30*/ "UTL6",      31,   0,
/* 31*/ "UTL7",      32,   0,
/* 32*/ "UTL8",      33,   0,
/* 33*/ "tape mark",0x7F,  0
};

/* Verbose Modes: */
#define NOVERBOSE   0
#define VERBOSE     1

/* Format Modes: */
#define NOFORMAT    0
#define FORMAT      1
#define XFORMAT     2

/* Blockid Modes: */
#define NOBLOCK     0
#define BLOCK       1

    char tapemode,
         labelstate,
         verbose,       /* state of -v switch */
         format,        /* state of -f,-F switch */
         blockid;       /* state of -b switch */
    int  vsize,         /* size from verbose switch */
         prtsize;       /* MIN vsize, num */

    /*
    **   Deal with command line arguments.
    */
    program = *av;        /* First argument is program name... */
    av++;
    ac--;

    if (ac > 5) {
        fprintf(stderr, "%s: incorrect number of arguments = %d\n", 
			program, ac);
        Usage();
        exit(1);
     }

     /*
     **   Set default values.
     */
     format = NOFORMAT;		/* Default is no format */
     verbose = NOVERBOSE;	/* Default is not verbose */
     vsize = 0;			/* Default for -v is 0 */
     blockid = NOBLOCK;		/* Default is no blockid */
     tape_dev[0] = NULL;	/* Default device supplied at end */

     while (ac > 0) {
     vsize_kludge:
          /*
          **   Format [-f | -F]
          */
          if (strcmp(*av, "-f") == 0) {
               if (format != NOFORMAT) {
                    fprintf(stderr, "%s: incorrect arguments\n", 
					program);
                    Usage();
                    exit(1);
               }
               format = FORMAT;
          }
          else if (strcmp(*av, "-F") == 0) {
               if (format != NOFORMAT) {
                    fprintf(stderr, "%s: incorrect arguments\n", 
					program);
                    Usage();
                    exit(1);
               }
               format = XFORMAT;
          }

          /*
          **   Verbose [-v[size]]
          */
          else if (strncmp(*av, "-v", 2) == 0) {
               if (verbose != NOVERBOSE) {
                    fprintf(stderr, "%s: incorrect arguments\n", 
					program);
                    Usage();
                    exit(1);
               }
               verbose = VERBOSE;
               sizestr=*av;
               sizestr+=2;
               if ((*sizestr==NULL) && (ac > 1)) {
                    ac--;
                    av++;
                    sizestr=*av;
                    vsize = (int)strtol(sizestr, &endstr, 10);
                    if (endstr[0] != 0) {
                         vsize=0;
                         goto vsize_kludge;
                    }
               } else {
                    vsize = (int)strtol(sizestr, &endstr, 10);
                    if (endstr[0] != 0) {
                         fprintf(stderr, "%s: incorrect -v size argument = %s\n", program, *av);
                         Usage();
                         exit(1);
                    }
               }
          }

          /*
          **   Blockid [-b]
          */
          else if (strcmp(*av, "-b") == 0) {
               if (blockid != NOBLOCK) {
                    fprintf(stderr, "%s: incorrect arguments\n", 
				program);
                    Usage();
                    exit(1);
               }
               blockid = BLOCK;
          }

          else {
               if (tape_dev[0] != 0) {
                    fprintf(stderr, "%s: incorrect arguments\n", 
				program);
                    Usage();
                    exit(1);
               }
               strncpy(tape_dev, *av, 80);
               if (tape_dev[0] == '-') {
                    fprintf(stderr, "%s: incorrect arguments\n", 
				program);
                    Usage();
                    exit(1);
               }
          }

          av++;
          ac--;
     }

     if (tape_dev[0] == 0) {
	strcpy(tape_dev, TAPEDEV);	/* Default tape device */
     }
#if DEBUG
	printf("dtspider: format = %d\n", format);
	printf("dtspider: verbose = %d, size = %d, end = {%s}\n", 
		verbose,  vsize, endstr);
	printf("dtspider: tapedevice={%s}\n", tape_dev);
	printf("dtspider: blockid = %d\n",blockid);
#endif

#if 0
	if(mynode()) {	/* only the root node should do this */
		printf("readtape (node %d): not on root node\n", 
			mynode());
		exit(1);
     }
#endif


     /*
     **   Get memory.
     */
#if DEBUG
     printf("dtspider: initializing buffer\n");
#endif
     if ((ptr = (unsigned char *)malloc(COUNT)) == 0x00) {
          fprintf(stderr, "%s: malloc failed\n", program);
          exit(1);
     }

     labelstate=0;
#if DEBUG
     tm = 3;
#endif
do { /*###*/
     /*
     **   Open tape device.
     */
     if (tm == 3) {
          printf("\n%s: waiting to mount next volume on %s - press <CR> when ready ",
program, tape_dev);
          gets((char *)asciilabel);
     }
#if DEBUG
     printf("dtspider: opening tape device %s . . . ",tape_dev);
     fflush(stdout);
#endif
     if ((fd = open(tape_dev, O_RDONLY)) == -1) {
          perror(tape_dev);
          exit(1);
     }

     /*
     ** Read and dump the tape.
     */
     printf("\n\n");
     i = 0;
     tm = 0;
     eod = 0;
     tapemode = 1;
     total = 0;
     subtotal=0;
     subblock=0;
     errorlab = 0;

     do {
          if ((blockid == BLOCK)&&((tapemode != DATA)||verbose)){
               rc = read_tape_position(fd,&ps);
               if (rc != 0) {
                    printf("WARNING: return from read_tape_position\
			= %d; -b option ignored\n\n",rc);
                    blockid = NOBLOCK;
               }
          }
          num = read(fd,ptr,COUNT);
          if (num < 0) {
               perror(tape_dev);
               fprintf(stderr, "%s: read failed - aborting\n", program);
               exit(1);
          }

          if (num) {          /* Got data */
               if ( (tapemode != DATA) || verbose) {
                    if (blockid == BLOCK) {
                         printf("[block %5d(0x%08x): %5d bytes] ", 
				i, ps.last_block, num);
                    } else {
                         printf("[block %5d: %5d bytes] ", i, num);
                    }
               }
               tm = 0;
               subtotal+=num;
               subblock++;
               /*
               **   Special case for eod to detect EOV.
               */
               if (eod == 1) {
                    strnetoa(ptr, asciilabel, 4);
                    if (strncmp(asciilabel, "EOV1", 4)==0) {
                         labelstate = 23;
                    }
                    eod = 0;
               }
          } /* data */

          else {              /* Tape Mark */
#if	CFS
               cleareof(fd);
#endif	/* CFS */
               if ((blockid == BLOCK) && verbose) {
                    printf("[block %5d(0x%08x): tape mark ----- %5d blocks,  %10d bytes ----]\n\n", i, ps.last_block, subblock, subtotal);
               } else {
                    printf("[block %5d: tape mark ----------- %5d blocks,  %10d bytes ----------]\n\n", i,subblock, subtotal);
               }
               subtotal = 0;
               subblock = 0;
               if (tapemode != DATA) {
                    tm++;
               }

               if ( (tapemode == HEADER) &&
                    (tm == 1) && 
                    (statetab[labelstate].tm_state == 0x7f) &&
                    (total == 160)) {
                       printf("Scratch tape - exiting\n");
                       exit(0);
               }

               if ( ((tapemode == HEADER) || (tapemode == TRAILER)) &&
                     (tm>=1) && format) {
                    if ((statetab[labelstate].tm_state == 0x7f) &&
                        (labelstate != 1)) {
                         printf("Invalid tape mark - expecting %s - aborting\n",
statetab[labelstate].label);
                         exit(1);
                    }

                    if (labelstate >= 23)
                         tm=3;          /* Flag to switch volumes */
                    labelstate = statetab[labelstate].tm_state;
               }

               if ((tapemode == TRAILER) && (tm == 2)) {
                    printf("Unexpected double tape mark - aborting\n");
                    exit(1);
               }

               tapemode+=1;
               if (tapemode == 3) {
                    eod = 1;
               }
               if (tapemode == 4) {
                    tapemode = 1;
               }
          }

          /*
          **   Formatted Labels.
          */
          if ( num && 
               (((tapemode==HEADER) || (tapemode==TRAILER)) && (num != 80)) ) {
               printf("Label block not 80 bytes long\n");
               errorlab = 1;
          }
          else if ( num && 
               ( ((tapemode==HEADER) || (tapemode==TRAILER)) && format)) {
               strnetoa(ptr, asciilabel, 80);
               prptr1=asciilabel;
               for (j=0; j<80; j++, prptr1++) {
                    if ( (*prptr1 > 0x7e) || (!isprint(*prptr1)) )
                         *prptr1=0x2E;  /* "." */
               }
               prptr1=asciilabel;
               if (statetab[labelstate].data_state == 0x7f) {
                    printf("Invalid label - expecting %s\n", statetab[labelstate].label);
                    errorlab = 1;
               }
               if ( strncmp(prptr1, statetab[labelstate].label, 4) != 0 ) {
                    printf("Invalid label - expecting %s\n", statetab[labelstate].label);
                    errorlab=1;
               }
               labelstate = statetab[labelstate].data_state;

               /*
               **   VOL1.
               */
               if (strncmp(prptr1, "VOL1", 4) == 0) {
                    printf(" LABEL={%.4s}\n",prptr1);
                    printf(" VOL_SER={%.6s}  rsvd1={%.1s}  vtoc_ptr={%.10s}\n",prptr1+4, prptr1+10, prptr1+11);
                    printf(" rsvd2={%.20s}  owner={%.10s}\n rsvd3={%.29s}\n\n",prptr1+21, prptr1+41, prptr1+51);
               }

               /*
               **   HDR1, EOV1 or EOF1.
               */
               else if ( (strncmp(prptr1, "HDR1", 4) == 0) ||
                         (strncmp(prptr1, "EOV1", 4) == 0) ||
                         (strncmp(prptr1, "EOF1", 4) == 0) ) {
                    printf(" LABEL={%.4s}\n",prptr1);
                    printf(" DATASET_ID={%.17s}  dataset_serno={%.6s}\n",  prptr1+4, prptr1+21);
                    printf(" vol_seqno={%.4s}  DATASET_SEQNO={%.4s}  gen_no={%.4s}  ver_no={%.2s}\n",prptr1+27, prptr1+31, prptr1+35, prptr1+39);
                    printf(" create_date={%.6s}  EXPIRE_DATE={%.6s}  SECURITY={%.1s}  BLK_COUNT={%.6s}\n", prptr1+41, prptr1+47, prptr1+53, prptr1+54);
                    printf(" sys_code={%.13s}  rsvd={%.7s}\n\n",prptr1+60, prptr1+73);
               }

               /*
               **   HDR2, EOV2 or EOF2.
               */
               else if ( (strncmp(prptr1, "HDR2", 4) == 0) ||
                         (strncmp(prptr1, "EOV2", 4) == 0) ||
                         (strncmp(prptr1, "EOF2", 4) == 0) ) {
                    printf(" LABEL={%.4s}\n",prptr1);
                    printf(" REC_FORMAT={%.1s}  BLOCK_LEN={%.5s}  REC_LEN={%.5s}\n", prptr1+4, prptr1+5, prptr1+10);
                    printf(" density={%.1s}  position={%.1s}  job={%.17s}\n", prptr1+15, prptr1+16, prptr1+17);
                    printf(" REC_TECHNIQUE={%.2s}  CONTROL_CHAR={%.1s}  rsvd1={%.1s}  BLOCK_ATTR={%.1s}\n", prptr1+34, prptr1+36, prptr1+37, prptr1+38, prptr1+48);
                    printf(" rsvd2={%.8s}  CKPT_ID={%.1s}  rsvd3={%.32s}\n\n",prptr1+39, prptr1+47, prptr1+48);
               }

               /*
               **   UHLn or UTLn.
               */
               else if ( (strncmp(prptr1, "UHL", 3) == 0) ||
                         (strncmp(prptr1, "UTL", 3) == 0)) {
                    printf(" LABEL={%.4s}\n",prptr1);
                    userlab=1;
               }

               /*
               **   Label Error
               */
               else {
                    errorlab = 1;
                    printf("Unknown label record:\n");
               }

          } else if (num && (verbose || ((tapemode==HEADER)||(tapemode==TRAILER)))) {
               printf ("\n");
          }

          /*
          **   Data records and User, Unformatted or Error labels.
          */
          if ( num &&
                ( ((tapemode!=DATA) && (format!=FORMAT)) ||
                  ((tapemode== DATA) && (vsize != 0)) ||
                   userlab || errorlab) ) {
               userlab=0;
               prptr1=ptr;
               prptr2=ptr;
               prtsize = num;
               if ((tapemode == DATA) && (num > vsize)) {
                    prtsize=vsize;
               }
               for(j=0, k=1; j<prtsize; j++, prptr1++, k++) {
                    if (k==1) { /* New line - print offset */
                         printf("%5X  ",j);
                    }
                    if (k==8)
                        printf("%02.2X-", (*prptr1)&0xFF);
                    else
                        printf("%02.2X ", (*prptr1)&0xFF);
                    *prptr1 = etoa(*prptr1);
                    if ( (*prptr1 > 0x7e) || (!isprint(*prptr1)) )
                         *prptr1=0x2E;  /* "." */
                    if (k==16) {   /* End of line - print EBCDIC characters */
                         printf("      %.16s\n",prptr2);
                         k=0;
                         prptr2+=16;
                    }
               }
               if (k != 1) {  /* Partial line if vsize is not mod 16 */
                    for(j=k; j<17; j++) printf("   ");
                    printf("      %.*s\n",k-1,prptr2);
               }
               printf("\n");
          }
          total += num;

          if (errorlab) {
               printf("aborting\n");
               exit(1);
          }

          if ( ((tapemode == HEADER)||(tapemode == TRAILER)) &&
               (num != 80) && (num != 0)) {
               printf("Label block not 80 bytes long - aborting\n");
               exit(1);
          }
     i++;
     } while(tm < 2);

     printf("Read %d blocks %d bytes.\n", i, total);
     close(fd);

} while ((labelstate==0) && (tm!=2)); /*###*/
     free(ptr);

#if DEBUG
     printf("dtspider: program terminating.\n");
#endif
}
