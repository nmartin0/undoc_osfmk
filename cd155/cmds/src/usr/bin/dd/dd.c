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
static char rcsid[] = "@(#)$RCSfile: dd.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:22:20 $";
#endif

/*
 *  BSD:	@(#)dd.c	4.6 (Berkeley) 6/1/88
 *  ATT:	@(#)dd.c        1.8
 * HPUX:	@(#)dd.c	66.2
 *  AIX:	@(#)dd.c        1.18  com/cmd/arch,3.1,9021 5/3/90 10:55
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
 */

#include <sys/file.h>
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <ctype.h>
#include <locale.h>
#include <nl_types.h>

#include "dd_msg.h"

#define	LCASE	01
#define	UCASE	02
#define	SWAB	04
#define NERR	010
#define SYNC	020

#define MSGSTR(num, str)	catgets(catd, MS_DD, num, str)
nl_catd catd;

unsigned int	ibs	= 512;
unsigned int	obs	= 512;
unsigned int	bs;
unsigned int	cbs;
unsigned int	ibc;
unsigned int	obc;
unsigned int	cbc;

int	fflag;
int	cflag;
int	nifr;
int	nipr;
int	nofr;
int	nopr;
int	ntrunc;
int	ibf;
int	obf;
int	skip;
int	seekn;
int	count;
int	nspace;
int	files	= 1;

char	*string;
char	*ifile;
char	*ofile;
char	*ibuf;
char	*obuf;
char	*op;

