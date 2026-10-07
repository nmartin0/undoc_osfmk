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
 * exportpaging - Generate the required swapon() syscalls necessary to export
 * a vnode paging service from the node where paging device (disk) resides.
 * The device-node number is located in the inode of the block special device
 * (e.g., /dev/io1/rz0b). The bootmagic variable 'EXPORT_PAGING' lists the
 * nodes which are exporting a vnode paging service. 'exportpaging' believes
 * 'EXPORT_PAGING' to be configured correctly with respect to local disks and
 * the bootmagic 'PAGER_NODE'.
 *
 * 'exportpaging' will match the valid node numbers from the 'EXPORT_PAGING'
 * bootmagic list to paging devices (Default 'rz0b' or overriden with PAGE_TO)
 * in '/dev/io[*]/'.  Once the block device is matched with the node # which
 * exports paging then a swpaon() syscall is executed using the full block
 * device pathname.
 *
 * $Id: exportpaging.c,v 1.15 1995/03/02 17:40:13 stans Exp $
 *
 * HISTORY
 * $Log: exportpaging.c,v $
 * Revision 1.15  1995/03/02  17:40:13  stans
 *  Support PAGE_TO=<1..3>rz0b,rz0d style bootmagic.
 * 	Fixed the node range processing in PAGE_TO bootmagic
 * 	Fixed bug preventing multiple paging devices at a node
 * 	Reduced execution time.
 *
 *  Reviewer: Shane
 *  Risk: medium
 *  Benefit or PTS #: 9454 & 12106
 *  Testing: WW07 sats, boot OakRidge XPS/150
 *
 */

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <string.h>
#include <mach.h>
#include <mach/boolean.h>
#include <device/device_types.h>
#include <device/disk_status.h>

#ifdef NULL
#undef NULL
#endif
#define NULL (char *)0

#define	DEBUG 0
#define USE_EXCEPTIONS_FILE 0	/* 1 == use /etc/exportpagin otherwise
				 * PAGE_TO bootmagic string.
				 */

typedef struct dirent dirent_t;

/*
 * nodes which export a vnode paging service to other micro-kernels.
 */
typedef struct XNODE {
  node_t	node;				/* node number */
  node_t	status;				/* ignore(0) | swapon(1) */
  char		*paging_dev[MAXPARTITIONS];	/* paging devices (pathnames) */
} xnode_t;

#define	XN_IGNORE 0
#define	XN_SWAPON 1
#define	XN_DEFAULT_PARTITION "rz0b"

#define	IO_DIRECTORY_PREFIX "io"
#define	EXCEPTIONS_FILE "/etc/exportpaging"

char		*pgmname;
char		*howto=" useage: exportpaging {-qndh}";
int		boot_node;
char		*xnodeList;
xnode_t		*xnodes;

#if	TEST
char		*bnl;
boolean_t	verbose=TRUE,
		debug=TRUE,
		fake_swapon=TRUE;
#else
boolean_t	verbose=FALSE,
		debug=FALSE,
		fake_swapon=FALSE;
#endif

extern char	*strdup();
extern char	*strchr();
extern char	*get_magic();
extern int	getbootint();
extern int	errno;

/*
 * Deduce the total number of nodes in the specified list excluding the
 * boot node. During the above procedure allocate and build the export table
 * which describes the node and the paging partitions/devices it exports.
 * An initial export table allocation size, in # of nodes, is passed in.
 * The table will expand to the correct size if need be. The actual number of
 * valid nodes is returned.
 *
 * inputs:
 *	list		string of node numbers "1,4,5,7,9..12,13"
 *	boot_node	node # of the boot node
 *	xpp		pointer to an export table pointer.
 *	scount		initial table allocation size, expressed in # of nodes.
 *
 * outputs:
 *	number of nodes in the list.
 */

