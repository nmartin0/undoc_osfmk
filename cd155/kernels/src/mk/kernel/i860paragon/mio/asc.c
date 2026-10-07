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
 * Copyright 1992 by Intel Corporation,
 * Santa Clara, California.
 * 
 *			  All Rights Reserved
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
 * $Log: asc.c,v $
 * Revision 0.12  1995/03/14  23:43:12  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 0.11  1994/11/18  20:44:58  mtm
 * Copyright additions/changes
 *
 * Revision 0.10  1994/08/31  21:24:04  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 0.7.2.2  1994/08/26  17:14:19  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: 10680
 *  Testing: Tape EATs, Tensors tapewrt/taperead, tar
 *  Module(s): mio_dma_restart_dataout_1, mio_dma_restart_dataout_2,
 *                io_dma_restart_dataout_3,
 *                mio_dma_disconn_1, mio_dma_disconn_2
 *
 * Revision 0.7.2.1  1994/08/04  16:06:32  richardg
 *  Reviewer: Jerry Coffman
 *  Risk: Medium
 *  Benefit or PTS #: 10171
 *  Testing: ran Tensor's tapewrt program simultaneously against both LUNs.
 *  Module(s): mio_dma_restart_dataout_1, mio_dma_restart_dataout_2
 *
 * Revision 0.7  1994/06/29  15:55:14  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *	     PTS 10033.
 *
 * Revision 0.6  1993/09/16  18:11:33  richardg
 * modified the memory allocation to evenly divide available MIO memory between existing scsi devices
 *
 * Revision 0.5  1993/06/30  22:39:28  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.4  1993/06/11  00:07:30  jerrie
 * Modify mio_dma_map to initialize tgt->transient_state.out_count with the
 * minimum of max_dma_data or ior->io_count.  Writes greater than 64k bytes
 * were causing SCSI bus resets to occur when simply setting out_count to
 * io_count.
 *
 * Revision 0.3  1993/05/25  19:07:13  rkl
 * Added the initialization of the ASTG disk sort algorithm variables
 * 'sawtooth' and 'optimize_read' through bootmagic.  The default action
 * is to use the standard OSF disk sort algorithm.  Setting
 * DISKSORT_SAWTOOTH=0 will cause disk requests which result in the least
 * amount of arm movement (regardless of the direction of the movment) to
 * be done first.  Setting DISKSORT_OPTIMIZE_READ=1 will put reads ahead of
 * writes.
 *
 * Revision 0.2  1992/08/31  10:47:12  andyp
 * SCSI mods from Jerrie.
 *
 * Revision 0.1  92/07/09  16:57:48  andyp
 * Initial checkin of Paragon files.
 * 
 * 
 */
/*
 *	File: asc.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	05/92
 *
 *	DMA interface functions for the Advanced SCSI Controller
 */

#include <asc.h>
#if	NASC > 0

#define NDMA NASC

#include <scsi/compat_30.h>
#include <scsi/scsi.h>
#include <scsi/scsi_defs.h>
#include <scsi/adapters/scsi_dma.h>

#include <sys/types.h>
#include <chips/busses.h>
#include <i860paragon/mio/mio.h>
#include <i860paragon/mio/miodev.h>

#ifdef PARAGON860	/* performance mod */
#include <scsi/rz.h>
#include <scsi/adapters/scsi_53C94.h>
#endif

extern void		outw();
extern unsigned short	inw();

extern scsi_dma_ops_t	mio_dma_ops;	/* at end */

caddr_t
init_asc_probe(vaddr, ctlr)
	caddr_t			vaddr;
	struct bus_ctlr		*ctlr;
{
	caddr_t virt;

	/*
	 * See if the GP node has an MIO card attached with a SCSI controller.
	 * mio_map_memory() returns the starting virtual address for the SCSI
	 * controller registers if it's there, NULL otherwise.
	 */
	virt = (caddr_t)mio_map_memory(ctlr->phys_address, ctlr->sysdep);
	if (virt == NULL) return NULL;

	/* Initialize controller 0's dma operations pointer */
	asc_set_dmaops(0, &mio_dma_ops);

