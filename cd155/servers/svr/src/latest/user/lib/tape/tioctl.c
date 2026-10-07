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
 *	tioctl.c 11.1 94/03/22 16:49:47
 */
static char	tioctl_ver[] = "@(#) sourcefile tioctl.c 11.1 94/03/22 16:49:47";

/*
 *
 */

/*
 *	Purpose:
 *		This is a collection of tape manipulation functions,
 *		specifically implemented for the 3480 CFS tape
 *		driver.  
 *
 *	Functions Supported:
 *
 *	Interface Description:
 *
 *	History:
 *12-12-91    ejg	Created
 *01-07-92    ejg added test for scsi error even with normal return.
 *                  added test_tape_ready and get_tape_sn routines.
 *
 */

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <fcntl.h>
#include "mtio.h"
#include <sys/stat.h>

extern int   errno;		/* standard UNIX error indicator  */

/************************ RESERVE_TAPE ****************************/

#if	CFS
int
reserve_tape (fn)
    char *fn;
{
BN bn;

set_cfn(fn);
fn = fio_cfn;
if (cfn_in_cfs())
    {
    bn = findname(fio_cfn);
    if (DIRBN(bn))
        {
        errno = EISDIR;
        return -1;
        }
    if (BNERROR(bn))
        {
        errno = ENOENT;
        return -1;
        }
    if (change_file(bn,
                    RESERVE,
                    u.u_procp->p_pid,
                    0,
                    (ulong) 0,
                    (int *) 0) == -1)
        return -1;
    return 0;
    }
errno = EINVAL;
return -1;
}
#endif	/* CFS */

/***************************** RELEASE_TAPE *********************/

#if	CFS
int
release_tape (fn)
    char *fn;
{
BN bn;

set_cfn(fn);
fn = fio_cfn;
if (cfn_in_cfs())
    {
    bn = findname(fio_cfn);
    if (DIRBN(bn))
        {
        errno = EISDIR;
        return -1;
        }
    if (BNERROR(bn))
        {
        errno = ENOENT;
        return -1;
        }
    if (change_file(bn,
                    RELEASE,
                    u.u_procp->p_pid,
                    0,
                    (ulong) 0,
                    (int *) 0) == -1)
        return -1;
    return 0;
    }
errno = EINVAL;
return -1;
}
#endif	/* CFS */

/************************ LOAD_DISPLAY_TAPE **********************/

void
load_display_tape (fd, msg)
    int  fd;
    char *msg;
{
	int ret;
	struct stat buffer;
	if ( (ret = fstat(fd, &buffer)) < 0) 
		printf("load_display_tape: bad fd errno=%d\n",errno);

	ret = ioctl( fd, MTIODISPLY, (load_display_t *) msg );
	if( ret < 0 ) {
		perror("load_display_tape ioctl");
	}
	/*
	 * If the device was not opened by us, we just return.
	 */
	if(fd < 0 && errno == EBUSY){
		printf("load_display_tape: device is EBUSY\n");
		return;
	}
}

/************************** UNLOAD_TAPE ***************************/

#if	CFS
int
unload_tape( fn )
    char *fn;
{
BN bn;
MT3480 ts;
DRIVE_INFO drive_info;

ts.mt3480_op = UNLOAD;
set_cfn (fn);
fn = fio_cfn;
if ( cfn_in_cfs() )
    {
    bn = findname (fio_cfn);
    if ( DIRBN(bn) )
        {
        errno = EISDIR;
        return -1;
        }
    if ( BNERROR(bn) )
        {
        errno = ENOENT;
        return -1;
        }
    if ( change_file( bn,
                     SPECIAL_OP,
                     ts.mt3480_op,
                     0,
                     (ulong)0,
                     (int *)0 ) == -1 )
	{
    	get_drive_info(bn.volume, &drive_info);
    	if ( (drive_info.scsi_sense.key & SCSI_KEY_MASK_LO) == SCSI_KEY_ILLEGAL_REQ )
        	errno = EINVAL;
        return -1;
	}
    get_drive_info(bn.volume, &drive_info);
    if ( (drive_info.scsi_sense.key & SCSI_KEY_MASK_LO) == SCSI_KEY_ILLEGAL_REQ )
        {
        errno = EINVAL;
        return -1;
        }
     return 0;
    }
errno = EINVAL;
return -1;
}
#endif	/* CFS */

/**************************** LOCATE_TAPE *************************/

int
locate_tape_id (fid, bid)
    int fid;
    unsigned int bid;
{
	int ret;
	struct mtop pos;

	pos.mt_op = MTLOCATE;
	pos.mt_count = bid;

printf("locate_tape_id: fid=0x%x, pos.mt_count=0x%x bid=%d / 0x%x\n",
		fid, pos.mt_count, bid, bid);

	if ( ret = ioctl(fid,MTIOCTOP,&pos) < 0 )
		perror("locate_tape_id ioctl");
	else
		printf("tape positioned at block # %08X\n",
			pos.mt_count);
	return(ret);
}

