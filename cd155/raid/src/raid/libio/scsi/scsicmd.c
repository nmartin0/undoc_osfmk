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
 *  File:   scsicmd.c 
 *  Usage:  scsicmd -[io] -s<data size> -f<filename>
 *  Author: Jerrie Coffman, Richard Griffiths
 *          Intel Corporation Supercomputer Systems Division
 *  Date:   10/92
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

#define CMD_LENGTH	12
#define BUF_LENGTH	(2 * 1024)

#define RM		"rm -f"


int		scsi_fd;

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

main(argc, argv)
int	argc;
char	*argv[];
{
	register int	i;
	register char	c;
	long		sz = 0;
	register int	done;
	int		dir = SCSI_NONE;
	int		ret;
	char		devname[256];
	char		cmdfile[256];
	char		datafile[256];
	int	errflg=0,iflg=0,oflg=0,dataflg=0;
	extern char *optarg;
	extern int optind;

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
	if (argc < 3) {
		fprintf(stderr, "usage: %s -[io] -c<cmd_file_name>\
 -d<data_file_name> -s<data size> -f<scsi_device>\n",argv[0]);
		exit(-1);
	}
	while ( ( c = getopt ( argc, argv, "ioc:d:s:f:" ) ) != EOF ) {
		switch ( c ) {
			case 'c':
				strcpy(cmdfile,optarg);
				break;
			case 'd':
				strcpy(datafile,optarg);
				dataflg++;
				break;
			case 'f':
				strcpy(devname,optarg);
				break;
			case 'i':
				if(oflg)
					errflg++;
				else {
				dir = SCSI_TO_MEM;
					iflg++;
				}
				break;
			case 'o':
				if(!iflg){
					dir = MEM_TO_SCSI;
					oflg++;
				}
				else
					errflg++;
				break;
			case 's':
				sz = strtol(optarg, (char **)NULL, 0);
				break;
			case '?':
				errflg++;
		}
	}
	if((iflg || oflg) && !dataflg)
		errflg++;
	if(errflg){
		fprintf(stderr, "usage: %s -[io] -c <cmd_file_name>\
 -d <data_file_name> -s <data size> -f <scsi_device>\n",argv[0]);
		exit(-1);
	}
	/*
	 * open SCSI device
	 */
	if ((scsi_fd = IO_Open(devname, O_RDWR)) == -1) {
		perror(argv[1]);
		exit(-1);
	}

	for (i = 0; i < CMD_LENGTH; i++) CDB.Byte[i] = 0;
	for (i = 0; i < BUF_LENGTH; i++) buf[i] = 0;

	/*
	 * initialize CDB structure
	 */
	CDB.Buffer = buf;
	read_cmd(cmdfile);
	if(dir == MEM_TO_SCSI)
		readwrite_buf(datafile,dir);
	ret = execute_cmd(dir,sz);
	if(dir == SCSI_TO_MEM)
		readwrite_buf(datafile,dir);

	/*
	 * close SCSI device
	 */
	if (IO_Close(scsi_fd) == -1) {
		perror(argv[1]);
		exit(-1);
	}
	exit(ret);
}

read_cmd(cmdfile)
char	*cmdfile;
{
	register int i;
	int tmp;
	char *s;

	/*
	 * open file
	 */
	if ((cmd_file = fopen(cmdfile, "r")) == NULL) {
		perror(cmdfile);
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
		perror(cmdfile);
		return -1;
	}

	return 0;
}

readwrite_buf(datafile,dir)
char	*datafile;
int	dir;
{
	register int i;
	int tmp;
	char *s;

	/*
	 * open file
	 */
	if(dir == SCSI_TO_MEM) {
		if ((buf_file = fopen(datafile, "w")) == NULL) {
			perror(datafile);
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
			perror(datafile);
			return -1;
		}
	}
	else {
		/*
	 	* open file
	 	*/
		if ((buf_file = fopen(datafile, "r")) == NULL) {
			perror(datafile);
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
			perror(datafile);
			return -1;
		}

	}
	return 0;
}

execute_cmd(dir, sz)
int	dir;
long	sz;
{
	int	ret;

	/*
	 * set buffer length according to direction
	 */
	if (dir != SCSI_NONE) {
		CDB.BufferLength = sz;
			}

	else CDB.BufferLength = 0;

	/*
	 * issue SCSI command
	 */
	if ((ret = IO_SCSI(scsi_fd, &CDB, dir)) == -1) {
		perror("IO_SCSI");
		return -1;
	}
	if (ret == -2){
		fprintf(stderr, "\nCHECK SENSE CONDITION\n");
		ret=2;
	}
	else
		ret=0;
		
	return (ret);
}

