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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: security.h,v $
 * Revision 1.4  1994/11/18  20:41:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:27:04  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:36:14  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:43:31  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:08  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:40:05  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/10/15  11:30:46  roy
 * Fix revision history comments.
 *
 * Revision 2.2  91/10/14  13:08:41  sjs
 * 	Iniital revision.
 *
 * Revision 3.0  91/09/13  12:47:43  sp
 * needed for mmap
 * 
 * Revision 1.7.2.2  91/02/21  12:35:04  seiden
 * 	Put pointers to matching header files to allow for proper maintenance.
 * 	[91/02/21  11:12:14  seiden]
 * 
 * Revision 1.7  90/12/06  14:04:59  devrcs
 * 	Cleanup copyright and history log comments.
 * 	[90/11/15  11:11:41  gm]
 * 
 * Revision 1.6  90/10/07  14:53:38  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:44:22  gm]
 * 
 * Revision 1.5  90/08/24  12:27:05  devrcs
 * 	Placed <kern/lock.h> under KERNEL ifdef.
 * 	[90/08/19  17:20:10  gm]
 * 
 * 	changes related to MP support
 * 	[90/08/14  17:57:37  hosking]
 * 
 * Revision 1.4  90/07/27  09:07:40  devrcs
 * 	Changes for type-widening
 * 	[90/07/23  12:57:19  seiden]
 * 
 * Revision 1.3  90/07/17  11:42:23  devrcs
 * 	All 3 process privilege sets are now part of the base system,
 * 	     as well as the si_privs_used field of the audit_info structure.
 * 	[90/07/10  22:03:23  seiden]
 * 
 * Revision 1.2  90/06/22  20:53:48  devrcs
 * 	Added from SecureWare.
 * 
 * 	This Module contains Proprietary Information of SecureWare, Inc.
 * 	and should be treated as Confidential.
 * 
 * 	General Purpose Defines and Macros for Security Modules
 * 	[90/06/10  00:03:42  seiden]
 * 
 * $EndLog$
 */
/* @(#)security.h	6.4 07:57:07 2/18/91 SecureWare */
/*
 * Copyright (c) 1987-1990 SecureWare, Inc.  All rights reserved.
 *
 * Based on:
 *	@(#)security.h	2.8.2.3 14:28:20 12/27/89 SecureWare
 */

#ifndef __SECURITY__
#define __SECURITY__

#include <sys/secdefines.h>

#if SEC_BASE

typedef unsigned char	unchar;
#ifndef _OSF_SOURCE
typedef unsigned long	ulong;
#endif
typedef unsigned long	mask_t;		/* used for audit masks */
typedef unsigned long	priv_t;		/* used for privilege vectors */

/*
 * PORT NOTE:
 * 
 * The security software assumes definitions of the uid_t, gid_t, mode_t,
 * dev_t, and pid_t typedefs.  If the underlying system does not have
 * definitions for them, you must include definitions in this file
 * that match the corresponding definitions assumed in the base source.
 */

/*
 * Union of various kinds of object identifiers.
 */

typedef union obj {
	char	*o_file;	/* Pathname */
	int	o_pid;		/* Process ID */
	int	o_semid;	/* Semaphore ID */
	int	o_shmid;	/* Shared Memory ID */
	int	o_msgid;	/* Message Queue ID */
	int	o_fdes;		/* Open File Descriptor */
}			obj_t;

/*
 * Type representing an object's normal Unix DAC
 * attributes that can be modified.
 */

typedef struct {
	uid_t	uid;		/* Owner user ID */
	gid_t	gid;		/* Owner group ID */
	mode_t	mode;		/* Permission bits */
} dac_t;

#if SEC_ARCH

typedef unsigned long	tag_t;	/* used for policy tags */

#define SEC_TAG_COUNT	8	/* number of tags in tag pools */

/*
 * Structure used by getlabel(2) and setlabel(2) for passing internal
 * representations of labels between user and kernel space
 */

typedef struct {
	unchar	code;		/* function/return code value */
	ushort	ir_length;	/* length of internal representation */
	char	*ir;		/* internal representation pointer */
} attr_t;

/* Values for the code field of an attr_t structure */

