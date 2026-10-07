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
 * nondisclosure agreement with Intel Corporation and may not be copied or
 * disclosed except in accordance with the terms of that agreement. 
 *
 * tgetblk.c 11.1 94/03/22 16:49:40 
 */
static char     tgetblk_ver[] = "@(#) sourcefile tgetblk.c 11.1 94/03/22 16:49:40";

/* 
******************  ENGINEERING CHANGE HISTORY *********************** 
 *
 * Date      Engineer                  Description 
 *
 * 2/25/92     G. Haycox       Module creation release to BETA 
 *
 *
 */

/*
 * tgetblk.c 
 *
 * INPUT:   handle: handle from the TALLOCTAPE 
 *
 * OUTPUT:  positive number: Current Block ID 
 *
 * negative number: indicating an error 
 *
 * Description: 
 *
 * This routine gets the Block ID information from the tape device for the
 * current location. 
 *
 * A call to sync_tio must be issued to allow any outstanding iread or iwrite
 * commands to complete. 
 *
 * TREAD: Position the tape to the block that will return the BLOCK ID for the
 * last record read. 
 *
 * TWRITE: Position the tape to the block that will return the BLOCK ID for the
 * last record written. 
 *
 *
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <mtio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <nodedef.h>
#include <msgtype.h>
#include <pid.h>
#include <cfs.h>

/*
 * #include <diskproc.h> #include <nameproc.h> 
 */
#include <fio.h>
#include "type.h"
#include "tapeio.h"
#include "tape3480.h"
#include "tioerr.h"
#include "tioproto.h"

#define	SYNC	0

extern TAPEMAIN tapemain;

