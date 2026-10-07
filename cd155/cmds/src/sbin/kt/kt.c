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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/kt/kt.c,v 1.2 1994/11/21 16:42:03 mtm Exp $
 *
 *
 */

#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <mach/kern_return.h>

#define DOCALLS 1

extern int optind;
extern char *optarg;

int	verbose;
int	node;
int	chan;
int	ftty;
int	raw_flag;

struct termios termios_cooked, termios_raw;

#define SPECIAL	'~'
#define FINISH	'.'

die()
{
	int	st;

#if DOCALLS
	st = mcmsg_console_close(chan);
#else
st = KERN_SUCCESS;
#endif
	if (st != KERN_SUCCESS) {
		kern_error("mcmsg_console_close", st, "failed");
	}
	if (raw_flag) {
		st = tcsetattr(ftty, TCSADRAIN, &termios_cooked);
		if (st == -1) {
			perror("tcsetattr tty");
		}
	}
	exit(0);
}

main(argc, argv)
	int	argc;
	char	**argv;
{
	int	c;
	int	st;
	int	t;
	int	f;
	int	sflag;

	optind = 1;
	while ((c = getopt(argc, argv, "v")) != EOF) {

		switch (c) {

		case 'v':
			verbose++;
			break;

		case '?':
usage:
			fprintf(stderr, "Usage: kt [-v] <node>\n");
			exit(1);
		}
	}
	if (argc != optind + 1) {
		goto usage;
	}
	node = atoi(argv[optind]);
#if DOCALLS
	st = mcmsg_console_open(node, &chan);
#else
st = KERN_SUCCESS; chan = 9;
#endif
	if (st != KERN_SUCCESS) {
		kern_error("mcmsg_console_open", st, "no channel available");
		exit(1);
	}

	sigset(SIGHUP, die);
	sigset(SIGINT, die);
	sigset(SIGQUIT, die);
	sigset(SIGABRT, die);
	sigset(SIGTERM, die);

	if (verbose) {
		printf("Open node %d on channel %d\n", node, chan);
		fflush(stdout);
	}

	f = open("/dev/tty", O_RDONLY | O_NDELAY, 0);
	if (f == -1) {
		perror("open tty");
		die();
	}
	if (verbose) {
		printf("Input now NDELAY\r\n");
		fflush(stdout);
	}
	st = tcgetattr(f, &termios_cooked);
	if (st == -1) {
		perror("tcgetattr tty");
		die();
	}
	ftty = f;

	raw_flag = 1;
	termios_raw = termios_cooked;
	termios_raw.c_iflag = IGNBRK | IGNPAR | ISTRIP;
	termios_raw.c_oflag = 0;
	termios_raw.c_lflag = 0;
	st = tcsetattr(f, TCSAFLUSH, &termios_raw);
	if (st == -1) {
		perror("tcsetattr tty");
		die();
	}
	if (verbose) {
		printf("Input now raw\r\n");
		fflush(stdout);
	}

	if (verbose) {
		printf("Enter read loop\r\n");
		fflush(stdout);
	}
	sflag = 1;
	for (;;) {

		c = 0;
		st = read(f, &c, 1);
		if (st == 1) {
			if (sflag == 1 && c == SPECIAL) {
				sflag = 2;
			} else {
				if (sflag == 2) {
					if (c == FINISH) {
						die();
					}
				}
				sflag = 0;
#if DOCALLS
				st = mcmsg_console_write(chan, c);
#else
st = KERN_SUCCESS;
#endif
				if (st != KERN_SUCCESS) {
					kern_error("mcmsg_console_write",
					           st, "no buffer space");
					die();
				} else if (st == -1 && errno != EAGAIN) {
					perror("read tty");
					die();
				}
			}
			if (c == '\r' || c == '\n') {
				sflag = 1;
			}
		}

#if DOCALLS
		st = mcmsg_console_read(chan, &c);
#else
st = KERN_FAILURE;
#endif
		if (st == KERN_FAILURE) {
#if 0
			flick();
#endif
			continue;
		}
		if (st != KERN_SUCCESS) {
			kern_error("mcmsg_console_read", st, "not ready");
			die();
		}
		putchar(c);
		fflush(stdout);
	}
}

kern_error(s, v, f)
	char	*s;
	int	v;
	char	*f;
{

	switch (v) {

	case KERN_FAILURE:
		fprintf(stderr, "%s: %s\n", s, f);
		break;

	case KERN_NO_ACCESS:
		fprintf(stderr, "%s: invalid channel\n", s);
		break;

	case KERN_INVALID_ARGUMENT:
		fprintf(stderr, "%s: invalid argument\n", s);
		break;

	default:
		fprintf(stderr, "%s: kernel error %d\n", s, v);
	}
}