count_nodes_in_list( char *list, int boot_node, xnode_t **xpp, int scount )
{
	register int		nodes=0;
	register char		*term;
	register boolean_t	done;
	register xnode_t	*xn, *xn_start;

	if ( list == (char *)0 )
		return 0;

	if ( strlen( list ) == 0 )
		return 0;

	/*
	 * malloc the initial list of xnodes
	 */
	xn = (xnode_t *)malloc( (scount * sizeof(xnode_t)) );
	if ( xn == (xnode_t *)0 ) {
		fprintf(stderr,"%s: unable to malloc(%d) xnode table\n",
			pgmname,(scount*sizeof(xnode_t)));
		exit(ENOMEM);
	}
	xn_start = xn;	/* save start of the export table */
	bzero( (char *)xn, (scount * sizeof(xnode_t)) );

	/*
	 * parse the EXPORTPAGING list, building the export table as we go.
	 */
	for(done=FALSE,term=list; !done; term++ ) {
		register int	snode, enode;

		snode = atoi_term(term, &term);
		if ( *term == '\0' )
			done = TRUE;

		/* do not count the boot node */
		if ( snode == boot_node )
			continue;
		/*
		 * Single node or a node range <1..34>?
		 */
		if ( *term == '.' ) { /* process a range, example: "4..9" */
			if ( *(term+1) != '.' ) {
				term++;	/* bad syntax */
				continue;
			}
			term += 2;	/* skipover ".." */
			enode = atoi_term(term, &term);	/* get ending node # */
			if ( *term == '\0' )
				done = TRUE;
			/*
			 * setup the export paging entries for the range.
			 * do start thru end-1; fall into single-node code
			 * for the rand end node.
			 */
			for( ; snode < enode; snode++ ) {
			        /*
			         * skip the boot node
			         */
			        if ( snode == boot_node )
				    continue;

				/*
				 * verify the node is alive and booted
				 */
#if TEST == 0
				if ( meshnode_is_up( snode ) == FALSE ) {
					if ( verbose ) {
						fprintf(stderr,
							"%s: node %d is down\n",
							pgmname, snode);
					}
					continue;
				}
#endif
				xn->node = snode;
				xn->status = XN_SWAPON;
				xn++;
				nodes++;	/* count the node */
			}
		}

		/*
		 * skip the boot node
		 */
		if ( snode == boot_node )
		    continue;

		/*
		 * verify the node is alive and valid.
		 */
#if TEST == 0
		if ( meshnode_is_up( snode ) == FALSE ) {
			if ( verbose )
				fprintf(stderr, "%s: node %d is down\n",pgmname,
						snode);
			continue;
		}
#endif
		/*
		 * initialize an export table entry for this node.
		 */
		xn->status = XN_SWAPON;
		xn->node = snode;
		xn++;		/* next slot */

		/*
		 * Count the node; have we exceeded the estimated export table
		 * size? If so, then expand it.
		 */
		if ( ++nodes >= scount ) {
			int	old_size;

			old_size = scount;
			scount *= 2;
			realloc( xn_start, (scount * sizeof(xnode_t)) );
			if ( xn_start == (xnode_t *)0 ) {
				fprintf(stderr,"%s: unable to realloc(%d) xnode table\n",
					pgmname,scount);
				exit(ENOMEM);
			}

			/* reset where we are in case realloc() had to copy */
			xn = &xn_start[nodes];

			/* clear new allocation, leave old initialized alone */
			bzero( xn, old_size*sizeof(xnode_t) ); 
		}
	}

	*xpp = xn_start;	/* return export table pointer */

	return nodes;		/* return count of nodes in export table */
}

/*
 * function called from scandir() to decide if this dirent should be added
 * to the list scandir() is building. Here we ar looking for io directory names
 * of the form "io*".
 *
 * inputs:
 *	dep == point to a dirent structure.
 *
 * outputs:
 *	boolean: TRUE == keep this dirent, FALSE == discard.
 */

