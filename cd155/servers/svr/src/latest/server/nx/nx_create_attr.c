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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992 Intel Corporation.
 *
 *      $Id: nx_create_attr.c,v 1.3 1994/11/18 20:37:43 mtm Exp $
 *
 */

/*
 * nx_create_attr.c
 *
 * HISTORY
 * $Log: nx_create_attr.c,v $
 * Revision 1.3  1994/11/18  20:37:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/20  18:00:20  mag
 * Added missing "scsi" attribute to scsi nodes
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 10231
 *  Testing: developer
 *  Module(s): nx_create_attr.c
 *
 * Revision 1.1  1994/06/01  20:35:51  mag
 * Initial revision
 *
 */

#undef	KERNEL

#include <mach.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <mach/norma_special_ports.h>
#include <device/device.h>
#include <device/disk_status.h>

int AddToAttributes(char*);
int bSimpleProbe(char*);
int SCSIProbe(char*, int, char*);
char* CopyStrip(char*, char*, int);
int GetMasterPort(mach_port_t);


typedef struct SCSI_Inquiry_Data {
#define SCSI_DISK       0x00
#define SCSI_TAPE       0x01
#define SCSI_PRINTER    0x02
#define SCSI_CPU        0x03
#define SCSI_WORM       0x04
#define SCSI_CDROM      0x05
#define SCSI_JUKEBOX    0x08
        unsigned int    Periph_Device_Type      : 5,
                Periph_Qualifier                : 3,
                Device_Type_Qualifier           : 7,
                Removable_Media                 : 1,
                ANSI_Version                    : 3,
                ECMA_Version                    : 3,
                ISO_Version                     : 2,
                Response_Data_Format            : 4,
                reserved_0                      : 3,
                AENC                            : 1;
        unsigned int    Additional_Length       : 8,
                reserved_1                      :16,
                SftReset                        : 1,
                CmdQue                          : 1,
                reserved_2                      : 1,
                Linked                          : 1,
                Sync                            : 1,
                WBus16                          : 1,
                WBus32                          : 1,
                RelAdr                          : 1;
        char    Vendor_ID[8];
        char    Product_ID[16];
        char    Revision_Level[4];
} SCSI_Inquiry_Data_t;


typedef struct Inquiry_CDB {
#define	SCSI_Inquiry	0x12
        u_int   Op_Code         : 8,
                EVPD            : 1,
                Resv1           : 4,
                LUN             : 3,
                Pg_Code         : 8,
                Reserved        : 8;
        u_char  Allocation_Length;
        u_char  Control_byte;
} Inquiry_CDB_t;


/*****************************************************************************
 * Create the node attribute strings for the node on which I a running
 *
 * Arguments:
 *
 *	portHostPriv		The priviledged host post
 *	pszNodeAddtributes	pointer to buffer to receive the attributes
 *	nBufLen			size of the supplied buffer
 *
 * Return:
 *	 0			Success
 *	-1			Some system call failed
 *	-2			Supplied buffer was too small
 *
 *****************************************************************************/

static int nCount;
static char* pszBufPtr;
static int nMaxLen;

static int nNodeSelf;
static mach_port_t portDeviceMaster;

int
nx_create_node_attributes(
  mach_port_t portHostPriv, char* pszNodeAttributes, int nBufLen)
{
    kern_return_t kr;
    struct host_basic_info HostInfo;
    char szBuff[30];
    char szDeviceIDString[100];
    int bEm, bHippi, bDisk, bTape, bPrinter, bWorm, bCDROM, bMediaChanger,
      bIO, bUnknownScsi, bSCSI;
    int nID;

    GetMasterPort(portHostPriv);

    pszBufPtr = pszNodeAttributes;
    *pszBufPtr = '\0';
    nMaxLen = nBufLen;

    nCount = HOST_BASIC_INFO_COUNT;
    kr = host_info(mach_host_self(), HOST_BASIC_INFO,
      (host_info_t) &HostInfo, (mach_msg_type_number_t*) &nCount);

    if (kr != KERN_SUCCESS) {
	return -1;
    }
	
    sprintf(szBuff, "%dproc,%dmb",
      HostInfo.avail_cpus, HostInfo.memory_size/(1024*1024));

    if (!AddToAttributes(szBuff))
	return -2;

    switch(HostInfo.cpu_subtype) {
    case CPU_SUBTYPE_PARAGON860:
	if (!AddToAttributes(",GP"))
	    return -2;
	break;
    case CPU_SUBTYPE_PARAGON860_MP:
	if (!AddToAttributes(",MP"))
	    return -2;
	break;
    default:
	return -1;
    }

/*
    if (mcmsg_nx_op(1) & 1)
	if (!AddToAttributes(",MCP"))
	    return -2;
 */

    bIO = 0;

    bEm = bSimpleProbe("em");
    bHippi = bSimpleProbe("ifhip");

    if (bEm || bHippi) {
	bIO = 1;
	if (!AddToAttributes(",net"))
	    return -2;
    }

    if (bEm)
	if (!AddToAttributes(",enet"))
	    return -2;

    if (bHippi)
	if (!AddToAttributes(",hippi"))
	    return -2;

    bDisk = 0;
    bTape = 0;
    bPrinter = 0;
    bWorm = 0;
    bCDROM = 0;
    bMediaChanger = 0;
    bUnknownScsi = 0;

    for (nID=0; nID<7; nID++) {
	szDeviceIDString[0] = '\0';

	switch (SCSIProbe("rz", nID, szDeviceIDString)) {
	case SCSI_DISK:		bDisk = 1;	    bSCSI = 1; bIO = 1; break;
	case SCSI_TAPE:		bTape = 1;	    bSCSI = 1; bIO = 1; break;
	case SCSI_PRINTER:	bPrinter = 1;	    bSCSI = 1; bIO = 1; break;
	case SCSI_WORM:		bWorm = 1;	    bSCSI = 1; bIO = 1; break;
	case SCSI_CDROM:	bCDROM = 1;	    bSCSI = 1; bIO = 1; break;
	case SCSI_JUKEBOX:	bMediaChanger = 1;  bSCSI = 1; bIO = 1; break;
	case -1:		/* no device */	    break;
	default:		bUnknownScsi = 1;   bSCSI = 1; bIO = 1; break;
	}

	if (szDeviceIDString[0])
	    if (!AddToAttributes(",") || !AddToAttributes(szDeviceIDString))
		return -2;
    }

    if (bSCSI)
	if (!AddToAttributes(",scsi"))
	    return -2;

    if (bDisk)
	if (!AddToAttributes(",disk"))
	    return -2;

    if (bTape)
	if (!AddToAttributes(",tape"))
	    return -2;

    if (bPrinter)
	if (!AddToAttributes(",printer"))
	    return -2;

    if (bWorm)
	if (!AddToAttributes(",worm"))
	    return -2;

    if (bCDROM)
	if (!AddToAttributes(",cdrom"))
	    return -2;

    if (bMediaChanger)
	if (!AddToAttributes(",media_changer"))
	    return -2;

    if (bIO)
	if (!AddToAttributes(",io"))
	    return -2;

    if (bUnknownScsi)
	if (!AddToAttributes(",unknown scsi"))
	    return -2;

    return 0;
}


