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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
/* @(#)audit.h	6.3 15:01:31 2/17/91 SecureWare */
#ifndef _SYS_AUDIT_H_
#define _SYS_AUDIT_H_

#if SEC_BASE

/*
 * Copyright (c) 1987-1990 SecureWare, Inc.  All Rights Reserved.
 *	
 * Based on:
 *	@(#)audit.h	2.16.3.7 10:18:40 2/5/90 SecureWare
 *
 * Definitions required by the audit subsystem and the audit device.
 */

#include <sys/secdefines.h>
#include <sys/auditsysc.h>
#include <sys/security.h>


/*
 * General Subsystem Constant Definitions
 */

#define PRIAUDIT	(PZERO - 1)	/* subsystem sleep priority */

#define	AUDIT_KPATHSIZE		128	/* kernel pathname size maximum */
#define	AUDIT_PATHSIZE		1024	/* user pathname size maximum */
#define	AUDIT_SYSNMSIZE		8	/* system name size maximum */
#define	AUDIT_NAMESIZE		8	/* user name size maximum */
#define	AUDIT_DIGITS		5	/* digits in sequence, session string */
#define AUDIT_PATH_ALLOC	1024	/* exec path maximum size */

#if SEC_AUDIT_SYMLINKS
#define AUDIT_SYMLINK_BUFSIZE	4096	/* symbolic link pathname maximum */
#endif

/* Subsystem read count/buffer space minimum thresholds */

#define AUDIT_MINIMUM_READ_COUNT	128	/* minimum read count */
#define AUDIT_MINIMUM_BUFFER_SIZE	32768	/* minimum buffer size */

/* Subsystem Default Values */

#define AUDIT_DEFAULT_READ_COUNT	4096	/* default daemon read count */
#define AUDIT_DEFAULT_MAX_COMPACT	1000000	/* compaction file max size */

/* Constants for Audit Minor Devices */

#define	AUDIT_READ_DEV	0		/* exclusive read device */
#define	AUDIT_WRITE_DEV	1		/* subsystem record write device */

/* Subsystem sleep flag for audit_getspace() */

#define AUDIT_NO_SLEEP	0		/* don't sleep waiting for space */
#define AUDIT_SLEEP	1		/* sleep if necessary for space */

/* Audit subsystem flags used in the secinfo structure */

#define AUDIT_SIP_RECALC	0x1	/* recalculate audit mask */
#define AUDIT_SIP_STARTUP	0x2	/* generate startup info record */
#define	AUDIT_SIP_IS_DAEMON	0x200	/* proc is daemon or child thereof */

/* Subsystem Defined Directory and File Names */

#define	AUDIT_RDEVICE	"/dev/auditr"		/* audit read device */
#define	AUDIT_WDEVICE	"/dev/auditw"		/* audit write device */

#define AUDIT_DMNPATH	"/tcb/bin/auditd"	/* audit daemon program */
#define	AUDIT_PARMSFILE	"/tcb/files/audit/audit_parms"     /* parameter file */
#define	AUDIT_SELFILE	"/tcb/files/audit/audit_select"  /* default sel file */

#define	AUDIT_LOGDIR	"/tcb/files/audit/"	/* logfile directory */
#define AUDIT_LOG_FILENAME	"CAFLOG.xxxxx"	/* compacted file name string */
#define	AUDIT_REDUCE_PARM_DIR	"/tcb/files/auditrparms/"
					/* select parameter file directory */


/*
 * Audit Subsystem macro definitions
 */

/*
 * Macro to recompute a process's audit event mask
 */

#define	emask_calc(cont, disp, i)	((~(cont) & (aud_cont.sys_mask[i])) |\
					((cont) & (disp)))

/* Macro to generate a system call audit record */

#ifdef _OSF_SOURCE
#define AUDIT_GENERATE_RECORD(args, error, retval)\
	if (AIP->si_audit_flags & AUDFLAG_ENABLED)\
		audit(args, error, retval);
#else
#define AUDIT_GENERATE_RECORD(path)\
	if (AIP->si_audit_flags & AUDFLAG_ENABLED)\
		audit(path);
#endif

#if SEC_AUDIT_SYMLINKS

/* Macro for auditing symbolic link contents */

#define	AUDIT_GENERATE_SYMLINK(head,tail,taillen,link,linklen)\
	if ((AIP->si_audit_flags & (AUDFLAG_ENABLED | AUDFLAG_SYMLINK)) ==\
			(AUDFLAG_ENABLED | AUDFLAG_SYMLINK))\
		audstub_symlink(head,tail,taillen,link,linklen);

#endif


/*
 * Definition of the various type of objects supported by the system
 * Changes to this table must be reflected in objects[] in seccmd/reduce.h
 */

/* Kernel objects */

