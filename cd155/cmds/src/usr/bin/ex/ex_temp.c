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
#if !defined(lint) && !defined(_NOIDENT)
static char *rcsid = "@(#)$RCSfile: ex_temp.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:24:05 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex_temp.c
 *
 * FUNCTION: KILLreg, REGblk, TSYNC, YANKline, YANKreg, blkio, cleanup,
 * fileinit, getREG, getblock, getline, kshift, mapreg, notpart, partreg,
 * putline, putreg, rbflush, regbuf, regio, shread, synctmp, tflush, tlaste
 *
 * ORIGINS: 3, 10, 13, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * ex_temp.c  1.10  com/cmd/edit/vi,3.1,9013 3/13/90 10:39:04
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#include "ex.h"
#include "ex_temp.h"
#include "ex_vis.h"
#include "ex_tty.h"

/*
 * Editor temporary file routines.
 * Very similar to those of ed, except uses 2 input buffers.
 */
#define READ	0
#define WRITE	1


static blkio();
static shread();
static getREG();
static kshift();
static rbflush();

static	NLchar	*getblock();

static	char	tfname[40];
static	char	rfname[40];
static	int	havetmp;
short tfile = -1;


static	short	rfile = -1;
/*
 * Vi insists on putting one 'NULL' line in the buffer (see fixzero()).
 * This means that for the most common case (entering vi with a filename),
 * the tempfile gets created, the NULL line is added, and then the file
 * you're editing gets loaded.  Since the tmpfile is not 'empty', it gets
 * deleted and recreated.  If the /tmp directory is big, this may take 
 * some time, so we make a special check for this case.
 *	if (tline == INCRMT * (HBLKS+2))
 *		return;
 */

fileinit()
{
	register char *p;
	register int i, j;
	struct stat stbuf;

	if (tline == INCRMT * (HBLKS+2))
		return;
	cleanup(0);
	close(tfile);
	tline = INCRMT * (HBLKS+2);
	blocks[0] = HBLKS;
	blocks[1] = HBLKS+1;
	blocks[2] = -1;
	dirtcnt = 0;
	iblock = -1;
	iblock2 = -1;
	oblock = -1;
	strcpy(tfname, svalue(DIRECTORY));
	if (stat(tfname, &stbuf)) {
dumbness:
		if (setexit() == 0)
			filioerr(tfname);
		else
			putNFL();
		cleanup(1);
		exit(1);
	}
	if ((stbuf.st_mode & S_IFMT) != S_IFDIR) {
		errno = ENOTDIR;
		goto dumbness;
	}
	ichanged = 0;
	ichang2 = 0;
	ignore(strcat(tfname, "/ExXXXXX"));
	for (p = strend(tfname), i = 5, j = getpid(); i > 0; i--, j /= 10)
		*--p = j % 10 | '0';
	tfile = creat(tfname, 0600);
	if (tfile < 0)
		goto dumbness;
	{
		extern stilinc; 	/* see below */
		stilinc = 0;
	}
	havetmp = 1;
	close(tfile);
	tfile = open(tfname, 2);
	if (tfile < 0)
		goto dumbness;
}

cleanup(all)
	short all;
{
	extern char termtype[];
	if (all) {
		putpad(exit_ca_mode);
		flush();
		resetterm();
		normtty--;
	}
	if (havetmp)
		unlink(tfname);
	havetmp = 0;
	if (all && rfile >= 0) {
		unlink(rfname);
		close(rfile);
		rfile = -1;
	}
	if (all == 1)
		exit(0);
}


getline(tl)
	line tl;
{
	register NLchar *bp, *lp;
	register int nl;

	lp = linebuf;
	bp = getblock(tl, READ);
	nl = nleft;
	tl &= ~OFFMSK;
	while (*lp++ = *bp++)
		if (--nl == 0) {
			bp = getblock(tl += INCRMT, READ);
			nl = nleft;
		}
}

putline()
{
	register NLchar *bp, *lp;
	register int nl;
	line tl;

	dirtcnt++;
	lp = linebuf;
	change();
	tl = tline;
	bp = getblock(tl, WRITE);
	nl = nleft;
	tl &= ~OFFMSK;
	while (*bp = *lp++) {
		if (*bp++ == '\n') {
			*--bp = 0;
			linebp = lp;
			break;
		}
		if (--nl == 0) {
			bp = getblock(tl += INCRMT, WRITE);
			nl = nleft;
		}
	}
	tl = tline;
	tline += (((lp - linebuf) + BNDRY - 1) >> SHFT) & 077776;
	return (tl);
}

int	read();
int	write();

