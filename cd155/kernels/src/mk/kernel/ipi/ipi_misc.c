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
 * $Log: ipi_misc.c,v $
 * Revision 1.11  1995/03/07  02:41:12  lenb
 *         move dclock() to model_dep.c (12502)
 *
 * Revision 1.10  1995/03/02  23:36:17  arlin
 *   Added Multiple Packet per Connection support
 *   and CONTinuation support for multiple I/O
 *   per packet and connection. F/W is R1.4
 *
 *   Reviewer: Jerrie Coffman, Bernie Keany
 *   Risk: medium
 *   Benefit or PTS #: 12411
 *   Testing: HiPPI EATs: Raw, TCP/IP, and IPI-3.
 *       Also developed special applications to test
 *       new MPC and CONT modes.
 *   Module(s):
 *     /i860paragon/hippi/
 *       hctlr.c, hctlr.h, rhippi.h, rhippi.c,
 *       hippi_status.h, hdc.c
 *     /ipi/ipi_misc.c ipi_defs.h, ipi.c,
 *     /device/ds_routines.c
 *
 * Revision 1.9  1995/02/02  21:04:32  jerrie
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
 * Revision 1.8  1995/02/02  19:13:15  jerrie
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
 * Revision 1.7  1994/12/16  21:42:40  jerrie
 * Corrected default setting of IPI_SLAVE_0_IFIELD.  Checked in with PTS 11392.
 *
 *  Reviewer:  Arlin Davis
 *  Risk:	    Low.
 *  Benefit or PTS #:  11392
 *  Testing:   Ran Evaluation test suites.
 *  Module(s): The following kernel files are being modified:
 * 		kernel/ipi/ipi_disk.c
 * 		kernel/ipi/ipi_master.c
 * 		kernel/ipi/ipi_misc.c
 * 		kernel/ipi/ipi_endian.h
 *
 * Revision 1.6  1994/11/18  20:51:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/10/04  16:16:11  jerrie
 * Added the missing printf("\n") following the call to attach in the
 * ipi_probe() routine.
 *
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: None
 *  Testing: Disabled initial scan of devices during auto-configuration and
 * 	opened a device to force the ipi_probe() routine to be called.
 *  Module(s): ipi_misc.c
 *
 * Revision 1.4  1994/06/28  23:52:14  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.3  1994/06/22  18:07:14  arlin
 * ifield problems, must be in big indian format.
 *
 * Revision 1.2  1994/06/15  19:46:51  lenb
 *         as this file is the only use of dclock in the kernel,
 *         move its defintion here in the hope it will be delted.
 *
 * Revision 1.1  1994/06/08  16:53:58  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi_misc.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Middle layer of the IPI driver.
 *	This file contains Controller and Device-independent functions.
 */

#include <ipi.h>
#if	NIPI > 0

#include <sys/types.h>
#include <chips/busses.h>
#include <sys/ioctl.h>

#include <ipi/ipi_compat.h>
#include <ipi/ipi_endian.h>
#include <ipi/ipi_status.h>
#include <ipi/ipi_defs.h>
#include <ipi/ipi_map.h>
#include <ipi/ipi-3.h>


/*
 * Overall driver state
 */
ipi_softc_t	ipi_softc_data[NIPI];		/* per HBA state */
ipi_softc_t	*ipi_softc[NIPI];		/* quick access & checking */
target_info_t	ipi_target_data[NIPI * MAX_IPI_TARGETS]; /* per target state */

/*
 * Miscellaneous config
 */
#if	MACH_ASSERT
int		ipi_watchdog_period = 20;	/* seconds */
#else	MACH_ASSERT
int		ipi_watchdog_period = 10;	/* seconds */
#endif	MACH_ASSERT
int		ipi_open_timeout    = 60;	/* seconds */
boolean_t	ipi_automatic_bbr   = TRUE;	/* reallocate bad blocks */
int		ipi_retries	    = 10;	/* max number of retries */
int		ipi_debug	    = 0;	/* debug flag */


/*
 * Bootmagic configuration variables
 */
static struct boot_config {
	char		*name;		/* bootmagic string */
	unsigned int	value;		/* default value    */
};

static struct boot_config	config_i_field[] = {
	{ "IPI_SLAVE_0_IFIELD",		IPI_DEFAULT_I_FIELD + 0 },
	{ "IPI_SLAVE_1_IFIELD",		IPI_DEFAULT_I_FIELD + 1 },
	{ "IPI_SLAVE_2_IFIELD",		IPI_DEFAULT_I_FIELD + 2 },
	{ "IPI_SLAVE_3_IFIELD",		IPI_DEFAULT_I_FIELD + 3 },
	{ "IPI_SLAVE_4_IFIELD",		IPI_DEFAULT_I_FIELD + 4 },
	{ "IPI_SLAVE_5_IFIELD",		IPI_DEFAULT_I_FIELD + 5 },
	{ "IPI_SLAVE_6_IFIELD",		IPI_DEFAULT_I_FIELD + 6 },
	{ "IPI_SLAVE_7_IFIELD",		IPI_DEFAULT_I_FIELD + 7 }
};

static struct boot_config	config_facility[] = {
	{ "IPI_SLAVE_0_FACILITY",	IPI_DEFAULT_FACILITY },
	{ "IPI_SLAVE_1_FACILITY",	IPI_DEFAULT_FACILITY },
	{ "IPI_SLAVE_2_FACILITY",	IPI_DEFAULT_FACILITY },
	{ "IPI_SLAVE_3_FACILITY",	IPI_DEFAULT_FACILITY },
	{ "IPI_SLAVE_4_FACILITY",	IPI_DEFAULT_FACILITY },
	{ "IPI_SLAVE_5_FACILITY",	IPI_DEFAULT_FACILITY },
	{ "IPI_SLAVE_6_FACILITY",	IPI_DEFAULT_FACILITY },
	{ "IPI_SLAVE_7_FACILITY",	IPI_DEFAULT_FACILITY }
};

static struct boot_config	config_partition[] = {
	{ "IPI_SLAVE_0_PARTITION",	IPI_DEFAULT_PARTITION },
	{ "IPI_SLAVE_1_PARTITION",	IPI_DEFAULT_PARTITION },
	{ "IPI_SLAVE_2_PARTITION",	IPI_DEFAULT_PARTITION },
	{ "IPI_SLAVE_3_PARTITION",	IPI_DEFAULT_PARTITION },
	{ "IPI_SLAVE_4_PARTITION",	IPI_DEFAULT_PARTITION },
	{ "IPI_SLAVE_5_PARTITION",	IPI_DEFAULT_PARTITION },
	{ "IPI_SLAVE_6_PARTITION",	IPI_DEFAULT_PARTITION },
	{ "IPI_SLAVE_7_PARTITION",	IPI_DEFAULT_PARTITION }
};

