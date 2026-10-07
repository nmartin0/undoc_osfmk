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
 * HISTORY
 * $Log: vs.c,v $
 * Revision 1.10  1994/11/18  20:53:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/09/28  20:39:35  slk
 * Added two header files.
 *
 *  Reviewer: Brent Olsen and John Litvin
 *  Risk: Low
 *  Benefit or PTS #: Fixes 11107, vs now builds.
 *  Testing: built and ran vs
 *  Module(s): user/etc/vs.c
 *
 * Revision 1.8  1994/03/18  00:16:24  slk
 * Dropped code fix from Locus
 *  * Revision 3.8  93/07/12  16:55:39  yazz
 *  * Replaced obsolete "syscall(SYS_table, ...)" calls with "table(...)"
 *  * and removed hard-coded definition for SYS_table within this .c file.
 *  Reviewer: Bob Yasi
 *  Risk: Low
 *  Benefit or PTS #: 8578
 *  Testing: ran vs without errors.
 *  Module(s):
 *
 * Revision 1.7  1993/07/14  18:50:17  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.6  1993/05/12  04:03:23  stefan
 * sys/table.h now needs sys/limits.h to compile.
 *
 * Revision 1.1.1.5  1993/07/01  21:15:48  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:27:54  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/01/22  19:26:08  stefan
 * Merged Locus 01-20-93 code drop.
 *
 * Revision 1.3  1992/11/30  23:00:51  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/09  18:03:00  cfj
 * Conflict resolution of 11/05/92 bug fix drop from Locus.
 *
 * Revision 1.1.2.1  1992/11/05  23:49:06  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 1.1.1.3  1993/05/03  17:57:24  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.7  1992/12/30  08:16:47  chrisp
 * Add forward references and conditional compilation to build for non-TNC.
 *
 * Revision 3.6  92/12/29  13:48:25  chrisp
 * Add a new option (-z) to report on the size of the vproc table. Since this
 * 	table is dynamically allocated, this size is of interest.
 * 
 * Revision 3.5  92/11/05  15:38:08  klh
 * MAX_MMAP_REGIONS is required by tnc_types.h, but defined in user.h only
 * if SHOW_UTT is defined (klh for chrisp)
 * 
 * Revision 3.4  92/09/29  09:21:40  roman
 * Change to use new vproc/pvproc field names.
 * Changes to display the foster ppid and the foster children list.
 * 
 * Revision 3.3  92/09/21  14:18:10  chrisp
 * Eliminate dpvp_defs.h.
 * 
 * Revision 3.2  92/08/25  14:57:22  chrisp
 * Don't print process group or seesion lists for processes that aren't
 * respective leaders.
 * 
 * Revision 3.1  92/04/21  12:24:52  chrisp
 * Add an option "-m" to report the reference counts of receive and send
 * 	rights for vproc ports held by the local server.
 * 
 * Revision 3.0  92/03/12  12:00:26  klh
 * Initial (local) checkin
 * 
 */

#define	SHOW_UTT
#include <sys/types.h>
#include <sys/limits.h>
#include <sys/table.h>
#include <sys/tty.h>
#include <uxkern/import_mach.h>
#ifdef TNC
#include <i860paragon/mcmsg/mcmsg_info.h>
#include <uxkern/bsd_types.h>
#include <tnc/dpvproc.h>
#else
#include <vproc/bpvproc.h>
#endif
#include <stdio.h>

extern void exit(int);

/* Forward references */
int  pid_to_vproc(int pid);
void vp_inferiors_print(int i);
void vp_group_print(int i);
void vp_session_print(int i);
void vp_heading();
void vp_relations_print(int i);
void vp_print(int i);

#define VPMAX 10000

extern int errno;
struct tbl_vprocinfo vp_array[VPMAX];
int	nvproc;
int	error,
	full,
	vproc_id,
	inferiors,
	all = 1,
	mach_ports,
	pgroup,
	session,
	table_size;

main(int argc, char *argv[])
{
	extern char *optarg;
	extern int optind;
	char ch;
	int i;

	while ((ch = getopt(argc, argv, "filmpsvz")) != EOF)
		switch (ch) {
		case 'f':
			full++;
			break;
		case 'i':
			inferiors++;
			break;
		case 'l':
			all++;
			break;
		case 'm':
			mach_ports++;
			break;
		case 'p':
			pgroup++;
			break;
		case 's':
			session++;
			break;
		case 'v':
			vproc_id++;
			break;
		case 'z':
			table_size++;
			break;
		case '?':
		default:
			error++;
		}
	if (optind < argc)
		all = 0;
	if (inferiors && all)
		error++;
	if (error) {
		printf("usage: vs [-f] [-i] [-s] [-p] [-v] [-z] ids\n");
		exit(1);
	}

	
	nvproc = table(TBL_VPROCINFO, 0, (char *)&vp_array, SHRT_MAX, 0);
	if (nvproc == -1) {
		printf("table() returned %d, errno = %d\n", nvproc, errno);
		exit(1);
	}
	if (nvproc > VPMAX) {
		printf("number of vprocs exceeds internal table space\n");
		(void) table(TBL_VPROCINFO, 0, (char *)&vp_array,
		 		VPMAX, sizeof(struct tbl_vprocinfo));
	} else {
		(void) table(TBL_VPROCINFO, 0, (char *)&vp_array,
		 		nvproc, sizeof(struct tbl_vprocinfo));
	}

	vp_heading();
	if (all) {
		for (i = 0; i < nvproc; i++)
			if (vp_array[i].vi_status != VI_EMPTY)
				vp_print(i);	
	} else
		for (; optind < argc; optind++) {
			i = vproc_id ? atoi(argv[optind]) :
				       pid_to_vproc(atoi(argv[optind]));
			if (0 <= i && i < nvproc) {
				vp_print(i);
				if (inferiors)
					vp_inferiors_print(i);
				if (pgroup)
					vp_group_print(i);
				if (session)
					vp_session_print(i);
			}
		}
}

