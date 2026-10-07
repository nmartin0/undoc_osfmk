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
static char rcsid[] = "@(#)$RCSfile: history.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:19 $";
#endif
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */
/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 
 *                  1988, 1989   AT&T
 *      All Rights Reserved

 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE 
 *      CODE OF AT&T.
 *      The copyright notice above does not 
 *      evidence any actual or intended
 *      publication of such source code.

 */
/*
 *   History file manipulation routines
 *
 *   David Korn
 *   AT&T Bell Laboratories
 *   Room 3C-526B
 *   Murray Hill, N. J. 07974
 *   Tel. x7975
 *
 */

/*
 * Each command in the history file starts on an even byte is null terminated.
 * The first byte must contain the special character H_UNDO and the second
 * byte is the version number.  The sequence H_UNDO 0, following a command,
 * nullifies the previous command. A six byte sequence starting with
 * H_CMDNO is used to store the command number so that it is not necessary
 * to read the file from beginning to end to get to the last block of
 * commands.  This format of this sequence is different in version 1
 * then in version 0.  Version 1 allows commands to use the full 8 bit
 * character set.  It can understand version 0 format files.
 */


#ifdef KSHELL
#   include	"defs.h"
#   include	"builtins.h"
#else
#   include	"io.h"
#   include	<signal.h>
#   include	<ctype.h>
#endif	/* KSHELL */
#include	"history.h"
#ifdef MULTIBYTE
#   include	"national.h"
#endif /* MULTIBYTE */

#ifndef KSHELL
#   define sh_heap(s)		strcpy(malloc(strlen(s)+1),(s))
#   define sh_arith(str)	atoi(str)
#   define new_of(type,x)	((type*)malloc((unsigned)sizeof(type)+(x)))
#   define e_unknown	 	"unknown"
#   define e_create	 	"cannot create"
#   define path_relative(x)	(x)
    static int			io_readbuff();
    static off_t		io_seek();
    static void			io_fclose();
    static int			io_getc();
    static int			io_init();
    static int			io_renumber();
    static int			io_mktmp();
    extern char			*strcpy();
    extern char			*getenv();
    extern char			*mktemp();
    char login_sh =		0;
#ifdef  KSH_88D
    char hist_fname[] =         "/.history";
#else
    char hist_fname[] =         "./history";
#endif  /* KSH_88D */
    struct history		*hist_ptr;
#endif	/* KSHELL */

static int fixmask;
#ifdef  KSH_88D
static void hist_trim(int);
#else
static void hist_trim();
#endif /* KSH_88D */
static int hist_nearend();
static int hist_check();
static int hist_clean();
#ifdef  KSH_88D
static void hist_free ();
#endif /* KSH_88D */
static int io_getbyte();
static int hist_version;
static struct history *wasopen;
static off_t hist_marker;	/* offset of last command marker */
static char htrim;		/* set while in hist_trim */
static const unsigned char hist_stamp[2] = { H_UNDO, H_VERSION };

#define HIST_RECENT	600

#ifdef MSG
#include "ksh_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_KSH,n,s) 
#else
#define MSGSTR(n,s) s
#endif

/*
 * open the history file
 * if HISTNAME is not given and userid==0 then no history file.
 * if login_sh and HISTFILE is longer than HISTMAX bytes then it is
 * cleaned up.
 * hist_open() returns 1, if histor file is open
 */