#define	OT_REGULAR		1	/* regular file */
#define	OT_DIRECTORY		2	/* directory */
#define	OT_DEVICE		3	/* block, char device */
#define	OT_PIPE			4	/* named, unnamed pipe */
#define	OT_PROCESS		5	/* self explanatory */
#define	OT_MEMORY		6	/*	""	*/
#define	OT_STREAM		7	/* V.3 stream */
#define	OT_SOCKET		8	/* Berkeley socket */
#define	OT_SHARED_MEMORY	9	/* shared memory segment */
#define	OT_SEMAPHORE		10	/* semaphore */
#define	OT_MESSAGE_QUEUE	11	/* message queue */
#define OT_FILE_DESCR		12	/* file descriptor */
#define	OT_SYMLINK		13	/* symbolic link */

/* Authentication database objects */

#define	OT_PWD			14	/* /etc/passwd */
#define	OT_GRP			15	/* /etc/group */
#define	OT_PRPWD		16	/* Protected Password Database */
#define	OT_TERM_CNTL		17	/* Terminal Control Database */
#define	OT_FILE_CNTL		18	/* File Control Database */
#define	OT_DFLT_CNTL		19	/* System Default Database */
#define	OT_SUBSYS		20	/* Subsystem Database */
#define OT_DEV_ASG		21	/* Device Assignment Database */
#define OT_LP_CNTL		22	/* Printer Control Database */

/* Window System objects */

#define OT_WINDOW		23	/* top level window */
#define OT_OVERRIDE_WINDOW	24	/* override redirect window */
#define	OT_PIXMAP		25	/* pixmap */
#define	OT_COLORMAP		26	/* colormap */
#define	OT_PROPERTY		27	/* window property */
#define	OT_ATOM			28	/* window system atom */

/*
 * Record Type constants used to signify the specific record
 * type and record content.  This value is determined from the
 * syscall_audit table on system call auditing and by the
 * application for self auditing.  The record type is indicated
 * in the audit record header.
 */

/* Record types generated by the kernel */

#define	RT_NORECORD		0	/* not an audit record */
#define	RT_SYSCALL		1	/* system call, no args */
#define	RT_SYSCALL_1		2	/* system call, 1 argument */
#define	RT_SYSCALL_2		3	/* system call, 2 arguments */
#define	RT_SECURITY		4	/* security parameter syscall */
#define	RT_ID_CHANGE		5	/* process id change record */
#define	RT_DAC_CHANGE		6	/* object DAC change */
#define	RT_BOOT_DOWN		7	/* startup/shutdown */
#define RT_PROCESS_INFO		10	/* startup process info record */

/* Record types generated by applications */

#define	RT_LOGIN		20	/* new process login record */
#define	RT_PASSWORD		21	/* password record */
#define	RT_LOCK			22	/* terminal/user lock record */
#define	RT_AUDIT		24	/* audit subsystem record */
#define	RT_DATABASE		25	/* system database records */
#define	RT_SUBSYSTEM		26	/* subsystem activity record */
#define RT_WINDOW_SYSTEM	27	/* window system audit record */
#define	RT_WINDOW_SERVER	28	/* window server audit record */

#define RT_APPLICATION		20	/* first application RT define */
#define RT_APPLICATION_MAX	28	/* largest application record type */

/*
 * Define the number of application record types so that each will have
 * its own array of threshold counts.
 */

#define	RT_APPLS	(RT_APPLICATION_MAX - RT_APPLICATION + 1)

/*
 * Event subcodes for various record types
 * Changes to event subcodes must be reflect in seccmd/reduce.h
 */

/* Kernel Startup/Shutdown Events */

#define	ES_KERNEL_STARTUP	1	/* boot record */
#define	ES_KERNEL_SHUTDOWN	2	/* shutdown record */

/* Login/Logoff Events:  RT_LOGIN */

#define	ES_LOGIN_LOCAL		1	/* successful login */
#define	ES_LOGIN_REMOTE		2	/* successful login */
#define	ES_LOGIN_FAILED		3	/* failed login */
#define	ES_LOGOFF		4	/* logoff */
#define ES_AUTOLOG_TIMER	5	/* TPATH: auto-logoff by timer */
#define ES_SESSION		6	/* TPATH: new trusted path session */
#define ES_TPLOGIN		7	/* TPATH: successful tpath user login */
#define ES_TPLOGOFF		8	/* TPATH: trusted path logoff */
#define ES_AUTOLOG_MULTI	9	/* CHOTS: auto-logoff multi login */
#define ES_MULTI_FAILED		10	/* CHOTS: login failed multi login */
#define ES_2P_SUCCESS		11	/* CHOTS: two person check succeeded */
#define ES_2P_FAILED		12	/* CHOTS: two person check failed */
#define ES_LOGIN_MAX		12

/* Password Events:  RT_PASSWORD */

#define	ES_PW_CHANGE		1	/* pw change */
#define	ES_PW_CHANGE_FAILED	2	/* pw change failed */
#define	ES_PW_CHANGE_NOPRIV	3	/* no priv for pw change */
#define ES_PW_MAX		3

