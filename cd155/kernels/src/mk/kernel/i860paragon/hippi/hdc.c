/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1993 by Intel Corporation,
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
 * $Log: hdc.c,v $
 * Revision 1.6  1995/03/02  23:32:38  arlin
 *  Added Multiple Packet per Connection support
 *  and CONTinuation support for multiple I/O
 *  per packet and connection. F/W is R1.4
 *
 *  Reviewer: Jerrie Coffman, Bernie Keany
 *  Risk: medium
 *  Benefit or PTS #: 12411
 *  Testing: HiPPI EATs: Raw, TCP/IP, and IPI-3.
 *     Also developed special applications to test
 *     new MPC and CONT modes.
 *  Module(s):
 *     /i860paragon/hippi/
 *      hctlr.c, hctlr.h, rhippi.h, rhippi.c,
 *      hippi_status.h, hdc.c
 *     /ipi/ipi_misc.c ipi_defs.h, ipi.c,
 *     /device/ds_routines.c
 *
 * Revision 1.5  1995/02/02  21:03:40  jerrie
 * Due to the call overhead of the kernel delay() function, the timeout
 * during the initial probe of an IPI device was longer than the IPI
 * watchdog timeout.  This resulted in several command retries being
 * sent to a dead device.  The probe timeout was lowered below the IPI
 * watchdog timeout.  Also, since the HiPPI channel timeout is longer
 * in a debug kernel, #if MACH_ASSERT's were added to increase the probe
 * and watchdog timeouts.
 *
 *  Reviewer:         Arlin Davis
 *  Risk:             Low.  Only affects the IPI device driver.
 *  Benefit or PTS #: 11821
 *  Testing:          Performed the test sequence outlined in the PTS bug
 *                    report.
 *  Module(s):        The following kernel files are being modified:
 *
 *                    kernel/i860paragon/hippi:       hdc.c
 *                    kernel/ipi:                     ipi_misc.c
 *
 * Revision 1.4  1995/02/02  19:06:21  jerrie
 * Configuration checks during the first open of an IPI device failed due
 * to an I-field conflict with another IPI device, but returned leaving the
 * driver initialized to bypass the configuration checks on subsequent opens.
 *
 * The driver was changed to correct this problem and also to change the
 * policy used to perform I-field checking.  Under the new policy, the driver
 * returns a device busy error if the conflicting slave is open, otherwise
 * it succeeds.  The master device open will always succeed, regardless of
 * any I-field conflicts, to allow the user can correct an I-field conflict.
 * An ioctl call to set the I-field to a new value will fail if the new
 * I-field conflicts with another open device, otherwise it will succeed
 * with a warning message displayed on the I/O node console.
 *
 *  Reviewer:         Arlin Davis
 *  Risk:             Low.  Only affects the IPI device driver.
 *  Benefit or PTS #: 11413
 *  Testing:          Performed the test sequence outlined in the PTS bug
 *                    report.  Ran several test cases opening and closing
 *                    IPI devices with conflicting I-fields, using the master
 *                    device, and using ioctls to set new I-field values.
 *
 *  Module(s):        The following kernel files are being modified:
 *
 *                    kernel/i860paragon/hippi:       hdc.c
 *                    kernel/ipi:                     ipi.c, ipi_defs.h,
 *                                                    ipi_master.c, ipi_misc.c
 *
 * Revision 1.3  1994/11/18  20:43:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/06/22  18:04:19  arlin
 * problem in hdc_src_intr() where requests got processed
 * out of order during async stress testing. next ptr corruption.
 *
 * Revision 1.1  1994/06/08  16:58:45  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	hdc.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Bottom layer of the IPI device driver.
 *	Interface to the HiPPI controller.
 */

#include <hdc.h>
#if	NHDC > 0

#include <sys/types.h>
#include <chips/busses.h>

#include <ipi/ipi_compat.h>
#include <ipi/ipi_endian.h>
#include <ipi/ipi_defs.h>
#include <ipi/ipi-3.h>

#include <i860paragon/hippi/hctlr.h>


/* forward declarations */
boolean_t	gimmeabreak();		/* drop into the debugger */
int		hdc_reset();
boolean_t	hdc_probe_target();
io_return_t	hdc_filter();
int		hdc_src_intr();
int		hdc_dst_intr();


/*
 * HiPPI Source channel control bits
 */
#define	HDC_SRC_LAST			SRC_CTL_LAST
#define	HDC_SRC_NO_DISCONNECT		SRC_CTL_NDSC
#define	HDC_SRC_CACHE_COHERENT		SRC_CTL_CACHE

/*
 * HiPPI Source channel status bits
 */
#define HDC_SRC_NO_INTERCONNECT		SRC_ERR_INTC
#define HDC_SRC_SEQUENCE_ERROR		SRC_ERR_SEQS
#define HDC_SRC_DST_SEQUENCE_ERROR	SRC_ERR_SEQD
#define HDC_SRC_PARITY_ERROR		SRC_ERR_PARI
#define HDC_SRC_TIMEOUT			SRC_ERR_TOWR
#define HDC_SRC_ODD_XFER		SRC_ERR_ALGN
#define HDC_SRC_SHORT_XFER		SRC_ERR_SWRD
#define HDC_SRC_CONTROLLER_RESET	SRC_ERR_RESET

#define HDC_SRC_STATUS_MASK	( \
					HDC_SRC_NO_INTERCONNECT    | \
					HDC_SRC_SEQUENCE_ERROR	   | \
					HDC_SRC_DST_SEQUENCE_ERROR | \
					HDC_SRC_PARITY_ERROR	   | \
					HDC_SRC_TIMEOUT		   | \
					HDC_SRC_ODD_XFER	   | \
					HDC_SRC_SHORT_XFER	   | \
					HDC_SRC_CONTROLLER_RESET     \
				)

