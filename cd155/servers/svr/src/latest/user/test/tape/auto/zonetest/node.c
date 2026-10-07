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
#include <malloc.h>
#include <ctype.h>
#include <convec.h>

void itoa(n, s)

int		n;
char	s[];
{
	int i,j,c,sign;

	if ((sign=n) < 0)
		n=0-n;

	i=0;
	do {
		s[i++]=n%10+'0';
	} while ((n /=10) > 0);

	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';

	for (i=0, j=strlen(s)-1; i<j; i++, j--) {
		c=s[i];
		s[i]=s[j];
		s[j]=c;
	}
}

main() {

int i,j,err,rc;
unsigned char array[30];
char zarray[30];
char iarray[30];

err=0;


/*---------------------------------------------------------------*/
printf("Node %d, Pid %d:  Limit testing itoz/ztoi . . .\n",mynode(),mypid());

	i=2147483647;
	rc=itoz(i, array, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz error 101, i=%d, rc=%d\n",mynode(),mypid(),i,rc);
		err=101;
	}
	rc=ztoi(array, &j, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi error 102, i=%d, rc=%d\n",mynode(),mypid(),i,rc);
		err=102;
	}
	if (i != j) {
		printf("Node %d, Pid %d:  ztoi error 103, i=%d, j=%d\n",mynode(),mypid(),i,j);
		err=103;
	}
	rc=ztoa(array, zarray, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoa error 201, i=%d, rc=%d\n",mynode(),mypid(),i,rc);
		err=201;
	}
	itoa(i, iarray);
	if (rc=strcmp(zarray, iarray) != 0) {
		printf("Node %d, Pid %d:  ztoa error 202, i=%d, zarray=%s, iarray=%s\n",mynode(),mypid(),i,zarray,iarray);
		err=202;
	}

	i=-2147483648;
	rc=itoz(i, array, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz error 105, i=%d, rc=%d\n",mynode(),mypid(),i,rc);
		err=101;
	}
	rc=ztoi(array, &j, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi error 106, i=%d, rc=%d\n",mynode(),mypid(),i,rc);
		err=102;
	}
	if (i != j) {
		printf("Node %d, Pid %d:  ztoi error 107, i=%d, j=%d\n",mynode(),mypid(),i,j);
		err=103;
	}
	rc=ztoa(array, zarray, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoa error 203, i=%d, rc=%d\n",mynode(),mypid(),i,rc);
		err=203;
	}
	i=-2147483647;
	itoa(i, iarray);
	iarray[10]++;
	if (rc=strcmp(zarray, iarray) != 0) {
		printf("Node %d, Pid %d:  ztoa error 204, i=%d, zarray=%s, iarray=%s\n",mynode(),mypid(),i,zarray,iarray);
		err=204;
	}

	array[10]++;
	rc=ztoi(array, &j, 10);
	if (rc != CERR_OVERFLOW) {
		printf("Node %d, Pid %d:  ztoi error 108, i=%d, rc=%d (expecting %d)\n",mynode(),mypid(),i,rc,CERR_OVERFLOW);
		err=108;
	}
	array[9]=0xff;
	rc=ztoa(array, zarray, 10);
	if (rc != CERR_INVALID_ZONED_NUM) {
		printf("Node %d, Pid %d:  ztoa error 205, i=%d, rc=%d (expecting %d)\n",mynode(),mypid(),i,rc,CERR_INVALID_ZONED_NUM);
		err=205;
	}

/*---------------------------------------------------------------*/
printf("Node %d, Pid %d:  Negative testing itoz/ztoi . . .\n",mynode(),mypid());

	rc=itoz(-10, array, 1);
	if (rc != CERR_OVERFLOW_ZONE_LEN) {
		printf("Node %d, Pid %d:  itoz err 1 rc=%d (expecting %d)\n",mynode(),mypid(),rc,CERR_OVERFLOW_ZONE_LEN);
		err=1;
	}
	rc=itoz(10, array, 1);
	if (rc != CERR_OVERFLOW_ZONE_LEN) {
		printf("Node %d, Pid %d:  itoz err 2 rc=%d (expecting %d)\n",mynode(),mypid(),rc,CERR_OVERFLOW_ZONE_LEN);
		err=2;
	}

	rc=itoz(-1000, array, 3);
	if (rc != CERR_OVERFLOW_ZONE_LEN) {
		printf("Node %d, Pid %d:  itoz err 3 rc=%d (expecting %d)\n",mynode(),mypid(),rc,CERR_OVERFLOW_ZONE_LEN);
		err=3;
	}
	rc=itoz(1000, array, 3);
	if (rc != CERR_OVERFLOW_ZONE_LEN) {
		printf("Node %d, Pid %d:  itoz err 4 rc=%d (expecting %d)\n",mynode(),mypid(),rc,CERR_OVERFLOW_ZONE_LEN);
		err=4;
	}

	rc=itoz(-10000000, array, 7);
	if (rc != CERR_OVERFLOW_ZONE_LEN) {
		printf("Node %d, Pid %d:  itoz err 5 rc=%d (expecting %d)\n",mynode(),mypid(),rc,CERR_OVERFLOW_ZONE_LEN);
		err=5;
	}
	rc=itoz(10000000, array, 7);
	if (rc != CERR_OVERFLOW_ZONE_LEN) {
		printf("Node %d, Pid %d:  itoz err 6 rc=%d (expecting %d)\n",mynode(),mypid(),rc,CERR_OVERFLOW_ZONE_LEN);
		err=6;
	}

	rc=ztoi(array, &j, 3);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 7 rc=%d\n",mynode(),mypid(),rc);
		err=7;
	}
	if (j != 00) {
		printf("Node %d, Pid %d:  ztoi err 8 j=%d\n",mynode(),mypid(),j);
		err=8;
	}

	for (i=0; i<30; i++) array[i] = 0xF1;
	rc=ztoi(array, &j, 30);
	if (rc != CERR_OVERFLOW) {
		printf("Node %d, Pid %d:  ztoi err 9 rc=%d (expecting %d)\n",mynode(),mypid(),rc,CERR_OVERFLOW);
		err=9;
	}

	rc=itoz(1, array, 9);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 10 rc=%d\n",mynode(),mypid(),rc);
		err=10;
	}
	rc=ztoi(array, &j, 9);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 11 rc=%d\n",mynode(),mypid(),rc);
		err=11;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 12 j=%d\n",mynode(),mypid(), j);
		err=12;
	}

	rc=itoz(1, array, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 13 rc=%d\n",mynode(),mypid(),rc);
		err=13;
	}
	rc=ztoi(array, &j, 10);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 14 rc=%d\n",mynode(),mypid(),rc);
		err=14;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 15 j=%d\n",mynode(),mypid(), j);
		err=15;
	}

	rc=itoz(1, array, 11);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 16 rc=%d\n",mynode(),mypid(),rc);
		err=16;
	}
	rc=ztoi(array, &j, 11);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 17 rc=%d\n",mynode(),mypid(),rc);
		err=17;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 18 j=%d\n",mynode(),mypid(), j);
		err=18;
	}

	rc=itoz(1, array, 12);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 19 rc=%d\n",mynode(),mypid(),rc);
		err=19;
	}
	rc=ztoi(array, &j, 12);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 20 rc=%d\n",mynode(),mypid(),rc);
		err=20;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 21 j=%d\n",mynode(),mypid(), j);
		err=21;
	}

	rc=itoz(1, array, 13);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 22 rc=%d\n",mynode(),mypid(),rc);
		err=22;
	}
	rc=ztoi(array, &j, 13);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 23 rc=%d\n",mynode(),mypid(),rc);
		err=23;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 24 j=%d\n",mynode(),mypid(), j);
		err=24;
	}

	rc=itoz(1, array, 14);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 25 rc=%d\n",mynode(),mypid(),rc);
		err=25;
	}
	rc=ztoi(array, &j, 14);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 26 rc=%d\n",mynode(),mypid(),rc);
		err=26;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 27 j=%d\n",mynode(),mypid(), j);
		err=27;
	}

	rc=itoz(1, array, 15);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 28 rc=%d\n",mynode(),mypid(),rc);
		err=28;
	}
	rc=ztoi(array, &j, 15);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 29 rc=%d\n",mynode(),mypid(),rc);
		err=29;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 30 j=%d\n",mynode(),mypid(), j);
		err=30;
	}

	rc=itoz(1, array, 16);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 31 rc=%d\n",mynode(),mypid(),rc);
		err=31;
	}
	rc=ztoi(array, &j, 16);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 32 rc=%d\n",mynode(),mypid(),rc);
		err=32;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 33 j=%d\n",mynode(),mypid(), j);
		err=33;
	}

	rc=itoz(1, array, 17);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 34 rc=%d\n",mynode(),mypid(),rc);
		err=34;
	}
	rc=ztoi(array, &j, 17);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 35 rc=%d\n",mynode(),mypid(),rc);
		err=35;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 36 j=%d\n",mynode(),mypid(), j);
		err=36;
	}

	rc=itoz(1, array, 18);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 37 rc=%d\n",mynode(),mypid(),rc);
		err=37;
	}
	rc=ztoi(array, &j, 18);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 38 rc=%d\n",mynode(),mypid(),rc);
		err=38;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 39 j=%d\n",mynode(),mypid(), j);
		err=39;
	}

	rc=itoz(1, array, 19);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 40 rc=%d\n",mynode(),mypid(),rc);
		err=40;
	}
	rc=ztoi(array, &j, 19);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 41 rc=%d\n",mynode(),mypid(),rc);
		err=41;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 42 j=%d\n",mynode(),mypid(), j);
		err=42;
	}

	rc=itoz(1, array, 20);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 43 rc=%d\n",mynode(),mypid(),rc);
		err=43;
	}
	rc=ztoi(array, &j, 20);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 44 rc=%d\n",mynode(),mypid(),rc);
		err=44;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 45 j=%d\n",mynode(),mypid(), j);
		err=45;
	}

	rc=itoz(1, array, 21);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 46 rc=%d\n",mynode(),mypid(),rc);
		err=46;
	}
	rc=ztoi(array, &j, 21);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 47 rc=%d\n",mynode(),mypid(),rc);
		err=47;
	}
	if (j != 1) {
		printf("Node %d, Pid %d:  ztoi err 48 j=%d\n",mynode(),mypid(), j);
		err=48;
	}

	array[10]=0xFA;
	rc=ztoi(array, &j, 21);
	if (rc != CERR_INVALID_ZONED_NUM) {
		printf("Node %d, Pid %d:  ztoi err 49 rc=%d (expecting %d)\n",mynode(),mypid(),rc,CERR_INVALID_ZONED_NUM);
		err=49;
	}
/*---------------------------------------------------------------*/

printf("Node %d, Pid %d:  Testing itoz/ztoi/ztoa . . .\n",mynode(),mypid());
for (i=-999; i<999; i++) {
	rc=itoz(i, array, 3);
	if (rc != 0) {
		printf("Node %d, Pid %d:  itoz err 50 rc=%d at i=%d\n",mynode(),mypid(), rc, i);
		err=50;
	}
	rc=ztoi(array, &j, 3);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoi err 51 rc=%d at i=%d\n",mynode(),mypid(), rc, i);
		err=51;
	}
	if (j != i) {
		printf("Node %d, Pid %d:  ztoi err 52 at i=%d j=%d\n",mynode(),mypid(), i, j);
		err=52;
	}
	rc=ztoa(array, zarray, 3);
	if (rc != 0) {
		printf("Node %d, Pid %d:  ztoa error 206, i=%d, rc=%d\n",mynode(),mypid(),i,rc);
		err=206;
	}
	itoa(i, iarray);
	if (rc=strcmp(zarray, iarray) != 0) {
		printf("Node %d, Pid %d:  ztoa error 207, i=%d, zarray=%s, iarray=%s\n",mynode(),mypid(),i,zarray,iarray);
		err=207;
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