	/* Allow the MIO to generate SCSI interrupts */
	mio_interrupt_enable(MIO_INTENABLE_SCSI);

	/* set disksort behavior */
	{
		extern	int	getbootint();
		extern	int	sawtooth, optimize_read;

		/*
		 *  DISKSORT_SAWTOOTH:
		 *
		 *  Effects disk arm movement algorithm.  Default operation
		 *  (sawtooth = 1) keeps arm moving in one direction until
		 *  there are no more requests in that direction.  This
		 *  the original OSF supplied algorithm.  When sawtooth = 0,
		 *  the arm will take the request that moves the arm the
		 *  least amount regardless of which direction that would be.
		 */
		sawtooth = getbootint("DISKSORT_SAWTOOTH", 1);

		/*
		 *  DISKSORT_OPTIMIZE_READ:
		 *
		 *  When optimize_read = 1, reads are placed ahead of writes
		 *  in the disk queue.  The default behavior does not favor
		 *  reads ahead of writes and is the original OSF supplied
		 *  algorithm.  The variable "sawtooth" must be set to 0 for
		 *  "optimize_read" to have any effect.
		 */
		optimize_read = getbootint("DISKSORT_OPTIMIZE_READ", 0);
	}
	return virt;
}

#define MIO_DMA_DIR_BIT		3		/* DMA direction bit */

#define MIO_DMA_READ		(1 << MIO_DMA_DIR_BIT)
#define MIO_DMA_WRITE		(0 << MIO_DMA_DIR_BIT)

#define MIO_DMA_MASK		0x3ffff		/* 18 bits */
#define MIO_DMA_ADDR(x)		(((unsigned)(x)) & MIO_DMA_MASK)

#define ASC_DMA_LO		(MIO_DMA_LO)
#define ASC_DMA_HI		(MIO_DMA_HI)

#define ASC_RAM			(mio_dinfo[MIO_DEV_SRAM]->address)
#define ASC_RAM_SIZE		(MIO_SCSI_RAM_SIZE)

struct dma_softc {
	int		dev_unit;
	volatile short	*dma_lo;	/* DMA counter (bits 0 - 15) */
	volatile short	*dma_hi;	/* DMA counter (bits 16, 17) */
	volatile char	*buff;		/* DMA memory (I/O space) */
	unsigned int	callback_arg1,
			callback_arg2,
			callback_arg3;
} mio_softc_data[NDMA];

typedef struct dma_softc *dma_softc_t;

dma_softc_t	mio_softc[NDMA];

#define MIO_DMA_GET(dma, adr)			\
	adr =					\
	  MIO_DMA_ADDR((inw((dma)->dma_hi) << 16) | inw((dma)->dma_lo)) << 1

#define MIO_DMA_PUT(dma, adr, dir)		\
	(adr) = MIO_DMA_ADDR((adr) >> 1);	\
	outw((dma)->dma_lo, (adr));		\
	outw((dma)->dma_hi, ((adr) >> 16) | dir)

/*
 * Statically partition the DMA buffer between targets.
 * This way we will eventually be able to attach/detach
 * drives on-fly.  And 64k/target is plenty for normal use.
 */
#define PER_TGT_DMA_SIZE	((ASC_RAM_SIZE / 7) & ~(sizeof(short) - 1))
#define	read_base(tgt)		(tgt->dma_ptr)
#define write_base(tgt)		(tgt->dma_ptr)

/*
 * Round to 4k (should be a multiple of 4k anyway)
 */
#ifdef PARAGON860	/* performance mod */
#define PER_TGT_BUFF_SIZE	(tgt->max_dma_data)
#else
#define	PER_TGT_BUFF_SIZE	((PER_TGT_DMA_SIZE >> 12) << 12)
#endif

/*
 * Statically allocated command & temp buffers
 * This way we can attach/detach drives on-fly
 */
#define PER_TGT_BUFF_DATA	256
static unsigned char	asc_buffer[NASC * (MAX_SCSI_TARGETS * MAX_LUNS) * PER_TGT_BUFF_DATA];

#define	u_min(a, b)	(((a) < (b)) ? (a) : (b))

