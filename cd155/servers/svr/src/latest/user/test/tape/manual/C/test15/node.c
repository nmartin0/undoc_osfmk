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
 *  test15.c
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
 *	to make it consistent with the LENGTH value in "test0" althought it
 * 	is not necessary.  (see "test0/node.c" comment)              
 *
 *----------------------------------------------------------------------------*/
#define DEBUG 1
#define TAPEDEV "/cfs/ctape0"
#define OWNER "Greg Tensa"
#define	TEST01 "TEST01"
#define	MAXTAPES	3
#define LENGTH (31*1024)
#define IMAX 101
#define JMAX 2
#define ZEROREAD 1
#define VERIFY 1
#define UEXITRET 1

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <t3480.h>

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


verify()	{
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


dsclose()	{
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


int uexit(code, exitparm)
int			code;
EXITPARM   *exitparm;
{
	int			 k;
	char		*exarray;

    printf("Node %d, Pid %d:    UEXIT value %d\n", node, pid,code);
	if ((code == 2)||(code == 3)||(code == 4)||(code==5)) {
		if (strcmp(dcb.dsname, exitparm->dsname) != 0) {
			printf("Node %d, Pid %d:  *** incorrect dsname [%s], expecting [%s] - aborting\n", node, pid, exitparm->dsname, dcb.dsname);
			errexit();
		}
		if (strcmp(volser[0].serialno, exitparm->volser) != 0) {
			printf("Node %d, Pid %d:  *** incorrect volser [%s], expecting [%s] - aborting\n", node, pid, exitparm->volser, volser[0].serialno);
			errexit();
		}
		if (dcb.dsseqno != exitparm->dsseqno) {
			printf("Node %d, Pid %d:  *** incorrect dsseqno = %d, expecting %d - aborting\n", node, pid, exitparm->dsseqno, dcb.dsseqno);
			errexit();
		}
		if (dcb.volseqno != exitparm->volseqno) {
			printf("Node %d, Pid %d:  *** incorrect volseqno = %d, expecting %d - aborting\n", node, pid, exitparm->volseqno, dcb.volseqno);
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
					printf("Node %d, Pid %d:  *** unexpected uexit data at byte %d=%d, expecting %d - aborting\n", node, pid,
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

/*PAGE*/
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
/*PAGE*/
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
	**	Write some data.
	*/
	printf("Node %d, Pid %d:  starting twrite loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			buf[0]=i;
			buf[i-1]=i;
			len=twrite(fd, buf, i);
			if (len != i) {
				printf("Node %d, Pid %d:  *** twrite error: len=%d i=%d j=%d - aborting\n", node, pid , len,i,j);
				errexit();
			}
		}
	}

	/*
	**	Close Data Set.
	*/
	dsclose();
#endif


#if 1
/*PAGE*/
    /*
    **	Open data set 1.
	*/
	dcb.read = 1;

	dsopen();

	
	printf("Node %d, Pid %d:  calling feov\n", node, pid);
	rc=feov(fd);
	printf("Node %d, Pid %d:  return from feov =%d\n", node, pid, rc);
	if (rc >= 0) {
		printf("Node %d, Pid %d:  *** feov missed error, rc=%d - aborting\n", node, pid, rc);
		errexit();
	}

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


	printf("\nNode %d, Pid %d:  Test PASSED\n", node, pid);
    exit(0);
}
