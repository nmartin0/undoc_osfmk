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
 
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dlmain.c
 Title:		Download main module
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:25:21 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains code to implement the disk array
		controller firmware download.

Modification 8/23/91:  Added code to print out percentage of download
	completed.  Currently using a fudge factor that is matched to 
	Motorola S-format records.  If the download starts using other
	formats, the code should be changed to recognize this. How? I don't
	know. - cas
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include "dlinclds.h"
#include <setjmp.h>

/* Added for percentage  -cas */
#include <sys/stat.h>

#ifdef MSDOS
#include <io.h>
#endif /* MSDOS */

jmp_buf env; /*used to save the stack in event of errors*/

/******************************************************************************
 ***				EXTERNAL REFERENCES	       		    ***
 *****************************************************************************/
void	read_block();		/* FILE */
void	write_block();		/* XFER */
void    udl_error();

/* External variables */
extern  lword   eval[];
extern  int	once_thru;        /* bool: good hex file flag in map file */
extern  int     sd_allocated;

/******************************************************************************
 ***				INTERNAL REFERENCES	       		    ***
 *****************************************************************************/
/* Local functions */
lword    ACF_DOWNLOAD();
void	initialize();
lword	process();
void	process_map();
void	download_file();
int	read_filename();

#define	DEFAULT_CHANNEL	0x30	/* the character "0" */

static int	USAGE;		/* bool: display usage message */
static int	MAP;		/* bool: next file is a map file */
static int	START_FLAG;	/* bool: set start address flag */
static int	FIRST_FILE;	/* bool: set for first downloaded file */
static int	RESET_FLAG;	/* bool: reset/resume flag for IPC driver */
static int	SA_GIVEN;	/* bool: starting address option given */
       int      verbose;        /* GLOBAL, bool: display flag */
static int	start_address;	/* pointer: starting address for LPF */
static int	ipc_fd;		/* IPC file descriptor */
static char	channel;	/* index: LPF board to download onto */

/* Added for DAC firmware download                                    */
       hw_zipcode       z;    /* location of DAC  */
static hw_zipcode_t    *ptr_hw_zipcode = ( hw_zipcode_t * )&z;  /* pointer to location of DAC */
       SCSI_DEV         *sd; /* pointer to SCSI Device routines  */
/* end of additions                                                   */  

/******************************************************************************
 ***				PROCEDURES		       		    ***
 *****************************************************************************/
lword
ACF_DOWNLOAD(hw_addr,fp,map_flag)
hw_zipcode hw_addr;
FILE *fp;
int map_flag;
{
      u_int status=0;

      z=hw_addr;
      if (setjmp(env) == 0) {
       initialize();
       status=process(fp);
       return (status);
      }
      else return(5);
}
/* **************************************************************************
support routine.
************************************************************************** */
movmem(from,count,to)
byte		*from;
unsigned int	count;
byte		*to;
{
	while (count-- != 0)
		*to++ = *from++;
}

/* ************* LOCAL FUNCTIONS *****************************************   
************************************************************************** */
static void initialize()
{
	start_address = 0;
	START_FLAG = TRUE;
	FIRST_FILE = TRUE;
	RESET_FLAG = TRUE;
	MAP = FALSE;
        verbose=FALSE;
	SA_GIVEN = FALSE;
        once_thru=FALSE;
        sd_allocated=FALSE;
}

/* **************************************************************************
************************************************************************** */
lword process(fp)
FILE *fp;
{
        FILE *bp;
	char	buffer[80];
/* Added for DAC firmware download                            */
        struct  dldata FDS;
/* End of addition                                         */
	if (read_filename(fp,&buffer[0],sizeof(buffer))) {
        	if ((bp=fopen (&buffer[0],"r"))!=(FILE *)NULL) {
        	MAP=TRUE;
        	}
        }
	else {
		fprintf(stderr,"\nERROR - Unable to open input file\n");
		return(3);
	}
	if (MAP)
	{
		process_map(fp,bp);
		MAP = FALSE;
	}
	else
	{
		rewind(fp);  /* start download at beginning of file */
		download_file(fp);
/* Added for DAC firmware download                            */
                if (once_thru) {
                 FDS.count=0;     /* write out a block of length 0 */
                 FDS.address=0;   /* for the DAC board             */
                 FDS.buffer=0;
       	         write_block(&FDS);			/* write data */
                }
	        destroy_SCSI_DEV(sd);
                sd_allocated=FALSE;
/* End of additions                                        */
	}
	return (0);
}

