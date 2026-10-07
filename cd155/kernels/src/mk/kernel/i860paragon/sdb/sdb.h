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
 * $Log: sdb.h,v $
 * Revision 1.3.8.1  1995/06/11  18:33:27  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.3  1995/03/28  01:08:27  jerrie
 * Added bootmagic bit definitions.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low.
 *  Benefit or PTS #: 12827
 *  Testing:	   Tested bootmagic by resetting with various settings.
 *  Module(s):	   See PTS report.
 *
 * Revision 1.2  1995/03/14  23:43:39  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	sdb.h
 *	Author:	Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	4/94
 *
 *	SCSI-16 daughter board hardware definitions
 */


#ifndef	_SDB_H_
#define	_SDB_H_

#ifndef	BIT
#define BIT(x)			(1 << ((x) & 31))
#endif	BIT

/*
 * The sdb_configuration variable allows control of the
 * software and hardware initialization via bootmagic.
 */
extern int	sdb_configuration;
#define 		SDB_CONFIGURATION_DISABLE_DMA		BIT(0)
#define 		SDB_CONFIGURATION_ENABLE_FLUSH		BIT(1)
#define 		SDB_CONFIGURATION_IGNORE_PARITY		BIT(2)

/*
 * Control/Status Registers
 *
 *	Type Of Bit:
 *			rw = Read/Write
 *			ro = Read-Only
 *			rs = Read-Sticky (set by h/w, write 1 to clear)
 *
 *	Response To Reset:
 *			0 = Reset (cleared on reset)
 *			1 = Set	  (set on reset)
 */

#define SDB_CONTROL		(0x80000000)	/* control register */
#define 	SDB_CONTROL_SCSI0_RESET		BIT(0)	/* rw 1 */	
#define 	SDB_CONTROL_SCSI1_RESET		BIT(1)	/* rw 1 */	
#define 	SDB_CONTROL_ETHER_RESET		BIT(2)	/* rw 1 */	
#define 	SDB_CONTROL_SERIAL_RESET	BIT(3)	/* rw 1 */
#define 	SDB_CONTROL_FLASH_WRITE		BIT(4)	/* rw 0 */
#define 	SDB_CONTROL_SPARE0		BIT(5)	/* rw 0 */
#define 	SDB_CONTROL_SPARE1		BIT(6)	/* rw 0 */
#define 	SDB_CONTROL_SPARE2		BIT(7)	/* rw 0 */
#define SDB_CONTROL_DEFAULT	(SDB_CONTROL_SCSI0_RESET |	\
				 SDB_CONTROL_SCSI1_RESET |	\
				 SDB_CONTROL_ETHER_RESET)

#define SDB_STATUS		(0x80000008)	/* status register */
#define 	SDB_STATUS_SCSI0_INSTALLED	BIT(0)	/* ro	*/
#define 	SDB_STATUS_SCSI1_INSTALLED	BIT(1)	/* ro	*/
#define		SDB_STATUS_SCSI0_DIFFERENTIAL	BIT(2)	/* ro	*/
#define		SDB_STATUS_SCSI1_DIFFERENTIAL	BIT(3)	/* ro	*/
#define		SDB_STATUS_SYSKEN		BIT(4)	/* ro	*/
#define		SDB_STATUS_JUMPER0		BIT(6)	/* ro	*/
#define		SDB_STATUS_JUMPER1		BIT(7)	/* ro	*/
/*
 *	When a parity error is detected on an access by a node processor,
 *	these eight bits show the byte in which the parity error was
 *	detected.  These bits are only valid when the SDB_ISTAT_PARITY
 *	bit in the interrupt status register is true, thus SDB_ISTAT_PARITY
 *	should not be cleared until these bits have been read.
 */
#define 	SDB_STATUS_PARITY0		BIT(8)	/* ro	*/
#define 	SDB_STATUS_PARITY1		BIT(9)	/* ro	*/
#define 	SDB_STATUS_PARITY2		BIT(10)	/* ro	*/
#define 	SDB_STATUS_PARITY3		BIT(11)	/* ro	*/
#define 	SDB_STATUS_PARITY4		BIT(12)	/* ro	*/
#define 	SDB_STATUS_PARITY5		BIT(13)	/* ro	*/
#define 	SDB_STATUS_PARITY6		BIT(14)	/* ro	*/
#define 	SDB_STATUS_PARITY7		BIT(15)	/* ro	*/

/*
 * Interrupt Status/Mask Registers
 *
 *	Type Of Bit:
 *			rw = Read/Write
 *			ro = Read-Only
 *			rs = Read-Sticky (set by h/w, write 1 to clear)
 *
 *	Response To Reset:
 *			0 = Reset (cleared on reset)
 *			1 = Set	  (set on reset)
 */
