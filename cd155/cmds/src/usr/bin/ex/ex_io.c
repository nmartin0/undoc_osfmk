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
static char *rcsid = "@(#)$RCSfile: ex_io.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:23:59 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex_io.c
 *
 * FUNCTION: checkmodeline, clrstats, edfile, filename, getargs, getfile,
 * getone, glob, gscan, iostats, putfile, rop, rop2, rop3, samei, source, wop,
 * wrerror
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
 * ex_io.c  1.14  com/cmd/edit/vi,3.1,9013 3/2/90 11:47:26
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#include "ex.h"
#include "ex_argv.h"
#include "ex_temp.h"
#include "ex_tty.h"
#include "ex_vis.h"
#include <sys/file.h>


/*
 * File input/output, source, preserve and recover
 */

static short iostats();
static void wrerror();
static int checkmodeline();

/*
 * Following remember where . was in the previous file for return
 * on file switching.
 */
static int	altdot;
static int	oldadot;
static short	wasalt;
short	isalt;
static long	cntch;		/* Count of characters on unit io */
static long	cntnull;	/* Count of nulls " */
static int	cntln;		/* Count of lines " */

#ifdef  FLOCKFILE
/*
 * The alternate, saved and current file are locked the extent of the
 * time that they are active. If the saved file is exchanged
 * with the alternate file, the file descriptors are exchanged
 * and the lock is not released.
 */
int     io_savedfile, io_altfile, io_curr ;
int     lock_savedfile, lock_altfile, lock_curr ;
#endif	/* FLOCKFILE */

/*
 * Parse file name for command encoded by comm.
 * If comm is E then command is doomed and we are
 * parsing just so user won't have to retype the name.
 */
filename(comm)
	int comm;
{
	register int c = comm, d;
	register int i;
#ifdef  FLOCKFILE
	int lock ;

	lock = 0 ;
#endif	/* FLOCKFILE */

	d = ex_getchar();
	if (endcmd(d)) {
		if (savedfile[0] == 0 && comm != 'f')
			error(msg(M_069, "No file|No current filename"));
		strcpy(file, savedfile);
#ifdef  FLOCKFILE
		if (io_curr && io_curr != io_savedfile) close(io_curr) ;
		lock = lock_curr = lock_savedfile ;
		io_curr = io_savedfile ;
#endif	/* FLOCKFILE */
		wasalt = (isalt > 0) ? isalt-1 : 0;
		isalt = 0;
		oldadot = altdot;
		if (c == 'e' || c == 'E')
			altdot = lineDOT();
		if (d == EOF)
			ungetchar(d);
	} else {
		ungetchar(d);
		getone();
		eol();
		if (savedfile[0] == 0 && c != 'E' && c != 'e') {
			c = 'e';
			edited = 0;
		}
		wasalt = strcmp(file, altfile) == 0;
		oldadot = altdot;
		switch (c) {

		case 'f':
			edited = 0;
			/* fall into ... */

		case 'e':
			if (savedfile[0]) {
#ifdef  FLOCKFILE
				if (strcmp(file,savedfile) == 0) break ;
#endif	/* FLOCKFILE */
				altdot = lineDOT();
				strcpy(altfile, savedfile);
#ifdef  FLOCKFILE
				if (io_altfile) close (io_altfile) ;
				io_altfile = io_savedfile ;
				lock_altfile = lock_savedfile ;
				io_savedfile = 0 ;
#endif	/* FLOCKFILE */
			}
			strcpy(savedfile, file);
#ifdef  FLOCKFILE
			io_savedfile = io_curr ;
			lock_savedfile = lock_curr ;
			io_curr = 0 ;           lock = lock_curr = 0 ;
#endif	/* FLOCKFILE */
			break;

		default:
			if (file[0]) {
#ifdef  FLOCKFILE
				if (wasalt) break ;
#endif
				if (c != 'E')
					altdot = lineDOT();
				strcpy(altfile, file);
#ifdef  FLOCKFILE
				if (io_altfile
				&& io_altfile != io_curr) close (io_altfile) ;
				io_altfile = io_curr ;
				lock_altfile = lock_curr ;
				io_curr = 0 ;           lock = lock_curr = 0 ;
#endif	/* FLOCKFILE */
			}
			break;
		}
	}
	if (hush && comm != 'f' || comm == 'E')
		return;
	if (file[0] != 0) {
		lprintf("\"%s\"", file);
		if (comm == 'f') {
			if (value(READONLY))
				ex_printf(msg(M_070, " [Read only]"));
			if (!edited)
				ex_printf(msg(M_071, " [Not edited]"));
			if (tchng)
				ex_printf(msg(M_072, " [Modified]"));
#ifdef  FLOCKFILE
			if (lock == LOCK_SH)
				ex_printf(msg(M_298, " [Shared lock]"));
			else if (lock == LOCK_EX)
				ex_printf(msg(M_299, " [Exclusive lock]"));
#endif	/* FLOCKFILE */
		}
		flush();
	} else
		ex_printf(msg(M_073, "No file "));
	if (comm == 'f') {
		if (!(i = lineDOL()))
			i++;
		ex_printf(msg(M_074, " line %d of %d --%ld%%--"), lineDOT(), lineDOL(),
		    (long) 100 * lineDOT() / i);
	}
}