/* read and write N-NLchars * sizeof(NLchar) bytes */
static NLchar * getblock(atl, iof)
	line atl;
	int iof;
{
	static int bno, off;

	bno = (atl >> OFFBTS) & BLKMSK;
	off = (atl << SHFT) & LBTMSK;
	if (bno >= NMBLKS)
		error(msg(M_194, " Tmp file too large"));
	nleft = BUFFERSIZ - off;
	if (bno == iblock) {
		ichanged |= iof;
		hitin2 = 0;
		return (ibuff + off);
	}
	if (bno == iblock2) {
		ichang2 |= iof;
		hitin2 = 1;
		return (ibuff2 + off);
	}
	if (bno == oblock)
		return (obuff + off);
	if (iof == READ) {
		if (hitin2 == 0) {
			if (ichang2) {
				blkio(iblock2, ibuff2, write);
			}
			ichang2 = 0;
			iblock2 = bno;
			blkio(bno, ibuff2, read);
			hitin2 = 1;
			return (ibuff2 + off);
		}
		hitin2 = 0;
		if (ichanged) {
			blkio(iblock, ibuff, write);
		}
		ichanged = 0;
		iblock = bno;
		blkio(bno, ibuff, read);
		return (ibuff + off);
	}
	if (oblock >= 0) {
			blkio(oblock, obuff, write);
	}
	oblock = bno;
	return (obuff + off);
}

#define INCORB	64
NLchar	incorb[INCORB+1][BUFFERSIZ];
#define pagrnd(a)	((char *)(((int)a)&~(BUFBYTES-1)))
int	stilinc = 0;	/* up to here not written yet */

static blkio(b, buf, iofcn)
	short b;
	NLchar *buf;
	int (*iofcn)();
{

	if (b < INCORB) {
		if (iofcn == read) {
			copy(buf, pagrnd(incorb[b+1]), BUFBYTES);
			return;
		}
		copy(pagrnd(incorb[b+1]), buf, BUFBYTES);
		if (laste) {
			if (b >= stilinc)
				stilinc = b + 1;
			return;
		}
	} else if (stilinc)
		tflush();
	lseek(tfile, (long) (unsigned) b * BUFBYTES, 0);
	if ((*iofcn)(tfile, (char *) buf, BUFBYTES) != BUFBYTES)
		filioerr(tfname);
}

tlaste()
{

	if (stilinc)
		dirtcnt = 0;
}


tflush()
{
	int i = stilinc;

	stilinc = 0;
	lseek(tfile, (long) 0, 0);
	if (write(tfile, pagrnd(incorb[1]), i * BUFBYTES) != (i * BUFBYTES))
		filioerr(tfname);
}
/*
 * Synchronize the state of the temporary file in case
 * a crash occurs.
 */
synctmp()
{
	register int cnt;
	register line *a;
	register short *bp;
	if (stilinc)
		return;
	if (dol == zero)
		return;
	/*
	 * In theory, we need to encrypt iblock and iblock2 before writing
	 * them out, as well as oblock, but in practice ichanged and ichang2
	 * can never be set, so this isn't really needed.  Likewise, the
	 * code in getblock above for iblock+iblock2 isn't needed.
	 */
	if (ichanged)
		blkio(iblock, ibuff, write);
	ichanged = 0;
	if (ichang2)
		blkio(iblock2, ibuff2, write);
	ichang2 = 0;
	if (oblock != -1)
		blkio(oblock, obuff, write);
	time(&H.Time);
	uid = getuid();
	*zero = (line) H.Time;
	for (a = zero, bp = blocks; a <= dol; a += BUFBYTES / sizeof *a, bp++) {
		if (*bp < 0) {
			tline = (tline + OFFMSK) &~ OFFMSK;
			*bp = ((tline >> OFFBTS) & BLKMSK);
			if (*bp > NMBLKS)
				error(msg(M_194, " Tmp file too large"));
			tline += INCRMT;
			oblock = *bp + 1;
			bp[1] = -1;
		}
		lseek(tfile, (long) (unsigned) *bp * BUFBYTES, 0);
		cnt = ((dol - a) + 2) * sizeof (line);
		if (cnt > BUFBYTES)
			cnt = BUFBYTES;
		if (write(tfile, (char *) a, cnt) != cnt) {
oops:
			*zero = 0;
			filioerr(tfname);
		}
		*zero = 0;
	}
	flines = lineDOL();
	lseek(tfile, 0l, 0);
	if (write(tfile, (char *) &H, sizeof H) != sizeof H)
		goto oops;
}