static struct boot_config	config_cmd_ref_min[] = {
	{ "IPI_SLAVE_0_CMD_REF_MIN",	IPI_DEFAULT_CMD_REF_MIN },
	{ "IPI_SLAVE_1_CMD_REF_MIN",	IPI_DEFAULT_CMD_REF_MIN },
	{ "IPI_SLAVE_2_CMD_REF_MIN",	IPI_DEFAULT_CMD_REF_MIN },
	{ "IPI_SLAVE_3_CMD_REF_MIN",	IPI_DEFAULT_CMD_REF_MIN },
	{ "IPI_SLAVE_4_CMD_REF_MIN",	IPI_DEFAULT_CMD_REF_MIN },
	{ "IPI_SLAVE_5_CMD_REF_MIN",	IPI_DEFAULT_CMD_REF_MIN },
	{ "IPI_SLAVE_6_CMD_REF_MIN",	IPI_DEFAULT_CMD_REF_MIN },
	{ "IPI_SLAVE_7_CMD_REF_MIN",	IPI_DEFAULT_CMD_REF_MIN }
};

static struct boot_config	config_cmd_ref_max[] = {
	{ "IPI_SLAVE_0_CMD_REF_MAX",	IPI_DEFAULT_CMD_REF_MAX },
	{ "IPI_SLAVE_1_CMD_REF_MAX",	IPI_DEFAULT_CMD_REF_MAX },
	{ "IPI_SLAVE_2_CMD_REF_MAX",	IPI_DEFAULT_CMD_REF_MAX },
	{ "IPI_SLAVE_3_CMD_REF_MAX",	IPI_DEFAULT_CMD_REF_MAX },
	{ "IPI_SLAVE_4_CMD_REF_MAX",	IPI_DEFAULT_CMD_REF_MAX },
	{ "IPI_SLAVE_5_CMD_REF_MAX",	IPI_DEFAULT_CMD_REF_MAX },
	{ "IPI_SLAVE_6_CMD_REF_MAX",	IPI_DEFAULT_CMD_REF_MAX },
	{ "IPI_SLAVE_7_CMD_REF_MAX",	IPI_DEFAULT_CMD_REF_MAX }
};


/*
 * Device-specific operations are switched off this table
 */

extern char		*ipi_disk_name(),
			*ipi_optical_name(),
			*ipi_tape_name();

extern ipi_ret_t	ipi_disk_optimize(),
			ipi_optical_optimize(),
			ipi_tape_optimize();

extern ipi_ret_t	ipi_disk_open(),
			ipi_optical_open(),
			ipi_tape_open();

extern ipi_ret_t	ipi_disk_close(),
			ipi_optical_close(),
			ipi_tape_close();

extern int		ipi_disk_strategy(),
			ipi_optical_strategy(),
			ipi_tape_strategy();

extern int		ipi_disk_start(),
			ipi_optical_start(),
			ipi_tape_start();

extern io_return_t	ipi_disk_get_status(),
			ipi_optical_get_status(),
			ipi_tape_get_status();

extern io_return_t	ipi_disk_set_status(),
			ipi_optical_set_status(),
			ipi_tape_set_status();

ipi_devsw_t	ipi_devsw[] = {
/* IPI_DISK */		{
			  ipi_disk_name,	  ipi_disk_optimize,
			  ipi_disk_open,	  ipi_disk_close,
			  ipi_disk_strategy,	  ipi_disk_start,
			  ipi_disk_get_status,	  ipi_disk_set_status
			},
/* IPI_OPTICAL */	{
			  ipi_optical_name,	  ipi_optical_optimize,
			  ipi_optical_open,	  ipi_optical_close,
			  ipi_optical_strategy,	  ipi_optical_start,
			  ipi_optical_get_status, ipi_optical_set_status
			},
/* IPI_TAPE */		{
			  ipi_tape_name,	  ipi_tape_optimize,
			  ipi_tape_open,	  ipi_tape_close,
			  ipi_tape_strategy,	  ipi_tape_start,
			  ipi_tape_get_status,	  ipi_tape_set_status
			},
			0
};


/*
 * Utilities
 */
void
ipi_go_and_wait(tgt, cmd_q)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
{
	register ipi_softc_t	*sc = ipi_softc[tgt->masterno];

	(*sc->go)(tgt, cmd_q);

	if (cmd_q->ior) {
		iowait(cmd_q->ior);
	} else {
		int	timeout;

		/*
		 * A timeout is needed for commands during auto-configuration.
		 * If a command times out while it is in progress and we assume
		 * something is wrong with the device and end the command.
		 * This is only used for the ipi_slave and ipi_attach routines
		 * before the watchdog is running.
		 */
		timeout = ipi_watchdog_period * 1000000; /* microseconds */

		while (
			/* before tgt->dev_ops */
		       ((cmd_q->state != CMD_COMPLETE) ||
			(cmd_q->flags & CMD_SRC_INTR_PENDING)) &&

			/* after tgt->dev_ops */
		       (cmd_q->state != CMD_FREE)
		      ) {

			delay(1);

			if (!timeout--) {
				if (cmd_q->state == CMD_IN_PROGRESS) {
					cmd_q->result = IPI_RET_DEVICE_DOWN;
					cmd_q->state  = CMD_COMPLETE;
					return;
				}
				timeout = ipi_watchdog_period * 1000000;
			}
		}
	}
}

void
ipi_go(tgt, cmd_q)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
{
	register ipi_softc_t	*sc = ipi_softc[tgt->masterno];

	/*
	 * This function is also defined as a macro which
	 * may provide some addtional performance.
	 */

	(*sc->go)(tgt, cmd_q);
}

#ifdef	IPI_STATISTICS

/*
 * Performance monitoring routines
 */

ipi_driver_checkin(ior)
	io_req_t	ior;
{
#ifdef	PARAGON860
	union part_time	time;

	time.dbl = dclock();

	ior->timestamp.lo = time.longs.lo;
	ior->timestamp.hi = time.longs.hi;
#endif	PARAGON860
}

ipi_driver_checkout(tgt, ior)
	target_info_t	*tgt;
	io_req_t	ior;
{
#ifdef	PARAGON860
	union part_time	elapsed, time;

	time.longs.lo = ior->timestamp.lo;
	time.longs.hi = ior->timestamp.hi;
	elapsed.dbl   = (dclock() - time.dbl) * 1000000.0;	/* us */

	if (ior->io_op & IO_READ)
		tgt->statistics.read.driver_time  += elapsed.dbl;
	else
		tgt->statistics.write.driver_time += elapsed.dbl;
#endif	PARAGON860
}


