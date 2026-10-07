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
 Name:		dlxfer.c
 Title:		Download transfer routine
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:25:24 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the code to download disk array controller
		firwmare using the write_buffer command.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include "dlinclds.h"
#include <setjmp.h>

/******************************************************************************
 ***			     VARIABLE DEFINITIONS			    ***
 *****************************************************************************/

#ifdef MSDOS		/* (ifdef may not be long-term solution) */
#include <io.h>
#endif

#define	IOERROR		-1
#define	MAX_XFER	256
/* offsets into chpb.buffer */
#define	CS_OFFSET	0	/* checksum */
#define	COUNT_OFFSET	1	/* byte count of data */
#define	ADR_HI_OFFSET	2	/* high word of address */
#define	ADR_LO_OFFSET	3	/* low word of address */
#define	DATA_OFFSET	4	/* start of data */
/* MODE selections for CH_RESET_SUSPEND_BOARD command */
#define	CONTINUE_MODE		0x0000
#define	SUSPEND_MODE		0x1111
#define	SOFTWARE_RESET_MODE	0x2222
#define	WARM_BOOT_MODE		0x3333
#define	COLD_BOOT_MODE		0x4444
#define	SNAPSHOT_COLD_MODE	0x5555

char	*strcat();
struct	tm *localtime();

void set_start_address();
int	check_CPF_status();
void	do_cpf_cmd();
void	do_ioctl();

extern	int	errno;
extern	lword	eval[];
extern  int Interactive;
extern  hw_zipcode z;
extern  jmp_buf env;
extern  void exit();

hw_zipcode_t *hw_addr_ptr;
int once_thru;
int sd_allocated;

/* local messages */
char *xfer_warning ="\n*** Warning ***: LPF encountered error during bringup.";
char *xfer_status  ="\n                 Board error status =";
char *xfer_continue="\nContinuing download.";
char *xfer_reset   ="\n*** Warning ***: LPF encountered error during reset.";
char *xfer_loaded  ="\nLPF downloaded.";

static char	*DEFAULT_IPC_PATH	= "/dev/ccc0";
static char	ipc_path[20];

static struct	CHPB	chpb;	/* channel parameter block for IPC driver */
static int	ipc_fd;		/* ipc file descriptor */
static int	param[2];	/* parameters for IPC driver */
static int	SUSPEND;	/* bool: flag denoting state of board */
static int	LOADED;		/* bool: flag denoting modules on board */
static int	FAILURE;	/* bool: flag denoting failure during reset */

/******************************************************************************
 ***			          PROCEDURES				    ***
 *****************************************************************************/

void open_IPC_device(ch)
char	ch;
{
	char	chbuf[2];
	lword	timeout;
	static	int	READY;

	chbuf[0] = ch;
	chbuf[1] = 0;
	strcpy(ipc_path,DEFAULT_IPC_PATH);
	strcat(ipc_path,chbuf);
	if ((ipc_fd = open(ipc_path, O_RDONLY | O_NDELAY, 0)) == IOERROR)
		udl_error(dl_xfer_err+0,NULL,NULL);

	timeout = (lword) time(0) + 15;	/* 15 second timeout ?? */
	READY = FALSE;
	SUSPEND = TRUE;
	LOADED = TRUE;
	for (;!READY;)
	{
		do_ioctl(CC_RETURN_STATUS);
		switch ((byte) (param[0] & 0xff))
		{
		case CC_NOBOARD:
			udl_error(dl_xfer_err+5,NULL,NULL);
			break;
		case CC_SUCCESS:
			if (check_CPF_status(param[1]))
			{
				READY = TRUE;
				break;
			}
		case CC_FAILURE:	/* treat as NOTLOADED */
			fprintf(stderr,"%s",xfer_warning);
			fprintf(stderr,"%s 0x%02x",xfer_status,param[1]>>8);
			fprintf(stderr,"%s",xfer_continue);
		case CC_NOTLOADED:
			READY = TRUE;
			SUSPEND = TRUE;	/* FALSE ? */
			LOADED = TRUE;	/* FALSE ? */
		default:
			break;
		}
		if ((!READY) && (timeout < (lword) time(0)))
		{
			/* set_start_address(0); */
			do_ioctl(CC_RESET_BOARD);
			udl_error(dl_xfer_err+4,NULL,NULL);
		}
	}

	if (SUSPEND)
	{
		chpb.buffer[0] = SUSPEND_MODE;
		do_cpf_cmd(CH_RESET_SUSPEND_BOARD, 2);
	}
}

