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
/*
 * OSF/1 Release 1.0
 *
 */

/*
 * rose_namegrp.h
 * Contains definitions for the OSF/ROSE name groups for which type or 
 * flag names are available.
 */

/* flags */

#define REGC_INITPROT_SHORT_NAMES	1
#define REGC_FLAGS_STRIPPED_NAMES	2
#define PKGC_FLAGS_STRIPPED_NAMES	3
#define SI_FLAGS_SHORT_NAMES		4
#define ENTC_FLAGS_STRIPPED_NAMES	5
#define MOH_FLAGS_STRIPPED_NAMES	6
#define RI_FLAGS_SHORT_NAMES		7

/* types */

#define LDC_CMD_TYPE_STRIPPED_NAMES	101
#define REGC_USAGE_TYPE_STRIPPED_NAMES	102
#define RI_SIZE_TYPE_STRIPPED_NAMES	103
#define SYMC_KIND_STRIPPED_NAMES	104
#define MOH_BYTE_ORDER_STRIPPED_NAMES	105
#define MOH_DATA_REP_ID_NAMES		106
#define MOH_CPU_TYPE_STRIPPED_NAMES	107
#define MOH_CPU_SUBTYPE_STRIPPED_NAMES	108
#define MOH_VENDOR_TYPE_STRIPPED_NAMES	109
#define FNTC_TYPE_STRIPPED_NAMES	110
#define MO_ERROR_MESSAGES		111

extern void rose_flags_print __ ((long, int));

extern void rose_flags_get __ ((long, int, char *));
                                          /* last arg is 80 char buf */
extern void rose_type_print __ ((int, int));
extern void rose_type_get __ ((int, int, char *));
                                          /* last arg is 50 char buf */