ipi_device_checkin(tgt, ior)
	target_info_t	*tgt;
	io_req_t	ior;
{
#ifdef	PARAGON860
	union part_time	elapsed, time;
	double		now;

	if (!ior || (ior->io_op & IO_INTERNAL))
		return;

	now = dclock();

	time.longs.lo = ior->timestamp.lo;
	time.longs.hi = ior->timestamp.hi;
	elapsed.dbl   = (now - time.dbl) * 1000000.0;		/* us */

	time.dbl = now;
	ior->timestamp.lo = time.longs.lo;
	ior->timestamp.hi = time.longs.hi;

	if (ior->io_op & IO_READ)
		tgt->statistics.read.driver_time  += elapsed.dbl;
	else
		tgt->statistics.write.driver_time += elapsed.dbl;
#endif	PARAGON860
}

ipi_device_checkout(tgt, ior)
	target_info_t	*tgt;
	io_req_t	ior;
{
#ifdef	PARAGON860
	union part_time	elapsed, time;
	double		now;

	if (!ior || (ior->io_op & IO_INTERNAL))
		return;

	now = dclock();

	time.longs.lo = ior->timestamp.lo;
	time.longs.hi = ior->timestamp.hi;
	elapsed.dbl   = (now - time.dbl) * 1000000.0;		/* us */

	time.dbl = now;
	ior->timestamp.lo = time.longs.lo;
	ior->timestamp.hi = time.longs.hi;

	if (ior->io_op & IO_READ)
		tgt->statistics.read.device_time  += elapsed.dbl;
	else
		tgt->statistics.write.device_time += elapsed.dbl;
#endif	PARAGON860
}
#endif


/*
 * Display statistics for all devices
 */
ipi_stats(reset)
	boolean_t	reset;
{
	int		master;
	ipi_softc_t	*sc;
	int		id;
	target_info_t	*tgt;
	double		bytes, seconds;

	for (master = 0; master < NIPI; master++) {

	    if (!(sc = ipi_softc[master]))
		continue;

	    if (!reset)
		printf("IPI Master %d Statistics:\n\n", master);

	    for (id = 0; id < MAX_IPI_TARGETS; id++) {
		/*
		 * Skip un-initialized targets
		 */
		if (!(tgt = sc->target[id]))
			continue;

		if (reset) {
			/*
			 * Clear the statistics
			 */

			tgt->statistics.read.requests	  = 0;
			tgt->statistics.read.xfer_count	  = 0;
			tgt->statistics.read.driver_time  = 0;
			tgt->statistics.read.device_time  = 0;

			tgt->statistics.write.requests	  = 0;
			tgt->statistics.write.xfer_count  = 0;
			tgt->statistics.write.driver_time = 0;
			tgt->statistics.write.device_time = 0;

			tgt->statistics.queue_max	  = 0;
			tgt->statistics.chain_max	  = 0;
			/*
			 * Don't clear these
			 * Depths are updated on the fly
			 *
			tgt->statistics.queue_depth	  = 0;
			tgt->statistics.chain_depth	  = 0;
			 */
			tgt->statistics.driver_full	  = 0;
			tgt->statistics.device_full	  = 0;
			tgt->statistics.retries		  = 0;

			continue;
		}

		/*
		 * Print a column header
		 */
		printf("\t");
		printf("Slave  ");
		printf("%-12s", "Requests");		/* 10 digits, 2 space */
		printf("%-12s", "Blocks");		/* 10 digits, 2 space */
		printf("%-12s", "Driver us");		/* 10 digits, 2 space */
		printf("%-12s", "Device us");		/* 10 digits, 2 space */
		printf("%-8s",  "KB/sec");		/*  6 digits, 2 space */
		printf("%-3s",  "MB/sec");		/*  3 digits, 0 space */
		printf("\n");

		/*
		 * Read statistics
		 */
		printf("Read:\t");
		printf(" %2d    ", id);
		printf("%-10u  ", tgt->statistics.read.requests);
		printf("%-10u  ", tgt->statistics.read.xfer_count);
		printf("%-10u  ", tgt->statistics.read.driver_time);
		printf("%-10u  ", tgt->statistics.read.device_time);
		bytes = (double)tgt->statistics.read.xfer_count *
			(double)tgt->block_size;
		bytes /= 1000.0;	/* convert bytes to Kbytes */
		seconds = (double)tgt->statistics.read.driver_time +
			  (double)tgt->statistics.read.device_time;
		seconds /= 1000000.0;	/* convert usecs to seconds */
		printf("%-6d  ", (seconds) ? (int)(bytes/seconds) : 0);
		bytes /= 1000.0;	/* convert Kbytes to Mbytes */
		printf("%-3d", (seconds) ? (int)(bytes/seconds) : 0);
		printf("\n");

		/*
		 * Write statistics
		 */
		printf("Write:\t");
		printf(" %2d    ", id);
		printf("%-10u  ", tgt->statistics.write.requests);
		printf("%-10u  ", tgt->statistics.write.xfer_count);
		printf("%-10u  ", tgt->statistics.write.driver_time);
		printf("%-10u  ", tgt->statistics.write.device_time);
		bytes = (double)tgt->statistics.write.xfer_count *
			(double)tgt->block_size;
		bytes /= 1000.0;	/* convert bytes to Kbytes */
		seconds = (double)tgt->statistics.write.driver_time +
			  (double)tgt->statistics.write.device_time;
		seconds /= 1000000.0;	/* convert usecs to seconds */
		printf("%-6d  ", (seconds) ? (int)(bytes/seconds) : 0);
		bytes /= 1000.0;	/* convert Kbytes to Mbytes */
		printf("%-3d", (seconds) ? (int)(bytes/seconds) : 0);
		printf("\n");

		/*
		 * Driver statistics
		 */
		printf("Max Queued: %d ",  tgt->statistics.queue_max);
		printf("Max Chained: %d ", tgt->statistics.chain_max);
		printf("Driver Full: %d ", tgt->statistics.driver_full);
		printf("Device Full: %d ", tgt->statistics.device_full);
		printf("Retries: %d", tgt->statistics.retries);
		printf("\n");

		printf("\n");
	    }
	}

	return 0;
}


/*
 * Display information for debugging
 */
