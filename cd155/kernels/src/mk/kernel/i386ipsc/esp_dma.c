/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS AS-IS
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: esp_dma.c,v $
 * Revision 1.5  1994/11/18  20:37:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:28:54  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:27:25  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:27:56  dleslie
 * First R1_0 release
 *
 * Revision 2.1.2.1  92/03/28  10:07:48  jeffreyh
 * 	Fix from intel
 * 	[92/03/26            jeffreyh]
 * 	New file from Intel for generic SCSI driver
 * 	[92/03/17            jeffreyh]
 * 
 */
/*
 *	File: esp_dma.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	12/91
 *
 *	Functions to support the DMA-like interface for the on-board SCSI
 */

#include <scsi/compat_30.h>
#include <scsi/scsi.h>
#include <scsi/scsi_defs.h>
#include <scsi/adapters/scsi_dma.h>
#include <scsi/adapters/scsi_53C90.h>
#include <scsi.h>
#include <i386ipsc/esp_ctlr.h>

#define gimmeabreak()		asm("int3")

extern scsi_dma_ops_t	esp_dma_ops;	/* at end */

/* 
 * Statically allocated command & temp buffers
 * This way we can attach/detach drives on-fly 
 */                                             
#define PER_TGT_BUFF_DATA	256             

static char	esp_buffer[NSCSI * 8 * PER_TGT_BUFF_DATA];

/* utilities */

#define	u_min(a,b)	(((a) < (b)) ? (a) : (b))

/*
 * Cold, probe time init
 */
opaque_t
esp_dma_init(unit, regp)
	int		unit;
	volatile char	*regp;
{
	return (opaque_t)0;
}

/*
 * A target exists
 */
void
esp_dma_new_target(dma, tgt)
	opaque_t	dma;
	target_info_t	*tgt;
{
	/* "virtual" address for our use */
	tgt->cmd_ptr = &esp_buffer[tgt->target_id * PER_TGT_BUFF_DATA];
	
	/* "physical" address for dma engine */
	tgt->dma_ptr = 0;

#ifdef	MACH_KERNEL
#else	/*MACH_KERNEL*/
	fdma_init(&tgt->fdma, scsi_per_target_virtual);
#endif	/*MACH_KERNEL*/
}

/*
 * Map, if necessary, user virtual addresses into
 * physical ones (or kernel virtuals if no true DMA)
 */
void
esp_dma_map(dma, tgt)
	opaque_t	dma;
	target_info_t	*tgt;
{
	/*
	 * We cannot do real DMA
	 */
	
#ifdef	MACH_KERNEL
#else	/*MACH_KERNEL*/
	if (tgt->ior)
		fdma_map(&tgt->fdma, tgt->ior);
#endif	/*MACH_KERNEL*/

	tgt->transient_state.out_count  = 0;
	tgt->transient_state.copy_count = 0;
	tgt->transient_state.dma_offset = 0;
}

int
esp_dma_start_cmd(dma, tgt)
	opaque_t	dma;
	register target_info_t	*tgt;
{
	register int	out_count;

	reset_fifo(tgt->hw_state);
	set_fifo_write(tgt->hw_state);

	/*
	 * Note that the out_count does not include the
	 * (optional) identify message but does include
	 * both the data and cmd out count.
	 */
	out_count = tgt->transient_state.cmd_count;

	return copy_out(tgt->hw_state, tgt->cmd_ptr, out_count);
}

void
esp_dma_end_xfer(dma, tgt, bytes_read)
	opaque_t	dma;
	target_info_t	*tgt;
	int		bytes_read;
{
	register char	*dma_ptr;

	/* Was this a write op ? if so, just return */
	if (bytes_read == 0) return;

	set_fifo_read(tgt->hw_state);

	/* would be nice to do this in parallel with xfer, earlier on */
	dma_ptr = tgt->dma_ptr + tgt->transient_state.dma_offset;

	copy_in(tgt->hw_state, dma_ptr, bytes_read);
}

