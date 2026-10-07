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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: tty_subr.c,v $
 * Revision 1.8  1994/11/18  20:28:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/01/13  17:52:19  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	bsd/uipc_usrreq.c, bsd/uipc_syscalls.c, bsd/tty_subr.c
 * 	bsd/tty_compat.c, bsd/svipc_shm.c, bsd/svipc_sem.c
 * 	bsd/subr_select.c, bsd/mach_signal.c, bsd/mach_core.c
 * 	bsd/mach_clock.c, bsd/ldr_exec.c, bsd/kern_utctime.c
 * 	bsd/kern_time.c, bsd/kern_sig.c, bsd/kern_resource.c
 * 	bsd/kern_prot.c, bsd/kern_proc.c, bsd/kern_mman.c
 * 	bsd/kern_fork.c, bsd/kern_exit.c, bsd/kern_exec.c
 * 	bsd/kern_descrip.c, bsd/kern_acct.c, bsd/init_main.c
 * 	bsd/cmu_syscalls.c
 *
 * Revision 1.6  1993/07/14  17:50:19  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:50:55  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:07:09  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:41  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.1.2.1  1993/02/16  20:03:17  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.3  1993/01/22  16:55:20  nandy
 * 1/20/93 drop from LOCUS
 *
 * Revision 2.3  92/12/29  12:51:12  chrisp
 * Character blocks are now dynamically allocated. cinit() initializes
 * 	a zone and pre-allocates and initializes nclist blocks (as
 * 	specified in param.c). If the free list is empty, further
 * 	blocks are zalloc()ed.
 * 
 * Revision 1.1.2.1  1992/11/06  00:07:37  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  1992/12/29  12:51:12  chrisp
 * Character blocks are now dynamically allocated. cinit() initializes
 * 	a zone and pre-allocates and initializes nclist blocks (as
 * 	specified in param.c). If the free list is empty, further
 * 	blocks are zalloc()ed.
 *
 * Revision 2.2  91/08/31  13:23:57  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/05  13:56:20  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  13:20:15  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:02:24  gm]
 * 
 * 	Must clear quote bits when freeing cblocks.
 * 	Move TSPLTTY and friends to tty.h.
 * 	[90/09/28  20:17:13  brezak]
 * 
 * 	Eliminate extraneous dependency on <sys/buf.h>
 * 	[90/09/23  21:34:44  jeffc]
 * 
 * Revision 1.4  90/06/22  20:07:33  devrcs
 * 	Changed ndbq routine to correctly look for quoted characters in
 * 	the clist.
 * 	[90/06/08  12:57:04  havens]
 * 
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Parallelzed for OSF/1:  enabled clist /locking.		nags@encore.com
 * 	Fixed ndbq when looking for quoted characters.		havens@osf.org
 * 	Merged Robert Coren's and Rich Morris's changes.	ers@osf.org
 * 	Posix tty support.					morris@osf.org
 * 	Fixes for first snapshot.				gm@osf.org
 * 	Mach 2.5 and Encore 0.6 merge				gm@osf.org
 * 	Early Mach 2.5 merge with Encore parallelization.	alan@encore.com
 * 	MMAX_MP:  code clean-up.				alan@encore.com
 * 	MMAX_MP:  added cfreelist locking.			alan@encore.com
 * 	More cleanup.						rpd@encore.com
 * 	Changes for cleanup.					gm0w@cmu.edu
 * 	Code cleanup cataclysm.					mwyoung@cmu.edu
 * 	Changes for I386 & MIPS: byte order			rvb@cmu.edu
 * 	[90/06/12  21:45:40  gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)tty_subr.c	7.3 (Berkeley) 1/26/88
 */
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/clist.h>
#include <kern/zalloc.h>


static struct cblock zblock;
zone_t	cblock_zone;

#define setquote(cp) \
	setbit(((char *)((int)(cp)&~CROUND)+sizeof(struct cblock *)), \
		(int)(cp)&CROUND)
#define isquote(cp) \
	isset(((char *)((int)(cp)&~CROUND)+sizeof(struct cblock *)), \
		(int)(cp)&CROUND)
#define cbptr(x) ((struct cblock *)(x))

udecl_simple_lock_data(,cblock_freelist_lock)

#define	CFREELIST_LOCK()	usimple_lock(&cblock_freelist_lock)
#define	CFREELIST_UNLOCK()	usimple_unlock(&cblock_freelist_lock)

#define	CFREELIST_RETURN(cblock)				\
			CFREELIST_LOCK();			\
			*(cblock) = zblock;			\
			(cblock)->c_next = cfreelist;		\
			cfreelist = (cblock);			\
			cfreecount += CBSIZE;			\
			CFREELIST_UNLOCK();

