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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* ldr_region.h
 * Definitions for loader region records
 *
 * Region records hold the information about each loaded region of an
 * object module.  Each region occupies a virtually-contiguous range
 * of bytes of the process' address space.  Regions of a module are
 * numbered starting at zero.  The region records are filled in by
 * the format-dependent manager's "map_region" routine, and used by
 * the format-dependent manager during relocation.
 *
 * This file depends on: <standards.h> <loader.h> ldr_types.h
 *
 * OSF/1 Release 1.0
 */

#ifndef _H_LDR_REGION
#define _H_LDR_REGION 


#define	LDR_REGION_VERSION	1	/* current structure version */

/* Flags for region record */

typedef	unsigned	ldr_region_flags_t;

#define	LRF_NONE	0
#define	LRF_LOADED	0x1		/* region has been mapped */


/* The region record itself */

typedef struct ldr_region_rec {
	int			lr_version; /* version number of structure */
	char			*lr_name; /* region name (NULL if none) */
	ldr_prot_t		lr_prot; /* region protection (rwx) */
	univ_t			lr_vaddr; /* starting va of region */
	univ_t			lr_mapaddr; /* va region is now mapped at */
	size_t			lr_size; /* size of region in bytes */
	ldr_region_flags_t	lr_flags; /* flags for this region */
	char			lr_reserved[36]; /* reserved */
} ldr_region_rec;


/* The following macros manipulate the flags in the region record */

#define lr_flag_loaded(reg)	((reg)->lr_flags |= LRF_LOADED)


/* Create a region list large enough to hold nregions worth of region
 * records, initialize it, and return it in *regions.  Version is the
 * version number of the structure.  Returns LDR_SUCCESS
 * on success, or negative status code on error.
 */

extern int
ldr_regions_create __((int nregions, int version, ldr_region_rec **retval));

/* Free a region list containing count records.  Returns LDR_SUCCESS
 * on success, or negative status code on error.
 */

extern int ldr_regions_free __((int nregions, ldr_region_rec *val));

#endif /* _H_LDR_REGION */