/*
 * HiPPI Destination channel status bits
 */
#define HDC_DST_NO_INTERCONNECT		DST_ERR_INTC
#define HDC_DST_SYNC_ERROR		DST_ERR_SEQS
#define HDC_DST_SEQUENCE_ERROR		DST_ERR_SEQD
#define HDC_DST_PARITY_ERROR		DST_ERR_PARI
#define HDC_DST_TIMEOUT			DST_ERR_TOWB
#define HDC_DST_LLRC_ERROR		DST_ERR_RLLE
#define HDC_DST_NO_BUFFERS		DST_ERR_EXBF
#define HDC_DST_PACKET_ABORTED		DST_ERR_ABRT
#define HDC_DST_MISALIGNED_XFER		DST_ERR_ALGN

#define HDC_DST_STATUS_MASK	( \
					HDC_DST_NO_INTERCONNECT	   | \
					HDC_DST_SYNC_ERROR	   | \
					HDC_DST_SEQUENCE_ERROR	   | \
					HDC_DST_PARITY_ERROR	   | \
					HDC_DST_TIMEOUT		   | \
					HDC_DST_LLRC_ERROR	   | \
					HDC_DST_NO_BUFFERS	   | \
					HDC_DST_PACKET_ABORTED	   | \
					HDC_DST_MISALIGNED_XFER	     \
				)



hctlr_filter_t	hdc_filter_data[NHDC];	/* per controller filter */


static char	hdc_signon[] = "\
IPI Device Driver\n\
Copyright (c) 1993 Intel Corporation, All Rights Reserved\n";

/*
 * State descriptor for this layer
 */
struct hdc_softc {
	watchdog_t	wd;		/* must be first for watchdog  */

	volatile char	state;		/* state info for this layer   */
#define	HDC_STATE_SPARE	0xff

	unsigned char	ntargets;	/* how many alive on this bus  */
	ipi_softc_t	*sc;		/* HBA independent information */
	hctlr_filter_t	*filter;	/* destination filter data     */
} hdc_softc_data[NHDC];

typedef struct hdc_softc	*hdc_softc_t;

hdc_softc_t	hdc_softc[NHDC];


/*
 * Definition of the driver for the auto-configuration program
 */
int	hdc_probe(), ipi_slave(), ipi_attach(), hdc_go();

caddr_t			hdc_std[NHDC] = { 0 };
struct bus_device	*hdc_dinfo[NHDC * MAX_IPI_TARGETS];
struct bus_ctlr		*hdc_minfo[NHDC];
struct bus_driver	hdc_driver = {
	/*
	 *	 structure		  structure    field
	 *     initialization		   field    description
	 */
		hdc_probe,		/* probe  - probe function     */
		ipi_slave,		/* slave  - slave function     */
		ipi_attach,		/* attach - attach function    */
		hdc_go,			/* dgo    - go function        */
		hdc_std,		/* addr   - device csr address */
		"ipi",			/* dname  - device driver name */
		hdc_dinfo,		/* dinfo  - bus_device pointer */
		"hdc",			/* mname  - controller name    */
		hdc_minfo,		/* minfo  - bus_ctlr pointer   */
		BUS_INTR_B4_PROBE	/* flags - flags               */
};


#if	MACH_ASSERT

/*
 * For debug kernels, the HiPPI channel timeout
 * is long.  The hdc timeout should (ideally) be
 * slightly longer than the HiPPI channel timeout.
 *
 * Setting HDC_TIMEOUT to 7000000 microseconds actually
 * provides about 18.4 seconds for the timeout due to
 * the fixed overhead of the delay() function.
 */
#define	HDC_TIMEOUT	7000000	/* microseconds */

#else	MACH_ASSERT

/*
 * The hdc timeout should (ideally) be slightly longer
 * than the HiPPI channel timeout.
 *
 * Setting HDC_TIMEOUT to 3000000 microseconds actually
 * provides about 7.9 seconds for the timeout due to
 * the fixed overhead of the delay() function.
 */
#define	HDC_TIMEOUT	3000000	/* microseconds */

#endif	MACH_ASSERT

/*
 * Timeout conversion macros
 *
 * The following macros attempt to compensate for the fixed
 * overhead of the delay() function.  Each delay(1) call
 * provides about 2.63 microseconds of actual delay.  The
 * macros yield approximate values.  The value 380228 was
 * determined as 1000000/2.63 microseconds.
 */
#define	HDC_TMO_ADJUST		380228
#define	HDC_TMO_TO_SEC(t)	((t) / HDC_TMO_ADJUST)
#define	HDC_SEC_TO_TMO(t)	((t) * HDC_TMO_ADJUST)

/*
 * The scatter/gather list must fit within one processor page.
 * This is because the i960 is handed a physical pointer to the
 * head of the scatter/gather list.  The entire list must fit
 * within this physical page.
 */
#define	HDC_MAX_SG_ENTRIES	\
	((PAGE_SIZE - sizeof(struct io_sglist_hdr))/sizeof(struct io_sg_entry))


/*
 * Probe routine:
 *	Should find out:
 *	    a) if the controller is present
 *	    b) which slaves are present
 *
 * Implementation:
 *	Send a NOP to each possible slave on the bus
 */
hdc_probe(virt, bus)
	unsigned int	virt;
	struct bus_ctlr	*bus;
{
	int		unit = bus->unit;
	hdc_softc_t     hdc = &hdc_softc_data[unit];
	ipi_softc_t	*sc;
	int		target_id;
	boolean_t	did_banner = FALSE;

	/*
	 * Initialize the hw descriptor
	 */
	hdc_softc[unit] = hdc;
	hdc->filter = (hctlr_filter_t *)&hdc_filter_data[unit];
	hdc->wd.reset = hdc_reset;

