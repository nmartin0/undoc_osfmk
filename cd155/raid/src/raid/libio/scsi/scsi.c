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
 
/*
 *        INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 */
/*
 *  File:   scsi.c
 *  Author: Jerrie Coffman
 *          Intel Corporation Supercomputer Systems Division
 *  Date:   7/92
 *
 *  Program to exercise the pass-through interface to the SCSI device driver
 */


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "passthru.h"
#ifndef FALSE
#define FALSE		0
#endif
#ifndef TRUE
#define TRUE		!FALSE
#endif

#define NO_DATA	0x01  
#define MEM_TO_SCSI_TGT	0x02
#define SCSI_TGT_TO_MEM	0x03

#define CMD_LENGTH	12
#define BUF_LENGTH	SCSI_BUF_MAX

#define EDITOR		"vi"
#define RM		"rm -f"

char		*editor;
char		default_editor[] = EDITOR;

int		scsi_fd;

char		cmd_file_name[L_tmpnam];
char		buf_file_name[L_tmpnam];
FILE		*cmd_file;
FILE		*buf_file;

typedef struct CDB_struct {
	unsigned char Byte[CMD_LENGTH];
	unsigned char *Buffer;
	unsigned long BufferLength;
} CDB_t;

CDB_t		CDB;
unsigned char	buf[BUF_LENGTH];

typedef struct device_parameters {
	int		node;		/* System bus number */
	int		address;	/* Base address for the board */
	unsigned char	bus;		/* Slot # in system bus */
	unsigned char	controller;	/* SCSI Bus # on the board */
	unsigned char	target_id;	/* Device PUN */
	unsigned char	lun;		/* Device LUN */
} dev_param_t;

typedef struct SCSI_Inquiry_Data {
	unsigned int	Periph_Device_Type	: 5,
		Periph_Qualifier		: 3,
		Device_Type_Qualifier		: 7,
		Removable_Media			: 1,
		ANSI_Version			: 3,
		ECMA_Version			: 3,
		ISO_Version			: 2,
		Response_Data_Format		: 4,
		reserved_0			: 3,
		AENC				: 1;
	unsigned int	Additional_Length	: 8,
		reserved_1			:16,
		SftReset			: 1,
		CmdQue				: 1,
		reserved_2			: 1,
		Linked				: 1,
		Sync				: 1,
		WBus16				: 1,
		WBus32				: 1,
		RelAdr				: 1;
	char	Vendor_ID[8];
	char	Product_ID[16];
	char	Revision_Level[4];
} SCSI_Inquiry_Data_t;

#define ADDITIONAL_SENSE_DATA_SIZE      (250 - 7)

typedef struct SCSI_Sense_Data {
        unsigned char   Error_Code                      : 4,
                        Error_Class                     : 3,
                        Valid                           : 1;
        unsigned char   Segment_Number;
        unsigned char   Sense_Key                       : 4,
                        Reserved                        : 1,
                        Incorrect_Length_Indicator      : 1,
                        End_Of_Media                    : 1,
                        Filemark                        : 1;
        unsigned char   Information_MSB;
        unsigned char   Information_Bytes2;
        unsigned char   Information_Bytes1;
        unsigned char   Information_Bytes0;
        unsigned char   Additional_Length;

        int     Command_Specific_Information;

        unsigned char   Additional_Sense_Code;
        unsigned char   Additional_Sense_Code_Qualifier;
        unsigned char   Field_Replaceable_Unit_Code;
        unsigned char   Illegal_Req_Bit_Pointer         : 3,
                        Illegal_Req_Bit_Valid           : 1,
                        Illegal_Req_Reserved            : 2,
                        Illegal_Req_Cmd_Data            : 1,
                        Sense_Key_Specific_Valid        : 1;
        unsigned short  Sense_Key_Specific_Data;
        char    Additional_Sense_Data[ADDITIONAL_SENSE_DATA_SIZE - 11];
} SCSI_Sense_Data_t;

#define  SCSI_SNS_RESERVED       0xf


