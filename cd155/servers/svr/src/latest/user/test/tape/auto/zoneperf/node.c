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
/*
#include </usr/ipsc/XDEV/i860/include-ipsc/sys/types.h>
#include <estat.h>
*/

esize_t esub(esize_t a, esize_t b);    /* a - b */
esize_t	start, end, delta;
char	array[20];
char	zarray[20];
int j,rc;
#include <ctype.h>
main() {

/*---------------------------------------------------------------*/

printf("Node %d, Pid %d:  Testing . . .\n",mynode(),mypid());

hwclock(&start);
rc=itoz(0x1234, array, 4);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  itoz           delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0x1234, 4)              = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
rc=ztoi(array, &j, 4);
hwclock(&end);

if (j != 0x1234) printf("Node %d, Pid %d:  j=%x\n",mynode(),mypid(),j);
delta = esub(end,start);
printf("Node %d, Pid %d:  ztoi           delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0x1234, 4)              = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
rc=ztoa(array, zarray, 4);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  ztoa           delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0x1234, 4)              = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*------*/

hwclock(&start);
rc=itoz(0x12345678, array, 9);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  itoz           delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0x12345678, 9)          = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
rc=ztoi(array, &j, 9);
hwclock(&end);

if (j != 0x12345678) printf("Node %d, Pid %d:  j=%x\n",mynode(),mypid(),j);
delta = esub(end,start);
printf("Node %d, Pid %d:  ztoi           delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0x12345678, 9)          = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
rc=ztoa(array, zarray, 9);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  ztoa           delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0x12345678, 9)          = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*------*/

hwclock(&start);
rc=itoz(0xf2345678, array, 9);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  itoz           delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0xf2345678, 9)          = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
rc=ztoi(array, &j, 9);
hwclock(&end);

if (j != 0xf2345678) printf("Node %d, Pid %d:  j=%x\n",mynode(),mypid(),j);
delta = esub(end,start);
printf("Node %d, Pid %d:  ztoi           delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0xf2345678, 9)          = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
rc=ztoa(array, zarray, 9);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  ztoa           delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0xf2345678, 9)          = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*------*/

hwclock(&start);
rc=itoz(0x2, array, 10);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  itoz           delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0x2, 10)                = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
rc=ztoi(array, &j, 10);
hwclock(&end);

if (j != 0x2) printf("Node %d, Pid %d:  j=%x\n",mynode(),mypid(),j);
delta = esub(end,start);
printf("Node %d, Pid %d:  ztoi           delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0x2, 10)                = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
rc=ztoa(array, zarray, 10);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  ztoa           delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (0x2, 10)                = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*---------------------------------------------------------------*/

printf("Node %d, Pid %d:                         complete\n",mynode(),mypid());
printf("Node %d, Pid %d:  Test PASSED\n",mynode(),mypid());

}
