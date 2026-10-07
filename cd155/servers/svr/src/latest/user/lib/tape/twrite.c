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
 *	twrite.c 11.1 94/03/22 16:49:52
 */
static char	twrite_ver[] = "@(#) sourcefile twrite.c 11.1 94/03/22 16:49:52";

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

/*--------------------------------------------------------------------------
 *
 *  twrite.c
 *
 *  INPUT:   handle: handle from the TALLOCTAPE
 *           buffer: pointer where the user data is for write
 *           length: length of the user buffer
 *
 *  OUTPUT:  positive:   Number of bytes written
 *
 *           -or-
 *
 *           negative: indicating an error
 *
 *  Description:
 *
 *     This routine accepts records from the user and writes them to
 *     a tape block buffer.  When the tape block buffer is full it is
 *     written to the tape device.
 *
 *     The user can configure different modes of operation for writing
 *     the tape device:
 *
 *         BUFFERED:
 *
 *             The user data is copied from buf to an internal tape
 *             block buffer to be queued for transfer to the tape
 *             device.
 *
 *         NON-BUFFERED:
 *
 *             The user data is sent directly to the tape device without
 *             buffering on the Compute node (RX).
 *
 *
 *       BLOCK MODE:
 *
 *         The record format and block attribute are ignored
 *         and the buffer passed by the user is considered
 *         to be the entire tape block.  The length parameter must
 *         be less than or equal to the block length in the DCB.
 *
 *       NON-BLOCK MODE:
 *
 *         Each invocation of the twrite passes 1 record.
 *
 *         Undefined Length records:   The length parameter must
 *             be less than or equal to block length in the DCB.
 *
 *         Fix Length records: Block length % record length must be 0.
 *           UNBUFFERED MODE: Block length must equal record length
 *                 (1 record per block) and the length parameter must
 *                 be equal to record length in the DCB.
 *
 *         Variable Length records:    BUFFERED MODE ONLY, length must
 *             be less than or equal to record length in the DCB.  Each
 *             record is placed into the tape block buffer, until it is
 *             full, then the buffer is written to the tape device.
 *
 *
 *
 -------------------------------------------------------------------------*/


#include <stdio.h>
#include "type.h"
#include "convt.h"
#include "cnvproto.h"
#include "tapeio.h"
#include "tape3480.h"
#include "tioerr.h"
#include <sys/errno.h>
#include "tioproto.h"

extern TAPEMAIN tapemain;

/* 
 * Disable the misaligned data access trap
 */
#define	NOTRAP	1