#define	CFREELIST_FETCH(cblockp)				\
			CFREELIST_LOCK();			\
			if ((*(cblockp) = cfreelist) != NULL) {	\
				cfreelist = (*(cblockp))->c_next;\
				cfreecount -= CBSIZE;		\
				(*(cblockp))->c_next = NULL;	\
			} else					\
				*(cblockp) = cblock_new();	\
			CFREELIST_UNLOCK();

struct cblock *
cblock_new()
{
	register struct cblock *cp;
	register int ccp;

	cp = (struct cblock *) zalloc(cblock_zone);
	/* note: zalloc() panics if unable to return memory */
	nclist++;

	ccp = (int)cp;
	ccp = (ccp+CROUND) & ~CROUND;
	if ((int) cp != ccp)
		panic("cblock_new: cblock alignment error");

	*cp = zblock;

	return(cp);
}

/*
 * Initialize clist by freeing all character blocks, then count
 * number of character devices. (Once-only routine)
 */
cinit()
{
	register struct cblock *cp;
	u_int		nclist_conf;

	/*
	 * Initialize zone for the cblock table and record that
	 * the table is empty until zalloc() is called (in cblock_new()).
	 */
	cblock_zone = zinit(sizeof(struct cblock), 0,
			    nclist*sizeof(struct cblock), "character_blocks");
	if (cblock_zone == NULL)
		panic("cinit: unable to zinit cblock zone");
	nclist_conf = nclist;
	nclist = 0;

	cfreelist = NULL;
	for(; nclist_conf > 0; nclist_conf--) {
		cp = cblock_new();
		cp->c_next = cfreelist;
		cfreelist = cp;
		cfreecount += CBSIZE;
	}
}

/*
 * Character list get/put
 */
getc(p)
	register struct clist *p;
{
	register struct cblock *bp;
	register int c;
	TSPLVAR(s)

	TSPLTTY(s);
	if (p->c_cc <= 0) {
		c = -1;
		p->c_cc = 0;
		p->c_cf = p->c_cl = NULL;
	} else {
		c = *p->c_cf & 0377;
		if (isquote(p->c_cf))
			c |= TTY_QUOTE;
		p->c_cf++;
		if (--p->c_cc<=0) {
 			bp = cbptr(p->c_cf-1);
 			bp = cbptr((int)bp & ~CROUND);
			p->c_cf = NULL;
			p->c_cl = NULL;
			CFREELIST_RETURN (bp);
		} else if (((int)p->c_cf & CROUND) == 0){
 			bp = cbptr(p->c_cf);
			bp--;
			p->c_cf = bp->c_next->c_info;
			CFREELIST_RETURN(bp);
		}
	}
	TSPLX(s);
	return (c);
}

/*
 * copy clist to buffer.
 * return number of bytes moved.
 */
q_to_b(q, cp, cc)
	register struct clist *q;
	register char *cp;
{
	register struct cblock *bp;
	register nc;
	char *acp;
	TSPLVAR(s)

	if (cc <= 0)
		return (0);
	TSPLTTY(s);
	if (q->c_cc <= 0) {
		q->c_cc = 0;
		q->c_cf = q->c_cl = NULL;
		TSPLX(s);
		return (0);
	}
	acp = cp;

	while (cc) {
		nc = sizeof (struct cblock) - ((int)q->c_cf & CROUND);
		nc = MIN(nc, cc);
		nc = MIN(nc, q->c_cc);
		(void) bcopy(q->c_cf, cp, (unsigned)nc);
		q->c_cf += nc;
		q->c_cc -= nc;
		cc -= nc;
		cp += nc;
		if (q->c_cc <= 0) {
 			bp = cbptr(q->c_cf - 1);
 			bp = cbptr((int)bp & ~CROUND);
			q->c_cf = q->c_cl = NULL;
			CFREELIST_RETURN(bp);
			break;
		}
		if (((int)q->c_cf & CROUND) == 0) {
 			bp = cbptr(q->c_cf);
			bp--;
			q->c_cf = bp->c_next->c_info;
			CFREELIST_RETURN(bp);
		}
	}
	TSPLX(s);
	return (cp-acp);
}

/*
 * Return count of contiguous characters
 * in clist starting at q->c_cf.
 * Stop counting if flag&character is non-null.
 */
ndqb(q, flag)
	register struct clist *q;
{
	register cc;
	TSPLVAR(s)

	TSPLTTY(s);
	if (q->c_cc <= 0) {
		cc = -q->c_cc;
		goto out;
	}
	cc = ((int)q->c_cf + CBSIZE) & ~CROUND;
	cc -= (int)q->c_cf;
	if (q->c_cc < cc)
		cc = q->c_cc;
	if (flag) {
		register char *p, *end;

		p = q->c_cf;
		end = p;
		end += cc;
		while (p < end) {
			if ( isquote(p) ) {
				cc = (int)p;
				cc -= (int)q->c_cf;
				break;
			}
			p++;
		}
	}
out:
	TSPLX(s);
	return (cc);
}

