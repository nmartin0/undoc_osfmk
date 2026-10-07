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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	sync_tio.c 11.1 94/03/22 16:49:35
 */
static char	sync_tio_ver[] = "@(#) sourcefile sync_tio.c 11.1 94/03/22 16:49:35";

/*
 *
 *  ******************  ENGINEERING CHANGE HISTORY ***********************
 *
 *    Date      Engineer                  Description
 *
 *  2/25/92     G. Haycox       Module creation release to BETA
 *
 *
 */

/* PAGE  */

/* sync_tio()
 *
 * INPUT:   lvcb_p:     pointer to the LVCB
 *          tape_io_p:  pointer to the TAPE_IO structure
 *
 * OUTPUT:  int:        0 if O.K.
 *                     -X if ERROR
 *
 * Description:
 *
 *      TREAD:  Allow the IREAD command to complete if one is
 *              outstanding.
 *
 *      TWRITE: Allow all outstanding IWRITE commands to complete,
 *              set the eod bit if EOM is encountered, verify the
 *              EOM condition persists if the eod bit is set.  Ensure
 *              all of the IWRITES complete without error.
 *
 * */

#include <stdio.h>
#include "type.h"
#include "tapeio.h"
#include "tape3480.h"
#include "tioerr.h"
#include "tioproto.h"

#if SYNC
int sync_tio(LVCB *lvcb_p, TAPE_IO *tape_io_p) {

    int return_val = 0;


#if SPIDER
    int spider = tapeio_glob.spider;

    if (spider & SPIDER_ENTER_EXIT) {
        fprintf(tape_io_p->logdev, "\n");
        PRN_LOC(" ENTRY: SYNC tape I/O Buffers\n");
        fprintf(tape_io_p->logdev, ">> %s - LVCB ptr: %08X \n",
                (lvcb_p->read) ? "READ" : "WRITE", lvcb_p);

    }
#endif

    if (lvcb_p->read) {

        /*--------------------------------------------------
         * Allow the IREAD to complete if it is outstanding
         --------------------------------------------------*/

        if (tape_io_p->head->mid) {

#if SPIDER
            if (spider & SPIDER_SYNC_TIO) {
                PRN_LOC("");
                fprintf(tape_io_p->logdev,
                    " MID: %08X BLOCKED, wait for IREAD to finish\n",
                    tape_io_p->head->mid);
            }
#endif
            /* the following "if" is to make sure the number of "tiowait"
                issued is the same as the number of "iread".  Otherwise, if
                the number of "tiowait" is more than "iread", the "tiodone" may
                access garbage location which will cause segmentation fault.
                see PTS 2646 */
            if (tape_io_p->oper_blked < tape_io_p->num_ireads) {

                ++tape_io_p->oper_blked;

            /*--------------------------------------------------
             * Just let the iread command complete, don't care
             * if there was an error, therefore pass a 0 to
             * the ird_status routine to indicate no error.
             --------------------------------------------------*/

		/*
                tiowait(tape_io_p->head->mid);
		*/
                ird_status(lvcb_p, tape_io_p, 0);
	    }

        }
    }

    else {  /* write */

        /*--------------------------------------------------
         * Allow all outstanding iwrites to complete
         --------------------------------------------------*/

        while (tape_io_p->tail->status == FULL) {

#if SPIDER
            if (spider & SPIDER_SYNC_TIO) {
                PRN_LOC("");
                fprintf(tape_io_p->logdev,
                    " MID: %08X BLOCKED, wait for IWRITE to finish\n",
                            tape_io_p->tail->mid    );

            }
#endif
		/*
            return_val = tiowait(tape_io_p->tail->mid);
	    */
            ++tape_io_p->oper_blked;
            tape_io_p->tail->status = EMPTY;
            tape_io_p->tail->mid = CMD_COMPLETE;

            if (tape_io_p->eod) {
                if (return_val == 0)
                    return_val = err_info[ERR_EXPECT_EOM].err_code;

                else if (return_val < 0) {
                    if (!tiseom(lvcb_p->fildes))
                        return_val = err_info[ERR_EXPECT_EOM].err_code;
                    else
                        return_val = 0;
                }
                if (return_val) {
                    ERR_PRN(err_info[ERR_EXPECT_EOM].err_str);
                    fprintf(tape_io_p->logdev, " (%d)\n", errno);
                    break;
                }
            }

            else if (return_val < 0) {
                if (tiseom(lvcb_p->fildes)) {
                    tape_io_p->eod = TRUE;
#if SPIDER
                    if (spider & SPIDER_SYNC_TIO) {
                        PRN_LOC(" EOM Encountered\n");
                    }
#endif
                }
                else {
                    ERR_PRN(err_info[ERR_TIODONE].err_str);
                    fprintf(tape_io_p->logdev, " (%d)\n", errno);
                    return_val = err_info[ERR_TIODONE].err_code;
                    break;
                }
            }
            ++tape_io_p->phy_blk_num;
#if SPIDER
            if (spider & SPIDER_SYNC_TIO) {
                PRN_LOC("");
                fprintf(tape_io_p->logdev, " IWRITE DONE  phy_blk_num: %d\n",
                            tape_io_p->phy_blk_num);

            }
#endif
            return_val = 0;
            tape_io_p->tail = tape_io_p->tail->next;
        }
    }


#if SPIDER
    if (spider & SPIDER_ENTER_EXIT) {
        PRN_LOC(" EXIT:");
        fprintf(tape_io_p->logdev, " return_val = %d\n\n", return_val);
    }
    fflush(tape_io_p->logdev);

#endif
    return (return_val);
}
#endif