	if (!(sc = ipi_master_alloc(unit, hdc)))
		return 0;

	hdc->sc = sc;

#if	0
	sc->max_io_count   = HDC_MAX_SG_ENTRIES * PAGE_SIZE;
#else
	sc->max_io_count   = IPI_UNLIMITED_IO_COUNT;		/* no limit */
#endif
	sc->hba_timeout	   = HDC_TMO_TO_SEC(HDC_TIMEOUT);	/* default  */
	sc->supports_sgio  = TRUE;
	sc->go		   = hdc_go;
	sc->watchdog	   = ipi_watchdog;
	sc->probe	   = hdc_probe_target;
	sc->filter	   = hdc_filter;

	/*
	 * Do we have a HiPPI controller and is it alive?
	 */
	if (hctlr_init() != D_SUCCESS)
		return 0;

	/*
	 * Print the driver signon message
	 */
	printf("%s%s%d:", hdc_signon, bus->name, unit);

	/*
	 * Set the HBA timeout two seconds longer than the
	 * HiPPI channel timeout for some safety margin
	 */
	if (sc->hba_timeout < hctlr_timer + 2)
		sc->hba_timeout = hctlr_timer + 2;

	/*
	 * Set the watchdog timeout two seconds longer
	 * than the HBA timeout for some safety margin
	 */
	if (ipi_watchdog_period < sc->hba_timeout + 2)
		ipi_watchdog_period = sc->hba_timeout + 2;

	if (getbootint("BOOT_IPI_DEVICE_SCAN", FALSE) == FALSE)
		return 1;

	/*
	 * Register the completion callback routine with a filter
	 */
	if (sc->filter(hdc, TRUE) != D_SUCCESS) {
		printf("hdc: Error setting destination channel filter\n");
		return 0;
	}

	/*
	 * For all possible slaves, see if the slave is out there and alive
	 */
	for (target_id = 0; target_id < MAX_IPI_TARGETS; target_id++) {
		register target_info_t	*tgt;
		cmd_queue_t		*cmd_q;

		/*
		 * Allocate a target descriptor
		 */
		tgt = ipi_slave_alloc(sc->masterno, target_id, hdc);

		/*
		 * Read bootmagic variables
		 */
		ipi_get_bootmagic(tgt);

		/*
		 * Check configuration variables for errors
		 */
		if (ipi_config_error(tgt, FALSE) != D_SUCCESS) {
			printf("ignoring slave %d%s\n", target_id,
				" due to bootmagic error");
			tgt->flags = 0;
			continue;
		}

		/*
		 * Allocate command buffers
		 */
		if (ipi_queue_alloc(tgt)) {
			printf("ignoring slave %d%s\n", target_id,
				" due to memory allocation error");
			tgt->flags = 0;
			continue;
		}

		/*
		 * Get a command buffer, issue a NOP command,
		 * and poll for completion or timeout
		 */
		if ((cmd_q = IPI_GET_CMD(tgt, TRUE)) == NULL)
			panic("hdc_probe");

		/*
		 * Issue the NOP command
		 */
		ipi3_nop(tgt, cmd_q);

		/*
		 * Wait for command completion or timeout
		 */
		if (hdc_wait(cmd_q, sc->hba_timeout)) {
			printf("slave %d: timer expired, state 0x%x\n",
				target_id, cmd_q->state);
			ipi_queue_free(tgt);		/* Free buffers */
			tgt->flags = 0;
			continue;
		}

		/*
		 * Check command result
		 *     If IPI_RET_SUCCESS or IPI_RET_ERROR then it's alive!
		 *     Otherwise no one out there, clear flags and continue.
		 */
		if ((cmd_q->result != IPI_RET_SUCCESS) &&
		    (cmd_q->result != IPI_RET_ERROR)) {
			/* no one out there */
			ipi_queue_free(tgt);		/* Free buffers */
			tgt->flags = 0;
			continue;
		}

		/*
		 * Found a target.
		 * Bump the number of targets on this controller.
		 * Display a message for the target.
		 */
		hdc->ntargets++;
		printf("%s%d", did_banner++ ? ", " : " slave(s) at ",
			target_id);

		/*
		 * Check for command errors.
		 */
		if (cmd_q->result == IPI_RET_ERROR)
			ipi3_error(tgt, cmd_q);

		/*
		 * Free the command buffer.
		 */
		IPI_REL_CMD(cmd_q);
	}
	printf(".\n");

	/*
	 * Clear the completion callback routine filter
	 */
	if (sc->filter(hdc, FALSE) != D_SUCCESS) {
		printf("hdc: Error clearing destination channel filter\n");
		return 0;
	}

	return 1;
}


io_return_t
hdc_filter(hdc, flag)
	hdc_softc_t	hdc;
	boolean_t	flag;	/* set or clear */
{
	hctlr_filter_t	*filter = hdc->filter;

	/*
	 * Set/Clear the completion callback routine with a filter
	 *	The filter is set/cleared for deliverly of
	 *	ULP ids destined for the IPI-3 master
	 */
	filter->ulp    = ULP_ID_IPI_3_MASTER;
	filter->offset = 0;
	filter->bsize  = 0;
	filter->min    = 0;
	filter->max    = 0;
	filter->ddone  = hdc_dst_intr;

	return (io_return_t)hctlr_recv_filter(filter, flag);
}


hdc_wait(cmd_q, timeout)
	cmd_queue_t	*cmd_q;
	int		timeout;
{
	/*
	 * Convert the timeout to microseconds
	 */
	timeout = HDC_SEC_TO_TMO(timeout);
	assert(timeout);

	/*
	 * Poll for command completion or timeout.  Note that
	 * the command state may be marked complete while the
	 * source channel interrupt is pending.
	 */
	while ((cmd_q->state != CMD_COMPLETE) ||
	       (cmd_q->flags & CMD_SRC_INTR_PENDING)) {
		delay(1);
		if (!timeout--)
			return 1;
	}

	return 0;
}


