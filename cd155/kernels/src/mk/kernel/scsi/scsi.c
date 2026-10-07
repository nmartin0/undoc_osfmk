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
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
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
 * $Log: scsi.c,v $
 * Revision 1.17  1995/03/28  01:38:05  jerrie
 * Initialize tgt->unit_no using the correct auto-config field.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low.
 *  Benefit or PTS #: 12827
 *  Testing:	   Developer
 *  Module(s):	   See PTS report.
 *
 * Revision 1.16  1995/03/14  23:48:45  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.15  1994/11/18  20:59:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.14  1994/10/28  17:52:03  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: 10964
 *  Testing: reset a 3480 during an MTOFFL ioctl
 *  Module(s): scsi_bus_was_reset()
 *
 * Revision 1.13  1994/08/31  21:25:55  mtm
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
 * Revision 1.11.8.1  1994/08/04  16:11:28  richardg
 *  Reviewer: Jerry Coffman
 *  Risk: Medium
 *  Benefit or PTS #: 10171
 *  Testing: ran Tensor's tapewrt program simultaneously against both LUNs.
 *  Module(s): scsi_slave_alloc, scsi_slave, scsi_attach, scsi_probe
 *
 * Revision 1.11  1994/02/10  18:56:43  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #:8112
 *  Testing: Tape EAT's, repeated booting with a tape loaded.
 *  Module(s): scsi_attach().
 *
 * Don't send a SCSI start unit command to tapes.  Exabytes are so slow responding
 * if they have a tape loaded, it will crash the booting of the mesh.  The start
 * unit occurs during the open also so there is no loss.
 *
 * Revision 1.10  1993/09/24  22:53:18  jerrie
 * Added support for SCSI media changer (jukebox) devices.
 *
 * Revision 1.9  1993/09/16  18:12:06  richardg
 * modified the memory allocation to evenly divide available MIO memory between existing scsi devices
 *
 * Revision 1.8  1993/08/09  23:34:31  jerrie
 * Changed the default of bootmagic variable BOOT_SEND_SCSI_START_UNIT to send
 * the start unit command, as requested by System Hardware Engineering.
 *
 * Revision 1.7  1993/06/30  22:53:48  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.6  1993/06/08  22:24:31  jerrie
 * dded scdisk_close routine to devs_ops tables for all device types using
 * disk routines.  Added lun parameter to scsi_probe and set tgt->lun.
 *
 * Revision 1.5  1993/05/12  19:05:09  jerrie
 * Fix for bug 5099.  Initialize the tgt->lun field to zero.
 * Added bootmagic variable BOOT_SEND_SCSI_START_UNIT to enable/disable
 * sending a start unit command at autoconf time.  The default is not
 * to send a start unit command.
 *
 * Revision 1.4  1993/04/27  20:47:56  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:53:23  dleslie
 * First R1_0 release
 *
 * Revision 2.12.2.1  92/03/28  10:15:49  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:32:37  jeffreyh]
 * 
 * Revision 2.13  92/02/23  22:44:40  elf
 * 	Changed the interface of a number of functions not to
 * 	require the scsi_softc pointer any longer.  It was
 * 	mostly unused, now it can be found via tgt->masterno.
 * 	[92/02/22  19:30:58  af]
 * 
 * Revision 2.12  91/08/24  12:28:21  af
 * 	Spls defs, corrected scsi_master_alloc() to allocate
 * 	as expected, support for processor devices, 
 * 	made it possible to avoid sync neg at all (for rb's sake),
 * 	no start unit on processor devices, understand our
 * 	own descriptor.
 * 	[91/08/02  04:01:50  af]
 * 
 * Revision 2.11  91/07/09  23:22:46  danner
 * 	Added gross luna88k ifdef to use <sd.h> instead of <scsi.h>. Will be
 * 	fixed when I understand how to use the configuration tools.
 * 	[91/07/09  11:08:15  danner]
 * 
 * Revision 2.10  91/06/25  20:56:48  rpd
 * 	Tweaks to make gcc happy.
 * 
 * Revision 2.9  91/06/19  11:57:17  rvb
 * 	mips->DECSTATION; vax->VAXSTATION
 * 	[91/06/12  14:02:21  rvb]
 * 
 * 	File moved here from mips/PMAX since it is now "MI" code, also
 * 	used by Vax3100 and soon -- the omron luna88k.
 * 	[91/06/04            rvb]
 * 
 * 	Printing of 'unsupported..' in scsi_slave was screwed cuz
 * 	ui->mi and ui->unit could be bogus at that point in time.
 * 	[91/05/30            af]
 * 
 * Revision 2.8  91/05/14  17:28:00  mrt
 * 	Correcting copyright
 * 
 * Revision 2.7  91/05/13  06:04:45  af
 * 	Added flags to control use of disconnect-reconnect mode.
 * 	Added device-specific close routine (for tapes).
 * 	Wait for start_unit to complete properly and tell the user
 * 	what we are waiting for.
 * 	[91/05/12  16:21:16  af]
 * 
 * Revision 2.6.1.1  91/03/29  16:59:15  af
 * 	Added flags to control use of disconnect-reconnect mode.
 * 	Added device-specific close routine (for tapes).
 * 	Wait for start_unit to complete properly and tell the user
 * 	what we are waiting for.
 * 
 * Revision 2.6  91/02/05  17:44:35  mrt
 * 	Added author notices
 * 	[91/02/04  11:18:01  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:16:40  mrt]
 * 
 * Revision 2.5  90/12/05  23:34:36  af
 * 	Cleanups, still awaits SCSI-2 fixes.
 * 	[90/12/03  23:38:14  af]
 * 
 * Revision 2.3.1.1  90/11/01  03:38:17  af
 * 	Created.
 * 	[90/09/03            af]
 */
/*
 *	File: scsi.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Middle layer of the SCSI driver: chip independent functions
 *	This file contains Controller and Device-independent functions
 */

#include <scsi.h>

#if	NSCSI > 0
#include <platforms.h>

#include <machine/machparam.h>		/* spl definitions */

#include <mach/std_types.h>
#include <sys/types.h>
#include <scsi/compat_30.h>

#include <chips/busses.h>
#include <scsi/scsi.h>
#include <scsi/scsi2.h>
#include <scsi/scsi_defs.h>
#include <machine/machparam.h>

#ifdef	VAXSTATION
/* We run some of this code on the interrupt stack */
#undef	spl0
#define	spl0()	spl1()
#endif	/*VAXSTATION*/

/*
 *	Overall driver state
 */

		/* per target state */
target_info_t	scsi_target_data[NSCSI*MAX_SCSI_TARGETS][MAX_LUNS];

scsi_softc_t	scsi_softc_data[NSCSI];		/* per HBA state */
scsi_softc_t	*scsi_softc[NSCSI];	/* quick access&checking */

/*
 * If a specific target should NOT be asked to go synchronous or wide
 * then its bit in this bitmap should be set.  Each SCSI controller
 * (Host Bus Adapter) can support at most MAX_SCSI_TARGETS targets
 * Use one short per controller.  A bit set to one means NO negotiation.
 * Patch with adb if necessary.
 */
unsigned short	scsi_no_synchronous_xfer[NSCSI];
unsigned short	scsi_no_wide_xfer[NSCSI];

/*
 * For certain targets it is wise to use the long form of the
 * read/write commands even if their capacity would not necessitate
 * it.  Same as above for usage.
 */
unsigned short	scsi_use_long_form[NSCSI];

/*
 * Control over disconnect-reconnect mode.
 */
unsigned short	scsi_might_disconnect[NSCSI] = /* do it if deemed appropriate */
		{ 0xffff, 0xffff};	       /* Fix by hand viz NSCSI */
unsigned short	scsi_should_disconnect[NSCSI] = /* just do it */
		{ 0, 0 };

/*
 * Control over tagged command queuing.
 */
unsigned short	scsi_should_queue[NSCSI] =	/* control of cmd queuing */
		{ 0xffff, 0xffff };

unsigned char	scsi_initiator_id[NSCSI] =	/* our id on the bus(ses) */
		{ 7, 7 };

/*
 * Miscellaneus config
 */
boolean_t	scsi_exabyte_filemarks = FALSE; /* use short filemarks */
int		scsi_watchdog_period = 20;	/* but exabyte needs >=30 for bspace */
int		scsi_delay_after_reset = 5000000;/* microseconds */
boolean_t	scsi_no_automatic_bbr = FALSE;	/* revector bad blocks automatically */

#ifdef	MACH_KERNEL
#else
/* This covers Exabyte's max record size */
unsigned int	scsi_per_target_virtual = 256*1024;
#endif	MACH_KERNEL


/*
 * Device-specific operations are switched off this table
 */

extern char
		*scdisk_name(),	*sctape_name(),	*scprt_name(),
		*sccpu_name(),	*scworm_name(),	*sccdrom_name(),
		*scscn_name(),	*scmem_name(),	*scjb_name(),
		*sccomm_name();
extern scsi_ret_t
		scdisk_optimize(),	sctape_optimize(),
		scprt_optimize(),	scscn_optimize(),
		scmem_optimize(),	scjb_optimize(),
		sccomm_optimize();
extern scsi_ret_t
		scdisk_open(),		scdisk_close(),
		sctape_open(),		sctape_close(),
		scjb_open(),		scjb_close();
extern int
		scdisk_strategy(),	sctape_strategy(),
		sccpu_strategy(),	scjb_strategy();
extern int
		scdisk_start(),		sctape_start(),
		sccpu_start(),		scjb_start();
extern io_return_t
		scdisk_set_status(),	scdisk_get_status(),
		sctape_set_status(),	sctape_get_status(),
		scjb_set_status(),	scjb_get_status();

scsi_devsw_t	scsi_devsw[] = {
/* SCSI_DISK */		{ scdisk_name, scdisk_optimize,
			  scdisk_open, scdisk_close,
			  scdisk_strategy, scdisk_start,
			  scdisk_get_status, scdisk_set_status },
/* SCSI_TAPE */		{ sctape_name, sctape_optimize,
			  sctape_open, sctape_close,
			  sctape_strategy, sctape_start,
			  sctape_get_status, sctape_set_status },
/* SCSI_PRINTER */	{ scprt_name, scprt_optimize, /*XXX*/},
/* SCSI_CPU */		{ sccpu_name, 0, 0, 0, sccpu_strategy, sccpu_start,},
/* SCSI_WORM */		{ scworm_name, scdisk_optimize,
			  scdisk_open, scdisk_close,
			  scdisk_strategy, scdisk_start,
			  scdisk_get_status, scdisk_set_status },
/* SCSI_CDROM */	{ sccdrom_name, scdisk_optimize,
			  scdisk_open, scdisk_close,
			  scdisk_strategy, scdisk_start,
			  scdisk_get_status, scdisk_set_status },
/* SCSI_SCANNER */	{ scscn_name, scscn_optimize, /*XXX*/ },
/* SCSI_MEMORY */	{ scmem_name, scdisk_optimize,
			  scdisk_open, scdisk_close,
			  scdisk_strategy, scdisk_start,
			  scdisk_get_status, scdisk_set_status },
/* SCSI_J_BOX */	{ scjb_name, scjb_optimize,
			  scjb_open, scjb_close,
			  scjb_strategy, scjb_start,
			  scjb_get_status, scjb_set_status },
#ifdef	SCSI2
/* SCSI_COMM */		{ sccomm_name, sccomm_optimize, /*XXX*/ },
#endif	SCSI2
			0
};

/*
 * Allocation routines for state structures
 */
scsi_softc_t *
scsi_master_alloc(unit, hw)
	char	*hw;
{
	scsi_softc_t	*sc;

	if (unit < NSCSI) {
		sc = &scsi_softc_data[unit];
		scsi_softc[unit] = sc;
		sc->masterno = unit;
		sc->hw_state = hw;
		return sc;
	}
	return 0;
}

target_info_t *
scsi_slave_alloc(unit, slave, hw)
	char	*hw;
{
	target_info_t	*tgt;
	int		i;

	
	/* loop through and init all possible LUNS */
	for(i = 0; i < MAX_LUNS; i++) {
		tgt = &scsi_target_data[(unit * MAX_SCSI_TARGETS) + slave][i];
		tgt->hw_state = hw;
		tgt->dev_ops = 0;	/* later */
		tgt->target_id = slave;
		tgt->true_lun = i; /* assign the LUN # */
		tgt->lun = i; /* assign the LUN # */
		tgt->masterno = unit;
		tgt->block_size = 1;	/* default */
		tgt->flags = TGT_ALIVE;
		tgt->sync_period = 0;
		tgt->sync_offset = 0;
		tgt->tag_q = NULL;
		queue_init(&tgt->free_tag_q);
		queue_init(&tgt->active_tag_q);
		simple_lock_init(&tgt->target_lock);

		scsi_softc[unit]->target[slave][i] = tgt;
	}

	/* return LUN 0 of the target ID */
	return scsi_softc[unit]->target[slave][0];
}

/*
 * Slave routine:
 *	See if the slave description (controller, unit, ..)
 *	matches one of the slaves found during probe
 *
 * Implementation:
 *	Send out an INQUIRY command to see what sort of device
 *	the slave is.
 * Notes:
 *	At this time the driver is fully functional and works
 *	off interrupts.
 * TODO:
 *	The SCSI2 spec says what exactly must happen: see F.2.3
 */
scsi_slave( ui, reg)
	struct bus_device	*ui;
	unsigned		reg;
{
	scsi_softc_t		*sc = scsi_softc[ui->ctlr];
	target_info_t		*tgt;
	scsi2_inquiry_data_t	*inq;
	int			ptype, pqual, scsi_std, s, i;

  s = spl0();	/* we need interrupts */
  for (i = 0; i < MAX_LUNS; i++) {

	tgt = sc->target[ui->slave][i];

	if (!tgt || !(tgt->flags & TGT_ALIVE))
		continue;

	/*
	 * May need to scan even if the target has previously been scanned.
	 * The dev_ops will be present but flags will only mark the target
	 * alive if called from the scsi_probe() routine.  Scanning again
	 * will determine if the device supports command queuing and cause
	 * the attach optimize function to assign the correct number of
	 * queue tags for the device.
	 *
	 * Might have scanned already
	 */
	 if (tgt->dev_ops && (tgt->flags & TGT_FULLY_PROBED))
	 	goto out;

#ifdef	SCSI2
	This is what should happen:
	- for all LUNs 
		INQUIRY
		scsi_verify_state (see)
		scsi_initialize (see)
#endif	SCSI2

	/*
	 * Set the target unit number to the bus device unit number.
	 * This allows us to distinguish between multiple SCSI busses
	 * by initializing the device unit differently for each bus.
	 */
	tgt->unit_no = ui->unit;

	if (BGET(scsi_no_wide_xfer,sc->masterno,tgt->target_id))
		tgt->flags |= TGT_DID_WIDE;

	if (BGET(scsi_no_synchronous_xfer,sc->masterno,tgt->target_id))
		tgt->flags |= TGT_DID_SYNCH;

	/*
	 * Ok, it is time to see what type of device this is, send an
	 * INQUIRY cmd and wait till done.  Must do the wide and sync
	 * negotiation here so we can send an identify message to select
	 * the LUN.
	 */
	/* Do wide bus  negotiation */
	if (!(tgt->flags & TGT_DID_WIDE))  scsi_test_unit_ready(tgt, 0);
	/* Do sync xfer negotiation */
	if (!(tgt->flags & TGT_DID_SYNCH)) scsi_test_unit_ready(tgt, 0);
	scsi_inquiry(tgt, SCSI_INQ_STD_DATA);

	inq = (scsi2_inquiry_data_t*)tgt->cmd_ptr;
	ptype = (inq->periph_type);
	pqual = (inq->periph_qual);

	if (pqual != SCSI_PERIPH_CONNECTED) { /* no LUN or not alive */
		tgt->flags = 0;
		continue;
	}

	switch (ptype) {
	case SCSI_DISK :
		{
		    /*
		     * Weird stuff for disk.  The partition field in the minor
		     * number is also the LUN field.  We're not supporting LUNs
		     * on disks yet, so we point each target/lun structure to
		     * target/lun 0.
		     */
		    int	j;

		    for (j = 1; j < MAX_LUNS; j++)
			sc->target[ui->slave][j] = sc->target[ui->slave][0];
		    i = MAX_LUNS;	/* no need to keep looping */
		}
		tgt->dev_ops = &scsi_devsw[ptype];
		break;
	case SCSI_TAPE :
		tgt->dev_ops = &scsi_devsw[ptype];
		break;
	case SCSI_PRINTER :
	case SCSI_CPU :
	case SCSI_WORM :
	case SCSI_CDROM :
	case SCSI_MEMORY :
	case SCSI_J_BOX :
#ifdef	SCSI2
	case SCSI_SCANNER :
	case SCSI_COMM :
/*	case SCSI_PREPRESS1 : reserved, really
	case SCSI_PREPRESS2 :	*/
#endif	SCSI2
		tgt->dev_ops = &scsi_devsw[ptype];
		break;
	default:
		printf("scsi%d: %s %d (0x%x). ", ui->ctlr,
		       "Unsupported device type at SCSI id", ui->slave,
			inq->periph_type);
		scsi_print_inquiry(inq, SCSI_INQ_STD_DATA, 0);
		tgt->flags = 0;
		splx(s);
		return 0;
	}

	if (inq->rmb)
		tgt->flags |= TGT_REMOVABLE_MEDIA;

	/*
	 * Tell the user we know this target, then see if we
	 * can be a bit smart about it.
	 */
	scsi_print_inquiry(inq, SCSI_INQ_STD_DATA, tgt->tgt_name);
	if (scsi_debug)
		scsi_print_inquiry(inq, SCSI_INQ_STD_DATA, 0);

	/*
	 * The specification says that even in SCSI-1 mode,
	 * the target should respond with the full SCSI-2
	 * inquiry data set.  See if the device supports
	 * the SCSI-2 standard, then check the SCSI-2
	 * inquiry data that we care about.
	 */
	scsi_std = (inq->ansi == 2 || inq->response_fmt == 2) ? 2 : 1;

	if (scsi_std == 2) {
		if (inq->CmdQue) {
		    tgt->flags |= TGT_TAGGED_QUEUING;
		}
		if (!inq->Wbus16 && !inq->Wbus32) {
		    BSET(scsi_no_wide_xfer,sc->masterno,tgt->target_id);
		    tgt->flags |= TGT_DID_WIDE;
		}
		if (!inq->Sync) {
		    BSET(scsi_no_synchronous_xfer,sc->masterno,tgt->target_id);
		    tgt->flags |= TGT_DID_SYNCH;
		}
	}

#ifdef	SCSI2
	/*
	 * The above says if it currently behaves as a scsi2,
	 * however scsi1 might just be the default setting.
	 * The spec say that even if in scsi1 mode the target
	 * should answer to the full scsi2 inquiry spec.
	 */
	scsi_std = (inq->ansi == 2 || inq->response_fmt == 2) ? 2 : 1;
#if nosey
	if (scsi_std == 2) {
		unsigned char	supp_pages[256], i;
		scsi2_impl_opdef_page_t	*impl;

		scsi_inquiry(tgt, SCSI_INQ_SUPP_PAGES);
		impl = (scsi2_impl_opdef_page_t	*)inq;
		npages = impl->page_len - 2;
		bcopy(impl->supp_opdef, supp_pages, npages);

		for (i = 0; i < npages; i++) {
			scsi_inquiry(tgt, supp_pages[i]);
			scsi_print_inquiry(inq, supp_pages[i], 0);
		}
	}
#endif	nosey

	if (scsi_std == 2) {
		scsi2_impl_opdef_page_t	*impl;
		int i;

		scsi_inquiry(tgt, SCSI_INQ_IMPL_OPDEF);
		impl = (scsi2_impl_opdef_page_t	*)inq;
		for (i = 0; i < impl->page_len - 2; i++)
			if (impl->supp_opdef[i] == SCSI2_OPDEF) {
				scsi_change_definition(tgt, SCSI2_OPDEF);
				/* if success .. */
					tgt->flags |= TGT_SCSI_2_MODE;
				break;
			}
	}
#endif	SCSI2

  }
out:
	splx(s);
	if (!(tgt = sc->target[ui->slave][0]))	/* set tgt back to LUN 0 */
		return 0;

	return (strcmp(ui->name, (*tgt->dev_ops->driver_name)(TRUE)) == 0);
}

#ifdef	SCSI2
scsi_verify_state(...)
{
verify_state: send test_unit_ready up to 3 times, each time it fails
(with check condition) send a requeste_sense. It is ok to get UNIT ATTENTION 
the first time only, NOT READY the second, only GOOD the last time.
If you get BUSY or RESERVATION CONFLICT retry.
}

scsi_initialize(...)
{

initialize: send start_unit with immed=0 (->disconnect), if fails
with check condition send requeste_sense and if "illegal request"
proceed anyways. Retry on BUSY.
Do a verify_state, then
disks:
	- mode_sense (current) if ANSI2 or needed by vendor (!!!!)
	  and if check-condition&illegal-request goto capacity
	- mode_sense (changeable)
	- if needed do a mode_select (yes, 512)
	- read_capacity
tapes:

}
#endif	SCSI2

/*
 * Attach routine:
 *	Fill in all the relevant per-slave data and make
 *	the slave operational.
 *
 * Implementation:
 *	Get target's status, start the unit and then
 *	switch off to device-specific functions to gather
 *	as much info as possible about the slave.
 */
scsi_attach(ui)
	register struct bus_device *ui;
{
	scsi_softc_t		*sc = scsi_softc[ui->mi->unit];
	target_info_t		*tgt;
	target_info_t		*ptgt = NULL; /* previous tgt */
	int			s, i, j;

  s = spl0();
  for (j = 0; j < MAX_LUNS; j++) {
	tgt = sc->target[ui->slave][j];
	if (!tgt || !(tgt->flags & TGT_ALIVE) || j != tgt->true_lun)
		continue;

	/* sense return from inquiry */
	/*
	 * not really needed and can garble the device id sign on strings
	 *
	 * scsi_request_sense(tgt, 0, 0);
	 */

#if	PARAGON860
	if (getbootint("BOOT_SEND_SCSI_START_UNIT", 1))
#endif	PARAGON860

	/*
	 * Do this twice, certain targets need it
	 */
	if ((tgt->dev_ops != &scsi_devsw[SCSI_CPU])    &&
	    (tgt->dev_ops != &scsi_devsw[SCSI_J_BOX])  &&
	    (tgt->dev_ops != &scsi_devsw[SCSI_TAPE])) { /* don't do tapes */
						        /* cause the exabytes
						 	 * are sooo slow */
		(void) scsi_start_unit(tgt, SCSI_CMD_SS_START, 0);
		i = 0;
		while (scsi_start_unit(tgt, SCSI_CMD_SS_START, 0) == SCSI_RET_RETRY) {
			if (i++ == 5)
				printf(".. not yet online ..");
			delay(1000000);
			if (i == 60) {
				printf(" seems hopeless.");
				break;
			}
		}
		if ((i >= 5) && (i < 60))
			printf(" ready.");
	}

	/*
	 * See if it is up and about
	 */
	if (scsi_test_unit_ready(tgt, 0)) {
		/* print the target info */
		if (!ptgt || (tgt->dev_ops != ptgt->dev_ops)) {
			if (ptgt) {
				printf("\n");
				/* space over to align device id strings */
				i  = 16;
				i += strlen(ui->name) +
				     strlen(ui->driver->mname);
				if (ui->unit  > 9) i++;
				if (ui->ctlr  > 9) i++;
				if (ui->slave > 9) i++;
				while (i--)
					printf(" ");
			}
			printf(" (%s %s)", 
			    (*tgt->dev_ops->driver_name)(FALSE),tgt->tgt_name);
#ifdef PARAGON860	/* performance mod */
			if (tgt->max_dma_data != SCSI_UNLIMITED_IO_COUNT)
				printf(" (%dK)", tgt->max_dma_data / 1024);
#endif
			printf(" LUN %d", j);
		} else {
			printf(", %d", j);
		}
		ptgt = tgt;
	}

#if	PARAGON860
	if (!getbootint("SCSI_TAGGED_QUEUING", 1))
		tgt->flags &= ~TGT_TAGGED_QUEUING;
#endif	PARAGON860

	if (tgt->dev_ops->optimize)
		(*tgt->dev_ops->optimize)(tgt);

	tgt->flags |= TGT_FULLY_PROBED;

  }
  splx(s);
}

/*
 * Probe routine:
 *	See if a device answers.  Used AFTER autoconf.
 *
 * Implementation:
 *	First ask the HBA to see if anyone is there at all, then
 *	call the scsi_slave and scsi_attach routines with a fake ui.
 */
boolean_t
scsi_probe( sc, tgt_ptr, target_id, lun, ior)
	scsi_softc_t		*sc;
	target_info_t		**tgt_ptr;
	int			target_id, lun;
	io_req_t		ior;
{
	struct bus_device	ui;
	target_info_t		*tgt;

	if (!sc->probe 			  || 
	    target_id >= MAX_SCSI_TARGETS || 
	    target_id == sc->initiator_id ||
	    lun >= MAX_LUNS)
		return FALSE;	/* sanity */

	if (sc->target[target_id][lun] == 0)
		scsi_slave_alloc( sc->masterno, target_id, sc->hw_state);
	tgt = sc->target[target_id][lun];
	tgt->flags = 0;/* we donno yet */

	/* mildly enquire */
	if (!(sc->probe)(tgt, ior))
		goto fail;

	/* There is something there, see what it is */
	bzero(&ui, sizeof(ui));
	ui.ctlr = sc->masterno;
	ui.unit = ui.slave = target_id;
	ui.name = "";

	/* this fails on the name for sure */
	(void) scsi_slave( &ui, 0 /* brrrr */);
	if ((tgt->flags & TGT_ALIVE) == 0)
		goto fail;

	{
		struct bus_ctlr	mi;

		mi.unit = sc->masterno;
		ui.mi = &mi;

		printf("%s at slave %d",
			(*tgt->dev_ops->driver_name)(TRUE), tgt->unit_no);

#ifdef PARAGON860	/* performance mod */
		/* display the target buffer size */
		if (tgt->max_dma_data != SCSI_UNLIMITED_IO_COUNT)
			printf(" (%dK)", tgt->max_dma_data / 1024);
#endif

		scsi_attach(&ui);
		printf("\n");
	}

	*tgt_ptr = tgt;
	return TRUE;
fail:
	for (lun = 0; lun < MAX_LUNS; lun++) {
		sc->target[target_id][lun]->flags = 0;
	}
	return FALSE;
}


/*
 * Watchdog routine:
 *	Issue a SCSI bus reset if a target holds up the
 *	bus for too long.
 *
 * Implementation:
 *	Each HBA that wants to use this should have a
 *	watchdog_t structure at the head of its hardware
 *	descriptor.  This variable is set by this periodic
 *	routine and reset on bus activity. If it is not reset on
 *	time (say some ten seconds or so) we reset the
 *	SCSI bus.
 * NOTE:
 *	An HBA must be ready to accept bus reset interrupts
 *	properly in order to use this.
 */
scsi_watchdog(hw)
	watchdog_t	*hw;
{
	unsigned int	s = splbio();

	switch (hw->watchdog_state) {
	case SCSI_WD_EXPIRED:

		/* double check first */
		if (hw->nactive == 0) {
			hw->watchdog_state = SCSI_WD_INACTIVE;
			break;
		}
		if (scsi_debug)
			printf("SCSI Watchdog expired\n");
		hw->watchdog_state = SCSI_WD_INACTIVE;
		(*hw->reset)(hw);
		break;

	case SCSI_WD_ACTIVE:

		hw->watchdog_state = SCSI_WD_EXPIRED;
		break;

	case SCSI_WD_INACTIVE:

		break;
	}

	/* do this here, fends against powered down devices */
	if (scsi_watchdog_period != 0)
		timeout(scsi_watchdog, hw, scsi_watchdog_period * hz);

	splx(s);
}


/*
 * BusReset Notification:
 *	Called when the HBA sees a BusReset interrupt
 *
 * Implementation:
 *	Go through the list of targets, redo the synch
 *	negotiation, and restart whatever operation was
 *	in progress for that target.
 */
scsi_bus_was_reset(sc)
	scsi_softc_t	*sc;
{
	register target_info_t	*tgt;
	int			i, j;

	/*
	 * Redo the synch and wide bus negotiation
	 */
	for (i = 0; i < MAX_SCSI_TARGETS; i++) {
	   for (j = 0; j < MAX_LUNS; j++) {
		io_req_t	ior;
		int		s;

		tgt = sc->target[i][j];
		if (!tgt || !(tgt->flags & TGT_ALIVE) ||
		    (tgt->flags & TGT_US))
			continue;

		tgt->flags &= ~(TGT_DID_SYNCH|TGT_DID_WIDE|TGT_DISCONNECTED);

		ior = tgt->ior;
		tgt->ior = 0;
		if (j == 0)
			printf("... tgt %d LUN %d", tgt->target_id, j);
		else
			printf(", %d", j);

		/*
		 * retry the wide bus negotiation
		 */
		if (BGET(scsi_no_wide_xfer,sc->masterno,tgt->target_id))
			tgt->flags |= TGT_DID_WIDE;
		else {
			s = spl0();
			scsi_test_unit_ready(tgt, 0);
			splx(s);
		}

		/*
		 * retry the synch negotiation
		 */
		if (BGET(scsi_no_synchronous_xfer,sc->masterno,tgt->target_id))
			tgt->flags |= TGT_DID_SYNCH;
		else {
			s = spl0();
			scsi_test_unit_ready(tgt, 0);
			splx(s);
		}

		tgt->ior = ior;
           }
	}

	/*
	 * Notify each target of the accident
	 */
	for (i = 0; i < MAX_SCSI_TARGETS; i++) {
	   for (j = 0; j < MAX_LUNS; j++) {
		if (!(tgt = sc->target[i][j]))
			continue;
		if (tgt->ior) {
			tgt->done = SCSI_RET_ABORTED|SCSI_RET_RETRY;
			(*tgt->dev_ops->restart)( tgt, TRUE);
		}
           }
	}

	printf("%s", " reset complete\n");
}

#ifdef	SCSI_STATISTICS
/*
 * Performance monitoring routines
 */

scsi_driver_checkin(ior)
	io_req_t	ior;
{
#ifdef	PARAGON860
	extern double	dclock();
	union part_time	time;

	time.dbl = dclock();

	ior->timestamp.lo = time.longs.lo;
	ior->timestamp.hi = time.longs.hi;
#endif	PARAGON860
}

scsi_driver_checkout(tgt, ior)
	target_info_t	*tgt;
	io_req_t	ior;
{
#ifdef	PARAGON860
	extern double	dclock();
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


scsi_device_checkin(tgt, ior)
	target_info_t	*tgt;
	io_req_t	ior;
{
#ifdef	PARAGON860
	extern double	dclock();
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

scsi_device_checkout(tgt, ior)
	target_info_t	*tgt;
	io_req_t	ior;
{
#ifdef	PARAGON860
	extern double	dclock();
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
#endif	SCSI_STATISTICS


/*
 * Display statistics for all devices
 */
scsi_stats(reset)
	boolean_t	reset;
{
	int		master;
	scsi_softc_t	*sc;
	int		id, lun;
	target_info_t	*tgt;
	double		bytes, seconds;

	for (master = 0; master < NSCSI; master++) {

	    if (!(sc = scsi_softc[master]))
		continue;

	    if (!reset)
		printf("SCSI Master %d Statistics:\n\n", master);

	    for (id = 0; id < MAX_SCSI_TARGETS; id++) {
		for (lun = 0; lun < MAX_LUNS; lun++) {

		    tgt = sc->target[id][lun];

		    /*
		     * Skip un-initialized targets
		     */
		    if (!tgt || !(tgt->flags & TGT_ALIVE))
			continue;

		    /*
		     * Weird stuff for disk.  We're not supporting LUNs
		     * on disks yet, so each target/lun structure points
		     * to target/lun 0.
		     */
		    if (lun != 0 && tgt == sc->target[id][0])
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
		    printf("Target ");
		    printf("%-12s", "Requests");	/* 10 digits, 2 space */
		    printf("%-12s", "Blocks");		/* 10 digits, 2 space */
		    printf("%-12s", "Driver us");	/* 10 digits, 2 space */
		    printf("%-12s", "Device us");	/* 10 digits, 2 space */
		    printf("%-8s",  "KB/sec");		/*  6 digits, 2 space */
		    printf("%-3s",  "MB/sec");		/*  3 digits, 0 space */
		    printf("\n");

		    /*
		     * Read statistics
		     */
		    printf("Read:\t");
		    printf(" %2d:%d   ", id, lun);
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
		    printf(" %2d:%d   ", id, lun);
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
	}

	return 0;
}

#endif	NSCSI > 0
