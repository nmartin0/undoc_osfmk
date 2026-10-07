/*
 *  S Y S T E M   B A L A N C E   S A T   D R I V E R   P R O G R A M
 *
 *
 *  DESCRIPTION:
 *
 *	sbs is an nx compiled program that can be used to run a single node
 *	program on multiple nodes of a user specifiable compute parition.
 *	Everything sent to stdout and stderr by the single nodes programs
 *	running on multiple compute nodes will be collected together and
 *	displayed in an ordered listing, labeling which output came from
 *	which processor.
 *
 *
 *  SYNTAX:
 *
 *      sbs [-d rundir] [-b bufsize] [-o outfile] [-t[+-]]
 *
 *
 *  ARGUMENTS:
 *
 *	-d rundir
 *		The -d swidth specifies a directory to change to before
 *		executing the specified program.  It can be used to execute
 *		programs that must be run from a specific directory, such
 *		as the SAT run scripts.  If the -d switch is not supplied,
 *		the current directory will be used.
 *
 *	-b bufsize
 *		The sbs program collects all output writen by the selected
 *		program directly into internal buffers for stdin and stdout
 *		using pipes.  This technique is used instead of temporary
 *		files to reduce disk I/O.  If the buffers overflow, an error
 *		message is displayed, and sbs aborts.  When this happens,
 *		re-run the program specifying a larger buffer size.  By
 *		default the buffer size is 4096 bytes for both the stdin and
 *		stderr buffers.
 *
 *	-o outfile
 *		By specifing the -o switch, all output can be captured in
 *		a user specified output file.  All output will still be
 *		written to stdout.
 *
 *	-t[+-]	The -t switch enables and disables elapsed time reporting.
 *		By default, no elapsed times are reported.  -t and -t+
 *		turn on elapsed time reporting, -t- turns it off.
 *
 *
 *  NOTE:
 *
 *	sbs also responds to the regular nx switches, such as -pn, -sz, etc.
 *
 *
 *  TO COMPILE FOR THE PARAGON:
 *
 *	cc -o sbs sbs -nx
 *
 *
 *  HISTORY:
 *
 *	Original version: Brad Seevers, July, 1993
 *      Removed call to setptype(0) to correct problem
 *        reported in PTS 8204: Scott Killops, February, 1994
 */

#include <stdio.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <time.h>
#include <poll.h>
#include <nx.h>

#define BUFSIZE 4096

/*
   START should be defined to be the name of the function to time and
   capture all output from.  For an entire Fortran program, it should
   be MAIN_.
*/
#define START linpack_

struct global_opts {
	char *dir;
	char *outfile;
	char cmd[2048];
	int do_time;
} static global_opts;

struct global_info {
	int mynode;
	int numnodes;
	int fd1[2], fd2[2];
	FILE *outfp;
	int bufsize;
	char *outbuf, *errbuf;
        double etime;
} static global_info;

/*
  Perform all initialiazation of global variables.
*/

static init()
{
	global_opts.dir = NULL;
	global_opts.outfile = NULL;

	global_opts.do_time = 0;

	global_info.mynode = mynode();
	global_info.numnodes = numnodes();
	global_info.outfp = stdout;

	global_info.bufsize = BUFSIZE;
}

/*
  Perform all initialization after command line argeuments have been read.
*/

static setup()
{
	if (pipe(global_info.fd1)) {
		fprintf(stderr, "unable to create pipe 1\n");
		exit(2);
	}

	if (pipe(global_info.fd2)) {
		fprintf(stderr, "unable to create pipe 2\n");
		exit(2);
	}

	if (global_opts.outfile) {
		global_info.outfp = fopen(global_opts.outfile, "w");
		if (!global_info.outfp) {
			fprintf(stderr,
				"unable to open file \"%s\" for output\n",
				global_opts.outfile);
			exit(2);
		}
	}

	if (global_opts.dir) {
		if (chdir(global_opts.dir) < 0) {
			fprintf(stderr,
				"Unable to change to directory \"%s\"\n",
				global_opts.dir);
			exit(2);
		}
	}

	global_info.outbuf = (char *)malloc(global_info.bufsize);
	if (!global_info.outbuf) {
		fprintf(stderr, "Unable to allocate outbuf\n");
		exit(2);
	}

	global_info.errbuf = (char *)malloc(global_info.bufsize);
	if (!global_info.errbuf) {
		fprintf(stderr, "Unable to allocate errbuf\n");
		exit(2);
	}
}

/*
  Process all command line arguments.
*/

