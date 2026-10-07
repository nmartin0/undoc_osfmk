/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright (c) 1990
 * Open Software Foundation, Inc.
 * 
 * Permission is hereby granted to use, copy, modify and freely distribute
 * the software in this file and its documentation for any purpose without
 * fee, provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation.  Further, provided that the name of Open
 * Software Foundation, Inc. ("OSF") not be used in advertising or
 * publicity pertaining to distribution of the software without prior
 * written permission from OSF.  OSF makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
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
 * OSF/1 Release 1.0
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