/* **************************************************************************
************************************************************************** */
void write_block(FDS)
struct	dldata *FDS;
{
 
/* Deleted original write_block routine for use of the DAC sw download */

 extern  SCSI_DEV *sd;
 int status;
 u_long j;

 if (sd_allocated!=TRUE)
 {

/* Make sure you can get exclusive access to every LUN off of the DAC.
   This is necessary in the 6298 environment where there could be many
   LUNs off of the DAC.                                                */

  hw_addr_ptr = (hw_zipcode_t *) &z;
  hw_addr_ptr->LUN = 0;
  for ( j = 1; j <= MAX_LUNS; j++ )
  {
  	debug("\nhw_addr->LUN is %ld",hw_addr_ptr->LUN);
   	debug("\nhw_addr is %ld",z);
   	sd=new_SCSI_DEV (z,TRUE);
   	status = sd->lock(sd);
   	hw_addr_ptr->LUN++;
   	if (( sd == (SCSI_DEV *)NULL ) || ( status != 0 ))
   	{
    		fprintf (stderr,"\n[acf] Error - Could not get exclusive access to the hardware device\n\n");
    		destroy_SCSI_DEV(sd);
    		if (!Interactive) exit(5);
    		else longjmp(env,1);
   	}
   	destroy_SCSI_DEV(sd);
  }
  sd_allocated=TRUE;

/* You can get exclusive access to all LUNs off of the DAC - Proceed!   */

/* All objects are destroyed - get one then download */
  hw_addr_ptr->LUN = 0;
  sd=new_SCSI_DEV (z,TRUE);
  status = sd->lock(sd);
  if (( sd == (SCSI_DEV *)NULL ) || ( status != 0 ))
  {
	fprintf (stderr,"\n[acf] Error - Could not get exclusive access to the hardware device\n\n");
   	destroy_SCSI_DEV(sd);
  	if (!Interactive) exit(5);
  	else longjmp(env,1);
  }
 }
 debug("\nData ptr is %x, count is %x,",FDS->buffer,FDS->count);
 debug(" offset is %x\n",FDS->address);
 status=sd->write_buffer(sd,FDS->buffer,FDS->count,FDS->address,TRUE);
 if (status!=0)
 {
/* Added to clean up hung controller - jpk */
		
	/* The purpose of this write_buffer is in the event of a failed
	   write buffer, after a _N_ successful write, the controller is
	   left in a _HUNG_ state.  To get its attention need to send this
	   down to let it know that we're done. */
	
	FDS->count = 0;
	FDS->address = 0;
	FDS->buffer = 0;
		
	status = sd->write_buffer(sd, FDS->buffer, FDS->count, FDS->address,
								  TRUE);
	once_thru = FALSE;
		
/* End of additions - jpk */
	fprintf(stderr,"\nError in performing write_buffer call\n");
   	destroy_SCSI_DEV(sd);
 	if (!Interactive) exit (5);
 	else longjmp(env,1);
 }
 once_thru=TRUE;
}

/* **************************************************************************
************************************************************************** */
void clear_LPF_memory()
{
	set_start_address((lword)0);
	do_ioctl(CC_RETURN_STATUS);
	if ((param[0] & 0xff) != CC_SUSPENDED)
	{
		chpb.buffer[0] = SUSPEND_MODE;
		do_cpf_cmd(CH_RESET_SUSPEND_BOARD, 2);
		/* reset_LPF(); */
	}
	chpb.buffer[0] = 0x0040;	/* start address = 0x00408000 */
	chpb.buffer[1] = 0x8000;
	chpb.buffer[2] = 0x000F;	/* length to fill  0x000F8000 */
	chpb.buffer[3] = 0x8000;	/* 1 Meg */
	chpb.buffer[4] = 0x0000;	/* fill value */
	do_cpf_cmd(CH_FILL_MEMORY,10);
}

/* **************************************************************************
************************************************************************** */
void set_start_address(adr)
lword	adr;
{
	chpb.buffer[0] = (word) (adr >> 16);
	chpb.buffer[1] = (word) adr;
	do_cpf_cmd(CH_SET_START_ADDRESS,4);
}

