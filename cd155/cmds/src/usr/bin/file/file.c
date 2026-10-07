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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: file.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:24:48 $";
#endif
/*
 * COMPONENT_NAME: (CMDFS) commands that manipulate files
 *
 * FUNCTIONS: file
 *
 * ORIGINS: 3, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 *	file.c	1.20  com/cmd/fs/file,3.1,9021 4/25/90 10:13:53";
 *
 */
/*
 *	File attempts to determine the type of a file by reading the
 *	first 512 bytes.  It uses a data file /etc/magic to look up
 *	machine specific magic numbers.
 */                                                                   

#include	<stdio.h> 
#include	<NLctype.h>
#include	<NLchar.h>
#include	<signal.h>
#include	<sys/param.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<utime.h>
#include	<limits.h>
#include	<stdlib.h>
#include	<locale.h>
#include <nl_types.h>
nl_catd catd;			/* global catalog descriptor */
#include "file_msg.h"
#define MSGSTR(C,D)		catgets(catd,MS_FILE,C,D)
#include        <time.h>

/*
**	Types
*/

#define	BYTE	0
#define SHORT   1
#define LONG    2
#define STR     3
#define DATE    4

/*
**	Opcodes
*/

#define	EQ	0
#define	GT	1
#define	LT	2
#define	ANY	4
#define SUB     8       /* or'ed in */

/*
**	Misc
*/

#define	NENT	400
#define	BSZ	128
#define	FBSZ	512
#define	reg	register

/* Ass lang comment char */
#define ASCOMCHAR '#'

/*
**	Structure of magic file entry
*/

struct	entry	{
	char	e_level;	/* 0 or 1 */
	char	e_type;
	char	e_opcode;
	long	e_off;		/* in bytes */
	long    e_mask;
	union	{
		long	num;
		char	*str;
	}	e_value;
	char	*e_str;
};

typedef	struct entry	Entry;

Entry	*mtab;
char	fbuf[FBSZ];
char	*mfile = "/etc/magic";
char	*fort[] = {
	"function","subroutine","common","dimension","block","integer",
	"real","data","double",0};
char	*asc[] = {
	"sys","mov","tst","clr","jmp",0};
char	*c[] = {
	"int","char","float","double","struct","extern",0};
char	*as[] = {
	"globl","byte","even","text","data","bss","comm",0};
void		mkmtab();

static void hexcpy();
char * CSloca();
char * CSlocc();
char * CSskpa();

int	i = 0;
int	fbsz;
int	ifd;
char	*filename; /* PTM 35328 */

#define	prf(x)	fprintf(stdout,"%s:%s", x, strlen(x)>6 ? "\t" : "\t\t");
#define	eprf(x)	fprintf(stderr,"%s:%s", x, strlen(x)>6 ? "\t" : "\t\t");

