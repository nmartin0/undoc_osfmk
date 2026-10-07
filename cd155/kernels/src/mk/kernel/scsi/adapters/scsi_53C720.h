/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */


/*
 * Copyright 1994 by Intel Corporation,
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
 * $Log: scsi_53C720.h,v $
 * Revision 1.3.8.1  1995/06/11  18:34:06  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.3  1995/03/28  01:39:38  jerrie
 * Increased controller interface dual-port RAM queue size.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low.
 *  Benefit or PTS #: 12827
 *  Testing:	   Developer
 *  Module(s):	   See PTS report.
 *
 * Revision 1.2  1995/03/14  23:51:01  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *      File:   scsi_53C720.h
 *      Author: Vineet Kumar & Jerrie Coffman
 *              Intel Corporation Supercomputer Systems Division
 *      Date:   April 25, 1994
 *
 *      Defines for the NCR 53C720 (SCSI I/O Processor) and data
 *      structures shared between the Controller and the Controller
 *      Interface
 *
 *      Created, from the NCR specs:
 *          "NCR 53C720 SCSI I/O Processor Data Manual, Rev 2.1"
 *          NCR Microelectronics Division, Colorado Spring, 7/93
 */

#ifndef _SCSI_53C720_H_
#define _SCSI_53C720_H_


/*
 * Register map
 */
typedef struct {
	volatile unsigned char	siop_scntl0; /* rw: SCSI control reg 0	      */
	volatile unsigned char	siop_scntl1; /* rw: SCSI control reg 1	      */
        volatile unsigned char  siop_scntl2; /* rw: SCSI control reg 2	      */
        volatile unsigned char  siop_scntl3; /* rw: SCSI control reg 3	      */
        volatile unsigned char  siop_scid;   /* rw: SCSI Chip ID reg	      */
        volatile unsigned char  siop_sxfer;  /* rw: SCSI Transfer reg	      */
	volatile unsigned char	siop_sdid;   /* rw: SCSI Destination ID	      */
        volatile unsigned char  siop_gpreg;  /* rw: General Purpose	      */
        volatile unsigned char  siop_sfbr;   /* rw: SCSI First Byte Received  */
        volatile unsigned char  siop_socl;   /* rw: SCSI Output Control Latch */
        volatile unsigned char  siop_ssid;   /* ro: SCSI Selector ID	      */
        volatile unsigned char  siop_sbcl;   /* rw: SCSI Bus Control Lines    */
        volatile unsigned char  siop_dstat;  /* ro: DMA status		      */
        volatile unsigned char  siop_sstat0; /* ro: SCSI status reg 0	      */
        volatile unsigned char  siop_sstat1; /* ro: SCSI status reg 1	      */
        volatile unsigned char  siop_sstat2; /* ro: SCSI status reg 2	      */
        volatile unsigned long  siop_dsa;    /* rw: Data Structure Address    */
        volatile unsigned char  siop_istat;  /* rw: Interrupt Status reg      */
        volatile unsigned char  siop_reserved1;/*   Register map padding      */
        volatile unsigned char  siop_reserved2;/*   Register map padding      */
        volatile unsigned char  siop_reserved3;/*   Register map padding      */
        volatile unsigned char  siop_ctest0; /* rw: Chip test register 0      */
        volatile unsigned char  siop_ctest1; /* ro: Chip test register 1      */
        volatile unsigned char  siop_ctest2; /* ro: Chip test register 2      */
        volatile unsigned char  siop_ctest3; /* ro: Chip test register 3      */
        volatile unsigned long  siop_temp;   /* rw: Temporary Stack reg	      */
        volatile unsigned char  siop_dfifo;  /* rw: DMA FIFO		      */
        volatile unsigned char  siop_ctest4; /* rw: Chip test register 4      */
        volatile unsigned char  siop_ctest5; /* rw: Chip test register 5      */
        volatile unsigned char  siop_ctest6; /* rw: Chip test register 6      */
        volatile unsigned char  siop_dbc0;   /* rw: DMA Byte Counter reg 0    */
        volatile unsigned char  siop_dbc1;   /* rw: DMA Byte Counter reg 1    */
        volatile unsigned char  siop_dbc2;   /* rw: DMA Byte Counter reg 2    */
        volatile unsigned char  siop_dcmd;   /* rw: DMA Command Register      */
        volatile unsigned long  siop_dnad;   /* rw: DMA Next Address for Data */
        volatile unsigned long  siop_dsp;    /* rw: DMA SCRIPTS Pointer	      */
        volatile unsigned long  siop_dsps;   /* rw: DMA SCRIPTS Pointer Save  */
        volatile unsigned long  siop_scratcha;/* rw: Scratch Pad A	      */
        volatile unsigned char  siop_dmode;  /* rw: DMA Mode reg	      */
        volatile unsigned char  siop_dien;   /* rw: DMA Interrupt Enable      */
        volatile unsigned char  siop_dwt;    /* rw: DMA Watchdog Timer	      */
        volatile unsigned char  siop_dcntl;  /* rw: DMA Control reg	      */
        volatile unsigned long  siop_adder;  /* ro: Sum out of internal adder */
	volatile unsigned char	siop_sien0;  /* rw: SCSI Interrupt Enable 0   */
        volatile unsigned char  siop_sien1;  /* rw: SCSI Interrupt Enable 1   */
        volatile unsigned short siop_sist;   /* ro: SCSI Interrupt Status     */
        volatile unsigned char  siop_slpar;  /* rw: SCSI Longitudinal Parity  */
        volatile unsigned char  siop_swide;  /* ro: SCSI Wide Residue Data    */
        volatile unsigned char  siop_macntl; /* rw: Memory Access Control     */
        volatile unsigned char  siop_gpcntl; /* rw: General Purpose Control   */
        volatile unsigned char  siop_stime0; /* rw: SCSI Timer 0	      */
        volatile unsigned char  siop_stime1; /* rw: SCSI Timer 1	      */
        volatile unsigned char  siop_respid0;/* rw: Response ID0	      */
        volatile unsigned char  siop_respid1;/* rw: Response ID1	      */
        volatile unsigned char  siop_stest0; /* ro: SCSI Test 0		      */
        volatile unsigned char  siop_stest1; /* ro: SCSI Test 1		      */
        volatile unsigned char  siop_stest2; /* rw: SCSI Test 2		      */
        volatile unsigned char  siop_stest3; /* rw: SCSI Test 3		      */
        volatile unsigned char  siop_sidl0;  /* ro: SCSI Input Data Latch 0   */
        volatile unsigned char  siop_sidl1;  /* ro: SCSI Input Data Latch 1   */
        volatile unsigned short siop_sidl2;   /* padding */
        volatile unsigned char  siop_sodl0;  /* rw: SCSI Output Data Latch 0  */
        volatile unsigned char  siop_sodl1;  /* rw: SCSI Output Data Latch 1  */
        volatile unsigned short siop_sodl2;   /* padding */
        volatile unsigned char  siop_sbdl0;  /* ro: SCSI Bus Data Lines 0     */
        volatile unsigned char  siop_sbdl1;  /* ro: SCSI Bus Data Lines 1     */
        volatile unsigned short siop_sbdl2;   /* padding */
        volatile unsigned long  siop_scratchb;/* rw: Scratch Pad B	      */
} siop_regmap_t;