int
AddToAttributes(char* pszAttr)
{
    int nLen = 0;
    char* pszChar = pszAttr;

/* How long is the string, and will it fit? */

    while (*pszChar++)
	nLen++;

    if ((nCount+nLen) >= nMaxLen)
	return 0;

    bcopy(pszAttr, pszBufPtr, nLen+1);
    pszBufPtr += nLen;
    nCount += nLen;
    return 1;
}


int
GetMasterPort(mach_port_t portHostPriv)
{
    if (norma_node_self(mach_task_self(), &nNodeSelf) != KERN_SUCCESS)
	return -1;

    if (norma_get_device_port(portHostPriv, nNodeSelf, &portDeviceMaster) !=
      KERN_SUCCESS)
	return -1;
    
    return 0;
}


int
bSimpleProbe(char* pszDev)
{
	kern_return_t kr;
	mach_port_t portDevice;

	kr = device_open(portDeviceMaster, D_READ, pszDev, &portDevice);

	if (kr != KERN_SUCCESS)
	    return 0;

	device_close(portDevice);

	return 1;
}


#define	STATUS_SIZE(x)	(sizeof(x)/sizeof(int))

int
SCSIProbe(char* pszDev, int nID, char* pszDeviceIDString)
{
	kern_return_t kr;
	mach_port_t portDevice;
	mach_msg_type_number_t	status_count;
	char szDeviceName[100];
	struct scsi_io ScsiIo;
	Inquiry_CDB_t* p = (Inquiry_CDB_t*) ScsiIo.cdb;
	SCSI_Inquiry_Data_t* d = (SCSI_Inquiry_Data_t*) ScsiIo.buf;
	char* pszChar;

	ScsiIo.cmd_count = sizeof(Inquiry_CDB_t);
	ScsiIo.direction = SCSI_TO_MEM;
	ScsiIo.buf_len = sizeof(SCSI_Inquiry_Data_t);

	bzero(p, sizeof(Inquiry_CDB_t));
	p->Op_Code = SCSI_Inquiry;
	p->Allocation_Length = sizeof(SCSI_Inquiry_Data_t);

	sprintf(szDeviceName, "%s%dp", pszDev, nID);

	kr = device_open(portDeviceMaster,
	  D_READ, szDeviceName, &portDevice);

	if (kr != KERN_SUCCESS)
	    return -1;

	kr = device_set_status(portDevice, DIOCSCSI,
	  (dev_status_t) &ScsiIo, STATUS_SIZE(ScsiIo));

	if (kr != KERN_SUCCESS)
	    return -1;

	status_count = STATUS_SIZE(ScsiIo);

	kr = device_get_status(portDevice, DIOCSCSI,
	  (dev_status_t) &ScsiIo, &status_count);

	if (kr != KERN_SUCCESS)
	    return -1;

	device_close(portDevice);

	pszChar =
	  CopyStrip(pszDeviceIDString, d->Vendor_ID, sizeof(d->Vendor_ID));
	*pszChar++ = ' ';

	pszChar =
	  CopyStrip(pszChar, d->Product_ID, sizeof(d->Product_ID));
	*pszChar++ = ' ';

	pszChar =
	  CopyStrip(pszChar, d->Revision_Level, sizeof(d->Revision_Level));

	return d->Periph_Device_Type;
}


/*
 * Copy chars from pszSource to pszDest.  Copy no more than nLen chars.
 * Remove trailing spaces.  Return pointer to terminating 0 byte.
 */

char*
CopyStrip(char* pszDest, char* pszSource, int nLen)
{
    char* pszRet;
    int bStripping = 1;

    pszDest += nLen;
    pszSource += nLen-1;

/* This is in case *all* characters are copied */

    pszRet = pszDest;
    *pszDest-- = '\0';

    while (nLen--) {
	if (bStripping && *pszSource == ' ') {
	    *pszDest = '\0';
	    pszRet = pszDest;
	} else {
	    *pszDest = *pszSource;
	    bStripping = 0;
	}
	pszDest--;
	pszSource--;
    }

    return pszRet;
}