void
mio_dma_rw(dma, dma_ptr, dir)
	dma_softc_t	dma;
	vm_offset_t	dma_ptr;
	short		dir;
{
	register vm_offset_t	phys;

	assert(((int)dma_ptr & 1) == 0);

	/*
	 * Get physical address of dma_ptr
	 */
	phys = kvtophys(dma_ptr);

	/*
	 * Load DMA registers with physical
	 * address and direction of transfer
	 */
	MIO_DMA_PUT(dma, phys, dir);
}

/*
 * Cold, probe time init
 */
opaque_t
mio_dma_init(controller, base, dma_bsizep)
	int		controller;
	vm_offset_t	base;
	int		*dma_bsizep;
{
	static int	unit = 0;
	dma_softc_t	dma;

	dma = mio_softc[unit] = &mio_softc_data[unit];

	dma->dev_unit = unit++;

	/*
	 * Initialize physical addresses for MIO board DMA registers
	 */
	dma->dma_lo   = (volatile short *)(ASC_DMA_LO);	
	dma->dma_hi   = (volatile short *)(ASC_DMA_HI);	

	/*
	 * Initialize virtual address of static RAM bottom
	 */
	dma->buff     = (volatile  char *)(ASC_RAM);

	/*
	 * Clear out dma buffer
	 */
	miozero(dma->buff, ASC_RAM_SIZE);

	/*
	 * Use special DMA mode - four word (eight byte) threshold
	 */
	*dma_bsizep = 4 * 16;

	return (opaque_t)dma;
}

/*
 * A target exists
 */
void
#ifdef PARAGON860	/* performance mod */
mio_dma_new_target(dma, tgt, ntargets)
	dma_softc_t	dma;
	target_info_t	*tgt;
	unsigned char ntargets;
#else
mio_dma_new_target(dma, tgt)
	dma_softc_t	dma;
	target_info_t	*tgt;
#endif
{
#ifdef PARAGON860	/* performance mod */
	static int targets = 0;
#endif
	int i;
	target_info_t	*ptgt;

	/* desc was allocated afresh */
	tgt->cmd_ptr = &asc_buffer[tgt->target_id * PER_TGT_BUFF_DATA];
#ifdef PARAGON860	/* performance mod */
	/* if ntargets = 7, use 64k as the buffer size. */
	if (ntargets == 7) {
	  tgt->max_dma_data = ASC_TC_MAX;
	  tgt->dma_ptr = (char *)dma->buff + tgt->target_id * tgt->max_dma_data; 
	}
	else { /* else divide the memory equally between targets */
	  tgt->max_dma_data = ((ASC_RAM_SIZE/ntargets) & ~(sizeof(short) - 1));
	  tgt->max_dma_data -= tgt->max_dma_data % RZ_DEFAULT_BSIZE;
	  tgt->dma_ptr = (char *)dma->buff + targets * tgt->max_dma_data; 
	  ++targets;
	}
	/* one dma area per target, so point all the LUNS in a target to the
	   same one. */
	for (i = 1; i < MAX_LUNS; i++) { 
	   ptgt = scsi_softc[tgt->masterno]->target[tgt->target_id][i];
	   ptgt->dma_ptr = tgt->dma_ptr;
	   ptgt->max_dma_data = tgt->max_dma_data;
	   ptgt->cmd_ptr = &asc_buffer[(ptgt->target_id + ptgt->true_lun) * PER_TGT_BUFF_DATA];
	}
#else
	tgt->dma_ptr = (char *)dma->buff + tgt->target_id * PER_TGT_BUFF_SIZE;
#endif
}

/*
 * Map, if necessary, user virtual addresses into
 * physical ones (or kernel virtuals if no true DMA)
 */
void
mio_dma_map(dma, tgt)
	dma_softc_t	dma;
	target_info_t	*tgt;
{
	/*
	 * We cannot do real DMA
	 */

	if ((tgt->cur_cmd == SCSI_CMD_WRITE) ||
	    (tgt->cur_cmd == SCSI_CMD_LONG_WRITE)) {
		io_req_t	ior = tgt->ior;
		unsigned int	max_dma;
		register int	count;

#ifdef PARAGON860	/* performance mod */
		max_dma = tgt->max_dma_data;
#else
		max_dma = scsi_softc[tgt->masterno]->max_dma_data;
#endif
		count = ior->io_count;
		tgt->transient_state.out_count = u_min(count, max_dma);
	} else {
		tgt->transient_state.out_count  = 0;
	}

	tgt->transient_state.copy_count = 0;
	tgt->transient_state.dma_offset = 0;
}