TSYNC()
{

	if (dirtcnt > MAXDIRT) {	/* mjm: 12 --> MAXDIRT */
		if (stilinc)
			tflush();
		dirtcnt = 0;
		synctmp();
	}
}

/*
 * Named buffer routines.
 * These are implemented differently than the main buffer.
 * Each named buffer has a chain of blocks in the register file.
 * Each read and write operation is in BUFBYTES chunks.
 * Each block contains roughly
 *		    BUFBYTES - sizeof(short) - sizeof(short)
 * NLchars of text
 *		[8190 NLchars with BUFBYTES == 16384 and sizeof(short) == 2],
 * and a previous and next block number.  We also have information
 * about which blocks came from deletes of multiple partial lines,
 * e.g. deleting a sentence or a LISP object.
 *
 * We maintain a free map for the temp file.  To free the blocks
 * in a register we must read the blocks to find how they are chained
 * together.
 *
 * The register routines take an NLchar as argument but support only
 * 'a'-'z' and '0'-'9' indices.  See cmdreg() in ex_cmds2.c which restricts
 * the range of a register name to ascii, alphabetic characters.
 *
 * BUG: 	The default savind of deleted lines in numbered
 *		buffers may be rather inefficient; it hasn't been profiled.
 */
struct	strreg {
	short	rg_flags;
	short	rg_nleft;
	short	rg_first;
	short	rg_last;
} strregs[('z'-'a'+1) + ('9'-'0'+1)], *strp;

/* sizeof(rbuf) == BUFBYTES, as used in regio() for read and write */
static struct	rbuf {
	short	rb_prev;
	short	rb_next;
	/* sizeof(rb_text) == sizeof(rbuf) - sizeof(first two fields) */
	NLchar	rb_text[BUFFERSIZ
		- ((sizeof (short) + sizeof(short)) / sizeof(NLchar))];
} *rbuf, KILLrbuf, putrbuf, YANKrbuf, regrbuf;
short	rused[256];
static short	rnleft;
static short	rblock;
static short	rnext;
static NLchar	*rbufcp;

regio(b, iofcn)
	short b;
	int (*iofcn)();
{

	if (rfile == -1) {
		strcpy(rfname, tfname);
		*(strend(rfname) - 7) = 'R';
		rfile = creat(rfname, 0600);
		if (rfile < 0)
oops:
			filioerr(rfname);
		close(rfile);
		rfile = open(rfname, 2);
		if (rfile < 0)
			goto oops;
	}
	lseek(rfile, (long) b * BUFBYTES, 0);
	if ((*iofcn)(rfile, rbuf, BUFBYTES) != BUFBYTES)
		goto oops;
	rblock = b;
}


static REGblk()
{
	register int i, j, m;

	for (i = 0; i < sizeof rused / sizeof rused[0]; i++) {
		m = (rused[i] ^ 0177777) & 0177777;
		if (i == 0)
			m &= ~1;
		if (m != 0) {
			j = 0;
			while ((m & 1) == 0)
				j++, m >>= 1;
			rused[i] |= (1 << j);
#ifdef RDEBUG
			printf("allocating block %d\n", i * 16 + j);
#endif
			return (i * 16 + j);
		}
	}
	error(msg(M_196, "Out of register space (ugh)"));
	/*NOTREACHED*/
}

static struct	strreg * mapreg(c)
	register int c;
{
	if (isupper(c))
		c = tolower(c);
	return (isdigit(c) ? &strregs[('z'-'a'+1)+(c-'0')] : &strregs[c-'a']);
}

int	shread();

static KILLreg(c)
	register int c;
{
	register struct strreg *sp;

	rbuf = &KILLrbuf;
	sp = mapreg(c);
	rblock = sp->rg_first;
	sp->rg_first = sp->rg_last = 0;
	sp->rg_flags = sp->rg_nleft = 0;
	while (rblock != 0) {
#ifdef RDEBUG
		printf("freeing block %d\n", rblock);
#endif
		rused[rblock / 16] &= ~(1 << (rblock % 16));
		regio(rblock, shread);
		rblock = rbuf->rb_next;
	}
}

/*VARARGS*/
static shread()
{
	struct front { short a; short b; };

	if (read(rfile, (char *) rbuf, sizeof (struct front)) == sizeof (struct front))
		return (sizeof (struct rbuf));
	return (0);
}