static dev_select(dep)
	dirent_t	*dep;
{
	if ( strncmp(dep->d_name,IO_DIRECTORY_PREFIX,2) == 0 ) {
		if ( isdigit(dep->d_name[2]) ) {
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * Verify the specified device has been created (ala rmknod) with a device
 * node number that matches the passed-in node number.
 *
 * inputs:
 *	device	(char *) device pathname
 *	node	(int) a node number from export paging bootmagic string.
 *
 * outputs:
 *	TRUE	Block device's node number matches specified node number.
 *	FALSE	NO match.
 */

boolean_t
node_we_want( device, node )
	char	*device;
	int	node;
{
	struct devstat		dvs;

	/*
	 * retrieve node number from the blk device inode
	 */
	if (devstat(device, &dvs) < 0) {
		return FALSE;
	}

	/*
	 * node we are looking for?
	 */
	return( node == dvs.dst_node );
}

/*
 * Is this device a block special device
 *
 * inputs:
 *	dev	device pathname
 *
 * outputs:
 *	TRUE	block special device
 *	FALSE	NOT a block-special device.
 */

boolean_t
block_special( dev )
	char	*dev;
{
	struct stat	sb;

	if (stat(dev, &sb) < 0) {
		return FALSE;
	}

	return( S_ISBLK(sb.st_mode) );
}

/*
 * Scan the "/dev/io*" directories devstat()ing the specified block device.
 * Attempt to match the input node number with the node number from the block
 * device inode. Inode device-node number is where the device request will
 * be directed. Since we support multiple paging devices at a single node, 
 * return a hint (iodir name index) to use on the next device at this node.
 * Assumption is : All paging devices specified for a node are located in the
 * same /dev/io??/ directory.
 *
 * inputs:
 *	node		node number of interest
 *	blkDevice	block device name to use for paging.
 *	path		(char **) pointer to a pointer to receive  the full
 *			block device pathname.
 *	ioDir_hint	start searching in the io{ioDir_hint}/ directory. 
 *
 * outputs:
 *	0 == Success
 *	1 == unable to match node-number with any device in /dev/io?
 *	2 == found the device but is is NOT a block special device.
 *		path pointer is written.
 */

find_paging_devices( node, blkDevice, path, ioDir_hint )
	int	node;		/* IN */
	char	*blkDevice;	/* IN */
	char	**path;		/* OUT */
	int	*ioDir_hint;	/* IN/OUT */
{
	register int		ionum;
	char			*cp, device[256];
	static int		entries;
	static dirent_t		**ioDirNames=(dirent_t **)0;

	/*
	 * guard aginst a full pathname, reduce to basename of just
	 * the block device name (e.g.'rz0b').
	 */
	if ( cp = rindex(blkDevice, '/') )
		blkDevice = cp++;

	/*
	 * get a list of /dev/io* names. Already initialized?
	 * scan the "/dev" directory looking for "io*" names we will later
	 * use to construct block device pathnames.
	 */
	if ( ioDirNames == (dirent_t **)0 ) {
		entries = scandir( "/dev", &ioDirNames, dev_select, 0 );
		if ( entries <= 0 ) {
			perror("scandir(/dev)");
			exit( errno );
		}
	}

	/*
	 * scan the list of possible "io*" directories doing a devstat() on
	 * the block paging device from each directory. We are looking to
	 * match the node number from the device inode aginst the the input
	 * node number.
	 */
	for(ionum = *ioDir_hint; ionum < entries; ionum++) {
		/*
		 * build the block paging device pathname.
		 */
		sprintf( device, "/dev/%s/%s", ioDirNames[ionum]->d_name,
								blkDevice);
		/*
		 * node we are looking for? Yes, clone the
		 * string & return.
		 */
		if ( node_we_want( device, node ) ) {
			*ioDir_hint = ionum;
			*path =  strdup(device);
			return( block_special(device) ? 0 : 2 );
		}
	}

	*ioDir_hint = 0;

	return(1);	/* failure....unable to find the device */
}

#if	USE_EXCEPTIONS_FILE
/*
 * Read the exceptions files "/etc/pagingtree" for exceptions to the default
 * rule "page on /dev/io?/rz0b". File format: ASCII text, white-space
 * separated with "#" as the 1st char in a line to indicate a comment line.
 * blank-lines "<newline>" are ignored also. Each non-blank or comment file
 * line is composed of two white-space separated fields.
 * Field one:
 *	'node-number' which identifies node to which the exception applies
 * 
 * Field two:
 *	'block-paging-device' identifies the device to use instead of 'rz0b'.
 *
 * node-number  block-paging-device
 *
 * inputs:
 *	xn		pointer to export-paging node table
 *	max_xnode	# of nodes in the table.
 *
 * outputs:
 *	none.
 *
 * side effects:
 *	export-paging node table can be modified by the exception file.
 *
 * delimiter tokens: "<space><tab><newline><null>"
 */


#define	TOKEN_DELIMITERS " 	\n\0"

void
applyExceptions( max_xnode )
	int	max_xnode;
{
	register xnode_t	*xn;
	register char		*cp;
	char			fileLine[BUFSIZ],sbuf[20];
	int			node;
	FILE			*fs;
	extern char		*fgets();
	extern char		*strtok();

	if ( (fs=fopen(EXCEPTIONS_FILE,"r")) == NULL ) {
		if ( errno == ENOENT )
			return;	/* no file, no exceptions then NO problem */
		fprintf(stderr,"%s: unable to open %s, ",pgmname,
			EXCEPTIONS_FILE);
		perror("");
		return; 
	}

	if ( debug )
		fprintf(stderr,"%s: processing file %s\n",pgmname,
			EXCEPTIONS_FILE);

	/*
	 * read file lines until EOF or error.
	 */
	while ( (cp=fgets( fileLine, BUFSIZ, fs)) != (char *)0 ) {
		if ( strlen(fileLine) <= 0 )
			continue;	/* an empty line, newline only */

#if	DEBUG
		if ( debug )
			fprintf(stderr,"%s: '%s'",pgmname,fileLine);
#endif
		if ( fileLine[0] == '#' )
			continue;	/* a comment line */
		/*
		 * Now have some real data to work with, tokenize.
		 * must have a node spec.
		 */
		if ( (cp=strtok(fileLine,TOKEN_DELIMITERS)) == (char *)0 )
			continue;

		if ( isdigit((*cp)) == FALSE ) {
			fprintf(stderr,"%s: bad node in '%s', ignored.\n",
				pgmname,fileLine);
			continue;
		}

		/* convert to usable node spec */
		node = atoi(cp);

		/* retrieve new paging device */
		if ( (cp=strtok((char *)0,TOKEN_DELIMITERS)) == (char *)0 )
			continue;

		for( xn=&xnodes[0]; xn < &xnodes[max_xnode]; xn++ ) {
			if (xn->node == node) {
				xn->paging_dev = strdup(cp);
				break;
			}
		}
	}
	fclose( fs );
}

#else	/* USE_EXCEPTIONS_FILE */

/*
 * Deduce the exceptions to the assumed default paging block device "rz0b".
 * Each node can support MAXPARTITION paging devices. Each paging device name
 * slot has been cleared assuming a <NULL> '(char *)0' slot zero will become
 * the default "rz0b"; tested/set before the call to find_paging_device().
 * Here we check the PAGE_TO paging device name override bootmagic string.
 * If we match a node number then insert the new paging device name(s) into
 * the first <NULL> paging device name slot.
 *
 * 'bootpp' generates a boot environment string "PAGE_TO=" which contains
 * a bracketed node-list and a block device name:
 * (e.g., "PAGE_TO=<24>rz0f,rz0d:<35,37..39>rz0e"). Node 24 uses the raw block
 * partition "/dev/io??/rz0f and /dev/io??/rz0d" while nodes 35,37,38 & 39 will
 * use "/dev/iox/rz0e".
 */

#define	TOKEN_DELIMITERS ":<,>"
#define MAX_NODES 500	/* a wild guess @ max */

void
applyExceptions( max_xnode )
	int	max_xnode;
{
	char			*str, *cp, *first, *rp;
	register xnode_t	*xn;
	int			nodes[MAX_NODES], nodeCount, prevNodeCount;
	boolean_t		found;

	/*
	 * retrieve the bootmagic strings "PAGE_TO". String can be null.
	 */
#if TEST
	str = "<1..3,124>rz0m,rz0l:<5,122..123>rz0a,rz0b,rz0d,rz0e";
	/* str = "<1..3>rz0b"; */
#else
	if ( (str=get_magic("PAGE_TO")) == (char *)0 )
		return;
#endif
	if ( debug )
		fprintf(stderr,"PAGE_TO=%s\n",str);

	/*
	 * parse the bootmagic string setting the block paging device name(s)
	 * for the specific node.
	 */
	first = str;	/* init str_tok() handling */
	while( (cp=strtok(first,TOKEN_DELIMITERS)) != NULL ) {
		/*
		 * 1st strtok() call, info str_tok() to process same string.
		 */
		if ( first )
			first = (char *)0;

		/*
		 * create a list ("nodes[]") of node #'s which use the paging
		 * device name(s). 'cp' points at 1st node number in the
		 * string. Loop, plucking off node numbers and node ranges
		 * building a node list. We stop when cp is NOT pointing at a
		 * digit. The terminal token, which terminates this loop, will
		 * be the first paging device name.
		 */
		nodeCount = 0;
		found = FALSE;

		while ( !found ) {
			/*
			 * valid node number?
			 */
			if ( isdigit((*cp)) == FALSE ) {
				found = TRUE;
				continue;
			}

			/*
			 * processing a node range <1..3,4>
			 */
			if ( (rp=strstr(cp,"..")) != NULL ) {
				int	snode, enode;

				snode = atoi(cp);
				enode = atoi(rp+2);
				for(; snode < enode; snode++) {
					nodes[nodeCount++] = snode;
				}
				cp = rp+2;	/* skip over ".." */
			}
			nodes[nodeCount++] = atoi(cp);

			if ( nodeCount > MAX_NODES ) {
				fprintf(stderr,
					"%s: too many nodes (> %d) in PAGE_TO bootmagic.\n", pgmname,MAX_NODES);
				exit(1);
			}	
			cp = strtok( (char*)0, TOKEN_DELIMITERS);
		}

		/*
		 * When processing multiple paging device names, a comma
		 * spearated list, we use the same set of nodes with a new
		 * device name. 'PAGE_TO=<1,3,5..7>rz0a,rz0b' the first time
		 * thru we have just build the node list and nodeCount == 5.
		 * We process all five nodes using 'rz0a'. Once finished the
		 * strtok() in the outer controlling while() loop will advance
		 * 'cp' past the comma and be pointing at 'rz0b'. 'rz0b' will
		 * will fail the node counting loop and drop thru to here with
		 * nodeCount == 0. We detect this case and use the previously
		 * built node list with the new paging device name.
		 *
		 * First pass thru or are we processing multiple paging devices
		 * for this node?
		 */
		if ( nodeCount == 0 )
			nodeCount = prevNodeCount;
		else
			prevNodeCount = nodeCount;

		/*
		 * 'cp' points at new paging block device name (e.g., 'rz0f').
		 * Compare the nodes which export a Vnode pager aginst the
		 * nodes in our paging device override list. If the node
		 * numbers match, then insert the new &/ additional block
		 * paging device name.
		 */
		while( nodeCount-- > 0 ) {
			for( xn=&xnodes[0]; xn < &xnodes[max_xnode]; xn++ ) {
				if (xn->node == nodes[nodeCount]) {
					register int j;

					/*
					 * do we have room for this partition?
					 */
					for(j=0; j < MAXPARTITIONS; j++) {
						if (xn->paging_dev[j] == '\0')
							break;
					}
					if ( j >= MAXPARTITIONS ) {
						fprintf(stderr,
							"%s: too many paritions(%d) for node %d\n",
							pgmname,MAXPARTITIONS,
							xn->node);
						break;
					}
					xn->paging_dev[j] = strdup(cp);
					if ( debug ) {
						fprintf(stderr,
					"Override: node %d paging to %s\n",
						xn->node,xn->paging_dev[j]);
					}
					break;
				}
			}
		}
	}
}
#endif	/* USE_EXCEPTIONS_FILE */

main(argc, argv)
	int	argc;
	char	*argv[];
{
	register xnode_t	*xn;
	char			*term;
	int			export_nodes, j;
	boolean_t		done;

	/* remember who I am */
	if ( pgmname = rindex(argv[0], '/'))
		pgmname++;
	else
		pgmname = argv[0];

	/*
	 * process any cmdline args.
	 */
	if ( argc > 1 ) {
		argc--;
		argv++;
		for(; argc > 0; argv++,argc--) {
			if (argv[0][0] != '-')
				continue;
			switch( argv[0][1] ) {
			  case 'h':
				fprintf(stderr,"%s: %s\n",pgmname, howto);
				exit(0);
				break;
			  case 'v':
				verbose = TRUE;
				break;
			  case 'q':
				verbose = FALSE;
				break;
			  case 'n':
				fake_swapon = TRUE;
				break;
			  case 'd':
				debug = TRUE;
				verbose = TRUE;
				break;
			  default:
				fprintf(stderr,"%s: bad switch %s, ignored.\n",
					pgmname,argv[0]);
				break;
			}
		}
	}

	/*
	 * find our boot node assuming the root filesystem device is always
	 * on the boot node.
	 */
	if ( (boot_node = getbootint("ROOT_DEVICE_NODE", -1)) == -1 ) {
		fprintf(stderr,"%s: can't find ROOT_DEVICE_NODE?\n",pgmname);
		exit( EINVAL );
	}
	if ( debug )
		fprintf(stderr,"boot node @ %d\n",boot_node);

	xnodeList = get_magic("EXPORT_PAGING");
#if	TEST
	xnodeList = "1..3,5,7,122..125";
	bnl = xnodeList;
#endif
	if ( debug )
		fprintf(stderr,"'EXPORT_PAGING=%s'\n",xnodeList);

	if ( (xnodeList == (char *)0) || (strlen(xnodeList) == 0) ) {
		fprintf(stderr,"%s: empty EXPORT_PAGING list?\n",pgmname);
		exit(1);
	} 

	/*
	 * Count the number of nodes in the export paging list, excluding
	 * the boot node. Return an export table pointer.
	 */
	export_nodes = count_nodes_in_list(xnodeList, boot_node, &xnodes, 200);

	if ( debug )
		fprintf(stderr,"%s: %d non-boot nodes export Vnode paging.\n",
			pgmname, export_nodes );
	/*
	 * any real work to do here? Do all nodes get their paging from the
	 * boot node?
	 */
	if ( export_nodes == 0 ) {
		if ( verbose )
			fprintf(stderr,
			"%s: Only the boot node exports a Vnode pager, exit.\n",
			pgmname);
		exit(0);
	}

	/*
	 * apply exceptions from '/etc/pagingtree' or "PAGE_TO" bootmagic.
	 */
	applyExceptions( export_nodes );

	/*
	 * find the block special paging device which owned by the specified
	 * node. The device node number is located in the raw and block
	 * special device inode. "/dev/io1/{r}rz*[abcdefg]" are all owned by
	 * one specific node. We are looking for "/dev/io?/rz0b". The issue
	 * is that "io1" does not imply node #1 just the first MIO node.
	 * Could we assume position of a node number in MIO_NODE_LIST ?
	 * Skip the boot node in case it is listed in xnodes.
	 */
	for( xn=&xnodes[0]; xn < &xnodes[export_nodes]; xn++ ) {
		int	rc, index;

		if ( xn->status != XN_SWAPON || xn->node == boot_node )
			continue;

		if ( xn->paging_dev[0] == (char *)0 )
			xn->paging_dev[0] = XN_DEFAULT_PARTITION;

		for(j=0,index=0; xn->paging_dev[j]; j++) {
			rc = find_paging_devices(xn->node,
						xn->paging_dev[j],
						&xn->paging_dev[j],
						&index);
			if ( rc ) {
				xn->status = XN_IGNORE;	/* ignore it! */
				fprintf(stderr,"%s: ",pgmname);
				if ( rc == 1 ) {
					fprintf(stderr,"No paging device (/dev/io*/%s) for node %d\n",
						xn->paging_dev[j], xn->node);
#if TEST == 0
					exit(1);
#endif
				} else {
					fprintf(stderr,"paging device (%s) for node %d is NOT a block special device.\n",
						xn->paging_dev[j], xn->node);
#if TEST == 0
					exit(2);
#endif
				}
			}
		}
	}

	/*
	 * print what we will do
	 */
	fprintf(stdout,"%s: Vnode Paging Services Initialized for:\n", pgmname);
	if (fake_swapon)
		fprintf(stdout,"%s: No swapon() syscall executed.\n",pgmname);

	for( xn=&xnodes[0]; xn < &xnodes[export_nodes]; xn++ ) {
		if ( xn->status != XN_SWAPON )
			continue;

		if ( xn->node == boot_node )
			continue;

		for(j=0; xn->paging_dev[j] != '\0'; j++) {
			fprintf(stdout,"   '%s' @ node %d\n",
						xn->paging_dev[j],xn->node);
			if ( fake_swapon )
				continue;

			/*
			 * swapon(blockDevice, prefered, 20MBytes, no-hiwater)
			 * 	Due to mysterious server bugs the low and hi
			 *	water arguments to swapon() are required for
			 *	correct completion of the swapon() syscall.
			 */
			if (swapon(xn->paging_dev[j],1,20*1024*1024,0) < 0) {
				fprintf(stderr, "%s: %s: ", pgmname,
						xn->paging_dev[0]);
				switch (errno) {
				case EINVAL:
       	                        	fprintf(stderr,
						"device not configured\n");
					break;
				case EBUSY:
					fprintf(stderr,
						"device already in use\n");
					break;
				default:
					perror(NULL);
					break;
				}
			}
		}
	}
}
