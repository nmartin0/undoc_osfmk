/*
 *
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */ 
/* Copyright 1990 NCR Corporation - Dayton, Ohio, USA */
#ident	"@(#)./usr/src/arch/mca/uts/i386/sys/scsi:cs.h	1.14"

#ifndef SYS_CS_H
#define SYS_CS_H
#ifdef MP
#include "sys/mp/mplock.h"
#else
#include "sys/mplock.h"
#endif
#include "sys/types.h"
#include "sys/scsi/dcb.h"
#include "sys/scsi/SCSI.h"
#include "sys/scsi/scsi_stat.h"

#ifdef tower32_700
#include "sys/inode.h"
#include "sys/inline.h"
#include "sys/sysmacros.h"
#include "sys/newb0.h"
#else
#include "sys/vtoc.h"
#include "sys/cmn_err.h"
#ifdef WYSE
#include "sys/scsi/mesa_scsi.h"
#endif
#endif

#define	SCSI_DEBUG_ON

#define DBUG

#ifdef DBUG
#define DEBUG   debug
#else
#define DEBUG(a)
#endif

#ifdef WYSE
#define Kernel_sprintf	printf
#endif
#ifdef MP
#define SCSI_Device_Init(x)     \
	mutex_init(&(x)->CS_DeviceLock, L_ADV_SPIN|L_EXCL|L_SPIN|L_RECURSIVE,&(x)->CS_DeviceLockInfo)
#define SCSI_Device_Lock(x, func, type) \
	mutex_lock(&(x)->CS_DeviceLock, (type), (func))
#define SCSI_Device_Unlock(x, s) \
	mutex_unlock(&(x)->CS_DeviceLock, (s))
#define SCSI_BusLock_Init(x)    \
	mutex_init(&(x)->LLD_BusLock,L_ADV_SPIN|L_EXCL|L_SPIN|L_RECURSIVE,&(x)-> LLD_BusLockInfo)
#define SCSI_BusLock_Lock(x, func, type) \
	mutex_lock(&(x)->LLD_BusLock, (type), (func))
#define SCSI_BusLock_Unlock(x, s) \
	mutex_unlock(&(x)->LLD_BusLock, (s))
#define SCSI_Op_Lock(spl)	mutex_lock(&CS_SCSI_Op_Pool_Lock, L_SPIN, spl)
#define SCSI_Op_Unlock(ipl)	mutex_unlock(&CS_SCSI_Op_Pool_Lock, ipl)
#else
#define SCSI_Device_Init(x)
#define SCSI_Device_Lock(x, func, type) NULL
#define SCSI_Device_Unlock(x, s)
#define SCSI_BusLock_Init(x)
#define SCSI_BusLock_Lock(x, func, type) NULL
#define SCSI_BusLock_Unlock(x, s)
#endif

/*
 * General Constants
 */
#define TRUE	 	1
#define FALSE	 	0
#define NULL	 	0
#define S_NOERROR	(errno_t)NULL
#define NULLBP		(struct buf *)NULL
#define BOGUS		-1
#define ONE_MILLION	1000000
#define Retry_Delay_Time        2 * CS_Hertz
#define MAX_BLOCK_SIZE  (0x7fffffff >> 9)
#define ASC		1
#define DESC		0

/*
 * Arbitrary max number of WD controllers. Must always be greater than or equal to the
 * constant NUMBER_OF_CONTROLLERS in wd3393.c.
 */
#define MAX_WD_CONTROLLERS			4	
#ifdef WYSE
#define	IO_BUSSES_PER_MACHINE			1
#else 
#define	IO_BUSSES_PER_MACHINE			2
#endif
#define	NUMBER_OF_CONTROLLERS_PER_IO_BUS	8
#define	SCSI_CONTROLLERS_PER_IO_BUS		4
#define	SCSI_BUSSES_PER_CONTROLLER		2

/*
 * Flag bits for general purpose debugging
 */
/* Device type specific drivers */

#define DK_HLD	0x00000001	/* Disk .. High-Level Driver */
#define TP_HLD	0x00000002	/* Disk .. High-Level Driver */
#define PT_HLD	0x00000004	/* Disk .. High-Level Driver */
#define PR_HLD	0x00000008	/* Disk .. High-Level Driver */
#define X4_HLD	0x00000010	/* Type 4 .. High-Level Driver */
#define X5_HLD	0x00000020	/* Type 5 .. High-Level Driver */
#define X6_HLD	0x00000040	/* Type 6 .. High-Level Driver */
#define X7_HLD	0x00000080	/* Type 7 .. High-Level Driver */
#define CH_HLD	0x00000100	/* Type 8 .. High-Level Driver */
#define X9_HLD	0x00000200	/* Type 9 .. High-Level Driver */

/* Generic High-Level Driver */

#define HLD	0x00000400	/* High Level Driver */

/* Hardware Specific drivers */

#define WD_DRVR	0x00000800
#define FP_DRVR	0x00001000
#define HPMSC	0x00002000
#ifdef WYSE
#define MESA_DRVR 0x10000000
#endif
#define cs_DONE 0x02000000
#define CDROM	0x40000000
#define OPTMEM	0x80000000


/* Function Specific Debug */

#define cs_OPEN		0x00004000
#define cs_CLOSE	0x00008000
#define cs_READ		0x00010000
#define cs_WRITE	0x00020000
#define cs_IOCTL	0x00040000
#define cs_STRATEGY	0x00080000
#define cs_INTR		0x00100000
#define cs_TRACE	0x04000000

/* Exceptions */
#define cs_ERR		0x00200000

/* Command Pages */
#define cs_SCSI_2	0x00400000

/* COI Functions */
#define COI		0x00800000
/*
 * Processor Lock Debug
 */
#define cs_LOCK		0x01000000
/*
 * I think errno_t will arrive in types.h someday but until then ...
 */
typedef int errno_t;
/*
 * The internal representation of an errno.

	_____________________________
	|	  |	    |	     |
	| 31 - 24 | 23 - 16 | 15 - 0 |
	|_________|_________|________|
	     |	       |	 |
	     |	       |	 |_____________ Unix errno
	     |	       |_______________________ SCSI Sense Key
	     |_________________________________ Low Level Driver Error Code
 */


#define ERRNO(err) ((errno_t)err & 0x0000ffff)
#define LOW_LEVEL_DRIVER_CODE(err)	((err >> 24) & 0xff)


/*
 *      Low-Level-Driver Error Codes. Shared among hardware drivers.
 */