int	getREG();
putreg(c)
	NLchar c;
{
	register line *odot = dot;
	register line *odol = dol;
	register int cnt;

	deletenone();
	appendnone();
	rbuf = &putrbuf;
	rnleft = 0;
	rblock = 0;
	rnext = mapreg(c)->rg_first;
	if (rnext == 0) {
		if (inopen) {
			splitw++;
			vclean();
			vgoto(WECHO, 0);
		}
		vreg = -1;
		error(msg(M_197, "Nothing in register %c"), c);
	}
	if (inopen && partreg(c)) {
		if (!FIXUNDO) {
			splitw++; vclean(); vgoto(WECHO, 0); vreg = -1;
			error(msg(M_198, "Can't put partial line inside macro"));
		}
		squish();
		addr1 = addr2 = dol;
	}
	cnt = append(getREG, addr2);
	if (inopen && partreg(c)) {
		unddol = dol;
		dol = odol;
		dot = odot;
		pragged(0);
	}
	killcnt(cnt);
	notecnt = cnt;
}

partreg(c)
	NLchar c;
{

	return (mapreg(c)->rg_flags);
}

notpart(c)
	register NLchar c;
{

	if (c)
		mapreg(c)->rg_flags = 0;
}

static getREG()
{
	register NLchar *lp = linebuf;
	register NLchar c;

	for (;;) {
		if (rnleft == 0) {
			if (rnext == 0)
				return (EOF);
			regio(rnext, read);
			rnext = rbuf->rb_next;
			rbufcp = rbuf->rb_text;
			rnleft = sizeof rbuf->rb_text / sizeof(NLchar);
		}
		c = *rbufcp;
		if (c == 0)
			return (EOF);
		rbufcp++, --rnleft;
		if (c == '\n') {
			*lp++ = 0;
			return (0);
		}
		*lp++ = c;
	}
}

YANKreg(c)
	register int c;
{
	register line *addr;
	register struct strreg *sp;
	NLchar savelb[LBSIZE];

	if (isdigit(c))
		kshift();
	if (islower(c))
		KILLreg(c);
	strp = sp = mapreg(c);
	sp->rg_flags = inopen && cursor && wcursor;
	rbuf = &YANKrbuf;
	if (sp->rg_last) {
		regio(sp->rg_last, read);
		rnleft = sp->rg_nleft;
		rbufcp = &rbuf->rb_text[sizeof(rbuf->rb_text) / sizeof(NLchar)
					 - rnleft];
	} else {
		rblock = 0;
		rnleft = 0;
	}
	CP(savelb,linebuf);
	for (addr = addr1; addr <= addr2; addr++) {
		getline(*addr);
		if (sp->rg_flags) {
			if (addr == addr2)
				*wcursor = 0;
			if (addr == addr1)
				NCstrcpy(linebuf, cursor);
		}
		YANKline();
	}
	rbflush();
	if (strcmp(Command, "delete") != 0)
		killed();
	CP(linebuf,savelb);
}

static kshift()
{
	register int i;

	KILLreg('9');
	for (i = '8'; i >= '0'; i--)
		copy(mapreg(i+1), mapreg(i), sizeof (struct strreg));
}

YANKline()
{
	register NLchar *lp = linebuf;
	register struct rbuf *rp = rbuf;
	register int c;

	do {
		c = *lp++;
		if (c == 0)
			c = '\n';
		if (rnleft == 0) {
			rp->rb_next = REGblk();
			rbflush();
			rblock = rp->rb_next;
			rp->rb_next = 0;
			rp->rb_prev = rblock;
			rnleft = sizeof rp->rb_text / sizeof (NLchar);
			rbufcp = rp->rb_text;
		}
		*rbufcp++ = c;
		--rnleft;
	} while (c != '\n');
	if (rnleft)
		*rbufcp = 0;
}

static rbflush()
{
	register struct strreg *sp = strp;

	if (rblock == 0)
		return;
	regio(rblock, write);
	if (sp->rg_first == 0)
		sp->rg_first = rblock;
	sp->rg_last = rblock;
	sp->rg_nleft = rnleft;
}

/* Register c to NLchar buffer buf of size buflen NLchar's */
regbuf(c, buf, buflen)
NLchar c;
NLchar *buf;
int buflen;
{
	register NLchar *p, *lp;

	rbuf = &regrbuf;
	rnleft = 0;
	rblock = 0;
	rnext = mapreg(c)->rg_first;
	if (rnext==0) {
		*buf = 0;
		error(msg(M_199, "Nothing in register %c"),c);
	}
	p = buf;
	while (getREG()==0) {
		for (lp=linebuf; *lp;) {
			if (p >= &buf[buflen])
				error(msg(M_200, "Register too long@to fit in memory"));
			*p++ = *lp++;
		}
		*p++ = '\n';
	}
	if (partreg(c)) p--;
	*p = '\0';
	getDOT();
}

