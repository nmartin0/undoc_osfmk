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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/* static char rcsid[] = "@(#)$RCSfile: sh_config.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:03 $"; */
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#ifndef _sh_config_
#define _sh_config_	1

#include <sys/limits.h>
#include <sys/types.h>

/* From analysis and running of config program */

#define _dirent_	1
#define _execargs_	1
#define _fcntl_		1
#define _locale_	1
#define _mnttab_	1
#define _termios_	1
#define _time_		1
#define _unistd_	1
#define	_sys_resource_	1
#define _sys_times_	1
#define _sys_utsname_	1
#define _sys_wait_	1
#define _usr_ucb_       1
#define _bin_grep_      1
#define _usr_bin_vi_    1
#define _bin_newgrp_    1

#define VOID		void

/* AIX modifications */
#define	_sys_siglist_	1		/* use sys_siglist var from libc */

/* install options */
#define POSIX		1
#define ECHO_RAW        1
#define ESH		1
#define JOBS		1
#define	LSTAT		1
#define NEWTEST         1
#define NGROUPS         NGROUPS_MAX
#define NFILE           OPEN_MAX
#define PIPE_ERR	29
#define OLDTEST         1
#define SHELLMAGIC	1
#define SIG_NORESTART	1
#define SYSCALL		1
#define VSH             1
#define WINSIZE         1

#define getpgid(a)	getpgrp()
#define sh_rand()	rand()
#define sig_begin()
#define sigrelease(s)	sigrelse(s)

#undef _ndir_


#endif