main(argc, argv)
int argc;
char **argv;
{
	reg	char	*p;
	reg	int	ch;
	reg	FILE	*fl;
	reg	int	cflg = 0, eflg = 0, fflg = 0;
	auto	char	ap[PATH_MAX];
	extern	int	optind;
	extern	char	*optarg;

	(void) setlocale (LC_ALL,"");
	catd = catopen(MF_FILE,0);

	while((ch = getopt(argc, argv, "cf:m:")) != EOF)
	switch(ch) {
	case 'c':		/* check for correct magic file syntax */
		cflg++;
		break;

	case 'f':		/* examine list of files in this file  */
		fflg++;
		if ((fl = fopen(optarg, "r")) == NULL) {
			fprintf(stderr,MSGSTR(CANTOPEN,"cannot open %s\n")
			, optarg);					/*MSG*/
		}
		break;

	case 'm':		/* alternate magic file. */
		mfile = optarg;
		break;

	case '?':
		eflg++;
		break;
	}
	if((!cflg) && (!fflg) && (eflg || optind == argc)) {
use:
		fprintf(stderr,MSGSTR(USAGE,				/*MSG*/
		"usage: file [-c] [-f ffile] [-m mfile] file...\n"));	/*MSG*/
		exit(2);
	}
	if(cflg) {
		reg	Entry	*ep;

		mkmtab(1);
		printf(MSGSTR(CHEAD,					/*MSG*/
		"level  off type opcode     mask    value  string\n")); /*MSG*/
		for(ep = mtab; ep->e_off != -1L; ep++) {
			/* VFMT */
			printf("%3d%7ld%5d%7o", ep->e_level, ep->e_off,
				ep->e_type, ep->e_opcode);
			if(ep->e_type == STR)
				printf("%18.18s", ep->e_value.str);
			else
				printf("%9lx%9lx", ep->e_mask, ep->e_value.num);
			printf("  %s", ep->e_str);
			printf("\n");
		}
		exit(0);
	}
	for(; fflg || optind < argc; optind += !fflg) {
		reg	int	l;

		if(fflg) {
			if((p = fgets(ap,PATH_MAX,fl)) == NULL) { 
				fflg = 0;
				optind--;
				continue;
			}
			l = strlen(p);
			if(l > 0)
				p[l - 1] = '\0';
		} else
			p = argv[optind];
		/* prf(p); */	/* PTM 35328 */
		filename = p;	/* PTM 35328 */
		type(p);
		if(ifd)
			close(ifd);
	}
	exit(0);
}

/*
 * NAME: type
 *                                                                    
 * FUNCTION:	Open each file and determine the type.  First stat the
 *		file then read the first 512 bytes and try to determine
 *		the best solution.
 *                                                                    
 * RETURN VALUE: none
 *			     return value)
 */  

