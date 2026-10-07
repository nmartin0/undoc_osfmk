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
/*	Copyright 1990 NCR Corporation - Dayton, Ohio, USA	*/
#ident  "@(#)./usr/src/arch/mca/uts/i386/sys/scsi:SCSI.h	1.17"

#ifndef _SYS_SCSI_H
#define _SYS_SCSI_H
#include <sys/types.h>

/*
 * Definitions that are shared by the SCSI driver, libio and applications.
 */

typedef long minor_t;  /* added by jerry */
#define NODEV (dev_t)(-1)      /* added by jerry */

typedef struct SCSI_Command_Block {
	unsigned char	cmd[12];
} SCSI_Command_Block_t;

/*
 * Inquiry data structure for SCSI 2.
 */

/*
 * True size of Inquiry_Data_t.  The Additional_Length + 4 will give the last
 * byte number.  To get the size you need to add 1 to this.
 */
#define Inquiry_Data_Size(ip) (ip->Additional_Length + 4 + 1)
/*
 * Size of E_InquiryData_t
 */
#define E_Inquiry_Data_Size(ip) (Inquiry_Data_Size(ip) + sizeof(unsigned short))

/*
 * Device Parameters structure
 */
struct IO_DeviceParameters {
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
unsigned  long	dp_IO_BusNumber;		/* System bus number */
#else
	int	dp_IO_BusNumber;		/* System bus number */
#endif
	int	dp_BaseAddress;			/* Base address for the board */
	unsigned char	dp_IO_BusSlot;		/* Slot # in system bus */
	unsigned char	dp_SCSI_BusNumber;	/* SCSI Bus # on the board */
	unsigned char	dp_PUN;			/* Device PUN */
	unsigned char	dp_LUN;			/* Device LUN */
};

/*
 * ANSI defined structure
 */
#ifdef BigEndian
typedef struct SCSI_Inquiry_Data {
	unsigned int	Periph_Qualifier	: 3,
		Periph_Device_Type	: 5,
		Removable_Media		: 1,
		Device_Type_Qualifier	: 7,
		ISO_Version		: 2,
		ECMA_Version		: 3,
		ANSI_Version		: 3,
		AENC			: 1,
		reserved_0		: 3,
		Response_Data_Format	: 4;
	unsigned int	Additional_Length	: 8,
		reserved_1		:16,
		RelAdr			: 1,
		WBus32			: 1,
		WBus16			: 1,
		Sync			: 1,
		Linked			: 1,
		reserved_2		: 1,
		CmdQue			: 1,
		SftReset		: 1;
	char	Vendor_ID[8];
	char	Product_ID[16];
	char	Revision_Level[4];
	char	Vendor_Specific[20];
} SCSI_Inquiry_Data_t;
#else
typedef struct SCSI_Inquiry_Data {
	unsigned int	Periph_Device_Type	: 5,
		Periph_Qualifier	: 3,
		Device_Type_Qualifier	: 7,
		Removable_Media		: 1,
		ANSI_Version		: 3,
		ECMA_Version		: 3,
		ISO_Version		: 2,
		Response_Data_Format	: 4,
		reserved_0		: 3,
		AENC			: 1;
	unsigned int	Additional_Length	: 8,
		reserved_1		:16,
		SftReset		: 1,
		CmdQue			: 1,
		reserved_2		: 1,
		Linked			: 1,
		Sync			: 1,
		WBus16			: 1,
		WBus32			: 1,
		RelAdr			: 1;
	char	Vendor_ID[8];
	char	Product_ID[16];
	char	Revision_Level[4];
} SCSI_Inquiry_Data_t;
#endif

/*
 * ANSI-Approved Version.  Applicable to the ANSI_Version field.
 */
#define ANSI_NoCompliance       0x00
#define ANSI_SCSI_1             0x01
#define ANSI_SCSI_2             0x02

/*
 * Response Data Format.
 */
#define RDF_SCSI_1              0x00
#define RDF_CCS                 0x01
#define RDF_SCSI_2              0x02

/*
 * Types of Queue Tag Messages. These are the messages actually passed to the
 * device.
 */
#define SIMPLE_QUEUE_TAG        0x20
#define HEAD_OF_QUEUE_TAG       0x21
#define ORDERED_QUEUE_TAG       0x22