#define LLDE_NOANS		0x01 /* Device did not answer (re)selection */
#define LLDE_DISCON		0x02 /* Premature Disconnect caused operation failure */
#define LLDE_ILLEGAL_COMMAND	0x03 /* Illegal command sent to controller */
#define LLDE_CHKSNS		0x04 /* A Check Condition caused an operation to fail */
#define LLDE_PARITY_ERROR	0x05 /* Parity Error occurred during transfer */
#define LLDE_XRETRYS		0x08 /* Retrys were exceeded */
#define LLDE_BADMSG		0x10 /* An unexpected message byte was received */
#define LLDE_BADSTAT		0x20 /* An unexpected status byte was received */
#define LLDE_BUSY		0x40 /* Device was busy, operation aborted */
#define LLDE_TIMEOUT		0x80 /* Operation timer expired, aborted */
#define LLDE_CONTROLLER		0x81 /* HPMSC/SMSC controller error, op failed */
#ifdef WYSE
#define splscsi         spl5
#define splscsi_dma     spl5
#else
#ifdef NST
#define splscsi		spl5
#define splscsi_dma	spl5
#else
#define splscsi		spl2
#define splscsi_dma	spl4
#endif
#endif

/*
 * Function extracts/builds errno_t from data in SCSI_Op_t.
 */
errno_t cs_get_error ();
/*
 * Maximum number of SCSI logical units supported per target. SCSI limits
 * this value to 8 ... the lower the better.
 */
#define MAX_LUN_PER_TARGET	4
/* 
 * Operation stack size per LUN
 */
#define	OP_STACK_SIZE	2
/*
 * Number of buffers that may be allocated on behalf of an IO operation.
 */
#define	NUMBER_OF_SCSI_TEMPORARY_BUFFERS 4
/* 
 * Type of reset
 */
#define	BUS_RESET	0
#define	DEVICE_RESET	1
/*
 * Device Identifier structure is used to choose the approriate command set
 * for this device. The choice is made during the initial open().
 */
typedef struct DeviceId {
	/*
	 * First 36 bytes of Inquiry data.
	 */
	unsigned int     Periph_Qualifier        : 3,
		Periph_Device_Type      : 5,
		Removable_Media         : 1,
		Device_Type_Qualifier   : 7,
		ISO_Version             : 2,
		ECMA_Version            : 3,
		ANSI_Version            : 3,
		AENC                    : 1,
		reserved_0              : 3,
		Response_Data_Format    : 4;
	unsigned int     Additional_Length       : 8,
		reserved_1              :16,
		RelAdr                  : 1,
		WBus32                  : 1,
		WBus16                  : 1,
		Sync                    : 1,
		Linked                  : 1,
		reserved_2              : 1,
		CmdQue                  : 1,
		SftReset		: 1;
	char    Vendor_ID[8];
	char    Product_ID[16];
	char    Revision_Level[4];
	/*
	 *	Key fields
	 */
	unsigned int	KF_PeriphQualifier		: 1,
		KF_DeviceTypeQualifier          : 1,
		KF_ISO_Version			: 1,
		KF_ECMA_Version			: 1,
		KF_ANSI_Version			: 1,
		KF_AENC				: 1,
		KF_ResponseDataFormat		: 1,
		KF_AdditionalLength		: 1,
		KF_RelAdr			: 1,
		KF_WBus32			: 1,
		KF_WBus16			: 1,
		KF_Sync				: 1,
		KF_Linked			: 1,
		KF_Reserved			: 1,
		KF_CmdQue			: 1,
		KF_SftReset			: 1,
		KF_VendorIdentification		: 1,
		KF_ProductIdentification	: 1,
		KF_ProductRevisionLevel		: 1,
		KF_Unused			:13;
	void					*Cmd_Page;
	short					*Timeout_Table;
	SCSI_Device_Configuration_t		*Configuration_Table;
	} DeviceId_t;

/*
 * Virtual Device (vdev) Access Macros.
 */

#define VDEV_Inquiry(dp) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_inquiry)(dp)

#define VDEV_Tp_Get_Density(dp) \
		(*((struct Tape_Cmd_Page *)((dp)->Cmd_Page))->vdev_get_density)(dp)

#define VDEV_Erase(dp, Immed, Long_Erase) \
		(*((struct Tape_Cmd_Page *)((dp)->Cmd_Page))->vdev_erase)(dp, Immed, Long_Erase)

#define VDEV_Format(dp, Format_Data, Complete_List, Defect_List_Format, Interleave, Parameter_List, Parameter_List_Length) \
		(*((struct Disk_Cmd_Page *)((dp)->Cmd_Page))->vdev_format)(dp, Format_Data, \
			Complete_List, Defect_List_Format, Interleave, Parameter_List, \
			Parameter_List_Length)

#define VDEV_Load_Unload(dp, Immed, EOT, Reten, Load) \
		(*((struct Tape_Cmd_Page *)((dp)->Cmd_Page))->vdev_load_unload)(dp, Immed, \
		EOT, Reten, Load)

#define VDEV_Mode_Select(dp, page_format, save, buffer, param_list_length) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_mode_select)(dp, \
		page_format, save, buffer, param_list_length)

#define VDEV_Mode_Sense(dp, page_control, page_code, buffer, allocation_length) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_mode_sense)(dp, page_control, \
		page_code, buffer, allocation_length)

#define VDEV_Prevent_Allow(dp, Prevent) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_prevent_allow)(dp, Prevent)

#define VDEV_Read(dp, blk, address, byte_cnt) \
		(*((struct Disk_Cmd_Page *)((dp)->Cmd_Page))->vdev_read)(dp, blk, address, byte_cnt)

#define VDEV_Read_Capacity(dp, PMI, blkno, buffer_address) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_read_capacity)(dp, PMI, blkno, \
		buffer_address)

#define VDEV_Read_Defect_Data(dp, Primary, Grown, Format, Alloc_Length, Address) \
		(*((struct Disk_Cmd_Page *)((dp)->Cmd_Page))->vdev_read_defect_data)(dp, Primary, \
		Grown, Format, Alloc_Length, Address)

#define VDEV_Reassign_Blk(dp, list) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_reassign)(dp, list)

#define VDEV_Rd_Block_Limits(dp, buffer_address) \
		(*((struct Tape_Cmd_Page *)((dp)->Cmd_Page))->vdev_read_block_limits)(dp, \
		buffer_address)

#define VDEV_Release(dp, Third_Party_Release, Third_Party_ID, Extent, Reserv_ID) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_release)(dp, Third_Party_Release, \
		Third_Party_ID, Extent, Reserv_ID)

#define VDEV_Request_Sense(dp) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_request_sense)(dp)

