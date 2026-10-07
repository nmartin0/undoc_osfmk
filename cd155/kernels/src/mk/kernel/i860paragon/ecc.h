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
 * ECC and Parity control bits, and the bootmagic letters that invoke them.
 *
 * Used by the following control knobs via associated bootmagic strings:
 *
 *	ecc_control_ce, "BOOT_ECC_CONTROL"
 *	parity_control_nic, "BOOT_PARITY_CONTROL_NIC"
 *	parity_control_trap, "BOOT_PARITY_CONTROL_TRAP"
 *	parity_control_intr, "BOOT_PARITY_CONTROL_INTR"
 */

#ifndef	_ECC_H_
#define	_ECC_H_ 1

#define ECC_CONTROL_PRINT_ALL		(1 << 0)	/* p - print all n */
#define ECC_CONTROL_PRINT_2		(1 << 1)	/* 2 - print 2^n */
#define	ECC_CONTROL_RED_LED		(1 << 2)	/* r - red LED on CE */
#define ECC_CONTROL_STOP		(1 << 3)	/* s - gimmeabreak */
#define ECC_CONTROL_DISABLE_INTR	(1 << 5)	/* I - test only */
#define ECC_CONTROL_DISABLE_CORRECTION	(1 << 6)	/* C - test only */
#define	ECC_CONTROL_DISABLE_PRESCRUB	(1 << 7)	/* P - test only */

#endif	/* _ECC_H_ */
