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
 *  testvsr.c
 *
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license agreement
 *  or nondisclosure agreement with Intel Corporation and may not be
 *  copied nor disclosed except in accordance with the terms of that
 *  agreement.
 *
 *  History:
 *   2-20-91   g t  Created
 *  10-06-92   cheng 	modified
 *	The LENGTH constant must be changed from 33*1024 to 31*1024 because
 *	the LENGTH is passed into "tread" as the "length" parameter which is
 * 	of type "int" with limit 32*1024-1 which is smaller than 33*1024
 *
 *----------------------------------------------------------------------------*/
#define DEBUG 1
#define TAPEDEV "/cfs/ctape0"
#define OWNER "Greg Tensa"
#define	TEST0 "TEST0 "
#define	MAXTAPES	4
#define LENGTH (31*1024)
#define IMAX 2001
#define JMAX 3
#define ZEROREAD 1
#define VERIFY 1

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
char	ichar;
int		len;
int		node,
		pid;
int		code2=0,
		code3=0;


dsopen()	{
	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", node, pid, dcb.dsname, dcb.dsseqno);
    rc=tdsopen(fd ,&dcb);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc=%d\n", node, pid,rc);
#endif
	if (rc < 0) {
		printf("Node %d, Pid %d:  *** tdsopen error=%d - aborting\n", node, pid , rc);
		errexit();
	}
}


#if VERIFY
verify()	{
	int		k;

	if (i<2) return;
	for (k=1; k<len-1; k++) {
		if (buf[k] != verifybuf[k])	{
			printf("Node %d, Pid %d:  *** data corruption error - i=%d, k=%d, expect=%d, actual=%d - aborting\n", node, pid, i, k, verifybuf[k], buf[k]);
			errexit();
		}
		buf[k]=0;
	}
}
#endif


dsclose()	{
#if ZEROREAD
	len=tread(fd, buf, LENGTH);
	if (len != 0) {
		printf("Node %d, Pid %d:  *** non-zero tread len at EOF = %d - aborting\n", node, pid , len);
		errexit();
	}
#if UEXITRET
	if (code2 != code3) {
		printf("Node %d, Pid %d:  *** incorrect user exit counts, code2=%d, code3=%d - aborting\n", node, pid , code2, code3);
		errexit();
	}
#endif
#endif
#if DEBUG
	printf("Node %d, Pid %d:  calling tdsclose\n", node, pid);
#endif
	rc=tdsclose(fd);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsclose rc=%d\n\n", node, pid,rc);
#endif
	if (rc < 0) {
		printf("Node %d, Pid %d:  *** tdsclose error=%d - aborting\n", node, pid , rc);
		errexit();
	}
#if UEXITRET
	if (code2 != code3) {
		printf("Node %d, Pid %d:  *** incorrect user exit counts, code2=%d, code3=%d - aborting\n", node, pid , code2, code3);
		errexit();
	}
#endif
}


main(ac, av)
	int ac;
	char **av;

 {
	node=mynode();
	pid=mypid();
#if VERIFY
	/*
	**	Build verify buffer.
	*/
	for (i=1, j=0; j<LENGTH; i++, j++) {
		if (i > 255) i=0;
		verifybuf[j]=i;
	}
#endif

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
	for (i=0; i<MAXTAPES; i++) {
		if (ac<i+3) {
			strcpy(volser[i].serialno, TEST0);
			volser[i].serialno[5]=i+0x31;
			volser[i].serialno[6] = NULL;
		} else {
			strcpy(volser[i].serialno, av[i+2]);
			volser[i].serialno[6] = NULL;
		}
	}
	volser[MAXTAPES-1].serialno[0] = NULL;


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
    vcb.exlst = (int *) NULL;

	printf("Node %d, Pid %d:  calling tapealloc\n", node, pid);
    fd = tapealloc(&vcb, tapedev);
#if DEBUG
	printf("Node %d, Pid %d:  return from tapealloc fd=%d\n\n", node, pid,fd);
#endif
	if (fd < 0) {
		printf("Node %d, Pid %d:  *** tapealloc error=%d - aborting\n", node, pid , fd);
		printf("Node %d, Pid %d:  Test FAILED\n",node,pid);
		exit(1);
	}
	set_tapedev(fd, 0, 1);	/* 4980, no unload ### */

    dcb.volseqno = 1;
    dcb.dsseqno = 0;
    dcb.read = 1;


#if 1
/*PAGE*/
    /*
    **	Open data set 1.
	*/
    strcpy(dcb.dsname,"VS011.X496.X500  ");
    dcb.dsseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 496;
	dcb.blocklen = 500;
	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i+=73) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			ichar=i&0xFF;
			if ((buf[0]&0xff)!=(ichar&0xff)) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0]&0xFF,ichar&0xFF,j);
				errexit();
			}
			if ((buf[i-1]&0xFF)!=(ichar&0xFF)) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1]&0xFF,ichar&0xFF,j);
				printf("Node %d, Pid %d:  buf[i-1] = %02X, i = %02X\n", node, pid ,
			buf[i-1]&0xFF, ichar&0xFF);
				errexit();
			}