#define VDEV_Reserve(dp, Third_Party_Release, Third_Party_ID, Extent, Reserv_ID, Length, Address) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_reserve)(dp, Third_Party_Release, \
		Third_Party_ID, Extent,Reserv_ID, Length, Addr)

#define VDEV_Rezero(dp) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_rezero)(dp)

#define VDEV_Rewind(dp, Immed) \
		(*((struct Tape_Cmd_Page *)((dp)->Cmd_Page))->vdev_rewind)(dp, Immed)

#define VDEV_Seek(dp, bp) \
		(*((struct Disk_Cmd_Page *)((dp)->Cmd_Page))->vdev_seek)(dp, bp)

#define VDEV_Send_Diagnostic(dp, Self_Test, Dev_Offline, Unit_Offline, address, count) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_send_diagnostic)(dp, \
		Self_Test, Dev_Offline, Unit_Offline, address, count)

#define VDEV_Space(dp, Function_Code, Spacing_Count) \
		(*((struct Tape_Cmd_Page *)((dp)->Cmd_Page))->vdev_space)(dp, Function_Code, \
		Spacing_Count)

#define VDEV_Tp_Set_Block_Length( dp ) \
		(*((struct Tape_Cmd_Page *)((dp)->Cmd_Page))->vdev_set_block_length)( dp )

#define VDEV_Start_Stop(dp, bp) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_start_stop)(dp, bp)

#define VDEV_Test_Ready(dp) \
		(*((struct All_Types_Cmd_Page *)((dp)->Cmd_Page))->vdev_test_ready)(dp)

#define VDEV_Tp_Read(dp, address, byte_cnt, Sili) \
		(*((struct Tape_Cmd_Page *)((dp)->Cmd_Page))->vdev_read)(dp, address, \
		byte_cnt, Sili)

#define VDEV_Tp_Write(dp, address, byte_cnt) \
		(*((struct Tape_Cmd_Page *)((dp)->Cmd_Page))->vdev_write)(dp, address, \
		byte_cnt)

#define VDEV_Tp_Verify(dp, address, Immed, Byte_Compare, Verify_Length) \
		(*((struct Tape_Cmd_Page *)((dp)->Cmd_Page))->vdev_verify)(dp, address, \
		Immed, Byte_Compare, Verify_Length)

#define VDEV_Dk_Verify(dp, Block_Number, Number_Of_Blocks, Byte_Compare, Address) \
		(*((struct Disk_Cmd_Page *)((dp)->Cmd_Page))->vdev_verify)(dp, Block_Number, \
		Number_Of_Blocks, Byte_Compare, Address)

#define VDEV_Write(dp, blk, address, byte_cnt) \
		(*((struct Disk_Cmd_Page *)((dp)->Cmd_Page))->vdev_write)(dp, blk, address, byte_cnt)

#define VDEV_Wrt_Fmark(dp, num, setmark, Immed) \
		(*((struct Tape_Cmd_Page *)((dp)->Cmd_Page))->vdev_write_filemark)(dp, num, setmark, \
		Immed)
#define VDEV_Flex_Set_Block_Length( dp, control ) \
		(*((struct Disk_Cmd_Page *)((dp)->Cmd_Page))->vdev_set_flex)(dp, control )

#define VDEV_Sync_Buffer(dp) \
		(*((struct Printer_Cmd_Page *)((dp)->Cmd_Page))->vdev_synchronize_buffer)(dp)

#define VDEV_Print(dp, addr, count) \
		(*((struct Printer_Cmd_Page *)((dp)->Cmd_Page))->vdev_print)(dp, addr, count)

#define VDEV_Printer_Setup(dp) \
		(*((struct Printer_Cmd_Page *)((dp)->Cmd_Page))->vdev_setup)(dp)

/*
 * General driver operations.
 */

#define VDEV_State(dp, bp) \
		((*((dp)->Cmd_Page))->vdev_state)(dp, bp)

#define VDEV_Mode(dp, bp) \
		((*((dp)->Cmd_Page))->vdev_mode)(dp, bp)

#define VDEV_Tally(dp, bp) ((*((dp)->Cmd_Page))->vdev_tally)(dp, bp)

struct All_Types_Cmd_Page {
	struct SCSI_Op	*(*vdev_test_ready)();
	struct SCSI_Op	*(*vdev_request_sense)();
	struct SCSI_Op	*(*vdev_inquiry)();
	struct SCSI_Op	*(*vdev_reserve)();
	struct SCSI_Op	*(*vdev_release)();
	struct SCSI_Op	*(*vdev_mode_sense)();
	struct SCSI_Op	*(*vdev_mode_select)();
	struct SCSI_Op	*(*vdev_send_diagnostic)();
	struct SCSI_Op	*(*vdev_receive_diagnostic)();
	/*
	 * Functions from here down are not implemented for all device types,
	 * but when implemented, one function suffices for all types which 
	 * support the command.
	 */
	struct SCSI_Op	*(*vdev_prevent_allow)();
	struct SCSI_Op	*(*vdev_read_capacity)();
	struct SCSI_Op	*(*vdev_reassign)();
	struct SCSI_Op	*(*vdev_rezero)();
	};

struct Disk_Cmd_Page {
	struct All_Types_Cmd_Page all;
	struct SCSI_Op	*(*vdev_format)();
	struct SCSI_Op	*(*vdev_read)();
	struct SCSI_Op	*(*vdev_read_defect_data)();
	struct SCSI_Op	*(*vdev_start_stop)();
	struct SCSI_Op	*(*vdev_synchronize_cache)();
	struct SCSI_Op	*(*vdev_verify)();
	struct SCSI_Op	*(*vdev_write)();
	struct SCSI_Op	*(*vdev_set_flex)();
	struct SCSI_Op	*(*vdev_user0)();
	struct SCSI_Op	*(*vdev_user1)();
	struct SCSI_Op	*(*vdev_user2)();
	struct SCSI_Op	*(*vdev_user3)();
	struct SCSI_Op	*(*vdev_user4)();
	struct SCSI_Op	*(*vdev_user5)();
	};

struct Tape_Cmd_Page {
	struct All_Types_Cmd_Page all;
	struct SCSI_Op	*(*vdev_erase)();
	struct SCSI_Op	*(*vdev_load_unload)();
	struct SCSI_Op	*(*vdev_read)();
	struct SCSI_Op	*(*vdev_read_block_limits)();
	struct SCSI_Op	*(*vdev_rewind)();
	struct SCSI_Op	*(*vdev_space)();
	struct SCSI_Op	*(*vdev_verify)();
	struct SCSI_Op	*(*vdev_write)();
	struct SCSI_Op	*(*vdev_write_filemark)();
	struct SCSI_Op  *(*vdev_set_block_length)();
	int		 (*vdev_get_density)();
	struct SCSI_Op	*(*vdev_user1)();
	struct SCSI_Op	*(*vdev_user2)();
	struct SCSI_Op	*(*vdev_user3)();
	struct SCSI_Op	*(*vdev_user4)();
	struct SCSI_Op	*(*vdev_user5)();
	};

