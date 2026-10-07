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
/* mach_o_header.h
 * 
 * This is the machine-independent declaration of the header for the
 * OSF version of the Mach-O object file format. The contents of the 
 * real file header are copied into this structure so that they can be 
 * interpreted in a straightforward way.
 *
 * NOTE:  THIS IS PRELIMINARY AND WILL PROBABLY CHANGE
 *
 * HISTORY
 * $Log: mach_o_header.h,v $
 * Revision 1.3  1994/11/18  21:10:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:15:33  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:58:26  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:16:57  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:19:33  nandy
 * Initial revision
 *
 * Revision 1.8  92/04/13  14:37:03  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.8  90/12/06  14:17:51  devrcs
 * 	Changes for osc1.0 release.
 * 	[90/12/01  18:39:52  gm]
 * 
 * Revision 1.7  90/10/07  20:30:53  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:50:30  gm]
 * 
 * Revision 1.6  90/07/27  10:41:13  devrcs
 * 	Added a header version field.
 * 	Added information about the size of the corresponding canonical header.
 * 	[90/07/17  15:43:32  melanie]
 * 
 * Revision 1.5  90/06/22  21:52:05  devrcs
 * 	Moved defines for moh_byte_order, moh_data_rep_id, moh_cpu_type,
 * 	moh_cpu_subtype, and moh_vendor_type values to mach_o_types.h.
 * 	[90/06/08  18:48:45  melanie]
 * 
 * Revision 1.4  90/05/24  23:08:02  devrcs
 * 	Renamed moh_load_map_off to moh_load_map_cmd_off (meaning is the same).
 * 	Bumped minor version to 3.
 * 
 * 	Changed minor version to 3.
 * 	[90/05/14  08:13:41  melanie]
 * 
 * Revision 1.3  90/04/27  23:06:41  devrcs
 * 	Incremented minor version.  Moved version fields.
 * 	Removed moh_file_type field.
 * 	Added flags for execable, executable, and unresolved.
 * 	Changed cpu_type_t, cpu_subtype_t, vendor_type_t to mo_...,
 * 	and prefixed the flags for these types with MO_; this was
 * 	to avoid name conflicts with mach_machine.h.
 * 	[90/04/19  13:45:04  melanie]
 * 
 * 	Changed FILE_TYPE_LM to FILE_TYPE_EXECUTABLE.
 * 	Changed major version to 0 and minor version to 1.
 * 	[90/04/04  13:22:07  melanie]
 * 
 * Revision 1.2  90/02/23  00:36:08  devrcs
 * 	initial coding
 * 	[90/02/20  16:45:08  melanie]
 * 
 * $EndLog$
*/

#ifndef _H_MACH_O_HEADER
#define _H_MACH_O_HEADER

#include <mach_o_types.h>

/*
 * Use decode_mach_o_hdr to convert the raw header in canonical
 * format to the structure defined in this header file.
 * Use encode_mach_o_hdr to convert this form into the canonical form. 
 */

typedef struct mo_header_t {
          mo_long_t		moh_magic;
	  mo_short_t		moh_major_version;
	  mo_short_t		moh_minor_version;
	  mo_short_t		moh_header_version;
	  mo_short_t		moh_max_page_size;
          mo_short_t		moh_byte_order;
          mo_short_t		moh_data_rep_id;
	  mo_cpu_type_t		moh_cpu_type;
	  mo_cpu_subtype_t	moh_cpu_subtype;
	  mo_vendor_type_t	moh_vendor_type;
	  mo_long_t		moh_flags;
	  mo_offset_t		moh_load_map_cmd_off;
	  mo_offset_t		moh_first_cmd_off;
	  mo_long_t		moh_sizeofcmds;
	  mo_long_t		moh_n_load_cmds;
	  mo_long_t		moh_reserved [2];
	} mo_header_t;

#define MOH_MAGIC             0xbeefface      /* ? */

/* definitions for the version fields */

#define MOH_MAJOR_VERSION	0
#define MOH_MINOR_VERSION	4
#define MOH_HEADER_VERSION	1

/* values for the sizes of versions of the corresponding raw headers */

#define	MO_SIZEOF_RAW_HDR	56

/* definitions of header flags */

#define MOH_RELOCATABLE_F	0x1	/* has loader relocation */
#define MOH_LINKABLE_F		0x2	/* has linker relocation */
#define MOH_EXECABLE_F		0x4	/* can be exec'd; has crt0 */
#define MOH_EXECUTABLE_F	0x8	/* can be loaded for execution */
#define MOH_UNRESOLVED_F	0x10	/* has unresolved import refs */

#endif /* _H_MACH_O_HEADER */