/* **************************************************************************
************************************************************************** */
#define DLF_BUFFERSIZE 2048+80

static void download_file(fp)
FILE *fp;
{
	lword	sadr;
	int	first_pass;
	struct	dldata FDS;
	byte	*buffer;

/* Added for percentage - cas */
#ifndef NOVELL
	unsigned long FileSize;
	unsigned long BytesSoFar;
	struct stat StatBuf;
#endif


/* Added for DAC firmware download                         */
        extern  SCSI_DEV *sd;
/* End of additions                                        */

 	if ((buffer = (byte *) zalloc(DLF_BUFFERSIZE * (sizeof(byte)))) == NULL)
 	{
		/* Should use ACF error routine */
 		printf("malloc error\n");
 		exit(0);
 	}
 
	first_pass = TRUE;
	FDS.buffer = &buffer[0];
	FDS.cs = 0;

/* added for percentage - cas */
#ifndef NOVELL
	(void) fstat(fileno(fp), &StatBuf);
	FileSize = (unsigned long) StatBuf.st_size;
	BytesSoFar = 0UL;
	if ( isatty( 2 ) )
		fprintf(stderr, " 0 Percent Complete\r");
#endif


	while ((FDS.count > 0) || first_pass)
	{
		FDS.count = DLF_BUFFERSIZE;		/* set buffer size */
		read_block(fp, &FDS);			/* get data */
		if (first_pass)
		{
			first_pass = FALSE;
			sadr = FDS.address;
		}
/* Added for DAC firmware download                    */
		if (FDS.count != 0) 
		{
			write_block(&FDS);			/* write data */
			/*  added for percentage - cas
			    The 2.4 is an empirical fudge factor for Motorola S-format
				records.  n bytes returned in FDS.count reflect 2.4n bytes
				actually read from the firmware file.
			*/
#ifndef NOVELL
			BytesSoFar += FDS.count;
			if ( isatty( 2 ) )
				fprintf(stderr, "%2.0d\r",
					(int) (2.4 * ((BytesSoFar * 100UL) / FileSize)));
#endif
		}
/* End of modification                                */
	}
	/* Honest at this point, I am sure we're. . . - cas */
#ifndef NOVELL
	if ( isatty( 2 ) )
		fprintf(stderr, "99 Percent Complete\r");
#endif
 
	if (FDS.address != 0)	sadr = FDS.address;

	if (FIRST_FILE && (start_address == 0) && !SA_GIVEN)
	{
		FIRST_FILE = FALSE;
		start_address = sadr;
	}

	free (buffer);

}
/* **************************************************************************
************************************************************************** */
static void process_map(mp,fp)
FILE *mp;
FILE *fp;
{
	char	buffer[80];
/* Added for DAC firmware download                            */
        struct  dldata FDS;
/* End of addition                                         */
	
        download_file(fp);
	while (!feof(mp))
	{
		if (read_filename(mp,&buffer[0],sizeof(buffer))) {
                 once_thru=TRUE;
                 if ((fp=fopen(&buffer[0],"r")) != (FILE *) NULL) {
                  download_file(fp);
                 }
                 else {
                  eval [dle_file_offset] = (lword) &buffer[0];
                  udl_error(dl_main_err+3,dle_file+1,eval);
                 }
                }
	}
/* Added for DAC firmware download                            */
        if (once_thru) {
         FDS.count=0;     /* write out a block of length 0 */
         FDS.address=0;   /* for the DAC board             */
         FDS.buffer=0;
         write_block(&FDS);			/* write data */
         }
        destroy_SCSI_DEV(sd);
        sd_allocated=FALSE;
/* End of additions                                        */
	fclose(mp);
}

/* **************************************************************************
************************************************************************** */
static int read_filename(fp,buf,size)
FILE	*fp;
char	buf[];
int	size;
{
	int	i;
	char	ch;
	
	ch = fgetc(fp);
	while ( (ch == '\r') || (ch == '\n') || (ch == ' ') || (ch == '\t') )
		ch = fgetc(fp);
	for (i = 0; i < size; i++)
	{
		if ( ((ch == '\r') || (ch == '\n') || (ch == ' ') || 
			(ch == '\t')) || feof(fp))	break;
		buf[i] = ch;
		ch = fgetc(fp);
	}
	buf[i] = 0;
	if (i > 0)	return(TRUE);
	return(FALSE);
}

/* **************************************************************************
************************************************************************** */