#define SEC_WILDCARD_TAG	2	/* Wildcard Tag Value */
#define SEC_ACTUAL_TAG		3	/* Mapped Tag Value */

/*
 * Type for indicating whether an attribute (IR or tag) is associated
 * with a subject or an object.
 */

typedef short attrtype_t;	/* distinguish subject vs object attributes */

/* Values for variables of type attrtype_t */

#define SEC_SUBJECT	1	/* for subject attributes */
#define SEC_OBJECT	2	/* for object attributes */

/*
 * Type representing a subject's attributes that are
 * significant to a DAC policy.
 */

typedef struct {
	uid_t	uid;		/* User Id */
	gid_t	gid;		/* Group Id */
#if SEC_GROUPS
	gid_t	groups[SEC_GROUPS];	/* Supplementary Group Ids */
#endif
} dacid_t;

/*
 * Type representing an object's normal Unix DAC attributes
 */

typedef struct {
	uid_t	cuid;		/* Creator user ID */
	gid_t	cgid;		/* Creator group ID */
	uid_t	uid;		/* Owner user ID */
	gid_t	gid;		/* Owner group ID */
	mode_t	mode;		/* UNIX permission bits */
} udac_t;

/*
 * Value of UNIX user/group IDs that are used to specify that an object
 * does not recognize the concept of "creator".
 */

#define SEC_NOCREATOR_UID	((uid_t) -3)
#define SEC_NOCREATOR_GID	((gid_t) -3)

/*
 * Bit flags representing individual components of Unix DAC information.
 */

#define	SEC_NEW_UID	0x1
#define	SEC_NEW_GID	0x2
#define	SEC_NEW_MODE	0x4
#define	SEC_NEW_GROUPS	0x8

/*
 * Access check modes recognized by SP_ACCESS().
 */

#define	SP_IOCTLACC	01000	/* for performing ioctl functions */
#define	SP_MOUNTACC	02000	/* for check between fs root and mount dir */
#define	SP_STATACC	04000	/* for stat(2)ing a file descriptor */
#define	SP_DELETEACC	010000	/* for deleting an object */
#define	SP_CREATEACC	020000	/* for checking directory before create */
#define	SP_LINKACC	040000	/* for check between object and target dir */
#define	SP_SETATTRACC	0100000	/* for changing object attributes */
#define	SP_KILLACC	0200000	/* for kill(2)ing another process */

/*
 * The following are used by the discretionary policy module to make access
 * decisions.  There is a dependency in the aclaccess() routine on the
 * values of these #defines.  They must match the definitions of IREAD, etc.
 * in the underlying system, and the discretionary module access function
 * must change to accommodate differences in the values appearing there.
 */

#define	SP_READACC	0400	/* for checking read access */
#define	SP_WRITEACC	0200	/* for checking write access */
#define	SP_EXECACC	0100	/* for checking execute access */

#endif /* SEC_ARCH */

/* These macros are for the manipulation of the mask_t type. */

#define	BITS_PER_BYTE		8
#define	SEC_MASK_BITS		(sizeof(mask_t) * BITS_PER_BYTE)
#define	WORD_OF_BIT(bit)	((bit) / SEC_MASK_BITS)
#define	BIT_IN_WORD(bit)	(1 << ((bit) % SEC_MASK_BITS))

/*
 * Macros to add, remove, and test bits in a multi-word bit mask
 * MP Note: Must do locking at a higher level
 */

#define ADDBIT(m,b)	m[WORD_OF_BIT(b)] |= BIT_IN_WORD(b)
#define RMBIT(m,b)	m[WORD_OF_BIT(b)] &= ~BIT_IN_WORD(b)
#define ISBITSET(m,b)	((m[WORD_OF_BIT(b)] & BIT_IN_WORD(b)) != 0)


/*
 * Definitions for system privileges. 
 * Changes to this list must be reflected in the priv_names[] table in
 * seccmd/reduce.h.
 */

