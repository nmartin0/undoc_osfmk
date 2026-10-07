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
 *	
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <convec.h>

main() {

typedef struct esize_t {
	long slow;
	long shigh;
} esize_t;


esize_t	i,j;
int		k,err,rc;
char	zarray[30];
char	iarray[30];
unsigned char array[30];

err=0;


/*---------------------------------------------------------------*/
printf("Node %d, Pid %d:  Limit testing eitoz/ztoei . . .\n",mynode(),mypid());

	i.slow=2147483647;
	i.shigh=1;
	rc=eitoz(i, array, 30);
	if (rc != 0) {
		printf("Node %d, Pid %d:  eitoz error 101, i=%x-%x, rc=%d\n",mynode(),mypid(), i.shigh, i.slow, rc);
		err=101;
	}
	rc=ztoei(array, &j, 30);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoei error 102, i=%x-%x, rc=%d\n",mynode(),mypid(), i.shigh, i.slow, rc);
		err=102;
	}
	if( ecmp(i,j) != 0) {
		printf("Node %d, Pid %d:  ztoei error 103, i=%x-%x, j=%x-%x\n",mynode(),mypid(),i.shigh,i.slow,j.shigh,i.slow);
		err=103;
	}
	array[10]++;
	rc=ztoei(array, &j, 30);
	if (rc != CERR_OVERFLOW) {
		printf("Node %d, Pid %d:  ztoei error 104, rc=%d (expecting %d)\n",mynode(),mypid(),rc,CERR_OVERFLOW);
		err=104;
	}


	i.slow=0xFFFFFFFF;
	i.shigh=100000000;
	rc=eitoz(i, array, 30);
	if (rc != 0) {
		printf("Node %d, Pid %d:  eitoz error 105, i=%x-%x, rc=%d\n",mynode(),mypid(), i.shigh, i.slow, rc);
		err=105;
	}
	rc=ztoei(array, &j, 30);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoei error 106, i=%x-%x, rc=%d\n",mynode(),mypid(), i.shigh, i.slow, rc);
		err=106;
	}
	if( ecmp(i,j) != 0) {
		printf("Node %d, Pid %d:  ztoei error 107, i=%x-%x, j=%x-%x\n",mynode(),mypid(),i.shigh,i.slow,j.shigh,i.slow);
		err=107;
	}

	i.slow=0xFFFFFFFF;
	i.shigh=-100000000;
	rc=eitoz(i, array, 30);
	if (rc != 0) {
		printf("Node %d, Pid %d:  eitoz error 109, i=%x-%x, rc=%d\n",mynode(),mypid(), i.shigh, i.slow, rc);
		err=109;
	}
	rc=ztoei(array, &j, 30);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoei error 110, i=%x-%x, rc=%d\n",mynode(),mypid(), i.shigh, i.slow, rc);
		err=110;
	}
	if( ecmp(i,j) != 0) {
		printf("Node %d, Pid %d:  ztoei error 111, i=%x-%x, j=%x-%x\n",mynode(),mypid(),i.shigh,i.slow,j.shigh,i.slow);
		err=111;
	}
	array[10]++;
	rc=ztoei(array, &j, 30);
	if (rc != CERR_OVERFLOW) {
		printf("Node %d, Pid %d:  ztoei error 112, i=%x-%x, rc=%d (expecting %d)\n",mynode(),mypid(),i.shigh,i.slow,rc,CERR_OVERFLOW);
		err=112;
	}

	i.slow=-2147483648;
	i.shigh=0xFFFFFFFF;
	rc=eitoz(i, array, 30);
	if (rc != 0) {
		printf("Node %d, Pid %d:  eitoz error 113, i=%x-%x, rc=%d\n",mynode(),mypid(), i.shigh, i.slow, rc);
		err=113;
	}
	rc=ztoei(array, &j, 30);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoei error 114, i=%x-%x, rc=%d\n",mynode(),mypid(), i.shigh, i.slow, rc);
		err=114;
	}
	if( ecmp(i,j) != 0) {
		printf("Node %d, Pid %d:  ztoei error 115, i=%x-%x, j=%x-%x\n",mynode(),mypid(),i.shigh,i.slow,j.shigh,i.slow);
		err=115;
	}
	array[10]++;
	rc=ztoei(array, &j, 30);
	if (rc != CERR_OVERFLOW) {
		printf("Node %d, Pid %d:  ztoei error 116, i=%x-%x, rc=%d (expecting %d)\n",mynode(),mypid(),i.shigh,i.slow,rc,CERR_OVERFLOW);
		err=116;
	}
