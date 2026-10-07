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
 
/*------------------------------------------------------------------------------
 *  test22.c
 *
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license agreement
 *  or nondisclosure agreement with Intel Corporation and may not be
 *  copied nor disclosed except in accordance with the terms of that
 *  agreement.
 *
 *  History:
 *  11- 4-91   g t  Created
 *  10-06-92   cheng 	modified
 *	The LENGTH constant is changed from 33*1024 to 31*1024 because
 *	to make it consistent with the LENGTH value in "test21" althought it
 * 	is not necessary.  (see "test21/node.c" comment)              
 *
 *----------------------------------------------------------------------------*/
#define DEBUG 1
#define TAPEDEV "/dev/io1/rmt6"
#define OWNER "Greg Tensa"
#define	TEST01 "TEST01"
#define	MAXTAPES	2
#define LENGTH (31*1024)
#define IMAX 101
#define JMAX 2
#define ZEROREAD 1
#define VERIFY 1
#define UEXITRET 1

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include "t3480.h"
#include "tioec.h"
#include "tioerr.h"

VOLSER	volser[MAXTAPES];
DCB 	dcb;
VCB 	vcb;
char 	tapedev[80];
int		fd,
		rc,
		i,j;
char	buf[LENGTH];
char	verifybuf[LENGTH];
int		len;
int		node,
		pid;
int		code2=0,
		code3=0,
		code4=0,
		code5=0;

dsopen()	{
	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d read=%d\n", node, pid, dcb.dsname, dcb.dsseqno, dcb.read);
    rc=tdsopen(fd ,&dcb);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc=%d\n", node, pid,rc);
#endif
	if (rc < 0) {
		printf("Node %d, Pid %d:  *** tdsopen error=%d - aborting\n", node, pid , rc);
		errexit();
	}
}

void
verify()	
{
	int		k;

	if (i<2) return;
	for (k=1; k<i-1; k++) {
		if (buf[k] != verifybuf[k])	{
			printf("Node %d, Pid %d:  *** data corruption error - i=%d, k=%d, expect=%d, actual=%d - aborting\n", node, pid, i, k, verifybuf[k], buf[k]);
			errexit();
		}
		buf[k]=0;
	}
}

void
dsclose()	
{
	printf("Node %d, Pid %d:  calling tdsclose\n", node, pid);
	rc=tdsclose(fd);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsclose rc=%d\n\n", node, pid,rc);
#endif
	if (rc < 0) {
		printf("Node %d, Pid %d:  *** tdsclose error=%d - aborting\n", node, pid , rc);
		errexit();
	}
}


int 
uexit(code, exitparm)
	int		code;
	EXITPARM	*exitparm;
{
	int		 k;
	char		*exarray;

    printf("Node %d, Pid %d:    UEXIT value %d\n", node, pid,code);
	if ((code == 2)||(code == 3)||(code == 4)||(code==5)) {
		if (strcmp(dcb.dsname, exitparm->dsname) != 0) {
			printf("Node %d, Pid %d:  *** incorrect dsname [%s], expected [%s] - aborting\n", node, pid, exitparm->dsname, dcb.dsname);
			errexit();
		}
		if (strcmp(volser[0].serialno, exitparm->volser) != 0) {
			printf("Node %d, Pid %d:  *** incorrect volser [%s], expected [%s] - aborting\n", node, pid, exitparm->volser, volser[0].serialno);
			errexit();
		}
		if (dcb.dsseqno != exitparm->dsseqno) {
			printf("Node %d, Pid %d:  *** incorrect dsseqno = %d, expected %d - aborting\n", node, pid, exitparm->dsseqno, dcb.dsseqno);
			errexit();
		}
		if (dcb.volseqno != exitparm->volseqno) {
			printf("Node %d, Pid %d:  *** incorrect volseqno = %d, expected %d - aborting\n", node, pid, exitparm->volseqno, dcb.volseqno);
			errexit();
		}
		exarray = (char *)exitparm;
		if ((code == 4)||(code == 5)) {
			for (k=0; k<=28; k++, exarray++) {
				exitparm->buffer[k] = *exarray;
			}
		}
		if ((code == 2)||(code==3)) { 
			for (k=0; k<=28; k++, exarray++) {
				if (exitparm->buffer[k] != *exarray) {
					printf("Node %d, Pid %d:  *** unexpected uexit data at byte %d=%d, expected %d - aborting\n", node, pid,
						 k, exitparm->buffer[k], *exarray);
					errexit();
				}
			}
		}
	} else {
		printf("Node %d, Pid %d:  *** unexpected user exit = %d - aborting\n", node, pid, code);
		errexit();
	}
	if (code==2) code2++;
	if (code==3) code3++;
	if (code==4) {
		exitparm->buffer[75] = code4&0x7f;
		code4++;
	}
	if (code==5) {
		exitparm->buffer[75] = code5&0x7f;
		code5++;
	}
    return(UEXITRET);
}