/*
 * Register defines
 */

/*
 * SIOP Control Register 0 (scntl0)
 */
#define	SIOP_SCNTL0_ARB_MASK	0xc0	/* Arbitration mode */
#	define	SIOP_ARB_SIMPLE	0x00	/* Try Arbitration Once */
#	define	SIOP_ARB_FULL	0xc0	/* Try Arbitration Until Success */
#define	SIOP_SCNTL0_START	0x20	/* Start Sequence */
#define	SIOP_SCNTL0_WATN	0x10	/* Select With ATN */
#define	SIOP_SCNTL0_EPC		0x08	/* Enable Parity Checking */
#define	SIOP_SCNTL0_EPG		0x04	/* Enable Parity Generation */
#define	SIOP_SCNTL0_AAP		0x02	/* Assert ATN on Parity Error */
#define	SIOP_SCNTL0_TRG		0x01	/* Target Mode */

/*
 * SIOP Control Register 1 (scntl1)
 */
#define	SIOP_SCNTL1_EXC		0x80	/* Extra Clock Cycle of data setup */
#define	SIOP_SCNTL1_ADB		0x40	/* Assert Data Bus */
#define	SIOP_SCNTL1_DHP		0x20	/* Disable Halt on Parity Error */
#define	SIOP_SCNTL1_CON		0x10	/* Connected */
#define	SIOP_SCNTL1_RST		0x08	/* Assert SCSI Bus RST */
#define	SIOP_SCNTL1_AESP	0x04	/* Force Bad Parity */
#define	SIOP_SCNTL1_IARB	0x02	/* Immediate Arbitration */
#define	SIOP_SCNTL1_SST		0x01	/* Start SCSI Transfer */

/*
 * SIOP Control Register 2 (scntl2)
 */
#define SIOP_SCNTL2_MASK        0xc9    /* scntl2 Mask */
#define SIOP_SCNTL2_SDU		0x80	/* SCSI Disconnect Unexpected */
#define SIOP_SCNTL2_CHM		0x40	/* Chained Mode */
#define SIOP_SCNTL2_WSS		0x08	/* Wide SCSI Send */
#define SIOP_SCNTL2_WSR		0x01	/* Wide SCSI Receive */

/*
 * SIOP Control Register 3 (scntl3)
 */
#define SIOP_SCNTL3_MASK        0x7f    /* scntl3 Mask */
#define SIOP_SCNTL3_SCF_MASK	0x70    /* Synchronous Clock Conv. Factor */
#	define SIOP_SCF_SHIFT	4	/* SCF Bit Shift */
#define SIOP_SCNTL3_EWS		0x08    /* Enable Wide SCSI */
#define SIOP_SCNTL3_CCF_MASK	0x07    /* Clock Conversion Factor */
#	define	SIOP_CCF_66MHz	0x0	/* 50.01 MHz to 66 MHz */
#	define	SIOP_CCF_25MHz	0x1	/* 16.67 MHz to 25 MHz */
#	define	SIOP_CCF_37MHz	0x2	/* 25.01 MHz to 37 MHz */
#	define	SIOP_CCF_50MHz	0x3	/* 37.51 MHz to 50 MHz */
#	define	SIOP_CCF_66MHz_	0x4	/* 50.01 MHz to 66 MHz */