enum {
	SecSuspendAudit,	/* Process does its own auditing */
	SecConfigAudit,		/* Process to configure audit */
	SecWriteAudit,		/* Process can write audit records */
	SecExecSUID,		/* Process can exec SUID programs */
	SecChmodSUGID,		/* Permit setting of SUID/SGID bits */
	SecChown,		/* Permit giving away files */
	SecAcct,		/* acct() system call */
	SecLimit,		/* Resource limit override */
	SecLock,		/* Lock process parts in core */
	SecLinkDir,		/* Link directories */
	SecMknod,		/* mknod() system call */
	SecMount,		/* Mount, unmount, swap space */
	SecSysAttr,		/* Set system attributes */
	SecProcIdent,		/* Change real ID */
	SecChroot,		/* chroot() system call */
	SecDebug,		/* Kernel debugging activities */
	SecShutdown,		/* Shutdown, reboot, remount */
	SecFilesys,		/* Configure disk */
	SecRemote,		/* Perform network administration */
	SecKill,		/* Kill any process */
	SecOwner,		/* Change file attributes */
	SecChpriv,		/* Set file privileges */
	SecAllowDAC,		/* Override DAC constraints */
	SecSUCompat,		/* Enable superuser compatibility */
	SecSUPropagate,		/* Enable propagation of SecSUCompat */

#if SEC_MAC
	SecDowngrade,		/* Downgrade file attributes */
	SecWriteupClrnce,	/* Upgrade info to clearance */
	SecWriteupSyshi,	/* Upgrade info to any level */
	SecMultdir,		/* View all of multilevel dir. */
	SecAllowMAC,		/* Override MAC constraints */
#endif

#if SEC_ILB
	SecAllowILB,		/* Override ILB constraints */
	SecILNoFloat,		/* Don't float information labels */
#endif

#if SEC_NCAV
	SecAllowNCAV,		/* Override caveat constraints */
#endif

	SecMaxPriv		/* Must be last element of enum */
};

/*
 * The code uses the defined constants rather than the enumerated
 * constants since most C compilers don't seem to allow the latter
 * to be used in arithmetic expressions.
 */

#define	SEC_SUSPEND_AUDIT	(int) SecSuspendAudit
#define SEC_CONFIG_AUDIT	(int) SecConfigAudit
#define SEC_WRITE_AUDIT		(int) SecWriteAudit
#define	SEC_EXECSUID		(int) SecExecSUID
#define	SEC_CHMODSUGID		(int) SecChmodSUGID
#define	SEC_CHOWN		(int) SecChown
#define	SEC_ACCT		(int) SecAcct
#define	SEC_LIMIT		(int) SecLimit
#define	SEC_LOCK		(int) SecLock
#define	SEC_LINKDIR		(int) SecLinkDir
#define	SEC_MKNOD		(int) SecMknod
#define	SEC_MOUNT		(int) SecMount
#define	SEC_SYSATTR		(int) SecSysAttr
#define	SEC_SETPROCIDENT	(int) SecProcIdent
#define	SEC_CHROOT		(int) SecChroot
#define	SEC_DEBUG		(int) SecDebug
#define	SEC_SHUTDOWN		(int) SecShutdown
#define	SEC_FILESYS		(int) SecFilesys
#define	SEC_REMOTE		(int) SecRemote
#define	SEC_KILL		(int) SecKill
#define	SEC_OWNER		(int) SecOwner
#define	SEC_CHPRIV		(int) SecChpriv
#define	SEC_ALLOWDACACCESS	(int) SecAllowDAC
#define	SEC_SUCOMPAT		(int) SecSUCompat
#define	SEC_SUPROPAGATE		(int) SecSUPropagate

#if SEC_MAC
#define	SEC_DOWNGRADE		(int) SecDowngrade
#define	SEC_WRITEUPCLEARANCE	(int) SecWriteupClrnce
#define	SEC_WRITEUPSYSHI	(int) SecWriteupSyshi
#define	SEC_MULTILEVELDIR	(int) SecMultdir
#define	SEC_ALLOWMACACCESS	(int) SecAllowMAC
#endif /* SEC_MAC */

#if SEC_ILB
#define	SEC_ALLOWILBACCESS	(int) SecAllowILB
#define	SEC_ILNOFLOAT		(int) SecILNoFloat
#endif /* SEC_ILB */

