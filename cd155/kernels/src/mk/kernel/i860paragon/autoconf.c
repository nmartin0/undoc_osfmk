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
 * Copyright 1992 by Intel Corporation,
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
 * Revision 0.12  1995/03/14  23:42:42  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 0.11  1994/11/18  20:42:11  mtm
 * Copyright additions/changes
 *
 * Revision 0.10  1994/06/08  17:02:00  arlin
 * Initial Release of HiPPI IPI-3 driver
 *
 * Revision 0.9  1993/10/27  19:59:28  andyp
 * Initial memory expansion card support.
 *
 * Revision 0.8  1993/09/24  22:58:45  jerrie
 * Added entries to allow scan for SCSI media changer (jukebox) devices.
 *
 * Revision 0.7  1993/06/30  22:36:57  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.6  1993/06/05  00:49:14  arlin
 * changed hippi_bd_present() to a more generic
 * expansion_id() call.
 *
 * Revision 0.5  1993/05/27  22:09:23  arlin
 * Added Hippi Driver Support, ARD
 *
 * Revision 0.4  1993/04/27  20:31:12  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 0.3.6.1  1993/04/22  18:31:07  dleslie
 * First R1_0 release
 *
 * Revision 0.5  1993/04/05  02:09:28  andyp
 * Expunged the refreshdebug() code.
 *
 * Revision 0.4  1993/04/05  01:41:14  andyp
 * Added new RPM driver probe.  Condensed the SCSI driver lines
 * as they no longer change often.  Cosmetic changes to probe
 * order and printout.
 *
 * Revision 0.3  1992/10/01  00:42:30  andyp
 * Added hooks for software refresh.
 *
 * Revision 0.2  1992/08/31  10:49:27  andyp
 * New SCSI mods from Jerrie.
 *
 * Revision 0.1  92/07/09  17:00:12  andyp
 * Initial checkin of Paragon files.
 * 
 *
 */
#include <sys/types.h>
#include <chips/busses.h>
#include <i860paragon/expansion.h>

#include <mio.h>
#if	NMIO > 0
#include <i860paragon/mio/mio.h>
#include <i860paragon/mio/miodev.h>
extern struct bus_driver	miodriver;
extern int			miointr();
#endif	NMIO > 0

#include <com.h>
#if	NCOM > 0
extern struct bus_driver	com_driver;
extern int			com_intr();
#endif	NCOM > 0

#include <rpm.h>
#if	NRPM > 0
#include <i860paragon/rpm.h>
extern struct bus_driver	rpm_driver;
#endif	NRPM > 0

#include <mioe.h>
#if	NMIOE > 0
extern struct bus_driver	mioe_driver;
extern int			mioe_intr();
#endif	NMIOE > 0

#include <asc.h>
#if	NASC > 0
extern struct bus_driver	asc_driver;
extern int			asc_intr();
#endif	NASC > 0

#include <sdb.h>
#if	NSDB > 0
#include <i860paragon/sdb/sdb.h>
#include <i860paragon/sdb/sdb_dev.h>
extern struct bus_driver	sdb_driver;
extern int			sdb_intr();
#endif	NSDB > 0

#include <sdbs.h>
#if	NSDBS > 0
extern struct bus_driver	sdbs_driver;
extern int			sdbs_intr();
#endif	NSDBS > 0

#include <sdbe.h>
#if	NSDBE > 0
extern struct bus_driver	sdbe_driver;
extern int			sdbe_intr();
#endif	NSDBE > 0

#include <siop.h>
#if	NSIOP > 0
extern struct bus_driver	siop_driver;
extern int			siop_intr();
#endif	NSIOP > 0

#include <hippi.h>
#if     NHIPPI > 0
#include <i860paragon/hippi/hippidev.h>
extern struct bus_driver        hippi_driver;
extern int                      hippi_intr();
extern struct bus_driver        ifhip_driver;
extern struct bus_driver        rhippi_driver;
#endif  NHIPPI > 0

#include <hdc.h>
#if     NHDC > 0
extern struct bus_driver        hdc_driver;
#endif  NHDC > 0