/*
 * SIOP Chip ID Register (scid)
 */
#define SIOP_SCID_MASK          0x6f    /* scid Mask */
#define SIOP_SCID_RRE		0x40	/* Response to Reselection Enable */
#define SIOP_SCID_SRE		0x20	/* Response to Selection Enable */
#define SIOP_SCID_ID_MASK	0x0f	/* Encoded 53C720 Chip SCSI ID
					 *	  Priority of the 16 IDs
					 * Highest			  Lowest
					 * 7 6 5 4 3 2 1 0 15 14 13 12 11 10 9 8
					 */

/*
 * SIOP Transfer Register (sxfer)
 */
#define SIOP_SXFER_MASK         0xef    /* sxfer Mask */
#define SIOP_SXFER_TP_MASK	0xe0	/* Synchronous Transfer Period */
					/* See NCR spec for formulas   */
#	define SIOP_TP_SHIFT	   5	/* TP Bit Shift */
#	define	SIOP_MIN_XFERP	   4
#       define  SIOP_MAX_XFERP     11
#define SIOP_SXFER_MO_MASK	0x0f	/* Maximum Synchronous Offset */
#	define	SIOP_SYNCH_OFFSET  8
#       define  SIOP_ASYNC_OFFSET  0

/*
 * SIOP Destination ID Register (sdid)
 */
#define	SIOP_SDID_ID_MASK	0x0f	/* Encoded Destination (target) ID */

/*
 * SIOP General Purpose Register (gpreg)
 */
#define SIOP_GPREG_MASK         0x1f    /* gpreg Mask */
#define SIOP_GPREG_IO_MASK	0x1f	/* General Purpose I/O Mask */
#define SIOP_GPREG_IO4		0x10	/* General Purpose I/O Bit 4 */
#define SIOP_GPREG_IO3		0x08	/* General Purpose I/O Bit 3 */
#define SIOP_GPREG_IO2		0x04	/* General Purpose I/O Bit 2 */
#define SIOP_GPREG_IO1		0x02	/* General Purpose I/O Bit 1 */
#define SIOP_GPREG_IO0		0x01	/* General Purpose I/O Bit 0 */

/*
 * SIOP Output Control Register (socl)
 */
#define SIOP_SOCL_REQ		0x80	/* SCSI Bus REQ Signal */
#define SIOP_SOCL_ACK		0x40	/* SCSI Bus ACK Signal */
#define SIOP_SOCL_BSY		0x20	/* SCSI Bus BSY Signal */
#define SIOP_SOCL_SEL		0x10	/* SCSI Bus SEL Signal */
#define SIOP_SOCL_ATN		0x08	/* SCSI Bus ATN Signal */
#define SIOP_SOCL_MSG		0x04	/* SCSI Bus MSG Signal */
#define SIOP_SOCL_CD		0x02	/* SCSI Bus C/D Signal */
#define SIOP_SOCL_IO		0x01	/* SCSI Bus I/O Signal */

/*
 * SIOP Selector ID Register (ssid)
 */
#define SIOP_SSID_VAL		0x80	/* Valid ID flag */
#define SIOP_SSID_ID_MASK	0x0f	/* Encoded (re)selector ID */

/*
 * SIOP Bus Control Lines Register (sbcl)
 */
#define SIOP_SBCL_REQ		0x80	/* SCSI Bus REQ Signal */
#define SIOP_SBCL_ACK		0x40	/* SCSI Bus ACK Signal */
#define SIOP_SBCL_BSY		0x20	/* SCSI Bus BSY Signal */
#define SIOP_SBCL_SEL		0x10	/* SCSI Bus SEL Signal */
#define SIOP_SBCL_ATN		0x08	/* SCSI Bus ATN Signal */
#define SIOP_SBCL_MSG		0x04	/* SCSI Bus MSG Signal */
#define SIOP_SBCL_CD		0x02	/* SCSI Bus C/D Signal */
#define SIOP_SBCL_IO		0x01	/* SCSI Bus I/O Signal */

/*
 * SIOP DMA Status Register (dstat)
 */
#define SIOP_DSTAT_DFE		0x80	/* DMA FIFO Empty */
#define SIOP_DSTAT_HPE		0x40	/* Host Parity Error */
#define SIOP_DSTAT_BF		0x20	/* Bus Fault */
#define SIOP_DSTAT_ABRT		0x10	/* Aborted */
#define SIOP_DSTAT_SSI		0x08	/* SCRIPT Single-Step Interrupt */
#define SIOP_DSTAT_SIR		0x04	/* SCRIPT Intr Instruction Received */
#define SIOP_DSTAT_WTD		0x02	/* Watchdog Timeout Detected */
#define SIOP_DSTAT_IID		0x01	/* Illegal Instruction Detected */

/*
 * SIOP Status 0 Register (sstat0)
 */