int
tget_blk_id (int handle)
{
    int             return_val = 0, blk_id;
    struct mtop     mt_ioctl;
    POSITION        pos;
    LVCB           *lvcb_p;
    TAPE_IO        *tape_io_p;


#if SPIDER

    int             spider = tapeio_glob.spider;

#endif

    if ((lvcb_p = tapemain.lvcblist[handle]) == NULL)
    {

	fprintf (stderr, "\n%s #%04d ERROR:  %s", __FILE__, __LINE__,
		 err_info[ERR_INVALID_HANDLE].err_str);
	fprintf (stderr, " (%d)\n", handle);
	fflush (stderr);

	return (err_info[ERR_INVALID_HANDLE].err_code);
    }
    tape_io_p = (TAPE_IO *) & (lvcb_p->tape_io);

#if SPIDER
    if (spider & SPIDER_ENTER_EXIT)
    {
	fprintf (tape_io_p->logdev, "\n");
	PRN_LOC (" ENTRY:");
	fprintf (tape_io_p->logdev, " Handle: %d\n", handle);
	fprintf (tape_io_p->logdev,
		 " LVCB ptr: %08X TAPE_IO ptr: %08X\n",
		 lvcb_p, tape_io_p);
    }
#endif

    do
    {
	if (tape_io_p->dmem_p == NULL)
	{
	    tio_log_init (lvcb_p);
	    ERR_PRN (err_info[ERR_TGET_DS_NOT_OPEN].err_str);
	    return_val = err_info[ERR_TGET_DS_NOT_OPEN].err_code;
	    break;
	}
	if (tape_io_p->eof)
	{
	    ERR_PRN (err_info[ERR_ATTEMPT_TGET_BLKID_EOF].err_str);
	    return_val = err_info[ERR_ATTEMPT_TGET_BLKID_EOF].err_code;
	    break;
	}
	/*--------------------------------------------------
         * If the previous block id is still valid then
         * return it, else determine it.
         --------------------------------------------------*/

	if (return_val = tape_io_p->block_id)
	{
	    printf ("tget_blk_id: tape_io_p->block_id=0x%x\n",tape_io_p->block_id);
	    break;
	}
	printf ("tget_blk_id: tape_io_p->block_id=0x%x\n",tape_io_p->block_id);
#if SYNC
	if ((return_val = sync_tio (lvcb_p, tape_io_p)) < 0)
	    break;
#endif
#if SYNC		
	/* there is no read ahead buffering with standard unix read/write */
	if (lvcb_p->read)
	{
	    /*--------------------------------------------------
             * Position tape back to the block of the last
             * record read.
             *
             * If a tape mark was read on the read-a-head
             * buffering then issue a BSF command.
             *
             --------------------------------------------------*/

	    if (tape_io_p->eod)
	    {
#if SPIDER
		if (spider & SPIDER_TGET_BLKID)
		    PRN_LOC (" At EOT side of TM, issue BSF\n");
#endif
		mt_ioctl.mt_op = MTBSF;
		mt_ioctl.mt_count = 1;
		if (ioctl (lvcb_p->fildes, MTIOCTOP, &mt_ioctl) < 0)
		{

		    ERR_PRN (err_info[ERR_CFS_IOCTL].err_str);
		    fprintf (tape_io_p->logdev,
			     " fd: %d MTBSF errno = %d\n",
			     lvcb_p->fildes, errno);
		    return_val = err_info[ERR_CFS_IOCTL].err_code;
		    break;

		}
		tape_io_p->eod = FALSE;
	    }
	    /*---------------------------------------------------
             * Determine the number of BSR commands that have
             * to be issued to position the tape to the block
             * of the record last read.
             *
             * If TREAD is currently processing a block
             *  then BSR  phy - blkcnt .. blocks
             * If TREAD is in a GAP then BSR
             *  (phy - blkcnt) + 1 .. blocks
             *
             --------------------------------------------------*/

	    mt_ioctl.mt_count = tape_io_p->phy_blk_num -lvcb_p->blockcnt;
	    printf ("tget_blk_id: mt_ioctl.mt_count=%d\n", mt_ioctl.mt_count);

	    /*--------------------------------------------------
             * If we are in a GAP and have previously read some
             * data, Then another BSR must be issued.  Also
             * reset the trunc_blk flag in case the read-ahead
             * buffering encounterred a truncated block before
             * the user got there.
             *
             * lvcb_p->blockcnt == 0 at the EOT side of
             *  the TM at the beginning of the dataset
             *          - or -
             *  After a locate has been issued.
             --------------------------------------------------*/

	    if (!tape_io_p->blk_proc_p && lvcb_p->blockcnt)
	    {
	 	printf ("tget_blk_id: tape_io_p->trunc_blk = FALSE\n");
		tape_io_p->trunc_blk = FALSE;
		++mt_ioctl.mt_count;
	    }
	    printf ("tget_blk_id: mt_ioctl.mt_count=%d\n", mt_ioctl.mt_count);

	} else
	{			/* write */
	    /*--------------------------------------------------
             * A BSR will need to be issued if the current
             * tape block buffer is EMPTY and not at BLK0,
             * to return the BLKID for the last block written.
             --------------------------------------------------*/

	    mt_ioctl.mt_count = (!tape_io_p->blk_proc_p &&
				 lvcb_p->blockcnt) ? 1 : 0;
	}


	if (mt_ioctl.mt_count)
	{
	    /*--------------------------------------------------
             * Issue the BSR commands.
             --------------------------------------------------*/

	    mt_ioctl.mt_op = MTBSR;	/* back space block */

#if SPIDER
	    if (spider & SPIDER_TGET_BLKID)
	    {
		PRN_LOC (" Sync Buffers: ");
		fprintf (tape_io_p->logdev, " issue %d BSR cmd(s)\n",
			 mt_ioctl.mt_count);
	    }
#endif
	    if (ioctl (lvcb_p->fildes, MTIOCTOP, &mt_ioctl) < 0)
	    {

		ERR_PRN (err_info[ERR_CFS_IOCTL].err_str);
		fprintf (tape_io_p->logdev,
			 " fd: %d MTBSR(%d) errno = %d\n",
			 lvcb_p->fildes, mt_ioctl.mt_count, errno);
		return_val = err_info[ERR_CFS_IOCTL].err_code;
		break;
	    }
	    printf ("tget_blk_id: tape_io_p->phy_blk_num=%d\n",tape_io_p->phy_blk_num);
	    tape_io_p->phy_blk_num -= mt_ioctl.mt_count;
	    printf ("tget_blk_id: after adjustment \n");
	    printf ("tget_blk_id: tape_io_p->phy_blk_num=%d\n",tape_io_p->phy_blk_num);
	}
#endif

	/*--------------------------------------------------
         * Issue the read_tape_position command to get
         * the BLOCK ID.
         --------------------------------------------------*/
#if SPIDER
	if (spider & SPIDER_TGET_BLKID)
	{
	    PRN_LOC (" Issue read_tape_position: ");
	}
#endif
	if (read_tape_position (lvcb_p->fildes, &pos) < 0)
	{
	    ERR_PRN (err_info[ERR_RD_TAPE_POS].err_str);
	    fprintf (tape_io_p->logdev, " return = %d\n", blk_id);
	    return_val = err_info[ERR_RD_TAPE_POS].err_code;
	    break;
	}
#if SPIDER
	if (spider & SPIDER_TGET_BLKID)
	{
	    PRN_LOC ("");
	    fprintf (tape_io_p->logdev, "BLKID = %08X\n", pos.first_block);
	}
#endif
#if SYNC	
	/* if we do not check the read ahead buffering above we do not need to 
	 * correct for it here.
	 */
	if (mt_ioctl.mt_count)
	{

	    /*--------------------------------------------------
             * If any BSR commands were issued then,
             * Issue a FSR command to get back to the
             * beginning of the next block.
             --------------------------------------------------*/

	    mt_ioctl.mt_op = MTFSR;	/* forward space block */
	    mt_ioctl.mt_count = 1;

#if SPIDER
	    if (spider & SPIDER_TGET_BLKID)
	    {
		PRN_LOC (" Reposition: issue fwd space block\n");
		fprintf (tape_io_p->logdev, " issue %d FSR cmd(s)\n",
			 mt_ioctl.mt_count);
	    }
#endif

	    if (ioctl (lvcb_p->fildes, MTIOCTOP, &mt_ioctl) < 0)
	    {

		ERR_PRN (err_info[ERR_CFS_IOCTL].err_str);
		fprintf (tape_io_p->logdev,
			 " fd: %d MTFSR(%d) errno = %d\n",
			 lvcb_p->fildes, mt_ioctl.mt_count, errno);
		return_val = err_info[ERR_CFS_IOCTL].err_code;
		break;
	    }
	    ++tape_io_p->phy_blk_num;
	}
#endif
    } while (0);

    return ((return_val) ? return_val : pos.first_block);
}
