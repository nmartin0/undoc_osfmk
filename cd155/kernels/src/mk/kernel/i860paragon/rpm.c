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
 * $Log: rpm.c,v $
 * Revision 1.6  1994/11/18  20:43:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/03/15  22:26:51  sean
 *  Reviewer:lenb
 *  Risk:low
 *  Benefit or PTS #: proper initialization of rpm
 *
 * Revision 1.4  1994/03/06  22:23:16  sean
 *  Reviewer:  steved
 *  Risk:  low
 *
 * Checking in these files which represent Steved's fixes for problems
 * in the following following areas:
 *
 * -fscan
 * -vdp clock skew
 * -bootmagic for processor mode
 *
 * ./ddb/db_command.c
 * ./ddb/db_print.c
 * ./i860/hardclock.c
 * ./i860paragon/mp.h
 * ./i860paragon/fscan.c
 * ./i860paragon/fscan.h
 * ./i860paragon/model_dep.c
 * ./i860paragon/mp_start.s
 * ./i860paragon/rpm.c
 * ./i860paragon/mp.c
 * ./i860paragon/interrupt.c
 * ./intel/pmap.c
 * ./ipsc/bootenv.c
 * ./kern/sched_prim.c
 * ./kern/mach_clock.c
 * ./vm/vm_pageout.c
 *
 * Revision 1.3  1993/12/14  23:00:24  steved
 * Fixed a multitude of problems using the real MP3 nodeboard. Added
 * new led routines to blink the additional LED's on the MP3 board.
 * Cleaned up some of the constants and removed those which are
 * not used on the GP nor MP3 boards.
 *
 * Revision 1.2  1993/06/30  22:37:46  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1993/04/22  17:58:46  dleslie
 * Initial revision
 *
 * Revision 0.1  1993/04/05  01:31:23  andyp
 * New RPM driver entry points.
 *
 *
 */

/*
 *	RPM resource utilization counters.
 *
 *	The RPM contains 9 counters.  Eight are used for monitoring
 *	bus ownership and MRC directional message traffic.  The other
 *	counter is driven by the global 10MHz backplane clock.
 *	They are resetable, and can be disabled.
 *
 */
#include <rpm.h>
#if	NRPM > 0

#if	NRPM > 1
	This driver does not support more than one device
#endif	NRPM > 1

#include <sys/types.h>
#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <device/device_types.h>
#include <device/io_req.h>
#include <device/buf.h>
#include <chips/busses.h>
#include <i860paragon/rpm.h>
#include <i860paragon/lbus.h>

/*
 *	For autoconfiguration...
 */
int	rpm_probe(), rpm_attach();

static struct bus_device	*rpm_info[NRPM];
static caddr_t		rpm_std[NRPM] = { (caddr_t) RPM_BASE_PADDR };
struct bus_driver	rpm_driver = {
	rpm_probe,		/* is the RPM installed? */
	0,			/* really a probe for slaves... */
	rpm_attach,		/* setup driver after the probe */
	0,			/* start transfer */
	rpm_std,		/* standard device csr addresses */
	"rpm",			/* name of the device */
	rpm_info,		/* backpointers to init structs */
	0,			/* name of a controller */
	0,			/* backpointers to init structs */
	0			/* flags */
};

boolean_t	rpm_probed;	/* have we probed yet? */
boolean_t	rpm_present;	/* is the RPM present? */

extern int paragon_mp3;


/*
 *	is the RPM installed?
 */
rpm_probe(vaddr, ctlr)
	caddr_t		vaddr;
	struct bus_ctlr	*ctlr;
{
	unsigned long	bits;
	int	i, max, wait;
	struct rpm	*rpm;
	unsigned long	reset, enable;

	if (rpm_probed) return rpm_present;
	rpm_probed = TRUE;
	rpm_present = FALSE;
	/*
	 *	The trick is to read the node control
	 *	register (a legal bus cycle that returns
	 *	semi-junk). The RPM has pull-downs on the
	 *	high 8 bits of the data bus.  If we get
	 *	back 0xff in the high byte, the RPM
	 *	isn't present.
	 *	For the MP3 board,the RPM will ALWAYS be present. 
	 */

	max = 20;
	wait = 50;
	for (i = 0; i < max; i++) {
		delay(wait);
		bits = inl(LB_NODE_CONTROL);
		if ((((bits >> 24) & 0xff) != 0xff) || (paragon_mp3)) {
			/*
			 *	Reset and enable all of the counters...
			 */
			rpm_present = TRUE;
			rpm_reset();
			return 1;
		}
	}
	return 0;
}

rpm_attach(device)
	struct bus_device	*device;
{
	return 0;
}


/*
 *	XXX I don't see how this routine could ever
 *	XXX be called...
 */