#define SIOP_SSTAT0_ILF		0x80	/* SIDL Least Significant Byte Full */
#define SIOP_SSTAT0_ORF		0x40	/* SODR Least Significant Byte Full */
#define SIOP_SSTAT0_OLF		0x20	/* SODL Least Significant Byte Full */
#define SIOP_SSTAT0_AIP		0x10	/* Arbitration In Progress */
#define SIOP_SSTAT0_LOA		0x08	/* Lost Arbitration */
#define SIOP_SSTAT0_WOA		0x04	/* Won Arbitration */
#define SIOP_SSTAT0_RST		0x02	/* SCSI Bus RST Signal */
#define SIOP_SSTAT0_SDP		0x01	/* Unlatched SCSI Bus SDP0 Parity */

/*
 * SIOP Status 1 Register (sstat1)
 */
#define SIOP_SSTAT1_FF_MASK	0xf0	/* FIFO Flags Mask */
#       define SIOP_FF_SHIFT    4
#       define SIOP_FF_MAX      9
#define SIOP_SSTAT1_SDP		0x08	/* Latched SCSI Bus SDP0 Parity */
#define SIOP_SSTAT1_MSG		0x04	/* SCSI Bus MSG Signal */
#define SIOP_SSTAT1_CD		0x02	/* SCSI Bus C/D Signal */
#define SIOP_SSTAT1_IO		0x01	/* SCSI Bus I/O Signal */

#define SIOP_PHASE(csr)		SCSI_PHASE(csr)

/*
 * SIOP Status 2 Register (sstat2)
 */
#define SIOP_SSTAT2_ILF1	0x80	/* SIDL Most Significant Byte Full */
#define SIOP_SSTAT2_ORF1	0x40	/* SODR Most Significant Byte Full */
#define SIOP_SSTAT2_OLF1	0x20	/* SODL Most Significant Byte Full */
#define SIOP_SSTAT2_SPL1	0x08	/* Latched SCSI Bus SDP1 Parity */
#define SIOP_SSTAT2_LDSC	0x02	/* Last Disconnect */
#define SIOP_SSTAT2_SDP1	0x01	/* Unlatched SCSI Bus SDP1 Parity */

/*
 * SIOP Interrupt Status Register (istat)
 */
#define	SIOP_ISTAT_ABRT		0x80	/* Abort Operation */
#define	SIOP_ISTAT_RST		0x40	/* Software Reset */
#define SIOP_ISTAT_SIGP         0x20    /* Signal Process */
#define SIOP_ISTAT_SEM          0x10    /* Semaphore */
#define	SIOP_ISTAT_CON		0x08	/* Connected */
#define	SIOP_ISTAT_INTF		0x04	/* Interrupt on the Fly */
#define	SIOP_ISTAT_SIP		0x02	/* SCSI Interrupt Pending */
#define	SIOP_ISTAT_DIP		0x01	/* DMA Interrupt Pending */

/*
 * SIOP Test 0 Register (ctest0)
 */
#define SIOP_CTEST0_CDIS	0x80	/* Cache Burst Disable */
#define SIOP_CTEST0_SC_MASK	0x60	/* Snoop Control Mask */
#define SIOP_CTEST0_GRP		0x10	/* Generate Receive Parity (pass-thr) */
#define SIOP_CTEST0_DFP		0x08	/* DMA FIFO Parity */
#define SIOP_CTEST0_EHP		0x04	/* Even Host Parity */
#define SIOP_CTEST0_TT1		0x02	/* Transfer Type Bit */
#define SIOP_CTEST0_C386E	0x01	/* Cache 386 Enable */

/*
 * SIOP Test 1 Register (ctest1)
 */
#define SIOP_CTEST1_FMT3	0x80	/* Byte Lane 3 Empty in DMA FIFO */
#define SIOP_CTEST1_FMT2	0x40	/* Byte Lane 2 Empty in DMA FIFO */
#define SIOP_CTEST1_FMT1	0x20	/* Byte Lane 1 Empty in DMA FIFO */
#define SIOP_CTEST1_FMT0	0x10	/* Byte Lane 0 Empty in DMA FIFO */
#define SIOP_CTEST_FMT_MASK	0xf0	/* Byte Lane Empty Mask */
#define SIOP_CTEST1_FFL3	0x08	/* Byte Lane 3 Full in DMA FIFO */
#define SIOP_CTEST1_FFL2	0x04	/* Byte Lane 2 Full in DMA FIFO */
#define SIOP_CTEST1_FFL1	0x02	/* Byte Lane 1 Full in DMA FIFO */
#define SIOP_CTEST1_FFL0	0x01	/* Byte Lane 0 Full in DMA FIFO */
#define SIOP_CTEST_FFL_MASK	0x0f	/* Byte Lane Full Mask */

/*
 * SIOP Test 2 Register (ctest2)
 */
#define SIOP_CTEST2_DDIR	0x80	/* Data Transfer Direction */
#define SIOP_CTEST2_SIGP	0x40	/* Signal Process */
#define SIOP_CTEST2_DFP		0x08	/* DMA FIFO Parity */
#define SIOP_CTEST2_TEOP	0x04	/* True End of Process */
#define SIOP_CTEST2_DREQ	0x02	/* Data Request Status */
#define SIOP_CTEST2_DACK	0x01	/* Data Acknowledge Status */

