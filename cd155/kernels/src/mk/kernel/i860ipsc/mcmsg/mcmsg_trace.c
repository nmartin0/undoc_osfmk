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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_trace.c,v 1.27 1994/11/18 20:42:01 mtm Exp $
 */

/*
 * mcmsg_trace.c
 *
 * Tracing support for debugging
 */

#include <ddb/db_command.h>
#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_nx.h>
#include <i860ipsc/mcmsg/mcmsg_hw.h>

#if	MSG_TRACE

struct {
	unsigned short	t_id;
	short		extra;
	unsigned long	hdr1;
	unsigned long	hdr2;
	unsigned long	hdr3;
	unsigned long	extra1;
	unsigned long	extra2;
	unsigned long	hwtime[2];
} mcmsg_trace_buf[NTRACE];

/*
	union {
		struct {
			unsigned short	control;
			unsigned short	give;
		} msg;
		unsigned long hdr;
	} u1;
	union {
		struct {
			unsigned short	length;
			unsigned short	sequence;
		} msg;
		unsigned long hdr;
	} u2;
	union {
		struct {
			long		source_pid;
		} msg;
		unsigned long hdr;
	} u3;
*/

#define t_hdr1		hdr1
#define t_hdr2		hdr2
#define t_hdr3		hdr3
#define t_control	hdr1 & 0xFFFF
#define t_give		hdr1 >> 16
#define t_length	hdr2 & 0xFFFF
#define t_sequence	hdr2 >> 16
#define t_source_pid	hdr3

#define TRACE_SEND	1
#define	TRACE_RECV	2
#define TRACE_PROVIDE	3
#define TRACE_DROP	4
#define TRACE_DEBUG	5
#define TRACE_TIME	6

unsigned long	mcmsg_trace_index = 0;
unsigned long	mcmsg_trace_count = 0;

char	*trace_id_name[] = {
	"**null**",
	"send    ",
	"receive ",
	"provide ",
	"lose    ",
	"debug   ",
	"time    ",
};

#if	PARAGON860

/*
 * Packet.h has macros for every packet type.
 * Here we use it to fill in the packet names.
 */

#define PACKET_DEFINE(n, control, sendmeth, name, sendname, send, recv) name,
#define PACKET_NULL(x)	"**null**",
#define PACKET_FILL(x)	"***ng***",
#define PACKET_END(x)

char	*control_name[] = {
#include "mcmsg_packet.h"
};

#else	PARAGON860

char	*control_name[] = {
	"**null**",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"IPC     ",
	"***ng***",
	"PNQ     ",
	"PNQL    ",
	"PAK     ",
	"PAKL    ",
	"PNA     ",
	"PRM     ",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"RK1     ",
	"RKN     ",
	"RKS     ",
	"RKR     ",
	"RKA     ",
	"NXS     ",
	"NXM     ",
	"NXQ     ",
	"NXC     ",
	"NXF     ",
	"NX1     ",
	"NXN     ",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"CONW    ",
	"CONT    ",
	"CONR    ",
	"***ng***",
	"BT0     ",
	"BT1     ",
	"BT2     ",
	"BT3     ",
	"***ng***",
};

#endif	PARAGON860

#endif MSG_TRACE

#if	PARAGON860


/*
 * Packet.h has macros for every packet type.
 * Here we use it to fill in the send method names.
 * XXX For R1.1 all send methods should be replaced with control values,
 *     for instance, SENDMETH_NX1 should be replaced with MCTRL_NX1 everywhere.
 */

#define PACKET_DEFINE(n, c, sendmeth, name, sendname, send, recv) sendname,
#define PACKET_NULL(x)	"SENDMETH_NULL",
#define PACKET_FILL(x)
#define PACKET_END(x)

char *method_name_list[] = {

#include "mcmsg_packet.h"
	"SENDMETH_RK1A",
	"SELMETH_APP",
	"SELMETH_PID_TASK",
	"SELMETH_PID_SEL",
	"SELMETH_PID",
	"SELMETH_PTYPE",
	"SELMETH_TASK",
	"SELMETH_SEQ",
	"SELMETH_XMSG",
	"SELMETH_RECV_TYPESEL",
	"SELMETH_RECV_NODESEL",
	"SELMETH_RECV_ANY",
	"SELMETH_RECV_SRC",
	"SELMETH_RECV_TYPESET",
	"SELMETH_RECV_TYPE",
	"SELMETH_RECV_TYPESRC",
	"SELMETH_RECV_XMSG",
	"SELMETH_TASK_PTYPE",
	"SELMETH_NODE_PTYPE",
};

#else	PARAGON860

char *method_name_list[] = {
	"**null**",
	"SELMETH_APP",
	"SELMETH_PID_TASK",
	"SELMETH_PID_SEL",
	"SELMETH_PID",
	"SELMETH_PTYPE",
	"SELMETH_TASK",
	"SELMETH_SEQ",
	"SELMETH_XMSG",
	"SELMETH_RECV_TYPESEL",
	"SELMETH_RECV_NODESEL",
	"SELMETH_RECV_ANY",
	"SELMETH_RECV_SRC",
	"SELMETH_RECV_TYPESET",
	"SELMETH_RECV_TYPE",
	"SELMETH_RECV_TYPESRC",
	"SELMETH_RECV_XMSG",
	"SELMETH_TASK_PTYPE",
	"SELMETH_NODE_PTYPE",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",
	"***ng***",

	"SENDMETH_NULL",
	"SENDMETH_PNQ",
	"SENDMETH_PNA",
	"SENDMETH_PAK",
	"SENDMETH_RK1",
	"SENDMETH_RKN",
	"SENDMETH_RKS",
	"SENDMETH_RKR",
	"SENDMETH_RK1A",
	"SENDMETH_RKA",
	"SENDMETH_NXS",
	"SENDMETH_NXM",
	"SENDMETH_NX1",
	"SENDMETH_NXF",
	"SENDMETH_NXN",
	"SENDMETH_NXQ",
	"SENDMETH_NXC",
	"SENDMETH_BOOT",
	"SENDMETH_CONW",
	"SENDMETH_CONT",
	"SENDMETH_CONR",
	"SENDMETH_PRM",
};