/********************** READ_TAPE_POSITION ***********************/

int
read_tape_position (fid, id_table)
	int fid;
	position_t *id_table;
{
	int ret;

	ret = ioctl( fid, MTIOPOS, id_table);
	if(ret < 0)
		perror("read_tape_position ioctl");
	else
		printf("read_tape_position: tape positioned at block # %08X\n",
			id_table->first_block);
	return(ret);
}

/******************** TEST_DRIVE_READY ***************************/

#if	CFS
int
test_drive_ready( fn )
    char *fn;
{
BN bn;
MT3480 ts;
DRIVE_INFO drive_info;

ts.mt3480_op = TEST_READY;
set_cfn (fn);
fn = fio_cfn;
if ( cfn_in_cfs() )
    {
    bn = findname (fio_cfn);
    if ( DIRBN(bn) )
        {
        errno = EISDIR;
        return -1;
        }
    if ( BNERROR(bn) )
        {
        errno = ENOENT;
        return -1;
        }
    if ( change_file( bn,
                     SPECIAL_OP,
                     ts.mt3480_op,
                     0,
                     (ulong)0,
                     (int *)0 ) == -1 )
	{
    	get_drive_info(bn.volume, &drive_info);
    	if ( (drive_info.scsi_sense.key & SCSI_KEY_MASK_LO) == SCSI_KEY_ILLEGAL_REQ )
       		errno = EINVAL;
    	else if ( (drive_info.scsi_sense.key & SCSI_KEY_MASK_LO) == SCSI_KEY_NOT_READY )
        	errno = EINVAL;
	else
		errno = EIO;
        return -1;
	}
     return 0;
    }
errno = EINVAL;
return -1;
}
#endif	/* CFS */

/************************* GET_TAPE_SN ***************************/

#if	CFS
int
get_tape_sn( fid, ascii_sn )
    int fid;
    unchar *ascii_sn;
{
LCB_HDR lcb;
int mask;
MT3480 ts;
DRIVE_INFO drive_info;
struct D5_DATA {
    unchar  machine_type[4];
    unchar  serial_number[4];
} tsn;
int i;

ts.mt3480_op = SEND_DIAG;
if ( change_file( fds[fid].bn,
                  SPECIAL_OP,
                  ts.mt3480_op,
                  0,
                  (ulong)0,
                  (int *)0 ) == -1 )
    {
    get_drive_info(fds[fid].bn.volume, &drive_info);
    if ( (drive_info.scsi_sense.key & SCSI_KEY_MASK_LO) == SCSI_KEY_ILLEGAL_REQ )
        errno = EINVAL;
    return -1;
    }
get_drive_info(fds[fid].bn.volume, &drive_info);
if ( (drive_info.scsi_sense.key & SCSI_KEY_MASK_LO) == SCSI_KEY_ILLEGAL_REQ )
        {
        errno = EINVAL;
        return -1;
        }
lcb.dpreq.request = CHANGE_FILE;
lcb.dpreq.subrequest = SPECIAL_OP;
lcb.dpcom.bn = fds[fid].bn;
lcb.dpreq.node = _mydirect();
lcb.dpreq.pid = _mypid();
lcb.dpreq.resptype = CFS_DRP_TYPE;
lcb.dpreq.gp1 = REC_DIAG;
lcb.dpreq.gp2 = 0;
lcb.dpreq.gp3 = 0;

mask = masktrap(1);
if (_csendrecv( CFS_DRQ_TYPE,
                &lcb.dpreq,
                sizeof(DPMSG),
                voltab[lcb.dpcom.bn.volume].node,
                DPPID,
                CFS_DRP_TYPE,
                &tsn,
                sizeof(tsn) ) < 0)
    {
    masktrap(mask);
    get_drive_info(fds[fid].bn.volume, &drive_info);
    if ( (drive_info.scsi_sense.key & SCSI_KEY_MASK_LO) == SCSI_KEY_ILLEGAL_REQ )
        errno = EINVAL;
    return -1;
    }
masktrap(mask);
ascii_sn[0] = (tsn.serial_number[0] >> 4) + '0';
ascii_sn[1] = (tsn.serial_number[0] & 0x0f) + '0';
ascii_sn[2] = (tsn.serial_number[1] >> 4) + '0';
ascii_sn[3] = (tsn.serial_number[1] & 0x0f) + '0';
ascii_sn[4] = (tsn.serial_number[2] >> 4) + '0';
ascii_sn[5] = (tsn.serial_number[2] & 0x0f) + '0';
ascii_sn[6] = (tsn.serial_number[3] >> 4) + '0';
ascii_sn[7] = (tsn.serial_number[3] & 0x0f) + '0';
get_drive_info(fds[fid].bn.volume, &drive_info);
if ( (drive_info.scsi_sense.key & SCSI_KEY_MASK_LO) == SCSI_KEY_ILLEGAL_REQ )
    {
    errno = EINVAL;
    return -1;
    }
return 0;
}
#endif	/* CFS */