main(ac, av)
	int ac;
	char **av;
{
	node=mynode();
	pid=mypid();
	/*
	**	Build verify buffer.
	*/
	for (i=1, j=0; j<LENGTH; i++, j++) {
		if (i > 255) i=0;
		verifybuf[j]=i;
	}

set_spider( -1 );

    /*
    **	Tape device.
	*/
	if (ac<2) {
		strcpy(tapedev, TAPEDEV);
	} else {
		strcpy(tapedev, av[1]);
	}

	/*
	**	Vol/Ser.
	*/
	if (ac<3) {
		strcpy(volser[0].serialno, TEST01);
	} else {
		strcpy(volser[0].serialno, av[2]);
		volser[0].serialno[6] = NULL;
	}
	volser[1].serialno[0] = NULL;

#if DEBUG
	printf("Node %d, Pid %d:  tapedev=%s\n", node, pid,tapedev);
	for (i=0; i<MAXTAPES; i++) {
		printf("Node %d, Pid %d:  volser %2d = %s\n", node, pid, i, volser[i].serialno);
		if (volser[i].serialno[0] == 0)
			break;
	}
	printf("Node %d, Pid %d:  \n", node, pid);
#endif

	/*
	**  Allocate tape drive.
	*/
    vcb.vollist = volser;
    vcb.exlst = &uexit;

	printf("Node %d, Pid %d:  calling tapealloc\n", node, pid);
    fd = tapealloc(&vcb, tapedev);
#if DEBUG
	printf("Node %d, Pid %d:  return from tapealloc fd=%d\n\n", node, pid,fd);
#endif
	if (fd < 0) {
		printf("Node %d, Pid %d:  *** tapealloc error=%d - aborting\n", node, pid , fd);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}
	set_tapedev(fd, 0, 1);	/* 4980, no unload ### */


    dcb.dsseqno = 0;
    dcb.volseqno = 1;
    dcb.read = 0;
	dcb.expirec = ' ';
	dcb.expireyy=00;
	dcb.expiredd=00;


#if 1
    /*
    **	Open data set 1.
	*/
    strcpy(dcb.dsname,"VB011.X104.X500  ");
    dcb.dsseqno = 1;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
    /*
    **	Open data set 2.
	*/
    strcpy(dcb.dsname,"VB014.X104.X500  ");
    dcb.dsseqno = 2;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
    /*
    **	Open data set 3.
	*/
    strcpy(dcb.dsname,"VB111.X104.X500  ");
    dcb.dsseqno = 3;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
    /*
    **	Open data set 1 for read.
	*/
    strcpy(dcb.dsname,"VB011.X104.X500  ");
    dcb.dsseqno = 1;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 104;
	dcb.blocklen = 500;
	dcb.read = 1;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
#if ZEROREAD
	len=tread(fd, buf, LENGTH);
	if (len != 0) {
		printf("Node %d, Pid %d:  *** non-zero tread len at EOF = %d - aborting\n", node, pid , len);
		errexit();
	}
#endif

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
    /*
    **	Open data set 4.
	*/
    strcpy(dcb.dsname,"VB114.X104.X500  ");
    dcb.dsseqno = 4;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 104;
	dcb.blocklen = 500;
    dcb.read = 0;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
    /*
    **	Open data set 5.
	*/
    strcpy(dcb.dsname,"VB100.X104.X500  ");
    dcb.dsseqno = 5;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
    /*
    **	Open data set 1 for read.
	*/
    strcpy(dcb.dsname,"VB011.X104.X500  ");
    dcb.dsseqno = 1;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 104;
	dcb.blocklen = 500;
	dcb.read = 1;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
#if ZEROREAD
	len=tread(fd, buf, LENGTH);
	if (len != 0) {
		printf("Node %d, Pid %d:  *** non-zero tread len at EOF = %d - aborting\n", node, pid , len);
		errexit();
	}
#endif

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **	Open data set 6.
	*/
    strcpy(dcb.dsname,"VS014.X104.X500  ");
    dcb.dsseqno = 6;

	dcb.recfm = 'V';
	dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 104;
	dcb.blocklen = 500;
	dcb.read = 0;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **	Open data set 7.
	*/
    strcpy(dcb.dsname,"Vb011.X104.X500  ");
    dcb.dsseqno = 7;

	dcb.recfm = 'V';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 8.
	*/
    strcpy(dcb.dsname,"Vb014.X104.X500  ");
    dcb.dsseqno = 8;

	dcb.recfm = 'V';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 9.
	*/
    strcpy(dcb.dsname,"Vb111.X104.X500  ");
    dcb.dsseqno = 9;

	dcb.recfm = 'V';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 10.
	*/
    strcpy(dcb.dsname,"Vb114.X104.X500  ");
    dcb.dsseqno = 10;

	dcb.recfm = 'V';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 10 for read.
	*/
    strcpy(dcb.dsname,"Vb114.X104.X500  ");
    dcb.dsseqno= 10;

	dcb.recfm = 'V';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 104;
	dcb.blocklen = 500;
	dcb.read = 1;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
#if ZEROREAD
	len=tread(fd, buf, LENGTH);
	if (len != 0) {
		printf("Node %d, Pid %d:  *** non-zero tread len at EOF = %d - aborting\n", node, pid , len);
		errexit();
	}
#endif

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 11.
	*/
    strcpy(dcb.dsname,"Vb100.X104.X500  ");
    dcb.dsseqno = 11;

	dcb.recfm = 'V';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 104;
	dcb.blocklen = 500;
	dcb.read = 0;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 12.
	*/
    strcpy(dcb.dsname,"Fb011.X100.X100  ");
    dcb.dsseqno = 12;

	dcb.recfm = 'F';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 13.
	*/
    strcpy(dcb.dsname,"Fb014.X100.X100  ");
    dcb.dsseqno = 13;

	dcb.recfm = 'F';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 14 for read (should fail).
	*/
    strcpy(dcb.dsname,"Fb000.X100.X100  ");
    dcb.dsseqno= 14;

	dcb.recfm = 'F';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 100;
	dcb.blocklen = 100;
	dcb.read = 1;

	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d read=%d\n", node, pid, dcb.dsname, dcb.dsseqno, dcb.read);
    rc=tdsopen(fd ,&dcb);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc=%d (should be unsuccessful)\n\n", node, pid,rc);
#endif
	if (rc != TERR_DSSEQNO_NOT_FOUND) {
		printf("Node %d, Pid %d:  *** tdsopen incorrect error=%d (expecting %d) - aborting\n", node, pid , rc, TERR_DSSEQNO_NOT_FOUND);
		errexit();
	}
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 14.
	*/
    strcpy(dcb.dsname,"Fb000.X100.X100  ");
    dcb.dsseqno = 14;

	dcb.recfm = 'F';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 100;
	dcb.blocklen = 100;
	dcb.read = 0;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 15.
	*/
    strcpy(dcb.dsname,"Fb111.X100.X100  ");
    dcb.dsseqno = 15;

	dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 4 for read (should fail).
	*/
    strcpy(dcb.dsname,"Fb000.X100.X100  ");
    dcb.dsseqno= 4;

	dcb.recfm = 'F';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 100;
	dcb.blocklen = 100;
	dcb.read = 1;

	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d read=%d\n", node, pid, dcb.dsname, dcb.dsseqno, dcb.read);
    rc=tdsopen(fd ,&dcb);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc=%d (should be unsuccessful)\n\n", node, pid,rc);
#endif
	if (rc != TERR_INVALID_DSNAME_SPECIFIED) {
		printf("Node %d, Pid %d:  *** tdsopen incorrect error=%d (expecting %d) - aborting\n", node, pid , rc, TERR_INVALID_DSNAME_SPECIFIED);
		errexit();
	}
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 16.
	*/
    strcpy(dcb.dsname,"Fb114.X100.X100  ");
    dcb.dsseqno = 16;

	dcb.recfm = 'F';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 100;
	dcb.blocklen = 100;
	dcb.read = 0;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 17.
	*/
    strcpy(dcb.dsname,"Fb100.X100.X100  ");
    dcb.dsseqno = 17;

	dcb.recfm = 'F';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 18.
	*/
    strcpy(dcb.dsname,"FB011.X100.X100  ");
    dcb.dsseqno = 18;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 18 (should fail).
	*/
    strcpy(dcb.dsname,"FB011.X100.X100  ");
    dcb.dsseqno = 18;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d read=%d\n", node, pid, dcb.dsname, dcb.dsseqno, dcb.read);
    rc=tdsopen(fd ,&dcb);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc=%d (should be unsuccessful)\n\n", node, pid,rc);
#endif
	if (rc != TERR_CANNOT_APPEND) {
		printf("Node %d, Pid %d:  *** tdsopen incorrect error=%d (expecting %d) - aborting\n", node, pid , rc, TERR_CANNOT_APPEND);
		errexit();
	}
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 19.
	*/
    strcpy(dcb.dsname,"FB014.X100.X100  ");
    dcb.dsseqno = 19;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 20.
	*/
    strcpy(dcb.dsname,"FB000.X100.X100  ");
    dcb.dsseqno = 20;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 8 (should fail).
	*/
    strcpy(dcb.dsname,"FB011.X100.X100  ");
    dcb.dsseqno = 8;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d read=%d\n", node, pid, dcb.dsname, dcb.dsseqno, dcb.read);
    rc=tdsopen(fd ,&dcb);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc=%d (should be unsuccessful)\n\n", node, pid,rc);
#endif
	if (rc != TERR_CANNOT_APPEND) {
		printf("Node %d, Pid %d:  *** tdsopen incorrect error=%d (expecting %d) - aborting\n", node, pid , rc, TERR_CANNOT_APPEND);
		errexit();
	}
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 21.
	*/
    strcpy(dcb.dsname,"FB011.X100.X500  ");
    dcb.dsseqno = 21;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 100;
    dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 22.
	*/
    strcpy(dcb.dsname,"FB014.X100.X500  ");
    dcb.dsseqno = 22;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 100;
    dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 23.
	*/
    strcpy(dcb.dsname,"FB111.X100.X500  ");
    dcb.dsseqno = 23;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 100;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 24.
	*/
    strcpy(dcb.dsname,"FB114.X100.X500  ");
    dcb.dsseqno = 24;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 100;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 25.
	*/
    strcpy(dcb.dsname,"FB100.X100.X500  ");
    dcb.dsseqno = 25;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 100;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 26.
	*/
    strcpy(dcb.dsname,"Ub011.X000.X500  ");
    dcb.dsseqno = 26;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 27.
	*/
    strcpy(dcb.dsname,"Ub014.X000.X500  ");
    dcb.dsseqno = 27;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 28.
	*/
    strcpy(dcb.dsname,"Ub111.X000.X500  ");
    dcb.dsseqno = 28;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 29.
	*/
    strcpy(dcb.dsname,"Ub114.X000.X500  ");
    dcb.dsseqno = 29;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 30.
	*/
    strcpy(dcb.dsname,"Ub000.X000.X500  ");
    dcb.dsseqno = 30;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 31.
	*/
    strcpy(dcb.dsname,"Ub100.X000.X500  ");
    dcb.dsseqno = 31;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 37 (should fail).
	*/
    strcpy(dcb.dsname,"Ub100.X000.X500  ");
    dcb.dsseqno = 37;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d read=%d\n", node, pid, dcb.dsname, dcb.dsseqno, dcb.read);
    rc=tdsopen(fd ,&dcb);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc=%d (should be unsuccessful)\n\n", node, pid,rc);
#endif
	if (rc != TERR_CANNOT_APPEND) {
		printf("Node %d, Pid %d:  *** tdsopen incorrect error=%d (expecting %d) - aborting\n", node, pid , rc, TERR_CANNOT_APPEND);
		errexit();
	}
#endif


#if 1
/*PAGE*/
    /*
    **  Open data set 3 (should fail).
	*/
    strcpy(dcb.dsname,"Ub100.X000.X500  ");
    dcb.dsseqno = 3;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d read=%d\n", node, pid, dcb.dsname, dcb.dsseqno, dcb.read);
    rc=tdsopen(fd ,&dcb);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc=%d (should be unsuccessful)\n\n", node, pid,rc);
#endif
	if (rc != TERR_CANNOT_APPEND) {
		printf("Node %d, Pid %d:  *** tdsopen incorrect error=%d (expecting %d) - aborting\n", node, pid , rc, TERR_CANNOT_APPEND);
		errexit();
	}
#endif


#if 1
/*PAGE*/
    /*
    **	Open data set 32.
	*/
    strcpy(dcb.dsname,"VS011.X496.X500  ");
    dcb.dsseqno=32;

	dcb.recfm = 'V';
	dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 496;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **	Open data set 33.
	*/
    strcpy(dcb.dsname,"VS014.X496.X500  ");
    dcb.dsseqno=33;

	dcb.recfm = 'V';
	dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 496;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **	Open data set 34.
	*/
    strcpy(dcb.dsname,"VR011.X496.X500  ");
    dcb.dsseqno=34;

	dcb.recfm = 'V';
	dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 496;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **	Open data set 35.
	*/
    strcpy(dcb.dsname,"VR014.X496.X500  ");
    dcb.dsseqno=35;

	dcb.recfm = 'V';
	dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 496;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **	Open data set 36.
	*/
    strcpy(dcb.dsname,"VR013.X496.X500  ");
    dcb.dsseqno=36;

	dcb.recfm = 'V';
	dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 3;
	dcb.reclen = 496;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **	Open data set 37.
	*/
    strcpy(dcb.dsname,"VR016.X4K.X4K    ");
    dcb.dsseqno=37;

	dcb.recfm = 'V';
	dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 6;
	dcb.reclen = 4092;
	dcb.blocklen = 4096;

	dsopen();

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


/*PAGE*/
	/*
	**	Deallocate tape.
	*/
	printf("Node %d, Pid %d:  calling tapedealloc\n", node, pid);
	rc=tapedealloc(fd);
#if DEBUG
	printf("Node %d, Pid %d:  return from tapedealloc rc=%d\n\n", node, pid,rc);
#endif
	if (rc < 0) {
		printf("Node %d, Pid %d:  *** tapedealloc error=%d - aborting\n", node, pid , rc);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}

	printf("Node %d, Pid %d:  User exit counts: code2=%d, code3=%d, code4=%d, code5=%d\n", node, pid, code2, code3, code4, code5);
	if (UEXITRET == 1) {
		if ((code2 != 3*8)||(code3 != 3*8)||(code4 != 37*8)||(code5 != 37*8)) {
			printf("Node %d, Pid %d:  *** incorrect user exit2/3 counts - aborting\n", node, pid );
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
	} else {
		if ((code2 != 0)||(code3 != 0)||(code4 != 37)||(code5 != 37)) {
			printf("Node %d, Pid %d:  *** incorrect user exit2/3 counts - aborting\n", node, pid );
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
	}

	printf("\nNode %d, Pid %d:  Test PASSED\n", node, pid);
    exit(0);
}