struct Printer_Cmd_Page {
	struct All_Types_Cmd_Page all;
	struct SCSI_Op	*(*vdev_format)();
	struct SCSI_Op	*(*vdev_print)();
	struct SCSI_Op	*(*vdev_slew_and_print)();
	struct SCSI_Op	*(*vdev_stop_print)();
	struct SCSI_Op	*(*vdev_synchronize_buffer)();
	struct SCSI_Op	*(*vdev_setup)();
	struct SCSI_Op	*(*vdev_user0)();
	struct SCSI_Op	*(*vdev_user1)();
	struct SCSI_Op	*(*vdev_user2)();
	struct SCSI_Op	*(*vdev_user3)();
	struct SCSI_Op	*(*vdev_user4)();
	struct SCSI_Op	*(*vdev_user5)();
	};


typedef struct        Cache_Tape      {
	int     		Cache_Size;
	int     		Cache_Error;
	int     		Cache_Start;
	int     		Cache_End;
	SCSI_Sense_Data_t	*Cache_Sense;
	unsigned int			Filemark_Found	: 1,
				End_Of_Media	: 1,
				Cache_Write	: 1,
	     			pad		:29;
	caddr_t 		Cache_Buffer;
} Cache_Tape_t;

/*
 *      The following structure is a means to keep track of the memory allocated
 *      so that when you free a device (dev_info) everything can be cleaned up
 *      without requiring either cs.c to know about all the alloc's or to call
 *      a device specific command page.  All operations that allocate data in the
 *      SCSI driver that do not immediately free the data should call SCSI_Zalloc and
 *      SCSI_Free to manage this list.
 */

#define SCSI_MEMORY_SIZE        16
struct  SCSI_Memory     {
	int     Size;
	caddr_t Address;
};

/* 
 * SCSI operation currently executing on an LUN 
 */
struct Current_Op {
	char 	status;
	char	head;
	struct	SCSI_Op		*entry[OP_STACK_SIZE];
};
#ifdef WYSE
struct Mesa_Current_Ops {
	char		status;
	struct SCSI_Op	*entry;
};
#endif

typedef struct P_OpenInfo {
	int	Open_Type;
	int	Layered_Open;
} P_OpenInfo_t;

/*
 * This is everything you ever wanted to know about the SCSI unit.
 * There is one and only one structure per unique SCSI PHYSICAL thingamajig.
 */
typedef struct SCSI_Device {
	struct SCSI_Device	*Device_forw;	/* doubly-linked list of dev's*/
	struct SCSI_Device	*Device_back;	/* on hash table	      */
	struct	SCSI_Op		*s_forw;	/* doubly-linked list of all  */
	struct	SCSI_Op		*s_back;	/* op's on queue (sorted)     */
	struct	SCSI_Op		*sort_forw;	/* doubly-linked list of      */
	struct	SCSI_Op		*sort_back;	/* sortable (rd/wr) commands  */
	struct	SCSI_Op		*PrivateOp;	/* queue of private ops       */
	int			 QueueSize;	/* DEBUG - size of dev queue  */
	dev_t			dev;		/* OS Dev		      */
	int	Is_Open;			/* bit per partition open.HLD */
	int	EXCL_Open;			/* bit per partition EXCL.HLD */
	unsigned int	First_Open	:1,	/* the first open of a device */
		Opening			:1,	/* HLD in cs_open code    HLD */
		Forget_On_Close		:1,	/* Remove the dev on closeHLD */
		ReadInfoOnClose		:1,	/* Reread device info     HLD */
		Write_Protected		:1,	/* Is this device WP?     HLD */
		Busy			:1,	/* Is this device BUSY    LLD */
		Initiator		:1,	/* Is this device an Initiator*/
		Filemark_Found  	:1,	/* was Filemark found?   TAPE */
		End_Of_Media    	:1,	/* was EOM Found?        TAPE */
		No_Data_Found   	:1,	/* was any data found?   TAPE */
		Rewound         	:1,	/* was a rewind done?    TAPE */
		Written         	:1,	/* was anything written? TAPE */
		Middle_Read     	:1,	/* Was a read issued?	 TAPE */
		Fixed			:1,	/* Fixed or variable?    TAPE */
		Caching			:1,	/* is this driver caching?    */
		Byte_Order_Exception	:1,	/* tells LLD if device swaps  */
		Sort_Direction		:1,	/* if sorting which way	  HLD */
		Sort_Device		:1,	/* can this device sort?  HLD */
		NegotiateSynchronous    :1,	/*         		  LLD */
		NegotiateQueueing       :1,	/*         		  LLD */
		NegotiateWide           :1,	/*          		  LLD */
		ReadyToNegotiate	:1,	/* HLD tells LLD it can neg.  */
		LowLevelDriverLocked	:1,	/* HLD can lock LLD for dev.  */
		FirstLUN		:1,	/* First LUN dev for PUN. HLD */
		pad                     :8;	/*                            */
	Partition_Identifier_t	SCSI_device;	/* Common SCSI idea of minor# */
	struct SCSI_Device	*Bus_forw;	/* doubly-linked list of dev's*/
	struct SCSI_Device	*Bus_back;	/* on this SCSI Bus           */
	struct SCSI_Device	*Next_Device;	/* Initiator-where to continue*/
	struct SCSI_Device	*Bus_Info;	/* Pointer back to Initiator  */
	int			block_size;	/* device's block size	      */
	int			previous_key;	/*                    SORTING */
	struct SCSI_Op		*turnaround;	/*                    SORTING */
	int			driver;		/* which LLD for this device  */
	void			*Cmd_Page;	/* which command page	      */
	short			*Timeout_Table;	/* which timeout table        */
	struct SCSI_Sort_Table	*sort_funcs;    /* if sortable-which algorithm*/
	struct SCSI_Op		*request_sense_op; /* ptr to sense op         */
	SCSI_Inquiry_Data_t	inquiry_data;	/* Device's inquiry data      */
	SCSI_Sense_Data_t       sense_data;	/* where sense data stored    */
	StatStructure_t		Statistics;	/* Device's statistics	      */
	P_OpenInfo_t		PartitionOpenInformation[32];
/* MP Stuff */
#ifdef MP
	mutex_t                 CS_DeviceLock;
	lockinfo_t              CS_DeviceLockInfo;
	unsigned int		CS_InitStatus;
#endif

	int                     Block_Length;   /* Length in bytes of each logical block */
	int                     Fragment_Mask;  /* Block Length -1 (assumes power-of-2 length) */
	struct SCSI_Op          **CommandQueue; /* All of the queued commands */
	int                     NumberOfCommandsQueued;
	int 			debug_tag1;	/* makes DEBUG easy	      */
	SCSI_Device_Configuration_t	*SCSI_Device_Configuration;
	SCSI_Device_Configuration_t	CurrentConfiguration;
	union	Driver_Specific_Info {
		struct WD_Driver {
			char	operation_level;
			struct	Current_Op	current_op;
		} WD_Driver;
		struct SIOP_Driver {
			char			operation_level;
			char			status;
			struct	SCSI_Op		*current_op;
			unsigned long		*SCSI_SCRIPT; 		/*SCSI Script area */
			unsigned long		*SENSE_SCRIPT; 		/*SENSE SCSI Script area */
		} SIOP_Driver;
		struct HA_Driver {
			int	x;
		} HA_Driver;
		struct FP_Driver {
			int x;
		} FP_Driver;
		struct SIMPL_Driver {
			struct	SCSI_Op		*current_op;
			struct	SCSI_Op		*untagged_op;
			struct	SCSI_Op		*auto_sense_op;
			void			*target_info;
			void			*busctrl;
			unsigned char		msg_bfr[8];
		} SIMPL_Driver;
#ifdef WYSE
		struct MESA_Driver {
                        int     			ffffffff;
                        struct  Mesa_Current_Ops	current_op[10];
                        u_char  			rqindex;
                        u_char  			cmdindex;
                        u_char  			pkindex;
                        u_char  			mxpacket;
                        int     			pk_count;
                        drp_t   			**packets;
                        iat_t   			**iats;
                        mrq_t   			**rqpkts;
	      } MESA_Driver;
#endif
	} Driver;
	int			debug_tag2;	/* makes DEBUG easy	      */
	union	Device_Specific_Info {
		struct Hard_Disk {
			Disk_Control_Block_t	*Disk_Control_Block_Ptr;
			DiskGeometry_t		 Geometry;
			int			current_mode;
		} Hard_Disk;
		struct Tape_Specific {
			int		Tape_Position;
			int		Maximum_Tape_Size;
			int		Maximum_Block_Length;
			int		Minimum_Block_Length;
			Cache_Tape_t    Cache;
#ifdef MP
			mutex_t         Cache_Lock; 
			lockinfo_t	Cache_LockInfo;
			unsigned int	Cache_LockStatus;
#endif
		} Tape;
		struct Additional_Device {
			/* Allows additional devices to be added        */
			/* and linked without re-compling               */
			/* full SCSI driver as long as structure        */
			/* added is less than 40 bytes.                 */
			int		your_variables[10];
		} Additional_Device;
	} type;
	int			debug_tag3;	/* makes DEBUG easy	      */
	struct SCSI_Memory	SCSI_Memory_Allocated[SCSI_MEMORY_SIZE];
} SCSI_Device_t;
	