/* Locking Events:  RT_LOCK */

#define	ES_USER_LOCK		1	/* login failure for locked user */
#define	ES_TERM_LOCK		2	/* login failure for locked terminal*/
#define ES_SET_USER_LOCK	3	/* lock user account */
#define ES_SET_USER_UNLOCK	4	/* unlock user account */
#define ES_SET_TERM_LOCK	5	/* lock terminal */
#define ES_SET_TERM_UNLOCK	6	/* unlock terminal */
#define	ES_TIME_LOCK		7	/* login failure for time lock */
#define ES_LOCK_MAX		7

/* Audit Subsystem Events:  RT_AUDIT */

#define ES_AUD_ENABLE		1	/* audit enabled */
#define	ES_AUD_DISABLE		2	/* disable auditing */
#define	ES_AUD_MODIFY		3	/* subsystem parameter modified */
#define	ES_AUD_REPORT		4	/* audit report generation */
#define	ES_AUD_ARCHIVE		5	/* compacted audit file archival */
#define	ES_AUD_DMN_INIT		6	/* compaction daemon start */
#define	ES_AUD_DMN_END		7	/* compaction daemon termination */
#define	ES_AUD_SHUTDOWN		8	/* audit shutdown notification */
#define	ES_AUD_ERROR		9	/* audit error condition */
#define ES_AUD_MAX		9

/* Database Activity Events:  RT_DATABASE */

#define	ES_DB_INTEGRITY		1	/* Auth database integrity */
#define	ES_FS_INTEGRITY		2	/* File system discr integrity */
#define	ES_DB_RESOURCE		3	/* Auth database resource denial */
#define	ES_SEC_FAILURE		4	/* Security action failed in auth DB */
#define ES_DB_MAX		4

/* Window System Events:  RT_WINDOW_SYSTEM and RT_WINDOW_SERVER */

#define	ES_WS_DISPLAY_OPEN	1	/* display connection opened */
#define ES_WS_DISPLAY_CLOSED	2	/* display connection closed */
#define ES_WS_WINDOW_MANAGE	3	/* manage a client window */
#define ES_WS_WINDOW_UNMANAGE	4	/* unmanage a client window */
#define ES_WS_TP_MANAGE		5	/* manage trusted path client window */
#define ES_WS_IL_CHANGE		6	/* window information label change */
#define ES_WS_IL_FLOAT		7	/* window information label float */
#define ES_WS_IIL_CHANGE	8	/* window input information label chg */
#define ES_WS_SL_CHANGE		9	/* window input information label chg */
#define ES_WS_RUN_LEVEL_CHANGE	10	/* trusted path run level change */
#define ES_WS_CUT_PASTE		11	/* successful data paste */
#define ES_WS_CUT_PASTE_ERROR	12	/* setup error on cut paste */
#define ES_WS_FILE_IL_CHANGE	13	/* file information label change */
#define ES_WS_FILE_IL_FAILED	14	/* file information label error */
#define ES_WS_FILE_SL_CHANGE	15	/* file sensitivity label change */
#define ES_WS_FILE_SL_FAILED	16	/* file sensitivity label error */
#define ES_WS_FILE_BOTH_CHANGE	17	/* file il/sl label change */
#define ES_WS_FILE_BOTH_FAILED	18	/* file il/sl label error */
#define	ES_WS_ROLE_ASSUMED	19	/* enter an administrative role */
#define	ES_WS_ROLE_ENDED	20	/* leave an administrative role */
#define ES_WS_SL_DOWNGRADE	21	/* SL downgrade needed for paste */
#define ES_WS_IL_DOWNGRADE	22	/* window IL downgrade */
#define ES_WS_PW_CHANGE		23	/* sucessful password change */
#define ES_WS_PW_CHANGE_FAILED	24	/* unsuccessful password change */
#define ES_WS_PW_NO_AUTH	25	/* no authorization to change pw */
#define ES_WS_CHANGE_CLRNCE	26	/* downgrade session clearance */
#define	ES_WS_ACCESS_DENIED	27	/* server access denial */
#define	ES_WS_INSUFF_PRIV	28	/* server privilege denial */
#define	ES_WS_SERVER_RESET	29	/* server reset */
#define ES_WS_MAX		29

/*
 * Define the largest event subcode that is used by any of the above
 * audit record types. If event subcodes are added, make the appropriate
 * change here if the largest group is expanded.
 */

#define ES_MAX_CODE	ES_WS_MAX	/* based on WINDOW_SYSTEM group */


/*
 * Data structure used to control the auditing of each system call.
 */

struct syscall_audit {
	uint	disp;		/* disposition flags */
	uint	event_type;	/* default event type */
	uint	record_type;	/* default record type */
};

/* Bit definitions for the disp field */

#define	AE_MAND		0x1		/* mandatory audit event */
#define AE_ERRONLY	0x2		/* audit on error only */
#define	AE_SELECT	0x4		/* audit if event selected */