boolean_t
hdc_probe_target(tgt, cmd_q)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
{
	hdc_softc_t	hdc = (hdc_softc_t)tgt->hw_state;
	ipi_softc_t	*sc = hdc->sc;
	boolean_t	ret = TRUE;

	/*
	 * Register the completion callback routine with a filter
	 */
	if (sc->filter(hdc, TRUE) != D_SUCCESS) {
		printf("hdc: Error setting destination channel filter\n");
		return FALSE;
	}

	/*
	 * Issue a NOP command and wait for completion or timeout
	 */
	ipi3_nop(tgt, cmd_q);

	if (hdc_wait(cmd_q, sc->hba_timeout) ||
	    (cmd_q->result != IPI_RET_SUCCESS)) {
		wd_t	*wdog = &hdc->wd.dog[tgt->target_id];

		/*
		 * Stop the watchdog for this command
		 */
		if (wdog->nactive-- == 1)
			wdog->state = IPI_WD_INACTIVE;

		ret = FALSE;
	}

	/*
	 * Clear the completion callback routine filter
	 */
	if (sc->filter(hdc, FALSE) != D_SUCCESS) {
		printf("hdc: Error clearing destination channel filter\n");
		return FALSE;
	}

	if (ret) {
		tgt->flags = TGT_ALIVE;
	}

	return ret;
}


hdc_go(tgt, cmd_q)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
{
	hctlr_src_t	*sreq = &cmd_q->sreq;
	register int	s;

	/* CLASS-2 Support */
	/*
	 * Clear the physical scatter/gather entry pointer
	 * Used for Class-2 packet support in destination interrupt
	 */
	cmd_q->iosge_phys = 0;
	/* CLASS-2 Support */

	/*
	 * Finish initialization of source channel request
	 */
	sreq->sdone   = hdc_src_intr;

	s = splipi();

	if (tgt->flags & (TGT_QUEUE_FULL | TGT_BBR_ACTIVE)) {
		/*
		 * The target command queue is full or bad block replacement
		 * is active.  Queue up this command in FIFO order.
		 */
		simple_lock(&tgt->target_lock);
		enqueue_tail(&tgt->waiting_cmds, (queue_entry_t)cmd_q);
		simple_unlock(&tgt->target_lock);

		/*
		 * Mark the command as ready
		 */
		cmd_q->state = CMD_READY;
	} else {
		hdc_softc_t	hdc;
		wd_t		*wdog;

		hdc = (hdc_softc_t)tgt->hw_state;
		wdog = &hdc->wd.dog[tgt->target_id];

		/*
		 * Queue this command for transmission and start the watchdog
		 */
		hdc_queue_request(tgt, cmd_q, wdog, sreq);
	}

	splx(s);
}


hdc_queue_request(tgt, cmd_q, wdog, sreq)
	target_info_t		*tgt;
	register cmd_queue_t	*cmd_q;
	register wd_t		*wdog;
	register hctlr_src_t	*sreq;
{
	/*
	 * Queue this source channel request
	 */
	if (hctlr_src_q(sreq) != D_SUCCESS) {
		/*
		 * The channel is down, set the command result
		 */
		cmd_q->result = IPI_RET_DEVICE_DOWN;
		cmd_q->state  = CMD_COMPLETE;

		if (tgt->flags & TGT_FULLY_PROBED)
			tgt->flags = 0;

		/*
		 * Call the command completion routine
		 */
		if (tgt->dev_ops)
			(tgt->dev_ops->restart)(tgt, cmd_q, TRUE);
	}

	/*
	 * Do the device check-in here.  Although this includes the controller
	 * interface and i960 driver times, it greatly simplifies doing time
	 * stamps during interrupts, especially if the destination channel
	 * completion interrupt comes in before the source channel interrupt.
	 * The driver times are small compared to the device times anyway.
	 */
	IPI_DEVICE_CHECKIN(cmd_q->tgt, cmd_q->ior);

	/*
	 * Mark the command as queued
	 */
	cmd_q->state = CMD_QUEUED;

	/*
	 * Start the watchdog for this command
	 */
	if (wdog->nactive++ == 0)
		wdog->state = IPI_WD_ACTIVE;
}


/*
 * Source Channel Interrupt routine
 */
