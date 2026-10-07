#define MAIN
/*
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 * stack: Paragon One-row/Two-row Conversion Utility
 *
 * These routine use Paragon mesh_x and mesh_y dimensions to convert
 * to/from two-row node numbering, using the "root" node numbering
 * system; i. e., starting at the upper left corner.
 *
 * In a two-row system, the right hand cabinets are logically stacked as
 * a block on top of the left hand cabinets.  So a one-row configuration 
 * that looks like this in terms of cabinets:
 *
 *	3 2 1 0
 *
 * would look like this in a two row configuration:
 *
 *      1 0
 *  	3 2
 * 
 * This code can handle two cases. If paragon_mesh_y == 16, we assume the
 * input is one-row (one cabinet = 16 nodes high), and convert to two-row
 * node numbering. If paragon_mesh_y == 32, we assume the input is two-row
 * (2 cabinets high), and convert the other way. Any other case will
 * result in an error.
 *
 * Input parameters, in order of priority:
 *	- Command line arguments (x and y dimensions, the nodes to convert)
 *	- Bootmagic file indicated as a command line argument (text file
 *	    containing bootmagic strings)
 *	- Bootmagic from OSF
 *
 *  In case the command line arguments do not tell which nodes to convert,
 *  then all the nodes are converted and displayed as a table.


 *      Copyright 1994  Intel Corporation.
 *
 */

#include <stdio.h>
#include "bootmagic.h"


/* Direction of translation */
#define	STACK	1	/* Go from one row to two */
#define	STRETCH	-1	/* Go from two rows to one
*/
/*
 * These are lifted so as to use a stock bootmagic.h file
 */

int whitespace = 1;			/* always give a whitespace list */
char *progname = "stack";                 /* just to resolve a reference */
int debug = 1;				/* ditto */

#ifdef MAIN

#ifdef PARAGON
  char *bootmagicfile = NULL;
#else
#ifdef DS
  char *bootmagicfile = "/usr/paragon/boot/bootmagic";
#else
  char *bootmagicfile = "./bootmagic";
#endif	DS
#endif PARAGON

extern int optind;
extern char *optarg;
extern char    *bootmagic;

/* init variables */
int columns = 4;	/* By default print 4 columns per screen */
int m_flag = 0;
int x_flag = 0;
int y_flag = 0;
int C_flag = 0;
int paragon_mesh_x;
int paragon_mesh_y;
int direction;

/*
 * Main - Not used for library module.
 */

main(argc, argv)
	int argc;
	char **argv;
{
	int c, node;
	char *cp;
	char *bootenvstr;

	/*
	 * Process input arguments.
	 */

	while ((c = getopt(argc, argv, "h:m:w:x:y:")) != EOF) {
		switch (c) {

		case 'x':
			paragon_mesh_x = atoi(optarg);
			if (paragon_mesh_x == 0)
				goto syntax;
			if (paragon_mesh_x % 4 != 0)
				goto syntax;
			x_flag++;
			break;

		/*
		 * The height dimension is 16 per cabinet.
		 */

		case 'y':
			paragon_mesh_y = atoi(optarg);
			if (paragon_mesh_y == 0)
				goto syntax;
			if (paragon_mesh_y % 16 != 0)
				goto syntax;
			y_flag++;
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
	fprintf(stderr, "Usage: %s [-xymwh] [<n>...]\n", argv[0]);
	fprintf(stderr, " -w <columns>     Number of columns in output\n");
	fprintf(stderr, " -m <file>        Path name of bootmagic file\n");
	fprintf(stderr, " -x <mesh_x>      Mesh width (multiple of 8)\n");
	fprintf(stderr, " -y <mesh_y>      Mesh height (must be 16 or 32) \n");
	fprintf(stderr, " -h               Help\n");
			exit(1);
		}

	}

	if ((m_flag == 0) && (x_flag == 0 || y_flag == 0 )) {
		/* 
 	 	 * Initialize X and Y coordinations to correct value.
	 	 */
		if (get_magic(bootmagicfile) < 0)
			exit(1);
		if (x_flag == 0)
			paragon_mesh_x = atoi(extract_bootenv("BOOT_MESH_X"));
		if (y_flag == 0)
			paragon_mesh_y = atoi(extract_bootenv("BOOT_MESH_Y"));
	}

	/*
	 * If a file was specified, use it.
	 */

	if (m_flag) {
		get_magic(bootmagicfile); /* get the bootmagic info */
		paragon_mesh_x = atoi(extract_bootenv("BOOT_MESH_X"));
		paragon_mesh_y = atoi(extract_bootenv("BOOT_MESH_Y"));
		cp = (char *) extract_bootenv("BOOT_NODE_LIST");
	}

#ifdef DEBUG
	printf("paragon_mesh_y = %d, paragon_mesh_x = %d \n", paragon_mesh_y ,
	paragon_mesh_x);
#endif
	/*
	 * Check for valid paragon_mesh_y
	 */
	if (paragon_mesh_y % 16 != 0)  {
		fprintf(stderr, "%s: mesh_y must be multiple of 16\n",
		    argv[0]);
		exit(1);		
	}

	/* If converting from one row to two, the number of cabinets
	   must be even
	 */
	if ((paragon_mesh_y  == 16) && (paragon_mesh_x % 8 != 0))  {
		fprintf(stderr, "%s: mesh_x must be multiple of 8\n",
		    argv[0]);
		exit(1);		
	}

	
	/* Which way are we translating ? */
	if (paragon_mesh_y  == 16)  {
		direction = STACK;	/* Go from one row to two */
	} else {
		direction = STRETCH;	/* Go from two row to one */
	}


	/*
	 * Convert and print arguments if any. 
	 */

	if (optind < argc) {		/* node values input */

		/*
		 * Process node input. 
		 */
		for (; optind < argc; optind++) {
			node = atoi(argv[optind]);
			if (direction == STACK) {
				printf("1-row = %4d, ", node);
				printf("2-row = %4d\n", stack(node));
			}
			else {
				/* Stretch it */
				printf("1-row = %4d, ", stretch(node));
				printf("2-row = %4d\n", node);
			}
		}
	}

	/*
	 * Print the bootmagic information because there are no input values
	 */

	else {
		if (x_flag || y_flag) {
			int i;
			char value[10];
			char *cpp;
			cpp = (char *)malloc(5*paragon_mesh_x*paragon_mesh_y);
			if (cpp == NULL) {
				perror("stack");
				exit(1);
			}
			for (i=0; i < paragon_mesh_x*paragon_mesh_y; i++) {
				sprintf(value, "%d ", i);
				strcat(cpp, value);
			}
			print_table(cpp);
			free(cpp);
		} else {
			/* get the bootmagic info */
			if (!bootmagic)
				get_magic(bootmagicfile); 
			cp = (char *) extract_bootenv("BOOT_NODE_LIST");
			print_table(cp);
		}
	}
	exit(0);
}

