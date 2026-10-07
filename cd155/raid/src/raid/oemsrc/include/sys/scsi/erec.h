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
/* Copyright 1989 NCR Corporation - Dayton, Ohio, USA */

#ident	"@(#)./usr/src/arch/mca/uts/i386/sys/scsi:erec.h		1.2"

/*--------------------------------------------------------------------------*/
/*
 * erec.h - Error Logging Structure Definitions
 *
 * Error records are a collection of defined fields which are
 * general in nature and which may be combined in various ways
 * to log a wide variety of error (or other) events.
 * Error records consist of two different types of structures:
 *
 *	1. Required header fields - defined in "ErrorHeader_t";
 *	2. Optional log fields added by the program logging the
 *	   error to describe the error - defined in separate
 *	   structure definitions depending on format field.
 *
 */
/*--------------------------------------------------------------------------*/

#ifndef _SYS_EREC_H
#define _SYS_EREC_H

#include <sys/types.h>
#ifdef MP
#include <sys/mp/mplock.h>
#endif
#include <sys/utsname.h>
#include <sys/scsi/elog.h>
#include <sys/scsi/SCSI.h>


/*--------------------------------------------------------------------------*/
#define	MAX_TYPE	 6
#define	MAX_SW_MODULE	17

/*
 * Error record header - required fields needed to log an event.
 */
typedef struct {
	unsigned char		E_Cause;
	unsigned char		E_Severity;
	unsigned char		E_Type[MAX_TYPE];
	unsigned char		E_Environment;
	unsigned char		E_SoftwareModule[MAX_SW_MODULE];
	unsigned char		E_ProcessorType;
	unsigned char		E_ProcessorNumber;
	unsigned char		E_BusType;
	unsigned char		E_BusNumber;
	unsigned char		E_BusSlotNumber;
	unsigned short		E_Temperature;
	unsigned short		E_Voltage;
	unsigned short		E_Speed;
	unsigned short		E_Count;
	time_t			E_Time;
	unsigned long		E_TimeMilliseconds;
	dev_t			E_DeviceNumber;
	unsigned long		E_Tag;
	unsigned long		E_RemainingLength;
} ErrorHeader_t;

/*
 * Error causes (E_Cause)
 */
#define E_ENVIRONMENT		'E'	/* Error caused by environment	*/
#define	E_HARDWARE		'H'	/* Error caused by hardware	*/
#define	E_MEDIA			'M'	/* Error caused by media	*/
#define	E_NOERROR		'N'	/* Non-error event		*/
#define	E_OPERATOR		'O'	/* Error caused by operator	*/
#define	E_SOFTWARE		'S'	/* Error caused by software	*/
#define E_UNDETERMINED		'U'	/* Cause of error undetermined	*/

/*
 * Error severity codes (E_Severity)
 */
#define	CRITICAL		'C'	/* Operation failed to recover;	*/
					/*  left the system unstable	*/
#define	DEGRADED		'D'
#define	FATAL			'F'
#define	WARNING			'W'	/* Operation succeeded; operator*/
					/*  warned about some condition	*/

#define	INFORMATION		'I'	/* Error message for informa-	*/
					/* tional purposes only		*/

/*
 * Error environment codes (E_Environment)
 */
#define	ENVIRONMENT_FIRMWARE	0
#define	ENVIRONMENT_OS		1

/*
 * Error processor types (E_ProcessorType)
 */
#define UPM	1
#define RP	2
#define FP	3
#define CCHP	4
#define CP	5
#define EP	6

/*
 * Error bus types (E_BusType)
 */
#define M1	1
#define M2	2
#define PT	3

/*==========================================================================*/
/*
 * The following structures define the optional portions
 * of the error log.
 */
/*==========================================================================*/

#define MAX_ID_CODE		16	/* maximum number of optional fields */

/*
 * Each optional portion of the error log begins with a description.
 */
typedef struct {
	unsigned short		E_IdentificationCode;
	unsigned short		E_FieldLength;
} E_Description_t;


/*--------------------------------------------------------------------------*/
#define	E_TIME_CHANGE		1

typedef struct {
	E_Description_t		E_Description;
	time_t			E_NewTime;
} E_TimeChange_t;


