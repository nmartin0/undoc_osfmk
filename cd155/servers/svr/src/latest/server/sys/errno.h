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
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: errno.h,v $
 * Revision 1.18  1995/01/20  18:27:20  joel
 *  Reviewer:      sean
 *  Risk:          low
 *  Benefit or PTS #: 11968 sender exits before recv_continue
 *  Testing:       testcase, message EATs
 *  Module(s): msgp_inq.c msgp_nxdat.c msgp_atdt.c mcmsg_nx.h nxlib.c
 *             libc/errlst.c  server/sys/errno.h
 *
 * Revision 1.17  1994/11/18  20:40:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.16  1994/06/01  20:32:45  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): server/sys/errno.h, server/sys/version.h server/nx/nx.c
 * 	    server/nx/nx.defs server/nx/nx_svr.defs server/nx/nx_types.defs,
 * 	    server/nx/nx_types.h, server/nx/nx_create_attr.c (new)
 * 	    emulator/i860/emul_machdep.c
 *  Related: libnx, allocator, bootmesh, mkpart, showpart, lspart
 *
 * Revision 1.15  1994/04/11  19:27:22  shala
 *  Risk: LOW
 *  Benefit or PTS #: Bug # 8703
 *  Testing: Used new errno.h to build and test applications.
 *  Module(s): svr/server/sys/errno.h
 *
 * Cleanup: needed to use ptype instead of pid in errno.h comments for NX errors.
 *
 * Revision 1.13.2.1  1993/12/15  20:01:25  shala
 *  Reviewer: Stan Smith
 *  Risk: Low
 *  Benefit or PTS #: Bug #7447
 *  Testing:
 * 	Build libpthread.a and libc_r.a with modified include files,
 * 	and tested a pthread application with the new libraries and
 *  	include files.
 *  Module(s):
 * 	sys/errno.h          in the server which pulled into cmds/libs
 *         usr/include/stdio.h  in the cmds/libs
 *
 * Added _THREAD_SAFE flag for finding the correct errno for pthread applications.
 *
 * Revision 1.13  1993/11/30  23:33:55  carbajal
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Added Error codes for R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.12  1993/10/04  18:45:11  cfj
 * Added EINCOMPAT error code.
 *
 * Revision 1.11  1993/09/21  21:15:32  carbajal
 * Added EAOVLP for PTS #6493
 *
 * Revision 1.10  1993/08/04  16:45:27  wunder
 * Added EBADID NX errno for bad async request ID
 *
 * Revision 1.9  1993/07/18  19:33:09  carbajal
 * Added EANXACCT
 *
 * Revision 1.8  1993/07/14  18:24:24  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:33:12  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/06  19:18:48  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.6  1993/04/03  03:07:30  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 * Revision 1.1.2.1.2.7  1993/02/16  20:04:55  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.1.2.1.2.6  1993/02/12  22:38:32  brad
 * Added support for disallowing simultaneous access to a PFS file
 * (VIO_PFS mode) and one of its stripefiles (VIO_STRIPED mode).
 *
 * Revision 1.1.2.1.2.5  1993/02/09  21:55:15  brad
 * Added ENOSTRIPEDIR.
 *
 * Revision 1.5  1993/01/20  18:18:25  andrews
 * Added EINTERNAL for miscellaneous internal errors in any routine.  Part of
 * fix for bug #3983.
 *
 * Revision 1.4  1993/01/18  20:23:48  cfj
 * Add definition for EANOEXIST.
 *
 * Revision 1.3.2.1  1993/01/18  20:22:32  cfj
 * Add definition for EANOEXIST.
 *
 * Revision 1.1.2.1.2.4  1993/01/09  00:04:54  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.3  1992/12/18  19:12:21  carbajal
 * Added EPBXRS
 *
 * Revision 1.1.2.1.2.3  1992/12/14  23:20:54  brad
 * Merged tip of old NX branch with PFS branch.
 * 
 * Revision 1.1.2.1.2.2  1992/12/12  01:49:18  brad
 * Added ENOTPFS error.
 * 
 * Revision 1.2  1992/11/30  22:41:45  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:13:23  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.2  1992/11/17  19:16:37  carbajal
 * Added EPLOCK for partition management
 *
 * Revision 1.1.2.1  1992/11/05  22:39:24  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:08  cfj
 * Bump major revision number.
 *
 * Revision 2.6  1992/10/23  01:59:33  cfj
 * T6 merge.
 *
 * Revision 2.5  1992/10/22  15:35:03  dbm
 * Added changes for PFS.
 *
 * Revision 2.4  1992/10/14  17:18:24  cfj
 * NX integration.
 *
 * Revision 2.4  1993/01/06  10:37:54  loverso
 * 	Fix notice.
 *
 * Revision 2.3  92/01/05  20:06:18  roy
 * 	1991/11/12  17:22:42  noemi
 * 	Added EREMOTEPORT, EBADPORT, and ELOCAL error codes.
 * 
 * Revision 2.2  91/08/31  14:04:01  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/07/31  15:42:27  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.16  90/10/31  14:06:25  devrcs
 * 	Move perror() declarations out of ANSI C.  THe official declarations
 * 	are in stdio.h, so they really should not be here at all.
 * 	[90/10/05  07:30:05  rabin]
 * 
 * Revision 1.15  90/10/07  14:49:59  devrcs
 * 	Protect sys_errlist and sys_nerr with ifndef _KERNEL
 * 	[90/10/04  18:34:20  brezak]
 * 
 * 	took sys_errlist and sys_nerr out of _ANSI_C_SOURCE
 * 	[90/10/04  16:50:55  rabin]
 * 
 * 	Fix STREAMS errors and comments, wrap others in parens.
 * 	[90/09/30  20:53:07  tmt]
 * 
 * 	Merged in ERELOCATED non-error code.
 * 	[90/09/30  12:56:10  jeffc]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:38:30  gm]
 * 
 * 	Added EDIRTY to tell user/fsck about clean filesystems.
 * 	[90/08/18  03:49:29  nags]
 * 
 * Revision 1.14  90/09/23  15:59:42  devrcs
 * 	Add loader-related errno's.
 * 	[90/09/08  17:45:44  lwa]
 * 
 * Revision 1.13  90/09/13  11:51:18  devrcs
 * 	Make EDEADLK = 11 and EAGAIN same as EWOULDBLOCK.
 * 	[90/08/30  10:17:28  brezak]
 * 
 * Revision 1.12  90/08/24  12:25:52  devrcs
 * 	Changes for new system call interface:
 * 	[90/08/17  17:48:07  gmf]
 * 
 * 	Add ERESTART and EJUSTRETURN.
 * 	Removed CMU RFS errnos.
 * 
 * Revision 1.11  90/08/09  13:28:37  devrcs
 * 	Define per thread errno mechanism
 * 	[90/08/02  14:22:28  sp]
 * 
 * 	Defined ESOFT and EMEDIA for LVM & disk drivers.
 * 	[90/07/19  15:38:43  jeffc]
 * 
 * Revision 1.10  90/07/27  09:07:08  devrcs
 * 	Clone driver support.
 * 	[90/07/20  17:06:00  nags]
 * 
 * 	Replace _POSIX_THREADS conditional with _REENTRANT for
 * 	multithreaded errno.
 * 	[90/06/29  08:57:22  sp]
 * 
 * Revision 1.8  90/06/22  20:52:47  devrcs
 * 	Added more STREAMS error numbers.
 * 	[90/06/11  13:57:29  nolting]
 * 
 * Revision 1.7  90/05/24  23:06:46  devrcs
 * 	Added STREAMS error numbers.
 * 	[90/05/18  13:10:19  nolting]
 * 
 * Revision 1.6  90/04/27  19:25:43  devrcs
 * 	Add place holder definition for pthread definition of errno. (from sp)
 * 	[90/04/20  13:04:41  tom]
 * 
 * 	Removed extra AIX errnos.
 * 	[90/04/18  11:46:16  tom]
 * 
 * Revision 1.5  90/03/13  20:49:07  mbrown
 * 	Removed duplicate definitions.
 * 	[90/02/27  15:20:44  tom]
 * 
 * 	Removed AIX definitions for ENOLCK and ENOSYS.
 * 	[90/02/23  19:02:42  tom]
 * 
 * 	Added numbers for Sys V IPC
 * 	[90/02/15  11:34:56  mbrown]
 * 
 * 	AIX merge first cut.
 * 	[90/02/09  18:02:27  tom]
 * 
 * Revision 1.4  90/02/23  00:32:31  devrcs
 * 	Merged following revisions for submission:
 * 
 * 	Added ENOLCK and ENOSYS errors for POSIX record/file locking.
 * 	[90/02/18  13:57:10  ers]
 * 
 * 	Added ENOMSG and EIDRM errors for Sys V IPC.
 * 	[90/02/15  19:31:13  bet]
 * 
 * Revision 1.3  90/01/02  20:15:46  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:33:55  gm
 * 	Added NFS errors under BSD44 conditional.
 * 	[89/12/25            ers]
 * 
 * $EndLog$
 */