int 
twrite(int handle, char *buffer, int length) 
{
	int      force_iwrite = 0, return_val = 0, sav_len = length;
	int      sdw_state = SDW_IDLE, sdw_reclen = 0, temp =0;
	int	error=0; 	/* store errno after write */
	bit16    block_len;
	char    *rec_p = NULL, *sdw_buf_p = NULL;
	ubit16	temp_ubit16;
	LVCB    *lvcb_p = NULL;
	TAPE_IO *tape_io_p;
	int i=0;

#if SPIDER
    int  spider = tapeio_glob.spider;
#endif

#if	NOTRAP
	asm ("trap r0,r18,r0");
#endif	/* NOTRAP */

    if ((lvcb_p = tapemain.lvcblist[handle]) == NULL) {

        fprintf(stderr, "\n%s #%04d ERROR:  %s", __FILE__, __LINE__,
                        err_info[ERR_INVALID_HANDLE].err_str);
        fprintf(stderr, " (%d)\n", handle);
        fflush(stderr);
		return(err_info[ERR_INVALID_HANDLE].err_code);
	}

	tape_io_p = (TAPE_IO *)&(tapemain.lvcblist[handle]->tape_io);

#if SPIDER
    if (spider & SPIDER_ENTER_EXIT) {
        fprintf(tape_io_p->logdev, "\n");
        PRN_LOC(" ENTRY:\n");
        fprintf(tape_io_p->logdev,
                ">> Handle: %d  LVCB ptr: %08X User buf: %08X record len: %d\n",
                    handle, lvcb_p, buffer, length);

    }
#endif

    do {
        if (lvcb_p->read || (tape_io_p->dmem_p == NULL)) {

            tio_log_init(lvcb_p);
            ERR_PRN(err_info[ERR_NOT_OPEN_FOR_WRITE].err_str);
		    return_val = err_info[ERR_NOT_OPEN_FOR_WRITE].err_code;
            break;
	    }
    	if (tape_io_p->eof) {

            ERR_PRN(err_info[ERR_ATTEMPT_WRT_AFTER_EOF].err_str);
            return_val = err_info[ERR_ATTEMPT_WRT_AFTER_EOF].err_code;
            break;
        }
        if (length == 0) {

            ERR_PRN(err_info[ERR_ZERO_LEN_WRITE].err_str);
            return_val = err_info[ERR_ZERO_LEN_WRITE].err_code;
            break;
	    }

        /*--------------------------------------------------
         *  Check to see if End of Media has been reached,
         *  which indicates (LEOT - Logical End of Tape).
         *
         *  If so, let all outstanding IWRITE commands
         *  complete.
         *
         *  Then call the eod() routine to build the EOV
         *  trailers and perform the Volume Switch.
         --------------------------------------------------*/

        if (tape_io_p->eod) {

#if SPIDER
            if (spider & SPIDER_TWRITE) {
                PRN_LOC(" EOM reached..SYNC outstanding iwrites\n");
            }
#endif

#if SYNC
            if ((return_val = sync_tio(lvcb_p, tape_io_p)) < 0) {
                break;
            }
#endif

            /* Call eod routine to switch volumes */

#if SPIDER
            if (spider & SPIDER_TWRITE) {
                PRN_LOC(" Call eod - Switch Volumes\n");
            }
#endif

            /*--------------------------------------------------
             * Set eof TRUE to prevent any commands while
             * outside of the dataset.
             *
             * Call eod() - Return to user if an ERROR occurs
             * Return_val is error code.
             *
             * If NEW_VOL is returned then a new tape volume
             * has been mounted and is positioned on the EOT
             * side of the Tape Mark that separates the Volume
             * Headers from the Data Set.  Reset the physical
             * block number (tape_io_p->phy_blk_num) to 0
             * because this tracks the number of blocks read
             * per tape.
             --------------------------------------------------*/

            tape_io_p->eof = TRUE;

            if ((return_val = eod(handle)) == NEW_VOL) {
                tape_io_p->eof = tape_io_p->eod = FALSE;
                tape_io_p->phy_blk_num = 0;

#if SPIDER
                if ((spider == 0) || (spider & SPIDER_TWRITE)) {

                    PRN_LOC(" EOM: Volume Switch\n>> ");
                    prn_tiostats(lvcb_p, tape_io_p);
                }
#endif

            }
            else {
                break;
            }
        }

        /*--------------------------------------------------
         * NON-BUFFERED (UNBUFFERED) MODE:
         *
         *  Each invocation of this routine passes a user
         * defined buffer to the tape device.  The length
         * is verified for each of the allowed record
         * formats and a synchronous _cwrite is issued.
         *
         * If a End-Of-Media (EOM) condition is sensed then
         * a flag is set so the next entry into twrite will
         * call the eod routine.
         *
         --------------------------------------------------*/

        if (!lvcb_p->buffered_mode) {
#if SPIDER
            if (spider & SPIDER_TWRITE) {
                PRN_LOC(" UNBUFFERED MODE\n");
            }
#endif
            switch(tape_io_p->record_format & REC_FORMAT) 
	{
                case FIXED:
		if (length != lvcb_p->rec2.blocklen) {
			ERR_PRN(err_info[ERR_INVALID_WRT_BUF_LEN].err_str);
			fprintf(tape_io_p->logdev,
				" FIXED: length (%d) != blocklen (%d)\n",
				length, lvcb_p->rec2.blocklen);
			return_val = err_info[ERR_INVALID_WRT_BUF_LEN].err_code;
		}
		break;
                case UNDEFINED:
                    if (length > lvcb_p->rec2.blocklen) {
                        ERR_PRN(err_info[ERR_INVALID_WRT_BUF_LEN].err_str);
                        fprintf(tape_io_p->logdev,
                                " UNDEF: length (%d) > blocklen (%d)\n",
                                length, lvcb_p->rec2.blocklen);
                 return_val = err_info[ERR_INVALID_WRT_BUF_LEN].err_code;
                    }
                    break;
                default:
                    ERR_PRN(err_info[ERR_INVALID_PARAM].err_str);
                    fprintf(tape_io_p->logdev, " (%d)\n",
                            tape_io_p->record_format & REC_FORMAT);
                    return_val = err_info[ERR_INVALID_PARAM].err_code;
            }
            if (return_val < 0) {
                break; /* while(1): ERROR */
            }
#if SPIDER
            if (spider & SPIDER_TWRITE) {
                PRN_LOC("");
                fprintf(tape_io_p->logdev,
                        " CWRITE: fildes %d  User Buf ptr %08X  Len %d\n",
                          lvcb_p->fildes, buffer, length);

            }
#endif
            if (write(lvcb_p->fildes, buffer, length) < 0 ) {
		error = errno;
#if TEOM	/* replace tiseom with errno check */
                if (tiseom(lvcb_p->fildes)) {
#endif	
		if ( error == ENOSPC ) {
		    printf("tiseom 1 reached\n");
#if SPIDER
                    if (spider & SPIDER_TWRITE) {
                        PRN_LOC(" EOM Encountered\n");
                    }
#endif
                    tape_io_p->eod = TRUE;
                }
                else {
                    ERR_PRN(err_info[ERR_CWRITE].err_str);
                    fprintf(tape_io_p->logdev, " (%d)\n", errno);
                    return_val = err_info[ERR_CWRITE].err_code;
                }

            }
#if SPIDER
            if (spider & SPIDER_TWRITE) {
                PRN_LOC(" CWRITE DONE\n");
            }
#endif
            ++tape_io_p->num_cwrites;
            ++lvcb_p->blockcnt;
            lvcb_p->written = TRUE;

            /*--------------------------------------------------
            *  After an actual write to the tape device, set the
            *  block_id field to 0 to invalidate the
            *  block ID of the last tget_blk_id command, this
            *  will force the next tget_blk_id command to
            *  actually go out to the drive for the block ID.
            --------------------------------------------------*/
            tape_io_p->block_id = 0;

            break;  /* while (1): DONE */
        }

        /*--------------------------------------------------
         * BUFFERED MODE:
         *
         *  Receive records and place into the tape block
         *  buffer until it is full or a full condition
         *  is met.  Then write the buffer out.  Whenever
         *  the tape_io_p->blk_proc_p is set to NULL the
         *  tape block buffer will be written to the tape
         *  device.
         *
         *  In variable record length record format mode
         *  a condition can exist where the current tape
         *  block buffer can not contain the record.  A force
         *  iwrite condition will be asserted to write the
         *  current tape block buffer to the tape device. The
         *  code will then restart to place the current record
         *  into the next available tape block buffer.
         *
         --------------------------------------------------*/

        /*--------------------------------------------------
         * But first, do some house cleaning, check the
         * tape block buffer at the tail to see if it has
         * been used for a iwrite (FULL).  If so see if all
         * the tape block buffers are outstanding (head == tail),
         * if this is true then BLOCK waiting for this buffer
         * to return (tiowait).
         *
         * If a iwrite is outstanding but not all of the buffers
         * are in use then just poll to see if this one is
         * fininshed (tiodone).
         *
         --------------------------------------------------*/

#if SPIDER
        if (spider & SPIDER_TWRITE) {
            PRN_LOC(" BUFFERED MODE\n");
        }
#endif
        if (tape_io_p->tail->status == FULL) {
#if SPIDER
            if (spider & SPIDER_TWRITE) {
                PRN_LOC(" Tail status: FULL ");
                fprintf(tape_io_p->logdev, " Bytes read: %08X\n",
                            tape_io_p->tail->length);

            }
#endif
            if (tape_io_p->head == tape_io_p->tail) {
#if SPIDER
                if (spider & SPIDER_TWRITE) {
                    PRN_LOC(" NOP, was  BLOCKED, wait for IWRITE to finish\n");
                }
#endif
		
		/* 
		 * Assuming not longer using iread/iwrite, we would have
		 * assume the write is done.
		 */
#if TWAIT
                return_val = tiowait(tape_io_p->tail->mid);  
		/* BLOCK til done */
                ++tape_io_p->oper_blked;
                tape_io_p->tail->status = EMPTY;
#endif
            }
            else if (!force_iwrite) {
#if TDONE
			if (return_val = tiodone(tape_io_p->tail->mid)) {
#endif
			/* using write instead of iwrite assume done */
			tape_io_p->tail->status = EMPTY;
#if TDONE
                	}
	                else {
	       	             ++tape_io_p->oper_busy;
#if SPIDER
       		             if (spider & SPIDER_TWRITE) 
       		                 PRN_LOC( " IWRITE still BUSY\n");
#endif
               		 }
#endif
            }
            force_iwrite = FALSE;

            if (tape_io_p->tail->status == EMPTY) {

                /* iwrite DONE - Check for End-Of-Media Condition */

                tape_io_p->tail->mid = CMD_COMPLETE;
                lvcb_p->written = TRUE;


#if SPIDER
                if (spider & SPIDER_TWRITE) {
                    PRN_LOC(" IWRITE Done\n");
                }
#endif
                if (return_val < 0) {
                    if (tiseom(lvcb_p->fildes)) {
                        tape_io_p->eod = TRUE;
#if SPIDER
                        if (spider & SPIDER_TWRITE) {
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
                tape_io_p->tail = tape_io_p->tail->next;

                /*--------------------------------------------------
                *  After an actual write to the tape device, set the
                *  block_id field to 0 to invalidate the
                *  block ID of the last tget_blk_id command, this
                *  will force the next tget_blk_id command to
                *  actually go out to the drive for the block ID.
                --------------------------------------------------*/
                tape_io_p->block_id = 0;

#if SPIDER
                if (spider & SPIDER_TWRITE) {
                    PRN_LOC("");
                    fprintf(tape_io_p->logdev,
                            " Switch Buffers: Next Tail ptr: %08X  phy_blk_cnt: %d\n",
                                tape_io_p->tail, tape_io_p->phy_blk_num );
                }
#endif
                if (tape_io_p->eod) {
			printf("eod set, continue\n");
                    continue;  /* top of loop - Wait for all iwrites switch VOL */
                }
            }
        }

        /*--------------------------------------------------
         * There is always a buffer available for write at
         * this point because of the BLOCKING action when
         * all of the buffers are out.
         *
         * When a tape block buffer switch takes place
         * (tape_io_p->blk_proc_p == NULL) then
         * set block length to 0.
         * Otherwise an existing tape block buffer is
         * still in use so restore the rec_p and block_len
         * to the proper values.
         --------------------------------------------------*/

        if (tape_io_p->blk_proc_p == (char *)NULL) {
            tape_io_p->blk_len = 0;
#if SPIDER
            if (spider & SPIDER_TWRITE) {
                PRN_LOC(" NEW Tape Block Buffer\n");
            }
#endif
        }

        block_len = tape_io_p->blk_len;
        rec_p = tape_io_p->blk_proc_p;
#if SPIDER
        if (spider & SPIDER_TWRITE) {
            PRN_LOC(" PROCESS Tape Block Buffer\n");
            fprintf(tape_io_p->logdev,
                    ">> buf_start: %08X  rec_p: %08X block_len: %d\n",
                        tape_io_p->head, rec_p,
                        block_len );
        }
#endif

        switch(tape_io_p->record_format & REC_FORMAT) {
            case VARIABLE:

                if ( tape_io_p->record_format & SPANNED_REC ) {
                    /*--------------------------------------------------
                     *  If this is the first segment of a record
                     *  then place the users buffer pointer and length
                     *  values into working variables to be used during
                     *  the record segmentation process.
                     *
                     *  Otherwise, this is not the first segment so
                     *  store the modified buffer pointer into the
                     *  variable 'buffer' which is used later as the
                     *  pointer where data is moved from the user
                     *  buffer to the tape block buffer.
                     --------------------------------------------------*/
                    if ( sdw_state == SDW_IDLE ) {
                        sdw_reclen = length;
                        sdw_buf_p = buffer;
                    }
                    else {
                        buffer = sdw_buf_p;
                    }
#if SPIDER
                    if (spider & SPIDER_TWRITE) {
                        PRN_LOC(" VARIABLE SPANNED:\n");
                        fprintf(tape_io_p->logdev,
                                ">> sdw_reclen: %d  sdw_buf_p: %08X\n",
                                    sdw_reclen, sdw_buf_p) ;
                    }
#endif

                    /*--------------------------------------------------
                     *  Test to ensure that if the recfm = V and
                     *  blkattr = S (variable spanned records that
                     *  are not blocked) that a new tape block is being
                     *  used (rec_p == 0).
                     --------------------------------------------------*/
                    if ( !(tape_io_p->record_format & BLOCKED_RECORDS) && rec_p ) {
                        ERR_PRN(err_info[ERR_INVALID_BLOCK_COND ].err_str);
                        return_val = err_info[ERR_INVALID_BLOCK_COND ].err_code;
                        break;
                    }
                    /*--------------------------------------------------
                     *  Determine the amount of data from the user
                     *  record to place into the next segment.
                     *
                     *     First check to see if a rec2.reclen record
                     *     segment will fit into the remaining space
                     *     of the tape block buffer.
                     *        NO:
                     *              If the remaining amount of data from
                     *              the user buffer (sdw_reclen) will fit
                     *              into this space then set length to
                     *              sdw_reclen.  Otherwise set length to
                     *              the amount of space left and write out
                     *              another segment.
                     *
                     *        YES:
                     *              Check to see if sdw_reclen will fit
                     *              into a rec2.reclen segment, if it does
                     *              set length to sdw_reclen, otherwise
                     *              set length to rec2.reclen - 4.
                     *
                     --------------------------------------------------*/
                    if ( lvcb_p->rec2.reclen >
                            lvcb_p->rec2.blocklen - block_len ) {
                        if ( sdw_reclen >
                                (temp = lvcb_p->rec2.blocklen - block_len - sizeof(RDW)) ) {
                            length = temp;
                        }
                        else {
                            length = sdw_reclen;
                        }
                    }
                    else {
                        if ( sdw_reclen >
                                (temp = lvcb_p->rec2.reclen - sizeof(RDW)) ) {
                            length = temp;
                        }
                        else {
                            length = sdw_reclen;
                        }
                    }

                    /*--------------------------------------------------
                     *  Now determine the SDW segment control code:
                     *
                     *  1. SDW_IDLE (0x00): If this record segment contains
                     *      all of the record data.
                     *  2. SDW_START (0x01): This is the first record segment
                     *      for the spanned record.
                     *  3. SDW_MID (0x03): This is another segment between
                     *      'start' and 'end' for the spanned record.
                     *  4. SDW_END (0x02): This is the last segment for
                     *      the spanned record.
                     *
                     --------------------------------------------------*/

                    if ( (sdw_reclen -= length) == 0 ) {
                        if ( sdw_state )
                            sdw_state = SDW_END;
                    }
                    else {
                        sdw_state = ( sdw_state == SDW_IDLE ) ?
                                                    SDW_START : SDW_MID ;
                    }
#if SPIDER
                    if (spider & SPIDER_TWRITE) {
                        PRN_LOC(" VARIABLE SPANNED:\n");
                        fprintf(tape_io_p->logdev,
                                ">> sdw_state: %02X segment length: %d  remaining bytes: %d\n",
                                    sdw_state, length, sdw_reclen) ;
                    }
#endif

                }
                /*--------------------------------------------------
                 *  Variable NOT spanned records, ensure that the
                 *  record length passed is less than or equal to
                 *  the rec2.reclen - 4.
                 --------------------------------------------------*/
                else if (length > (lvcb_p->rec2.reclen - sizeof(RDW))) {
                    ERR_PRN(err_info[ERR_INVALID_WRT_BUF_LEN].err_str);
                    fprintf(tape_io_p->logdev,
                                " VARIABLE: length (%d) > reclen (%d)\n",
                                length, lvcb_p->rec2.reclen);
                    return_val = err_info[ERR_INVALID_WRT_BUF_LEN].err_code;
                    break;
                }

                /*--------------------------------------------------
                 *  If partial buffer:  Will the current record fit
                 *  into the remaining space ?
                 *
                 *      yes - Build the Record Descriptor Word (RDW)
                 *            and break out to code that moves the
                 *            record into the buffer (see below)
                 *
                 *      no  - Build the Block Descriptor Word (BDW)
                 *            and force an iwrite for this tape block
                 *            buffer.  By forcing an iwrite, when
                 *            return from iwrite the code will restart
                 *            at the (DO) to place the current record
                 *            into the next tape block buffer.
                 *
                 --------------------------------------------------*/

                if (rec_p) {  /* partial buffer */
		if ( (block_len + length + sizeof(RDW) ) > 
			lvcb_p->rec2.blocklen) 
		{

                        /*--------------------------------------------------
                         *  Record will not fit. Build the BDW and force
                         *  a iwrite for the current tape block buffer.
                         --------------------------------------------------*/
#if SPIDER
                        if (spider & SPIDER_TWRITE) {
                            PRN_LOC(" VARIABLE: RECORD won't fit\n");
                            fprintf(tape_io_p->logdev,
                                    " length + RDW: %d  block_len: %d\n",
                                     length + sizeof(RDW), block_len) ;
                        }
#endif
                        rec_p = (char *)NULL;
                        force_iwrite = TRUE;
#if SPIDER
                        if (spider & SPIDER_TWRITE) {
                            PRN_LOC(" FORCE IWRITE\n");
                        }
#endif
                        break;
                    }
                }

                /*--------------------------------------------------
                 *  New tape block buffer - allocate space for the
                 *  BDW (position rec_p to the RDW of the 1st
                 *  record and build the Record Descriptor Word (RDW).
                 --------------------------------------------------*/
                else {
                    rec_p = (char *)&(((VAR_BLK_BUF *)tape_io_p->head)->rdw);
                    block_len = sizeof(BDW);
#if SPIDER
                    if (spider & SPIDER_TWRITE) {
                        PRN_LOC(" VARIABLE: ALLOC space for BDW\n");
                    }
#endif
                }

                /*--------------------------------------------------
                 *  Build the Record Descriptor Word (RDW)
                 *
                 *  If Variable Records:
                 *      Length High byte
                 *      Lengh  Low byte
                 *      0
                 *      0
                 *
                 *  If Spanned Records (RDW is a SDW):
                 *      Length High byte
                 *      Length Low byte
                 *      Segment Control Code
                 *      0
                 --------------------------------------------------*/
		*((bit16 *)rec_p) = ( bit16 )itoI_16(length + sizeof(RDW)); 
                rec_p += sizeof(bit16);

                if ( tape_io_p->record_format & SPANNED_REC )
                    *((bit16 *)rec_p) = sdw_state;
                else
                    *((bit16 *)rec_p) = 0;

                rec_p += sizeof(bit16);
                block_len += sizeof(RDW);
#if SPIDER
                if (spider & SPIDER_TWRITE) {
                    PRN_LOC(" VARIABLE: BUILD RDW");
                    fprintf(tape_io_p->logdev, " (len: %d bytes)\n",
                            length + sizeof(RDW) );
                }
#endif

                break;

            case FIXED:

                if (length != lvcb_p->rec2.reclen) {
                    ERR_PRN(err_info[ERR_INVALID_WRT_BUF_LEN].err_str);
                    fprintf(tape_io_p->logdev,
                                " FIXED: length (%d) != reclen (%d)\n",
                                length, lvcb_p->rec2.reclen);
                    return_val = err_info[ERR_INVALID_WRT_BUF_LEN].err_code;
                }
                else {
#if SPIDER
                    if (spider & SPIDER_TWRITE) {
                        PRN_LOC(" FIXED: ");
                        fprintf(tape_io_p->logdev,
                                "record len: %d  Cuml tape blklen: %d\n",
                                length, block_len);
                    }
#endif
                }

                break;


            case UNDEFINED:

                if (length > lvcb_p->rec2.blocklen) {
                    ERR_PRN(err_info[ERR_INVALID_WRT_BUF_LEN].err_str);
                    fprintf(tape_io_p->logdev,
                                " UNDEFINED: length (%d) > blocklen (%d)\n",
                                length, lvcb_p->rec2.blocklen);
                    return_val = err_info[ERR_INVALID_WRT_BUF_LEN].err_code;
                }
                else {
#if SPIDER
                    if (spider & SPIDER_TWRITE) {
                        PRN_LOC(" UNDEFINED: ");
                        fprintf(tape_io_p->logdev,
                                "record len: %d  tape block len: %d\n",
                                length, block_len);
                    }
#endif
                }

                break;

            default:
                ERR_PRN(err_info[ERR_INVALID_PARAM].err_str);
                fprintf(tape_io_p->logdev, " (%d)\n",
                        tape_io_p->record_format & REC_FORMAT);
                return_val = err_info[ERR_INVALID_PARAM].err_code;
        }

        if (return_val < 0) {   /* ERROR */
            break;
        }

        if (!force_iwrite) {
            /*--------------------------------------------------
             * FIXED or UNDEFINED record format (rec_p = 0): When a
             * buffer switch occurs (new buffer) set the block process
             * pointer to the TOP of the buffer.  Variable record
             * format already set the pointer up.
             --------------------------------------------------*/

            if (rec_p == NULL) {
                rec_p = (char *)&((BLK_BUF *)tape_io_p->head)->data;
            }

#if SPIDER
            if (spider & SPIDER_TWRITE) {
                PRN_LOC(" Copy User Data to Tape Block Buffer\n");
                fprintf(tape_io_p->logdev,
                        ">> User Buf: %08X  tape blk ptr: %08X  length: %d\n",
                         buffer, rec_p, length );
            }
#endif
            memcpy(rec_p, buffer, length);
            block_len += length;

            switch (tape_io_p->record_format & REC_FORMAT) {

                case VARIABLE:

                    /*--------------------------------------------------
                     *  Build / Update the Block Descriptor Word (BDW)
                     --------------------------------------------------*/

                    ((VAR_BLK_BUF *)tape_io_p->head)->bdw.blk_len = itoI_16(block_len);
                    ((VAR_BLK_BUF *)tape_io_p->head)->bdw.reserved = 0;

                    /*--------------------------------------------------
                     *  If block attribute is UNBLOCKED records
                     *   1 record / block
                     *
                     *      -- or --
                     *
                     *  If the blkattr is 'R' for blocked spanned records
                     *  then a tape block may contain multiple segments
                     *  from different records, but cannot contain multiple
                     *  segments from the same record.
                     *
                     *      -- or --
                     *
                     *  If this tape block buffer can't receive a 1 byte
                     *  record on the next invocation (another full
                     *  condition)
                     *
                     *  then, write it out.
                     --------------------------------------------------*/

                    if ((tape_io_p->record_format & BLOCKED_RECORDS) ==
                                                    UNBLOCKED_RECORDS) {

#if SPIDER
                        if (spider & SPIDER_TWRITE) {
                            PRN_LOC(" VARIABLE (UNBLOCKED): Issue IWRITE\n");
                        }
#endif

                    }
                    else if ( (tape_io_p->record_format & BLK_S_RECORDS) &&
                                                                sdw_reclen) {
#if SPIDER
                        if (spider & SPIDER_TWRITE) {
                            PRN_LOC(" VARIABLE SPANNED (Blocked):\n");
                            fprintf(tape_io_p->logdev,
                                    ">> Issue i_write (1 segment/block for each record)\n");
                        }
#endif
                    }

                    else if (block_len >= (lvcb_p->rec2.blocklen -
                                                    (sizeof(RDW)+1))) {
#if SPIDER
                        if (spider & SPIDER_TWRITE) {
                            PRN_LOC(" VARIABLE: Buffer Full\n");
                            fprintf(tape_io_p->logdev,
                                    ">> block_len: %d\n", block_len);
                        }
#endif
                    }
                    else
                        break;

                    /*--------------------------------------------------
                     *  If any of the above conditions are true then
                     *  set rec_p to 0 which will cause the current
                     *  tape block to be written out.
                     --------------------------------------------------*/
                    rec_p = (char *)NULL;
                    break;

                case FIXED:

                    /*--------------------------------------------------
                     *  If the tape block buffer is full then write it out.
                     --------------------------------------------------*/

                    if (block_len == lvcb_p->rec2.blocklen) {
#if SPIDER
                        if (spider & SPIDER_TWRITE) {
                            PRN_LOC(" FIXED: Buffer Full\n");
                            fprintf(tape_io_p->logdev,
                                    ">> block_len: %d\n", block_len);
                        }
#endif
                        rec_p = NULL;
                    }
                    break;

                case UNDEFINED:
                    /*--------------------------------------------------
                     *  Every invocation sends an entire tape block, so
                     *  write it out.
                     --------------------------------------------------*/

                    rec_p = NULL;
                    break;
            }
        }


        /* 
	 * ISSUE IWRITE 
	 */
        if (rec_p == NULL) {
		tape_io_p->head->length = write(lvcb_p->fildes,
                    &((BLK_BUF *)tape_io_p->head)->data, block_len);
		error = errno;
		if (tape_io_p->head->length  < 0) {
			ERR_PRN(err_info[ERR_IWRITE].err_str);
			fprintf(tape_io_p->logdev, " (%d)\n", errno);
			return_val = err_info[ERR_IWRITE].err_code;
			break;
		}
            ++tape_io_p->num_iwrites;
            ++lvcb_p->blockcnt;
            tape_io_p->head->status = FULL;
#if SPIDER
            if (spider & SPIDER_TWRITE) {
                PRN_LOC(" IWRITE: ");
                fprintf(tape_io_p->logdev, 
			"Fildes: %d  Buf_ptr: %08X  Length: %d\n",
                        lvcb_p->fildes,
			&((BLK_BUF *)tape_io_p->head)->data, block_len);
                fprintf(tape_io_p->logdev, 
			">> Bytes written: %08X  Block Count: %d\n",
                        tape_io_p->head->length, lvcb_p->blockcnt);
            }
#endif
            tape_io_p->head = tape_io_p->head->next;
            tape_io_p->blk_proc_p = (char *)NULL;
#if SPIDER
            if (spider & SPIDER_TWRITE) {
                PRN_LOC(" Switch Buffers");
                fprintf(tape_io_p->logdev, " New head: %08X\n",
                            tape_io_p->head);
            }
#endif
            if (force_iwrite && !(tape_io_p->record_format & SPANNED_REC) ) 
                continue;
        }
        else {

            /*--------------------------------------------------
             * Save the current block process pointer and
             * block length for the next pass
             --------------------------------------------------*/
            tape_io_p->blk_proc_p = rec_p + length;
            tape_io_p->blk_len = block_len;
printf("saving: block_len %d blk_proc_p is 0x%x\n", block_len, tape_io_p->blk_proc_p);

#if SPIDER
            if (spider & SPIDER_TWRITE) {
                PRN_LOC(" Tape Block Buffer NOT Full\n");
                fprintf(tape_io_p->logdev, ">> rec_p: %08X  block_len: %d\n",
                            tape_io_p->blk_proc_p, tape_io_p->blk_len);
            }
#endif

        }
        if (sdw_reclen) {
            /*--------------------------------------------------
                *  Adjust the sdw_buf_p by the number of bytes
                *  moved from the user buffer.  This pointer is
                *  used only during SPANNED record support,
                *  otherwise it is not used.
                --------------------------------------------------*/
            sdw_buf_p += length;
            continue;
        }
        break;

    } while (1); /* used to get to the top again */


    if ( return_val >= 0 ) {
        ++tape_io_p->num_records;
        return_val = sav_len;
    }
#if SPIDER
    if (spider & SPIDER_ENTER_EXIT) {
        PRN_LOC(" EXIT:");
        fprintf(tape_io_p->logdev,
            " return value: %d records processed: %d\n\n",
                    return_val, tape_io_p->num_records );
    }
    fflush(tape_io_p->logdev);

#endif
#if	NOTRAP
	asm ("trap r0,r3,r0");
#endif	/* NOTRAP */
    return(return_val);
}

