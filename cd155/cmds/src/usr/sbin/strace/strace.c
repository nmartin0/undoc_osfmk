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
static char rcsid[] = "@(#)$RCSfile: strace.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:16:53 $";
#endif

/** Copyright (c) 1988  Mentat Inc.
 ** strace.c 1.5, last change 1/12/90
 **/


#include <stropts.h>
#include <sys/strlog.h>
#include <stdio.h>

extern	char	* ctime(   time_t * clock   );

extern	int	errno;
#ifdef NETWARE
extern	int	daylight;
extern	int	timezone;
#endif

main (argc, argv)
	int	argc;
	char	** argv;
{
	char	buf[LOGMSGSZ];
	char	ch;
	char	* cp;
	char	** cpp;
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	int	fd;
	int	flags;
	int	i1;
	struct log_ctl	logctl;
	struct strioctl	stri;
	struct trace_ids trace1;

	if ((fd = open("/dev/strlog", 2)) == -1) {
		perror("couldn't open log device");
		exit(1);
	}
	stri.ic_cmd = I_TRCLOG;
	stri.ic_timout = -1;
	stri.ic_dp = (char *)&trace1;
	stri.ic_len = sizeof(trace1);
	if (argc > 1) {
		if (((argc - 1) % 3) != 0) {
			fprintf(stderr, "usage: [ mid sid level] ...\n");
			exit(1);
		}
		for (i1 = 1, cpp = &argv[1]; i1 < argc; i1 += 3) {
			if (strcmp(cpp[0], "all") == 0)
				trace1.ti_mid = -1;
			else
				trace1.ti_mid = atoi(cpp[0]);
			if (strcmp(cpp[1], "all") == 0)
				trace1.ti_sid = -1;
			else
				trace1.ti_sid = atoi(cpp[1]);
			if (strcmp(cpp[2], "all") == 0)
				trace1.ti_level = (char)-1;
			else
				trace1.ti_level = atoi(cpp[2]);
			if (ioctl(fd, I_STR, &stri) == -1) {
				fprintf(stderr, "ERROR: strace already running\n");
				exit(1);
			}
		}
	} else {
		trace1.ti_mid = -1;
		trace1.ti_sid = -1;
		trace1.ti_level = (char)-1;
		if (ioctl(fd, I_STR, &stri) == -1) {
			fprintf(stderr, "ERROR: strace already running\n");
			exit(1);
		}
	}
	ctlbuf.buf = (char *)&logctl;
	ctlbuf.maxlen = sizeof(logctl);
	databuf.buf = buf;
	databuf.maxlen = sizeof(buf);
	flags = 0;
	while (getmsg(fd, &ctlbuf, &databuf, &flags) != -1) {
#ifdef NETWARE
		logctl.ttime += timezone - (daylight * 3600);
#endif
		cp = ctime(&logctl.ttime);
		cp[strlen(cp)-6] = '\0';
		cp += 8;
		if (logctl.flags & SL_FATAL)
			ch = 'F';
		else if (logctl.flags & SL_ERROR)
			ch = 'E';
		else
			ch = ' ';
		printf("%d %s %u %d %c", logctl.seq_no, cp,
			logctl.ltime, logctl.level, ch);
		printf(".... %d %d %s\n", logctl.mid, logctl.sid, buf);
		fflush(stdout);
	}
}