#if VERIFY
			verify();
#endif
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
    **	Open data set 2.
	*/
    strcpy(dcb.dsname,"VS014.X496.X500  ");
    dcb.dsseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 496;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i+=79) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			ichar=i&0xFF;
			if ((buf[0]&0xff)!=(ichar&0xff)) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0]&0xFF,ichar&0xFF,j);
				errexit();
			}
			if ((buf[i-1]&0xFF)!=(ichar&0xFF)) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1]&0xFF,ichar&0xFF,j);
				errexit();
			}
#if VERIFY
			verify();
#endif
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
    **	Open data set 3.
	*/
    strcpy(dcb.dsname,"VR011.X496.X500  ");
    dcb.dsseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 496;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i+=87) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			ichar=i&0xFF;
			if ((buf[0]&0xff)!=(ichar&0xff)) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0]&0xFF,ichar&0xFF,j);
				errexit();
			}
			if ((buf[i-1]&0xFF)!=(ichar&0xFF)) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1]&0xFF,ichar&0xFF,j);
				errexit();
			}
#if VERIFY
			verify();
#endif
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
    **	Open data set 4.
	*/
    strcpy(dcb.dsname,"VR014.X496.X500  ");
    dcb.dsseqno++;
    dcb.volseqno = 3;

	dcb.recfm = 'V';
	dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 496;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i+=93) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			ichar=i&0xFF;
			if ((buf[0]&0xff)!=(ichar&0xff)) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0]&0xFF,ichar&0xFF,j);
				errexit();
			}
			if ((buf[i-1]&0xFF)!=(ichar&0xFF)) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1]&0xFF,ichar&0xFF,j);
				errexit();
			}
#if VERIFY
			verify();
#endif
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
    **  Open data set 5 (should fail).
	*/
    strcpy(dcb.dsname,"VR013.X496.X501  ");
    dcb.dsseqno= 5;

	dcb.recfm = 'V';
	dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 3;
	dcb.reclen = 496;
	dcb.blocklen = 500;

	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", node, pid, dcb.dsname, dcb.dsseqno);
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
    **	Open data set 5.
	*/
    strcpy(dcb.dsname,"VR013.X496.X500  ");
    dcb.dsseqno= 5;

	dcb.recfm = 'V';
	dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 3;
	dcb.reclen = 496;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i+=29) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			ichar=i&0xFF;
			if ((buf[0]&0xff)!=(ichar&0xff)) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0]&0xFF,ichar&0xFF,j);
				errexit();
			}
			if ((buf[i-1]&0xFF)!=(ichar&0xFF)) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1]&0xFF,ichar&0xFF,j);
				errexit();
			}
#if VERIFY
			verify();
#endif
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
    **	Open data set 6.
	*/
    strcpy(dcb.dsname,"VR016.X4K.X4K    ");
    dcb.dsseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 6;
	dcb.reclen = 4092;
	dcb.blocklen = 4096;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<3; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != LENGTH) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			ichar=i&0xFF;
			if ((buf[0]&0xff)!=(ichar&0xff)) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0]&0xFF,ichar&0xFF,j);
				errexit();
			}
			if ((buf[i-1]&0xFF)!=(ichar&0xFF)) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1]&0xFF,ichar&0xFF,j);
				errexit();
			}
#if VERIFY
			verify();
#endif
		}
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
		printf("Node %d, Pid %d:  Test FAILED\n",node,pid);
		exit(1);
	}


	printf("\nNode %d, Pid %d:  Test PASSED\n", node, pid);
    exit(0);
}
