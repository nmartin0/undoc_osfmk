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
 
#include "bootmesh.h"

/* Edit this string with each revision.  Prints in verbose mode */
static char	*Version = "Version: R1.3, 9/7/94";

extern 	int optind;
extern 	char *optarg;
extern	compute_node_list();

/* option flags */
char 	*progname;
int	verbose = 0;
int	progress = 0;
int	one_server = 0;
int	one_detect_off = 0;
int	broadcast_enabled = 0;
int 	debug = 0;
int 	warn = 0;
int	arg_node = -1;
int	arg_timeout = -1;
int	poll_all = 0;
int	control_flags = 0;
#ifdef SLOWDOWN
int	slowdown = 0;
int	slowdown_secs = SLOWDOWN_SECS;
#endif SLOWDOWN

/* VARARGS */
PROGRESS(a,b,c,d,e)
char *a, *b, *c, *d, *e;
{
	if (progress || verbose) {
		fprintf(stderr,a,b,c,d,e);
		fflush(stderr);
	}
}

/* VARARGS */
VERBOSE(a,b,c,d,e)
char *a, *b, *c, *d, *e;
{
	if (verbose)
		fprintf(stderr,a,b,c,d,e);
}

main(argc, argv)
int argc;
char *argv[];
{
	FILE	*fd;
	int 	i;

	/* get arguments, if any */
	progname = argv[0];
	if (get_params(argc,argv) < 0) {
		usage();
		exit(1);
	}

	VERBOSE("\n%s\n", Version);	/* Show the version */

	/* read in bootmagic */
	if (get_magic(bootmagicfile) < 0) {
		exit(1);
	}

	if (poll_setup() < 0) {
		exit(1);
	}

	/* boot all nodes and start kernels running */
	delete_bootenv("BOOT_MY_NODE");

	if (one_server || multiple_kernels == 0) {	
	  	/* identify mesh nodes */
		if (compute_node_list(boot_node_list) < 0){
			fprintf(stderr,"bad mesh specifications");
			exit(1);
		}
		if (boot_nodes() < 0) {
			exit(1);
		}

		/* Rendezvous with all nodes to make sure they're alive */
		if (poll_nodes(meshlist) < 0) {
			exit(1);
		}

		/* Now accumulate the attribute strings and write to the file */

		meshlist[my_physical_node()].n_attrs = (char*) malloc(1024);

		nx_create_node_attributes(task_by_pid(-1),
		  meshlist[my_physical_node()].n_attrs, 1024);
		strcat(meshlist[my_physical_node()].n_attrs, ",bootnode");

		fd = fopen(ATTR_FILE, "w");
		if (!fd)
			ERR("Cannot open attributes files");

		for (i = 0; i < maxnodes; i++) {
			nodetype*	cp = meshlist + i;
			char*		attr;

			if (cp->n_attrs)
				attr = cp->n_attrs;
			else
				attr = "";
			
			fprintf(fd, "%s\n", attr);
#ifdef DEBUG
			VERBOSE("node %4d attribute: \"%s\"\n", i, attr);
#endif DEBUG
		}

		fclose(fd);

	}
	else
	{
		/* call boot_nodes() multiple times */
		if (boot_multiple_kernels() < 0)
			exit(1);
	}

	VERBOSE("%s...done\n",progname);
}

usage()
{
        printf("Usage %s:\n",progname);
        printf("\t[-b]:               Enable broadcast\n");
        printf("\t[-DdHvw]:           [Dd]ebug | Help | verbose | warn \n");
        printf("\t[-V]:               Verbose reporting of boot progress\n");
        printf("\t[-e emulatorfile]:  Specify Paragon emulator file\n");
        printf("\t[-s serverfile]:    Specify Paragon server file\n");
        printf("\t[-k kernelfile]:    Specify Paragon kernel file\n");
        printf("\t[-m masterfile]:    Specify Paragon bootmagic file\n");
        printf("\t[-n bootnode]       Specify node to boot\n");
        printf("\t[-o ]               Boot one kernel/server on all nodes\n");
        printf("\t[-O ]               Disable automatic one kernel & server detection\n");
        printf("\t[-t millisecs]      Specify polling timeout value\n");
        printf("\t[-p]                Poll all nodes despite first timeout\n");
	printf("\t[-P]                Skip polling init'd servers\n");
	printf("\t[-RKSEMG]:          Skip download of:\n");
	printf("\t                    R ... Reset command(s)\n");
	printf("\t                    K ... Kernel(s)\n");
	printf("\t                    S ... Server(s)\n");
	printf("\t                    E ... Emulator(s)\n");
	printf("\t                    M ... bootMagic(s)\n");
	printf("\t                    G ... Goto command(s)\n");
#ifdef SLOWDOWN
        printf("\t[-Z]:               Enable slow gotos\n");
        printf("\t[-z microsec]:      Microseconds to sleep between gotos\n");
#endif SLOWDOWN
}

