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
/*              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license
 * agreement or nondisclosure agreement with Intel Corporation
 * and may not be copied or disclosed except in accordance
 * with the terms of that agreement.
 *
 * CBS.C - Paragon Cabinet/Backplane/Slot Conversion Utility
 *
 * These routine use Paragon mesh_x and mesh_y dimensions to convert
 * to/from CBS and/or root partition ids.  These routines, using the
 * define MAIN, can be used as a command.  When used in this manner,
 * tables of CBS nomenclature and/or root partition ids can be printed.
 *
 * When the CBS data is used as an ASCII string, it is of the form CCBSS,
 * where CC numeric from 0 to 99, B is alphabetic from A to D or a to D,
 * and SS is numeric from 0 to 15.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/cbs/main.c,v 1.7 1994/11/21 16:40:05 mtm Exp $
 *
 *
 */

#include <stdio.h>
#include "bootmagic.h"

#define BP_BASE 'A'

/*
 * These are lifted so as to use a stock bootmagic.h file
 */

int whitespace = 1;			/* always give a whitespace list */
char *progname = "cbs";                 /* just to resolve a reference */
int debug = 0;				/* ditto */

#ifdef MAIN

#ifdef PARAGON
  char *bootmagicfile = NULL;
#else
#ifdef DS
  char *bootmagicfile = "/usr/paragon/boot/bootmagic";
#else
  char *bootmagicfile = "./bootmagic";
#endif
#endif

extern int optind;
extern char *optarg;

char *cbstos();				/* forward reference */

int columns = 4;			/* init variables */
int delta_flag = 0;
int m_flag = 0;
int x_flag = 0;
int y_flag = 0;
int C_flag = 0;
int paragon_mesh_x;
int paragon_mesh_y;
int paragon_cabrows = 1;	/* initialize cabrows to 1 */

/*
 * Main - Not used for library module.
 */