#define SDB_ISTAT		(0x80000010)	/* interrupt status register */
#define 	SDB_ISTAT_SCSI0			BIT(0)	/* ro 0 */
#define 	SDB_ISTAT_SCSI1			BIT(1)	/* ro 0 */
#define 	SDB_ISTAT_ETHER			BIT(2)	/* rs 0 */
#define 	SDB_ISTAT_SERIAL		BIT(3)	/* ro 0 */
#define 	SDB_ISTAT_DMA0			BIT(4)	/* rs 0 */
#define 	SDB_ISTAT_DMA1			BIT(5)	/* rs 0 */
#define 	SDB_ISTAT_DMA2			BIT(6)	/* rs 0 */
#define 	SDB_ISTAT_ETHER_PARITY		BIT(7)	/* rs 0 */
#define 	SDB_ISTAT_PARITY		BIT(8)	/* rs 0 */
#define SDB_ISTAT_DEFAULT		(SDB_ISTAT_ETHER	| \
					 SDB_ISTAT_DMA0		| \
					 SDB_ISTAT_DMA1		| \
					 SDB_ISTAT_DMA2		| \
					 SDB_ISTAT_ETHER_PARITY | \
					 SDB_ISTAT_PARITY)

/*
 *	Interrupts from any of the interrupt sources on the card are
 *	enabled by a bit vector.  When set to 1, the interrupt is masked.
 *	When set to 0, the interrupt is enabled.
 */
#define SDB_IMASK		(0x80000018)	/* interrupt mask register */
#define 	SDB_IMASK_SCSI0			BIT(0)	/* rw 1 */
#define 	SDB_IMASK_SCSI1			BIT(1)	/* rw 1 */
#define 	SDB_IMASK_ETHER			BIT(2)	/* rw 1 */
#define 	SDB_IMASK_SERIAL		BIT(3)	/* rw 1 */
#define 	SDB_IMASK_DMA0			BIT(4)	/* rw 1 */
#define 	SDB_IMASK_DMA1			BIT(5)	/* rw 1 */
#define 	SDB_IMASK_DMA2			BIT(6)	/* rw 1 */
#define 	SDB_IMASK_ETHER_PARITY		BIT(7)	/* rw 1 */
#define 	SDB_IMASK_PARITY		BIT(8)	/* rw 1 */
#define SDB_IMASK_DEFAULT		(SDB_IMASK_SCSI0	| \
					 SDB_IMASK_SCSI1	| \
					 SDB_IMASK_ETHER	| \
					 SDB_IMASK_SERIAL	| \
					 SDB_IMASK_DMA0		| \
					 SDB_IMASK_DMA1		| \
					 SDB_IMASK_DMA2		| \
					 SDB_IMASK_ETHER_PARITY | \
					 SDB_IMASK_PARITY)

/*
 * FPGA Programming Registers
 */
#define SDB_FPGA_CONFIG		(0x80000020)	/* FPGA configuration */
#define 	SDB_FPGA_CONFIG_3K_PROG		BIT(0)	/* rw 0 */
#define 	SDB_FPGA_CONFIG_3K_DONE		BIT(1)	/* ro   */
#define 	SDB_FPGA_CONFIG_3K_INIT		BIT(2)	/* ro   */
#define 	SDB_FPGA_CONFIG_4K_PROG		BIT(3)	/* rw 0 */
#define 	SDB_FPGA_CONFIG_4K_DONE		BIT(4)	/* ro   */
#define 	SDB_FPGA_CONFIG_4K_INIT		BIT(5)	/* ro   */
#define 	SDB_FPGA_CONFIG_DATA		BIT(6)	/* rw   */
#define 	SDB_FPGA_JTAG_TDO		BIT(7)	/* ro   */
#define 	SDB_FPGA_JTAG_TDI		BIT(8)	/* rw   */
#define 	SDB_FPGA_JTAG_TCK		BIT(9)	/* rw   */
#define 	SDB_FPGA_JTAG_TMS		BIT(10)	/* rw   */
#define 	SDB_FPGA_PRFACC			BIT(11)	/* rw 1 */

#define SDB_FPGA_CLOCK		(0x80000028)	/* FPGA clock	      */

/*
 * Diagnostic Control Register
 *
 *	Type Of Bit:
 *			rw = Read/Write
 *			ro = Read-Only
 *			rs = Read-Sticky (set by h/w, write 1 to clear)
 *
 *	Response To Reset:
 *			0 = Reset (cleared on reset)
 *			1 = Set	  (set on reset)
 */
#define SDB_DIAG		(0x80000030)	/* diagnostic control */
#define 	SDB_DIAG_BAD_PARITY0		BIT(0)	/* rw 0 */
#define 	SDB_DIAG_BAD_PARITY1		BIT(1)	/* rw 0 */
#define 	SDB_DIAG_PARITY_SEL0		BIT(2)	/* rw	*/
#define 	SDB_DIAG_PARITY_SEL1		BIT(3)	/* rw	*/
#define 	SDB_DIAG_PARITY_SEL2		BIT(4)	/* rw	*/
#define 	SDB_DIAG_BYPASS			BIT(6)	/* rw 0 */
#define 	SDB_DIAG_NEXT_NEAR		BIT(7)	/* rw 0 */
#define 	SDB_DIAG_KEEP_BUS		BIT(8)	/* rw 0 */