static char *sns_msg[SCSI_SNS_RESERVED+1] = {
        "No Sense Data",/* shouldn't happen */
        "Recovered",
        "Unit not ready",
        "Medium",
        "Hardware failure",
        "Illegal request",
        "Unit Attention Condition",
        "Protection",
        "Blank Check",
        "Vendor Unique",
        "Copy Operation Aborted",
        "Aborted Command",
        "Equal Comparison",
        "Volume Overflow",
        "Miscompare",
        "Reserved"
};

/*
 * Table of the official SCSI-2 error messages
 * Last update:
 *	X3T9.2/86-109, Revision 10c, March 9, 1990
 */
static struct addtl_sns_keys_msg {
	unsigned char	byte12;
	unsigned char	byte13;
	char		*means;
} addtl_sns_msgs[] = {
	{ 0x00, 0x00, "No additional sense information" },
	{ 0x00, 0x01, "Filemark detected" },
	{ 0x00, 0x02, "End-of-partition/medium detected" },
	{ 0x00, 0x03, "Setmark detected" },
	{ 0x00, 0x04, "Beginning of partition/medium detected" },
	{ 0x00, 0x05, "End-of-data detected" },
	{ 0x00, 0x06, "I/O process terminated" },
	{ 0x00, 0x11, "Audio play operation in progress" },
	{ 0x00, 0x12, "Audio play operation paused" },
	{ 0x00, 0x13, "Audio play operation successfully completed" },
	{ 0x00, 0x14, "Audio play operation stopped due to error" },
	{ 0x00, 0x15, "No current audio status to return" },
	{ 0x01, 0x00, "No index/sector signal" },
	{ 0x02, 0x00, "No seek complete" },
	{ 0x03, 0x00, "Peripheral device write fault" },
	{ 0x03, 0x01, "No write current" },
	{ 0x03, 0x02, "Excessive write errors" },
	{ 0x04, 0x00, "Logical unit not ready, cause not reportable" },
	{ 0x04, 0x01, "Logical unit is in process of becoming ready" },
	{ 0x04, 0x02, "Logical unit not ready, initializing command required" },
	{ 0x04, 0x03, "Logical unit not ready, manual intervention required" },
	{ 0x04, 0x04, "Logical unit not ready, format in progress" },
	{ 0x05, 0x00, "Logical unit does not respond to selection" },
	{ 0x06, 0x00, "No reference position found" },
	{ 0x07, 0x00, "Multiple peripheral devices selected" },
	{ 0x08, 0x00, "Logical unit communication failure" },
	{ 0x08, 0x01, "Logical unit communication time-out" },
	{ 0x08, 0x02, "Logical unit communication parity error" },
	{ 0x09, 0x00, "Track following error" },
	{ 0x09, 0x01, "Tracking servo failure" },
	{ 0x09, 0x02, "Focus servo failure" },
	{ 0x09, 0x03, "Spindle servo failure" },
	{ 0x0a, 0x00, "Error log overflow" },
	{ 0x0c, 0x00, "Write error" },
	{ 0x0c, 0x01, "Write error recovered with auto-reallocation" },
	{ 0x0c, 0x02, "Write error - auto-reallocation failed" },
	{ 0x10, 0x00, "Id CRC or ECC error" },
	{ 0x10, 0x04, "Recovered data with LEC" },
	{ 0x11, 0x00, "Unrecovered read error" },
	{ 0x11, 0x01, "Read retries exhausted" },
	{ 0x11, 0x02, "Error too long to correct" },
	{ 0x11, 0x03, "Multiple read errors" },
	{ 0x11, 0x04, "Unrecovered read error - auto-reallocate failed" },
	{ 0x11, 0x05, "L-EC uncorrectable error" },
	{ 0x11, 0x06, "CIRC unrecovered error" },
	{ 0x11, 0x07, "Data resynchronization error" },
	{ 0x11, 0x08, "Incomplete block read" },
	{ 0x11, 0x09, "No gap found" },
	{ 0x11, 0x0a, "Miscorrected error" },
	{ 0x11, 0x0b, "Unrecovered read error - recommend reassignment" },
	{ 0x11, 0x0c, "Unrecovered read error - recommend rewrite the data" },
	{ 0x12, 0x00, "Address mark not found for id field" },
	{ 0x13, 0x00, "Address mark not found for data field" },
	{ 0x14, 0x00, "Recorded entity not found" },
	{ 0x14, 0x01, "Record not found" },
	{ 0x14, 0x02, "Filemark or setmark not found" },
	{ 0x14, 0x03, "End-of-data not found" },
	{ 0x14, 0x04, "Block sequence error" },
	{ 0x15, 0x00, "Random positioning error" },
	{ 0x15, 0x01, "Mechanical positioning error" },
	{ 0x15, 0x02, "Positioning error detected by read of medium" },
	{ 0x16, 0x00, "Data synchronization mark error" },
	{ 0x17, 0x00, "Recovered data with no error correction applied" },
	{ 0x17, 0x01, "Recovered data with retries" },
	{ 0x17, 0x02, "Recovered data with positive head offset" },
	{ 0x17, 0x03, "Recovered data with negative head offset" },
	{ 0x17, 0x04, "Recovered data with retries and/or CIRC applied" },
	{ 0x17, 0x05, "Recovered data using previous sector id" },
	{ 0x17, 0x06, "Recovered data without ECC - data auto-reallocated" },
	{ 0x17, 0x07, "Recovered data without ECC - recommend reassignment" },
	{ 0x18, 0x00, "Recovered data with error correction applied" },
	{ 0x18, 0x01, "Recovered data with error correction and retries applied" },
	{ 0x18, 0x02, "Recovered data - data auto-reallocated" },
	{ 0x18, 0x03, "Recovered data with CIRC" },
	{ 0x18, 0x05, "Recovered data - recommended reassignment" },
	{ 0x19, 0x00, "Defect list error" },
	{ 0x19, 0x01, "Defect list not available" },
	{ 0x19, 0x02, "Defect list error in primary list" },
	{ 0x19, 0x03, "Defect list error in grown list" },
	{ 0x1a, 0x00, "Parameter list length error" },
	{ 0x1b, 0x00, "Synchronous data transfer error" },
	{ 0x1c, 0x00, "Defect list not found" },
	{ 0x1c, 0x01, "Primary defect list not found" },
	{ 0x1c, 0x02, "Grown defect list not found" },
	{ 0x1d, 0x00, "Miscompare during verify operation" },
	{ 0x1e, 0x00, "Recovered id with ECC correction" },
	{ 0x20, 0x00, "Invalid command operation code" },
	{ 0x21, 0x00, "Logical block address out of range" },
	{ 0x21, 0x01, "Invalid element address" },
	{ 0x22, 0x00, "Illegal function" },
	{ 0x24, 0x00, "Invalid field in CDB" },
	{ 0x24, 0x02, "Log parameters changed" },
	{ 0x25, 0x00, "Logical unit not supported" },
	{ 0x26, 0x00, "Invalid field in parameter list" },
	{ 0x26, 0x01, "Parameter not supported" },
	{ 0x26, 0x02, "Parameter value invalid" },
	{ 0x26, 0x03, "Threshold parameters not supported" },
	{ 0x27, 0x00, "Write protected" },
	{ 0x28, 0x00, "Not ready to ready transition (medium may have changed)" },
	{ 0x28, 0x01, "Import or export element accessed" },
	{ 0x29, 0x00, "Power on, reset, or bus device reset occurred" },
	{ 0x2a, 0x00, "Parameters changed" },
	{ 0x2a, 0x01, "Mode parameters changed" },
	{ 0x2b, 0x00, "Copy cannot execute since host cannot disconnect" },
	{ 0x2c, 0x00, "Command sequence error" },
	{ 0x2c, 0x01, "Too many windows specified" },
	{ 0x2c, 0x02, "Invalid combination of windows specified" },
	{ 0x2d, 0x00, "Overwrite error on update in place" },
	{ 0x2f, 0x00, "Commands cleared by another initiator" },
	{ 0x30, 0x00, "Incompatible medium installed" },
	{ 0x30, 0x01, "Cannot read medium - unknown format" },
	{ 0x30, 0x02, "Cannot read medium - incompatible format" },
	{ 0x30, 0x03, "Cleaning cartridge installed" },
	{ 0x31, 0x00, "Medium format corrupted" },
	{ 0x31, 0x01, "Format command failed" },
	{ 0x32, 0x00, "No defect spare location available" },
	{ 0x32, 0x01, "Defect list update failure" },
	{ 0x33, 0x00, "Tape length error" },
	{ 0x36, 0x00, "Ribbon, ink, or toner failure" },
	{ 0x37, 0x00, "Rounded parameter" },
	{ 0x39, 0x00, "Saving parameters not supported" },
	{ 0x3a, 0x00, "Medium not present" },
	{ 0x3b, 0x00, "Sequential positioning error" },
	{ 0x3b, 0x01, "Tape position error at beginning of medium" },
	{ 0x3b, 0x02, "Tape position error at end of medium" },
	{ 0x3b, 0x03, "Tape or electronic vertical forms unit not ready" },
	{ 0x3b, 0x04, "Slew failure" },
	{ 0x3b, 0x05, "Paper jam" },
	{ 0x3b, 0x06, "Failed to sense top-of-form" },
	{ 0x3b, 0x07, "Failed to sense bottom-of-form" },
	{ 0x3b, 0x08, "Reposition error" },
	{ 0x3b, 0x09, "Read past end of medium" },
	{ 0x3b, 0x0a, "Read past beginning of medium" },
	{ 0x3b, 0x0b, "Position past end of medium" },
	{ 0x3b, 0x0c, "Position past beginning of medium" },
	{ 0x3b, 0x0d, "Medium destination element full" },
	{ 0x3b, 0x0e, "Medium source element empty" },
	{ 0x3d, 0x00, "Invalid bits in identify message" },
	{ 0x3e, 0x00, "Logical unit has not self-configured yet" },
	{ 0x3f, 0x00, "Target operating conditions have changed" },
	{ 0x3f, 0x01, "Microcode has been changed" },
	{ 0x3f, 0x02, "Changed operating definition" },
	{ 0x3f, 0x03, "Inquiry data has changed" },
	{ 0x40, 0x00, "RAM failure" },
	{ 0x40, 0xff, "Diagnostic failure on component <NN>" },
	{ 0x41, 0x00, "Data path failure" },
	{ 0x42, 0x00, "Power on or self-test failure" },
	{ 0x43, 0x00, "Message error" },
	{ 0x44, 0x00, "Internal target failure" },
	{ 0x45, 0x00, "Select or reselect failure" },
	{ 0x46, 0x00, "Unsuccessful soft reset" },
	{ 0x47, 0x00, "SCSI parity error" },
	{ 0x48, 0x00, "Initiator detected message received" },
	{ 0x49, 0x00, "Invalid message error" },
	{ 0x4a, 0x00, "Command phase error" },
	{ 0x4b, 0x00, "Data phase error" },
	{ 0x4c, 0x00, "Logical unit failed self-configuration" },
	{ 0x4e, 0x00, "Overlapped commands attempted" },
	{ 0x50, 0x00, "Write append error" },
	{ 0x50, 0x01, "Write append position error" },
	{ 0x50, 0x02, "Position error related to timing" },
	{ 0x51, 0x00, "Erase failure" },
	{ 0x52, 0x00, "Cartridge fault" },
	{ 0x53, 0x00, "Media load or eject failed" },
	{ 0x53, 0x01, "Unload tape failure" },
	{ 0x53, 0x02, "Medium removal prevented" },
	{ 0x54, 0x00, "SCSI to host system interface failure" },
	{ 0x55, 0x00, "System resource failure" },
	{ 0x57, 0x00, "Unable to recover table-of-contents" },
	{ 0x58, 0x00, "Generation does not exist" },
	{ 0x59, 0x00, "Updated block read" },
	{ 0x5a, 0x00, "Operator request or state change input (unspecified)" },
	{ 0x5a, 0x01, "Operator medium removal request" },
	{ 0x5a, 0x02, "Operator selected write protect" },
	{ 0x5a, 0x03, "Operator selected write permit" },
	{ 0x5b, 0x00, "Log exception" },
	{ 0x5b, 0x01, "Threshold condition met" },
	{ 0x5b, 0x02, "Log counter at maximum" },
	{ 0x5b, 0x03, "Log list codes exhausted" },
	{ 0x5c, 0x00, "RPL status change" },
	{ 0x5c, 0x01, "Spindles synchronized" },
	{ 0x5c, 0x02, "Spindles not synchronized" },
	{ 0x60, 0x00, "Lamp failure" },
	{ 0x61, 0x00, "Video acquisition error" },
	{ 0x61, 0x01, "Unable to acquire video" },
	{ 0x61, 0x02, "Out of focus" },
	{ 0x62, 0x00, "Scan head positioning error" },
	{ 0x63, 0x00, "End of user area encountered on this track" },
	{ 0x64, 0x00, "Illegal mode for this track" },
	{ 0, 0, 0}
};

