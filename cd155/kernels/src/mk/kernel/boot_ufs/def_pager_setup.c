/*
 * 
 * $Copyright
 * Copyright 1991, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */

/*
 * SSD HISTORY
 * $Log: def_pager_setup.c,v $
 * Revision 1.9  1994/11/18  20:24:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/06/28  23:31:08  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.7  1994/01/27  02:11:48  stans
 *  Removed 'export_paging' no longer used.
 *  Reviewer: len
 *  Risk: low
 *  Benefit or PTS #: code cleanup
 *  Testing: developer
 *
 * Revision 1.6  1993/09/28  17:46:18  andyp
 * Update for the R1.2 release.
 *
 *
 *	Added the support for local default pagers
 *	from OSF's tree. Recovered OSF's log.
 *	[andyp@ssd.intel.com]
 *
 * Revision 1.5  1993/06/30  22:16:06  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/05/24  19:01:48  stans
 * Removed support for dev_hack_count and multiple default paging files.
 *
 * Revision 1.3  1993/04/27  20:13:38  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:13:09  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: def_pager_setup.c,v
 * Revision 1.1  1992/09/30  01:58:59  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.10.3.2  92/09/15  17:13:35  jeffreyh
 * 	mods for naming different default paging files on the iPSC;
 * 		/mach_servers/paging_file.x
 * 	[92/09/10            stans@ssd.intel.com]
 * 
 * Revision 2.10.3.1  92/03/03  16:12:42  jeffreyh
 * 	Change wiring_allowed to wiring_required in default pager's
 * 	map with appropriate comment.
 * 	[92/02/20  15:15:22  dlb]
 * 
 * Revision 2.10  91/08/28  11:08:46  jsb
 * 	Added struct file_direct and associated functions.
 * 	[91/08/19            rpd]
 * 
 * Revision 2.9  91/07/31  17:23:15  dbg
 * 	Pass master host port to file_wire.
 * 	[91/07/30  16:37:07  dbg]
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: def_pager_setup.c,v
 * Revision 1.1  1992/09/30  01:58:59  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.10.3.2  92/09/15  17:13:35  jeffreyh
 * 	mods for naming different default paging files on the iPSC;
 * 		/mach_servers/paging_file.x
 * 	[92/09/10            stans@ssd.intel.com]
 * 
 * Revision 2.10.3.1  92/03/03  16:12:42  jeffreyh
 * 	Change wiring_allowed to wiring_required in default pager's
 * 	map with appropriate comment.
 * 	[92/02/20  15:15:22  dlb]
 * 
 * Revision 2.10  91/08/28  11:08:46  jsb
 * 	Added struct file_direct and associated functions.
 * 	[91/08/19            rpd]
 * 
 * Revision 2.9  91/07/31  17:23:15  dbg
 * 	Pass master host port to file_wire.
 * 	[91/07/30  16:37:07  dbg]
 * 
 * Revision 2.8  91/05/14  15:21:21  mrt
 * 	Correcting copyright
 * 
 * Revision 2.7  91/02/05  17:00:34  mrt
 * 	Changed to new copyright
 * 	[91/01/28  14:54:27  mrt]
 * 
 * Revision 2.6  90/12/20  16:35:09  jeffreyh
 * 	Added init for  default_partition_lock
 * 	[90/12/11            jeffreyh]
 * 
 * Revision 2.5  90/08/27  21:44:42  dbg
 * 	Use 'new' file_io package.
 * 	[90/07/18            dbg]
 * 
 * Revision 2.4  90/06/02  14:45:17  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  21:28:45  rpd]
 * 
 * Revision 2.3  90/01/11  11:41:03  dbg
 * 	Use bootstrap_task print routines.
 * 	[89/12/20            dbg]
 * 
 * Revision 2.2  89/09/08  11:22:01  dbg
 * 	Created.
 * 	[89/09/01  17:12:55  dbg]
 * 
 * 31-Aug-89  David Golub (dbg) at Carnegie-Mellon University
 *	Use new open_file routine.
 *
 */
/* CMU_ENDHIST */
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
 */
#include <boot_ufs/file_io.h>
#include <mach/mach_interface.h>
#include <mach/mach_user_internal.h>
#include <mach/mach_port_internal.h>
#include <boot_ufs/boot_printf.h>


#include <kern/kalloc.h>
#include <kern/thread.h>
#include <kern/task.h>
#include <vm/vm_map.h>

/*
 * Set up default pager
 */
char	paging_file_name[128] = "\0";

extern void	create_default_partition();
decl_simple_lock_data(extern ,default_partition_lock);

extern char *strbuild();

void
default_pager_setup(master_host_port, master_device_port, rootname)
	mach_port_t master_host_port;
	mach_port_t master_device_port;
	char	*rootname;
{
	register struct file		*fp;
	register struct file_direct	*fdp;
	register int	result;

	simple_lock_init(&default_partition_lock);
	fp = (struct file *) kalloc(sizeof *fp);
	bzero((char *)fp, sizeof *fp);

	(void) strbuild(paging_file_name,
			"/dev/",
			rootname,
			"/mach_servers/paging_file",
			(char *)0);

	while (TRUE) {
	    result = open_file(master_device_port,
			       paging_file_name,
			       fp);
	    if (result == 0)
		break;
	    printf("Can't open paging file %s: %d\n",
		   paging_file_name,
		   result);

	    bzero(paging_file_name, sizeof(paging_file_name));
	    printf("Paging file name ? ");
	    gets(paging_file_name, sizeof(paging_file_name));

	    if (paging_file_name[0] == 0) {
		printf("*** WARNING: running without paging area!\n");
		return;
	    }
	}

	printf("Paging file %s found\n", paging_file_name);

	fdp = (struct file_direct *) kalloc(sizeof *fdp);
	bzero((char *)fdp, sizeof *fdp);

	result = open_file_direct(&fp->f_dev, fdp);
	if (result)
	    panic("Can't open paging file\n");

	result = add_file_direct(fdp, fp);
	if (result)
	    panic("Can't read disk addresses: %d\n", result);

	/*
	 * Wire the buffers, since they will be used by the default pager.
	 * Set flag indicating that all implicitly created memory is
	 *	to be wired.
	 */
	current_task()->map->wiring_required = TRUE;

	result = file_wire_direct(master_host_port, fdp);
	if (result)
	    panic("Can't wire down file buffers: %d", result);

	/*
	 * Set up the default paging partition
	 */
	create_default_partition(fdp->fd_size * fdp->fd_fs->fs_bsize,
				 page_read_file_direct,
				 page_write_file_direct,
				 (char *)fdp);

	/*
	 * Our caller will become the default pager - later
	 */
}