static parse_opts(cp)
char *cp[];
{
        while (cp[0] && cp[0][0] == '-') {
                switch (cp[0][1]) {
                case 'd':			/* specify working directory */
                        if (cp[0][2]) {
                                global_opts.dir = &cp[0][2];
                        }
                        else if (cp[1]) {
                                global_opts.dir = cp[1];
                                cp++;
                        }
                        else {
                                fprintf(stderr,
					"-d switch requires an argument\n");
                                exit(1);
                        }
                        break;

		case 'b':			/* select buffer size */
			if (cp[0][2]) {
				global_info.bufsize = atoi(&cp[0][2]);
			}
			else if (cp[1]) {
				global_info.bufsize = atoi(cp[1]);
				cp++;
			}
			else {
                                fprintf(stderr,
					"-b switch requires an argument\n");
                                exit(1);
                        }
			break;

                case 'o':			/* specify output file */
                        if (cp[0][2]) {
                                global_opts.outfile = &cp[0][2];
                        }
                        else if (cp[1]) {
                                global_opts.outfile = cp[1];
                                cp++;
                        }
                        else {
                                fprintf(stderr,
					"-o switch requires an argument\
n");
                                exit(1);
                        }
                        break;
		case 't':			/* turn on timing */
			if (!strcmp(cp[0], "-t") || !strcmp(cp[0], "-t+")) {
				global_opts.do_time = 1;
			}
			else if (!strcmp(cp[0], "-t-")) {
				global_opts.do_time = 0;
			}
			else {
                        	fprintf(stderr, "Unknown switch \"%s\"\n",
					cp[0]);
	                        exit(1);
			}
			break;

                default:
                        fprintf(stderr, "Unknown switch \"%s\"\n", cp[0]);
			exit(1);
                }
                cp++;
        }

	/*
	  Build command line to execute in global_opts.cmd from remaining
	  arguements.
	*/

        global_opts.cmd[0] = 0;

        while (cp[0]) {
                sprintf(global_opts.cmd, global_opts.cmd[0] ? "%s %s" : "%s%s",
		        global_opts.cmd, cp[0]);
                cp++;
        }
}

static int exec_func()
{
	int rc;
        int fd;
	double starttime;

	/*
	  We won't need these files descriptors
	*/

        close(global_info.fd1[0]);
        close(global_info.fd2[0]);

	/*
	  Close stdout and stderr and bind output pipe files descriptors
	  to stdout and stderr to pump output from program directly down
	  the pipes without creating any temporay files.
	*/

        close(1);
        close(2);

        fd = dup(global_info.fd1[1]);
        if (fd != 1) {
        	char *cp;

                cp = "cannot bind stdout to fd\n";
                write(global_info.fd2[1], cp, strlen(cp));

                exit(2);
        }

        fd = dup(global_info.fd2[1]);
        if (fd != 2) {
                char *cp;

                cp = "cannot bind stderr to fd\n";
                write(global_info.fd2[1], cp, strlen(cp));

                exit(2);
        }

        /* start timer */

        starttime = dclock();

	/*
	  Execute specified program, grab return code.
	*/

	/* NOTE: function being called must return (no Fortran STOPs) */

        rc = START();

        /* stop timer */

        global_info.etime = dclock() - starttime;

	/* send elapsed time to parent process */

	if (global_opts.do_time) {
		csend(0, &global_info.etime, sizeof(global_info.etime),
		      global_info.mynode, 0);
	}

	/*
	  Close down remaining file descriptors so parent process can
	  determine we are done.
	*/

        close(global_info.fd1[1]);
        close(global_info.fd2[1]);

	return rc;
}

/*
  Collect all output from child process into stdout and stderr buffers.
*/

