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
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * mach_o_vals.h
 * Machine-independent definitions that are either used in machine-dependent
 * files or that should be in place that is "central" to all Mach-O
 * (OSF/ROSE) header files.
 *
 * HISTORY
 * $Log: mach_o_vals.h,v $
 * Revision 1.3  1994/11/18  21:10:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:15:42  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:58:29  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:00  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:19:36  nandy
 * Initial revision
 *
 * Revision 1.5  92/04/13  14:37:07  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.5  90/12/06  14:17:53  devrcs
 * 	Changes for osc1.0 release.
 * 	[90/12/01  18:39:58  gm]
 * 
 * Revision 1.4  90/10/07  20:30:57  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:50:37  gm]
 * 
 * Revision 1.3  90/08/24  13:53:02  devrcs
 * 	Added CPU_SUBTYPE for 386
 * 	[90/08/15  14:34:51  kevins]
 * 
 * Revision 1.2  90/07/27  10:41:17  devrcs
 * 	Added definitions of possible values for moh_byte_order, moh_data_rep_id,
 * 	moh_cpu_type, moh_cpu_subtype, moh_vendor_type, moved from
 * 	mach_o_types.h.
 * 	Added definitions for moh_magic.
 * 
 * 	Added definitions of some error values.
 * 	[90/07/17  15:43:37  melanie]
 * 
 * $EndLog$
 */

#ifndef _H_MACH_O_VALS
#define _H_MACH_O_VALS

/* definitions for the mo_header_t.moh_magic field */

#define MOH_MAGIC_LSB		0xcefaefbe
#define MOH_MAGIC_MSB		0xbeefface

/* definitions for the mo_header_t.moh_byte_order field */

#define BO_LSB			1
#define BO_MSB			2

/* definitions for the mo_header_t.moh_data_rep_id field */

#define DREP_GCC_LSB		0x1	/* GCC, little endian */

/*
 * definitions for the mo_header_t.moh_cpu_type field; 
 * (the values may be changed/refined but will probably not duplicate
 * those in mach/machine.h -- these are for the loader to know
 * what kind of machines the code will run on)
 */

#define MO_CPU_TYPE_MIPS	((mo_cpu_type_t) 1)
#define MO_CPU_TYPE_NS32000	((mo_cpu_type_t) 2)
#define MO_CPU_TYPE_I386	((mo_cpu_type_t) 3)
#define MO_CPU_TYPE_M68000	((mo_cpu_type_t) 4)

/* definitions for the mo_header_t.moh_cpu_subtype field;
 * these will be changed/refined as well -- eventually there should be
 * a separate list for each class of machines
 */

#define MO_CPU_SUBTYPE_PMAX	((mo_cpu_subtype_t) 1)
#define MO_CPU_SUBTYPE_MMAX	((mo_cpu_subtype_t) 2)
#define MO_CPU_SUBTYPE_AT386    ((mo_cpu_subtype_t) 3)

/* definitions for the mo_header_t.moh_vendor_type field */

#define MO_VENDOR_TYPE_OSF	1
#define OUR_VENDOR_TYPE		MO_VENDOR_TYPE_OSF

/* definitions for the Mach-O (OSF/ROSE)-specific error values */

#define MO_ERROR_BAD_MAGIC		-100
#define MO_ERROR_BAD_HDR_VERS		-101
#define MO_ERROR_BAD_RAW_HDR_VERS 	-102
#define MO_ERROR_BUF2SML		-103
#define MO_ERROR_OLD_RAW_HDR_FILE	-104
#define MO_ERROR_UNSUPPORTED_VERS	-105

#define MO_HDR_CONV_SUCCESS		0

#endif /* _H_MACH_O_VALS */