/*
 * Get the argument words for a command into genbuf
 * expanding # and %.
 */
getargs()
{
	register int c;
	register NLchar *cp;
	register char *fp;		/* filename pointer */
	static NLchar fpatbuf[32];	/* hence limit on :next +/pat */

	pastwh();
	if (peekchar() == '+') {
		for (cp = fpatbuf;;) {
			*cp++ = c = ex_getchar();
			if (cp >= &fpatbuf[NCSIZE(fpatbuf)])
				error(msg(M_075, "Pattern too long"));
			if (c == '\\' && NCisspace(peekchar()))
				c = ex_getchar();
			if (c == EOF || NCisspace(c)) {
				ungetchar(c);
				*--cp = 0;
				firstpat = &fpatbuf[1];
				break;
			}
		}
	}
	if (skipend())
		return (0);
	NCdecstr("echo ", genbuf,6); cp = &genbuf[5];
	for (;;) {
		c = ex_getchar();
		if (endcmd(c)) {
			ungetchar(c);
			break;
		}
		switch (c) {

		case '\\':
			if (any(peekchar(), "#%|"))
				c = ex_getchar();
			/* fall into... */

		default:
			if (cp > &genbuf[LBSIZE - 2])
flong:
				error(msg(M_076, "Argument buffer overflow"));
			*cp++ = c;
			break;

		case '#':
			fp = altfile;
			if (*fp == 0)
				error(msg(M_077, "No alternate filename@to substitute for #"));
			goto filexp;

		case '%':
			fp = savedfile;
			if (*fp == 0)
				error(msg(M_078, "No current filename@to substitute for %%"));
filexp:
			while (*fp) {
				if (cp > &genbuf[LBSIZE - 2])
					goto flong;
				*cp++ = NLsgetc(fp);
			}
			break;
		}
	}
	*cp = 0;
	return (1);
}

/*
 * Glob the argument words in genbuf, or if no globbing
 * is implied, just split them up directly.
 */
glob(gp)
	struct glob *gp;
{
	int pvec[2];
	register char **argv = gp->argv;
	register char *cp = gp->argspac;
	register int c;
	char ch[2];
	int nleft = NCARGS;

	gp->argc0 = 0;
	if (gscan() == 0) {
		register NLchar *v = genbuf + 5;	  /* strlen("echo ") */

		for (;;) {
			while (NCisspace(*v))
				v++;
			if (!*v)
				break;
			*argv++ = cp;
			while (*v && !NCisspace(*v)) {
				NLsputc(cp, *v);
				v++;
			}
			*cp++ = 0;
			gp->argc0++;
		}
		*argv = 0;
		return;
	}
	if (pipe(pvec) < 0)
		error(msg(M_079, "Can't make pipe to glob"));
	pid = fork();
	io = pvec[0];
	if (pid < 0) {
		close(pvec[1]);
		error(msg(M_080, "Can't fork to do glob"));
	}
	if (pid == 0) {
		int oerrno;
		char tmpbuf[sizeof(genbuf)];

		close(1);
		dup(pvec[1]);
		close(pvec[0]);
		close(2);	/* so errors don't mess up the screen */
		open("/dev/null", 1);
		NCencstr(genbuf,tmpbuf,sizeof(tmpbuf));
		execl(svalue(SHELL), "sh", "-c", tmpbuf, 0);
		oerrno = errno; close(1); dup(2); errno = oerrno;
		filioerr(svalue(SHELL));
	}
	close(pvec[1]);
	do {
		*argv = cp;
		for (;;) {
			c = 1;
			ch[1] = 0;
			if (read(io, ch, 1) != 1) {
				close(io);
				c = -1;
			} else if (NCisshift(ch[0])) {
				if (read(io, &ch[1], 1) != 1) {
					close(io);
					c = -1;
				}
			}
			if (c <= 0 || NCisspace(NCdechr(ch)))
				break;
			*cp++ = ch[0];
			if (ch[1]) {
				if (--nleft - 1 > 0)
					*cp++ = ch[1];
				else
					cp[-1] = '\0'; /* better safe than sorry */
			}
			if (--nleft <= 0)
				error(msg(M_081, "Arg list too long"));
		}
		if (cp != *argv) {
			--nleft;
			*cp++ = 0;
			gp->argc0++;
			if (gp->argc0 >= NARGS)
				error(msg(M_082, "Arg list too long"));
			argv++;
		}
	} while (c >= 0);
	waitfor();	/* part of ex_unix.c */
	if (gp->argc0 == 0)
		error(msg(M_083, "No match"));
}