type(file)
char	*file;
{
	int	j,nl;
	char	ch;
	struct	stat	mbuf;
	struct	utimbuf	times;
	char 	slink[PATH_MAX];
	int nlsc, c1, NLShighb, highb; 

	ifd = -1;
	if(lstat(file, &mbuf) < 0) {
		fprintf(stderr,MSGSTR(SCANTOPEN,"cannot stat %s\n"), file);		/*MSG*/
		return;
	}
	switch (mbuf.st_mode & S_IFMT) {
	case S_IFLNK:
		prf(file);
		printf(MSGSTR(MLINK,"symbolic link"));			/*MSG*/
		j = readlink(file, slink, sizeof slink -1);
		if (j >= 0) {
			slink[j]='\0';
			printf(MSGSTR(MTO," to %s"), slink);		/*MSG*/
		}
		printf("\n");
		return;	

	case S_IFCHR:
		prf(file);
		printf(MSGSTR(MCHAR,"character"));			/*MSG*/
		goto spcl;

	case S_IFDIR:
		prf(file);
		printf(MSGSTR(MDIR,"directory\n"));			/*MSG*/
		return;

	case S_IFIFO:
		prf(file);
		printf(MSGSTR(MFIFO,"fifo\n"));				/*MSG*/
		return;

	case S_IFSOCK:
		prf(file);
		printf(MSGSTR(MSOCK,"socket\n"));			/*MSG*/
		return;

	case S_IFBLK:
		prf(file);
		printf(MSGSTR(MBLOCK,"block"));				/*MSG*/

spcl:
		printf(MSGSTR(MSPEC," special (%d/%d)\n"),		/*MSG*/	
		major(mbuf.st_rdev), minor(mbuf.st_rdev));		/*MSG*/
		return;
	}
	ifd = open(file, 0);
	if(ifd < 0) {
		eprf(file);
		fprintf(stderr,MSGSTR(ECOFR,"cannot open for reading\n"));	/*MSG*/
		return;
	}
	fbsz = read(ifd, fbuf, FBSZ);
	if(fbsz == 0) {
		prf(file);
		printf(MSGSTR(MEMPTY,"empty\n"));			/*MSG*/
		goto out;
	}
	if(sccs()) {
		prf(file);
		printf(MSGSTR(MSCCS,"sccs\n"));				/*MSG*/
		goto out;
	}
	if(ckmtab())
		goto out;
	prf(file); /* PTM  35328 */
	i = 0;
	if(ccom() == 0)
		goto notc;
	while(fbuf[i] == '#') {
		j = i;
		while(fbuf[i++] != '\n') {
			if(i - j > 255) {
				printf(MSGSTR(M_DATA,"data\n"));	/*MSG*/
				goto out;
			}
			if(i >= fbsz)
				goto notc;
		}
		if(ccom() == 0)
			goto notc;
	}
check:
	if(lookup(c) == 1) {
		while((ch = fbuf[i++]) != ';' && ch != '{')
			if(i >= fbsz)
				goto notc;
		printf(MSGSTR(MCPROG,"c program text"));		/*MSG*/
		goto outa;
	}
	nl = 0;
	while(fbuf[i] != '(') {
		if(fbuf[i] <= (unsigned) 0)
			goto notas;
		if(fbuf[i] == ';'){
			i++; 
			goto check; 
		}
		if(fbuf[i++] == '\n')
			if(nl++ > 6)goto notc;
		if(i >= fbsz)goto notc;
	}
	while(fbuf[i] != ')') {
		if(fbuf[i++] == '\n')
			if(nl++ > 6)
				goto notc;
		if(i >= fbsz)
			goto notc;
	}
	while(fbuf[i] != '{') {
		if(fbuf[i++] == '\n')
			if(nl++ > 6)
				goto notc;
		if(i >= fbsz)
			goto notc;
	}
	printf(MSGSTR(MPROG,"c program text"));				/*MSG*/
	goto outa;
notc:
	i = 0;
	while(fbuf[i] == 'c' || fbuf[i] == '#') {
		while(fbuf[i++] != '\n')
			if(i >= fbsz)
				goto notfort;
	}
	if(lookup(fort) == 1){
		printf(MSGSTR(MFORT,"fortran program text"));		/*MSG*/
		goto outa;
	}
notfort:
	i = 0;
	if(ascom() == 0)
		goto notas;
	j = i-1;
	if(fbuf[i] == '.') {
		i++;
		if(lookup(as) == 1){
			printf(MSGSTR(MASS,"assembler program text")); 	/*MSG*/
			goto outa;
		}
		else if(j != -1 && fbuf[j] == '\n' && isalpha((int)fbuf[j+2])){
			printf(MSGSTR(MTPROC,				/*MSG*/
			     "[nt]roff, tbl, or eqn input text"));	/*MSG*/
			goto outa;
		}
	}
	while(lookup(asc) == 0) {
		if(ascom() == 0)
			goto notas;
		while(fbuf[i] != '\n' && fbuf[i++] != ':')
			if(i >= fbsz)
				goto notas;
		while(fbuf[i] == '\n' || fbuf[i] == ' ' || fbuf[i] == '\t')
			if(i++ >= fbsz)
				goto notas;
		j = i - 1;
		if(fbuf[i] == '.'){
			i++;
			if(lookup(as) == 1) {
				printf(MSGSTR(MASS,"assembler program text")); 
				goto outa; 
			}
			else if(fbuf[j] == '\n' && isalpha((int)fbuf[j+2])) {
				printf(MSGSTR(MTPROC,			/*MSG*/
				  "[nt]roff, tbl, or eqn input text"));	/*MSG*/
				goto outa;
			}
		}
	}
	printf(MSGSTR(MASS,"assembler program text"));			/*MSG*/
	goto outa;
notas:
	highb = NLShighb = 0;
	for(i=0; i < fbsz; i++) {
		nlsc = fbuf[i];
		if (NCisshift (nlsc)) {
			c1 = fbuf [++i];
			if (_NCdec2 (nlsc, c1, nlsc) == 1) --i;
			if (NCisNLchar (nlsc)) {
				printf(MSGSTR(MDORNLS,"data or NLS text\n"));
				goto out;
			}
		}
		if(fbuf[i]&0200) { 
			if ((unsigned char)(fbuf[0])==(unsigned char)'\100' &&
			    (unsigned char)(fbuf[1])==(unsigned char)'\357') {
				printf(MSGSTR(TROFF,"troff output\n"));	/*MSG*/
				goto out;
			}
			if (NCisNLchar (nlsc) && NCisprint (nlsc))
				NLShighb++;
			else
				highb++;
		}
	}
	if (highb  || NLShighb) {
		if (highb)
			printf(MSGSTR(M_DATA,"data\n"));		/*MSG*/
		else
			printf(MSGSTR(MDORNLS,"data or NLS text\n"));	/*MSG*/
		goto out;
	}

	if (access(file, X_OK) == 0)
		printf(MSGSTR(MCOMD,"commands text"));			/*MSG*/

	else if(english(fbuf, fbsz))
/*ASCII*/
		printf(MSGSTR(MENG,"English text"));			/*MSG*/
	else
		printf(MSGSTR(MASCII,"ascii text"));			/*MSG*/
outa:
	while(i < fbsz)
		if((fbuf[i++]&0377) > 127) {
			printf(MSGSTR(AGARB," with garbage\n"));	/*MSG*/
			goto out;
		}
	printf("\n");
out:
	close(ifd);
	times.actime = mbuf.st_atime;
	times.modtime = mbuf.st_mtime;
	utime(file, &times);
}