#ifdef KERNEL

/*
 * Data structure representing the internal state of the audit subsystem.
 * Several different locks protect this structure.  Each field is
 * annotated to show what lock protects that field.
 * CONF    - field protected by the audit configuration lock
 * AUDIT   - field protected by the audit lock
 * BOTH    - can't write unless holding BOTH audit lock and configuration lock
 *         - holding either one prevents state change except in fatal
 *         - error paths
 * NONE	   - not locked (constants or unimportant data)
 * LOCK	   - the actual lock
 *
 * Note that we use ints here to hold many things that could fit
 * just as well in a char.  This is done to avoid possible problems on
 * machines that can't do atomic memory operations on anything smaller than
 * an int.  (Load, mask, store could cause problems if other fields in
 * that word weren't protected by the same lock that protects the
 * byte we're trying to manipulate.)
 */

struct audit_control {
	char	auditid[8];	/* NONE		ASCII "AUDCONT" constant*/

	/*
	 * Some of these fields aren't really protected by conventional
	 * locks, but we know that they're only set when auditing is
	 * disabled, and tested when auditing is enabled.  In particular,
	 * panic and symlink are constant once auditing is enabled, and
	 * only tested after auditing is enabled.
	 */

	int	panic;		/* CONF		panic on error		*/

#ifdef SEC_AUDIT_SYMLINKS
	int	symlink;	/* audit symbolic links		        */
#endif

	int	init;		/* CONF		initialized		*/
	int	enabled;	/* BOTH		audit is enabled	*/
	int	terminated;	/* AUDIT	auditing terminated	*/
	int	no_sleep;	/* 		daemon can't sleep	*/



#ifdef	_OSF_SOURCE
	lock_data_t config_lock;/* LOCK		audit configuration lock*/
	lock_data_t lock;	/* LOCK		audit subsystem lock    */
#else
	uint	config_lock;	/* LOCK		audit configuration lock*/
	uint	lock;		/* LOCK		audit subsystem lock    */
#endif

	ulong	event_id;	/* AUDIT	event sequence number   */

	/* Variables to track the exclusive read process */

	struct proc *d_procp;	/* CONF		daemon proc pointer     */
	pid_t	d_pid;		/* CONF		daemon process id       */
	pid_t	lock_pid;	/* AUDIT	last pid to audit_lock()*/
	ulong	lock_count;	/* AUDIT	audit lock count        */
	ulong	unlock_count;	/* AUDIT	audit unlock count      */

	uint	session;	/* CONF		system boot session #   */

	/* Subsystem tunable collection parameters */

	mask_t	sys_mask[AUDIT_MASK_SIZE]; /*	CONF system event mask  */

	uint	uid_count;	/* CONF		user id select count    */
	uint	gid_count;	/* CONF		group id select count   */
	char	*id_pool;	/* CONF		ptr to id buffer pool   */
	uint	idpool_size;	/* CONF		size of id buffer pool  */
	uid_t	*uids;		/* CONF		uid array	        */
	gid_t	*gids;		/* CONF		gid array	        */

	/* Process startup information record control */

	uint	procinfo_size;	/* CONF		size of procinfo buffer */
	char	*procinfo_addr;	/* CONF		procinfo buffer address */

	/* sensitivity level limits */
#if SEC_MAC
	tag_t	slevel_min;	/* CONF		lower collection limit  */
	tag_t	slevel_max;	/* CONF		upper collection limit  */
#endif

	/* Daemon process control variables */

	uint	read_count;	/* AUDIT	daemon proc read count  */
	long	read_offset;	/* AUDIT	daemon proc read offset */

	/* Audit buffer pool variables	*/

	uint	buff_size;	/* AUDIT	size of audit buffer    */
	uint	buff_offset;	/* AUDIT	current buffer offset   */
	uint	buff_count;	/* AUDIT	# unread bytes in buffer*/
	char	*buff_addr;	/* AUDIT	address of audit buffer */

	/*
	 * Remaining fields are used to retain boot information.
	 * These are meaningless without boot PROM changes.
	 */

	ino_t	kinode;		/* NONE		kernel boot inode       */
	char	kpath[AUDIT_KPATHSIZE];
				/* NONE		kernel path name	*/
};

extern struct audit_control aud_cont;

/*
 * Return 1 if a process is registered as the audit daemon
 * Callers must be careful to avoid races with audit startup and termination
 */

#define AUDIT_DAEMON_RUNNING() (aud_cont.d_procp && aud_cont.d_pid)

/*
 * Return 1 if the given bit is set in the current process' event mask.
 */
#define audit_emask_bit_set(i)	ISBITSET(SIP->si_emask, i)

/*
 * Return 1 if the process whose proc structure address is "p" is
 * the audit daemon. This may not be reliable unless p == u.u_procp.
 */

#define IS_AUDIT_DAEMON(p) \
	((aud_cont.d_procp == (p)) && (aud_cont.d_pid == (p) -> p_pid))

