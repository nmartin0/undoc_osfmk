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
 * create a Paragon/Mach 3.0 'default' paging file.
 *
 * create_pf size-in-KbyteBlocks{M} {filename(s)}
 *
 *	if size number is terminated with a "M" then treat size a # of megaBytes
 *	default filename: "paging_file"
 *
 * $Id: create_pf.c,v 1.4 1994/11/21 16:40:56 mtm Exp $
 *
 * HISTORY
 * $Log: create_pf.c,v $
 * Revision 1.4  1994/11/21  16:40:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/01/12  17:47:09  stans
 *  Support creation of multiple paging files
 * 	(e.g., create_pf 64M /t1/paging_file /t2/paging_file).
 *
 *  Reviewer: stans
 *  Risk: low
 *  Benefit: ease of use.
 *  Testing: developer
 *
 * Revision 1.2  1992/08/20  22:14:03  stans
 * added RCS log and id macros.
 *
 */

#include <sys/file.h>
#include <errno.h>
#include <stdio.h>

extern int errno;

#define BLOCK_SIZE 1024
#define	BUFSIZE BLOCK_SIZE*128

char *pgmname;

/*
 * create a default paging file
 *	create the file, write and close.
 *
 * inputs:
 *	pgfile		path name of the new file.
 *	fileBlks	# of 1024 byte blocks to be written
 *	buf		address of write buffer
 *	bufsize		bytesize of the write buffer [size of each write()].
 *
 * outputs:
 *	0 == success
 *	Otherwise an error.
 */

create_paging_file( pgfile, fileBlks , buf, bufsize)
	char	*pgfile;
	int	fileBlks;
	char	*buf;
	int	bufsize;
{
  register int	fd, nb, wsize, rc;

  fprintf(stdout,"%s: Creating '%s' of size (%d Meg) %d blocks.\n", pgmname,
	pgfile, (fileBlks*BLOCK_SIZE)/(1024*1024), fileBlks);

  /* create the file */
  fd = creat(pgfile, 0666);
  if (fd == -1) {
      perror("creat");
      return(1);
  }

  nb = (fileBlks*BLOCK_SIZE);
  while ( nb > 0 ) {
	wsize = bufsize;
	if ( nb < bufsize  )
		wsize = BLOCK_SIZE;

	if ( (rc=write(fd, buf, wsize)) != wsize ) {
		perror("write()");
		return( errno );
	}
	nb -= wsize;
  }

  close(fd);

  return (0);
}


main(argc, argv)
  int	argc;
  char	*argv[];
{
  char	*pgfile="paging_file";
  char	*buf;
  int	rc, num_blocks;
  int	bufsize=BUFSIZE;
  extern char	*strrchr(), *sbrk();

  if ( argc <= 1 ) {
      fprintf(stderr, "Usage: %s xM {filename(s)}\n", argv[0]);
      exit(1);
  }

  /* remember my name */
  if ( (pgmname=(char *)strrchr(argv[0],'/')) )
	pgmname++;	/* skip over '/' */
  else
	pgmname = argv[0];

  /*
   * "-b" is an experimental hack to 'runtime' vary the write() buffer size
   * in an attempt to determine the fastest execution time buffer size.
   */
  if ( argv[1][0] == '-' ) {
	char	*cp;

	switch( argv[1][1] ) {
	  case 'b':
		cp = &argv[1][2];
		if ( *cp == '\0' ) {
			argv++;
			argc--;
			cp = argv[1];
		}
		bufsize = atoi(cp)*BLOCK_SIZE;
		break;
	}
	argv++;
	argc--;
  }

  /*
   * size specified in MegaBytes? otherwise 1K blocks
   */
  if ( strchr(argv[1],'M') != 0 )
 	num_blocks = atoi(argv[1]) * 1024;
  else 
	num_blocks = atoi(argv[1]);

  if ( num_blocks == 0 ) {
	fprintf(stderr,"%s: Invalid block count %d\n", pgmname, num_blocks);
	exit( 1 );
  } 

  /*
   * allocate the write buffer
   */
  if ( (buf=sbrk(bufsize)) == (char *) -1 ) {
	perror("sbrk()");
	exit( errno );
  }

  /*
   * paging filename(s) specified?
   */
  if ( argc > 2 ) {
	int	j = 2;

	while( (pgfile = argv[j]) ) {
		if ( (rc=create_paging_file(pgfile, num_blocks, buf, bufsize)) )
			break;
		j++;
	}
  }
  else {
	/* default filename, create one default paging file. */
	rc = create_paging_file( pgfile, num_blocks, buf, bufsize );
  }

  exit( rc );
}
