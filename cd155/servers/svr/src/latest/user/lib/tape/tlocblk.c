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
 *	tlocblk.c 11.1 94/03/22 16:49:48
 */
static char	tlocblk_ver[] = "@(#) sourcefile tlocblk.c 11.1 94/03/22 16:49:48";

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

/* tlocblk.c
 *
 * INPUT:   handle:     handle from the TALLOCTAPE
 *          block_id:   Block ID to position to.
 *
 * OUTPUT:               0 : Locate to block successful.
 *
 *          negative number: Indicates an error
 *
 * Description:
 *
 *     This routine will positon the tape to the BLOCK ID
 *     specified.
 *
 *     This command is only allowed in READ mode,  an ERROR will
 *     be indicated if this command is issued in WRITE mode.
 *
 * */

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <msgtype.h>
#include "type.h"
#include "tapeio.h"
#include "tape3480.h"
#include "tioerr.h"
#include "tioproto.h"

extern TAPEMAIN tapemain;

int tlocate_blk(int handle, int block_id) {

    int      return_val = 0;
    LVCB    *lvcb_p;
	TAPE_IO *tape_io_p;


#if SPIDER

    int  spider = tapeio_glob.spider;

#endif

    if ((lvcb_p = tapemain.lvcblist[handle]) == NULL) {

            fprintf(stderr, "\n%s #%04d ERROR:  %s", __FILE__, __LINE__,
                        err_info[ERR_INVALID_HANDLE].err_str);
            fprintf(stderr, " (%d)\n", handle);
            fflush(stderr);

        return(err_info[ERR_INVALID_HANDLE].err_code);
    }

    tape_io_p = (TAPE_IO *)&(lvcb_p->tape_io);

#if SPIDER
    if (spider & SPIDER_ENTER_EXIT) {
        fprintf(tape_io_p->logdev, "\n");
        PRN_LOC(" ENTRY:");
        fprintf(tape_io_p->logdev, " Handle: %d  BLKID: %08X\n",
                    handle, block_id);
    }
#endif

    do {

        if (!lvcb_p->read || (tape_io_p->dmem_p == NULL)) {

            tio_log_init(lvcb_p);
            ERR_PRN(err_info[ERR_TLOC_NOT_OPEN_RD].err_str);
            return_val = err_info[ERR_TLOC_NOT_OPEN_RD].err_code;
            break;
	    }
        if (tape_io_p->eof) {

            ERR_PRN(err_info[ERR_ATTEMPT_TLOC_EOF].err_str);
            return_val = err_info[ERR_ATTEMPT_TLOC_EOF].err_code;
            break;
	    }

#if SYNC
        if ((return_val = sync_tio(lvcb_p, tape_io_p)) < 0) {
            break;
        }
#endif

        /*--------------------------------------------------
         * Reset the tape block status fields and all of
         * the buffer status to there initial state,
         * effectively flushing the tape block buffer
         * information.
         *
         * Then perform the locate command.
         *
         * Then if buffered mode issue a IREAD command.
         --------------------------------------------------*/

        tape_io_p->blk_proc_p = (char *)NULL;
        tape_io_p->blk_len = 0;

        /*--------------------------------------------------
        * Reset head and tail to the beginning and
        * make all buffers status EMPTY
        --------------------------------------------------*/

        tape_io_p->head = tape_io_p->tail = tape_io_p->dmem_p;
        do {

            tape_io_p->head->status = EMPTY;
            tape_io_p->head = tape_io_p->head->next;

        } while (tape_io_p->head != tape_io_p->tail);


        if (tape_io_p->eod) {

#if SPIDER
            if (spider & SPIDER_TLOCATE) {
                PRN_LOC(" Reset EOD conditions\n");
            }
#endif

            tape_io_p->eod = FALSE;
        }

#if SPIDER
        if (spider & SPIDER_TLOCATE) {
            PRN_LOC(" Call locate_tape_id\n");
        }
#endif

        if ((return_val = locate_tape_id(lvcb_p->fildes, block_id)) < 0) {
            ERR_PRN(err_info[ERR_LOC_TAPE_ID].err_str);
            fprintf(tape_io_p->logdev, " return = %d\n", return_val);
            return_val = err_info[ERR_LOC_TAPE_ID].err_code;
            break;
        }

        /*--------------------------------------------------
         * Reset the blockcnt and phy_blk_num to 0, now
         * these become internal relative tape positioning
         * control fields and have no relationship to
         * the physical (block) tape position.
         *
         * The lvcb.locate_issued field indicates to the
         * label processing code that the blockcnt is no
         * longer valid.
         --------------------------------------------------*/

        tape_io_p->phy_blk_num = lvcb_p->blockcnt = 0;
        lvcb_p->located = TRUE;

        if (lvcb_p->buffered_mode) {

#if SPIDER
            if (spider & SPIDER_TLOCATE) {
                PRN_LOC(" Call issue_iread (6)\n");
            }
#endif
            if ( (return_val = issue_iread(lvcb_p->fildes, tape_io_p,
                            (char *)&((BLK_BUF *)tape_io_p->head)->data,
                                        lvcb_p->rec2.blocklen)) < 0) {
                break;

            }
        }

    } while (0);

    return(return_val);
}