/*
 * NAME: mkmtab
 *                                                                    
 * FUNCTION: 	Create a table of file specific values (from /etc/magic
 *		by default.  If -m option is defined, use that file.
 *
 * RETURN VALUE:  None
 */  

void
mkmtab(cflg)
int cflg;
{
	reg     char    *p;
	reg	Entry	*ep;
	FILE    *fp;
	char    buf[BSZ];
	int     lcnt = 0;
	Entry   *mend;
	char    *nextp;
	static  char whitespace[] = " \t";
	static  char FmtErr[] =
	    "file: format error at line %d, missing %s field.\n";
	static  char MemErr[] =
	    "file: no memory for magic table.\n";
	static  char OpnErr[] =
	    "file: cannot open magic file \"%s\".\n";
	static  char OflErr[] =
	    "file: magic tab overflow - increase NENT in file.c.\n";

	if ((ep = (Entry *) calloc((size_t)sizeof(Entry),(size_t)NENT)) == NULL)
	{
	    fprintf(stderr,MSGSTR(NOMEM,MemErr));
	    exit(2);
	}
	mtab = ep;
	mend = &mtab[NENT];
	if ((fp = fopen(mfile, "r")) == NULL)
	{   
            fprintf(stderr,MSGSTR(COMF,OpnErr), mfile);
	    _exit(2);
	}

	while (fgets(buf, BSZ, fp) != NULL)
	{   
	    lcnt++;
	    p = buf;
	    if (*p == '\n' || *p == '#')
		continue;
	    /* LEVEL */
	    if (*p == '>')
	    {   ep->e_level = 1;
		p++;
	    }
	    /* OFFSET */
	    p = CSskpa(p, whitespace);
	    if (*p == '\0')
	    {   if (cflg)
		    fprintf(stderr,MSGSTR(FMTERR,FmtErr), 
		    lcnt,MSGSTR(XOFF,"offset"));
		continue;
	    }
	    ep->e_off = strtol(p, &nextp, 0);
	    p = CSskpa(nextp, whitespace);
	    if (*p == '\0')
	    {   if (cflg)
		    fprintf(stderr,MSGSTR(FMTERR,FmtErr), 
		    lcnt, MSGSTR(XTYPE,"type"));
		continue;
	    }

	    if (*p == 'd')
		ep->e_type = DATE;

	    if (*p == 's')
	    {   if (*(p+1) == 'h')
		    ep->e_type = SHORT;
		else
		    ep->e_type = STR;
	    } else
		if (*p == 'l')
		    ep->e_type = LONG;
	    p = CSskpa(CSloca(p, whitespace), whitespace);
	    /* OP-VALUE */
	    if (*p == '\0')
	    {   if (cflg)
		    fprintf(stderr,MSGSTR(FMTERR,FmtErr), 
		    lcnt, MSGSTR(XVALUE,"value"));
		continue;
	    }
	    if (ep->e_type != STR)
	    {   ep->e_mask = -1L;
		if (*p == '&')
		{   ep->e_mask = strtol(p+1, &nextp, 0);
		    p = nextp;
		}
		switch(*p)
		{ case '=':
		    ep->e_opcode = EQ;
		    p++;
		    break;
		  case '>':
		    ep->e_opcode = GT;
		    p++;
		    break;
		  case '<':
		    ep->e_opcode = LT;
		    p++;
		    break;
		  case 'x':
		    ep->e_opcode = ANY;
		    p++;
		    break;
		}
	    }
	    if (ep->e_opcode != ANY)
	    {   if (ep->e_type != STR)
		    /* if the '&' opcode was specified, then we want the result
		       of the & operation to be equal to the mask */
		    if (ep->e_mask != -1)
			ep->e_value.num = ep->e_mask;
		    else
			ep->e_value.num = strtol(p, &nextp, 0);
		else
		{   nextp = CSloca(p, whitespace);
		    *nextp++ = '\0';
		    if ((ep->e_value.str = (char *) malloc((size_t)strlen(p) + 1)) == NULL)
			{
				perror ("malloc");
				exit (-1);
			}
		    if (cflg)
		    	strcpy(ep->e_value.str, p);
		    else
		    {
		    	if (p[0] == '0' && p[1] == 'x')
		    	{	p += 2;
				hexcpy(ep->e_value.str, p);
		    	}
		    	else
		    		strcpy(ep->e_value.str, p);
		    }
		}
		p = nextp;
	    }
	    p = CSskpa(p, whitespace);
	    *CSlocc(p , '\n') = '\0';
	    if (*p != '\0')
	    {   if ((ep->e_str = malloc((size_t)strlen(p) + 1)) == NULL)
		{
			eprf(filename); /* PTM 35328 */
			perror ("malloc");
			exit(-1);
		}
		strcpy(ep->e_str, p);
		if (*CSlocc(p, '%') != '\0')
		    ep->e_opcode |= SUB;
	    }
	    if( ++ep >= mend)
	    {   
		fprintf(stderr,MSGSTR(OVFL,OflErr));
		exit(2);
	    }
	}
	ep->e_off = -1L;
	fclose(fp);
}


