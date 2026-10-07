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
 * tio_init.c 11.1 94/03/22 16:49:45 
 */
static char     tio_init_ver[] = "@(#) sourcefile tio_init.c 11.1 94/03/22 16:49:45";

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
 * tio_init.c 
 *
 * INPUT:   handle:     handle returned by the TAPEALLOC command 
 *
 * OUTPUT:  int:        errors (TBD) 
 *
 * Description: 
 *
 * This routine checks the different record format types for acceptable
 * parameters within the DCB. 
 *
 * Memory is allocated for the tape block buffers if buffered mode is true. 
 *
 * Memory is allocated for one tape control structure if buffered mode is false. 
 *
 * The main tape_io structure is initialized, and the tape block buffer headers
 * are initialized and the buffers linked if they are present (buffered
 * mode). 
 *
 * If buffered mode and the data set is openned for read the first iread command
 * is issued here to prime the pipe. 
 *
 */

#include <stdio.h>
#include <time.h>

/*
 * #include "malloc.h" 
 */
#include "type.h"
#include "tapeio.h"
#include "tape3480.h"
#include "tioerr.h"
#include "tioproto.h"

extern TAPEMAIN tapemain;
char            input[80];

int
tape_io_init (int handle)
{
    int		blk_buf_size, num_bufs, i, return_val = 0;
    BLK_BUF_HDR	*blk_p;
    LVCB	*lvcb_p;
    TAPE_IO	*tape_io_p;
    char	*temp_p;
    char	buffer[1];
    int		count=0;
    int		length=sizeof(buffer);	

#if SPIDER
    int             spider = tapeio_glob.spider;
#endif

    do
    {
	if ((lvcb_p = tapemain.lvcblist[handle]) == NULL)
	{

	    fprintf (stderr, "\n%s #%04d ERROR:  %s", __FILE__, __LINE__,
		     err_info[ERR_INVALID_HANDLE].err_str);
	    fprintf (stderr, " (%d)\n", handle);
	    fflush (stderr);

	    return_val = err_info[ERR_INVALID_HANDLE].err_code;
	    break;
	}
	tio_log_init (lvcb_p);
	tape_io_p = (TAPE_IO *) & (lvcb_p->tape_io);
	tape_io_p->dmem_p = (void *) NULL;

#if SPIDER
	if (spider & SPIDER_ENTER_EXIT)
	{
	    fprintf (tape_io_p->logdev, "\n");
	    PRN_LOC (" ENTRY:");
	    fprintf (tape_io_p->logdev, " Handle: %d  LVCB ptr: %08X \n",
		     handle, lvcb_p);

	}
	if ((spider == 0) || (spider & (SPIDER_TAPEIO_INIT)))
	{

	    /*--------------------------------------------------
             * Print pertinent DCB fields
             --------------------------------------------------*/
	    fprintf (tape_io_p->logdev, "\n");
	    PRN_LOC (" TAPE and DCB INFO\n>> ");

	    log_time (tape_io_p->logdev);

	    /*
	     * fprintf(tape_io_p->logdev, ">> NODE: %d\n", _mydirect()); 
	     */
	    fprintf (tape_io_p->logdev, ">> VOLID: %s  DSNAME: %s\n",
		     lvcb_p->vol1.volser,
		     lvcb_p->rec1.dsname);
	    fprintf (tape_io_p->logdev, ">> %s:  %s  %s\n",
		     (lvcb_p->read) ? "READ" : "WRITE",
		   (lvcb_p->block_mode) ? "BLOCKMODE" : "USE RECFM/BLKATTR",
		     (lvcb_p->buffered_mode) ? "BUFFERED" : "UNBUFFERED");
	    if (lvcb_p->buffered_mode)
	    {
		fprintf (tape_io_p->logdev, ">> num_tapeblk_bufs: %d\n",
			 lvcb_p->num_tapeblk_bufs);
	    }
	    fprintf (tape_io_p->logdev, ">> recfm: %c  blkattr: %c\n",
		     lvcb_p->rec2.recfm, lvcb_p->rec2.blkattr);
	    fprintf (tape_io_p->logdev, ">> blocklen: %d reclen: %d\n",
		     lvcb_p->rec2.blocklen,
		     lvcb_p->rec2.reclen);

	}
#endif

	if ((lvcb_p->rec2.blocklen > 32760) || (lvcb_p->rec2.blocklen == 0))
	{

	    ERR_PRN (err_info[ERR_BLOCKSIZE].err_str);
	    fprintf (tape_io_p->logdev, " (%05d)\n", lvcb_p->rec2.blocklen);

	    return_val = err_info[ERR_BLOCKSIZE].err_code;
	    break;
	}
	/*--------------------------------------------------
        * Convert the ASCII fields for record format,
        * block attribute to Numerics
        --------------------------------------------------*/

	switch (lvcb_p->rec2.recfm)
	{
	case 'U':
	    tape_io_p->record_format = UNDEFINED;
	    break;
	case 'F':
	    tape_io_p->record_format = FIXED;
	    break;
	case 'V':
	    tape_io_p->record_format = VARIABLE;
	    break;
	default:
	    ERR_PRN (err_info[ERR_REC_FM].err_str);
	    fprintf (tape_io_p->logdev, " (%c)\n", lvcb_p->rec2.recfm);
	    return_val = err_info[ERR_REC_FM].err_code;
	    break;
	}
	if (return_val)
	{
	    break;
	}
	switch (lvcb_p->rec2.blkattr)
	{
	case 'B':
	    tape_io_p->record_format |= BLOCKED_RECORDS;
	    break;
	case 'S':
	    tape_io_p->record_format |= SPAN_OR_STD_RECORDS;
	    break;
	case 'R':
	    tape_io_p->record_format |= BLK_S_RECORDS;
	    break;
	case ' ':
	    break;
	default:
	    ERR_PRN (err_info[ERR_INVALID_BLK_ATTR].err_str);
	    fprintf (tape_io_p->logdev, " (%c)\n", lvcb_p->rec2.blkattr);
	    return_val = err_info[ERR_INVALID_BLK_ATTR].err_code;
	    break;
	}
	if (return_val)
	{
	    break;
	}
#if SPIDER
	if (spider & SPIDER_TAPEIO_INIT)
	{
	    PRN_LOC (" CALC blk_buf_size and set num_bufs\n");
	    fprintf (tape_io_p->logdev, ">> record_format: %02X\n",
		     tape_io_p->record_format);
	}
#endif

	switch (tape_io_p->record_format & REC_FORMAT)
	{
	case UNDEFINED:
	    if (lvcb_p->rec2.reclen != 0)
	    {
		ERR_PRN (err_info[ERR_UNDEF_REC_SIZE].err_str);
		fprintf (tape_io_p->logdev, " reclen = %05d\n",
			 lvcb_p->rec2.reclen);
		return_val = err_info[ERR_UNDEF_REC_SIZE].err_code;
		break;

	    }
	    break;
	case FIXED:
	    if ((lvcb_p->rec2.reclen == 0) ||
		(lvcb_p->rec2.blocklen % lvcb_p->rec2.reclen))
	    {

		ERR_PRN (err_info[ERR_NON_INTEGRAL_REC_SIZE].err_str);
		fprintf (tape_io_p->logdev, " blocklen = %05d reclen = %05d\n",
			 lvcb_p->rec2.blocklen, lvcb_p->rec2.reclen);
		return_val = err_info[ERR_NON_INTEGRAL_REC_SIZE].err_code;
		break;
	    }
	    /*--------------------------------------------------
             * Check to ensure that the rec2.blocklen and
             * rec2.reclen are equal for
             * FIXED (UNBLOCKED)
             *      - or -
             * UNBUFFERED mode (NOT blockmode)
             --------------------------------------------------*/

	    if ((((tape_io_p->record_format & BLK_ATTR) ==
		  UNBLOCKED_RECORDS) ||
		 (!lvcb_p->buffered_mode && !lvcb_p->block_mode)) &&
		(lvcb_p->rec2.blocklen != lvcb_p->rec2.reclen))
	    {

		ERR_PRN (err_info[ERR_FIXED_RECLEN].err_str);
		fprintf (tape_io_p->logdev, " blocklen = %05d reclen = %05d\n",
			 lvcb_p->rec2.blocklen, lvcb_p->rec2.reclen);
		return_val = err_info[ERR_FIXED_RECLEN].err_code;
	    }
	    break;
	case VARIABLE:
	    if (!lvcb_p->buffered_mode && !lvcb_p->block_mode)
	    {
		ERR_PRN (err_info[ERR_INVALID_NON_BUFFERED_MODE].err_str);
		fprintf (tape_io_p->logdev, " Variable not Allowed\n");
		return_val = err_info[ERR_INVALID_NON_BUFFERED_MODE].err_code;
	    } else
		if ((lvcb_p->rec2.reclen >
		     (lvcb_p->rec2.blocklen -sizeof (BDW))) ||
		    (lvcb_p->rec2.reclen < sizeof (RDW) + 1))
	    {

		ERR_PRN (err_info[ERR_VAR_RECLEN].err_str);
		fprintf (tape_io_p->logdev, " blocklen = %05d reclen = %05d\n",
			 lvcb_p->rec2.blocklen, lvcb_p->rec2.reclen);
		return_val = err_info[ERR_VAR_RECLEN].err_code;
	    }
	    break;
	}
	if (return_val)
	{
	    break;
	}
	if (lvcb_p->block_mode)
	{
	    tape_io_p->record_format = UNDEFINED;
	}
	if (lvcb_p->buffered_mode)
	{
	    blk_buf_size = lvcb_p->rec2.blocklen + sizeof (BLK_BUF_HDR);
#if SPIDER
	    if (spider & SPIDER_TAPEIO_INIT)
	    {
		PRN_LOC ("");
		fprintf (tape_io_p->logdev,
			 " blk_buf_size = buf_hdr (%d) + blocklen (%d)\n",
			 sizeof (BLK_BUF_HDR), lvcb_p->rec2.blocklen);
	    }
#endif

	    /* Align the buffers on 32 bit boundaries */

	    if (i = (blk_buf_size % 4))
	    {
		blk_buf_size += (4 -i);
#if SPIDER
		if (spider & SPIDER_TAPEIO_INIT)
		{
		    PRN_LOC ("");
		    fprintf (tape_io_p->logdev,
			     " Add %d to align on 32 bit boundary\n", 4 -i);
		}
#endif
	    }
	    if ((num_bufs = lvcb_p->num_tapeblk_bufs) <= 0)
	    {

		ERR_PRN (err_info[ERR_INVALID_NUM_BUFS].err_str);
		fprintf (tape_io_p->logdev, " (%d)\n", num_bufs);
		return_val = err_info[ERR_INVALID_NUM_BUFS].err_code;
		break;
	    }
	} else
	{			/* non-buffered, only need 1 header */
	    blk_buf_size = sizeof (BLK_BUF_HDR);
	    num_bufs = 1;
	}

#if SPIDER
	if (spider & SPIDER_TAPEIO_INIT)
	{
	    PRN_LOC (" ");
	    fprintf (tape_io_p->logdev,
		     "blk_buf_size: %d num_bufs: %d Total: %d\n",
		     blk_buf_size, num_bufs, (blk_buf_size * num_bufs));
	}
#endif
	if ((tape_io_p->dmem_p = (void *) malloc (blk_buf_size * num_bufs)) ==
	    (void *) NULL)
	{

	    ERR_PRN (err_info[ERR_MEMORY_ALLOC_ERROR].err_str);
	    fprintf (tape_io_p->logdev, " (%d)\n", errno);

	    return_val = err_info[ERR_MEMORY_ALLOC_ERROR].err_code;
	    break;
	}
#if SPIDER
	if (spider & SPIDER_TAPEIO_INIT)
	{
	    PRN_LOC (" ");
	    fprintf (tape_io_p->logdev, "MALLOC: dmem_p = %08X\n",
		     tape_io_p->dmem_p);
	}
#endif
	tape_io_p->head = tape_io_p->tail = tape_io_p->dmem_p;
	tape_io_p->phy_blk_num = 0;
	tape_io_p->block_id = 0;
	tape_io_p->num_records = 0;
	tape_io_p->oper_blked = 0;
	tape_io_p->oper_busy = 0;
	tape_io_p->num_ireads = 0;
	tape_io_p->num_iwrites = 0;
	tape_io_p->num_cwrites = 0;
	tape_io_p->blk_proc_p = (char *) NULL;
	tape_io_p->blk_len = 0;
	tape_io_p->trunc_blk = FALSE;
	tape_io_p->eod = FALSE;
	tape_io_p->eof = FALSE;

	/*--------------------------------------------------
         * The label processing code calls this routine
         * after processing the IBM standard labels with
         * the last operation being a read of the TM, therefor
         * the FEOF flag must be cleared in case another tape
         * mark follows this TM (Empty Data Set).  If two
         * tape marks are read in a row the CFS_READ code
         * disables the read function and always returns 0.
         --------------------------------------------------*/
#if CLREOF
	cleareof (lvcb_p->fildes);
#endif
	/* initialize the BLK_BUF tape block buffer structures */

	for (i = 0, blk_p = tape_io_p->head; i < num_bufs -1; ++i)
	{

	    blk_p->next = (BLK_BUF_HDR *) ((char *) blk_p + blk_buf_size);
	    blk_p->mid = CMD_COMPLETE;
	    blk_p->status = EMPTY;
	    blk_p = (BLK_BUF_HDR *) blk_p->next;
#if SPIDER
	    if (spider & SPIDER_TAPEIO_INIT)
	    {
		PRN_LOC (" ");
		fprintf (tape_io_p->logdev, "BLK_HDR[%d].next: %08X\n",
			 i, blk_p);
	    }
#endif

	}

	/*--------------------------------------------------
        * Wrap the last tape block buffer to the first
        --------------------------------------------------*/

	blk_p->next = (BLK_BUF_HDR *) tape_io_p->head;
	blk_p->mid = CMD_COMPLETE;
	blk_p->status = EMPTY;

#if SPIDER
	if (spider & SPIDER_TAPEIO_INIT)
	{
	    PRN_LOC (" ");
	    fprintf (tape_io_p->logdev, "BLK_HDR[%d].next: %08X\n",
		     i, blk_p->next);
	}
#endif

	/*--------------------------------------------------
	* if buffered mode and data set is open for read
	* then issue the first iread here
	*
	--------------------------------------------------*/

	if (lvcb_p->read && lvcb_p->buffered_mode)
	{

#if SPIDER
	    if (spider & SPIDER_TAPEIO_INIT)
	    {
		PRN_LOC (" Call issue_iread (5)\n");
	    }
#endif
	    if ((return_val = issue_iread (lvcb_p->fildes, tape_io_p,
			      (char *) &((BLK_BUF *) tape_io_p->head)->data,
					   lvcb_p->rec2.blocklen)) < 0)
		break;
	}
    } while (0);		/* to provide 1 exit point here */

#if SPIDER
    if (spider & SPIDER_ENTER_EXIT)
    {
	PRN_LOC (" EXIT:");
	fprintf (tape_io_p->logdev, " return value: %d\n\n", return_val);
    }
    fflush (tape_io_p->logdev);

#endif

    if ((return_val < 0) &&
	(return_val != err_info[ERR_INVALID_HANDLE].err_code))
    {
	tape_io_exit (handle);
    }
    return (return_val);
}