#if SEC_NCAV
#define	SEC_ALLOWNCAVACCESS	(int) SecAllowNCAV
#endif /* SEC_NCAV */

#define	SEC_MAX_SPRIV		((int) SecMaxPriv - 1)

/* Number of priv_t words required to hold a privilege vector */

#define	SEC_SPRIVVEC_SIZE	(WORD_OF_BIT(SEC_MAX_SPRIV) + 1)

typedef priv_t	privvec_t[SEC_SPRIVVEC_SIZE];

/*
 * Audit Subsystem event type definitions
 * Changes to this table must be reflected in the event_msg[] table in
 * seccmd/reduce.h
 */

#define	ET_NOEVENT		0	/* not an audited event */
#define	ET_BOOT_DOWN		1	/* startup/shutdown */
#define	ET_LOGIN		2	/* login events */
#define	ET_PROCESS		3	/* process create/delete */
#define	ET_OBJECT_AVAIL		4	/* make available */
#define	ET_OBJECT_MAP		5	/* map to subject */
#define	ET_OBJECT_MOD		6	/* modify object */
#define	ET_OBJECT_UNAV		7	/* object unavailable */
#define	ET_OBJECT_CREAT		8	/* create object */
#define	ET_OBJECT_DEL		9	/* delete object */
#define	ET_DAC_CHANGE		10	/* change modes */
#define	ET_ACCESS_DENIAL	11	/* access denied */
#define	ET_SYS_ADMIN		12	/* system admin actions */
#define	ET_INSUFF_PRIV		13	/* insufficient privilege */
#define	ET_RES_DENIAL		14	/* resource denial */
#define	ET_IPC			15	/* inter-process comm */
#define	ET_PROCESS_MOD		16	/* change process control fields */
#define	ET_AUDIT		17	/* audit subsystem events */
#define	ET_SUBSYSTEM		18	/* special subsystem events */
#define ET_PRIVILEGE		19	/* use of privilege event */
#define ET_AUTHORIZATION	20	/* use of authorization event */
#define ET_SEC_LEVEL		21	/* set security level events */
#define ET_WINDOW_SYSTEM	22	/* window system events */

/*
 * Define the maximum number of audit event types and the maximum number
 * of mask_t array entries required to accommodate.
 */

#define AUDIT_MAX_EVENT		22	/* largest event number in CMW */
#define	AUDIT_MASK_SIZE		1	/* only 1 long required in mask_t */

/*  some default file mode settings */

#define	SEC_DEFAULT_MODE	0700
#define	SEC_CORE_MODE		0600


/*
 * System calls that share the security() interface to the kernel.
 * If these are reordered, you must also reorder the function table
 * in sec_calls.c.
 * Changes to these definitions must be reflected in the security_msg[]
 * table in seccmd/reduce.h.
 */

#define	SEC_STOPIO		1
#define	SEC_GETLUID		2
#define	SEC_SETLUID		3
#define	SEC_STATPRIV		4
#define	SEC_CHPRIVSYS		5

#if SEC_ARCH

#define	SEC_EACCESS		6

#if SEC_MAC
#define	SEC_MKMULTDIR		7
#define	SEC_RMMULTDIR		8
#define	SEC_ISMULTDIR		9
#endif

#define	SEC_GETLABEL		10
#define	SEC_SETLABEL		11
#define	SEC_LMOUNT		12
#define SEC_ISLABELEDFS		13

#if SEC_NCAV
#define SEC_MK2PERSON		14
#define SEC_RM2PERSON		15
#define SEC_IS2PERSON		16
#endif

#endif /* SEC_ARCH */

#if SEC_WIS
#define	SEC_SETSESSION		17
#define	SEC_KILLSESSION		18
#endif


/*
 *  The different kinds of privilege vectors.
 *  Changes to these definitions must be reflected in the priv_msg[] table
 *  in seccmd/reduce.h
 */

#define	SEC_EFFECTIVE_PRIV	1
#define	SEC_KERNEL_AUTHS	2
#define	SEC_MAXIMUM_PRIV	SEC_KERNEL_AUTHS	/* historical */
#define	SEC_BASE_PRIV		3
#define	SEC_POTENTIAL_PRIV	4
#define	SEC_GRANTED_PRIV	5

