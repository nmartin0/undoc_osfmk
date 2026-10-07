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
 * $Log: md.c,v $
 * Revision 0.8  1994/11/18  20:42:42  mtm
 * Copyright additions/changes
 *
 * Revision 0.7  1994/07/12  19:19:20  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 0.6  1994/06/30  21:04:09  dbm
 * Added the DIOMRINFO to enable the ram disk to return the mach_record_size
 * that was required for IPI-3 functionality.
 *
 * Revision 0.5  1994/06/28  23:43:36  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 0.4.4.2  1994/03/08  00:49:41  andyp
 * Added the *right* casts to get rid of a compile-time warning.
 *
 * Revision 0.4.4.1  1994/03/08  00:37:57  andyp
 * Added a cast to remove a compile-time warning.
 *
 * Revision 0.4  1993/10/20  15:36:30  rkl
 * Added mdsetstatus() function and fixed a bug so the RAM disk could be used
 * in multi-user mode.
 *
 * Revision 0.3  1993/06/30  22:37:28  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.2  1993/01/18  18:59:47  regnier
 * support ramdisk device_get_status()
 *
 * Revision 0.1  1992/07/09  17:00:30  andyp
 * Initial checkin of Paragon files.
 *
 * Revision 2.2  91/12/10  16:29:59  jsb
 * 	New files from Intel
 * 	[91/12/10  16:11:01  jsb]
 * 
 */ 
/*
 *	from prp, SSD Intel
 *	revised by andyp@ssd.intel.com after looking at a driver
 *	supplied by Zonnie Williamson at CMU.
 *
 *	md0 uses, if present, the ram disk loaded by the bootstrap.
 *	If the ram disk was *not* loaded by the bootstrap, md0
 *	behaves just like the other drives.
 *
 *	The size of a ram disk (other than md0) is determined on the
 *	first write to a recently opened disk.  So, to get a 4MB ram disk,
 *	seek to 4MB and write one block.  The memory is
 *	reclaimed on the last close (implying it should be
 *	held open between the write and the mount).
 *
 *	XXX I have not tested the functionality described in the
 *	XXX previous paragraph.
 */
 
#include <md.h>
#if	NMD > 0

#include <sys/types.h>
#include <sys/ioctl.h>
#include <vm/vm_kern.h>
#include <device/buf.h>
#include <device/errno.h>
#include <device/device_types.h>
#include <device/disk_status.h>

#define MD_BLOCK_SIZE	2048		/* DFLT_512 */

unsigned char *md_address;	/* set by i860paragon/model_dep.c */
unsigned long md_size;		/* set by i860paragon/model_dep.c */

int	mddebug = 0;

struct ramdisk {
	vm_offset_t	ram_data;
	vm_size_t	ram_size;
	int		ram_mode;
};

static struct ramdisk	ramdisk[NMD];


static void md_debug_print(f, number, ior)
	char		*f;
	int		number;
	io_req_t	ior;
{
	int	s = splhigh();

	printf("%s(n=%d,ior=0x%x): num=%d blk=%5d cnt=%5d off=%6d\n",
		f,
		number,
		ior,
		(ior) ? ior->io_unit : 0,
		(ior) ? ior->io_recnum : 0,
		(ior) ? ior->io_count : 0,
		(ior) ? ior->io_recnum * MD_BLOCK_SIZE : 0);

	splx(s);
}


io_return_t mdopen(number, mode, ior)
	int		number;
	dev_mode_t	mode;
	io_req_t	ior;
{
	struct ramdisk	*r;

	if ((number < 0) || (number >= NMD))
		return D_NO_SUCH_DEVICE;

	r = &ramdisk[number];

	/*
	 *	ramdisk 0 may have been loaded by the bootloader
	 *	(and if it was, it won't be deallocated...)
	 */
	if ((number == 0) && (md_size != 0)) {
		r->ram_data = (vm_offset_t) md_address;
		r->ram_size = (vm_size_t) md_size;
	}

	if ((r->ram_mode = mode) == 0) {
		r->ram_mode = D_READ | D_WRITE;
	}

	if (mddebug) {
		md_debug_print("mdopen", 0, 0);
	}

	return D_SUCCESS;
}


io_return_t mdclose(number)
	int	number;
{
	struct ramdisk	*r;

	r = &ramdisk[number];
	if ((number != 0) || (md_size == 0)) {
		kmem_free(kernel_map, r->ram_data, r->ram_size);
	}
	r->ram_data = 0;
	r->ram_size = 0;
	r->ram_mode = 0;

	if (mddebug) {
		md_debug_print("mdclose", number, 0);
	}

	return D_SUCCESS;
}


