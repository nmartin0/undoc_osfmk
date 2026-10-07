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
 * INTEL CORPORATION PROPRIETARY INFORMATION 
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with Intel Corporation and may not be copied nor
 * disclosed except in accordance with the terms of that agreement. 
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <convec.h>

void 
itoa (n, s)
    int             n;
    char            s[];
{
    int             i, j, c, sign;

    if ((sign = n) < 0)
	n = 0 -n;

    i = 0;
    do
    {
	s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0)
	s[i++] = '-';
    s[i] = '\0';

    for (i = 0, j = strlen (s) -1; i < j; i++, j--)
    {
	c = s[i];
	s[i] = s[j];
	s[j] = c;
    }
}

main ()
{
    int             a, b, c, d, e, f, g, h;
    int             i, j, k, err, rc, mynod, numnod;
    unsigned char   array[30];
    char            zarray[30];
    char            iarray[30];

    err = 0;
    mynod = mynode ();
    numnod = numnodes ();

    /*---------------------------------------------------------------*/
    if (mynod == 0)
    {
	printf ("Node %d, Pid %d:  node %d: limit testing\n",
		mynode (), mypid (), mynod);

	i = 2;
	rc = itop (i, array, 1);
	if (rc != 0)
	{
	    printf ("Node %d, Pid %d:  itop error 101, i=%d, rc=%d\n",
		    mynode (), mypid (), i, rc);
	    err = 101;
	} else
	{
	    if (array[0] != 0x2c)
	    {
		printf ("Node %d, Pid %d:  itop error 102 i=%d array[0]=%x, expect %x\n", 
			mynode (), mypid (), i, array[0], 0x2c);
		err = 102;
	    }
	}
	rc = ptoa (array, zarray, 1);
	if (rc != 0)
	{
	    printf ("Node %d, Pid %d:  ptoa error 201, i=%d, rc=%d\n",
		    mynode (), mypid (), i, rc);
	    err = 201;
	}
	itoa (i, iarray);
	if (rc = strcmp (zarray, iarray) != 0)
	{
	    printf ("Node %d, Pid %d:  ztoa error 202, i=%d, zarray=%s, iarray=%s\n", 
			mynode (), mypid (), i, zarray, iarray);
	    err = 202;
	}
	i = -2;
	rc = itop (i, array, 1);
	if (rc != 0)
	{
	    printf ("Node %d, Pid %d:  itop error 103, i=%d, rc=%d\n",
		    mynode (), mypid (), i, rc);
	    err = 103;
	} else
	{
	    if (array[0] != 0x2d)
	    {
		printf ("Node %d, Pid %d:  itop error 104 i=%d array[0]=%x, expect %x\n", 
			mynode (), mypid (), i, array[0], 0x2d);
		err = 104;
	    }
	}
	rc = ptoa (array, zarray, 1);
	if (rc != 0)
	{
	    printf ("Node %d, Pid %d:  ptoa error 203, i=%d, rc=%d\n",
		    mynode (), mypid (), i, rc);
	    err = 203;
	}
	itoa (i, iarray);
	if (rc = strcmp (zarray, iarray) != 0)
	{
	    printf ("Node %d, Pid %d:  ztoa error 204, i=%d, zarray=%s, iarray=%s\n", 
		mynode (), mypid (), i, zarray, iarray);
	    err = 204;
	}
	i = 2147483647;
	rc = itop (i, array, 5);
	if (rc != CERR_OVERFLOW_PACKED_LEN)
	{
	    printf ("Node %d, Pid %d:  itop error 105, i=%d, rc=%d (expecting %d)\n", 
		mynode (), mypid (), i, rc, CERR_OVERFLOW_PACKED_LEN);
	    err = 105;
	}
	rc = itop (i, array, 0);
	if (rc != CERR_INVALID_PACKED_LEN)
	{
	    printf ("Node %d, Pid %d:  itop error 106, i=%d, rc=%d (expecting %d)\n", 
		mynode (), mypid (), i, rc, CERR_INVALID_PACKED_LEN);
	    err = 106;
	}
	rc = itop (i, array, 6);
	if (rc != 0)
	{
	    printf ("Node %d, Pid %d:  itop error 107, i=%d, rc=%d\n", 
			mynode (), mypid (), i, rc);
	    err = 107;
	} else
	{
	    if (array[0] != 02)
	    {
		printf ("Node %d, Pid %d:  itop error 108 i=%d array[0]=%x, expect %x\n", 
			mynode (), mypid (), i, array[0], 02);
		err = 108;
	    }
	    if (array[1] != 0x14)
	    {
		printf ("Node %d, Pid %d:  itop error 109 i=%d array[1]=%x, expect %x\n", 
			mynode (), mypid (), i, array[1], 0x14);
		err = 109;
	    }
	    if (array[2] != 0x74)
	    {
		printf ("Node %d, Pid %d:  itop error 110 i=%d array[2]=%x, expect %x\n", 
			mynode (), mypid (), i, array[2], 0x74);
		err = 110;
	    }
	    if (array[3] != 0x83)
	    {
		printf ("Node %d, Pid %d:  itop error 111 i=%d array[3]=%x, expect %x\n", 
			mynode (), mypid (), i, array[3], 0x83);
		err = 111;
	    }
	    if (array[4] != 0x64)
	    {
		printf ("Node %d, Pid %d:  itop error 112 i=%d array[4]=%x, expect %x\n", 
			mynode (), mypid (), i, array[4], 0x64);
		err = 112;
	    }
	    if (array[5] != 0x7c)
	    {
		printf ("Node %d, Pid %d:  itop error 113 i=%d array[5]=%x, expect %x\n", 
			mynode (), mypid (), i, array[5], 0x7c);
		err = 113;
	    }
	    rc = ptoi (array, &j, 6);
	    if (rc != 0)
	    {
		printf ("Node %d, Pid %d:  ptoi error 114, i=%d, rc=%d\n", 
			mynode (), mypid (), i, rc);
		err = 114;
	    }
	    if (i != j)
	    {
		printf ("Node %d, Pid %d:  ptoi error 115, i=%d, j=%d\n", 
			mynode (), mypid (), i, j);
		err = 115;
	    }
	    rc = ptoa (array, zarray, 6);
	    if (rc != 0)
	    {
		printf ("Node %d, Pid %d:  ptoa error 205, i=%d, rc=%d\n", 
			mynode (), mypid (), i, rc);
		err = 203;
	    }
	    itoa (i, iarray);
	    if (rc = strcmp (zarray, iarray) != 0)
	    {
		printf ("Node %d, Pid %d:  ztoa error 206, i=%d, zarray=%s, iarray=%s\n", 
			mynode (), mypid (), i, zarray, iarray);
		err = 204;
	    }
	    rc = ptoi (array, &j, 0);
	    if (rc != CERR_INVALID_PACKED_LEN)
	    {
		printf ("Node %d, Pid %d:  ptoi error 116, i=%d, rc=%d (expecting %d)\n", 
			mynode (), mypid (), i, rc, CERR_INVALID_PACKED_LEN);
		err = 116;
	    }
	    rc = ptoa (array, zarray, 0);
	    if (rc != CERR_INVALID_PACKED_LEN)
	    {
		printf ("Node %d, Pid %d:  ptoa error 206, i=%d, rc=%d (expecting %d)\n", 
			mynode (), mypid (), i, rc, CERR_INVALID_PACKED_LEN);
		err = 206;
	    }
	    array[5] = 0x8c;
	    rc = ptoi (array, &j, 6);
	    if (rc != CERR_OVERFLOW)
	    {
		printf ("Node %d, Pid %d:  ptoi error 117, i=%d, rc=%d (expecting %d)\n", 
			mynode (), mypid (), i, rc, CERR_OVERFLOW);
		err = 117;
	    }
	    rc = ptoi (array, &j, 5);
	    if (rc != CERR_INVALID_SIGN)
	    {
		printf ("Node %d, Pid %d:  ptoi error 118, i=%d, rc=%d (expecting %d)\n", 
			mynode (), mypid (), i, rc, CERR_INVALID_SIGN);
		err = 118;
	    }
	    rc = ptoa (array, zarray, 5);
	    if (rc != CERR_INVALID_SIGN)
	    {
		printf ("Node %d, Pid %d:  ptoa error 208, i=%d, rc=%d\n", 
			mynode (), mypid (), i, rc, CERR_INVALID_SIGN);
		err = 208;
	    }
	    array[5] = 0x0c;
	    array[4] = 0xa0;
	    rc = ptoi (array, &j, 6);
	    if (rc != CERR_INVALID_PACKED_NUM)
	    {
		printf ("Node %d, Pid %d:  ptoi error 119, i=%d, rc=%d (expecting %d)\n", 
			mynode (), mypid (), i, rc, CERR_INVALID_PACKED_NUM);
		err = 119;
	    }
	    rc = ptoa (array, zarray, 6);
	    if (rc != CERR_INVALID_PACKED_NUM)
	    {
		printf ("Node %d, Pid %d:  ptoa error 209, i=%d, rc=%d (expecting %d)\n", 
			mynode (), mypid (), i, rc, CERR_INVALID_PACKED_NUM);
		err = 209;
	    }
	}

	i = -2147483648;
	rc = itop (i, array, 6);
	if (rc != 0)
	{
	    printf ("Node %d, Pid %d:  itop error 120, i=%d, rc=%d\n",
		    mynode (), mypid (), i, rc);
	    err = 120;
	} else
	{
	    if (array[0] != 02)
	    {
		printf ("Node %d, Pid %d:  itop error 121 i=%d array[0]=%x, expect %x\n", 
			mynode (), mypid (), i, array[0], 02);
		err = 121;
	    }
	    if (array[1] != 0x14)
	    {
		printf ("Node %d, Pid %d:  itop error 122 i=%d array[1]=%x, expect %x\n", 
			mynode (), mypid (), i, array[1], 0x14);
		err = 122;
	    }
	    if (array[2] != 0x74)
	    {
		printf ("Node %d, Pid %d:  itop error 123 i=%d array[2]=%x, expect %x\n", 
			mynode (), mypid (), i, array[2], 0x74);
		err = 123;
	    }
	    if (array[3] != 0x83)
	    {
		printf ("Node %d, Pid %d:  itop error 124 i=%d array[3]=%x, expect %x\n", 
			mynode (), mypid (), i, array[3], 0x83);
		err = 124;
	    }
	    if (array[4] != 0x64)
	    {
		printf ("Node %d, Pid %d:  itop error 125 i=%d array[4]=%x, expect %x\n", 
			mynode (), mypid (), i, array[4], 0x64);
		err = 125;
	    }
	    if (array[5] != 0x8d)
	    {
		printf ("Node %d, Pid %d:  itop error 126 i=%d array[5]=%x, expect %x\n", 		mynode (), mypid (), i, array[5], 0x8d);
		err = 126;
	    }
	    rc = ptoi (array, &j, 6);
	    if (rc != 0)
	    {
		printf ("Node %d, Pid %d:  ptoi error 127, i=%d, rc=%d\n", 
			mynode (), mypid (), i, rc);
		err = 127;
	    }
	    if (i != j)
	    {
		printf ("Node %d, Pid %d:  ptoi error 128, i=%d, j=%d\n", 
			mynode (), mypid (), i, j);
		err = 128;
	    }
	    rc = ptoa (array, zarray, 6);
	    if (rc != 0)
	    {
		printf ("Node %d, Pid %d:  ptoa error 210, i=%d, rc=%d\n", 
			mynode (), mypid (), i, rc);
		err = 210;
	    }
	    i = -2147483647;
	    itoa (i, iarray);
	    iarray[10]++;
	    if (rc = strcmp (zarray, iarray) != 0)
	    {
		printf ("Node %d, Pid %d:  ztoa error 211, i=%d, zarray=%s, iarray=%s\n", 
			mynode (), mypid (), i, zarray, iarray);
		err = 211;
	    }
	    array[5] = 0x9d;
	    rc = ptoi (array, &j, 6);
	    if (rc != CERR_OVERFLOW)
	    {
		printf ("Node %d, Pid %d:  ptoi error 129, i=%d, rc=%d (expecting %d)\n", 
			mynode (), mypid (), i, rc, CERR_OVERFLOW);
		err = 129;
	    }
	}
    }
    /*---------------------------------------------------------------*/
    if (mynod > 8)
    {
	printf ("Node %d, Pid %d:  node %d: exiting due to no use. PASSED\n",
		mynode (), mypid (), mynod);
	exit (0);
    }
    if (numnod > 8)
	numnod = 8;
    k = 8 / numnod;
    printf ("Node %d, Pid %d:  checking %d to %d\n",
	    mynode (), mypid (), mynod * 10000000 * k, (mynod + 1) * 10000000 * k -1);

    for (a = mynod * k; a < mynod * k + k; a += 1)
    {
	i = a * 10000000;
	for (b = 0; b < 10; b += 1)
	{
	    for (c = 0; c < 10; c += 1)
	    {
		for (d = 0; d < 10; d += 1)
		{
		    for (e = 0; e < 10; e += 1)
		    {
			for (f = 0; f < 10; f += 1)
			{
			    for (g = 0; g < 10; g += 1)
			    {
				for (h = 0; h < 10; h += 1)
				{
				    rc = itop (i, array, 6);
				    if (rc != 0)
				    {
					printf ("Node %d, Pid %d:  itop error 1, i=%d, rc=%d\n",
						mynode (), mypid (), i, rc);
					err = 1;
				    } else
				    {
					if (array[0] != 0)
					{
					    printf ("Node %d, Pid %d:  itop error 2 i=%d array[0]=%x, expect %x\n", mynode (), mypid (), i, array[0], 0);
					    err = 2;
					}
					if (array[1] != a)
					{
					    printf ("Node %d, Pid %d:  itop error 3 i=%d array[1]=%x, expect %x\n", mynode (), mypid (), i, array[1], a);
					    err = 3;
					}
					if (array[2] != 16 * b + c)
					{
					    printf ("Node %d, Pid %d:  itop error 4 i=%d array[2]=%x, expect %x\n", mynode (), mypid (), i, array[2], 16 * b + c);
					    err = 4;
					}
					if (array[3] != 16 * d + e)
					{
					    printf ("Node %d, Pid %d:  itop error 5 i=%d array[3]=%x, expect %x\n", mynode (), mypid (), i, array[3], 16 * d + e);
					    err = 5;
					}
					if (array[4] != 16 * f + g)
					{
					    printf ("Node %d, Pid %d:  itop error 6 i=%d array[4]=%x, expect %x\n", mynode (), mypid (), i, array[4], 16 * f + g);
					    err = 6;
					}
					if (array[5] != 16 * h + 0x0c)
					{
					    printf ("Node %d, Pid %d:  itop error 7 i=%d array[5]=%x, expect %x\n", mynode (), mypid (), i, array[5], 16 * h + 0x0c);
					    err = 7;
					}
					rc = ptoi (array, &j, 6);
					if (rc != 0)
					{
					    printf ("Node %d, Pid %d:  ptoi error 8, i=%d, rc=%d\n", mynode (), mypid (), i, rc);
					    err = 8;
					}
					if (i != j)
					{
					    printf ("Node %d, Pid %d:  ptoi error 9, i=%d, j=%d\n", mynode (), mypid (), i, j);
					    err = 9;
					}
				    }
				    rc = itop (0 -i, array, 6);
				    if (rc != 0)
				    {
					printf ("Node %d, Pid %d:  itop error 11, i=%d, rc=%d\n", mynode (), mypid (), 0 -i, rc);
					err = 11;
				    } else
				    {
					if (array[0] != 0)
					{
					    printf ("Node %d, Pid %d:  itop error 12 i=%d array[0]=%x, expect %x\n", mynode (), mypid (), 0 -i, array[0], 0);
					    err = 12;
					}
					if (array[1] != a)
					{
					    printf ("Node %d, Pid %d:  itop error 13 i=%d array[1]=%x, expect %x\n", mynode (), mypid (), 0 -i, array[1], a);
					    err = 13;
					}
					if (array[2] != 16 * b + c)
					{
					    printf ("Node %d, Pid %d:  itop error 14 i=%d array[2]=%x, expect %x\n", mynode (), mypid (), 0 -i, array[2], 16 * b + c);
					    err = 14;
					}
					if (array[3] != 16 * d + e)
					{
					    printf ("Node %d, Pid %d:  itop error 15 i=%d array[3]=%x, expect %x\n", mynode (), mypid (), 0 -i, array[3], 16 * d + e);
					    err = 15;
					}
					if (array[4] != 16 * f + g)
					{
					    printf ("Node %d, Pid %d:  itop error 16 i=%d array[4]=%x, expect %x\n", mynode (), mypid (), 0 -i, array[4], 16 * f + g);
					    err = 16;
					}
					if (array[5] != 16 * h + 0x0d)
					{
					    if (i != 0)
					    {
						printf ("Node %d, Pid %d:  itop error 17 i=%d array[5]=%x, expect %x\n", mynode (), mypid (), 0 -i, array[5], 16 * h + 0x0c);
						err = 17;
					    }
					}
					rc = ptoi (array, &j, 6);
					if (rc != 0)
					{
					    printf ("Node %d, Pid %d:  ptoi error 18, i=%d, rc=%d\n", mynode (), mypid (), 0 -i, rc);
					    err = 18;
					}
					if (0 -i != j)
					{
					    printf ("Node %d, Pid %d:  ptoi error 19, i=%d, j=%d\n", mynode (), mypid (), 0 -i, j);
					    err = 19;
					}
				    }
				    i++;
				}
			    }
			}
		    }
		}
	    }
	}
    }

    /*---------------------------------------------------------------*/

    if (err)
    {
	printf ("Node %d, Pid %d:  node %d: Test FAILED, last error=%d\n",
		mynode (), mypid (), mynod, err);
    } else
    {
	printf ("Node %d, Pid %d:  node %d: Test PASSED\n", mynode (), mypid (), mynod);
    }
    exit (err);
}