main(argc, argv)
int	argc;
char	*argv[];
{
	register int	i;
	register int	done;

#ifdef	SUPERUSER
	/*
	 * check for superuser access
	 */
	if (geteuid() != 0) {
		fprintf(stderr, "%s must be run by superuser\n", argv[0]);
		exit(-1);
	}
#endif

	/*
	 * check invocation
	 */
	if (argc != 2) {
		fprintf(stderr, "usage: %s <scsi_device>\n", argv[0]);
		exit(-1);
	}

	/*
	 * open SCSI device
	 */
	if ((scsi_fd = IO_Open(argv[1], O_RDWR)) == -1) {
		perror(argv[1]);
		exit(-1);
	}

	/*
	 * get EDITOR environment variable, if it exists
	 */
	if ((editor = (char *)getenv("EDITOR")) == NULL) {
		editor = default_editor;
	}

	/*
	 * create temporary files
	 */
	if (create_tmp() == -1) {
		exit(-1);
	}

	/*
	 * clear buffers
	 */
	for (i = 0; i < CMD_LENGTH; i++) CDB.Byte[i] = 0;
	for (i = 0; i < BUF_LENGTH; i++) buf[i] = 0;

	/*
	 * initialize CDB structure
	 */
	CDB.Buffer = buf;

	/*
	 * enter command loop
	 */
	done = FALSE;
	while (!done) {

		printf("\n\
1. Edit SCSI Command\n\
2. Edit Data Buffer\n\
3. Execute SCSI Command\n\
4. Locate %s\n\
q. Quit scsi \n\
\n\
> ", argv[1]);

		switch (getchar()) {

			case '1':
				if (edit_cmd()) {
					clean_exit(-1);
				}
				break;

			case '2':
				if (edit_buf()) {
					clean_exit(-1);
				}
				break;

			case '3':
				if (execute_cmd()) {
					clean_exit(-1);
				}
				continue;

			case '4':
				if (locate(argv[1])) {
					clean_exit(-1);
				}
				break;

			case '?':
				printf("\
Additional commands:\n\
\n\
\ti = Perform SCSI Inquiry command\n\
\tr = Perform SCSI Request Sense command\n\
\n\
Press <return> to continue: ");
				while(getchar() != '\n');
				break;

			case 'i':
				if (inquire(argv[1])) {
					clean_exit(-1);
				}
				break;
			case 'r':
				if (request_sense(argv[1])) {
					clean_exit(-1);
				}
				break;

			case ' ':
			case '\t':
				break;

			case '\n':
				continue;

			case 'q':
			case EOF:
				printf("\n");
				done = TRUE;
				continue;

			default:
				fprintf(stderr, "Invalid command\n");
				break;
		}

		/*
		 * eat characters until EOL
		 */
		while(getchar() != '\n');
	}

	/*
	 * close SCSI device
	 */
	if (IO_Close(scsi_fd) == -1) {
		perror(argv[1]);
		clean_exit(-1);
	}

	clean_exit(0);
}