ipi_dump()
{
	int		master;
	ipi_softc_t	*sc;
	int		id;
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	register int	i;
	char		*msg;

	for (master = 0; master < NIPI; master++) {

	    if (!(sc = ipi_softc[master]))
		continue;

	    printf("IPI Master %d State:\n\n", master);

	    for (id = 0; id < MAX_IPI_TARGETS; id++) {
		/*
		 * Skip un-initialized targets
		 */
		if (!(tgt = sc->target[id]))
			continue;

		/* Identify the slave device */
		printf("Slave %d: (%s %s)\n", id,
			(*tgt->dev_ops->driver_name)(FALSE), tgt->tgt_name);

		/* Display the configuration data */
		printf("\tI-Field 0x%08x  Facility 0x%02x  Partition 0x%02x\n",
			htonl(tgt->i_field), tgt->facility, tgt->partition);
		printf("\tCommand Reference:  0x%04x to 0x%04x  (width %d)\n",
			tgt->cmd_ref_min,
			tgt->cmd_ref_min + tgt->cmd_ref_len - 1,
			tgt->cmd_ref_len);
		printf("\tFlags:  0x%x\n", tgt->flags);

		/* Walk the command queues and display interesting things */
		cmd_q = tgt->cmd_q;
		for (i = 0; i < tgt->cmd_ref_len; i++, cmd_q++) {
			printf("\tCommand Queue %d @ 0x%08x\n", i, cmd_q);

			/* Command queue state */
			switch (cmd_q->state) {
				case CMD_FREE:
					msg = "Free";
					break;
				case CMD_LOCKED:
					msg = "Locked";
					break;
				case CMD_READY:
					msg = "Ready";
					break;
				case CMD_QUEUED:
					msg = "Queued";
					break;
				case CMD_IN_PROGRESS:
					msg = "In Progress";
					break;
				case CMD_DATA_IN:
					msg = "Data In";
					break;
				case CMD_DATA_OUT:
					msg = "Data Out";
					break;
				case CMD_STATUS:
					msg = "Status";
					break;
				case CMD_COMPLETE:
					msg = "Complete";
					break;
				default:
					msg = "UNKNOWN";
					break;
			}
			printf("\t\tState:  %s (%d)\n", msg, cmd_q->state);

			/* Result is only valid if state is complete */
			switch (cmd_q->result) {
				case IPI_RET_SUCCESS:
					msg = "Success";
					break;
				case IPI_RET_RETRY:
					msg = "Retry";
					break;
				case IPI_RET_ERROR:
					msg = "Error";
					break;
				case IPI_RET_ABORTED:
					msg = "Aborted";
					break;
				case IPI_RET_DEVICE_DOWN:
					msg = "Device Down";
					break;
				default:
					msg = "UNKNOWN";
					break;
			}
			printf("\t\tResult: %s (%d)\n", msg, cmd_q->result);

			printf("\t\tSource Request Address:  0x%08x\n",
				&cmd_q->sreq);
			printf("\t\tFP Header Address:       0x%08x\n",
				cmd_q->hdr_ptr);
			printf("\t\tIPI Command Address:     0x%08x\n",
				cmd_q->cmd_ptr);
			printf("\t\tIO Request Address:      0x%08x\n",
				cmd_q->ior);
			printf("\t\tCommand Reference Number: 0x%04x\n",
				cmd_q->cmd_ref);

			switch (cmd_q->dir) {
				case NO_DATA:
					msg = "No Data";
					break;
				case DATA_IN:
					msg = "Data In";
					break;
				case DATA_OUT:
					msg = "Data Out";
					break;
				default:
					msg = "UNKNOWN";
					break;
			}
			printf("\t\tData Direction: %s (%d)\n", msg,
				cmd_q->dir);

			printf("\t\tFlags: 0x%x\n", cmd_q->flags);
		}

		printf("\n");
	    }
	}

	return 0;
}


/*
 * Allocation routines for state structures
 */
ipi_softc_t *
ipi_master_alloc(unit, hw)
	int	unit;
	char	*hw;
{
	ipi_softc_t	*sc;

	if (unit < NIPI) {
		sc = &ipi_softc_data[unit];
		ipi_softc[unit]  = sc;
		sc->masterno	 = unit;
		sc->max_io_count = IPI_UNLIMITED_IO_COUNT;
		sc->hw_state	 = hw;
		return sc;
	}
	return 0;
}

target_info_t *
ipi_slave_alloc(unit, slave, hw)
	int	unit;
	int	slave;
	char	*hw;
{
	target_info_t	*tgt;
	int		min, max;

	tgt = &ipi_target_data[(unit * MAX_IPI_TARGETS) + slave];

	queue_init(&tgt->waiting_cmds);
	tgt->ior	  = 0;
	tgt->flags	  = TGT_ALIVE;
	tgt->hw_state	  = hw;
	tgt->cmd_q	  = NULL;	/* later */
	tgt->dev_ops	  = NULL;	/* later */
	simple_lock_init(&tgt->target_lock);
	tgt->masterno	  = unit;
	tgt->max_io_count = ipi_softc[unit]->max_io_count;
	tgt->target_id	  = slave;
	tgt->i_field	  = htonl(IPI_DEFAULT_I_FIELD);
	tgt->facility	  = IPI_DEFAULT_FACILITY;
	tgt->partition	  = IPI_DEFAULT_PARTITION;
	tgt->cmd_ref_min  = IPI_DEFAULT_CMD_REF_MIN;
	tgt->cmd_ref_len  = (IPI_DEFAULT_CMD_REF_MAX-IPI_DEFAULT_CMD_REF_MIN+1);
	tgt->block_size	  = IPI_DEFAULT_BLOCK_SIZE;
	tgt->open_count	  = 0;
	tgt->vendor	  = 0;

	ipi_softc[unit]->target[slave] = tgt;
	return tgt;
}


/*
 * Read the bootmagic configuration variables for the given target.
 * Use defaults if not set.
 */
ipi_get_bootmagic(tgt)
	target_info_t	*tgt;
{
	register struct boot_config	*config;

	/*
	 * First, read in all the values, then check them
	 * That way if one is bad we still have the good values that follow
	 * NOTE: i_field must be big endian for channel (htonl macro).
	 */

	/* I-field */
	config = &config_i_field[tgt->target_id];
	tgt->i_field = htonl(getbootint(config->name, config->value));

	/* facility base */
	config = &config_facility[tgt->target_id];
	tgt->facility = getbootint(config->name, config->value);

	/* partition */
	config = &config_partition[tgt->target_id];
	tgt->partition = getbootint(config->name, config->value);

	/* min and max command reference numbers */
	config = &config_cmd_ref_min[tgt->target_id];
	tgt->cmd_ref_min = getbootint(config->name, config->value);
	config = &config_cmd_ref_max[tgt->target_id];
	tgt->cmd_ref_len = getbootint(config->name, config->value); /* max */
	tgt->cmd_ref_len = tgt->cmd_ref_len - tgt->cmd_ref_min + 1;

	return 0;
}


/*
 * Check all the configuration variables for the given target.
 */