/*--------------------------------------------------------------------------*/
#define	E_START_RECORD		2
#define E_SYSNAME_LENGTH	16
#define E_MAX_DEV		256
#define E_CONF_SIZE     ((E_MAX_DEV + 8*sizeof(long) - 1)/(8*sizeof(long)))

typedef struct {
	E_Description_t		E_Description;
	unsigned char		E_SystemName[E_SYSNAME_LENGTH];
	unsigned char		E_ReleaseNumber[E_SYSNAME_LENGTH];
	long			E_MemorySize;
        long            	E_Bdev_Conf[E_CONF_SIZE];    /* configuration */
        long            	E_Cdev_Conf[E_CONF_SIZE];    /* bitmaps       */

} E_Start_t;


/*--------------------------------------------------------------------------*/
#define	E_POWER_DOWN		3
#define	E_POWER_UP		4

typedef struct {
	E_Description_t		E_Description;
} E_PowerCycle_t;


/*--------------------------------------------------------------------------*/
#define	E_TALLY			5

#define	MAX_TALLY_NAME		18
#define MAX_PROTOCOL		4

typedef struct {
	E_Description_t		E_Description;
	unsigned short		E_TallyId;
	unsigned char		E_Reserved[2];
	unsigned long		E_PreviousTallyValue;
	unsigned long		E_TallyValue;
	unsigned long		E_Threshold;
	unsigned long		E_ObservedThreshold;
	long			E_Total;
	char			E_TallyName[MAX_TALLY_NAME];
	char			E_Protocol[MAX_PROTOCOL];
} E_Tally_t;


/*--------------------------------------------------------------------------*/
#define	E_SCSI_HEADER		6
#define	E_INQUIRY_DATA		7
#define	E_SENSEDATA		8
#define	E_IO_STATISTICS		9
#define	E_SCSI_COMMAND		10

typedef struct {
	E_Description_t		E_Description;
	unsigned short		E_HardwareType;
	unsigned short		E_TargetStatus;
	unsigned short		E_NumberOfRetries;
	dev_t			E_Device;
} E_SCSI_Header_t;

typedef struct {
	E_Description_t		E_Description;
	SCSI_Inquiry_Data_t	E_SCSI_InquiryData;
} E_InquiryData_t;

typedef struct {
	E_Description_t		E_Description;
	SCSI_Sense_Data_t	E_SCSI_SenseData;
} E_SenseData_t;

typedef struct E_SCSI_CommandBlock {
	E_Description_t		E_Description;
	SCSI_Command_Block_t	E_SCSI_CCB_Block;
} E_SCSI_CommandBlock_t;


/*----------------------------------------------------------------------------*/
                                        /* TYPE: all errors            */
#define E_COMMON_ERROR	11              /* Parameterized Error Message */
#define LABEL_SIZE      16

/*
 *      Template for the error record that is logged by cmn_err()
 *      This record will be immediately followd by the text of the message.
 *      The field length will be sizeof(E_CommmonError_t) + strlen(message).
 */
typedef struct {
	E_Description_t		E_Description;
        char            	E_Label[LABEL_SIZE]; /* cmn_err message label */
} E_CommonError_t;


/*--------------------------------------------------------------------------*/

/*
 * ioctl commands for error device driver
 */
#define ERR_DISPLAY	01
#define ERR_CLEAR	02


/*--------------------------------------------------------------------------*/

#if defined(_KERNEL)
/*
 * multiprocessing locks 
 */

/*
 * FIA_log_lock - this lock is used to single thread all calls to the 
 *		  FIA log functions.
 */
extern mutex_t  FIA_log_lock;
extern lockinfo_t FIA_log_info;

/*
 * log_flag - this flag is set when somebody is waiting to read a log record
 */
extern int	log_flag;
#define	E_WAITING_FOR_RECORD	1	/* possible values for log_flag */

extern mutex_t  log_flag_lock;  /* log_flag multiprocessing lock */
extern LockInfo_t log_flag_info;

/*
 * block device activity bitmap
 *	each bit corresponds to a block device as defined in systm.h
 */
extern long	blkacty;
extern ProcessorLock_t	blkacty_lock;
extern LockInfo_t blkacty_info;
#endif /* _KERNEL */

#endif /* _SYS_EREC_H */
