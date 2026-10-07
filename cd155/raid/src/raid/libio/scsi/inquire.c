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

#ifndef FALSE
#define FALSE		0
#endif
#ifndef TRUE
#define TRUE		!FALSE
#endif

#define SCSI_NONE	0x01
#define MEM_TO_SCSI	0x02
#define SCSI_TO_MEM	0x03

#define SCSI_DISK       0x00
#define SCSI_TAPE       0x01
#define SCSI_PRINTER    0x02
#define SCSI_CPU        0x03
#define SCSI_WORM       0x04
#define SCSI_CDROM      0x05
#define SCSI_JUKEBOX    0x08

#define CMD_LENGTH	12
#define BUF_LENGTH	(2 * 1024)

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

int passthruflg = 0;

main(argc, argv)
int	argc;
char	*argv[];
{
	register int	i,o;
	register int	done;
	extern char *optarg;
	extern int optind;
	char filename[256] = "";
	
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
	 if (argc < 2) {
		fprintf(stderr, "usage: %s <scsi_device>\n", argv[0]);
		exit(-1);
	}
	
	while ((o = getopt(argc, argv, "f:p")) != EOF)
        {

        switch (o)
        {
                case 'p':       passthruflg++;
                                break;
                case 'f':       strcpy(filename,optarg);
                                break;


        }  /* switch */
        }  /* while getopt */

	if(!strlen(filename))
		strcpy(filename,argv[optind]);

	/*
	 * open SCSI device
	 */
	if ((scsi_fd = IO_Open(filename, O_RDWR)) == -1) {
		perror(filename);
		exit(-1);
	}

	if(passthruflg){
		if (passthru()){
			clean_exit(-1);
		}
	}
	else
	if (inquire(0)) {
		clean_exit(-1);
	}

	/*
	 * close SCSI device
	 */
	if (IO_Close(scsi_fd) == -1) {
		perror(filename);
		clean_exit(-1);
	}

	clean_exit(0);
}

inquire(lun)
int	lun;
{
	SCSI_Inquiry_Data_t	inquiry_data;
	char device_type[30] = "";
	
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
	 switch(inquiry_data.Periph_Device_Type) {
		case SCSI_DISK:
			strcpy(device_type,"disk ");
			break;
		case SCSI_TAPE:
			strcpy(device_type,"tape ");
			break;
		case SCSI_PRINTER:
			strcpy(device_type,"printer ");
			break;
		case SCSI_CPU:
			strcpy(device_type,"cpu ");
			break;
		case SCSI_WORM:
			strcpy(device_type,"worm ");
			break;
		case SCSI_CDROM:
			strcpy(device_type,"cdrom ");
			break;
		case SCSI_JUKEBOX:
			strcpy(device_type,"media_changer ");
			break;
	}

	printf("%s%.8s%.16s%.4s\n",
	device_type,
	inquiry_data.Vendor_ID,
	inquiry_data.Product_ID,
	inquiry_data.Revision_Level);

	return 0;
}

passthru()
{
	SCSI_Inquiry_Data_t	inquiry_data;
	char device_type[30] = "";
	register int i,chan,lun=0;
	
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
	 switch(inquiry_data.Periph_Device_Type) {
		case SCSI_DISK:
			strcpy(device_type,"disk ");
			break;
		case SCSI_TAPE:
			strcpy(device_type,"tape ");
			break;
		case SCSI_PRINTER:
			strcpy(device_type,"printer ");
			break;
		case SCSI_CPU:
			strcpy(device_type,"cpu ");
			break;
		case SCSI_WORM:
			strcpy(device_type,"worm ");
			break;
		case SCSI_CDROM:
			strcpy(device_type,"cdrom ");
			break;
		case SCSI_JUKEBOX:
			strcpy(device_type,"media_changer ");
			break;
	}

	printf("%s%.8s%.16s%.4s\n",
	device_type,
	inquiry_data.Vendor_ID,
	inquiry_data.Product_ID,
	inquiry_data.Revision_Level);

	if(strncmp(inquiry_data.Vendor_ID,"NCR",3)){
		printf("Not a RAID\n");
		return(-1);
	}
	else {
		for(i=0;i<CMD_LENGTH;i++) /* clear CDB */
			CDB.Byte[i] = 0x00;

		chan = 80;
		for (i=5;i>0;i--){
			passthru_cmd(chan);
			chan -= 16;
		}
	}
	return 0;
}

passthru_cmd(channel)
int	channel;
{
	int	ret;
	int	dir = SCSI_NONE;

	/*
	 * setup passthru CDB
	 */
		CDB.Byte[0] = 0x14;
		CDB.Byte[1] = 0xC3; /* use an unused lun (6) */
		CDB.Byte[2] = channel;
		CDB.Byte[4] = sizeof(SCSI_Inquiry_Data_t);;

	/*
	 * issue SCSI command
	 */
	if ((ret = IO_SCSI(scsi_fd, &CDB, dir)) == -1) {
		perror("IO_SCSI");
		return -1;
	}
	if (ret == -2) {
		fprintf(stderr, "\nFailed PassThru  - CHECK SENSE CONDITION\n");
		clean_exit(-1);
	}
	inquire(6);

	return 0;
}
clean_exit(code)
int	code;
{
	exit(code);
}
