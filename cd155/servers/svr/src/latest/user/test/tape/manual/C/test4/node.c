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
 *  test4.c
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
 *	The LENGTH constant is changed from 33*1024 to 32*1024-1 because
 *	to make it consistent with the LENGTH value in "test3" althought it
 * 	is not necessary.  (see "test3/node_a.c" comment)              
 *
 *----------------------------------------------------------------------------*/
#define DEBUG 1
#define DEBUG_CODE -1
#define TAPEDEV "/dev/io1/rmt6"
#define OWNER "Greg Tensa"
#define	TEST0 "TEST0 "
#define	MAXTAPES	5
#define LENGTH (32*1024-1)
#define UEXITRET 1

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
int             code4 = 0, code5 = 0, code6 = 0, code7 = 0;
int             eov = 0;

int 
uexit (code, exitparm)
    int             code;
    EXITPARM       *exitparm;
{
    int             k;
    char           *exarray;

    printf ("Node %d, Pid %d:    UEXIT value %d\n", node, pid, code);
    if ((code == 4) || (code == 5))
    {
	if (strcmp (dcb.dsname, exitparm->dsname) != 0)
	{
	    printf ("Node %d, Pid %d:  *** incorrect dsname [%s], expected [%s] - aborting\n", node, pid, exitparm->dsname, dcb.dsname);
	    printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	    exit (1);
	}
	if (strcmp (volser[eov].serialno, exitparm->volser) != 0)
	{
	    printf ("Node %d, Pid %d:  *** incorrect volser [%s], expected [%s] - aborting\n", node, pid, exitparm->volser, volser[0].serialno);
	    printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	    exit (1);
	}
	if (dcb.dsseqno != exitparm->dsseqno)
	{
	    printf ("Node %d, Pid %d:  *** incorrect dsseqno = %d, expected %d - aborting\n", node, pid, exitparm->dsseqno, dcb.dsseqno);
	    printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	    exit (1);
	}
	if (eov + 1 != exitparm->volseqno)
	{
	    printf ("Node %d, Pid %d:  *** incorrect volseqno = %d, expected %d - aborting\n", node, pid, exitparm->volseqno, dcb.volseqno);
	    printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	    exit (1);
	}
	exarray = (char *) exitparm;
	for (k = 0; k <= 28; k++, exarray++)
	{
	    exitparm->buffer[k] = *exarray;
	}
    } else
    if ((code != 6) && (code != 7))
    {
	printf ("Node %d, Pid %d:  *** unexpected user exit = %d - aborting\n", 		node, pid, code);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
    if (code == 4)
    {
	exitparm->buffer[75] = code4 & 0x7f;
	code4++;
    }
    if (code == 5)
    {
	exitparm->buffer[75] = code5 & 0x7f;
	code5++;
    }
    if (code == 7)
	code7++;
    if (code == 6)
    {
	code6++;
	eov++;
    }
    return (UEXITRET);
}


main (ac, av)
    int             ac;
    char          **av;

{
    node = mynode ();
    pid = mypid ();

    /*
     *	Initialize write buffer. 
     */
    for (i = 0, j = 0; i < LENGTH; i++, j++)
    {
	if (j == 256)
	    j = 0;
	buf[i] = j;
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
	printf ("Node %d, Pid %d:  volser %2d = %s\n", 
		node, pid, i, volser[i].serialno);
	if (volser[i].serialno[0] == 0)
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
    printf ("Node %d, Pid %d:  return from tapealloc fd=%d\n\n", 
		node, pid, fd);
#endif
    if (fd < 0)
    {
	printf ("Node %d, Pid %d:  *** tapealloc error=%d - aborting\n", 
		node, pid, fd);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }

	set_spider( DEBUG_CODE );

#if 1
    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "UB101.LONG0      ");
    dcb.volseqno = 1;
    dcb.dsseqno = 1;
    dcb.expirec = ' ';
    dcb.expireyy = 00;
    dcb.expiredd = 00;
    /* dcb.exlst = (char *)NULL; */
    dcb.recfm = 'U';
    dcb.blocklen = 32000;
    dcb.reclen = 0;
    dcb.blkattr = 'B';
    dcb.read = 0;
    dcb.buffered_mode = 0;
    dcb.block_mode = 1;
    dcb.num_tapeblk_bufs = 1;


    printf ("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", 
		node, pid, dcb.dsname, dcb.dsseqno);
    rc = tdsopen (fd, &dcb);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsopen rc=%d\n\n", node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tdsopen error=%d - aborting\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }

    /*
     *	Write some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting twrite loop\n", node, pid);
    for (j = 1; j < 2; j++)
    {
	for (i = 1; i < 101; i++)
	{
	    buf[0] = i;
	    if (eov != 1)
		j = 0;
	    len = twrite (fd, buf, 32000);
	    if (len != 32000)
	    {
		printf ("Node %d, Pid %d:  *** twrite error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit (1);
	    }
	}
    }


    /*
     *	Close Data Set. 
     */
    printf ("Node %d, Pid %d:  calling tdsclose\n", node, pid);
    rc = tdsclose (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsclose rc=%d\n\n", node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tdsclose error=%d - aborting\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
#endif


#if 1
    /*
     *	Open data set 2. 
     */
    strcpy (dcb.dsname, "UB101B.LONG0     ");
    dcb.volseqno = 2;
    dcb.dsseqno = 2;
    dcb.expirec = ' ';
    dcb.expireyy = 00;
    dcb.expiredd = 00;
    /* dcb.exlst = (char *)NULL; */
    dcb.recfm = 'U';
    dcb.blocklen = 32000;
    dcb.reclen = 0;
    dcb.blkattr = 'B';
    dcb.read = 0;
    dcb.buffered_mode = 0;
    dcb.block_mode = 1;
    dcb.num_tapeblk_bufs = 1;


    printf ("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", 
		node, pid, dcb.dsname, dcb.dsseqno);
    rc = tdsopen (fd, &dcb);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsopen rc=%d\n\n", node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tdsopen error=%d - aborting\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }

    /*
     *	Write some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting twrite loop\n", node, pid);
    for (j = 1; j < 2; j++)
    {
	for (i = 1; i < 101; i++)
	{
	    buf[0] = i;
	    if (eov != 3)
		j = 0;
	    len = twrite (fd, buf, 32000);
	    if (len != 32000)
	    {
		printf ("Node %d, Pid %d:  *** twrite error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit (1);
	    }
	}
    }


    /*
     *	Close Data Set. 
     */
    printf ("Node %d, Pid %d:  calling tdsclose\n", node, pid);
    rc = tdsclose (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsclose rc=%d\n\n", node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tdsclose error=%d - aborting\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
#endif

#if 1

    /*
     *	Open data set 3. 
     */
    strcpy (dcb.dsname, "Ub101.X32760     ");
    dcb.volseqno = 4;
    dcb.dsseqno = 3;
    dcb.expirec = ' ';
    dcb.expireyy = 00;
    dcb.expiredd = 00;
    /* dcb.exlst = (char *)NULL; */

    dcb.recfm = 'U';
    dcb.blocklen = 32760;
    dcb.reclen = 0;
    dcb.blkattr = ' ';

    dcb.read = 0;
    dcb.buffered_mode = 0;
    dcb.block_mode = 1;
    dcb.num_tapeblk_bufs = 1;


    printf ("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", 
		node, pid, dcb.dsname, dcb.dsseqno);
    rc = tdsopen (fd, &dcb);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsopen rc=%d\n\n", node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tdsopen error=%d - aborting\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }

    /*
     *	Write some data. 
     */
    len = 1;
    printf ("Node %d, Pid %d:  starting twrite loop\n", node, pid);
    for (j = 1; j < 2; j++)
    {
	for (i = 1; i < 20; i++)
	{
	    buf[0] = i;
	    len = twrite (fd, buf, 32760);
	    if (len != 32760)
	    {
		printf ("Node %d, Pid %d:  *** twrite error: len=%d i=%d j=%d ***aborting***\n", node, pid, len, i, j);
		printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
		exit (1);
	    }
	}
    }


    /*
     *	Close Data Set. 
     */
    printf ("Node %d, Pid %d:  calling tdsclose\n", node, pid);
    rc = tdsclose (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsclose rc=%d\n\n", node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tdsclose error=%d - aborting\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
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
    printf ("Node %d, Pid %d:  User exit counts: code4=%d, code5=%d, code6=%d, code7=%d\n", node, pid, code4, code5, code6, code7);
    if (UEXITRET == 1)
    {
	if ((code4 != 6 * 8) || (code5 != 6 * 8))
	{
	    printf ("Node %d, Pid %d:  *** incorrect user exit4/5 counts - aborting\n", node, pid);
	    printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	    exit (1);
	}
    } else
    {
	if ((code4 != 6) || (code5 != 6))
	{
	    printf ("Node %d, Pid %d:  *** incorrect user exit4/5 counts - aborting\n", node, pid);
	    printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	    exit (1);
	}
    }
    if (code6 != 3)
    {
	printf ("Node %d, Pid %d:  *** incorrect user exit6 counts - aborting\n", node, pid);
	printf ("Node %d, Pid %d:  Test FAILED\n", node, pid);
	exit (1);
    }
    printf ("\nNode %d, Pid %d:  Test PASSED\n", node, pid);
    exit (0);
}