static collect_local_output(pid)
int pid;
{
        int out_pos = 0;
        int err_pos = 0;
        int i, rc;
	struct pollfd fds[2];
        int open_fds = 0x2 | 0x1;

	/*
	  Close files we don't need
	*/

        close(global_info.fd1[1]);
        close(global_info.fd2[1]);

	/*
	  Prepare poll data structure for polling (see man 2 poll).
	*/

        fds[0].fd = global_info.fd1[0];
        fds[0].events =  POLLIN;

        fds[1].fd = global_info.fd2[0];
        fds[1].events = POLLIN;

	/*
	  Loop, reading all output from child process into buffers until
	  it finishes.
	*/

        do {
        	fds[0].revents = 0;
                fds[1].revents = 0;

                if (poll(fds, 2, -1) < 0) {
                    fprintf(stderr, "error polling fds\n");
                    kill(pid, -9);
                    exit(2);
                }

                if (fds[0].revents & POLLIN) {
                        i = read(global_info.fd1[0],
				 &global_info.outbuf[out_pos],
				 global_info.bufsize - out_pos);
                        if (i) {
                          out_pos += i;
                          if (out_pos >= global_info.bufsize) {
                                fprintf(stderr, "out buffer overflow\n");
                                kill(pid, -9);
                                exit(2);
                          }
                        }
                        else {
                          open_fds &= ~1;
                        }
                }

                if (fds[1].revents & POLLIN) {
                        i = read(global_info.fd2[0],
				 &global_info.errbuf[err_pos],
				 global_info.bufsize - err_pos);
                        if (i) {
                          err_pos += i;
                          if (err_pos >= global_info.bufsize) {
                                fprintf(stderr, "err buffer overflow\n");
                                kill(pid, -9);
                                exit(2);
                          }
                        }
                        else {
                          open_fds &= ~2;
                        }
                }
	} while (open_fds);

	if (global_opts.do_time) {
		crecv(0, &global_info.etime, sizeof(global_info.etime));
	}

	/*
	  Collect exit code from child process (This should be the code
	  returned form the program specified on the command line).
	*/

        wait(&rc);

	/*
	  Null terminate the buffers.
	*/

        global_info.outbuf[out_pos] = 0;
        global_info.errbuf[err_pos] = 0;

        if (!(rc & 0xff)) {
        	rc >>= 8;
        }

	return rc;
}

/*
  Display all output that has been collected from the single processor
  programs.
*/

static report_all_output(rc)
int rc;
{
	int i, j;
	char dir[MAXPATHLEN];

        /* ship everything to node 0 */

        if (!global_info.mynode) {
                fprintf(global_info.outfp, "Number of processors: %d\n",
			global_info.numnodes);

		getwd(dir);
                fprintf(global_info.outfp, "Directory: %s\n\n",dir);

		if (global_opts.do_time) {
                	fprintf(global_info.outfp,
				"Processor: %d  Exit: %d  Time: %f\n", 0, rc,
				global_info.etime);
		}
		else {
                	fprintf(global_info.outfp,
				"Processor: %d  Exit: %d\n", 0, rc);
		}
		if (*global_info.outbuf) {
                	fprintf(global_info.outfp,
				"Standard Out: ----------------------\n%s",
				global_info.outbuf);
		}
                if (*global_info.errbuf) {
			fprintf(global_info.outfp,
				"Standard Error: --------------------\n%s",
				global_info.errbuf);
		}

                for (i = 1; i < global_info.numnodes; i++) {
                        csend(i*5+0, &j, 0, i, 0);

			crecv(i*5+1, &global_info.etime, sizeof(double));
                        crecv(i*5+2, &rc, sizeof(rc));
                        crecv(i*5+3, global_info.outbuf, global_info.bufsize);
                        crecv(i*5+4, global_info.errbuf, global_info.bufsize);

			if (global_opts.do_time) {
                        	fprintf(global_info.outfp,
					"\nProcessor: %d  Exit: %d  Time: %f\n",
					i, rc, global_info.etime);
			}
			else {
                        	fprintf(global_info.outfp,
					"\nProcessor: %d  Exit: %d\n", i, rc);
			}
			if (*global_info.outbuf) {
                        	fprintf(global_info.outfp,
					"Standard Out: ----------------------\n%s",
					global_info.outbuf);
			}
			if (*global_info.errbuf) {
                               	fprintf(global_info.outfp,
					"Standard Error: --------------------\n%s",
					global_info.errbuf);
			}
                }
        }
        else {
                crecv(global_info.mynode*5+0, &j, sizeof(j));

		csend(global_info.mynode*5+1, &global_info.etime,
		      sizeof(double), 0, 0);
                csend(global_info.mynode*5+2, &rc, sizeof(rc), 0, 0);
                csend(global_info.mynode*5+3, global_info.outbuf,
		      strlen(global_info.outbuf)+1, 0, 0);
                csend(global_info.mynode*5+4, global_info.errbuf,
		      strlen(global_info.errbuf)+1, 0, 0);
        }
}

main(argc,argv)
int	argc;
char	*argv[];
{
	int pid;
	int rc;
	double starttime;

	init();

	parse_opts(argv+1);

	setup();

	pid = fork();

	if (pid < 0) {
		fprintf(stderr, "unable to fork\n");
		exit(2);
	}

	if (pid) {

		rc = collect_local_output(pid);

		report_all_output(rc);
	}
	else {

		setptype(1);

		rc = exec_func();
	}

	exit(0);
}