hdc_src_intr(unit, sreq, count)
	int			unit;
	register hctlr_src_t	*sreq;
	register int		count;
{
	hdc_softc_t		hdc;
	wd_t			*wdog;
	register cmd_queue_t	*cmd_q;
	register unsigned int	csr;
	register hctlr_src_t	*next;

	assert(count > 0);

	hdc = hdc_softc[unit];

	/*
	 * Loop through count times, checking each request as we go.
	 */
	for (; count--; sreq = next) {

		next = (hctlr_src_t *)queue_next(&sreq->links);

		assert(sreq);

		/*
		 * Initialize some pointers
		 */
		cmd_q = (cmd_queue_t *)sreq;
		wdog = &hdc->wd.dog[cmd_q->tgt->target_id];

		if (wdog->nactive)
			wdog->state = IPI_WD_ACTIVE;

		/*
		 * Check for controller errors
		 */
		if ((csr = sreq->error_status & HDC_SRC_STATUS_MASK) == 0) {
			/*
			 * No errors.
			 * The command is now in progress on the slave device
			 *
			 * We may get the destination interrupt before the
			 * source interrupt, so check the state prior to
			 * setting the new value.  It should have been
			 * queued if we just sent it out, otherwise the
			 * destination interrupt came in first.
			 */
			if (cmd_q->state == CMD_QUEUED) {
				cmd_q->state = CMD_IN_PROGRESS;
			} else {
				target_info_t	*tgt = cmd_q->tgt;

				/*
				 * A destination channel interrupt was
				 * received before the source interrupt
				 */
				assert(cmd_q->flags & CMD_SRC_INTR_PENDING);

				/*
				 * Clear the pending source interrupt flag
				 */
				cmd_q->flags &= ~CMD_SRC_INTR_PENDING;

				/*
				 * If the command is complete call
				 * the command completion routine
				 */
				if ((cmd_q->state == CMD_COMPLETE) &&
				    (tgt->dev_ops)) {
					(tgt->dev_ops->restart)(tgt, cmd_q,
						TRUE);
				}
			}

			continue;
		}

		/*
		 * Give controller resets highest priority
		 */
		if (csr & HDC_SRC_CONTROLLER_RESET) {
			/*
			 * Handle the command that was reset
			 */
			hdc_was_reset(hdc, cmd_q);

			/*
			 * Continue to the next source request
			 */
			continue;
		}

		/*
		 * Check no channel connection or connection timeouts
		 */
		if (csr & (HDC_SRC_NO_INTERCONNECT | HDC_SRC_TIMEOUT)) {
			target_info_t	*tgt = cmd_q->tgt;

			/*
			 * Non-existant or powered off devices here.
			 * The controller should have retried several
			 * times before giving up.  Instead of returning
			 * IPI_RET_RETRY just mark the device as down.
			 */
			cmd_q->result = IPI_RET_DEVICE_DOWN;

			/* Powered off or loose cable ? */
			if (tgt->flags & TGT_FULLY_PROBED)
				tgt->flags = 0;

			/*
			 * Clear the pending source interrupt flag
			 */
			cmd_q->flags &= ~CMD_SRC_INTR_PENDING;

			hdc_end(tgt, cmd_q, wdog);

			/*
			 * Continue to the next source request
			 */
			continue;
		}

		/*
		 * If any other type of error occured the target
		 * should generate a response message for the
		 * command.  Therefore, we will just fall through,
		 * printing an error message along the way.
		 */
		printf("\nhdc: source channel error 0x%08x\n", csr);

		if (csr & HDC_SRC_SEQUENCE_ERROR)
		  printf("Control signal sequence error from source\n");
		if (csr & HDC_SRC_DST_SEQUENCE_ERROR)
		  printf("Control signal sequence error from destination\n");
		if (csr & HDC_SRC_PARITY_ERROR)
		/*
		 * printf("Pieces of nine, pieces of nine! A parroty error!\n");
		 */
		  printf("Parity error occurred during transfer\n");
		if (csr & HDC_SRC_ODD_XFER)
		  printf("Odd number of 32-bit HiPPI words\n");
		if (csr & HDC_SRC_SHORT_XFER)
		  printf("Last word was short, not a 32-bit HiPPI word\n");

		/*
		 * We may get the destination interrupt before the
		 * source interrupt, so check the state prior to
		 * setting the new value.  It should have been
		 * queued if we just sent it out.
		 */
		if (cmd_q->state == CMD_QUEUED) {
			cmd_q->state = CMD_IN_PROGRESS;
		} else {
			target_info_t	*tgt = cmd_q->tgt;

			/*
			 * A destination channel interrupt was
			 * received before the source interrupt
			 */
			assert(cmd_q->flags & CMD_SRC_INTR_PENDING);

			/*
			 * Clear the pending source interrupt flag
			 */
			cmd_q->flags &= ~CMD_SRC_INTR_PENDING;

			/*
			 * If the command is complete call
			 * the command completion routine
			 */
			if ((cmd_q->state == CMD_COMPLETE) && (tgt->dev_ops)) {
				(tgt->dev_ops->restart)(tgt, cmd_q, TRUE);
			}
		}
	}
}


/*
 * Destination Channel Interrupt routine
 */