struct	bus_ctlr	bus_master_init[] = {

#if	NMIO > 0
	{
		&miodriver,	/* driver:	driver for this controller */
		"mio",		/* name:	name of the controller */
		0,		/* unit:	index in driver */
		miointr,	/* intr:	interrupt handler */
		0,		/* address:	device virtual address */
		0,		/* am:		address modifier */
		(caddr_t) MIO_BASE_SRAM,/* phys_address:dev physical address */
		'?',		/* adaptor:	slot where found */
		0,		/* alive:	probed successfully */
		0,		/* flags:	any special conditions */
		0,		/* sysdep:	system dependent */
		0		/* sysdep1:	system dependent */
	},
#endif	NMIO > 0

#if	NASC > 0
	{
		&asc_driver,	/* driver:	driver for this controller */
		"asc",		/* name:	name of the controller */
		0,		/* unit:	index in driver */
		asc_intr,	/* intr:	interrupt handler */
		0,		/* address:	device virtual address */
		0,		/* am:		address modifier */
		(caddr_t) MIO_SCSI_START,/* phys_address:dev physical address */
		'?',		/* adaptor:	slot where found (wildcard) */
		0,		/* alive:	probed successfully */
		0,		/* flags:	any special conditions */
		(caddr_t) (16*2),/* sysdep:     (length in bytes) */
		0		/* sysdep1:	system dependent */
	},
#endif	NASC > 0

#if	NSDB > 0
	{
		&sdb_driver,	/* driver:	driver for this controller */
		"sdb",		/* name:	name of the controller */
		0,		/* unit:	index in driver */
		sdb_intr,	/* intr:	interrupt handler */
		0,		/* address:	device virtual address */
		0,		/* am:		address modifier */
		(caddr_t) SDB_SRAM_START,/* phys_address:dev physical address */
		'?',		/* adaptor:	slot where found */
		0,		/* alive:	probed successfully */
		0,		/* flags:	any special conditions */
		0,		/* sysdep:	system dependent */
		0		/* sysdep1:	system dependent */
	},
#endif	NSDB > 0

#if	NSIOP > 0
	{
		&siop_driver,	/* driver:	driver for this controller */
		"siop",		/* name:	name of the controller */
		0,		/* unit:	index in driver */
		siop_intr,	/* intr:	interrupt handler */
		0,		/* address:	device virtual address */
		0,		/* am:		address modifier */
		(caddr_t) SDB_SCSI0_START,/* phys_address:dev phys address */
		0,		/* adaptor:	slot where found (wildcard) */
		0,		/* alive:	probed successfully */
		0,		/* flags:	any special conditions */
		(caddr_t) (24*4),/* sysdep:     (length in bytes) */
		0		/* sysdep1:	system dependent */
	},
#if	NSIOP > 1
	{
		&siop_driver,	/* driver:	driver for this controller */
		"siop",		/* name:	name of the controller */
		1,		/* unit:	index in driver */
		siop_intr,	/* intr:	interrupt handler */
		0,		/* address:	device virtual address */
		0,		/* am:		address modifier */
		(caddr_t) SDB_SCSI1_START,/* phys_address:dev phys address */
		1,		/* adaptor:	slot where found (wildcard) */
		0,		/* alive:	probed successfully */
		0,		/* flags:	any special conditions */
		(caddr_t) (24*4),/* sysdep:     (length in bytes) */
		0		/* sysdep1:	system dependent */
	},
#endif	NSIOP > 1
#endif	NSIOP > 0

#if     NHIPPI > 0
	{
		&hippi_driver,  /* driver:      driver for this controller */
		"hippi",        /* name:        name of the controller */
		0,              /* unit:        index in driver */
		hippi_intr,     /* intr:        interrupt handler */
		0,              /* address:     device virtual address */
		0,              /* am:          address modifier */
		(caddr_t) HIPPI_SRAM_START, /* phys_address: dev address */
		'?',            /* adaptor:     slot where found */
		0,              /* alive:       probed successfully */
		0,              /* flags:       any special conditions */
		0,              /* sysdep:      system dependent */
		0               /* sysdep1:     system dependent */
	},
#endif  NHIPPI > 0

#if     NHDC > 0
		/*
		 * hdc_driver MUST be configured AFTER the hippidriver
		 */
	{
		&hdc_driver,    /* driver:      driver for this controller  */
		"hdc",          /* name:        name of the controller      */
		0,              /* unit:        index in driver             */
		0,              /* intr:        interrupt handler           */
		0,              /* address:     device virtual address      */
		0,              /* am:          address modifier            */
		0,              /* phys_address: dev address                */
		'?',            /* adaptor:     slot where found (wildcard) */
		0,              /* alive:       probed successfully         */
		0,              /* flags:       any special conditions      */
		0,              /* sysdep:      system dependent            */
		0               /* sysdep1:     system dependent            */
	},
#endif  NHDC > 0

	{ 0 }
};