int
mio_dma_start_cmd(dma, tgt)
	dma_softc_t	dma;
	register target_info_t	*tgt;
{
	register char	*dma_ptr;
	register int	out_count;

	dma_ptr = write_base(tgt);

	/*
	 * Note that the out_count does not include the
	 * (optional) identify message but does include
	 * both the data and cmd out count.
	 */
	out_count = tgt->transient_state.cmd_count;
	miocopy(tgt->cmd_ptr, dma_ptr, out_count);

	/* loadup DMA engine with phys of write_base(tgt) */
	mio_dma_rw(dma, dma_ptr, MIO_DMA_WRITE);

	return out_count;
}

void
mio_dma_end_xfer(dma, tgt, bytes_read)
	dma_softc_t	dma;
	target_info_t	*tgt;
	int		bytes_read;
{
	unsigned char	*to;

	/* nothing needed on writes */
	if (bytes_read == 0) return;

	if ((tgt->cur_cmd != SCSI_CMD_READ) &&
	    (tgt->cur_cmd != SCSI_CMD_LONG_READ)) {

		/* hba-indep code expects it there */
		to = tgt->cmd_ptr;
	} else {
		register io_req_t	ior = tgt->ior;

		assert(ior != 0);
		assert(ior->io_op & IO_READ);

		to = (unsigned char *)
			(ior->io_data + tgt->transient_state.copy_count);
	}

	miocopy(read_base(tgt) + tgt->transient_state.dma_offset,
		to, bytes_read);
}

void
mio_dma_end_cmd(dma, tgt, ior)
	dma_softc_t	dma;
	target_info_t	*tgt;
	io_req_t	ior;
{
}

int
mio_dma_start_datain(dma, tgt)
	dma_softc_t	dma;
	register target_info_t	*tgt;
{
	register char	*dma_ptr;
	register int	count;

	dma_ptr = read_base(tgt);
	mio_dma_rw(dma, dma_ptr, MIO_DMA_READ);

	count = tgt->transient_state.in_count;
	count = u_min(count, PER_TGT_BUFF_SIZE);

	return count;
}

int
mio_dma_restart_datain_1(dma, tgt)
	dma_softc_t	dma;
	target_info_t	*tgt;
{
	register char	*dma_ptr;
	register int	count;

	dma_ptr = read_base(tgt);
	mio_dma_rw(dma, dma_ptr, MIO_DMA_READ);

	count = tgt->transient_state.in_count;
	count = u_min(count, PER_TGT_BUFF_SIZE);

	return count;
}

int
mio_dma_restart_datain_2(dma, tgt, xferred)
	dma_softc_t	dma;
	target_info_t	*tgt;
	int		xferred;
{
	register char		*dma_ptr;
	register unsigned char	*to;
	register int		count;

	if ((tgt->cur_cmd != SCSI_CMD_READ) &&
	    (tgt->cur_cmd != SCSI_CMD_LONG_READ)) {

		/* hba-indep code expects it there */
		to = tgt->cmd_ptr;
	} else {
		register io_req_t	ior = tgt->ior;

		assert(ior != 0);
		assert(ior->io_op & IO_READ);

		to = (unsigned char *)
			(ior->io_data + tgt->transient_state.copy_count);
	}

	dma_ptr = read_base(tgt) + tgt->transient_state.dma_offset;

	/* copy what we got */
	miocopy(dma_ptr, to, xferred);

	tgt->transient_state.dma_offset  = 0;
	tgt->transient_state.copy_count += xferred;

	/* figure out how much should be left */
	count = tgt->transient_state.in_count;
	count -= xferred;
	count = u_min(count, PER_TGT_BUFF_SIZE);

	/* get some more */
	dma_ptr = read_base(tgt);
	mio_dma_rw(dma, dma_ptr, MIO_DMA_READ);

	return count;
}

