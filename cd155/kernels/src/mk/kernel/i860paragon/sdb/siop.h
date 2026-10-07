/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: siop.h,v $
 * Revision 1.2.8.1  1995/06/11  18:33:51  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1995/03/14  23:44:03  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	siop.h
 *	Author:	Jerrie Coffman and Vineet Kumar
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	5/94
 *
 *	SCSI-16 SIOP hardware definitions
 */


#ifndef	_SIOP_H_
#define	_SIOP_H_

/*
 * Addresses of important modules in SCRIPTs code
 */
#define SCRIPT_INIT(base)		((base) + Ent_init)
#define SCRIPT_SCHEDULER(base)		((base) + Ent_schedular)
#define SCRIPT_MAIN(base)		((base) + Ent_main)

/*
 * Address of SCRIPT variables for debugging
 */
#define SCRIPT_CNTRLR_Q_P(base)		((base) + Ent_cntrlr_q_p)
#define SCRIPT_REQ_DONE_PTR(base)	((base) + Ent_req_done_ptr)
#define SCRIPT_IGNORE_WIDE_FLAG(base)	((base) + Ent_ignore_wide_flag)
#define SCRIPT_COPY_BUF(base)		((base) + Ent_copy_buf)
#define SCRIPT_VALUE_LOGIC_ANAL(base)   ((base) + Ent_data_analyzer_trig)

/*
 * Addresses of SCRIPT modules for debugging
 */
#define SCRIPT_ADD32(base)		((base) + Ent_add32)
#define SCRIPT_CMP32(base)		((base) + Ent_cmp32)
#define SCRIPT_MSG_SENT(base)		((base) + Ent_msg_sent)
#define SCRIPT_CMD_PHASE(base)		((base) + Ent_cmd_phase)
#define SCRIPT_CMD_SENT(base)		((base) + Ent_cmd_sent)
#define SCRIPT_COPY_ENTRY(base)		((base) + Ent_copy_entry)
#define SCRIPT_DATA_OUT_PHASE(base)	((base) + Ent_data_out_phase)
#define SCRIPT_DATA_IN_PHASE(base)	((base) + Ent_data_in_phase)
#define SCRIPT_ERR_UNKNOWN_PHASE(base)	((base) + Ent_err_unknown_phase)
#define SCRIPT_GOT_RESELECTED(base)	((base) + Ent_got_reselected)
#define SCRIPT_ISZERO32(base)		((base) + Ent_iszero32)
#define SCRIPT_MSG_IN_PHASE(base)	((base) + Ent_msg_in_phase)
#define SCRIPT_SCHED_EMPTY(base)	((base) + Ent_sched_empty)
#define SCRIPT_SEL_NO_ATN(base)		((base) + Ent_sel_no_atn)
#define SCRIPT_SHL16(base)		((base) + Ent_shl16)
#define SCRIPT_STATUS_PHASE(base)	((base) + Ent_status_phase)
#define SCRIPT_TARG_ID_Q_TAG_TO_ADDR(base)	\
	((base) + Ent_targ_id_q_tag_to_addr)

#endif	_SIOP_H_