/*
 * Read Capacity Data structure.
 */

typedef struct Read_Capacity {
	int	blkno;
	int	capacity;
} Read_Capacity_Data_t;

/*
 * Read Block Limits structure.
 */

#ifdef BigEndian
typedef struct Block_Limits {
	unsigned int	reserved		: 8,
		Maximum_Block_Length	:24;
	short	Minimum_Block_Length;
} Read_Block_Limits_Data_t;
#else
typedef struct Block_Limits {
	char	reserved;
	char	Maximum_Block_Len_HI;
	char	Maximum_Block_Len_MI;
	char	Maximum_Block_Len_LO;
	short	Minimum_Block_Length;
} Read_Block_Limits_Data_t;
#endif

/*
 * Sense Data Declarations.
 */

/*
 * TOTAL Sense data size should be 255 bytes.
 */
#define ADDITIONAL_SENSE_DATA_SIZE	(250 - 7)

/*
 * Macros for extracting the residual count or block numbers from the odd
 * aligned "Information" field of the sense data.
 */

#ifdef BigEndian
#define Get_Residual(sense_pointer) ((sense_pointer->Information_MSB << 24) \
			    	     | sense_pointer->Information_Bytes2_1_0)
#else
#define Get_Residual(sense_pointer) ((sense_pointer->Information_MSB << 24) \
			    	     | (sense_pointer->Information_Bytes2<<16) \
			    	     | (sense_pointer->Information_Bytes1<<8) \
			    	     | sense_pointer->Information_Bytes0) 
#endif

#define Get_Logical_Block_Number(sense_pointer)	(Get_Residual(sense_pointer))

#define Is_Valid(sense_pointer)	(sense_pointer->Valid)

#define Is_Extended_Sense(sp) (sp->Error_Class == 0x7)

/*
 * Macro to interpret sense data to determine true size of valid data
 */
#define SCSI_Sense_Size(sp) (((SCSI_Sense_Data_t *)(sp))->Additional_Length +8)
/*
 * Size of a E_SenseData_t
 */
#define E_SCSI_Sense_Size(sp) (SCSI_Sense_Size(sp) + sizeof(unsigned short))

#ifdef BigEndian

typedef struct SCSI_Sense_Data {
	unsigned int	Valid				: 1,
		Error_Class			: 3,
		Error_Code			: 4,
		Segment_Number			: 8,
		Filemark			: 1,
		End_Of_Media			: 1,
		Incorrect_Length_Indicator	: 1,
		Reserved			: 1,
		Sense_Key			: 4,
		Information_MSB			: 8;

	unsigned int	Information_Bytes2_1_0		:24,
		Additional_Length		: 8;

	int	Command_Specific_Information;

	unsigned char	Additional_Sense_Code;
	unsigned char	Additional_Sense_Code_Qualifier;
	unsigned short	Field_Replaceable_Unit_Code	: 8,
			Sense_Key_Specific_Valid	: 1,
			Illegal_Req_Cmd_Data		: 1,
			Illegal_Req_Reserved		: 2,
			Illegal_Req_Bit_Valid		: 1,
			Illegal_Req_Bit_Pointer		: 3;
	unsigned short	Sense_Key_Specific_Data;
	char	Additional_Sense_Data[ADDITIONAL_SENSE_DATA_SIZE - 11];
} SCSI_Sense_Data_t;
#else

typedef struct SCSI_Sense_Data {
	unsigned char	Error_Code			: 4,
			Error_Class			: 3,
			Valid				: 1;
	unsigned char	Segment_Number;
	unsigned char	Sense_Key			: 4,
			Reserved			: 1,
			Incorrect_Length_Indicator	: 1,
			End_Of_Media			: 1,
			Filemark			: 1;
	unsigned char 	Information_MSB;
	unsigned char	Information_Bytes2;
	unsigned char	Information_Bytes1;
	unsigned char	Information_Bytes0;
	unsigned char 	Additional_Length;

	int	Command_Specific_Information;

	unsigned char	Additional_Sense_Code;
	unsigned char	Additional_Sense_Code_Qualifier;
	unsigned char	Field_Replaceable_Unit_Code;
	unsigned char	Illegal_Req_Bit_Pointer		: 3,
			Illegal_Req_Bit_Valid		: 1,
			Illegal_Req_Reserved		: 2,
			Illegal_Req_Cmd_Data		: 1,
			Sense_Key_Specific_Valid	: 1;
	unsigned short	Sense_Key_Specific_Data;
	char	Additional_Sense_Data[ADDITIONAL_SENSE_DATA_SIZE - 11];
} SCSI_Sense_Data_t;
#endif