/*
 * Scan genbuf for shell metacharacters.
 * Set is union of v7 shell and csh metas.
 */
gscan()
{
	register NLchar *cp;

	for (cp = genbuf; *cp; cp++)
		if (any(*cp, "~{[*?$`'\"\\"))
			return (1);
	return (0);
}


/*
 * Parse one filename into file.
 */
struct glob G;
getone()
{
	register char *str;

	if (getargs() == 0)
		error(msg(M_084, "Missing filename"));
	glob(&G);
	if (G.argc0 > 1)
		error(msg(M_085, "Ambiguous|Too many file names"));
	str = G.argv[G.argc0 - 1];
	if (strlen(str) > FNSIZE - 4)
		error(msg(M_086, "Filename too long"));
	strcpy(file, str);
}

/*
 * Read a file from the world.
 * C is command, 'e' if this really an edit (or a recover).
 */
rop(c,r)
	int c,r;
{
	register int i;
	struct stat stbuf;
	short magic;
	static int ovro;	/* old value(READONLY) */
	static int denied;	/* 1 if READONLY was set due to file permissions */
#ifdef  FLOCKFILE
	int *lp, *iop;
#endif	/* FLOCKFILE */

	if (c != 'r') {         /* To prevent caryover of readonly option to new file */
		if (value(READONLY) && denied) {
			value(READONLY) = ovro;
			denied = 0;
		}
	}
	io = open(file, 0);
	if (io < 0) {
		if (c == 'e' && errno == ENOENT) {
			edited++;
			/*
			 * If the user just did "ex foo" he is probably
			 * creating a new file.  Don't be an error, since
			 * this is ugly, and it screws up the + option.
			 */
			strcpy(origfile,"");
			if (!seenprompt) {
				ex_printf(msg(M_087, " [New file]"));
				noonl();
				return;
			}
		}
		syserror(0);
	}
	if (fstat(io, &stbuf))
		syserror(0);
	switch (stbuf.st_mode & S_IFMT) {

	case S_IFBLK:
		error(msg(M_088, " Block special file"));

	case S_IFCHR:
		if (isatty(io))
			error(msg(M_089, " Teletype"));
		if (samei(&stbuf, "/dev/null"))
			break;
		error(msg(M_090, " Character special file"));

	case S_IFDIR:
		error(msg(M_091, " Directory"));

	case S_IFREG:
		i = read(io, (char *) &magic, sizeof(magic));
		lseek(io, 0l, 0);
		if (i != sizeof(magic))
			break;
		switch (magic) {

		case 0737:	/* XCOFF executable (xxx) */
		case 0637:	/* XCOFF executable (RT) */
		case 0514:	/* 386 demand pagged executable */
		case 0403:	/* GPOFF executable */
		case 0405:	/* data overlay on exec */
		case 0407:	/* unshared */
		case 0410:	/* shared text */
		case 0411:	/* separate I/D */
		case 0413:	/* VM/Unix demand paged */
		case 0430:	/* PDP-11 Overlay shared */
		case 0431:	/* PDP-11 Overlay sep I/D */
			error(msg(M_092, " Executable"));

/*
 *  Change the #if defined below to #if !defined to activate this code
 *  or place -MYDEBUG in makefile.  This code will then report on the
 *  magic number of any file which gets past the code above.  This is 
 *  useful in controling file editing in vi.
 *  See also /etc/magic for other magic numbers of interest.
 */
#if defined(MYDEBUG)
		default:
		ex_printf("%o octal %d dec %x hex %d bytes",magic,magic,magic,sizeof(magic));
		error(msg(M_094, " Non-ascii file")); /* Magic number report */
#endif
		}
	}
	if (c != 'r') {
		if ((stbuf.st_mode & 0222) == 0 || access(file, 2) < 0) {
			ovro = value(READONLY);
			denied = 1;
			value(READONLY) = 1;
		}
	}
	if (hush == 0 && value(READONLY)) {
		ex_printf(msg(M_095, " [Read only]"));
		flush();
	}
#ifdef  FLOCKFILE
	/*
	 * Attempt to lock the file. We use an sharable lock if reading
	 * the file, and an exclusive lock if editting a file.
	 * The lock will be released when the file is no longer being
	 * referenced. At any time, the editor can have as many as
	 * three files locked, and with different lock statuses.
	 */
	/*
	 * if this is either the saved or alternate file or current file,
	 * point to the appropriate descriptor and file lock status.
	 */
	if (strcmp (file,savedfile) == 0) {
		if (!io_savedfile) io_savedfile = dup(io) ;
		lp = &lock_savedfile ;  iop = &io_savedfile ;
	} else if (strcmp (file,altfile) == 0) {
		if (!io_altfile) io_altfile = dup(io) ;
		lp = &lock_altfile ;    iop = &io_altfile ;
	} else {
		/* throw away current lock, accquire new current lock */
		if (io_curr) close (io_curr) ;
		io_curr = dup(io) ;
		lp = &lock_curr ;       iop = &io_curr ;
		lock_curr = 0 ;
	}
	if (c == 'r' || value(READONLY) || *lp == 0) {
		/* if we have a lock already, don't bother */
		if (!*lp) {
			/* try for a shared lock */
			if (flock(*iop, LOCK_SH|LOCK_NB) < 0
			&& errno == EWOULDBLOCK) {
				ex_printf (msg(M_300,
			" [FILE BEING MODIFIED BY ANOTHER PROCESS]"));
				flush();
				goto fail_lock ;
			} else *lp = LOCK_SH ;
		}
	}
	if ( c != 'r'  && !value(READONLY) && *lp != LOCK_EX) {
		/* if we are editting the file, upgrade to an exclusive lock. */
		if (flock(*iop, LOCK_EX|LOCK_NB) < 0 && errno == EWOULDBLOCK) {
			ex_printf (msg(M_301,
				" [File open by another process]"));
			flush();
		} else *lp = LOCK_EX ;
	}
fail_lock:
#endif	/* FLOCKFILE */
	/* capture the name of the original file */
	if (r == 0)
		strcpy(origfile,file);
#ifdef TRACE
	if (trace)
		fprintf(trace,"\nrop: file %s, origfile %s\n",file,origfile);
#endif
	if (c == 'r')
		setdot();
	else
		setall();
	if (FIXUNDO && inopen && c == 'r')
		undap1 = undap2 = dot + 1;
	rop2();
	rop3(c);
}