/* **************************************************************************
************************************************************************** */
void close_IPC_device(reset)
int	reset;
{
	lword		clock;
	struct	tm	*dt;

	if (reset)	{if (LOADED)	reset_LPF();}
	else		
	{
		chpb.buffer[0] = CONTINUE_MODE;
		if (LOADED)	do_cpf_cmd(CH_RESET_SUSPEND_BOARD, 2);
	}

	if (FAILURE)	fprintf(stderr,"%s",xfer_loaded);
	/* set date/time */
	clock = time(0);
	dt = localtime((long *)&clock);
	chpb.buffer[0] = (word) (dt->tm_mon+1)&0x00ff;
	chpb.buffer[1] = (word) ((dt->tm_mday << 8) + (byte) dt->tm_year);
	chpb.buffer[2] = (word) (dt->tm_hour&0x00ff);
	chpb.buffer[3] = (word) ((dt->tm_min << 8) + (byte) dt->tm_sec);
	do_cpf_cmd(CH_SET_DATE_TIME_FROM_HOST, 8);
	close(ipc_fd);
}

/* ******************** LOCAL ROUTINES *********************************** */
/* **************************************************************************
************************************************************************** */
reset_LPF()
{
	static	int	READY;

	do_ioctl(CC_RESET_BOARD);

	/* wait for reset to complete */
	FAILURE = READY = FALSE;
	for (;!READY;)
	{
		do_ioctl(CC_RETURN_STATUS);
		switch ((byte) (param[0] & 0xff))
		{
		case CC_FAILURE:	/* treat as NOTLOADED */
			fprintf(stderr,"%s",xfer_reset);
			fprintf(stderr,"%s 0x%02x",xfer_status,param[1]>>8);
			FAILURE = TRUE;
		case CC_NOTLOADED:
			READY = TRUE;
			break;
		case CC_SUCCESS:
			if (check_CPF_status(param[1]))	READY = TRUE;
		default:
			break;
		}
	}
}


/* **************************************************************************
statuses =	B   busy
		R   reset
		D   level 0 diagnostic
		CD  continuous diagnostics
		XD  extended diagnostics
		L   application load is requested
		S   boot suspended
		F   failed start-up procedures

return:	TRUE if LPF is working (may be currently running an application)
	FALSE if LPF not finished diagnostics, resetting
	NO RETURN if CPF failed status set
************************************************************************** */
static int check_CPF_status(sts)
int	sts;
{
	lword		timeout;
	static	int	READY;

	if ((sts & 0x01) != 0)
	{
		set_start_address(0);
		do_ioctl(CC_RESET_BOARD);
		/* wait for reset to complete */
		timeout = (lword) time(0) + 45;	/* 45 second timeout ?? */
		READY = FALSE;
		for (;!READY;)
		{
			do_ioctl(CC_RETURN_STATUS);
			switch ((byte) (param[0] & 0xff))
			{
			case CC_FAILURE:
			case CC_NOTLOADED:
			case CC_SUCCESS:
				READY = TRUE;
			default:
				break;
			}
			if ((!READY) && (timeout < (lword) time(0)))
				udl_error(dl_xfer_err+4,NULL,NULL);
		}

		return(FALSE);
		/*
		eval[dle_cpf_offset] =  sts;
		udl_error(dl_xfer_err+6, dle_cpf+1, eval);
		*/
	}
	if ((sts & 0x78) != 0)	return(FALSE);
	return(TRUE);
}


/* **************************************************************************
************************************************************************** */
static void do_cpf_cmd(cmd, len)
int	cmd;
int	len;
{
	chpb.command = cmd;
	chpb.length = len;
	param[1] = (int) &chpb;
	do_ioctl(CC_ISSUE_CHANNEL_COMMAND);
	if ((param[0] & 0xff) == CC_TIMEOUT)
		udl_error(dl_xfer_err+3,NULL,NULL);
	if ((chpb.status & 0xff) != 0x80)
	{
		eval[dle_ipc_offset] = param[0];
		eval[dle_cpf_offset] = chpb.status;
		udl_error(dl_xfer_err+2, dle_ipc+dle_cpf+2, eval);
	}
}

/* **************************************************************************
************************************************************************** */
static void do_ioctl(function)
int	function;
{
/*  Not needed & causes Dos compilation problems.
	if (ioctl(ipc_fd, function, param))
	{
		eval[dle_ipc_offset] = errno;
		udl_error(dl_xfer_err+1,dle_ipc+1,eval);
	}
*/
}