/* @(#)errno.h	1.16  com/inc/sys,3.1,8943 10/23/89 18:36:36 */
/*
 * COMPONENT_NAME: errno.h
 *                                                                    
 * ORIGIN: 27
 *
 * Copyright International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */                                                                   

#ifndef _SYS_ERRNO_H_
#define _SYS_ERRNO_H_
#include <standards.h>

/*
 *
 *      The ANSI, POSIX, and XOPEN standards require that certain values be
 *	in errno.h.  The standards allow additional macro definitions,
 *      beginning with an E and an uppercase letter.
 *
 *      This header includes all the ANSI, POSIX, and XOPEN required entries.  
 *
 */
 
#ifdef _ANSI_C_SOURCE
/*
 * Error codes
 */

#ifndef	_KERNEL

#if defined(_REENTRANT) || defined(_THREAD_SAFE)
/*
 * Per thread errno is provided by the threads provider. Both the extern int
 * and the per thread value must be maintained buy the threads libarary.
 */

extern int *_errno();

#define errno	(*_errno())

#else	/* _REENTRANT */

extern int errno;

#endif	/* _REENTRANT */

#endif  /* !_KERNEL */


#define ESUCCESS        0               /* Successful */
#define EPERM		1		/* Not owner */
#define ENOENT		2		/* No such file or directory */
#define ESRCH		3		/* No such process */
#define EINTR		4		/* Interrupted system call */
#define EIO		5		/* I/O error */
#define ENXIO		6		/* No such device or address */
#define E2BIG		7		/* Arg list too long */
#define ENOEXEC		8		/* Exec format error */
#define EBADF		9		/* Bad file number */
#define ECHILD		10		/* No children */
#define EDEADLK		11		/* Operation would cause deadlock */
#define ENOMEM		12		/* Not enough core */
#define EACCES		13		/* Permission denied */
#define EFAULT		14		/* Bad address */
#define ENOTBLK		15		/* Block device required */
#define EBUSY		16		/* Mount device busy */
#define EEXIST		17		/* File exists */
#define EXDEV		18		/* Cross-device link */
#define ENODEV		19		/* No such device */
#define ENOTDIR		20		/* Not a directory*/
#define EISDIR		21		/* Is a directory */
#define EINVAL		22		/* Invalid argument */
#define ENFILE		23		/* File table overflow */
#define EMFILE		24		/* Too many open files */
#define ENOTTY		25		/* Not a typewriter */
#define ETXTBSY		26		/* Text file busy */
#define EFBIG		27		/* File too large */
#define ENOSPC		28		/* No space left on device */
#define ESPIPE		29		/* Illegal seek */
#define EROFS		30		/* Read-only file system */
#define EMLINK		31		/* Too many links */
#define EPIPE		32		/* Broken pipe */

