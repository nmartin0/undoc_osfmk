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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * This file was modified and extended by the Center for High Performance
 * Computing of Worcester Polytechnic Institute on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: conf.h,v $
 * Revision 1.5  1994/11/18  20:40:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:24:00  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:32:54  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:18:24  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:41:30  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:14  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:37:54  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:41:01  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.6  1993/01/08  14:31:11  durriya
 * 	add node # as arg to BDEVSW_CLOSE, BDEVSW_IOCTL, CDEVSW_CLOSE,
 * 	CDEVSW_READ, CDEVSW_WRITE, CDEVSW_IOCTL for OSF1_ADFS     durriya
 *
 * Revision 2.4  92/01/05  20:06:03  roy
 * 	1991/09/22  17:08:35  noemi
 * 	OSF1/ADFS update
 * 
 * Revision 2.3  91/11/25  16:15:17  rabii
 *	Added remote devices
 *
 * Revision 2.2  91/08/31  14:03:09  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/07/31  15:42:08  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/31  14:06:21  devrcs
 * 	Change BDEVSW_PSIZE macro to check for existence of psize
 * 	entry in bdevsw table before invoking it.
 * 	[90/10/24  17:27:41  dlb]
 * 
 * Revision 1.8  90/10/07  14:49:36  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:37:56  gm]
 * 
 * Revision 1.7  90/09/23  15:59:38  devrcs
 * 	Fixed for ANSI compliance
 * 	[90/09/05  17:14:23  rossi]
 * 
 * Revision 1.6  90/07/27  09:06:56  devrcs
 * 	Improved device switch locking.
 * 	[90/07/20  17:05:50  nags]
 * 
 * Revision 1.5  90/06/22  20:52:40  devrcs
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Device funnelling and macros.			nags@encore.com
 * 	Device switch locking.				knight@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Added d_ioctl field to bdevsw.			noemi@osf.org
 * 	Integrated early Mach2.5 with Encore MP.	alan@encore.com
 * 	More cleanup.					rpd@cmu.edu
 * 	Got rid of MACH conditionals and non-MACH code.	mrt@cmu.edu
 * 	Made declarations "extern", for Mips compiler.	af@cmu.edu
 * 	Adjusted include file references.		mwyoung@cmu.edu
 * 	Added l_select field to linesw for X on the RT.	bolosky@cmu.edu
 * 	MACH: removed definition of swdevt.		bolosky@cmu.edu
 * 	[90/06/12  21:38:00  gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)conf.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_CONF_H_
#define _SYS_CONF_H_

#ifdef	_KERNEL
#include <sys/unix_defs.h>
#endif

#ifdef	OSF1_SERVER
#define	C_MINOR		(0x100)		/* not all minors have same name */
#define	C_BLOCK(n)	(0x200 | ((n)<<16))
					/* fold 'n' minor device numbers
					   into partitions on same device */
#define	C_BLOCK_GET(f)	(((f)>>16) & 0xFF)
#endif	OSF1_SERVER

/*
 * Declaration of block device
 * switch. Each entry (row) is
 * the only link between the
 * main unix code and the driver.
 * The initialization of the
 * device switches is in the
 * file conf.c.
 */

struct bdevsw
{
#ifdef	OSF1_SERVER
	char	*d_name;
#endif	OSF1_SERVER
	int	d_flags;
	int	(*d_open)();
	int	(*d_close)();
	int	(*d_strategy)();
	int	(*d_dump)();
	int	(*d_psize)();
	int	(*d_ioctl)();
#ifdef	_KERNEL
	udecl_funnel_data(,d_funnel)
#endif
};

#ifdef	_KERNEL
extern struct	bdevsw bdevsw[];
extern int	nblkdev;			/* bdevsw size */
#endif

struct cdevsw
{
#ifdef	OSF1_SERVER
	char	*d_name;
	int	d_flags;
#endif	OSF1_SERVER
	int	(*d_open)();
	int	(*d_close)();
	int	(*d_read)();
	int	(*d_write)();
	int	(*d_ioctl)();
	int	(*d_stop)();
	int	(*d_reset)();
	struct tty *d_ttys;
	int	(*d_select)();
	int	(*d_mmap)();
#ifdef	_KERNEL
	udecl_funnel_data(,d_funnel)
#endif
#ifdef	OSF1_SERVER
	mach_port_t (*d_port)();
#endif	OSf1_SERVER
};

#ifdef	_KERNEL
extern struct	cdevsw cdevsw[];
extern int	nchrdev;			/* cdevsw size */
#endif

/*
 * tty line control switch.
 */

struct linesw
{
	int	(*l_open)();
	int	(*l_close)();
	int	(*l_read)();
	int	(*l_write)();
	int	(*l_ioctl)();
	int	(*l_rint)();
	int	(*l_rend)();
	int	(*l_meta)();
	int	(*l_start)();
	int	(*l_modem)();
#ifdef	IBM_CONSOLE
	int	(*l_select)();
#endif
};

#ifdef	_KERNEL
extern struct	linesw linesw[];
#endif

