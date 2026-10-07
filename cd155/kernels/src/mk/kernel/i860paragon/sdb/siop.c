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
 * $Log: siop.c,v $
 * Revision 1.3.8.1  1995/06/11  18:33:48  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.3  1995/03/28  01:15:59  jerrie
 * Changed conditional compile of DMA to a check of sdb_configuration.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low.
 *  Benefit or PTS #: 12827
 *  Testing:	   Tested bootmagic by resetting with various settings.
 *  Module(s):	   See PTS report.
 *
 * Revision 1.2  1995/03/14  23:44:00  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	siop.c
 *	Author:	Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	4/94
 *
 *	Interface functions for the SCSI I/O Processor
 */


#include <siop.h>
#if	NSIOP > 0

#if	NSIOP > 2
This module supports a maximum of two SCSI I/O Processors
#endif

#include <sys/types.h>
#include <chips/busses.h>

#include <scsi/compat_30.h>
#include <scsi/scsi.h>
#include <scsi/scsi_defs.h>

#include <i860paragon/expansion.h>

#include <i860paragon/sdb/sdb.h>
#include <i860paragon/sdb/sdb_dev.h>
#include <i860paragon/sdb/sdb_flash.h>
#include <i860paragon/sdb/siop_script.h>
#include <i860paragon/sdb/script/siop_ent.h>

#include <scsi/adapters/scsi_53C720.h>

extern void		outw();
extern void		outb();
extern unsigned short	inw();
extern unsigned char	inb();

#define SIOP_SRAM		(SIOPS_START_SRAM)
#define SIOP_SRAM_SIZE		(SIOPS_SIZEOF_SRAM  / NSIOP)

#define SIOP_DPRAM		(SIOPS_START_DPRAM)
#define SIOP_DPRAM_SIZE		(SIOPS_SIZEOF_DPRAM / NSIOP)

#define SIOP_FLASH		(SDB_FLASH_ADDR)
#define SIOP_FLASH_SIZE		(SDB_SIZEOF_FLASH)

/*
 * As described in the MOVE MEMORY instruction on page 3-34 of the
 * NCR 53C8XX Family Programming Guide, if cache line burst mode is
 * enabled, the lower four bits of the source and destination must be
 * identical.  An illegal instruction results if the two addresses are
 * not on the same byte offset within a given longword.  Align request
 * tables to a 16-byte boundary.
 */
#define SIOP_BURST_ALIGN	16
#define SIOP_BURST_MASK		(SIOP_BURST_ALIGN - 1)


/*
 * Map in SIOP registers, enable chip interrupts, and set disksort behavior
 */
caddr_t
siop_reg_ptr(ctlr)
	struct bus_ctlr	*ctlr;
{
	int	unit = ctlr->unit;
	caddr_t	virt;

	/*
	 * See if the node has a SCSI-16 daughter board attached
	 * and the SIOP and grand-daughter card are installed.
	 */
	if (!siop_installed(unit))
		return NULL;

	/*
	 * See if the node has a SCSI-16 daughter board attached.
	 * sdb_map_memory() returns the starting virtual address
	 * of the SIOP registers if the board is there, NULL otherwise.
	 */
	virt = (caddr_t)sdb_map_memory(ctlr->phys_address, ctlr->sysdep);
	if (virt == NULL) return NULL;