rpm_reset()
{
	struct rpm	*rpm;
	unsigned long	reset, enable;

	if (rpm_present) {
		rpm = (struct rpm *) RPM_BASE_PADDR;
		enable = RPM_MRC_NORTH_ENABLE |
			RPM_MRC_SOUTH_ENABLE |
			RPM_MRC_EAST_ENABLE  |
			RPM_MRC_WEST_ENABLE  |
			RPM_CPU0_ENABLE  |
			RPM_CPU1_ENABLE  |
			RPM_CPU2_ENABLE  |
			RPM_BUSBUSY_ENABLE  |
			RPM_DRAMBUSY_ENABLE  |
			RPM_LTU_ENABLE   |
			RPM_EXP_ENABLE;
		reset = RPM_MRC_NORTH_RESET |
			RPM_MRC_SOUTH_RESET |
			RPM_MRC_EAST_RESET  |
			RPM_MRC_WEST_RESET  |
			RPM_CPU0_RESET	    |
			RPM_CPU1_RESET	    |
			RPM_CPU2_RESET  |
			RPM_BUSBUSY_RESET  |
			RPM_DRAMBUSY_RESET  |
			RPM_LTU_RESET	    |
			RPM_EXP_RESET;
		outl(&rpm->rpm_control, reset | enable);
	}
}


/*
 *	Open the RPM device.
 */
rpm_open(dev, flag, ior)
	dev_t		dev;
	int		flag;
	io_req_t	ior;
{
	if (minor(dev) >= NRPM)
		return D_NO_SUCH_DEVICE;
	if (rpm_present == FALSE)
		return D_NO_SUCH_DEVICE;
	return D_SUCCESS;
}


/*
 *	Close the RPM device.
 */
rpm_close(dev, flag)
	dev_t	dev;
	int	flag;
{
	return D_SUCCESS;
}


/*
 *	Read/Write the RPM counters.
 *
 *	read -- sample the counters
 *	write -- reset and enable the counters
 *
 */
rpm_strategy(ior)
	io_req_t	ior;
{
	struct rpm	*src, *dst;
	int	s;

	if (ior->io_op & IO_READ) {
		if (ior->io_count == 0) {
			iodone(ior);
			return;
		}
		if (ior->io_count != sizeof(struct rpm)) {
			ior->io_op |= IO_ERROR;
			ior->io_error = D_INVALID_SIZE;
			iodone(ior);
			return;
		}
		src = (struct rpm *) RPM_BASE_VADDR;
		dst = (struct rpm *) ior->io_data;
		s = sploff();
		dst->rpm_time = src->rpm_time;
		dst->rpm_cpu0 = src->rpm_cpu0;
		dst->rpm_cpu1 = src->rpm_cpu1;
		dst->rpm_cpu2 = src->rpm_cpu2;
		dst->rpm_dbus = src->rpm_dbus;
		dst->rpm_dram = src->rpm_dram;
		dst->rpm_ltu = src->rpm_ltu;
		dst->rpm_exp = src->rpm_exp;
		dst->rpm_north = src->rpm_north;
		dst->rpm_south = src->rpm_south;
		dst->rpm_east = src->rpm_east;
		dst->rpm_west = src->rpm_west;
		splon(s);
		iodone(ior);
		return;
	}

	ior->io_op |= IO_ERROR;
	ior->io_error = D_IO_ERROR;
	iodone(ior);
}


/*
 *	Read the RPM counters
 */
rpm_read(dev, ior)
	dev_t		dev;
	io_req_t	ior;
{
	if (dev != ior->io_unit)
		ior->io_unit = dev;
	return block_io(rpm_strategy, minphys, ior);
}


/*
 *	Write to the RPM counters (not really)
 */
rpm_write(dev, ior)
	dev_t		dev;
	io_req_t	ior;
{
	if (dev != ior->io_unit)
		ior->io_unit = dev;
	return block_io(rpm_strategy, minphys, ior);
}


/*
 *	Map in the RPM.
 */
vm_offset_t rpm_mmap(dev, off, prot)
	dev_t		dev;
	vm_offset_t	off;
	vm_prot_t	prot;
{
	if (minor(dev) != 0)
		return -1;
	return i860_btop(RPM_BASE_PADDR);
}


rpm_getstat(number, code, data, data_count)
	int		number, code, *data_count;
	dev_status_t	*data;

{
	return D_INVALID_OPERATION;
}


rpm_setstat(dev, flavor, status, count)
	dev_t		dev;
	int		flavor;
	dev_status_t	status;
	u_int		count;
{
	return D_INVALID_OPERATION;
}


rpm_devinfo(number, code, data)
	int	number, code, *data;
{
	return D_INVALID_OPERATION;
}

#endif	NRPM > 0