/*
 * Logic Analyzer Trigger
 */
#define SDB_TRIGGER		(0x80000038)	/* logic analyzer trigger */

/*
 * SCSI Controller Semaphore Registers
 *
 *	Before accessing SCSI the controller registers, the semaphore
 *	should be polled until the access bit is set.  When all register
 *	accesses are complete, a write to the semaphore will release the lock.
 */
#define SDB_SCSI0_SEMAPHORE	(0x80000040)	/* SCSI 0 semaphore */
#define		SDB_SCSI0_ACCESS		BIT(0)	/* rw   */
#define SDB_SCSI1_SEMAPHORE	(0x80000048)	/* SCSI 1 semaphore */
#define		SDB_SCSI1_ACCESS		BIT(0)	/* rw   */

/*
 * Serial EEPROM Access Register
 *
 *	Type Of Bit:
 *			rw = Read/Write
 *			ro = Read-Only
 *			rs = Read-Sticky (set by h/w, write 1 to clear)
 *
 *	Response To Reset:
 *			0 = Reset (cleared on reset)
 *			1 = Set	  (set on reset)
 */
#define SDB_SEEPROM		(0x80000050)	/* Serial EEPROM register */
#define		SDB_SEEPROM_CLOCK		BIT(0)	/* rw 1 */
#define		SDB_SEEPROM_DATA		BIT(1)	/* rw 0 */
#define		SDB_SEEPROM_DIR			BIT(2)	/* rw 0 */
#define		SDB_SEEPROM_WRITE		BIT(3)	/* rw 1 */

/*
 * Controller Chip Base Addresses
 */
#define	SDB_ETHER_START		(0x80100000)	/* LAN base address    */
#define	SDB_SCSI0_START		(0x80200000)	/* SCSI 0 base address */
#define	SDB_SCSI1_START		(0x80300000)	/* SCSI 1 base address */
#define	SDB_SERIAL_START	(0x80400000)	/* serial base address */

/*
 * Performance Monitor Registers
 *
 *	Type Of Bit:
 *			rw = Read/Write
 *			ro = Read-Only
 *			rs = Read-Sticky (set by h/w, write 1 to clear)
 *
 *	Response To Reset:
 *			0 = Reset (cleared on reset)
 *			1 = Set	  (set on reset)
 */
#define SDB_PERF_ETHER		(0x80500000)	/* rw	*/
#define SDB_PERF_SCSI0		(0x80500008)	/* rw	*/
#define SDB_PERF_SCSI1		(0x80500010)	/* rw	*/
#define SDB_PERF_IDLE		(0x80500018)	/* rw	*/
#define SDB_PERF_NODE		(0x80500020)	/* rw	*/
#define SDB_PERF_SDB		(0x80500028)	/* rw	*/
#define	SDB_PERF_START		SDB_PERF_ETHER	/* perf cntr base addr */

/*
 * DMA Control Registers (written by the SCSI controllers only)
 */
#define SDB_DMA_FLUSH_WR	(0x80600000)	/* wo, flush DMA FIFO to node */
#define SDB_DMA_FLUSH_RD	(0x80620000)	/* wo, flush DMA FIFO to siop */
#define SDB_DMA_INTR		(0x80610000)	/* wo   */

/*
 * Memory Addresses
 */
#define SDB_SRAM_START		(0x80700000)	/* sram base address   */
#define SDB_DPRAM_START		(0x80800000)	/* dpram base address  */
#define SDB_DPRAM_OPP_START	(0x80900000)	/* dpram opposite addr */
#define SDB_FLASH_START		(0xbffc0000)	/* flash base address  */
#define SDB_TOPOF_FLASH		(0xbfffffff)	/* top of flash        */

/*
 * Memory Sizes
 */
#define SDB_SRAM_SIZE		(512 * 1024)
#define SDB_DPRAM_SIZE		( 32 * 1024)
#define SDB_FLASH_SIZE		(256 * 1024)

/*
 * The Static and Dual-Port RAM are divided among the controllers as follows
 */

/*
 * The top 0k bytes of static RAM is reserved for the ethernet controller
 * The remaining bottom area is reserved for the SCSI I/O processors
 */
#define ETHER_SIZEOF_SRAM	(0 * 1024)
#define SIOPS_SIZEOF_SRAM	(SDB_SRAM_SIZE - ETHER_SIZEOF_SRAM)

/*
 * The top 6k bytes of dual-port RAM is reserved for the DMA FIFOs
 * The next 10k bytes of dual-port RAM is reserved for the ethernet controller
 * The bottom 16k bytes of dual-port RAM is reserved for the SCSI I/O processors
 */
#define DMA_SIZEOF_DPRAM	( 6 * 1024)
#define ETHER_SIZEOF_DPRAM	(10 * 1024)
#define SIOPS_SIZEOF_DPRAM	\
		(SDB_DPRAM_SIZE - (DMA_SIZEOF_DPRAM + ETHER_SIZEOF_DPRAM))

#endif	_SDB_H_
