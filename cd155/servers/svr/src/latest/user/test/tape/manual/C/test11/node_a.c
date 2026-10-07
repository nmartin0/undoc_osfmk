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
 *  test11a.c
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
 *	The LENGTH constant must be changed from 33*1024 to 31*1024 because
 *	the LENGTH is passed into "tread" as the "length" parameter which is
 * 	of type "int" with limit 32*1024-1 which is smaller than 33*1024
 *
 *----------------------------------------------------------------------------*/
#define DEBUG	1
#define DEBUG_CODE	0

#define TAPEDEV "/dev/io1/rmt6"
#define OWNER "Greg Tensa"
#define	TEST01 "TEST01"
#define	MAXTAPES	2
#define LENGTH (31*1024)
#define IMAX 100
#define JMAX 2
#define ZEROREAD 1
#define VERIFY 1

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include "t3480.h"

VOLSER          volser[MAXTAPES];
DCB             dcb;
VCB             vcb;
char            tapedev[80];
int             fd, rc, i, j;
char            buf[LENGTH];
char            verifybuf[LENGTH];
int             len;
int             node, pid;

void
print_buf()
{
        int     indx;

	printf("buf = ");
        for ( indx=0; indx < IMAX; indx++ )
                printf( "%d ", buf[indx]);
	printf("\n");
}

dsopen ()
{
    printf ("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", 
		node, pid, dcb.dsname, dcb.dsseqno);
    rc = tdsopen (fd, &dcb);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsopen rc=%d\n", node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tdsopen error=%d - aborting\n", 
		node, pid, rc);
	errexit ();
    }
    rc = tget_blk_id (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from open tget_blk_id rc=%8x\n", 
		node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tget_blk_id error=%d - aborting\n", 
		node, pid, rc);
	errexit ();
    }
}


#if VERIFY
verify ()
{
    int	k;

    if (i < 2)
	return;
    for (k = 1; k < len -1; k++)
    {
	if (buf[k] != verifybuf[k])
	{
	    printf ("Node %d, Pid %d:  ", node, pid);
	    printf (" *** data corruption error - i=%d, k=%d, expect=%d, actual=%d - aborting\n", 
		i, k, verifybuf[k], buf[k]);
	    errexit ();
	}
	buf[k] = 0;
    }
}

#endif


dsclose ()
{
    rc = tget_blk_id (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from close tget_blk_id rc=%8x\n", 
		node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tget_blk_id error=%d - aborting\n", 
		node, pid, rc);
	errexit ();
    }
#if ZEROREAD
    len = tread (fd, buf, LENGTH);
    if (len != 0)
    {
	printf ("Node %d, Pid %d:  ", node, pid);
	printf ("*** non-zero tread len at EOF = %d - aborting\n", len);
	errexit ();
    }
#endif
    rc = tdsclose (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsclose rc=%d\n\n", node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tdsclose error=%d - aborting\n", 
		node, pid, rc);
	errexit ();
    }
}


int 
uexit (code, exitparm)
    int             code;
    EXITPARM       *exitparm;
{
    printf ("Node %d, Pid %d:  \n*** uexit called with value %d - aborting\n", 
		node, pid, code);
    errexit ();
}


main (ac, av)
    int             ac;
    char          **av;

