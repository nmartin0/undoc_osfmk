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
 *  test8.c
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
 *	The LENGTH constant is changed from 33*1024 to 32*1024-1 because
 *	to make it consistent with the LENGTH value in "test7". 
 * 	(see "test7/node.c" comment)              
 *
 *----------------------------------------------------------------------------*/
#define DEBUG 1
#define TAPEDEV0 "/dev/io1/rmt6"
#define TAPEDEV1 "/cfs/ctape1"
#define	TEST0 "TEST0 "
#define	TEST1 "TEST1 "
#define	MAXTAPES	5
#define LENGTH (32*1024-1)
#define UEXITRET0 1
#define UEXITRET1 1

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <t3480.h>

VOLSER	volser0[MAXTAPES];
VOLSER	volser1[MAXTAPES];
DCB 	dcb0;
DCB 	dcb1;
VCB 	vcb0;
VCB 	vcb1;
char 	tapedev0[80];
char 	tapedev1[80];
int		fd0,
		fd1,
		rc0,
		rc1,
		i,j;
char	buf[LENGTH];
int		len;
int		node,
		pid;
int		code04=0,
		code05=0,
		code06=0,
		code07=0;
int		code14=0,
		code15=0,
		code16=0,
		code17=0;
int		eov0=0;
int		eov1=0;