hdc_dst_intr(unit, dst)
	int		unit;
	hctlr_dst_t	*dst;
{
	hdc_softc_t		hdc;
	hippi_fp_t		*hippi_fp;
	ipi_response_t		*rsp;
	register target_info_t	*tgt;
	wd_t			*wdog;
	unsigned short		cmd_ref;
	register cmd_queue_t	*cmd_q;
	unsigned int		csr;
	int			type;
	register int		fb_size, d2_spill;	/* CLASS-2 Support */

	/*
	 * Initialize some pointers
	 * Do some sanity checks along the way
	 */
	/* Cannot not be done and have errors */
	assert (!(!dst->pkt_done && (dst->error_status & HDC_DST_STATUS_MASK)));
	hdc = hdc_softc[unit];
	hippi_fp = (hippi_fp_t *)dst->fb_ptr;
        /* ULP ID must be slave to master */
	assert(hippi_fp->ulp_id == ULP_ID_IPI_3_MASTER);
	/* Must contain a D1 area */
	assert(((hippi_fp->d1_size_msb << D1_SIZE_MSB_SHIFT) |
		(hippi_fp->d1_size_lsb << 0)) != 0);
	rsp = (ipi_response_t *)(hippi_fp + 1);
        /* The response slave field must contain a valid target */
	assert((rsp->slave >= 0) && (rsp->slave < MAX_IPI_TARGETS));
	tgt = hdc->sc->target[rsp->slave];
	/* There must be a valid target descriptor allocated */
	assert(tgt != 0);
	wdog = &hdc->wd.dog[tgt->target_id];

	/*
	 * Now set up some variables
	 *
	 *	Note that cmd_ref (and therefore cmd_q) may be invalid if this
	 *	is a non-associated response (as indicated by opcode of 0xff)
	 */
	cmd_ref = (rsp->reference_msb << 8) | (rsp->reference_lsb << 0);
	cmd_q = tgt->cmd_q + (cmd_ref - tgt->cmd_ref_min);
	type = rsp->type;

	/*
	 * See if we'er done
	 */
	if (dst->pkt_done) {

#if	0
		/*
		 * Do some additional checking
		 */
		if ((cmd_ref < tgt->cmd_ref_min) ||
		    (cmd_ref > (tgt->cmd_ref_min + tgt->cmd_ref_len))) {
			printf(
			"\nSlave %d: Invalid Command Reference number 0x%04x\n",
				rsp->slave, cmd_ref);
			return;
		}
#endif

		/* CLASS-2 Support */
		/*
		 * Restore the original scatter/gather
		 * entry values, if they were adjusted 
		 */
		if (cmd_q->iosge_phys) {
			io_sglist_t	sgp = cmd_q->ior->io_sgp;
			
			sgp->iosg_list[0].iosge_phys   = cmd_q->iosge_phys;
			sgp->iosg_list[0].iosge_length = cmd_q->iosge_length;

			cmd_q->iosge_phys   = 0;
			cmd_q->iosge_length = 0;
		}
		/* CLASS-2 Support */

		/*
		 * Check for controller errors
		 */
		if ((csr = dst->error_status & HDC_DST_STATUS_MASK) != 0) {

			/*
			 * If a controller interface software layer detects
			 * an error in the first burst, no attempt is made
			 * to filter the burst and deliver it to the driver
			 * interrupt.  It may be possible to match corrupted
			 * data with the filter and deliver the burst to the
			 * wrong ULP driver.  Instead, the controller interface
			 * software layer will abort the rest of the packet.
			 * If an error occurs in any other burst, the ULP
			 * driver interrupt routine will be called to handle
			 * the error.  This means the pkt_done flag can never
			 * be FALSE and have errors in the error_status bits.
			 */

			printf("\nhdc: destination channel error 0x%x\n", csr);

			if (csr & HDC_DST_NO_INTERCONNECT)
			  printf("Lost interconnect signal during transfer\n");
			if (csr & HDC_DST_SYNC_ERROR)
			  printf("Synchronization error\n");
			if (csr & HDC_DST_SEQUENCE_ERROR)
			  printf("Destination sequence error\n");
			if (csr & HDC_DST_PARITY_ERROR)
		/*
		 * printf("Pieces of nine, pieces of nine! A parroty error!\n");
		 */
			  printf("Parity error occurred during transfer\n");
			if (csr & HDC_DST_TIMEOUT)
			  printf("Timed out waiting for burst\n");
			if (csr & HDC_DST_LLRC_ERROR)
			  printf("LLRC error detected\n");
			if (csr & HDC_DST_NO_BUFFERS)
			  printf("Receive buffers exhausted\n");
			if (csr & HDC_DST_PACKET_ABORTED)
			  printf("Packet aborted\n");
			if (csr & HDC_DST_MISALIGNED_XFER)
			  printf("Misaligned transfer\n");

			return hdc_dst_error(tgt, cmd_q, wdog, rsp);
		}

		switch (type) {

			case IPI_TRANSFER_NOTIFICATION:
			{
				/*
				 * See if data in was handled
				 * on the first time through
				 */
				if (cmd_q->state == CMD_DATA_IN)
					return;

				/* CLASS-2 Support */
				/*
				 * Since the command queue state was not
				 * marked as CMD_DATA_IN this is the first
				 * interrupt for the transfer.  Because the
				 * dst->pkt_done flag was set all of the data
				 * fit in the first controller byte buffer.
				 *
				 * We will attempt to handle this case by
				 * copying the data received to the I/O
				 * request data buffer.
				 */

				/*
				 * Add the size of the D1 area to the size
				 * of the HiPPI header.  This gives us the
				 * actual size of the first burst.
				 */
				fb_size = sizeof(hippi_fp_t) +
				  (((hippi_fp->d1_size_msb<<D1_SIZE_MSB_SHIFT) |
				  hippi_fp->d1_size_lsb) * 8);

				d2_spill = dst->xfer_len - fb_size;
				assert(d2_spill > 0);

				if (ipi_debug) {
					printf("\nhdc: Slave %d ", rsp->slave);
					printf("first busrt size: %d, ",
						fb_size);
					printf("last busrt size:  %d\n",
						d2_spill);
				}

				bcopy((unsigned char *)dst->fb_ptr + fb_size,
					cmd_q->sreq.ior->io_data, d2_spill);

				/*
				 * Mark the command state as data in.  If we
				 * have not yet received the source channel
				 * interrupt (determined by checking for the
				 * expected state) set the interrupt pending
				 * flag.
				 */
				if (cmd_q->state == CMD_QUEUED) {
					cmd_q->flags |= CMD_SRC_INTR_PENDING;
				}

				cmd_q->state = CMD_DATA_IN;

				/*
				 * Update the watchdog
				 */
				if (wdog->nactive)
					wdog->state = IPI_WD_ACTIVE;

				return;
				/* CLASS-2 Support */
			}

			case IPI_STANDARD_RESPONSE:
			case IPI_EXTENDED_RESPONSE:
			{
				/*
				 * Mark the command state as status.  If we
				 * have not yet received the source channel
				 * interrupt (determined by checking for the
				 * expected states) set the interrupt pending
				 * flag.
				 */
				if (cmd_q->state == CMD_QUEUED) {
					cmd_q->flags |= CMD_SRC_INTR_PENDING;
				}

				cmd_q->state = CMD_STATUS;

				cmd_q->result =
					(rsp->status_msb == IPI_SUCCESSFUL) ?
					IPI_RET_SUCCESS : IPI_RET_ERROR;

				if (cmd_q->result == IPI_RET_ERROR ||
				    cmd_q->dir == NO_DATA) {
					assert(dst->xfer_len<=HIPPI_BURST_SIZE);
					bcopy(hippi_fp, cmd_q->hdr_ptr,
						dst->xfer_len);
				}

				return hdc_end(tgt, cmd_q, wdog);
			}

			case IPI_ASYNCHRONOUS_RESPONSE:
				return
				  hdc_async_response(tgt, cmd_q, rsp, cmd_ref);

			case IPI_EMBEDDED_DATA_RESPONSE:
			default:
				panic("Unsupported IPI response type");
				return;
		}
	}

	/*
	 * If the pkt_done flag is FALSE this should be a transfer
	 * notification response for a read operation.  Also,
	 * the command reference number must be within range of
	 * this target.
	 */
	assert(type == IPI_TRANSFER_NOTIFICATION);
	assert((cmd_ref >= tgt->cmd_ref_min) &&
	       (cmd_ref < (tgt->cmd_ref_min + tgt->cmd_ref_len)));

#if	0
	/*
	 * Do some additional checking
	 */
	if ((cmd_ref < tgt->cmd_ref_min) ||
	    (cmd_ref > (tgt->cmd_ref_min + tgt->cmd_ref_len))) {
		printf("\nSlave %d: Invalid Command Reference number 0x%04x\n",
			rsp->slave, cmd_ref);
		printf("Aborting HiPPI packet\n");
		hctlr_recv_buffer(NULL);
		return;
	}
#endif

	/* CLASS-2 Support */
	/*
	 * The HiPPI controller hardware cannot detect short first bursts
	 * on the destination channel.  Unfortunately, in IPI the first
	 * burst is the only one permitted to be short.  We are handed a
	 * controller buffer which has some of the D2 area appended to the
	 * D1 area.  There are more D2 data and bursts in the controller
	 * FIFO.  The i960 is waiting for a scatter/gather list telling it
	 * where to put the rest of the data.  The figure below illustrates
	 * receiving a short first burst followed full bursts.  Notice how
	 * the first D2 burst spilled over into the buffer.
	 *
	 *
	 *	|     We get this    |        These are in the FIFO 
	 *	|<------ 1k+32 ----->|<------- 1k ------->|<------- 1k --
	 *	+-----------++-------|---------------++---|--------------
	 *	| FP header ||       Full D2         ||       More Full
	 *	|  and D1   ||        Burst          ||       D2 Bursts
	 *	+-----------++-----------------------++------------------
	 *
	 */

	/*
	 * Add the size of the D1 area to the size of the HiPPI header.
	 * This gives us the actual size of the first burst.
	 */
	fb_size = sizeof(hippi_fp_t) +
		(((hippi_fp->d1_size_msb << D1_SIZE_MSB_SHIFT) |
		hippi_fp->d1_size_lsb) * 8);

	/*
	 * If the first burst was short we must adjust the scatter/gather list
	 */
	if ((d2_spill = dst->xfer_len - fb_size) > 0) {

		io_sglist_t	sgp = cmd_q->ior->io_sgp;

		/*
		 * We only adjust the first entry so there must
		 * be enough data here to make the adjustment.
		 * I don't think this will be a problem for HiPPI
		 * attached devices because the d2_spill must be
		 * less than 1016+32 bytes.  The device block size
		 * should be greater than that, but just in case...
		 */
		assert(sgp->iosg_list[0].iosge_length >= d2_spill);

		/*
		 * if (ipi_debug)
		 *	printf("hdc_dst_intr: Adjusting iosge by %d bytes\n",
		 *		d2_spill);
		 */

		/*
		 * Save the original scatter/gather entry values.
		 * We must restore it later in case we need to
		 * retry the operation.
		 */
		cmd_q->iosge_phys   = sgp->iosg_list[0].iosge_phys;
		cmd_q->iosge_length = sgp->iosg_list[0].iosge_length;

		/*
		 * Adjust the scatter/gather list.
		 * The i960 doesn't look at the sgp->iosg_hdr.length
		 * field so we won't bother adjusting it.
		 */
		sgp->iosg_list[0].iosge_phys   += d2_spill;
		sgp->iosg_list[0].iosge_length -= d2_spill;
	}
	/* CLASS-2 Support */

	/*
	 * Turn the controller loose
	 */
	hctlr_recv_buffer(cmd_q->ior->io_sgp);

	/* CLASS-2 Support */
	/*
	 * While the HiPPI controller is busy receiving
	 * the rest of the packet, we can move the data
	 * that spilled over into the first buffer.
	 */
	if (d2_spill)
		bcopy((unsigned char *)dst->fb_ptr + fb_size,
			cmd_q->sreq.ior->io_data, d2_spill);
	/* CLASS-2 Support */

	/*
	 * Mark the command state as data in.  If we have not yet
	 * received the source channel interrupt (determined by
	 * checking for the expected state) set the interrupt
	 * pending flag.
	 */
	if (cmd_q->state == CMD_QUEUED) {
		cmd_q->flags |= CMD_SRC_INTR_PENDING;
	}

	cmd_q->state = CMD_DATA_IN;

	/*
	 * Update the watchdog
	 */
	if (wdog->nactive)
		wdog->state = IPI_WD_ACTIVE;
}