#if SEC_FSCHANGE

/*  Bits in the type flag word in the secure disk inode. */

#define	SEC_I_MLD		0x1
#define	SEC_I_MLDCHILD		0x2
#if SEC_CHOTS
#define SEC_I_2PERSON		0x4
#endif

#endif /* SEC_FSCHANGE */

#if SEC_MAC

/*  Codes for handling of multilevel directory path components. */

#define	SEC_MLD_PASS		0
#define	SEC_MLD_DIVERT		1
#define	SEC_MLD_ERROR		2
#define	SEC_MLD_DOTDOT		3

#endif /* SEC_MAC */

/*  Minimum PID number not created by the system at init time */

#define	SEC_MINPID		3	/* PID 2 is vhand */

#if SEC_ARCH

/* Definitions of types of attributes that can be passed through sockets
 * using the "access rights" mechanism.  These values are chosen to be
 * large negative numbers to facilitate distinguishing an expanded rights
 * buffer from an old-style array of file descriptors.  An expanded rights
 * buffer is a series of entries, each beginning with a 16 bit type flag
 * followed by a 16 bit length followed by the actual attribute consisting
 * of the specified number of bytes.
 */

#define	SEC_RIGHTS_FDS		((ushort) 0xACC0)
#define	SEC_RIGHTS_PRIVS	((ushort) 0xACC1)
#if SEC_ILB
#define	SEC_RIGHTS_ILB		((ushort) 0xACC2)
#endif
#define	SEC_EXTENDED_RIGHTS	((ushort) 0xACC3)

#endif /* SEC_ARCH */




#ifdef _KERNEL

/*
 * The following macros protect various SIP attributes.  This is a
 * system-wide lock for now, since various system calls access
 * attributes for other processes.  Deadlocks might happen with
 * finer granularity locking, and the locks are never held for
 * very long.  Hopefully the collision rate won't be high enough
 * to worry about.
 * If you add locks here, don't forget to change sec_init() to
 * call simple_lock_init() on them.
 */

#ifdef	_OSF_SOURCE
#ifndef _KERN_LOCK_H_
#include <kern/lock.h>
#endif

decl_simple_lock_data (extern, sip_attr_lock)
decl_simple_lock_data (extern, sip_audit_lock)
decl_simple_lock_data (extern, sip_audit_flag_lock)

#define SIP_ATTR_LOCK()		simple_lock(&sip_attr_lock);
#define SIP_ATTR_UNLOCK()	simple_unlock(&sip_attr_lock);

#define SIP_AUDIT_LOCK()	simple_lock(&sip_audit_lock);
#define SIP_AUDIT_UNLOCK()	simple_unlock(&sip_audit_lock);

#define SIP_AUDIT_FLAG_LOCK()	simple_lock(&sip_audit_flag_lock);
#define SIP_AUDIT_FLAG_UNLOCK()	simple_unlock(&sip_audit_flag_lock);


#else	/* _OSF_SOURCE */
#define SIP_ATTR_LOCK()
#define SIP_ATTR_UNLOCK()
#define SIP_AUDIT_LOCK()
#define SIP_AUDIT_UNLOCK()
#define SIP_AUDIT_FLAG_LOCK()
#define SIP_AUDIT_FLAG_UNLOCK()
#endif	/* _OSF_SOURCE */

#endif	/* _KERNEL */