rop2()
{
	line *first, *last, *a;

	deletenone();
	clrstats();
	first = addr2 + 1;
	ignore(append(getfile, addr2));
	last = dot;
	/*
	 * If the modeline variable is set, check the first and last five
	 * lines of the file for a mode setting line.
	 */
	if (value(MODELINE)) {
		for (a=first; a<=last; a++) {
			if (a==first+5 && last-first > 10)
				a = last - 4;
			getline(*a);
			checkmodeline(linebuf);
		}
	}
}

rop3(c)
	int c;
{

	if (iostats() == 0 && c == 'e')
		edited++;
	if (c == 'e') {
		if (wasalt || firstpat) {
			register line *addr = zero + oldadot;

			if (addr > dol)
				addr = dol;
			if (firstpat) {
				static NLchar s_dollar[2] = { '$', 0 };
				/*
				 * Save and restore peekc/globp
				 * this is probably not needed, but
				 * is done out of paranoia. The
				 * only thing which needs to be done is
				 * to set peekc to 0.  This fixes a bug
				 * that occurs when modeline is used in
				 * combination with "+n" on the command line
				 */
				NLchar    *oglobp;
				int     savepeekc;

				oglobp = globp;
				savepeekc = peekc;
				peekc = 0;
				globp = (*firstpat) ? firstpat : s_dollar;
				commands(1,1);
				firstpat = 0;
				globp = oglobp;
				peekc = savepeekc;
			} else if (addr >= one) {
				if (inopen)
					dot = addr;
				markpr(addr);
			} else
				goto other;
		} else
other:
			if (dol > zero) {
				if (inopen)
					dot = one;
				markpr(one);
			}
		if(FIXUNDO)
			undkind = UNDNONE;
		if (inopen) {
			vcline = 0;
			vreplace(0, lines, lineDOL());
		}
	}
	if (laste) {
		tlaste();
		laste = 0;
		ex_sync();
	}
}

