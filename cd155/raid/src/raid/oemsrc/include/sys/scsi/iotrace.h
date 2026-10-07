/*
 *
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */ 
/* Copyright 1990 NCR Corporation - Dayton, Ohio, USA */
#ident	"@(#)./sccs/NSTSRC/relintel/COMMON/usr/src/arch/mca/uts/i386/sys/scsi/iotrace.h:iotrace.h	1.3"

#ifndef SYS_IOTRACE_H
#define SYS_IOTRACE_H
#include "sys/signal.h"
#include "sys/ddi.h"

long	lbolt_val;
typedef struct	IO_Trace {
	clock_t	IO_Request_Arrive;	/* arrival to common scsi	*/
	clock_t	IO_Start;		/* lower driver command start	*/
	clock_t	IO_Done;		/* iodone from common scsi	*/
	unsigned IO_b_bcount;		/* transfer count		*/
	daddr_t	IO_b_blkno;		/* block # on device		*/
	dev_t	IO_b_edev;		/* expanded dev field		*/
	int	IO_b_flags;		/* see defines in buf.h		*/
} IO_Trace_t;

struct proc_t *DemonAddr;

#define	IO_TRACE_MAX	32768
#define	IO_TRACE_RESTART	30000

#define IO_TRACE_INIT \
	if (IO_Trace_ptr == NULL) { \
		IO_Trace_ptr = (struct IO_Trace *) \
			kmem_zalloc(sizeof(struct IO_Trace)*IO_TRACE_MAX, KM_SLEEP);\
		io_trace_cnt = -1;\
	}\

#define IO_TRACE_REQUEST_ARRIVE(bp) \
	if (io_trace_cnt >= 0 && IO_Trace_ptr) { \
		if (drv_getparm(LBOLT, &lbolt_val) == 0) { \
			(IO_Trace_ptr + io_trace_cnt)->IO_Request_Arrive = lbolt_val;\
		} \
		(IO_Trace_ptr + io_trace_cnt)->IO_b_bcount = (bp)->b_bcount;\
		(IO_Trace_ptr + io_trace_cnt)->IO_b_blkno = (bp)->b_blkno;\
		(IO_Trace_ptr + io_trace_cnt)->IO_b_edev = (bp)->b_edev;\
		(IO_Trace_ptr + io_trace_cnt)->IO_b_flags = (bp)->b_flags;\
		(bp)->b_reltime = io_trace_cnt++;\
		if ((io_trace_cnt >= IO_TRACE_RESTART) && (io_trace_demon_pid != 0)) {\
			if ((DemonAddr = prfind(io_trace_demon_pid)) != NULL)\
				psignal(DemonAddr, SIGUSR1);\
			else\
				io_trace_demon_pid = 0;\
		}\
		if (io_trace_cnt >= IO_TRACE_MAX) {\
			io_trace_cnt = -1;\
			printf ("io_trace_cnt >= IO_TRACE_MAX");\
		}\
	}\
	else {\
		(bp)->b_reltime = 0;\
	}\
		


/*#define IO_TRACE_START(bp) \
	if (io_trace_cnt >= 0 && (bp) && IO_Trace_ptr) { \
		if (drv_getparm(LBOLT, &lbolt_val) == 0) { \
			(IO_Trace_ptr + (bp)->b_reltime)->IO_Start = lbolt_val;\
		} \
	}\*/

#define IO_TRACE_START(s_op) \
	if (io_trace_cnt >= 0 && (s_op) && (s_op)->s_bp && IO_Trace_ptr) { \
		if (drv_getparm(LBOLT, &lbolt_val) == 0) { \
			(IO_Trace_ptr + (s_op)->s_bp->b_reltime)->IO_Start = lbolt_val;\
		} \
		(IO_Trace_ptr + (s_op)->s_bp->b_reltime)->IO_b_bcount = (s_op)->s_total_count;\
	}\



#define IO_TRACE_DONE(bp) \
	if (io_trace_cnt >= 0 && (bp) && IO_Trace_ptr) { \
		if (drv_getparm(LBOLT, &lbolt_val) == 0) { \
			(IO_Trace_ptr + (bp)->b_reltime)->IO_Done = lbolt_val;\
		} \
	}\

#endif