/* math software */
#define EDOM		33		/* Argument too large */
#define ERANGE		34		/* Result too large */
			/* STREAMS: packet size of of configured range */

/* non-blocking and interrupt i/o */
#define EWOULDBLOCK	35		/* Operation would block */
#define EAGAIN		EWOULDBLOCK	/* ditto */
#define EINPROGRESS	36		/* Operation now in progress */
#define EALREADY	37		/* Operation already in progress */

/* ipc/network software */

	/* argument errors */
#define ENOTSOCK	38		/* Socket operation on non-socket */
#define EDESTADDRREQ	39		/* Destination address required */
#define EMSGSIZE	40		/* Message too long */
#define EPROTOTYPE	41		/* Protocol wrong type for socket */
#define ENOPROTOOPT	42		/* Protocol not available */
#define EPROTONOSUPPORT	43		/* Protocol not supported */
#define ESOCKTNOSUPPORT	44		/* Socket type not supported */
#define EOPNOTSUPP	45		/* Operation not supported on socket */
#define EPFNOSUPPORT	46		/* Protocol family not supported */
#define EAFNOSUPPORT	47		/* Address family not supported by protocol family */
#define EADDRINUSE	48		/* Address already in use */
#define EADDRNOTAVAIL	49		/* Can't assign requested address */

	/* operational errors */