/*
 * Are these two really the same inode?
 */
samei(sp, cp)
	struct stat *sp;
	char *cp;
{
	struct stat stb;

	if (stat(cp, &stb) < 0 || sp->st_dev != stb.st_dev)
		return (0);
	return (sp->st_ino == stb.st_ino);
}

/* Returns from edited() */
#define	EDF	0		/* Edited file */
#define	NOTEDF	-1		/* Not edited file */
#define	PARTBUF	1		/* Write of partial buffer to Edited file */

/*
 * Write a file.
 */
wop(dofname)
short dofname;	/* if 1 call filename, else use savedfile */
{
	register int c, exclam, nonexist, exist;
	line *saddr1, *saddr2;
	struct stat stbuf;

#ifdef TRACE
		if (trace)
			fprintf(trace,"entering wop: file %s, origfile %s\n",file,origfile);
#endif
	c = 0;
	exclam = 0;
	if (dofname) {
		if (peekchar() == '!')
			exclam++, ignchar();
		ignore(skipwh());
		while (peekchar() == '>')
			ignchar(), c++, ignore(skipwh());
		if (c != 0 && c != 2)
			error(msg(M_096, "Write forms are 'w' and 'w>>'"));
		filename('w');	/* writes the filename on the cmd line */
	} else {
		if (savedfile[0] == 0)
			error(msg(M_097, "No file|No current filename"));
		saddr1=addr1;
		saddr2=addr2;
		addr1=one;
		addr2=dol;
		strcpy(file, savedfile); /* Grab original file name */
		if (inopen) {
			vclrech(0);
			splitw++;
		}
		lprintf("\"%s\"", file);
	}
	nonexist = stat(file, &stbuf);
#ifdef TRACE
		if (trace)
			fprintf(trace,"wop at nonexist: file %s, origfile %s\n",file,origfile);
#endif
	switch (c) {

	case 0:
		if (!exclam && (!value(WRITEANY) || value(READONLY)))
		switch (edfile()) {
		
		case NOTEDF:
			if (nonexist)
				break;
			if ((stbuf.st_mode & S_IFMT) == S_IFCHR) {
				if (samei(&stbuf, "/dev/null"))
					break;
				if (samei(&stbuf, "/dev/tty"))
					break;
			}
			io = open(file, 1);
			if (io < 0)
				syserror(0);
			if (!isatty(io))
				serror(msg(M_098, " File exists| File exists - use \"w! %s\" to overwrite"), file);
			close(io);
#ifdef TRACE
		if (trace)
			fprintf(trace,"wop at NOTEDF break: file %s, origfile %s\n",file,origfile);
#endif
			break;

		case EDF:
			if (value(READONLY))
				error(msg(M_099, " File is read only"));
#ifdef TRACE
		if (trace)
			fprintf(trace,"wop at EDF break: file %s, origfile %s\n",file,origfile);
#endif
			break;

		case PARTBUF:
			if (value(READONLY))
				error(msg(M_099, " File is read only"));
			error(msg(M_101, " Use \"w!\" to write partial buffer"));
		}  /* end of switch (edfile()) */
cre:
#ifdef TRACE
		if (trace)
			fprintf(trace,"wop at cre: file %s, origfile %s\n",file,origfile);
#endif
		io = creat(file, 0666);
		if (io < 0)
			syserror(0);
		writing = 1;
#ifdef TRACE
		if (trace)
			fprintf(trace,"wop after writing = 1: file %s, origfile %s\n",file,origfile);
#endif
		if (hush == 0)
			if (nonexist)
				ex_printf(msg(M_102, " [New file]"));
			else if (value(WRITEANY) && edfile() != EDF)
				ex_printf(msg(M_103, " [Existing file]"));
#ifdef TRACE
		if (trace)
			fprintf(trace,"wop at PARTBUF break: file %s, origfile %s\n",file,origfile);
#endif
		break;

	case 2:
		io = open(file, 1);
		if (io < 0) {
			if (exclam || value(WRITEANY))
				goto cre;
			syserror(0);
		}
#ifdef TRACE
		if (trace)
			fprintf(trace,"wop at case2 break: file %s, origfile %s\n",file,origfile);
#endif
		lseek(io, 0l, 2);
		break;
	}
#ifdef TRACE
		if (trace)
			fprintf(trace,"wop before putfile(): file %s, origfile %s\n",file,origfile);
#endif
	putfile(0);
#ifdef TRACE
		if (trace)
			fprintf(trace,"wop after putfile(): file %s, origfile %s\n",file,origfile);
#endif
	fsync (io);
	ignore(iostats());
	if (c != 2 && addr1 == one && addr2 == dol) {
		if (eq(file, savedfile))
			edited = 1;
		ex_sync();
	}
	if (!dofname) {
		addr1 = saddr1;
		addr2 = saddr2;
	}
	writing = 0;
}