/*
 * NAME: ckmtab
 *                                                                    
 * FUNCTION: 
 *
 * RETURN VALUE: 1 fail
 *		 0 pass
 */  

ckmtab()
{

	reg	Entry	*ep;
	reg	char	*p;
	reg	int	lev1 = 0;
	auto	long	val;
	static	char	init = 0;
        char    *timebuf;

        timebuf = (char *) calloc(NLTBMAX,1);

	if(!init) {
		mkmtab(0);
		init = 1;
	}
	for(ep = mtab; ep->e_off != -1L; ep++) {
		if(lev1) {
			if(ep->e_level != 1)
				break;
		} else if(ep->e_level == 1)
			continue;
		p = &fbuf[ep->e_off];
		switch(ep->e_type) {

		case DATE:
		        /* Warning: the functionality here is not portable in that
			   byte ordering differences will cause problems.  The magic
			   file needs to provide some information about the byte
			   ordering. */
		        timebuf = ctime( (time_t*) p);
			printf(ep->e_str, (char*) timebuf);
			continue;

		case STR:
			if(strncmp(p,ep->e_value.str,strlen(ep->e_value.str)))
				continue;
			if(!lev1)
				prf(filename); /* PTM 35328 */
			if(ep->e_opcode & SUB)
				printf(ep->e_str, ep->e_value.str);
			else
				printf(ep->e_str);
			lev1 = 1;
			continue;
		case BYTE:
			val = (long)(*(unsigned char *) p);
			break;

		case SHORT:
			val = (long)(*(unsigned short *) p);
			break;

		case LONG:
			val = (*(long *) p);
			break;
		}
		val &= ep->e_mask;
		switch(ep->e_opcode & ~SUB) {
		case EQ:
			if(val != ep->e_value.num)
				continue;
			break;
		case GT:
			if(val <= ep->e_value.num)
				continue;
			break;
		case LT:
			if(val >= ep->e_value.num)
				continue;
			break;
		}
		if(!lev1)
			prf(filename); /* PTM 35328 */
		if(ep->e_level != 0)
			putchar(' ');
		if(ep->e_opcode & SUB)
			printf(ep->e_str, val);
		else
			printf(ep->e_str);
		lev1 = 1;
	}
	if(lev1) {
		putchar('\n'); 	
		return(1);
	}
	return(0);
}