struct	bus_device	bus_device_init[] = {

#if	NRPM > 0
	{
		&rpm_driver,	/* driver */
		"rpm",		/* name */
		0,		/* unit */
		0,		/* intr */
		0,		/* address */
		0,		/* am */
		(caddr_t) RPM_BASE_PADDR,/* phys_address */
		'?',		/* adaptor (wildcard) */
		0,		/* alive */
		-1,		/* controller */
		-1,		/* slave */
		0,		/* flags */
		0,		/* mi */
		0,		/* next */
		0,		/* sysdep */
		0		/* sysdep1 */
	},
#endif	NRPM > 0

#if	NMIO > 0
	{
		/* 512KB on-board static ram */
		&miodriver,	/* driver */
		"miomem",	/* name */
		0,		/* unit */
		miointr,	/* intr */
		0,		/* address */
		0,		/* am */
		(caddr_t) MIO_BASE_SRAM, /* phys_address */
		'?',		/* adaptor (wildcard) */
		0,		/* alive */
		0,		/* controller */
		-1,		/* slave */
		0,		/* flags */
		0,		/* mi */
		0,		/* next */
		(caddr_t) (512*1024),/* sysdep (length in bytes) */
		0		/* sysdep1 */
	},
	{
		/* 256KB on-board flash eeprom */
		&miodriver,	/* driver */
		"miomem",	/* name */
		1,		/* unit */
		miointr,	/* intr */
		0,		/* address */
		0,		/* am */
		(caddr_t) 0xbffc0000,/* phys_address */
		'?',		/* adaptor (wildcard) */
		0,		/* alive */
		0,		/* controller */
		-1,		/* slave */
		0,		/* flags */
		0,		/* mi */
		0,		/* next */
		(caddr_t) (256*1024),/* sysdep (length in bytes) */
		0		/* sysdep1 */
	},
#endif	NMIO > 0

#if	NCOM > 0
	{
		&com_driver,	/* driver */
		"com",		/* name */
		0,		/* unit */
		com_intr,	/* intr */
		0,		/* address */
		0,		/* am */
		(caddr_t) MIO_SERIAL_START,/* phys_address */
		'?',		/* adaptor (wildcard) */
		0,		/* alive */
		-1,		/* controller */
		-1,		/* slave */
		0,		/* flags */
		0,		/* mi */
		0,		/* next */
		0,		/* sysdep */
		0		/* sysdep1 */
	},
#endif	NCOM > 0

#if	NMIOE > 0
	{
		&mioe_driver,	/* driver */
		"mioe",		/* name */
		0,		/* unit */
		mioe_intr,	/* intr */
		0,		/* address */
		0,		/* am */
		(caddr_t) 0x80070000,/* phys_address */
		'?',		/* adaptor (wildcard) */
		0,		/* alive */
		-1,		/* controller */
		-1,		/* slave */
		0,		/* flags */
		0,		/* mi */
		0,		/* next */
		0,		/* sysdep */
		0		/* sysdep1 */
	},
#endif	NMIOE > 0

#if	NASC > 0

	{ &asc_driver, "rz", 0, 0, 0, 0, 0, '?', 0, 0, 0, 0, 0, 0, 0, 0 },
	{ &asc_driver, "rz", 1, 0, 0, 0, 0, '?', 0, 0, 1, 0, 0, 0, 0, 0	},
	{ &asc_driver, "rz", 2, 0, 0, 0, 0, '?', 0, 0, 2, 0, 0, 0, 0, 0 },
	{ &asc_driver, "rz", 3, 0, 0, 0, 0, '?', 0, 0, 3, 0, 0, 0, 0, 0 },
	{ &asc_driver, "rz", 4, 0, 0, 0, 0, '?', 0, 0, 4, 0, 0, 0, 0, 0	},
	{ &asc_driver, "rz", 5, 0, 0, 0, 0, '?', 0, 0, 5, 0, 0, 0, 0, 0	},
	{ &asc_driver, "rz", 6, 0, 0, 0, 0, '?', 0, 0, 6, 0, 0, 0, 0, 0 },
	{ &asc_driver, "rz", 7, 0, 0, 0, 0, '?', 0, 0, 7, 0, 0, 0, 0, 0 },

	{ &asc_driver, "tz", 0, 0, 0, 0, 0, '?', 0, 0, 0, 0, 0, 0, 0, 0 },
	{ &asc_driver, "tz", 1, 0, 0, 0, 0, '?', 0, 0, 1, 0, 0, 0, 0, 0	},
	{ &asc_driver, "tz", 2, 0, 0, 0, 0, '?', 0, 0, 2, 0, 0, 0, 0, 0 },
	{ &asc_driver, "tz", 3, 0, 0, 0, 0, '?', 0, 0, 3, 0, 0, 0, 0, 0	},
	{ &asc_driver, "tz", 4, 0, 0, 0, 0, '?', 0, 0, 4, 0, 0, 0, 0, 0	},
	{ &asc_driver, "tz", 5, 0, 0, 0, 0, '?', 0, 0, 5, 0, 0, 0, 0, 0	},
	{ &asc_driver, "tz", 6, 0, 0, 0, 0, '?', 0, 0, 6, 0, 0, 0, 0, 0	},
	{ &asc_driver, "tz", 7, 0, 0, 0, 0, '?', 0, 0, 7, 0, 0, 0, 0, 0	},

