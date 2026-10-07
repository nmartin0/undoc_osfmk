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
 *  test14b.c
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
 *	to make it consistent with the LENGTH value in "test14a" althought it
 * 	is not necessary.  (see "test14a/node.c" comment)              
 *
 *----------------------------------------------------------------------------*/
#define DEBUG 1
#define TAPEDEV "/cfs/ctape0"
#define OWNER "Greg Tensa"
#define	TEST0 "TEST0 "
#define	MAXTAPES	2
#define LENGTH (31*1024)
#define IMAX 101
#define JMAX 4

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
int		len;
int		node,
		pid;
char	lnum='1';
int		eov=0;


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
    printf("Node %d, Pid %d:  \nUEXIT:  called with value %d\n", node, pid,code);
/*  printf("Node %d, Pid %d:  volseqno=%d, volser=%s, dsname=%s\n", node, pid, exitparm->volseqno, exitparm->volser, exitparm->dsname);
	printf("Node %d, Pid %d:  dsseqno=%d, bufferhd=%x, buffer=%x\n", node, pid, exitparm->dsseqno, exitparm->bufferhd, exitparm->buffer);
*/	if ((code == 4)||(code==5)) {
		strcpy(exitparm->buffer, "UHLx This is a user label                                                 x");
		stratoe(exitparm->buffer, exitparm->buffer);
		exitparm->buffer[3] = atoe((char) lnum);
		lnum++;
		if (lnum == '9') lnum='1';
	}
	if (code == 5) exitparm->buffer[1] = atoe((char) 'T');
	if (code == 6) eov=1;
    return(0);

}

main(ac, av)
	int ac;
	char **av;

 {
	node=mynode();
	pid=mypid();
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
	set_tapedev(fd, 0, 1);	/* 4980, no unload ### */


    dcb.read = 0;
	dcb.expirec = ' ';
	dcb.expireyy=00;
	dcb.expiredd=00;


/*PAGE*/
    /*
    **	Open data set 2.
	*/
    strcpy(dcb.dsname,"VB014.X104.X500  ");
    dcb.dsseqno=2;
    dcb.volseqno=1;

	dcb.recfm = 'V';
	dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
	dcb.reclen = 104;
	dcb.blocklen = 500;

	printf("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", node, pid, dcb.dsname, dcb.dsseqno);
    rc=tdsopen(fd ,&dcb);
#if DEBUG
	printf("Node %d, Pid %d:  return from tdsopen rc=%d\n", node, pid,rc);
#endif
	if (rc >= 0) {
		printf("Node %d, Pid %d:  *** tdsopen missed error=%d - aborting\n", node, pid , rc);
		errexit();
	}


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
