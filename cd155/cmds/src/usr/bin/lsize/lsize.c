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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/lsize/lsize.c,v 1.5 1994/11/19 01:29:52 mtm Exp $
 *
 * HISTORY
 * $Log: lsize.c,v $
 * Revision 1.5  1994/11/19  01:29:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/02/25  16:51:30  brad
 * Merged revision 1.2.16.2 from the R1.2 branch.
 *
 * Revision 1.3  1994/01/08  01:03:11  brad
 * Merge of bug fix for PTS #5958 from R1.2 branch.
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <errno.h>
#include <nx.h>

/*
 * Macros for manipulating extended offsets and sizes.
 */

/*
 * LESS:
 *      Returns TRUE if extended number e1 is less than extended number e2.
 *
 */
#define LESS(e1, e2) (_ecmp((e1), (e2)) < 0)

/*
 * GREATER:
 *      Returns TRUE if extended number e1 is greater than extended number e2.
 *
 */
#define GREATER(e1, e2) (_ecmp((e1), (e2)) > 0)

/*
 * EQUAL:
 *      Returns TRUE if extended number e1 is equal to extended number e2.
 *
 */
#define EQUAL(e1, e2) (_ecmp((e1), (e2)) == 0)

esize_t         e_neg_one      = { -1, -1 };           /* extended -1 */

extern char	*optarg;
extern int	optind;

esize_t		checksize();
extern esize_t	_eadd1();


/*
 * lsize
 *
 * The lsize command sets the size of the file(s) specified on the command
 * line, preallocating disk storage if necessary (if the size of the file
 * is increasing).  If the file(s) does not already exist, the lsize command
 * creates it.
 */
main(argc, argv)
int argc;
char **argv;
{
	int	 	status = 0;
	int		cp, fd;
	int		new_file, aflag = 0;
	esize_t		offset, target_size, new_size;
	struct estat	statb;
	char		sp[20];

	while ((cp = getopt(argc, argv, "a")) != EOF) {
		switch (cp) {
		case 'a':
			aflag++;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 2)
		usage();

	offset = checksize(*argv++);

	for (; --argc > 0; argv++) {
		new_file = 0;
		fd = open(*argv, O_WRONLY);
		if (fd < 0) {
			if (errno != ENOENT) {
				perror("lsize");
				status = 1;
				continue;
			}
			fd = creat(*argv, 0666);
			if (fd < 0) {
				perror("lsize");
				status = 1;
				continue;
			} else {
				new_file = 1;
			}
		}

		/*
		 * If we are extending a pre-existing file, compute the target
		 * size.  This is used not only to size the file, but to 
		 * perform error checking after the file is sized.
		 */
		target_size = offset;
		if ((aflag) && (!new_file)) {
			if (_festat(fd, &statb) != 0) {
				perror(*argv);
				status = 1;
				continue;
			}

			target_size = _eadd(target_size, statb.st_size);
			if ((EQUAL(target_size, e_neg_one)) &&
			    (errno != ESUCCESS)) {
				perror(*argv);
				close(fd);
				if (new_file)
					unlink(*argv);
				status = 1;
				continue;
			}
		}

		new_size = _esize(fd, target_size, SIZE_SET);
		if ((EQUAL(new_size, e_neg_one)) && (errno != ESUCCESS)) {
			perror(*argv);
			close(fd);
			if (new_file)
				unlink(*argv);
			status = 1;
			continue;
		}
				
		if (LESS(new_size, target_size)) {
			if ( _etos(new_size, sp) < 0) {
				perror(*argv);
				status = 1;
				continue;
			}
			fprintf(stderr,
				"lsize: No space left on device, %s bytes allocated for %s.\n",
				sp, *argv);
		}

		close(fd);
	}

	exit(status);
}


esize_t checksize(s)
char	*s;
{
	esize_t	t, tmp;
	char	*p;
	int	eflg;

	t.slow = 0;
	t.shigh = 0;
	tmp = t;
	p = s;
	eflg = 0;

	while (*p >= '0' && *p <= '9') {
		t = _eadd1(_emul(t, 10), *p++ - '0');
		if (LESS(t, tmp)) {	/* Check for overflow */
			fprintf(stderr, "lsize: Size %s is too large.\n", s);
			exit(2);
		}
		tmp = t;
	}
	if (*p == 'k' || *p == 'K') {
		t = _emul(t, 1024);
		if (*(p+1) != '\0')
			eflg = 1;
	}
	else if (*p == 'm' || *p == 'M') {
		t = _emul(t, 1048576);
		if (*(p+1) != '\0')
			eflg = 1;
	}
	else if (*p == 'g' || *p == 'G') {
		t = _emul(t, 1073741824);
	     	if (*(p+1) != '\0')
			eflg = 1;
	}
	else if (*p != '\0') {
			eflg = 1;
	}

	if (eflg) {
		fprintf(stderr, "lsize: Invalid size %s.\n", s);
		exit(2);
	}

	if (t.shigh < 0) {
		fprintf(stderr, "lsize: Size %s is too large.\n", s);
		exit(2);
	}

	return t;
}


usage()
{
	fprintf(stderr, "usage: lsize [-a] size[k|m|g] file ...\n");
	fprintf(stderr, " [-a]:   Append to the file\n");
	fprintf(stderr, " size:   Size in bytes [kilobytes][megabytes][gigabytes]\n");
	fprintf(stderr, " file:   File name\n");
	exit(1);
}