#ifdef	_KERNEL
#include <ser_compat.h>
#include <kern/lock.h>

/*
 * Character and Block Device Switch Locking definition.
 *
 * A device switch lock guards a particular entry in a
 * device switch table; these locks are used by the
 * driver open routines.  Note that these locks have
 * nothing to do with synchronizing the drivers themselves;
 * they guard the use of entries in the devsw arrays.
 */
typedef struct dswlock {
        lock_data_t     dsw_lock;               /* devsw entry r/w lock */
        int             dsw_flags;              /* devsw entry status flags */
} dswlock_t;

/*
 * Parallel device switch lock arrays, one entry
 * for each entry in cdevsw and bdevsw.
 */
extern dswlock_t cdevlock[];                    /* cdevsw lock structure */
extern dswlock_t bdevlock[];                    /* bdevsw lock structure */

						/* dsw flag defines */
#define DSW_INUSE	0x0001
						/* Lock macros for open() */
#define CDEVSW_READ_LOCK(maj)		lock_read(&(cdevlock[maj].dsw_lock))
#define CDEVSW_WRITE_LOCK(maj)		lock_write(&(cdevlock[maj].dsw_lock))
#define CDEVSW_READ_UNLOCK(maj)		lock_done(&(cdevlock[maj].dsw_lock))
#define CDEVSW_WRITE_UNLOCK(maj)	lock_done(&(cdevlock[maj].dsw_lock))
#define BDEVSW_READ_LOCK(maj)		lock_read(&(bdevlock[maj].dsw_lock))
#define BDEVSW_WRITE_LOCK(maj)		lock_write(&(bdevlock[maj].dsw_lock))
#define BDEVSW_WRITE_UNLOCK(maj)	lock_done(&(bdevlock[maj].dsw_lock))
#define BDEVSW_READ_UNLOCK(maj)		lock_done(&(bdevlock[maj].dsw_lock))
#define CDEVSW_LOCK_INIT(maj)		\
		lock_init2(&(cdevlock[maj].dsw_lock), TRUE, LTYPE_CDEVSW)
#define BDEVSW_LOCK_INIT(maj)		\
		lock_init2(&(bdevlock[maj].dsw_lock), TRUE, LTYPE_BDEVSW)

#ifdef __STDC__
#define PASTE(a,b) a##b
#else
#define PASTE(a,b) a/**/b
#endif

/*
 * Backwards compatibility for unparallelized device drivers.
 * Unparallelized device drivers can be funnelled.	XXX
 */
#define	DEVSW_FUNNEL(sw,maj)		FUNNEL(PASTE(sw,devsw)[(maj)].d_funnel)
#define	DEVSW_UNFUNNEL(sw,maj)		UNFUNNEL(PASTE(sw,devsw)[(maj)].d_funnel)

/*
 * The following macros hide the use of funnelling from most
 * kernel code.  Most of the macros use _DVC_ but there are
 * three special cases for fetching an associated tty array,
 * flags, or the partition size (optional entry).
 */
#define	_DVC_(sw,f,maj,args,ret)					\
MACRO_BEGIN								\
	DEVSW_FUNNEL(sw,(maj));						\
	(ret) = (*PASTE(sw,devsw)[(maj)].PASTE(d_,f))args;		\
	DEVSW_UNFUNNEL(sw,(maj));					\
MACRO_END

#define	_TTYS_(sw,maj,min,ret)						\
MACRO_BEGIN								\
	DEVSW_FUNNEL(sw,(maj));						\
	(ret) = &PASTE(sw,devsw)[(maj)].d_ttys[(min)];			\
	DEVSW_UNFUNNEL(sw,(maj));					\
MACRO_END

#define	_FLAGS_(sw,maj,ret)						\
MACRO_BEGIN								\
	DEVSW_FUNNEL(sw,(maj));						\
	(ret) = PASTE(sw,devsw)[(maj)].d_flags;				\
	DEVSW_UNFUNNEL(sw,(maj));					\
MACRO_END

#define	_DVC_OPT_(sw,f,maj,args,ret)					\
MACRO_BEGIN								\
	DEVSW_FUNNEL(sw,(maj));						\
	if (PASTE(sw,devsw)[(maj)].PASTE(d_,f))				\
		(ret) = (*PASTE(sw,devsw)[(maj)].PASTE(d_,f))args;	\
	else								\
  		(ret) = -1;						\
	DEVSW_UNFUNNEL(sw,(maj));					\
MACRO_END

/*
 * These macros were developed to have a single interface for the cdevsw
 * and the bdevsw in code, but easily adding funneling.
 *
 * They are all called with 'similar' arguments:
 *	(major device #, arg1, arg2, ..., result)
 * where:
 *	result will be set to the return value of the b/cdevsw call.
 *	arg[12...] is the list of arguments to be passed into the call.
 */
/*
 * Special-case open macros to use devsw locking to synchronize with 
 * dynamic installation of drivers.
 */