/*
 * Is file the edited file?
 * Work here is that it is not considered edited
 * if this is a partial buffer, and distinguish
 * all cases.
 */
edfile()
{

	if (!edited || !eq(file, savedfile))
		return (NOTEDF);
	return (addr1 == one && addr2 == dol ? EDF : PARTBUF);
}

/*
 * Extract the next line from the io stream.
 */
getfile()
{
	register NLchar c;
	register char *fp;
	register NLchar *lp;
	register int keep = 0;
	static char inbuf[LBSIZE+1];
	static char *nextip;

	lp = linebuf;
	fp = nextip;
	do {
		if (--ninbuf < 0) {
			/* read less than sizeof(inbuf) and keep 1 byte */
			/* in case a 2 byte character spans two reads */
			ninbuf = read(io, inbuf+keep, LBSIZE) - 1;
			if (ninbuf < 0) {
				if (keep > 0) {
					if (lp >= &linebuf[LBSIZE]) {
						error(msg(M_105, " Line too long"));
					}
					*lp++ = inbuf[0];
				}
				if (lp != linebuf) {
					if (lp >= &linebuf[LBSIZE]) {
						error(msg(M_105, " Line too long"));
					}
					lp++;
					ex_printf(msg(M_104, " [Incomplete last line]"));
					break;
				}
				return (EOF);
			}
			fp = inbuf;
			cntch += ninbuf+1;
			/* ninbuf now should reflect bytes in buffer */
			ninbuf += keep;
			keep = 0;
		}
		if (lp >= &linebuf[LBSIZE]) {
			error(msg(M_105, " Line too long"));
		}
		/* 2 byte character may span buffer reads */
		if (ninbuf == 0 && NCisshift(*fp)) {
			inbuf[0] = *fp;
			keep = 1;
			continue;
		} else
			c = NLsgetc(fp);
		if (NCchrlen(c) == 2)
			--ninbuf;
		if (c == 0) {
			cntnull++;
			continue;
		}
		*lp++ = c;
	} while (c != '\n');
	*--lp = 0;
	nextip = fp;
	cntln++;
	return (0);
}

/*
 * Write a range onto the io stream.
 */
putfile(isfilter)
int isfilter;
{
	line *a1;
	register char *fp, *lp;
	register int nib;
	char outbuf[BUFFERSIZ];
	char tmpbuf[sizeof(linebuf)];
	NLchar olb[LBSIZE];

	a1 = addr1;
	clrstats();
	cntln = addr2 - a1 + 1;
	if (cntln == 0)
		return;
	nib = BUFFERSIZ;
	fp = outbuf;
/* Save linebuf in olb to fix problem with autowrite on ^A command */
#ifdef TRACE
		if (trace)
			fprintf(trace,"putfile() at entry: file %s, origfile %s\n",file,origfile);
#endif
	CP(olb, linebuf);
	do {
		getline(*a1++);
		NCencstr(linebuf,tmpbuf,sizeof(tmpbuf));
		lp = tmpbuf;
		for (;;) {
			if (--nib < 0) {
				nib = fp - outbuf;
				if (write(io, outbuf, nib) != nib) {
					wrerror();
				}
				cntch += nib;
				nib = BUFFERSIZ - 1;
				fp = outbuf;
			}
			if ((*fp++ = *lp++) == 0) {
				fp[-1] = '\n';
				break;
			}
		}
	} while (a1 <= addr2);
	nib = fp - outbuf;
	CP(linebuf, olb);
#ifdef TRACE
		if (trace)
			fprintf(trace,"in putfile(): io %s, file %s, origfile %s\n",io,file,origfile);
#endif
	if (write(io, outbuf, nib) != nib) {
		wrerror();
	}
	cntch += nib;
}