hdc_end(tgt, cmd_q, wdog)
	register target_info_t	*tgt;
	register cmd_queue_t	*cmd_q;
	wd_t			*wdog;
{
	/*
	 * Device check-in statistics are started when the
	 * state transitions to CMD_QUEUED.  Therefore we
	 * complete the device check-out anytime we end
	 * a command.
	 */
	IPI_DEVICE_CHECKOUT(tgt, cmd_q->ior);

	/*
	 * Mark this command complete and bump down the watchdog trigger level
	 */
	cmd_q->state = CMD_COMPLETE;

	if (wdog->nactive-- == 1)
		wdog->state = IPI_WD_INACTIVE;

	/*
	 * We've finished a command, go ahead and clear the queue full flag
	 */
	tgt->flags &= ~TGT_QUEUE_FULL;

	/*
	 * Start any commands chained up for this target
	 */
	while (!queue_empty(&tgt->waiting_cmds) &&
	       !(tgt->flags & (TGT_QUEUE_FULL | TGT_BBR_ACTIVE))) {

		simple_lock(&tgt->target_lock);
		hdc_queue_request(tgt, cmd_q, wdog,
			(hctlr_src_t *)dequeue_head(&tgt->waiting_cmds));
		simple_unlock(&tgt->target_lock);
	}

	/*
	 * Bail out if the source channel interrupt pending is still pending
	 */
	if (cmd_q->flags & CMD_SRC_INTR_PENDING)
		return;

	/*
	 * Call the command completion routine
	 */
	if (tgt->dev_ops)
		(tgt->dev_ops->restart)(tgt, cmd_q, TRUE);
}


