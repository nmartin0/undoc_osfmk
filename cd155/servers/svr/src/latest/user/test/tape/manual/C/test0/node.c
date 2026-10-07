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
 *  test0.c:  Negative tdsopen testing
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
 *	to make it consistent with the LENGTH value in other testcase althought
 * 	it is not necessary.  (see "test1/node_a.c" comment)              
 *
 *----------------------------------------------------------------------------*/
#define DEBUG	1
#define	DOALL	1

#define TAPEDEV "/dev/io1/rmt6"
#define OWNER "Greg Tensa"
#define	TEST01 "TEST01"
#define	MAXTAPES	2
#define LENGTH (31*1024)

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <t3480.h>
#include <tioec.h>

int             fd, rc, i, j;
int             len;
int             node, pid;
char            tapedev[80];
char            buf[LENGTH];
char            lnum = '1';
VOLSER          volser[MAXTAPES];
DCB             dcb;
VCB             vcb;

void
dsopen (int experr)
{
    printf ("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", 
		node, pid, dcb.dsname, dcb.dsseqno);
    rc = tdsopen (fd, &dcb);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsopen rc=%d\n\n", node, pid, rc);
#endif
    if (rc != experr)
    {
	printf ("Node %d, Pid %d:  *** tdsopen incorrect error: ",node,pid);
	printf ("rc=%d (expecting %d) - aborting\n", rc, experr);
	errexit ();
    }
}


int
uexit (code, exitparm)
    int             code;
    EXITPARM       *exitparm;
{
    printf ("Node %d, Pid %d:  \nUEXIT:  called with value %d\n", 
		node, pid, code);
    return (0);
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
	volser[0].serialno[6] = NULL;
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
	printf ("Node %d, Pid %d:  Test FAILED\n\n", node, pid);
	exit (1);
    }
    set_tapedev (fd, 0, 1);     /* 4980, no unload ### */

    dcb.volseqno = 1;
    dcb.dsseqno = 1;
    dcb.read = 0;
    dcb.expirec = ' ';
    dcb.expireyy = 00;
    dcb.expiredd = 00;


