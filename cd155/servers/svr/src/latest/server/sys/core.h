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
 * Definition of the Paragon OSF/1 core file format
 *
 * $Id: core.h,v 1.3 1994/11/18 20:40:12 mtm Exp $
 *
 * HISTORY
 * $Log: core.h,v $
 * Revision 1.3  1994/11/18  20:40:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/27  15:41:37  johannes
 * changed header
 *
 */

#ifndef __CORE_H__
#define __CORE_H__

#include <sys/types.h>
#include <sys/time.h>

#include <mach/mach_types.h>

#define CORE_MAGIC	0xdeadbeef	/* core file magic number */
#define CORE_SWAP	0xff00		/* byte swap field */
#define CORE_VERSION	1		/* initial core file version */
#define CORE_TYPE_FULL	0xffffffff	/* full core file */
#define CORE_TYPE_TRACE	0x00010000	/* only stack trace */

struct core_proc_info {
    pid_t	c_pid;		/* process id */ 
    pid_t	c_ppid;		/* parent process id */ 
    pid_t	c_pgid;		/* process group leader id */ 
    long	c_prglen;	/* length of program path */
    off_t	c_prgname;	/* offset of relative path name of program */
    long	c_rootlen;	/* length of exec root directory path */
    off_t	c_rootname;	/* offset of exec root directory path */
    long	c_cwdlen;	/* length of exec current directory path */
    off_t	c_cwdname;	/* offset of exec current directory path */
};

/*
* Description of a VM region, so it can be recreated from the core
* file.
*/

struct core_region_desc {
    off_t	    r_offset;	/* offset of VM region in core file */
    vm_address_t    r_vaddr;	/* virtual address of region start */
    vm_size_t	    r_size;	/* region size (bytes) */
    vm_size_t	    r_sav_off;	/* offset of saved part of region */
    vm_prot_t	    r_prot;	/* VM protection (e.g. VM_PROT_READ) */
    vm_prot_t	    r_max_prot;	/* maximum allowed VM protection */
};

struct core_hdr {
    int		c_magic;	/* core file magic number */
    unsigned short c_swap;	/* byte swap field */
    short	c_version;	/* core file version */
    int		c_type;		/* core file type */
    struct timeval c_timdat;	/* time & date core file created */
    int		c_signo;	/* signal which killed us, if any */
    int		c_sigcode;	/* signal subcode */
    long	c_numnodes;	/* number of nodes in the partition */
    long	c_node;		/* logical node number of the dumping process */
    long	c_ptype;	/* ptype of the dumping process */
    off_t	c_procinfo;	/* offset of process info */
    off_t	c_applinfo;	/* offset of application info (APPLINFO_T) */
    long	c_nregions;	/* number of region descriptors */
    off_t	c_firstregion;	/* offset of first region desc in core file */
    long	c_nthreads;	/* number of active threads */
    off_t	c_firstthread;	/* offset of first thread_info in core file */
    long	c_activethread;	/* index of last active (faulting) thread */
};

#endif