/*
 * Security relevant process state information.
 * This structure contains state information that spans system calls.
 * The security_info structure contains security-related fields that
 * might normally be in the user structure.  However, putting them there
 * would require various utilities (ps, etc.) that read from kernel
 * memory to be different depending on whether security is/isn't configured
 * into the kernel, and what options (SEC_ILB, etc.) were used to build the
 * kernel.
 *
 * SIP is always a pointer to the security_info structure for the currently
 * running process. The security_info structures for other processes may be
 * accessed through the secinfo array.  Unfortunately, this is done more often
 * than we might like in the MP world.  For this reason, MP locks are
 * organized such that each lock protects a field (or fields) of *all*
 * security_info structures, rather than having a per-structure lock.
 *
 * Unless otherwise noted, locks are system-wide spin locks.  No other locks
 * may be taken after taking one of these locks. None of these locks is
 * intended to be held for a long time, so lock contention hopefully won't
 * make system-wide locking a problem.
 * 
 * In general, fields on each line of the table below must be protected by
 * the same lock, while fields on different lines could be protected by
 * different locks if lock contention is ever a problem.
 *
 *	lock name		protects
 *
 *	SIP_ATTR_LOCK		si_luid, si_luid_set
 *	SIP_ATTR_LOCK		si_bpriv, si_epriv, si_mpriv, si_spriv
 * 	SIP_ATTR_LOCK		si_diversion, si_pslevel
 *	SIP_AUDIT_LOCK		si_emask, si_emask_cont, si_emask_disp
 *	SIP_AUDIT_FLAG_LOCK	si_audit_flags
 *	SIP_ILB_LOCK		si_pilevel, si_subjnum, si_subj_bits
 *	SIP_NCAV_LOCK		si_pncav
 *	SIP_WIS_LOCK		si_sid
 *	SIP_TAG_LOCK		si_tag
 *
 * As of 7/25/90, locking of these is believed complete except for
 * SIP_NCAV_LOCK (not supported in OSF/1) and SIP_TAG_LOCK.
 * ILB support has known races between ilb_check_float() and ilb_do_float().
 */

struct security_info {
	/*
	 * These fields are part of the base secure system, providing
	 * support for login UID, process privileges, and process
	 * specific selection of event types to be audited.
	 */

	/*
	 * The following fields are for login UID tracking.  The login ID
	 * for a process may not change once set.  MP Note:  si_luid must
	 * be set before si_luid_set, so that most code paths don't need
	 * to do locking.
	 */

	char		si_luid_set;	/* 1 = si_luid set, 0 = not set */
	uid_t		si_luid;	/* login user id (LUID) */

#if SEC_WIS

	/*
	 * si_sid contains the login session id, used 
	 * Because the si_sid field is so rarely accessed, there is only one
	 * system-wide lock for it.  See killsession() and setsession().
	 */

	pid_t		si_sid;		/* session id */
#endif

	ushort	si_audit_flags;			/* audit subsystem flags */

	/*
	 * The following fields are related to audit event masks.
	 * These are set in several places, but primarily used by
	 * audit_check().
	 */

	mask_t	si_emask[AUDIT_MASK_SIZE];	/* effective audit event mask*/
	mask_t	si_emask_cont[AUDIT_MASK_SIZE];	/* audit event control mask */
	mask_t	si_emask_disp[AUDIT_MASK_SIZE];	/* audit disposition mask */

	privvec_t	si_epriv;	/* effective privileges */
	/*
	 * These fields provide storage for the additional privilege sets
	 * associated with processes for the sake of checking discrete
	 * privileges in lieu of superuser checks.
	 */
	privvec_t	si_mpriv;	/* kernel authorizations */
	privvec_t	si_bpriv;	/* base privileges */
	privvec_t	si_spriv;	/* potential privs of current image */

#if SEC_ARCH
	/*
	 * These fields support the security policy architecture.
	 */
	tag_t	si_tag[SEC_TAG_COUNT];	/* process security tags */

#if SEC_MAC
#define	SI_DIVERSION_SIZE 15	/* Multilevel directory diversion dir name */
	tag_t		si_pslevel;	/* copy of SL tag for auditing */
	char	si_diversion[SI_DIVERSION_SIZE];/* diversion name for MLD */
#endif	/* SEC_MAC */

#if SEC_ILB
	/*
	 * MP Note:
	 * Be extra careful about holding SIP_ILB_LOCK before reading
	 * si_subjnum.  There is a brief window in secinfo_dup() where
	 * si_subjnum is wrong.  Without the lock, you may hit it.
	 */

	tag_t		si_pilevel;	/* copy of IL tag for auditing */
	int		si_subjnum;	/* subject number */
	mask_t		*si_subj_bits;	/* bitmap of objects for ILB maint */
#endif	/* SEC_ILB */

#if SEC_NCAV
	tag_t		si_pncav;	/* copy of NCAV tag for auditing */
#endif	/* SEC_NCAV */

#endif	/* SEC_ARCH */
};


