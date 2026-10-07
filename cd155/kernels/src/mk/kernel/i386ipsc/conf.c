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
 * Revision 1.5  1994/11/18  20:37:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:28:39  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:27:11  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:27:38  dleslie
 * First R1_0 release
 *
 * Revision 2.5.2.2  92/05/27  00:38:56  jeffreyh
 * 	exobyte tape support.
 * 	[92/04/26	stans@ssd.intel.com]
 * 
 * Revision 2.5.2.1  92/03/28  10:06:56  jeffreyh
 * 	Changes needed for generic scsi on iPSC386
 * 	[92/03/20  12:20:45  jeffreyh]
 * 
 * Revision 2.5  91/12/10  16:29:52  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:32:10  jsb]
 * 
 * Revision 2.4  91/08/28  11:11:55  jsb
 * 	Added entries for new dev_info field.
 * 	[91/08/27  17:54:02  jsb]
 * 
 * Revision 2.3  91/06/18  20:50:11  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  20:05:51  jsb]
 * 
 * Revision 2.2  90/12/04  14:47:04  jsb
 * 	First checkin.
 * 	[90/12/04  10:55:41  jsb]
 * 
 */ 

/*
 * Device switch for ipsc.
 */

#include <device/conf.h>

extern int block_io_mmap();

extern int	timeopen(), timeclose(), timemmap();
#define	timename		"time"

extern int      ioplopen(), ioplclose(), ioplmmap();
#define ioplname                "iopl"
 
#include <sd.h>
#if	NSD > 0
extern int	sdopen(), sdclose(), sdread(), sdwrite();
#define	sdname			"sd"
#endif

#include <esp.h>
#if	NESP > 0
int	rz_open(), rz_close(), rz_read(), rz_write();
int	rz_get_status(), rz_set_status(), rz_devinfo();

#define rzname	"rz"	/* disk */
#define tzname	"tz"	/* tape */
#endif	NESP

#include <usm.h>
#if	NUSM > 0
extern int	usmopen(), usmclose(), usmread(), usmwrite();
extern int	usmgetstat(), usmsetstat(), usmportdeath();
#define	usmname			"usm"
#endif

#include <cnp.h>
#if	NCNP > 0
extern int	cnpopen(), cnpoutput(), cnpgetstat(), cnpsetstat(),
		cnpsetinput();
#define	cnpname			"cnp"
#endif NCNP > 0

#include <md.h>
#if	NMD > 0
extern int	mdopen(), mdclose(), mdread(), mdwrite();
#define	mdname			"md"
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
#if	NSD > 0
	{ sdname,	sdopen,		sdclose,	sdread,
	  sdwrite,	nulldev,	nulldev,	nulldev,
	  nodev,	nulldev,	nulldev,	16,
	  nodev },
#endif
#if	NESP > 0
	{ rzname,	rz_open,	rz_close,	rz_read,
	  rz_write,	rz_get_status,	rz_set_status,	nodev,
	  nodev,	nulldev,	nulldev,	16,
	  rz_devinfo },
	{ tzname,	rz_open,	rz_close,	rz_read,
	  rz_write,	rz_get_status,	rz_set_status,	nodev,
	  nodev,	nulldev,	nulldev,	0,
	  nodev },
#endif	NESP
#if	NCNP > 0
	{ cnpname,	cnpopen,	nulldev,	nulldev,
	  cnpoutput,	cnpgetstat,	cnpsetstat,	nulldev,
	  cnpsetinput,	nulldev,	nulldev,	0,
	  nodev },
#endif
#if	NMD > 0
	{ mdname,	mdopen,		mdclose,	mdread,
	  mdwrite,	nulldev,	nulldev,	nulldev,
	  nodev,	nulldev,	nulldev,	4,
	  nodev },
#endif
	{ timename,	timeopen,	timeclose,	nulldev,
	  nulldev,	nulldev,	nulldev,	timemmap,
	  nodev,	nulldev,	nulldev,	0,
	  nodev },

        { ioplname,     ioplopen,       ioplclose,      nodev,
          nodev,        nodev,          nodev,          ioplmmap,
          nodev,        nulldev,        nulldev,	0,
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