int
pid_to_vproc(int pid)
{
	int i;
	for (i = 0; i < nvproc; i++)
		if (vp_array[i].vi_status != VI_EMPTY &&
		    vp_array[i].vi_pid == pid)
			return(i);
	return(-1);
}

void
vp_inferiors_print(int i)
{
	int j;

	j = vp_array[i].vi_head_childl;
	while (j != -1) {
		vp_print(j);
		vp_inferiors_print(j);
		j = vp_array[j].vi_childl;
	}
}

void
vp_group_print(int i)
{
	int j;

	if (vp_array[i].vi_flag & PV_PGRPLEADER) {
		j = vp_array[i].vi_head_pgrpl;
		while (j != -1) {
			if (j != i)
				vp_print(j);
			j = vp_array[j].vi_pgrpl;
		}
	}
}

void
vp_session_print(int i)
{
	int j;

	if (vp_array[i].vi_flag & PV_SESSIONLEADER) {
		j = vp_array[i].vi_sessionl;
		while (j != -1) {
			vp_print(j);
			j = vp_array[j].vi_sessionl;
		}
	}
}
	
void
vp_heading()
{
	if (table_size)
		printf("%d vprocs allocated\n", nvproc);
	printf("vproc\t[pid]\trefcnt\tpproc\tppid\tpgid\tsid\tfppid\tcommand\n");
	printf("----------------------------------------");
	printf("----------------------------------------\n");
}

void
vp_print(int i)
{
	struct tbl_vprocinfo *v = vp_array + i;

	printf("%d\t%d\t%d", i, v->vi_pid, v->vi_ref_cnt);
	if (v->vi_status == VI_REMOTE) {
		printf("\tremote\n");
	} else {
		char args[20];

		switch (v->vi_pproc_slot) {
		case -1:
			strcpy(args, "[no process]");
			break;
		case 0:
		 	strcpy(args, "[process 0]");
			break;
		default:
			if (table(TBL_ARGUMENTS, (int) v->vi_pid,
				    args, 1, sizeof(args)) == -1)
				strcpy(args, "[no priv]");
		}

		printf("\t%d\t%d\t%d\t%d\t%d\t%.16s\n",
			v->vi_pproc_slot, v->vi_ppid, v->vi_pgid, v->vi_sid, v->vi_foster_ppid, args);
	}
	if (full) {
		printf("\tflag bits:\n");
		if (v->vi_flag & PV_HAS_PORT_RIGHT)
			printf("\t\thas_port_right\n");
		if (v->vi_flag & PV_HAS_REMOTE_RIGHT)
			printf("\t\thas_remote_right\n");
		if (v->vi_flag & PV_IS_LOCAL)
			printf("\t\thas_receive_right\n");
		if (v->vi_flag & PV_SZOMB)
			printf("\t\tzombie\n");
		if (v->vi_flag & PV_SSTOP)
			printf("\t\tstopped\n");
		if (v->vi_flag & PV_SCTTY)
			printf("\t\thas_controlling_terminal\n");
		if (v->vi_flag & PV_PGRPLEADER)
			printf("\t\tpgrp_leader(jobc %d)\n", v->vi_jobc);
		if (v->vi_flag & PV_SESSIONLEADER)
			printf("\t\tsession_leader\n");
		if (v->vi_flag & PV_CTTY_NODE)
			printf("\t\tctty_node(%d,%d)\n",
				major(v->vi_cttyd), minor(v->vi_cttyd));
		if (v->vi_status != VI_REMOTE)
			vp_relations_print(i);	
	}
	if (full || mach_ports) {
		printf("\tMach port rights: receives %d, sends %d\n",
			v->vi_mach_receives, v->vi_mach_sends );
	}
}

void
vp_relations_print(int i)
{
	int child, pgrp_mem, sess_mem;

	child = vp_array[i].vi_head_childl;
	if (child != -1) {
		printf("\tchildren vi[pid]:");
		while (child != -1) {
			printf(" %d[%d]",
				child, vp_array[child].vi_pid);
			child = vp_array[child].vi_childl;
		}
		printf("\n");
	}
	child = vp_array[i].vi_head_foster_childl;
	if (child != -1) {
		printf("\tfoster_children vi[pid]:");
		while (child != -1) {
			printf(" %d[%d]",
				child, vp_array[child].vi_pid);
			child = vp_array[child].vi_foster_childl;
		}
		printf("\n");
	}
	if (vp_array[i].vi_flag & PV_PGRPLEADER) {
		pgrp_mem = vp_array[i].vi_head_pgrpl;
		if (pgrp_mem != -1) {
			printf("\tprocess group vi[pid]:");
			while (pgrp_mem != -1) {
				printf(" %d[%d]",
					pgrp_mem, vp_array[pgrp_mem].vi_pid);
				pgrp_mem = vp_array[pgrp_mem].vi_pgrpl;
			}
			printf("\n");
		}
	}
	if (vp_array[i].vi_flag & PV_SESSIONLEADER) {
		sess_mem = vp_array[i].vi_sessionl;
		if (sess_mem != -1) {
			printf("\tsession vi[pid]:");
			while (sess_mem != -1) {
				printf(" %d[%d]",
					sess_mem, vp_array[sess_mem].vi_pid);
				sess_mem = vp_array[sess_mem].vi_sessionl;
			}
			printf("\n");
		}
	}
}
