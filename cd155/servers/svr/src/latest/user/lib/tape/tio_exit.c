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
 *	tio_exit.c 11.1 94/03/22 16:49:43
 */
static char	tio_exit_ver[] = "@(#) sourcefile tio_exit.c 11.1 94/03/22 16:49:43";

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

/* tio_exit.c
 *
 * INPUT:   handle:     handle returned by the TAPEALLOC command
 *
 * OUTPUT:  int:        0 or errors (TBD)
 *
 * Description:
 *
 *      Memory that was allocated by tio_init is deallocated (free).
 *
 *
 * */

#include <stdio.h>
#include <sys/ioctl.h>
#include "mtio.h"
#include "type.h"
#include "tapeio.h"
#include "tape3480.h"
#include "tioerr.h"
#include "tioproto.h"

extern TAPEMAIN tapemain;

/* 
 * Do not need to wait on reads and writes in an asynchronous environment.
 */
#define	SYNC	0

int 
tape_io_exit(int handle) 
{
    int             return_val = 0;
    struct          mtop mt_ioctl;
    BLK_BUF_HDR    *blk_p;
    LVCB           *lvcb_p;
    TAPE_IO        *tape_io_p;

#if SPIDER
    int spider = tapeio_glob.spider;
#endif

    do {

        if ((lvcb_p = tapemain.lvcblist[handle]) == NULL) {

            fprintf(stderr, "\n%s #%04d ERROR:  %s", __FILE__, __LINE__,
                          err_info[ERR_INVALID_HANDLE].err_str);
            fprintf(stderr, " (%d)\n", handle);
            fflush(stderr);

            return_val = err_info[ERR_INVALID_HANDLE].err_code;
            break;
        }
        tape_io_p = (TAPE_IO *)&(lvcb_p->tape_io);

#if SPIDER
        if (spider & SPIDER_ENTER_EXIT) {
            fprintf(tape_io_p->logdev, "\n");
            PRN_LOC(" ENTRY:");
            fprintf(tape_io_p->logdev, " Handle: %d  LVCB ptr: %08X \n",
                        handle, lvcb_p);

        }
#endif

        if (lvcb_p->read) {
#if SYNC
            sync_tio(lvcb_p, tape_io_p);
#endif	/* SYNC */

            /*--------------------------------------------------
             * Close Data Set without previously hitting the
             * tapemark (TM).  Need to FSF 1 to get to the
             * EOT side of the TM.
             --------------------------------------------------*/

            if (!tape_io_p->eod) {

#if SPIDER
                if (spider & SPIDER_SYNC_TIO) {
                    PRN_LOC(" CLOSE Data Set:\
                            issue FSF to Position to EOT side of TM\n");
                }
#endif
                mt_ioctl.mt_op = MTFSF;
                mt_ioctl.mt_count = 1;

                if (ioctl(lvcb_p->fildes, MTIOCTOP, &mt_ioctl) < 0) {

                    ERR_PRN(err_info[ERR_CFS_IOCTL].err_str);
                    fprintf(tape_io_p->logdev,
                        " fd: %d MTFSF errno = %d\n",
                        lvcb_p->fildes, errno);
                    return_val = err_info[ERR_CFS_IOCTL].err_code;
                    break;

                }
                tape_io_p->eod = TRUE;
            }
        }


        else {
            /*--------------------------------------------------
             * WRITE: Allow all outstanding IWRITE operations
             *        to complete, then write out the partial
             *        buffer if it exists.
             --------------------------------------------------*/
#if SYNC
            if ((return_val = sync_tio(lvcb_p, tape_io_p)) == 0) {
#endif	/* SYNC */
                if (tape_io_p->blk_proc_p) {
#if SPIDER
                    if (spider & SPIDER_TAPEIO_EXIT) {
                        PRN_LOC(" Write Partial Block");
                        fprintf(tape_io_p->logdev, " %d bytes\n",
                                    tape_io_p->blk_len);
                    }
#endif
                    if (write(lvcb_p->fildes,
                                &((BLK_BUF *)tape_io_p->head)->data,
                                        tape_io_p->blk_len) < 0 ) {

                        ERR_PRN(err_info[ERR_CWRITE].err_str);
                        fprintf(tape_io_p->logdev, " (%d)\n", errno);
                        return_val = err_info[ERR_CWRITE].err_code;
                    }
                    else {
                        ++lvcb_p->blockcnt;
                        ++tape_io_p->num_cwrites;
                        tape_io_p->blk_proc_p = (char *)NULL;
                        return_val = 0;
#if SPIDER
                        if (spider & SPIDER_TAPEIO_EXIT) {
                            PRN_LOC("");
                            fprintf(tape_io_p->logdev, " Block Count %d\n",
                                        lvcb_p->blockcnt);
                        }
#endif
                    }
                }
#if SYNC
            } 
#endif	/* SYNC */
        }


#if SPIDER

        if ((spider == 0) || (spider & SPIDER_TAPEIO_EXIT)) {

            PRN_LOC(" Close Data Set\n>> ");
            prn_tiostats(lvcb_p, tape_io_p);
        }

        if (spider & SPIDER_ENTER_EXIT) {
            PRN_LOC(" ");
            fprintf(tape_io_p->logdev,
                " EXIT: Free mem: dmem_p = %08X  return_val = %d\n\n",
                    tape_io_p->dmem_p, return_val);
        }

#endif
        fflush(tape_io_p->logdev);      /* flush logdev in case stderr */

        if (tape_io_p->logdev != stderr) {
            fclose(tape_io_p->logdev);    /* close the log file */
            tape_io_p->logdev = stderr;   /* set back to stderr */
        }

        if (tape_io_p->dmem_p) {
            free(tape_io_p->dmem_p);     /* free memory allocated at tape_io_init() */
            tape_io_p->dmem_p = (char *)NULL;
        }

    } while(0);

    return (return_val);
}


