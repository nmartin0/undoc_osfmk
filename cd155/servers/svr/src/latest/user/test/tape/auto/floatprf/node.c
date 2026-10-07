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
#include <estat.h>
*/
#include <ctype.h>

esize_t esub(esize_t a, esize_t b);    /* a - b */
esize_t	start, end, delta;
double 			dbli,dblj,ibmd;
unsigned int	ibm,err;
float			i,j;
int				rc;
char			arri[5],arrj[5],arribm[5];
main() {

/*---------------------------------------------------------------*/

printf("Node %d, Pid %d:  Testing . . .\n",mynode(),mypid());

i=8000.0;
hwclock(&start);
rc=istoIs(&i, &ibm, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  istoIs         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (8000)                   = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
Istois(&ibm, &j, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  Istois         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (8000)                   = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*------*/
i=1.1;
hwclock(&start);
rc=istoIs(&i, &ibm, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  istoIs         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (1.1)                    = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
Istois(&ibm, &j, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  Istois         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (1.1)                    = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*------*/

i=6.5536e+23;
hwclock(&start);
rc=istoIs(&i, &ibm, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  istoIs         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (6.5536e+23)             = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
Istois(&ibm, &j, 1);
hwclock(&end);
printf("hello there 2\n");

delta = esub(end,start);
printf("Node %d, Pid %d:  Istois         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (6.5536e+23)             = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*------*/

*((float *)&arri[0])=6.5536e+23;
hwclock(&start);
rc=istoIs(((float *)&arri[0]), ((unsigned int *)&arribm[0]), 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  istoIs         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (6.5536e+23)odd          = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
Istois(((unsigned int *)&arribm[0]), ((float *)&arrj[0]), 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  Istois         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (6.5536e+23)odd          = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*------*/

dbli=8000.0;
hwclock(&start);
rc=idtoIs(&dbli, &ibm, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  idtoIs         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (8000)                   = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
Istoid(&ibm, &dblj, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  Istoid         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (8000)                   = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*------*/

dbli=1.1;
hwclock(&start);
rc=idtoIs(&dbli, &ibm, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  idtoIs         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (1.1)                    = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
Istoid(&ibm, &dblj, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  Istoid         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (1.1)                    = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*------*/

dbli=6.5536e+23;
hwclock(&start);
rc=idtoIs(&dbli, &ibm, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  idtoIs         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (6.5536e+23)             = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
Istoid(&ibm, &dblj, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  Istoid         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (6.5536e+23)             = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*------*/

dbli=8000.0;
hwclock(&start);
rc=idtoId(&dbli, &ibmd, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  idtoId         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (8000)                   = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
Idtoid(&ibmd, &dblj, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  Idtoid         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (8000)                   = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*------*/

dbli=1.1;
hwclock(&start);
rc=idtoId(&dbli, &ibmd, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  idtoId         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (1.1)                    = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
Idtoid(&ibmd, &dblj, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  Idtoid         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (1.1)                    = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*------*/

dbli=6.5536e+23;
hwclock(&start);
rc=idtoId(&dbli, &ibmd, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  idtoId         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (6.5536e+23)             = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

hwclock(&start);
Idtoid(&ibmd, &dblj, 1);
hwclock(&end);

delta = esub(end,start);
printf("Node %d, Pid %d:  Idtoid         delta time = %08u %08u00 ns\n",mynode(),mypid(), delta.shigh, delta.slow);
printf("Node %d, Pid %d:   (6.5536e+23)             = %08u usec\n\n",mynode(),mypid(), delta.slow / 10);

/*---------------------------------------------------------------*/

printf("Node %d, Pid %d:                         complete\n",mynode(),mypid());
printf("Node %d, Pid %d:  Test PASSED\n",mynode(),mypid());

}
