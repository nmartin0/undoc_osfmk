//
// 
// $Copyright
// Copyright 1991 , 1994, 1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
// 
 
//
// Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
// Santa Clara, California.
// 
//                          All Rights Reserved
// 
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose and without fee is hereby granted,
// provided that the above copyright notice appears in all copies and that
// both the copyright notice and this permission notice appear in
// supporting documentation, and that the name of Intel not be used in
// advertising or publicity pertaining to distribution of the software
// without specific, written prior permission.
// 
// INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
// ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
// SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
// ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
// THIS SOFTWARE.
// 

//
// msgp_asm.h
//
// Defines for message passing assembly language
//
// $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_asm.h,v 1.11 1994/11/18 20:46:31 mtm Exp $
// 
#include <mach_assert.h>

	.file "msgp_dispatch.s"

#if	BIGPKTS && BUMPERS
/* Big Packets + Bumpers not supported in ASM */
#define HANDCODE	0
#else	BIGPKTS && BUMPERS
#define	HANDCODE	!MACH_ASSERT
#endif	BIGPKTS && BUMPERS

#ifdef  BUMPERS
#define BUMPERS 1
#else   BUMPERS
#define BUMPERS 0
#endif  BUMPERS

#ifdef  BIGPKTS
#define BURST   0
#else   BIGPKTS
#define BURST   1
#endif  BIGPKTS

#define TRACE_CALL	0



POST_MAX	=	128
POST_SLOTS	=	64
POST_SHIFT	=	6
EPSR_INT	=	0x00020000

MCTRL_BUMP	=	4
MCTRL_END	=	128

MCTRL_NXC	=	21

hw_recv_status	=	0
hw_send_int_enable =	4
hw_recv_enable	=	8
hw_send_enable	=	16
hw_bump_hdr	=	24
hw_tx_stop	=	32
hw_tx_go	=	40
hw_send_loop	=	48

MSG_PAGE_SIZE	=	8192
FIFO_ALIGN	=	8
LTU_ALIGN	=	32
LTU_MIN		=	64

NIC		=	0xFFFF0000
EOD		=	0xFFFF1000

nic_set		=	0
nic_clear	=	8
nic_control	=	16
nic_status	=	24
nic_io		=	32
nic_fifo_offset	=	40
nic_reset_test	=	56
nic_eod		=	0x1000

NIC_RX_LOW	=	0x200
NIC_RX_1	=	0x100
NIC_RX_2	=	0x080
NIC_ERRORS	=	0x1FFFC000
NIC_TX_NOT_ALMOST_FULL	=	0x2
NIC_TX_ALMOST	=	0x4
NIC_TX_EMPTY	=	0x8
#if	BUMPERS
NIC_STAT_MASK	=	NIC_ERRORS + NIC_RX_1 + NIC_RX_2
NIC_STAT_OK	=	NIC_RX_1 + NIC_RX_2
#else	BUMPERS
NIC_STAT_MASK	=	NIC_ERRORS + NIC_RX_1
NIC_STAT_OK	=	NIC_RX_1
#endif	BUMPERS
NIC_TX_STOP	=	0x11
NIC_TX_GO	=	0x10

post_method	=	0
post_arg0	=	4
post_arg1	=	8
post_arg2	=	12
post_arg3	=	16
post_arg4	=	20
post_arg5	=	24
post_arg6	=	28
post_arg7	=	32
post_arg8	=	36
post_arg9	=	40
post_mt		=	56
post_status	=	60


// Selector and select item declarations

SELMETH_APP		= MCTRL_END + 1
SELMETH_PID_TASK	= MCTRL_END + 2
SELMETH_PID_SEL		= MCTRL_END + 3
SELMETH_PID		= MCTRL_END + 4
SELMETH_PTYPE		= MCTRL_END + 5
SELMETH_TASK		= MCTRL_END + 6
SELMETH_SEQ		= MCTRL_END + 7
SELMETH_XMSG		= MCTRL_END + 8
SELMETH_RECV_TYPESEL	= MCTRL_END + 9
SELMETH_RECV_NODESEL	= MCTRL_END + 10
SELMETH_RECV_ANY	= MCTRL_END + 11
SELMETH_RECV_SRC	= MCTRL_END + 12
SELMETH_RECV_TYPESET	= MCTRL_END + 13
SELMETH_RECV_TYPE	= MCTRL_END + 14
SELMETH_RECV_TYPESRC	= MCTRL_END + 15
SELMETH_RECV_XMSG	= MCTRL_END + 16
SELMETH_TASK_PTYPE	= MCTRL_END + 17
SELMETH_NODE_PTYPE	= MCTRL_END + 18

#define SELECT_HASH_LEN	64

sel_fail	= 0
sel_method	= 4
sel_zero	= 8
sel_hash	= 12

si_link		= 0
si_method	= 4
si_nextmethod	= 6
si_value	= 8
si_item		= 12
si_mcmsg_task	= 16

si_nxrq_req	= 20
si_nxrq_xmsg	= 24
si_nxrq_pidsi	= 28
si_nxrq_seqlink	= 32
si_nxrq_orgnode	= 36
si_nxrq_dptype	= 40
si_nxrq_sptype	= 44
si_nxrq_seq	= 48
si_nxrq_dnode	= 50
si_nxrq_buf	= 52
si_nxrq_count	= 56
si_nxrq_take	= 60
si_nxrq_msgtype	= 64
si_nxrq_stop	= 68
si_nxrq_offset	= 72
si_nxrq_vmast	= 76

si_ppid_node	= 20
si_ppid_flags	= 24
si_ppid_route	= 28
si_ppid_savail	= 32
si_ppid_rgive	= 36
si_ppid_rtotal	= 40
si_ppid_rtarget	= 44
si_ppid_rtaken	= 48
si_ppid_swait	= 52
si_ppid_rkrwant	= 56
si_ppid_rkrseq	= 60
si_ppid_rkrpid	= 64
si_ppid_rkrtype	= 68
si_ppid_rkrptype= 72
si_ppid_rkrorg	= 76
si_ppid_rkrlink	= 80
si_ppid_avllink	= 84

// Mcmsg_task delcarations

mt_link		= 0
mt_method	= 4
mt_pid		= 8
mt_task		= 12
mt_dirbase	= 16
mt_ptypelist	= 20
mt_nodeptypelist	= 24
mt_pid_sel	= 28
mt_node		= 32
mt_numnodes	= 36
mt_selpath	= 40
mt_xmsg_rdy	= 44
mt_xmsg_head	= 48
mt_xmsg_tail	= 52
mt_wired_start	= 56
mt_wired_end	= 60
mt_provided	= 64
mt_assigned	= 68
mt_assign_tgt	= 72
mt_nxport	= 76
mt_masktrap	= 80
mt_appl_app	= 84
mt_appl_plk	= 88
mt_appl_pkt	= 92
mt_appl_membuf	= 96
mt_appl_memexp	= 100
mt_appl_memeach	= 104
mt_appl_sthresh	= 108
mt_appl_scount	= 112
mt_appl_gthresh	= 116
mt_availneed	= 120
mt_rkrecvneed	= 124
mt_swaitunk	= 128

// Xmsg declarations

xmsg_size	= 64