/*
 * Define to assign transfer lengths and count that cross an integer boundary.
 */

#ifdef BigEndian
#define	Assign_Byte2_1_and_Byte0(Byte2_1, Byte0, Value)	Byte2_1=Value>>8; Byte0=Value&0xff;
#else
#define	Assign_Byte2_1_and_Byte0(Byte2_1, Byte0, Value)	Byte2_1=swap_bytes(Value>>8); Byte0=Value&0xff;
#endif

typedef struct SCSI_Sort_Table {
	struct  SCSI_Op		*(*sort_queue)();
	void			(*change_sort_key)();
	void			(*finishup_sort_op)();
	int			(*retry_op)();
} SCSI_Sort_Table_t;
/*
 * Hashing stuff for locating SCSI_Device structures.
 */
typedef struct SCSI_Device_Hash {
	SCSI_Device_t	*Device_forw;
	SCSI_Device_t	*Device_back;
} SCSI_Device_Hash_t;

/*
 * Number of hash buckets for hashing of SCSI_Device structures.
 * Must be a power of 2 value.
 */
#define SCSI_BUCKET_COUNT	64
#define SHMASK	(SCSI_BUCKET_COUNT -1)
/*
 * SHSHIFT discards partition bits.
 */
#define SHSHIFT			4

#ifdef SVR40
#define SCSI_DEV_HASH(OS_Device) ((SCSI_Device_t *)(&SCSI_Device_Hash_Table[(OS_Device >> SHSHIFT) & SHMASK]))
#else
#define SCSI_DEV_HASH(OS_Device) ((SCSI_Device_t *)(&SCSI_Device_Hash_Table[(OS_Device >> 14) & 0x7]))
#endif

/*
 * Linked-List I/O vector structure for scatter-gather where necessary.  Count, address, JUMP, and
 * forward pointer (s_iov) must remain in the order or will break 53c700.  cew
 */

#define	iov_forw	s_iov
#define	iov_flag	s_addr
#define	iov_freecount	s_bcount

typedef struct SCSI_IO_Vector {
	int			s_bcount;
	caddr_t		 	s_addr;
	struct SCSI_IO_Vector	*s_iov;
	union   VectorUnion {
		struct Sscript {
			int			s_bcount;	/* SCSI SCRIPT::  MOVE s_bcount, s_addr */
			paddr_t			s_addr;
			int			JUMP;		/* SCSI SCRIPT::  JUMP NEXT */
			paddr_t			NEXT;
		}SSCRIPT;
		struct Simpl {
			int			residual;
			paddr_t			s_addr;
			struct SCSI_IO_Vector	*last_iov;
		} SIMPL;
		struct FreeInfo {
			struct SCSI_IO_Vector	*h_forw;
			struct SCSI_IO_Vector	*h_back;
			struct SCSI_IO_Vector	*iov_back;
			int			iov_maxcount;
		}FreeInfo;
	} VU;
	struct SCSI_IO_Vector			*Free_Header;
	struct buf				*bp;
	int					s_total_count;
	int					iov_wanted;
} SCSI_IO_Vector_t;

/*
 * The structure to hold all pointers necessary for disconnect/reconnect
 */

typedef struct Save_SCSI_Pointers {
	SCSI_IO_Vector_t        *s_iov_ptr;
	caddr_t                 s_addr;
	int                     s_bcount;
	int			s_total_count;
} Save_SCSI_Pointers_t;

/*
 * The true, although ephemeral, meaning of the l_dev_t.
 */
