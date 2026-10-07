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
 
#ifndef	_I860_MP_H
#define	_I860_MP_H 1

/* Interprocessor doorbell interrupt reasons (bit numbers) */

#define MP_TLB_FLUSH    0x00
#define MP_CLOCK        0x01
#define MP_KDB          0x02
#define MP_AST          0x03
#define MP_HALT         0x04
#define MP_PANIC        0x05
#define MP_NOP          0x06
#define MP_NETSEND      0x07
#define MP_NETRECV      0x08
#define MP_RPCREQ	0x09
#define MP_RPCREPLY	0x0A
#define MP_SCAN_INPUT	0x0B
#define MP_SCAN_OUTPUT	0x0C
#define MP_FREEZE	0x0D
#define MP_AST_MCMSG	0x0E
#define MP_unused	0x0F
#define MP_RPCDEPART	0x10
#define MP_RDMASEND	0x11
#define MP_RDMARECV	0x12
#define MP_RDMATXF	0x13
#define MP_RDMARXF	0x14
#define MP_CONSLOG_AST	0x15

extern int	cpu_int_word[];
extern int	cpu_interrupt(int dest_cpu);
#define CPU_INT_REASON_SET(bitnum,acpu) i_bit_set(bitnum, &cpu_int_word[acpu])
#define CPU_INT_REASON_CLR(bitnum,acpu) i_bit_clear(bitnum, &cpu_int_word[acpu])

#if	NCPUS > 1
extern void	freeze_other_cpus();
extern void	thaw_other_cpus();
#else	/* NCPUS > 1 */
#define freeze_other_cpus()
#define thaw_other_cpus()
#endif	/* NCPUS > 1 */

#endif	/* _I860_MP_H */