/*
 * PRINT_TABLE  
 */

print_table(cp)
	char *cp;
{
	int column, root ;

	if (direction == STACK)
		printf("Converting from one row to two\n");
	else if (direction == STRETCH)
		printf("Converting from two rows to one\n");
	else {
		/* This should be impossible, the case is covered
		   for debugging */
		fprintf(stderr,"Crazy direction\n");
		exit(1);
	}

	printf("\n Mesh = %3d X %2d\n",
		paragon_mesh_x, paragon_mesh_y);
	printf(" Format:  one-row = two-row\n\n");


	/*
	 * For each entry in the table, print the value.
	 */

	for (;;) {
		for (column = 0; column < columns; column++) {
			if (*cp == 0) {
				break;
			}
			root = atoi(cp);        /* convert it to int */
			if (direction == STACK) {
				printf("%4d = ", root);	
				printf("%4d,  ", stack(root) );	
			}
			else {
				printf("%4d =  ", stretch(root) );	
				printf("%4d,  ", root);	
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

#endif	MAIN

/* Convert 1-row value to 2-row value.  That is, stack half the cabinets
 * on top of the other half.
 */
int
stack(node)
int node;
{
	int	one_row_x, one_row_y, two_row_x, two_row_y;
	int	new_mesh_x, new_mesh_y;

	new_mesh_x = paragon_mesh_x / 2;
	new_mesh_y = paragon_mesh_y * 2;

	one_row_x = node % paragon_mesh_x;
	one_row_y = node / paragon_mesh_x;

	if ( one_row_x >= new_mesh_x) { 
		two_row_x = one_row_x - new_mesh_x;
		two_row_y = one_row_y;
	}
	else {
		two_row_x = one_row_x;
		two_row_y = one_row_y + paragon_mesh_y;
	}

	return((two_row_y * new_mesh_x) + two_row_x);
}

/* Convert 2-row value to 1-row value.  Pull off the top row of cabinets
 * and stretch them out to the right.
 */
int
stretch(node)
int node;
{
	int	one_row_x, one_row_y, two_row_x, two_row_y;
	int	new_mesh_x, new_mesh_y;

	new_mesh_x = paragon_mesh_x * 2;
	new_mesh_y = paragon_mesh_y / 2;

	two_row_x = node % paragon_mesh_x;
	two_row_y = node / paragon_mesh_x;

	if ( two_row_y >= new_mesh_y) { 
		one_row_x = two_row_x;
		one_row_y = two_row_y - new_mesh_y;
	}
	else {
		one_row_x = two_row_x + paragon_mesh_x;
		one_row_y = two_row_y ;
	}

	return((one_row_y * new_mesh_x) + one_row_x);
}