/*
 * System call audit information.
 * This structure provides storage for information gathered during system
 * call execution for the audit subsystem.  Nothing in this structure
 * need be maintained between system calls.
 */

struct audit_info {
	uchar		si_event_type;	/* audit event class */
	uchar		si_record_type;	/* audit record type */
	ushort		si_error;	/* security syscall error values */
	ushort		si_audit_flags;	/* audit subsystem flags */
	short		si_syscall;	/* syscall number */
	short		si_objtype;	/* object type code */
	uint		si_objid;	/* object id */
	dac_t		si_audit_old;	/* old DAC values */
	dac_t		si_audit_new;	/* new DAC values */
	char		*si_path[2];	/* pathname buffer pointers */
	int		si_pathlen[2];	/* pathname buffer lengths */
	short		si_privilege;	/* priv whose absence caused denial */
	short		si_privtype;	/* vector affected by chpriv syscall */
	privvec_t	si_new_privs;	/* new vector from chpriv syscall */

#if SEC_AUDIT_SYMLINKS
	/*
	 * These fields record the actual paths (and their lengths) traversed
	 * by pathnames containing symbolic links.  There are two of each to
	 * accommodate system calls that take two pathnames.
	 */
	char	*si_symlink[2];		/* symbolic link buffer pointer */
	int	si_symlink_size[2];	/* size of symlink buffer pathname */
#endif /* SEC_AUDIT_SYMLINKS */

	privvec_t	si_privs_used;	/* privileges used by syscall */

#if SEC_ARCH
	ushort		si_label;	/* label identifier for setlabel(2) */
	tag_t		si_cur_label;	/* old label for setlabel(2) */
	tag_t		si_new_label;	/* new label for setlabel(2) */
	ushort		si_access;	/* policy that denied access */
	ushort		si_access_mode;	/* mode of attempted access */

	/*
	 * The following fields support auditing of policy-specific security
	 * attributes associated with the objects affected by a system call.
	 * In addition to the process tag field for each policy, there are two
	 * object tag fields per policy for syscalls that take two pathnames.
	 */

#if SEC_MAC
	tag_t		si_oslevel;	/* first object sensitivity label */
	tag_t		si_oslevel2;	/* second object sensitivity label */
#endif /* SEC_MAC */

#if SEC_ILB
	tag_t		si_oilevel;	/* first object information label */
	tag_t		si_oilevel2;	/* second object information label */
	tag_t		si_float;	/* result of infomation label float */
#endif /* SEC_ILB */

#if SEC_NCAV
	tag_t		si_oncav;	/* first object caveat set */
	tag_t		si_oncav2;	/* second object caveat set */
#endif /* SEC_NCAV */
#endif /* SEC_ARCH */
};

/*
 * The si_error field further discriminates the reason for a security failure.
 * On return from a system call, the bits defined by SEC_ERRNO_MASK are
 * filled with the contents of the si_error field.  The value of
 * SEC_ERRNO_SHIFT tells how to shift si_error into the return value.
 * When adding a new error here be sure to correspondingly update sec_errlst.c
 * and libsec.msg in libsecurity.
 */

#define SEC_ERRNO_MASK		 0xff00	/* mask for security error */
#define SEC_ERRNO_SHIFT		 8	/* how far to shift si_error */

