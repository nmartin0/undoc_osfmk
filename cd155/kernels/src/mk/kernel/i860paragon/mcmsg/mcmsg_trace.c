/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_trace.c,v 1.58 1995/02/14 22:02:27 joel Exp $
 */

/*
 * mcmsg_trace.c
 *
 * Tracing support for debugging
 */

#define	MCMSG_TRACE_MODULE	1

#include <ddb/db_command.h>
#include <ddb/db_sym.h>
#include <ddb/db_task_thread.h>
#include <machine/db_machdep.h>

#include <i860paragon/mcmsg/mcmsg_ext.h>
#if	NX
#include <i860paragon/mcmsg/mcmsg_nx.h>
#endif	NX
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_mp.h>
#include <i860paragon/dp.h>
#include <i860paragon/msgp/msgp_hw.h>

extern mcmsg_task_t *mcmsg_user_mcmsg_task[];
extern post_page_t  *mcmsg_kernel_post_page;
extern int           mcmsg_kernel_post_page_in;
extern int           mcmsg_kernel_post_page_out;
extern int           mcmsg_user_post_tasks;
extern post_page_t  *mcmsg_user_post_page[POST_TASK_MAX];
extern int           mcmsg_user_post_page_out[POST_TASK_MAX];

#if	MSG_TRACE

#define	NTRACEID	(sizeof(trace_id_name)/sizeof(trace_id_name[0]))
char	*trace_id_name[] = {
	"**null**",
	"send    ",
	"receive ",
	"provide ",
	"lose    ",
	"debug   ",
	"time    ",
	"call    ",
	"callx   ",
};

extern char	*post_name_list[];
extern int	post_args_list[];
extern int	post_module_list[];
extern char	*control_name[];

#endif MSG_TRACE

extern char	*method_name_list[];

mcmsg_trace_init()
{

#if	MSG_TRACE
	mcmsg_trace_module_mask = getbootint("BOOT_MCMSG_TRACE", -1);
#endif	MSG_TRACE
}

#ifdef	DB_MACHINE_COMMANDS


/*
 * This mt overrides for debugging.
 */
mcmsg_task_t *mcmsg_debug_mcmsg_task = 0;

/*
 * select the mt for debug 
 */
mcmsg_task_t *
mcmsg_debug_mt()
{
	mcmsg_task_t *mt;

	/* Use overriding task? */
	if (mcmsg_debug_mcmsg_task != 0) {
		mt = mcmsg_debug_mcmsg_task;
		db_printf("mcmsg_task       0x%08x\n", mt);
		return mt;
	}
		
	/* any parallel tasks? */
	if (mcmsg_user_post_tasks == 0) {
		db_printf("No parallel tasks\n");
		return ((mcmsg_task_t *)0);
	}

	/* pick the last task in list as default */
	mt = mcmsg_user_mcmsg_task[mcmsg_user_post_tasks-1];
	db_printf("mcmsg_task[%d]   0x%08x\n", mcmsg_user_post_tasks-1, mt);
	return mt;
}


static char *method_name(m)
	int	m;
{

	if (m < METHOD_MAX) {
		return method_name_list[m];
	} else if (m == 0xDEAD) {
		return "**free**";
	} else {
		return "***ng***";
	}
}

/*
 *	Pretty print pid_si structure.
 */
static print_pid_si(si)
select_item_t *si;
{
	if (si == 0) {
		return;
	}
	db_printf(" method         %s\n", method_name(si->method));
	db_printf(" nextmethod     %s\n", method_name(si->nextmethod));
	db_printf(" link           0x%08x\n", si->link);
	db_printf(" value          %10u\n", si->value);
	db_printf(" item           0x%08x\n", si->item);
	db_printf(" mcmsg_task     0x%08x\n", si->mcmsg_task);
	db_printf(" node           %10u\n", si->ppid.node);
	db_printf(" process_lock   %10u\n", si->ppid.process_lock);
	db_printf(" send_ready     %10u\n", si->ppid.send_ready);
	db_printf(" route          %10u\n", si->ppid.route);
	db_printf(" send_avail     %10u\n", si->ppid.send_avail);
	db_printf(" recv_give      %10u\n", si->ppid.recv_give);
	db_printf(" recv_total     %10u\n", si->ppid.recv_total);
	db_printf(" recv_target    %10u\n", si->ppid.recv_target);
	db_printf(" recv_taken     %10u\n", si->ppid.recv_taken);
	db_printf(" send_wait      0x%08x\n", si->ppid.send_wait);
	db_printf(" when_recvd     0x%08x\n", si->ppid.when_recvd);
	db_printf(" rk_recv_want   %10u\n", si->ppid.rk_recv_want);
	db_printf(" rk_recv_seq    %10u\n", si->ppid.rk_recv_seq);
	db_printf(" rk_recv_pid    %10u\n", si->ppid.rk_recv_pid);
	db_printf(" rk_recv_type   %10u\n", si->ppid.rk_recv_type);
	db_printf(" rk_recv_ptype  %10u\n", si->ppid.rk_recv_ptype);
	db_printf(" rk_recv_link   0x%08x\n", si->ppid.rk_recv_link);
	db_printf(" avail_link     0x%08x\n", si->ppid.avail_link);
	db_printf(" attach_link    0x%08x\n", si->ppid.attach_link);
	db_printf(" att_pend_link    0x%08x\n", si->ppid.att_pend_link);
	db_printf(" sub_pid_si     0x%08x\n", si->ppid.sub_pid_si);
}

#if	MSG_TRACE

mcmsg_db_match(index, modif)
	int		index;
	char		*modif;
{
	char		*p;

	if (index < 0 || index >= NTRACE) {
		return 0;
	}
	if (modif == 0 || modif[0] == '\0') {
		if ((mcmsg_trace_buf[index].t_control) >= MCTRL_END) {
			return 1;
		}
		return 1;
	}

	/*
	 * modif=m matches module mask.
	 */
	if (*modif == 'm') {
		char *p = modif + 1;
		int	mask;

		mask = 0;
		while (*p >= '0' && *p <= '9') {
			mask = mask*10 + *p++ - '0';
		}
		return (mask & (1 << mcmsg_trace_buf[index].t_module)) != 0;
	}
	/*
	 * modif=g matches traces with give
	 */
	if (*modif == 'g') {
		if ((mcmsg_trace_buf[index].t_give) != 0 &&
		    mcmsg_trace_buf[index].t_id == TRACE_RECV) {
			return 1;
		}
#if	NX
		if ((mcmsg_trace_buf[index].t_control) == MCTRL_NX1 &&
		    mcmsg_trace_buf[index].t_id == TRACE_SEND) {
			return 1;
		}
#endif	NX
		return 0;
	}

	/*
	 * modif=x matches all send and receive traces
	 */
	if (*modif == 'x') {
		if (mcmsg_trace_buf[index].t_id == TRACE_RECV ||
			mcmsg_trace_buf[index].t_id == TRACE_SEND) {
			return 1;
		}
		return 0;
	}

	/*
	 * match on modif char
	 */
	for (p = modif; *p; p++) {
		if (mcmsg_trace_buf[index].t_id >= NTRACEID) {
			continue;
		}
		if (*p == trace_id_name[mcmsg_trace_buf[index].t_id][0]) {
			return 1;
		}
		if (mcmsg_trace_buf[index].t_id != TRACE_SEND &&
		    mcmsg_trace_buf[index].t_id != TRACE_RECV) {
			continue;
		}
		if ((mcmsg_trace_buf[index].t_control) >= MCTRL_END) {
			continue;
		}
		if (*p == control_name[mcmsg_trace_buf[index].t_control][0]) {
			return 1;
		}
	}
	return 0;
}

#endif	MSG_TRACE

