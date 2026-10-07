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
 *  test13.c
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
 *	The LENGTH constant must be changed from 33*1024 to 31*1024 because
 *	the LENGTH is passed into "tread" as the "length" parameter which is
 * 	of type "int" with limit 32*1024-1 which is smaller than 33*1024
 *
 *----------------------------------------------------------------------------*/
#define TESTNUM1 1
#define TESTNUM2 0
#define TESTNUM3 0
#define DEBUG 1
#define TAPEDEV "/cfs/ctape0"
#define OWNER "Greg Tensa"
#define	TEST0 "TEST0 "
#define	MAXTAPES	10
#define LENGTH (31*1024)
#define IMAX 101
#define JMAX 4
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
int		len;
int		node,
		pid;
int		code6=0,
		code7=0,
		code8=0;


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
#endif
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
    printf("Node %d, Pid %d:   UEXIT:  value %d\n", node, pid,code);
	if (code == 6) code6++;
	else if (code == 7) code7++;
	else if (code == 8) {
		code8++;
		printf("Node %d, Pid %d:  Enter next vol/ser: ", node, pid);
		gets(exitparm->volser);
		if (exitparm->volser[0] != 0) {
			return(1);
		}
	}
    else {
		printf("Node %d, Pid %d:  *** unknown user exit - aborting\n", node, pid );
		errexit();
	}
	return(0);
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
    vcb.exlst = &uexit;

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

    dcb.volseqno = 0;
    dcb.dsseqno = 0;
    dcb.read = 1;

#if TESTNUM1
#if 1
/*PAGE*/
    /*
    **	Open data set 1.
	*/
    strcpy(dcb.dsname,"VB011.X104.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    strcpy(dcb.dsname,"VB014.X104.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    strcpy(dcb.dsname,"VB111.X104.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    strcpy(dcb.dsname,"VB114.X104.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **	Open data set 5.
	*/
    strcpy(dcb.dsname,"VB100.X104.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    strcpy(dcb.dsname,"VS014.X104.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **	Open data set 7.
	*/
    strcpy(dcb.dsname,"Vb011.X104.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 8.
	*/
    strcpy(dcb.dsname,"Vb014.X104.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 9.
	*/
    strcpy(dcb.dsname,"Vb111.X104.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 10.
	*/
    strcpy(dcb.dsname,"Vb114.X104.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
#endif

#if TESTNUM2
#if 1
/*PAGE*/
    /*
    **  Open data set 11.
	*/
    strcpy(dcb.dsname,"Vb100.X104.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'V';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 12.
	*/
    strcpy(dcb.dsname,"Fb011.X100.X100  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 13.
	*/
    strcpy(dcb.dsname,"Fb014.X100.X100  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 14.
	*/
    strcpy(dcb.dsname,"Fb000.X100.X100  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 15.
	*/
    strcpy(dcb.dsname,"Fb111.X100.X100  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 16.
	*/
    strcpy(dcb.dsname,"Fb114.X100.X100  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 17.
	*/
    strcpy(dcb.dsname,"Fb100.X100.X100  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 18.
	*/
    strcpy(dcb.dsname,"FB011.X100.X100  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 19.
	*/
    strcpy(dcb.dsname,"FB014.X100.X100  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 20.
	*/
    strcpy(dcb.dsname,"FB000.X100.X100  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 100;
	dcb.blocklen = 100;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
#endif

#if TESTNUM3
#if 1
/*PAGE*/
    /*
    **  Open data set 21.
	*/
    strcpy(dcb.dsname,"FB011.X100.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 100;
    dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 22.
	*/
    strcpy(dcb.dsname,"FB014.X100.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 100;
    dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 23.
	*/
    strcpy(dcb.dsname,"FB111.X100.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 100;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 24.
	*/
    strcpy(dcb.dsname,"FB114.X100.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 100;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 25.
	*/
    strcpy(dcb.dsname,"FB100.X100.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 100;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, 100);
			if (len != 100) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 26.
	*/
    strcpy(dcb.dsname,"Ub011.X000.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 27.
	*/
    strcpy(dcb.dsname,"Ub014.X000.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 28.
	*/
    strcpy(dcb.dsname,"Ub111.X000.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 29.
	*/
    strcpy(dcb.dsname,"Ub114.X000.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 30.
	*/
    strcpy(dcb.dsname,"Ub000.X000.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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
    **  Open data set 31.
	*/
    strcpy(dcb.dsname,"Ub100.X000.X500  ");
    dcb.dsseqno++;
    dcb.volseqno++;

	dcb.recfm = 'U';
	dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
	dcb.reclen = 0;
	dcb.blocklen = 500;

	dsopen();

	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	for (j=1; j<JMAX; j++) {
		for (i=1; i<IMAX; i++) {
			len=tread(fd, buf, LENGTH);
			if (len != i) {
				printf("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid , len,i,j);
				errexit();
			}
			if (buf[0]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid ,buf[0],i,j);
				errexit();
			}
			if (buf[i-1]!=i) {
				printf("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid ,buf[i-1],i,j);
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

	printf("Node %d, Pid %d:  User exit counts: code6=%d, code7=%d, code8=%d\n", node, pid, code6, code7, code8);
	printf("\nNode %d, Pid %d:  Test PASSED\n", node, pid);
    exit(0);
}
