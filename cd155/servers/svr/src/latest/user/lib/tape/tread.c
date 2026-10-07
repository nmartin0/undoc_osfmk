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
 * tread.c 11.1 94/03/22 16:49:51 
 */
static char     tread_ver[] = "@(#) sourcefile tread.c 11.1 94/03/22 16:49:51";

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
 * tread.c 
 *
 * INPUT:	handle: handle from the TALLOCTAPE 
 *		buffer: pointer where the data is to be placed 
 *		length: length of the user buffer 
 *
 * OUTPUT:  positive number: number of bytes moved to buffer. 
 * -or- 
 * zero:   indicating EOF 
 * -or- 
 * negative number: indicating an error 
 *
 * Description: 
 *
 * This routine reads tape blocks from the 3480 tape device and either passes
 * the entire block or deblocks the records from the block depending on the
 * format of the records written within the tape block and the parameters in
 * the data set control block (DCB). 
 *
 * Tape block buffering can be configured.  The number of buffers to use for
 * buffering will be set in the DCB structure. 
 *
 */

#include <stdio.h>
#include "type.h"
#include "convt.h"
#include "cnvproto.h"
#include "tapeio.h"
#include "tape3480.h"
#include "tioerr.h"
#include "tioproto.h"

#define SPIDER	1

extern TAPEMAIN tapemain;

int
tread (handle, buffer, length)
	int handle; 
	char *buffer; 
	int length;
{

    int             return_val = 0, sdw_state = SDW_IDLE, sdw_reclen = 0, temp;
    bit16           block_len = 0, record_len = 0;
    char           *rec_p, *sdw_buf_p = buffer;
    LVCB           *lvcb_p;
    TAPE_IO        *tape_io_p;
    int             i = 0;


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
	fprintf (tape_io_p->logdev, " Handle: %d  Buffer: %08X  Length: %d\n",
		 handle, buffer, length);
	fprintf (tape_io_p->logdev,
		 " LVCB ptr: %08X TAPE_IO ptr: %08X\n",
		 lvcb_p, tape_io_p);
    }
