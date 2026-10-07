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
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <loader.h>

extern char *program;

extern void cmd_info(pid_t);
extern void dump_minfo(ldr_module_info_t *);
extern void dump_rinfo(ldr_module_t, ldr_region_info_t *);

void
cmd_info(pid)
	pid_t         pid;
{
	ldr_module_t	  module;
	ldr_module_info_t minfo;
	ldr_region_t	  region;
	ldr_region_info_t rinfo;
	ldr_process_t     process;
	int		  rc;
	size_t		  size;

	process = (ldr_process_t)pid;

	if ((rc = ldr_xattach(process)) < 0) {
		if (-rc == EFAULT)
			fprintf(stderr, "%s: loader not present in process\n",
				program);
		else
			fprintf(stderr, "%s: ldr_xattach() failed: %s\n",
				program, strerror(-rc));
		exit(1);
	}

	module = LDR_NULL_MODULE;

	for (;;) {

		ldr_next_module(process, &module);
		if (module == LDR_NULL_MODULE)
			break;

		if ((rc = ldr_inq_module(process, module, &minfo,
					 sizeof(minfo), &size)) < 0) {
			fprintf(stderr, "%s: ldr_inq_module() failed: %s\n",
				program, strerror(-rc));
			exit(1);
		}

		if (size < sizeof(minfo))
			fprintf(stderr, "%s: warning: short module info record %d\n",
				program, size);

		dump_minfo(&minfo);

		for (region = 0; region < minfo.lmi_nregion; region++) {

			if ((rc = ldr_inq_region(process, module, region,
						 &rinfo, sizeof(rinfo),
						 &size)) < 0) {
				fprintf(stderr, "%s: ldr_inq_module() failed: %s\n",
					program, strerror(-rc));
				exit(1);
			}

			if (size < sizeof(rinfo))
				fprintf(stderr, "%s: warning: short region info record %d\n",
					program, size);

			dump_rinfo(module, &rinfo);
		}
	}

	(void)ldr_xdetach(process);
}

void
dump_minfo(minfo)
	ldr_module_info_t *minfo;
{
	printf("%d\t\"%s\"\t(Nregion %d Flags 0x%08x):\n", minfo->lmi_modid,
	       minfo->lmi_name, minfo->lmi_nregion, minfo->lmi_flags);
}

void
dump_rinfo(mod, rinfo)
	ldr_module_t       mod;
	ldr_region_info_t *rinfo;
{
	printf("\t%d\t%s\n", rinfo->lri_region_no, rinfo->lri_name);
	printf("\t\tVA 0x%08x\tMA 0x%08x\tSZ %8d\tProt 0x%03x\n",
	       rinfo->lri_vaddr, rinfo->lri_mapaddr,
	       rinfo->lri_size, rinfo->lri_prot);
}
