/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: sysconfig.h,v $
 * Revision 1.4  1994/11/18  20:41:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:28:43  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:37:52  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:20  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:58  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:40:58  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:13:31  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:45:30  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:54:31  devrcs
 * 	filsys mod
 * 	[90/10/02  01:14:43  knight]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:45:44  gm]
 * 
 * Revision 1.4  90/09/23  16:00:21  devrcs
 * 	Dymanic configuration integration
 * 	[90/09/13  14:24:48  knight]
 * 
 * Revision 1.3  90/08/24  12:27:34  devrcs
 * 	Changed field names of device_config
 * 	[90/08/15  01:31:40  knight]
 * 
 * 	Cleaned up comments.
 * 	[90/08/15  00:02:43  knight]
 * 
 * 	Added defines for configure flags for device interrupts, device switch, and
 * 	device configuration structure.
 * 	[90/08/14  23:59:49  knight]
 * 
 * 	Added interrupt handler ih_flag defines
 * 	[90/08/13  23:38:40  knight]
 * 
 * 	I do not like "merge".
 * 	[90/07/11  19:41:15  knight]
 * 
 * 	Added interrupt handler ih_flag defines
 * 	[90/08/13  23:38:40  knight]
 * 
 * 	I do not like "merge".
 * 	[90/07/11  19:41:15  knight]
 * 
 * 	Cleaned up comments.
 * 	[90/08/15  00:02:43  knight]
 * 
 * 	Added defines for configure flags for device interrupts, device switch, and
 * 	device configuration structure.
 * 	[90/08/14  23:59:49  knight]
 * 
 * 	Added interrupt handler ih_flag defines
 * 	[90/08/13  23:38:40  knight]
 * 
 * 	I do not like "merge".
 * 	[90/07/11  19:41:15  knight]
 * 
 * 	Added interrupt handler ih_flag defines
 * 	[90/08/13  23:38:40  knight]
 * 
 * 	I do not like "merge".
 * 	[90/07/11  19:41:15  knight]
 * 
 * 	Added _t to types.
 * 	[90/07/11  19:29:42  knight]
 * 
 * 	Renamed defines for consistent name space usage.
 * 	Fixed ifdef.
 * 	[90/07/11  14:31:10  knight]
 * 
 * 	Changes enum cfg_op to #defines.
 * 	[90/07/11  14:20:56  knight]
 * 
 * 	Renamed configure.h to sysconfig.h.
 * 	Removed loader references.
 * 	Added cfg_entrypt_t.
 * 	[90/07/11  08:42:53  knight]
 * 
 * 	Renamed defines for consistent name space usage.
 * 	Fixed ifdef.
 * 	[90/07/11  14:31:10  knight]
 * 
 * 	Changes enum cfg_op to #defines.
 * 	[90/07/11  14:20:56  knight]
 * 
 * 	Renamed configure.h to sysconfig.h.
 * 	Removed loader references.
 * 	Added cfg_entrypt_t.
 * 	[90/07/11  08:42:53  knight]
 * 
 * $EndLog$
 */

#ifndef _SYS_SYSCONFIG_H_
#define _SYS_SYSCONFIG_H_

#include <sys/types.h>

typedef int (*sysconfig_entrypt_t)();		/* kernel module entry point */
typedef int   sysconfig_op_t;			/* configuration operation */

#define	SYSCONFIG_PARAM_MAX	NBPG		/* max size of param buffer */

/*
 *	kmodcall() sysconfig_op_t types
 */
#define SYSCONFIG_NOSPEC	0x00		
#define SYSCONFIG_CONFIGURE	0x01	
#define SYSCONFIG_UNCONFIGURE	0x02
#define SYSCONFIG_QUERY		0x04

/*
 * 	Device interrupt handler ih_flag defines for adding a interrupt handler.
 * 	Also used by configuration manager methods.
 */
#define IH_VEC_DYNAMIC_OK       0x0001  /* Allow relocation of target vector */
#define IH_VEC_MULTIPLE_OK      0x0002  /* Allow multiple handlers per vector */
#define IH_VEC_PASS_ISP         0x0004  /* Pass ptr to interrupt frame to ISR */

#define IH_DRV_NONPARALLEL      0x8000  /* Driver is non-parallelized */
#define IH_DRV_NEWMAJOR         0x4000  /* Force install at new major number */




/*
 * 	Device interrupt handler defines.
 *	Used by device subsystems when adding an interrupt handler.
 *	Also passed by cfgmgr methods to device subsystem configuration 
 *		entry points (via (via device_admin_t)).
 */
#define IH_VEC_DYNAMIC_OK       0x0001  /* Allow relocation of target vector */
#define IH_VEC_MULTIPLE_OK      0x0002  /* Allow multiple handlers per vector */
#define IH_VEC_PASS_ISP         0x0004  /* Pass ptr to interrupt frame to ISR */

/*
 * 	Device switch defines.
 *	Used by device subsystems when adding an device switch.
 *	Also passed by cfgmgr methods to device subsystem configuration 
 *		entry points (via (via device_admin_t)).
 */
#define IH_DRV_NONPARALLEL      0x8000  /* Driver is non-parallelized */
#define IH_DRV_NEWMAJOR         0x4000  /* Force install at new major number */
#define IH_DRV_USEMAJOR         0x2000  /* Force install at new major number */


/*
 * Configuration I/O structure definitions
 */

/*
 * File System configuration entry point in/out data structures
 */
#define	OSF_FILESYS_CONFIG_10	0x04026020 /* OSF/1 filesys_config_t version */

typedef struct filesys_config {
	uint	fc_version;
	uint	fc_type;
	ulong	fc_flags;
} filesys_config_t;


/*
 * Device System configuration entry point in/out data structures
 */
#define	OSF_DEVICE_CONFIG_10	0x04026021 /* OSF/1 device_config_t version */

typedef struct {
	uint	dc_version;
	uint	dc_errcode;		/* Additional error information */
	long	dc_bmajnum;		/* Preferred block major number */
	long	dc_cmajnum;		/* Preferred char major number */
	long	dc_begunit;		/* 1st minor device number in range */
	long	dc_numunit;		/* number of minor device numbers */
	long	dc_dsflags;		/* Device switch config flags */
	long	dc_ihflags;		/* Interrupt switch config flags */
	long	dc_ihlevel;		/* Preferred interrupt level */
} device_config_t;

#if     MULTIMAX
struct mmax_devconf {
	long	mdc_level;		/* Preferred interrupt level */
	long	mdc_bmajnum;		/* Preferred block major number */
	long	mdc_cmajnum;		/* Preferred char major number */
	long	mdc_minnum;		/* 1st minor device number in range */
	long	mdc_flags;		/* Configuration flags */
	long	mdc_errcode;		/* Additional error information */
};
#endif

#endif _SYS_SYSCONFIG_H_