/*
 * SCSI Sense Key Definitons
 */
#define Key_No_Sense            0x00
#define Key_Recovered_Error     0x01
#define Key_Not_Ready           0x02
#define Key_Medium_Error        0x03
#define Key_Hardware_Error      0x04
#define Key_Illegal_Request     0x05
#define Key_Unit_Attention      0x06
#define Key_Data_Protect        0x07
#define Key_Blank_Check         0x08
#define Key_Vendor_Specific     0x09
#define Key_Copy_Aborted        0x0a
#define Key_Aborted_Command     0x0b
#define Key_Equal               0x0c
#define Key_Volume_Overflow     0x0d
#define Key_Miscompare          0x0e
#define Key_Reserved            0x0f


typedef struct WD_Registers {
	int Driver_Flags;		/* Private Data, Not to be interpreted	*/
	int Driver_State0;
	int Driver_State1;
	int Driver_State2;
} WD_Registers_t;

typedef struct FPIII_Registers {
	int Driver_Flags;		
	int Driver_State0;
	int Driver_State1;
	int Driver_State2;
} FPIII_Registers_t;

typedef struct SCSI_CCB {
	SCSI_Command_Block_t SCSI_Command_Block;
	char	SCSI_status;
	char	SCSI_message;
	int Driver_Error;
	union Hardware_Specific {
		WD_Registers_t		WD_Registers;
		FPIII_Registers_t	FPIII_Registers;
	} Hardware_Specific;
} E_SCSI_Command_Block_t;



/*
 * The SCSI "Mode Select" Structure.
 */

/*
 * Page control values for the "Mode Sense" command. Placed in bits 7,6 of byte 2
 * of the SCSI command block by the "VDEV_ModeSense" function.
 */
#define PageControlCurrent	0x00
#define PageControlChangeable	0x01
#define PageControlDefault	0x02
#define PageControlSaved	0x03


#define PageCode_0              0
#define PageCode_1              1
#define PageCode_2              2
#define PageCode_3              3
#define PageCode_4              4
#define PageCode_5              5
#define PageCode_6              6
#define PageCode_7              7
#define PageCode_8              8
#define PageCode_9              9


/*
 * ANSI defined data structures .. Don,t change these unless the ANSI committee thinks
 * its OK.
 */

/*

                     Table 7-61: Mode Parameter Header(6)

==============================================================================
  Bit|   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0    |
Byte |        |        |        |        |        |        |        |        |
==============================================================================
 0   |                           Mode Data Length                            |
-----|-----------------------------------------------------------------------|
 1   |                           Medium Type                                 |
-----|-----------------------------------------------------------------------|
 2   |                           Device-Specific Parameter                   |
-----|-----------------------------------------------------------------------|
 3   |                           Block Descriptor Length                     |
==============================================================================
*/
typedef struct ModeParameterHeader6 {
	unsigned char ModeDataLength;
	unsigned char MediumType;
	unsigned char DeviceSpecificParameter;
	unsigned char BlockDescriptorLength;
} ModeParameterHeader6_t;

/*
	Table 7-63: Mode Parameter Block Descriptor

==============================================================================
  Bit|   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0    |
Byte |        |        |        |        |        |        |        |        |
==============================================================================
 0   |                           Density Code                                |
-----|-----------------------------------------------------------------------|
 1   | (MSB)                                                                 |
-----|---                                                                 ---|
 2   |                           Number of Blocks                            |
-----|---                                                                 ---|
 3   |                                                                 (LSB) |
-----|-----------------------------------------------------------------------|
 4   |                           Reserved                                    |
-----|-----------------------------------------------------------------------|
 5   | (MSB)                                                                 |
-----|---                                                                 ---|
 6   |                           Block Length                                |
-----|---                                                                 ---|
 7   |                                                                 (LSB) |
==============================================================================
*/
#ifdef BigEndian