main(argc, argv)
	int argc;
	char **argv;
{
	int c;
	char *cp;
	char *bootenvstr;

	/*
	 * Process input arguments.
	 */

	while ((c = getopt(argc, argv, "hDb:c:C:m:w:x:y:")) != EOF) {
		switch (c) {

		/*
		 * The width dimension is always 4 per cabinet.
		 */

		case 'x':
			paragon_mesh_x = atoi(optarg);
			if (paragon_mesh_x == 0)
				goto syntax;
			if (paragon_mesh_x % 4 != 0)
				goto syntax;
			x_flag++;
			break;

		case 'c':
			paragon_mesh_x = atoi(optarg) * 4;
			if (paragon_mesh_x == 0) /* can't have 0 cabs */
				goto syntax;
			x_flag++;
			break;

		/*
		 * The height dimension is 4 per backplane.
		 */

		case 'y':
			paragon_mesh_y = atoi(optarg);
			if (paragon_mesh_y == 0)
				goto syntax;
			if (paragon_mesh_y % 4 != 0)
				goto syntax;
			y_flag++;
			break;

		case 'b':
			paragon_mesh_y = atoi(optarg) * 4;
			if (paragon_mesh_y == 0)   /* can't have 0 bp's */
				goto syntax;
			y_flag++;
			break;

		case 'C':
			paragon_cabrows = atoi(optarg);
			if (paragon_cabrows == 0)  /* can't have 0 rows */
			        goto syntax;
			C_flag++;
			break;

		case 'D':
			delta_flag++;
			break;

		case 'm':
			bootmagicfile = optarg;
			m_flag++;
			break;

		case 'w':
			columns = atoi(optarg);
			if (columns == 0)
				goto syntax;
			break;

		case 'h':
		default:
syntax:
	fprintf(stderr, "Usage: %s [-cbxymwh] [<n>...]\n", argv[0]);
	fprintf(stderr, " -w <columns>     Number of columns in output\n");
	fprintf(stderr, " -m <file>        Path and name of bootmagic file\n");
	fprintf(stderr, " -c <cabinets>    Number of cabinets\n");
	fprintf(stderr, 
	    " -b <backplanes>  Number of backplanes in each cab\n");
	fprintf(stderr, " -x <mesh_x>      Mesh width (cabinets*4)\n");
	fprintf(stderr, " -y <mesh_y>      Mesh height (backplanes*4)\n");
	fprintf(stderr, "                  x and y must be a multiple of 4\n");
	fprintf(stderr, " -h               Help\n");
			exit(1);
		}

	}

	if ((m_flag == 0) && (x_flag == 0 || y_flag == 0 || C_flag == 0)) {
		/* 
 	 	 * Initialize X and Y coordinations to correct value.
	 	 */
		if (get_magic(bootmagicfile) < 0)
			exit(1);
		if (x_flag == 0)
			paragon_mesh_x = atoi(extract_bootenv("BOOT_MESH_X"));
		if (y_flag == 0)
			paragon_mesh_y = atoi(extract_bootenv("BOOT_MESH_Y"));
		if (C_flag == 0) {
			bootenvstr = (char *)extract_bootenv("BOOT_CAB_ROWS");
			if (bootenvstr != NULL)
				paragon_cabrows = atoi(bootenvstr);
		}
	}

	/*
	 * If a file was specified, use it.
	 */

	if (m_flag) {
		get_magic(bootmagicfile); /* get the bootmagic info */
		paragon_mesh_x = atoi(extract_bootenv("BOOT_MESH_X"));
		paragon_mesh_y = atoi(extract_bootenv("BOOT_MESH_Y"));
		bootenvstr = (char *)extract_bootenv("BOOT_CAB_ROWS");
		if (bootenvstr != NULL)
			paragon_cabrows = atoi(bootenvstr);
		cp = (char *) extract_bootenv("BOOT_NODE_LIST");
	}

	/*
	 * Finally, check that the relationship between paragon_mesh_y
	 * and paragon_cabrows is correct. 
	 * We have to assume that if we have more than one cabinet,
	 * then paragon_mesh_y will be a multiple of 16 (i.e. fully
	 * loaded.
	 */
	if ((paragon_mesh_y >= 16) && 
	    paragon_mesh_y != paragon_cabrows * 16) {
		fprintf(stderr, "%s: mesh_y must be multiple of cabrows\n",
		    argv[0]);
		exit(1);		
	}

	/*
	 * Convert and print arguments if any.  If no tables is specified,
	 * then do nothing.
	 */

	if (optind < argc) {		/* node values input */

		/*
		 * Process node input.  The values are either in root partition
		 * identifier format (numbers from 0 to n), or in the CCBSS
		 * cabinet/backplane/slot syntax.
		 */

		for (; optind < argc; optind++) {

			/*
			 * First of all, if this is a CBS string,
			 * print the values.
			 */

			if (iscbs(argv[optind])) {
				printf("CBS = %s, ", argv[optind]);
				c = cbs2root(atocbs(argv[optind]));
				if (c < 0)
					printf("ROOT = ****, ");
				else
					printf("ROOT = %4d, ", c);
				c = atocbs(argv[optind]);
				printf("DELTA = %4d\n", cbs2delta(c));
			}

			/*
			 * Not a CBS value, so print just the CBS and ROOT.
			 */

			else {
				printf("ROOT = %4d, ", atoi(argv[optind]));
				printf("CBS = %s\n",
					   cbstos(root2cbs(atoi(argv[optind]))));
			}
		}
	}

	/*
	 * Print the bootmagic information because there is no input values
	 */

	else {
		if (x_flag || y_flag) {
			int i;
			char value[10];
			char *cpp;
			cpp = (char *)malloc(5*paragon_mesh_x*paragon_mesh_y);
			if (cpp == NULL) {
				perror("cbs");
				exit(1);
			}
			for (i=0; i < paragon_mesh_x*paragon_mesh_y; i++) {
				sprintf(value, "%d ", i);
				strcat(cpp, value);
			}
			print_table(cpp);
			free(cpp);
		} else {
			get_magic(bootmagicfile); /* get the bootmagic info */
			cp = (char *) extract_bootenv("BOOT_NODE_LIST");
			print_table(cp);
		}
	}
	exit(0);
}

/*
 * PRINT_TABLE - Print in order of the root identifiers
 */