/*
 * A write error has occurred;	if the file being written was
 * the edited file then we consider it to have changed since it is
 * now likely scrambled.
 */
static void wrerror()
{

#ifdef TRACE
		if (trace)
			fprintf(trace,"wrerror() at entry: file %s, origfile %s\n",file,origfile);
#endif
	if (eq(file, savedfile) && edited)
		change();
	syserror(1);
}

/*
 * Source command, handles nested sources.
 * Traps errors since it mungs unit 0 during the source.
 */
short slevel;
short ttyindes;

source(fil, okfail)
	char *fil;
	short okfail;
{
	jmp_buf osetexit;
	register int saveinp, ointty, oerrno;
	NLchar *saveglobp;
	int savepeekc;				/* code point */
	signal(SIGINT, SIG_IGN);
	saveinp = dup(0);
	savepeekc = peekc;
	saveglobp = globp;
	peekc = 0; globp = 0;
	if (saveinp < 0)
		error(msg(M_106, "Too many nested sources"));
	if (slevel <= 0)
		ttyindes = saveinp;
	close(0);
	if (open(fil, 0) < 0) {
		oerrno = errno;
		setrupt();
		dup(saveinp);
		close(saveinp);
		errno = oerrno;
		if (!okfail)
			filioerr(fil);
		return;
	}
	slevel++;
	ointty = intty;
	intty = isatty(0);
	oprompt = value(PROMPT);
	value(PROMPT) &= intty;
	getexit(osetexit);
	setrupt();
	if (setexit() == 0)
		commands(1, 1);
	else if (slevel > 1) {
		close(0);
		dup(saveinp);
		close(saveinp);
		slevel--;
		resexit(osetexit);
		reset();
	}
	intty = ointty;
	value(PROMPT) = oprompt;
	close(0);
	dup(saveinp);
	close(saveinp);
	globp = saveglobp;
	peekc = savepeekc;
	slevel--;
		resexit(osetexit);
}

/*
 * Clear io statistics before a read or write.
 */
clrstats()
{

	ninbuf = 0;
	cntch = 0;
	cntln = 0;
	cntnull = 0;
}

/*
 * Io is finished, close the unit and print statistics.
 * Messages modified to remove (s) usage for translation.
 */
static short iostats()
{
	close(io);
	io = -1;
	if (hush == 0) {
		if (value(TERSE))
			ex_printf(msg(M_107, " %d/%ld"), cntln, cntch);
		else {
			if (cntln == 1 && cntch == 1)
				ex_printf(msg(M_292, " %d line, %ld character"), cntln, cntch);
			if (cntln == 1 && cntch > 1)
				ex_printf(msg(M_292, " %d line, %ld characters"), cntln, cntch);
			if (cntln > 1 && cntch > 1)
				ex_printf(msg(M_292, " %d lines, %ld characters"), cntln, cntch);
		}
		if (cntnull == 1) {
			ex_printf(msg(M_109, " (%ld null)"), cntnull);
		}
		if (cntnull > 1) {
	
			ex_printf(msg(M_109, " (%ld nulls)"), cntnull);
		}
		noonl();
		flush();
	}
	return (cntnull != 0);
}

# define index NCstrchr
# define rindex NCstrrchr
static checkmodeline(ln)
NLchar *ln;
{
	NLchar *beg, *end;
	NLchar cmdbuf[1024];
        int savpc;
        short savle;

	beg = index(ln, ':');
	if (beg == NULL || beg < &ln[3])
		return;
	if (beg[-3] != ' ' && beg[-3] != '\t') return;
	if (!(beg[-2] == 'e' && beg[-1] == 'x') &&
	    !(beg[-2] == 'v' && beg[-1] == 'i')) return;
	(void)NCstrncpy(cmdbuf, beg+1, NCSIZE (cmdbuf));
	end = rindex(cmdbuf, ':');
	if (end == NULL)
		return;
	*end = 0;
        savpc = peekc; peekc = 0;
        savle = laste;
	globp = cmdbuf;
	commands(1, 1);
        peekc = savpc;
        laste = savle;
}