ipi_ret_t
ipi_config_error(tgt, warning)
	target_info_t	*tgt;
	boolean_t	warning;
{
	ipi_ret_t	ret;

	/*
	 * Check all the values
	 */
	if ((ret = ipi_config_check(tgt, IPISIFIELD,   warning)) != D_SUCCESS)
		return ret;
	if ((ret = ipi_config_check(tgt, IPISFACILITY, warning)) != D_SUCCESS)
		return ret;
	if ((ret = ipi_config_check(tgt, IPISPART,     warning)) != D_SUCCESS)
		return ret;
	if ((ret = ipi_config_check(tgt, IPISCMDREF,   warning)) != D_SUCCESS)
		return ret;

	return ret;
}


/*
 * Check the target configuration variable.
 */
ipi_ret_t
ipi_config_check(tgt, type, warning)
	target_info_t	*tgt;
	unsigned int	type;
	boolean_t	warning;
{
	ipi_softc_t	*sc = ipi_softc[tgt->masterno];
	target_info_t	*t;
	int		id;

	switch (type) {

		case IPISIFIELD:
		{
			for (id = 0; id < MAX_IPI_TARGETS; id++) {

				/*
				 * Skip un-initialized targets,
				 * dead targets, and the given target
				 */
				if (!(t = sc->target[id])   ||
				    !(t->flags & TGT_ALIVE) ||
				    (t == tgt))
					continue;

				/*
				 * A conflict occurs when the I-fields match
				 * and the conflicting target is open
				 */
				if (tgt->i_field == t->i_field) {
				    if (t->open_count > 0) {
					/*
					 * If the conflicting target is
					 * open this is a hard error.
					 */
			    		printf(
			"%s: IPI slave %d I-field conflicts with slave %d\n",
					  "ERROR", tgt->target_id, id);

					return D_ALREADY_OPEN;
				    } else if (warning) {
					/*
					 * The conflicting target is not open.
					 * We want to display a warning message
					 * then continue checking.
					 */
			    		printf(
			"%s: IPI slave %d I-field conflicts with slave %d\n",
					  "WARNING", tgt->target_id, id);
				    }
				}
			}
			break;
		}

		case IPISFACILITY:
		{
			/*
			 * Place holder for future facility checks
			 */
			break;
		}

		case IPISPART:
		{
			/*
			 * Place holder for future partition checks
			 */
			break;
		}

		case IPISCMDREF:
		{
			if ((tgt->cmd_ref_min > IPI_MAX_CMD_REF)       ||
			    ((tgt->cmd_ref_len - 1) > IPI_MAX_CMD_REF) ||
			    ((tgt->cmd_ref_min + tgt->cmd_ref_len - 1) >
			      IPI_MAX_CMD_REF)) {
				printf("\
%s: IPI slave %d command reference number range \
exceeds the maximum value of 0x%x\n",
					(warning) ? "WARNING" : "ERROR",
					tgt->target_id, IPI_MAX_CMD_REF);
				return D_INVALID_OPERATION;
			}
			/*
			 * There must be room for at least one
			 * normal command and one abort command.
			 */
			if (tgt->cmd_ref_len < 2) {
				printf("\
%s: IPI slave %d command reference number range must be >= 2\n",
					(warning) ? "WARNING" : "ERROR",
					tgt->target_id);
				return D_INVALID_OPERATION;
			}
			break;
		}

		default:
			return D_INVALID_OPERATION;
	}

	return D_SUCCESS;
}


/*
 * Allocation routine for command queues
 */
ipi_queue_alloc(tgt)
	target_info_t	*tgt;
{
	int		bytes;
	caddr_t		buf_ptr;
	cmd_queue_t	*cmd_q;
	register int	i;

	/*
	 * Allocate and zero fill the command queue data structures
	 */
	bytes = tgt->cmd_ref_len * sizeof(cmd_queue_t);
	if (kmem_alloc_wired(kernel_map, &tgt->cmd_q, bytes) != KERN_SUCCESS)
		return D_NO_MEMORY;
	bzero(tgt->cmd_q, bytes);

	/*
	 * Allocate and zero fill the command buffers
	 *
	 * For best performance, command buffers must
	 * be cache-line aligned and wired down
	 */
	bytes = tgt->cmd_ref_len * HIPPI_BURST_SIZE;
	if (kmem_alloc_wired(kernel_map, &buf_ptr, bytes) != KERN_SUCCESS)
		return D_NO_MEMORY;
	bzero(buf_ptr, bytes);

	/*
	 * Do the once only initialization
	 */
	cmd_q = tgt->cmd_q;
	for (i = 0; i < tgt->cmd_ref_len; i++, cmd_q++) {
		hctlr_src_t	*sreq;

		sreq = &cmd_q->sreq;

		/*
		 * Distribute the command buffers among the queues
		 *
		 * The hdr_ptr and cmd_ptr are the virtual address for the
		 * device driver.  The hdr_ptr points to the HiPPI-FP header
		 * and cmd_ptr points to the IPI-3 command buffer.
		 * The fb_ptr is the physical address for the HiPPI controller
		 */
		cmd_q->hdr_ptr = (hippi_fp_t *)
			((unsigned char *)buf_ptr + (i * HIPPI_BURST_SIZE));
		cmd_q->cmd_ptr = (caddr_t)
			((unsigned char *)cmd_q->hdr_ptr + sizeof(hippi_fp_t));
		sreq->fb_ptr = (unsigned char *)kvtophys(cmd_q->hdr_ptr);

		/*
		 * Set the source channel request i-field
		 * All others were zeroed above
		 */
		sreq->i_field = tgt->i_field;

		/*
		 * Initialize some other command queue structure fields
		 * All others were zeroed above
		 */
		cmd_q->state   = CMD_FREE;
		cmd_q->cmd_ref = tgt->cmd_ref_min + i;
		cmd_q->tgt     = tgt;
	}

	return D_SUCCESS;
}

/*
 * Free previously allocated command queues
 */
ipi_queue_free(tgt)
	target_info_t	*tgt;
{
	cmd_queue_t	*cmd_q = tgt->cmd_q;
	int		bytes;

	if (cmd_q == NULL)
		return 0;

	/*
	 * Must free the command buffers first
	 * The first buffer is the head of the list
	 */
	bytes = tgt->cmd_ref_len * HIPPI_BURST_SIZE;
	kmem_free(kernel_map, cmd_q->cmd_ptr, bytes);

	/*
	 * Now free the command queue data structures
	 */
	bytes = tgt->cmd_ref_len * sizeof(cmd_queue_t);
	kmem_free(kernel_map, cmd_q, bytes);

	/*
	 * Clear command queue field
	 */
	tgt->cmd_q = NULL;

	return 0;
}