int  hist_open(void)
{
	register int fd;
	register struct history *fp;
	register char *histname;
	char fname[TMPSIZ];
	char hname[256];
	int maxlines;
	register char *cp;
	register off_t hsize = 0;
	int his_start;


	if(hist_ptr)
		return(1);
	histname = nam_strval(HISTFILE);
	if(!histname)
	{
		
		if(cp=nam_strval(HOME))
			cp = sh_copy(cp,hname);
		else
			cp = hname;
		sh_copy(hist_fname,cp); /*NOTX*/
		histname = hname;
	}
	*fname = 0;
	if(fp=wasopen)
	{
		/* reuse history file if same name */
		wasopen = 0;
#ifdef  KSH_88D
                hist_ptr = fp;
                if(strcmp(histname,fp->fixname)==0)
                        return(1);
                else
                        hist_free();
#else
		if(strcmp(histname,fp->fixname)==0)
		{
			hist_ptr = fp;
			return(1);
		}
		else
		{
			io_fclose(fp->fixfd);
			free((void *)fp);
		}
#endif /* KSH_88D */
	}
retry:
	histname = path_relative(histname);
	if((fd=open(histname,O_APPEND|O_RDWR|O_CREAT,S_IREAD|S_IWRITE))>=0)
	{
		hsize=io_seek(fd,(off_t)0,SEEK_END);
	}
	/* make sure that file has history file format */
	if(hsize && hist_check(fd))
	{
		io_fclose(fd);
		hsize = 0;
#ifdef  KSH_88D
                if (unlink(histname) >= 0)
                        goto retry;
                fd = -1;
#else
                unlink(histname);
                goto retry;
#endif /* KSH_88D */
	}
	if(fd < 0)
	{
#ifdef KSHELL
		/* don't allow root a history_file in /tmp */
		if(sh.userid)
#endif	/* KSHELL */
			fd = io_mktmp(fname);
	}
	if(fd<0)
		return(0);
	fd = io_renumber(fd,FCIO);
	if(cp=nam_strval(HISTSIZE))
		maxlines = (unsigned)sh_arith(cp);
	else
		maxlines = HIS_DFLT;
	for(fixmask=16;fixmask <= maxlines; fixmask <<=1 );
	if(!(fp=new_of(struct history,(--fixmask)*sizeof(off_t))))
	{
		io_fclose(fd);
		return(0);
	}
	hist_ptr = fp;
	fp->fixfd = fd;
	fp->fixmax = maxlines;
	io_init(fd,(struct fileblk*)0,(char*)0);
	fp->fixfp = io_ftable[fd];
	fp->fixind = 1;
	fp->fixcmds[1] = 2;
	fp->fixcnt = 2;
	fp->fixname = sh_heap(histname);
	if(hsize==0)
		/* put special characters at front of file */
	{
		write(fd,(char*)hist_stamp,(unsigned)2);
		p_setout(fd);
	}
	/* initialize history list */
	else
	{
		int first;
		off_t size = (HISMAX/4)+maxlines*HISLINE;
		first = fp->fixind = hist_nearend(fd,hsize-size);
		hist_eof();	 /* this sets fixind to last command */
		his_start = fp->fixind-maxlines;
		if(his_start <=0)
			his_start = 1;
		while(first > his_start)
		{
			size += size;
			first = hist_nearend(fd,hsize-size);
			fp->fixind = first;
		}
		hist_eof();
	}
	if(*fname)
		unlink(fname);
	if(hist_clean(fp->fixfd) && his_start>1 && hsize > HISMAX)
	{
#ifdef  KSH_88D
#ifdef DBUG
                p_setout(ERRIO);
                p_num(getpid(),':');
                p_str("hist_trim hsize",'=');
                p_num(hsize,NL);
                p_flush();
                hist_trim(fp->fixind-maxlines);
#endif /* DBUG */
#else
		int fdo;
		fdo = io_renumber(fp->fixfd, USERIO);
		fp->fixfd = fdo;
		unlink(histname);
		hist_trim(fdo,fp->fixind-maxlines);
#endif /* KSH_88D */
	}
	return(1);
}

#ifdef  KSH_88D
/*
 * close the history file and free the space
 */

static void hist_free()
{
        register struct history *fp = hist_ptr;
        io_fclose(fp->fixfd);
        free((char*)fp);
        hist_ptr = 0;
}

#endif /* KSH_88D */
/*
 * check history file format to see if it begins with special byte
 */

static int hist_check(fd)
register int fd;
{
	unsigned char magic[2];
	io_seek(fd,(off_t)0,SEEK_SET);
	if((read(fd,(char*)magic,(unsigned)2)!=2) || (magic[0]!=H_UNDO))
		return(1);
	hist_version = magic[1];
	return(0);
}

/*
 * Decide whether to clean out the history file
 */
 
#ifdef LOCK_EX
#  undef LOCK_EX
#  undef F_SETLK
#  include	<sys/file.h>
#endif /*LOCK_EX */
#ifdef F_SETLK 
#  if defined(F_SETLK) && !defined(F_WRLCK)
#     undef F_SETLK
#     include <fcntl.h>
#  endif
   static struct flock hislock = { F_WRLCK, 0, (off_t)0, (off_t)2, (pid_t)0 };