#ifdef BigEndian
#ifdef tower32_700
typedef struct OS_Device {
     unsigned int	OS_Major        : 8,
		IO_Bus          : 3,
		SCSI_Controller : 4,
		SCSI_Bus        : 3,
		PUN             : 4,
		LUN             : 3,
		reserved        : 3,
		Partition       : 4;
	} OS_Device_t;
#else
typedef struct OS_Device {
	unsigned int     OS_Major        : 14,
		IO_Bus          : 3,
		SCSI_Controller : 3,
		SCSI_Bus        : 2,
		LUN             : 3,
		PUN             : 3,
		Partition       : 4;
	} OS_Device_t;
#endif
#else
typedef struct OS_Device {
	unsigned int	Partition       : 4,
		PUN             : 4,
		LUN             : 3,
		SCSI_Bus        : 2,
		SCSI_Controller : 3,
		IO_Bus          : 2,
	     	OS_Major        : 14;
	} OS_Device_t;
#endif
typedef struct tmp_buffer {
	int	 s_size;
	void	*s_addr;
} temp_buffer_t; 
/*
 * The scsi_op structure includes all info needed to perform an I/O operation.
 */
typedef struct	SCSI_Op {
	struct buf		*s_bp;
	SCSI_Device_t		*s_dev;
	struct	SCSI_Op		*s_forw;
	struct	SCSI_Op		*s_back;
	struct  SCSI_Op		*sort_forw;
	struct  SCSI_Op		*sort_back;
	struct  SCSI_Op		*h_forw;
	struct  SCSI_Op		*h_back;
	unsigned int 		 	S_ACTIVE		: 1,	/* This operation on the lu queue */
				S_DONE			: 1,
				S_ERROR			: 1,
				S_ASYNC			: 1,	/* This op to be freed by cs_iodone */
				S_NO_LOG		: 1,	/* Don't log errors for this op */
				S_DELAY			: 1,	/* Delay retries (Unit Busy etc.) */
				S_0			: 1,
				S_1			: 1,
				S_2			: 1,
				S_3			: 1,
				S_4			: 1,
				S_5			: 1,
				S_6			: 1,
				S_7			: 1,
				S_8			: 1,
				S_9			: 1,
				S_10			: 1,
				S_11			: 1,
				S_12			: 1,
				S_13			: 1,
				S_14			: 1,
				S_15			: 1,
				S_16			: 1,
				S_FREE			: 1,
				Byte_Order_Exception	: 1,
				Concat			: 1,
				Concatable		: 1,
				Static			: 1,
				Save			: 1,
				DIR_NONE		: 1,
				DIR_DEV_TO_MEM		: 1,
				DIR_MEM_TO_DEV		: 1;
	SCSI_Command_Block_t	 s_scsi_cmd_blk;
	unsigned char	 	 s_op_state;		/* Current state of operation */
	unsigned char	 	 s_queue_type;		/* Queue Tag Message type */
	int			 s_retrys;		/* Retry counter for command */
	int			 s_delay_time;		/* Clock ticks to wait before retrying */
	int			 s_maxretrys;		/* Maximum number of retrys allowed for this command */
	int                      s_block_number;        /* (FOR SORTING) starting block number */
	int                      s_cylinder;            /* (FOR SORTING) starting cylinder number */
	int			 s_timeout;		/* Timeout in seconds for operation */
#define	op_freecount		 s_timeout
#define op_wanted		 s_timeout
	int			 s_timer;		/* Storage for current operation timer */
	int			 (*s_coi)();		/* Function to call on interrupt */
	int			 s_residual;		/* DataNotTransferredPriorToCompletion */
	struct SCSI_Op          *Free_Header;
	struct SCSI_Op          *concat_forw;
	struct SCSI_Op          *concat_back;
	SCSI_IO_Vector_t	 s_iov;			/* IO Vector (LinkPointer, Addr, ByteCnt)*/
	SCSI_IO_Vector_t	*s_iov_ptr;
	SCSI_IO_Vector_t	*s_iov_last;
	int			 s_total_count;		/* Total transfer byte count		*/
	int			 s_save_count;
#define	op_maxcount		 s_total_count
	temp_buffer_t		 s_error_record_ptr;	/* Pointer to the error record from kern */
	int			 s_error;		/* LLD's error code and os error code */
#define	op_flag			 s_error
	int			 s_iostart;		/* for accounting info */
	temp_buffer_t		 s_temp[NUMBER_OF_SCSI_TEMPORARY_BUFFERS];
	union	Driver_Specific_Info_Per_SCSI_Op {
		struct SIOP_Op {
			unsigned long		RestartSCRIPT;	
			unsigned long		PMM_Move;
			unsigned long		PMM_ReturnAddress;
			unsigned long		PMM_ScriptAddress;
			unsigned long		PMM_ByteCountLeft;
			SCSI_IO_Vector_t	IOV;		/* IOV to hold PhaseMisMatch Info.*/
			unsigned long		*SCSI_SCRIPT; 	/*SCSI Script area */
			struct PhysicalAddresses {
				paddr_t	 SCSI_Message_Bytes;
				paddr_t	 SCSI_Status_Byte;
				paddr_t	 s_scsi_cmd_blk;
			} PA;
		} SIOP;
		struct WD_Driver_Op {
			char SCSI_DMA_state[MAX_WD_CONTROLLERS * 2];
			char WD_Command_Type;
 			char SCSI_Phase;
		} WD_Driver_Op;
		struct HA_Driver_Op {
			struct SCSI_Op *av_forw;
		} HA_Driver_Op;
		struct FP_Driver_Op {
			int x;
		} FP_Driver_Op;
		struct SIMPL_Op {
			unsigned char		sequence_start;
			unsigned char		flags;
			int			Qtag_nbr;
			struct SCSI_Op		*op_with_sense;
			int			wide_residue;
		} SIMPL_Op;
	} Driver_Op;
	Save_SCSI_Pointers_t	Save_SCSI_Pointers;
	unsigned char		SCSI_Status_Byte;
	int 			SCSI_Message_Size;
	unsigned char		SCSI_Message_Bytes[12];
	SCSI_Sense_Data_t	*s_sense;
	int 			s_owner_id;	/* Something for LLCS driver */
} SCSI_Op_t;

#define	KEEP		0
#define	DONT_KEEP	1

/*
 * Action specified by device specific driver via s_coi function.
 */

#define COI_NO_ACTION	0	/* Don't do anything specific, just let nature take its course	*/
#define COI_RETRY	1	/* requests a retry but op->s_maxretrys and op->s_retrys are higher precedence. */
#define COI_NO_RETRY	2	/* coi function has determined that this op should not be retried.*/
#define COI_EXIT	3	/* coi function has determined  that cs_iodone should return immediately without releasing the op and without retrying */