typedef struct ModeBlockDescriptor {
	unsigned int	DensityCode	: 8,
		NumberOfBlocks	:24;
	unsigned int	reserved	:8,
		BlockLength	:24;
	} ModeBlockDescriptor_t;
#else

typedef struct ModeBlockDescriptor {
	unsigned int	NumberOfBlocks	:24,
		DensityCode	: 8;
	unsigned int	BlockLength	:24,
		reserved	:8;
	} ModeBlockDescriptor_t;

#endif
typedef struct SCSI_ModeSelectData {
	ModeParameterHeader6_t	Header;
	ModeBlockDescriptor_t	BlockDescriptor;
	} ModeSelectData_t;

typedef struct PageHeader {
	unsigned char PageCode;
	unsigned char PageLength;
	} PageHeader_t;

typedef struct Mode_Select {
		int dummy;
	} Mode_Select_t;

/*
 * The "Mode Sense" Structure.
 */

#ifdef BigEndian

typedef struct {
	ModeParameterHeader6_t	Header;
	ModeBlockDescriptor_t	BlockDescriptor;
	PageHeader_t	PageHeader;
	unsigned short	TracksPerZone;
	unsigned short	AlternateSectorsPerZone;
	unsigned short	AlternateTracksPerZone;
	unsigned short	AlternateTracksPerLogicalUnit;
	unsigned short	SectorsPerTrack;
	unsigned short	DataBytesPerPhysicalSector;
	unsigned short	Interleave;
	unsigned short	TrackSkewFactor;
	unsigned short	CylinderSkewFactor;
	unsigned int		SSEC		: 1,
			HSEC		: 1,
			RMB		: 1,
			SURF		: 1,
			reserved0	: 8,
			reserved1	:16;
} FormatDevicePage_t;

typedef struct {
	ModeParameterHeader6_t	Header;
	ModeBlockDescriptor_t	BlockDescriptor;
	PageHeader_t	PageHeader;
	unsigned char	NumberOfCylinders[3];
	unsigned char	NumberOfHeads;
	unsigned char	WritePrecompCylinder[3];
	unsigned char	ReducedWriteCurrentCylinder[3];
	unsigned short	DriveStepRate;
	unsigned char	LandingZoneCylinder[3];
	unsigned char	Resv_RPL;
	unsigned char	RotationalOffset;
	unsigned char	Reserved0;
	unsigned short	MediumRotationRate;
	unsigned char	Reserved[2];
} RigidDiskGeometry_t;

typedef struct {
	ModeParameterHeader6_t	Header;
	ModeBlockDescriptor_t	BlockDescriptor;
	PageHeader_t	PageHeader;
	unsigned short	TransferRate;
	unsigned char	NumberofHeads;
	unsigned char	SectorsPerTrack;
	unsigned short	BytesPerSector;
	unsigned short  NumberOfCylinders;
	unsigned short	WritePrecompCylinder;
	unsigned short	ReducedWriteCurrentCylinder;
	unsigned short	DriveStepRate;
	unsigned char	DriveStepPulseWidth;
	unsigned char   HeadSettleDelay[2];
	unsigned char	MotorOnDelay;
	unsigned char	MotorOffDelay;
	unsigned char	TRDY_SSN_MO;
	unsigned char	StepPulsesPerCylinder;
	unsigned char	WritePrecompensationValue;
	unsigned char	HeadLoadDelay;	
	unsigned char	HeadUnloadDelay;
	unsigned char	Pin34Pin2;
	unsigned char	Pin4Pin1;
	unsigned short	MediumRotationRate;
	unsigned short	Reserved1;
} FlexibleDisk_t; 

#else

typedef struct {
	ModeParameterHeader6_t	Header;
	ModeBlockDescriptor_t	BlockDescriptor;
	PageHeader_t	PageHeader;
	unsigned char	TracksPerZone[2];
	unsigned char	AlternateSectorsPerZone[2];
	unsigned char	AlternateTracksPerZone[2];
	unsigned char	AlternateTracksPerLogicalUnit[2];
	unsigned char	SectorsPerTrack[2];
	unsigned char	DataBytesPerPhysicalSector[2];
	unsigned char	Interleave[2];
	unsigned char	TrackSkewFactor[2];
	unsigned char	CylinderSkewFactor[2];
	unsigned char	reserved0	: 4,
			SURF		: 1,
			RMB		: 1,
			HSEC		: 1,
			SSEC		: 1;
	unsigned char	reserved1[2];
} FormatDevicePage_t;