/*
 * Locate and lock the next available command buffer
 */
cmd_queue_t *
ipi_get_cmd(tgt, camp_on)
	target_info_t	*tgt;
	boolean_t	camp_on;
{
	register int	i;
	cmd_queue_t	*cmd_q = tgt->cmd_q;
	int		depth  = tgt->cmd_ref_len;

	/*
	 * Return a pointer to the next available command buffer
	 */

	/* The last command buffer is reserved to allow aborts */
	depth--;

	for (i = 0; i < depth; i++, cmd_q++) {
		if (cmd_q->state == CMD_FREE) {
			cmd_q->state = CMD_LOCKED;
			return cmd_q;
		}
	}

	if (camp_on) {
		printf("%s%d: cannot locate a free command buffer\n",
			(tgt->dev_ops) ?
			    (*tgt->dev_ops->driver_name)(TRUE) : "Slave ",
			tgt->target_id);
	}

	return NULL;
}


/*
 * Locate the next response parameter
 */
ipi_param_t *
ipi_param(rsp, prm, id)
	ipi_response_t 	*rsp;
	ipi_param_t 	*prm;
	int		id;	
{
	int		total_length;

	/*
	 * Locate the given parameter id in the IPI response.
	 * The variable prm is the current offset in the response.
	 * If prm is NULL there is no offset and the search should
	 * start from the beginning of the response, otherwise the
	 * current parameter is skipped and the search begins at the
	 * new offset.  This allows the routine to be called multiple
	 * times to search for parameters and lets the caller just
	 * pass in what was returned from the previous call.
	 */

	total_length  = (rsp->packet_len_msb << 8) | rsp->packet_len_lsb;
	total_length += 2; /* we want the size of the length field included */

	/* Make sure there are some parameters */
	if (total_length == sizeof(ipi_response_t))
		return NULL;

	for (prm = (prm == NULL) ?
		   /* Start searching from the beginning of the response */
		   (ipi_param_t *)(rsp + 1) : 
		   /* Skip the current parameter and then start searching */
		   (ipi_param_t *)((unsigned char *)prm + prm->length + 1);

	     /* Make sure we didn't walk off the end */
	     ((unsigned char *)prm - (unsigned char *)rsp) < total_length;

	     /* Try the next parameter */
	     prm = (ipi_param_t *)((unsigned char *)prm + prm->length + 1)) {

		if (prm->id == id)
			return prm;
	}

	return NULL;
}


/*
 * Slave routine:
 *	See if the slave description (controller, unit, ..) matches one
 *	of the slaves found during probe.
 *
 * Implementation:
 *	Send out an ATTRIBUTES command to see what type of device the slave is.
 *
 * Notes:
 *	At this time the driver is fully functional and works off interrupts.
 */
ipi_slave(bus, reg)
	struct bus_device	*bus;
	unsigned int		reg;
{
	ipi_softc_t		*sc = ipi_softc[bus->ctlr];
	target_info_t		*tgt = sc->target[bus->slave];
	cmd_queue_t		*cmd_q;
	int			class;
	int			ret, retry;
	int			s;

	if (!tgt || !(tgt->flags & TGT_ALIVE))
		return 0;

	/* Might have scanned already */
	if (tgt->dev_ops)
		goto out;

	tgt->unit_no = bus->slave;

	/*
	 * Register the destination channel filter
	 */
	if (sc->filter(tgt->hw_state, TRUE) != D_SUCCESS) {
		printf("ipi%d: Error setting destination channel filter\n",
			bus->ctlr);
		tgt->flags = 0;
		return 0;
	}

	s = spl0();	/* we need interrupts */

	/*
	 * See if we know about this vendor's device
	 */
	if (!ipi_vendor(tgt, &class)) {
		ipi_response_t		*rsp;
		ipi_facilities_t	*facility;

		/* Get a command buffer */
		if ((cmd_q = IPI_GET_CMD(tgt, TRUE)) == NULL) {
			panic("ipi_slave");
			tgt->flags = 0;
			/*
			 * Clear the destination channel filter
			 */
			if (sc->filter(tgt->hw_state, FALSE) != D_SUCCESS) {
				printf("ipi%d: %s\n", bus->ctlr,
				  "Error clearing destination channel filter");
			}
			splx(s);
			return 0;
		}
		cmd_q->ior = NULL;

		/*
		 * Ok, we have to see what type of device this is.
		 * Send an ATTRIBUTES command with parameter id
		 * IPI_PARAM_SLAVE_FACILITIES (0x68).
		 */
		cmd_q->flags |= CMD_OPTIONAL;
		retry = 0;
		do {
			ret = ipi3_get_attribute(tgt, cmd_q,
				IPI_PARAM_SLAVE_FACILITIES);
		} while ((ret == IPI_RET_RETRY) && (retry++ <= ipi_retries));
		cmd_q->flags &= ~CMD_OPTIONAL;

		IPI_REL_CMD(cmd_q);	/* Free the command buffer */

		/* check for errors */
		if (ret != IPI_RET_SUCCESS) {
			if (ret == IPI_RET_ERROR)
				ipi3_error(tgt, cmd_q);
			printf("ipi%d: %s %d\n", bus->ctlr,
				"Error determining device type at IPI id",
				bus->slave);
			tgt->flags = 0;
			/*
			 * Clear the destination channel filter
			 */
			if (sc->filter(tgt->hw_state, FALSE) != D_SUCCESS) {
				printf("ipi%d: %s\n", bus->ctlr,
				  "Error clearing destination channel filter");
			}
			splx(s);
			return 0;
		}

		rsp = (ipi_response_t *)cmd_q->cmd_ptr;
		facility = (ipi_facilities_t *)(rsp + 1);
		class = facility->class;
	}

	switch (class) {
		case IPI_DISK    :
		case IPI_OPTICAL :
		case IPI_TAPE    :
			tgt->dev_ops = &ipi_devsw[class - 1];
			break;

		case IPI_COMMUNICATIONS :
		default:
			printf("ipi%d: %s %d (0x%02x)\n", bus->ctlr,
				"Unsupported device type at IPI id",
				bus->slave, class);
			ipi_display_attributes(tgt, class, NULL);
			tgt->flags = 0;
			/*
			 * Clear the destination channel filter
			 */
			if (sc->filter(tgt->hw_state, FALSE) != D_SUCCESS) {
				printf("ipi%d: %s\n", bus->ctlr,
				  "Error clearing destination channel filter");
			}
			splx(s);
			return 0;
	}

	/*
	 * Tell the user we know this target.
	 * See if we can be a bit smart about it.
	 */
	ipi_display_attributes(tgt, class, tgt->tgt_name);
	if (ipi_debug)
		ipi_display_attributes(tgt, class, NULL);
		
	/*
	 * Clear the destination channel filter
	 */
	if (sc->filter(tgt->hw_state, FALSE) != D_SUCCESS) {
		printf("ipi%d: %s\n", bus->ctlr,
			"Error clearing destination channel filter");
		tgt->flags = 0;
		splx(s);
		return 0;
	}

	splx(s);

out:
	return (strcmp(bus->name, (*tgt->dev_ops->driver_name)(TRUE)) == 0);
}


