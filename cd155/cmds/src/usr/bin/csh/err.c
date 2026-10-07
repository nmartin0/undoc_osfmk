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
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: err.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:27 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: error Perror itoa perror bferr seterr seterr2 seterrc
 *
 * ORIGINS: 10,26,27
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
 * (Copyright statements and/or associated legends of other
 * companies whose code appears in any part of this module must
 * be copied here.)
 */

/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 

#include <sys/ioctl.h>
#include "sh.h"

/*
 * C Shell
 */

bool	errspl;			/* Argument to error was spliced by seterr2 */
uchar_t	one[2] = { '1', 0 };
uchar_t	*onev[2] = { one, NOSTR };
/*
 * Print error string s
 * This routine always resets or exits.  The flag haderr
 * is set so the routine who catches the unwind can propogate
 * it if they want.
 *
 * Note that any open files at the point of error will eventually
 * be closed in the routine process in sh.c which is the only
 * place error unwinds are ever caught.
 */
error(s)
uchar_t *s;
{
	register uchar_t **v;
	register uchar_t *ep;

	/*
	 * Must flush before we print as we wish output before the error
	 * to go on (some form of) standard output, while output after
	 * goes on (some form of) diagnostic output.
	 * If didfds then output will go to 1/2 else to FSHOUT/FSHDIAG.
	 * See flush in sh.print.c.
	 */
	flush();
	haderr = 1;		/* Now to diagnostic output */
	timflg = 0;		/* This isn't otherwise reset */
	if (v = pargv)
		pargv = 0, blkfree(v);
	if (v = gargv)
		gargv = 0, blkfree(v);

	/*
	 * A zero arguments causes no printing, else print
	 * an error diagnostic here.
	 */
	if (s){
		printf("%s.\n", (char *)s); 
	}

	didfds = 0;		/* Forget about 0,1,2 */
	if ((ep = err) && errspl) {
		errspl = 0;
		xfree(ep);
	}
	errspl = 0;

	/*
	 * Go away if -e or we are a child shell
	 */

	if (exiterr || child)
		exit(1);

	/*
	 * Reset the state of the input.
	 * This buffered seek to end of file will also
	 * clear the while/foreach stack.
	 */
	btoeof();

	setq("status", onev, &shvhed);
	if (tpgrp > 0)  {
	    struct sigvec nsv, osv;
		
	    nsv.sv_handler = SIG_IGN;
	    nsv.sv_mask = SA_RESTART;
	    nsv.sv_onstack = 0;
	    (void)sigvec(SIGTTOU, &nsv, &osv);
	    ioctl(FSHTTY, TIOCSPGRP, &tpgrp);
	    (void)sigvec(SIGTTOU, &osv, (struct sigvec *)0);
	}
	reset();		/* Unwind */
}

/*
 * Perror is the shells version of perror which should otherwise
 * never be called.
 */
Perror(s)
	uchar_t *s;
{
	/*
	 * Perror uses unit 2, thus if we didn't set up the fd's
	 * we must set up unit 2 now else the diagnostic will disappear
	 */
	if (!didfds) {
		register int oerrno = errno;

		dcopy(SHDIAG, 2);
		errno = oerrno;
	}
	l_perror(s);
	error(NOSTR);		/* To exit or unwind */
}

itoa (n,s)
  uchar_t *s;
  int n;

{
  int i, j, sign;
  uchar_t c;

  if ((sign = n) < 0)
    n = -n;

  i = 0;
  do
    s[i++] = n % 10 + '0';
  while ((n /= 10) > 0);

  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';

  j=strlen(s)-1; 
  for (i=0; i<j; i++)
  {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
    j--;
  }

}

/*
 *
 * Print the error indicated
 * in the cerror cell.
 */

extern int sys_nerr;
extern char *sys_errlist[];

l_perror(s)
uchar_t	*s;
{
	uchar_t buf[NL_TEXTMAX];
	uchar_t *c, tmpstr[80];
	register int n;

	c = &buf[0];

	if( (errno < sys_nerr) && (errno > 0) )
		c = (uchar_t *)sys_errlist[errno];
	else    {
                sprintf((char *)buf, MSGSTR(M_NOERR, "Unknown error %d occurred.\n"),
                        errno); 
                c = buf;
                }

	n = strlen(s);
	if(n) {
		(void) write(2, s, (unsigned)n);
		(void) write(2, ": ", 2);
	}
	(void) write(2, c, (unsigned)strlen(c));
	(void) write(2, "\n", 1);
}

bferr(cp)
	uchar_t *cp;
{

	flush();
	haderr = 1;
	printf("%s: ", bname);
	error(cp);
}

/*
 * The parser and scanner set up errors for later by calling seterr,
 * which sets the variable err as a side effect; later to be tested,
 * e.g. in process.
 */
seterr(s)
	uchar_t *s;
{

	if (err == 0)
		err = s, errspl = 0;
}

/* Set err to a splice of cp and dp, to be freed later in error() */
seterr2(cp, dp)
	uchar_t *cp, *dp;
{

	if (err)
		return;
	err = strspl(cp, dp);
	errspl++;
}

/* Set err to a splice of cp with a string form of character d */
seterrc(cp, d)
	uchar_t *cp, d;
{
	uchar_t chbuf[2];

	chbuf[0] = d;
	chbuf[1] = 0;
	seterr2(cp, chbuf);
}
