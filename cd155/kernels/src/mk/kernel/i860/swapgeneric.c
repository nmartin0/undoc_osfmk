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
 * $Log: swapgeneric.c,v $
 * Revision 2.12  1994/11/18  20:39:43  mtm
 * Copyright additions/changes
 *
 * Revision 2.11  1993/06/30  22:31:33  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.10  1993/05/24  20:09:44  stans
 * Removed device forwarding hacks along with support for multiple default
 * pagers. Included OSF code supporting a debug node.
 *
 * Revision 2.9  1993/04/27  20:28:36  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 2.8.2.2  1993/04/22  18:28:59  dleslie
 * First R1_0 release
 *
 * Revision 2.9  1993/02/16  18:21:54  SSD
 * fixed panic string to identify ROOT_DEVICE_NODE instead of ROOT_DEV_NODE.
 *
 * Revision 2.8  1993/02/01  23:53:37  andyp
 * Tree-of-pagers fixes from stans.
 *
 * Revision 2.7  1992/12/21  18:35:58  prp
 * Reduce kernel boot verbosity
 * Add getbootint() for convenience and uniformity
 *
 * Revision 2.6  1992/10/03  00:27:07  SSD
 * added PARAGON860 dev_fwd_hack_count; until dev forward to remote device
 * port works correctly.
 *
 * Revision 2.5  1992/09/24  21:08:09  stans
 * add support for bootenv var root_fs_node and export_paging.
 *
 * Revision 2.4  1992/09/24  20:46:30  stans
 * paragon norma12
 *
 * Revision 2.3  91/06/18  20:51:53  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:59:02  jsb]
 * 
 * Revision 2.2  90/12/04  14:49:26  jsb
 * 	First checkin.
 * 	[90/12/03  21:33:58  jsb]
 * 
 */
#include <mach/boolean.h>

/*
 * Get root device.  Temporarily hard-coded.
 */

#if	iPSC860
char *root_name = "sd0a";

get_root_device()
{
	printf("root on %s\n", root_name);
}
#endif	iPSC860

#if	PARAGON860

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


char	*root_name;
char	*boot_devices[] = {
	"md0a",
	"sd0a",
	0
};


boolean_t	debug_kernel = FALSE;

get_root_device()
{
	char		new_root_device[16], c;
	int		i = 0, getroot = 0;
	char		*boot_root_dev, *cp;
	extern char	*getbootenv();
	extern int	root_fs_node, root_dev_node;
	extern int	boot_mk_verbose;

	if ( ((cp = getbootenv("DEBUG_NODE")) != 0) &&
	    (atoi(cp) == node_self()) ) {
		debug_kernel = TRUE;
		/*
		 * This is a debug node.  Find out where the debug
		 * root device node is, and use it for root_dev_node.
		 */
		if ((cp = getbootenv("DEBUG_ROOT_DEVICE_NODE")) != 0) {
			root_dev_node = atoi(cp);
		} else {
			printf("DEBUG_NODE %d\n", node_self());
			panic("DEBUG_ROOT_DEVICE_NODE not specified");
		}
		if ((cp = getbootenv("DEBUG_BOOT_ROOT_DEV")) != 0) {
			root_name = skipcomma(cp);
			printf("DEBUG_NODE %d: debug_root_dev_node %d, debug_boot_root_dev %s\n",
				node_self(), root_dev_node, root_name);
		} else {
			printf("DEBUG_NODE %d\n", node_self());
			panic("DEBUG_BOOT_ROOT_DEV not specified");
		}
		return;
	}

        /* MK node address where root device lives */
        if ( (cp = getbootenv("ROOT_DEVICE_NODE")) != 0 ) {
                root_dev_node = atoi( cp );
        }
	else
                panic("bootmagic ROOT_DEVICE_NODE missing.");

	root_name = boot_devices[1];

	if ((boot_root_dev = getbootenv("BOOT_ROOT_DEV")) != 0) {
		root_name = skipcomma(boot_root_dev);
	} else if ((boot_root_dev = getbootenv("BOOT_860_ROOT_DEV")) != 0) {
		root_name = skipcomma(boot_root_dev);
	}

        /* find out where the root default pager lives */
	root_fs_node = getbootint("ROOT_FS_NODE",root_fs_node);

	getroot = getbootenv("RB_ASKNAME") || getbootenv("RB_860_ASKNAME");

	while (getroot) {
		printf("root device [ ");
		for (i = 0; boot_devices[i]; i++) {
			printf("%d=%s ", i, boot_devices[i]);
		}
		printf(" default=%s ] (choose 0 thru %d): ",
			root_name, i-1);

		gets(new_root_device, sizeof(new_root_device));
		if (new_root_device[0]) {
			if ((new_root_device[0] >= '0') &&
			    (new_root_device[0] <= '9')) {
				c = new_root_device[0] - '0';
				if ((c >= 0) && (c < i)) {
					root_name = boot_devices[c];
					getroot = 0;
				}
			}
		} else {
			getroot = 0;
		}
	}

	if (boot_mk_verbose) {
		printf("root on %s @ node %d\n", root_name,root_dev_node);
	}
}
#endif	PARAGON860