/*
 * Gather and display information about the slave
 */
ipi_display_attributes(tgt, class, name)
	target_info_t	*tgt;
	int		class;
	char		*name;
{
	cmd_queue_t		*cmd_q;
	ipi_response_t		*rsp;
	ipi_vendor_t		*ven;
	char			*devname;
	char			dev[IPI_TARGET_NAME_LEN];
	register int		i, j = 0;
	boolean_t		space;
	int			ret, retry;
	static char		*periph_names[5] = {
		"device",	/* unknown */
		"disk",
		"optical",
		"tape",
		"communication"
	};

	/* Get a command buffer */
	if ((cmd_q = IPI_GET_CMD(tgt, TRUE)) == NULL) {
		panic("ipi_display_attributes");
		return;
	}
	cmd_q->ior = NULL;

	/*
	 * Get vendor ID information
	 */
	retry = 0;
	do {
		ret = ipi3_get_attribute(tgt, cmd_q, IPI_PARAM_VENDOR_ID);
	} while ((ret == IPI_RET_RETRY) && (retry++ <= ipi_retries));

	/* check for errors */
	if (ret != IPI_RET_SUCCESS) {
		printf("slave %d: Error determining vendor ID\n",
			tgt->target_id);
		IPI_REL_CMD(cmd_q);	/* Free the command buffer */
		return;
	}

	rsp = (ipi_response_t *)cmd_q->cmd_ptr;
	ven = (ipi_vendor_t *)(rsp + 1);

	devname = name ? name : dev;

	/* Manufacturer Identification */
	for (space = TRUE, i = 0; i < 16; i++) {
		if (ven->identification[i] == ' ') {
			if (space) continue;
			else space = TRUE;
		} else space = FALSE;
		devname[j++] = ven->identification[i];
	}
	if (!space) devname[j++] = ' ';

	/* Manufacturer Model Number */
	for (space = TRUE, i = 0; i < 8; i++) {
		if (ven->model[i] == ' ') {
			if (space) continue;
			else space = TRUE;
		} else space = FALSE;
		devname[j++] = ven->model[i];
	}
	if (!space) devname[j++] = ' ';

	/* Manufacturer Revision Number */
	for (space = TRUE, i = 0; i < 4; i++) {
		if (ven->revision[i] == ' ') {
			if (space) continue;
			else space = TRUE;
		} else space = FALSE;
		devname[j++] = ven->revision[i];
	}
	if (space) j--;		/* no spaces at the end of the string */
	devname[j] = '\0';

	if (!name) {
		ipi_slave_config_t		*scfg;
		ipi_addressee_config_t		*acfg;
		
		/*
		 * If name pointer is NULL this is an unsupported device
		 * Display as much useful information as we can
		 */

		printf(" slave %d: (%s, IPI-3 %s)", tgt->target_id, devname,
			(class >= 5) ? "device" : periph_names[class]);

		retry = 0;
		do {
			ret = ipi3_get_attribute(tgt, cmd_q,
				IPI_PARAM_SLAVE_CONFIGURATION);
		} while ((ret == IPI_RET_RETRY) && (retry++ <= ipi_retries));

		/* check for errors */
		if (ret != IPI_RET_SUCCESS) {
			printf(
"slave %d: Error determining slave configuration\n",
				tgt->target_id);
			IPI_REL_CMD(cmd_q);	/* Free the command buffer */
			return;
		}

		scfg = (ipi_slave_config_t *)(rsp + 1);

		printf("\n\tSupports: ");
		if (scfg->mux_data_transfers)
		    printf("\n\t\tMultiplexed Data Transfers");
		if (scfg->extended_substatus)
		    printf("\n\t\tExtended Substatus");
		if (scfg->master_termination)
		    printf("\n\t\tMaster Command Termination");
		if (scfg->odd_transfers)
		    printf("\n\t\tOdd Octet Transfers");
		if (scfg->alias_addressing)
		    printf("\n\t\tAlias Addressing");
		if (scfg->synonym_addressing)
		    printf("\n\t\tSynonym Addressing");
		if (scfg->facility_transfers)
		    printf("\n\t\tFacility-Faciltiy Transfers");
		if (scfg->facilities_different)
		    printf("\n\t\tFacilities of Different Classes");
		if (scfg->interlock_transfers)
		    printf("\n\t\tInterlock Data Transfers");
		if (scfg->streaming_transfers)
		    printf("\n\t\tStreaming Data Transfers");
		if (scfg->multi_cmd_extents)
		    printf("\n\t\tMultiple Command Extents");
		if (scfg->master_throttling)
		    printf("\n\t\tMaster Throttling");
		if (scfg->facility_config)
		    printf("\n\t\tFacility Configuration");
		if (scfg->defineable_part)
		    printf("\n\t\tMaster-Defineable Maintenance Partitions");
		if (scfg->imbedded_data)
		    printf("\n\t\tImbedded Data Responses");
		if (scfg->transfer_notification)
		    printf("\n\t\tTransfer Notification Packets");
		if (scfg->level_2)
		    printf("\n\t\tLevel 2 Operations");

		retry = 0;
		do {
			ret = ipi3_get_attribute(tgt, cmd_q,
				IPI_PARAM_ADDRESSEE_CONFIGURATION);
		} while ((ret == IPI_RET_RETRY) && (retry++ <= ipi_retries));

		/* check for errors */
		if (ret != IPI_RET_SUCCESS) {
			printf(
"slave %d: Error determining addressee configuration\n",
				tgt->target_id);
			IPI_REL_CMD(cmd_q);	/* Free the command buffer */
			return;
		}

		acfg = (ipi_addressee_config_t *)(rsp + 1);
		if (acfg->min_queued > 0)
		    printf("\n\t\t%d guaranteed queued commands",
			acfg->min_queued);
		if (acfg->max_queued == 1)
		    printf("\n\t\tNo command queuing");
		printf("\n\t\tCommand stack size of %d", acfg->cmd_stack);

		printf("\n");
	}

	IPI_REL_CMD(cmd_q);	/* Free the command buffer */
}


/*
 * Attach routine:
 *	Fill in all the relevant per-slave data and make the slave operational.
 *
 * Implementation:
 *	Call the optimization routine to set up the device.
 */