#endif

    do
    {
	/*--------------------------------------------------
        *  If unbuffered mode is selected then for every
        *  invocation of the tread command 1 tape block will
        *  be passed to the user buffer area.  The conditions
        *  for unbuffered operation are:
        *      1) fixed block 1 record per block
        *      2) undefined length record  (block mode also)
        *  These conditions are checked in the tape_io_init
        *  routine before the user can issue a tread command.
        --------------------------------------------------*/

	if (!lvcb_p->read || (tape_io_p->dmem_p == NULL))
	{
	    tio_log_init (lvcb_p);
	    ERR_PRN (err_info[ERR_NOT_OPEN_FOR_READ].err_str);
	    return_val = err_info[ERR_NOT_OPEN_FOR_READ].err_code;
	    break;
	}
	if (tape_io_p->eof)
	{
	    ERR_PRN (err_info[ERR_ATTEMPT_RD_PAST_EOF].err_str);
	    return_val = err_info[ERR_ATTEMPT_RD_PAST_EOF].err_code;
	    break;
	}
#if SPIDER
	if (spider & SPIDER_TREAD)
	{
	    PRN_LOC (" ");
	    fprintf (tape_io_p->logdev, "%sEOD\n",
		     (tape_io_p->eod) ? "" : "NOT at ");
	}
#endif
	if (!tape_io_p->eod)
	{

#if SPIDER
	    if (spider & SPIDER_TREAD)
	    {
		PRN_LOC (" ");
		fprintf (tape_io_p->logdev, "%sBUFFERED MODE\n",
			 (lvcb_p->buffered_mode) ? "" : "UN");
	    }
#endif

	    /*--------------------------------------------------
             * UNBUFFERED MODE
             --------------------------------------------------*/

	    if (!lvcb_p->buffered_mode)
	    {
		switch (tape_io_p->record_format & REC_FORMAT)
		{
		case FIXED:
		    record_len = (bit16) lvcb_p->rec2.blocklen;
		    if ((bit16) length < record_len)
		    {
			ERR_PRN (err_info[ERR_INVALID_RD_BUF_LEN].err_str);
			fprintf (tape_io_p->logdev, 
				" -> blocklen = %05d length = %05d\n",
				lvcb_p->rec2.blocklen, length);
			return_val = err_info[ERR_INVALID_RD_BUF_LEN].err_code;
		    }
printf("FIXED: record_len=%d\n",record_len);
		    break;
		case UNDEFINED:
		    record_len = (bit16) length;
		    break;
printf("UNDEFINED: record_len=%d\n",record_len);
		default:
		    ERR_PRN (err_info[ERR_INVALID_PARAM].err_str);
		    fprintf (tape_io_p->logdev, 
				" -> blocklen = %05d length = %05d\n",
			     	lvcb_p->rec2.blocklen, length);
		    return_val = err_info[ERR_INVALID_PARAM].err_code;
		}	/* end switch REC_FORMAT */
		if (return_val < 0)
		{
		    break;
		}
#if SPIDER
		if (spider & SPIDER_TREAD)
		{
		    PRN_LOC (" Call issue_iread (1) \n");
		}
#endif
		if ((return_val = issue_iread (lvcb_p->fildes, tape_io_p,
					       buffer, record_len)) < 0)
		{
		    break;
		}
	    }	/* endif UNBUFFERED MODE */
#if SPIDER
	    if (spider & SPIDER_TREAD)
	    {
		PRN_LOC (" Call check_iread\n");
	    }
#endif

	    if ((return_val = check_iread (lvcb_p, tape_io_p)) < 0)
		break;

	}

	do
	{
	    if (tape_io_p->blk_proc_p == NULL)
	    {	/* New Block */

		tape_io_p->block_id = 0;

		/*--------------------------------------------------
			  * New block, check for end-of-data (eod)
                 --------------------------------------------------*/
#if SPIDER
		if (spider & SPIDER_TREAD)
		{
		    PRN_LOC (" New Tape Block Buffer -");
		    fprintf (tape_io_p->logdev, " Tail: %08X Read Len: %d %s\n",
			     tape_io_p->tail,
			     tape_io_p->tail->length,
			     (tape_io_p->eod) ? "at EOD" : "");
		}
#endif

		if (tape_io_p->eod &&
		    (tape_io_p->head == tape_io_p->tail) &&
		    (tape_io_p->head->status == EMPTY))
		{

#if SPIDER
		    if (spider & SPIDER_TREAD)
		    {
			PRN_LOC (" Sync at TM - Call eod\n");
		    }
#endif
		    tape_io_p->eof = TRUE;
		    record_len = tape_io_p->head->length;
		    return_val = eod (handle);
#if SPIDER
		    if ((spider == 0) || (spider & SPIDER_TREAD))
		    {
			PRN_LOC (" eod() return: ");
			fprintf (tape_io_p->logdev, "%d\n", return_val);
		    }
#endif
		    break;	/* while(0): check for EOV - new volume */
		}
		/*--------------------------------------------------
                 *  For unbuffered mode: transfer is complete so
                 *  return back to the user.
                 --------------------------------------------------*/

		if (!lvcb_p->buffered_mode)
		{
		    record_len = tape_io_p->head->length;
#if SPIDER
		    if (spider & SPIDER_TREAD)
		    {
			PRN_LOC (" UNBUFFERED READ Done\n");
		    }
#endif
		    break;	/* while(0): Unbuffered mode */
		}
		/*--------------------------------------------------
                 *  For buffered mode: Process the current read
                 *  buffer (tail pointer)
                 --------------------------------------------------*/

		rec_p = (char *) &((BLK_BUF *) tape_io_p->tail)->data;

		switch (tape_io_p->record_format & REC_FORMAT)
		{
		case VARIABLE:
		    block_len = Itoi_16 (*(bit16 *) rec_p);	/* BDW */
		    if ((block_len > lvcb_p->rec2.blocklen) ||
			(block_len != (bit16) tape_io_p->tail->length))
		    {
			ERR_PRN (err_info[ERR_BWD_LENGTH].err_str);
			fprintf (tape_io_p->logdev,
				 "\n>> (HDR2)blocklen = %d BDW len = %d\
                                                        Tapeblk size = %d\n",
				 lvcb_p->rec2.blocklen,
				 block_len,
				 tape_io_p->tail->length);
			return_val = err_info[ERR_BWD_LENGTH].err_code;
			break;
		    }
#if SPIDER
		    if (spider & SPIDER_TREAD)
		    {
			PRN_LOC (" ");
			fprintf (tape_io_p->logdev,
				 "VARIABLE BDW length: %d\n", block_len);
		    }
#endif
		    block_len -= sizeof (BDW);
		    rec_p += sizeof (BDW);
		    break;
		case FIXED:

		    block_len = (bit16) tape_io_p->tail->length;
		    if (block_len % lvcb_p->rec2.reclen)
		    {
			ERR_PRN (err_info[ERR_FIX_BLK_LEN].err_str);
			fprintf (tape_io_p->logdev,
				 " rec2.reclen = %d  tape blklen = %d\n",
				 lvcb_p->rec2.reclen, block_len);
			return_val = err_info[ERR_FIX_BLK_LEN].err_code;
			break;

		    }
		    switch (tape_io_p->record_format & BLK_ATTR)
		    {

		    case BLOCKED_RECORDS:
			break;

		    case STANDARD_REC:
			if (tape_io_p->trunc_blk)
			{
			    ERR_PRN (err_info[ERR_TRUNC_BLK].err_str);
			    fprintf (tape_io_p->logdev,
				  " rec2.blocklen = %d  tape blklen = %d\n",
				     lvcb_p->rec2.blocklen, block_len);
			    return_val = err_info[ERR_TRUNC_BLK].err_code;
			    break;

			}
			if (block_len < lvcb_p->rec2.blocklen)
			{
			    tape_io_p->trunc_blk = TRUE;
			}
			break;

		    case UNBLOCKED_RECORDS:
			if (block_len != lvcb_p->rec2.blocklen)
			{
			    ERR_PRN (err_info[ERR_FIX_BLK_LEN].err_str);
			    fprintf (tape_io_p->logdev,
				  " rec2.blocklen = %d  tape blklen = %d\n",
				     lvcb_p->rec2.blocklen, block_len);
			    return_val = err_info[ERR_FIX_BLK_LEN].err_code;
			}
			break;

		    }
		    if (return_val < 0)
		    {
			break;
		    }
#if SPIDER
		    if (spider & SPIDER_TREAD)
		    {
			PRN_LOC (" ");
			fprintf (tape_io_p->logdev,
				 "FIXED block length: %d\n", block_len);
		    }
#endif
		    break;
		case UNDEFINED:
		    printf ("undefined\n");
		    if ((block_len = (bit16) tape_io_p->tail->length) >
			lvcb_p->rec2.blocklen)
		    {
			ERR_PRN (err_info[ERR_FIX_BLK_LEN].err_str);
			fprintf (tape_io_p->logdev,
				 " rec2.blocklen = %d  tape blklen = %d\n",
				 lvcb_p->rec2.blocklen, block_len);

			return_val = err_info[ERR_FIX_BLK_LEN].err_code;
		    }
#if SPIDER
		    else
		    if (spider & SPIDER_TREAD)
		    {
			PRN_LOC (" ");
			fprintf (tape_io_p->logdev,
				 "UNDEFINED block length: %d\n",
				 block_len);
		    }
#endif
		    break;

		default:
		    ERR_PRN (err_info[ERR_BAD_REC_FORMAT].err_str);
		    fprintf (tape_io_p->logdev, " -> %d\n",
			     tape_io_p->record_format & REC_FORMAT);
		    return_val = err_info[ERR_BAD_REC_FORMAT].err_code;
		}	/* end switch REC_FORMAT */
		if (return_val < 0)
		    break;
	    } else	/* not a new block, processing current block */
	    {

		/*--------------------------------------------------
                 * still processing current block,
                 * restore saved values.
                 --------------------------------------------------*/

		rec_p = tape_io_p->blk_proc_p;
		block_len = tape_io_p->blk_len;
#if SPIDER
		if (spider & SPIDER_TREAD)
		{
		    PRN_LOC (" Still processing Block - ");
		    fprintf (tape_io_p->logdev,
			     "Tail: %08X rec_p: %08X block_len: %d\n",
			     tape_io_p->tail, rec_p, block_len);
		}
#endif
	    }	/* end of processing current block */

	    /*--------------------------------------------------
             *  Deblock record from the tape block buffer
             --------------------------------------------------*/

	    switch (tape_io_p->record_format & REC_FORMAT)
	    {
	    case VARIABLE:
		record_len = Itoi_16 (*(bit16 *) rec_p);	/* RDW */

		if ((record_len < (sizeof (RDW) + 1)) ||
		    (record_len > block_len) ||
		    (record_len > (bit16) lvcb_p->rec2.reclen))
		{
		    return_val = err_info[ERR_RDW_LENGTH].err_code;
		}
		if ((tape_io_p->record_format & BLOCKED_RECORDS) ==
		    UNBLOCKED_RECORDS)
		{
		    if (record_len != block_len)
		    {
			return_val = err_info[ERR_RDW_LENGTH].err_code;
		    }
		}
		if (return_val < 0)
		{
		    ERR_PRN (err_info[ERR_RDW_LENGTH].err_str);
		    fprintf (tape_io_p->logdev,
			" block_len = %d  rec2.reclen = %d  RDW len = %d\n",
			     block_len, lvcb_p->rec2.reclen, record_len);
		    break;
		}
		record_len -= sizeof (RDW);
		block_len -= sizeof (RDW);

		if (tape_io_p->record_format & SPANNED_REC)
		{
		    temp = *(rec_p + 2) & 0xFF;
		    switch (sdw_state)
		    {
		    case SDW_IDLE:
			if ((temp != SDW_IDLE) && (temp != SDW_START))
			{
			    return_val = err_info[ERR_INVALID_SDW_STATE].err_code;
			}
			break;
		    case SDW_START:
		    case SDW_MID:
			if ((temp != SDW_MID) && (temp != SDW_END))
			{
			    return_val = err_info[ERR_INVALID_SDW_STATE].err_code;
			}
			buffer = sdw_buf_p;
			break;
		    default:
			break;
		    }
		    if (return_val < 0)
		    {
			ERR_PRN (err_info[ERR_INVALID_SDW_STATE].err_str);
			fprintf (tape_io_p->logdev,
				 ">> Current State = %d  SDW state = %d\n",
				 sdw_state, temp);
			break;
		    }
		    sdw_state = temp;
		    sdw_reclen += record_len;
		}		/* end if SPANNED_REC */
		rec_p += sizeof (RDW);

#if SPIDER
		if (spider & SPIDER_TREAD)
		{
		    PRN_LOC (" ");
		    fprintf (tape_io_p->logdev,
		    "VARIABLE RDW  reclen: %d\n", record_len + sizeof (RDW));
		}
#endif

		break;
	    case FIXED:
		record_len = (bit16) lvcb_p->rec2.reclen;
#if SPIDER
		if (spider & SPIDER_TREAD)
		{
		    PRN_LOC (" ");
		    fprintf (tape_io_p->logdev,
			     "FIXED record length: %d\n", record_len);
		}
#endif
		break;
	    case UNDEFINED:
		record_len = block_len;
#if SPIDER
		if (spider & SPIDER_TREAD)
		{
		    PRN_LOC (" ");
		    fprintf (tape_io_p->logdev,
			     "UNDEFINED record_length: %d\n", record_len);
		}
#endif
		break;
	    default:
		break;
	    }/* end switch record format, Deblock record from the tape block buffer */

	    if (return_val < 0)
		break;

	    if (record_len > (bit16) length)
	    {
printf("tread: ERR_INVALID_RD_BUF_LEN\n");
		ERR_PRN (err_info[ERR_INVALID_RD_BUF_LEN].err_str);
		fprintf (tape_io_p->logdev,
			 " -> record_len = %d  user buf len = %d\n",
			 record_len, length);
		return_val = err_info[ERR_INVALID_RD_BUF_LEN].err_code;
		break;
	    }
#if SPIDER
	    if (spider & SPIDER_TREAD)
	    {
		PRN_LOC (" Copy Tape Block Data to User Memory\n");
		fprintf (tape_io_p->logdev,
			 "User Buf: %08X  rec_p: %08X  length: %d\n",
			 buffer, rec_p, record_len);
	    }
#endif
	    memcpy (buffer, rec_p, record_len);

	} while (0);

	if (return_val < 0)		/* ERROR has occurred */
	    break;

	/*--------------------------------------------------
         * eod   eof
         *
         *  0     0  - On BOT side of TM and EOM (in dataset)
         *              do nothing pass thru
         *  1     0  - Read into a TM have not called eod()
         *              do nothing pass thru
         *  1     1  - On EOT side of TM, eod() returned eof
         *              break to return (0)
         *  0     1  - Volume switch occurred
         *              continue at while (1), start over
         *              at top of tread
         --------------------------------------------------*/

	if (tape_io_p->eof)
	{

	    if (return_val == 0)
	    {
#if SPIDER
		if ((spider == 0) || (spider & SPIDER_TREAD))
		{
		    PRN_LOC (" EOF indicated\n");
		}
#endif
		break;
	    } else
	    {			/* NEW_VOL (1) */
#if SPIDER
		if (spider & SPIDER_TREAD)
		{
		    PRN_LOC (" NEW VOLUME Mounted\n");
		}
		if ((spider == 0) || (spider & SPIDER_TREAD))
		{

		    PRN_LOC (" EOV: Volume Switch\n>> ");
		    prn_tiostats (lvcb_p, tape_io_p);

		}
#endif
		/*--------------------------------------------------
                 * The label processing code calls this routine
                 * after processing the IBM standard labels with
                 * the last operation being a read of the TM, therefore
                 * the FEOF flag must be cleared in case another tape
                 * mark follows this TM (Empty Data Set).  If two
                 * tape marks are read in a row the CFS_READ code
                 * disables the read function and always returns 0.
                 --------------------------------------------------*/
#if CLREOF
		cleareof (lvcb_p->fildes);
#endif
		tape_io_p->eod = tape_io_p->eof = FALSE;
		tape_io_p->phy_blk_num = 0;
		continue;

	    }	/* end else NEW_VOL (1) */
	}	/* end if tape_io_p->eof */

	/*--------------------------------------------------
         *  If buffered mode, update and save block length
         *  and record pointer for next invocation,
         *  deblocking process.
         --------------------------------------------------*/
	if ((lvcb_p->buffered_mode) &&
	    (tape_io_p->blk_len = (block_len -record_len)))
	{

	    tape_io_p->blk_proc_p = (rec_p + record_len);
#if SPIDER
	    if (spider & SPIDER_TREAD)
	    {
		PRN_LOC (" Still DATA in Tape Block buffer\n");
		fprintf (tape_io_p->logdev,
			 "blk_proc_p: %08X  blk_len: %d\n",
			 tape_io_p->blk_proc_p, tape_io_p->blk_len);
	    }
#endif
	} else
	{			/* last record processed switch buffers */
	    tape_io_p->blk_proc_p = NULL;
	    tape_io_p->tail->status = EMPTY;
	    tape_io_p->tail->length = 0;
	    tape_io_p->tail = tape_io_p->tail->next;
	    ++lvcb_p->blockcnt;

#if SPIDER
	    if (spider & SPIDER_TREAD)
	    {
		PRN_LOC (" Tape Block buffer EMPTY - Switch Buffers\n");
		fprintf (tape_io_p->logdev,
			 "Blockcount: %d  New Tail ptr: %08X\n\n",
			 lvcb_p->blockcnt, tape_io_p->tail);
	    }
#endif
	    /*--------------------------------------------------
            *
            * If buffered mode and there is only 1 tape block
            * buffer then issue the next iread
            *
            --------------------------------------------------*/

	    if (lvcb_p->buffered_mode && (lvcb_p->num_tapeblk_bufs == 1))
	    {
#if SPIDER
		if (spider & SPIDER_TREAD)
		{
		    PRN_LOC (" Call issue_iread (2) \n");
		}
#endif
		if ((return_val = issue_iread (lvcb_p->fildes, tape_io_p,
			      (char *) &((BLK_BUF *) tape_io_p->head)->data,
					       lvcb_p->rec2.blocklen)) < 0)
		    break;
	    }
	}
	if (tape_io_p->record_format & SPANNED_REC)
	{
	    if ((sdw_state == SDW_START) || (sdw_state == SDW_MID))
	    {
		sdw_buf_p += record_len;
		continue;
	    }
	}
	break;

    } while (1);		/* Allow restart of function on Volume Switch
				 * (EOV) */


    if (return_val >= 0)
    {
	++tape_io_p->num_records;
	return_val = (tape_io_p->record_format & SPANNED_REC) ?
	    sdw_reclen : record_len;
    }
