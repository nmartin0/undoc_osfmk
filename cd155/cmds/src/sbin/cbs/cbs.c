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
 
#define MAIN
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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/cbs/cbs.c,v 1.2 1994/11/21 16:40:02 mtm Exp $
 *
 *
 */

#include <stdio.h>

/*
 * Exported globals
 */

char		cbs_separator = '?';

int		paragon_mesh_x = 8;
int		paragon_mesh_y = 16;

/*
 * Main program - not used for library module
 */

#ifdef	MAIN

int		columns = 4;

char		*cbstos();

int		cbs_flag;
int		root_flag;

int		no_tables;

extern int	optind;
extern char	*optarg;

main(argc, argv)
	int	argc;
	char	**argv;
{
	int	c;

	while ((c = getopt(argc, argv, "hRCnc:b:x:y:s:w:")) != EOF) {
		switch (c) {

		case 'x':
			paragon_mesh_x = atoi(optarg);
			if (paragon_mesh_y == 0)
				goto syntax;
			if (paragon_mesh_x % 4 != 0)
				goto syntax;
			break;

		case 'c':
			paragon_mesh_x = atoi(optarg)*4;
			if (paragon_mesh_x == 0)
				goto syntax;
			break;

		case 'y':
			paragon_mesh_y = atoi(optarg);
			if (paragon_mesh_y == 0)
				goto syntax;
			if (paragon_mesh_y % 4 != 0)
				goto syntax;
			break;

		case 'b':
			paragon_mesh_y = atoi(optarg)*4;
			if (paragon_mesh_y == 0)
				goto syntax;
			break;

		case 's':
			cbs_separator = *optarg;
			break;

		case 'w':
			columns = atoi(optarg);
			if (columns == 0)
				goto syntax;
			break;

		case 'R':
			root_flag++;
			break;

		case 'C':
			cbs_flag++;
			break;

		case 'n':
			no_tables++;
			break;

		case 'h':
		default:
syntax:
	fprintf(stderr, "Usage: %s [-cbxyswh] [<n>...]\n", argv[0]);
	fprintf(stderr, " -c <cabinets>    Number of cabinets\n");
	fprintf(stderr, " -b <backplanes>  Number of backplanes in each cab\n");
	fprintf(stderr, " -x <mesh_x>      Mesh width (cabinets*4)\n");
	fprintf(stderr, " -y <mesh_y>      Mesh height (backplanes*4)\n");
	fprintf(stderr, "                  x and y must be a multiple of 4\n");
	fprintf(stderr, " -s <separator>   CBS format separator or letter\n");
	fprintf(stderr, " -w <columns>     Number of columns in output\n");
	fprintf(stderr, " -R               Convert to root partition\n");
	fprintf(stderr, " -C               Convert to CBS\n");
	fprintf(stderr, " -n               Don't print tables if no args\n");
	fprintf(stderr, " -h               Help\n");
			exit(1);
		}
	}

	if (optind < argc || no_tables) {
		for (; optind < argc; optind++) {
			if (iscbs(argv[optind])) {
				if (cbs_flag && !root_flag) {
					printf("%s\n", argv[optind]);
				} else if (cbs_flag && root_flag) {
					printf("%5d %s\n",
					 cbs2root(atocbs(argv[optind])),
					 argv[optind]);
				} else {
					printf("%d\n",
					 cbs2root(atocbs(argv[optind])));
				}
			} else {
				if (root_flag && !cbs_flag) {
					printf("%s\n", argv[optind]);
				} else if (cbs_flag && root_flag) {
					printf("%5d %s\n",
					 atoi(argv[optind]),
					 cbstos(root2cbs(atoi(argv[optind]))));
				} else {
					printf("%s\n",
					 cbstos(root2cbs(atoi(argv[optind]))));
				}
			}
		}
	} else {
		if (root_flag) {
			root_table();
		} else if (cbs_flag) {
			cbs_table();
		} else {
			root_table();
			cbs_table();
		}
	}
	exit(0);
}

