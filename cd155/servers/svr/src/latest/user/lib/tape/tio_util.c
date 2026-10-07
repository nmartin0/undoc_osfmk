/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	tio_util.c 11.1 94/03/22 16:49:46
 */
static char	tio_util_ver[] = "@(#) sourcefile tio_util.c 11.1 94/03/22 16:49:46";

/*
 *
 *  ******************  ENGINEERING CHANGE HISTORY ***********************
 *
 *    Date      Engineer                  Description
 *
 *  2/25/92     G. Haycox       Module creation release to BETA
 *
 *
 */


/* tio_util()
 *
 *      This module contains the following functions:
 *
 *          tisbot() - If an error occurs after a BSF or BSR the
 *                     application can issue this command to determine
 *                     if the command caused the tape to move into BOT.
 *
 *          tiseof() - Determines if on a read error if a TM was read.
 *
 *          tiseom() - Determines if on a write the LEOT has been encountered.
 *
 *          tiodone() - Check to see if a IREAD or IWRITE command has completed.
 *                      After an IREAD the global 'tapeio_glob.read_cnt' contains
 *                      the number of data bytes read.
 *
 *          tiowait() - Hang on the Poll of the IREAD or IWRITE until it
 *                      completes.  (uses tiodone())
 *
 *	    cleareof() - Clear the FEOF bit in the fs_fid status word.
 *
 *
 * */

#include <stdio.h>
#include <fcntl.h>
/*
#include <malloc.h>
*/
#include <sys/types.h>
#include <errno.h>
#include <nodedef.h>
#include <msgtype.h>
#include <pid.h>
#include <cfs.h>
/*
#include <diskproc.h>
#include <nameproc.h>
*/
#include <fio.h>
#include <iod.h>
#include <scsi.h>
#include "type.h"
#include "tapeio.h"
#include "tape3480.h"
#include "tioerr.h"
#include "tioproto.h"

extern TAPEMAIN tapemain;
extern SCSI_SENSE   scsi_sense;

void free_iod();

_iod_t *free_iod_q;
int _iod_sync_level;


#if 0
int tisbot(int fd) {

    return ((!(scsi_sense.key & 0x0F) && !scsi_sense.code &&
                    (scsi_sense.qualifier == 04)) ? 1 : 0 );
}
#endif

#if TDONE
int 
tiodone(int iod) 
{
    _iod_t *iods = (_iod_t *)iod;
	int trapsave;
	char done;
	int 	ret;
printf("in tiodone 1\n");
        if (iod == 0) {
                return 1;
	}


printf("in tiodone 2\n");
printf("iods is 0x%x\n", iods);
printf("iods->_sync_handler is 0x%x\n", iods->_sync_handler);
	while (iods->_sync_handler) {
		void (*hdlr)();

		trapsave = masktrap(1);
		hdlr = iods->_sync_handler;
		if (hdlr) {
printf("found hdlr\n");
			iods->_sync_handler = 0;
			hdlr(0, iods->_sync_handler_count);
		}
		masktrap(trapsave);
	}

        trapsave = masktrap(1);
        done = (iods->count == 0 && iods->busy == 0);
        masktrap(trapsave);
printf("in tiodone 3\n");

        if (done) {
	printf("done \n");
		if (iods->actual < 0) {
	printf("iods->actual < 0 \n");
			errno = iods->errno;
			free_iod(iods);
			return -1;
		}
        	tapeio_glob.read_cnt = iods->actual;
printf("in tiodone 4\n");
		free_iod(iods);
		return 1;
	}
	ret=iodone(iods);
	if (ret)
        	tapeio_glob.read_cnt = iods->actual;
printf("out\n");
	return(ret);
}
#endif

#if TEOF
int 
tiseof(fd) 
	int fd;
{
	int	ret;

#if	CFS
	return ( (fds[fd].fs_pid & FEOF) ? 1 : 0);
#endif	/* CFS */

	printf("tiseof: fd=0x%x\n",fd);

	if ( (ret = _iseof( fd )) < 0 )
		perror( "iseof" );	

	printf("tiseof: ret=%s\n",ret);

	return ( ret );

}
#endif

void 
cleareof(int fd) {
#if CLREOF
	fds[fd].fs_pid &= ~FEOF;
#endif
	return(1);
}

int 
tiseom(int fd) 
{
	return(1);
    return ( (fds[fd].fs_pid & FLEOT) ? 1 : 0);
}

#if TWAIT
int tiowait(int iod) {

	int st;

	while (1) {
		st = tiodone(iod);
		if (st != 0) {
			break;
		}
		flick();
	}
	return ((st == 1) ? 0 : st);
}
#endif

