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
 *  test9.c
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
 *	The LENGTH constant is changed from 33*1024 to 31*1024 because
 *	to make it consistent with the LENGTH value in "test0" althought it
 * 	is not necessary.  (see "test0/node.c" comment)              
 *
 *----------------------------------------------------------------------------*/
#define DEBUG 1
#define TAPEDEV "/dev/io1/rmt6"
#define OWNER "Greg Tensa"
#define	TEST01 "TEST01"
#define	MAXTAPES	2
#define LENGTH (31*1024)
#define IMAX 101
#define JMAX 2

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <t3480.h>

VOLSER          volser[MAXTAPES];
DCB             dcb;
VCB             vcb;
char            tapedev[80];
int             fd, rc, i, j;
char            buf[LENGTH];
int             len;
int             node, pid;
char            lnum = '1';
int             code4 = 0, code5 = 0, code9 = 0;
int             retval = 0;


dsopen ()
{
    printf ("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", 
		node, pid, dcb.dsname, dcb.dsseqno);
    rc = tdsopen (fd, &dcb);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsopen rc=%d\n", node, pid, rc);
#endif /* DEBUG */
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
#endif /* DEBUG */
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
    printf ("Node %d, Pid %d:    UEXIT value %d\n", node, pid, code);
    if (code == 4)
	code4++;
    if (code == 5)
	code5++;
    if (code == 9)
	code9++;
    return (retval);

}

main (ac, av)
    int             ac;
    char          **av;

{
    node = mynode ();
    pid = mypid ();

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
	volser[i].serialno[6] = NULL;
    }
    volser[1].serialno[0] = NULL;


#if DEBUG
    printf ("Node %d, Pid %d:  tapedev=%s\n", node, pid, tapedev);
    for (i = 0; i < MAXTAPES; i++)
    {
	printf ("Node %d, Pid %d:  volser %2d = %s\n", 
		node, pid, i, volser[i].serialno);
	if (volser[i].serialno[0] == 0)
	    break;
    }
    printf ("Node %d, Pid %d:  \n", node, pid);
#endif /* DEBUG */

    /*
     *  Allocate tape drive. 
     */
    vcb.vollist = volser;
    vcb.exlst = &uexit;

    printf ("Node %d, Pid %d:  calling tapealloc\n", node, pid);
    fd = tapealloc (&vcb, tapedev);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tapealloc fd=%d\n\n", 
		node, pid, fd);