#define ENETDOWN	50		/* Network is down */
#define ENETUNREACH	51		/* Network is unreachable */
#define ENETRESET	52		/* Network dropped connection on reset */
#define ECONNABORTED	53		/* Software caused connection abort */
#define ECONNRESET	54		/* Connection reset by peer */
#define ENOBUFS		55		/* No buffer space available */
#define EISCONN		56		/* Socket is already connected */
#define ENOTCONN	57		/* Socket is not connected */
#define ESHUTDOWN	58		/* Can't send after socket shutdown */
#define ETOOMANYREFS	59		/* Too many references: can't splice */
#define ETIMEDOUT	60		/* Connection timed out */
#define ECONNREFUSED	61		/* Connection refused */

	/* */
#define ELOOP		62		/* Too many levels of symbolic links */
#define ENAMETOOLONG	63		/* File name too long */

/* should be rearranged */
#define EHOSTDOWN	64		/* Host is down */
#define EHOSTUNREACH	65		/* No route to host */
#define ENOTEMPTY	66		/* Directory not empty */

/* quotas & mush */
#define EPROCLIM	67		/* Too many processes */
#define EUSERS		68		/* Too many users */
#define EDQUOT		69		/* Disc quota exceeded */

#define ENOLCK		77		/* No locks available */
#define ENOSYS		78		/* Function not implemented */

/*
 *  NFS errors.
 */
#define ESTALE		70		/* Stale NFS file handle */

#ifndef	OSF1_ADFS
#define EREMOTE		71		/* Too many levels of remote in path */
#endif
#define EBADRPC		72		/* RPC struct is bad */
#define ERPCMISMATCH	73		/* RPC version wrong */
#define EPROGUNAVAIL	74		/* RPC prog. not avail */
#define EPROGMISMATCH	75		/* Program version wrong */
#define EPROCUNAVAIL	76		/* Bad procedure for program */

/* Sys V IPC errors */
#define ENOMSG		80		/* No msg matches receive request */
#define EIDRM		81		/* Msg queue id has been removed */

/* STREAMS */

#define	ENOSR		82		/* Out of STREAMS resources */
#define	ETIME		83		/* System call timed out */
#define	EBADMSG		84		/* Next message has wrong type */
#define EPROTO		85		/* STREAMS protocol error */
#define ENODATA		86		/* No message on stream head read q */
#define ENOSTR		87		/* fd not associated with a stream */
/* Not visible outside kernel */
#define ECLONEME	88		/* Tells open to clone the device */

/* Filesystem */

#define	EDIRTY		89		/* Mounting a dirty fs w/o force */

/* Loader errors */

#define	EDUPPKG		90		/* duplicate package name on install */
#define	EVERSION	91		/* version number mismatch */
#define	ENOPKG		92		/* unresolved package name */
#define	ENOSYM		93		/* unresolved symbol name */
#define ETEST           94              /*TEST*/

#ifdef	OSF1_ADFS
/*
 * OSF1_ADFS errors.
 */