#endif	/* KERNEL */


/*
 * Descriptor for variable length fields in audit records
 */

typedef union {
	char	*ptr;		/* string pointer */
	uint	count;		/* string lngth */
} ptr_t;


/*
 * Structure definitions for the various record types generated by
 * the kernel and application programs that are auditing themselves.
 */

/* Common header for all records */

struct audit_header {
	ushort	rec_length;	/* record length plus header */
	ushort	obj_type;	/* object type */
	time_t	tstamp;		/* date/time stamp */
	ulong	event_id;	/* event sequence id */
	ushort	event_type;	/* event type */
	ushort	record_type;	/* audit record type */
	pid_t	pid;		/* process id */
};

/*
 * Generic system call record, no pathnames.
 */

struct syscall_record {		/* RT_SYSCALL */
	struct audit_header aud_hdr;
	short	syscall;	/* system call number */
	long	arg[4];		/* user syscall arguments */
	short	rval1;		/* syscall return values */
	short	rval2;
	char	error;		/* u.u_error return value */
	char	sec_error;	/* AIP->si_error return value */
	ushort	audit_flags;	/* process audit flags */
	short	privilege;	/* privilege identifier */
	ushort	access;		/* policy access code */
	ushort	access_mode;	/* policy access mode */
#if SEC_MAC
	tag_t	pslevel;	/* process security level */
	tag_t	oslevel;	/* object security level */
	tag_t	oslevel2;	/* object security level */
#endif
#if SEC_ILB
	tag_t	pilevel;	/* process information level */
	tag_t	oilevel;	/* object information level */
	tag_t	oilevel2;	/* object information level */
	tag_t	ifloat;		/* subect/object float information level */
#endif
#if SEC_NCAV
	tag_t	pncav;		/* process caveat set */
	tag_t	oncav;		/* object caveat set */
	tag_t	oncav2;		/* object caveat set */
#endif
	privvec_t	priv;	/* privilege use vector */
};

#define	SYSREC_SIZE	(sizeof(struct syscall_record))

/* Bit definitions for the audit_flags field */

#define AUDFLAG_SUID		0x1	/* setuid program */
#define AUDFLAG_SGID		0x2	/* setgid program */
#define AUDFLAG_SIGTERM		0x4	/* process terminated by signal */
#define AUDFLAG_LUID_UNSET	0x8	/* process luid not set */
#if SEC_ILB
#define AUDFLAG_OBJECT_FLOAT	0x10	/* object label floated on call */
#define AUDFLAG_SUBJECT_FLOAT	0x20	/* subject label floated on call */
#define AUDFLAG_OBJECT_RESET	0x40	/* object label reset when empty */
#endif
#define	AUDFLAG_PATH1		0x80	/* first pathname processing */
#define	AUDFLAG_PATH2		0x100	/* second pathname processing */
#define	AUDFLAG_SYMLINK		0x200	/* symlink auditing enabled */
#define	AUDFLAG_ENABLED		0x400	/* auditing enabled */

/*
 * Record format for system calls with one pathname parameter.
 */

struct syscall_record1 {	/* RT_SYSCALL_1 */
	struct syscall_record syscall_rec;
	struct exec_info {
		privvec_t  effpriv;	/* effective privileges */
		uid_t	   euid;	/* effective uid (SUID program) */
		gid_t	   egid;	/* effective gid (SGID program) */
	}	exec_info;
	ptr_t	text1;		/* path name pointer */
#if SEC_AUDIT_SYMLINKS
	ptr_t	symlink1;	/* symbolic link path */
#endif
};

#define	SYSREC1_SIZE	(sizeof(struct syscall_record1))

/*
 * Record format for system calls with two pathname parameters.
 */

struct syscall_record2 {	/* RT_SYSCALL_2 */
	struct syscall_record syscall_rec;
	ptr_t	text1;		/* first path name */
	ptr_t	text2;		/* second path name */
#if SEC_AUDIT_SYMLINKS
	ptr_t	symlink1;	/* symbolic link path */
	ptr_t	symlink2;	/* symbolic link path */
#endif
};

#define	SYSREC2_SIZE	(sizeof(struct syscall_record2))

/*
 * Record format for the security system call.
 */

struct security_record {	/* RT_SECURITY */
	struct syscall_record syscall_rec;
	ushort	function;	/* security syscall function */
#if SEC_ARCH
	ushort	label_id;	/* policy label identifier */
	tag_t	old_label;	/* old label tag */
	tag_t	new_label;	/* new label tag */
#endif
	uid_t	luid;		/* login user id */
	ushort	privtype;	/* privilege type */
	ushort	objtype;	/* object type */
	uint	objid;		/* object id */
	privvec_t priv;		/* privilege vector */
#if SEC_ARCH || SEC_PRIV
	ptr_t	text1;		/* object name */
	ptr_t	text2;		/* object name (lmount) */
#if SEC_AUDIT_SYMLINKS
	ptr_t	symlink1;	/* symbolic link path */
	ptr_t	symlink2;	/* symbolic link path (lmount) */
#endif
#endif
};

