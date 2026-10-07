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
 * $Log: siop_script.h,v $
 * Revision 1.3.8.1  1995/06/11  18:33:55  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.3  1995/03/28  01:20:09  jerrie
 * Increase controller interface queue size.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low.
 *  Benefit or PTS #: 12827
 *  Testing:	   Developer
 *  Module(s):	   See PTS report.
 *
 * Revision 1.2  1995/03/14  23:44:05  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	siop_script.h
 *	Author:	Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	5/94
 *
 *	NCR 53C720 SCRIPT definitions
 */


#ifndef	_SIOP_SCRIPT_H_
#define	_SIOP_SCRIPT_H_

#ifdef	SIOP0

#define	SIOP			0
#define SCRIPT			siop0_script
#define LABELPATCHES		siop0_labelpatches
#define INSTRUCTIONS		siop0_instructions
#define PATCHES			siop0_patches
#define SIZEOF_SCRIPT		siop0_script_size
#define REG_BASE		SDB_SCSI0_START

#endif	SIOP0

#ifdef	SIOP1

#define	SIOP			1
#define SCRIPT			siop1_script
#define LABELPATCHES		siop1_labelpatches
#define INSTRUCTIONS		siop1_instructions
#define PATCHES			siop1_patches
#define SIZEOF_SCRIPT		siop1_script_size
#define REG_BASE		SDB_SCSI1_START

#endif	SIOP1

#if	MACH_ASSERT
#define SCRIPT_SIZE		(5 * 1024)	/* scripts with debugging */
#else	MACH_ASSERT
#define SCRIPT_SIZE		(4 * 1024)	/* scripts without debugging */
#endif	MACH_ASSERT

#define SCRIPT_SHARED_DATA_SIZE	(SIOPS_SIZEOF_DPRAM / NSIOP)
#define RESELECT_TABLE_SIZE	\
	(sizeof(unsigned long) * MAX_QUEUE_TAGS * MAX_SCSI_TARGETS)
#define SIZEOF_REQ_TBL_AREA	\
	SIOPS_SIZEOF_SRAM - ((SCRIPT_SIZE + RESELECT_TABLE_SIZE) * NSIOP)

#if	1
#define SCRIPT_Q_SIZE	( 						   \
		(SCRIPT_SHARED_DATA_SIZE - 128)				   \
		/							   \
		(sizeof(struct request_table) + sizeof(struct cntrlr_buf)) \
			)
#else	1
#define SCRIPT_Q_SIZE	8
#endif	1

#ifdef	SIOP

#define SCRIPT_ADDR		(SDB_SRAM_START + (SCRIPT_SIZE * SIOP))
#define SCRIPT_SHARED_DATA_ADDR	\
			(SDB_DPRAM_START + (SCRIPT_SHARED_DATA_SIZE * SIOP))

#define shrd_data	((struct cntrlr_shrd_data *)SCRIPT_SHARED_DATA_ADDR)

#define CNTRLR_Q	shrd_data->cntrlr_q
#define REQ_DONE_Q	shrd_data->req_done_q
#define RESELECT_TBL	\
	(SDB_SRAM_START + (SCRIPT_SIZE * NSIOP) + (RESELECT_TABLE_SIZE * SIOP))

#define NO_WORK_TO_DO	(&shrd_data->stat.byte.no_work_to_do)
#define HALT_REASON	(&shrd_data->stat.byte.halt_reason)
#define STATUS_BUF	(&shrd_data->stat.byte.status_buf)
#define MSG_IN_CNT	(&shrd_data->stat.byte.msg_in_count)
#define MSG_IN_BUF	(&shrd_data->msg_in.byte[0])
#define NEXT_ADDR	(&shrd_data->next_addr)
#define REQ_TBL_PTR	(&shrd_data->req_tbl_clram_ptr)

#endif	SIOP

#endif	_SIOP_SCRIPT_H_