#define EREMOTE		100		/* path crosses remote mount point */
#define EBADPORT	101		/* failed port to struct translation */
#define EREMOTEPORT	102		/* returned port is remote */
#define ELOCAL          103             /* handle operation locally */
#endif

#ifdef _KERNEL
/* pseudo-errors returned inside kernel to modify return to process */
#define	ERESTART	(-1)		/* restart syscall */
#define	EJUSTRETURN	(-2)		/* don't modify regs, just return */
#endif

/* Internal Disk/Block Device error codes */
/*
 * These error codes are used in the b_error field of the struct buf,
 * and should never be seen outside the kernel. EMEDIA is returned by
 * a disk driver to indicate a hard ECC error, or similar disk media
 * failure. The caller uses this to distinguish media failures (which
 * it may wish to relocate or otherwise correct) from drive offline 
 * or other problems that would be futile to retry or attempt to correct.
 * The b_resid field should indicate the data successfully transferred
 * before the error occurred.
 * The filesystem always converts this to EIO before returning to user space.
 * ESOFT indicates a correctable error occurred: the data transfer 
 * occurred correctly, but an ECC error was encountered. The caller
 * may choose to take corrective action based on this indication.
 * ERELOCATED is a 'success' code indicating that a defect relocation
 * request was performed successfully.
 */
#define	ESOFT		123
#define	EMEDIA		124
#define	ERELOCATED	125


/* Partition Management Error Codes */
#define EPALLOCERR      130             /* allocator internal error */
#define EPXRS           131             /* exceeds partition resources */
#define EPBADNODE       132             /* bad node specification */
#define EPINVALPART     133             /* partition not found */
#define EPINVALPRI      134             /* invalid priority */
#define EPNOTEMPTY      135             /* partition not empty */
#define EPINVALMOD      136             /* invalid mode */
#define EPPARTEXIST     137             /* partition exists */
#define EPINVALSCHED    138             /* invalid scheduling */
#define EPACCES         139             /* partition permission denied */
#define ERFORK          140             /* do a rfork instead of a fork */
#define EANXACCT        141             /* NX accounting permission denied */
#define EAOVLP          142             /* Request overlaps with nodes in use */
#define EINVALSPS	143	/* Change to SPACE SHARED partition not allowed */
#define EAREJPLK	144	/* Use of -plk not allowed in gang scheduled partition */
#define ESCHEDCONF	145	/* Scheduling parameters conflict with allocator configuration */
#define EEXCEEDCONF	146	/* Exceeded allocator configuration parameters */
#define EAINVALPKT      150     /* packet size invalid or out of range */
#define EAINVALMBF      151     /* memory buffer invalid or out of range */
#define EAINVALMEX      152     /* memory export invalid or out of range */
#define EAINVALMEA      153     /* memory each invalid or out of range */
#define EAINVALSTH      154     /* send threshold invalid or out of range */
#define EAINVALSCT      155     /* send count invalid or out of range */
#define EAINVALGTH      156     /* give threshold invalid or out of range */
#define EANOTPGL        157     /* calling process not process group leader */
#define EAEXIST         158     /* application exists for process group */
#define EPINUSER	159		/* Invalid User */
#define EPINGRP		160		/* Invalid group */
#define EPINRN		161		/* Invalid partition rename */
#define EPLOCK		162	/* Partition lock denied */
#define EPBXRS		163	/* partition request contains bad/missing nodes */
#define EANOEXIST	164	/* application does not exist for process group */
#define EINTERNAL	165	/* Miscellaneous internal error */
#define EPNOMATCH	166	/* Attribute selector specified & some nodes
				   in the map or rectangle do no qualify */

