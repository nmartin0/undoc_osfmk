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
 * OSF/1 Release 1.0.3
 */
/* @(#)$RCSfile: unistd.h,v $ $Revision: 1.6 $ (OSF) $Date: 1994/11/19 02:41:05 $ */
/*
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 * 
 * unistd.h	1.17  com/inc,3.1,8943 9/22/89 15:13:11
 */

#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <standards.h>
#include <sys/types.h>  /* for time_t */
#include <sys/access.h>	/* for the "access" function */

/*
 * POSIX requires that certain values be included in unistd.h.  It also
 * requires that when _POSIX_SOURCE is defined only those standard
 * specific values are present.  This header includes all the POSIX
 * required entries.
 */

#ifdef _POSIX_SOURCE
/* Symbolic constants for the "lseek" function: */
#define SEEK_SET 0	/* Set file pointer to "offset" */
#define SEEK_CUR 1	/* Set file pointer to current plus "offset" */
#define SEEK_END 2	/* Set file pointer to EOF plus "offset" */


extern char **environ;          /* Environment description variable */


#ifdef _NO_PROTO

extern int access();
extern unsigned int alarm();
extern int chdir();
extern int chown();
extern int close();
extern char *ctermid();
extern char *cuserid();
extern int dup();
extern int dup2();
extern int execl();
extern int execv();
extern int execle(); 
extern int execve();
extern int execlp();
extern int execvp();
extern void _exit();
extern pid_t fork();
extern long fpathconf();
extern char *getcwd();
extern gid_t getegid();
extern char *getenv();
extern uid_t geteuid();
extern gid_t getgid();
extern int getgroups();
extern char *getlogin();
extern pid_t getpgrp();
extern pid_t getpid();
extern pid_t getppid();
extern uid_t getuid();
extern int isatty();
extern int link();
extern off_t lseek();
extern long pathconf();
extern int pause();
extern int pipe();
extern ssize_t read();
extern int rename();
extern int rmdir();
extern int setgid();
extern int setpgid();
extern int setsid(); 
extern int setuid();
extern unsigned int sleep();
extern long sysconf();
extern pid_t tcgetpgrp();
extern int tcsetpgrp();
extern char *ttyname();
extern int unlink();
extern ssize_t write(); 
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
extern int getlogin_r();
extern char *ttyname_r();
#endif

#else		/* POSIX required prototypes */

extern int access(char *, int );
extern unsigned int alarm(unsigned int );
extern int chdir(char *);
extern int chown(char *, uid_t , gid_t );
extern int close(int );
extern char *ctermid(char *);
extern char *cuserid(char *);
extern int dup(int );
extern int dup2(int , int );
extern int execl(const char *, const char *, ...);
extern int execv(const char *, char * const []);
extern int execle(const char *, const char *, ...); 
extern int execve(const char *, char * const [], char * const []);
extern int execlp(const char *, const char *,  ...); 
extern int execvp(const char *, char * const []);
extern void _exit(int );
extern pid_t fork(void);
extern long fpathconf(int , int );
extern char *getcwd(char *, size_t );
extern gid_t getegid(void);
extern char *getenv(const char *);
extern uid_t geteuid(void);
extern gid_t getgid(void);
extern int getgroups(int , gid_t grouplist[]);
extern char *getlogin(void);
extern pid_t getpgrp(void);
extern pid_t getpid(void);
extern pid_t getppid(void);
extern uid_t getuid(void);
extern int isatty(int );
extern int link(char *, char *);
extern off_t lseek(int , off_t , int );
extern long pathconf(char *, int );
extern int pause(void);
extern int pipe(int fd[]);
extern ssize_t read(int , void *, size_t);
extern int rename(const char *, const char *);
extern int rmdir(char *);
extern int setgid(gid_t );
extern int setpgid(pid_t , pid_t );
extern pid_t setsid(void);
extern int setuid(uid_t );
extern unsigned int sleep(unsigned int );
extern long sysconf(int );
extern pid_t tcgetpgrp(int );
extern int tcsetpgrp(int , pid_t );
extern char *ttyname(int );
extern int unlink(char *);
extern ssize_t write(int , void *, size_t); 
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
extern int getlogin_r(char *, int);
extern char *ttyname_r(int, char *, int);
#endif
#endif		/* !_NO_PROTO	*/

#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2

#define _POSIX_JOB_CONTROL	/* implementation supports job control */
#define _POSIX_SAVED_IDS	/* saved set-user-ID and set-group-ID */
#define _POSIX_VERSION		199009L

#define _POSIX_CHOWN_RESTRICTED	 0 /* chown requires appropriate privileges */
#define _POSIX_NO_TRUNC	     	 0 /* too-long path components gen errors */
#define _POSIX_VDISABLE		((unsigned char)'\377')

#define	_POSIX_THREADS		/* 1003.4a (pthreads) comformant */
#define _POSIX_THREAD_ATTR_STACKSIZE /* support for specifiable stack sizes */
#define	_POSIX_REENTRANT_FUNCTIONS  /* multithreaded 1003.1 interfaces */

#ifndef NULL
#define NULL	(void *)0
#endif


/* arguments for the sysconf() function */

#define _SC_ARG_MAX		1
#define _SC_CHILD_MAX		2
#define _SC_CLK_TCK		3
#define _SC_JOB_CONTROL		4
#define _SC_NGROUPS_MAX		5
#define _SC_OPEN_MAX		6
#define _SC_SAVED_IDS		8
#define _SC_VERSION		9
#define _SC_STREAM_MAX          13
#define _SC_TZNAME_MAX          14

/* arguments for the pathconf() function */

#define _PC_CHOWN_RESTRICTED	10
#define _PC_LINK_MAX		11
#define _PC_MAX_CANON		12
#define _PC_MAX_INPUT		13
#define _PC_NAME_MAX		14
#define _PC_NO_TRUNC		15
#define _PC_PATH_MAX		16
#define _PC_PIPE_BUF		17
#define _PC_VDISABLE 		18

#endif /* _POSIX_SOURCE */

#ifdef _XOPEN_SOURCE

#define _SC_PASS_MAX		7
#define _SC_XOPEN_VERSION	19

#define _XOPEN_VERSION		3

#define F_ULOCK	0	/* Unlock a previously locked region */
#define F_LOCK	1	/* Lock a region for exclusive use */
#define F_TLOCK	2	/* Test and lock a region for exclusive use */
#define F_TEST	3	/* Test a region for other processes locks */

#endif /* _XOPEN_SOURCE */

#ifdef _AES_SOURCE

#define _SC_ATEXIT_MAX		10
#define _SC_PAGE_SIZE		11
#define _SC_AES_OS_VERSION	12

#define _AES_OS_VERSION		1

#ifdef _NO_PROTO
extern int fchown();
extern int ftruncate();
extern int readlink();
extern int setgroups();
extern int symlink();
extern int truncate();
#else
extern int fchown(int, uid_t, gid_t);
extern int ftruncate(int, off_t);
extern int readlink(const char *, char *, int);
extern int setgroups(int, gid_t []);
extern int symlink(const char *, const char *);
extern int truncate(const char *, off_t);
#endif

#endif 	/* _AES_SOURCE */

#endif /* _UNISTD_H_ */