/*---------------------------------------------------------------*/
printf("Node %d, Pid %d:  Negative testing eitoz/ztoei . . .\n",mynode(),mypid());

	i.shigh=0;
	i.slow=-10;
	rc=eitoz(i, array, 1);
	if (rc != CERR_OVERFLOW_ZONE_LEN) {
		printf("Node %d, Pid %d:  eitoz err 1 rc=%d (expectind %d)\n",mynode(),mypid(),rc, CERR_OVERFLOW_ZONE_LEN);
		err=1;
	}
	i.slow=10;
	rc=eitoz(i, array, 1);
	if (rc != CERR_OVERFLOW_ZONE_LEN) {
		printf("Node %d, Pid %d:  eitoz err 2 rc=%d (expectind %d)\n",mynode(),mypid(),rc, CERR_OVERFLOW_ZONE_LEN);
		err=2;
	}

	i.slow=-1000;
	rc=eitoz(i, array, 3);
	if (rc != CERR_OVERFLOW_ZONE_LEN) {
		printf("Node %d, Pid %d:  eitoz err 3 rc=%d (expectind %d)\n",mynode(),mypid(),rc, CERR_OVERFLOW_ZONE_LEN);
		err=3;
	}
	i.slow=1000;
	rc=eitoz(i, array, 3);
	if (rc != CERR_OVERFLOW_ZONE_LEN) {
		printf("Node %d, Pid %d:  eitoz err 4 rc=%d (expectind %d)\n",mynode(),mypid(),rc, CERR_OVERFLOW_ZONE_LEN);
		err=4;
	}

	i.slow=-10000000;
	rc=eitoz(i, array, 7);
	if (rc != CERR_OVERFLOW_ZONE_LEN) {
		printf("Node %d, Pid %d:  eitoz err 5 rc=%d (expectind %d)\n",mynode(),mypid(),rc, CERR_OVERFLOW_ZONE_LEN);
		err=5;
	}
	i.slow=10000000;
	rc=eitoz(i, array, 7);
	if (rc != CERR_OVERFLOW_ZONE_LEN) {
		printf("Node %d, Pid %d:  eitoz err 6 rc=%d (expectind %d)\n",mynode(),mypid(),rc, CERR_OVERFLOW_ZONE_LEN);
		err=6;
	}

	for (k=0; k<30; k++) array[k] = 0xF1;
	rc=ztoei(array, &j, 30);
	if (rc != CERR_OVERFLOW) {
		printf("Node %d, Pid %d:  ztoei err 9 rc=%d (expectind %d)\n",mynode(),mypid(),rc, CERR_OVERFLOW);
		err=9;
	}

	i.slow=1;
	rc=eitoz(i, array, 9);
	if (rc != 0) {
		printf("Node %d, Pid %d:  eitoz err 10 rc=%d\n",mynode(),mypid(),rc);
		err=10;
	}
	rc=ztoei(array, &j, 9);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoei err 11 rc=%d\n",mynode(),mypid(),rc);
		err=11;
	}
	if (ecmp(i,j) != 0) {
		printf("Node %d, Pid %d:  ztoei err 12 j=%x-%x\n",mynode(),mypid(), j.shigh,j.slow);
		err=12;
	}

	rc=eitoz(i, array, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  eitoz err 13 rc=%d\n",mynode(),mypid(),rc);
		err=13;
	}
	rc=ztoei(array, &j, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoei err 14 rc=%d\n",mynode(),mypid(),rc);
		err=14;
	}
	if (ecmp(i,j) != 0) {
		printf("Node %d, Pid %d:  ztoei err 15 j=%x-%x\n",mynode(),mypid(), j.shigh,j.slow);
		err=15;
	}

	rc=eitoz(i, array, 11);
	if (rc != 0) {
		printf("Node %d, Pid %d:  eitoz err 16 rc=%d\n",mynode(),mypid(),rc);
		err=16;
	}
	rc=ztoei(array, &j, 11);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoei err 17 rc=%d\n",mynode(),mypid(),rc);
		err=17;
	}
	if (ecmp(i,j) != 0) {
		printf("Node %d, Pid %d:  ztoei err 18 j=%x-%x\n",mynode(),mypid(), j.shigh,j.slow);
		err=18;
	}

	rc=eitoz(i, array, 27);
	if (rc != 0) {
		printf("Node %d, Pid %d:  eitoz err 46 rc=%d\n",mynode(),mypid(),rc);
		err=46;
	}
	rc=ztoei(array, &j, 27);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoei err 47 rc=%d\n",mynode(),mypid(),rc);
		err=47;
	}
	if (ecmp(i,j) != 0) {
		printf("Node %d, Pid %d:  ztoei err 48 j=%x-%x\n",mynode(),mypid(), j.shigh,j.slow);
		err=48;
	}

	array[10]=0xFA;
	rc=ztoei(array, &j, 27);
	if (rc != CERR_INVALID_ZONED_NUM) {
		printf("Node %d, Pid %d:  ztoei err 49 rc=%d (expecting %d)\n",mynode(),mypid(),rc,CERR_INVALID_ZONED_NUM);
		err=49;
	}