#endif	PARAGON860

mcmsg_hwclock()
{

#if	iPSC860 || PARAGON860
asm("counter_reg	=	0xFFFF4630"); /* Works on RX or GP node */
asm("		orh	ha%counter_reg, r0, r31	// Set up register address");
asm("		fld.d	l%counter_reg(r31), f16	// Read counter into f16-17");
asm("		fst.l	f16, 0(r16)		// Store at pointer as two");
asm("		fst.l	f17, 4(r16)		//  longs to avoid traps");
#endif	iPSC860 || PARAGON860
}

static char *method_name(m)
	int	m;
{

	if (m < sizeof(method_name_list)/sizeof(method_name_list[0])) {
		return method_name_list[m];
	} else if (m == 0xDEAD) {
		return "**free**";
	} else {
		return "***ng***";
	}
}

#if MSG_TRACE
mcmsg_trace_gen(id, hdr1, hdr2, hdr3, extra, extra1, extra2)
	int		id;
	unsigned long	hdr1;
	unsigned long	hdr2;
	unsigned long	hdr3;
	int		extra;
	unsigned long	extra1;
	unsigned long	extra2;
{
	register	p1;
	register	p2;

#if	NODUPS
	p1 = mcmsg_trace_index;
	if (p1 == 0) {
		p1 = NTRACE-1;
	} else {
		p1 -= 1;
	}
	if (mcmsg_trace_buf[p1].t_id == id &&
	    mcmsg_trace_buf[p1].t_hdr1 == hdr1 &&
	    mcmsg_trace_buf[p1].t_hdr2 == hdr2 &&
	    mcmsg_trace_buf[p1].t_hdr3 == hdr3) {
		p2 = p1;
		if (p2 == 0) {
			p2 = NTRACE-1;
		} else {
			p2 -= 1;
		}
		if (mcmsg_trace_buf[p2].t_id == id &&
		    mcmsg_trace_buf[p2].t_hdr1 == hdr1 &&
		    mcmsg_trace_buf[p2].t_hdr2 == hdr2 &&
		    mcmsg_trace_buf[p2].t_hdr3 == hdr3) {
			mcmsg_hwclock(mcmsg_trace_buf[p1].hwtime);
			mcmsg_trace_buf[p1].extra = extra;
			mcmsg_trace_buf[p1].extra1 = extra1;
			mcmsg_trace_buf[p1].extra2 = extra2;
			return;
		}
	}
#endif	NODUPS
	mcmsg_hwclock(mcmsg_trace_buf[mcmsg_trace_index].hwtime);
	mcmsg_trace_buf[mcmsg_trace_index].t_id = id;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr1 = hdr1;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr2 = hdr2;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr3 = hdr3;
	mcmsg_trace_buf[mcmsg_trace_index].extra = extra;
	mcmsg_trace_buf[mcmsg_trace_index].extra1 = extra1;
	mcmsg_trace_buf[mcmsg_trace_index].extra2 = extra2;
	mcmsg_trace_index++;
	mcmsg_trace_count++;
	if (mcmsg_trace_index == NTRACE) {
		mcmsg_trace_index = 0;
	}
}

mcmsg_trace_send(hdr1, hdr2, hdr3, extra, extra1, extra2)
	unsigned long	hdr1;
	unsigned long	hdr2;
	unsigned long	hdr3;
	int		extra;
	unsigned long	extra1;
	unsigned long	extra2;
{

	mcmsg_trace_gen(TRACE_SEND, hdr1, hdr2, hdr3, extra, extra1, extra2);
}

mcmsg_trace_recv(hdr1, hdr2, hdr3, extra, extra1, extra2)
	unsigned long	hdr1;
	unsigned long	hdr2;
	unsigned long	hdr3;
	int		extra;
	unsigned long	extra1;
	unsigned long	extra2;
{

	mcmsg_trace_gen(TRACE_RECV, hdr1, hdr2, hdr3, extra, extra1, extra2);
}

mcmsg_trace_provide(xmsg, xp)
	xmsg_t		*xmsg;
	xmsg_t		*xp;
{

	mcmsg_hwclock(mcmsg_trace_buf[mcmsg_trace_index].hwtime);
	mcmsg_trace_buf[mcmsg_trace_index].t_id = TRACE_PROVIDE;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr1 = (unsigned long)xmsg;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr2 = (unsigned long)xp;
	if (xp != 0) {
		mcmsg_trace_buf[mcmsg_trace_index].t_hdr3 = xp->size;
	} else {
		mcmsg_trace_buf[mcmsg_trace_index].t_hdr3 = 0;
	}
	mcmsg_trace_buf[mcmsg_trace_index].extra = 0;
	mcmsg_trace_index++;
	mcmsg_trace_count++;
	if (mcmsg_trace_index == NTRACE) {
		mcmsg_trace_index = 0;
	}
}

mcmsg_trace_drop(s, v)
	char		*s;
	unsigned long	v;
{

	mcmsg_hwclock(mcmsg_trace_buf[mcmsg_trace_index].hwtime);
	mcmsg_trace_buf[mcmsg_trace_index].t_id = TRACE_DROP;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr1 = (unsigned long)s;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr2 = (unsigned long)v;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr3 = 0;
	mcmsg_trace_buf[mcmsg_trace_index].extra = 0;
	mcmsg_trace_index++;
	mcmsg_trace_count++;
	if (mcmsg_trace_index == NTRACE) {
		mcmsg_trace_index = 0;
	}
}

