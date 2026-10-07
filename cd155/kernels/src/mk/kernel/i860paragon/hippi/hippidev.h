/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1992 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: hippidev.h,v $
 * Revision 1.5  1994/11/18  20:43:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/10/05  21:36:58  arlin
 * fixed hippi_intr nesting problems.
 * corrected spl level and added baton
 * support for MP. SPLHIPPI_NOI
 *
 *  Reviewer: Jerrie Coffman, Len Brown
 *  Risk: low
 *  Benefit or PTS #: 11095
 *  Testing: HiPPI IPI-3 Eat, 32 nodes.
 *  Module(s): hippi.c hippidev.h
 *
 * Revision 1.3  1994/06/08  17:00:09  arlin
 * Updated for R1.3 IPI-3 support
 *
 */
/*
 *      File:   hippidev.h
 *      Author: Arlin Davis    
 *              Intel Corporation Supercomputer Systems Division
 *      Date:   10/93
 *
 *      Master Device Driver Definitions for HiPPI expansion board.
 */

#define HIPPI_DEV_SRAM	0	/* 128KB static RAM */
#define HIPPI_DEV_FLASH	1	/* 256KB flash EEPROM */
#define	NHIPPIDEVS	2

#define HIPPI_SRAM_START       0x82000000
#define HIPPI_FLASH_START      0x83000000

/* changed to support FAB3 HiPPI boards */
#define HIPPI_SRAM_SIZE        (128*1024)       /* 128KB */
#define HIPPI_FLASH_SIZE       (256*1024)       /* 256KB */

/* 6 byte ULA location always located at last 8 bytes of FLASH */
#define HIPPI_FLASH_ULA_ADDR   ((HIPPI_FLASH_START + HIPPI_FLASH_SIZE) - 8)

#define HIPPI_I960_NMI          (0x800001A0)	/* reset controller, Write only */
#define HIPPI_INTR_STATUS       (0x800000A0)	/* interrupt status register, R/W */
#define	HIPPI_PERF_COUNTERS_BASE_ADDR (0x90000000) /* performance counters */

#define HIPPI_SRC_INTR          0x00000001     /* SRC channel interrupt */
#define HIPPI_DST_INTR          0x00000002     /* DST channel interrupt */
#define HIPPI_CTLR_INTR         0x00000004     /* Ctlr interrupt  */

/* priority level of HiPPI drivers */
#define SPLHIPPI  splnet
#define SPLHIPPI_NOI  splnet_noi

