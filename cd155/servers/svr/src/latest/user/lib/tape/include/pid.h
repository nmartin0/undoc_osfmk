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
 *	pid.h 11.1 94/03/25 16:00:10
 *
 * .{ TEXT
 * .H 4 pid.h
 *
 * This module defines all system PIDs.
 * 
 * .} TEXT
 * .{ DETAIL
 */

/* ID used by applications to indicate "any pid" */
#define WILD_PID		-1

/* ID used within system to indicate "inactive" or "free" or "no process" */
#define UNUSED_PID		-40

/* ID used for nonexistent memory */
#define NONEXIST_PID		-41

/* ID used for termorary marking */
#define TEMP_PID		-42

/* Used for general system functions */
/*
#define GENERIC_PID		-1
*/

/* Used for host system call errors */
/*
#define HOST_SYSCAL_PID		-2
*/

/* Reserved for node internal system errors, used by cubeload */
/*
#define KERNEL_PID		-3
*/

/* Used by cubelog */
/*
#define CUBELOG_PID		-6
*/

/* System log ID */
#define SYSLOG_PID		-(getpid() % 100 + 101)

/* Error logger PID for sending log messages */
/*
#define ERR_PID			-8
*/

/* Used by cube utilities */
#define BOOTCUBE_PID		-9

/* Used for loader process in node */
#define LOADER_PID		-10

/* Reserved for debugger */
#define DEBUGGER_PID		-11

/* Special PID used to identify prefix of global send message from host */
#define GSPPID			-12

/* Special PID used to identify message body for global send from host */
#define GSMPID			-13

/* Used by getcube/relcube */
#define GETCUBE_PID		-14

/* PID associated with node system buffers */
#define SYSTEM_PID		-15

/* PID of host process on node */
#define GSPROC_PID		-16

/* Used for host debugging */
#define HDPID			-17

/* Used for VX driver */
#define VXPID			-18

/* Used for node system buffers */
#define SYSBUF_PID		-19

/*
 * The following are pids used by the commserver to separate requests
 */

/* User message requests to commserver */
#define COMM_PID		-20

/* Used for DECON */
#define CDBG_PID		-21

/* Dumps message and request queues */
#define DEBUG_PID		-24

/* Starts a compile */
#define COMP_PID		-25

/* Used by setpid */
#define SETPID_PID		-26

/* Used by cubeinfo */
#define INFO_PID		-27

/* Used by attachcube */
#define ATTCH_PID		-28

/* Used by lifeline */
#define LIFELINE_PID		-29

/* Used by to_cube */
#define TO_CUBE_FD		-30

/* Used by newfs */
#define NEWFS_PID		-31

/* Used by flush */
#define FLUSH_PID		-32

/* Used by comm server up */
#define COMMSER_UP_PID		-33

/* Loader process pid */
#define LOAD_PROC_PID		-34

/* Used by attachcube */
#define QMATTCH_PID		-35

/* Used by attachcube */
#define LFLUSH_PID		-36

/* rcam process pid */
#define RCAM_PID		-37

/* tocube process pid */
#define TOCUBE_PID		-38

/* tohost process pid */
#define TOHOST_PID		-39

/* pids -40 to -42 used in NX */

/* kill pid */
#define KILL_PID		-43

/* startcube pid */
#define START_PID		-44

/* set cfs_anchor pid */
#define CFS_ANCHOR_PID		-45

/* get physnode pid */
#define GET_PNODE_PID		-46

/* needforce pid (remote host only) */
#define NFORCE_PID		-47

/* Process Log pid */
#define PLOG_PID		-48

/* CFS pids */
#define DPPID			-60
#define NPPID			-61
#define APPID			-62
#define DCHK_PID		0
#define FCHK_PID		0

/* PID of master file server for file i/o */
/* PID's -64, -65, -66, and -67 reserved */
#define FILE_BASE_PID		-64
#define FILE_TEMP_PID		-68

/* Decon message demon pid (cdemon) */
#define CMSG_PID		-69

/*
 * CCLISP reserved PID's - CCLISP is currently using the range from -80
 *   to -450.  Part of this range will change when CCLISP switches to using
 *   the new improved system file servers.
 */

#define CCLK_MONITOR		-100	/* CCLISP Monitor - for break msgs. */
#define CCLK_READER		-99	/* CCLISP Reader task */
#define CCLK_DISP_SERVER	-98	/* CCLISP Display server on Xenix */
#define CCLK_KBD_SERVER		-97	/* CCLISP Keyboard server on Xenix */
#define CCLK_FILE_MASTER	-96	/* Master file server (OPEN, etc.) */
#define CCLK_FILE_SLAVE1	-90	/* Slave file server (READ, WRITE) */
#define CCLK_FILE_SLAVE2	-89	/* " */
#define CCLK_FILE_SLAVE3	-88	/* " */
#define CCLK_FILE_SLAVE4	-87	/* " */
#define CCLK_FILE_SLAVE5	-86	/* " */
#define CCLK_FILE_SLAVE6	-85	/* " */

#define REMOTE_EVAL_1		-450	/* Used by CCLISP during remote EVAL */
#define REMOTE_EVAL_2		-449	/* Used by CCLISP during remote EVAL */

#define LDONE_PID		-600	/* used by remote commser	*/
#define CUBEUSED_PID		-601	/* used by remote commser	*/

/* .} DETAIL */
