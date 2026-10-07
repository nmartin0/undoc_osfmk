/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
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
 *      filename.h 9.5 92/04/27 15:53:08
 *
 * .{ TEXT
 *
 * .H 4 filename.h
 *
 * This file contains the file names used in the system.
 * 
 * .} TEXT
 */

#define FS_FNAME        "/usr/ipsc/lib/fserver"
#define CS_FNAME        "/usr/ipsc/lib/commser"
#define LIFE_FNAME      "/usr/ipsc/lib/lifeline"
#define NX_386_FNAME    "/usr/ipsc/lib/nx.b"
#define NX_860_FNAME    "/usr/i860/ipsc/lib/nx.b"
#define RC0_FNAME       "/usr/ipsc/lib/rc0"
#define RC1_FNAME       "/usr/ipsc/lib/rc1"
#define RC2_FNAME       "/usr/ipsc/lib/rc2"
#define RC3_FNAME       "/usr/ipsc/lib/rc3"
#define HB_FNAME        "/usr/ipsc/lib/hbits"
#define BL_386_FNAME    "/usr/ipsc/lib/bootld"
#define BL_860_FNAME    "/usr/i860/ipsc/lib/bootld"
#define SOCKPROC_FNAME  "/usr/ipsc/lib/sockproc"
#define FSERVPIDNAME    "/tmp/fs.pid"
#define CUBECONF_FNAME  "/usr/ipsc/conf/cubeconf"
#define PLCK_FNAME      "/usr/ipsc/log/plock"

/*
 * MACS files
 */

#define NXACCOUNT_FNAME	"/etc/nxaccount"
#define NXACCOMM_FNAME	"/etc/nxaccomm"
#define NXACCOUNT_TEMP_FNAME	"/etc/nxaccount.macs"
#define NXACCOMM_TEMP_FNAME	"/etc/nxaccomm.macs"

#define MACS_START_SCRIPT "/sbin/init.d/macs"
#define MACS_MACD_FNAME	"/usr/lib/macs/macd"
#define MACHEART_FNAME	"/usr/lib/macs/macheart"

#define MACS_JREC_FNAME	"/usr/bin/jrec"
#define MACS_AREP_FNAME	"/usr/bin/arep"
#define MACLIST_FNAME	"/usr/bin/maclist"

#define MACS_CONF_FILE  "/usr/spool/macs/conf/macs.conf"
#define MACS_NQSTABLE_FILE  "/usr/spool/macs/conf/nqstable"
#define MACS_ACTABLE_FILE  "/usr/spool/macs/conf/actable"

#define MACD_DATABASE_FILE  "/usr/spool/macs/private/macd.data"

#define MACS_LOG_PATH	"/usr/spool/macs/log.d"
#define MACD_START_LOG	"/usr/spool/macs/log.d/macdstart.log"
#define MACD_ERR_LOG	"/usr/spool/macs/log.d/macderr.log"
#define MACD_LOG_PATH	"/usr/spool/macs/log.d/macdlog.d"
#define MACUPD_LOG_PATH	"/usr/spool/macs/log.d/macupdlog.d"
#define MACADMIN_LOG_FNAME	"/usr/spool/macs/log.d/macadmin.log"
#define MACD_LASTLOG_FNAME	"/usr/spool/macs/log.d/last.log"
#define MACD_TEMPLOG_FNAME	"/usr/spool/macs/log.d/temp.log"
#define HEART_BEAT_FNAME	"/usr/spool/macs/log.d/heart.beat"
#define BOOTMESH_LOG	"/etc/bootmesh.log"
#define LASTBOOT_LOG	"/usr/spool/macs/log.d/lastboot.log"
#define LASTBOOT_LOG_TMP	"/usr/spool/macs/log.d/lastboot.tmp"