void
mio_dma_restart_datain_3(dma, tgt)
	dma_softc_t	dma;
	target_info_t	*tgt;
{
	/* Nothing needed here */
}

boolean_t
mio_dma_start_dataout(dma, tgt, regp, cmd)
	dma_softc_t	dma;
	register target_info_t	*tgt;
	volatile char	*regp, cmd;
{
	register char		*dma_ptr;
	register unsigned char	*from;
	register int		count;

	dma_ptr = write_base(tgt);
	count = tgt->transient_state.out_count;
	count = u_min(count, PER_TGT_BUFF_SIZE);

	if ((tgt->cur_cmd == SCSI_CMD_WRITE) ||
	    (tgt->cur_cmd == SCSI_CMD_LONG_WRITE)) {
		io_req_t	ior = tgt->ior;

		from = (unsigned char *)ior->io_data;

		/* avoid leaks */
		if (count < tgt->block_size) {
			miozero(dma_ptr + count, tgt->block_size - count);
			tgt->transient_state.out_count = tgt->block_size;
		}
	} else {
		from = tgt->cmd_ptr + tgt->transient_state.cmd_count;
	}

	tgt->transient_state.copy_count = count;
	miocopy(from, dma_ptr, count);

	mio_dma_rw(dma, dma_ptr, MIO_DMA_WRITE);

	return TRUE;
}

int
mio_dma_restart_dataout_1(dma, tgt) /* cow added */
	dma_softc_t	dma;
	target_info_t	*tgt;
{
	register int		count;
	register char		*dma_ptr;
	register unsigned char	*from;
	io_req_t		ior = tgt->ior;

	dma_ptr = write_base(tgt);
	count = tgt->transient_state.out_count;
	count = u_min(count, PER_TGT_BUFF_SIZE);
	assert(count > 0);

	if ((tgt->cur_cmd == SCSI_CMD_WRITE) ||
	    (tgt->cur_cmd == SCSI_CMD_LONG_WRITE)) {
		from = (unsigned char *)
			(ior->io_data + tgt->transient_state.copy_count);

		/* avoid leaks */
		if (count < tgt->block_size) {
			miozero(dma_ptr + count, tgt->block_size - count);
			tgt->transient_state.out_count = tgt->block_size;
		}

	} else {
		from = tgt->cmd_ptr + tgt->transient_state.cmd_count;
	}

	miocopy(from, dma_ptr, count);

	return count;
}

int
mio_dma_restart_dataout_2(dma, tgt, xferred) /* cow added */
	dma_softc_t	dma;
	register target_info_t	*tgt;
	int		xferred;
{
	register int	count;
	register char	*dma_ptr;
	register char	*from;
	io_req_t	ior = tgt->ior;

	dma_ptr = write_base(tgt);
	tgt->transient_state.copy_count += xferred;

	from = ior->io_data + tgt->transient_state.copy_count;

	/* figure out how much should be left */
	count = tgt->transient_state.out_count;
	count = u_min(count, PER_TGT_BUFF_SIZE);

	/* avoid leaks */
	if (count < tgt->block_size) {
		miozero(dma_ptr + count, tgt->block_size - count);
		tgt->transient_state.out_count = tgt->block_size;
	}

	miocopy(from, dma_ptr, count);

	return count;
}

int
mio_dma_restart_dataout_3(dma, tgt, regp)
	dma_softc_t	dma;
	target_info_t	*tgt;
	volatile char	*regp;
{
	register char	*dma_ptr;
	int		extra;

	/* ship some more */
	dma_ptr = write_base(tgt);

	/*
	 * See NCR spec, Page 12 about 'preloading the FIFO'
	 */
	if ((int)dma_ptr & 1) {
		*regp = *dma_ptr++;
		extra = 1;
	} else
		extra = 0;

	mio_dma_rw(dma, dma_ptr, MIO_DMA_WRITE);

	return extra;
}

void
mio_dma_restart_dataout_4(dma, tgt)
	dma_softc_t	dma;
	target_info_t	*tgt;
{
	/* Nothing needed here */
}

