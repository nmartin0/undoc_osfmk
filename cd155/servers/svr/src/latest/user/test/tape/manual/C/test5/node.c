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
 *  test5.c
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
#define	TEST0 "TEST0 "
#define	MAXTAPES	2
#define LENGTH (31*1024)

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <t3480.h>

VOLSER	volser[MAXTAPES];
DCB 	dcb;
VCB 	vcb;
char 	tapedev[80];
char	poop[10];
int		fd,
		rc,
		i, j;
char	buf[LENGTH];
int		len;
int		node,
		pid;
char	lnum='1';
int		eov=0;
int		errprnt=0;

int uexit(code, exitparm)
int			code;
EXITPARM   *exitparm;
{
	printf("Node %d, Pid %d:  UEXIT:  called with value %d\n", node, pid,code);
	return(1);
}

main(ac, av)
	int ac;
	char **av;

 {
	node=mynode();
	pid=mypid();
	/*
	**	Vol/Ser.
	*/
	/* printf("Node %d, Pid %d:  Volume Serial Number: xxxxxx\n", node, pid); */
	for (i=0; i<MAXTAPES-1; i++) {
		/* printf("Node %d, Pid %d:                     %d: ", node, pid, i); */
		/* printf("%.5s%1d\b\b\b\b\b\b", TEST0, i+1); */
		/* gets(volser[i].serialno); */
		if (volser[i].serialno[0] == 0) {
			strcpy(volser[i].serialno, TEST0);
			volser[i].serialno[5]=i+0x31;
		}
		volser[i].serialno[6] = NULL;
	}
	volser[MAXTAPES-1].serialno[0] = NULL;

#if DEBUG
	if (node==0) {
		for (i=0; i<MAXTAPES; i++) {
			printf("Node %d, Pid %d:  volser %2d = %s\n", node, pid, i, volser[i].serialno);
			if (volser[i].serialno[0] == 0)
				break;
		}
		printf("Node %d, Pid %d:  \n", node, pid);
	}
#endif

    /*
    **	Tape device.
	*/
	if (node==0) {
		if (ac<2) {
			strcpy(tapedev, TAPEDEV);
		} else {
			strcpy(tapedev, av[1]);
		}
		printf("Node %d, Pid %d:  tapedev=%s\n", node, pid,tapedev);
		csend(1, tapedev, sizeof(tapedev), -1, -1);
	} else {
		crecv(1, tapedev, sizeof(tapedev));
	}

/*PAGE*/
	/*
	**  Allocate tape drive.
	*/
    vcb.vollist = volser;
    vcb.exlst = &uexit;

	printf("Node %d, Pid %d:  node %d calling tapealloc\n", node, pid,node);
pop:
    fd = tapealloc(&vcb, tapedev);
#if DEBUG
	if ((errprnt == 0) ||(fd>=0))
		printf("Node %d, Pid %d:  return from node %d tapealloc fd=%d\n\n", node, pid, node, fd);
#endif
	if (fd < 0) {
		if (fd != TERR_COULD_NOT_RESERVE_TAPE) {
			printf("Node %d, Pid %d:  *** node %d tapealloc error=%d - expecting - aborting\n", node, pid , node, fd, TERR_COULD_NOT_RESERVE_TAPE);
			printf("\nNode %d, Pid %d:  Test FAILED\n", node, pid);
			exit(1);
		}

		if (errprnt == 0) {
			printf("Node %d, Pid %d:  *** node %d tapealloc return=%d - waiting\n", node, pid , node, fd);
			errprnt++;
		}
		goto pop;
	}

	printf("Node %d, Pid %d:  Press <cr> to release node %d\n", node, pid,node);
	gets(poop);

	if (node==0) {
		printf("Node %d, Pid %d:  Node 0 exiting without deallocate\n", node, pid);
		printf("\nNode %d, Pid %d:  Test PASSED\n", node, pid);
		exit(0);
	}

/*PAGE*/
	/*
	**	Deallocate tape.
	*/
	printf("Node %d, Pid %d:  node %d calling tapedealloc\n", node, pid,node);
	rc=tapedealloc(fd);
#if DEBUG
	printf("Node %d, Pid %d:  return from node %d tapedealloc rc=%d\n\n", node, pid,node,rc);
#endif
	if (rc < 0) {
		printf("Node %d, Pid %d:  *** tapedealloc node %d error=%d - aborting\n", node, pid ,node, rc);
		exit(1);
	}


	for(i=0; i<10000000; i++) j=i/3+9;

	printf("\nNode %d, Pid %d:  Test PASSED\n", node, pid);
}