unsigned char etoa[] =	/* EBCDIC to ASCII conversion table */
{
	0000,0001,0002,0003,0234,0011,0206,0177,
	0227,0215,0216,0013,0014,0015,0016,0017,
	0020,0021,0022,0023,0235,0205,0010,0207,
	0030,0031,0222,0217,0034,0035,0036,0037,
	0200,0201,0202,0203,0204,0012,0027,0033,
	0210,0211,0212,0213,0214,0005,0006,0007,
	0220,0221,0026,0223,0224,0225,0226,0004,
	0230,0231,0232,0233,0024,0025,0236,0032,
	0040,0240,0241,0242,0243,0244,0245,0246,
	0247,0250,0325,0056,0074,0050,0053,0174,
	0046,0251,0252,0253,0254,0255,0256,0257,
	0260,0261,0041,0044,0052,0051,0073,0176,
	0055,0057,0262,0263,0264,0265,0266,0267,
	0270,0271,0313,0054,0045,0137,0076,0077,
	0272,0273,0274,0275,0276,0277,0300,0301,
	0302,0140,0072,0043,0100,0047,0075,0042,
	0303,0141,0142,0143,0144,0145,0146,0147,
	0150,0151,0304,0305,0306,0307,0310,0311,
	0312,0152,0153,0154,0155,0156,0157,0160,
	0161,0162,0136,0314,0315,0316,0317,0320,
	0321,0345,0163,0164,0165,0166,0167,0170,
	0171,0172,0322,0323,0324,0133,0326,0327,
	0330,0331,0332,0333,0334,0335,0336,0337,
	0340,0341,0342,0343,0344,0135,0346,0347,
	0173,0101,0102,0103,0104,0105,0106,0107,
	0110,0111,0350,0351,0352,0353,0354,0355,
	0175,0112,0113,0114,0115,0116,0117,0120,
	0121,0122,0356,0357,0360,0361,0362,0363,
	0134,0237,0123,0124,0125,0126,0127,0130,
	0131,0132,0364,0365,0366,0367,0370,0371,
	0060,0061,0062,0063,0064,0065,0066,0067,
	0070,0071,0372,0373,0374,0375,0376,0377,
};
unsigned char atoe[] =	/* ASCII to EBCDIC conversion table */
{
	0000,0001,0002,0003,0067,0055,0056,0057,
	0026,0005,0045,0013,0014,0015,0016,0017,
	0020,0021,0022,0023,0074,0075,0062,0046,
	0030,0031,0077,0047,0034,0035,0036,0037,
	0100,0132,0177,0173,0133,0154,0120,0175,
	0115,0135,0134,0116,0153,0140,0113,0141,
	0360,0361,0362,0363,0364,0365,0366,0367,
	0370,0371,0172,0136,0114,0176,0156,0157,
	0174,0301,0302,0303,0304,0305,0306,0307,
	0310,0311,0321,0322,0323,0324,0325,0326,
	0327,0330,0331,0342,0343,0344,0345,0346,
	0347,0350,0351,0255,0340,0275,0232,0155,
	0171,0201,0202,0203,0204,0205,0206,0207,
	0210,0211,0221,0222,0223,0224,0225,0226,
	0227,0230,0231,0242,0243,0244,0245,0246,
	0247,0250,0251,0300,0117,0320,0137,0007,
	0040,0041,0042,0043,0044,0025,0006,0027,
	0050,0051,0052,0053,0054,0011,0012,0033,
	0060,0061,0032,0063,0064,0065,0066,0010,
	0070,0071,0072,0073,0004,0024,0076,0341,
	0101,0102,0103,0104,0105,0106,0107,0110,
	0111,0121,0122,0123,0124,0125,0126,0127,
	0130,0131,0142,0143,0144,0145,0146,0147,
	0150,0151,0160,0161,0162,0163,0164,0165,
	0166,0167,0170,0200,0212,0213,0214,0215,
	0216,0217,0220,0152,0233,0234,0235,0236,
	0237,0240,0252,0253,0254,0112,0256,0257,
	0260,0261,0262,0263,0264,0265,0266,0267,
	0270,0271,0272,0273,0274,0241,0276,0277,
	0312,0313,0314,0315,0316,0317,0332,0333,
	0334,0335,0336,0337,0352,0353,0354,0355,
	0356,0357,0372,0373,0374,0375,0376,0377,
};
unsigned char atoibm[] =	/* slightly different ASCII to EBCDIC table */
{
	0000,0001,0002,0003,0067,0055,0056,0057,
	0026,0005,0045,0013,0014,0015,0016,0017,
	0020,0021,0022,0023,0074,0075,0062,0046,
	0030,0031,0077,0047,0034,0035,0036,0037,
	0100,0132,0177,0173,0133,0154,0120,0175,
	0115,0135,0134,0116,0153,0140,0113,0141,
	0360,0361,0362,0363,0364,0365,0366,0367,
	0370,0371,0172,0136,0114,0176,0156,0157,
	0174,0301,0302,0303,0304,0305,0306,0307,
	0310,0311,0321,0322,0323,0324,0325,0326,
	0327,0330,0331,0342,0343,0344,0345,0346,
	0347,0350,0351,0255,0340,0275,0137,0155,
	0171,0201,0202,0203,0204,0205,0206,0207,
	0210,0211,0221,0222,0223,0224,0225,0226,
	0227,0230,0231,0242,0243,0244,0245,0246,
	0247,0250,0251,0300,0117,0320,0241,0007,
	0040,0041,0042,0043,0044,0025,0006,0027,
	0050,0051,0052,0053,0054,0011,0012,0033,
	0060,0061,0032,0063,0064,0065,0066,0010,
	0070,0071,0072,0073,0004,0024,0076,0341,
	0101,0102,0103,0104,0105,0106,0107,0110,
	0111,0121,0122,0123,0124,0125,0126,0127,
	0130,0131,0142,0143,0144,0145,0146,0147,
	0150,0151,0160,0161,0162,0163,0164,0165,
	0166,0167,0170,0200,0212,0213,0214,0215,
	0216,0217,0220,0232,0233,0234,0235,0236,
	0237,0240,0252,0253,0254,0255,0256,0257,
	0260,0261,0262,0263,0264,0265,0266,0267,
	0270,0271,0272,0273,0274,0275,0276,0277,
	0312,0313,0314,0315,0316,0317,0332,0333,
	0334,0335,0336,0337,0352,0353,0354,0355,
	0356,0357,0372,0373,0374,0375,0376,0377,
};