print_table(cp)
	char *cp;
{
	int column, root, cbs;

	printf("\nConfiguration = Cabinets %2d, Backplanes %d, Mesh %3d X %2d\n\n",
		(paragon_mesh_x / 4) * paragon_cabrows , paragon_mesh_y / 4,
		paragon_mesh_x, paragon_mesh_y);

	if (delta_flag) {
		printf(" Root ID = DELTA ID\n\n");
	}
	else {
		printf(" Root ID = CBS ID\n\n");
	}

	/*
	 * For each entry in the table, print the value.
	 */

	for (;;) {
		for (column = 0; column < columns; column++) {
			if (*cp != 0) {
				root = atoi(cp);        /* convert it to int */
				printf("%4d = ", root);	/* print the root id */
				cbs = root2cbs(root);  /* convert to cbs int */
				if (delta_flag) {          /* print delta id */
					printf("%4d  ", cbs2delta(cbs));
				}
				else {
					printf("%s  ", cbstos(cbs));
				}
			}
			while ((*cp != 0) && (*cp != ' ')) {
				cp++;
			}
			if (*cp == 0) {
				break;
			}
			cp++;
		}
		printf("\n");
		if (*cp == 0) {
			break;
		}
	}
	printf("\n");
	return;
}

#endif

/*
 * ROOT2CBS - Covnvert ROOT id integer to CBS integer
 * The CBS integer is up to int bits, ...CCBBSSSS.
 */

root2cbs(root)
	int root;
{
	int cab, bp, slot, x, y;
	int vcol, vrow, nvcol, nvrow;

	if (paragon_cabrows == 1) {
		x = paragon_mesh_x - (root % paragon_mesh_x) - 1;
		y = paragon_mesh_y - (root / paragon_mesh_x) - 1;
		if (y < 0) {

#if	DEBUG
			printf("root2cbs(%d)  x %d  y %d  return -1\n", 
			    root, x, y);
#endif

			return -1;
		}

		cab = x / 4;
		bp = y / 4;
		slot = "ahipbgjocfkndelm"[(y % 4)*4 + (x % 4)] - 'a';

#if	DEBUG
		printf(
		"root2cbs(%d)  x %d  y %d   cab %d  bp %d  slot %d   cbs %d\n",
		    root, x, y, cab, bp, slot,
		    (cab << 6) | (bp << 4) | slot);
#endif
	} else {
		x = paragon_mesh_x - (root % paragon_mesh_x) - 1;
		y = paragon_mesh_y - (root / paragon_mesh_x) - 1;
		nvcol = paragon_mesh_x / 4;
		nvrow = paragon_mesh_y / 16;
		vcol = (root % paragon_mesh_x) / 4;
		/* rows go from 0 to (nvrow - 1) */
		vrow =  nvrow - (y / 16) - 1;

		cab = vrow * nvcol + (nvcol - (vcol+1));
		bp = (y % 16) / 4;
		slot = "ahipbgjocfkndelm"[(y % 4)*4 + (x % 4)] - 'a';
	}

	return (cab << 6) | (bp << 4) | slot;
}

/*
 * CBS2ROOT - convert CBS integer to ROOT id integer
 * The CBS integer is up to int bits, ...CCBBSSSS.
 */

cbs2root(cbs)
	int cbs;
{
	int cab, bp, slot, x, y;
	int vcol, vrow, nvcol;

	if (cbs == -1) {
		return -1;
	}

	cab = cbs >> 6;
	bp = (cbs >> 4) & 3;
	slot = cbs & 15;

	
	/* The following lines were changed from:
	 *
	 * x = paragon_mesh_x - (cab * 4 + slot / 4) - 1;
	 * y = paragon_mesh_y - (bp * 4 + ("0123321001233210"[slot] - '0')) - 1
	 * to the following to allow 2 row to work
	 *
	 * Remember that the organization of a machine with multiple
	 * rows is as follows:
	 *
	 *  4  3  2  1  0
	 *  9  8  7  6  5
	 *  14 13 12 11 10
	 */
	if (paragon_cabrows == 1) {
		x = paragon_mesh_x - (cab * 4 + slot / 4) - 1;
		y = paragon_mesh_y - (bp * 4 + 
		    ("0123321001233210"[slot] - '0')) - 1;
	} else {
		vcol = cab % paragon_cabrows;
		nvcol = paragon_mesh_x / 4;
		vrow = cab / paragon_cabrows;
		x = paragon_mesh_x - 
		    (((nvcol -  (vcol + 1)) * 4) + slot/4) - 1;
		y = paragon_mesh_y - 
		    (((paragon_cabrows - (vrow + 1)) * 16) + (bp * 4) + 
		    ("0123321001233210"[slot] - '0')) - 1;
	}
	
	if (x >= paragon_mesh_x || x < 0 || y >= paragon_mesh_y || y < 0)
		return -1;
	else
		return	paragon_mesh_x * y + x;
}