hdc_dst_error(tgt, cmd_q, wdog, rsp)
	register target_info_t	*tgt;
	register cmd_queue_t	*cmd_q;
	register wd_t		*wdog;
	register ipi_response_t	*rsp;
{
	/*
	 * There was an error on the destination channel when the D2 data
	 * area was received (bursts other than the first burst).
	 */

	/*
	 * If we don't have a valid target just ignore this one.
	 * The watchdog should catch the command that caused it.
	 */
	if (tgt ==  0 || tgt->target_id != rsp->slave)
		return;

	/*
	 * Set command result to retry
	 */
	cmd_q->result = IPI_RET_RETRY;

	return hdc_end(tgt, cmd_q, wdog);
}


hdc_async_response(tgt, cmd_q, rsp, cmd_ref)
	register target_info_t	*tgt;
	register cmd_queue_t	*cmd_q;
	register ipi_response_t	*rsp;
	unsigned short		cmd_ref;
{
	/*
	 * During an asynchronous response we don't want to modify
	 * anything in the cmd_q structure since it may already be
	 * in use for another command.
	 */

	printf("Slave %d: ", rsp->slave);

	if (rsp->opcode == IPI_CMD_ASYNC_RESPONSE)
	  /* non-associated asynchronous response */
	  printf("Non-associated asynchronous response\n");
	else {
	  /* associated asynchronous response */
	  printf("Associated asynchronous response\n");
	  printf("Command Reference 0x%04x, Opcode 0x%02x, Modifier 0x%02x\n",
			cmd_ref, rsp->opcode, rsp->modifier);
	}
}


hdc_reset(hdc, target_id)
	hdc_softc_t	hdc;
	int		target_id;
{
	/*
	 * This routine is invoked when we have sent a command to a
	 * target and the watchdog timed out waiting for a response.
	 * We should attempt to make a graceful recovery.
	 *
	 * Note that the way the watchdog works is that as long as any
	 * commands are making progress on the target, the watchdog will
	 * not invoke this routine (refer to case b below).
	 *
	 * We may have:
	 *
	 *	a) queued the command with the controller driver
	 *	   but the hdc_src_intr routine was not called.
	 *	   In this case, the state should be CMD_QUEUED.
	 *	   This should never happen because the controller
	 *	   driver would have to be to completely lost.
	 *
	 *	b) sent the command to the slave device but never
	 *	   received a response from the slave.  In this
	 *	   case, the state should be CMD_IN_PROGRESS.
	 *	   We may be backed up with commands because the
	 *	   target is apparently accepting commands without
	 *	   responding to them within the watchdog timeout
	 *	   period.
	 *
	 * Case b above is the most likely and the implementation of
	 * this routine will revolve around that senerio.  We will
	 * scan the target for commands that are currently marked
	 * as CMD_IN_PROGRESS, mark them for retry, and call the
	 * command completion routine to restart the request.
	 */

	register target_info_t	*tgt = hdc->sc->target[target_id];
	register cmd_queue_t	*cmd_q = tgt->cmd_q;
	int			depth  = tgt->cmd_ref_len;
	int			i;

	for (i = 0; i < depth; i++, cmd_q++) {

		if (cmd_q->state != CMD_IN_PROGRESS)
			continue;

		IPI_DEVICE_CHECKOUT(tgt, cmd_q->ior);

		cmd_q->result = IPI_RET_RETRY;
		cmd_q->state  = CMD_COMPLETE;
		cmd_q->flags &= ~CMD_SRC_INTR_PENDING;

		if (tgt->dev_ops)
			(tgt->dev_ops->restart)(tgt, cmd_q, TRUE);
	}
}


/*
 * The HiPPI daughter card was reset
 */
hdc_was_reset(hdc, cmd_q)
	register hdc_softc_t	hdc;
	register cmd_queue_t	*cmd_q;
{
	register int	target_id = cmd_q->tgt->target_id;
	wd_t		*wdog = &hdc->wd.dog[target_id];

	printf("hdc: slave %d command was reset (%d)\n",
		target_id, ++hdc->wd.reset_count);

	IPI_DEVICE_CHECKOUT(cmd_q->tgt, cmd_q->ior);

	/*
	 * We are given the source channel request that was active
	 * when the reset occurred.  Bump down the watchdog trigger
	 * level and call the MI ipi_was_reset function to retry
	 * the request.
	 */
	if (wdog->nactive-- == 1)
		wdog->state = IPI_WD_INACTIVE;

	ipi_was_reset(cmd_q);
}

#endif	NHDC > 0