/*
 * SIOP Test 3 Register (ctest3)
 */
#define SIOP_CTEST3_VER_MASK	0xf0	/* Chip Revision Level Mask */
#define SIOP_CTEST3_FLF		0x08	/* Flush DMA FIFO */
#define SIOP_CTEST3_CLF		0x04	/* Clear DMA FIFO */
#define SIOP_CTEST3_FM		0x02	/* Fetch Pin Mode */
#define SIOP_CTEST3_SM		0x01	/* Snoop Pins Mode */

/*
 * SIOP Chip Revision
 */
#define SIOP_REV_MASK		(SIOP_CTEST3_VER_MASK)
#define SIOP_REV_SHIFT		4
#define SIOP_REV(rev)		((unsigned char)((rev) & SIOP_REV_MASK) >> \
				 SIOP_REV_SHIFT)

/*
 * SIOP DMA FIFO Register (dfifo)
 */
#define SIOP_DFIFO_MASK		0x7f	/* Byte Offset Counter Mask */

/*
 * SIOP Test 4 Register (ctest4)
 */
#define SIOP_CTEST4_MUX		0x80	/* Host Bus Multiplex Mode */
#define SIOP_CTEST4_ZMOD	0x40	/* High Impedance Mode */
#define SIOP_CTEST4_ZSD		0x20	/* SCSI High Impedance Mode */
#define SIOP_CTEST4_SRTM	0x10	/* Shadow Register Test Mode */
#define SIOP_CTEST4_EHPC	0x08	/* Enable Host Parity Check */
#define SIOP_CTEST4_FBL_MASK	0x07	/* FIFO Byte Lane Control */
#	define	SIOP_DMA_LANE_DISABLE	0x0	/* Normal Operation */
#	define	SIOP_DMA_LANE_0		0x4
#	define	SIOP_DMA_LANE_1		0x5
#	define	SIOP_DMA_LANE_2		0x6
#	define	SIOP_DMA_LANE_3		0x7

/*
 * SIOP Test 5 Register (ctest5)
 */
#define SIOP_CTEST5_MASK        0xd8    /* ctest5 Mask */
#define SIOP_CTEST5_ADCK	0x80	/* Clock Address incrementor */
#define SIOP_CTEST5_BBCK	0x40	/* Clock Byte Counter */
#define SIOP_CTEST5_MASR	0x10	/* Master Control of Set/Reset Pulses */
#define SIOP_CTEST5_DDIR	0x08	/* DMA Direction */

/*
 * SIOP DMA Command Register (dcmd)
 */
#define SIOP_DCMD_TYPE_MASK	0xc0	/* Instruction Type Mask */
#define SIOP_DCMD_OP_CODE_MASK	0x78	/* Instruction Op Code Mask */
#define SIOP_DCMD_MODIFIER_MASK	0x07	/* Instruction Modifier Mask */

/*
 * SIOP DMA Mode Register (dmode)
 */
#define SIOP_DMODE_BL_MASK	0xc0	/* Burst Length Mask */
#	define	SIOP_BURST_SHIFT	6
#	define	SIOP_2_XFER_BURST	0x0
#	define	SIOP_4_XFER_BURST	0x1
#	define	SIOP_8_XFER_BURST	0x2
#	define	SIOP_16_XFER_BURST	0x3
#define SIOP_DMODE_FC_MASK	0x30	/* Function Code Mask */
#	define	SIOP_FC_SFHIT	4
#define SIOP_DMODE_PD		0x08	/* Program/Data */
#define SIOP_DMODE_FAM		0x04	/* Fixed Address Mode */
#define SIOP_DMODE_UO		0x02	/* User Programmable Transfer Type */
#define SIOP_DMODE_MAN		0x01	/* Manual Start Mode */

/*
 * SIOP DMA Interrupt Enable Register (dien)
 */
#define SIOP_DIEN_MASK          0x7f    /* dien Mask */
#define SIOP_DIEN_HPED		0x40    /* Host Parity Error Detected */
#define SIOP_DIEN_BF		0x20    /* Bus Fault */
#define	SIOP_DIEN_ABRT		0x10	/* Aborted */
#define	SIOP_DIEN_SSI		0x08	/* SCRIPT Single-Step Interrupt */
#define	SIOP_DIEN_SIR		0x04	/* SCRIPT Intr Instruction Received */
#define	SIOP_DIEN_WTD		0x02	/* Watchdog Timeout Detected */
#define	SIOP_DIEN_IID		0x01	/* Illegal Instruction Detected */

/*
 * SIOP DMA Control Register (dcntl)
 */
#define SIOP_DCNTL_STE		0x80	/* Size Throttle Enable */
#define SIOP_DCNTL_BSM		0x40	/* Bus Mode */
#define SIOP_DCNTL_EA		0x20	/* Enable ACK */
#define SIOP_DCNTL_SSM		0x10	/* Single-Step Mode */
#define SIOP_DCNTL_BW16		0x08	/* Host Bus Width Equal to 16 */
#define SIOP_DCNTL_STD		0x04	/* Start DMA Operation */
#define SIOP_DCNTL_FA		0x02	/* Fast Arbitration */
#define SIOP_DCNTL_COM		0x01	/* 53C700 Compatibility */