typedef struct {
	ModeParameterHeader6_t	Header;
	ModeBlockDescriptor_t	BlockDescriptor;
	PageHeader_t	PageHeader;
	unsigned char	NumberOfCylinders[3];
	unsigned char	NumberOfHeads;
	unsigned char	WritePrecompCylinder[3];
	unsigned char	ReducedWriteCurrentCylinder[3];
	unsigned char	DriveStepRate[2]; /* XXX */
	unsigned char	LandingZoneCylinder[3];
	unsigned char	Resv_RPL;
	unsigned char	RotationalOffset;
	unsigned char	Reserved0;
	unsigned char	MediumRotationRate[2]; /* XXX */
	unsigned char	Reserved[2];
} RigidDiskGeometry_t;

typedef struct {
	ModeParameterHeader6_t	Header;
	ModeBlockDescriptor_t	BlockDescriptor;
	PageHeader_t	PageHeader;
	unsigned short	TransferRate;
	unsigned char	NumberofHeads;
	unsigned char	SectorsPerTrack;
	unsigned short	BytesPerSector;
	unsigned short  NumberOfCylinders;
	unsigned short	WritePrecompCylinder;
	unsigned short	ReducedWriteCurrentCylinder;
	unsigned short	DriveStepRate;
	unsigned char	DriveStepPulseWidth;
	unsigned char   HeadSettleDelay[2];
	unsigned char	MotorOnDelay;
	unsigned char	MotorOffDelay;
	unsigned char	TRDY_SSN_MO;
	unsigned char	StepPulsesPerCylinder;
	unsigned char	WritePrecompensationValue;
	unsigned char	HeadLoadDelay;	
	unsigned char	HeadUnloadDelay;
	unsigned char	Pin34Pin2;
	unsigned char	Pin4Pin1;
	unsigned short	MediumRotationRate;
	unsigned short	Reserved1;
} FlexibleDisk_t; 
#endif

/*
 * Access macros to resolve "Big/Little Endian" puke and SCSI's 
 * total disregard of alignment.
 */

/*
 * Page 3 ... Format Parameters
 */
#ifdef BigEndian
#define ModeSenseBytesPerSector(ModeSenseData)  \
		(int)(ModeSenseData->DataBytesPerPhysicalSector)
#define ModeSenseSectorsPerTrack(ModeSenseData)  \
		(int)(ModeSenseData->SectorsPerTrack)
#else
#define ModeSenseBytesPerSector(ModeSenseData)  \
		((int)(ModeSenseData->DataBytesPerPhysicalSector[0] << 8) | \
		(int)(ModeSenseData->DataBytesPerPhysicalSector[1]))
#define ModeSenseSectorsPerTrack(ModeSenseData)  \
		((int)(ModeSenseData->SectorsPerTrack[0] << 8) | \
		(int)(ModeSenseData->SectorsPerTrack[1]))
#endif

/*
 * Page 4 ... Rigid Disk Geometry
 */
#define ModeSenseCylinders(ModeSenseData) \
	    (int)((ModeSenseData->NumberOfCylinders[0] << 16) \
		| (ModeSenseData->NumberOfCylinders[1] << 8) \
		| (ModeSenseData->NumberOfCylinders[2]))

#define ModeSenseHeads(ModeSenseData) \
			(int)(ModeSenseData->NumberOfHeads)

/*
 * Density definitions for Reel-to-Reel tape
 */
#define HIGH_DENSITY	0
#define LOW_DENSITY	1
#define MEM_DENSITY	2
#define DEFAULT_DENSITY	0x00
#define BPI_800		0x01
#define BPI_1600	0x02
#define BPI_6250	0x03
#define BPI_3200	0x06

/*
 * SCSI 2 peripheral device types as expressed in the Peripheral Device Type
 * field of inquiry data.
 */