#endif
static int hist_clean(fd)
register int fd;
{
	register int r;
	struct stat statb;
#ifdef LOCK_EX
	r = (flock(fd,LOCK_NB|LOCK_EX)==0);
#else
#  ifdef F_SETLK
	r = (fcntl(fd,F_SETLK,&hislock)==0);
#  else
	r = sh.login_sh;
#  endif
#endif
	if(fstat(fd,&statb)>=0)
	{
		/* see if history file was recently accessed */
		if(r)
		{
			if((time((time_t *)0)-statb.st_mtime) < HIST_RECENT)
				r = 0;
		}
	}
	return(r);
}

/*
 * Copy the last <n> commands to a new file and make this the history file
 */

#ifdef  KSH_88D
static void hist_trim(int n)
#else
static void hist_trim(fdo,n)
int fdo;
#endif /* KSH_88D */
{
	register struct fileblk *fp;
	register struct history *hist_new;
	register int c;
	struct history *hist_old = hist_ptr;
#ifdef  KSH_88D
        off_t old,new=0;
        int fdo = io_renumber(hist_ptr->fixfd, USERIO);
        hist_ptr->fixfd = fdo;
        unlink(hist_ptr->fixname);
#endif /* KSH_88D */
	hist_ptr = 0;
	if(!hist_open())
	{
		/* use the old history file */
		hist_ptr = hist_old;
		hist_ptr->fixfd = io_renumber(fdo,FCIO);
		return;
	}
	hist_new = hist_ptr;
	p_setout(hist_new->fixfd);
	fp = hist_new->fixfp;
	if(n < 0)
		n = 0;
	htrim++;
	do
	{
		hist_ptr = hist_old;
#ifdef  KSH_88D
                old = new;
                new = io_seek(fdo,hist_position(++n),SEEK_SET);
#else
                io_seek(fdo,hist_position(++n),SEEK_SET);
#endif /* KSH_88D */
		hist_ptr = hist_new;
#if (defined(NLS) || defined(KJI))
		while((c=io_getbyte(fdo))!=EOF && c)
#else
		while((c=io_getc(fdo))!=EOF && c)
#endif
		{
			if(fp->ptr >= fp->last)
				p_flush();
			*fp->ptr++ = c;
			hist_new->fixcnt++;
		}
#ifdef KSHELL
		st.states |= FIXFLG;
#endif	/* KSHELL */
		hist_flush();
	}
#ifdef  KSH_88D
        while(new > old && c!=EOF);
#else
        while(c!=EOF);
#endif /* KSH_88D */
	htrim = 0;
	hist_cancel();
	io_fclose(fdo);
	free((void *)hist_old);
}

/*
 * position history file at size and find next command number 
 */

static int hist_nearend(fd,size)
register int fd;
off_t size;
{
	register int n = 0;
	register int state = 0;
	register int c;
	if(size <=0)
		goto begin;
	io_seek(fd,size,SEEK_SET);
	/* skip to numbered command and return the number */
	/* numbering commands occur after a null and begin with H_CMDNO */
#if (defined(NLS) || defined(KJI))
	while((c=io_getbyte(fd))!=EOF) switch(state)
#else
	while((c=io_getc(fd))!=EOF) switch(state)
#endif
	{
		case 1:
			if(c==H_CMDNO)
			{
				hist_ptr->fixcnt = size + n + 6;
				state = 2;
			}
			break;

		case 2:
			/* see if H_CMDNO is followed by 0 */
			if(hist_version && c)
			{
				n += 2;
				state = 0;
				break;
			}
			n = 0;

		case 3:
		case 4:
		case 5:
			if(hist_version)
				n = (n<<8) + c;
			else if(state<4)
				n = (n<<7) + (c&0177);
			state++;
			break;

		case 6:
			return(n);

		default:
			state = (c==0);
			n++;
	}
begin:
	io_seek(fd,(off_t)2,SEEK_SET);
	hist_ptr->fixcnt = 2;
	return(1);
}

/*
 * This routine marks the history file as closed.  The file actually
 * closes when the program exits, or when you open a history file
 * with a different name.
 */

void hist_close(void)
{
	wasopen = hist_ptr;
	hist_ptr = 0;
}