#define F_NXERR 170
#define EQPBUF          (F_NXERR+0)     /* Invalid buffer pointer */
#define EQBLEN          (F_NXERR+1)     /* Buffer length exceeds allocation */
#define EQLEN           (F_NXERR+2)     /* Invalid length */
#define EQTIME          (F_NXERR+3)     /* Time limit exceeded */
#define EQMSGLONG       (F_NXERR+4)     /* Received message too long for buffer
*/
#define EQPID           (F_NXERR+5)     /* Invalid ptype */
#define EQNODE          (F_NXERR+6)     /* Invalid node */
#define EQTYPE          (F_NXERR+7)     /* Invalid type */
#define EQMID           (F_NXERR+8)     /* Invalid message id */
#define EQHND           (F_NXERR+9)     /* Invalid handler type */
#define EQNOPROC        (F_NXERR+10)    /* Out of process slots */
#define EQUSEPID        (F_NXERR+11)    /* Ptype already in use */
#define EQNOACT         (F_NXERR+12)    /* No active process */
#define EQBADFIL        (F_NXERR+13)    /* Invalid object file */
#define EQPARAM         (F_NXERR+14)    /* Invalid parameter */
#define EQPFIL          (F_NXERR+15)    /* Invalid file name pointer */
#define EQPCNODE        (F_NXERR+16)    /* Invalid cnode pointer */
#define EQPCPID         (F_NXERR+17)    /* Invalid cpid pointer */
#define EQPCCODE        (F_NXERR+18)    /* Invalid ccode pointer */
#define EQPRIV          (F_NXERR+19)    /* Privileged operation */
#define EQMEM           (F_NXERR+20)    /* Not enough memory */
#define EQNOMID         (F_NXERR+21)    /* Too many requests */
#define EQSET           (F_NXERR+22)    /* Ptype already set */
#define EQNOSET         (F_NXERR+23)    /* No ptype defined */
#define EQUSM           (F_NXERR+24)    /* Invalid diagnostic channel usm id */
#define EQDIM           (F_NXERR+25)    /* Invalid dimension */
#define EQMODE          (F_NXERR+26)    /* Invalid diagnostic channel mode */
#define EQSTATUS        (F_NXERR+27)    /* Invalid diagnostic channel status */
#define EQMSGSHORT      (F_NXERR+28)    /* Received message too short for buffer
 */
#define ECFPS           (F_NXERR+29)    /* Seek to different file pointers */
#define ENFPS           (F_NXERR+30)    /* Different file pointers */
#define EMIXIO          (F_NXERR+31)    /* Mixed file operations */
#define EIMODE          (F_NXERR+32)    /* Bad io mode number */
#define ESETIO          (F_NXERR+33)    /* File is not synchronized */
#define ENOCFS          (F_NXERR+34)    /* No CFS available */
#define EQESIZE         (F_NXERR+35)    /* Invalid size */
#define ERDEOF          (F_NXERR+36)    /* Attempt to read past end of file */
#define EQPATH          (F_NXERR+37)    /* Path name too long */
#define EQTAM           (F_NXERR+38)    /* Max number of applications under
                                           debug was reached.               */
#define EQPGRP          (F_NXERR+39)    /* Supplied processes group does not
                                           exist or is under control of
                                           another TAM.                     */
#define EFSNOTSUPP	(F_NXERR+40)	/* Operation not supported by this  */
					/* file system.                     */
#define ESTRIPED	(F_NXERR+41)	/* PFS file (internal use only) */
#define ENOTPFS		(F_NXERR+42)	/* non-striped regular file in a PFS */
#define ENOSDIR		(F_NXERR+43)	/* PFS stripe dir not available */
#define EPFSBUSY	(F_NXERR+44)	/* PFS stripe file in use */
#define EBADID		(F_NXERR+45)	/* Asynchronous request ID invalid */
#define EINCOMPAT       (F_NXERR+46)    /* The application and the OS are
					   of incompatible revisions.      */
#define EQSENDABORT 	(F_NXERR+47)    /* Sender aborted during message 
						receive */

#endif /* _ANSI_C_SOURCE */

#if defined(_OSF_SOURCE) && !defined(_KERNEL)

#ifdef _NO_PROTO
extern void perror();
#else  /* _NO_PROTO */
extern void perror(const char *);
#endif  /* _NO_PROTO */

extern char *sys_errlist[];
extern int sys_nerr;

#endif	/* defined(_OSF_SOURCE) && !defined(_KERNEL) */

#endif /* _SYS_ERRNO_H_ */