#define SCSI_DAD_TYPE		0x00
#define SCSI_SAD_TYPE		0x01
#define SCSI_PRT_TYPE		0x02
#define SCSI_PROC_TYPE		0x03
#define SCSI_WORM_TYPE		0x04
#define SCSI_CD_ROM_TYPE	0x05
#define SCSI_SCAN_TYPE		0x06
#define SCSI_OPTICAL_TYPE	0x07
#define SCSI_CHANGER_TYPE	0x08
#define SCSI_COMM_TYPE		0x09
#define SCSI_UNKNOWN_TYPE	0x1f

#define Number_Of_SCSI_Types	32

/*
 * Macros for use determining device types
 */
#define SCSI_REMOVEABLE_BIT      	0x80
#define SCSI_TYPE_BITS  		0x7F
#define SCSI_REMOVEABLE_MEDIA(type) 	(type & SCSI_REMOVEABLE_BIT)
#define IS_SCSI_DAD_TYPE(type)		((type & SCSI_TYPE_BITS) == SCSI_DAD_TYPE)
#define IS_SCSI_SAD_TYPE(type)		((type & SCSI_TYPE_BITS) == SCSI_SAD_TYPE)
#define IS_SCSI_PRT_TYPE(type)		((type & SCSI_TYPE_BITS) == SCSI_PRT_TYPE)
#define IS_SCSI_PROC_TYPE(type)		((type & SCSI_TYPE_BITS) == SCSI_PROC_TYPE)
#define IS_SCSI_WORM_TYPE(type)		((type & SCSI_TYPE_BITS) == SCSI_WORM_TYPE)
#define IS_SCSI_CD_ROM_TYPE(type)	((type & SCSI_TYPE_BITS) == SCSI_CD_ROM_TYPE)
#define IS_SCSI_SCAN_TYPE(type)		((type & SCSI_TYPE_BITS) == SCSI_SCAN_TYPE)
#define IS_SCSI_OPTICAL_TYPE(type)	((type & SCSI_TYPE_BITS) == SCSI_OPTICAL_TYPE)
#define IS_SCSI_CHANGER_TYPE(type)	((type & SCSI_TYPE_BITS) == SCSI_CHANGER_TYPE)
#define IS_SCSI_COMM_TYPE(type)		((type & SCSI_TYPE_BITS) == SCSI_COMM_TYPE)

/*
 * Stuff to be moved to scsi_io.h
 */
#define MAGIC_MASK	0xffff
#define SCSI_NO_LOG	0x80000000
#define SCSI_NO_RETRY	0x40000000
#define SCSI_NO_DISPLAY	0x20000000

/*
 * The following define helps the driver determine that the
 * ioctl request is from a SCSI libio function. 
 */
#define	LIBIO_MAGIC     0x0f

/*
 * The following defines are imbedded in the first argument
 * passed to the SCSI driver's ioctl routine to define which
 * SCSI command page will be executed.
 */

#define	IO_INQUIRY		0x01
#define IO_TESTREADY		0x02
#define IO_DIAGNOSTIC		0x03
#define IO_MODESENSE		0x04
#define IO_MODESELECT		0x05
#define IO_SCSI_CDB		0x06
#define IO_READ			0x07
#define IO_WRITE		0x08
#define IO_VERIFY		0x09
#define IO_ERASE		0x0a
#define IO_RETENSION		0x0b
#define IO_CONDITION		0x0c
#define IO_REWIND		0x0d
#define IO_SPACE		0x0e
#define IO_FORMAT		0x0f
#define IO_OPEN			0x10
#define IO_CLOSE		0x11
#define IO_DEFECT		0x12
#define IO_REASSIGN		0x13
#define IO_BLOCK_READ		0x14
#define IO_BLOCK_WRITE		0x15
#define IO_BLOCK_VERIFY		0x16
#define IO_FILEMARKS		0x17
#define IO_LOAD			0x18
#define IO_REZERO		0x19
#define IO_READCAPACITY		0x1a
#define IO_FLEX_EJECT		0x1b
#define IO_IS_A_FLEX		0x1c
#define IO_Tower_IOCCPSWP	0x1d
#define IO_Tower_IOCRPSWP	0x1e
#define IO_SET_TAPECAPACITY	0x1f
#define IO_READ_TAPECAPACITY	0x20
#define IO_FLEX_SEEK		0x21
#define IO_MOVE_MEDIUM		0x22
#define IO_GETDEVPARMS		0x23
#define IO_GETDEVNODE		0x24
#define IO_BUS_DEVICE_RESET	0x25