int
mio_dma_start_msgin(dma, tgt)
	dma_softc_t	dma;
	target_info_t	*tgt;
{
	return 0;
}

void
mio_dma_end_msgin()
{
	/* nothing needed */
}

boolean_t
mio_dma_disconn_1(dma, tgt, xferred)
	dma_softc_t	dma;
	target_info_t	*tgt;
	int		xferred;
{
/* 	tgt->transient_state.dma_offset += xferred; */
	tgt->transient_state.copy_count += xferred;

	return FALSE;
}

boolean_t
mio_dma_disconn_2(dma, tgt)
	dma_softc_t	dma;
	target_info_t	*tgt;
{
	register unsigned char	*from;
	register char		*to;
	register int		count;

/* never do this */
	return FALSE;
	/*
	 * Postponed initial write
	 */

	to = write_base(tgt);
	count = tgt->transient_state.out_count;
	count = u_min(count, PER_TGT_BUFF_SIZE);

	if ((tgt->cur_cmd == SCSI_CMD_WRITE) ||
	    (tgt->cur_cmd == SCSI_CMD_LONG_WRITE)) {
		io_req_t	ior = tgt->ior;

		from = (unsigned char *)ior->io_data;

		/* avoid leaks */
		if (count < tgt->block_size) {
			miozero(to + count, tgt->block_size - count);
			tgt->transient_state.out_count = tgt->block_size;
		}
	} else {
		from = tgt->cmd_ptr + tgt->transient_state.cmd_count;
	}

	tgt->transient_state.copy_count = count;

	dma->callback_arg1 = (unsigned int)from;
	dma->callback_arg2 = (unsigned int)to;
	dma->callback_arg3 = (unsigned int)count;

	return TRUE;
}

boolean_t
mio_dma_disconn_3(dma, tgt, xferred)
	dma_softc_t	dma;
	target_info_t	*tgt;
	int		xferred;
{
	register int		offset;
	register unsigned char	*to;

	offset = tgt->transient_state.dma_offset;

	if ((tgt->cur_cmd != SCSI_CMD_READ) &&
	    (tgt->cur_cmd != SCSI_CMD_LONG_READ)) {

		/* hba-indep code expects it there */
		to = tgt->cmd_ptr;
	} else {
		register io_req_t	ior = tgt->ior;

		assert(ior != 0);
		assert(ior->io_op & IO_READ);

		to = (unsigned char *)
			(ior->io_data + tgt->transient_state.copy_count);
	}

	dma->callback_arg1 = (unsigned int)read_base(tgt) + offset;
	dma->callback_arg2 = (unsigned int)to;
	dma->callback_arg3 = (unsigned int)xferred;

	tgt->transient_state.dma_offset  = 0;
	tgt->transient_state.copy_count += xferred;

	return TRUE;
}

#define mio_dma_disconn_4		mio_dma_disconn_3
#define mio_dma_disconn_5		mio_dma_disconn_1

void
mio_dma_disconn_callback(dma)
	dma_softc_t	dma;
/*	target_info_t	*tgt; */
{
	/* copy what we got */
	miocopy(dma->callback_arg1, dma->callback_arg2, dma->callback_arg3);
}

scsi_dma_ops_t mio_dma_ops = {
	mio_dma_init,
	mio_dma_new_target,
	mio_dma_map,
	mio_dma_start_cmd,
	mio_dma_end_xfer,
	mio_dma_end_cmd,
	mio_dma_start_datain,
	mio_dma_start_msgin,
	mio_dma_end_msgin,
	mio_dma_start_dataout,
	mio_dma_restart_datain_1,
	mio_dma_restart_datain_2,
	mio_dma_restart_datain_3,
	mio_dma_restart_dataout_1,
	mio_dma_restart_dataout_2,
	mio_dma_restart_dataout_3,
	mio_dma_restart_dataout_4,
	mio_dma_disconn_1,
	mio_dma_disconn_2,
	mio_dma_disconn_3,
	mio_dma_disconn_4,
	mio_dma_disconn_5,
	mio_dma_disconn_callback
};

#endif	NASC > 0