/*
 * SIOP Interrupt Enable 0 Register (sien0)
 */
#define	SIOP_SIEN0_M_A		0x80	/* Phase Mismatch or ATN Active */
#define	SIOP_SIEN0_CMP		0x40	/* Function Complete */
#define	SIOP_SIEN0_SEL		0x20	/* Selected */
#define SIOP_SIEN0_RSL		0x10    /* Reselected */
#define	SIOP_SIEN0_SGE		0x08	/* SCSI Gross Error */
#define	SIOP_SIEN0_UDC		0x04	/* Unexpected Disconnect */
#define	SIOP_SIEN0_RST		0x02	/* SCSI Bus RST Asserted */
#define	SIOP_SIEN0_PAR		0x01	/* SCSI Bus Parity Error */

/*
 * SIOP Interrupt Enable 1 Register (sien1)
 */
#define SIOP_SIEN1_MASK         0x07    /* sien1 Mask */
#define SIOP_SIEN1_STO		0x04    /* Selection/Reselection Timeout */
#define SIOP_SIEN1_GEN		0x02    /* General Purpose Timer Expired */
#define SIOP_SIEN1_HTH		0x01    /* Handshake-Handshake Timer Expired */

/*
 * SIOP Interrupt Status 0 Register (sist0)
 */
#define SIOP_SIST0_M_A		0x80	/* Phase Mismatch or ATN Active */
#define SIOP_SIST0_CMP		0x40	/* Function Complete */
#define SIOP_SIST0_SEL		0x20	/* Selected */
#define SIOP_SIST0_RSL		0x10	/* Reselected */
#define SIOP_SIST0_SGE		0x08	/* SCSI Gross Error */
#define SIOP_SIST0_UDC		0x04	/* Unexpected Disconnect */
#define SIOP_SIST0_RST		0x02	/* SCSI Bus RST Asserted */
#define SIOP_SIST0_PAR		0x01	/* SCSI Bus Parity Error */

/*
 * SIOP Interrupt Status 1 Register (sist1)
 */
#define SIOP_SIST1_STO		0x04	/* Selection/Reselection Timeout */
#define SIOP_SIST1_GEN		0x02	/* General Purpose Timer Expired */
#define SIOP_SIST1_HTH		0x01	/* Handshake-Handshake Timer Expired */

/*
 * SIOP Memory Access Control Register (macntl)
 */
#define SIOP_MACNTL_MASK        0x0f    /* macntl Mask */
#define SIOP_MACNTL_DATA_WR	0x08	/* Local Memory Data Write */
#define SIOP_MACNTL_DATA_RD	0x04	/* Local Memory Data Read */
#define SIOP_MACNTL_P_SCRIPT	0x02	/* Local Memory Ptr to SCRIPTs fetch */
#define SIOP_MACNTL_SCRIPT	0x01	/* Local Memory SCRIPTs fetch */

/*
 * SIOP General Purpose Control Register (gpcntl)
 */
#define SIOP_GPCNTL_MASK        0x1f    /* gpcntl Mask */
#define SIOP_GPCNTL_IO_MASK	0x1f	/* General Purpose I/O Mask */
#define SIOP_GPCNTL_IO4		0x10	/* General Purpose I/O Bit 4 */
#define SIOP_GPCNTL_IO3		0x08	/* General Purpose I/O Bit 3 */
#define SIOP_GPCNTL_IO2		0x04	/* General Purpose I/O Bit 2 */
#define SIOP_GPCNTL_IO1		0x02	/* General Purpose I/O Bit 1 */
#define SIOP_GPCNTL_IO0		0x01	/* General Purpose I/O Bit 0 */

/*
 * SIOP Timer 0 Register (stime0)
 */
#define SIOP_STIME0_HTH_MASK	0xf0	/* Handshake-Handshake Timer Period */
#	define SIOP_HTH_SHIFT	4
#define SIOP_STIME0_SEL_MASK	0x0f	/* Selection Timeout Period */

/*
 * SIOP Timer 1 Register (stime1)
 */
#define SIOP_STIME1_MASK        0x0f    /* stime1 Mask */
#define SIOP_STIME1_GEN_MASK	0x0f	/* General Purpose Timer Period */

/*
 *	The following timeout values apply to the
 *	Handshake-to-Handshake Period Timer (stime0),
 *	Selection Timeout Period (stime0), and the
 *	General Purpose Timer Period (stime1).
 */
