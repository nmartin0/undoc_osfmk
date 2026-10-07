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
 *  test3a.c
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
 *	The LENGTH constant must be changed from 33*1024 to 32*1024-1 because
 *	the LENGTH is passed into "tread" as the "length" parameter which is
 * 	of type "int" with limit 32*1024-1 which is smaller than 33*1024.  
 *	(the value is not set to 31*1024 as the other programs because
 *	 the statement after the "tread" statement has a length value > 31*1024)
 *
 *----------------------------------------------------------------------------*/
#define DEBUG 1
#define TAPEDEV "/cfs/ctape0"
#define OWNER "Greg Tensa"
#define	TEST0 "TEST0 "
#define	MAXTAPES	5
#define LENGTH (32*1024-1)
#define VERIFY 1
#define UEXITRET 1

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <t3480.h>
#include <tioec.h>

VOLSER	volser[MAXTAPES];
DCB 	dcb;
VCB 	vcb;
char 	tapedev[80];
int		fd,
		rc,
		i,
		j;
char	buf[LENGTH];
char	verifybuf[LENGTH];
int		len;
int		node,
		pid;
int		code2=0,
		code3=0,
		code6=0,
		code7=0;
int		eov=0;

int uexit(code, exitparm)
int			code;
EXITPARM   *exitparm;
{
	int			 k;
	char		*exarray;

    printf("Node %d, Pid %d:    UEXIT value %d\n", node, pid,code);
	if ((code==2)||(code==3)) {
		if (strcmp(dcb.dsname, exitparm->dsname) != 0) {
			printf("Node %d, Pid %d:  *** incorrect dsname [%s], expected [%s] - aborting\n", node, pid, exitparm->dsname, dcb.dsname);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		if (strcmp(volser[eov].serialno, exitparm->volser) != 0) {
			printf("Node %d, Pid %d:  *** incorrect volser [%s], expected [%s] - aborting\n", node, pid, exitparm->volser, volser[eov].serialno);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		if (dcb.dsseqno != exitparm->dsseqno) {
			printf("Node %d, Pid %d:  *** incorrect dsseqno = %d, expected %d - aborting\n", node, pid, exitparm->dsseqno, dcb.dsseqno);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		if (eov+1 != exitparm->volseqno) {
			printf("Node %d, Pid %d:  *** incorrect volseqno = %d, expected %d - aborting\n", node, pid, exitparm->volseqno, dcb.volseqno);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		exarray = (char *)exitparm;
		for (k=0; k<=28; k++, exarray++) {
			if (exitparm->buffer[k] != *exarray) {
				printf("Node %d, Pid %d:  *** unexpected uexit data at byte %d=%d, expected %d - aborting\n", node, pid,
					 k, exitparm->buffer[k], *exarray);
				printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
				exit(1);
			}
		}
	} else if ((code!=6)&&(code!=7)) {
		printf("Node %d, Pid %d:  *** unexpected user exit = %d - aborting\n", node, pid, code);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}
	if (code==2) {
		if (exitparm->buffer[75] != (code2&0x7f)) {
			printf("Node %d, Pid %d:  unexpected code2 count = %d, should be %d - aborting\n", node, pid,exitparm->buffer[75], code2&0x7f);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		code2++;
	}
	if (code==3) {
		if (exitparm->buffer[75] != (code3&0x7f)) {
			printf("Node %d, Pid %d:  unexpected code3 count = %d, should be %d - aborting\n", node, pid,exitparm->buffer[75], code3&0x7f);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		code3++;
	}
	if (code==7) code7++;
	if (code==6) {
		code6++;
		eov++;
	}
    return(0);
}


#if VERIFY
verify()	{
	int		k;

	for (k=1; k<len-1; k++) {
		if (buf[k] != verifybuf[k])	{
			printf("Node %d, Pid %d:  *** data corruption error - i=%d, k=%d, expect=%d, actual=%d - aborting\n", node, pid, i, k, verifybuf[k], buf[k]);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
		buf[k]=0;
	}
}
#endif


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
	for (i=0, j=0; j<LENGTH; i++, j++) {
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
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}



/*PAGE*/
    /*
    **	Open data set 1.
	*/
    strcpy(dcb.dsname,"UB014.LONG0      ");
    dcb.volseqno = 1;
    dcb.dsseqno = 1;
	dcb.expirec = ' ';
	dcb.expireyy=00;
	dcb.expiredd=00;
    /* dcb.exlst = (char *)NULL; */

	dcb.recfm = 'U';
	dcb.blocklen = 32000;
	dcb.reclen = 0;
	dcb.blkattr = 'B';

    dcb.read = 1;
    dcb.buffered_mode = 1;
    dcb.block_mode = 0;
    dcb.num_tapeblk_bufs = 4;

	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", node, pid, dcb.dsname, dcb.dsseqno);
    rc=tdsopen(fd ,&dcb);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc=%d\n\n", node, pid,rc);
#endif
	if (rc < 0) {
		printf("Node %d, Pid %d:  *** tdsopen error=%d - aborting\n", node, pid , rc);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}


	/*
	**	Read some data.
	*/
	i=0;
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	while (len != 0) {
		len=tread(fd, buf, LENGTH);
		i++;
		if ((len != 32000) && (len != 0)) {
			printf("Node %d, Pid %d:  *** tread error: len=%d treadcnt=%d ***aborting***\n", node, pid , len,i);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
#if VERIFY
	verify();
#endif
	}
	printf("Node %d, Pid %d:  read %d records\n", node, pid, i-1);


	/*
	**	Close Data Set.
	*/
	printf("Node %d, Pid %d:  calling tdsclose\n", node, pid);
	rc=tdsclose(fd);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsclose rc=%d\n\n", node, pid,rc);
#endif
	if (rc < 0) {
		printf("Node %d, Pid %d:  *** tdsclose error=%d - aborting\n", node, pid , rc);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}


#if 1
/*PAGE*/
    /*
    **	Open data set 3.
	*/
    strcpy(dcb.dsname,"Ub019.X32760     ");
    dcb.volseqno = 4;
	eov++;
	eov++;
	if (code2) code2=40;
	if (code3) code3=40;
    dcb.dsseqno = 3;
	dcb.expirec = ' ';
	dcb.expireyy=00;
	dcb.expiredd=00;
    /* dcb.exlst = (char *)NULL; */

	dcb.recfm = 'U';
	dcb.blocklen = 32760;
	dcb.reclen = 0;
	dcb.blkattr = ' ';

    dcb.read = 1;
    dcb.buffered_mode = 1;
    dcb.block_mode = 0;
    dcb.num_tapeblk_bufs = 9;


	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", node, pid, dcb.dsname, dcb.dsseqno);
    rc=tdsopen(fd ,&dcb);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc=%d\n\n", node, pid,rc);
#endif
	if (rc < 0) {
		printf("Node %d, Pid %d:  *** tdsopen error=%d - aborting\n", node, pid , rc);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}


	/*
	**	Read some data.
	*/
	len=1;
	printf("Node %d, Pid %d:  starting tread loop\n", node, pid);
	while (len != 0) {
		len=tread(fd, buf, LENGTH);
		if ((len != 32760) & (len != 0)) {
			printf("Node %d, Pid %d:  *** tread error: len=%d i=%d ***aborting***\n", node, pid , len,i);
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
#if VERIFY
	verify();
#endif
	}
	len=tread(fd, buf, LENGTH);
	if (len != TERR_ATTEMPT_RD_PAST_EOF) {
		printf("Node %d, Pid %d:  last read returned len=%d (expecting %d)\n", node, pid, len, TERR_ATTEMPT_RD_PAST_EOF);
		errexit();
	}


	/*
	**	Close Data Set.
	*/
	printf("Node %d, Pid %d:  calling tdsclose\n", node, pid);
	rc=tdsclose(fd);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsclose rc=%d\n\n", node, pid,rc);
#endif
	if (rc < 0) {
		printf("Node %d, Pid %d:  *** tdsclose error=%d - aborting\n", node, pid , rc);
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}
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

	printf("Node %d, Pid %d:  User exit counts: code2=%d, code3=%d, code6=%d, code7=%d\n", node, pid,code2, code3, code6, code7);
	if (UEXITRET == 1) {
		if ((code2 != 6*8)||(code3 != 6*8)) {
			printf("Node %d, Pid %d:  *** incorrect user exit2/3 counts - aborting\n", node, pid );
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
	} else {
		if ((code2 != 0)||(code3 != 0)) {
			printf("Node %d, Pid %d:  *** incorrect user exit2/3 counts - aborting\n", node, pid );
			printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}
	}
	if (code6 != 1) {
		printf("Node %d, Pid %d:  *** incorrect user exit6/7 counts - aborting\n", node, pid );
		printf("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit(1);
	}
	printf("\nNode %d, Pid %d:  Test PASSED\n", node, pid);
    exit(0);
}