/*
 * Data direction constants
 */
#define IO_NONE			0x01
#define IO_MEM_TO_DEV		0x02
#define IO_DEV_TO_MEM		0x03

/*
 * Non-I/O ioctls
 */

#define IO_SET_CONFIGURATION	0xfb
#define IO_GET_CONFIGURATION	0xfc
#define IO_IDENTIFY		0xfd
#define IO_ECHO_ARGS		0xfe
#define IO_SET_DEBUG		0xff

/*
 * Configuration options for each device (Assigned via the Device Identification Table
 * found in space.c).
 * Options are read and set as in gtty stty. No error is returned if an option
 * is set to an unsupported state (i.e. Wide_Transfer set for a SCSI-1 device), however the
 * driver will set the options to the current values which can be read by "IO_GET_CONFIGURATION".
 */
typedef struct SCSI_Device_Configuration {
	unsigned char		Disconnect;		/* TRUE or FALSE */
	unsigned char		SynchronousOffset;	/* 0=Async, Nonzero=Sync, 0xff=default */
	unsigned char		SynchronousPeriod;	/* 0=Async, Nonzero=Sync, 0xff=default */
	unsigned char		Wide_Transfer_Size;	/* 8, 16, or 32 */
	unsigned char		Target_Support;		/* TRUE or FALSE */
	unsigned char		Queue_Size;		/* 0 - 255 */
	struct SCSI_Sort_Table 	*Sort_Table;		/* Address of Sort Table */
	unsigned char		Reserved1;
	unsigned char		Reserved2;
} SCSI_Device_Configuration_t;

/*
 * The following structure is used for the passing of a SCSI command
 * block directly through the library interface to the driver.  This
 * direct SCSI ccb interface is very much like a razor-sharp knife.
 * PROCEED WITH CAUTION!
 */
#define CDB_LENGTH     12

typedef struct CDB_struct {
	unsigned char Byte[CDB_LENGTH];
	unsigned char *Buffer;
	unsigned long BufferLength;
} CDB_t;



/*
 * The following structure is used by the I/O library for passing the SCSI sense
 * data pointer, command indicator, and argument list for a particular
 * command to the SCSI driver's ioctl routine.
 */
typedef struct SCSI_Arguments {
	SCSI_Sense_Data_t	*SenseData;
	int             	Command_Indicator;
	int             	ArgumentList[16];
} SCSI_Arguments_t;

/*
 * Protective timers
 */
#define DefaultProtectiveTimer	(60 * 60)   /* 1 hour */
#define OneHourTimer		(60 * 60) 
#define ThirtySecondTimer	(30)   

/*
 * Default Block Size for fixed length blocks
 */
#define	DefaultBlockSize	512

/*
 *	SCSI Commands
 */

#define	 SCSI_TestUnitReady		0x00
#define	 SCSI_Rewind			0x01
#define	 SCSI_Rezero			0x01
#define	 SCSI_RequestSense		0x03
#define	 SCSI_Format			0x04
#define	 SCSI_ReassignBlock		0x07
#define  SCSI_Read			0x08
#define  SCSI_Write			0x0a
#define	 SCSI_WriteFilemark		0x10
#define	 SCSI_Space			0x11
#define  SCSI_Inquiry			0x12
#define  SCSI_Verify			0x13
#define	 SCSI_ModeSelect		0x15
#define	 SCSI_ModeSense			0x1a
#define	 SCSI_Reserve			0x16
#define	 SCSI_Release			0x17
#define	 SCSI_Erase			0x19
#define	 SCSI_ModeSelect		0x15
#define	 SCSI_ModeSense			0x1a
#define  SCSI_LoadUnload		0x1b
#define  SCSI_ReceiveDiagnostic		0x1c
#define  SCSI_SendDiagnostic		0x1d
#define	 SCSI_ReadCapacity		0x25
#define	 SCSI_WriteVerify 		0x2e
#define  SCSI_BlockVerify		0x2f
#define	 SCSI_ReadDefectData 		0x37
#define	 SCSI_EjectFlex			0x10

#endif