void
mcmsg_db_trace(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
#if	MSG_TRACE
	int		i;
	int		j;
	int		n;
	int		c;
	unsigned long	w;
	unsigned long	w0;
	int		tflag;
	int		sflag;

	tflag = 0;
	if (modif != 0 && *modif == 't') {
		tflag++;
		modif++;
	} else if (modif != 0 && *modif == 'z') {
		sflag++;
		modif++;
	}
	if (count == -1)
		count = 20;
	if (have_addr) {
		if (addr > mcmsg_trace_count) {
			db_printf("Too large\n");
			return;
		}
		i = mcmsg_trace_index - (mcmsg_trace_count - addr + 1);
		if (i < 0) {
			i += NTRACE;
		}
		if (i < 0) {
			db_printf("Too small\n");
			return;
		}
		n = count;
		c = addr;
	} else {
		i = mcmsg_trace_index;
		for (n = 0, c = mcmsg_trace_count + 1; n < count; ) {
			if (i > 0) {
				j = i - 1;
			} else {
				j = NTRACE-1;
			}
			if (mcmsg_trace_buf[j].t_id == 0) {
				break;
			}
			i = j;
			if (mcmsg_db_match(j, modif)) {
				n++;
			}
			c--;
			if (j == mcmsg_trace_index) {
				break;
			}
		}
	}
	if (n == 0) {
		return;
	}
	db_printf("%10u:          Control    Give  Length Seq      SrcDst\n",
		mcmsg_trace_count);
	w0 = mcmsg_trace_buf[i].hwtime[0];
	for (; n > 0; c++) {
		if (mcmsg_db_match(i, modif)) {
			if (tflag) {
				w = mcmsg_trace_buf[i].hwtime[0] - w0;
				w0 = mcmsg_trace_buf[i].hwtime[0];
			} else if (sflag) {
				w = mcmsg_trace_buf[i].stack * 64;
			} else {
				w = c;
			}
			switch (mcmsg_trace_buf[i].t_id) {

			case TRACE_SEND:
			case TRACE_RECV:
				if ((mcmsg_trace_buf[i].t_control) >=
				    MCTRL_END) {
					mcmsg_trace_buf[i].hdr1 &= 0xFFFF0000;
				}
				switch (mcmsg_trace_buf[i].extra) {

				case 0:
			db_printf("%10u. %8s %8s %5u %7u %5u %10u \n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
				    control_name[mcmsg_trace_buf[i].t_control],
					mcmsg_trace_buf[i].t_give,
					mcmsg_trace_buf[i].t_length,
					mcmsg_trace_buf[i].t_sequence,
					mcmsg_trace_buf[i].t_source_pid);
				  break;

				case 1:
			db_printf("%10u. %8s %8s %5u %7u %5u %10u  %08x\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
				    control_name[mcmsg_trace_buf[i].t_control],
					mcmsg_trace_buf[i].t_give,
					mcmsg_trace_buf[i].t_length,
					mcmsg_trace_buf[i].t_sequence,
					mcmsg_trace_buf[i].t_source_pid,
					mcmsg_trace_buf[i].extra1);
				  break;

				case 2:
			db_printf("%10u. %8s %8s %5u %7u %5u %10u  %08x %08x\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
				    control_name[mcmsg_trace_buf[i].t_control],
					mcmsg_trace_buf[i].t_give,
					mcmsg_trace_buf[i].t_length,
					mcmsg_trace_buf[i].t_sequence,
					mcmsg_trace_buf[i].t_source_pid,
					mcmsg_trace_buf[i].extra1,
					mcmsg_trace_buf[i].extra2);
				  break;

				default:
			db_printf("%10u. %8s %8s %5u %7u %5u %10u  %8d\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
				    control_name[mcmsg_trace_buf[i].t_control],
					mcmsg_trace_buf[i].t_give,
					mcmsg_trace_buf[i].t_length,
					mcmsg_trace_buf[i].t_sequence,
					mcmsg_trace_buf[i].t_source_pid,
					mcmsg_trace_buf[i].extra);
				  break;
				}
				break;

			case TRACE_PROVIDE:
				db_printf("%10u. %7s %08X       %7u          %08X\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1,
					mcmsg_trace_buf[i].t_hdr3,
					mcmsg_trace_buf[i].t_hdr2);
				break;

			case TRACE_DROP:
				db_printf("%10u. %7s %08X %s\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr2,
					mcmsg_trace_buf[i].t_hdr1);
				break;

			case TRACE_DEBUG:
				switch (mcmsg_trace_buf[i].extra) {

				case 0:
				  db_printf("%10u. %7s %10s\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1);
				  break;

				case 1:
				  db_printf("%10u. %7s %10s %08X\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1,
					mcmsg_trace_buf[i].t_hdr2);
				  break;

				case 2:
				  db_printf("%10u. %7s %10s %08X %08X\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1,
					mcmsg_trace_buf[i].t_hdr2,
					mcmsg_trace_buf[i].t_hdr3);
				  break;

				case 3:
				  db_printf("%10u. %7s %10s %08X %08X %08X\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1,
					mcmsg_trace_buf[i].t_hdr2,
					mcmsg_trace_buf[i].t_hdr3,
					mcmsg_trace_buf[i].extra1);
				  break;

				case 4:
				  db_printf("%10u. %7s %10s %08X %08X %08X %08X\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1,
					mcmsg_trace_buf[i].t_hdr2,
					mcmsg_trace_buf[i].t_hdr3,
					mcmsg_trace_buf[i].extra1,
					mcmsg_trace_buf[i].extra2);
				  break;

				default:
				  db_printf("%10u. %7s %10s %12d\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1,
					mcmsg_trace_buf[i].extra);
				  break;
				}
				break;

			case TRACE_TIME:
				db_printf("%10u. %7s %s\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1);
				break;

			case TRACE_CALL:
				j = mcmsg_trace_buf[i].extra;
				db_printf("%10u. %7s %-8s(",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					post_name_list[j]);
				if (post_args_list[j] >= 1) {
					db_printf(" %08x",
						mcmsg_trace_buf[i].t_hdr1);
				}
				if (post_args_list[j] >= 2) {
					db_printf(", %08x",
						mcmsg_trace_buf[i].t_hdr2);
				}
				if (post_args_list[j] >= 3) {
					db_printf(", %08x",
						mcmsg_trace_buf[i].t_hdr3);
				}
				if (post_args_list[j] >= 4) {
					db_printf(", %08x",
						mcmsg_trace_buf[i].extra1);
				}
				if (post_args_list[j] >= 5) {
					db_printf(", %08x",
						mcmsg_trace_buf[i].extra2);
				}
				if (post_args_list[j] <= 5) {
					db_printf(" )\n");
				} else {
					db_printf(",\n");
				}
				break;

			case TRACE_CALLX:
				j = mcmsg_trace_buf[i].extra;
				db_printf("                               %08x",
					mcmsg_trace_buf[i].t_hdr1);
				if (post_args_list[j] >= 7) {
					db_printf(", %08x",
						mcmsg_trace_buf[i].t_hdr2);
				}
				if (post_args_list[j] >= 8) {
					db_printf(", %08x",
						mcmsg_trace_buf[i].t_hdr3);
				}
				if (post_args_list[j] >= 9) {
					db_printf(", %08x",
						mcmsg_trace_buf[i].extra1);
				}
				if (post_args_list[j] >= 10) {
					db_printf(", %08x",
						mcmsg_trace_buf[i].extra2);
				}
				db_printf(" )\n");
				break;

			}
			n--;
		}
		i++;
		if (i == NTRACE) {
			i = 0;
		}
		if (i == mcmsg_trace_index) {
			break;
		}
	}

#else	MSG_TRACE

	db_printf("Message tracing not enabled\n");

#endif	MSG_TRACE
}


void
mcmsg_db_nicreset(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{

	mcmsg_nic_reset();
}

char *
nic_status_name[35] = {
	"not-full",
	"not-almost-full",
	"almost-empty",
	"empty",

	"full",
	"almost-full",
	"not-almost-empty",
	"two",
	"one",
	"not-empty",

	"eod-in",
	"rcv-rdy",
	"xmt-rdy",
	"eod-last",

	"crc0",
	"crc1",
	"pr-par0",
	"pr-par1",
	"pr-par2",
	"pr-par3",
	"pr-par4",
	"pr-par5",
	"pr-par6",
	"pr-par7",
	"net-par0",
	"net-par1",
	"xmt-overrun",
	"rcv-overrun",
	"rcv-underrun",

	"xreq",
	"xack",
	"rreq",
	"rack",
	"xmip",
	"rmip"
};

void
mcmsg_db_nicregs(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	nic_reg	t;
	nic_reg	u;
	int	i;
	int	c;
	int	n;
	extern char *mcmsg_nic_trace_name[];

#if	MACH_ASSERT

	if (modif != 0 && *modif == 't') {
		modif++;

		if (count == -1)
			count = 20;
		if (have_addr) {
			if (addr > mcmsg_nic_trace_count) {
				db_printf("Too large\n");
				return;
			}
			i = mcmsg_nic_trace_index -
			    (mcmsg_nic_trace_count - addr) + 1;
			if (i < 0) {
				i += NIC_TRACE_SIZE;
			}
			if (i < 0) {
				db_printf("Too small\n");
				return;
			}
			n = count;
			c = addr;
		} else {
			if (mcmsg_nic_trace_count >= count) {
				addr = mcmsg_nic_trace_count - count;
			} else {
				addr = 0;
			}
			i = mcmsg_nic_trace_index -
			    (mcmsg_nic_trace_count - addr) + 1;
			if (i < 0) {
				i += NIC_TRACE_SIZE;
			}
			n = count;
			c = addr;
		}
		if (n == 0) {
			return;
		}
		for (;;) {
			if (mcmsg_nic_trace_buf[i].place > 0 &&
			    mcmsg_nic_trace_buf[i].place <= 15) {
				db_printf("%10u. %s %08X %08X %6u %6u\n",
					c,
					mcmsg_nic_trace_name[
						mcmsg_nic_trace_buf[i].place],
					mcmsg_nic_trace_buf[i].status.halfs.hi,
					mcmsg_nic_trace_buf[i].status.halfs.lo,
					mcmsg_nic_trace_buf[i].scount,
					mcmsg_nic_trace_buf[i].rcount);
			}
			if (i == mcmsg_nic_trace_index) {
				t.full = NIC.status.full;
				u.full = NIC.control.full;
				break;
			}
			if (--n == 0) {
				t.full = mcmsg_nic_trace_buf[i].status.full;
				break;
			}
			i++;
			if (i == NIC_TRACE_SIZE) {
				i = 0;
			}
			c++;
		}
	} else {
		t.full = NIC.status.full;
		u.full = NIC.control.full;
	}
#else	MACH_ASSERT
	t.full = NIC.status.full;
	u.full = NIC.control.full;
#endif	MACH_ASSERT

	if (modif != 0 && *modif == 'e') {
		extern unsigned long mcmsg_nic_err_stat;

		t.halfs.lo = mcmsg_nic_err_stat;
		modif++;
	}

	db_printf("            NIC status      %08X %08X\n",
		t.halfs.hi, t.halfs.lo);
	db_printf("            NIC control     %08X %08X\n",
		u.halfs.hi, u.halfs.lo);

	db_printf("\nNIC status transmit");
	for (i = 0; i < 4; i++) {
		if (t.halfs.lo & (1 << i)) {
			db_printf(" %s", nic_status_name[i]);
		}
	}
	db_printf("\n            receive");
	for (; i < 10; i++) {
		if (t.halfs.lo & (1 << i)) {
			db_printf(" %s", nic_status_name[i]);
		}
	}
	db_printf("\n            general");
	for (; i < 14; i++) {
		if (t.halfs.lo & (1 << i)) {
			db_printf(" %s", nic_status_name[i]);
		}
	}
	db_printf("\n             errors");
	for (; i < 29; i++) {
		if (t.halfs.lo & (1 << i)) {
			db_printf(" %s", nic_status_name[i]);
		}
	}
	db_printf("\n            network");
	for (; i < 32; i++) {
		if (t.halfs.lo & (1 << i)) {
			db_printf(" %s", nic_status_name[i]);
		}
	}
	for (; i < 35; i++) {
		if (t.halfs.hi & (1 << (i - 32))) {
			db_printf(" %s", nic_status_name[i]);
		}
	}
	db_printf("\n");

	if (modif != 0 && *modif == 'd') {
		modif++;

		n = 0;
		db_printf("\nFIFO data          NIC receive,general,error status\n");
		while (t.halfs.lo & NIC_STAT_CAN_READ_1) {
			u.full = NIC.io.full;
			t.full = NIC.status.full;
			db_printf(" %08X %08X ", u.halfs.hi, u.halfs.lo);
			for (i = 4; i < 29; i++) {
				if (t.halfs.lo & (1 << i)) {
					db_printf(" %s", nic_status_name[i]);
				}
			}
			db_printf("\n");
			n++;
		}
		db_printf("%d words\n", n);
	}

	if (modif != 0 && *modif == 'r') {
		modif++;
		db_printf("\nFIFO data          NIC receive,general,error status\n");
		u.full = NIC.io.full;
		t.full = NIC.status.full;
		db_printf(" %08X %08X ", u.halfs.hi, u.halfs.lo);
		for (i = 4; i < 29; i++) {
			if (t.halfs.lo & (1 << i)) {
				db_printf(" %s", nic_status_name[i]);
			}
		}
		db_printf("\n");
	}
}

#if	BIGPKTS
/*
 * print out concurrent send/recv globals
 */
void
mcmsg_db_bigpkts()
{
	extern int mcmsg_local_send;

	if (mcmsg_send_in_progress)
	{
		db_printf("mcmsg_send_in_progress %d\n",
			mcmsg_send_in_progress);
		db_printf("mcmsg_send_count_remain %d\n",
			mcmsg_send_count_remain);
		db_printf("mcmsg_send_addr_remain 0x%x\n",
			mcmsg_send_addr_remain);
	}

	if (mcmsg_recv_waiting)
		db_printf("mcmsg_recv_waiting %d\n", mcmsg_recv_waiting);

	if (mcmsg_send_waiting)
		db_printf("mcmsg_send_waiting %d\n", mcmsg_send_waiting);

	if (mcmsg_local_send)
		db_printf("mcmsg_local_send %d\n", mcmsg_local_send);
}
#endif	BIGPKTS

/*
 *	Get some communications info.
 */
void
mcmsg_db_comm()
{
extern int dispatch_state;
#if    BIGPKTS && BUMPERS
extern int mcmsg_lockup_recoveries;
#endif BIGPKTS && BUMPERS
char *st;

	mcmsg_db_nicregs(0,0,-1,0);
	db_printf("\n");
	mcmsg_db_dp();
	db_printf("\n");
	mcmsg_db_mcmsg_hw();

#if MSG_TRACE
	switch(dispatch_state) {
		case 0:
			st = "idle";
			break;
		case 1:
			st = "servicing nic interrupt";
			break;
		case 2:
			st = "servicing kernel request";
			break;
		case 3:
			st = "servicing user request";
			break;
		default:
			st = "** in invalid state **";
			break;
	}
	db_printf ("\nMessage Processor is %s\n", st);
#endif MSG_TRACE

#if    BIGPKTS && BUMPERS
	db_printf ("NIC lockup recovery count = %d\n", mcmsg_lockup_recoveries);
#endif BIGPKTS && BUMPERS

#if	BIGPKTS
	mcmsg_db_bigpkts();
#endif	BIGPKTS
}

/*
 *	Quick & Dirty DP ASIC display routine.
 *	Use via debugger "!mcmsg_db_dp"
 */
mcmsg_db_dp()
{
	unsigned long lo, hi;
	char *s;

	lo = inl(DP_STATUS_LO);
	hi = inl(DP_STATUS_HI);
	db_printf ("dp status  lo = 0x%08x   status hi = 0x%08x\n", lo, hi);
	lo = inl(DP_CONTROL0_READ);
	hi = inl(DP_CONTROL1_READ);
	db_printf ("dp control0   = 0x%08x   control1 =  0x%08x\n", lo, hi);

	hi = inl(DP_LTU0_COUNT);
	s = (char *)0;
	if ((hi & 0x1ff) != 0) s = " **NON-ZERO**";
	db_printf ("dp ltu0 (RECV) Count = 0x%08x %s\n", hi, s);

	hi = inl(DP_LTU1_COUNT);
	s = (char *)0;
	if ((hi & 0x1ff) != 0) s = " **NON-ZERO**";
	db_printf ("dp ltu1 (SEND) Count = 0x%08x %s\n", hi, s);
}

/*
 * Display mcmsg_hw structure.
 *
 */
mcmsg_db_mcmsg_hw()
{
int i;

	db_printf("mcmsg_hw:");
	db_printf("\nrecv_status    : %08X", mcmsg_hw.recv_status);
    for (i=0; i < 32; i++) {
        if (mcmsg_hw.recv_status & (1 << i)) {
            db_printf(" %s", nic_status_name[i]);
        }
    }
#ifdef RECV_INTR_MODE_2
	if ((mcmsg_hw.recv_status & RECV_INTR_MODE_2) == RECV_INTR_MODE_2)
		db_printf(" (Mode 2)");
	else 
#endif
	if ((mcmsg_hw.recv_status & RECV_INTR_MODE_1) == RECV_INTR_MODE_1)
		db_printf(" (Mode 1)");
	else db_printf(" Invalid mode");

	db_printf("\nrecv_enable    : %08X %08X", mcmsg_hw.recv_enable.halfs.hi,
		mcmsg_hw.recv_enable.halfs.lo);

	db_printf("\nsend_int_enable: %d", mcmsg_hw.send_int_enable);
	db_printf("\nsend_enable    : %08X %08X", mcmsg_hw.send_enable.halfs.hi,
		mcmsg_hw.send_enable.halfs.lo);
	for (i = 0; i < 32; i++) {
		if (mcmsg_hw.send_enable.halfs.lo & (1 << i)) {
			db_printf(" %s", nic_status_name[i]);
		}
	}
	db_printf("\n");
	return 0;
}


#if NX

#define XNSTATES (sizeof(xstates)/sizeof(xstates[0]))
static
char *xstates[] = {
	"XMSG_FREE ",
	"XMSG_EMPTY",
	"XMSG_FULL ",
	"XMSG_TRASH",
	"XMSG_STOP ",
	"XMSG_CONT ",
	"XMSG_SPLIT",
	"XMSG_BUSY",
};

#define NNSTATES (sizeof(nstates)/sizeof(nstates[0]))
static
char *nstates[] = {
	"NX_FREE",
	"NX_ACTIVE",
	"NX_COMPLETE",
	"NX_BUFFERED",
};

void
mcmsg_db_rbuf(addr, have_addr, count)
	unsigned long	addr;
	int		have_addr;
	int		count;
{
	mcmsg_task_t	*mt;
	xmsg_t		*xp;
	unsigned long	did[200];
	unsigned long	msg_type;
	int		i;
	int		j;
	select_item_t	*si;

	mt = mcmsg_debug_mt();
	if (mt == 0) {
		return;
	}
	if (count == -1 || count > 200)
		count = 200;
	if (!have_addr) {
		db_printf("rdy %08x  head %08x  tail %08x\n",
			mt->xmsg_rdy,
			mt->xmsg_head, mt->xmsg_tail);
		addr = (unsigned long)(mt->xmsg_rdy);
	}
	if (addr != 0) {
	    db_printf("xmsg_addr  dest_ptype  msg_type  src_node  src_ptype      count\n");
	}
	for (i = 0; i < count; i++) {
		xp = (xmsg_t *)mcmsg_validate_line(mt, addr);
		if (xp == 0) {
			db_printf("Invalid address\n");
			return;
		} 
		if (xp->state == XMSG_EMPTY) {
			break;
		}
		si = xp->si;
		if ((unsigned long)si >= (unsigned long)mcmsg_memory &&
	    		(unsigned long)si <
			 (unsigned long)mcmsg_memory +  MMSIZE &&
	    		((unsigned long)si & 0x3) == 0 &&
	    		(unsigned long)si->nxrq.xmsg == addr) {
			msg_type = si->nxrq.msg_type;
		} else {
			msg_type = xp->msg_type;
		}
		db_printf("%#8x  %10d  %#8x  %8d   %8d %10u\n",
			addr, xp->dest_ptype, msg_type, xp->source_node, 
			xp->source_ptype, xp->length);
		did[i] = addr;

		addr = (unsigned long)(xp->link);
		if (addr == 0) {
			break;
		}
		for (j = 0; j < i; j++) {
			if (did[j] == addr) {
				db_printf("looped back to %08x\n", addr);
				return;
			}
		}
	}
	if ((i >= 200) && (addr != 0)) {
		db_printf("more than %d messages buffered\n",i);
	}
}


void
mcmsg_db_buf(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	mcmsg_task_t	*mt;
	xmsg_t		*xp;
	unsigned long	prevaddr;
	unsigned long	did[20];
	int		i;
	int		j;
	int		scanflag;
	unsigned long	total;

	scanflag = (modif != 0 && *modif == 's');
	mt = mcmsg_debug_mt();
	if (mt == 0) {
		return;
	}
	if (count == -1 || count > 20)
		count = 20;
	if (!have_addr) {
		db_printf("rdy %08x  head %08x  tail %08x\n",
			mt->xmsg_rdy,
			mt->xmsg_head, mt->xmsg_tail);
		addr = (unsigned long)(mt->xmsg_rdy);
	}
	total = 0;
	for (i = 0; i < count; i++) {
		xp = (xmsg_t *)mcmsg_validate_line(mt, addr);
		if (xp == 0) {
			db_printf("Invalid address\n");
			return;
		}
		db_printf("%08x: %10s size %08x link %08x len %08x chain %01d\n",
			addr,
			xp->state < XNSTATES? xstates[xp->state] : "????????? ",
			xp->size, xp->link, xp->length, xp->chain_number);
		did[i] = addr;
		if (xp->state == XMSG_EMPTY) {
			if (total == 0) {
				if (mt->xmsg_head != addr) {
					db_printf("damaged head\n");
				}
			}
			total += xp->size + sizeof(xmsg_t);
		}

		if (xp->state >= XNSTATES) {
			db_printf("invalid state\n");
			break;
		}
		if (scanflag) {
			addr = addr + sizeof(xmsg_t) + xp->size;
		} else {
			prevaddr = addr;
			addr = (unsigned long)(xp->link);
		}
		if (addr == 0) {
			if (!have_addr &&
			    !scanflag) {
				if (mt->xmsg_tail != prevaddr) {
					db_printf("damaged tail\n");
				}
				if (mt->provided + sizeof(xmsg_t) !=
				    total) {
					db_printf("damaged provided %08x\n",
						mt->provided);
				}
			}
			break;
		} else if (!have_addr &&
			   !scanflag &&
			   mt->xmsg_tail == prevaddr) {
			db_printf("damaged tail %08x\n", mt->xmsg_tail);
		}
		for (j = 0; j < i; j++) {
			if (did[j] == addr) {
				db_printf("looped back to %08x\n", addr);
				return;
			}
		}
	}
	db_printf(" total    %10d      %08X\n", i+1, total);
}

void
mcmsg_db_attach(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	mcmsg_task_t	*mt;
	int		i;
	int		first;
	int		last;
	int		aflag;
	unsigned long	send_avail_sum;
	unsigned long	recv_target_sum;
	unsigned long	recv_total_sum;
	unsigned long	recv_give_sum;
	unsigned long	recv_avail_sum;
	unsigned long	numnodes;
	select_item_t	*pid_st;
	select_item_t	*pid_si;
	int		t;

	mt = mcmsg_debug_mt();
	if (mt == 0) {
		return;
	}
	numnodes = mt->numnodes;
	if (!have_addr) {
		first = 0;
		last = numnodes;
		aflag = (modif != 0 && *modif == 'a');
	} else {
		first = addr;
		last = first;
		aflag = 1;
	}
	send_avail_sum = 0;
	recv_target_sum = 0;
	recv_total_sum = 0;
	recv_give_sum = 0;
	recv_avail_sum = 0;
	db_printf(
"  Node        Pid   Send avail     Recv target   total   give   avail\n");
	pid_si = mt->attached;
	while (pid_si != 0) {
		if (pid_si == 0) {
			db_printf("Null link in attached list item 0x%x\n",pid_si);
			break;	
		}
		if (aflag || pid_si->ppid.send_avail != 0 ||
			    pid_si->ppid.recv_target != 0) {
			db_printf("%5u. %10u     %7u        %7u %7u %7u %7u\n",
				pid_si->ppid.node,
				pid_si->value,
				pid_si->ppid.send_avail,
				pid_si->ppid.recv_target,
				pid_si->ppid.recv_total,
				pid_si->ppid.recv_give,
				pid_si->ppid.recv_total -
				 pid_si->ppid.recv_give);
			send_avail_sum += pid_si->ppid.send_avail;
			recv_target_sum += pid_si->ppid.recv_target;
			recv_total_sum += pid_si->ppid.recv_total;
			recv_give_sum += pid_si->ppid.recv_give;
			recv_avail_sum += pid_si->ppid.recv_total -
					pid_si->ppid.recv_give;
		}
		pid_si = pid_si->ppid.attach_link;
	}
	db_printf(
"                      -------        ------- ------- ------- -------\n");
	db_printf(" total                %7u        %7u %7u %7u %7u\n",
		send_avail_sum,
		recv_target_sum,
		recv_total_sum,
		recv_give_sum,
		recv_avail_sum);
	if (mt != 0) {
		db_printf(" provided  %7u          assigned %7u\n",
			mt->provided, mt->assigned);
	}
}


void
mcmsg_db_sendwait(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	mcmsg_task_t	*mt;
	int		i;
	int		first;
	int		last;
	int		aflag;
	unsigned long	numnodes;
	select_item_t	*pid_st;
	select_item_t	*pid_si;
	select_item_t	*si;
	select_item_t	*sl;
	select_item_t	*st;

	int		t, tt;

	mt = mcmsg_debug_mt();
	if (mt == 0) {
		return;
	}
	numnodes = mt->numnodes;
	if (!have_addr) {
		first = 0;
		last = numnodes;
		aflag = (modif != 0 && *modif == 'a');
	} else {
		first = addr;
		last = first;
		aflag = 1;
	}
	    db_printf("destnode destptype   msg_type    length   dest_pid     pid_si savail         si\n");
	pid_st = (select_item_t *)(mt->pid_sel->zero);
	if (pid_st != 0) {
	    t = mt->numnodes;
	    pid_si = pid_st->link;
	    for (;;) {
		if (pid_si == 0) {
			db_printf("Broken link in hash bucket zero\n");
			break;
		}
		if (pid_si->ppid.send_wait != 0) {
		    sl = st = pid_si->ppid.send_wait;
		    si = sl->link;
		    tt = MAXLOOP;
		    for(;;) {
			db_printf("%5u %6u %#10x %9u %#10x %#10x %6u %#10x\n",
				pid_si->ppid.node,
				si->nxrq.dest_ptype,
				si->nxrq.msg_type,
				si->nxrq.count,
				pid_si->value,
				pid_si,
				pid_si->ppid.send_avail,
				si);
			if (si == st) 
				break;
			sl = si; si = si->link;
			if (tt-- == 0) {
			    db_printf("loop in sendwait list pid_si %#10x\n",
				pid_si);
			    break;
			}
		    }
		}
		if (pid_si == pid_st) {
			break;
		}
		if (t-- == 0) {
			db_printf("Loop in hash bucket %d\n", i);
			break;
		}
		pid_si = pid_si->link;
	    }
	}
	for (i = 0; i < SELECT_HASH_LEN; i++) {
		pid_st = (select_item_t *)(mt->pid_sel->hash[i]);
		if (pid_st == 0) {
			continue;
		}
		t = mt->numnodes;
		pid_si = pid_st->link;
		for (;;) {
			if (pid_si == 0) {
				db_printf("Broken link in hash bucket %d\n", i);
				break;
			}
			if (pid_si->ppid.send_wait != 0) {
		    	    sl = st = pid_si->ppid.send_wait;
		    	    si = sl->link;
		    	    for(;;) {
				db_printf("%8u %9u %#10x %9u %#10x %#10x %6u %#8x\n",
					pid_si->ppid.node,
					si->nxrq.dest_ptype,
					si->nxrq.msg_type,
					si->nxrq.count,
					pid_si->value,
					pid_si,
					pid_si->ppid.send_avail,
					pid_si->ppid.send_wait);
				if (si == st) 
					break;
				sl = si; si = si->link;
		    	    }
			}
			if (pid_si == pid_st) {
				break;
			}
			if (t-- == 0) {
				db_printf("Loop in hash bucket %d\n", i);
				break;
			}
			pid_si = pid_si->link;
		}
	}
}

void
mcmsg_db_avail(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	mcmsg_task_t	*mt;
	int		i;
	int		first;
	int		last;
	int		aflag;
	unsigned long	send_avail_sum;
	unsigned long	recv_target_sum;
	unsigned long	recv_total_sum;
	unsigned long	recv_give_sum;
	unsigned long	recv_avail_sum;
	unsigned long	numnodes;
	select_item_t	*pid_st;
	select_item_t	*pid_si;
	int		t;

	mt = mcmsg_debug_mt();
	if (mt == 0) {
		return;
	}
	numnodes = mt->numnodes;
	if (!have_addr) {
		first = 0;
		last = numnodes;
		aflag = (modif != 0 && *modif == 'a');
	} else {
		first = addr;
		last = first;
		aflag = 1;
	}
	send_avail_sum = 0;
	recv_target_sum = 0;
	recv_total_sum = 0;
	recv_give_sum = 0;
	recv_avail_sum = 0;
	db_printf(
"  Node        Pid   Send avail     Recv target   total   give   avail\n");
	for (i = 0; i < SELECT_HASH_LEN; i++) {
		pid_st = (select_item_t *)(mt->pid_sel->hash[i]);
		if (pid_st == 0) {
			continue;
		}
		t = mt->numnodes;
		pid_si = pid_st->link;
		for (;;) {
			if (pid_si == 0) {
				db_printf("Broken link in hash bucket %d\n", i);
				break;
			}
			if (aflag ||
			    pid_si->ppid.send_avail != 0 ||
			    pid_si->ppid.recv_target != 0) {
			db_printf("%5u. %10u     %7u        %7u %7u %7u %7u\n",
					pid_si->ppid.node,
					pid_si->value,
					pid_si->ppid.send_avail,
					pid_si->ppid.recv_target,
					pid_si->ppid.recv_total,
					pid_si->ppid.recv_give,
					pid_si->ppid.recv_total -
					 pid_si->ppid.recv_give);
				send_avail_sum += pid_si->ppid.send_avail;
				recv_target_sum += pid_si->ppid.recv_target;
				recv_total_sum += pid_si->ppid.recv_total;
				recv_give_sum += pid_si->ppid.recv_give;
				recv_avail_sum += pid_si->ppid.recv_total -
					pid_si->ppid.recv_give;
			}
			if (pid_si == pid_st) {
				break;
			}
			if (t-- == 0) {
				db_printf("Loop in hash bucket %d\n", i);
				break;
			}
			pid_si = pid_si->link;
		}
	}
	db_printf(
"                      -------        ------- ------- ------- -------\n");
	db_printf(" total                %7u        %7u %7u %7u %7u\n",
		send_avail_sum,
		recv_target_sum,
		recv_total_sum,
		recv_give_sum,
		recv_avail_sum);
	if (mt != 0) {
		db_printf(" provided  %7u          assigned %7u\n",
			mt->provided, mt->assigned);
	}
}

void
mcmsg_db_mmem(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	extern unsigned long	mcmsg_memalloc_count;
	extern unsigned long	mcmsg_memfree_count;
	extern unsigned long	mcmsg_memcoll_count;
	extern unsigned long	mcmsg_meminlp_count;
	extern unsigned long	mcmsg_memcfail_count;
	extern unsigned long	mcmsg_memifail_count;
	extern unsigned long	mcmsg_memalloc[];
	int		i;
	int		n;
	unsigned long	*m;
	unsigned long	tf;
	unsigned long	cf;
	unsigned long	ta;
	unsigned long	ca;

	if (modif != 0 && *modif == 'a') {
		mcmsg_db_mmema();
		return;
	}
	db_printf("l2size         size         free    allocated\n");
	tf = 0;
	cf = 0;
	ta = 0;
	ca = 0;
	for (i = 0; i < LOG2MMSIZE; i++) {
		n = 0;
		m = mcmsg_memlist[i];
		while (m != 0) {
			n++;
			if (n == 999999) {
				db_printf("loop at 0x%08x\n", m);
				break;
			}
			m = (unsigned long *)(m[0]);
		}
		db_printf(" %4d. %12u %12u %12u\n",
			i, 1 << i, n, mcmsg_memalloc[i]);
		cf += n;
		tf += n * (1 << i);
		ca += mcmsg_memalloc[i];
		ta += mcmsg_memalloc[i] * (1 << i);
	}
	db_printf(" total %12u %12u %12u %12u\n\n", tf, cf, ca, ta);

	db_printf("counts      success\t      failure\n\n");
	db_printf(" alloc %12u\t%12u\n",
		mcmsg_memalloc_count - mcmsg_memcfail_count,
		mcmsg_memcfail_count);
	db_printf(" coll  %12u\t%12u\n",
		mcmsg_memcoll_count - mcmsg_memcfail_count,
		mcmsg_memcfail_count);
	db_printf(" inlp  %12u\t%12u\n",
		mcmsg_meminlp_count - mcmsg_memifail_count,
		mcmsg_memifail_count);
	db_printf(" free  %12u\n", mcmsg_memfree_count);
}

mcmsg_db_mmema_search(addr, l2size, head)
	unsigned long	addr;
	int		l2size;
	int		head;
{
	int		f;
	int		g;
	unsigned long	*m;

	for (m = mcmsg_memlist[l2size]; m != 0; m = (unsigned long *)m[0]) {
		if (m == (unsigned long *)addr) {
			return 1;
		}
	}
	if (l2size >= 4) {
		f = mcmsg_db_mmema_search(addr, l2size - 1, head);
		g = mcmsg_db_mmema_search(addr + (1 << (l2size - 1)),
					  l2size - 1,
					  f);
		return g;
	}
	if (head) {
		db_printf(" 0x%08x\n", addr);
	}
	return 0;
}

mcmsg_db_mmema()
{

	mcmsg_db_mmema_search(mcmsg_memory, LOG2MMSIZE, 1);
}

extern select_t mcmsg_seq_sel;

void
mcmsg_db_selseq(addr, have_addr )
	unsigned long	addr;
	int		have_addr;
{
	int		i;
	int		j;
	register int		t, tt;
	mcmsg_task_t	*mt;
	register select_item_t	*si;
	register select_item_t	*sl;
	register select_item_t	*st;
	register select_t       *sel;
	register select_item_t	*sel_i;
	register select_item_t	*sel_l;
	register select_item_t	*sel_t;
	xmsg_t          *xp;

	if (!have_addr) {
		mt = mcmsg_debug_mt();
	} else {
		mt = (mcmsg_task_t *)addr;
		db_printf("mcmsg_task      0x%08x\n", mt);
	}
	if (mt == 0) {
		return;
	}

	db_printf(" destptype  recv type  src_node  src_ptype       length           si\n");
	sel = &mcmsg_seq_sel;
	sel_t = sel->zero;
	if (sel_t != 0) {
		t = MAXLOOP;
		sel_l = sel_i = sel_t->link;
		for(;;) {
		    tt = MAXLOOP;
		    while (sel_i != 0) {
			mt = sel_i->mcmsg_task;
			if (mt == 0) {
		    		db_printf("    ?????  %#8x    %6d     %6d   %10u   %#08x\n", 
				    sel_i->nxrq.msg_type, sel_i->nxrq.dest_node,
				    sel_i->nxrq.dest_ptype, sel_i->nxrq.count,
				    sel_i);
			} else {
				xp = (xmsg_t *)mcmsg_validate_line(mt, sel_i->nxrq.xmsg);
		    		db_printf(" %9d   %#8x    %6d     %6d   %10u   %#08x\n", 
				    xp->dest_ptype, sel_i->nxrq.msg_type, 
				    xp->source_node, xp->source_ptype,
				    sel_i->nxrq.count, sel_i);
			}
			sel_i = sel_i->nxrq.seq_link;
			if (tt-- == 0) {
			    db_printf("Loop in sequence \n");
			}
				
		    }
		    sel_i = sel_l;
		    if (sel_i == sel_t) 
			break;
		    if (t-- == 0) {
			db_printf("Loop in sequence sel hash bucket %d\n", i);
			break;
		    }
		    sel_l = sel_i = sel_l->link;
		}
	}
	for (i = 0; i < SELECT_HASH_LEN; i++) {
		sel_t = (select_item_t *) sel->hash[i];
		if (sel_t == 0) 
			continue;
		t = MAXLOOP;
	    	sel_l = sel_i = sel_t->link;
		for(;;) {
	    	    while (sel_i != 0) {
			mt = sel_i->mcmsg_task;
			if (mt == 0) {
		    		db_printf("    ?????  %#8x    %6d     %6d   %10u   %#08x\n", 
				    sel_i->nxrq.msg_type, sel_i->nxrq.dest_node,
				    sel_i->nxrq.dest_ptype, sel_i->nxrq.count,
				    sel_i);
			} else {
				xp = (xmsg_t *)mcmsg_validate_line(mt, sel_i->nxrq.xmsg);
		    		db_printf(" %9d   %#8x    %6d     %6d   %10u   %#08x\n", 
				    xp->dest_ptype, sel_i->nxrq.msg_type, 
				    xp->source_node, xp->source_ptype,
				    sel_i->nxrq.count, sel_i);
			}
		        sel_i = sel_i->nxrq.seq_link;
	    	    }
	    	    sel_i = sel_l;
		    if (sel_i == sel_t) 
			break;
		    if (t-- == 0) {
			db_printf("Loop in sel %#10x hash bucket %d\n", sel_i, i);
			break;
		    }
		    sel_l = sel_i = sel_l->link;
		}
	}
	return;
}


void
mcmsg_db_selpath(addr, have_addr )
	unsigned long	addr;
	int		have_addr;
{
	int		i;
	int		j;
	register int		t;
	register int		tt;
	mcmsg_task_t	*mt;
	register select_item_t	*si;
	register select_item_t	*sl;
	register select_item_t	*st;
	register select_t       *sel;
	register select_item_t	*sel_i;
	register select_item_t	*sel_t;


	if (!have_addr) {
		mt = mcmsg_debug_mt();
	} else {
		mt = (mcmsg_task_t *)addr;
		db_printf("mcmsg_task      0x%08x\n", mt);
	}
	if (mt == 0) {
		return;
	}
	st = mt->selection_path;
	if (st == 0) {
		return;
	}

	db_printf("   recv type    node   ptype       length           si\n");
	sl = st;
	si = sl->link;
	t = MAXLOOP;
	for (;;) {
	    if (si->method != SELMETH_RECV_TYPESEL) {
		db_printf("  %#10x  %6d  %6d   %10u   %#08x\n", 
			si->nxrq.msg_type, si->nxrq.dest_node,
			si->nxrq.dest_ptype, si->nxrq.count, si);
	    } else {
		/* SELMETH_RECV_TYPESEL */
		sel = si->item;
		sel_t = sel->zero;
		if (sel_t != 0) {
			tt = MAXLOOP;
			for(;;) {
			    sel_i = sel_t->link;
			    db_printf("  %#10x  %6d  %6d   %10u   %#08x\n", 
				sel_i->nxrq.msg_type, sel_i->nxrq.dest_node,
				sel_i->nxrq.dest_ptype, sel_i->nxrq.count,
				sel_i);
			    if (sel_i == sel_t) 
				break;
			    if (t-- == 0) {
				db_printf("Loop in zero entry sel %#10xd\n", sel_i);
				break;
			    }
			    sel_i = sel_i->link;
			}
		}
		for (i = 0; i < SELECT_HASH_LEN; i++) {
			sel_t = (select_item_t *) sel->hash[i];
			if (sel_t == 0) 
				continue;
			tt = MAXLOOP;
			for(;;) {
			    sel_i = sel_t->link;
			    db_printf("  %#10x  %6d  %6d   %10u   %#08x\n", 
				sel_i->nxrq.msg_type, sel_i->nxrq.dest_node,
				sel_i->nxrq.dest_ptype, sel_i->nxrq.count,
				sel_i);
			    if (sel_i == sel_t) 
				break;
			    if (t-- == 0) {
				db_printf("Loop in sel %#10x hash bucket %d\n", sel_i, i);
				break;
			    }
			    sel_i = sel_i->link;
			}
			
		}
	    }
				
	    if (si == st)
		break;
	    sl = si;
	    si = si->link;
	    if(t-- == 0) {
		db_printf("Loop in selection path mt %#10x\n", mt);
		break;
	    }
	}
	return;
}

void
mcmsg_db_mt(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	int		i;
	int		j;
	mcmsg_task_t	*mt;
	ptype_list_t	*pt;
	int		method;

	if (!have_addr) {
		mt = mcmsg_debug_mt();
	} else {
		mt = (mcmsg_task_t *)addr;
		db_printf("mcmsg_task      0x%08x\n", mt);
	}
	if (mt == 0) {
		return;
	}
	db_printf(" method                 %s\n", method_name(mt->method));
	db_printf(" link                   0x%08x\n", mt->link);
	db_printf(" app                    %10u\n", mt->applinfo.app);
	db_printf(" pid                    %10d\n", mt->pid);
	db_printf(" task                   0x%08x\n", mt->task);
	db_printf(" dirbase                0x%08x\n", mt->dirbase);
	db_printf(" task_ptype_list        0x%08x\n", mt->task_ptype_list);
	db_printf(" node_ptype_list        0x%08x\n", mt->node_ptype_list);
	db_printf(" pid_sel                0x%08x\n", mt->pid_sel);
	db_printf(" node                   %10u\n", mt->node);
	db_printf(" numnodes               %10u\n", mt->numnodes);
	db_printf(" pkt_size               %10u\n", mt->applinfo.pkt_size);
	db_printf(" selection_path         0x%08x\n", mt->selection_path);
	db_printf(" xmsg_rdy               0x%08x\n", mt->xmsg_rdy);
	db_printf(" xmsg_head              0x%08x\n", mt->xmsg_head);
	db_printf(" xmsg_tail              0x%08x\n", mt->xmsg_tail);
	db_printf(" wired_start            0x%08x\n", mt->wired_start);
	db_printf(" wired_end              0x%08x\n", mt->wired_end);
	db_printf(" provided               %10u\n", mt->provided);
	db_printf(" assigned               %10u\n", mt->assigned);
	db_printf(" reserved               %10u\n", mt->reserved);
	db_printf(" noc                    %10u\n", mt->applinfo.noc);
	db_printf(" memory_each            %10u\n", mt->applinfo.memory_each);
	db_printf(" send_threshold         %10u\n", mt->applinfo.send_threshold);
	db_printf(" send_amount            %10u\n", mt->applinfo.send_count);
	db_printf(" give_threshold         %10u\n", mt->applinfo.give_threshold);
	db_printf(" process_lock           %10u\n", mt->applinfo.process_lock);
	db_printf(" avail_need             0x%08x\n", mt->avail_need);
	db_printf(" rk_recv_need           %10u\n", mt->rk_recv_need);
	db_printf(" send_wait_unk          0x%08x\n", mt->send_wait_unk);
	db_printf(" attached               0x%08x\n", mt->attached);
	db_printf(" attach_wait            0x%08x\n", mt->attach_wait);
	db_printf(" attach_pend            0x%08x\n", mt->attach_pend);
	db_printf("\n");

	pt = mt->task_ptype_list;
	if (pt == 0) {
		return;
	}
	db_printf("task_ptype_list\n");
	db_printf(" method                 %s\n", method_name(pt->method));
	db_printf(" last_entry             %10u\n", pt->last_entry);
	db_printf(" app                    %10u\n", pt->app);
	db_printf(" numnodes               %10u\n", pt->numnodes);
	db_printf(" phys_node_list         0x%08x\n", pt->phys_node_list);
	db_printf(" refcount               0x%08x\n", pt->refcount);

    db_printf(" ptype entries           ptype  mcmsg_task  offset\n");
	for (i = 0; i <= pt->last_entry; i++) {
		db_printf(" entry %3u          %10d 0x%08x %10u\n",
			i, 
			pt->ptype_entry[i].ptype,
			pt->ptype_entry[i].item,
			pt->ptype_entry[i].offset);
		if (i > 20) {
			db_printf(" more...\n");
			break;
		}
	}
	db_printf("\n");

	db_printf("phys_node_list         pid\n");
	for (i = 0; i <= pt->numnodes && i < 16; i++) {
		db_printf(" node  %3u          %10d\n", i, pt->phys_node_list[i]);
	}
	db_printf("\n");

	pt = mt->node_ptype_list;
	if (pt == 0) {
		return;
	}
	db_printf("node_ptype_list\n");
	db_printf(" method                 %s\n", method_name(pt->method));
	db_printf(" last_entry             %10u\n", pt->last_entry);
	db_printf(" app                    %10u\n", pt->app);
	db_printf(" numnodes               %10u\n", pt->numnodes);
	db_printf(" phys_node_list         0x%08x\n", pt->phys_node_list);
	db_printf(" refcount               0x%08x\n", pt->refcount);

    db_printf(" ptype entries           ptype  node_list    offset\n");
	for (i = 0; i <= pt->last_entry; i++) {
		db_printf(" entry %3u          %10d 0x%08x %10u\n",
			i, 
			pt->ptype_entry[i].ptype,
			pt->ptype_entry[i].item,
			pt->ptype_entry[i].offset);
		if (pt->ptype_entry[i].item != 0) {
			for (j = 0; j <= pt->numnodes && j < 16; j++) {
				db_printf(" node  %3u          %10d\n",
				 j,
				 ((node_list_t *)(pt->ptype_entry[i].item))[j]);
			}
		}
	}
	db_printf("\n");
}

void
mcmsg_db_pid(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	mcmsg_task_t	*mt;
	int		i;
	select_item_t	*si;

	mt = mcmsg_debug_mt();
	if (mt == 0) {
		return;
	}
	si = mcmsg_lookup_remote(mt, addr);
	db_printf("pid %10u select item 0x%08x\n", addr, si);
	if (si == 0) {
		return;
	}
	print_pid_si(si);
}

void
mcmsg_db_sel(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	int		i;
	int		t;
	select_t	*sel;
	select_item_t	*sh, *st, *si;

	sel = (select_t *)addr;
	db_printf("selector 0x%08x\n", sel);
	if (sel == 0) {
		return;
	}
	db_printf(" method         %s\n", method_name(sel->method));
	db_printf(" fail           0x%08x\n", sel->fail);
	db_printf(" zero           0x%08x\n", sel->zero);
	for (i = 0; i < SELECT_HASH_LEN; i++) {
		st = sel->hash[i];
		if (st == 0) {
			continue;
		}
		sh = st->link;
		t = 10;
		si = sh;
		db_printf(" Hash %d:\n", i);
		for (;;) {
			db_printf(" %10d     0x%08x\n", si->value, si);
			if (si == st) {
				break;
			}
			if (si == 0) {
				db_printf("  broken, giving up\n");
				break;
			}
			si = si->link;
			if (t-- == 0) {
				db_printf("  10 loops, giving up\n");
				break;
			}
		}
	}
}

void
mcmsg_db_item(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	mcmsg_task_t	*mt;
	int		i;
	select_item_t	*si;
	nxreq_t		*np;
	xmsg_t		*xp;

	si = (select_item_t *)addr;
	if (si == 0) {
		db_printf("null selector\n");
		return;
	}
	if (si->method == SELMETH_PID) {
		db_printf("select item     0x%08x\n", si);
		print_pid_si(si);
	} else {
		mt = si->mcmsg_task;
		if (mt != 0) {
			np = (nxreq_t *)mcmsg_validate_line(mt, si->nxrq.request);
			xp = (xmsg_t *)mcmsg_validate_line(mt, si->nxrq.xmsg);
		} else {
			np = 0;
			xp = 0;
		}

		db_printf("select item    0x%08x", si);
		if (np != 0) {
			db_printf(" nxreq         0x%08x", np);
		}
		if (xp != 0) {
			db_printf("  xmsg          0x%08x", xp);
		}
		db_printf("\n");

		db_printf("method %18s", method_name(si->method));
		if (np != 0) {
			db_printf(" state         %10s  ",
		    np->state < NNSTATES? nstates[np->state] : "????????? ");
		}
		if (xp != 0) {
			db_printf("state          %10s",
		    xp->state < XNSTATES? xstates[xp->state] : "????????? ");
		}
		db_printf("\n");

		db_printf("nextmethod %14s", method_name(si->nextmethod));
		if (np != 0) {
			db_printf(" err           %10d  ", np->err);
		}
		if (xp != 0) {
			db_printf("chain         %#10x", xp->chain_number);
		}
		db_printf("\n");

		db_printf("link          0x%08x", si->link);
		if (np != 0) {
			db_printf("  link          0x%08x", np->link);
		}
		if (xp != 0) {
			db_printf("  link          0x%08x", xp->link);
		}
		db_printf("\n");

		db_printf("value         %10u", si->value);
		if (np != 0) {
			db_printf("  req           0x%08x", np->req);
		}
		if (xp != 0) {
			db_printf("  backlink       %#10x", xp->backlink);
		}
		db_printf("\n");

		db_printf("item          0x%08x", si->item);
		if (np != 0) {
			db_printf("  info          0x%08x", np->info);
		}
		if (xp != 0) {
			db_printf("  size          %10u", xp->size);
		}
		db_printf("\n");

		db_printf("mcmsg_task    0x%08x", si->mcmsg_task);
		if (xp != 0) {
			db_printf("                            totalsize     %10u", xp->totalsize);
		}
		db_printf("\n");

		db_printf("request       0x%08x", si->nxrq.request);
		db_printf("\n");

		db_printf("xmsg          0x%08x", si->nxrq.xmsg);
		if (np != 0) {
			db_printf("  xmsg          0x%08x", np->xmsg);
		}
		if (xp != 0) {
			db_printf("  si            0x%08x", xp->si);
		}
		db_printf("\n");

		db_printf("msg_type      %10d", si->nxrq.msg_type);
		if (np != 0) {
			db_printf("  type          %10d", np->type);
		}
		if (xp != 0) {
			db_printf("  msg_type      %10d", xp->msg_type);
		}
		db_printf("\n");

		db_printf("take          %10d", si->nxrq.take);
		if (np != 0) {
			db_printf("  bcount        %10d", np->bcount);
		}
		if (xp != 0) {
			db_printf("  xmsg_data     %10d", xp->xmsg_data);
		}
		db_printf("\n");

		db_printf("buf           0x%08x", si->nxrq.buf);
		if (np != 0) {
			db_printf("  buf           0x%08x", np->buf);
		}
		if (xp != 0) {
			db_printf("  buf           0x%08x",
			    (unsigned long)(si->nxrq.xmsg) + sizeof(xmsg_t));
		}
		db_printf("\n");

		db_printf("count         %10d", si->nxrq.count);
		if (np != 0) {
			db_printf("  bsize         %10d", np->bsize);
		}
		if (xp != 0) {
			db_printf("  length        %10u", xp->length);
		}
		db_printf("\n");

		db_printf("stop          %10d", si->nxrq.stop);
		if (xp != 0) {
			db_printf("                            xmsg_stop     %10d", xp->xmsg_stop);
		}
		db_printf("\n");

		db_printf("offset        %10d", si->nxrq.offset);
		if (np != 0) {
			db_printf("  boffset       %10d", np->boffset);
		}
		if (xp != 0) {
			db_printf("  xmsg_offset   %10d", xp->xmsg_offset);
		}
		db_printf("\n");

		db_printf("dest_node     %10d", si->nxrq.dest_node);
		if (np != 0) {
			db_printf("  node          %10d", np->node);
		}
		db_printf("\n");

		db_printf("dest_ptype    %10d", si->nxrq.dest_ptype);
		if (np != 0) {
			db_printf("  ptype         %10d", np->ptype);
		}
		if (xp != 0) {
			db_printf("  dest_ptype    %10d", xp->dest_ptype);
		}
		db_printf("\n");

		db_printf("sequence      %10u", si->nxrq.sequence);
		if (np != 0) {
			db_printf("  localinfo 0   %10d", np->localinfo[0]);
		}
		db_printf("\n");

		db_printf("seq_link      0x%08x", si->nxrq.seq_link);
		if (np != 0) {
			db_printf("  localinfo 1   %10d", np->localinfo[1]);
		}
		db_printf("\n");

		db_printf("pid_si        0x%08x", si->nxrq.pid_si);
		if (np != 0) {
			db_printf("  localinfo 2   %10d", np->localinfo[2]);
		}
		if (xp != 0) {
			db_printf("  source_node   %10d", xp->source_node);
		}
		db_printf("\n");

		db_printf("source_ptype  %10d", si->nxrq.source_ptype);
		if (np != 0) {
			db_printf("  localinfo 3   %10d", np->localinfo[3]);
		}
		if (xp != 0) {
			db_printf("  source_ptype  %10d", xp->source_ptype);
		}
		db_printf("\n");
	}
}

void
mcmsg_db_xmsg(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	mcmsg_task_t	*mt;
	int		i;
	xmsg_t		*xp;
	select_item_t	*si;

	mt = mcmsg_debug_mt();
	if (mt == 0) {
		return;
	}
	xp = (xmsg_t *)mcmsg_validate_line(mt, addr);

	if (xp == 0) {
		db_printf("invalid address %08x\n", addr);
		return;
	}

	si = xp->si;
	if ((unsigned long)si >= (unsigned long)mcmsg_memory &&
	    (unsigned long)si <
			(unsigned long)mcmsg_memory +  MMSIZE &&
	    ((unsigned long)si & 0x3) == 0 &&
	    (unsigned long)si->nxrq.xmsg == addr) {
		mcmsg_db_item(si, have_addr, count, modif);
		return;
	}

	db_printf("  xmsg           0x%08x\n", xp);
	db_printf("   chain         0x%08x\n", xp->chain_number);
	db_printf("   state         %10s\n",
	    xp->state < XNSTATES? xstates[xp->state] : "????????? ");
	db_printf("   link          0x%08x\n", xp->link);
	db_printf("   backlink      0x%08x\n", xp->backlink);
	db_printf("   size          %10u\n", xp->size);
	db_printf("   totalsize     %10u\n", xp->totalsize);
	db_printf("   si            0x%08x\n", xp->si);

	if (xp->state == XMSG_STOP || xp->state == XMSG_CONT) {
		db_printf("   xmsg_offset   0x%08x\n", xp->xmsg_offset);
		db_printf("   xmsg_data     0x%08x\n", xp->xmsg_data);
		db_printf("   xmsg_stop     0x%08x\n", xp->xmsg_stop);
	} else {
		db_printf("   msg_type      %10d\n", xp->msg_type);
		db_printf("   source_ptype  %10d\n", xp->source_ptype);
		db_printf("   dest_ptype    %10d\n", xp->dest_ptype);
	}
	db_printf("   buf           0x%08x\n",
	    (unsigned long)(addr) + sizeof(xmsg_t));
	db_printf("   length        %10u\n", xp->length);
	db_printf("   source_node   %10d\n", xp->source_node);
	db_printf("   prev_adjacent 0x%08x\n", xp->prev_adjacent);
	db_printf("   next_adjacent 0x%08x\n", xp->next_adjacent);
	db_printf("\n");
}

void
mcmsg_db_nxreq(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	mcmsg_task_t	*mt;
	int		i;
	nxreq_t		*np;
	xmsg_t		*xp;
	select_item_t	*si;

	mt = mcmsg_debug_mt();
	if (mt == 0) {
		return;
	}

	np = (nxreq_t *)mcmsg_validate_long(mt, addr);
	if (np == 0) {
		db_printf("invalid address %08x\n", addr);
		return;
	}

	xp = (xmsg_t *)mcmsg_validate_line(mt, addr);
	if (xp != 0) {
		si = xp->si;
		if ((unsigned long)si >= (unsigned long)mcmsg_memory &&
		    (unsigned long)si <
			(unsigned long)mcmsg_memory +  MMSIZE &&
		    ((unsigned long)si & 0x3) == 0 &&
		    (unsigned long)si->nxrq.request == addr) {
			mcmsg_db_item(si, have_addr, count, modif);
			return;
		}
	}

	if (np != 0) {
		xp = (xmsg_t *)mcmsg_validate_line(mt, np->xmsg);
		db_printf("  nxreq          0x%08x", np);
		if (xp != 0) {
			db_printf("  xmsg           0x%08x", xp);
		}
		db_printf("\n");

		db_printf("   state         %10s",
		    np->state < NNSTATES? nstates[np->state] : "????????? ");
		if (xp != 0) {
			db_printf("   state         %10s",
		    xp->state < XNSTATES? xstates[xp->state] : "????????? ");
		}
		db_printf("\n");

		db_printf("   link          0x%08x", np->link);
		if (xp != 0) {
			db_printf("   link          0x%08x", xp->link);
		}
		db_printf("\n");

		db_printf("   req           0x%08x", np->req);
		if (xp != 0) {
			db_printf("   size          %10u", xp->size);
		}
		db_printf("\n");

		db_printf("   info          0x%08x", np->info);
		if (xp != 0) {
			db_printf("   si            0x%08x", xp->si);
		}
		db_printf("\n");

		db_printf("   xmsg          0x%08x", np->xmsg);
		db_printf("\n");

		db_printf("   type          %10d", np->type);
		if (xp != 0) {
			db_printf("   msg_type      %10d", xp->msg_type);
		}
		db_printf("\n");

		db_printf("   bcount        %10d", np->bcount);
		db_printf("\n");

		db_printf("   buf           0x%08x", np->buf);
		if (xp != 0) {
			db_printf("   buf           0x%08x",
			    (unsigned long)(np->xmsg) + sizeof(xmsg_t));
		}
		db_printf("\n");

		db_printf("   bsize         %10d", np->bsize);
		if (xp != 0) {
			db_printf("   length        %10u", xp->length);
		}
		db_printf("\n");

		db_printf("   boffset       %10d", np->boffset);
		db_printf("\n");

		db_printf("   node          %10d", np->node);
		db_printf("\n");

		db_printf("   ptype         %10d", np->ptype);
		if (xp != 0) {
			db_printf("   dest_ptype    %10d", xp->dest_ptype);
		}
		db_printf("\n");

		db_printf("   localinfo 0   %10d", np->localinfo[0]);
		db_printf("\n");

		db_printf("   localinfo 1   %10d", np->localinfo[1]);
		db_printf("\n");

		db_printf("   localinfo 2   %10d", np->localinfo[2]);
		if (xp != 0) {
			db_printf("   source_node   %10d", xp->source_node);
		}
		db_printf("\n");

		db_printf("   localinfo 3   %10d", np->localinfo[3]);
		if (xp != 0) {
			db_printf("   source_ptype  %10d", xp->source_ptype);
		}
		db_printf("\n");
	}
}
#endif NX


extern unsigned int db_maxoff;
extern unsigned int db_maxval;
extern unsigned int db_minval;

mcmsg_printsym(off, strategy)
        db_expr_t       off;
        db_strategy_t   strategy;
{
	unsigned long   d;
        char            *name;
        unsigned long   value;
        db_sym_t        cursym;

        if (off >= db_maxval || off < db_minval) {
                db_printf("%#n", off);
                return;
        }
        cursym = db_search_task_symbol(off, strategy, &d, TASK_NULL);

        db_symbol_values(cursym, &name, &value);
        if (name == 0 || d >= db_maxoff || value == 0) {
                db_printf("%x", off);
                return;
        }
        db_printf("%s", name);
        if (d)
                db_printf("+0x%x", d);

}

void
mcmsg_db_postpage(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	int i,j,k, method, page, cnt;
        register post_page_t    *upp;
	register long *argpp;

	if ((modif != 0) & (*modif == 'p')) {
	    char *p = modif + 1;

	    page = 0;
	    while (*p >= '0' && *p <= '9') {
	    	page = page*10 + *p++ - '0';
	    }
	    if (page >= POST_PAGE_SLOTS) {
                db_printf("page too large\n");
    	        return;
            }
	    db_printf("mcmsg_kernel_post_page %d\n",page);
	    if ((cnt = count) < 1) cnt = 1;
	    for (i = 0; i < cnt; i++) {
	        method = mcmsg_kernel_post_page[page + i].method;
#if	MSG_TRACE
	        db_printf("%s ",post_name_list[method]);
#endif
	        mcmsg_printsym(mcmsg_post_switch[method],DB_STGY_ANY);
#if	MSG_TRACE
	        if (post_args_list[method] > 0) {
#endif
	            argpp = &mcmsg_kernel_post_page[i].arg0;
	            db_printf("(");
	            db_printf("%x",*argpp);
#if	MSG_TRACE
	            for(k = 1; k < post_args_list[method]; k++) {
#else 
	            for(k = 1; k < 8; k++) {
#endif
	                db_printf(", %x",*argpp++);
	            }
	            db_printf(")");
#if	MSG_TRACE
	        }
#endif
		db_printf("\n");
	    }
	    return;
	}

	db_printf("mcmsg_kernel_post_page %x in %d, out %d\n",
		mcmsg_kernel_post_page, mcmsg_kernel_post_page_in,
			mcmsg_kernel_post_page_out);
	if ((cnt = count) <= 1) {
	    cnt = mcmsg_kernel_post_page_in + 3 - mcmsg_kernel_post_page_out;
	}
	for(i = mcmsg_kernel_post_page_out; 
	      (i < POST_PAGE_SLOTS) & (i < (cnt + mcmsg_kernel_post_page_out)); 
	       i++) {
	    method = mcmsg_kernel_post_page[i].method;
	    if (i == mcmsg_kernel_post_page_out) {
	        db_printf("out -> ");
	    } else {
	        db_printf("       ");
	    }
#if     MSG_TRACE
	    db_printf("%s ",post_name_list[method]);
#endif
	    mcmsg_printsym(mcmsg_post_switch[method],DB_STGY_ANY);
#if     MSG_TRACE
	    if (post_args_list[method] > 0) {
#endif
		argpp = &mcmsg_kernel_post_page[i].arg0;
	        db_printf("(");
	        db_printf("%x",*argpp);
#if     MSG_TRACE
	        for(k = 1; k < post_args_list[method]; k++) {
#else
                    for(k = 1; k < 8; k++) {
#endif
	            db_printf(", %x",*argpp++);
	        }
	    db_printf(")");
#if     MSG_TRACE
	    }
#endif
	    db_printf("\n");
	}

	db_printf("\nmcmsg_user_post_page %x\n", mcmsg_user_post_page);
	if (mcmsg_user_post_tasks == 0) {
	    db_printf("No user tasks\n");
	} else {
	    for (j = 0; j < mcmsg_user_post_tasks; j++) {
 	        db_printf("task %d %x  out %d\n", j, mcmsg_user_mcmsg_task[j],
	    	     mcmsg_user_post_page_out[j]);
		if ((cnt = count) <= 0) {
	           cnt =  5;
		}
	        for(i = mcmsg_user_post_page_out[j]; 
	                 (i < POST_PAGE_SLOTS) & 
	                 (i < mcmsg_user_post_page_out[j] + cnt);
		         i++) {
	            upp = &mcmsg_user_post_page[j][i];
		    if (i == mcmsg_user_post_page_out[j]) {
	                db_printf("out -> ");
		    } else {
	                db_printf("       ");
		    }
#if     MSG_TRACE
	            db_printf("%s ",post_name_list[upp->method]);
#endif
	            mcmsg_printsym(mcmsg_post_switch[upp->method],DB_STGY_ANY);
#if     MSG_TRACE
	            if (post_args_list[upp->method] > 0) {
#endif
		        argpp = &(upp->arg0);
	                db_printf("(");
	                db_printf("%x",*argpp);
#if     MSG_TRACE
	                for(k = 1; k < post_args_list[upp->method]; k++) {
#else
                        for(k = 1; k < 8; k++) {
#endif
	                    db_printf(", %x",*argpp++);
	                }
	                db_printf(")");
#if     MSG_TRACE
	            }
#endif
	            db_printf("\n");
	        }
	    }
	}
}

extern int mcmsg_regs[];

void
mcmsg_db_mcpregs(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	db_printf("mcmsg_regs 0x%x\n",mcmsg_regs);

	db_printf("	Message Co-Processor registers\n");
	db_printf("r0   0x%-8x	r1   0x%-8x\n",mcmsg_regs[0],mcmsg_regs[1]);
	db_printf("sp   0x%-8x	fp   0x%-8x\n",mcmsg_regs[2],mcmsg_regs[3]);
	db_printf("r4   0x%-8x	r5   0x%-8x\n",mcmsg_regs[4],mcmsg_regs[5]);
	db_printf("r6   0x%-8x	r7   0x%-8x\n",mcmsg_regs[6],mcmsg_regs[7]);
	db_printf("r8   0x%-8x	r9   0x%-8x\n",mcmsg_regs[8],mcmsg_regs[9]);
	db_printf("r10  0x%-8x	r11  0x%-8x\n",mcmsg_regs[10],mcmsg_regs[11]);
	db_printf("r12  0x%-8x	r13  0x%-8x\n",mcmsg_regs[12],mcmsg_regs[13]);
	db_printf("r14  0x%-8x	r15  0x%-8x\n",mcmsg_regs[14],mcmsg_regs[15]);
	db_printf("r16  0x%-8x	r17  0x%-8x\n",mcmsg_regs[16],mcmsg_regs[17]);
	db_printf("r18  0x%-8x	r19  0x%-8x\n",mcmsg_regs[18],mcmsg_regs[19]);
	db_printf("r20  0x%-8x	r21  0x%-8x\n",mcmsg_regs[20],mcmsg_regs[21]);
	db_printf("r22  0x%-8x	r23  0x%-8x\n",mcmsg_regs[22],mcmsg_regs[23]);
	db_printf("r24  0x%-8x	r25  0x%-8x\n",mcmsg_regs[24],mcmsg_regs[25]);
	db_printf("r26  0x%-8x	r27  0x%-8x\n",mcmsg_regs[26],mcmsg_regs[27]);
	db_printf("r28  0x%-8x	r29  0x%-8x\n",mcmsg_regs[28],mcmsg_regs[29]);
	db_printf("r30  0x%-8x	r31  0x%-8x\n",mcmsg_regs[30],mcmsg_regs[31]);

}


void
mcmsg_db_mcptrace(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	char		nomodif[1];

	nomodif[0] = 0;

	if (mcmsg_regs[3] == 0) {
		db_printf("No saved registers, wait for a trap or assert\n");
		return;
	}

	db_stack_trace_cmd(mcmsg_regs[3], TRUE, count, nomodif);
}

#endif	DB_MACHINE_COMMANDS