#if SEC_ARCH

/*
 * Values for the label_id field
 * Changes to this table must be reflected in the label_ids[] table in
 * seccmd/reduce.h
 */

#define	AUDIT_NO_LABEL	0	/* invalid label identifier */
#define AUDIT_MAC_CLR	1	/* MAC clearance label */
#define AUDIT_MAC_PROC	2	/* MAC process label */
#define AUDIT_MAC_OBJ	3	/* MAC object label */
#define AUDIT_ACL_OBJ	4	/* ACL object label */
#define AUDIT_ILB_PROC	5	/* ILB process label */
#define AUDIT_ILB_OBJ	6	/* ILB object label */
#define AUDIT_NCAV_PROC	7	/* NCAV process caveat set */
#define AUDIT_NCAV_OBJ	8	/* NCAV object caveat set */

#endif

#define	SECREC_SIZE	(sizeof(struct security_record))

/*
 * Record format for system calls that change process identity
 */

struct id_record {		/* RT_ID_CHANGE */
	struct syscall_record syscall_rec;
	uid_t	euid;		/* effective uid */
	gid_t	egid;		/* effective group id */
	uid_t	ruid;		/* real uid */
	gid_t	rgid;		/* real gid */
#if SEC_GROUPS
	gid_t	groups[SEC_GROUPS];	/* supplementary groups */
#endif
};

#define	IDREC_SIZE	(sizeof(struct id_record))

/*
 * Record format for system calls that change an object's owner/group/mode
 */

struct dac_record {		/* RT_DAC_CHANGE */
	struct syscall_record syscall_rec;
	dac_t	old;		/* old DAC values */
	dac_t	new;		/* new DAC values */
	ptr_t	text1;		/* file pathname */
#if SEC_AUDIT_SYMLINKS
	ptr_t	symlink1;	/* symbolic link path */
#endif
};

#define	DACREC_SIZE	(sizeof(struct dac_record))

/*
 * Record format for system startup/shutdown
 */

struct boot_record {		/* RT_BOOT_DOWN */
	struct syscall_record syscall_rec;
	ino_t	kinode;		/* kernel boot inode */
	ushort	code;		/* event code */
	char	sysname[AUDIT_SYSNMSIZE];	/* system name */
	ptr_t	kpath;		/* kernel path pointer */
};

#define	BOOTREC_SIZE	(sizeof(struct boot_record))

/*
 * Record format for user login
 */

struct login_audit {		/* RT_LOGIN */
	struct audit_header aud_hdr;
	char	username[AUDIT_NAMESIZE];	/* login name */
	ushort	code;		/* function code */
	uid_t	luid;		/* login userid */
	gid_t	rgid;		/* real gid */
	dev_t	ttyd;		/* controlling terminal */
	ptr_t   problem;        /* null(for success) or reason for failure */
	ptr_t	cdir;		/* current directory */
	ptr_t	terminal;	/* stdin terminal name */
#if SEC_MAC
	ptr_t	sec_level;	/* login security level */
#endif
#if SEC_NCAV
	ptr_t	ncav;		/* login caveat set */
#endif
#if SEC_GROUPS
	ptr_t	groups;		/* supplementary groups */
#endif
};

#define	LOGINAUDIT_SIZE	(sizeof(struct login_audit))

/*
 * Record format for password changes
 */

struct passwd_audit {		/* RT_PASSWORD */
	struct audit_header aud_hdr;
	char	username[AUDIT_NAMESIZE];	/* login user name */
	ushort	code;		/* function code */
};

#define	PASSWDAUDIT_SIZE	(sizeof(struct passwd_audit))

/*
 * Record format for user/terminal locking
 */

struct lock_audit {		/* RT_LOCK */
	struct audit_header aud_hdr;
	char	username[12];	/* login username or terminal */
	ushort	code;		/* lock function code */
	ushort	trys;		/* failed attempts */
};

#define	LOCKAUDIT_SIZE	(sizeof(struct lock_audit))

/*
 * Record format for audit subsystem events
 */
struct audit_actions {		/* RT_AUDIT */
	struct audit_header aud_hdr;
	ushort	code;		/* audit function code */
	ptr_t	text1;		/* initial collection file name */
};

#define	AUDITACTIONS_SIZE	(sizeof(struct audit_actions))

/*
 * Record format for authentication database changes
 */

struct database_activity {	/* RT_DATABASE */
	/* Fixed entries */
	struct audit_header aud_hdr;
	ptr_t	command;	/* command name */
	ushort	code;		/* Type of database audit activity. */
	/* Variable entries -- their use is dependent on the code */
	ushort	object;		/* System object type or database object type */
	ptr_t	action;		/* security action that failed */
	ptr_t	result;		/* result of failure to global action tried */
};