/*
 * NAME: lookup
 *                                                                    
 * FUNCTION:  Look up in the table made in mkmtab from /etc/magic the
 *		type and check for a match with the current position.
 *                                                                    
 * RETURN VALUE: 	1 - Match
 *			0 - no match
 */  

lookup(tab)
reg	char **tab;
{
	reg	char	r;
	reg	int	k,j,l;

	while(fbuf[i] == ' ' || fbuf[i] == '\t' || fbuf[i] == '\n')
		i++;
	for(j=0; tab[j] != 0; j++) {
		l = 0;
		for(k=i; ((r=tab[j][l++]) == fbuf[k] && r != '\0');k++);
		if(r == '\0')
			if(fbuf[k] == ' ' || fbuf[k] == '\n' || fbuf[k] == '\t'
			    || fbuf[k] == '{' || fbuf[k] == '/') {
				i=k;
				return(1);
			}
	}
	return(0);
}

/*
 * NAME: ccom
 *                                                                    
 * FUNCTION:	Check to see if the read in buffer is in C format.  
 *                                                                    
 * RETURN VALUE: 0 - not C format
 *		 1 - C format
 */  
ccom()
{
	reg	char	cc;

	while((cc = fbuf[i]) == ' ' || cc == '\t' || cc == '\n')
		if(i++ >= fbsz)
			return(0);
	if(fbuf[i] == '/' && fbuf[i+1] == '*') {
		i += 2;
		while(fbuf[i] != '*' || fbuf[i+1] != '/') {
			if(fbuf[i] == '\\')
				i += 2;
			else
				i++;
			if(i >= fbsz)
				return(0);
		}
		if((i += 2) >= fbsz)
			return(0);
	}
	if(fbuf[i] == '\n')
		if(ccom() == 0)
			return(0);
	return(1);
}

/*
 * NAME: ascom
 *                                                                    
 * FUNCTION:	Check to see if the read in buffer is in ascom format.  
 *                                                                    
 * RETURN VALUE: 0 - not assembler
 *		 1 - assembler
 */  
ascom()
{
	while(fbuf[i] == ASCOMCHAR) {
		i++;
		while(fbuf[i++] != '\n')
			if(i >= fbsz)
				return(0);
		while(fbuf[i] == '\n')
			if(i++ >= fbsz)
				return(0);
	}
	return(1);
}

/*
 * NAME: sccs
 *                                                                    
 * FUNCTION:	Check to see if the read in buffer is in sccs format.  
 *		<1h######> in the first line.
 *                                                                    
 * RETURN VALUE: 0 - not sccs
 *		 1 - sccs
 */  

sccs() {
	reg int j;

	if(fbuf[0] == 1 && fbuf[1] == 'h')
		for(j=2; j<=6; j++)
			if(isdigit((int)fbuf[j])) continue;
			else return(0);
	else
		return(0);
	return(1);
}

