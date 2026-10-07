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
 * HISTORY
 * $Log: jctype.h,v $
 * Revision 1.3  1994/11/18  21:09:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:14:38  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:38  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:16:05  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:45  nandy
 * Initial revision
 *
 * Revision 1.5  92/04/13  14:35:53  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.5  90/10/31  15:15:30  devrcs
 * 	add #include <standardarg.h>
 * 	[90/10/05  14:03:54  kumi]
 * 
 * 	add external functions declare for sjis awk
 * 	[90/08/13  16:36:49  kumi]
 * 
 * Revision 1.4  90/10/07  20:28:15  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:46:36  gm]
 * 
 * Revision 1.3  90/04/27  23:06:31  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/16  13:47:54  tom]
 * 
 * Revision 1.2  90/03/13  21:22:26  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:21:40  tom]
 * 
 * $EndLog$
 */
/* @(#)jctype.h 1.5  com/inc,3.1,9013 2/27/90 22:08:29 */
/*
 * COMPONENT_NAME: jctype.h
 *                                                                    
 * ORIGIN: IBM
 *
 * Copyright International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */                                                                   

/*	jctype - Japanese (SJIS) extensions to ctype
 *						rcd  12-Apr-88
 */

#ifndef _JCTYPE_H_
#define _JCTYPE_H_

#include <standards.h>

extern unsigned int	_jistoatab[][91];
extern unsigned short	_atojistab[];
extern unsigned char	_jctype0_[], _jctype1_[][256];

#define _tojupper(c)	((c)-0x21)
#define _tojlower(c)	((c)+0x21)

/* Note that encodings are not strictly bit-per-attribute. */
#define	_J1	0xc0		/* field 1: japanese-symbol mask */
#define	_Jk	0x40		/* katakana */
#define	_JH	0x80		/* hiragana */
#define	_JK	0xc0		/* kanji */

#define	_JA	0x10		/* upper case alphabetic */
#define	_Ja	0x20		/* lower case alphabetic */

#define	_J2	0x0c		/* field 2: hex/digit/punct mask */
#define	_JX	0x04		/* hex-digit flag */
#define	_JD	0x0c		/* digit = numeral and hex */
#define	_JB	0x14		/* upper case hex letter */
#define	_Jb	0x24		/* lower case hex letter */

#define	_JP	0x08		/* punct (looks like non-hex numeral) */
#define	_JG	0x02		/* other graphic character */
#define	_JR	0x01		/* reserved (unassigned but valid) char */

#ifndef lint

#define	_jattr(c)	(_jctype1_[_jctype0_[(unsigned short)(c)>>8]][(c)&0xff])
#define	_legaltop(c)	(_jctype0_[c] > 1)
#define	_legalbot(c)	(_jctype1_[7][c] != 0)

#define isj1bytekana(c) (NCchrlen(c) == 1 && isjkata(c))
#endif /* NOT lint */
#ifdef   _ANSI_C_SOURCE
#ifdef _NO_PROTO
extern int isjalpha();
extern int isjalnum();
extern int isjdigit();
extern int isjpunct();
extern int isjprint();
extern int isjspace();
extern int isjgraph();
extern int isjxdigit();
extern int isjlower();
extern int isjupper();
extern int isjparen();
extern int isjkanji();
extern int isjhira();
extern int isjkata();
extern int tojupper();
extern int tojlower();

#else /* _NO_PROTO */

extern int isjalpha(int);
extern int isjalnum(int);
extern int isjdigit(int);
extern int isjgraph(int);
extern int isjprint(int);
extern int isjpunct(int);
extern int isjspace(int);
extern int isjxdigit(int);
extern int isjupper(int);
extern int isjlower(int);
extern int isjparen(int);
extern int isjkanji(int);
extern int isjhira(int);
extern int isjkata(int);
extern int tojlower(int);
extern int tojupper(int);

#endif /* _NO_PROTO */
#endif /*  _ANSI_C_SOURCE */

#endif /* _JCTYPE_H_ */
