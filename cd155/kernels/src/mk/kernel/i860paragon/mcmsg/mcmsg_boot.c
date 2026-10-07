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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_boot.c,v 1.7 1995/03/07 02:36:35 lenb Exp $
 */

/*
 * mcmsg_boot.c
 *
 * boot message support
 */

#define	MCMSG_MODULE	MCMSG_MODULE_BOOT

#include <i860paragon/mcmsg/mcmsg_ext.h>
#if 0
#include <i860paragon/mcmsg/mcmsg_nx.h>
#endif
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_boot.h>

static int
mcmsg_boot_send_done()
{
	return (mcmsg_boot_request.control == 0);
}

syscall_mcmsg_boot_send(host, node, flags, control, sequence, buf, count)
	mach_port_t		host;
	unsigned long		node;
	unsigned long		flags;
	unsigned long		control;
	unsigned long		sequence;
	unsigned long		buf;
	long			count;
{

	if ((control & 0xFFFF) < MCTRL_ALLOW_LO ||
	    (control & 0xFFFF) > MCTRL_ALLOW_HI) {
		return -1;
	}
	if ((count & (PKT_GRAN-1)) != 0) {
		return -1;
	}
	if (count > sizeof(mcmsg_boot_request.buf)) {
		return -1;
	}


	/*
	 * The MCP is still busy, spin a bit to see if it completes.
	 */
	if (!mcmsg_boot_send_done())
	{
		int i;

		for (i = 0; !mcmsg_boot_send_done(); ++i)
		{
			if (i > 1000)
			{
				return(0);
			}
		}
	}

#if	BOOT_DELAY
	/*
	 * NIC-A workaround
	 * Can't send a new message until the remote nodes drain their FIFOs.
	 *
	 * spin here if the wait is short, else return to user.
	 */
	{
		double now;
		extern double dclock();

		while ((now = dclock()) < mcmsg_boot_request.when)
		{
			double delta = mcmsg_boot_request.when - now;

			if (delta > 150e-6)
			{
				delay(50);
				return(0);
			}
		}
	}
#endif	BOOT_DELAY

	mcmsg_boot_request.node = node;
	mcmsg_boot_request.flags = flags;
	mcmsg_boot_request.count = count | (sequence << 16);
	mcmsg_boot_request.bufp = (unsigned char *)
		mcmsg_validate_real(mcmsg_boot_request.buf,
				    current_task()->map->pmap->dirbase,
				    __FILE__,
				    __LINE__);
	bcopy(buf, mcmsg_boot_request.buf, count);

	mcmsg_boot_request.control = control;

	mcmsg_post(POST_BOOTSEND);
	return 1;
}