edit_cmd()
{
	register int i;
	int tmp;
	char *s;

	/*
	 * open file
	 */
	if ((cmd_file = fopen(cmd_file_name, "w")) == NULL) {
		perror(cmd_file_name);
		return -1;
	}

	/*
	 * write buffer to file
	 */
	for (i = 0; i < CMD_LENGTH; i++) {
		fprintf(cmd_file, " %02x", CDB.Byte[i]);
	}

	/*
	 * close file
	 */
	if (fclose(cmd_file) != 0) {
		perror(cmd_file_name);
		return -1;
	}

	/*
	 * allocate memory for the editor command
	 */
	if ((s = (char *)malloc(strlen(editor) + L_tmpnam + 2)) == NULL) {
		perror("malloc");
		return -1;
	}

	/*
	 * invoke editor
	 */
	sprintf(s, "%s %s", editor, cmd_file_name);
	if (system(s) < 0) {
		perror(editor);
		return -1;
	}

	/*
	 * free command buffer
	 */
	free(s);

	/*
	 * open file
	 */
	if ((cmd_file = fopen(cmd_file_name, "r")) == NULL) {
		perror(cmd_file_name);
		return -1;
	}

	/*
	 * read buffer from file
	 */
	for (i = 0; i < CMD_LENGTH; i++) {
		fscanf(cmd_file, " %02x", &tmp);
		CDB.Byte[i] = (unsigned char)tmp;
	}

	/*
	 * close file
	 */
	if (fclose(cmd_file) != 0) {
		perror(cmd_file_name);
		return -1;
	}

	return 0;
}