void
esp_dma_end_cmd(dma, tgt, ior)
	opaque_t	dma;
	target_info_t	*tgt;
	io_req_t	ior;
{
#ifdef	MACH_KERNEL
#else	/*MACH_KERNEL*/
	fdma_unmap(&tgt->fdma, ior);
#endif	/*MACH_KERNEL*/
}

int
esp_dma_start_datain(dma, tgt)
	opaque_t	dma;
	register target_info_t	*tgt;
{
	register char	*dma_ptr;
	register int	count;

	reset_fifo(tgt->hw_state);
	set_fifo_read(tgt->hw_state);

	/* setup tgt->dma_ptr to tgt->cmd_ptr or to ior->io_data */
	if (tgt->cur_cmd == SCSI_CMD_READ ||
	    tgt->cur_cmd == SCSI_CMD_LONG_READ)
		dma_ptr = tgt->ior->io_data;
	else
		/* swear it won't disconnect or else */
		dma_ptr = tgt->cmd_ptr;
	tgt->dma_ptr = dma_ptr;

	count = tgt->transient_state.in_count;
	count = u_min(count, ESP_TC_MAX);

	return count;
}

int
esp_dma_restart_datain_1(dma, tgt)
	opaque_t	dma;
	target_info_t	*tgt;
{
	register int	count;

	set_fifo_read(tgt->hw_state);

	count = tgt->transient_state.in_count;
	count = u_min(count, ESP_TC_MAX);

	return count;
}

int
esp_dma_restart_datain_2(dma, tgt, xferred)
	opaque_t	dma;
	target_info_t	*tgt;
	int		xferred;
{
	register char	*dma_ptr;

	/* keep dma_offset anyways */
	dma_ptr = tgt->dma_ptr + tgt->transient_state.dma_offset;

	xferred = copy_in(tgt->hw_state, dma_ptr, xferred);

	tgt->transient_state.dma_offset += xferred;
	tgt->transient_state.copy_count += xferred;

	return xferred;
}

void
esp_dma_restart_datain_3(dma, tgt)
	opaque_t	dma;
	target_info_t	*tgt;
{
	/* Nothing needed here */
}

boolean_t
esp_dma_start_dataout(dma, tgt, regp, cmd)
	opaque_t	dma;
	register target_info_t	*tgt;
	volatile char	*regp, cmd;
{
	extern unsigned int esp_max_dma();
	register char	*dma_ptr;
	register int	count;
	boolean_t	write;

	reset_fifo(tgt->hw_state);
	set_fifo_write(tgt->hw_state);

	/* setup tgt->dma_ptr to tgt->cmd_ptr or to ior->io_data */
	if ((tgt->cur_cmd == SCSI_CMD_WRITE) ||
	    (tgt->cur_cmd == SCSI_CMD_LONG_WRITE)) {
		io_req_t	ior = tgt->ior;
		unsigned int	max_dma;

		write = TRUE;
		dma_ptr = ior->io_data;

		max_dma = esp_max_dma(tgt->hw_state);
		count = u_min(ior->io_count, max_dma);
		tgt->transient_state.out_count = count;

	} else {

		write = FALSE;
		dma_ptr = tgt->cmd_ptr + tgt->transient_state.cmd_count;
		count = tgt->transient_state.out_count;

	}
	tgt->dma_ptr = dma_ptr;

	count = u_min(count, ESP_TC_MAX);
	count = copy_out(tgt->hw_state, dma_ptr, count);
	tgt->transient_state.copy_count = count;

	/* avoid leaks */
	if (write && (count < tgt->block_size)) {
		count = tgt->block_size - count;
		count = copy_out_zero(tgt->hw_state, count);
		tgt->transient_state.copy_count += count;
	}

	return FALSE;
}