{
    node = mynode ();
    pid = mypid ();
#if VERIFY

    /*
     *	Build verify buffer. 
     */
    for (i = 1, j = 0; j < LENGTH; i++, j++)
    {
	if (i > 255)
	    i = 0;
	verifybuf[j] = i;
    }
#endif

    /*
     *	Tape device. 
     */
    if (ac < 2)
    {
	strcpy (tapedev, TAPEDEV);
    } else
    {
	strcpy (tapedev, av[1]);
    }

    /*
     *	Vol/Ser. 
     */
    if (ac < 3)
    {
	strcpy (volser[0].serialno, TEST01);
    } else
    {
	strcpy (volser[0].serialno, av[2]);
	volser[0].serialno[6] = NULL;
    }
    volser[1].serialno[0] = NULL;

#if DEBUG
	set_spider ( DEBUG_CODE  );

    printf ("Node %d, Pid %d:  tapedev=%s\n", node, pid, tapedev);
    for (i = 0; i < MAXTAPES; i++)
    {
	printf ("Node %d, Pid %d:  ",node,pid);
	printf ("volser %2d = %s\n", i, volser[i].serialno);
	if (volser[i].serialno[0] == NULL)
	    break;
    }
    printf ("Node %d, Pid %d:  \n", node, pid);
#endif

    /*
     *  Allocate tape drive. 
     */
    vcb.vollist = volser;
    vcb.exlst = &uexit;

    printf ("Node %d, Pid %d:  calling tapealloc\n", node, pid);
    fd = tapealloc (&vcb, tapedev);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tapealloc fd=%d\n\n", node, pid, fd);
#endif
    if (fd < 0)
    {
	printf ("Node %d, Pid %d:  *** tapealloc error=%d - aborting\n", 
		node, pid, fd);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
    set_tapedev (fd, 0, 1);	/* 4980, no unload ### */

    dcb.volseqno = 1;
    dcb.dsseqno = 0;
    dcb.read = 1;


#if 0
    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VB011.X104.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, i);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 0
    /*
     *	Open data set 2. 
     */
    strcpy (dcb.dsname, "VB014.X104.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, i);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *	Open data set 3. 
     */
    strcpy (dcb.dsname, "VB111.X104.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, i);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif

#if 1
    /*
     *	Open data set 4. 
     */
    strcpy (dcb.dsname, "VB114.X104.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif

#if 1
    /*
     *	Open data set 5. 
     */
    strcpy (dcb.dsname, "VB100.X104.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    printf("buf[0]=%d, i=%d, j=%d\n", buf[0], i, j);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  ",node, pid);
		printf("*** tread error: len=%d i=%d j=%d ***aborting***\n", 
			len, i, j);
		print_buf();
		errexit (); 
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  ", node, pid);
		printf ("*** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", 
		    buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  ", node, pid);
		printf ("*** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", 
		    buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif

#if 0
    /*
     *	Open data set 6. 
     */
    strcpy (dcb.dsname, "VS014.X104.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif

#if 0
    /*
     *	Open data set 7. 
     */
    strcpy (dcb.dsname, "Vb011.X104.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 0
    /*
     *  Open data set 8. 
     */
    strcpy (dcb.dsname, "Vb014.X104.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 9. 
     */
    strcpy (dcb.dsname, "Vb111.X104.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 10. 
     */
    strcpy (dcb.dsname, "Vb114.X104.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 11. 
     */
    strcpy (dcb.dsname, "Vb100.X104.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 12. 
     */
    strcpy (dcb.dsname, "Fb011.X100.X100  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 100;
    dcb.blocklen = 100;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 0
    /*
     *  Open data set 13. 
     */
    strcpy (dcb.dsname, "Fb014.X100.X100  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 100;
    dcb.blocklen = 100;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 14. 
     */
    strcpy (dcb.dsname, "Fb000.X100.X100  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 100;
    dcb.blocklen = 100;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 15. 
     */
    strcpy (dcb.dsname, "Fb111.X100.X100  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 100;
    dcb.blocklen = 100;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 0
    /*
     *  Open data set 16. 
     */
    strcpy (dcb.dsname, "Fb114.X100.X100  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 100;
    dcb.blocklen = 100;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 17. 
     */
    strcpy (dcb.dsname, "Fb100.X100.X100  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 100;
    dcb.blocklen = 100;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 18. 
     */
    strcpy (dcb.dsname, "FB011.X100.X100  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 100;
    dcb.blocklen = 100;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 0
    /*
     *  Open data set 19. 
     */
    strcpy (dcb.dsname, "FB014.X100.X100  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 100;
    dcb.blocklen = 100;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 20. 
     */
    strcpy (dcb.dsname, "FB000.X100.X100  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 100;
    dcb.blocklen = 100;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 21. 
     */
    strcpy (dcb.dsname, "FB011.X100.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 100;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 0
    /*
     *  Open data set 22. 
     */
    strcpy (dcb.dsname, "FB014.X100.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 100;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 23. 
     */
    strcpy (dcb.dsname, "FB111.X100.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 100;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 0
    /*
     *  Open data set 24. 
     */
    strcpy (dcb.dsname, "FB114.X100.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 100;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 25. 
     */
    strcpy (dcb.dsname, "FB100.X100.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 100;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, 100);
	    if (len != 100)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 26. 
     */
    strcpy (dcb.dsname, "Ub011.X000.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, i);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 0
    /*
     *  Open data set 27. 
     */
    strcpy (dcb.dsname, "Ub014.X000.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 0;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 28. 
     */
    strcpy (dcb.dsname, "Ub111.X000.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 0
    /*
     *  Open data set 29. 
     */
    strcpy (dcb.dsname, "Ub114.X000.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 0;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 30. 
     */
    strcpy (dcb.dsname, "Ub000.X000.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 0;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 1
    /*
     *  Open data set 31. 
     */
    strcpy (dcb.dsname, "Ub100.X000.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 0;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Read some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting tread loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
	{
	    len = tread (fd, buf, LENGTH);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** tread error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		errexit ();
	    }
	    if (buf[0] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[0]=%d i=%d j=%d ***aborting", node, pid, buf[0], i, j);
		errexit ();
	    }
	    if (buf[i -1] != i)
	    {
		printf ("Node %d, Pid %d:  *** tread miscompare: buf[i-1]=%d i=%d j=%d ***aborting", node, pid, buf[i -1], i, j);
		errexit ();
	    }
#if VERIFY
	    verify ();
#endif
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif
    /*
     *	Deallocate tape. 
     */
    printf ("Node %d, Pid %d:  calling tapedealloc\n", node, pid);
    rc = tapedealloc (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tapedealloc rc=%d\n\n", 
		node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tapedealloc error=%d - aborting\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
    printf ("\nNode %d, Pid %d:  Test PASSED\n", node, pid);
    exit (0);
}