#if 1

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Fb011BL.ne.RL    ");

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_NON_INTEGRAL_REC_SIZE);


    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Fb011BL.eq.X0    ");

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Fb011BL.eq.X32761");

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 32761;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);


    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Fb000BL.ne.RL    ");

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_NON_INTEGRAL_REC_SIZE);


    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Fb000BL.eq.X0    ");

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);


    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Fb000BL.eq.X32761");

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 32761;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);


    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Fb010            ");

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_NON_INTEGRAL_REC_SIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Fb111BL.eq.X0    ");

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);


    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Fb111BL.eq.X32761");

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 32761;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "FB011BL.nmod.RL  ");

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_NON_INTEGRAL_REC_SIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "FB011RL.gt.BL    ");

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 500;
    dcb.blocklen = 100;

    dsopen (TERR_NON_INTEGRAL_REC_SIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "FB011BL.eq.X0    ");

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "FB011BL.eq.X32761");

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 32761;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "FB000BL.ne.RL    ");

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_NON_INTEGRAL_REC_SIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "FB000BL.eq.X0    ");

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "FB000BL.eq.X32761");

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 32761;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "FB010            ");

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_NON_INTEGRAL_REC_SIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "FB111BL.eq.X0    ");

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "FB111BL.eq.X32761");

    dcb.recfm = 'F';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 32761;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Ub011RL.ne.X0    ");

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_UNDEF_REC_SIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Ub011BL.eq.X0    ");

    dcb.recfm = 'F';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Ub011BL.eq.X32761");

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Ub000RL.ne.X0    ");

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_UNDEF_REC_SIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Ub000BL.eq.X0    ");

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Ub000BL.eq.X32761");

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 0;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Ub010            ");

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 0;
    dcb.blocklen = 500;

    dsopen (TERR_INVALID_NUM_BUFS);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Ub111BL.eq.X0    ");

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Ub111BL.eq.X32761");

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Ub111RL.ne.X0    ");

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 01;
    dcb.blocklen = 3276;

    dsopen (TERR_UNDEF_REC_SIZE);


    /* PAGE     */

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "UX011            ");

    dcb.recfm = 'U';
    dcb.blkattr = 'X';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 3276;

    dsopen (TERR_INVALID_BLK_ATTR);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "FX011            ");

    dcb.recfm = 'F';
    dcb.blkattr = 'X';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 100;
    dcb.blocklen = 500;

    dsopen (TERR_INVALID_BLK_ATTR);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Vb011BL.ng.XRLp4 ");

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 497;
    dcb.blocklen = 500;

    dsopen (TERR_VAR_RECLEN);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VS011BL.ng.XRLp4 ");

    dcb.recfm = 'V';
    dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 497;
    dcb.blocklen = 500;

    dsopen (TERR_VAR_RECLEN);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VR011BL.ng.XRLp4 ");

    dcb.recfm = 'V';
    dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 497;
    dcb.blocklen = 500;

    dsopen (TERR_VAR_RECLEN);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Vb011BL.eq.X0    ");

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VS011BL.eq.X0    ");

    dcb.recfm = 'V';
    dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VR011BL.eq.X0    ");

    dcb.recfm = 'V';
    dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Vb011BL.eq.X32761");

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 3276;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VS011BL.eq.X32761");

    dcb.recfm = 'V';
    dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 3276;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VR011BL.eq.X32761");

    dcb.recfm = 'V';
    dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 3276;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Vb011RL.eq.X4    ");

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 4;
    dcb.blocklen = 500;

    dsopen (TERR_VAR_RECLEN);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VS011RL.eq.X4    ");

    dcb.recfm = 'V';
    dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 4;
    dcb.blocklen = 500;

    dsopen (TERR_VAR_RECLEN);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VR011RL.eq.X4    ");

    dcb.recfm = 'V';
    dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 4;
    dcb.blocklen = 500;

    dsopen (TERR_VAR_RECLEN);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Vb000            ");

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_INVALID_NON_BUFFERED_MODE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VS000            ");

    dcb.recfm = 'V';
    dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_INVALID_NON_BUFFERED_MODE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VR000            ");

    dcb.recfm = 'V';
    dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_INVALID_NON_BUFFERED_MODE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Vb010            ");

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_INVALID_NUM_BUFS);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VS010            ");

    dcb.recfm = 'V';
    dcb.blkattr = 'S';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_INVALID_NUM_BUFS);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VR010            ");

    dcb.recfm = 'V';
    dcb.blkattr = 'R';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_INVALID_NUM_BUFS);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Vb111BL.eq.X0    ");

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VS111BL.eq.X0    ");

    dcb.recfm = 'V';
    dcb.blkattr = 'S';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VR111BL.eq.X0    ");

    dcb.recfm = 'V';
    dcb.blkattr = 'R';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Vb111BL.eq.X32761");

    dcb.recfm = 'V';
    dcb.blkattr = ' ';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 3276;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VS111BL.eq.X32761");

    dcb.recfm = 'V';
    dcb.blkattr = 'S';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 3276;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VR111BL.eq.X32761");

    dcb.recfm = 'V';
    dcb.blkattr = 'R';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 3276;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VB111BL.eq.X0    ");

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VB111BL.eq.X32761");

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 1;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 5;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VB011RL.gt.BL    ");

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 500;
    dcb.blocklen = 100;

    dsopen (TERR_VAR_RECLEN);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VB011BL.eq.X0    ");

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 0;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VB011RL.eq.X4    ");

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 4;
    dcb.blocklen = 90;

    dsopen (TERR_VAR_RECLEN);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VB011BL.eq.X32761");

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 3276;
    dcb.blocklen = 32761;

    dsopen (TERR_BLOCKSIZE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VB011BL.ng.XRLp4 ");

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 3276;
    dcb.blocklen = 3279;

    dsopen (TERR_VAR_RECLEN);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VB000            ");

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 0;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_INVALID_NON_BUFFERED_MODE);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VB010            ");

    dcb.recfm = 'V';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 0;
    dcb.reclen = 104;
    dcb.blocklen = 500;

    dsopen (TERR_INVALID_NUM_BUFS);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "VX011            ");

    dcb.recfm = 'V';
    dcb.blkattr = 'X';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 327;
    dcb.blocklen = 3276;

    dsopen (TERR_INVALID_BLK_ATTR);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "Qb011            ");

    dcb.recfm = 'Q';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 3276;

    dsopen (TERR_REC_FM);

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "XB011            ");

    dcb.recfm = 'X';
    dcb.blkattr = 'B';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 327;
    dcb.blocklen = 3276;

    dsopen (TERR_REC_FM);


    /*
     *	Write some data (should fail). 
     */
    printf ("Node %d, Pid %d:  calling twrite\n", node, pid);
    len = twrite (fd, buf, 100);
#if DEBUG
    printf ("Node %d, Pid %d:  return from twrite len=%d\n\n", node, pid, len);