int
esp_dma_restart_dataout_1(dma, tgt)
	opaque_t	dma;
	target_info_t	*tgt;
{
	register int    copy_now;
	register char	*dma_ptr;

	reset_fifo(tgt->hw_state);
	set_fifo_write(tgt->hw_state);

	dma_ptr = tgt->dma_ptr + tgt->transient_state.dma_offset;

	copy_now = u_min(tgt->transient_state.out_count, ESP_TC_MAX);

	if (copy_now <= 0) {
		printf("Invalid copy count: 0x%x\n", copy_now);
		gimmeabreak();
	}

	copy_now = copy_out(tgt->hw_state, dma_ptr, copy_now);

	tgt->transient_state.copy_count += copy_now;

	return tgt->transient_state.out_count;
}

int
esp_dma_restart_dataout_2(dma, tgt, xferred)
	opaque_t	dma;
	register target_info_t	*tgt;
	int		xferred;
{
	register int    should_copy, copied, copy_now;
	register char	*dma_ptr;

	should_copy = u_min(tgt->transient_state.out_count, ESP_TC_MAX);
	copied = tgt->transient_state.copy_count;
	copy_now = should_copy - copied;

	if (copy_now <= 0) {
		printf("Invalid copy count: 0x%x\n", copy_now);
		gimmeabreak();
	}

	dma_ptr = tgt->dma_ptr + tgt->transient_state.dma_offset + copied;

	copy_now = copy_out(tgt->hw_state, dma_ptr, copy_now);

	tgt->transient_state.copy_count += copy_now;

	return tgt->transient_state.copy_count;
}

int
esp_dma_restart_dataout_3(dma, tgt, regp)
	opaque_t	dma;
	target_info_t	*tgt;
	volatile char	*regp;
{
	return 0;
}

void
esp_dma_restart_dataout_4(dma, tgt)
	opaque_t	dma;
	target_info_t	*tgt;
{
	/* Nothing needed here */
}

int
esp_dma_start_msgin(dma, tgt)
	opaque_t	dma;
	target_info_t	*tgt;
{
	return 0;
}

void
esp_dma_end_msgin(dma, tgt)
	opaque_t	dma;
	target_info_t	*tgt;
{
	/* Nothing needed here */
}

boolean_t
esp_dma_disconn_1(dma, tgt, xferred)
	opaque_t	dma;
	target_info_t	*tgt;
	int		xferred;
{
	tgt->transient_state.dma_offset += xferred;
	tgt->transient_state.copy_count = 0;

	return FALSE;
}

boolean_t
esp_dma_disconn_2(dma, tgt)
	opaque_t	dma;
	target_info_t	*tgt;
{
	/* Nothing needed */
	return FALSE;
}

boolean_t
esp_dma_disconn_3(dma, tgt, xferred)
	opaque_t	dma;
	target_info_t	*tgt;
	int		xferred;
{
	register char	*dma_ptr;

	/* keep dma_offset anyways */
	dma_ptr = tgt->dma_ptr + tgt->transient_state.dma_offset;

	xferred = copy_in(tgt->hw_state, dma_ptr, xferred);

	tgt->transient_state.dma_offset += xferred;
	tgt->transient_state.copy_count = 0;

	return FALSE;
}

#define	esp_dma_disconn_4		esp_dma_disconn_3
#define	esp_dma_disconn_5		esp_dma_disconn_1
#define	esp_dma_disconn_callback	0	/* never called */

scsi_dma_ops_t esp_dma_ops = {
	esp_dma_init,
	esp_dma_new_target,
	esp_dma_map,
	esp_dma_start_cmd,
	esp_dma_end_xfer,
	esp_dma_end_cmd,
	esp_dma_start_datain,
	esp_dma_start_msgin,
	esp_dma_end_msgin,
	esp_dma_start_dataout,
	esp_dma_restart_datain_1,
	esp_dma_restart_datain_2,
	esp_dma_restart_datain_3,
	esp_dma_restart_dataout_1,
	esp_dma_restart_dataout_2,
	esp_dma_restart_dataout_3,
	esp_dma_restart_dataout_4,
	esp_dma_disconn_1,
	esp_dma_disconn_2,
	esp_dma_disconn_3,
	esp_dma_disconn_4,
	esp_dma_disconn_5,
	esp_dma_disconn_callback
};