edit_buf()
{
	register int i;
	int tmp;
	char *s;

	/*
	 * open file
	 */
	if ((buf_file = fopen(buf_file_name, "w")) == NULL) {
		perror(buf_file_name);
		return -1;
	}

	/*
	 * write buffer to file
	 */
	for (i = 0; i < BUF_LENGTH; i++) {
		if (i && ((i % 16) == 0)) fprintf(buf_file, "\n");
		fprintf(buf_file, " %02x", buf[i]);
	}

	/*
	 * close file
	 */
	if (fclose(buf_file) != 0) {
		perror(buf_file_name);
		return -1;
	}

	/*
	 * allocate memory for the editor command
	 */
	if ((s = (char *)malloc(strlen(editor) + L_tmpnam + 2)) == NULL) {
		perror("malloc");
		return -1;
	}

	/*
	 * invoke editor
	 */
	sprintf(s, "%s %s", editor, buf_file_name);
	if (system(s) < 0) {
		perror(editor);
		return -1;
	}

	/*
	 * free command buffer
	 */
	free(s);

	/*
	 * open file
	 */
	if ((buf_file = fopen(buf_file_name, "r")) == NULL) {
		perror(buf_file_name);
		return -1;
	}

	/*
	 * read buffer from file
	 */
	for (i = 0; i < BUF_LENGTH; i++) {
		fscanf(buf_file, " %02x", &tmp);
		buf[i] = (unsigned char)tmp;
	}

	/*
	 * close file
	 */
	if (fclose(buf_file) != 0) {
		perror(buf_file_name);
		return -1;
	}

	return 0;
}