/*
 * The field s_op_state is set to the value of "INITIAL_STATE" by the cs_alloc_SCSI_Op
 * routine.
 */
#define INITIAL_STATE   5

/*
 * Macros for manipluation of the SCSI_Op_t.
 */

/*
 * Insert request at head of queue.
 */
#define cs_insert_head(dp, op) { \
	register int queipl; \
	register int opl = splscsi(); \
	(op)->s_timeout = (dp)->Timeout_Table[(op)->s_scsi_cmd_blk.cmd[0]];\
	queipl=SCSI_Device_Lock(dp, splscsi, L_SPIN); \
	(dp)->s_forw->s_back = (op); \
	(op)->s_forw = (dp)->s_forw; \
	(dp)->s_forw = (op); \
	(op)->s_back = (SCSI_Op_t *)(dp); \
	(dp)->QueueSize++; \
	(dp)->Statistics.SCSI_Stats.ios.io_qcnt++; \
	SCSI_Device_Unlock(dp, queipl); \
	splx(opl); \
}
/*
 * Hang request on tail of device queue.
 */
#define cs_insert_tail(dp, op) { \
	register int queipl; \
	register int opl = splscsi(); \
	(op)->s_timeout = (dp)->Timeout_Table[(op)->s_scsi_cmd_blk.cmd[0]];\
	queipl=SCSI_Device_Lock(dp, splscsi, L_SPIN); \
	(dp)->s_back->s_forw = (op); \
	(op)->s_back = (dp)->s_back; \
	(dp)->s_back = (op); \
	(op)->s_forw = (SCSI_Op_t *)(dp); \
	(dp)->QueueSize++; \
	(dp)->Statistics.SCSI_Stats.ios.io_qcnt++; \
	SCSI_Device_Unlock(dp, queipl); \
	splx(opl); \
}

/*
 * Remove request from queue.
 */
#define cs_remove_queue(op) { \
	register int queipl; \
	register int opl = splscsi(); \
	queipl=SCSI_Device_Lock(op->s_dev, splscsi, L_SPIN); \
	(op)->s_back->s_forw = (op)->s_forw; \
	(op)->s_forw->s_back = (op)->s_back; \
	(op)->s_forw = 0; \
	(op)->s_back = 0; \
	if (op->sort_back) { \
		(op)->sort_back->sort_forw = (op)->sort_forw; \
	} \
	if (op->sort_forw) { \
		(op)->sort_forw->sort_back = (op)->sort_back; \
	} \
	(op)->sort_back = 0; \
	(op)->sort_forw = 0; \
	(op)->s_dev->QueueSize--; \
	SCSI_Device_Unlock(op->s_dev, queipl); \
	splx(opl); \
}

/*
 * Determine enqueued-ness of op.
 */
#define is_on_a_queue(op) (op->s_back != 0)

#define COMPAT_REQ(op1,op2)     ((op1->DIR_NONE == op2->DIR_NONE) && (op1->DIR_DEV_TO_MEM == op2->DIR_DEV_TO_MEM) && (op1->DIR_MEM_TO_DEV == op2->DIR_MEM_TO_DEV))

/*
 * spec fix -- not checking concatable status for second op - for now
 * limit operations to get to 48k - alan
 */

#define CONCAT_REQ(op1,blk_size,op2)	\
((op1->s_block_number + op1->s_total_count / blk_size == op2->s_block_number) \
&& (op1->S_ASYNC) && (op2->S_ASYNC) && (op1->Concatable) && (op2->Concatable) \
&&((op1->s_total_count+op2->s_total_count)<=(48*1024)))

#define	CONCAT_SORT	0x01
#define	CYLINDER_SORT	0x02
/*
 * Function declarations.
 */
SCSI_Device_t		*cs_makedev();
SCSI_Op_t 		*cs_alloc_SCSI_op();

/*
 * Device switch table used to reference the functions which are unique
 * to a certain SCSI device type such as reading of the "bootblock" for
 * a disk or rewinding a tape on open.
 */
struct csdevsw {
	int		(*d_open)();
	int		(*d_close)();
	int		(*d_read)();
	int		(*d_write)();
	SCSI_Op_t *	(*d_strategy)();
	int		(*d_ioctl)();
	int		(*d_init)();
	int		(*d_size)();
	int		(*d_pfr)();
	void		(*d_comp)();
	int		d_flag;
};
extern struct csdevsw csdevsw [];
/*
 * Switch table used to access a LLD entry point such as "xxhw_start".
 * Note that synchronous functions return errno_t.
 */

struct LLD_Switch {
	errno_t	(*d_init)();
	errno_t	(*d_open)();
	void	(*d_start)();
	errno_t	(*d_pfr)();
	errno_t	(*d_configure)();
	int	(*d_intr)();
};
extern struct LLD_Switch LLD_Switch [];

#define LLD_Configure(dev_info) ((*LLD_Switch[dev_info->driver].d_configure)(dev_info))
#define	LLD_Init(driver,io_bus)	((*LLD_Switch[driver].d_init)(driver,io_bus))
#define LLD_Open(dev_info) ((*LLD_Switch[dev_info->driver].d_open)(dev_info))
#define LLD_Start(op) (op->S_ERROR ? cs_iodone(op) : ((*LLD_Switch[op->s_dev->driver].d_start)\
			(op->s_dev, op)))
#define LLD_Pfr(driver) ((*LLD_Switch[driver].d_pfr)(driver))
#define LLD_Intr(io_bus, ctrl) ((*LLD_Switch[dev_info->driver].d_intr)(io_bus, ctrl))
/*
 * Default number of Low Level Drivers configured.
 */
#ifdef WYSE
#define NUMBER_OF_LLD           1
#else
#define NUMBER_OF_LLD           3
#endif

/*
 * MACRO definitions 
 */
/*
 * Inquiry data interpretation/manipulation macros. The variable devinfo is a pointer
 * to SCSI_Device_t structure.
 */
#define NOT_PRESENT(devp)	(devp->inquiry_data.Periph_Qualifier != 0)
#define REMOVABLE(devp)	(devp->inquiry_data.Removable_Media)
#define SCSI_DAD(devp)		(devp->inquiry_data.Periph_Device_Type == SCSI_DAD_TYPE)
#define SCSI_SAD(devp)		(devp->inquiry_data.Periph_Device_Type == SCSI_SAD_TYPE)
#define SCSI_TYPE(devp)	(devp->inquiry_data.Periph_Device_Type)
#define IS_FLEX(devp)		(SCSI_DAD(devp) && REMOVABLE(devp))