mcmsg_trace_debug(s, n, v1, v2, v3, v4)
	char		*s;
	unsigned long	n;
	unsigned long	v1;
	unsigned long	v2;
	unsigned long	v3;
	unsigned long	v4;
{

	mcmsg_hwclock(mcmsg_trace_buf[mcmsg_trace_index].hwtime);
	mcmsg_trace_buf[mcmsg_trace_index].t_id = TRACE_DEBUG;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr1 = (unsigned long)s;
	mcmsg_trace_buf[mcmsg_trace_index].extra = n;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr2 = v1;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr3 = v2;
	mcmsg_trace_buf[mcmsg_trace_index].extra1 = v3;
	mcmsg_trace_buf[mcmsg_trace_index].extra2 = v4;
	mcmsg_trace_index++;
	mcmsg_trace_count++;
	if (mcmsg_trace_index == NTRACE) {
		mcmsg_trace_index = 0;
	}
}

mcmsg_trace_time(s)
	char		*s;
{

	mcmsg_hwclock(mcmsg_trace_buf[mcmsg_trace_index].hwtime);
	mcmsg_trace_buf[mcmsg_trace_index].t_id = TRACE_TIME;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr1 = (unsigned long)s;
	mcmsg_trace_buf[mcmsg_trace_index].extra = 0;
	mcmsg_trace_index++;
	mcmsg_trace_count++;
	if (mcmsg_trace_index == NTRACE) {
		mcmsg_trace_index = 0;
	}
}

#endif	MSG_TRACE