	if (sdb_configuration & SDB_CONFIGURATION_DISABLE_DMA) {
		/*
		 * Allow the SCSI-16 daughter board to generate SIOP interrupts
		 */
		sdb_interrupt_enable((unit == 0) ?
			SDB_IMASK_SCSI0 : SDB_IMASK_SCSI1);
	} else {
		/*
		 * Allow the SCSI-16 daughter board to generate DMA interrupts
		 */
		sdb_interrupt_enable((unit == 0) ?
			SDB_IMASK_DMA0 : SDB_IMASK_DMA1);
	}

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


/*
 * Return virtual pointers to controller shared data, reselect
 * table, request table, and the request table size in bytes.
 */
siop_map(ctlr, shared_p, reselect_p, request_p)
	struct bus_ctlr	*ctlr;
	caddr_t		*shared_p;
	caddr_t		*reselect_p;
	caddr_t		*request_p;
{
	/*
	 * Initialize table pointers with virtual addresses
	 * Given that NSIOP is two, memory is divided as follows:
	 *
	 *						   SRAM
	 *					+------------------------+
	 *					|                        |
	 *					| Request Table  (472KB) |
	 *		    DPRAM		|                        |
	 *	+--------------------------+	+------------------------+
	 *	| H/W DMA FIFOs     (6KB)  |	| siop1 Reselect (16KB)  |
	 *	+--------------------------+	+------------------------+
	 *	| Enet Controller   (10KB) |	| siop0 Reselect (16KB)  |
	 *	+--------------------------+	+------------------------+
	 *	| siop1 Shared Data (8KB)  |	| siop1 SCRIPT   (4KB)   |
	 *	+--------------------------+	+------------------------+
	 *	| siop0 Shared Data (8KB)  |	| siop0 SCRIPT   (4KB)   |
	 *	+--------------------------+	+------------------------+
	 *
	 * NOTE:
	 *	The request table area must be aligned on a 16 byte
	 *	boundary to allow the SIOP's to perform cache line bursts
	 */

	assert(sizeof(struct cntrlr_shrd_data) <= SCRIPT_SHARED_DATA_SIZE);
	*shared_p = 
		(caddr_t)(SIOP_DPRAM + (SCRIPT_SHARED_DATA_SIZE * ctlr->unit));

	*reselect_p = (caddr_t)(SIOP_SRAM +
		(SCRIPT_SIZE * NSIOP) + (RESELECT_TABLE_SIZE * ctlr->unit));

	*request_p = (caddr_t)(SIOP_SRAM +
		(SCRIPT_SIZE * NSIOP) + (RESELECT_TABLE_SIZE * NSIOP));
	/* should already be aligned */
	assert(((unsigned long)(*request_p) & SIOP_BURST_MASK) == 0);
	/* align up to 32 byte boundary */
	*request_p = (caddr_t)(((unsigned long)(*request_p) + SIOP_BURST_MASK) &
		~SIOP_BURST_MASK);
}


/*
 * Determine if the node has a SCSI-16 daughter board attached
 * and the SIOP and grand-daughter card are installed.
 */
boolean_t
siop_installed(unit)
{
	extern int	expansion_id();
	int		siop;

	if ((unit != 0) && (unit != 1)) {
		printf("siop_installed: bad unit number (unit == %d)\n", unit);
		return FALSE;
	}

	/* check for a SCSI-16 expansion board type */
	if (expansion_id() != EXP_ID_SCSI)
		return FALSE;

	siop = (unit == 0) ?
		SDB_STATUS_SCSI0_INSTALLED : SDB_STATUS_SCSI1_INSTALLED;

	return (inw(SDB_STATUS) & siop) ? TRUE : FALSE;
}


/*
 * Perform a hard reset of the SCSI processor
 */
void
siop_reset_chip(unit)
	int	unit;
{
	int	s, siop;

	if ((unit != 0) && (unit != 1)) {
		printf("siop_reset_chip: bad unit number (unit == %d)\n", unit);
		return;
	}
	siop = (unit == 0) ? SDB_CONTROL_SCSI0_RESET : SDB_CONTROL_SCSI1_RESET;

	s = sploff();
	outw(SDB_CONTROL, inw(SDB_CONTROL) |  siop);
	delay(10);
	outw(SDB_CONTROL, inw(SDB_CONTROL) & ~siop);
	splon(s);
}


/*
 * Determine the SCSI chip clock frequency
 */
int
siop_clock(unit)
	int	unit;
{
	extern int	expansion_id();
	int		sdb_revision;

	sdb_revision = inb(EXP_ID_ADDR) & 0x00000007;

	switch (sdb_revision) {

		/*
		 * Hard coded to default for now, but we may need to return
		 * a different chip clock frequency value based on the SCSI
		 * Daughter Board revision level
		 */
		default:
			return 50;	/* SCSI chip clock frequency in MHz */
	}
	/*NOTREACHED*/
}


/*
 * Determine if the SCSI bus is in differential mode
 */
boolean_t
siop_diff(unit)
	int	unit;
{
	int	siop;

	if ((unit != 0) && (unit != 1)) {
		printf("siop_diff: bad unit number (unit == %d)\n", unit);
		return FALSE;
	}
	siop = (unit == 0) ?
		SDB_STATUS_SCSI0_DIFFERENTIAL : SDB_STATUS_SCSI1_DIFFERENTIAL;

	return (inw(SDB_STATUS) & siop) ? TRUE : FALSE;
}


/*
 * Load NCR 53C720 SCRIPT
 */
unsigned long
siop_script_load(unit)
	int	unit;
{
	extern unsigned long	siop0_script[];
	extern unsigned long	siop1_script[];
	extern unsigned long	siop0_script_size;
	extern unsigned long	siop1_script_size;
	unsigned char		*from;
	volatile unsigned char	*to;
	int			size;

	if (unit == 0) {
		from = (unsigned char *)siop0_script;
		size = siop0_script_size;
	} else if (unit == 1) {
		from = (unsigned char *)siop1_script;
		size = siop1_script_size;
	} else panic("Invalid unit number: %d\n", unit);

	if (size > SCRIPT_SIZE) {
		panic("SIOP Script size (%d) exceeds available memory (%d)\n",
			size, SCRIPT_SIZE);
	}

	to = (volatile unsigned char *)(SIOP_SRAM + (SCRIPT_SIZE * unit));

	sdbzero(to, size);

	sdbcopy(from, to, size);

	return (unsigned long)to;	/* virtual address to base of script */
}


/*
 * Patch NCR 53C720 SCRIPT
 */
void
siop_script_patch(unit)
	int		unit;
{
	extern unsigned long	siop0_labelpatches[];
	extern unsigned long	siop1_labelpatches[];
	extern unsigned long	siop0_patches;
	extern unsigned long	siop1_patches;
	unsigned long		*labelpatches;
	unsigned long		patches;
	unsigned long		*script;
	int			i;

	if (unit == 0) {
		labelpatches = (unsigned long *)siop0_labelpatches;
		patches = siop0_patches;
	} else if (unit == 1) {
		labelpatches = (unsigned long *)siop1_labelpatches;
		patches = siop1_patches;
	} else panic("Invalid unit number: %d\n", unit);

	script = (unsigned long *)(SIOP_SRAM + (SCRIPT_SIZE * unit));

	for (i = 0; i < patches; i++) {
		script[labelpatches[i]] += (unsigned long)kvtophys(script);
	}
}

/*
 * Clear the sticky DMA interrupt bit
 */
void
siop_clear_dma_int(unit)
	int		unit;
{
	if (sdb_configuration & SDB_CONFIGURATION_DISABLE_DMA) {
		return;
	} else {
		outw(SDB_ISTAT, (unit == 0) ? SDB_ISTAT_DMA0 : SDB_ISTAT_DMA1);
	}
}

#endif	NSIOP > 0