/*---------------------------------------------------------------*/

printf("Node %d, Pid %d:  Testing eitoz/ztoei . . .\n",mynode(),mypid());
i.shigh=0;
for (i.slow=-999; i.slow<999; i.slow++) {
	rc=eitoz(i, array, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  eitoz err 50 rc=%d at i=%x-%x\n",mynode(),mypid(), rc, i.shigh, i.slow);
		err=50;
	}
	rc=ztoei(array, &j, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoei err 51 rc=%d at i=%x-%x\n",mynode(),mypid(), rc, i.shigh, i.slow);
		err=51;
	}
	if( ecmp(i,j) != 0) {
		printf("Node %d, Pid %d:  ztoei err 52 at i=%x-%x j=%x-%x\n",mynode(),mypid(), i.shigh, i.slow, j.shigh, j.slow);
		err=52;
	}
}

i.shigh=-1;
for (i.slow=-999; i.slow<0; i.slow++) {
	rc=eitoz(i, array, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  eitoz err 53 rc=%d at i=%x-%x\n",mynode(),mypid(), rc, i.shigh, i.slow);
		err=53;
	}
	rc=ztoei(array, &j, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoei err 54 rc=%d at i=%x-%x\n",mynode(),mypid(), rc, i.shigh, i.slow);
		err=54;
	}
	if( ecmp(i,j) != 0) {
		printf("Node %d, Pid %d:  ztoei err 55 at i=%x-%x j=%x-%x\n",mynode(),mypid(), i.shigh, i.slow, j.shigh, j.slow);
		err=55;
	}
}

i.shigh=100;
for (i.slow=-999; i.slow<999; i.slow++) {
	rc=eitoz(i, array, 12);
	if (rc != 0) {
		printf("Node %d, Pid %d:  eitoz err 56 rc=%d at i=%x-%x\n",mynode(),mypid(), rc, i.shigh, i.slow);
		err=56;
	}
	rc=ztoei(array, &j, 12);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoei err 57 rc=%d at i=%x-%x\n",mynode(),mypid(), rc, i.shigh, i.slow);
		err=57;
	}
	if( ecmp(i,j) != 0) {
		printf("Node %d, Pid %d:  ztoei err 58 at i=%x-%x j=%x-%x\n",mynode(),mypid(), i.shigh, i.slow, j.shigh, j.slow);
		err=58;
	}
}

/*---------------------------------------------------------------*/
if (err) {
	printf("Node %d, Pid %d:  Test FAILED, last error=%d\n",mynode(),mypid(),err);
} else {
	printf("Node %d, Pid %d:  Test PASSED\n",mynode(),mypid());
}
exit(err);
}
