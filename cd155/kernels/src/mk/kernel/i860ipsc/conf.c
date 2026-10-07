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
 * $Log: conf.c,v $
 * Revision 1.3  1994/11/18  20:40:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/06/30  22:34:42  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1992/09/22  18:10:32  regnier
 * Initial revision
 *
 * Revision 2.4  91/08/28  11:12:46  jsb
 * 	Added entries for new dev_info field.
 * 	[91/08/27  17:54:12  jsb]
 * 
 * Revision 2.3  91/06/18  20:52:19  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  19:01:05  jsb]
 * 
 * Revision 2.2  90/12/04  14:49:59  jsb
 * 	First checkin.
 * 	[90/12/03  21:47:04  jsb]
 * 
 */

#include <device/conf.h>

extern int	timeopen(), timeclose(), timemmap();
#define	timename		"time"

#include <usm.h>
#if	NUSM > 0
extern int	usmopen(), usmclose(), usmread(), usmwrite();
extern int	usmgetstat(), usmsetstat(), usmportdeath();
#define	usmname			"usm"
#endif

/*
 * List of devices - console must be at slot 0
 */
struct dev_ops	dev_name_list[] =
{
	/*name,		open,		close,		read,
	  write,	getstat,	setstat,	mmap,
	  async_in,	reset,		port_death,	subdev,
	  dev_info */

#if	NUSM > 0
	{ usmname,	usmopen,	usmclose,	usmread,
	  usmwrite,	usmgetstat,	usmsetstat,	nulldev,
	  nodev,	nulldev,	usmportdeath,	0,
	  nodev },
#endif
	{ timename,	timeopen,	timeclose,	nulldev,
	  nulldev,	nulldev,	nulldev,	timemmap,
	  nodev,	nulldev,	nulldev,	0,
	  nodev },
};
int	dev_name_count = sizeof(dev_name_list)/sizeof(dev_name_list[0]);

/*
 * Indirect list.
 */
struct dev_indirect dev_indirect_list[] = {

	/* console */
	{ "console",	&dev_name_list[0],	0 }
};
int	dev_indirect_count = sizeof(dev_indirect_list)
				/ sizeof(dev_indirect_list[0]);