/*
 * NAME: english
 *                                                                    
 * FUNCTION:	Check to see if the read in buffer is in english format.  
 *		Just check the punctuation.
 *                                                                    
 * RETURN VALUE: 0 - not english
 *		 1 - english
 */  
english (bp, n)
char *bp;
{
#	define NASC 128
	reg	int	j, vow, freq, rare;
	reg	int	badpun = 0, punct = 0;
	auto	int	ct[NASC];

	if (n<50)
		return(0); /* no point in statistics on squibs */
	for(j=0; j<NASC; j++)
		ct[j]=0;
	for(j=0; j<n; j++)
	{
		if (isascii(bp[j]))
			ct[bp[j]|040]++;
		switch (bp[j])
		{
		case '.': 
		case ',': 
		case ')': 
		case '%':
		case ';': 
		case ':': 
		case '?':
			punct++;
			if(j < n-1 && bp[j+1] != ' ' && bp[j+1] != '\n')
				badpun++;
		}
	}
	if (badpun*5 > punct)
		return(0);
	vow = ct['a'] + ct['e'] + ct['i'] + ct['o'] + ct['u'];
	freq = ct['e'] + ct['t'] + ct['a'] + ct['i'] + ct['o'] + ct['n'];
	rare = ct['v'] + ct['j'] + ct['k'] + ct['q'] + ct['x'] + ct['z'];
	if(2*ct[';'] > ct['e'])
		return(0);
	if((ct['>']+ct['<']+ct['/'])>ct['e'])
		return(0);	/* shell file test */
	return (vow*5 >= n-ct[' '] && freq >= 10*rare);
}

/*
 * NAME: hexcpy
 *                                                                    
 * RETURN VALUE: None
 */  
static void
hexcpy(str,p)
char *str;
char *p;
{
	reg c;
	for (c = 0; isxdigit((int)*p) && *p != '\0'; str++, p++) 
	{
		if (*p < 'A')
			*str = (*p & 07) << 4;
		else 	*str = (((*p + 9) & 017) << 4);
		p++;
		if (*p < 'A')
			*str |= (*p & 017);
		else	
			*str |= ((*p + 9) & 017);
	}
}		

char *
CSskpa(str, set)
	register char *str, *set; {
	register char *setp;
	register setc, strc;

	if (str && set) {
		for (;;) {
			NLchar str_ch;
			NLchar set_ch;

			NCdec (str, &str_ch);
			for (setp = set;;) {
				if (*setp == '\0') /* no match found */
					return (str);
				setp += NCdec (setp, &set_ch);
				if (str_ch == set_ch)
					break;
			}
			str += NCchrlen (str_ch);
		}
	}
	return str;
}





char *
CSloca(str, set)
	register char *str, *set; {
	register char *setp;
	register setc, strc;

	if (str && set) {
		while (1) {
			NLchar strch;
			register char *srchset;

			NCdec (str, &strch); /* grab successive characters */

			if (strch == '\0') /* end of string */
				return (str);

			srchset = set;

			while (1) {
				NLchar ch;

				NCdec (srchset, &ch); /* check next character */

				if (ch == '\0') /* end of search set */
				    break;

				if (strch == ch)
				    return (str);

				srchset += NCchrlen (ch);
			}

			str += NCchrlen (strch);
		}

	}
	return str;
}




char *
CSlocc(str, chr)
	register char *str;
	register chr; {
	register c;

	if (str) {
		if (NCchrlen (chr) == 1) {
			while (c = *str++)
				/* two byte character != one byte character */
				if (NCisshift (c)) {
					if (*str)
						str++; /* eat second byte */
				} else
					if (c == chr)
						break;
		} else {
			while (c = *str++) {
				register char *nstr;
				NLchar ch;

				if (NCisshift (c)) {
					/* get pointer to the NLchar */
					nstr = str-1;
					NCdec (nstr, &ch);
					if (ch == chr)
					    break;
					if (*str)
					    str++; /* next character */
				}
			}
		}
		--str;
	}
	return str;
}