main(argc, argv)
int	argc;
char	**argv;
{
	int (*conv)();
	register char *ip;
	register c;
	int ebcdic(), ibm(), ascii(), null(), cnull(), block(), unblock();
	void term();
	int flatten(), tonls(), fromnls();
	int a;

	(void) setlocale(LC_ALL, "");
	catd = catopen(MF_DD, 0);

	conv = null;
	for(c=1; c<argc; c++) {
		string = argv[c];
		if(match("ibs=")) {
			ibs = number();
			continue;
		}
		if(match("obs=")) {
			obs = number();
			continue;
		}
		if(match("cbs=")) {
			cbs = number();
			continue;
		}
		if (match("bs=")) {
			bs = number();
			continue;
		}
		if(match("if=")) {
			ifile = string;
			continue;
		}
		if(match("of=")) {
			ofile = string;
			continue;
		}
		if(match("skip=")) {
			skip = number();
			continue;
		}
		if(match("seek=")) {
			seekn = number();
			continue;
		}
		if(match("count=")) {
			count = number();
			continue;
		}
		if(match("files=")) {
			files = number();
			continue;
		}
		if(match("conv=")) {
		cloop:
			if(match(","))
				goto cloop;
			if(*string == '\0')
				continue;
			if(match("ebcdic")) {
				conv = ebcdic;
				goto cloop;
			}
			if(match("ibm")) {
				conv = ibm;
				goto cloop;
			}
			if(match("ascii")) {
				conv = ascii;
				goto cloop;
			}
#ifdef KJI
			if (match("tosjis")) {
				conv = tonls;
				goto cloop;
			}
			if (match("fromsjis")) {
				conv = fromnls;
				goto cloop;
			}
#endif
			if (match("tonls")) {
				conv = tonls;
				goto cloop;
			}
			if (match("fromnls")) {
				conv = fromnls;
				goto cloop;
			}
			if (match("flatten")) {
				conv = flatten;
				goto cloop;
			}
			if(match("block")) {
				conv = block;
				goto cloop;
			}
			if(match("unblock")) {
				conv = unblock;
				goto cloop;
			}
			if(match("lcase")) {
				cflag |= LCASE;
				goto cloop;
			}
			if(match("ucase")) {
				cflag |= UCASE;
				goto cloop;
			}
			if(match("swab")) {
				cflag |= SWAB;
				goto cloop;
			}
			if(match("noerror")) {
				cflag |= NERR;
				goto cloop;
			}
			if(match("sync")) {
				cflag |= SYNC;
				goto cloop;
			}
		}
		fprintf(stderr, MSGSTR(EARG, "dd: bad arg: %s\n"), string);
		exit(1);
	}
	if(conv == null && cflag&(LCASE|UCASE))
		conv = cnull;

	ibf = ifile ? open(ifile, O_RDONLY) : dup(0);
	if(ibf < 0) {
		perror(ifile);
		exit(1);
	}

	if (ifile && ofile) {
		struct stat sbuf_in, sbuf_out;

		if (stat(ifile, &sbuf_in) == -1 ) {
			perror(ifile);
			exit(1);
		}
		/*
		   If output file already exists AND it's not a special file AND
		   it's inode/device numbers are the same - don't stomp on it.
		*/
		if (stat(ofile, &sbuf_out) == 0 &&
		     !(sbuf_in.st_mode & (S_IFCHR | S_IFBLK)) &&
		     sbuf_in.st_dev == sbuf_out.st_dev &&
		     sbuf_in.st_ino == sbuf_out.st_ino ) {
			fprintf(stderr,
			  MSGSTR(ESTOMP, "dd: can't copy file onto itself\n"));
			exit(1);
		}
	}

	obf = ofile ? open(ofile, O_WRONLY|O_CREAT|O_TRUNC, 0666) : dup(1);
	if(obf < 0) {
		perror(ofile);
		exit(1);
	}

	if (bs) {
		ibs = obs = bs;
		if (conv == null)
			fflag++;
	}
	if(ibs == 0 || obs == 0) {
		fprintf(stderr,
			MSGSTR(EBLKSIZ, "dd: block sizes cannot be zero\n"));
		exit(1);
	}
	ibuf = (char *)malloc(ibs);
	if (fflag)
		obuf = ibuf;
	else
		obuf = (char *)malloc(obs);
	if(ibuf == NULL || obuf == NULL) {
		fprintf(stderr, MSGSTR(ENOMRY, "dd: not enough memory\n"));
		exit(1);
	}
	ibc = 0;
	obc = 0;
	cbc = 0;
	op = obuf;

	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, term);
	while(skip) {
		read(ibf, ibuf, ibs);
		skip--;
	}
	while(seekn) {
		lseek(obf, (long)obs, 1);
		seekn--;
	}

loop:
	if(ibc-- == 0) {
		ibc = 0;
		if(count==0 || nifr+nipr!=count) {
			if(cflag&(NERR|SYNC))
			for(ip=ibuf+ibs; ip>ibuf;)
				*--ip = 0;
			ibc = read(ibf, ibuf, ibs);
		}
		if(ibc == -1) {
			perror(MSGSTR(EREAD, "dd read error"));
			if((cflag&NERR) == 0) {
				flsh();
				term(2);
			}
			ibc = 0;
			for(c=0; c<ibs; c++)
				if(ibuf[c] != 0)
					ibc = c;
			stats();
		}
		if(ibc == 0 && --files<=0) {
			flsh();
			term(0);
		}
		if(ibc != ibs) {
			nipr++;
			if(cflag&SYNC)
				ibc = ibs;
		} else
			nifr++;
		ip = ibuf;
		c = ibc >> 1;
		if(cflag&SWAB && c)
		do {
			a = *ip++;
			ip[-1] = *ip;
			*ip++ = a;
		} while(--c);
		ip = ibuf;
		if (fflag) {
			obc = ibc;
			flsh();
			ibc = 0;
		}
		goto loop;
	}
	c = 0;
	c |= *ip++;
	c &= 0377;
	(*conv)(c);
	goto loop;
}