#	define	SIOP_TIMEOUT_DISABLE	0x0	/* Timeout disabled   */
#	define	SIOP_TIMEOUT_100_us	0x1	/* 100   usec Timeout */
#	define	SIOP_TIMEOUT_200_us	0x2	/* 200   usec Timeout */
#	define	SIOP_TIMEOUT_400_us	0x3	/* 400   usec Timeout */
#	define	SIOP_TIMEOUT_800_us	0x4	/* 800   usec Timeout */
#	define	SIOP_TIMEOUT_1_6_ms	0x5	/* 1.6   msec Timeout */
#	define	SIOP_TIMEOUT_3_2_ms	0x6	/* 3.2   msec Timeout */
#	define	SIOP_TIMEOUT_6_4_ms	0x7	/* 6.4   msec Timeout */
#	define	SIOP_TIMEOUT_12_8_ms	0x8	/* 12.8  msec Timeout */
#	define	SIOP_TIMEOUT_25_6_ms	0x9	/* 25.6  msec Timeout */
#	define	SIOP_TIMEOUT_51_2_ms	0xa	/* 51.2  msec Timeout */
#	define	SIOP_TIMEOUT_102_4_ms	0xb	/* 102.4 msec Timeout */
#	define	SIOP_TIMEOUT_204_8_ms	0xc	/* 204.8 msec Timeout */
#	define	SIOP_TIMEOUT_409_6_ms	0xd	/* 409.6 msec Timeout */
#	define	SIOP_TIMEOUT_819_2_ms	0xe	/* 819.2 msec Timeout */
#	define	SIOP_TIMEOUT_1_6_sec	0xf	/* 1.6+  sec  Timeout */

/*
 * SIOP Test 0 Register (stest0)
 */
#define SIOP_STEST0_SLT		0x08	/* Selection Response Logic Test */
#define SIOP_STEST0_ART		0x04	/* Arbitration Priority Encoder Test */
#define SIOP_STEST0_SOZ		0x02	/* Synchronous Offset Zero */
#define SIOP_STEST0_SOM		0x01	/* Synchronous Offset Maximum */

/*
 * SIOP Test 1 Register (stest1)
 */
#define SIOP_STEST1_SFP1	0x02	/* Most Significant FIFO Byte Parity */
#define SIOP_STEST1_SFP0	0x01	/* Least Significant FIFO Byte Parity */

/*
 * SIOP Test 2 Register (stest2)
 */
#define SIOP_STEST2_SCE		0x80	/* SCSI Control Enable */
#define SIOP_STEST2_ROF		0x40	/* Reset SCSI Offset */
#define SIOP_STEST2_DIF		0x20	/* SCSI Differential Mode */
#define SIOP_STEST2_SLB		0x10	/* SCSI Loopback Mode */
#define SIOP_STEST2_SZM		0x08	/* SCSI High-Impedance Mode */
#define SIOP_STEST2_AWS		0x04	/* Always Wide SCSI */
#define SIOP_STEST2_EXT		0x02	/* Extend REQ/ACK Filtering */
#define SIOP_STEST2_LOW		0x01	/* SCSI Low Level Mode */

/*
 * SIOP Test 3 Register (stest3)
 */
#define SIOP_STEST3_TE		0x80	/* TolerANT Enable */
#define SIOP_STEST3_STR		0x40	/* SCSI FIFO Test Read */
#define SIOP_STEST3_HSC		0x20	/* Halt SCSI Clock */
#define SIOP_STEST3_DSI		0x10	/* Disable Single Initiator Response */
#define SIOP_STEST3_S16		0x08	/* 16-bit System */
#define SIOP_STEST3_TTM		0x04	/* Timer Test Mode */
#define SIOP_STEST3_CSF		0x02	/* Clear SCSI FIFO */
#define SIOP_STEST3_STW		0x01	/* SCSI FIFO Test Write */

/*----------------------------------------------------------------------------*/

/* macros */
#define is_tgt_q_full(tgt)		(tgt->flags & TGT_QUEUE_FULL)
#define tgt_q_full(tgt)			(tgt->flags |= TGT_QUEUE_FULL)
#define tgt_q_not_full(tgt)		(tgt->flags &= (~TGT_QUEUE_FULL))
#define is_tag_queuing_enabled(tgt)     (tgt->flags & TGT_TAGGED_QUEUING)
#define tag_queuing_enable(tgt) 	(tgt->flags |= TGT_TAGGED_QUEUING)
#define tag_queuing_disable(tgt) 	(tgt->flags &= (~TGT_TAGGED_QUEUING))
#define cntrlr_q_full(siop)     \
              (siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].req_tbl.src_ptr)
/* the file kernel/kern/queue.h does not have queue_iterate_reverse(...)
   macro to iterate through the list in the reverse order; it has 
   queue_iterate(..) */
#define queue_iterate_reverse(head, elt, type, field)	 \
        for ((elt) = (type) queue_last(head);       	\
             !queue_end((head), (queue_entry_t)(elt)); 	\
             (elt) = (type) queue_prev(&(elt)->field))
#define get_cntrlr_intf_tag(siop, req_tbl_clram_ptr)	\
    req_tbl_clram_image[req_tbl_clram_ptr - siop->req_tbl_ptr_clram_phys]

