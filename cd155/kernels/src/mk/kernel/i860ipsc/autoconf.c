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
 * Revision 1.6  1994/11/18  20:40:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  22:34:40  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/04/27  20:29:04  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.3.2.2  1993/04/22  18:29:17  dleslie
 * First R1_0 release
 *
 * Revision 1.1  1992/09/22  18:09:42  regnier
 * Initial revision
 *
 * Revision 2.7.2.1  92/02/18  19:01:59  jeffreyh
 * 	[stan smith] changes to support new boot config sequence.
 * 	[92/02/13  12:49:44  jeffreyh]
 * 
 * Revision 2.7  91/12/10  16:31:41  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:33:20  jsb]
 * 
 * Revision 2.6  91/08/28  11:12:44  jsb
 * 	Eliminated call to ask_bootstrap_server.
 * 	Split dcm_init call into dcm_init_{send,recv}.
 * 	[91/08/26  15:40:29  jsb]
 * 
 * Revision 2.5  91/07/01  08:24:29  jsb
 * 	Call ask_bootstrap_server.
 * 	[91/06/29  16:39:30  jsb]
 * 
 * Revision 2.4  91/06/18  20:52:17  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  19:00:55  jsb]
 * 
 * Revision 2.3  91/06/17  15:45:33  jsb
 * 	Changed to match i386ipsc autoconf.c.
 * 	[91/06/17  10:35:07  jsb]
 * 
 * Revision 2.2  90/12/04  14:49:56  jsb
 * 	First checkin.
 * 	[90/12/03  21:46:29  jsb]
 * 
 */
#include <sys/reboot.h>


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



/*
 * Can override hard-coded root_name used by get_root_device()
 *	"root_name" defined in i860/swapgeneric.c
 */
ask_root_device()
{
	static char	root[64];
	char		*boot_root_dev;
	extern char	*root_name, *getbootenv();

	if (getbootenv("RB_ASKNAME") || getbootenv("RB_860_ASKNAME")) {

		printf("root device? [%s] ", root_name);
		gets(root, sizeof(root));
		if (root[0] != '\0') {
			root_name = root;
		}
		return;
	}

	/* i860 specific */
	if ((boot_root_dev = getbootenv("BOOT_860_ROOT_DEV")) != 0) {
		root_name = skipcomma(boot_root_dev);
		return;
	}

	/* valid for both i860 & i386 kernels */
	if ((boot_root_dev = getbootenv("BOOT_ROOT_DEV")) != 0) {
		root_name = skipcomma(boot_root_dev);
		return;
	}
}


/*
 *	decode envirnment vars for device info:
 *	"root_dev_node" defined in i860/swapgeneric.c
 */

probeio()
{
	char		*cp;
	extern char	*export_paging, *getbootenv();
	extern int	root_fs_node, root_dev_node;

	if ( boothowto & RB_ASKNAME )
		ask_root_device();

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
		panic("BOOT_ROOT_DEV not specified");

	/* find out where the root default pager lives */
	if ( (cp = getbootenv("ROOT_FS_NODE")) != 0 ) {
		root_fs_node = atoi( cp );
	}

	/*
	 * are we exporting a vnode pager? Checked in boot_ufs/bootstrap.c
	 * do_i_export_paging() returns the /mch_servers/paging_file suffix
	 * if we are exporting paging, otherwise NULL.
	 */
	if ( (cp = getbootenv("EXPORT_PAGING")) != 0 ) {
		int	node = node_self();
		extern char	*do_i_export_paging();	/* i860/loose_ends.c */

		if ( node != root_fs_node )
			export_paging = do_i_export_paging( node, cp );
	}

	/*
	 * Initialize the DCM routing hardware.
	 */
	dcm_init_send();
	dcm_init_recv();
}
