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
 * $Log: conf.c,v $
 * Revision 0.14  1995/03/28  01:23:45  jerrie
 * Added generic ethernet device interface handler.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low.
 *  Benefit or PTS #: 12827
 *  Testing:	   Developer
 *  Module(s):	   See PTS report.
 *
 * Revision 0.13  1995/03/14  23:42:46  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 0.12  1995/02/02  21:32:58  jerrie
 * Macros used in the IPI device driver require the number of sub-devices
 * to be set to 16.  The kernel dev_ops structure master device entry had
 * the number of sub-devices incorrectly set to 0.  In addition, the server
 * character device name string function was not checking for the number of
 * sub-devices in the cdevsw table and was incorrectly using the number of
 * sub-devices from the bdevsw table when building the character device name.
 *
 *  Reviewer:         Arlin Davis
 *  Risk:             Low.  Only affects the IPI device driver.
 *  Benefit or PTS #: 12064
 *  Testing:          Ran the EAT test mention in the PTS bug report.
 *                    Developed a test program to manually exercise the
 *                    interface.
 *  Module(s):        The following files are being modified:
 *
 *                    kernel/i860paragon:     conf.c
 *                    server/i860:            ipi_ops.c
 *
 * Revision 0.11  1994/11/18  20:42:14  mtm
 * Copyright additions/changes
 *
 * Revision 0.10  1994/06/08  17:02:51  arlin
 * Initial Release of HiPPI IPI-3 driver
 *
 * Revision 0.9  1994/01/07  19:57:25  rkl
 *  Reviewer: andyp
 *  Risk: low
 *  Benefit or PTS #: Added ability to access performance counters on MIO
 *  Testing: Yes
 *  Module(s): if_mioe.c mio.h conf.c
 *
 * Revision 0.8  1993/10/20  15:35:10  rkl
 * Added mdsetstatus() as part of configuration to allow the RAM disk to be
 * used in a multi-user system.
 *
 * Revision 0.7  1993/10/07  22:09:43  stans
 * rkl's time tracing support
 *
 * Revision 0.6  1993/06/30  22:37:00  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.5  1993/05/27  22:10:12  arlin
 * Added Hippi Driver Support, ARD
 *
 * Revision 0.4  1993/04/27  20:31:15  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 0.3.6.1  1993/04/22  18:31:11  dleslie
 * First R1_0 release
 *
 * Revision 0.4  1993/04/05  01:33:08  andyp
 * Added entry points for the RPM driver.
 *
 * Revision 0.3  1993/01/18  19:01:08  regnier
 * support ramdisk device_get_status()
 *
 * Revision 0.2  1992/09/21  18:10:54  andyp
 * Change the name of the disks from "sd" to "rz" for norma12.
 *
 * Revision 0.1  1992/07/09  17:00:19  andyp
 * Initial checkin of Paragon files.
 *
 *
 */

#include <device/conf.h>

/*
 *	Console device
 */
int	cons_open(), cons_close(), cons_read(), cons_write(),
	cons_get_status(), cons_set_status(), cons_portdeath();
#define	cons_name	"cons"

/*
 *	Time device (mappable)
 */
extern int	timeopen(), timeclose(), timemmap();
#define	timename		"time"

/*
 *	RAM disk
 */
#include <md.h>
#if	NMD > 0
extern int	mdopen(), mdclose(), mdread(), mdwrite();
extern int	mdgetstatus(), mdsetstatus(), mddevinfo();
#define	mdname			"md"
#endif	NMD > 0

/*
 *	MIO and SCSI-16 SCSI things
 */
#include <asc.h>
#include <siop.h>
#if	NASC > 0 || NSIOP > 0
extern int	rz_open(), rz_close(), rz_read(), rz_write();
extern int	rz_get_status(), rz_set_status(), rz_devinfo();
#define	rzname	"rz"	/* disk */
#define tzname	"tz"	/* tape */
#define jzname	"jz"	/* jukebox */
#define mzname	"mz"	/* optical memory */
#endif	NASC > 0 || NSIOP > 0

/*
 *	MIO Ethernet
 */
#include <mioe.h>
#if	NMIOE > 0
extern int	mioe_open(), mioe_output(), mioe_getstat(), mioe_setstat(),
		mioe_setinput(), mioe_mmap();
#define	mioename	"em"
#endif	NMIOE > 0

/*
 *	MIO uart
 */
#include <com.h>
#if	NCOM > 0
extern int	com_open(), com_close(), com_read(), com_write();
extern int	com_getstat(), com_setstat(), com_portdeath();
#define com_name	"com"
#endif	NCOM > 0

/*
 *	SCSI-16 Board Performance Counters
 */