#define	DATABASEACTIVITY_SIZE	(sizeof(struct database_activity))

/*
 * Record format for protected subsystem events
 */

struct subsystem_activity {	/* RT_SUBSYSTEM */
	struct audit_header aud_hdr;
	ptr_t	command;	/* command name (subsystems can have > 1) */
	gid_t	egid;		/* starting EGID (subsystem name) */
	ptr_t	action;		/* action that failed */
	ptr_t	result;		/* result of failure to subsys/cmd */
};

#define	SUBSYSTEMACTIVITY_SIZE	(sizeof(struct subsystem_activity))

/*
 * Record format for window system events generated by window manager
 */

struct window_system_audit {	/* RT_WINDOW_SYSTEM */
	struct audit_header aud_hdr;
	char	login_user[AUDIT_NAMESIZE];	/* login user name */
	ushort	code;		/* event sub-code */
	ulong	window;		/* window id */
	ulong	src_window;	/* source window for paste */
	ptr_t	text1;		/* text string */
	ptr_t	text2;		/* text string */
	ptr_t	text3;		/* text string */
	ptr_t	text4;		/* text string */
};

#define WINDOWSYSTEM_SIZE	(sizeof(struct window_system_audit))

/*
 * Record format for window system events generated by X server
 */

struct window_server_audit {	/* RT_WINDOW_SERVER */
	struct audit_header aud_hdr;
	uid_t	luid;		/* user identity */
	ushort	client;		/* client index */
	ushort	code;		/* event subcode */
	ushort	objtype;	/* object type */
	ulong	id1;		/* object identifier */
	ulong	id2;		/* object identifier */
	ptr_t	label1;		/* SL or IL */
	ptr_t	label2;		/* SL or IL */
	ptr_t	text;		/* text string */
};

#define	WINDOWSERVER_SIZE	(sizeof(struct window_server_audit))


/*
 * Process information record format for auditing existing processes
 * upon subsystem startup.
 */

#define AUDIT_PSARGMAX		40

struct process_info_record {	/* RT_PROCESS_INFO */
	struct audit_header aud_hdr;
	pid_t	pid;		/* process id */
	pid_t	ppid;		/* parent process id */
	uid_t	luid;		/* process luid */
	uid_t	euid;		/* effective userid */
	gid_t	egid;		/* effective group id */
	uid_t	ruid;		/* real userid */
	gid_t	rgid;		/* real group id */
	privvec_t effpriv;	/* effective privileges */
#if SEC_GROUPS
	gid_t	groups[SEC_GROUPS];	/* supplementary group list */
#endif
#if SEC_MAC
	tag_t	pslevel;	/* process sensitivity level */
#endif
#if SEC_ILB
	tag_t	pilevel;	/* process information level */
#endif
#if SEC_NCAV
	tag_t	pncav;		/* process caveat set */
#endif
	char	psargs[AUDIT_PSARGMAX];		/* command string arguments */
	ushort	fd_count;	/* open file count */
	struct open_fd {
		ushort	fd;	/* file descriptor */
		ushort	flags;	/* mode flags */
		dev_t	device; /* device major minor number */
		ino_t	inode;	/* inode number */
	}	open_fd[2];	/* open file information array */
};

/* Bit definitions for flags field of struct open_fd */

#define AUDIT_OPENFD_READ	0x1	/* fd open for read */
#define AUDIT_OPENFD_WRITE	0x2	/* fd open for write */
#define AUDIT_OPENFD_SOCKET	0x4	/* fd is for a socket */

/* Indices for the open_fd array */

#define	AUDIT_PROCINFO_RDIR	0	/* root directory-entry 0 */
#define AUDIT_PROCINFO_CDIR	1	/* current directory-entry 1 */
#define AUDIT_PROCINFO_FILES	2	/* file entries-start at entry 2 */

#define AUDITOPENFD_SIZE	(sizeof(struct open_fd))
#define PROCESSINFOREC_SIZE	(sizeof(struct process_info_record))


/*
 * Data structures used to support the ioctl(2) interface to the
 * audit device.
 */

/* Structure used with AUDIT_ENABLE command */

struct audit_init {
	uint	buf_length;	/* lngth of data including header */
	mask_t	audit_flags;	/* audit control flags */
	mask_t	event_mask[AUDIT_MASK_SIZE];	/* system event mask */
	char    root_dir[AUDIT_PATHSIZE];	/* root filesystem auditing */
	uint	read_count;	/* daemon read count to satisfy */
	long	caf_maxsize;	/* compacted audit file max size */
	uint	dir_count;	/* directory count */
	uint	uid_count;	/* uid selection count */
	uint	gid_count;	/* gid selection count */
	ulong	dir_offset;	/* fseek of directory names */
	ulong	uid_offset;	/* fseek of uids to select */
	ulong	gid_offset;	/* fseek of gids to select */
	uint	max_memory;	/* maximum bound for audit buffer size */
	ulong	session;	/* system boot session number */
#if SEC_MAC
	tag_t	slevel_min;	/* minimum sensitivity level selection */
	tag_t	slevel_max;	/* maximum sensitivity level selection */
#endif
	ushort	rt_audit[RT_APPLS][ES_MAX_CODE];	/* rt audit counts */
};

