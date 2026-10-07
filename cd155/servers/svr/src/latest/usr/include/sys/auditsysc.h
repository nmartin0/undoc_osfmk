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
 * Copyright (c) 1987-1990 SecureWare, Inc.  All Rights Reserved.
 *
 * @OSF_COPYRIGHT@
 *
 */
/*
 * HISTORY
 * $Log: auditsysc.h,v $
 * Revision 1.3  1994/11/18  21:11:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:18:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:01:27  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:20:40  cfj
 * Bump major revision number.
 *
 * Revision 1.3  1992/04/13  14:33:22  stans
 * "OSF-distribution-ad8.5.2"
 *
 * Revision 1.3  90/10/07  20:37:33  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:00:31  gm]
 * 
 * Revision 1.2  90/06/22  21:52:36  devrcs
 * 	Used to allow compiling of libsecurity until the secureware
 * 	intergrates a better method to create this file from.
 * 	[90/06/20  13:40:43  mckeen]
 * 
 * $EndLog$
 */

#ifdef _OSF_SOURCE

#if CMUCS_RP || !defined(KERNEL)
#else  
#endif  
#ifdef sun4 
#else  
#endif  
#define	AUDIT_EXIT	1		/* rexit exit */
#define	AUDIT_FORK	2		/* fork */
#define	AUDIT_READ	3		/* read */
#define	AUDIT_WRITE	4		/* write */
#define	AUDIT_OPEN	5		/* open */
#define	AUDIT_CLOSE	6		/* close */
#define	AUDIT_CREAT	8		/* creat */
#define	AUDIT_LINK	9		/* link */
#define	AUDIT_UNLINK	10		/* unlink */
#define	AUDIT_EXECV	11		/* execv */
#define	AUDIT_CHDIR	12		/* chdir */
#define	AUDIT_MKNOD	14		/* mknod */
#define	AUDIT_SETUID	23		/* setuid */
#ifdef sun 
#else  
#endif  
#ifdef COMPAT_43 
#else  
#define	AUDIT_RECVMSG	27		/* recvmsg */
#define	AUDIT_SENDMSG	28		/* sendmsg */
#define	AUDIT_RECVFROM	29		/* recvfrom */
#define	AUDIT_ACCEPT	30		/* accept */
#endif  
#define	AUDIT_KILL	37		/* kill */
#define	AUDIT_DUP	41		/* dup */
#define	AUDIT_PIPE	42		/* pipe */
#ifdef KTRACE 
#else  
#endif  
#if defined(balance) || defined(mips)
#else  
#endif  
#ifdef notyet 
#else  
#endif  
#if MACH_AFS 
#else  
#endif  
#define	AUDIT_REBOOT	55		/* reboot */
#define	AUDIT_PLOCK	56		/* plock */
#define	AUDIT_SYMLINK	57		/* symlink */
#define	AUDIT_EXECVE	59		/* execve */
#define	AUDIT_CHROOT	61		/* chroot */
#ifdef notyet 
#else  
#endif  
#define	AUDIT_VFORK	66		/* vfork */
#if MACH_AFS 
#else  
#define	AUDIT_SETGROUPS	80		/* setgroups */
#endif  
#ifdef mips 
#else  
#endif  
#define	AUDIT_SETHOSTNAME	88		/* sethostname */
#define	AUDIT_DUP2	90		/* dup2 */
#define	AUDIT_FCNTL	92		/* fcntl */
#define	AUDIT_SOCKET	97		/* socket */
#define	AUDIT_CONNECT	98		/* connect */
#ifdef COMPAT_43 
#define	AUDIT_ACCEPT	99		/* oaccept accept */
#else  
#endif  
#ifdef COMPAT_43 
#define	AUDIT_SEND	101		/* osend send */
#define	AUDIT_RECV	102		/* orecv recv */
#else  
#endif  
#define	AUDIT_BIND	104		/* bind */
#if defined(balance) || defined(mips)
#else  
#endif  
#ifdef COMPAT_43 
#define	AUDIT_RECVMSG	113		/* orecvmsg recvmsg */
#define	AUDIT_SENDMSG	114		/* osendmsg sendmsg */
#else  
#endif  
#ifdef TRACE 
#else  
#endif  
#if defined(vax) || defined(i386)
#else  
#endif  
#define	AUDIT_READV	120		/* readv */
#define	AUDIT_WRITEV	121		/* writev */
#define	AUDIT_FCHOWN	123		/* fchown */
#define	AUDIT_FCHMOD	124		/* fchmod */
#ifdef COMPAT_43 
#define	AUDIT_RECVFROM	125		/* orecvfrom recvfrom */
#else  
#endif  
#define	AUDIT_SETREUID	126		/* setreuid */
#define	AUDIT_SETREGID	127		/* setregid */
#define	AUDIT_RENAME	128		/* rename */
#define	AUDIT_TRUNCATE	129		/* truncate */
#define	AUDIT_FTRUNCATE	130		/* ftruncate */
#if MACH_AFS 
#else  
#endif  
#define	AUDIT_SETGID	132		/* setgid */
#define	AUDIT_SENDTO	133		/* sendto */
#define	AUDIT_SOCKETPAIR	135		/* socketpair */
#define	AUDIT_MKDIR	136		/* mkdir */
#define	AUDIT_RMDIR	137		/* rmdir */
#ifdef COMPAT_43 
#else  
#endif  
#define	AUDIT_SETHOSTID	143		/* sethostid */
#ifdef COMPAT_43 
#else  
#endif  
#define GENERIC 1
#ifdef ibmrt 
#undef GENERIC 
#if MACH_AFS 
#else  
#endif  
#define	AUDIT_GETDIRENTRIES	170		/* getdirentries */
#define	AUDIT_SETDOMAINNAME	177		/* setdomainname */
#endif /* ibmrt */
#ifdef sun 
#undef GENERIC 
#define	AUDIT_GETDIRENTRIES	156		/* getdirentries */
#define	AUDIT_SETDOMAINNAME	163		/* setdomainname */
#if MACH_AFS 
#else /* MACH_AFS */
#endif /* MACH_AFS */
#endif /* sun */
#if defined(vax) || defined(i386)
#undef GENERIC 
#if MACH_AFS 
#else  
#endif  
#define	AUDIT_GETDIRENTRIES	164		/* getdirentries */
#define	AUDIT_SETDOMAINNAME	170		/* setdomainname */
#if MACH_AFS 
#else  
#endif  
#endif /* defined(vax) || defined(i386) */
#if defined(balance) 
#undef GENERIC 
#define	AUDIT_GETDIRENTRIES	151		/* getdirentries */
#define	AUDIT_SETDOMAINNAME	163		/* setdomainname */
#if MACH_AFS 
#else /* MACH_AFS */
#endif /* MACH_AFS */
#endif /* defined(balance) */
#ifdef mips 
#undef GENERIC 
#define	AUDIT_GETDIRENTRIES	159		/* getdirentries */
#define	AUDIT_SETDOMAINNAME	166		/* setdomainname */
#endif /* mips */
#ifdef multimax 
#undef GENERIC 
#define	AUDIT_GETDIRENTRIES	156		/* getdirentries */
#define	AUDIT_SETDOMAINNAME	163		/* setdomainname */
#if MACH_AFS 
#else  
#endif  
#endif /* multimax */
#ifdef GENERIC 
#define	AUDIT_GETDIRENTRIES	156		/* getdirentries */
#define	AUDIT_SETDOMAINNAME	163		/* setdomainname */
#if MACH_AFS && !defined(PMAX)
#else /* MACH_AFS  && !defined(PMAX) */
#ifdef PMAX 
#define	AUDIT_MSGCTL	172		/* msgctl */
#define	AUDIT_MSGGET	173		/* msgget */
#define	AUDIT_MSGRCV	174		/* msgrcv */
#define	AUDIT_MSGSND	175		/* msgsnd */
#define	AUDIT_SEMCTL	176		/* semctl */
#define	AUDIT_SEMGET	177		/* semget */
#define	AUDIT_SEMOP	178		/* semop */
#else /* PMAX */
#endif /* PMAX */
#endif /* MACH_AFS && !defined(PMAX) */
#endif /* GENERIC */
#ifdef PMAX 
#undef GENERIC 
#define	AUDIT_PLOCK	181		/* plock */
#ifdef notyet 
#else /* notyet */
#endif /* notyet */
#undef SYS_mount 
#ifdef notyet 
#undef SYS_unmount 
#else /* notyet */
#endif /* notyet */
#else /* PMAX */
#ifdef ibmrt 
#else /* ibmrt */
#endif /* ibmrt */
#if MACH_VMTP 
#if VMTP_INVOKE 
#else /*VMTP_INVOKE */
#endif /* VMTP_INVOKE */
#else /* MACH_VMTP */
#endif /* MACH_VTMP */
#endif /* PMAX */
#ifdef PMAX 
#if MACH_AFS 
#else /* MACH_AFS */
#endif /* MACH_AFS */
#else /* PMAX */
#endif /* PMAX */
#define	AUDIT_MSGCTL	200		/* msgctl */
#define	AUDIT_MSGGET	201		/* msgget */
#define	AUDIT_MSGRCV	202		/* msgrcv */
#define	AUDIT_MSGSND	203		/* msgsnd */
#define	AUDIT_SEMCTL	204		/* semctl */
#define	AUDIT_SEMGET	205		/* semget */
#define	AUDIT_SEMOP	206		/* semop */
#define	AUDIT_SHMAT	209		/* shmat */
#define	AUDIT_SHMCTL	210		/* shmctl */
#define	AUDIT_SHMDT	211		/* shmdt */
#define	AUDIT_SHMGET	212		/* shmget */
#if SEC_BASE 
#define	AUDIT_SECURITY	222		/* security */
#else  
#endif  
#if MACH_AFS || !defined(KERNEL)
#undef SYS_pioctl 
#undef SYS_setpag 
#undef SYS_iopen 
#undef SYS_icreate 
#undef SYS_iread 
#undef SYS_iwrite 
#undef SYS_iinc 
#undef SYS_idec 
#undef SYS_afs_call 
#else /* MACH_AFS || !defined(KERNEL) */
#endif /* MACH_AFS || !defined(KERNEL) */
#ifdef PMAX 
#else /* PMAX */
#endif /* PMAX */
#endif