#ifdef	OSF1_ADFS
#define	BDEVSW_OPEN(maj,dev,mode,flag,node,ret)				\
MACRO_BEGIN								\
	BDEVSW_READ_LOCK(maj);						\
	DEVSW_FUNNEL(b,(maj));						\
	(ret) = (*bdevsw[(maj)].d_open)(dev,mode,flag,node);		\
	DEVSW_UNFUNNEL(b,(maj));					\
	BDEVSW_READ_UNLOCK(maj);					\
MACRO_END
#else
#define	BDEVSW_OPEN(maj,dev,mode,flag,ret)				\
MACRO_BEGIN								\
	BDEVSW_READ_LOCK(maj);						\
	DEVSW_FUNNEL(b,(maj));						\
	(ret) = (*bdevsw[(maj)].d_open)(dev,mode,flag);			\
	DEVSW_UNFUNNEL(b,(maj));					\
	BDEVSW_READ_UNLOCK(maj);					\
MACRO_END
#endif

#ifdef	OSF1_ADFS
#define	CDEVSW_OPEN(maj,dev,mode,flag,newdev,node,ret)			\
MACRO_BEGIN								\
	CDEVSW_READ_LOCK(maj);						\
	DEVSW_FUNNEL(c,(maj));						\
	(ret) = (*cdevsw[(maj)].d_open)(dev,mode,flag,newdev,node);	\
	DEVSW_UNFUNNEL(c,(maj));					\
	CDEVSW_READ_UNLOCK(maj);					\
MACRO_END
#else
#define	CDEVSW_OPEN(maj,dev,mode,flag,newdev,ret)			\
MACRO_BEGIN								\
	CDEVSW_READ_LOCK(maj);						\
	DEVSW_FUNNEL(c,(maj));						\
	(ret) = (*cdevsw[(maj)].d_open)(dev,mode,flag,newdev);		\
	DEVSW_UNFUNNEL(c,(maj));					\
	CDEVSW_READ_UNLOCK(maj);					\
MACRO_END
#endif

#ifdef OSF1_ADFS

#define	BDEVSW_CLOSE(m,d,n,f,f2,r)	_DVC_(b,close,m,(d,n,f,f2),r)
#define	BDEVSW_IOCTL(m,d,n,c,d1,f,r)	_DVC_(b,ioctl,m,(d,n,c,d1,f),r)
#define	BDEVSW_PSIZE(m,d,n,r)		_DVC_OPT_(b,psize,m,(d,n),r)

#define	CDEVSW_CLOSE(m,d,n,f,mo,r)	_DVC_(c,close,m,(d,n,f,mo),r)
#define	CDEVSW_READ(m,d,n,u,f,r)	_DVC_(c,read,m,(d,n,u,f),r)
#define	CDEVSW_WRITE(m,d,n,u,f,r)       _DVC_(c,write,m,(d,n,u,f),r)
#define	CDEVSW_IOCTL(m,d,n,com,t,f,r)	_DVC_(c,ioctl,m,(d,n,com,t,f),r)
#define	CDEVSW_SELECT(m,d,n,e,re,s,r)	_DVC_(c,select,m,(d,e,re,s,n),r)

#else

#define	BDEVSW_CLOSE(m,d,f,f2,r)	_DVC_(b,close,m,(d,f,f2),r)
#define	BDEVSW_IOCTL(m,d,c,d1,f,r)	_DVC_(b,ioctl,m,(d,c,d1,f),r)
#define	BDEVSW_PSIZE(m,d,r)		_DVC_OPT_(b,psize,m,(d),r)

#define	CDEVSW_CLOSE(m,d,f,mo,r)	_DVC_(c,close,m,(d,f,mo),r)
#define	CDEVSW_READ(m,d,u,f,r)		_DVC_(c,read,m,(d,u,f),r)
#define	CDEVSW_WRITE(m,d,u,f,r)		_DVC_(c,write,m,(d,u,f),r)
#define	CDEVSW_IOCTL(m,d,com,t,f,r)	_DVC_(c,ioctl,m,(d,com,t,f),r)
#define	CDEVSW_SELECT(m,d,e,re,s,r)	_DVC_(c,select,m,(d,e,re,s),r)

#endif



#define	BDEVSW_STRATEGY(m,bp,r)		_DVC_(b,strategy,m,(bp),r)
#define	BDEVSW_DUMP(m,d,lo,s,bl,f,r)	_DVC_(b,dump,m,(d,lo,s,bl,f),r)
#define	BDEVSW_FLAGS(m,r)		_FLAGS_(b,m,r)

#define	CDEVSW_STOP(m,t,w,r)		_DVC_(c,stop,m,(t,w),r)
#define	CDEVSW_RESET(m,i,r)		_DVC_(c,reset,m,(i),r)
#define	CDEVSW_TTYS(maj,min,r)		_TTYS_(c,maj,min,r)
#define	CDEVSW_MMAP(m,d,o,p,r)		_DVC_(c,mmap,m,(d,o,p),r)

#endif	/* _KERNEL */
#endif	/* _SYS_CONF_H_ */