/* tio_log_init()
 *
 * INPUT:   lvcb_p
 *
 * OUTPUT:  none
 *
 * Description:
 *
 *      Open the tapeio log, use stderr if the open fails
 *
 * */

void tio_log_init(LVCB *lvcb_p) {


    char *open_mode = "a";  /* open new file for append */
    char device[22];
    char logpath[39] = "/usr/ipsc/3480log";
    int i;

    if ((lvcb_p->tape_io.logdev == stderr) ||
                (lvcb_p->tape_io.logdev == NULL)) {

        i = strlen(lvcb_p->path);
        while(i && (lvcb_p->path[i--] != '/'));  /* find the last '/' character */
        if ( i ) {
            ++i;
        }
        strcpy(device, &lvcb_p->path[i]);
        strcat(device, ".log");
        strcat(logpath, device);

        if((lvcb_p->tape_io.logdev = fopen(logpath, open_mode)) == (FILE *)NULL) {

            fprintf(stderr, "FOPEN failed for logfile (%s), Using stderr\n",
                                                                    logpath);
            lvcb_p->tape_io.logdev = stderr;
        }
        else {
            chmod(logpath, 0777);  /* wide open access */
        }
    }
}


/* set_spider()
 *
 * INPUT:   handle:      handle returned by the TAPEALLOC command
 *          spider_val:  value to set in the spider variable
 *
 * OUTPUT:       0 or +X:  Old spider value
 *                    -X:  Error - Bad Handle
 *
 * Description:
 *
 *      Allow the application program to set the spider value.
 *
 * */

int 
set_spider(int spider_val) 
{

    int old_spider;
    LVCB *lvcb_p;

    old_spider = tapeio_glob.spider;
    tapeio_glob.spider = spider_val;
printf("set_spider: ");
printf("old_spider=0x%x, tapeio_glob.spider=0x%x, spider_val=0x%x\n", 
	old_spider, tapeio_glob.spider, spider_val);
    return(old_spider);

}


/* set_tapedev()
 *
 * INPUT:   handle:      handle returned by the TAPEALLOC command
 *          device:      value to set in the lvcb.tapedev variable
 *
 * OUTPUT:       0       OK
 *               -X:     Error - Bad Handle
 *
 * Description:
 *
 *      Allow the application program to set the tape device.
 *
 *      0: default - 4980
 *      1:           9 track or exabyte
 *
 * */

int 
set_tapedev(int handle, int tapedev, int no_unld) 
{
    LVCB *lvcb_p;

    if ((lvcb_p = tapemain.lvcblist[handle]) == NULL) {

        fprintf(stderr, "\n%s #%04d ERROR:  %s", __FILE__, __LINE__,
                        err_info[ERR_INVALID_HANDLE].err_str);
        fprintf(stderr, " (%d)\n", handle);
        fflush(stderr);

        return (err_info[ERR_INVALID_HANDLE].err_code);
    }

    lvcb_p->tapedev = tapedev;
    lvcb_p->debug_nounload = no_unld;
    return(0);

}


/* PAGE  */

/* log_time()
 *
 * INPUT:   logdev:      FILE * to the log device
 *
 *
 * Description:
 *
 *      Put the date and time into the log.
 *
 * */

void    log_time(FILE *logdev) {

    int time_val;

    time(&time_val);
    fprintf(logdev, "%s", ctime(&time_val));
}


/* PAGE  */

void    prn_tiostats(LVCB *lvcb_p, TAPE_IO *tape_io_p) {

    log_time(tape_io_p->logdev);

    fprintf(tape_io_p->logdev,
        ">> Tape blocks processed: %d\n", lvcb_p->blockcnt);
    fprintf(tape_io_p->logdev,
        ">> Records processed: %d\n", tape_io_p->num_records);
    fprintf(tape_io_p->logdev,
        ">> ireads issued: %d\n", tape_io_p->num_ireads);
    fprintf(tape_io_p->logdev,
        ">> iwrites issued: %d\n", tape_io_p->num_iwrites);
    fprintf(tape_io_p->logdev,
        ">> cwrites issued: %d\n", tape_io_p->num_cwrites);
    fprintf(tape_io_p->logdev,
        ">> Wait for buffer: %d\n", tape_io_p->oper_blked);
    fprintf(tape_io_p->logdev,
        ">> Buffer busy during poll: %d\n\n", tape_io_p->oper_busy);

}


void
free_iod(iod)
	_iod_t *iod;
{

	if (iod->async) {
		iod->link = free_iod_q;
		free_iod_q = iod;
	}
	else {
		_iod_sync_level--;
	}
}