int uexit0(code, exitparm)
int			code;
EXITPARM   *exitparm;
{
	int			 k;
	char		*exarray;

    printf("Node %d, Pid %d:    UEXIT0 value %d\n", node, pid,code);
	if ((code == 4)||(code==5)) {
		if (strcmp(dcb0.dsname, exitparm->dsname) != 0) {
			printf("Node %d, Pid %d:  *** incorrect dsname [%s], expected [%s] - aborting\n", node, pid, exitparm->dsname, dcb0.dsname);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		if (strcmp(volser0[eov0].serialno, exitparm->volser) != 0) {
			printf("Node %d, Pid %d:  *** incorrect volser0 [%s], expected [%s] - aborting\n", node, pid, exitparm->volser, volser0[eov0].serialno);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		if (dcb0.dsseqno != exitparm->dsseqno) {
			printf("Node %d, Pid %d:  *** incorrect dsseqno = %d, expected %d - aborting\n", node, pid, exitparm->dsseqno, dcb0.dsseqno);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		if (eov0+1 != exitparm->volseqno) {
			printf("Node %d, Pid %d:  *** incorrect volseqno = %d, expected %d - aborting\n", node, pid, exitparm->volseqno, dcb0.volseqno);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		exarray = (char *)exitparm;
		for (k=0; k<=28; k++, exarray++) {
			exitparm->buffer[k] = *exarray;
		}
	} else if ((code != 6) && (code != 7)) {
		printf("Node %d, Pid %d:  *** unexpected user exit0 = %d - aborting\n", node, pid, code);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}
	if (code==4) {
		exitparm->buffer[75] = code04&0x7f;
		code04++;
	}
	if (code==5) {
		exitparm->buffer[75] = code05&0x7f;
		code05++;
	}
	if (code==7) code07++;
	if (code==6) {
		code06++;
		eov0++;
	}
    return(UEXITRET0);
}


int uexit1(code, exitparm)
int			code;
EXITPARM   *exitparm;
{
	int			 k;
	char		*exarray;

    printf("Node %d, Pid %d:    UEXIT1 value %d\n", node, pid,code);
	if ((code == 4)||(code==5)) {
		if (strcmp(dcb1.dsname, exitparm->dsname) != 0) {
			printf("Node %d, Pid %d:  *** incorrect dsname [%s], expected [%s] - aborting\n", node, pid, exitparm->dsname, dcb1.dsname);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		if (strcmp(volser1[eov1].serialno, exitparm->volser) != 0) {
			printf("Node %d, Pid %d:  *** incorrect volser1 [%s], expected [%s] - aborting\n", node, pid, exitparm->volser, volser1[eov1].serialno);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		if (dcb1.dsseqno != exitparm->dsseqno) {
			printf("Node %d, Pid %d:  *** incorrect dsseqno = %d, expected %d - aborting\n", node, pid, exitparm->dsseqno, dcb1.dsseqno);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		if (eov1+1 != exitparm->volseqno) {
			printf("Node %d, Pid %d:  *** incorrect volseqno = %d, expected %d - aborting\n", node, pid, exitparm->volseqno, dcb1.volseqno);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		exarray = (char *)exitparm;
		for (k=0; k<=28; k++, exarray++) {
			exitparm->buffer[k] = *exarray;
		}
	} else if ((code != 6) && (code != 7)) {
		printf("Node %d, Pid %d:  *** unexpected user1 exit = %d - aborting\n", node, pid, code);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}
	if (code==4) {
		exitparm->buffer[75] = code14&0x7f;
		code14++;
	}
	if (code==5) {
		exitparm->buffer[75] = code15&0x7f;
		code15++;
	}
	if (code==7) code17++;
	if (code==6) {
		code16++;
		eov1++;
	}
    return(UEXITRET1);
}


main(ac, av)
	int ac;
	char **av;

 {
	node=mynode();
	pid=mypid();
	/*
	**	Initialize write buffer.
	*/
	for (i=0, j=0; i<LENGTH; i++, j++) {
		if (j==256) j=0;
		buf[i]=j;
	}

    /*
    **	Tape device 0.
	*/
	if (ac<2) {
		strcpy(tapedev0, TAPEDEV0);
	} else {
		strcpy(tapedev0, av[1]);
	}

	/*
	**	Vol/Ser 0.
	*/
	for (i=0; i<MAXTAPES; i++) {
		if (ac<i+3) {
			strcpy(volser0[i].serialno, TEST0);
			volser0[i].serialno[5]=i+0x31;
			volser0[i].serialno[6] = NULL;
		} else {
			strcpy(volser0[i].serialno, av[i+2]);
			volser0[i].serialno[6] = NULL;
		}
	}
	volser0[MAXTAPES-1].serialno[0] = NULL;


#if DEBUG
	printf("Node %d, Pid %d:  tapedev0=%s\n", node, pid,tapedev0);
	for (i=0; i<MAXTAPES; i++) {
		printf("Node %d, Pid %d:  volser0[%1d] = %s\n", node, pid, i, volser0[i].serialno);
		if (volser0[i].serialno[0]  == 0)
			break;
	}
	printf("Node %d, Pid %d:  \n", node, pid);
#endif

    /*
    **	Tape device 1.
	*/
	if (ac<3) {
		strcpy(tapedev1, TAPEDEV1);
	} else {
		strcpy(tapedev1, av[2]);
	}

	/*
	**	Vol/Ser 1.
	*/
	for (i=0; i<MAXTAPES; i++) {
		if (ac<i+4+MAXTAPES) {
			strcpy(volser1[i].serialno, TEST1);
			volser1[i].serialno[5]=i+0x31;
			volser1[i].serialno[6] = NULL;
		} else {
			strcpy(volser1[i].serialno, av[i+3+MAXTAPES]);
			volser1[i].serialno[6] = NULL;
		}
	}
	volser1[MAXTAPES-1].serialno[0] = NULL;


#if DEBUG
	printf("Node %d, Pid %d:  tapedev1=%s \n", node, pid,tapedev1);
	for (i=0; i<MAXTAPES; i++) {
		printf("Node %d, Pid %d:  volser1[%1d] = %s\n", node, pid, i, volser1[i].serialno);
		if (volser1[i].serialno[0] == 0)
			break;
	}
	printf("Node %d, Pid %d:  \n", node, pid);
#endif

/*PAGE*/
	/*
	**  Allocate tape drive 0.
	*/
    vcb0.vollist = volser0;
    vcb0.exlst = &uexit0;

	printf("Node %d, Pid %d:  calling tapealloc 0\n", node, pid);
    fd0 = tapealloc(&vcb0, tapedev0);
#if DEBUG
	printf("Node %d, Pid %d:  return from tapealloc 0 fd0=%d\n\n", node, pid,fd0);
#endif
	if (fd0 < 0) {
		printf("Node %d, Pid %d:  *** tapealloc 0 error=%d - aborting\n", node, pid , fd0);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}

	/*
	**  Allocate tape drive 1.
	*/
    vcb1.vollist = volser1;
    vcb1.exlst = &uexit1;

	printf("Node %d, Pid %d:  calling tapealloc 1\n", node, pid);
    fd1 = tapealloc(&vcb1, tapedev1);
#if DEBUG
	printf("Node %d, Pid %d:  return from tapealloc 1 fd1=%d\n\n", node, pid,fd1);
#endif
	if (fd1 < 0) {
		printf("Node %d, Pid %d:  *** tapealloc 1 error=%d - aborting\n", node, pid , fd1);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}



#if 1
/*PAGE*/
    /*
    **	Open data set 0-1.
	*/
    strcpy(dcb0.dsname,"UB014.Z.LONG0    ");
    dcb0.volseqno = 1;
    dcb0.dsseqno = 1;
	dcb0.expirec = ' ';
	dcb0.expireyy=00;
	dcb0.expiredd=00;

	dcb0.recfm = 'U';
	dcb0.blocklen = 32000;
	dcb0.reclen = 0;
	dcb0.blkattr = 'B';

    dcb0.read = 0;
    dcb0.block_mode = 0;
    dcb0.buffered_mode = 1;
    dcb0.num_tapeblk_bufs = 4;


	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", node, pid, dcb0.dsname, dcb0.dsseqno);
    rc0=tdsopen(fd0 ,&dcb0);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc0=%d\n\n", node, pid,rc0);
#endif
	if (rc0 < 0) {
		printf("Node %d, Pid %d:  *** tdsopen error=%d - aborting\n", node, pid , rc0);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}


/*PAGE*/
    /*
    **	Open data set 1-1.
	*/
    strcpy(dcb1.dsname,"UB013.I.LONG1    ");
    dcb1.volseqno = 1;
    dcb1.dsseqno = 1;
	dcb1.expirec = ' ';
	dcb1.expireyy=00;
	dcb1.expiredd=00;

	dcb1.recfm = 'U';
	dcb1.blocklen = 32001;
	dcb1.reclen = 0;
	dcb1.blkattr = 'B';

    dcb1.read = 0;
    dcb1.block_mode = 0;
    dcb1.buffered_mode = 1;
    dcb1.num_tapeblk_bufs = 3;


	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", node, pid, dcb1.dsname, dcb1.dsseqno);
    rc1=tdsopen(fd1 ,&dcb1);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc1=%d\n\n", node, pid,rc1);
#endif
	if (rc1 < 0) {
		printf("Node %d, Pid %d:  *** tdsopen error=%d - aborting\n", node, pid , rc1);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}


	/*
	**	Write some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting twrite loop\n", node, pid);
	for (j=1; j<2; j++) {
		for (i=1; i<101; i++) {
			if (eov1+eov0!=2) j=0;
			buf[0]=i;
			buf[1]=0;
			len=twrite(fd0, buf, 32000);
			if (len != 32000) {
				printf("Node %d, Pid %d:  *** twrite 0 error: len=%d i=%d j=%d - aborting\n", node, pid , len,i,j);
				printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
				exit(1);
			}
			buf[1]=1;
			len=twrite(fd1, buf, 32001);
			if (len != 32001) {
				printf("Node %d, Pid %d:  *** twrite 1 error: len=%d i=%d j=%d - aborting\n", node, pid , len,i,j);
				printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
				exit(1);
			}
		}
	}


	/*
	**	Close Data Set 0-1.
	*/
	printf("Node %d, Pid %d:  calling tdsclose 0\n", node, pid);
	rc0=tdsclose(fd0);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsclose 0 rc0=%d\n\n", node, pid,rc0);
#endif
	if (rc0 < 0) {
		printf("Node %d, Pid %d:  *** tdsclose 0 error=%d - aborting\n", node, pid , rc0);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}

	/*
	**	One last write, then Close Data Set 1-1.
	*/
	buf[0]=1;
	buf[1]=1;
	len=twrite(fd1, buf, 32001);
	if (len != 32001) {
		printf("Node %d, Pid %d:  *** twrite 1+ error: len=%d i=%d j=%d - aborting\n", node, pid , len,i,j);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}
	printf("Node %d, Pid %d:  calling tdsclose 1\n", node, pid);
	rc1=tdsclose(fd1);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsclose 1 rc1=%d\n\n", node, pid,rc1);
#endif
	if (rc1 < 0) {
		printf("Node %d, Pid %d:  *** tdsclose 1 error=%d - aborting\n", node, pid , rc1);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}
#endif


/*PAGE*/
    /*
    **	Open data set 2.
	*/
    strcpy(dcb0.dsname,"UB11B.LONG0      ");
    dcb0.volseqno = 2;
    dcb0.dsseqno = 2;
	dcb0.expirec = ' ';
	dcb0.expireyy=00;
	dcb0.expiredd=00;

	dcb0.recfm = 'U';
	dcb0.blocklen = 32000;
	dcb0.reclen = 0;
	dcb0.blkattr = 'B';

    dcb0.read = 0;
    dcb0.buffered_mode = 1;
    dcb0.block_mode = 1;
    dcb0.num_tapeblk_bufs = 1;


	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", node, pid, dcb0.dsname, dcb0.dsseqno);
    rc0=tdsopen(fd0 ,&dcb0);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc0=%d\n\n", node, pid,rc0);
#endif
	if (rc0 < 0) {
		printf("Node %d, Pid %d:  *** tdsopen error=%d - aborting\n", node, pid , rc0);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}


	/*
	**	Write some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting twrite loop\n", node, pid);
	for (j=1; j<2; j++) {
		for (i=1; i<101; i++) {
			buf[0]=i;
			if (eov1+eov0!=3) j=0;
			len=twrite(fd0, buf, 32000);
			if (len != 32000) {
				printf("Node %d, Pid %d:  *** twrite error: len=%d i=%d j=%d - aborting\n", node, pid , len,i,j);
				printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
				exit(1);
			}
		}
	}


	/*
	**	Close Data Set.
	*/
	printf("Node %d, Pid %d:  calling tdsclose\n", node, pid);
	rc0=tdsclose(fd0);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsclose rc0=%d\n\n", node, pid,rc0);
#endif
	if (rc0 < 0) {
		printf("Node %d, Pid %d:  *** tdsclose error=%d - aborting\n", node, pid , rc0);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}

/*PAGE*/
	/*
	**	Deallocate tape 0.
	*/
	printf("Node %d, Pid %d:  calling tapedealloc 0\n", node, pid);
	rc0=tapedealloc(fd0);
#if DEBUG
	printf("Node %d, Pid %d:  return from tapedealloc 0 rc0=%d\n\n", node, pid,rc0);
#endif
	if (rc0 < 0) {
		printf("Node %d, Pid %d:  *** tapedealloc 0 error=%d - aborting\n", node, pid , rc0);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}

	/*
	**	Deallocate tape 1.
	*/
	printf("Node %d, Pid %d:  calling tapedealloc 1\n", node, pid);
	rc1=tapedealloc(fd1);
#if DEBUG
	printf("Node %d, Pid %d:  return from tapedealloc 1 rc1=%d\n\n", node, pid,rc1);
#endif
	if (rc1 < 0) {
		printf("Node %d, Pid %d:  *** tapedealloc 1 error=%d - aborting\n", node, pid , rc1);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}

	printf("Node %d, Pid %d:  User exit 0 counts: code4=%d, code5=%d, code6=%d, code7=%d\n", node, pid,code04, code05, code06, code07);
	printf("Node %d, Pid %d:  User exit 1 counts: code4=%d, code5=%d, code6=%d, code7=%d\n", node, pid,code14, code15, code16, code17);
	if (UEXITRET0 == 1) {
		if ((code04 != 4*8)||(code05 != 4*8)) {
			printf("Node %d, Pid %d:  *** incorrect user exit04/5 counts - aborting\n", node, pid );
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
	} else {
		if ((code04 != 4)||(code05 != 4)) {
			printf("Node %d, Pid %d:  *** incorrect user exit04/5 counts - aborting\n", node, pid );
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
	}
	if (code06 != 2) {
		printf("Node %d, Pid %d:  *** incorrect user exit06/7 counts - aborting\n", node, pid );
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}
	if (UEXITRET1 == 1) {
		if ((code14 != 2*8)||(code15 != 2*8)) {
			printf("Node %d, Pid %d:  *** incorrect user exit14/5 counts - aborting\n", node, pid );
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
	} else {
		if ((code14 != 2)||(code15 != 2)) {
			printf("Node %d, Pid %d:  *** incorrect user exit14/5 counts - aborting\n", node, pid );
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
	}
	if (code16 != 1) {
		printf("Node %d, Pid %d:  *** incorrect user exit16 counts - aborting\n", node, pid );
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}
	printf("\nNode %d, Pid %d:  Test PASSED\n", node, pid);
    exit(0);
}
