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
 * OSF/1 Release 1.0.2
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: acu.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:42:31 $";
#endif
/*
acu.c	1.6  com/cmd/tip,3.1,9013 12/21/89 16:51:33";
 */
/* 
 * COMPONENT_NAME: UUCP acu.c
 * 
 * FUNCTIONS: MSGSTR, acuabort, acutype, connect, disconnect 
 *
 * ORIGINS: 10  26  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

/* Static char sccsid[] = "acu.c	5.3 (Berkeley) 4/3/86"; */

#include "tip.h"

static acu_t *acu = NOACU;
static int conflag;
static int acuabort();
static acu_t *acutype();
static jmp_buf jmpbuf;

/*
 * Establish connection for tip
 *
 * If DU is true, we should dial an ACU whose type is AT.
 * The phone numbers are in PN, and the call unit is in CU.
 *
 * If the PN is an '@', then we consult the PHONES file for
 *   the phone numbers.  This file is /etc/phones, unless overriden
 *   by an exported shell variable.
 *
 * The data base files must be in the format:
 *	host-name[ \t]*phone-number
 *   with the possibility of multiple phone numbers
 *   for a single host acting as a rotary (in the order
 *   found in the file).
 */
char *
connect()
{
	register char *cp = PN;
	char *phnum, string[256];
	FILE *fd;
	int tried = 0;

	if (!DU) {		/* regular connect message */
		if (CM != NOSTR)
			pwrite(FD, CM, size(CM));
		logent(value(HOST), "", DV, MSGSTR(CALLCOMPLETE, "call completed"));
		return (NOSTR);
	}
	/*
	 * @ =>'s use data base in PHONES environment variable
	 *        otherwise, use /etc/phones
	 */
	signal(SIGINT, (void(*)(int)) acuabort);
	signal(SIGQUIT, (void(*)(int)) acuabort);
	if (setjmp(jmpbuf)) {
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		printf(MSGSTR(CALLABORT, "\ncall aborted\n")); /*MSG*/
		logent(value(HOST), "", "", MSGSTR(CALLABORT2, "call aborted")); /*MSG*/
		if (acu != NOACU) {
			boolean(value(VERBOSE)) = FALSE;
			if (conflag)
				disconnect(NOSTR);
			else
				(*acu->acu_abort)();
		}
		return( MSGSTR(INTERRUPT, "interrupt"));
	}
	if ((acu = acutype(AT)) == NOACU)
		return (MSGSTR(UNKNOWNACU, "unknown ACU type")); /*MSG*/
	if (*cp != '@') {
		while (*cp) {
			for (phnum = cp; *cp && *cp != ','; cp++)
				;
			if (*cp)
				*cp++ = '\0';
			
			if (conflag = (*acu->acu_dialer)(phnum, CU)) {
				logent(value(HOST), phnum, acu->acu_name,
					MSGSTR(CALLCOMPLETE, "call completed")); /*MSG*/
				return (NOSTR);
			} else
				logent(value(HOST), phnum, acu->acu_name,
					MSGSTR(CALLFAIL, "call failed")); /*MSG*/
			tried++;
		}
	} else {
		if ((fd = fopen(PH, "r")) == (FILE *)TIP_NOFILE) {
			printf("%s: ", PH);
			return (MSGSTR(NOPNFILE, "can't open phone number file")); /*MSG*/
		}
		while (fgets(string, sizeof(string), fd) != NOSTR) {
			for (cp = string; !any(*cp, " \t\n"); cp++)
				;
			if (*cp == '\n') {
				fclose(fd);
				return (MSGSTR(UNRECHOST, "unrecognizable host name")); /*MSG*/
			}
			*cp++ = '\0';
			if (strcmp(string, value(HOST)))
				continue;
			while (any(*cp, " \t"))
				cp++;
			if (*cp == '\n') {
				fclose(fd);
				return (MSGSTR(NOPN, "missing phone number")); /*MSG*/
			}
			for (phnum = cp; *cp && *cp != ',' && *cp != '\n'; cp++)
				;
			if (*cp)
				*cp++ = '\0';
			
			if (conflag = (*acu->acu_dialer)(phnum, CU)) {
				fclose(fd);
				logent(value(HOST), phnum, acu->acu_name,
					MSGSTR(CALLCOMPLETE, "call completed")); /*MSG*/
				return (NOSTR);
			} else
				logent(value(HOST), phnum, acu->acu_name,
					MSGSTR(CALLFAIL, "call failed")); /*MSG*/
			tried++;
		}
		fclose(fd);
	}
	if (!tried)
		logent(value(HOST), "", acu->acu_name, MSGSTR(NOPN, "missing phone number")); /*MSG*/
	else
		(*acu->acu_abort)();
	return (tried ? MSGSTR(CALLFAIL, "call failed") : MSGSTR(NOPN, "missing phone number")); /*MSG*/ /*MSG*/
}

disconnect(reason)
	char *reason;
{
	if (!conflag) {
		logent(value(HOST), "", DV, MSGSTR(CALLTERM, "call terminated")); /*MSG*/ 
		return;
	}
	if (reason == NOSTR) {
		logent(value(HOST), "", acu->acu_name, MSGSTR(CALLTERM, "call terminated")); /*MSG*/
		if (boolean(value(VERBOSE)))
			printf(MSGSTR(DISCONNECTING, "\r\ndisconnecting...")); /*MSG*/
	} else 
		logent(value(HOST), "", acu->acu_name, reason);
	(*acu->acu_disconnect)();
}

static int
acuabort(s)
{
	signal(s, SIG_IGN);
	longjmp(jmpbuf, 1);
}

static acu_t *
acutype(s)
	register char *s;
{
	register acu_t *p;
	extern acu_t acutable[];

	for (p = acutable; p->acu_name != '\0'; p++)
		if (!strcmp(s, p->acu_name))
			return (p);
	return (NOACU);
}