#define ESEC_MAC_CONFIG_FAILURE  1	/* MAC configuration file error */
#define ESEC_WILDCARD_TAG	 2	/* operation fails on wildcard tag */
#define ESEC_ACL_CONFIG_FAILURE	 3	/* ACL configuration file error */
#define ESEC_WILD_SUBJ_SL	 4	/* subject has wildcard SL */
#define ESEC_WILD_DIR_SL	 5	/* directory has wildcard SL */
#define ESEC_MAC_NOT_EQUAL	 6	/* subject and object SLs not equal */
#define ESEC_MAC_NEED_PRIVILEGE	 7	/* no privilege for operation */
#define ESEC_NO_DECISION	 8	/* no decision available from daemon */
#define ESEC_MAC_SDOM		 9	/* Subject does not dominate */
#define ESEC_MAC_ODOM		10	/* Object does not dominate */
#define ESEC_MAC_NO_CLEARANCE	11	/* Setting SL and clearance not set */
#define ESEC_MAC_DOM_CLEARANCE	12	/* Setting SL higher than clearance */
#define ESEC_MAC_NO_SL		13	/* Can't change CL/IL if SL not set */
#define ESEC_MAC_UP_CLEARANCE	14	/* Can't raise clearance */
#define ESEC_MAC_IL_RELATION	15	/* Violation of SL/IL relationship */
#define ESEC_MAC_FS_RELATION	16	/* Violation of increasing tree */
#define ESEC_MAC_NO_WRITE	17	/* Need write access for operation */
#define ESEC_NOT_OWNER		18	/* Must be owner of object */
#define ESEC_IS_MLD		19	/* Operation invalid on multilvl dir */
#define ESEC_DIR_NOT_EMPTY	20	/* Operation invalid (non-empty dir) */
#define ESEC_MAC_IL_NOCOMB	21	/* Cannot combine information labels */
#define ESEC_UNEXT_FS		22	/* Not extended filesystem format */
#define ESEC_NOT_REG		23	/* File is not regular file */
#define ESEC_SETATTR_FAIL	24	/* Cannot set file attributes */
#define ESEC_INVALID_IR		25	/* Cannot map internal rep to tag */
#define ESEC_NCAV_CONFIG_FAILURE 26	/* NCAV configuration file error */
#define	ESEC_NOT_MLD		27	/* Directory is not multilevel */
#define	ESEC_EXT_FS		28	/* Operation invalid on extended fs */
#define ESEC_MAX_ERRNO		28

#ifdef KERNEL

extern struct security_info *secinfo;

#ifdef _OSF_SOURCE

/*
 * Declare pointers to the security_info and audit_info structures
 * for the currently running entity.  These are automatically
 * switched at context-switch time.  SIP is a per-process pointer,
 * while AIP is maintained on a per-thread basis.
 */

#define	SIP	(&secinfo[u.u_procp - proc])
#define	AIP	((struct audit_info *) &u.uthread[1])
#else /* !_OSF_SOURCE */
extern struct security_info *SIP;
extern struct audit_info *audinfo, *AIP;
#endif /* !_OSF_SOURCE */

/*
 * OSF code uses the "BM" (bogus memory) macro to attempt to provide support
 * for machines on which word operations aren't guaranteed atomic.
 * This adds needless clutter to the code, and increases the risk of
 * bugs related to lock/unlock mismatches.  For now, we'll try to do
 * the equivalent thing in a simpler way. The macros here are for
 * "normal" systems.  BM equivalents of the macros are in sec/sec_subrs.c.
 */

#ifdef _OSF_SOURCE
#include <bogus_memory.h>
#endif

#ifndef BOGUS_MEMORY
#define	vnode_type(vp)		((vp)->v_type)
#define	fp_flags(fp)		((fp)->f_flag)
#define	mp_flags(mp)		((mp)->m_flag)
#define process_flags(p)	((p)->p_flag)
#endif /* BOGUS_MEMORY */

/*
 * Define some macros to check file types.
 * The basic idea here is that we can hide some of the differences
 * between vnode-based file systems and inode-based file systems
 * in macros instead of littering the code with conditionals.
 */
 
#ifdef _OSF_SOURCE
#define IS_REG_FILE(vp)		(vnode_type((vp)) == VREG)
#define IS_DIRECTORY(vp)	(vnode_type((vp)) == VDIR)
#define IS_BLOCK_DEV(vp)	(vnode_type((vp)) == VBLK)
#define IS_CHAR_DEV(vp)		(vnode_type((vp)) == VCHR)
#define IS_SYMLINK(vp)		(vnode_type((vp)) == VLNK)
#define IS_SOCKET(vp)		(vnode_type((vp)) == VSOCK)
#define IS_FIFO(vp)		(vnode_type((vp)) == VFIFO)
#endif /* _OSF_SOURCE */

#endif /* KERNEL */

#endif /* SEC_BASE */
#endif /* __SECURITY__ */
