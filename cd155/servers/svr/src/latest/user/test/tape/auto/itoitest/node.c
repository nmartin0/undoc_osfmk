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
#include <malloc.h>
#include <ctype.h>
main() {

unsigned short int i,j,k,l,err;
unsigned int m,n,o,p,q,r,s;
err=0;
/*---------------------------------------------------------------*/

printf("Node %d, Pid %d:  Testing ItoI_16 . . .\n",mynode(),mypid());
for (j=256; j != 0 ; j+=256) {
	for (i=0; i < 256; i++) {
		k=i+j;
		if (Itoi_16(k) != i*256+j/256) {
			printf("Node %d, Pid %d:  Itoi_16 error 1 on %x\n",mynode(),mypid(),k);
			printf("Node %d, Pid %d:  itoi=%x, myxalc=%x\n",mynode(),mypid(),Itoi_16(k),i*256+j/256);
			err=1;
		}
		if (itoI_16(k) != i*256+j/256) {
			printf("Node %d, Pid %d:  itoI_16 error 2 on %x\n",mynode(),mypid(),k);
			printf("Node %d, Pid %d:  itoi=%x, myxalc=%x\n",mynode(),mypid(),itoI_16(k),i*256+j/256);
			err=2;
		}
		if (Itoi_16(k) != itoI_16(k)) {
			printf("Node %d, Pid %d:  ItoI_16 error 3 on %x\n",mynode(),mypid(),k);
			err=3;
		}
		l=itoI_16(k);
		if (Itoi_16(l) != k) {
			printf("Node %d, Pid %d:  ItoI_16 error 4 on %x\n",mynode(),mypid(),k);
			err=4;
		}
		l=Itoi_16(k);
		if (itoI_16(l) != k) {
			printf("Node %d, Pid %d:  ItoI_16 error 5 on %x\n",mynode(),mypid(),k);
			err=5;
		}
	}
}

/*---------------------------------------------------------------*/

printf("Node %d, Pid %d:  Testing ItoI_32 . . .\n",mynode(),mypid());
i=mynode();
j=256/numnodes();
printf("Node %d, Pid %d:  node %d: checking %d to %d\n",mynode(),mypid(),i,i*j,(i+1)*j-1);
for (p=i*j; p < (i+1)*j ; p++) {
for (o=0; o < 256 ; o++) {
for (n=0; n < 256 ; n++) {
	for (m=0; m < 256; m++) {
		q=(p*256*65536)+(o*65536)+(n*256)+m;
		s=(m*256*65536)+(n*65536)+(o*256)+p;
		if (Itoi_32(q) != s) {
			printf("Node %d, Pid %d:  Itoi_32 error 11 on %x %x\n",mynode(),mypid(),q, s);
			err=11;
		}
		if (itoI_32(q) != s) {
			printf("Node %d, Pid %d:  itoI_32 error 12 on %x %x\n",mynode(),mypid(),q, s);
			err=12;
		}
		if (Itoi_32(q) != itoI_32(q)) {
			printf("Node %d, Pid %d:  ItoI_32 error 13 on %x\n",mynode(),mypid(),q);
			err=13;
		}
		r=itoI_32(q);
		if (Itoi_32(r) != q) {
			printf("Node %d, Pid %d:  ItoI_32 error 14 on %x %x\n",mynode(),mypid(),q, r);
			err=14;
		}
		r=Itoi_32(q);
		if (itoI_32(r) != q) {
			printf("Node %d, Pid %d:  ItoI_32 error 15 on %x %x\n",mynode(),mypid(),q, r);
			err=15;
		}
	}
}
}
printf("Node %d, Pid %d:  %d of 256\n",mynode(),mypid(),p);}

/*---------------------------------------------------------------*/

if (err) {
	printf("Node %d, Pid %d:  Test FAILED, last error=%d\n",mynode(),mypid(),err);
} else {
	printf("Node %d, Pid %d:  Test PASSED\n",mynode(),mypid());
}
exit(err);
}