#ifdef	DB_MACHINE_COMMANDS

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
		if ((mcmsg_trace_buf[index].t_control) >=
		    sizeof(control_name)/sizeof(control_name[0])) {
			return 1;
		}
		return 1;
	}
	if (*modif == 'g') {
		if ((mcmsg_trace_buf[index].t_give) != 0 &&
		    mcmsg_trace_buf[index].t_id == TRACE_RECV) {
			return 1;
		}
		if ((mcmsg_trace_buf[index].t_control) == MCTRL_NX1 &&
		    mcmsg_trace_buf[index].t_id == TRACE_SEND) {
			return 1;
		}
		return 0;
	}
	for (p = modif; *p; p++) {
		if (mcmsg_trace_buf[index].t_id >=
		    sizeof(trace_id_name)/sizeof(trace_id_name[0])) {
			continue;
		}
		if (*p == trace_id_name[mcmsg_trace_buf[index].t_id][0]) {
			return 1;
		}
		if (mcmsg_trace_buf[index].t_id != TRACE_SEND &&
		    mcmsg_trace_buf[index].t_id != TRACE_RECV) {
			continue;
		}
		if ((mcmsg_trace_buf[index].t_control) >=
		    sizeof(control_name)/sizeof(control_name[0])) {
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

	tflag = 0;
	if (modif != 0 && *modif == 't') {
		tflag++;
		modif++;
	}
	if (count == -1)
		count = 20;
	if (have_addr) {
		if (addr > mcmsg_trace_count) {
			printf("Too large\n");
			return;
		}
		i = mcmsg_trace_index - (mcmsg_trace_count - addr + 1);
		if (i < 0) {
			i += NTRACE;
		}
		if (i < 0) {
			printf("Too small\n");
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
	printf("%10u:          Control    Give  Length Seq      Source\n",
		mcmsg_trace_count);
	w0 = mcmsg_trace_buf[i].hwtime[0];
	for (; n > 0; c++) {
		if (mcmsg_db_match(i, modif)) {
			if (tflag) {
				w = mcmsg_trace_buf[i].hwtime[0] - w0;
				w0 = mcmsg_trace_buf[i].hwtime[0];
			} else {
				w = c;
			}
			switch (mcmsg_trace_buf[i].t_id) {

			case TRACE_SEND:
			case TRACE_RECV:
				if ((mcmsg_trace_buf[i].t_control) >=
				    sizeof(control_name)/sizeof(control_name[0])) {
					mcmsg_trace_buf[i].hdr1 &= 0xFFFF0000;
				}
				switch (mcmsg_trace_buf[i].extra) {

				case 0:
			printf("%10u. %8s %8s %5u %7u %5u %10u \n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
				    control_name[mcmsg_trace_buf[i].t_control],
					mcmsg_trace_buf[i].t_give,
					mcmsg_trace_buf[i].t_length,
					mcmsg_trace_buf[i].t_sequence,
					mcmsg_trace_buf[i].t_source_pid);
				  break;

				case 1:
			printf("%10u. %8s %8s %5u %7u %5u %10u  %08x\n",
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
			printf("%10u. %8s %8s %5u %7u %5u %10u  %08x %08x\n",
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
			printf("%10u. %8s %8s %5u %7u %5u %10u  %8d\n",
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
				printf("%10u. %7s %08X       %7u          %08X\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1,
					mcmsg_trace_buf[i].t_hdr3,
					mcmsg_trace_buf[i].t_hdr2);
				break;

			case TRACE_DROP:
				printf("%10u. %7s %08X %s\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr2,
					mcmsg_trace_buf[i].t_hdr1);
				break;

			case TRACE_DEBUG:
				switch (mcmsg_trace_buf[i].extra) {

				case 0:
				  printf("%10u. %7s %10s\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1);
				  break;

				case 1:
				  printf("%10u. %7s %10s %08X\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1,
					mcmsg_trace_buf[i].t_hdr2);
				  break;

				case 2:
				  printf("%10u. %7s %10s %08X %08X\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1,
					mcmsg_trace_buf[i].t_hdr2,
					mcmsg_trace_buf[i].t_hdr3);
				  break;

				case 3:
				  printf("%10u. %7s %10s %08X %08X %08X\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1,
					mcmsg_trace_buf[i].t_hdr2,
					mcmsg_trace_buf[i].t_hdr3,
					mcmsg_trace_buf[i].extra1);
				  break;

				case 4:
				  printf("%10u. %7s %10s %08X %08X %08X %08X\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1,
					mcmsg_trace_buf[i].t_hdr2,
					mcmsg_trace_buf[i].t_hdr3,
					mcmsg_trace_buf[i].extra1,
					mcmsg_trace_buf[i].extra2);
				  break;

				default:
				  printf("%10u. %7s %10s %12d\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1,
					mcmsg_trace_buf[i].extra);
				  break;
				}
				break;

			case TRACE_TIME:
				printf("%10u. %7s %s\n",
					w,
					trace_id_name[mcmsg_trace_buf[i].t_id],
					mcmsg_trace_buf[i].t_hdr1);
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

	printf("Message tracing not enabled\n");

#endif	MSG_TRACE
}


#define XNSTATES (sizeof(xstates)/sizeof(xstates[0]))
static
char *xstates[] = {
	"XMSG_FREE ",
	"XMSG_EMPTY",
	"XMSG_FULL ",
	"XMSG_TRASH",
	"XMSG_STOP ",
	"XMSG_CONT ",
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
mcmsg_db_buf(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	xmsg_t		*xp;
	unsigned long	prevaddr;
	unsigned long	did[20];
	int		i;
	int		j;
	int		scanflag;
	unsigned long	total;

	scanflag = (modif != 0 && *modif == 's');
	printf("mcmsg_task %08x\n", mcmsg_task);
	if (mcmsg_task == 0) {
		return;
	}
	if (count == -1 || count > 20)
		count = 20;
	if (!have_addr) {
		printf("head %08x  tail %08x\n",
			mcmsg_task->xmsg_head, mcmsg_task->xmsg_tail);
		addr = (unsigned long)(mcmsg_task->xmsg_head);
	}
	total = 0;
	for (i = 0; i < count; i++) {
		xp = (xmsg_t *)mcmsg_validate_line(addr);
		if (xp == 0) {
			printf("Invalid address\n");
			return;
		}
		printf("%08x: %10s size %08x link %08x len %08x\n",
			addr,
			xp->state < XNSTATES? xstates[xp->state] : "????????? ",
			xp->size, xp->link, xp->length);
		did[i] = addr;
		total += xp->size + sizeof(xmsg_t);

		if (scanflag) {
			addr = addr + sizeof(xmsg_t) + xp->size;
		} else {
			prevaddr = addr;
			addr = (unsigned long)(xp->link);
		}
		if (addr == 0) {
			if (!have_addr &&
			    !scanflag) {
				if (mcmsg_task->xmsg_tail !=
				    (xmsg_t *)prevaddr) {
					printf("damaged tail %08x\n",
						mcmsg_task->xmsg_tail);
				}
				if (mcmsg_task->provided != total) {
					printf("damaged provided %08x\n",
						mcmsg_task->provided);
				}
			}
			break;
		} else if (!have_addr &&
			   !scanflag &&
			   mcmsg_task->xmsg_tail == (xmsg_t *)prevaddr) {
			printf("damaged tail %08x\n", mcmsg_task->xmsg_tail);
		}
		for (j = 0; j < i; j++) {
			if (did[j] == addr) {
				printf("looped back to %08x\n", addr);
				return;
			}
		}
	}
	printf(" total    %10d      %08X\n", i+1, total);
}

void
mcmsg_db_avail(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
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

	if (mcmsg_task == 0) {
		return;
	}
	numnodes = mcmsg_task->numnodes;
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
	printf(
"  Node        Pid   Send avail     Recv target   total   give   avail\n");
	for (i = 0; i < SELECT_HASH_LEN; i++) {
		pid_st = (select_item_t *)(mcmsg_task->pid_sel->hash[i]);
		if (pid_st == 0) {
			continue;
		}
		t = mcmsg_task->numnodes;
		pid_si = pid_st->link;
		for (;;) {
			if (pid_si == 0) {
				printf("Broken link in hash bucket %d\n", i);
				break;
			}
			if (aflag ||
			    pid_si->ppid.send_avail != 0 ||
			    pid_si->ppid.recv_target != 0) {
			printf("%5u. %10u     %7u        %7u %7u %7u %7u\n",
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
				printf("Loop in hash bucket %d\n", i);
				break;
			}
			pid_si = pid_si->link;
		}
	}
	printf(
"                      -------        ------- ------- ------- -------\n");
	printf(" total                %7u        %7u %7u %7u %7u\n",
		send_avail_sum,
		recv_target_sum,
		recv_total_sum,
		recv_give_sum,
		recv_avail_sum);
	if (mcmsg_task != 0) {
		printf(" provided  %7u          assigned %7u\n",
			mcmsg_task->provided, mcmsg_task->assigned);
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
	printf("l2size         size         free    allocated\n");
	tf = 0;
	cf = 0;
	ta = 0;
	ca = 0;
	for (i = 0; i < sizeof(mcmsg_memlist)/sizeof(mcmsg_memlist[0]); i++) {
		n = 0;
		m = mcmsg_memlist[i];
		while (m != 0) {
			n++;
			if (n == 999999) {
				printf("loop at 0x%08x\n", m);
				break;
			}
			m = (unsigned long *)(m[0]);
		}
		printf(" %4d. %12u %12u %12u\n",
			i, 1 << i, n, mcmsg_memalloc[i]);
		cf += n;
		tf += n * (1 << i);
		ca += mcmsg_memalloc[i];
		ta += mcmsg_memalloc[i] * (1 << i);
	}
	printf(" total %12u %12u %12u %12u\n\n", tf, cf, ca, ta);

	printf("counts      success\t      failure\n\n");
	printf(" alloc %12u\t%12u\n",
		mcmsg_memalloc_count - mcmsg_memcfail_count,
		mcmsg_memcfail_count);
	printf(" coll  %12u\t%12u\n",
		mcmsg_memcoll_count - mcmsg_memcfail_count,
		mcmsg_memcfail_count);
	printf(" inlp  %12u\t%12u\n",
		mcmsg_meminlp_count - mcmsg_memifail_count,
		mcmsg_memifail_count);
	printf(" free  %12u\n", mcmsg_memfree_count);
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
		printf(" 0x%08x\n", addr);
	}
	return 0;
}

mcmsg_db_mmema()
{

	mcmsg_db_mmema_search(mcmsg_memory, LOG2MMSIZE, 1);
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
		mt = mcmsg_task;
	} else {
		mt = (mcmsg_task_t *)addr;
	}
	printf("mcmsg_task      0x%08x\n", mt);
	if (mt == 0) {
		return;
	}
	printf(" method                 %s\n", method_name(mt->method));
	printf(" link                   0x%08x\n", mt->link);
	printf(" app                    %10u\n", mt->applinfo.app);
	printf(" pid                    %10d\n", mt->pid);
	printf(" task                   0x%08x\n", mt->task);
	printf(" dirbase                0x%08x\n", mt->dirbase);
	printf(" task_ptype_list        0x%08x\n", mt->task_ptype_list);
	printf(" node_ptype_list        0x%08x\n",
		mt->task_ptype_list->node_ptype_list);
	printf(" pid_sel                0x%08x\n", mt->pid_sel);
	printf(" node                   %10u\n", mt->node);
	printf(" numnodes               %10u\n", mt->numnodes);
	printf(" pkt_size               %10u\n", mt->applinfo.pkt_size);
	printf(" selection_path         0x%08x\n", mt->selection_path);
	printf(" xmsg_head              0x%08x\n", mt->xmsg_head);
	printf(" xmsg_tail              0x%08x\n", mt->xmsg_tail);
	printf(" wired_start            0x%08x\n", mt->wired_start);
	printf(" wired_end              0x%08x\n", mt->wired_end);
	printf(" provided               %10u\n", mt->provided);
	printf(" assigned               %10u\n", mt->assigned);
	printf(" assign_target          %10u\n", mt->assign_target);
	printf(" memory_each            %10u\n", mt->applinfo.memory_each);
	printf(" send_threshold         %10u\n", mt->applinfo.send_threshold);
	printf(" send_amount            %10u\n", mt->applinfo.send_count);
	printf(" give_threshold         %10u\n", mt->applinfo.give_threshold);
	printf(" process_lock           %10u\n", mt->applinfo.process_lock);
	printf(" avail_need             0x%08x\n", mt->avail_need);
	printf(" rk_recv_need           %10u\n", mt->rk_recv_need);
	printf(" send_wait_unk          0x%08x\n", mt->send_wait_unk);
	printf("\n");

	pt = mt->task_ptype_list;
	if (pt == 0) {
		return;
	}
	printf("task_ptype_list\n");
	printf(" method                 %s\n", method_name(pt->method));
	printf(" last_entry             %10u\n", pt->last_entry);
	printf(" app                    %10u\n", pt->app);
	printf(" numnodes               %10u\n", pt->numnodes);
	printf(" phys_node_list         0x%08x\n", pt->phys_node_list);
	printf(" node_ptype_list        0x%08x\n", pt->node_ptype_list);
	printf(" refcount               0x%08x\n", pt->refcount);

    printf(" ptype entries           ptype  mcmsg_task  offset\n");
	for (i = 0; i <= pt->last_entry; i++) {
		printf(" entry %3u          %10d 0x%08x %10u\n",
			i, 
			pt->ptype_entry[i].ptype,
			pt->ptype_entry[i].item,
			pt->ptype_entry[i].offset);
		if (i > 20) {
			printf(" more...\n");
			break;
		}
	}
	printf("\n");

	printf("phys_node_list         pid\n");
	for (i = 0; i <= pt->numnodes && i < 16; i++) {
		printf(" node  %3u          %10d\n", i, pt->phys_node_list[i]);
	}
	printf("\n");

	pt = pt->node_ptype_list;
	if (pt == 0) {
		return;
	}
	printf("node_ptype_list\n");
	printf(" method                 %s\n", method_name(pt->method));
	printf(" last_entry             %10u\n", pt->last_entry);
	printf(" app                    %10u\n", pt->app);
	printf(" numnodes               %10u\n", pt->numnodes);
	printf(" phys_node_list         0x%08x\n", pt->phys_node_list);
	printf(" node_ptype_list        0x%08x\n", pt->node_ptype_list);
	printf(" refcount               0x%08x\n", pt->refcount);

    printf(" ptype entries           ptype  node_list    offset\n");
	for (i = 0; i <= pt->last_entry; i++) {
		printf(" entry %3u          %10d 0x%08x %10u\n",
			i, 
			pt->ptype_entry[i].ptype,
			pt->ptype_entry[i].item,
			pt->ptype_entry[i].offset);
		if (pt->ptype_entry[i].item != 0) {
			for (j = 0; j <= pt->numnodes && j < 16; j++) {
				printf(" node  %3u          %10d\n",
				 j,
				 ((node_list_t *)(pt->ptype_entry[i].item))[j]);
			}
		}
	}
	printf("\n");
}

void
mcmsg_db_pid(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	int		i;
	select_item_t	*si;

	if (mcmsg_task == 0) {
		return;
	}
	si = mcmsg_lookup_remote(addr);
	printf("pid %10u selector 0x%08x\n", addr, si);
	if (si == 0) {
		return;
	}
	printf(" method         %s\n", method_name(si->method));
	printf(" nextmethod     %s\n", method_name(si->nextmethod));
	printf(" link           0x%08x\n", si->link);
	printf(" value          %10u\n", si->value);
	printf(" item           0x%08x\n", si->item);
	printf(" mcmsg_task     0x%08x\n", si->mcmsg_task);
	printf(" node           %10u\n", si->ppid.node);
	printf(" process_lock   %10u\n", si->ppid.process_lock);
	printf(" send_ready     %10u\n", si->ppid.send_ready);
	printf(" route          %10u\n", si->ppid.route);
	printf(" send_avail     %10u\n", si->ppid.send_avail);
	printf(" recv_give      %10u\n", si->ppid.recv_give);
	printf(" recv_total     %10u\n", si->ppid.recv_total);
	printf(" recv_target    %10u\n", si->ppid.recv_target);
	printf(" recv_taken     %10u\n", si->ppid.recv_taken);
	printf(" send_wait      0x%08x\n", si->ppid.send_wait);
	printf(" rk_recv_want   %10u\n", si->ppid.rk_recv_want);
	printf(" rk_recv_seq    %10u\n", si->ppid.rk_recv_seq);
	printf(" rk_recv_pid    %10u\n", si->ppid.rk_recv_pid);
	printf(" rk_recv_type   %10u\n", si->ppid.rk_recv_type);
	printf(" rk_recv_ptype  %10u\n", si->ppid.rk_recv_ptype);
	printf(" rk_recv_link   0x%08x\n", si->ppid.rk_recv_link);
	printf(" avail_link     0x%08x\n", si->ppid.avail_link);
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
	printf("selector 0x%08x\n", sel);
	if (sel == 0) {
		return;
	}
	printf(" method         %s\n", method_name(sel->method));
	printf(" fail           0x%08x\n", sel->fail);
	printf(" zero           0x%08x\n", sel->zero);
	for (i = 0; i < SELECT_HASH_LEN; i++) {
		st = sel->hash[i];
		if (st == 0) {
			continue;
		}
		sh = st->link;
		t = 10;
		si = sh;
		for (;;) {
			printf(" %10d     0x%08x\n", si->value, si);
			if (si == st) {
				break;
			}
			if (si == 0) {
				printf("  broken, giving up\n");
				break;
			}
			si = si->link;
			if (t-- == 0) {
				printf("  10 loops, giving up\n");
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
	int		i;
	select_item_t	*si;
	nxreq_t		*np;
	xmsg_t		*xp;

	si = (select_item_t *)addr;
	if (si == 0) {
		printf("null selector\n");
		return;
	}
	if (si->method == SELMETH_PID) {
		printf("selector        0x%08x\n", si);
		printf(" method         %s\n", method_name(si->method));
		printf(" nextmethod     %s\n", method_name(si->nextmethod));
		printf(" link           0x%08x\n", si->link);
		printf(" value          %10u\n", si->value);
		printf(" item           0x%08x\n", si->item);
		printf(" mcmsg_task     0x%08x\n", si->mcmsg_task);
		printf(" node           %10u\n", si->ppid.node);
		printf(" process_lock   %10u\n", si->ppid.process_lock);
		printf(" route          %10u\n", si->ppid.route);
		printf(" send_avail     %10u\n", si->ppid.send_avail);
		printf(" recv_give      %10u\n", si->ppid.recv_give);
		printf(" recv_total     %10u\n", si->ppid.recv_total);
		printf(" recv_target    %10u\n", si->ppid.recv_target);
		printf(" recv_taken     %10u\n", si->ppid.recv_taken);
		printf(" send_wait      0x%08x\n", si->ppid.send_wait);
		printf(" rk_recv_want   %10u\n", si->ppid.rk_recv_want);
		printf(" rk_recv_seq    %10u\n", si->ppid.rk_recv_seq);
		printf(" rk_recv_pid    %10u\n", si->ppid.rk_recv_pid);
		printf(" rk_recv_type   %10u\n", si->ppid.rk_recv_type);
		printf(" rk_recv_ptype  %10u\n", si->ppid.rk_recv_ptype);
		printf(" rk_recv_link   0x%08x\n", si->ppid.rk_recv_link);
		printf(" avail_link     0x%08x\n", si->ppid.avail_link);
	} else {
		if (mcmsg_task != 0) {
			mcmsg_phys = current_task() != mcmsg_task->task;
			np = (nxreq_t *)mcmsg_validate_line(si->nxrq.request);
			xp = (xmsg_t *)mcmsg_validate_line(si->nxrq.xmsg);
		} else {
			np = 0;
			xp = 0;
		}

		printf("selector       0x%08x", si);
		if (np != 0) {
			printf("  nxreq          0x%08x", np);
		}
		if (xp != 0) {
			printf("  xmsg           0x%08x", xp);
		}
		printf("\n");

		printf(" method %19s", method_name(si->method));
		if (np != 0) {
			printf(" state         %10s  ",
		    np->state < NNSTATES? nstates[np->state] : "????????? ");
		}
		if (xp != 0) {
			printf(" state         %10s",
		    xp->state < XNSTATES? xstates[xp->state] : "????????? ");
		}
		printf("\n");

		printf(" nextmethod %15s", method_name(si->nextmethod));
		printf("\n");

		printf(" link          0x%08x", si->link);
		if (np != 0) {
			printf("   link          0x%08x", np->link);
		}
		if (xp != 0) {
			printf("   link          0x%08x", xp->link);
		}
		printf("\n");

		printf(" value         %10u", si->value);
		if (np != 0) {
			printf("   req           0x%08x", np->req);
		}
		if (xp != 0) {
			printf("   size          %10u", xp->size);
		}
		printf("\n");

		printf(" item          0x%08x", si->item);
		if (np != 0) {
			printf("   info          0x%08x", np->info);
		}
		printf("\n");

		printf(" mcmsg_task    0x%08x", si->mcmsg_task);
		if (np != 0) {
			printf("   incoming      0x%08x", np->incoming);
		}
		printf("\n");

		printf(" request       0x%08x", si->nxrq.request);
		if (np != 0) {
			printf("   update        %10u", np->update);
		}
		printf("\n");

		printf(" xmsg          0x%08x", si->nxrq.xmsg);
		if (np != 0) {
			printf("   xmsg          0x%08x", np->xmsg);
		}
		if (xp != 0) {
			printf("   si            0x%08x", xp->si);
		}
		printf("\n");

		printf(" msg_type      %10d", si->nxrq.msg_type);
		if (np != 0) {
			printf("   type          %10d", np->type);
		}
		if (xp != 0) {
			printf("   msg_type      %10d", xp->msg_type);
		}
		printf("\n");

		printf(" take          %10d", si->nxrq.take);
		if (np != 0) {
			printf("   bcount        %10d", np->bcount);
		}
		printf("\n");

		printf(" buf           0x%08x", si->nxrq.buf);
		if (np != 0) {
			printf("   buf           0x%08x", np->buf);
		}
		if (xp != 0) {
			printf("   buf           0x%08x",
			    (unsigned long)(si->nxrq.xmsg) + sizeof(xmsg_t));
		}
		printf("\n");

		printf(" count         %10d", si->nxrq.count);
		if (np != 0) {
			printf("   bsize         %10d", np->bsize);
		}
		if (xp != 0) {
			printf("   length        %10u", xp->length);
		}
		printf("\n");

		printf(" stop          %10d", si->nxrq.stop);
		if (np != 0) {
			printf("   boffset       %10d", np->boffset);
		}
		printf("\n");

		printf(" offset        %10d", si->nxrq.offset);
		printf("\n");

		printf(" dest_node     %10d", si->nxrq.dest_node);
		if (np != 0) {
			printf("   node          %10d", np->node);
		}
		printf("\n");

		printf(" dest_ptype    %10d", si->nxrq.dest_ptype);
		if (np != 0) {
			printf("   ptype         %10d", np->ptype);
		}
		if (xp != 0) {
			printf("   dest_ptype    %10d", xp->dest_ptype);
		}
		printf("\n");

		printf(" sequence      %10u", si->nxrq.sequence);
		if (np != 0) {
			printf("   localinfo 0   %10d", np->localinfo[0]);
		}
		printf("\n");

		printf(" seq_link      0x%08x", si->nxrq.seq_link);
		if (np != 0) {
			printf("   localinfo 1   %10d", np->localinfo[1]);
		}
		printf("\n");

		printf(" pid_si        0x%08x", si->nxrq.pid_si);
		if (np != 0) {
			printf("   localinfo 2   %10d", np->localinfo[2]);
		}
		if (xp != 0) {
			printf("   source_node   %10d", xp->source_node);
		}
		printf("\n");

		printf(" source_ptype  %10d", si->nxrq.source_ptype);
		if (np != 0) {
			printf("   localinfo 3   %10d", np->localinfo[3]);
		}
		if (xp != 0) {
			printf("   source_ptype  %10d", xp->source_ptype);
		}
		printf("\n");
	}
}

void
mcmsg_db_xmsg(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	int		i;
	xmsg_t		*xp;
	select_item_t	*si;

	if (mcmsg_task == 0) {
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;
	xp = (xmsg_t *)mcmsg_validate_line(addr);

	if (xp == 0) {
		printf("invalid address %08x\n", addr);
		return;
	}

	si = xp->si;
	if ((unsigned long)si >= (unsigned long)mcmsg_memory &&
	    (unsigned long)si <
			(unsigned long)mcmsg_memory + sizeof(mcmsg_memory) &&
	    ((unsigned long)si & 0x3) == 0 &&
	    (unsigned long)si->nxrq.xmsg == addr) {
		mcmsg_db_item(si, have_addr, count, modif);
		return;
	}

	printf("  xmsg           0x%08x\n", xp);
	printf("   state         %10s\n",
	    xp->state < XNSTATES? xstates[xp->state] : "????????? ");
	printf("   link          0x%08x\n", xp->link);
	printf("   size          %10u\n", xp->size);
	printf("   si            0x%08x\n", xp->si);

	if (xp->state == XMSG_STOP || xp->state == XMSG_CONT) {
		printf("   xmsg_offset   0x%08x\n", xp->xmsg_offset);
		printf("   xmsg_data     0x%08x\n", xp->xmsg_data);
		printf("   xmsg_stop     0x%08x\n", xp->xmsg_stop);
	} else {
		printf("   msg_type      %10d\n", xp->msg_type);
		printf("   source_ptype  %10d\n", xp->source_ptype);
		printf("   dest_ptype    %10d\n", xp->dest_ptype);
	}
	printf("   buf           0x%08x\n",
	    (unsigned long)(addr) + sizeof(xmsg_t));
	printf("   length        %10u\n", xp->length);
	printf("   source_node   %10d\n", xp->source_node);
	printf("\n");
}

void
mcmsg_db_nxreq(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	int		i;
	nxreq_t		*np;
	xmsg_t		*xp;
	select_item_t	*si;

	if (mcmsg_task == 0) {
		return;
	}
	mcmsg_phys = current_task() != mcmsg_task->task;

	np = (nxreq_t *)mcmsg_validate_long(addr);
	if (np == 0) {
		printf("invalid address %08x\n", addr);
		return;
	}

	xp = (xmsg_t *)mcmsg_validate_line(addr);
	if (xp != 0) {
		si = xp->si;
		if ((unsigned long)si >= (unsigned long)mcmsg_memory &&
		    (unsigned long)si <
			(unsigned long)mcmsg_memory + sizeof(mcmsg_memory) &&
		    ((unsigned long)si & 0x3) == 0 &&
		    (unsigned long)si->nxrq.request == addr) {
			mcmsg_db_item(si, have_addr, count, modif);
			return;
		}
	}

	if (np != 0) {
		xp = (xmsg_t *)mcmsg_validate_line(np->xmsg);
		printf("  nxreq          0x%08x", np);
		if (xp != 0) {
			printf("  xmsg           0x%08x", xp);
		}
		printf("\n");

		printf("   state         %10s",
		    np->state < NNSTATES? nstates[np->state] : "????????? ");
		if (xp != 0) {
			printf("   state         %10s",
		    xp->state < XNSTATES? xstates[xp->state] : "????????? ");
		}
		printf("\n");

		printf("   link          0x%08x", np->link);
		if (xp != 0) {
			printf("   link          0x%08x", xp->link);
		}
		printf("\n");

		printf("   req           0x%08x", np->req);
		if (xp != 0) {
			printf("   size          %10u", xp->size);
		}
		printf("\n");

		printf("   info          0x%08x", np->info);
		if (xp != 0) {
			printf("   si            0x%08x", xp->si);
		}
		printf("\n");

		printf("   incoming      0x%08x", np->incoming);
		printf("\n");

		printf("   update        %10u", np->update);
		printf("\n");

		printf("   xmsg          0x%08x", np->xmsg);
		printf("\n");

		printf("   type          %10d", np->type);
		if (xp != 0) {
			printf("   msg_type      %10d", xp->msg_type);
		}
		printf("\n");

		printf("   bcount        %10d", np->bcount);
		printf("\n");

		printf("   buf           0x%08x", np->buf);
		if (xp != 0) {
			printf("   buf           0x%08x",
			    (unsigned long)(np->xmsg) + sizeof(xmsg_t));
		}
		printf("\n");

		printf("   bsize         %10d", np->bsize);
		if (xp != 0) {
			printf("   length        %10u", xp->length);
		}
		printf("\n");

		printf("   boffset       %10d", np->boffset);
		printf("\n");

		printf("   node          %10d", np->node);
		printf("\n");

		printf("   ptype         %10d", np->ptype);
		if (xp != 0) {
			printf("   dest_ptype    %10d", xp->dest_ptype);
		}
		printf("\n");

		printf("   localinfo 0   %10d", np->localinfo[0]);
		printf("\n");

		printf("   localinfo 1   %10d", np->localinfo[1]);
		printf("\n");

		printf("   localinfo 2   %10d", np->localinfo[2]);
		if (xp != 0) {
			printf("   source_node   %10d", xp->source_node);
		}
		printf("\n");

		printf("   localinfo 3   %10d", np->localinfo[3]);
		if (xp != 0) {
			printf("   source_ptype  %10d", xp->source_ptype);
		}
		printf("\n");
	}
}

#if	PARAGON860
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
#if	MACH_ASSERT
	nic_reg	t;
	nic_reg	u;
	int	i;
	int	c;
	int	n;
	extern char *mcmsg_nic_trace_name[];
	extern mcmsg_recv_enable;

	if (count == -1)
		count = 20;
	if (have_addr) {
		if (addr > mcmsg_nic_trace_count) {
			printf("Too large\n");
			return;
		}
		i = mcmsg_nic_trace_index - (mcmsg_nic_trace_count - addr) + 1;
		if (i < 0) {
			i += NIC_TRACE_SIZE;
		}
		if (i < 0) {
			printf("Too small\n");
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
		i = mcmsg_nic_trace_index - (mcmsg_nic_trace_count - addr) + 1;
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
		    mcmsg_nic_trace_buf[i].place <= 10) {
			printf("%10u. %s %08X %08X %6u %6u\n",
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
			printf("            NIC status      %08X %08X\n",
				t.halfs.hi, t.halfs.lo);
			u.full = NIC.control.full;
			printf("            NIC control     %08X %08X\n",
				u.halfs.hi, u.halfs.lo);
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

	printf("\nNIC status transmit");
	for (i = 0; i < 4; i++) {
		if (t.halfs.lo & (1 << i)) {
			printf(" %s", nic_status_name[i]);
		}
	}
	printf("\n            receive");
	for (; i < 10; i++) {
		if (t.halfs.lo & (1 << i)) {
			printf(" %s", nic_status_name[i]);
		}
	}
	printf("\n            general");
	for (; i < 14; i++) {
		if (t.halfs.lo & (1 << i)) {
			printf(" %s", nic_status_name[i]);
		}
	}
	printf("\n             errors");
	for (; i < 29; i++) {
		if (t.halfs.lo & (1 << i)) {
			printf(" %s", nic_status_name[i]);
		}
	}
	printf("\n            network");
	for (; i < 32; i++) {
		if (t.halfs.lo & (1 << i)) {
			printf(" %s", nic_status_name[i]);
		}
	}
	for (; i < 35; i++) {
		if (t.halfs.hi & (1 << (i - 32))) {
			printf(" %s", nic_status_name[i]);
		}
	}
	printf("\n");

#else	MACH_ASSERT

	printf("NIC tracing not enabled\n");

#endif	MACH_ASSERT
}
#endif	PARAGON860

#endif	DB_MACHINE_COMMANDS