flsh()
{
	register c;
	void term();

	if(obc) {
		c = write(obf, obuf, obc);
		if(c < 0) {
			perror(MSGSTR(EWRITE, "dd write error"));
			stats();
			exit(2);
		}
		if((c >= 0) && (c != obc)) {
			nopr++;
			fprintf(stderr, MSGSTR(EPART, "dd: unable to write complete record\n"));
			stats();
			exit(2);
			
		}
		if(obc == obs)
			nofr++;
		else
			nopr++;
		
		obc = 0;
	}
}

match(s)
char *s;
{
	register char *cs;

	cs = string;
	while(*cs++ == *s)
		if(*s++ == '\0')
			goto true;
	if(*s != '\0')
		return(0);

true:
	cs--;
	string = cs;
	return(1);
}

number()
{
	register char *cs;
	register n;

	cs = string;
	n = 0;
	while(*cs >= '0' && *cs <= '9')
		n = n*10 + *cs++ - '0';
	for(;;)
	switch(*cs++) {

	case 'k':
		n *= 1024;
		continue;

	case 'w':
		n *= 2;
		continue;

	case 'b':
		n *= 512;
		continue;

	case '*':
	case 'x':
		string = cs;
		n *= number();

	case '\0':
		return(n);
	}
	/* never gets here */
}

cnull(cc)
{
	register c;

	c = cc;
	if(cflag&UCASE && islower(c))
		c = toupper(c);
	if(cflag&LCASE && isupper(c))
		c = tolower(c);
	null(c);
}

null(c)
{

	*op = c;
	op++;
	if(++obc >= obs) {
		flsh();
		op = obuf;
	}
}

ascii(cc)
{
	register c;

	c = etoa[cc] & 0377;
	if(cbs == 0) {
		cnull(c);
		return;
	}
	if(c == ' ')
		nspace++;
	else {
		while(nspace > 0) {
			null(' ');
			nspace--;
		}
		cnull(c);
	}

	if(++cbc >= cbs) {
		null('\n');
		cbc = 0;
		nspace = 0;
	}
}

unblock(cc)
{
	register c;

	c = cc & 0377;
	if(cbs == 0) {
		cnull(c);
		return;
	}
	if(c == ' ')
		nspace++;
	else {
		while(nspace > 0) {
			null(' ');
			nspace--;
		}
		cnull(c);
	}

	if(++cbc >= cbs) {
		null('\n');
		cbc = 0;
		nspace = 0;
	}
}

ebcdic(cc)
{
	register c;

	c = cc;
	if(cflag&UCASE && islower(c))
		c = toupper(c);
	if(cflag&LCASE && isupper(c))
		c = tolower(c);
	c = atoe[c] & 0377;
	if(cbs == 0) {
		null(c);
		return;
	}
	if(cc == '\n') {
		while(cbc < cbs) {
			null(atoe[' ']);
			cbc++;
		}
		cbc = 0;
		return;
	}
	if(cbc == cbs)
		ntrunc++;
	cbc++;
	if(cbc <= cbs)
		null(c);
}

ibm(cc)
{
	register c;

	c = cc;
	if(cflag&UCASE && islower(c))
		c = toupper(c);
	if(cflag&LCASE && isupper(c))
		c = tolower(c);
	c = atoibm[c] & 0377;
	if(cbs == 0) {
		null(c);
		return;
	}
	if(cc == '\n') {
		while(cbc < cbs) {
			null(atoibm[' ']);
			cbc++;
		}
		cbc = 0;
		return;
	}
	if(cbc == cbs)
		ntrunc++;
	cbc++;
	if(cbc <= cbs)
		null(c);
}

block(cc)
{
	register c;

	c = cc;
	if(cflag&UCASE && islower(c))
		c = toupper(c);
	if(cflag&LCASE && isupper(c))
		c = tolower(c);
	c &= 0377;
	if(cbs == 0) {
		null(c);
		return;
	}
	if(cc == '\n') {
		while(cbc < cbs) {
			null(' ');
			cbc++;
		}
		cbc = 0;
		return;
	}
	if(cbc == cbs)
		ntrunc++;
	cbc++;
	if(cbc <= cbs)
		null(c);
}