execute_cmd()
{
	int	done;
	int	dir;
	int	ret;
	char	s[80];

	/*
	 * eat characters until EOL
	 */
	while(getchar() != '\n');

	/*
	 * get command direction
	 */
	done = FALSE;
	while (!done) {

		printf("\n\
SCSI Data Phase direction:\n\
\n\
\t1. No Data Phase\n\
\t2. Data Out\n\
\t3. Data In\n\
\n\
> ");

		switch (getchar()) {

			case '1':
				dir = NO_DATA;
				done = TRUE;
				break;

			case '2':
				dir = MEM_TO_SCSI_TGT;
				done = TRUE;
				break;

			case '3':
				dir = SCSI_TGT_TO_MEM;
				done = TRUE;
				break;

			case ' ':
			case '\n':
			case '\t':
				break;

			case EOF:
				clean_exit(0);

			default:
				fprintf(stderr, "Invalid command\n");
				break;
		}
	}

	/*
	 * eat characters until EOL
	 */
	while(getchar() != '\n');

	/*
	 * set buffer length according to direction
	 */
	if (dir != NO_DATA) {

		/*
		 * get buffer length from user
		 */
		for (;;) {
			printf("\nEnter buffer length: ");
			fgets(s, sizeof(s), stdin);
			ret = strtol(s, (char **)NULL, 0);

			if ((ret > 0) && (ret <= BUF_LENGTH)) {
				CDB.BufferLength = ret;
				break;
			}

			printf("Buffer length must be > 0 and <= %d\n", 
				BUF_LENGTH);
		}
	} else CDB.BufferLength = 0;

	/*
	 * issue SCSI command
	 */
	if ((ret = IO_SCSI(scsi_fd, &CDB, dir)) == -1) {
		perror("IO_SCSI");
		return -1;
	}
	if (ret == -2) fprintf(stderr, "\nCHECK SENSE CONDITION\n");

	return 0;
}

