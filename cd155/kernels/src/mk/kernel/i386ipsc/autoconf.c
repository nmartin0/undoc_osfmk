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
 * Copyright (c) 1991 Carnegie Mellon University
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
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
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
 * $Log: autoconf.c,v $
 * Revision 1.5  1994/11/18  20:37:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:28:35  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:27:07  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:27:34  dleslie
 * First R1_0 release
 *
 * Revision 2.6.2.3  92/09/15  17:15:41  jeffreyh
 * 	Intel changes for new enviroment variables.
 * 	[92/09/15  13:57:03  jeffreyh]
 * 
 * Revision 2.6.2.2  92/03/28  10:06:51  jeffreyh
 * 	Changes needed for generic scsi on iPSC386
 * 	[92/03/20  12:20:30  jeffreyh]
 * 
 * Revision 2.6.2.1  92/02/18  18:58:01  jeffreyh
 * 	Added routines to support new boot config sequence.
 * 	[92/02/13  12:33:07  jeffreyh]
 * 
 * Revision 2.6  91/08/03  18:17:47  jsb
 * 	Eliminated call to ask_bootstrap_server.
 * 	Only call ask_root_device if RB_ASKNAME.
 * 	Split dcm_init call into dcm_init_{send,recv}.
 * 	[91/07/25  18:38:51  jsb]
 * 
 * Revision 2.5  91/07/01  08:24:08  jsb
 * 	Call ask_bootstrap_server.
 * 	[91/06/29  16:39:11  jsb]
 * 
 * Revision 2.4  91/06/18  20:50:00  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  20:05:31  jsb]
 * 
 * Revision 2.3  91/06/06  17:04:20  jsb
 * 	Added implementation and call to ask_root_device.
 * 	[91/05/13  16:59:05  jsb]
 * 
 * Revision 2.2.1.1  91/01/07  11:03:51  jsb
 * 	Added implementation and call to ask_root_device.
 * 
 * Revision 2.2  90/12/04  14:46:57  jsb
 * 	First checkin.
 * 	[90/12/04  10:55:12  jsb]
 * 
 */ 

#include <sys/types.h>
#include <sys/reboot.h>
#include <i386at/atbus.h>

#include <cnp.h>
#if	NCNP > 0
extern	struct	isa_dev	cnpinfo[];
#endif	NCNP

#include <esp.h>
#if	NESP > 0

#include <chips/busses.h>
#include <i386ipsc/esp_ctlr.h>

/*
 *	Box-specific routines and data required by the MI code
 */

extern struct bus_driver esp_driver;
extern int esp_intr();

struct bus_ctlr bus_master_init[] = {
/*driver,       name,   unit, intr, address+am, phys, adpt, alive, flags, */
{ &esp_driver,  "esp",  0,    esp_intr,  0x0,0, 0,    '?',    0,     0, },
        0
};

struct bus_device bus_device_init[] = {
/* driver,      name, unit,intr,addr+am,phys, adaptor,alive,ctlr,slave,flags,*/
{ &esp_driver,  "rz",   0,  0,  0x0,0,  0,    '?',     0,   0,   0,    0, },
{ &esp_driver,  "rz",   1,  0,  0x0,0,  0,    '?',     0,   0,   1,    0, },
{ &esp_driver,  "rz",   2,  0,  0x0,0,  0,    '?',     0,   0,   2,    0, },
{ &esp_driver,  "rz",   3,  0,  0x0,0,  0,    '?',     0,   0,   3,    0, },
{ &esp_driver,  "rz",   4,  0,  0x0,0,  0,    '?',     0,   0,   4,    0, },
{ &esp_driver,  "rz",   5,  0,  0x0,0,  0,    '?',     0,   0,   5,    0, },
{ &esp_driver,  "rz",   6,  0,  0x0,0,  0,    '?',     0,   0,   6,    0, },
{ &esp_driver,  "rz",   7,  0,  0x0,0,  0,    '?',     0,   0,   7,    0, },

{ &esp_driver,  "tz",   0,  0,  0x0,0,  0,    '?',     0,   0,   0,    0, },
{ &esp_driver,  "tz",   1,  0,  0x0,0,  0,    '?',     0,   0,   1,    0, },
{ &esp_driver,  "tz",   2,  0,  0x0,0,  0,    '?',     0,   0,   2,    0, },
{ &esp_driver,  "tz",   3,  0,  0x0,0,  0,    '?',     0,   0,   3,    0, },
{ &esp_driver,  "tz",   4,  0,  0x0,0,  0,    '?',     0,   0,   4,    0, },
{ &esp_driver,  "tz",   5,  0,  0x0,0,  0,    '?',     0,   0,   5,    0, },
{ &esp_driver,  "tz",   6,  0,  0x0,0,  0,    '?',     0,   0,   6,    0, },
{ &esp_driver,  "tz",   7,  0,  0x0,0,  0,    '?',     0,   0,   7,    0, },

