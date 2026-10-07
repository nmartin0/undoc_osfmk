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
 * chk_ird.c 11.1 94/03/22 16:49:12 
 */
static char     chk_ird_ver[] = "@(#) sourcefile chk_ird.c 11.1 94/03/22 16:49:12";

/* 
******************  ENGINEERING CHANGE HISTORY *********************** 
 *
 * Date      Engineer                  Description 
 *
 * 2/25/92     G. Haycox       Module creation release to BETA 
 *
 *
 */

/*--------------------------------------------------
 * chk_ird.c       check_iread()
 *
 * INPUT:   lvcb_p:     pointer to the LVCB
 *          tape_io_p:  pointer to the main tape control structure
 *
 *
 * OUTPUT:      +X: Number of bytes read
 *               0: Read into a Tape Mark or IREAD still busy
 *                      (tape_io_p->eod is set if TM).
 *              -X: ERROR
 *
 *
 * Description:
 *
 *      This routine ensures that iread commands remain outstanding.
 *      When a TM is encountered then the 'end of data' bit is set and
 *      no further iread commands will be issued.
 *
 *      If the condition where an iread is in progress and all of the
 *      tape block buffers are empty then this routine will block until
 *      the iread completes.
 *
 --------------------------------------------------*/

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <pid.h>
#include <cfs.h>
#include <iod.h>
#include <fio.h>
#include "type.h"
#include "tapeio.h"
#include "tape3480.h"
#include "tioerr.h"
#include "tioproto.h"

int 
check_iread (LVCB * lvcb_p, TAPE_IO * tape_io_p)
{

    int             return_val = 0, rc;

#if SPIDER
    int             spider = tapeio_glob.spider;

    if (spider & SPIDER_ENTER_EXIT)
    {
	fprintf (tape_io_p->logdev, "\n");
	PRN_LOC (" ENTRY:");
	fprintf (tape_io_p->logdev,
		 " LVCB ptr: %08X TAPE_IO ptr: %08X\n",
		 lvcb_p, tape_io_p);
    }
#endif
	if ( tape_io_p->head->length )
		tape_io_p->head->status = FULL;
    do
    {

#if SPIDER
	if (spider & SPIDER_CHK_IRD)
	{
	    PRN_LOC (" ");
	    fprintf (tape_io_p->logdev,
		     "Head: %08X  Tail: %08X  Head->status: %s\n",
		     tape_io_p->head, tape_io_p->tail,
		     (tape_io_p->head->status == EMPTY) ? "EMPTY" : "FULL");
	}
#endif

	if (tape_io_p->head->status == EMPTY)
	{
	    /*--------------------------------------------------
            *
            *  If the current buffer is EMPTY
            *  then issue the iread command.
            *  (First read never issued.)
            *
            --------------------------------------------------*/
#if SYNC
	    if (tape_io_p->head->mid == CMD_COMPLETE)
	    {
		printf("check_iread: mid = CMD_COMPLETE\n");
#if SPIDER
		if (spider & SPIDER_CHK_IRD)
		{
		    PRN_LOC (" Call issue_iread (3)\n");
		}
#endif
		if ((return_val = issue_iread (lvcb_p->fildes, tape_io_p,
			      (char *) &((BLK_BUF *) tape_io_p->head)->data,
					       lvcb_p->rec2.blocklen)) < 0)
		    break;
	    }
#endif
	    /*--------------------------------------------------
            *
            * Check to see if this is a read outstanding with all
            * of the read buffers in a EMPTY state.  If it is,
            * then wait for the read to complete (BLOCKED).
            *
            --------------------------------------------------*/
#if SPIDER
	    if (spider & SPIDER_CHK_IRD)
	    {
		PRN_LOC ("");
		fprintf (tape_io_p->logdev, " Bytes read: %08X\n",
			 tape_io_p->head->length);
	    }
#endif

#if SYNC
	    if (tape_io_p->head == tape_io_p->tail)
	    {
		printf("check_iread: head==tail : NOP, was oper_blked\n ");
#if SPIDER
		if (spider & SPIDER_CHK_IRD)
		{
		    PRN_LOC (" BLOCKED, wait for IREAD to finish\n");
		}
#endif
		++tape_io_p->oper_blked;
		/* using read instead of iread means no wait */
		 return_val = tiowait(tape_io_p->head->mid); 
	    }

	    /*--------------------------------------------------
            * Or, check to see if the iread is done for another
            * buffer, if not just leave
            --------------------------------------------------*/
	    else
	    {
		printf("check_iread: head!=tail : NOP, was oper_busy\n ");
		/* using read instead of iread means we do not need tiodone */
		 if((return_val = tiodone(tape_io_p->head->mid)) == 0)  
#if SPIDER
		if (spider & SPIDER_CHK_IRD)
		{
		    PRN_LOC (" IREAD still BUSY\n");
		}
#endif
		++tape_io_p->oper_busy;
		break;
	    }
#endif

	    /*--------------------------------------------------
             * IREAD DONE: Check the status of the IREAD
             --------------------------------------------------*/

	    if ((return_val = ird_status (lvcb_p, tape_io_p, return_val)) <= 0)
		{
		break;		/* Hit a TM or ERROR */
		}

	}
	/*--------------------------------------------------
        * If the next buffer status is EMPTY
        * then position the head pointer to the next
        * buffer and issue the next iread.
        --------------------------------------------------*/
#if SYNC 
	/* not using more than one buffer for the unix read/write */
	if ((tape_io_p->head->next->mid == CMD_COMPLETE) &&
	    (tape_io_p->head->next->status == EMPTY))
	{

#if SPIDER
	    if (spider & SPIDER_CHK_IRD)
	    {
		PRN_LOC (" NEXT Buffer Available - Call issue_iread (4)\n");
	    }
#endif
	    tape_io_p->head = tape_io_p->head->next;
	    if ((rc = issue_iread (lvcb_p->fildes, tape_io_p,
			      (char *) &((BLK_BUF *) tape_io_p->head)->data,
				   lvcb_p->rec2.blocklen)) < 0)
	    {

		return_val = rc;
		break;
	    }
	}
#if SPIDER
	else
	{
	    if (spider & SPIDER_CHK_IRD)
	    {
		PRN_LOC (" NEXT Buffer NOT Available\n");
	    }
	}
#endif
#endif /* not using more than one buffer for the standard unix read / write */
    } while (0);

#if SPIDER
    if (spider & SPIDER_ENTER_EXIT)
    {
	PRN_LOC (" EXIT:");
	fprintf (tape_io_p->logdev, " return value: %d\n\n", return_val);
    }
#endif

    return (return_val);
}


