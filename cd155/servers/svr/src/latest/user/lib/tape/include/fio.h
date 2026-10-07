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
 *	fio.h 11.1 94/03/25 15:51:51
 */
/** static char	fioh_ver[] = "@(#) fio.h 11.1 94/03/25 15:51:51";
/**/

/*
 *
 * . { TEXT
 * .H 4 fio.h
 *
 * Fileio declarations, used in the top level
 * of the CFS library.
 *
 * . } TEXT
 */
#if REMOTEHOST
#define FILE_TABLE_SIZE 64
#else
#define FILE_TABLE_SIZE 100
#endif


/* Initialization flag, to allow transparent initialization of the library */
extern int fileio_init_flag;

/* Values of file_ds.hnode to distinguish between host I/O and CFS */
#define F_UNUSED	-1
#define F_RESERVED	-2
#define F_CFS		-3
#define F_PIPE		-4
#define F_SOC		-5

/* Bit fields of file_ds.fs_pid, also includes open flags in 0xFFF */
#define FOMASK		0x000FFF
#define FACC		0x001000	
#define FUPD 		0x002000	
#define FCHG		0x004000	
#define DUPFLG		0x008000
#define ISDUPF(f)	(f & DUPFLG)
#define FPDONE		0x010000
#define FNOBLOCK	0x020000
#define FEOF		0x080000
#define FVBLOCK         0x100000  /* Set for variable block tape. */
#define FCLS_ON_EXEC    0x200000  /* Set for close on exec */
#define FEOD            0x400000  /* Set for end of tape data indicator */

/**** 3480 change by Ernie Granese 11/12/91 - JTI ****/

#define FLEOT           0x1000000 /* Set for Logical EOT for '3480' tape */
#define FPEOT           0x2000000 /* Set for Physical EOT for '3480' tape */

/**** end 3480 change - JTI ****/

/* The following macros are used for setting/getting file i/o mode */
#define CIOMODE(fg)	((fg >> 17) & 0x3)
#define NIOMODE(md)	(md << 17)

/* File descriptor table */
extern struct file_ds {
	int	hnode;
	int 	fs_pid;
	int	fd;
	BN	bn;
	esize_t	position;
	struct pipebuf *pbuf;
	char	*dirbuf;
} fds[];

/* File position table */
extern struct file_pos {
	int	cnt;
	esize_t	pos;
} fps[];

/* proc structure for UNIX drivers */
struct proc {
        char    p_stat;                 /* status of process */
        char    p_pri;                  /* priority, negative is high */
        char    p_cpu;                  /* cpu usage for scheduling */
        char    p_nice;                 /* nice for cpu usage */
        int     p_pgrp;                 /* name of process group leader */
        int     p_pid;                  /* unique process id*/
        caddr_t p_wchan;
};

/* user structure for UNIX drivers */

typedef struct  unix_user {
	unsigned char 	u_error;	/* return error code */
	unsigned short	u_uid;		/* effective user id */
	unsigned short	u_gid;		/* effective group id */
	unsigned short	u_ruid;		/* real user id */
	unsigned short	u_rgid;		/* real group id */
	struct proc	*u_procp;	/* pointer to proc struct */
	int 		*u_ap;		/* pointer to arglist */
	union {				/* syscall return values */
		struct {
			int 	r_val1;
			int 	r_val2;
		}r_req;
		off_t	r_off;
		time_t	r_time;
	} u_r;
	char    *u_base;		/* base address for IO */
        unsigned u_count;       	/* bytes remaining for IO */
        long    u_offset;	       	/* offset in file for IO */
        short   u_fmode;        	/* file mode for IO */
        unsigned short  u_pbsize;      	/* Bytes in block for IO */
        unsigned short  u_pboff;       	/* offset in block for IO */
        short   u_pbdev;        	/* real device for IO */
        long    u_rablock;      	/* read ahead block address */
        short   u_errcnt;       	/* syscall error count */
        char    *u_dirp;         	/* pathname pointer */
        char    *u_pofile;      	/* Ptr to open file flag array.  */
        int     u_arg[6];       	/* arguments to current system call */
        short   *u_ttyp;                /* pointer to pgrp in "tty" struct */
        short   u_ttyd;                 /* controlling tty dev */
        short   u_cmask;                /* mask for file creation */
	struct {
		int sock_node;		/* slot # PBX/TCP brd for this proc */
		struct proc pp;		/* Unix psuedo proc struct */
	} nx;
} user_t;

#define u_rval1 u_r.r_req.r_val1
#define u_rval2 u_r.r_req.r_val2
#define u_roff  u_r.r_off
#define u_rtime u_r.r_time


/* Pipe buffer */
struct pipebuf {
	int	flags;
	int	cnt;
	int	pos;
	char	data[5120];
};

extern char fio_cfn[256];	/* Canonical filename */

extern char fio_cwd[256];	/* Current working directory */

extern char fio_mnt[256];	/* Mount directory for cfs */

extern unsigned short fio_uid;	/* Real user id */
extern unsigned short fio_euid;	/* Effective user id */
extern unsigned short save_setuid;  /* Effective user id saved before setuid */
extern unsigned short fio_gid;	/* Real group id */
extern int fio_cmask;		/* File creation mask */

BN locblock();
BN filealloc();

/*
 * Local node and pid
 */

extern _local_node;
extern _local_pid;