root_table()
{
	int	row;
	int	column;
	int	root;
	int	cbs;
	int	test;
	char	*s;
	char	c;

	printf("\fCabinets %3d    Backplanes %d          Mesh %d X %d\n\n",
		paragon_mesh_x / 4, paragon_mesh_y / 4,
		paragon_mesh_x, paragon_mesh_y);

	for (row = 0; row < paragon_mesh_x * paragon_mesh_y / columns; row++) {
		for (column = 0; column < columns; column++) {
			root = row  + column *
			      (paragon_mesh_x * paragon_mesh_y / columns);
			cbs = root2cbs(root);
			s = cbstos(cbs);
			printf(" %7d   %-8s", root, s);

			test = atocbs(s);
			if (test != cbs) {
				printf("\natocbs (s='%c') error, returns %d\n",
					cbs_separator, test);
			}
			test = cbs2root(cbs);
			if (test != root) {
				printf("\ncbs2root(%d) error, returns %d\n",
					cbs, test);
			}

			if (!(cbs_separator >= 'B' && cbs_separator <= 'Z' ||
			      cbs_separator >= 'b' && cbs_separator <= 'z')) {
				c = cbs_separator;
				cbs_separator = '?';
				test = atocbs(s);
				if (test != cbs) {
				 printf("\natocbs (s='?') error, returns %d\n",
						test);
				}
				cbs_separator = c;
			}
		}
		printf("\n");
	}
}

root_table2()
{
	int	row;
	int	column;
	int	root;
	int	cbs;
	int	test;
	char	*s;

	printf("\fCabinets %3d    Backplanes %d          Mesh %d X %d\n\n",
		paragon_mesh_x / 4, paragon_mesh_y / 4,
		paragon_mesh_x, paragon_mesh_y);

	for (row = 0; row < paragon_mesh_x * paragon_mesh_y / columns; row++) {
		for (column = 0; column < columns; column++) {
			root = column + row * columns;
			cbs = root2cbs(root);
			s = cbstos(cbs);
			printf(" %7d   %-8s", root, s);
		}
		printf("\n");
	}
}

cbs_table()
{
	int	row;
	int	column;
	int	root;
	int	cbs;
	int	test;
	char	*s;

	printf("\fCabinets %3d    Backplanes %d          Mesh %d X %d\n\n",
		paragon_mesh_x / 4, paragon_mesh_y / 4,
		paragon_mesh_x, paragon_mesh_y);

	for (row = 0; row < paragon_mesh_x * paragon_mesh_y / columns; row++) {
		for (column = 0; column < columns; column++) {
			cbs = paragon_mesh_x * paragon_mesh_y - 1 -
			      (row + column *
				 (paragon_mesh_x * paragon_mesh_y / columns));
			s = cbstos(cbs);
			root = cbs2root(cbs);
			printf("   %-6s   %3d    ", s, root);
		}
		printf("\n");
	}
}
#endif MAIN

root2cbs(root)
	int	root;
{
	int	x;
	int	y;
	int	cab;
	int	bp;
	int	slot;

	x = paragon_mesh_x - (root % paragon_mesh_x) - 1;
	y = paragon_mesh_y - (root / paragon_mesh_x) - 1;
	if (y < 0) {
#if	DEBUG
	printf("root2cbs(%d)  x %d  y %d  return -1\n", root, x, y);
#endif	DEBUG
		return -1;
	}

	cab = x / 4;
	bp = y / 4;
	slot = "ahipbgjocfkndelm"[(y % 4)*4 + (x % 4)] - 'a';
#if	DEBUG
	printf("root2cbs(%d)  x %d  y %d   cab %d  bp %d  slot %d   cbs %d\n",
		root, x, y, cab, bp, slot,
		(cab << 6) | (bp << 4) | slot);
#endif	DEBUG

	return (cab << 6) | (bp << 4) | slot;
}

cbs2root(cbs)
	int	cbs;
{
	int	cab;
	int	bp;
	int	slot;
	int	x;
	int	y;

	if (cbs == -1) {
		return -1;
	}

	cab = cbs >> 6;
	bp = (cbs >> 4) & 3;
	slot = cbs & 15;

	x = paragon_mesh_x - (cab * 4 + slot / 4) - 1;
	y = paragon_mesh_y - (bp * 4 + ("0123321001233210"[slot] - '0')) - 1;

	return	paragon_mesh_x * y + x;
}