	{ &asc_driver, "mz", 0, 0, 0, 0, 0, '?', 0, 0, 0, 0, 0, 0, 0, 0 },
	{ &asc_driver, "mz", 1, 0, 0, 0, 0, '?', 0, 0, 1, 0, 0, 0, 0, 0 },
	{ &asc_driver, "mz", 2, 0, 0, 0, 0, '?', 0, 0, 2, 0, 0, 0, 0, 0 },
	{ &asc_driver, "mz", 3, 0, 0, 0, 0, '?', 0, 0, 3, 0, 0, 0, 0, 0 },
	{ &asc_driver, "mz", 4, 0, 0, 0, 0, '?', 0, 0, 4, 0, 0, 0, 0, 0 },
	{ &asc_driver, "mz", 5, 0, 0, 0, 0, '?', 0, 0, 5, 0, 0, 0, 0, 0 },
	{ &asc_driver, "mz", 6, 0, 0, 0, 0, '?', 0, 0, 6, 0, 0, 0, 0, 0 },
	{ &asc_driver, "mz", 7, 0, 0, 0, 0, '?', 0, 0, 7, 0, 0, 0, 0, 0 },

	{ &asc_driver, "jz", 0, 0, 0, 0, 0, '?', 0, 0, 0, 0, 0, 0, 0, 0 },
	{ &asc_driver, "jz", 1, 0, 0, 0, 0, '?', 0, 0, 1, 0, 0, 0, 0, 0	},
	{ &asc_driver, "jz", 2, 0, 0, 0, 0, '?', 0, 0, 2, 0, 0, 0, 0, 0 },
	{ &asc_driver, "jz", 3, 0, 0, 0, 0, '?', 0, 0, 3, 0, 0, 0, 0, 0	},
	{ &asc_driver, "jz", 4, 0, 0, 0, 0, '?', 0, 0, 4, 0, 0, 0, 0, 0	},
	{ &asc_driver, "jz", 5, 0, 0, 0, 0, '?', 0, 0, 5, 0, 0, 0, 0, 0	},
	{ &asc_driver, "jz", 6, 0, 0, 0, 0, '?', 0, 0, 6, 0, 0, 0, 0, 0	},
	{ &asc_driver, "jz", 7, 0, 0, 0, 0, '?', 0, 0, 7, 0, 0, 0, 0, 0	},

#endif	NASC > 0

#if	NSDB > 0
	{
		/* On-board static ram */
		&sdb_driver,	/* driver */
		"sdbmem",	/* name */
		SDB_DEV_SRAM,	/* unit */
		sdb_intr,	/* intr */
		0,		/* address */
		0,		/* am */
		(caddr_t) SDB_SRAM_START, /* phys_address */
		'?',		/* adaptor (wildcard) */
		0,		/* alive */
		0,		/* controller */
		-1,		/* slave */
		0,		/* flags */
		0,		/* mi */
		0,		/* next */
		(caddr_t) SDB_SRAM_SIZE,/* sysdep (length in bytes) */
		0		/* sysdep1 */
	},
	{
		/* On-board dual-port ram */
		&sdb_driver,	/* driver */
		"sdbmem",	/* name */
		SDB_DEV_DPRAM,	/* unit */
		sdb_intr,	/* intr */
		0,		/* address */
		0,		/* am */
		(caddr_t) SDB_DPRAM_START, /* phys_address */
		'?',		/* adaptor (wildcard) */
		0,		/* alive */
		0,		/* controller */
		-1,		/* slave */
		0,		/* flags */
		0,		/* mi */
		0,		/* next */
		(caddr_t) SDB_DPRAM_SIZE,/* sysdep (length in bytes) */
		0		/* sysdep1 */
	},
	{
		/* On-board flash eeprom */
		&sdb_driver,	/* driver */
		"sdbmem",	/* name */
		SDB_DEV_FLASH,	/* unit */
		sdb_intr,	/* intr */
		0,		/* address */
		0,		/* am */
		(caddr_t) SDB_FLASH_START,/* phys_address */
		'?',		/* adaptor (wildcard) */
		0,		/* alive */
		0,		/* controller */
		-1,		/* slave */
		0,		/* flags */
		0,		/* mi */
		0,		/* next */
		(caddr_t) SDB_FLASH_SIZE,/* sysdep (length in bytes) */
		0		/* sysdep1 */
	},
#endif	NSDB > 0

#if	NSDBS > 0
	{
		&sdbs_driver,	/* driver */
		"sdbs",		/* name */
		0,		/* unit */
		sdbs_intr,	/* intr */
		0,		/* address */
		0,		/* am */
		(caddr_t) SDB_SERIAL_START,/* phys_address */
		'?',		/* adaptor (wildcard) */
		0,		/* alive */
		-1,		/* controller */
		-1,		/* slave */
		0,		/* flags */
		0,		/* mi */
		0,		/* next */
		0,		/* sysdep */
		0		/* sysdep1 */
	},
#endif	NSDBS > 0

#if	NSDBE > 0
	{
		&sdbe_driver,	/* driver */
		"sdbe",		/* name */
		0,		/* unit */
		sdbe_intr,	/* intr */
		0,		/* address */
		0,		/* am */
		(caddr_t) SDB_ETHER_START,/* phys_address */
		'?',		/* adaptor (wildcard) */
		0,		/* alive */
		-1,		/* controller */
		-1,		/* slave */
		0,		/* flags */
		0,		/* mi */
		0,		/* next */
		0,		/* sysdep */
		0		/* sysdep1 */
	},
#endif	NSDBE > 0

#if	NSIOP > 0

