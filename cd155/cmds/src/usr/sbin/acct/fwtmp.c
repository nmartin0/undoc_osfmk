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
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: fwtmp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:16 $";
#endif

/*
 * COMPONENT_NAME: (CMDACCT) Command Accounting
 *
 * FUNCTIONS: inp
 *
 * ORIGINS: 3,9,27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
#if !lint
static char sccsid[] = "fwtmp.c	1.2  com/cmd/acct,3.1,8943 10/24/89 10:56:09";
#endif

# include <stdio.h>
# include <sys/types.h>
# include "acctdef.h"
# include <sys/acct.h>
# include <locale.h>

static int	inp();
struct	utmp	Ut;
char    timbuf[BUFSIZ];
struct tm *localtime();

#define OFMT "%-*.*s %-*.*s %-*.*s %*.*hd %*.*hd %*.*ho %*.*ho %ld %-*.*s %s"

main ( c, v, e )
char	**v, **e;
int	c;
{

	int	iflg,cflg;

	(void) setlocale (LC_ALL,"");
	iflg = cflg = 0;

	if ( --c > 0 ) {
		if(**++v == '-') while(*++*v) switch(**v){
		case 'c':
			cflg++;
			continue;
		case 'i':
			iflg++;
			continue;
		}
	}

	for(;;){
		if(iflg){
			if(inp(stdin,&Ut) == EOF)
				break;
		} else {
			if(fread(&Ut,sizeof Ut, 1, stdin) != 1)
				break;
		}
		if(cflg)
			(void)fwrite(&Ut,sizeof Ut, 1, stdout);
		else {
	/*
	 * The utmp string elements use the whole array, so there need not be
	 * a trailing \0. Therefore string output is EXACTLY the field size. 
	 */
			(void)strftime(timbuf, BUFSIZ, "%c %Z %n", 
				localtime(&Ut.ut_time));
			(void)printf( OFMT,
				NSZ, NSZ, Ut.ut_user,
				ISZ, ISZ, Ut.ut_id,
				LSZ, LSZ, Ut.ut_line,
				PSZ, PSZ, Ut.ut_pid,
				TSZ, TSZ, Ut.ut_type,
				ETSZ, ETSZ, Ut.ut_exit.e_termination,
				EESZ, EESZ, Ut.ut_exit.e_exit,
				Ut.ut_time,
				HSZ, HSZ, Ut.ut_host,
				timbuf);
		}
	}
	return ( 0 );
}

static
inp(file, u)
FILE *file;
register struct utmp *u;
{

	char	buf[BUFSIZ];
	register char *p;
	register int i, len;

	if(fgets((p = buf), BUFSIZ, file)==NULL)
		return EOF;
	len = strlen(p);

	/*
	 * There can be empty input strings! So we can't use sscanf("..%s...").
	 * Fill the strings with \0 from the end.
	 */
	strncpy(u->ut_user, p, NSZ);
	for( i = NSZ-1; i >= 0 && u->ut_user[i] == ' '; i-- ) u->ut_user[i] = '\0';
        p += (NSZ+1);
        if((p-buf) > len)       return 0;

        strncpy(u->ut_id, p, ISZ);
	for( i = ISZ-1; i >= 0 && u->ut_id[i] == ' '; i-- ) u->ut_id[i] = '\0';
        p += (ISZ+1);
        if((p-buf) > len)       return 0;

        strncpy(u->ut_line, p, LSZ);
	for( i = LSZ-1; i >= 0 && u->ut_line[i] == ' '; i-- ) u->ut_line[i] = '\0';
        p += (LSZ+1);
        if((p-buf) > len)       return 0;

        u->ut_pid = (pid_t) atol(p);
        p += (PSZ+1);
        if((p-buf) > len)       return 0;

        u->ut_type = (short) atoi(p);
        p += (TSZ+1);
        if((p-buf) > len)       return 0;

        u->ut_exit.e_termination = (short) atoi(p);
        p += (ETSZ+1);
        if((p-buf) > len)       return 0;

        u->ut_exit.e_exit = (short) atoi(p);
        p += (EESZ+1);
        if((p-buf) > len)       return 0;

        u->ut_time = atol(p);
        p += (TISZ+1);
        if((p-buf) > len)       return 0;

        strncpy(u->ut_host, p, HSZ);
	for( i = HSZ-1; i >= 0 && u->ut_host[i] == ' '; i-- ) u->ut_host[i] = '\0';
	return 0;
}