#endif /* DEBUG */
    if (fd < 0)
    {
	printf ("Node %d, Pid %d:  *** tapealloc error=%d - aborting\n", 
		node, pid, fd);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
    set_tapedev (fd, 0, 1);	/* 4980, no unload ### */


    dcb.dsseqno = 0;
    dcb.volseqno = 1;
    dcb.read = 0;

    /*
     * expiration date is year 2092, day 100.  This will make sure the
     * datesets will not expire in the near future (unless the program is
     * used 100 year later - may be it will ) 
     */
    dcb.expirec = '0';
    dcb.expireyy = 92;
    dcb.expiredd = 100;

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
     *	Write some data. 
     */
    printf ("Node %d, Pid %d:  starting twrite loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
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

    /*
     *	Deallocate tape. 
     */
    printf ("Node %d, Pid %d:  calling tapedealloc\n", node, pid);
    rc = tapedealloc (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tapedealloc rc=%d\n\n", 
		node, pid, rc);
#endif /* DEBUG */
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tapedealloc error=%d - aborting\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
    /*
     *  Allocate tape drive. 
     */
    vcb.vollist = volser;
    vcb.exlst = &uexit;

#if DEBUG
    printf ("Node %d, Pid %d:  tapedev=%s\n", node, pid, tapedev);
    for (i = 0; i < MAXTAPES; i++)
    {
	printf ("Node %d, Pid %d:  volser %2d = %s\n", node, pid, i, volser[i].serialno);
	if (volser[i].serialno[0] == 0)
	    break;
    }
    printf ("Node %d, Pid %d:  \n", node, pid);
#endif /* DEBUG */

    printf ("Node %d, Pid %d:  calling tapealloc for %s\n", 
		node, pid, tapedev);
    fd = tapealloc (&vcb, tapedev);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tapealloc fd=%d\n\n", 
		node, pid, fd);
#endif /* DEBUG */
    if (fd < 0)
    {
	printf ("Node %d, Pid %d:  *** tapealloc error=%d - aborting\n", 
		node, pid, fd);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
    set_tapedev (fd, 0, 1);	/* 4980, no unload ### */

    /*
     *	Open data set 2 (should fail because the dateset not yet
     * expire). 
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

    printf ("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", 
		node, pid, dcb.dsname, dcb.dsseqno);
    rc = tdsopen (fd, &dcb);
#if DEBUG
	printf ("Node %d, Pid %d: ");
	printf (" return from tdsopen rc=%d (should be unsuccessful)\n", 
		node, pid, rc);
#endif /* DEBUG */
    if (rc != TERR_EXPIRATION_DATE)
    {
	printf ("Node %d, Pid %d:  *** tdsopen incorrect error=%d (expecting %d) - aborting\n", node, pid, rc, TERR_EXPIRATION_DATE);
	errexit ();
    }
    /*
     *	Open data set 2 (should fail because the user exit return -1
     * and the dataset not yet expire.  -1 value means not to write). 
     */
    retval = -1;
    printf ("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", 
		node, pid, dcb.dsname, dcb.dsseqno);
    rc = tdsopen (fd, &dcb);
#if DEBUG
    printf("Node %d, Pid %d: tdsopen returns rc=%d (should be unsuccessful)\n", 
		node, pid, rc);
#endif /* DEBUG */
    if (rc != TERR_EXPIRATION_DATE)
    {
	printf ("Node %d, Pid %d:  ");
	printf ("*** tdsopen bad error=%d (expect %d) aborting\n", 
		node, pid, rc, TERR_EXPIRATION_DATE);
	errexit ();
    }

    /*
     *	Open data set 2. (writing successful because the user exit
     * value "retval"=1 which means that although the dataset is not yet
     * expired, write anyway) 
     */
    retval = 1;
    dsopen ();

    /*
     *	Write some data. 
     */
    printf ("Node %d, Pid %d:  starting twrite loop\n", node, pid);
    for (j = 1; j < JMAX; j++)
    {
	for (i = 1; i < IMAX; i++)
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

    /*
     *	Open data set 3 (should fail). 
     */
    retval = -1;
    strcpy (dcb.dsname, "VB014.X105.X500  ");
    dcb.dsseqno++;

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 4;
    dcb.reclen = 105;
    dcb.blocklen = 500;

    printf ("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", 
		node, pid, dcb.dsname, dcb.dsseqno);
    rc = tdsopen (fd, &dcb);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsopen rc=%d (should be unsuccessful)\n", node, pid, rc);
#endif /* DEBUG */
    if (rc != TERR_EXPIRATION_DATE)
    {
	printf ("Node %d, Pid %d:  ");
	printf ("*** tdsopen incorrect error=%d (expecting %d) - aborting\n", 
		node, pid, rc, TERR_EXPIRATION_DATE);
	errexit ();
    }

    /*
     *	Deallocate tape. 
     */
    printf ("Node %d, Pid %d:  calling tapedealloc\n", node, pid);
    rc = tapedealloc (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tapedealloc rc=%d\n\n", 
		node, pid, rc);
#endif /* DEBUG */
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tapedealloc error=%d - aborting\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
    printf ("Node %d, Pid %d:  code4=%d, code5=%d, code9=%d", 
		node, pid, code4, code5, code9);
    if (code4 != 9)
    {
	printf ("Node %d, Pid %d:  Incorrect code4 value - expecting %d\n", 
		node, pid, 9);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
    if (code5 != 9)
    {
	printf ("Node %d, Pid %d:  Incorrect code5 value - expecting %d\n", 
		node, pid, 9);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
    if (code9 != 4)
    {
	printf ("Node %d, Pid %d:  Incorrect code9 value - expecting %d\n", 
		node, pid, 4);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
    printf ("\nNode %d, Pid %d:  Test PASSED\n", node, pid);
    exit (0);
}
