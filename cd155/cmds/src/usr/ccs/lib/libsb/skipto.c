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
/*
 *  skipover and skipto -- skip over characters in string
 *
 *  Usage:	p = skipto (string,charset);
 *		p = skipover (string,charset);
 *
 *  char *p,*charset,*string;
 *
 *  Skipto returns a pointer to the first character in string which
 *  is in the string charset; it "skips until" a character in charset.
 *  Skipover returns a pointer to the first character in string which
 *  is not in the string charset; it "skips over" characters in charset.
 */

static unsigned char tab[256] = {
	0};

char *skipto (string,charset)
unsigned char *string, *charset;
{
	register unsigned char *setp,*strp;

	tab[0] = 1;		/* Stop on a null, too. */
	for (setp=charset;  *setp;  setp++) tab[*setp]=1;
	for (strp=string;  tab[*strp]==0;  strp++)  ;
	for (setp=charset;  *setp;  setp++) tab[*setp]=0;
	return ((char *)strp);
}

char *skipover (string,charset)
unsigned char *string, *charset;
{
	register unsigned char *setp,*strp;

	tab[0] = 0;		/* Do not skip over nulls. */
	for (setp=charset;  *setp;  setp++) tab[*setp]=1;
	for (strp=string;  tab[*strp];  strp++)  ;
	for (setp=charset;  *setp;  setp++) tab[*setp]=0;
	return ((char *)strp);
}