/*
 * Process command line options
 */
get_params(argc, argv)
	int argc;
	char **argv;
{
	char c;
        optind = 1;
#ifdef DEBUG
DBG("get_params\n");
#endif DEBUG

#ifdef SLOWDOWN
#define ARGSTRING "bDdHVvwe:k:s:m:n:t:opRKSEMGPZz:"
#else
#define ARGSTRING "bDdHVvwe:k:s:m:n:t:opRKSEMGP"
#endif SLOWDOWN
	while ((c = getopt(argc, argv, ARGSTRING)) != EOF) {

                switch (c) {

                case 'b':       /* use broadcast to reach meshnodes */
			broadcast_enabled++;
                        break;

                case 'v':       /* verbose mode */
			verbose++;
                        break;

                case 'V':       /* verbose Progress */
			progress++;
                        break;

                case 'w':       /* issue warnings */
			warn++;
                        break;

                case 'H':       /* help */
			usage();
			exit(1);

                case 'D':       /* debug mode */
                case 'd':       /* debug mode */
			debug++;
                        break;

		case 'k':	/* override kernel file name */
			boot_kernel = optarg;
			break;

		case 'e':	/* override emulator file name */
			boot_emulator = optarg;
			break;

		case 's':	/* override server file name */
			boot_server = optarg;
			break;

                case 'm':       /* override system bootmagic with file */
			bootmagicfile = optarg;
                        break;

                case 'n':       /* override bootmagic nodelist */
			arg_node= atoi(optarg);
                        break;

                case 't':       /* specify timeout during polling */
			arg_timeout= 1000*atoi(optarg);
                        break;

                case 'o':       /* Boot one kernel/server only */
			one_server++;
                        break;

                case 'O':       /* disable autodetect of one kernel& server */
			one_detect_off++;
                        break;

                case 'p':       /* poll all nodes even if some timeout */
			poll_all++;
                        break;

		case 'R':
			control_flags |= SKIP_RESET;
			break;

		case 'K':
			control_flags |= SKIP_KERNEL;
			break;

		case 'S':
			control_flags |= SKIP_SERVER;
			break;

		case 'E':
			control_flags |= SKIP_EMULATOR;
			break;

		case 'M':
			control_flags |= SKIP_MAGIC;
			break;

		case 'G':
			control_flags |= SKIP_GOTO;
			break;

		case 'P':
			control_flags |= SKIP_POLLING;
			break;

#ifdef SLOWDOWN
                case 'z':       /* specify interval between gotos */
			slowdown_secs= atoi(optarg);
                        break;

                case 'Z':       /* force serialized goto commands */
			slowdown++;
                        break;
#endif SLOWDOWN	
		case '?':
			return(-1);
		}

	}
	/* these two modes are incompatible */
	if (arg_node != -1)
		broadcast_enabled = 0;

	VERBOSE("%s...\n",progname);
	VERBOSE("\tbroadcast mode = %s\n", (broadcast_enabled)?"on":"off");
	VERBOSE("\tkernel file    = %s\n", boot_kernel);
	VERBOSE("\tserver file    = %s\n", boot_server);
	VERBOSE("\temulator file  = %s\n", boot_emulator);
	VERBOSE("\tbootmagic file = %s\n", bootmagicfile);
	VERBOSE("\tdebug          = %s\n", (debug)?"on":"off");
	VERBOSE("\twarn           = %s\n", (warn)?"on":"off");
	if (arg_node != -1)
		VERBOSE("\tnode           = %d\n", arg_node);
	if (arg_timeout != -1)
		VERBOSE("\ttimeout        = %d\n", arg_timeout);
	VERBOSE("\tpoll_all       = %s\n", (poll_all)?"on":"off");
#ifdef SLOWDOWN
	VERBOSE("\tslowdown       = %s\n", (slowdown)?"on":"off");
	if (slowdown)
		VERBOSE("\tslowdown_secs  = %d\n", slowdown_secs);
#endif SLOWDOWN

	return(0);
}

