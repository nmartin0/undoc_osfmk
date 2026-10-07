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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: tnc_types_gen.c,v $
 * Revision 1.9  1995/02/01  21:59:01  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.8  1994/11/18  20:44:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/02  22:30:30  chrisp
 * In dpvpop_reap(), perform PVPOP_RMV_PGRP_LIST() for zombie child
 * only if child's pgrp leader is not its parent; return child pgid.
 * In dvpop_wait(), analyze pgid returned for zombie child and call
 * PVPOP_RMV_PGRP_LIST() if this is the parent pid.
 *
 * Support added for waitmulti() - viz: elder reporting and reap multi
 * operation (refer to rtask_cli_vproc.c).
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): dpvproc.h dvp_pvpops.c dvp_vpops.c pvp.ops pvps.ops rtask.h
 * 	    rtask_cli_vproc.c rtask_server.c rtask_svr_vproc.c
 * 	    spanning_tree.c tnc_async.defs tnc_server_side.c
 * 	    tnc_types.defs tnc_types.h tnc_types_gen.c
 *
 * Revision 1.6  1993/07/14  18:35:17  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.5  1993/07/01  20:49:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:26:48  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/05/03  17:47:45  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1992/11/30  22:49:08  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.3  1992/11/09  18:01:12  cfj
 * Conflict resolution of 11/05/92 bug fix drop from Locus.
 *
 * Revision 1.1.2.2  1992/11/06  20:32:29  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:47:16  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.11  93/06/19  15:41:37  yazz
 * [ ad1.04 merge ]
 * 	Replace MAX_MMAP_REGIONS with TNC_MAX_MMAP_REGIONS.
 * 
 * Revision 3.10  93/04/23  07:53:46  roman
 * Remove extraneous copyright notice.
 *
 * Revision 3.9  92/11/04  11:23:26  chrisp
 * Remove extraneous #ifndef generated in tnc_types_gen.h since tnc_types.h
 * 	no longer includes it.
 * 
 * Revision 3.8  92/10/28  15:56:42  roman
 * Put in generated ifndef's to get compilations with no redefinitions
 * 	of MAX_BLOB_SIZE.
 * 
 * Revision 3.7  92/10/01  10:35:31  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 3.6  92/09/29  08:23:00  roman
 * Add size of procset structure.
 * 
 * Revision 3.5  92/06/11  16:11:41  mjl
 * Added declarations to support pipe/FIFO/socket relocation.
 * 
 * Revision 3.4  92/06/05  15:41:24  roman
 * Fix minor spelling error.
 * Generate size of mmap structure array correctly.
 * 
 * Revision 3.3  92/03/27  11:35:12  roman
 * Add new type for size of array of credentials cache nodes.
 * 
 * Revision 3.2  92/02/14  09:03:38  roman
 * Add new symbolic constant to be used for the size of vproc lists (used
 * for parent-child-sibling list, pgrp list, and session list).
 * 
 * Revision 3.1  92/01/07  18:28:32  roman
 * Add declarations needed to support mmap structures in tnc.defs.
 * 
 * Revision 3.0  91/12/13  09:17:19  roman
 * Initial submission. File used to generate .h file used by MiG, because
 * MiG isn't very good at evaluating stuff.
 * 
 */

/*
 * Generate definitions for MiG interfaces.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/procset.h>
#include <sys/user.h>
#include <tnc/rtask.h>
#include <tnc/reloc.h>

main()
{
	printf("#define\tPROCSET_SIZE %d\n",
			sizeof(struct procset) / sizeof(int));
	printf("#define\tRFORK_DATA_SIZE %d\n",
			sizeof(struct rf_data) / sizeof(int));
	printf("#define\tMIGRATE_DATA_SIZE %d\n",
			sizeof(struct mi_data) / sizeof(int));
	printf("#define\tREXEC_DATA_SIZE %d\n",
			sizeof(struct re_data) / sizeof(int));
	printf("#define\tMAXCOMLEN_SIZE %d\n",
			MAXCOMLEN + 1);
	printf("#define\tMAXLOGNAME_SIZE %d\n",
			MAXLOGNAME);	/* MAXCOMLEN is +1 & MAXLOGNAME isn't */
	printf("#define\tTNC_MMAP_ARRAY_SIZE %d\n",
			TNC_MAX_MMAP_REGIONS);
	printf("#define\tMMAP_STRUCT_SIZE %d\n",
			sizeof(struct mmap_struct) / sizeof(int));
	printf("#define\tMAX_VPROC_LIST_SIZE %d\n",
			MAX_VPROC_LIST);
	printf("#define\tCREDENTIALS_CACHE_ARRAY_SIZE %d\n",
			CREDENTIALS_CACHE_SIZE);

	printf("#define\tMAGIC_MACH_MSG_SIZE %d\n",
			MAGIC_MACH_MSG_SIZE);
	printf("#define\tRELOC_PORT_ARRAY_SIZE %d\n",
			RELOC_PORT_ARRAY_SIZE);
	printf("#define\tMAX_BLOB_SIZE %d\n",
			MAX_BLOB_SIZE);

	printf("#define\tRSTAT_SIZE %d\n",
			sizeof(rstat_t) / sizeof(int));

	return (0);
};