        0
};

#endif	NESP

#include <sd.h>
#if	NSD > 0
#if	(NESP > 0) && (NSD > 0)
	you do not want to do this
#endif
#endif	NSD


/*
 * look past a comma, if present
 */
static char *skipcomma(s)
register char	*s;
{
	register char	*tmp = s;

	while (*tmp) {
		if (*tmp++ == ',')
			return tmp;
	}
	return s;
}


/* overrides hard-coded root_name used by get_root_device() */
ask_root_device()
{
	static char	root[64];
	char		*boot_root_dev;
	extern char	*root_name, *getbootenv();

	if (getbootenv("RB_ASKNAME") || getbootenv("RB_386_ASKNAME")) {

		printf("root device? [%s] ", root_name);
		gets(root, sizeof(root));
		if (root[0] != '\0') {
			root_name = root;
		}
		return;
	}

	if ((boot_root_dev = getbootenv("BOOT_386_ROOT_DEV")) != 0) {
		root_name = skipcomma(boot_root_dev);
		return;
	}

	if ((boot_root_dev = getbootenv("BOOT_ROOT_DEV")) != 0) {
		root_name = skipcomma(boot_root_dev);
		return;
	}
}


probeio()
{
	int			i;
	struct isa_dev		*dev_p;
	struct isa_driver	*drv_p;
	char			*cp;
	extern int		root_fs_node, root_dev_node;
	extern char		*export_paging, *getbootenv();

	if (boothowto & RB_ASKNAME) {
		ask_root_device();
	}

        /* MK node address where root device lives */
        if ( (cp = getbootenv("ROOT_DEVICE_NODE")) != 0 ) {
                root_dev_node = atoi( cp );
        }
        else {
		/* format: "BOOT_ROOT_DEV=130,sd0a" */
                if ( (cp = getbootenv("BOOT_ROOT_DEV")) != 0 )
                        root_dev_node = atoi( cp );
        }
        if ( ! root_dev_node )
                panic("no BOOT_ROOT_DEV specified");

	/* where the root default pager resides */
	if ( (cp = getbootenv("ROOT_FS_NODE")) != 0 ) {
		root_fs_node = atoi( cp );
	}

#ifndef	iPSC386
        /*
         * are we exporting a vnode pager? Checked in boot_ufs/bootstrap.c
         * do_i_export_paging() returns the /mch_servers/paging_file suffix
         * if we are exporting paging, otherwise NULL.
         */
        if ( (cp = getbootenv("EXPORT_PAGING")) != 0 ) {
                int     node = node_self();
                extern char     *do_i_export_paging();  /* i860/loose_ends.c */

                if ( node != root_fs_node )
                        export_paging = do_i_export_paging( node, cp );
        }
#endif

#if NCNP > 0
	for (i = 0; i < NCNP; i++) {
		dev_p = &cnpinfo[i];
		drv_p = dev_p->dev_driver;

		if (drv_p->driver_probe(dev_p->dev_addr, i)) {
			printf("cnp%d at 0x%x\n", i, dev_p->dev_addr);
			dev_p->dev_alive = 1;
			drv_p->driver_attach(dev_p);
		}
	}
#endif NCNP

	/*
	 * Initialize the DCM routing hardware.
	 */
	dcm_init_send();
	dcm_init_recv();

#if	NESP > 0
	/*
	 * Initialize the SCSI software/hardware.
	 */
	configure_bus_master("esp", SCSI_FIFO, 0, 0, "socket ");
#endif	NESP
}