/*
 * CBSTOS - Convert from CBS integer to CBS string.
 * The CBS integer is up to int bits, ...CCBBSSSS.
 */

char *
cbstos(cbs)
	int	cbs;
{
	static char s[16];
	int	cab, bp, slot;

	if (cbs == -1) {
		return "*****";
	}

	cab = cbs >> 6;
	bp = (cbs >> 4) & 3;
	slot = cbs & 15;

	sprintf(s, "%02d%c%02d", cab, bp + BP_BASE, slot);
	return s;
}

/*
 * ISCBS
 *
 * Determine if the value is in CBS format.  If not, return an error.
 */

iscbs(s)
	char *s;
{
	int i;

	/*
	 * Convert the cabinet value to integer, check the value
	 */

	i = atoi(s);			/* convert to int */
	if (i < 0 || i > 99) {		/* not valid */
		return 0;
	}
	while (*s >= '0' && *s <= '9') {   /* find end of numeric value */
		s++;
	}
	if (*s == 0) {			/* end of string, not good */
		return 0;
	}

	/*
	 * Non-numberic value found, check for valid backplane value.
	 */

	if ((*s < 'A' || *s > 'd') || (*s > 'D' && *s < 'a')) {
		return 0;
	}

	/*
	 * Convert the slot value to integer, check the value.
	 */

	if (*s == 0) {			/* end of string, not good */
		return 0;
	}
	i = atoi(s);			/* convert to int */
	if (i < 0 || i > 15) {		/* not valid */
		return 0;
	}
	return 1;			/* it's ok */
}

/*
 * ATOCBS - Convert from ASCII CBS string to CBS integer.
 * The CBS integer is 8 bits, CCBBSSSS.
 */

atocbs(s)
	char *s;
{
	int cab, bp, slot;

	/*
	 * Convert the cabinet value to integer, check the value
	 */

	cab = atoi(s);			/* convert to int */
	if (cab < 0 || cab > 99) {	/* not valid */
		return -1;
	}
	while (*s >= '0' && *s <= '9') {   /* find end of numeric value */
		s++;
	}
	if (*s == 0) {			/* end of string, not good */
		return -1;
	}

	/*
	 * Non-numeric value found, check for valid backplane value.
	 */

	if ((*s < 'A' || *s > 'd') || (*s > 'D' && *s < 'a')) {
		return -1;
	}

	if ((*s >= 'A') && (*s <= 'Z')) {
		bp = (int) (*s - 'A');	/* upper case letter */
	}
	else if ((*s >= 'a') && (*s <= 'z')) {
		bp = (int) (*s - 'a');	/* lower case letter */
	}
	else {
		return -1;		/* neither, error */
	}
	s++;

	/*
	 * Convert the slot value to integer, check the value.
	 */

	if (*s == 0) {			/* end of string, not good */
		return -1;
	}
	slot = atoi(s);			/* convert to int */
	if (slot < 0 || slot > 15) {	/* not valid */
  		return -1;
	}
	return ((cab << 6) | (bp << 4) | slot);
}

cbs2delta(cbs)
	int cbs;
{
	/*
	 * Delta numbering.  It starts at the bottom
	 * backplane, rightmost cabinet and counts
	 * left to right through each backplane until
	 * the upper left corner, then goes to the
	 * bottom backplane, next cabinet to the left
	 * and continues, etc.  The computation is:
	 * (cab*64) + (bp*16) + slot.
	 */

	return(((cbs >> 6) * 64) + (((cbs >> 4) & 3) * 16) + (cbs & 15));
}