#if SPIDER
    if (spider & SPIDER_ENTER_EXIT)
    {
	PRN_LOC (" EXIT:");
	fprintf (tape_io_p->logdev, " return value: %d\n\n", return_val);
    }
    fflush (tape_io_p->logdev);

#endif
    return (return_val);
}


/*
 * issue_iread() 
 *
 * INPUT:   
 * fildes:    file descriptor 
 * tape_io_p: pointer the TAPE_IO structure
 * buffer:    pointer the data for the iread 
 * length:    length of the data buffer 
 *
 * OUTPUT:  zero:   iread issued O.K. 
 *
 * -or- 
 *
 * negative number: indicating an error 
 *
 * Description: 
 *
 * This routine issues the _iread command and checks to ensure that the _iread
 * was initiated O.K. 
 *
 */

int
issue_iread (fildes, tape_io_p, buffer, length)
	int	fildes; 
	TAPE_IO	*tape_io_p; 
	char	*buffer; 
	int	length;
{
    int             return_val = 0;

#if SPIDER
    int             spider = tapeio_glob.spider;
#endif

    do
    {

	if ((tape_io_p->head->length = read (fildes, buffer, length)) < 0)
	{
	    ERR_PRN (err_info[ERR_IREAD].err_str);
	    fprintf (tape_io_p->logdev, " errno: %d\n", errno);
	    return_val = err_info[ERR_IREAD].err_code;
	    break;
	}
#if SPIDER

	if (spider & SPIDER_TREAD)
	{
	    PRN_LOC (" IREAD: ");
	    fprintf (tape_io_p->logdev,
		     "fd: %d Head: %08X Buf_ptr: %08X Len: %d\n",
		     fildes,
		     tape_io_p->head,
		     buffer,
		     length);
	    fprintf (tape_io_p->logdev, "Bytes read: %08X\n\n",
		     tape_io_p->head->length);

	}
#endif

	++tape_io_p->num_ireads;

    } while (0);
    return (return_val);
}