#include <sdb.h>
#if	NSDB > 0
extern int	sdb_open(), sdb_mmap();
#define sdbname	"sdb"	/* SCSI-16 daughter board */
#endif	NSDB > 0

/*
 *	SCSI-16 Ethernet
 */
#include <sdbe.h>
#if	NSDBE > 0
extern int	sdbe_open(), sdbe_close(), sdbe_output();
extern int	sdbe_getstat(), sdbe_setstat(), sdbe_setinput();
#define sdbename	"es"
#endif	NSDBE > 0

/*
 *	SCSI-16 UART
 */
#include <sdbs.h>
#if	NSDBS > 0
extern int	sdbs_open(), sdbs_close(), sdbs_read(), sdbs_write();
extern int	sdbs_getstat(), sdbs_setstat(), sdbs_portdeath();
#define sdbs_name	"sdbs"
#endif	NSDBS > 0

/*
 *	Generic Daughter Board Ethernet
 */
#if	NMIOE > 0 || NSDBE > 0
extern int	enet_open(), enet_close(), enet_output();
extern int	enet_getstat(), enet_setstat(), enet_setinput(), enet_mmap();
#define	enetname	"em"	/* same as MIO for backward compatibility */
#endif	NMIOE > 0 || NSDBE > 0

/*
 *	RPM device (mappable)
 */
#include <rpm.h>
#if	NRPM > 0
extern int	rpm_open(), rpm_close(), rpm_read(), rpm_write();
extern int	rpm_getstat(), rpm_setstat();
extern int	rpm_mmap(), rpm_reset(), rpm_devinfo();
#define rpm_name	"rpm"
#endif	NRPM > 0

#if     TIME_TRACE
/*
 *      Time tracing control device
 */
extern int	ttrace_open(), ttrace_close(), ttrace_read(), ttrace_write();
extern int	ttrace_getstat(), ttrace_setstat();
extern int	ttrace_mmap(), ttrace_devinfo();
#define ttrace_name "ttrace"
#endif

/*
 *      HiPPI
 */
#include <hippi.h>
#if     NHIPPI > 0
extern int      ifhip_open(), ifhip_output(), ifhip_getstat(), ifhip_setstat();
extern int      ifhip_setinput(), ifhip_devinfo(), hippi_mmap();
extern int      rhippi_open(), rhippi_close(), rhippi_read(), rhippi_write();
extern int      rhippi_getstat(), rhippi_setstat(), rhippi_devinfo(), rhippi_setinput();
#define ifhip_name      "ifhip"
#define rhippi_name     "rhippi"
#endif  NHIPPI > 0

/*
 *      IPI
 */
#include <hdc.h>
#if     NHDC > 0
extern int      ipm_open(), ipm_close();
extern int      ipi_open(), ipi_close();
extern int      ipi_read(), ipi_write();
extern int      ipm_get_status(), ipm_set_status();
extern int      ipi_get_status(), ipi_set_status();
extern int      ipi_devinfo();
#define ipmname "ipm"   /* master   dev  */
#define imdname "imd"   /* magnetic disk */
#define iodname "iod"   /* optical  disk */
#define imtname "imt"   /* magnetic tape */
#endif  NHDC > 0


/*
 * List of devices - console must be at slot 0
 */
struct dev_ops	dev_name_list[] =
{
	/*name,		open,		close,		read,
	  write,	getstat,	setstat,	mmap,
	  async_in,	reset,		port_death,	subdev,
	  dev_info */

	{ cons_name,	cons_open,	cons_close,	cons_read,
	  cons_write,	cons_get_status,cons_set_status,nodev,
	  nodev,	nulldev,	cons_portdeath,	0,
	  nodev },