ipi_attach(bus)
	register struct bus_device	*bus;
{
	ipi_softc_t		*sc = ipi_softc[bus->mi->unit];
	target_info_t		*tgt = sc->target[bus->slave];
	int			s;

	printf(" (%s %s)", (*tgt->dev_ops->driver_name)(FALSE), tgt->tgt_name);

	/*
	 * Register the destination channel filter
	 */
	if (sc->filter(tgt->hw_state, TRUE) != D_SUCCESS) {
		printf("\nipi%d: Error setting destination channel filter\n",
			bus->ctlr);
		tgt->flags = 0;
		return 0;
	}

	s = spl0();	/* we need interrupts */

	if (tgt->dev_ops->optimize)
		(*tgt->dev_ops->optimize)(tgt);
	else
		tgt->flags |= TGT_FULLY_PROBED;

	/*
	 * Clear the destination channel filter
	 */
	if (sc->filter(tgt->hw_state, FALSE) != D_SUCCESS) {
		printf("ipi%d: %s\n", bus->ctlr,
			"Error clearing destination channel filter");
		tgt->flags = 0;
		return 0;
	}

	splx(s);
}


/*
 * Probe routine:
 *	See if a device answers.  Used AFTER auto-configuration.
 *
 * Implementation:
 *	First ask the HBA to see if anyone is there at all, then
 *	call the ipi_slave and ipi_attach routines with a fake bus parameter.
 */
ipi_ret_t
ipi_probe(sc, tgt_ptr, dev, ior)
	ipi_softc_t		*sc;
	target_info_t		**tgt_ptr;
	int			dev;
	io_req_t		ior;
{
	struct bus_device	bus;
	target_info_t		*tgt;
	wd_t			*wdog;
	cmd_queue_t		*cmd_q;
	int			target_id;
	ipi_ret_t		ret;

	target_id = IPI_SLAVE(dev);

	if (!sc->probe || target_id >= MAX_IPI_TARGETS)
		return D_NO_SUCH_DEVICE;	/* sanity */

	/*
	 * Make sure we have a target descriptor
	 */
	if (sc->target[target_id] == 0) {
		/* Allocate a target descriptor */
		tgt = ipi_slave_alloc(sc->masterno, target_id, sc->hw_state);

		/* Read bootmagic variables */
		ipi_get_bootmagic(tgt);
	} else
		tgt = sc->target[target_id];

	/* Check for configuration errors */
	if ((ret = ipi_config_error(tgt, FALSE)) != D_SUCCESS) {
		return ret;
	}

	tgt->flags = 0;	/* we don't know yet */

	/* Clear the target watchdog */
	wdog = &((watchdog_t *)tgt->hw_state)->dog[target_id];
	wdog->nactive = 0;

	/* Allocate command buffers if needed */
	if ((tgt->cmd_q == NULL) && ipi_queue_alloc(tgt)) {
		printf("Cannot allocate command buffers\n");
		return D_NO_MEMORY;
	}

	/* Get a command buffer */
	if ((cmd_q = IPI_GET_CMD(tgt, TRUE)) == NULL) {
		panic("ipi_probe");
		return D_DEVICE_DOWN;
	}
	cmd_q->ior = NULL;

	/* Mildly inquire */
	if (!(sc->probe)(tgt, cmd_q)) {
		IPI_REL_CMD(cmd_q);
		goto fail;
	}

	IPI_REL_CMD(cmd_q);

	/* There is something there, see what it is */
	bzero(&bus, sizeof(bus));
	bus.ctlr = sc->masterno;
	bus.unit = bus.slave = target_id;
	bus.name = "";	/* we don't know what name to look for */

	/* This fails on the name for sure */
	(void)ipi_slave(&bus, 0 /* brrrr */);
	if ((tgt->flags & TGT_ALIVE) == 0)
		goto fail;

	{
		struct bus_ctlr	mi;

		mi.unit = sc->masterno;
		bus.mi = &mi;
		printf("%s at slave %d ",
			(*tgt->dev_ops->driver_name)(TRUE), target_id);
		ipi_attach(&bus);
		printf("\n");
		if ((tgt->flags & TGT_ALIVE) == 0)
			goto fail;
	}

	*tgt_ptr = tgt;
	return D_SUCCESS;

fail:
	tgt->flags = 0;
	return D_NO_SUCH_DEVICE;
}


/*
 * Watchdog routine:
 *	Retry commands if a target holds up for too long.
 *
 * Implementation:
 *	Each HBA that wants to use this must have a watchdog_t structure
 *	at the head of its hardware descriptor.  A variable is set by
 *	this periodic routine and reset on command activity. If it is
 *	not reset on time (some ten seconds or so) we retry the command.
 */
ipi_watchdog(hw)
	watchdog_t	*hw;
{
	register int	target_id;
	register wd_t	*wdog;
	int		s = splipi();

	for (target_id = 0; target_id < MAX_IPI_TARGETS; target_id++) {
		wdog = &hw->dog[target_id];

		switch (wdog->state) {

			case IPI_WD_EXPIRED:	/* Woof! Woof! Grrrrrrrrrr! */
				/* double check first */
				if (wdog->nactive == 0) {
					wdog->state = IPI_WD_INACTIVE;
					break;
				}

				if (ipi_debug)
					printf(
					    "IPI slave %d watchdog expired\n",
						target_id);
				wdog->nactive = 0;
				wdog->state   = IPI_WD_INACTIVE;
				(*hw->reset)(hw, target_id);
				break;

			case IPI_WD_ACTIVE:	/* Scratch, Scratch...Sniff */
				wdog->state = IPI_WD_EXPIRED;
				break;

			case IPI_WD_INACTIVE:	/* Zzzzzzzzzzzzzzzzzzzzz... */
			default:
				break;
		}
	}

	/* Do this here, fends against powered down devices */
	if (ipi_watchdog_period != 0)
		timeout(ipi_watchdog, hw, ipi_watchdog_period * hz);

	splx(s);
}


/*
 * Reset Notification:
 *	Called when the HBA sees a reset interrupt
 *
 * Implementation:
 *	Restart whatever operation was in progress for that target.
 */
ipi_was_reset(cmd_q)
	cmd_queue_t	*cmd_q;
{
	register target_info_t	*tgt = cmd_q->tgt;

	/*
	 * Notify the target command of the accident
	 */
	cmd_q->result = IPI_RET_ABORTED | IPI_RET_RETRY;
	cmd_q->state  = CMD_COMPLETE;

	if (tgt->dev_ops)
		(*tgt->dev_ops->restart)(tgt, cmd_q, TRUE);
}

#endif	NIPI > 0
