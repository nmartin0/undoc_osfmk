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
 
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#ifdef TEST
#define DEVTAB	"./devtab"
#else
#define DEVTAB	"/etc/devtab"
#endif TEST

char 	*cbs();
extern	char *cbstos();

int	paragon_mesh_x = -1;
int	paragon_mesh_y = -1;
char 	*bmfile = NULL;
char 	*devtabfile = DEVTAB;
int 	matchnode;

char 	*progname;
int 	debug;

extern	int optind;
extern	char *optarg;

/*
** Recommended usages:
**	mkdevtab <node>
**	mkdevtab -b <#backplanes> -c <#cabinets> <node>
**	mkdevtab -x <nodes in X dimension> -y <nodes in Y dimension> <node>
**	mkdevtab -m <bootmagicfile> <node>
**
** Convert root partition node to io device number, and print
** io device number on stdout.
** Determine io device number by searching for existing entry in /etc/devtab.
** If no entry exists, create one.
** /etc/devtab starts with a header indicating the number of iodevno's
** already allocated.  Individual entries are of the form:
** 	/dev/io<iodevno><TAB><root node id>TAB<CBS node id>
**
** This program only registers mapping in /etc/devtab.
** Use ~makedev" to create /dev/io<n> directory and devices.
*/
main(argc,argv)
	int argc;
	char **argv;
{
	FILE *f, *ff;
	char devfile[100], cbsbuf[25], *cbsp, c;
	int node, eof, nbytes, iodev;
	int niodev = -1;
	int oldfile = 1;

	get_param(argc,argv);

	cbsp = cbs(matchnode);
	if (strcmp(cbsp,"***") == 0) {
		fprintf(stderr,"Node %d out of bounds for %d x %d mesh\n",
			matchnode,paragon_mesh_x,paragon_mesh_y);
		goto err;
	}

	ff = tmpfile();
	if (ff == NULL) {
		fprintf(stderr,"Cannot open tmp file\n");
		goto err;
	}

	f = fopen(devtabfile,"r+");
	if (f == NULL) {
		f = fopen(devtabfile,"w");
		if (f == NULL) {
			fprintf(stderr,"Cannot open %s\n",devtabfile);
			goto err;
		}
		niodev = 0;
		oldfile = 0;
		goto newentry;
	}

	fscanf(f, "IONODES = %d", &niodev);
#ifdef DEBUG
if (debug) printf("existing IONODES = %d\n",niodev);
#endif DEBUG

	/* scan file for matching node */
	eof = -1;
	while (1) {
		eof = fscanf(f, "%s\t%d\t%s", devfile, &node, cbsbuf);
		if (eof == EOF)
			break;
		if (node == matchnode) {
			if (strcmp(cbsp,cbsbuf)) {
				fprintf(stderr,
				"Bad match on CBS (node %d, cbs %s) in %s\n",
				node, cbsp, devtabfile);
				goto err;
			}
			sscanf(devfile,"/dev/io%d",&iodev);
			printf("%d",iodev);
			exit(0);
		}
	}

newentry:

	/* write out new header */
	fprintf(ff,"IONODES = %d\n",niodev+1);

	/* open old file, toss header, copy all existing entries */
	if (oldfile) {
		rewind(f);
		fscanf(f, "IONODES = %d\n", &eof);
		while (1) {
			nbytes = fread(&c, 1, 1, f);
			if (nbytes == 0)
				break;
#ifdef DEBUG
if (debug==2) printf("copying %c\n",c);
#endif DEBUG
			fwrite(&c, nbytes, 1, ff);
		}
	}

	/* write new entry */
	fprintf(ff,"/dev/io%d\t%d\t%s\n",niodev,matchnode,cbsp);

	/* overwrite real file with tmp file */
	rewind(f); rewind(ff);
	while (1) {
		nbytes = fread(&c, 1, 1, ff);
		if (nbytes == 0)
			break;
		fwrite(&c, nbytes, 1, f);
	}

	printf("%d",niodev);
	exit(0);

err:
	if (niodev == 0)
		unlink(devtabfile);
	printf("%d",-1);
	exit(-1);
}

get_param(argc,argv)
	int argc;
	char **argv;
{
	char c;

	progname = argv[0];
	while ((c = getopt(argc, argv, "dHb:c:x:y:m:f:")) != EOF) {
		switch (c) {

		case 'b':
			paragon_mesh_y = atoi(optarg)*4;
			if (paragon_mesh_y == 0)
				usage(1);
			break;

		case 'c':
			paragon_mesh_x = atoi(optarg)*4;
			if (paragon_mesh_x == 0)
				usage(1);
			break;

		case 'x':
			paragon_mesh_x = atoi(optarg);
			if (paragon_mesh_y == 0)
				usage(1);
			if (paragon_mesh_x % 4 != 0)
				usage(1);
			break;

		case 'y':
			paragon_mesh_y = atoi(optarg);
			if (paragon_mesh_y == 0)
				usage(1);
			if (paragon_mesh_y % 4 != 0)
				usage(1);
			break;

		case 'd':
			debug++;
			break;

		case 'H':
			usage(0);
			break;

		case 'm':
			bmfile = optarg;
			break;

		case 'f':
			devtabfile = optarg;
			break;

		case '?':
			usage(1);
			break;
		}
	}
	if (paragon_mesh_x < 0 || paragon_mesh_y < 0) {
		/* get dimensions from bootmagic */
		get_magic(bmfile);
		paragon_mesh_x = atoi(extract_bootenv("BOOT_MESH_X"));
		paragon_mesh_y = atoi(extract_bootenv("BOOT_MESH_Y"));
	}
	if (optind >= argc) 
		usage(1);
	matchnode = atoi(argv[optind]);

#ifdef DEBUG
if (debug) printf("mesh_x = %d, mesh_y = %d\n",paragon_mesh_x,paragon_mesh_y);
if (debug) printf("matchnode = %d\n",matchnode);
#endif DEBUG

}

usage(errflg)
	int errflg;
{
	fprintf(stderr,
		"Usage: %s [-Hd] [-bcxy] [<n>...] [-m file] <node>\n",
		progname);
	fprintf(stderr, " -H               Help\n");
	fprintf(stderr, " -d               Enable debug output\n");
	fprintf(stderr, " -b <backplanes>  Number of backplanes in each cab\n");
	fprintf(stderr, " -c <cabinets>    Number of cabinets\n");
	fprintf(stderr, " -x <mesh_x>      Mesh width (cabinets*4)\n");
	fprintf(stderr, " -y <mesh_y>      Mesh height (backplanes*4)\n");
	fprintf(stderr, "               ...x and y must be a multiple of 4\n");
	fprintf(stderr, " -m <filename>    file containing bootmagic\n");
	fprintf(stderr, " -f <filename>    alternate devtab\n");
	if (errflg) {
		printf("%d",-1);
		exit(-1);
	}
	exit(0);
}


char *
cbs(node)
	int node;
{
     	return( cbstos(root2cbs(node)) );
}
