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
 *	12/14/92 fc	modified
 *	The constant 8589934592 overflows in compilation.  It is changed to
 *	23147483648*4.
 *	
 */

main() {
	double 			dbli,dblj;
	unsigned int	ibm,err;
	float			i,j;
	int				rc;
	double			ibmd;

	err=0;
/*----------------------------------------------------------------------*/
	printf("Node %d, Pid %d:  Testing istois/istoid: -to+8000 by .03125. . .\n",mynode(),mypid());
	for (i=-8000,dbli=-8000; i<8000; i+=.125/4, dbli+=.125/4)	{
		rc=0;
		rc=istoIs(&i, &ibm, 1);
		Istois(&ibm, &j, 1);
		if ((rc != 0)||(i!=j)) {
			printf("Node %d, Pid %d:  Istois error 1:\n",mynode(),mypid());
			printf("Node %d, Pid %d:     i=%f = 0x%8.8X\n",mynode(),mypid(), i,*((int *)&i));
			printf("Node %d, Pid %d:     ibm=0x%8.8X rc=%d\n",mynode(),mypid(), ibm, rc);
			printf("Node %d, Pid %d:     j=%f = 0x%8.8X\n\n",mynode(),mypid(), j,*((int *)&j));
			err=1;
		}

		rc=0;
		rc=idtoIs(&dbli, &ibm, 1);
		if (rc != 0) {
			printf("Node %d, Pid %d:  idtoIs error 10 rc=%d (expecting %d)\n",mynode(),mypid(),rc,0);
			err=10;
		}
		Istoid(&ibm, &dblj, 1);
		if (dbli != dblj) {
			printf("Node %d, Pid %d:  Istoid error 2:\n",mynode(),mypid());
			printf("Node %d, Pid %d:     dbli=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dbli, ((int *)&dbli)[1], ((int *)&dbli)[0]);
			printf("Node %d, Pid %d:     ibm=0x%8.8X, rc=%d\n",mynode(),mypid(), ibm, rc);
			printf("Node %d, Pid %d:     dblj=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dblj, ((int *)&dblj)[1], ((int *)&dblj)[0]);
			err=2;
		}

		rc=0;
		rc=idtoId(&dbli, &ibmd, 1);
		if (rc != 0) {
			printf("Node %d, Pid %d:  idtoId error 11 rc=%d (expecting %d)\n",mynode(),mypid(),rc,0);
			err=11;
		}
		Idtoid(&ibmd, &dblj, 1);
		if (dbli != dblj) {
			printf("Idtoid error 3:\n");
			printf("   dbli=%e = 0x%8.8X-%8.8X\n", dbli, ((int *)&dbli)[1], ((int *)&dbli)[0]);
			printf("   ibm=0x%8.8X-%8.8X, rc=%d\n", ((int *)&ibmd)[0], ((int *)&ibmd)[1], rc);
			printf("   dblj=%e = 0x%8.8X-%8.8X\n", dblj, ((int *)&dblj)[1], ((int *)&dblj)[0]);
			err=3;
		}
	}
/*----------------------------------------------------------------------*/
	printf("Node %d, Pid %d:  Testing istois/istoid: -to+1 by .001953125. . .\n",mynode(),mypid());
	for (i=-1,dbli=-1; i<1; i+=1.0/512, dbli+=1.0/512)	{
		rc=0;
		rc=istoIs(&i, &ibm, 1);
		Istois(&ibm, &j, 1);
		if ((rc != 0)||(i!=j)) {
			printf("Node %d, Pid %d:  Istois error 4:\n",mynode(),mypid());
			printf("Node %d, Pid %d:     i=%f = 0x%8.8X\n",mynode(),mypid(), i,*((int *)&i));
			printf("Node %d, Pid %d:     ibm=0x%8.8X rc=%d\n",mynode(),mypid(), ibm, rc);
			printf("Node %d, Pid %d:     j=%f = 0x%8.8X\n\n",mynode(),mypid(), j,*((int *)&j));
			err=4;
		}

		rc=0;
		rc=idtoIs(&dbli, &ibm, 1);
		if (rc != 0) printf("Node %d, Pid %d:  rc=%d\n",mynode(),mypid(),rc);
		Istoid(&ibm, &dblj, 1);
		if (dbli != dblj) {
			printf("Node %d, Pid %d:  Istoid error 5:\n",mynode(),mypid());
			printf("Node %d, Pid %d:     dbli=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dbli, ((int *)&dbli)[1], ((int *)&dbli)[0]);
			printf("Node %d, Pid %d:     ibm=0x%8.8X, rc=%d\n",mynode(),mypid(), ibm, rc);
			printf("Node %d, Pid %d:     dblj=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dblj, ((int *)&dblj)[1], ((int *)&dblj)[0]);
			err=5;

		rc=0;
		rc=idtoId(&dbli, &ibmd, 1);
		if (rc != 0) {
			printf("Node %d, Pid %d:  idtoId error 12 rc=%d (expecting %d)\n",mynode(),mypid(),rc,0);
			err=12;
		}
		Idtoid(&ibmd, &dblj, 1);
		if (dbli != dblj) {
			printf("Node %d, Pid %d:  Idtoid error 6:\n",mynode(),mypid());
			printf("Node %d, Pid %d:     dbli=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dbli, ((int *)&dbli)[1], ((int *)&dbli)[0]);
			printf("Node %d, Pid %d:     ibm=0x%8.8X-%8.8X, rc=%d\n",mynode(),mypid(), ((int *)&ibmd)[0], ((int *)&ibmd)[1], rc);
			printf("Node %d, Pid %d:     dblj=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dblj, ((int *)&dblj)[1], ((int *)&dblj)[0]);
			err=6;
		}
		}
	}
/*----------------------------------------------------------------------*/
	printf("Testing istois/istoid: -to+8589934592 by 8. . .\n");
	for (i=-2147483648*4,dbli=-2147483648*4; i<2147483648*4; i+=8, dbli+=8)	{
		rc=0;
		rc=istoIs(&i, &ibm, 1);
		Istois(&ibm, &j, 1);
		if ((rc != 0)||(i!=j)) {
			printf("Node %d, Pid %d:  Istois error 7:\n",mynode(),mypid());
			printf("Node %d, Pid %d:     i=%f = 0x%8.8X\n",mynode(),mypid(), i,*((int *)&i));
			printf("Node %d, Pid %d:     ibm=0x%8.8X rc=%d\n",mynode(),mypid(), ibm, rc);
			printf("Node %d, Pid %d:     j=%f = 0x%8.8X\n\n",mynode(),mypid(), j,*((int *)&j));
			err=7;
		}

		rc=0;
		rc=idtoIs(&dbli, &ibm, 1);
		if (rc != 0) {
			printf("Node %d, Pid %d:  idtoId error 13 rc=%d (expecting %d)\n",mynode(),mypid(),rc,0);
			err=13;
		}
		Istoid(&ibm, &dblj, 1);
		if (dbli != dblj) {
			printf("Node %d, Pid %d:  Istoid error 8:\n",mynode(),mypid());
			printf("Node %d, Pid %d:     dbli=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dbli, ((int *)&dbli)[1], ((int *)&dbli)[0]);
			printf("Node %d, Pid %d:     ibm=0x%8.8X, rc=%d\n",mynode(),mypid(), ibm, rc);
			printf("Node %d, Pid %d:     dblj=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dblj, ((int *)&dblj)[1], ((int *)&dblj)[0]);
			err=8;
		}

		rc=0;
		rc=idtoId(&dbli, &ibmd, 1);
		if (rc != 0) {
			printf("Node %d, Pid %d:  idtoId error 14 rc=%d (expecting %d)\n",mynode(),mypid(),rc,0);
			err=14;
		}
		Idtoid(&ibmd, &dblj, 1);
		if (dbli != dblj) {
			printf("Node %d, Pid %d:  Idtoid error 9:\n",mynode(),mypid());
			printf("Node %d, Pid %d:     dbli=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dbli, ((int *)&dbli)[1], ((int *)&dbli)[0]);
			printf("Node %d, Pid %d:     ibm=0x%8.8X-%8.8X, rc=%d\n",mynode(),mypid(), ((int *)&ibmd)[0], ((int *)&ibmd)[1], rc);
			printf("Node %d, Pid %d:     dblj=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dblj, ((int *)&dblj)[1], ((int *)&dblj)[0]);
			err=9;
		}
	}
/*----------------------------------------------------------------------*/
	printf("Node %d, Pid %d:  Bounds testing: . . .\n",mynode(),mypid());
	printf("Node %d, Pid %d:  NOTE:  Requires manual checking!!\n",mynode(),mypid());
	i=1.1;
	rc=0;
	rc=istoIs(&i, &ibm, 1);
	Istois(&ibm, &j, 1);
	printf("Node %d, Pid %d:     i=%f = 0x%8.8X\n",mynode(),mypid(), i,*((int *)&i));
	printf("Node %d, Pid %d:     ibm=0x%8.8X rc=%d\n",mynode(),mypid(), ibm, rc);
	printf("Node %d, Pid %d:     j=%f = 0x%8.8X\n\n",mynode(),mypid(), j,*((int *)&j));

	dbli=1.1;
	rc=0;
	rc=idtoIs(&dbli, &ibm, 1);
	Istoid(&ibm, &dblj, 1);
	printf("Node %d, Pid %d:     dbli=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dbli, ((int *)&dbli)[1], ((int *)&dbli)[0]);
	printf("Node %d, Pid %d:     ibm=0x%8.8X, rc=%d\n",mynode(),mypid(), ibm, rc);
	printf("Node %d, Pid %d:     dblj=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dblj, ((int *)&dblj)[1], ((int *)&dblj)[0]);


	dbli=1.1;
	rc=0;
	rc=idtoId(&dbli, &ibmd, 1);
	Idtoid(&ibmd, &dblj, 1);
	printf("Node %d, Pid %d:     dbli=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dbli, ((int *)&dbli)[1], ((int *)&dbli)[0]);
	printf("Node %d, Pid %d:     ibm=0x%8.8X-%8.8X, rc=%d\n",mynode(),mypid(), ((int *)&ibmd)[0], ((int *)&ibmd)[1], rc);
	printf("Node %d, Pid %d:     dblj=%e = 0x%8.8X-%8.8X\n",mynode(),mypid(), dblj, ((int *)&dblj)[1], ((int *)&dblj)[0]);
/*----------------------------------------------------------------------*/
	if (err == 0)
		printf("Node %d, Pid %d:  Test PASSED\n\n",mynode(),mypid());
	else
		printf("Node %d, Pid %d:  Test FAILED, last error=%d\n\n",mynode(),mypid(),err);
}