/*
 * This routine reads the history file from the present position
 * to the end-of-file and puts the information in the in-core
 * history table
 * Note that H_CMDNO is only recognized at the beginning of a command
 * and that H_UNDO as the first character of a command is skipped
 * unless it is followed by 0.  If followed by 0 then it cancels
 * the previous command.
 */

void hist_eof(void)
{
	register struct history *fp = hist_ptr;
	register int c;
	register int incr = 0;
	register int oldc = 0;
	register off_t count = fp->fixcnt;
	int skip = 0;
	io_seek(fp->fixfd,count,SEEK_SET);
#ifdef INT16
	while((c=io_getc(fp->fixfd))!=EOF)
	{
#else
	/* could use io_getc() but this is faster */
	while(1)
	{
		c = *(fp->fixfp->ptr)++;
		if(c==0 && (fp->fixfp->ptr > fp->fixfp->last))
		{
			c = io_readbuff(fp->fixfp);
			if(c == EOF)
				break;
		}
		c &= STRIP;
#endif /* INT16 */
		count++;
		if(skip-- > 0)
		{
			if(skip==2)
				hist_marker = count;
			oldc = 0;
			continue;
		}
		if(c == 0)
		{
			if(oldc==H_CMDNO && incr==0)
				skip = 3;
			fp->fixind += incr;
			fp->fixcmds[fp->fixind&fixmask] = count;
			incr = 0;
		}
		else if(oldc == 0)
		{
			if(c == H_CMDNO)
			{
				/* old format history file */
				if(hist_version==0)
					skip = 4;
				incr = 0;
			}
			else if(c==H_UNDO)
				incr = -1;
		}
		else
			incr = 1;
		oldc = c;
	}
	fp->fixcnt = count;
#ifdef  KSH_88D
        p_setout (fp->fixfd);
#endif /* KSH_88D */
}

/*
 * This routine will cause the previous command to be cancelled
 */

void hist_cancel(void)
{
	register struct history *fp = hist_ptr;
	register int c;
	if(!fp)
		return;
	p_setout(fp->fixfd);
	p_char(H_UNDO);
	p_char(0);
	p_flush();
	fp->fixcnt += 2;
	c = (--fp->fixind)&fixmask;
	fp->fixcmds[c] = fp->fixcnt;
#ifdef  KSH_88D
        p_setout(fp->fixfd);
#endif /* KSH_88D */
}

/*
 * This routine adds one or two null bytes and flushes the history buffer
 */

void hist_flush(void)
{
	register struct history *fp = hist_ptr;
	register struct fileblk *fd;
	register int c;
	int flush = 0;
	int savcount;
	if(!fp)
		return;
#ifdef KSHELL
	if(!(st.states&FIXFLG))
		return;
	st.states &= ~FIXFLG;
#endif	/* KSHELL */
	fd = fp->fixfp;
	if(htrim)
	{
		p_char(0);
		fp->fixcnt++;
		goto set_count;
	}
#ifdef  KSH_88D
        if((fp->fixcnt=lseek(fp->fixfd,(off_t)0,SEEK_END)) <0)
        {
#ifdef DBUG
                p_setout(ERRIO);
                p_num(getpid(),':');
                p_str("hist_flush: EOF seek failed errno",'=');
                p_num(errno,NL);
                p_flush();
#endif /* DBUG */
                hist_free();
                if(!htrim)
                        hist_open();
                return;
        }
#else
	fp->fixcnt = lseek(fp->fixfd,(off_t)0,SEEK_END);
#endif /* KSH_88D */
	p_setout(fp->fixfd);
	/* remove whitespace from end of commands */
#ifdef KJI
	do
	{
		register char *p, *q;
		wchar_t d;

		p = q = fd->base;
		while((p += NLchrlen(p)) < fd->ptr)
			q = p;
		NCdec(q,&d);
		if(!isspace(d))
		{
			/* don't remove spaces/tabs that are backquoted */
			if(d=='\\' && *(fd->ptr+1)!='\n') 
				fd->ptr += NLchrlen(fd->ptr);
			break;
		}
		fd->ptr = q;
	} while(fd->ptr > fd->base);
	/* don't count empty lines */
	if(fd->ptr <= fd->base && !fp->fixflush)
#else
	while(--fd->ptr >= fd->base)
	{
		c= *fd->ptr;
		if(!isspace(c))
		{
			if(c=='\\' && *(fd->ptr+1)!='\n')
				fd->ptr++;
			break;
		}
	}
	/* don't count empty lines */
	if(++fd->ptr <= fd->base && !fp->fixflush)
#endif /* NLS || KJI */
	{
		fp->fixind--;
		goto set_count;
	}
	p_char('\n');
	p_char(0);
	flush++;
	fp->fixcnt += (fd->ptr-fd->base);
set_count:
	/* start each command on an even byte boundary */
	if(fp->fixcnt&01)
	{
		fp->fixcnt++;
		p_char(0);
		flush++;
	}
	c = (++fp->fixind)&fixmask;
	savcount = fp->fixcmds[c];
	fp->fixcmds[c] = fp->fixcnt;
	if(fp->fixcnt > hist_marker+IOBSIZE/2)
	{
		/* put line number in file */
		fp->fixcnt += 6;
		p_char(H_CMDNO);
		p_char(0);
		c = (fp->fixind>>16);
		p_char(c);
		c = (fp->fixind>>8);
		p_char(c);
		c = fp->fixind;
		p_char(c);
		p_char(0);
		flush++;
		fp->fixcmds[c&fixmask] = fp->fixcnt;
		hist_marker = fp->fixcnt;
	}
	if(flush && !htrim)
	{
		p_flush();
		/* check for write errors, like ulimit */
		if(fd->flag&IOERR)
		{
			c = (fp->fixind--)&fixmask;
			fp->fixcmds[c] = savcount;
		}
	}
	fp->fixflush = 0;
}

/*
 * return byte offset in history file for command <n>
 */

off_t hist_position(int n)
{
	register struct history *fp = hist_ptr;
	return(fp->fixcmds[n&fixmask]);
}

/*
 * write the command starting at offset <offset> onto file <fd>.
 * if character <last> appears before newline it is deleted
 * each new-line character is replaced with string <nl>.
 */

void hist_list(off_t offset, int last, char *nl)
{
	register int oldc=0;
	register int c;
	register struct history *fp = hist_ptr;
	if(offset<0 || !fp)
	{
		p_str(MSGSTR(E_UNKNOWN, (char *)e_unknown),'\n'); /*MSG*/
		return;
	}
	io_seek(fp->fixfd,offset,SEEK_SET);
	while((c = io_getc(fp->fixfd)) != EOF)
	{
		if(c && oldc=='\n')
			p_str(nl,0);
		else if(oldc==last &&  c=='\n')
			return;
		else if(c==0 && last)
			return;
		else if(oldc)
			p_char(oldc);
		oldc = c;
		if(c==0)
			return;
	}
	return;
}
		 
/*
 * find index for last line with given string
 * If flag==0 then line must begin with string
 * direction < 1 for backwards search
*/

histloc hist_find(char *string, int index1, int flag, int direction)
{
	register struct history *fp = hist_ptr;
	int index2;
#ifdef KJI
	wchar_t	wcharTmp;
#endif
	off_t offset;
	histloc location;
	location.his_command = -1;
	if(!fp)
		return(location);
	/* leading ^ means beginning of line unless escaped */
	if(flag)
	{
#ifdef KJI
                wcharTmp = index2;
                NCdec(string,&wcharTmp);
                index2 = wcharTmp;
#else
		index2 = *string;
#endif
		if(index2=='\\')
			string++;
		else if(index2=='^')
		{
			flag=0;
			string++;
		}
	}
	index2 = fp->fixind;
	if(direction<0)
	{
		index2 -= fp->fixmax;
		if(index2<1)
			index2 = 1;
		if(index1 <= index2)
			return(location);
	}
	else if(index1 >= index2)
		return(location);
	while(index1!=index2)
	{
		direction>0?++index1:--index1;
		offset = hist_position(index1);
		if((location.his_line=hist_match(offset,string,flag))>=0)
		{
			location.his_command = index1;
			return(location);
		}
#ifdef KSHELL
		/* allow a search to be aborted */
		if(sh.trapnote&SIGSET)
			break;
#endif /* KSHELL */
	}
	return(location);
}

/*
 * search for <string> in history file starting at location <offset>
 * If flag==0 then line must begin with string
 * returns the line number of the match if successful, otherwise -1
 */

int hist_match(off_t offset, char *string, int flag)
{
	register char *cp;
	register int c;
	register struct history *fp = hist_ptr;
	register off_t count;
	int line = 0;
#if (defined(NLS) || defined(KJI))
        register int d;
        int first = *string;
#endif /* NLS || KJI */
#ifdef MULTIBYTE
	int nbytes = 0;
#endif /* MULTIBYTE */
#if (defined(NLS) || defined(KJI))
	if (NCisshift(first))
		first = (first<<8) | *(string+1);
#endif
	do
	{
		if(offset>=0)
		{
			io_seek(fp->fixfd,offset,SEEK_SET);
			count = offset;
		}
		offset = -1;
		for(cp=string;*cp;cp++)
		{
#if (defined(NLS) || defined(KJI))
			d = *cp;
			if(NCisshift(d)) {
				cp++;
				d = (d<<8) | *cp;
			}
#endif
			if((c=io_getc(fp->fixfd)) == EOF || c ==0)
				break;
#if (defined(NLS) || defined(KJI))
			count += NCchrlen(c);
#else
			count++;
#endif
#ifdef MULTIBYTE
			/* always position at character boundary */
			if(--nbytes > 0)
			{
				if(cp==string)
				{
					cp--;
					continue;
				}
			}
			else
			{
				nbytes = echarset(c);
				nbytes = in_csize(nbytes) + (nbytes>=2);
			}
#endif /* MULTIBYTE */
			if(c == '\n')
				line++;
			/* save earliest possible matching character */
#if (defined(NLS) || defined(KJI))
			if(flag && c == first && offset<0)
#else
                        if(flag && c == *string && offset<0)
#endif /* NLS || KJI */
				offset = count;
#if (defined(NLS) || defined(KJI))
			if(d != c )
#else
                        if(*cp != c )
#endif /* NLS || KJI */
				break;
		}
		if(*cp==0) /* match found */
			return(line);
	}
	while(flag && c && c != EOF);
	return(-1);
}


#if ESH || VSH
/*
 * copy command <command> from history file to s1
 * at most MAXLINE characters copied
 * if s1==0 the number of lines for the command is returned
 * line=linenumber  for emacs copy and only this line of command will be copied
 * line < 0 for full command copy
 * -1 returned if there is no history file
 */

int hist_copy(char *s1, int command, int line)
{
	register int c;
	register struct history *fp = hist_ptr;
	register int count = 0;
#ifdef  KSH_88D
        register char *s1max = s1+MAXLINE;
#else
        register char *s1max = s1+MAXLINE-1;
#endif /* KSH_88D */
	off_t offset;
	if(!fp)
		return(-1);
	offset =  hist_position(command);
	io_seek(fp->fixfd,offset,SEEK_SET);
	while ((c = io_getc(fp->fixfd)) && c!=EOF)
	{
		if(c=='\n')
		{
			if(count++ ==line)
				break;
			else if(line >= 0)	
				continue;
		}
		if(s1 && (line<0 || line==count))
		{
			if(s1 >= s1max)
			{
#ifdef  KSH_88D
                                *--s1 = 0;
#else
                                *s1 = 0;
#endif /* KSH_88D */
				break;
			}
#if (defined(NLS) || defined(KJI))
			/* avoid truncation in the middle of 2-byte chars */
			if(NCchrlen(c) > 1) 
			{
				if((s1+1) >= s1max) {
					*s1 = 0;
					break;
				}
				*s1++ = c>>8;
			}
			*s1++ = c&STRIP;
#else
			*s1++ = c;
#endif
		}
			
	}
	io_seek(fp->fixfd,(off_t)0,SEEK_END);
	if(s1==0)
		return(count);
	if(count && (c= *(s1-1)) == '\n')
		s1--;
	*s1 = '\0';
	return(count);
}

/*
 * return word number <word> from command number <command>
 */

char *hist_word(char *s1, int word)
{
#ifdef KJI
	wchar_t c;
	int chlen;
#else
	wchar_t c;
#endif
	register char *cp = s1;
	register int flag = 0;
	if(hist_ptr==0)
#ifdef KSHELL
		return(sh.lastarg);
#else
		return((char*)0);
#endif /* KSHELL */
	hist_copy(s1,hist_ptr->fixind-1,-1);
#ifdef KJI
	for(; chlen=NCdec(cp,&c), c>(wchar_t)0; cp+=chlen)
#else
	for(;c = *cp;cp++)
#endif
	{
		c = isspace(c);
		if(c && flag)
		{
			*cp = 0;
			if(--word==0)
				break;
			flag = 0;
		}
		else if(c==0 && flag==0)
		{
			s1 = cp;
			flag++;
		}
	}
	*cp = 0;
	return(s1);
}

#endif	/* ESH */

#ifdef ESH
/*
 * given the current command and line number,
 * and number of lines back or foward,
 * compute the new command and line number.
 */

histloc hist_locate(int command, int line, int lines)
{
	histloc next;
	line += lines;
	if(!hist_ptr)
	{
		command = -1;
		goto done;
	}
	if(lines > 0)
	{
		register int count;
		while(command <= hist_ptr->fixind)
		{
			count = hist_copy(NIL,command,-1);
			if(count > line)
				goto done;
			line -= count;
			command++;
		}
	}
	else
	{
		register int least = hist_ptr->fixind-hist_ptr->fixmax;
		while(1)
		{
			if(line >=0)
				goto done;
			if(--command < least)
				break;
			line += hist_copy(NIL,command,-1);
		}
		command = -1;
	}
	next.his_command = command;
	return(next);
done:
	next.his_line = line;
	next.his_command = command;
	return(next);
}
#endif	/* ESH */

#if (defined(NLS) || defined(KJI))
/*
 * returns the next character(byte) from file <fd>
 */
static io_getbyte(fd)
int fd;
{
	register struct fileblk *fp = io_ftable[fd];
	register int c;
	if(!fp)
		return(EOF);
	if(c= *fp->ptr++)
		return(c&STRIP);
	if(fp->ptr <= fp->last)
		return(0);
	return(io_readbuff(fp));
}
#endif /* NLS || KJI */


#ifdef KSHELL

/*
 * given a file containing a command and a string of the form old=new,
 * execute the command with the string old replaced by new
 */
void hist_subst(const char *command, int fd, char *replace)
{
	register char *new=replace;
	register char *sp = stak_begin();
	register int c;
	struct fileblk fb;
	char inbuff[IOBSIZE+1];
	char *string;
	while(*++new != '='); /* skip to '=' */
	io_init(fd,&fb,inbuff);
#if (defined(NLS) || defined(KJI))
	while ((c=io_getbyte(fd)) != EOF)
#else
	while ((c=io_getc(fd)) != EOF)
#endif
		*sp++ = c;
	string = stak_end(sp);
	io_fclose(fd);
	*new++ =  0;
	if((sp=sh_substitute(string,replace,new))==0)
		sh_fail(command,MSGSTR(E_SUBST, (char *)e_subst)); /*MSG*/
	*(new-1) =  '=';
	p_setout(hist_ptr->fixfd);
	p_str(sp,0);
	hist_flush();
	p_setout(ERRIO);
	p_str(sp,0);
	sh_eval(sp);
}

#else /* !KSHELL */

/*
 * initialize file structure
 */

static io_init(fd,fp,buf)
int fd;
register struct fileblk *fp;
char *buf;
{
	if(!fp)
	{
		fp = new_of(struct fileblk,IOBSIZE+1);
		buf = (char*)(fp+1);
		fp->flag = IOFREE;
	}
	else
		fp->flag = 0;
	fp->fdes = fd;
	fp->fseek = 0;
	fp->base = fp->ptr  = fp->last = buf;
	*fp->ptr = 0;
	fp->flag |= (IORW|IOREAD);
	io_ftable[fd] = fp;
}

/*
 * returns the next character from file <fd>
 */

static io_getc(fd)
int fd;
{
	register struct fileblk *fp = io_ftable[fd];
	register int c;
	if(!fp)
		return(EOF);
	if(c= *fp->ptr++)
		return(c&STRIP);
	if(fp->ptr <= fp->last)
		return(0);
	return(io_readbuff(fp));
}

/*
 * This special version does not handle ptrname==1
 * It also saves a lot of real seeks on history file
 */

static off_t hoffset;

static off_t io_seek(fd, offset, ptrname)
int fd;
off_t	offset;
register int	ptrname;
{
	register struct fileblk *fp;
	register int c;
	off_t	p;

	if(!(fp=io_ftable[fd]))
		return(lseek(fd,offset,ptrname));
	fp->flag &= ~IOEOF;
	if(!(fp->flag&IOREAD))
	{
		p_flush();
	}
	c = 0;
	/* check history file to see if already in the buffer */
	if(fd==FCIO && ptrname==0 && (fp->flag&IOREAD) && offset<hoffset)
	{
		p = hoffset - (fp->last - fp->base);
		if(offset >= p)
		{
			fp->ptr = fp->base + (int)(offset-p);
			return(offset);
		}
		else
		{
			c = offset&(IOBSIZE-1);
			offset -= c;
		}
	}
	if(fp->flag&IORW)
	{
		fp->flag &= ~(IOWRT|IOREAD);
		fp->last = fp->ptr = fp->base;
		*fp->last = 0;
	}
	p = lseek(fd, offset, ptrname);
	if(fd==FCIO)
	{
		if(ptrname==0)
			hoffset = p;
		else
			hoffset = -1;
		if(c)
		{
			io_readbuff(fp);
			fp->ptr += (c-1);
		}
	}
	return(p);
}

/*
 * Read from file into fp
 */

static int io_readbuff(fp)
register struct fileblk *fp;
{
	register int syncread;
	register int n;

	if (fp->flag & IORW)
		fp->flag |= IOREAD;
	if (!(fp->flag&IOREAD))
		return(EOF);

	fp->ptr = fp->last;
#ifdef  KSH_88D
#ifdef  SYSCALL
        n = syscall(3, filenum(fp),fp->base, IOBSIZE);
#else
        n = rEAd (filenum(fp), fp->base, IOBSIZE);
#endif /* SYSCALL */
#else
        n = syscall(3, filenum(fp),fp->base, IOBSIZE);
#endif /* KSH_88D */
	fp->ptr = fp->base;
	if(n > 0)
		fp->last = fp->base + n;
	else
		fp->last = fp->ptr;
	*fp->last = 0;
	if (n <= 0)
	{
		if (n == 0)
		{
			fp->flag |= IOEOF;
			if (fp->flag & IORW)
				fp->flag &= ~IOREAD;
		}
		else
			fp->flag |= IOERR;
		return(-1);
	}
	if(fp->fdes==FCIO)
		hoffset += n;
	return(*fp->ptr++&STRIP);
}


/*
 * close file stream and reopen for reading and writing
 */

static void io_fclose(fd)
register int fd;
{
	register struct fileblk *fp = io_ftable[fd];
	/* reposition seek pointer if necessary */
	if(fp && !(fp->flag&IOREAD))
		p_flush();
	close(fd);
	if(fp && (fp->flag&IOFREE))
		free(fp);
	io_ftable[fd] = 0;
}

/*
 * move the file number fa to unit fb
 */

static int io_renumber(fa, fb)
register int	fa;
register int 	fb;
{
	if(fa >= 0)
	{
		close(fb);
		fcntl(fa,0,fb); /* normal dup */
#ifdef  KSH_88D
                if(io_ftable[fb] = io_ftable[fa])
                        io_ftable[fb]->fdes = fb;
                io_ftable[fa] = 0;
#endif /* KSH_88D */
		close(fa);
		/* set fb close-on-exec */
#ifdef F_SETFD
		fcntl(fb,F_SETFD,1);
#else
#   ifdef FIOCLEX
		ioctl(fb, FIOCLEX, NULL);
#   endif /* FIOCLEX */
#endif	/* F_SETFD */
	}
	return(fb);
}

static char template[] = "/tmp/histXXXXXX";
/*
 * return file descriptor for an open file
 */

static int io_mktmp(fname)
register char *fname;
{
	int fd;
	strcpy(fname,template);
	if((fd=open(mktemp(fname),O_CREAT|O_RDWR,RW_ALL)) < 0)
		sh_fail(fname,MSGSTR(E_CREATE, (char *)e_create)); /*MSG*/
#ifdef  KSH_88D
        return (fd);
#endif /* KSH_88D */
}

#endif /* KSHELL */