	{ timename,	timeopen,	timeclose,	nulldev,
	  nulldev,	nulldev,	nulldev,	timemmap,
	  nodev,	nulldev,	nulldev,	0,
	  nodev },

#if	NMD > 0
	{ mdname,	mdopen,		mdclose,	mdread,
	  mdwrite,	mdgetstatus,	mdsetstatus,	nulldev,
	  nodev,	nulldev,	nulldev,	16,
	  mddevinfo },
#endif	NMD > 0

#if	0
#if	NCOM > 0
	{ com_name,	com_open,	com_close,	com_read,
	  com_write,	com_getstat,	com_setstat,	nodev,
	  nodev,	nulldev,	com_portdeath,	0,
	  nodev },
#endif	NCOM > 0
#endif	0

#if	NASC > 0 || NSIOP > 0
	{ rzname,	rz_open,	rz_close,	rz_read,
	  rz_write,	rz_get_status,	rz_set_status,	nodev,
	  nodev,	nulldev,	nulldev,	16,
	  rz_devinfo },
	{ tzname,	rz_open,	rz_close,	rz_read,
	  rz_write,	rz_get_status,	rz_set_status,	nodev,
	  nodev,	nulldev,	nulldev,	0,
	  rz_devinfo },
	{ jzname,	rz_open,	rz_close,	rz_read,
	  rz_write,	rz_get_status,	rz_set_status,	nodev,
	  nodev,	nulldev,	nulldev,	0,
	  rz_devinfo },
	{ mzname,	rz_open,	rz_close,	rz_read,
	  rz_write,	rz_get_status,	rz_set_status,	nodev,
	  nodev,	nulldev,	nulldev,	0,
	  rz_devinfo },
#endif	NASC > 0 || NSIOP > 0

#if	0
#if	NMIOE > 0
	{ mioename,	mioe_open,	nulldev,	nulldev,
	  mioe_output,	mioe_getstat,	mioe_setstat,	mioe_mmap,
	  mioe_setinput,nulldev,	nulldev,	0,
	  nodev },
#endif	NMIOE > 0
#endif	0

#if	NSDB > 0
	{ sdbname,	sdb_open,	nulldev,	nodev,
	  nodev,	nulldev,	nulldev,	sdb_mmap,
	  nodev,	nulldev,	nulldev,	0,
	  nodev },
#endif	NSDB > 0

#if	0
#if	NSDBS > 0
	{ sdbs_name,	sdbs_open,	sdbs_close,	sdbs_read,
	  sdbs_write,	sdbs_getstat,	sdbs_setstat,	nodev,
	  nodev,	nulldev,	sdbs_portdeath,	0,
	  nodev },
#endif	NSDBS > 0
#endif	0

#if	0
#if	NSDBE > 0
	{ sdbename,	sdbe_open,	sdbe_close,	nulldev,
	  sdbe_output,	sdbe_getstat,	sdbe_setstat,	nodev,
	  sdbe_setinput,nulldev,	nulldev,	0,
	  nodev },
#endif	NSDBE > 0
#endif	0

#if	NMIOE > 0 || NSDBE > 0
	{ enetname,	enet_open,	enet_close,	nulldev,
	  enet_output,	enet_getstat,	enet_setstat,	enet_mmap,
	  enet_setinput,nulldev,	nulldev,	0,
	  nodev },
#endif	NMIOE > 0 || NSDBE > 0

#if	NRPM > 0
	{ rpm_name,	rpm_open,	rpm_close,	rpm_read,
	  rpm_write,	rpm_getstat,	rpm_setstat,	rpm_mmap,
	  nodev,	rpm_reset,	nulldev,	0,
	  rpm_devinfo },
#endif	NRPM > 0

#if	TIME_TRACE
	{ ttrace_name,	ttrace_open,	ttrace_close,	ttrace_read,
	  ttrace_write,	ttrace_getstat,	ttrace_setstat,	ttrace_mmap,
	  nodev,	nulldev,	nulldev,	0,
	  ttrace_devinfo },
#endif	/* TIME_TRACE */

#if     NHIPPI > 0
        { ifhip_name,    ifhip_open,     nulldev,        nulldev,
	  ifhip_output,  ifhip_getstat,  ifhip_setstat,  hippi_mmap,
	  ifhip_setinput,  nulldev,      nulldev,        0,
	  ifhip_devinfo },

	{ rhippi_name,  rhippi_open,    rhippi_close,   rhippi_read,
	  rhippi_write, rhippi_getstat, rhippi_setstat, nulldev,
	  rhippi_setinput, nulldev,     nulldev,        0,
	  rhippi_devinfo },
#endif  NHIPPI > 0

#if     NHDC > 0
	{ ipmname,      ipm_open,       ipm_close,      nulldev,
	  nulldev,      ipm_get_status, ipm_set_status, nodev,
	  nodev,        nulldev,        nulldev,        16,
	  nodev },
	{ imdname,      ipi_open,       ipi_close,      ipi_read,
	  ipi_write,    ipi_get_status, ipi_set_status, nodev,
	  nodev,        nulldev,        nulldev,        16,
	  ipi_devinfo },
	{ iodname,      ipi_open,       ipi_close,      ipi_read,
	  ipi_write,    ipi_get_status, ipi_set_status, nodev,
	  nodev,        nulldev,        nulldev,        16,
	  ipi_devinfo },
	{ imtname,      ipi_open,       ipi_close,      ipi_read,
	  ipi_write,    ipi_get_status, ipi_set_status, nodev,
	  nodev,        nulldev,        nulldev,        16,
	  ipi_devinfo },
#endif  NHDC > 0

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