	{ &siop_driver, "rz",  0, 0, 0, 0, 0, '?', 0, 0,  0, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",  1, 0, 0, 0, 0, '?', 0, 0,  1, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",  2, 0, 0, 0, 0, '?', 0, 0,  2, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",  3, 0, 0, 0, 0, '?', 0, 0,  3, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",  4, 0, 0, 0, 0, '?', 0, 0,  4, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",  5, 0, 0, 0, 0, '?', 0, 0,  5, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",  6, 0, 0, 0, 0, '?', 0, 0,  6, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",  7, 0, 0, 0, 0, '?', 0, 0,  7, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",  8, 0, 0, 0, 0, '?', 0, 0,  8, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",  9, 0, 0, 0, 0, '?', 0, 0,  9, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz", 10, 0, 0, 0, 0, '?', 0, 0, 10, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz", 11, 0, 0, 0, 0, '?', 0, 0, 11, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz", 12, 0, 0, 0, 0, '?', 0, 0, 12, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz", 13, 0, 0, 0, 0, '?', 0, 0, 13, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz", 14, 0, 0, 0, 0, '?', 0, 0, 14, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz", 15, 0, 0, 0, 0, '?', 0, 0, 15, 0, 0, 0, 0, 0 },

	{ &siop_driver, "tz",  0, 0, 0, 0, 0, '?', 0, 0,  0, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",  1, 0, 0, 0, 0, '?', 0, 0,  1, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",  2, 0, 0, 0, 0, '?', 0, 0,  2, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",  3, 0, 0, 0, 0, '?', 0, 0,  3, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",  4, 0, 0, 0, 0, '?', 0, 0,  4, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",  5, 0, 0, 0, 0, '?', 0, 0,  5, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",  6, 0, 0, 0, 0, '?', 0, 0,  6, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",  7, 0, 0, 0, 0, '?', 0, 0,  7, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",  8, 0, 0, 0, 0, '?', 0, 0,  8, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",  9, 0, 0, 0, 0, '?', 0, 0,  9, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz", 10, 0, 0, 0, 0, '?', 0, 0, 10, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz", 11, 0, 0, 0, 0, '?', 0, 0, 11, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz", 12, 0, 0, 0, 0, '?', 0, 0, 12, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz", 13, 0, 0, 0, 0, '?', 0, 0, 13, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz", 14, 0, 0, 0, 0, '?', 0, 0, 14, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz", 15, 0, 0, 0, 0, '?', 0, 0, 15, 0, 0, 0, 0, 0 },

	{ &siop_driver, "mz",  0, 0, 0, 0, 0, '?', 0, 0,  0, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",  1, 0, 0, 0, 0, '?', 0, 0,  1, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",  2, 0, 0, 0, 0, '?', 0, 0,  2, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",  3, 0, 0, 0, 0, '?', 0, 0,  3, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",  4, 0, 0, 0, 0, '?', 0, 0,  4, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",  5, 0, 0, 0, 0, '?', 0, 0,  5, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",  6, 0, 0, 0, 0, '?', 0, 0,  6, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",  7, 0, 0, 0, 0, '?', 0, 0,  7, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",  8, 0, 0, 0, 0, '?', 0, 0,  8, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",  9, 0, 0, 0, 0, '?', 0, 0,  9, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz", 10, 0, 0, 0, 0, '?', 0, 0, 10, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz", 11, 0, 0, 0, 0, '?', 0, 0, 11, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz", 12, 0, 0, 0, 0, '?', 0, 0, 12, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz", 13, 0, 0, 0, 0, '?', 0, 0, 13, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz", 14, 0, 0, 0, 0, '?', 0, 0, 14, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz", 15, 0, 0, 0, 0, '?', 0, 0, 15, 0, 0, 0, 0, 0 },

	{ &siop_driver, "jz",  0, 0, 0, 0, 0, '?', 0, 0,  0, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",  1, 0, 0, 0, 0, '?', 0, 0,  1, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",  2, 0, 0, 0, 0, '?', 0, 0,  2, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",  3, 0, 0, 0, 0, '?', 0, 0,  3, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",  4, 0, 0, 0, 0, '?', 0, 0,  4, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",  5, 0, 0, 0, 0, '?', 0, 0,  5, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",  6, 0, 0, 0, 0, '?', 0, 0,  6, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",  7, 0, 0, 0, 0, '?', 0, 0,  7, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",  8, 0, 0, 0, 0, '?', 0, 0,  8, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",  9, 0, 0, 0, 0, '?', 0, 0,  9, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz", 10, 0, 0, 0, 0, '?', 0, 0, 10, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz", 11, 0, 0, 0, 0, '?', 0, 0, 11, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz", 12, 0, 0, 0, 0, '?', 0, 0, 12, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz", 13, 0, 0, 0, 0, '?', 0, 0, 13, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz", 14, 0, 0, 0, 0, '?', 0, 0, 14, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz", 15, 0, 0, 0, 0, '?', 0, 0, 15, 0, 0, 0, 0, 0 },

#if	NSIOP > 1

	{ &siop_driver, "rz",16+ 0, 0, 0, 0, 0, '?', 0, 1,  0, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+ 1, 0, 0, 0, 0, '?', 0, 1,  1, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+ 2, 0, 0, 0, 0, '?', 0, 1,  2, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+ 3, 0, 0, 0, 0, '?', 0, 1,  3, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+ 4, 0, 0, 0, 0, '?', 0, 1,  4, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+ 5, 0, 0, 0, 0, '?', 0, 1,  5, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+ 6, 0, 0, 0, 0, '?', 0, 1,  6, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+ 7, 0, 0, 0, 0, '?', 0, 1,  7, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+ 8, 0, 0, 0, 0, '?', 0, 1,  8, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+ 9, 0, 0, 0, 0, '?', 0, 1,  9, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+10, 0, 0, 0, 0, '?', 0, 1, 10, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+11, 0, 0, 0, 0, '?', 0, 1, 11, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+12, 0, 0, 0, 0, '?', 0, 1, 12, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+13, 0, 0, 0, 0, '?', 0, 1, 13, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+14, 0, 0, 0, 0, '?', 0, 1, 14, 0, 0, 0, 0, 0 },
	{ &siop_driver, "rz",16+15, 0, 0, 0, 0, '?', 0, 1, 15, 0, 0, 0, 0, 0 },

	{ &siop_driver, "tz",16+ 0, 0, 0, 0, 0, '?', 0, 1,  0, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+ 1, 0, 0, 0, 0, '?', 0, 1,  1, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+ 2, 0, 0, 0, 0, '?', 0, 1,  2, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+ 3, 0, 0, 0, 0, '?', 0, 1,  3, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+ 4, 0, 0, 0, 0, '?', 0, 1,  4, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+ 5, 0, 0, 0, 0, '?', 0, 1,  5, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+ 6, 0, 0, 0, 0, '?', 0, 1,  6, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+ 7, 0, 0, 0, 0, '?', 0, 1,  7, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+ 8, 0, 0, 0, 0, '?', 0, 1,  8, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+ 9, 0, 0, 0, 0, '?', 0, 1,  9, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+10, 0, 0, 0, 0, '?', 0, 1, 10, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+11, 0, 0, 0, 0, '?', 0, 1, 11, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+12, 0, 0, 0, 0, '?', 0, 1, 12, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+13, 0, 0, 0, 0, '?', 0, 1, 13, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+14, 0, 0, 0, 0, '?', 0, 1, 14, 0, 0, 0, 0, 0 },
	{ &siop_driver, "tz",16+15, 0, 0, 0, 0, '?', 0, 1, 15, 0, 0, 0, 0, 0 },

	{ &siop_driver, "mz",16+ 0, 0, 0, 0, 0, '?', 0, 1,  0, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+ 1, 0, 0, 0, 0, '?', 0, 1,  1, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+ 2, 0, 0, 0, 0, '?', 0, 1,  2, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+ 3, 0, 0, 0, 0, '?', 0, 1,  3, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+ 4, 0, 0, 0, 0, '?', 0, 1,  4, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+ 5, 0, 0, 0, 0, '?', 0, 1,  5, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+ 6, 0, 0, 0, 0, '?', 0, 1,  6, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+ 7, 0, 0, 0, 0, '?', 0, 1,  7, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+ 8, 0, 0, 0, 0, '?', 0, 1,  8, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+ 9, 0, 0, 0, 0, '?', 0, 1,  9, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+10, 0, 0, 0, 0, '?', 0, 1, 10, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+11, 0, 0, 0, 0, '?', 0, 1, 11, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+12, 0, 0, 0, 0, '?', 0, 1, 12, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+13, 0, 0, 0, 0, '?', 0, 1, 13, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+14, 0, 0, 0, 0, '?', 0, 1, 14, 0, 0, 0, 0, 0 },
	{ &siop_driver, "mz",16+15, 0, 0, 0, 0, '?', 0, 1, 15, 0, 0, 0, 0, 0 },

	{ &siop_driver, "jz",16+ 0, 0, 0, 0, 0, '?', 0, 1,  0, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+ 1, 0, 0, 0, 0, '?', 0, 1,  1, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+ 2, 0, 0, 0, 0, '?', 0, 1,  2, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+ 3, 0, 0, 0, 0, '?', 0, 1,  3, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+ 4, 0, 0, 0, 0, '?', 0, 1,  4, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+ 5, 0, 0, 0, 0, '?', 0, 1,  5, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+ 6, 0, 0, 0, 0, '?', 0, 1,  6, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+ 7, 0, 0, 0, 0, '?', 0, 1,  7, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+ 8, 0, 0, 0, 0, '?', 0, 1,  8, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+ 9, 0, 0, 0, 0, '?', 0, 1,  9, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+10, 0, 0, 0, 0, '?', 0, 1, 10, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+11, 0, 0, 0, 0, '?', 0, 1, 11, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+12, 0, 0, 0, 0, '?', 0, 1, 12, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+13, 0, 0, 0, 0, '?', 0, 1, 13, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+14, 0, 0, 0, 0, '?', 0, 1, 14, 0, 0, 0, 0, 0 },
	{ &siop_driver, "jz",16+15, 0, 0, 0, 0, '?', 0, 1, 15, 0, 0, 0, 0, 0 },

#endif	NSIOP > 1
#endif	NSIOP > 0

#if     NHIPPI > 0
	{
        	/* 128KB on-board static ram */
		&hippi_driver,  /* driver */
		"hippimem",     /* name */
		0,              /* unit */
		hippi_intr,     /* intr */
		0,              /* address */
		0,              /* am */
		(caddr_t) HIPPI_SRAM_START, /* phys_address */
		'?',            /* adaptor (wildcard) */
		0,              /* alive */
		0,              /* controller */
		-1,             /* slave */
		0,              /* flags */
		0,              /* mi */
		0,              /* next */
		(caddr_t) HIPPI_SRAM_SIZE,/* sysdep (length in bytes) */
		0               /* sysdep1 */
	},
        {
		/* 256KB on-board flash eeprom */
		&hippi_driver,   /* driver */
		"hippimem",     /* name */
		1,              /* unit */
		hippi_intr,     /* intr */
		0,              /* address */
		0,              /* am */
		(caddr_t) HIPPI_FLASH_START,/* phys_address */
		'?',            /* adaptor (wildcard) */
		0,              /* alive */
		0,              /* controller */
		-1,             /* slave */
		0,              /* flags */
		0,              /* mi */
		0,              /* next */
		(caddr_t) HIPPI_FLASH_SIZE, /* sysdep (length in bytes) */
		0               /* sysdep1 */
	},
        {
		/* HiPPI network interface driver, scatter-gather support */
		&ifhip_driver,  /* driver */
		"ifhip",        /* name */
		0,              /* unit */
		hippi_intr,     /* intr */
		0,              /* address */
		0,              /* amr */
		(caddr_t) HIPPI_SRAM_START,      /* phys_address */
		'?',            /* adaptor (wildcard) */
		0,              /* alive */
		-1,             /* controller */
		-1,             /* slave */
		0,              /* flags */
		0,              /* mi    */
		0,              /* next   */
		0,              /* sysdep */
		0               /* sysdep1 */
	},
        {
	/* HiPPI raw interface driver, scatter-gather support */
		&rhippi_driver, /* driver */
		"rhippi",       /* name */
		0,              /* unit */
		hippi_intr,     /* intr */
                0,              /* address */
		0,              /* amr */
		0,              /* phys_address */
		'?',            /* adaptor (wildcard) */
		0,              /* alive */
		-1,             /* controller */
		-1,             /* slave */
		0,              /* flags */
		0,              /* mi    */
		0,              /* next   */
		0,              /* sysdep */
		0               /* sysdep1 */
	},
#endif  NHIPPI > 0

#if     NHDC > 0

	{ &hdc_driver, "imd", 0, 0, 0, 0, 0, '?', 0, 0, 0, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imd", 1, 0, 0, 0, 0, '?', 0, 0, 1, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imd", 2, 0, 0, 0, 0, '?', 0, 0, 2, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imd", 3, 0, 0, 0, 0, '?', 0, 0, 3, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imd", 4, 0, 0, 0, 0, '?', 0, 0, 4, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imd", 5, 0, 0, 0, 0, '?', 0, 0, 5, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imd", 6, 0, 0, 0, 0, '?', 0, 0, 6, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imd", 7, 0, 0, 0, 0, '?', 0, 0, 7, 0, 0, 0, 0, 0 },

	{ &hdc_driver, "iod", 0, 0, 0, 0, 0, '?', 0, 0, 0, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "iod", 1, 0, 0, 0, 0, '?', 0, 0, 1, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "iod", 2, 0, 0, 0, 0, '?', 0, 0, 2, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "iod", 3, 0, 0, 0, 0, '?', 0, 0, 3, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "iod", 4, 0, 0, 0, 0, '?', 0, 0, 4, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "iod", 5, 0, 0, 0, 0, '?', 0, 0, 5, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "iod", 6, 0, 0, 0, 0, '?', 0, 0, 6, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "iod", 7, 0, 0, 0, 0, '?', 0, 0, 7, 0, 0, 0, 0, 0 },

	{ &hdc_driver, "imt", 0, 0, 0, 0, 0, '?', 0, 0, 0, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imt", 1, 0, 0, 0, 0, '?', 0, 0, 1, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imt", 2, 0, 0, 0, 0, '?', 0, 0, 2, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imt", 3, 0, 0, 0, 0, '?', 0, 0, 3, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imt", 4, 0, 0, 0, 0, '?', 0, 0, 4, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imt", 5, 0, 0, 0, 0, '?', 0, 0, 5, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imt", 6, 0, 0, 0, 0, '?', 0, 0, 6, 0, 0, 0, 0, 0 },
	{ &hdc_driver, "imt", 7, 0, 0, 0, 0, '?', 0, 0, 7, 0, 0, 0, 0, 0 },

#endif  NHDC > 0

	{ 0 }
};


probeio()
{
	struct bus_device *device;
	struct bus_ctlr	*master;
	int	i = 0;
	extern int	ipsc_physnode;
	char 	*board;
	extern int expansion_id();

#if	NRPM > 0
	{
		configure_bus_device("rpm", 0, RPM_BASE_PADDR,
			ipsc_physnode, "gp");
	}
#endif	NRPM > 0

	if (node_status_register_expansion_present() == 0)
		return;

	switch (expansion_id() & EXP_ID_MASK) {
	case EXP_ID_HIPPI:
		board = "hippi";
		break;

	case EXP_ID_MIO:
		board = "mio";
		break;

	case EXP_ID_SCSI:
		board = "sdb";
		break;

	case EXP_ID_MEMX16:
	case EXP_ID_MEMX32:
	case EXP_ID_MEMX64:
	case EXP_ID_MEMX128:
	default:
		return;
	}

#if	NSDB > 0
    configure_bus_master("sdb", 0, SDB_SRAM_START, ipsc_physnode, "node");
#endif	NSDB > 0

#if	NSIOP > 0
    configure_bus_master("siop", 0, SDB_SCSI0_START, 0, "expansion");
#endif	NSIOP > 0

#if	NSIOP > 1
    configure_bus_master("siop", 0, SDB_SCSI1_START, 1, "expansion");
#endif	NSIOP > 1

	for (master = bus_master_init; master->driver; master++)
		if (configure_bus_master(master->name, master->address,
				master->phys_address, i, "expansion"))
			i++;

	for (device = bus_device_init; device->driver; device++) {
		if (device->alive || device->ctlr != -1)
			continue;
		if (configure_bus_device(device->name, device->address,
				device->phys_address, i, board))
			i++;
		else
			device->adaptor = ' ';
	}
}