int mdstrategy(ior)
	io_req_t	ior;
{
	char		*src, *dst;
	struct ramdisk	*r;
	int		offset, s;

	if (mddebug) {
		md_debug_print("mdstrategy", ior->io_unit, ior);
	}

	if (ior->io_count == 0) {
		iodone(ior);
		return;
	}

	if (ior->io_count & (MD_BLOCK_SIZE - 1)) {
		ior->io_op |= IO_ERROR;
		ior->io_error = D_INVALID_SIZE;
		iodone(ior);
		return;
	}

	offset = MD_BLOCK_SIZE * ior->io_recnum;

	r = &ramdisk[ior->io_unit];

	if ((offset > r->ram_size) ||
	    ((offset == r->ram_size) && !(ior->io_op & IO_READ))) {
		ior->io_op |= IO_ERROR;
		ior->io_error = D_INVALID_RECNUM;
		iodone(ior);
		return;
	}
	if (offset == r->ram_size) {
		ior->io_residual = ior->io_count;
		iodone(ior);
		return;
	}

	if (ior->io_op & IO_READ) {
		if ((r->ram_mode & D_READ) == 0) {
			ior->io_op |= IO_ERROR;
			ior->io_error = D_IO_ERROR;
			iodone(ior);
			return;
		}
		src = (char *) (r->ram_data + offset);
		dst = (char *) (ior->io_data);
	} else {
		if ((r->ram_mode & D_WRITE) == 0) {
			ior->io_op |= IO_ERROR;
			ior->io_error = D_IO_ERROR;
			iodone(ior);
			return;
		}
		src = (char *) (ior->io_data);
		dst = (char *) (r->ram_data + offset);
	}

	mdcopy(src, dst, ior->io_count);
	ior->io_residual = 0;
	iodone(ior);

	return D_SUCCESS;
}


io_return_t mdread(number, ior)
	int		number;
	io_req_t	ior;
{
	if (number != ior->io_unit)
		ior->io_unit = number;

	if (mddebug) {
		md_debug_print("mdread", number, ior);
	}

	return block_io(mdstrategy, minphys, ior);
}


io_return_t mdwrite(number, ior)
	int		number;
	io_req_t	ior;
{
	struct ramdisk	*r;
	kern_return_t	cc;
	int		offset;

	r = &ramdisk[number];

	if (r->ram_size == 0) {
		offset = ior->io_recnum * MD_BLOCK_SIZE;
		r->ram_size = offset + ior->io_count;
		r->ram_data = 0;
		cc = kmem_alloc(kernel_map, &r->ram_data, r->ram_size);
		if (cc != KERN_SUCCESS) {
			r->ram_size = 0;
			return D_NO_MEMORY;
		}
		bzero(r->ram_data, r->ram_size);
	}

	if (number != ior->io_unit)
		ior->io_unit = number;

	if (mddebug) {
		md_debug_print("mdwrite", number, ior);
	}

	return block_io(mdstrategy, minphys, ior);
}


io_return_t mdgetstatus(number, code, data, data_count)
	int		number, code, *data_count;
	dev_status_t	*data;
{
	struct ramdisk	*r;

	r = &ramdisk[number];

	switch (code) {
	case DEV_GET_SIZE:
		data[DEV_GET_SIZE_DEVICE_SIZE] = (dev_status_t) r->ram_size;
		data[DEV_GET_SIZE_RECORD_SIZE] = (dev_status_t) MD_BLOCK_SIZE;
		*data_count = DEV_GET_SIZE_COUNT;
		break;

	case DIOMRINFO:
		*(int *)data = MD_BLOCK_SIZE;
		*data_count = 1;
		break;

	default:
		return D_IO_ERROR;
	}

	return D_SUCCESS;
}

io_return_t
mdsetstatus(number, flavor, status, status_count)
	int		number;
	int		flavor;
	dev_status_t	status;
	unsigned int	status_count;
{
	struct ramdisk	*r;
	char	*src, *dst;
	io_return_t error = D_SUCCESS;

	r = &ramdisk[number];

	switch (flavor) {
	case DIOCSRETRIES:
	case DIOCWLABEL:	
		break;

	case DIOCSDINFO:
	case DIOCWDINFO:
		src = (char *) (status);
		dst = (char *) (r->ram_data + LABELOFFSET);
		mdcopy(src, dst, sizeof(struct disklabel));
		break;

	default:
 		error = D_INVALID_OPERATION;
	}
	return error;
}

io_return_t mddevinfo(number, code, data)
	int	number, code, *data;
{
	switch (code) {
	case D_INFO_BLOCK_SIZE:
		*data = MD_BLOCK_SIZE;
		break;
	default:
		return D_IO_ERROR;
	}

	return D_SUCCESS;
}


mdcopy(src, dst, cnt)
char	*src, *dst;
int	cnt;
{
	int	s = splhigh();

	if (mddebug) {
		printf("mdcopy(src=0x%x,dst=0x%x,cnt=%d)\n",
			src, dst, cnt);
	}
	bcopy(src, dst, cnt);

	splx(s);
}


#endif	NMD > 0