char *
scsi_print_add_sense_keys(key, qualif)
        unsigned char  key, qualif;
{
	char	mesg[256];
        struct addtl_sns_keys_msg      *msg;

        for (msg = addtl_sns_msgs; msg->means; msg++) {
                if (msg->byte12 != key) continue;
                if ((msg->byte12 == 0x40) && qualif) {
                        /*sprintf(mesg,"%s", msg->means);*/
        		return(msg->means);
                }
                if (msg->byte13 == qualif) {
                        /*sprintf(mesg,"%s", msg->means); */
        		return(msg->means);
                }
        };
        sprintf(mesg,"Unknown additional sense keys");

        return(mesg);
}

request_sense()
{
	 SCSI_Sense_Data_t	sense_data;
	 char	*asc_mesg;
	 int	field;
	 int lun;

	while(getchar() != '\n');
	printf("enter lun #  = ");
	lun=getchar();
	lun = atoi(&lun);
	if(lun < 0 || lun > 7)
		lun = 0;
	if (IO_RequestSense(scsi_fd, &sense_data,lun) == -1) {
		perror("IO_RequestSense");
		return -1;
	}
	field = sense_data.Sense_Key_Specific_Data & 0xff;
	field += sense_data.Sense_Key_Specific_Data >>8;
asc_mesg = (char *) scsi_print_add_sense_keys(sense_data.Additional_Sense_Code, 
	sense_data.Additional_Sense_Code_Qualifier),
	printf("\n\
\t\t\t- SCSI Request Sense Data -\n\
\n\
\t\t  Sense Key: %s\n\
\tAddition Sense Code: %s\n\
\tASC: 0x%02x\tASCQ: 0x%02x\t\
\tAdditional Length: 0x%02x\n\
\n Incorrect Length Indicator: 0x%02x\
   End of Media: 0x%02x\
    File Mark: 0x%02x\n\
", sns_msg[sense_data.Sense_Key],
asc_mesg,sense_data.Additional_Sense_Code,
sense_data.Additional_Sense_Code_Qualifier,sense_data.Additional_Length,
sense_data.Incorrect_Length_Indicator,
sense_data.End_Of_Media,sense_data.Filemark);
	if(sense_data.Information_MSB || sense_data.Information_Bytes0 ||
	   sense_data.Information_Bytes1 || sense_data.Information_Bytes2)
	   printf("Command Specific Information: 0x%02x\t0x%02x\t0x%02x\t0x%02x\n",
	sense_data.Information_Bytes0,sense_data.Information_Bytes1,
	sense_data.Information_Bytes2,sense_data.Information_MSB);
	if(sense_data.Field_Replaceable_Unit_Code)
		printf("FRU: 0x%02x\n",sense_data.Field_Replaceable_Unit_Code);
	if(sense_data.Sense_Key_Specific_Valid){
		printf("\nSense Key specific field is valid\n");
		printf("Error is in %s ",
			sense_data.Illegal_Req_Cmd_Data ? "CDB" : "DATA");
		printf("Byte %d ",field);
		if(sense_data.Illegal_Req_Bit_Valid)
			printf("bit 0x%02x\n",
			sense_data.Illegal_Req_Bit_Pointer);
	}
		
	return 0;
}
locate(dev)
char	*dev;
{
	dev_param_t	dev_param;

	/*
	 * get device parameters
	 */
	if (IO_GetDeviceParameters(scsi_fd, &dev_param) == -1) {
		perror("IO_GetDeviceParameters");
		return -1;
	}

	/*
	 * print location
	 * don't bother printing the base address and system bus number
	 */
	printf("\n\
Physical location of %s\n\
\n\
\tNode:       %4d\n\
\tController: %4d\n\
\tSCSI ID:    %4d\n\
\tLUN:        %4d\n",
		dev,
		dev_param.node, dev_param.controller,
		dev_param.target_id, dev_param.lun);

	return 0;
}