char *
cbstos(cbs)
	int	cbs;
{
	static char s[16];
	int	cab;
	int	bp;
	int	slot;
	char	separator;

	if (cbs == -1) {
		return "***";
	}
	if (cbs_separator == '?') {
		separator = 'A';
	} else {
		separator = cbs_separator;
	}

	cab = cbs >> 6;
	bp = (cbs >> 4) & 3;
	slot = cbs & 15;

	if (separator >= 'A' && separator <= 'Z' ||
	    separator >= 'a' && separator <= 'z') {
		sprintf(s, "%d%c%d", cab, bp + separator, slot);
	} else {
		sprintf(s, "%d%c%d%c%d",
			cab, separator, bp, separator, slot);
	}
	return s;
}

iscbs(s)
	char	*s;
{
	char	separator;

	if (*s < '0' || *s > '9') {
		return 0;
	}
	while (*s >= '0' && *s <= '9') {
		s++;
	}
	if (*s == 0) {
		return 0;
	}

	if (cbs_separator == '?' &&
	    *s >= 'A' && *s <= 'Z') {
		separator = 'A';
	} else if (cbs_separator == '?' &&
	           *s >= 'a' && *s <= 'z') {
		separator = 'a';
	} else {
		separator = cbs_separator;
	}

	if (separator >= 'A' && separator <= 'Z' ||
	    separator >= 'a' && separator <= 'z') {
		s++;
		if (*s < '0' || *s > '9') {
			return 0;
		}

	} else {

		if (separator == '?') {
			s++;
		} else {
			if (*s++ != separator) {
				return 0;
			}
		}
		if (*s < '0' || *s > '9') {
			return 0;
		}
		while (*s >= '0' && *s <= '9') {
			s++;
		}
		if (*s == 0) {
			return 0;
		}
		if (separator == '?') {
			s++;
		} else {
			if (*s++ != separator) {
				return 0;
			}
		}
		if (*s < '0' || *s > '9') {
			return 0;
		}
	}

	return 1;
}

atocbs(s)
	char	*s;
{
	int	cab;
	int	bp;
	int	slot;
	char	separator;

	cab = 0;
	if (*s < '0' || *s > '9') {
		return -1;
	}
	while (*s >= '0' && *s <= '9') {
		cab = 10*cab + *s++ - '0';
	}
	if (*s == 0) {
		return -1;
	}

	if (cbs_separator == '?' &&
	    *s >= 'A' && *s <= 'Z') {
		separator = 'A';
	} else if (cbs_separator == '?' &&
	           *s >= 'a' && *s <= 'z') {
		separator = 'a';
	} else {
		separator = cbs_separator;
	}

	if (separator >= 'A' && separator <= 'Z' ||
	    separator >= 'a' && separator <= 'z') {
		bp = *s++ - separator;
		if (bp < 0 || bp >= 4) {
			return -1;
		}
		if (*s < '0' || *s > '9') {
			return -1;
		}
		slot = 0;
		while (*s >= '0' && *s <= '9') {
			slot = 10*slot + *s++ - '0';
		}

	} else {

		if (separator == '?') {
			s++;
		} else {
			if (*s++ != separator) {
				return -1;
			}
		}
		if (*s < '0' || *s > '9') {
			return -1;
		}
		bp = 0;
		while (*s >= '0' && *s <= '9') {
			bp = 10*bp + *s++ - '0';
		}
		if (*s == 0) {
			return -1;
		}
		if (separator == '?') {
			s++;
		} else {
			if (*s++ != separator) {
				return -1;
			}
		}
		if (*s < '0' || *s > '9') {
			return -1;
		}
		slot = 0;
		while (*s >= '0' && *s <= '9') {
			slot = 10*slot + *s++ - '0';
		}
	}

	return (cab << 6) | (bp << 4) | slot;
}
