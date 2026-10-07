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
 *  testvsw.c
 *
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license agreement
 *  or nondisclosure agreement with Intel Corporation and may not be
 *  copied nor disclosed except in accordance with the terms of that
 *  agreement.
 *
 *  History:
 *   2-20-92   g t  Created
 *  10-06-92   cheng 	modified
 *	The LENGTH constant is changed from 33*1024 to 31*1024 because
 *	to make it consistent with the LENGTH value in "testvsr" althought it
 * 	is not necessary.  (see "testvsr/node.c" comment)              
 *
 *----------------------------------------------------------------------------*/
#define DEBUG 1
#define TAPEDEV "/dev/io1/rmt6"
#define OWNER "Greg Tensa"
#define	TEST0 "TEST0 "
#define	MAXTAPES	4
#define LENGTH (31*1024)
#define IMAX 2001
#define JMAX 3

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
int             len;
int             node, pid;
int             code4 = 0, code5 = 0;

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
}


dsclose ()
{
    printf ("Node %d, Pid %d:  calling tdsclose\n", node, pid);
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


main (ac, av)
    int             ac;
    char          **av;
{
    node = mynode ();
    pid = mypid ();

    /*
     *	Build buffer for verify. 
     */
    for (i = 1, j = 0; j < LENGTH; i++, j++)
    {
	if (i > 255)
	    i = 0;
	buf[j] = i;
    }

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
    for (i = 0; i < MAXTAPES; i++)
    {
	if (ac < i + 3)
	{
	    strcpy (volser[i].serialno, TEST0);
	    volser[i].serialno[5] = i + 0x31;
	    volser[i].serialno[6] = NULL;
	} else
	{
	    strcpy (volser[i].serialno, av[i + 2]);
	    volser[i].serialno[6] = NULL;
	}
    }
    volser[MAXTAPES -1].serialno[0] = NULL;


#if DEBUG
    printf ("Node %d, Pid %d:  tapedev=%s\n", node, pid, tapedev);
    for (i = 0; i < MAXTAPES; i++)
    {
	printf ("Node %d, Pid %d:  volser %2d = %s\n", node, pid, i, volser[i].serialno);
	if (volser[i].serialno[0] == 0)
	    break;
    }
    printf ("Node %d, Pid %d:  \n", node, pid);
#endif

    /*
     *  Allocate tape drive. 
     */
    vcb.vollist = volser;
    vcb.exlst = (int *) NULL;

    printf ("Node %d, Pid %d:  calling tapealloc\n", node, pid);
    fd = tapealloc (&vcb, tapedev);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tapealloc fd=%d\n\n", node, pid, fd);
#endif
    if (fd < 0)
    {
	printf ("Node %d, Pid %d:  *** tapealloc error=%d - aborting\n", node, pid, fd);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
    set_tapedev (fd, 0, 1);	/* 4980, no unload ### */


    dcb.dsseqno = 0;
    dcb.volseqno = 1;
    dcb.read = 0;
    dcb.expirec = ' ';
    dcb.expireyy = 92;
    dcb.expiredd = 10;


#if 0
    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VS011.X496.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 496;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Write some data. 
     */
    printf ("Node %d, Pid %d:  starting twrite loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i += 73)
	{
	    buf[0] = i;
	    buf[i -1] = i;
	    len = twrite (fd, buf, i);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** twrite error: len=%d i=%d j=%d - aborting\n", node, pid, len, i, j);
		errexit ();
	    }
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
    strcpy (dcb.dsname, "VS014.X496.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 496;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Write some data. 
     */
    printf ("Node %d, Pid %d:  starting twrite loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i += 79)
	{
	    buf[0] = i;
	    buf[i -1] = i;
	    len = twrite (fd, buf, i);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** twrite error: len=%d i=%d j=%d - aborting\n", node, pid, len, i, j);
		errexit ();
	    }
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 0
    /*
     *	Open data set 3. 
     */
    strcpy (dcb.dsname, "VR011.X496.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 496;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Write some data. 
     */
    printf ("Node %d, Pid %d:  starting twrite loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i += 87)
	{
	    buf[0] = i;
	    buf[i -1] = i;
	    len = twrite (fd, buf, i);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** twrite error: len=%d i=%d j=%d - aborting\n", node, pid, len, i, j);
		errexit ();
	    }
	}
	len = feov (fd);
	dcb.volseqno += 1;
	if (len != 0)
	{
	    printf ("Node %d, Pid %d:  *** feov error=%d\n", node, pid, len);
	    errexit ();
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 0
    /*
     *	Open data set 4. 
     */
    strcpy (dcb.dsname, "VR014.X496.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 496;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Write some data. 
     */
    printf ("Node %d, Pid %d:  starting twrite loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i += 93)
	{
	    buf[0] = i;
	    buf[i -1] = i;
	    len = twrite (fd, buf, i);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** twrite error: len=%d i=%d j=%d - aborting\n", node, pid, len, i, j);
		errexit ();
	    }
	}
    }

    /*
     *	Close Data Set. 
     */
    dsclose ();
#endif


#if 0
    /*
     *	Open data set 5. 
     */
    strcpy (dcb.dsname, "VR013.X496.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 3;
    dcb.reclen = 496;
    dcb.blocklen = 500;

    dsopen ();

    /*
     *	Write some data. 
     */
    printf ("Node %d, Pid %d:  starting twrite loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i += 29)
	{
	    buf[0] = i;
	    buf[i -1] = i;
	    len = twrite (fd, buf, i);
	    if (len != i)
	    {
		printf ("Node %d, Pid %d:  *** twrite error: len=%d i=%d j=%d - aborting\n", node, pid, len, i, j);
		errexit ();
	    }
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
    strcpy (dcb.dsname, "VR016.X4K.X4K    ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 6;
    dcb.reclen = 4092;
    dcb.blocklen = 4096;

    dsopen ();

    /*
     *	Write some data. 
     */
    printf ("Node %d, Pid %d:  starting twrite loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < 3; i++)
	{
	    buf[0] = i;
	    buf[i -1] = i;
	    len = twrite (fd, buf, LENGTH);
	    if (len != LENGTH)
	    {
		printf ("Node %d, Pid %d:  *** twrite error: len=%d i=%d j=%d - aborting\n", node, pid, len, i, j);
		errexit ();
	    }
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