/*
 * Flush cc bytes from q.
 */
ndflush(q, cc)
	register struct clist *q;
	register cc;
{
	register struct cblock *bp;
	char *end;
	int rem;
	TSPLVAR(s)

	TSPLTTY(s);
	if (q->c_cc <= 0)
		goto out;
	while (cc>0 && q->c_cc) {
 		bp = cbptr((int)q->c_cf & ~CROUND);
		if ((int)bp == (((int)q->c_cl-1) & ~CROUND)) {
			end = q->c_cl;
		} else {
			end = (char *)((int)bp + sizeof (struct cblock));
		}
		rem = end - q->c_cf;
		if (cc >= rem) {
			cc -= rem;
			q->c_cc -= rem;
			q->c_cf = bp->c_next->c_info;
			CFREELIST_RETURN(bp);
		} else {
			q->c_cc -= cc;
			q->c_cf += cc;
			if (q->c_cc <= 0) {
				CFREELIST_RETURN(bp);
			}
			break;
		}
	}
	if (q->c_cc <= 0) {
		q->c_cf = q->c_cl = NULL;
		q->c_cc = 0;
	}
out:
	TSPLX(s);
}


putc(c, p)
	register struct clist *p;
{
	struct cblock *bp;
	register char *cp;
	TSPLVAR(s)

	TSPLTTY(s);
 	if ((cp = p->c_cl) == NULL || p->c_cc < 0 ) {	/* no cblocks yet */
		CFREELIST_FETCH(&bp);
		if (bp == NULL)
			return -1;
		p->c_cf = cp = bp->c_info;
	} else if (((int)cp & CROUND) == 0) {
 		bp = cbptr(cp) - 1;	/* pointer arith */
		CFREELIST_FETCH(&bp->c_next);
		if (bp->c_next == NULL)
			return -1;
		cp = bp->c_next->c_info;
	}
 	if (c&TTY_QUOTE)
 		setquote(cp);
	*cp++ = c;
	p->c_cc++;
	p->c_cl = cp;
	TSPLX(s);
	return (0);
}

/*
 * copy buffer to clist.
 * return number of bytes not transfered.
 */
b_to_q(cp, cc, q)
	register char *cp;
	struct clist *q;
	register int cc;
{
	register char *cq;
	struct cblock *bp;
	register nc;
	int acc;
	TSPLVAR(s)

	if (cc <= 0)
		return (0);
	acc = cc;
	TSPLTTY(s);
	if ((cq = q->c_cl) == NULL || q->c_cc < 0) {
		CFREELIST_FETCH(&bp);
		if (bp == NULL)
			goto out;
		q->c_cf = cq = bp->c_info;
	}

	while (cc) {
		if (((int)cq & CROUND) == 0) {
 			bp = cbptr(cq) - 1;
			CFREELIST_FETCH(&bp->c_next);
			if (bp->c_next == NULL)
				goto out;
			cq = bp->c_next->c_info;
		}
		nc = MIN(cc, sizeof (struct cblock) - ((int)cq & CROUND));
		(void) bcopy(cp, cq, (unsigned)nc);
		cp += nc;
		cq += nc;
		cc -= nc;
	}
out:
	q->c_cl = cq;
	q->c_cc += acc - cc;
	TSPLX(s);
	return (cc);
}

/*
 * Given a non-NULL pointter into the list (like c_cf which
 * always points to a real character if non-NULL) return the pointer
 * to the next character in the list or return NULL if no more chars.
 *
 * Callers must not allow getc's to happen between nextc's so that the
 * pointer becomes invalid.  Note that interrupts are NOT masked.
 */
char *
nextc(p, cp, c)
	register struct clist *p;
	register char *cp;
 	register int *c;
{

	if (p->c_cc && ++cp != p->c_cl) {
 		if (((int)cp & CROUND) == 0) {
 			cp = (cbptr(cp))[-1].c_next->c_info;
 		}
 		*c = *cp;
 		if (isquote(cp))
 			*c |= TTY_QUOTE;
		return (cp);
	}
	return (0);
}

/*
 * Remove the last character in the list and return it.
 */