#endif
    if (len != TERR_NOT_OPEN_FOR_WRITE)
    {
	printf ("Node %d, Pid %d:  *** twrite incorrect error: ",node, pid);
	printf( "len=%d (expecting %d) - aborting\n",
		len, TERR_NOT_OPEN_FOR_WRITE);
	errexit ();
    }

    /*
     *	Close Data Set (should fail). 
     */
    printf ("Node %d, Pid %d:  calling tdsclose\n", node, pid);
    rc = tdsclose (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsclose rc=%d\n\n", node, pid, rc);
#endif
    if (rc != TERR_NO_DATASET_OPENED)
    {
	printf ("Node %d, Pid %d:  *** tdsclose incorrect error: ",node,pid);
	printf (" rc=%d (expecting %d) - aborting\n",
		rc, TERR_NO_DATASET_OPENED);
	printf ("Node %d, Pid %d:  calling tapedealloc\n", node, pid);
	rc = tapedealloc (fd);
	printf ("Node %d, Pid %d:  return from tapedealloc rc=%d\n\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  Test FAILED at dsseqno=%d\n", 
		node, pid, dcb.dsseqno);
	exit (1);
    }
#endif

    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "only.file        ");

    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.blocklen = 100;
    dcb.reclen = 0;

    printf ("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", 
		node, pid, dcb.dsname, dcb.dsseqno);
    rc = tdsopen (fd, &dcb);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsopen rc=%d\n\n", node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tdsopen error: rc=%d - aborting\n", 
		node, pid, rc);
	errexit ();
    }

    /*
     *	Open data set 2 (should fail). 
     */
    strcpy (dcb.dsname, "Ub011            ");

    dcb.dsseqno = 2;
    dcb.recfm = 'U';
    dcb.blkattr = 'b';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.reclen = 0;
    dcb.blocklen = 3276;

    dsopen (TERR_ALREADY_TDSOPEND);

    /*
     *	Close Data Set. 
     */
    printf ("Node %d, Pid %d:  calling tdsclose\n", node, pid);
    rc = tdsclose (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsclose rc=%d\n\n", node, pid, rc);
#endif
    if (rc != 0)
    {
	printf ("Node %d, Pid %d:  *** tdsclose error: rc=%d - aborting\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  calling tapedealloc\n", node, pid);
	rc = tapedealloc (fd);
	printf ("Node %d, Pid %d:  return from tapedealloc rc=%d\n\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  Test FAILED at dsseqno=%d\n", 
		node, pid, dcb.dsseqno);
	exit (1);
    }

#if	DOALL
    /*
     *	Open data set 1. 
     */
    strcpy (dcb.dsname, "only.file        ");

    dcb.dsseqno = 1;
    dcb.recfm = 'U';
    dcb.blkattr = ' ';
    dcb.block_mode = 0;
    dcb.buffered_mode = 1;
    dcb.num_tapeblk_bufs = 1;
    dcb.blocklen = 100;
    dcb.reclen = 0;
    dcb.read = 1;

    printf ("Node %d, Pid %d:  calling tdsopen: dsname={%17s} dsseqno=%d\n", 
		node, pid, dcb.dsname, dcb.dsseqno);
    rc = tdsopen (fd, &dcb);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsopen rc=%d\n\n", node, pid, rc);
#endif
    if (rc < 0)
    {
	printf ("Node %d, Pid %d:  *** tdsopen error: rc=%d - aborting\n", 
		node, pid, rc);
	errexit ();
    }
    printf ("Node %d, Pid %d:  calling tread\n", node, pid);
    len = tread (fd, buf, 100);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tread len=%d\n\n", node, pid, len);
#endif
    if (len != 0)
    {
	printf ("Node %d, Pid %d:  *** tread error: len=%d expecting 0", 
		node, pid, len);
	printf ("- aborting\n");
	errexit ();
    }

    /*
     *	Close Data Set. 
     */
    printf ("Node %d, Pid %d:  calling tdsclose\n", node, pid);
    rc = tdsclose (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tdsclose rc=%d\n\n", node, pid, rc);
#endif
    if (rc != 0)
    {
	printf ("Node %d, Pid %d:  *** tdsclose error: rc=%d - aborting\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  calling tapedealloc\n", node, pid);
	rc = tapedealloc (fd);
	printf ("Node %d, Pid %d:  return from tapedealloc rc=%d\n\n", 
		node, pid, rc);
	printf ("Node %d, Pid %d:  Test FAILED at dsseqno=%d\n", 
		node, pid, dcb.dsseqno);
	exit (1);
    }
#endif	/* DOALL */

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
	printf ("Node %d, Pid %d:  Test FAILED\n\n", node, pid);
	exit (1);
    }

    /*
     *	Deallocate tape again - should error. 
     */
    printf ("Node %d, Pid %d:  calling tapedealloc\n", node, pid);
    rc = tapedealloc (fd);
#if DEBUG
    printf ("Node %d, Pid %d:  return from tapedealloc rc=%d\n\n", 
		node, pid, rc);
#endif
    if (rc != TERR_INV_HANDLE)
    {
	printf ("Node %d, Pid %d:  *** tapedealloc incorrect error=%d (expecting %d) - aborting\n",
		node, pid, rc, TERR_INV_HANDLE);
	printf ("Node %d, Pid %d:  Test FAILED\n\n", node, pid);
	exit (1);
    }
    printf ("\nNode %d, Pid %d:  Test PASSED\n", node, pid);
    exit (0);
}