#define	AUDITINIT_SIZE	(sizeof(struct audit_init))

/* Bit definitions for the audit_flags field */

#define AUDIT_COMPACT_FLAG	0x1	/* 0=no compact, 1=compact */
#define	AUDIT_PANIC_FLAG	0x2	/* 0=shutdown, 1=panic */
#define AUDIT_REAL_TIME		0x4	/* 0=normal, 1=real time */
#if SEC_AUDIT_SYMLINKS
#define AUDIT_SYMLINK_FLAG	0x8	/* 0=no audit, 1=audit symlinks */
#endif


/* Structure used with AUDIT_SYSMASK, AUDIT_USERMASK, AUDIT_DAEMON commands */

struct audit_ioctl {
	uint	read_count;			/* daemon read count */
	mask_t	user_control[AUDIT_MASK_SIZE];	/* user control event mask */
	mask_t	user_disp[AUDIT_MASK_SIZE];	/* user disposition mask */
	mask_t	system_mask[AUDIT_MASK_SIZE];	/* system event mask */
};

/* Structure used with AUDIT_STATS command */

struct audit_stats {
	uint	session;	/* current session number */
	ulong	total_bytes;	/* total bytes written */
	ulong	total_recs;	/* total records written */
	ulong	kernel_recs;	/* total kernel written audit records */
	ulong	syscall_recs;	/* system call audit record count */
	ulong	syscall_norecs;	/* system call audit record count */
	ulong	appl_recs;	/* application audit record count */
	ulong	read_count;	/* number of device reads */
	ulong	write_count;	/* number of device writes */
	ulong	highest_offset;	/* maximum audit buffer depth */
	ulong	buffer_sleep;	/* count of audit space sleeps */
};

/*
 * The lock that protects the preceding structure, and macros to lock
 * and unlock it 
 */

#ifdef KERNEL
	decl_simple_lock_data(extern, audit_stats_lock)
#define AUDIT_STATS_LOCK()	simple_lock(&audit_stats_lock)	
#define AUDIT_STATS_UNLOCK()	simple_unlock(&audit_stats_lock)	
#endif
#if SEC_MAC

/* Structure used with AUDIT_SLEVEL command */

struct audit_slevel {
	tag_t	slevel_min;	/* minimum sensitivity level selection */
	tag_t	slevel_max;	/* minimum sensitivity level selection */
};

#endif


/*
 * Audit Log File Header Structure
 */

struct log_header {
	char	id[8];		/* "AUDITLOG" */
	char	sys[8];		/* system name */
	ulong	flag;		/* information on session */
	ulong	read_size;	/* maximum read buffer size */
	ulong	session;	/* log session number */
	ulong	comp_files;	/* number of compacted file entries */
	ulong	caf_maxsize;	/* maximum compaction file size */
	time_t	start;		/* time stamp of log file start */
	time_t	stop;		/* time stamp of log file end */
	ulong	records;	/* number of audit records */
	ulong	total;		/* amount of uncompacted data */
	ulong	compacted;	/* amount of compacted data */
	ulong	audit_messages;	/* audit application msgrcv() calls */
	ushort	proc_entries;	/* number of system proc[] table entries */
	ushort	fd_entries;	/* fd slots per process */
};

/* Values for the flag field */

#define	COMPACTED	1		/* compacted */
#define	UNCOMPACTED	2		/* not compacted */

#define	AUDIT_LOGID	"AUDITLOG"

/*
 * Selective Audit Collection/Reduction Structure
 */

struct audit_select {
	ulong	sel_size;		/* size of selectivity data */
	ushort	uid_count;	/* number of user id ushort entries */
	ushort	gid_count;	/* number of group id ushort entries */
	ushort	object_count;	/* number of variable length object names */
	mask_t	event_mask[AUDIT_MASK_SIZE];  /* mask of selected event types */
	ulong	uid_offset;	/* offset to "ushort uids[uid_count]" */
	ulong	gid_offset;	/* offset to "ushort gids[gid_count]" */
	ulong	object_offset;	/* offset to "char **objects" */
	time_t	start;		/* select after this time */
	time_t	stop;		/* select before this time */
#if SEC_MAC
	tag_t	slevel_min;	/* minimum process sen. level selection */
	tag_t	slevel_max;	/* maximum process sen. level selection */
	tag_t	olevel_min;	/* minimum object sen. level selection */
	tag_t	olevel_max;	/* maximum object sen. level selection */
#endif
};

#define	AUDITSELECT_SIZE	(sizeof(struct audit_select))

#endif /* SEC_BASE */
#endif /* _SYS_AUDIT_H_ */
