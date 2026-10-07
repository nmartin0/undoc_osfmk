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
/* switch.c
 * 
 * the loader switch
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>

#ifdef	LDR_PRELOAD_MGR
extern int ldr_preload_entry();
#endif /* LDR_PRELOAD_MGR */

#ifdef	LDR_MACHO_MGR
extern int ldr_macho_entry();
#endif /* LDR_MACHO_MGR */

#ifdef	LDR_COFF_MGR
extern int ldr_coff_entry();
#endif /* LDR_COFF_MGR */

#ifdef	LDR_AOUT_MGR
extern int ldr_aout_entry();
#endif /* LDR_AOUT_MGR */

#ifdef	LDR_DUMMY_MGR
extern int ldr_dummy_entry();
#endif /* LDR_DUMMY_MGR */

/* NOTE that entries in this table should be in the order
 * that the recognizers should be called.  In particular,
 * the preload manager should always be the first manager
 * in the table.
 */

ldr_entry_pt_t ldr_manager_entries[] = {

#ifdef	LDR_PRELOAD_MGR
	ldr_preload_entry,
#endif /* LDR_PRELOAD_MGR */

#ifdef	LDR_MACHO_MGR
	ldr_macho_entry,
#endif /* LDR_MACHO_MGR */

#ifdef	LDR_COFF_MGR
	ldr_coff_entry,
#endif /* LDR_COFF_MGR */

#ifdef	LDR_AOUT_MGR
	ldr_aout_entry,
#endif /* LDR_AOUT_MGR */

#ifdef	LDR_DUMMY_MGR
	ldr_dummy_entry,
#endif /* LDR_DUMMY_MGR */

};

int n_ldr_mgr_entries = sizeof(ldr_manager_entries) / sizeof(ldr_entry_pt_t);