#define FLEX_MODE(devp) (devp->type.Hard_Disk.current_mode)

#define IS_OPEN(dp) (dp->Is_Open)

#define PARTITION_START(devp, part) \
	(devp->type.Hard_Disk.Disk_Control_Block_Ptr->Partition_Control_Blocks[part].Partition_Start)

#define PARTITION_SIZE(devp, part) \
	(devp->type.Hard_Disk.Disk_Control_Block_Ptr->Partition_Control_Blocks[part].Partition_Size)

#define LAST_BLK_IN_PARTITION(devp, part) \
	(devp->type.Hard_Disk.Disk_Control_Block_Ptr->Partition_Control_Blocks[part].Partition_Size + \
	PARTITION_START(devp, part) - 1)
/*
 * Perhaps we should do a Mk_Part_ID(unix_dev) ?? Who knows.
 */
#ifndef tower32_700
#define GET_PARTITION(unix_dev) (unix_dev & 0x0f)
#else
#define GET_PARTITION(unix_dev) ((unix_dev >> 2) & 0x0f)
#endif

/* common macros  -- NOTE: THESE ARE NOW FUNCTIONS */

#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) < (b) ? (b) : (a))
#define LONG_ALIGN(a)	(if(a & 3) {a &= ~3;a += 4; })

#ifndef SVR40
#define ptob            ctob
#define btop            btoct
#define btopr           btoc
#endif


typedef struct SCSI_BUSInfo {
	SCSI_Device_t 	*device_info;		/* Pointer to initiator's information	*/
	unsigned char	SCSI_DMA_Channel;	/* Current Host Adaptor DMA_Channel	*/
	unsigned int	flags;			/* Bus flags for reset conditions	*/
	unsigned char	state;			/* Bus state flag			*/
	unsigned char	mode;			/* Initial SCSI machine mode		*/
	unsigned char	current_pun;		/* Current target scsi id		*/
	unsigned char	current_lun;		/* Current target logical unit number	*/
	unsigned char	scsi_id;		/* SCSI bus id (PUN) of this processor	*/
	unsigned int	base_addr_offset;	/* Offset of Bus (LLD Dependant)	*/
/* MP STUFF */
#ifdef MP
	mutex_t         LLD_BusLock;
	lockinfo_t      LLD_BusLockInfo;
	unsigned int	LLD_InitStatus;
#endif

} SCSI_BUSInfo_t;

typedef struct SCSI_CtrlInfo {
	unsigned char	Initialized;		
	unsigned int	driver;
	unsigned int	flags;	
	int		BaseAddress;		/* Current Host Adaptor Base Address	*/
	unsigned int	SCSI_Vector;		/* Current Host Adaptor Vector Number	*/
	unsigned char	HostID_Low;
	unsigned char	HostID_High;
	unsigned char	SlotNumber;		/* This is the slot number in the bus	*/
	unsigned int	NumberOfBusses;
	struct   SCSI_BUSInfo *SCSI_BUSInfo[2];
	union	Controller_Specific_Info {
	   struct ncr53c90 {
	      unsigned char	haPOS0;
	      unsigned char	haPOS1;
	   } ncr53c90;
	   struct ncr53c700{
	      int		ClockFrequencyBits;
	      int		tcp;
	      unsigned char 	haPOS0;
	      unsigned char 	haPOS1;
	   } ncr53c700;
	   struct SIMPL {
	      unsigned int just_junk_now;
	   } SIMPL;
	} Controller;
	caddr_t		AdditionalInformation;
} SCSI_CtrlInfo_t;

typedef struct IO_BusInfo {
	unsigned char	state;			/* present? */
	unsigned char	bus_type;		/* MCA, MultiBus I, etc.. */
	unsigned char	mode;			/* PRIMARY or EXTENDED MCA BUS? */
	unsigned int	number_of_slots;
	struct   SCSI_CtrlInfo *SCSI_CtrlInfo[NUMBER_OF_CONTROLLERS_PER_IO_BUS];
	caddr_t		AdditionalInformation;
} IO_BusInfo_t;

#define	IO_BUS_PRESENT			0x01
#define IO_BUS_NOT_PRESENT		0x02
	
/*
 * 	Flags Used By SCSI_BusInfo.flags
 */


#define	F_PFR_IN_PROGRESS		0x01	/* 1 Bit for power fail recovery in progress */
#define	F_SCSI_BUS_RESET		0x02	/* 1 Bit for SCSI Bus reset */
#define	F_SELECTING			0x04	/* 1 Bit for Selection in Progress Flag */
#define	F_WDRESET			0x08	/* 1 Bit for wd3393 reset in progress Flag */
#define	F_ADVANCED_MODE			0x10	/* 1 Bit for wd3393 in advanced mode */
#define	F_SYNCHRONOUS_HANDSHAKING	0x20	/* 1 Bit for handshaking in progress */
#define	F_PRESENT			0x40	/* wd3393 is present on board or submodule */
#define	F_BUS_DEVICE_RESET		0x80	/* 1 Bit for Bus Device Reset in progress */
#define	F_RESELECTING			0x100	/* Waiting for a device to reselect	*/

/*
 *	Defines for the type of Bus
 */
#define	MICRO_CHANNEL	0x00
#define	PARALLEL_BUS	0x01
#define	MULTI_BUS1	0x02
#define	MULTI_BUS2	0x03
#define	ON_BOARD	0x04
#ifdef WYSE
#define M_BUS           0x99
#define VME             0x98
#endif

/*
 *	Defines modes for bus
 */
#define	PARALLEL_PRIMARY	0x00
#define	MCA_PRIMARY		0x00
#define	MCA_EXTENDED		0x01

/*
 *	System Hardware Stuff.  We need to know if a Parallel Bus might be in the
 *	system, if one might be in the system then the HLD will have to allocate
 *	resources for it; if FALSE then ignore the Parallel Bus.
 */

#define	CHECK_FOR_PARALLEL_BUS	TRUE

/*
 * Beginning of Statistics and Tallies Circular Queues
 */
SCSI_Device_t		*SCSI_Stat_Ptr;
SCSI_Tallies_t		*SCSI_Tally_Ptr;

#endif

#ifdef PERF_PART_ON

#define PERF_PART        0x0f   /* Last partition of device is NULL Partition */

extern unsigned int      cs_perf_part;

#define PERF_RAW         0x01   /* Return after read or write */
#define PERF_STRAT       0x02   /* Return in beginning of strategy */
#define PERF_COMMON_SCSI 0x04   /* Return just before touching hardware */
#endif