/*--------------------------------------------------
 * ird_status()
 *
 * INPUT:   lvcb_p:     pointer to the LVCB
 *          tape_io_p:  pointer to the main tape control structure
 *
 *
 * OUTPUT:      +X: Number of bytes read
 *               0: Read into a Tape Mark
 *              -X: ERROR
 *
 * Description:
 *
 *      The status of the IREAD that just completed is returned.
 *
 *
 --------------------------------------------------*/
int 
ird_status (LVCB * lvcb_p, TAPE_IO * tape_io_p, int iread_rv)
{

    int	return_val;
    int	read_ret = tape_io_p->head->length;
#if SPIDER
    int             spider = tapeio_glob.spider;
#endif

    /* check for a TM */

    tape_io_p->head->mid = CMD_COMPLETE;

    if ( read_ret == 0 )/* last read returned 0, means read end of file */
    {

#if SPIDER
	if (spider & SPIDER_CHK_IRD)
	{
	    PRN_LOC (" IREAD COMPLETE: TM Encountered\n");
	}
#endif

	/*--------------------------------------------------
         * TapeMark encountered:
         * 
         * Clear the FEOF flag in the (fid) to ensure that
         * if another tape mark is read next (2 consequtive
         * tape marks) that the cfs_read
         * logic does not set the FEOD flag and disable
         * the read logic (zero byte read without the FEOF
         * flag will be indicated).
         *
         * The read-a-head buffering may encounter a tapemark
         * before the user actually issues a TREAD that would
         * hit the tapemark.  If the tape mark is encountered
         * (during the read-a-head buffering) and the user
         * issues a tget_blk_id or tlocate_blk command then
         * the FEOF must be cleared because the tape will be
         * physically re-positioned to who knows where and the
         * next read may be into another tapemark which would
         * cause the condition listed above.
         --------------------------------------------------*/
#if CLREOF
	cleareof (lvcb_p->fildes);
#endif
	tape_io_p->eod = TRUE;
	tape_io_p->head->status = EMPTY;
	tape_io_p->head->length = 0;
	return_val = 0;
    } else
    {

	if (iread_rv < 0)
	{
	    tape_io_p->head->length = 0;
	    ERR_PRN (err_info[ERR_IREAD].err_str);
	    fprintf (tape_io_p->logdev, " errno = %d\n", errno);
	    return_val = err_info[ERR_IREAD].err_code;

	} else
	if ( tape_io_p->head->length )
	{

#if SPIDER
	    if (spider & SPIDER_CHK_IRD)
	    {
		PRN_LOC (" IREAD COMPLETE:");
		fprintf (tape_io_p->logdev,
			 " %d bytes read\n", tape_io_p->head->length);
	    }
#endif

	    ++tape_io_p->phy_blk_num;
	    tape_io_p->head->status = FULL;

#if SPIDER
	    if (spider & SPIDER_CHK_IRD)
	    {
		PRN_LOC (" ");
		fprintf (tape_io_p->logdev, "physical block cnt: %d\n",
			 tape_io_p->phy_blk_num);
	    }
#endif
	    return_val = tape_io_p->head->length;
	} else
	{			/* ZERO bytes read */

	    ERR_PRN (err_info[ERR_IREAD].err_str);
	    fprintf (tape_io_p->logdev, " 0 length read\n");
	    return_val = err_info[ERR_IREAD].err_code;

	}
    }
    return (return_val);
}