inquire(dev)
char	*dev;
{
	SCSI_Inquiry_Data_t	inquiry_data;
	 int lun;

	while(getchar() != '\n');
	printf("enter lun #  = ");
	lun=getchar();
	lun = atoi(&lun);
	if(lun < 0 || lun > 7)
		lun = 0;

	/*
	 * SCSI Inquiry
	 */
	if (IO_Inquiry(scsi_fd, &inquiry_data,lun) == -1) {
		perror("IO_Inquiry");
		return -1;
	}

	/*
	 * print Inquiry data
	 */
	printf("\n\
SCSI Inquiry Data:\n\
\n\
\tPeriph_Device_Type      0x%x\n\
\tPeriph_Qualifier        0x%x\n\
\tDevice_Type_Qualifier   0x%x\n\
\tRemovable_Media         0x%x\n\
\tANSI_Version            0x%x\n\
\tECMA_Version            0x%x\n\
\tISO_Version             0x%x\n\
\tResponse_Data_Format    0x%x\n\
\treserved_0              0x%x\n\
\tAENC                    0x%x\n\
\tAdditional_Length       0x%x\n\
\treserved_1              0x%x\n\
\tSftReset                0x%x\n\
\tCmdQue                  0x%x\n\
\treserved_2              0x%x\n\
\tLinked                  0x%x\n\
\tSync                    0x%x\n\
\tWBus16                  0x%x\n\
\tWBus32                  0x%x\n\
\tRelAdr                  0x%x\n\
\tVendor_ID               \"%.8s\"\n\
\tProduct_ID              \"%.16s\"\n\
\tRevision_Level          \"%.4s\"\n\
",	inquiry_data.Periph_Device_Type,
	inquiry_data.Periph_Qualifier,
	inquiry_data.Device_Type_Qualifier,
	inquiry_data.Removable_Media,
	inquiry_data.ANSI_Version,
	inquiry_data.ECMA_Version,
	inquiry_data.ISO_Version,
	inquiry_data.Response_Data_Format,
	inquiry_data.reserved_0,
	inquiry_data.AENC,
	inquiry_data.Additional_Length,
	inquiry_data.reserved_1,
	inquiry_data.SftReset,
	inquiry_data.CmdQue,
	inquiry_data.reserved_2,
	inquiry_data.Linked,
	inquiry_data.Sync,
	inquiry_data.WBus16,
	inquiry_data.WBus32,
	inquiry_data.RelAdr,
	inquiry_data.Vendor_ID,
	inquiry_data.Product_ID,
	inquiry_data.Revision_Level);

	return 0;
}

create_tmp()
{
	/*
	 * create temporary files
	 */
	if (tmpnam(cmd_file_name) == NULL) {
		perror(cmd_file_name);
		return -1;
	}
	if ((cmd_file = fopen(cmd_file_name, "w+")) == NULL) {
		perror(cmd_file_name);
		return -1;
	}
	if (fclose(cmd_file) != 0) {
		perror(cmd_file_name);
		return -1;
	}

	if (tmpnam(buf_file_name) == NULL) {
		perror(buf_file_name);
		return -1;
	}
	if ((buf_file = fopen(buf_file_name, "w+")) == NULL) {
		perror(buf_file_name);
		return -1;
	}
	if (fclose(buf_file) != 0) {
		perror(buf_file_name);
		return -1;
	}

	return 0;
}

remove_tmp()
{
	char		s[sizeof(RM) + L_tmpnam +  2];

	/*
	 * remove temporary files
	 */
	sprintf(s, "%s %s", RM, cmd_file_name);
	if (system(s) < 0) {
		/* don't complain about errors
		perror(RM);
		return -1;
		*/
	}

	sprintf(s, "%s %s", RM, buf_file_name);
	if (system(s) < 0) {
		/* don't complain about errors
		perror(RM);
		return -1;
		*/
	}

	return 0;
}

clean_exit(code)
int	code;
{
	/*
	 * remove temporary files
	 */
	if (remove_tmp() == -1) {
		exit(-1);
	}

	exit(code);
}

#if	0

/*
 * these functions are normally provided by libio.a
 * but are stubbed in here for testing purposes
 */

IO_Open()
{
	return 0;
}

IO_Close()
{
	return 0;
}

IO_SCSI()
{
	return 0;
}

IO_GetDeviceParameters(fd, p)
int		fd;
dev_param_t	*p;
{
	p->node = 0;
	p->address = 0;
	p->bus = 0;
	p->controller = 0;
	p->target_id = 0;
	p->lun = 0;

	return 0;
}
#endif