flatten(cc)
register int cc;
{
	register nlc;
	static int lastc = 0;

	if (lastc) {
		_NCdec2(lastc, cc, nlc);
		cnull(NCflatchr(nlc));
		lastc = 0;
	} else if (NCisshift(cc))
		lastc = cc;
	else if (0x80 <= cc)
		cnull(NCflatchr(cc));
	else
		cnull(cc);
}

/*  Data shared between tonls() and nlsterm() */

static char nl_seq[NLESCMAX-1] = "\0";
static int nl_snx = 0;
static char nl_mne[NLESCMAX-1] = "\0";
static int nl_mnlen = 0;
static int nl_state = 0;
static NLchar nl_nlc;

#define XLATE	8
#define BADCODE	9
#define SUCSTAT	10

tonls(ic)
int ic;
{
	register int cc, i, xc;
	int tmpI;
#define sgetc()		((!stackempty()) ? unstackc() : ic)
#define stackc(c)	(nl_seq[nl_snx++] = (c))
#define unstackc()	(nl_seq[--nl_snx])
#define stackempty()	(!nl_snx)

	for (; ; ) {
		cc = sgetc();
		switch (nl_state)
			{
		case 0:
			nl_mnlen = 0;
			if (cc != '\\') {
				if (!stackempty())
					cnull(cc);
				break;
			}
			nl_state = 1;
			break;
		case 1:
			if (cc != '<') {
				null('\\');
				stackc(cc);
				nl_state = 0;
				break;
			}
			nl_state = 2;
			break;
		case 2:
		case 3:
			if (cc != '>' && nl_mnlen < NLESCMAX ) {
				nl_mne[nl_mnlen++] = cc;
				break;
			}

#ifdef KJI
			if ( 4 == nl_mnlen && 1 == ishexesc(nl_mne) ) {
				nl_nlc =
					(atohex(nl_mne[0]) & 0xf) << 12 |
					(atohex(nl_mne[1]) & 0xf) << 8 |
					(atohex(nl_mne[2]) & 0xf) << 4 |
					(atohex(nl_mne[3]) & 0xf);
				nl_state = SUCSTAT;
				break;
			}
#endif

			if (0 <= _NLunescval(nl_mne, nl_mnlen, &nl_nlc))
				nl_state = (cc == '>') ? SUCSTAT : BADCODE;
			else {
				null('\\'), null('<');
				do {
					nl_mnlen--;
					stackc(nl_mne[nl_mnlen]);
				} while ( nl_mnlen > 0 );
				nl_state = 0;
			}
			break;
		case BADCODE:
			if (cc != '>') {
				null('\\'), null('<');
				stackc(nl_mne[1]);
				stackc(nl_mne[0]);
				nl_state = 0;
				break;
			}
			nl_state = SUCSTAT;
			break;
		case SUCSTAT:
			for (i = 0, xc = NCenc(&nl_nlc, nl_mne); i < xc; ++i)
				null(nl_mne[i]);
			stackc(cc);
			nl_state = 0;
			break;
			}
		if (stackempty())
			break;
	}
	if (!nl_state)
		cnull(cc);
}

nlsterm()
{
	int i, xc;

	flsh();
	op = obuf;
	switch (nl_state)
	{
	case 0:
		break;
	case 1:
		null('\\');
		break;
	case 2:
		null('\\'), null('<');
		break;
	case 3:
		null('\\'), null('<'), null(nl_mne[0]);
		break;
	case 4:
		null('\\'), null('<'), null(nl_mne[0]), null(nl_mne[1]);
		break;
	case SUCSTAT:
		xc = NCenc(&nl_nlc, nl_mne);
		for (i = 0; i < xc; ++i)
			null(nl_mne[i]);
		nl_state = 0;
		break;
	}
	flsh();
}

fromnls(cc)
register int cc;
{
	char esc[NLESCMAX+1];
	NLchar nlc;
	register int i;
	static int lastc = 0;

	if (lastc) {
		_NCdec2(lastc, cc, nlc);
		lastc = 0;
	} else if (NCisshift(cc))
		lastc = cc;
	else
		nlc = cc;
	if (!lastc) {
		esc[i = NCesc(&nlc, esc)] = '\0';
		for (cc = 0; cc < i; ++cc)
			null(esc[cc]);
	}
}

void
term(c)
{
	nlsterm();
	stats();
	exit(c);
}

stats()
{

	fprintf(stderr, MSGSTR(RECSIN, "%u+%u records in\n"), nifr, nipr);
	fprintf(stderr, MSGSTR(RECSOUT, "%u+%u records out\n"), nofr, nopr);
	if(ntrunc)
		fprintf(stderr,
			MSGSTR(TRUNRECS, "%u truncated records\n"), ntrunc);
}