struct request_table {
#define NOT_ALIGNED_16_BYTE     0xf
    struct {
        unsigned char not_used; /* least significant byte */
        unsigned char sxfer;
        unsigned char dest_id;  /* target ID */
        unsigned char scntl3;   /* most significant byte */
    } bus_conf;
    unsigned long not_used;     /* 53C720 requires this field to be unused */
    struct {
        unsigned long len;
        unsigned char *ptr;
#define OFST_MSG_OUT_IN_REQ_TBL  16
#define MAX_MSG_OUT_LGT      8
	unsigned char bytes[MAX_MSG_OUT_LGT];
    } msg_out;
    struct {
        unsigned long len;
        unsigned char *ptr;
#define OFST_CMD_IN_REQ_TBL  32
#define MAX_CMD_LGT          12
	unsigned char bytes[MAX_CMD_LGT];
    } cmd;
#define OFST_SG_ENTS_PTR_IN_REQ_TBL    44
    unsigned char * sg_ent_ptr;
    /* sg_ent must be the last element in this structure so that in the 
       req_tbl_ptr_clram[], sg_ents continuing to the next req_tbl occupy 
       contiguous locations in clram; also SG_ENTS must be an odd number to
       align entries in sglist to 16 bytes boundary because the 8 byte header
       at the top of sglist misaligns the entries */
    unsigned long not_used1;
    unsigned long not_used2;
#define OFST_SG_ENTS_IN_REQ_TBL    56
    struct {
        unsigned long len;
        unsigned char *ptr;
#define SG_ENTS		9 /* NOTE: must be an odd number to align entries in
			     sg list to 16 byte boundary */
    } sg_ent[SG_ENTS];
};

struct cntrlr_buf {
    struct {
        unsigned char q_tag;
        unsigned char target_id; /* host writes zero in high order nibble */
        unsigned char align2;
        unsigned char align3;
    } info;
    struct {
        struct request_table *src_ptr;
        struct request_table *dest_ptr;
        unsigned long cnt;
    } req_tbl;
    struct {
        unsigned char *src_ptr;
        unsigned char *dest_ptr;
        unsigned long cnt;
#define SG_LISTS		2	/* assume in code greater than 2 */
    } sg_list[SG_LISTS];
    unsigned long end_marker; /* value of zero marks the end of scatter-gather
                               */
};

struct cntrlr_shrd_data {
    /* write-only by SIOP before it interrupts and halts; read by host
       when SIOP is executing and write by host when SIOP has halted */
    volatile union {
        unsigned long word;
        struct {
            unsigned char no_work_to_do;
            unsigned char halt_reason;
	    /* values of cntrlr_shrd_data.stat.byte.halt_reason; halt.reason 
	       is non-zero only after SIOP executes the "INT 0" instruction */
#define     STAT_NOT_GOOD           0x1     /* check (cntrlr_shrd_data.stat.
					       byte.status_buf) for more info
					       */
#define     REQ_DONE_Q_FULL         0x2
#define     ERROR_UNKNOWN_PHASE     0x3
#define     ERROR_RESELECT_ID       0x4
#define     OTHER_MSG               0x5     /* check (cntrlr_shrd_data.stat.
					       byte.msg_in_count) for more 
					       info */
#define     ERROR_WRONG_PHASE       0x6
#define     SIOP_RESET              0xfd    /* used by host only, not SIOP */
#define     SELECT_TIMEOUT          0xfe    /* used by host only, not SIOP */
#define     DEBUG_MSG               0xff    /* used by host only, not SIOP */
            unsigned char status_buf;
            unsigned char msg_in_count;
        } byte;
    } stat;
    volatile struct {
#define MAX_EXD_MSG      8
        unsigned char byte[MAX_EXD_MSG];
    } msg_in;
    volatile unsigned long next_addr;
    volatile struct request_table *req_tbl_clram_ptr; /* SIOP writes the phys
							 addr of request table
							 ptr in CLRAM of
							 current request in 
							 here */
#define	OFFSET_TO_REQ_DONE_Q	20
#define REQ_DONE_Q_SIZE         SCRIPT_Q_SIZE
    volatile struct request_table *req_done_q[REQ_DONE_Q_SIZE];
    /* NOTE: For SIOP to do cache-line xfer of request_table from source in 
       DPRAM to destination in CLRAM, the low 4 bits must be the same for 
       the source and dest address */
#define	OFFSET_TO_PAD			\
		(OFFSET_TO_REQ_DONE_Q + \
		(sizeof(struct request_table *) * REQ_DONE_Q_SIZE))
#define	PAD_SIZE		(16 - (OFFSET_TO_PAD % 16))
    volatile unsigned char pad_to_16_bytes[PAD_SIZE]; /* make low 4 bits same */
#define CNTRLR_Q_SIZE           SCRIPT_Q_SIZE
    volatile struct request_table req_tbl[CNTRLR_Q_SIZE];
    volatile struct cntrlr_buf cntrlr_q[CNTRLR_Q_SIZE];
};

/* SIOP uses the following defines with INT instruction to have the 
   host print the debug message; zero is not used because it means a
   non-debug INT from SIOP */
#define DEBUG_MSG0_LOW          1
#define DEBUG_MSG_LOW_LAST      1
#define DEBUG_MSG0_MED          101
#define DEBUG_MSG_MED_LAST      157
#define DEBUG_MSG0_HIGH         201
#define DEBUG_MSG_HIGH_LAST     201
#define DEBUG_ASSERT0           301
#define DEBUG_ASSERT_LAST       306

#endif  _SCSI_53C720_H_