unputc(p)
	register struct clist *p;
{
	struct cblock *bp;
	register int c;
	TSPLVAR(s)

	TSPLTTY(s);
	if (p->c_cc <= 0)
		c = -1;
	else {
		c = *--p->c_cl;
		if (isquote(p->c_cl))
			c |= TTY_QUOTE;
		if (--p->c_cc <= 0) {
 			bp = cbptr(p->c_cl);
 			bp = cbptr((int)bp & ~CROUND);
			p->c_cl = p->c_cf = NULL;
			/*
			 * Worry:  why aren't we checking cwaiting
			 * in the original code?  The new code will
			 * wake up cblock waiters.
			 */
			CFREELIST_RETURN(bp);
 		} else if (p->c_cl == (cbptr((int)p->c_cl & ~CROUND))->c_info) {
			p->c_cl = (char *)((int)p->c_cl & ~CROUND);
 			bp = cbptr(p->c_cf);
 			bp = cbptr((int)bp & ~CROUND);
 			while (bp->c_next != cbptr(p->c_cl))
				bp = bp->c_next;
			p->c_cl = (char *)(bp + 1);
			CFREELIST_RETURN(bp->c_next);
			bp->c_next = NULL;
		}
	}
	TSPLX(s);
	return (c);
}

/*
 * Put the chars in the from que
 * on the end of the to que.
 */
catq(from, to)
	struct clist *from, *to;
{
#ifdef notdef
	char bbuf[CBSIZE*4];
#endif
	register c;
	TSPLVAR(s)

	TSPLTTY(s);
	if (to->c_cc == 0) {
		*to = *from;
		from->c_cc = 0;
		from->c_cf = NULL;
		from->c_cl = NULL;
		TSPLX(s);
		return;
	}
	TSPLX(s);
#ifdef notdef
	while (from->c_cc > 0) {
		c = q_to_b(from, bbuf, sizeof bbuf);
		(void) b_to_q(bbuf, c, to);
	}
#endif
 	/* XXX - FIX */
 	while ((c = getc(from)) >= 0)
 		putc(c, to);
}

#ifdef	unneeded
/*
 * Integer (short) get/put using clists.
 */
typedef	u_short word_t;

getw(p)
	register struct clist *p;
{
	register int c;
	register struct cblock *bp;
	TSPLVAR(s)

	if (p->c_cc <= 1)
		return(-1);
	if (p->c_cc & 01) {
		c = getc(p);
#if	BYTE_MSF
		return (getc(p) | (c<<8));
#else
		return (c | (getc(p)<<8));
#endif
	}
	TSPLTTY(s);
#if	BYTE_MSF
	c = (((u_char *)p->c_cf)[1] << 8) | ((u_char *)p->c_cf)[0];
#else
 	c = (((u_char *)p->c_cf)[0] << 8) | ((u_char *)p->c_cf)[1];
#endif
	p->c_cf += sizeof (word_t);
	p->c_cc -= sizeof (word_t);
	if (p->c_cc <= 0) {
 		bp = cbptr(p->c_cf-1);
 		bp = cbptr((int)bp & ~CROUND);
		p->c_cf = NULL;
		p->c_cl = NULL;
		bp->c_next = cfreelist;
		cfreelist = bp;
		cfreecount += CBSIZE;
		if (cwaiting) {
			wakeup(&cwaiting);
			cwaiting = 0;
		}
	} else if (((int)p->c_cf & CROUND) == 0) {
 		bp = cbptr(p->c_cf);
		bp--;
		p->c_cf = bp->c_next->c_info;
		bp->c_next = cfreelist;
		cfreelist = bp;
		cfreecount += CBSIZE;
		if (cwaiting) {
			wakeup(&cwaiting);
			cwaiting = 0;
		}
	}
	TSPLX(s);
	return (c);
}

putw(c, p)
	register struct clist *p;
	word_t c;
{
	register struct cblock *bp;
	register char *cp;
	TSPLVAR(s)

	TSPLTTY(s);
	if (cfreelist==NULL) {
		TSPLX(s);
		return(-1);
	}
	if (p->c_cc & 01) {
#if	BYTE_MSF
		(void) putc(c>>8, p);
		(void) putc(c, p);
#else
		(void) putc(c, p);
		(void) putc(c>>8, p);
#endif
	} else {
		if ((cp = p->c_cl) == NULL || p->c_cc < 0 ) {
			if ((bp = cfreelist) == NULL) {
				TSPLX(s);
				return (-1);
			}
			cfreelist = bp->c_next;
			cfreecount -= CBSIZE;
			bp->c_next = NULL;
			p->c_cf = cp = bp->c_info;
		} else if (((int)cp & CROUND) == 0) {
 			bp = cbptr(cp) - 1;
			if ((bp->c_next = cfreelist) == NULL) {
				TSPLX(s);
				return (-1);
			}
			bp = bp->c_next;
			cfreelist = bp->c_next;
			cfreecount -= CBSIZE;
			bp->c_next = NULL;
			cp = bp->c_info;
		}
#if	BYTE_MSF
		((u_char *)cp)[0] = c>>8;
		((u_char *)cp)[1] = c;
#else
		*(word_t *)cp = c;
#endif
		p->c_cl = cp + sizeof (word_t);
		p->c_cc += sizeof (word_t);
	}
	TSPLX(s);
	return (0);
}
#endif	/* unneeded */
