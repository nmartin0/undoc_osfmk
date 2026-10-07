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
#include <sys/types.h>
#include <estat.h>

esize_t esub(esize_t a, esize_t b);    /* a - b */
esize_t	j, start, end, delta;
char	array[30];
int 	rc;
#include <ctype.h>
main() {

/*---------------------------------------------------------------*/

printf("Node %d, Pid %d:  Testing . . .\n",mynode(),mypid());

j.shigh=0;
j.slow=0x1234;
hwclock(&start);
rc=eitop(j, array, 4);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  eitop          delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0x1234, 4)              = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);
if (rc != 0) printf("Node %d, Pid %d:  WARNING:  rc=%d\n",mynode(),mypid(),rc);

hwclock(&start);
rc=ptoei(array, &j, 4);
hwclock(&end);

if ((j.slow != 0x1234)||(j.shigh!=0)) printf("Node %d, Pid %d:  j=%x-%x\n",mynode(),mypid(),j.shigh,j.slow);
delta = esub(end,start);
printf("Node %d, Pid %d:  ptoei          delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0x1234, 4)              = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);
if (rc != 0) printf("Node %d, Pid %d:  WARNING:  rc=%d\n",mynode(),mypid(),rc);

/*------*/

j.shigh=1000;
j.slow=0x12345678;
hwclock(&start);
rc=eitop(j, array, 30);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  eitop          delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (1000,0x12345678, 30)    = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);
if (rc != 0) printf("Node %d, Pid %d:  WARNING:  rc=%d\n",mynode(),mypid(),rc);

hwclock(&start);
rc=ptoei(array, &j, 30);
hwclock(&end);

if ((j.slow != 0x12345678)||j.shigh!=1000) printf("Node %d, Pid %d:  j=%x-%x\n",mynode(),mypid(),j.shigh,j.slow);
delta = esub(end,start);
printf("Node %d, Pid %d:  ptoei          delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (1000,0x12345678, 30)    = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);
if (rc != 0) printf("Node %d, Pid %d:  WARNING:  rc=%d\n",mynode(),mypid(),rc);

/*------*/

j.shigh=-1;
j.slow=0xf2345678;
hwclock(&start);
rc=eitop(j, array, 10);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  eitop          delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (-1,0xf2345678, 10)      = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);
if (rc != 0) printf("Node %d, Pid %d:  WARNING:  rc=%d\n",mynode(),mypid(),rc);

hwclock(&start);
rc=ptoei(array, &j, 10);
hwclock(&end);

if ((j.slow != 0xf2345678)||(j.shigh != -1)) printf("Node %d, Pid %d:  j=%x-%x\n",mynode(),mypid(),j.shigh,j.slow);
delta = esub(end,start);
printf("Node %d, Pid %d:  ptoei          delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (-1,0xf2345678, 10)      = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);
if (rc != 0) printf("Node %d, Pid %d:  WARNING:  rc=%d\n",mynode(),mypid(),rc);

/*---------------------------------------------------------------*/

printf("Node %d, Pid %d:                         complete\n",mynode(),mypid());
printf("Node %d, Pid %d:  Test PASSED\n",mynode(),mypid());
}
