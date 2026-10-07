/*******************************************************************************
*
*  C O M M U N I C A T I O N   T E S T
*
*
*  SYNTAX:
*	comtest [-f] [-h] [-i] [-u] [-c msgcnt] [-m numlens] [-n numiter] \
*	    [-t type_spec] [-b hxw] [-k hxw] [-r {1|2|3}] [-o] [-pn part_name]
*
*  NOTE:   
*	For other options, see 'application' in the Paragon OSF/1 Commands
* 	Reference Manual
*
*  ARGUMENTS:
*
*       -f      Use forced message types for all tests.
*               [Default: Standard message types.]
*
*       -h      Help flag.  This switch causes the entire SYNTAX and ARGUMENTS
*               sections to be written to stdout.
*               [Default: No help message.]
*
*       -i      Interactive mode.  Use of this switch causes comtest to go
*               into the interactive mode.  This will allow repeating a test.
*               [Default: Do not repeat]
*
*       -u      Underscore flag.  Use the communication routines with no
*               error checking, the underscore routines:
*               _csend/_isend/_crecv/_irecv.
*               [Default:  Use routines with error checking
*               csend/isend/crecv/irecv.]
*
*	-c msgcnt
*		Msgcnt is the number of messages at each stage of the test.
*               It is included to allow a simple scheme for measuring the
*               aggregate bandwidth of the bus.  It is functionally like
*               asynchronous send/receive but is much simpler to implement.
*               Increasing this parameter causes some increase in run-time
*               due to the increase in messages, but since the messages can
*               occur simultaneously, the increase is not linear. The maximum
*               value for msgcnt is 8 for messages of length up to 256 kbytes
*               (numlens <= 17), 4 for 512 kbytes (numlens = 18), 2 for 1
*               Mbytes (numlens = 19), and 2 for 2 Mbyte (numlens = 20).
*		[Default: 1, Maximum: dependent on numlens - see above]
*
*	-m numlens
*		Number of message lengths used.  The 1st message length is 0.
*               Subsequent message lengths are given by 2^(i+1) bytes with
*               i starting at 2 (8 bytes, 16 bytes, 32 bytes, ..., 2 Mbyte).
*		[Default: 13 - up to 16 kbytes , Maximum: 20 - for 2 Mbyte].
*
*	-n numiter
*		This is the number of passes taken for each test.  It is
*               provided to allow direct control over the amount of elapsed
*               time between calls to the timer routines, thereby allowing for
*               greater accuracy.
*		[Default: 25]
*
*	-t type_spec
*		Test type flag. This flag may be used to run tests of a
*               particular type.  Type_spec may be any combination of the
*               letters below.
*		[Default: Run all tests.]
*
*                       p   Ping Pong
*                       e   Exchange
*                       r   Ring
*                       b   Broadcast
*                       f   Fan-in
*                       a   All-to-all
*                       l   Latency
*                       n   No Test (Dissables the default selection of all
*                           tests; does not override them if they are selected.)
*
*			Note: 
*			The fan-in test needs extended communication
*			buffers for messages of size larger than 4
*			kbytes.
*
*
*	-b hxw
*		Flag for the bisection test. Latency and bandwidth are
*		measured when messages are sent and then returned in a 
*		rectangular mesh, between:
*
*                       - the left half of the mesh, and its right half
*			  on 'horizontal' direction (this is a 'vertical
*			  mirror' exchange)
*                       - the upper half of the mesh, and its lower half
*                         on 'vertical' direction (this is a 'horizontal
*                         mirror' exchange)
*
*		It is the user's responsibility to insure that the partition
*		with h x w nodes is actually rectangular, with h rows and
*		w columns
*
*       -k hxw
*               Flag for the corner-to-corner test. Latency and bandwidth are
*               measured when messages are sent and then returned in a 
*               rectangular mesh, between:
*
*                       - the upper-left corner, and the upper-right corner
*                         on 'horizontal' direction
*                       - the upper-left corner, and the bottom-left corner
*                         on 'vertical' direction
*
*               It is the user's responsibility to insure that the partition
*               with h x w nodes is actually rectangular, with h rows and
*               w columns
*			
*	-r {1|2|3}
*		Flag for the 'random' test. It may be followed by any
*               combinations of 1, 2, and 3; e.g. '-r 13' selects execution
*               of random tests 1 and 3.  In test 1, a number of messages
*               equal to the number of participating nodes are sent from n
*               randomly chosen sources to n randomly chosen destinations.
*               Test 2 is similar, but the sources are restricted to the lower
*		part of the partition, and the destinations to the upper part.
*		In test 3 a random number of pairs of nodes from the
*               participating nodes exchange a random number of messages of
*               random lengths, a random number of times (the random values
*               are upper limited by the maximum specifications for the run).
*
*	-o	Flag for the order test.  Each node in turn receives messages
*               of lengths 256 kbytes, 0 bytes, and 8 bytes (in this order),
*               from 5 other nodes. The arrival order is tested, and an error
*               is reported if it is not identical with the departure order of
*               the messages. This test is not affected by the '-f' flag, and
*		is performed only for non-force typed messages.
*
*  NOTE:
*	If an option appears several times on the command line, the last
*       occurrence overrides any previous occurrences.
*
*  DESCRIPTION:
*	This program was designed to measure communication performance.  Since
*	it stresses the message passing system extensively, it can also be an
*	integral part of the Systems Acceptance Test.
*
*	Latency and bandwidth of the network hardware are measured along with
*	software latencies of both synchronous and asynchronous messages.  The
*	summary printout includes message passing bandwidth and latencies.
*	When run as an acceptance test or to measure the communication rates,
*	it is suggested that no other applications be running in the partition,
*       as these may significantly affect the performance.  Multiple copies of
*       this program running in the same partition, however, may be used to
*       measure the performance loss from mesh sharing.
*
*	Parameters which affect communication performance are covered by the
*	tests, and include:
*
*		1) Forced Type Messages
*		2) Synchronous and Asynchronous Messages
*
*	Specific kinds of tests include:
*
*		1) Ping-Pong (back and forth communications between only two
*                  nodes)
*			- near neighbor and far neighbor (nodes 0 and 1, and
*                         then, 0 and numnodes - 1, where numnodes is the total
*                         number of nodes)
*
*		2) Exchange (simultaneous data swap between only two nodes)
*			- near neighbor and far neighbor (nodes 0 and 1, and
*                         then, 0 and numnodes - 1)
*
*		3) Ring (pass to next higher node number, and back to node 0)
*
*		4) Broadcast (node 0 sends to all other nodes)
*
*		5) Fan-in
*			- this is the reverse of broadcast, i.e., all nodes
*                         send to a single node (node 0, and then, node
*                         numnodes - 1)
*
*               6) All-to-all (all the nodes, organized in pairs, exchange
*                         messages; numnodes - 1 rounds are neccesary if
*                         numnodes is a power of 2, and numnodes rounds
*                         otherwise)
*
*		7) Bisection: a rectangular mesh of height h and width w
*                         is assumed (the program only checks that h * w 
*			  == numnodes). 
*			- the latency and the bandwidth are measured in 
*			  two situations, when: first, all the 'nodes 
*			  on the left hand side' send and then get 
*		   	  back messages from their symmetrics 'on the right 
*			  hand side' (the axis of symmetry is vertical, and 
*			  may overlap a 'column' of nodes); second, all the 
*			  nodes in the 'upper half' send and then get back 
*			  messages with their symmetrics in the 'lower half' 
*			  (the axis of symmetry is horizontal).
*
*               8) Corner-to-corner: a rectangular mesh of height h and
*			  width w is assumed (the program only checks that
*                         h * w == numnodes).
*                       - the latency and the bandwidth are measured in 
*                         two situations, when: first, node 0 (upper left
*                         corner) sends and then gets back messages from the
*                         upper right corner ; second, node 0 (upper left corner) 
*			  corner) sends and then gets back messages from the
*			  lower left corner.
*
*               9) Random (all nodes send messages to all the other nodes,
*                         but with randomly chosen sources, destinations,
*                         message lengths, and numbers of iterations. The
*                         random test has three separate subtests (see the
*                         flag '-r' above))
*
*               10) Latency
*                       - measures the overhead (0 byte messages) of the
*                         message passing routines
*                       - Measures standard latency (alpha)
*			- the '-f' flag does not affect this test
*
*
*  IMPORTANT NOTE:
*
*	The numbers of bytes that may be received by a node in a short interval 
*	of time, and that may require buffering, are, for each test:
*
*		- pingpong: msgcnt * msglen   bytes
*		  (msgcnt = 1 and msglen <= 16 kbytes in the default 
*		  case, for numlens  = 13)
*		
*		- exchange: msgcnt * msglen   bytes
*                 (msgcnt = 1 and msglen <= 16 kbytes in the default 
*		  case, for numlens  = 13)
*               
*               - ring: msgcnt * msglen   bytes
*                 (msgcnt = 1 and msglen <= 16 kbytes in the default 
*		  case, for numlens  = 13)
*               
*               - broadcast: msgcnt * msglen  bytes
*                 (msgcnt = 1 and msglen <= 16 kbytes in the default 
*		  case, for numlens  = 13)
*               
*               - fanin: msgcnt * msglen * (numnodes - 1)  bytes
*		  (msgcnt = 1 and msglen <= 16 kbytes in the default
*		  case, for numlens  = 13; for a larger number of nodes, 
*		  a smaller value for numlens is necessary, and also,
*		  extended buffers for receiving messages - see the '-mbf'
*		  and '-mex' options) [likely to cause problems, if buffering
*		  space is not sufficient]
*
*		- all-to-all: msglen   bytes
*		  (msglen <= 16 kbytes in the default case, for numlens  = 13)
*
*               - bisection: msgcnt * msglen   bytes (msgcnt = 1 and
*                 msglen <= 16 kbytes in the default case, for numlens  = 13)
*
*               - corner-to-corner: msgcnt * msglen   bytes (msgcnt = 1 and
*                 msglen <= 16 kbytes in the default case, for numlens  = 13)
*
*               - order: 5 * (256 k + 8)   bytes
*
*		- random 1: at most msgcnt * msglen * numnodes  bytes
*                 (msgcnt = 1 and msglen <= 16 kbytes in the default
*                 case, for numlens  = 13; for a larger number of nodes,
*                 a smaller value for numlens is necessary, and also,
*                 extended buffers for receiving messages - see the '-mbf'
*                 and '-mex' options) [likely to cause problems, if buffering
*                 space is not sufficient]
*
*               - random 2: at most msgcnt * msglen * (numnodes / 2)  bytes
*                 (msgcnt = 1 and msglen <= 16 kbytes in the default
*                 case, for numlens  = 13; for a larger number of nodes,
*                 a smaller value for numlens is necessary, and also,
*                 extended buffers for receiving messages - see the '-mbf'
*                 and '-mex' options) [likely to cause problems, if buffering
*                 space is not sufficient]
*
*               - random 3: at most MAXCNT * msglen)  bytes
*		  (MAXCNT = 8 and msglen <= 16 kbytes in the default
*                 case, for numlens  = 13)
*
*
*  TO COMPILE FOR THE PARAGON:
*
*		icc -DPARAGON -o comtest comtest.c -nx -lm
*
*  TO COMPILE FOR THE IPSC/860:
*		
*		Comment out the definition for the environment variable PARAGON
*		in commtest.c, if present.
*
*		icc -o comtest comtest.c -lm
*  
*
*  USAGE EXAMPLES:
*
*
*	1) Find out info about the program:
*
*		comtest -h -pn my_partition
*
*	2) Run the pingpong, exchange, ring, broadcast, fanin, all-to-all,
*	   and latency tests, with the default settings: msgcnt = 1, 
*	   numiter = 25, message lengths from 0 bytes up to 16 kbytes
*          (numlens = 13), non-forced type messages, with error checking,
*          on my_partition:
*	   
*		comtest -pn my_partition
*
*	3) Run only the all-to-all test:
*
*		comtest -ta -pn my_partition
*
*	4) Run only the all-to-all test, with forced type messages and message
*	   lengths up to 32 kbytes:
*
*		comtest -f -ta -m 14 -pn my_partition
*	
*	5) Run only the 'random' tests, with msgcnt = 3:
*
*		comtest -tn -r 123 -c 3 -pn my_partition
*
*	6) Run only the third 'random' test and the order test, using 
*	   numiter = 50 iterations to gather data for statistical processing:
*
*		comtest -tn -r 3 -o -n 50 -pn my_partition
*
*	7) Run only the fan-in test, with extended communication buffers:
*
*		comtest -tf -mbf 6000000 -mex 5000000 -pn my_partition
*
*	8) Run the bisection test, on a rectangular mesh (previously
*	   allocated), with 4 rows and 5 columns:
*
*		comtest -tn -b 4x5 -pn my_partition
*
*       9) Run the corner-to-corner test, on a rectangular mesh (previously
*          allocated), with 4 rows and 5 columns, using force-type messages:
*
*               comtest -f -tn -k 4x5 -pn my_partition
*
*       10) Run only the ring test, with message lengths up to 2 Mbytes. Use
*          the -plk option to improve communication performance:
*
*               comtest -tr -m 20 -plk -pn my_partition
*
*       11) Run only the ring test, with message lengths up to 256 kbytes, a
*          message count of 8, and the -plk option:
*
*               comtest -tr -m 17 -c 8 -plk -pn my_partition
*
*	12) Run the pingpong, exchange, ring, broadcast, fanin, all-to-all,
*          and latency tests, with all the default settings, with one 
*	   exception - use communication routines without error checking
*	   (_csend, _isend, _crecv, _irecv). Use the '-i' flag to allow
*	   running again the test without exiting the program:
*
*		comtest -u -i -pn my_partition
*
*
*  PROGRAMMER'S NOTES:
*
*
*	1) Bandwidth
*	   Bandwidth is defined here as the aggregate data transfer rate.
*	   If N messages are sent during the course of a test, then
*	   the bandwidth figure for that test will be N*MSGLEN/T,
*	   where MSGLEN is the size of each message, and T is the time
*	   from when the first message is sent to when the last message
*	   is received.  Note that the number
*	   of bytes actually transferred is proportional to N.  Hence,
*	   for broadcast, for example, N is (numnodes-1) since the data
*	   must be transferred to numnodes-1 processors.
*
*	2) Timer Accuracy - Some of the tests utilize a round trip scheme to
*          allow only a single node's clock to be utilized.  In particular,
*          this is used in the latency test to obtain the highest degree of
*          accuracy.
*
*	3) Initial implementation uses only synchronous messages.  Since no CP
*	   cycles are in this code, there would be of little benefit to
*          asynchronous communications.  When the message coprocessor support
*          comes on line, it may be more significant to add asynchronous
*          support.
*
*	4) Hardware Latency.  This is the minimum delay within each iMRC and is
*	   given by:
*			37 ns (if no turn)
*			67 ns (if turn)
*	   Specs for the above are 40 and 80.
*	   There will be order L hardware latencies, as a worst case, where L
*          is the length or width of the mesh.  For the largest configurations,
*          32x32, this translates to about 32*40=1280 ns (number of turns in a
*          message is at most 1, hence average latency will be very close to
*          40 ns).  Since software latency is about 10,000 ns, this iMRC
*          latency will only be about 10% of software latency, even in a worst
*          case scenario.  It will not be included in the performance
*          estimates.
*
*	5) Hardware Bandwidth.  This is the transfer rate across the message
*          passing bus and is used in calculating expected performance.  It is
*          given by:
*			200 MB/Sec (fast streaming mode, the default)
*			150 MB/Sec (slow streaming mode)
*			 80 MB/Sec (interlocked mode)
*	   All calculations here use 200 MB/Sec.
*
*	6) This program is designed to run on a contiguous set of logical nodes.
*	   To measure the effect of running on different mesh shapes, setup a
*	   partition of the desired shape.
*
*
*  ERRORS:
*	1) Two types of run-time errors are monitored:
*		i) Data Validity --> All data is checked against expected
*                  values.  Any discrepancies are flagged.  Data checking is
*                  done outside all timing loops.
*              ii) Communication Performance Anomalies --> All tests are timed
*                  and compared with expected communication rates.  Any rates
*                  which fall outside a specified tolerance are reported as an
*                  error.  Note that communication rates are all computed using
*                  the wall clock timer dclock.  Hence, these rates may be
*                  significantly impacted by other jobs which may be running in
*                  the same compute partition.
*
*
*  FUTURE ENHANCEMENTS
*
*	Possible future enhancements may include (in random order):
*
*	1) Print the seed for a random test, and allow it as an input parameter,
*	   in order to be able to repeat a certain random test
*
*	2) Add tests for asynchronous sends and receives, with interrupt driven
*	   handlers
*
*	3) Add tests using multiple threads of control and/or multiple processes
*	   per node
*
*	4) Add a flag to allow generating plots corresponding to the numerical
*	   data gathered
*
*	5) Compute bandwidths with latency included, as well as without
*
*	6) Modify the '-m message_lengths_end' flag to 
*	   '-m message_lengths_begin message_lengths_end', with the default
*	   value 1 for message_lengths_begin
*
*	7) Add tests for global operations
*
*	8) Add tests for extended communication system calls
*
*  HISTORY:
*
*	Original Version --> Mike Martell, August 92
*       Added the all-to-all and random functions  --> 
*               Marius Cornea-Hasegan, June 1993
*       Added the bisection, corner-to-corner, and order tests ; added
*               statistics computation to all the tests -->
*               Marius Cornea-Hasegan, June 1993
*
*
*******************************************************************************/

/* M A C R O S  */

#define CSENDRECV(st,sb,sl,to,p,rt,rb,rl) ti.underscore ? \
	_csendrecv(st,sb,sl,to,p,rt,rb,rl) : csendrecv(st,sb,sl,to,p,rt,rb,rl)
#define CSEND(t,b,l,to,p) ti.underscore ? \
	_csend(t,b,l,to,p) : csend(t,b,l,to,p)
#define CRECV(t,b,l)      ti.underscore ? \
	_crecv(t,b,l) : crecv(t,b,l)
#define ISEND(t,b,l,to,p) ti.underscore ? \
	_isend(t,b,l,to,p) : isend(t,b,l,to,p)
#define IRECV(t,b,l)      ti.underscore ? \
	_irecv(t,b,l) : irecv(t,b,l)
#define PAGESIZE 4096		/* pagesize in bytes */

/* I N C L U D E   F I L E S  */

#ifdef PARAGON
#include <nx.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#else
#include <cube.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "comtest.h"

/* G L O B A L   V A R I A B L E S  */

int             errno;
int             node;		/* Mynode number */
int             nodes;		/* Number of nodes */
int             nodesm1;	/* Number of nodes - 1 */
struct TEST_INFO ti;		/* Contains all test parms */
struct STAT_INFO si;		/* Contains all statistics results */

char            *optarg;		/* Pointer to next option string */
int             optind;		/* Current option index */
double          s1[MAXITER], s2[MAXITER], s3[MAXITER];	/* Timing arrays */
double          S3[MAXLENS][MAXITER];
double          work[MAXITER];	/* Work array for gdhigh */

/* global variables and arrays for the 'random message passing' test		 */
unsigned int    seed;
long            extra_bytes[MAXLENS][MAXITER];
long            Extra[MAXLENS][MAXITER];
int             pair[MAXNODES];
int             rand_msglen_ind[MAXNODES];
int             rand_msgcnt[MAXNODES];
int             rand_numiter[MAXNODES];
double          S1[MAXLENS][MAXITER], S2[MAXLENS][MAXITER];	/* 3rd test */
double          Work[MAXLENS][MAXITER];

/*
 * Use doubles for message buffers for 16 byte alignment. Also, this causes
 * all bytes to be initialized and checked
 */

char           *bufs, *bufr;	/* Pointers to send/recv bufs */

/* struct rusage   r_usage; */	/* Resource usage structure */

extern double   dclock();

main(argc, argv)
	int             argc;
	char           *argv[];
{

	int             i;
	int             tmp;
	char            repeat[2];	/* the element repeat[1] 	 */

	/* Initialization */
	init();

	if (nodes == 1) {
		fprintf(stderr, "COMTEST - NODE %d : *** ERROR *** - USE AT LEAST"
			" 2 NODES\n", mynode());
		exit(1);
	}

	/* Set defaults */
	set_defaults();

	/* Parse command line, set parms */
	parse_cmd(argc, argv);

	/* Limit random tests to MAXNODES nodes */
	if (ti.random && (nodes > MAXNODES)) {
		if (node == 0) {
			fprintf(stderr, "COMTEST - NODE %d : *** ERROR *** - RUN RANDOM TESTS ON NO MORE THAN %d NODES\n", node, MAXNODES);
		}
		exit(1);
	}

	/* Print comtest header */
	print_hdr('a');

#ifdef DBG
	if (node == 0) {
		printf("comnode DBG: received values for ti are\n");
		printf("\tnumiter = %d\n", ti.numiter);
		printf("\tmsgcnt = %d\n", ti.msgcnt);
		printf("\tforce = %d\n", ti.force);
		printf("\thelp = %d\n", ti.help);
		printf("\tinteractive = %d\n", ti.interactive);
		printf("\tunderscore = %d\n", ti.underscore);
		printf("\tnodes = %d\n", ti.nodes);
		printf("\tpart_name = %s\n", ti.part_name);
		printf("\ttype_spec = %s\n", ti.type_spec);
		printf("\trandom = %d\n", ti.random);
		printf("\trand_spec = %s\n", ti.rand_spec);
		printf("\torder = %d\n", ti.order);
	}
#endif

	/* R U N   T E S T   D R I V E R S  */

REPEAT:

	if (ti.help) {
		if (node == 0) {

#ifdef PARAGON
			system("more comtest.syntax");
#else
			printf("While on the iPSC/860, please read the "
				"'comtest.syntax' file, or,\n"
			 "equivalently, the first part of 'comtest.c'\n\n");
#endif
		}
		exit(0);
	}
	if (do_test_type("p"))
		pingpong_drv();
	if (do_test_type("e"))
		exchange_drv();
	if (do_test_type("r"))
		ring_drv();
	if (do_test_type("b"))
		broadcast_drv();
	if (do_test_type("f"))
		fanin_drv();
	if (do_test_type("a"))
		alltoall_drv();
	if (do_test_type("l"))
		latency_drv();
	if (do_test_type("n"));

	if (ti.bisection) {
		/* Print comtest header */
		print_hdr('b');

		if (ti.width > 1)
			bisection_drv(H);
		if (ti.height > 1)
			bisection_drv(V);
	}

        if (ti.corner) {
                /* Print comtest header */
                print_hdr('k');

                if (ti.width > 1)
                        corner_drv(H);
                if (ti.height > 1)
                        corner_drv(V);
        }

	if (ti.random) {
		/* Print comtest header */
		print_hdr('r');

		random_drv();
	}
	if (ti.order) {
		/* Print comtest header */
		print_hdr('o');

		order_drv();
	}
	/* Check for repeat */

	if (ti.interactive) {
		if (node == 0) {
			printf("\n");
			printf("====================\n");
			printf("  Repeat? (y/n): ");
			scanf("%s", repeat);
			printf("\n");

#ifdef PARAGON
			kill(0, SIGCONT);
#endif



			csend(REPEAT_MSG, repeat, 1, -1, NODE_PTYPE);
		} else {

#ifdef PARAGON
			pause();
#endif

			crecv(REPEAT_MSG, repeat, 1);
		}

		if (repeat[0] == 'y' || repeat[0] == 'Y')
			goto REPEAT;
	}
	/* Dump getrusage parms */

	/*
	 * if (node == 0) { if (getrusage(RUSAGE_SELF,&r_usage) == -1) {
	 * fprintf(stderr, "COMTEST - NODE %d *** ERROR in getrusage: errno =
	 * %d\n", mynode(), errno); exit(1); } printf("\nGetrusage
	 * Results:\n\n"); printf("      Page Reclaims  =
	 * %d\n",r_usage.ru_minflt); printf("      Page Faults    =
	 * %d\n",r_usage.ru_majflt); printf("      Swaps          =
	 * %d\n",r_usage.ru_nswap); printf("      Messages Sent  =
	 * %d\n",r_usage.ru_msgsnd); printf("      Messages Recvd =
	 * %d\n",r_usage.ru_msgrcv); printf("      Signals Recvd  =
	 * %d\n",r_usage.ru_nsignals); printf("      Invol Con Sw.  =
	 * %d\n",r_usage.ru_nivcsw); }
	 */
}


/****** pingpong

  DESCRIPTION:
	Measures the time to pass a message over and back between 
	two processors.

*/

void
pingpong(n, passes, msgcnt, msglen)
	int             n;	/* Total Number of nodes */
	int             passes;	/* Number of passes over and back */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message (bytes) */
{
	int             i, ipass, imsg;
	int             numwords;
	void            getstats();
	double          zero = 0.0;


        /* Clean arrays for timing data         */
        for (i = 0; i < MAXITER; i++) {
                s1[i] = 0;
                s2[i] = 0;
                work[i] = 0;
        }

	gsync();

        /* Loop on passes */
        for (ipass = 0; ipass < passes; ipass++) {
                /* Clear the recv buffer */
                numwords = msglen / 8 * msgcnt;
                dcopy(numwords, &zero, 0, bufr, 1);

                s1[ipass] = dclock();

                /* Loop on msgcnt */
                for (imsg = 0; imsg < msgcnt; imsg++) {
			/* Start the handoff */
                        if (node == 0) {
                                CSEND(PINGPONG_MSG + imsg, bufs + imsg * msglen,
					msglen, n - 1, NODE_PTYPE);
			} /* Node n-1: Receive handoff, then pass back */
			else if (node == (n - 1)) {
                                CRECV(PINGPONG_MSG + imsg, bufr + imsg * msglen,
                                        msglen);
                                CSEND(PINGPONG_MSG + imsg, bufr + imsg *
                                        msglen, msglen, 0, NODE_PTYPE);
                        }
                }       /* msgcnt loop */

                /* First Node: Wait to get handoff back */
                if (node == 0) {
                        for (imsg = 0; imsg < msgcnt; imsg++) {
                                CRECV(PINGPONG_MSG + imsg, bufr + imsg * msglen,
                                        msglen);
                        }
                }

                s2[ipass] = dclock() - s1[ipass];

                /* Check the correctness of the received message */
               	if (compare(numwords, bufs, bufr)) {
			if(node == 0) {
                        	fprintf(stderr, "COMTEST - NODE %d : *** PINGPONG TEST "
                                	"ERROR ***\n", mynode());
                        	return;
                	}
		}
        }
}

/****** pingpong_f

  DESCRIPTION:
	Measures the time to pass a message over and back between two 
	processors using forced type messages.

*/

void
pingpong_f(n, passes, msgcnt, msglen)
	int             n;	/* Total Number of nodes */
	int             passes;	/* Number of passes over and back */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message (bytes) */
{
	int             i, ipass, imsg;
	void            getstats();
	int             rid[MAXCNT];	/* Recv msg ID's */
	int             tmp;
	int             numwords;
	double          zero = 0.0;

	/* Clean arrays for timing data         */
	for (i = 0; i < MAXITER; i++) {
		s1[i] = 0;
		s2[i] = 0;
		work[i] = 0;
	}

	gsync();

	/* Loop on passes */
	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);
		s1[ipass] = dclock();
		/* post recvs */
		if (node == 0 || node == (n - 1)) {
			for (imsg = 0; imsg < msgcnt; imsg++) {
				rid[imsg] = IRECV(PINGPONG_F_MSG + imsg, 
					bufr + imsg * msglen, msglen);
			}
		}
		/* Let other processor know he can force send to me */
		if (node == 0)
			CSEND(NUMITER_MSG + ipass, &tmp, 0, n - 1, NODE_PTYPE);
		else if (node == (n - 1))
			CSEND(NUMITER_MSG + ipass, &tmp, 0, 0, NODE_PTYPE);

		/* Wait for OK to force send to next processor */
		if (node == 0 || node == (n - 1))
			CRECV(NUMITER_MSG + ipass, &tmp, 0);

		/* Loop on msgcnt */

		for (imsg = 0; imsg < msgcnt; imsg++) {

			if (node == 0)
				/* Start the handoff */
				CSEND(PINGPONG_F_MSG + imsg, bufs + imsg * msglen, 
					msglen, n - 1, NODE_PTYPE);

			/* Node n-1: Receive handoff, then pass back */
			else if (node == (n - 1)) {
				msgwait(rid[imsg]);
				CSEND(PINGPONG_F_MSG + imsg, bufr + imsg * msglen, 
					msglen, 0, NODE_PTYPE);
			}
		}

		/* First Node: Wait to get handoff back */
		if (node == 0) {
			for (imsg = 0; imsg < msgcnt; imsg++) {
				msgwait(rid[imsg]);
			}
		}
		s2[ipass] = dclock() - s1[ipass];
		/* Check the correctness of the received message */
		if (compare(numwords, bufs, bufr)) {
			if (node == 0) {
				fprintf(stderr, "COMTEST - NODE %d : *** PING-PONG"
					" (FORCE) "
					"TEST ERROR ***\n", mynode());
				return;
			}
		}
	}
}


/****** pingpong_drv

  DESCRIPTION:
	This is the ping-pong driver routine. It calls the pingpong timing 
	test and prints results. The test is performed between near 
	neighbors and far neighbors.

*/

pingpong_drv()
{
	int             imsg;
	double          start, delta;
	double          bytes_t;
	int             icnt;
	int             jcnt;	/* DEBUG */
	int             ilen;
	int             i;

	/* Fill send buffer */
	start = 0;
	delta = 7.7777;
	dramp(BUFSIZE, start, delta, bufs);

	/* Header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   P I N G - P O N G   T I M I N G   S U M M A R Y\n");
		printf("\n");
		printf("   Latency Description:\n");
		printf("      This is half the time for one message in a group of \n");
                printf("      zero length messages to be passed to a neighbor and \n");
		printf("      back. Times for the nearest and the farthest neighbor\n");
		printf("      are given, and are per iteration. \n");
		printf("      Note: The highest numbered logical node is assumed to\n");
		printf("      be the farthest away. \n");
		printf("\n");
		printf("   Bandwidth Description:\n");
		printf("      This is aggregate bandwidth.  All data transfers are\n");
		printf("      included and consist of 2*msglen*msgcnt bytes.      \n");
		printf("      Note, times are per iteration.\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
	if (node == 0) {
		printf("        <---   Near Latency (usecs)  ---->   <-----  Far Latency (usecs)  ---->\n");
		printf("MSGCNT   Max    Min    Mean  Median Stddev    Max    Min    Mean  Median Stddev\n");
		printf("======  ====== ====== ====== ====== ======   ====== ====== ====== ====== ======\n");
	}
	if (ti.force)
		pingpong_f(ti.nodes, ti.numiter, 1, 0);	/* get code in cache */
	else
		pingpong(ti.nodes, ti.numiter, 1, 0);	/* get code in cache */

	/* Loop over message counts */

	for (jcnt = 1; jcnt <= 32 * MAXCNT; jcnt = jcnt * 2) {
		icnt = jcnt / 32;
		if (icnt == 0)
			icnt = 1;

		/* Near Latency measurement, zero length message */

		if (ti.force)
			pingpong_f(2, ti.numiter, icnt, 0);
		else
			pingpong(2, ti.numiter, icnt, 0);

		/* Take half for one-way trip only	 */
		for (i = 0; i < ti.numiter; i++)
			s2[i] = s2[i] / (2. * icnt);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Get statistics */
			getstats(s2, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf("%6d %7d%7d%7d%7d%7d", icnt,
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));
		}
		/* Far Latency measurement, zero length message */

		if (ti.force)
			pingpong_f(ti.nodes, ti.numiter, icnt, 0);
		else
			pingpong(ti.nodes, ti.numiter, icnt, 0);

		/* Take half for one-way trip only      */
		for (i = 0; i < ti.numiter; i++)
			s2[i] = s2[i] / (2. * icnt);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Get statistics */
			getstats(s2, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf(" %7d%7d%7d%7d%7d\n",
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));
		}
	}

	/* Bandwidth header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      ***************************************\n");
		printf("                      *   N E A R E S T   N E I G H B O R   *\n");
		printf("                      ***************************************\n");
		printf("\n");
		printf(" MSGLEN  <--------- T I M E  (ms) -------->  <-- B A N D W I D T H  (MB/S) -->\n");
		printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
		printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ====== ======\n");
	}
	if (ti.force)
		pingpong_f(2, ti.numiter, ti.msgcnt, 0);	/* Get code in cache */
	else
		pingpong(2, ti.numiter, ti.msgcnt, 0);	/* Get code in cache */

	for (ilen = 0; ilen < ti.numlens; ilen++) {

		/* Nearest Neighbor */

		if (ti.force)
			pingpong_f(2, ti.numiter, ti.msgcnt, ti.msglen[ilen]);
		else
			pingpong(2, ti.numiter, ti.msgcnt, ti.msglen[ilen]);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			int             k;
			/* Timing Statistics */

			/*
			 * DEBUG printf("Message length = %d\n\n",
			 * ti.msglen[ilen]);
			 * 
			 * for(k = 0 ; k < ti.numiter ; k++) printf("iter = %d
			 * time = %f\n", k, (float)s2[k]);
			 */

			getstats(s2, ti.numiter, &si);
			printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
			       ti.msglen[ilen],
			       si.max * 1000, si.min * 1000, si.mean * 1000,
			       si.median * 1000, si.stddev * 1000);

			/* Bandwidth Statistics */
			for (i = 0; i < ti.numiter; i++) {
				bytes_t = (double) ti.msgcnt * ti.msglen[ilen] * 2;
				s2[i] = bytes_t / s2[i] / 1e6;
			}
			getstats(s2, ti.numiter, &si);
			printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
			     si.max, si.min, si.mean, si.median, si.stddev);
		}
	}

	/*
	 * DEBUG return;
	 */

	/* Bandwidth header  - Farthest Neighbor */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      ***************************************\n");
		printf("                      *  F A R T H E S T   N E I G H B O R  *\n");
		printf("                      ***************************************\n");
		printf("\n");
		printf(" MSGLEN  <--------- T I M E  (ms) -------->  <-- B A N D W I D T H (MB/S) -->\n");
		printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
		printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ====== ======\n");
	}
	if (ti.force)
		pingpong_f(ti.nodes, ti.numiter, ti.msgcnt, 0);	/* Get code in cache */
	else
		pingpong(ti.nodes, ti.numiter, ti.msgcnt, 0);	/* Get code in cache */

	for (ilen = 0; ilen < ti.numlens; ilen++) {

		/* Farthest Neighbor */

		if (ti.force)
			pingpong_f(ti.nodes, ti.numiter, ti.msgcnt, ti.msglen[ilen]);
		else
			pingpong(ti.nodes, ti.numiter, ti.msgcnt, ti.msglen[ilen]);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Timing Statistics */
			getstats(s2, ti.numiter, &si);
			printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
			       ti.msglen[ilen],
			       si.max * 1000, si.min * 1000, si.mean * 1000,
			       si.median * 1000, si.stddev * 1000);

			/* Bandwidth Statistics */
			for (i = 0; i < ti.numiter; i++) {
				bytes_t = (double) ti.msgcnt * ti.msglen[ilen] * 2;

				/*
				 * s2[] contains times prior and bandwidths
				 * after
				 */
				/* this assignment				  */
				s2[i] = bytes_t / s2[i] / 1e6;
			}
			getstats(s2, ti.numiter, &si);
			printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
			     si.max, si.min, si.mean, si.median, si.stddev);
		}
	}
}


/****** exchange

  DESCRIPTION:
	Measures the time to exchange messages between two nodes.  Asynchronous
	receives are posted, to avoid deadlock situations which can occur when
	the msglen is large and both nodes send at the same time.

*/

void
exchange(n, passes, msgcnt, msglen)
	int             n;	/* Total Number of nodes */
	int             passes;	/* Number of passes over and back */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message (bytes) */
{
	int             i, ipass, imsg;
	void            getstats();
	int             rid[MAXCNT];	/* Recv msg IDs */
	int             numwords;
	double          zero = 0.0;

	/* Clean arrays for timing data         */
	for (i = 0; i < MAXITER; i++) {
		s1[i] = 0;
		s2[i] = 0;
		work[i] = 0;
	}

	/* Loop on passes */

	gsync();
	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);
		s1[ipass] = dclock();
		/* post recvs */
		if (node == 0 || node == (n - 1)) {
			for (imsg = 0; imsg < msgcnt; imsg++) {
				rid[imsg] = IRECV(EXCHANGE_MSG + imsg, 
					bufr + imsg * msglen, msglen);
			}
		}
		/* Loop on msgcnt */
		for (imsg = 0; imsg < msgcnt; imsg++) {
			if (node == 0) {
				CSEND(EXCHANGE_MSG + imsg, bufs + imsg * msglen, 
					msglen, n - 1, NODE_PTYPE);
			} else if (node == (n - 1)) {
				CSEND(EXCHANGE_MSG + imsg, bufs + imsg * msglen, 
					msglen, 0, NODE_PTYPE);
			}
		}

		/* Both Nodes: Wait for messages */
		if (node == 0 || node == (n - 1)) {
			for (imsg = 0; imsg < msgcnt; imsg++) {
				msgwait(rid[imsg]);
			}
		}
		s2[ipass] = dclock() - s1[ipass];
		/* Check the correctness of the received message */
		if (compare(numwords, bufs, bufr)) {
			if (node == 0 || node == (n - 1)) {
				fprintf(stderr, "COMTEST - NODE %d : *** EXCHANGE "
					"TEST ERROR ***\n", mynode());
				return;
			}
		}
	}
}

/****** exchange_f

  DESCRIPTION:
	Measures the time to exchange messages between two nodes using forced
	type messages.

*/

void
exchange_f(n, passes, msgcnt, msglen)
	int             n;	/* Total Number of nodes */
	int             passes;	/* Number of passes over and back */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message (bytes) */
{
	int             i, ipass, imsg;
	int             rid[MAXCNT];	/* Recv msg ID's */
	int             tmp;
	int             numwords;
	double          zero = 0.0;

	/* Clean arrays for timing data         */
	for (i = 0; i < MAXITER; i++) {
		s1[i] = 0;
		s2[i] = 0;
		work[i] = 0;
	}

	/* Loop on passes */
	gsync();
	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);
		s1[ipass] = dclock();

		/* post recvs */
		if (node == 0 || node == (n - 1)) {
			for (imsg = 0; imsg < msgcnt; imsg++) {
				rid[imsg] = IRECV(EXCHANGE_F_MSG + imsg, 
					bufr + imsg * msglen, msglen);
			}
		}
		/* Handshake for forced send */
		if (node == 0)
			CSEND(NUMITER_MSG + ipass, &tmp, 0, n - 1, NODE_PTYPE);
		else if (node == n - 1)
			CSEND(NUMITER_MSG + ipass, &tmp, 0, 0, NODE_PTYPE);

		/* Wait for OK to force send to next processor */
		if (node == 0 || node == (n - 1))
			CRECV(NUMITER_MSG + ipass, &tmp, 0);

		/* Loop on msgcnt */

		for (imsg = 0; imsg < msgcnt; imsg++) {
			if (node == 0)
				CSEND(EXCHANGE_F_MSG + imsg, bufs + imsg * msglen, 
					msglen, n - 1, NODE_PTYPE);
			else if (node == (n - 1))
				CSEND(EXCHANGE_F_MSG + imsg, bufs + imsg * msglen, 
					msglen, 0, NODE_PTYPE);
		}

		/* Both Nodes: Wait for messages */
		if (node == 0 || node == (n - 1))
			for (imsg = 0; imsg < msgcnt; imsg++)
				msgwait(rid[imsg]);
		s2[ipass] = dclock() - s1[ipass];
		/* Check the correctness of the received message */
		if (compare(numwords, bufs, bufr)) {
			if (node == 0 || node == (n - 1)) {
				fprintf(stderr, "COMTEST - NODE %d : *** EXCHANGE"
					"  (FORCE) "
					"TEST ERROR ***\n", mynode());
				return;
			}
		}
	}
}


/****** exchange_drv

  DESCRIPTION:
	This is the exchange driver routine.  It calls the exchange timing
	test and prints results.  The test is performed between near neighbors
	and far neighbors.

*/

exchange_drv()
{
	int             imsg;
	double          start, delta;
	double          bytes_t;
	int             icnt;
	int             jcnt;	/* DEBUG */
	int             ilen;
	int             i;

	/* Fill send buffer */
	start = 0;
	delta = 7.7777;
	dramp(BUFSIZE, start, delta, bufs);

	/* Header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   E X C H A N G E   T I M I N G   S U M M A R Y\n");
		printf("\n");
		printf("   Latency Description:\n");
		printf("      This is the time for one single message in a group \n");
                printf("      of zero length messages to be exchanged between \n");
		printf("      processors.  Times for the  nearest and the farthest \n");
		printf("      neighbor are given, and are per iteration.\n");
		printf("      Note: The highest numbered logical node is assumed to\n");
		printf("      be the farthest away. \n");
		printf("\n");
		printf("   Bandwidth Description:\n");
		printf("      This is aggregate bandwidth.  All data transfers are\n");
		printf("      included and consist of 2*msglen*msgcnt bytes.      \n");
		printf("      Note, times are per iteration.\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
	/* Loop over message counts */
	if (node == 0) {
		printf("        <---   Near Latency (usecs)  ---->   <-----  Far Latency (usecs)  ---->\n");
		printf("MSGCNT   Max    Min    Mean  Median Stddev    Max    Min    Mean  Median Stddev\n");
		printf("======  ====== ====== ====== ====== ======   ====== ====== ====== ====== ======\n");
	}
	if (ti.force)
		exchange_f(ti.nodes, ti.numiter, 1, 0);	/* get code in cache */
	else
		exchange(ti.nodes, ti.numiter, 1, 0);	/* get code in cache */

	/* DEBUG  for (icnt = 1; icnt <= MAXCNT; icnt = icnt * 2) { */

	/*
	 * will generate the sequence 1, 1, 2, 4, ... MAXCNT for icnt, but
	 * will
	 */
	/* not use the first 1 in determining the latency			 */
	for (jcnt = 1; jcnt <= 32 * MAXCNT; jcnt = jcnt * 2) {
		icnt = jcnt / 32;
		if (icnt == 0)
			icnt = 1;

		/* Near Latency measurement, zero length message */

		if (ti.force)
			exchange_f(2, ti.numiter, icnt, 0);
		else
			exchange(2, ti.numiter, icnt, 0);

		/* Take one message of length 0 only	 */
		for (i = 0; i < ti.numiter; i++)
			s2[i] = s2[i] / icnt;

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Get statistics */
			getstats(s2, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf("%6d %7d%7d%7d%7d%7d", icnt,
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));
		}
		/* Far Latency measurement, zero length message */

		if (ti.force)
			exchange_f(ti.nodes, ti.numiter, icnt, 0);
		else
			exchange(ti.nodes, ti.numiter, icnt, 0);

		/* Take one message of length 0 only    */
		for (i = 0; i < ti.numiter; i++)
			s2[i] = s2[i] / icnt;

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Get statistics */
			getstats(s2, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf(" %7d%7d%7d%7d%7d\n",
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));
		}
	}

	/* Bandwidth header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      ***************************************\n");
		printf("                      *   N E A R E S T   N E I G H B O R   *\n");
		printf("                      ***************************************\n");
		printf("\n");
		printf(" MSGLEN  <--------- T I M E  (ms) -------->  <-- B A N D W I D T H   (MB/S) -->\n");
		printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
		printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ====== ======\n");
	}
	/* Loop over different message lengths */

	if (ti.force)
		exchange_f(2, ti.numiter, ti.msgcnt, 0);	/* Get code in cache */
	else
		exchange(2, ti.numiter, ti.msgcnt, 0);	/* Get code in cache */

	for (ilen = 0; ilen < ti.numlens; ilen++) {

		/* Nearest Neighbor */

		if (ti.force)
			exchange_f(2, ti.numiter, ti.msgcnt, ti.msglen[ilen]);
		else
			exchange(2, ti.numiter, ti.msgcnt, ti.msglen[ilen]);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Timing Statistics */
			getstats(s2, ti.numiter, &si);
			printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
			       ti.msglen[ilen],
			       si.max * 1000, si.min * 1000, si.mean * 1000,
			       si.median * 1000, si.stddev * 1000);

			/* Bandwidth Statistics */
			for (i = 0; i < ti.numiter; i++) {
				bytes_t = (double) ti.msgcnt * ti.msglen[ilen] * 2;
				s2[i] = bytes_t / s2[i] / 1e6;
			}
			getstats(s2, ti.numiter, &si);
			printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
			     si.max, si.min, si.mean, si.median, si.stddev);
		}
	}

	/* Bandwidth header  - Farthest Neighbor */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      *****************************************\n");
		printf("                      *   F A R T H E S T   N E I G H B O R   *\n");
		printf("                      *****************************************\n");
		printf("\n");
		printf(" MSGLEN  <--------- T I M E  (ms) -------->  <-- B A N D W I D T H  (MB/S) -->\n");
		printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
		printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ====== ======\n");
	}
	if (ti.force)
		exchange_f(ti.nodes, ti.numiter, ti.msgcnt, 0);	/* Get code in cache */
	else
		exchange(ti.nodes, ti.numiter, ti.msgcnt, 0);	/* Get code in cache */

	for (ilen = 0; ilen < ti.numlens; ilen++) {

		/* Farthest Neighbor */

		if (ti.force)
			exchange_f(ti.nodes, ti.numiter, ti.msgcnt, ti.msglen[ilen]);
		else
			exchange(ti.nodes, ti.numiter, ti.msgcnt, ti.msglen[ilen]);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Timing Statistics */
			getstats(s2, ti.numiter, &si);
			printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
			       ti.msglen[ilen],
			       si.max * 1000, si.min * 1000, si.mean * 1000,
			       si.median * 1000, si.stddev * 1000);

			/* Bandwidth Statistics */
			for (i = 0; i < ti.numiter; i++) {
				bytes_t = (double) ti.msgcnt * ti.msglen[ilen] * 2;
				s2[i] = bytes_t / s2[i] / 1e6;
			}
			getstats(s2, ti.numiter, &si);
			printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
			     si.max, si.min, si.mean, si.median, si.stddev);
		}
	}
}


/****** ring

  DESCRIPTION:
	Measures the time to pass messages around a ring of processors.

*/

void
ring(n, passes, msgcnt, msglen)
	int             n;	/* Number of nodes in the ring */
	int             passes;	/* Number of passes around the ring */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message (bytes) */
{
	int             i, ipass, imsg;
	void            getstats();
	double          zero = 0.0;
	int             numwords;

	/* Loop on passes */

	gsync();
	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);

		s1[ipass] = dclock();
		/* Loop on msgcnt */
		for (imsg = 0; imsg < msgcnt; imsg++) {
			/* Start the handoff */
			if (node == 0) {
				CSEND(RING_MSG + imsg, bufs + imsg * msglen, 
					msglen, 1, NODE_PTYPE);
			}
			/* Other nodes: Receive handoff, then pass on */
			else {
				CRECV(RING_MSG + imsg, bufr + imsg * msglen, 
					msglen);
				if (node == (n - 1)) {
					CSEND(RING_MSG + imsg, bufr + imsg * 
						msglen, msglen, 0, NODE_PTYPE);
				} else {
					CSEND(RING_MSG + imsg, bufr + imsg * 
						msglen, msglen, node + 1, 
						NODE_PTYPE);
				}
			}
		}		/* msgcnt loop */

		/* First Node: Wait to get handoff back */
		if (node == 0) {
			for (imsg = 0; imsg < msgcnt; imsg++) {
				CRECV(RING_MSG + imsg, bufr + imsg * msglen, 
					msglen);
			}
		}
		s2[ipass] = dclock() - s1[ipass];
		/* Check the correctness of the received message */
		if (compare(numwords, bufs, bufr)) {
			fprintf(stderr, "COMTEST - NODE %d : *** RING TEST "
				"ERROR ***\n", mynode());
			return;
		}
	}			/* ipass loop */
}

/****** ring_f

  DESCRIPTION:
	Measures the time to pass messages around a ring of processors
	using forced type messages.

*/

void
ring_f(n, passes, msgcnt, msglen)
	int             n;	/* Number of nodes in the ring */
	int             passes;	/* Number of passes around the ring */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message (bytes) */
{
	int             i, ipass, imsg;
	int             rid[MAXCNT];	/* Recv msg ID's */
	void            getstats();
	int             tmp;
	double          zero = 0.0;
	int             numwords;

	/* Loop on passes */

	gsync();
	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);

		s1[ipass] = dclock();
		/* Loop on msgcnt */
		/* All nodes post recvs */
		for (imsg = 0; imsg < msgcnt; imsg++) {
			rid[imsg] = IRECV(RING_F_MSG + imsg, bufr + imsg * msglen, 
				msglen);
		}

		/* Let previous processor know he can force send to me */
		if (node == 0)
			CSEND(NUMITER_MSG + ipass, &tmp, 0, nodesm1, NODE_PTYPE);
		else
			CSEND(NUMITER_MSG + ipass, &tmp, 0, node - 1, NODE_PTYPE);

		/* Wait for OK to force send to next processor */
		CRECV(NUMITER_MSG + ipass, &tmp, 0);

		for (imsg = 0; imsg < msgcnt; imsg++) {
			if (node == 0) {
				/* Start the handoff */
				CSEND(RING_F_MSG + imsg, bufs + imsg * msglen, 
					msglen, 1, NODE_PTYPE);
			} else {
				/* Other nodes: Receive handoff, then pass on */
				msgwait(rid[imsg]);
				if (node == (n - 1))
					CSEND(RING_F_MSG + imsg, bufr + imsg * 
						msglen, msglen, 0, NODE_PTYPE);
				else
					CSEND(RING_F_MSG + imsg, bufr + imsg * 
						msglen, msglen, node + 1, NODE_PTYPE);
			}
		}

		/* First Node: Wait to get handoff back */
		if (node == 0) {
			for (imsg = 0; imsg < msgcnt; imsg++) {
				msgwait(rid[imsg]);
			}
		}
		s2[ipass] = dclock() - s1[ipass];
		/* Check the correctness of the received message */
		if (compare(numwords, bufs, bufr)) {
			fprintf(stderr, "COMTEST - NODE %d : *** RING (FORCE) "
				"TEST ERROR ***\n", mynode());
			return;
		}
	}
}


/****** ring_drv

  DESCRIPTION:
	This is the ring driver routine. It calls the ring timing test and 
	prints results.

*/

ring_drv()
{
	int             imsg;
	double          start, delta;
	double          bytes_t;
	int             icnt;
	int             jcnt;	/* DEBUG */
	int             ilen;
	int             i;

	/* Fill send buffer */
	start = 0;
	delta = 7.7777;
	dramp(BUFSIZE, start, delta, bufs);

	/* Header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   R I N G   T I M I N G   S U M M A R Y\n");
		printf("\n");
		printf("   Latency Description:\n");
		printf("      This is the time for a group of zero length messages\n");
		printf("      to be passed around a ring of nodes. Also given is\n");
		printf("      the time necessary for one hop (from a node, to the\n");
		printf("      next one in the ring ). In this latter case, the \n");
                printf("      times are given for a single message in the group of\n");
		printf("      zero length messages\n");
 		printf("      Note, times are per iteration.\n");
		printf("\n");
		printf("   Bandwidth Description:\n");
		printf("      This is aggregate bandwidth.  All data transfers are\n");
		printf("      included and consist of nodes*msglen*msgcnt bytes.  \n");
		printf("      Note, times are per iteration.\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
	if (node == 0) {
		printf("        <---   Ring Latency (usecs)  ---->   <---- 1 Hop Latency (usecs)  ---->\n");
		printf("MSGCNT   Max    Min    Mean  Median Stddev    Max    Min    Mean  Median Stddev\n");
		printf("======  ====== ====== ====== ====== ======   ====== ====== ====== ====== ======\n");
	}
	if (ti.force)
		ring_f(ti.nodes, ti.numiter, 1, 0);	/* get ring code in
							 * cache */
	else
		ring(ti.nodes, ti.numiter, 1, 0);	/* get ring code in
							 * cache */

	/* Loop over message counts */
	for (jcnt = 1; jcnt <= 32 * MAXCNT; jcnt = jcnt * 2) {
		icnt = jcnt / 32;
		if (icnt == 0)
			icnt = 1;

		/* Latency measurement, zero length message */
		if (ti.force)
			ring_f(ti.nodes, ti.numiter, icnt, 0);
		else
			ring(ti.nodes, ti.numiter, icnt, 0);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		/* Take 1 / (ti.nodes + icnt - 1) for one hop only      */
		for (i = 0; i < ti.numiter; i++)
			s3[i] = s2[i] / (ti.nodes + icnt - 1.);

		if (node == 0) {
			/* Get statistics for the whole ring */
			getstats(s2, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf("%6d %7d%7d%7d%7d%7d", icnt,
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));

			/* Get statistics for one hop */
			getstats(s3, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf(" %7d%7d%7d%7d%7d\n",
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));
		}
	}

	/* Bandwidth header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      **********************************************\n");
		printf("                      *   B A N D W I D T H   S T A T I S T I C S  *\n");
		printf("                      **********************************************\n");
		printf("\n");
		printf(" MSGLEN  <--------- T I M E  (ms) -------->  <-- B A N D W I D T H   (MB/S) -->\n");
		printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
		printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ====== ======\n");
	}
	if (ti.force)
		ring_f(ti.nodes, ti.numiter, ti.msgcnt, 0);	/* Get code in cache */
	else
		ring(ti.nodes, ti.numiter, ti.msgcnt, 0);	/* Get code in cache */

	/* Loop over different message lengths */
	for (ilen = 0; ilen < ti.numlens; ilen++) {

		if (ti.force)
			ring_f(ti.nodes, ti.numiter, ti.msgcnt, ti.msglen[ilen]);
		else
			ring(ti.nodes, ti.numiter, ti.msgcnt, ti.msglen[ilen]);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Timing Statistics */
			getstats(s2, ti.numiter, &si);
			printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
			       ti.msglen[ilen],
			       si.max * 1000, si.min * 1000, si.mean * 1000,
			       si.median * 1000, si.stddev * 1000);

			/* Bandwidth Statistics */
			for (i = 0; i < ti.numiter; i++) {
				bytes_t = (double) ti.msgcnt * ti.msglen[ilen] * 
					ti.nodes;

				/*
				 * s2[] contains times before, and bandwidths
				 * after
				 */
				/* the assignment					 */
				s2[i] = bytes_t / s2[i] / 1e6;
			}
			getstats(s2, ti.numiter, &si);
			printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
			     si.max, si.min, si.mean, si.median, si.stddev);
		}
	}
}


/****** broadcast

  DESCRIPTION:
	Perform a number of broadcasts of a given length and return the time.

*/

void
broadcast(n, passes, msgcnt, msglen)
	int             n;	/* Number of nodes used */
	int             passes;	/* Number of passes  */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message */
{
	int		i;
	int             ipass, imsg;
	int             numwords;
	double          zero = 0.0;
	void            getstats();

        /* Clean arrays for timing data         */
        for (i = 0; i < MAXITER; i++) {
                s1[i] = 0;
                s2[i] = 0;
                work[i] = 0;
        }

        gsync();

        /* Loop on passes */
	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);

		s1[ipass] = dclock();
		/* Loop on msgcnt */
		for (imsg = 0; imsg < msgcnt; imsg++) {
			/* Node 0 does the broadcast */
			if (node == 0)
				CSEND(BROADCAST_MSG + imsg, bufs + imsg * msglen, 
					msglen, -1, NODE_PTYPE);

			/* Other nodes: Receive handoff, then pass on */
			else
				CRECV(BROADCAST_MSG + imsg, bufr + imsg * msglen, 
					msglen);
		}
		s2[ipass] = dclock() - s1[ipass];
		/* Check the correctness of the received message */
		if (node != 0) {
			if (compare(numwords, bufs, bufr)) {
				fprintf(stderr, "COMTEST - NODE %d : *** BROADCAST "
					"TEST ERROR ***\n", mynode());
				return;
			}
		}
	}			/* ipass loop */
}

/****** broadcast_f

  DESCRIPTION:
	Perform a number of broadcasts of a given length and return the time
	using forced type messages.

  PROGRAMMER'S NOTES:
	1) The use of msgcnt>1 can be used to amortize the overhead of having
	node 0 (the broadcasting node) wait for the OK from all nodes before
	doing the broadcast.

	2) Note that the overhead of the OK msg may be greater than the benefits
	of the one trip protocol.  This is definitely true on the 860.

*/

void
broadcast_f(n, passes, msgcnt, msglen)
	int             n;	/* Number of nodes used */
	int             passes;	/* Number of passes */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message */
{
	int		i;
	int             ipass, imsg;
	int             rid[MAXCNT];	/* Recv msg ID's */
	int             tmp;
	int             inode;
	int             numwords;
	double          zero = 0.0;
	void            getstats();

        /* Clean arrays for timing data         */
        for (i = 0; i < MAXITER; i++) {
                s1[i] = 0;
                s2[i] = 0;
                work[i] = 0;
        }

        gsync();

        /* Loop on passes */
	for (ipass = 0; ipass < passes; ipass++) {
		/* Check the correctness of the received message */
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);

		s1[ipass] = dclock();
		/* All nodes (except node 0) post recvs */
		if (node != 0) {
			for (imsg = 0; imsg < msgcnt; imsg++) {
				rid[imsg] = IRECV(BROADCAST_F_MSG + imsg, 
					bufr + imsg * msglen, msglen);
			}

			/* Let node 0 know he can force send to me */
			CSEND(NUMITER_MSG + ipass, &tmp, 0, 0, NODE_PTYPE);
		}
		/* Node 0 waits for OK to force send from all nodes */
		else
			for (inode = 1; inode < nodes; inode++)
				CRECV(NUMITER_MSG + ipass, &tmp, 0);

		/* Loop on msgcnt */

		for (imsg = 0; imsg < msgcnt; imsg++) {

			/* Node 0 does the broadcast */
			if (node == 0)
				CSEND(BROADCAST_F_MSG + imsg, bufs + imsg * msglen, 
					msglen, -1, NODE_PTYPE);

			/* Other nodes: Receive broadcast */
			else
				msgwait(rid[imsg]);
		}
		s2[ipass] = dclock() - s1[ipass];
		/* Check the correctness of the received message */
		if (node != 0) {
			if (compare(numwords, bufs, bufr)) {
				fprintf(stderr, "COMTEST - NODE %d : *** BROADCAST "
				      "(FORCE) TEST ERROR ***\n", mynode());
				return;
			}
		}
	}			/* ipass loop */
}


/****** broadcast_drv

  DESCRIPTION:
	This is the broadcast driver routine. It calls the broadcast timing
	test and prints results. Time is measured from the start of the 
	broadcast (sent by node 0) to when the last node receives the message.

*/

broadcast_drv()
{
	int             imsg;
	double          start, delta;
	double          bytes_t;
	int             icnt;
	int             jcnt;	/* DEBUG */
	int             ilen;
	int             i;
	int             log2;

	/* Fill send buffer */
	start = 0;
	delta = 7.7777;
	dramp(BUFSIZE, start, delta, bufs);

	/* Header */
	if (node == 0) {
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   B R O A D C A S T   T I M I N G   S U M M A R Y\n");
		printf("\n");
		printf("   Latency Description:\n");
		printf("      This is the time for a group of zero length messages\n");
		printf("      to be broadcast to all the nodes. Also, the time \n");
                printf("      necessary for the messages of length 0 to reach the\n");
                printf("      'next level down' in the minimum height spanning\n");
                printf("      tree used for broadcast, is given. In this latter\n");
                printf("      case, the times are given for one single message in\n");
                printf("      the group of zero length messages. A height of\n");
                printf("      log2(numnodes) of the spanning tree is assumed. \n"); 
                printf("      Note, times are per iteration.\n");
		printf("\n");
		printf("   Bandwidth Summary:\n");
		printf("      This is aggregate bandwidth.  All data transfers are\n");
		printf("      are included and consist of (nodes-1)*msglen*msgcnt \n");
		printf("      bytes.  Note, times are per iteration.\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
	if (node == 0) {
		printf("        <-- Broadcast Latency  (usecs) -->   <---  1 Level Latency (usecs) --->\n");
		printf("MSGCNT   Max    Min    Mean  Median Stddev    Max    Min    Mean  Median Stddev\n");
		printf("======  ====== ====== ====== ====== ======   ====== ====== ====== ====== ======\n");
	}
	if (ti.force)
		broadcast_f(ti.nodes, ti.numiter, 1, 0);	/* Get broadcast in
								 * cache */
	else
		broadcast(ti.nodes, ti.numiter, 1, 0);	/* Get broadcast in
							 * cache */

	/* Loop over message counts */
	for (jcnt = 1; jcnt <= 32 * MAXCNT; jcnt = jcnt * 2) {
		icnt = jcnt / 32;
		if (icnt == 0)
			icnt = 1;

		/* Latency measurement, zero length message */
		if (ti.force)
			broadcast_f(ti.nodes, ti.numiter, icnt, 0);
		else
			broadcast(ti.nodes, ti.numiter, icnt, 0);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		/*
		 * Take only 1 / (log(ti.nodes)) to account for the spanning
		 * tree
		 */

		log2 = log10(ti.nodes) / log10(2); /* take the integral part */

		for (i = 0; i < ti.numiter; i++)
			s3[i] = s2[i] / (log2 * icnt);

		if (node == 0) {
			/* Get statistics for the entire broadcast */
			getstats(s2, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf("%6d %7d%7d%7d%7d%7d", icnt,
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));

			/* Get statistics for one level of the spanning tree */
			getstats(s3, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf(" %7d%7d%7d%7d%7d\n",
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));

		}
	}

	/* Bandwidth header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      **********************************************\n");
		printf("                      *   B A N D W I D T H   S T A T I S T I C S  *\n");
		printf("                      **********************************************\n");
		printf("\n");
		printf(" MSGLEN  <--------- T I M E  (ms) -------->  <-- B A N D W I D T H   (MB/S) -->\n");
		printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
		printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ====== ======\n");
	}
	/* Loop over different message lengths */
	for (ilen = 0; ilen < ti.numlens; ilen++) {

		if (ti.force)
			broadcast_f(ti.nodes, ti.numiter, ti.msgcnt,
				    ti.msglen[ilen]);
		else
			broadcast(ti.nodes, ti.numiter, ti.msgcnt,
				  ti.msglen[ilen]);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Timing Statistics */
			getstats(s2, ti.numiter, &si);
			printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
			       ti.msglen[ilen],
			       si.max * 1000, si.min * 1000, si.mean * 1000,
			       si.median * 1000, si.stddev * 1000);

			/* Bandwidth Statistics */
			for (i = 0; i < ti.numiter; i++) {
				bytes_t = (double) ti.msgcnt * ti.msglen[ilen] * 
					(ti.nodes - 1);

				/*
				 * s2[] contains times before, and bandwidths
				 * after
				 */
				/* the assignment                                       */
				s2[i] = bytes_t / s2[i] / 1e6;
			}
			getstats(s2, ti.numiter, &si);
			printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
			     si.max, si.min, si.mean, si.median, si.stddev);
		}
	}
}


/****** fanin

  DESCRIPTION:
	Perform a number of round trip fanins of a given length and return the
	time for a single divided by two.

*/

void
fanin(n, passes, msgcnt, msglen)
	int             n;	/* Number of nodes used */
	int             passes;	/* Number of passes */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message */
{

	int             ipass, imsg;
	int             i;
	int             numwords;
	double          zero = 0.0;

	/* Clean arrays for timing data         */
	for (i = 0; i < MAXITER; i++) {
		s1[i] = 0;
		s2[i] = 0;
		work[i] = 0;
	}

	/* Loop on passes */

	gsync();
	for (ipass = 0; ipass < passes; ipass++) {
		s1[ipass] = dclock();
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);

		/* Loop on msgcnt */

		/* Step 1: Send to last node */

		for (imsg = 0; imsg < msgcnt; imsg++) {

			/* All but last node send */
			if (node < n - 1) {
				CSEND(FANIN_MSG + node + msgcnt * n, bufs + 
					imsg * msglen, msglen, n - 1, NODE_PTYPE);
			}

			/*
			 * Last node acts as collector. Collect messages from
			 * different nodes into single buffer for now.
			 */
			if (node == n - 1) {
				for (i = 0; i < n - 1; i++) {
					CRECV(FANIN_MSG + i + msgcnt * n, 
						bufr + imsg * msglen, msglen);
				}
			}
		}

		gsync();

		/* Step 2: Send to first node */

		for (imsg = 0; imsg < msgcnt; imsg++) {

			/* All but first node send */
			if (node > 0) {
				CSEND(FANIN_MSG + node + msgcnt * n, bufr + 
					imsg * msglen, msglen, 0, NODE_PTYPE);
			}

			/*
			 * First node acts as collector. Collect messages
			 * from different nodes into single buffer for now.
			 */
			if (node == 0) {
				for (i = 0; i < n - 1; i++) {
					CRECV(FANIN_MSG + i + 1 + msgcnt * n, 
						bufr + imsg * msglen, msglen);
				}
			}
		}
		/* Divide by two since two fan-in operations */
		s2[ipass] = (dclock() - s1[ipass]) / 2.0;
		/* Check the correctness of the received message */
		if (node == 0 || node == n - 1) {
			if (compare(numwords, bufs, bufr)) {
				fprintf(stderr, "COMTEST - NODE %d : *** FANIN "
					"TEST ERROR ***\n", mynode());
				return;
			}
		}
	}			/* ipass loop */
}

/****** fanin_f

  DESCRIPTION:
	Perform a number of fanins of a given length and return the time
	using forced type messages.

*/

void
fanin_f(n, passes, msgcnt, msglen)
	int             n;	/* Number of nodes used */
	int             passes;	/* Number of passes */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message */
{

	int             ipass, imsg;
	int             rid[MAXCNT][MAXNODES];	/* Recv msg ID's */
	int             tmp;
	int             inode;
	int             i;
	int             numwords;
	double          zero = 0.0;

	/* Clean arrays for timing data         */
	for (i = 0; i < MAXITER; i++) {
		s1[i] = 0;
		s2[i] = 0;
		work[i] = 0;
	}

	/* Loop on passes */

	gsync();
	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);
		s1[ipass] = dclock();

		/*
		 * First and last node post all recvs. Use same buffer for
		 * each node for now.
		 */
		if (node == 0 || node == n - 1) {
			for (imsg = 0; imsg < msgcnt; imsg++) {
				for (i = 0; i < n - 1; i++) {
					rid[imsg][i] = IRECV(FANIN_F_MSG + imsg, 
						bufr + imsg * msglen, msglen);
				}
			}
		}
		/* Use gsync for now to synchronize */
		gsync();


		/* S T E P   1:  Send to Last Node */

		/* Loop on msgcnt */

		for (imsg = 0; imsg < msgcnt; imsg++) {

			/* All but last node send */
			if (node < n - 1)
				CSEND(FANIN_F_MSG + imsg, bufs + imsg * msglen, 
					msglen, n - 1, NODE_PTYPE);

			/* Last node collects messages. */
			else {
				for (i = 0; i < n - 1; i++) {
					msgwait(rid[imsg][i]);
				}
			}
		}

		/* S T E P   2:  Send to First Node */

		/* Loop on msgcnt */

		for (imsg = 0; imsg < msgcnt; imsg++) {

			/* All but first node send */
			if (node > 0)
				CSEND(FANIN_F_MSG + imsg, bufs + imsg * msglen, 
					msglen, 0, NODE_PTYPE);

			/* First node collects messages. */
			else {
				for (i = 0; i < n - 1; i++) {
					msgwait(rid[imsg][i]);
				}
			}
		}
		/* Divide by two since two fan-in operations */
		s2[ipass] = (dclock() - s1[ipass]) / 2.0;
		/* Check the correctness of the received message */
		if (node == 0 || node == n - 1) {
			if (compare(numwords, bufs, bufr)) {
				fprintf(stderr, "COMTEST - NODE %d : *** FANIN "
					"(FORCE) TEST ERROR ***\n", mynode());
				return;
			}
		}
	}			/* ipass loop */
}


/****** fanin_drv

  DESCRIPTION:
	This is the fan-in driver routine.  It calls the fan-in timing test
	and prints results.  This is a round trip test where nodes 0 through 
	N-2 send to node N-1, followed by nodes 1 through N-1 sending back to 
	node 0.

*/

fanin_drv()
{
	int             imsg;
	double          start, delta;
	double          abw;
	double          bytes_t;
	int             icnt;
	int             jcnt;	/* DEBUG */
	int             ilen;
	int             i;

	/* Fill send buffer */
	start = 0;
	delta = 7.7777;
	dramp(BUFSIZE, start, delta, bufs);

	/* Header */
	if (node == 0) {

		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   F A N I N   T I M I N G   S U M M A R Y\n");
		printf("\n");
		printf("   Latency Description:\n");
		printf("      This is the time for a group of zero length messages\n");
		printf("      to be fanned-in. Also, the time necessary for one\n");
                printf("      message of the group to be fanned-in, is given.\n");
                printf("      Note, times are per iteration. \n");
		printf("\n");
		printf("   Bandwidth Summary:\n");
		printf("      This is aggregate bandwidth.  All data transfers are\n");
		printf("      are included and consist of (nodes-1)*msglen*msgcnt\n");
		printf("	  bytes.  Note, times are per iteration.\n");
                printf("\n");
                printf("\n");
                printf("      W A R N I N G : THIS TEST WILL NOT WORK FOR MORE THAN\n");
                printf("      256 NODES. FOR A LARGER NUMBER OF NODES, USE REDUCED\n");
                printf("      MESSAGE LENGTHS. FOR EXAMPLE :\n");
                printf("      pexec \"comtest -tf -m 9 -mbf 6000000 -mex 5000000\" "
                              "-pn mypartition\n");
                printf("\n");
                printf("      ALWAYS USE EXTENDED COMMUNICATION BUFFERS.\n");
		printf("\n");
		printf("      FOR FORCED TYPE MESSAGES, THIS TEST CANNOT BE RUN IN \n");
		printf("      MORE THAN 125 NODES BECAUSE OF THE LIMITED NUMBER OF\n");
		printf("      IRECV-S THAT CAN BE POSTED.\n");
                printf("\n");

		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
	if (node == 0) {
		printf("        <---   Fanin Latency (usecs)  --->   <---- 1 Msg Latency (usecs)  ---->\n");
		printf("MSGCNT   Max    Min    Mean  Median Stddev    Max    Min    Mean  Median Stddev\n");
		printf("======  ====== ====== ====== ====== ======   ====== ====== ====== ====== ======\n");
	}
	if (ti.force)
		fanin_f(ti.nodes, ti.numiter, 1, 0);	/* Get fan-in in cache */
	else
		fanin(ti.nodes, ti.numiter, 1, 0);	/* Get fan-in in cache */

	/* Loop over message counts */
	for (jcnt = 1; jcnt <= 32 * MAXCNT; jcnt = jcnt * 2) {
		icnt = jcnt / 32;
		if (icnt == 0)
			icnt = 1;

		/* 980 is ABOUT the limit for the number of irecv-s that may be posted */
		if(icnt * ti.nodes > 980)
			continue;

		/* Latency measurement, zero length message */
		if (ti.force)
			fanin_f(ti.nodes, ti.numiter, icnt, 0);
		else
			fanin(ti.nodes, ti.numiter, icnt, 0);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		/* Take only 1 / (ti.nodes - 1) for one incoming message */
		for (i = 0; i < ti.numiter; i++)
			s3[i] = s2[i] / ((ti.nodes - 1.) * icnt);

		if (node == 0) {
			/* Get statistics */
			getstats(s2, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf("%6d %7d%7d%7d%7d%7d", icnt,
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));

			/* Get statistics for one incoming message */
			getstats(s3, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf(" %7d%7d%7d%7d%7d\n",
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));
		}
	}

	/* Bandwidth header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      **********************************************\n");
		printf("                      *   B A N D W I D T H   S T A T I S T I C S  *\n");
		printf("                      **********************************************\n");
		printf("\n");
		printf(" MSGLEN  <--------- T I M E  (ms) -------->  <-- B A N D W I D T H   (MB/S) -->\n");
		printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
		printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ====== ======\n");
	}
	if (ti.force)
		fanin_f(ti.nodes, ti.numiter, 1, 0);	/* Get fan-in in cache */
	else
		fanin(ti.nodes, ti.numiter, 1, 0);	/* Get fan-in in cache */

	/* Loop over different message lengths */
	for (ilen = 0; ilen < ti.numlens; ilen++) {

		/* if (ti.force && ti.msglen[ilen]>100) */
		if (ti.force)
			fanin_f(ti.nodes, ti.numiter, ti.msgcnt,
				ti.msglen[ilen]);
		else
			fanin(ti.nodes, ti.numiter, ti.msgcnt, ti.msglen[ilen]);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Timing Statistics */
			getstats(s2, ti.numiter, &si);
			printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
			       ti.msglen[ilen],
			       si.max * 1000, si.min * 1000, si.mean * 1000,
			       si.median * 1000, si.stddev * 1000);

			/* Bandwidth Statistics */
			for (i = 0; i < ti.numiter; i++) {
				bytes_t = (double) ti.msgcnt * ti.msglen[ilen] * (ti.nodes - 1);

				/*
				 * s2[] contains times before, and bandwidths
				 * after
				 */
				/* the assignment                                       */
				s2[i] = bytes_t / s2[i] / 1e6;
			}
			getstats(s2, ti.numiter, &si);
			printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
			     si.max, si.min, si.mean, si.median, si.stddev);
		}
	}
}


/****** alltoall

  DESCRIPTION:
        Send messages from all the nodes to all the nodes. Asynchronous
        receives are posted, to avoid deadlock situations which can occur 
	when the msglen is large and both nodes send at the same time.

*/

void
alltoall(n, passes, msgcnt, msglen)
	int             n;	/* Total number of nodes */
	int             passes;	/* Number of passes */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message */
{
	int             i, ipass, imsg, round;
	int             mid;	/* Recv msg ID's */
	int             mydst;
	int             mypartner;
	int             numwords;
	double          zero = 0.0;
	int             it_is_power_of_2;
	int             is_power_of_2();

	it_is_power_of_2 = is_power_of_2(n);

	gsync();

	/* Loop on passes */

	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * ti.msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);

		s1[ipass] = dclock();
		/* Loop on msgcnt */
		for (imsg = 0; imsg < msgcnt; imsg++) {
			/* Start the handoff */
			if (it_is_power_of_2) {
				for (round = 1; round < n; round++) {
					mypartner = node ^ round;
					mid = IRECV(ALLTOALL_MSG + round - 1 + n *
						imsg, bufr + imsg * msglen, msglen);
					CSEND(ALLTOALL_MSG + round - 1 + n * imsg,
					      bufs + imsg * msglen, msglen,
					      mypartner, NODE_PTYPE);
					msgwait(mid);
				}	/* round loop */
			} else {
				for (round = 0; round < n; round++) {
					mypartner = (n + round - node) % n;
					mid = IRECV(ALLTOALL_MSG + round - 1 +
						n * imsg, bufr + imsg * msglen, 
						msglen);

					/*
					 * send to yourself too, it won't
					 * affect timing
					 */
					CSEND(ALLTOALL_MSG + round - 1 + n * imsg,
					      bufs + imsg * msglen, msglen,
					      mypartner, NODE_PTYPE);
					msgwait(mid);
				}	/* round loop */
			}
		}		/* msgcnt loop */
		s2[ipass] = dclock() - s1[ipass];
		/* Check the correctness of the received message */
		if (compare(numwords, bufs, bufr)) {
			fprintf(stderr, "COMTEST - NODE %d : *** ALL-TO-ALL TEST"
				" ERROR ***\n", mynode());
			return;
		}
		/* make sure messages from different passes do not get mixed */
		gsync();
	}			/* ipass loop */
}

/****** alltoall_f

  DESCRIPTION:
        Send force type messages from all the nodes to all the nodes.

*/

void
alltoall_f(n, passes, msgcnt, msglen)
	int             n;	/* Total number of nodes */
	int             passes;	/* Number of passes */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message */
{
	int             i, ipass, imsg, round, tmp;
	int             mid;	/* Recv msg ID's */
	int             mysrc, mydst;
	int             mypartner;
	int             numwords;
	double          zero = 0.0;
	int             it_is_power_of_2;
	int             is_power_of_2();

	it_is_power_of_2 = is_power_of_2(n);

	gsync();

	/* Loop on passes */

	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);

		s1[ipass] = dclock();

		/* Loop on msgcnt */

		for (imsg = 0; imsg < msgcnt; imsg++) {
			/* Start the handoff */
			if (it_is_power_of_2) {
				for (round = 1; round < n; round++) {
					mypartner = node ^ round;
					mid = IRECV(ALLTOALL_F_MSG + round - 1 +
						n * imsg, bufr + imsg * msglen, 
						msglen);
					CSEND(CONF_MSG + round - 1 + n * imsg,
					    	&tmp, 0, mypartner, NODE_PTYPE);
					CRECV(CONF_MSG + round - 1 + n * imsg,
					      	&tmp, 0);
					CSEND(ALLTOALL_F_MSG + round - 1 + n * imsg,
					      	bufs + imsg * msglen, msglen,
					      	mypartner, NODE_PTYPE);
					msgwait(mid);
				}	/* round loop */
			} else {/* the number of nodes is not a power of two */
				for (round = 0; round < n; round++) {
					mypartner = (n + round - node) % n;
					mid = IRECV(ALLTOALL_F_MSG + round - 1 +
						n * imsg, bufr + imsg * msglen, 
						msglen);
					CSEND(CONF_MSG + round - 1 + n * imsg,
					    	&tmp, 0, mypartner, NODE_PTYPE);
					CRECV(CONF_MSG + round - 1 + n * imsg,
					      	&tmp, 0);
					CSEND(ALLTOALL_F_MSG + round - 1 + n * imsg,
					      	bufs + imsg * msglen, msglen,
					      	mypartner, NODE_PTYPE);
					msgwait(mid);
				}	/* round loop */
			}
		}		/* msgcnt loop */
		/* Check the correctness of the received message */
		if (compare(numwords, bufs, bufr)) {
			fprintf(stderr, "COMTEST - NODE %d : *** ALL-TO-ALL "
				"(FORCE) TEST ERROR ***\n", mynode());
			return;
		}
		s2[ipass] = dclock() - s1[ipass];
		/* make sure messages from different passes do not get mixed */
		gsync();
	}			/* ipass loop */
}


/****** alltoall_drv

  DESCRIPTION:
        This is the alltoall driver routine.  It calls the alltoall message
        passing timing test and prints results.

*/

alltoall_drv()
{
	double          start, delta;
	double          abw;
	double          temp;
	int             latpi;	/* Latency per iter, the time for msgcnt 0
				 * length messages. Time is in usec/msg. */
	double          bytes_t;
	int             icnt;
	int             jcnt;	/* DEBUG */
	int             ilen;
	int             i;

	/* Fill send buffer */
	start = 0;
	delta = 7.7777;
	dramp(BUFSIZE, start, delta, bufs);

	/* Header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   A L L  T O  A L L  T I M I N G   S U M M A R Y\n");
		printf("\n");
		printf("   Latency Description:\n");
		printf("      This is the time for a group of zero length messages\n");
		printf("      to be passed from all the nodes, to all the nodes.\n");
                printf("      As this is done in numnodes - 1 steps if numnodes \n");
                printf("      is a power of 2, and numnodes steps otherwise, the\n");
		printf("      time for one such step is also given : the latency\n");
                printf("      for one round is for a single message in the group\n");
                printf("      of messages of length 0. Note, times are\n");
		printf("      per iteration.\n");
		printf("\n");
		printf("   Bandwidth Description:\n");
		printf("      This is aggregate bandwidth.  All data transfers are\n");
		printf("      included and consist of nodes*(nodes-1)*msglen*msgcnt bytes.  \n");
		printf("      Note, times are per iteration.\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
	if (node == 0) {
		printf("        <--- Overall Latency (usecs) --->   <---- 1 Round Latency (usecs) ---->\n");
		printf("MSGCNT   Max    Min    Mean  Median Stddev    Max    Min    Mean  Median Stddev\n");
		printf("======  ====== ====== ====== ====== ======   ====== ====== ====== ====== ======\n");
	}
	/* get alltoall code in cache */
	if (ti.force)
		alltoall_f(ti.nodes, ti.numiter, 1, 0);
	else
		alltoall(ti.nodes, ti.numiter, 1, 0);

	/* Loop over message counts */
	for (jcnt = 1; jcnt <= 32 * MAXCNT; jcnt = jcnt * 2) {
		icnt = jcnt / 32;
		if (icnt == 0)
			icnt = 1;

		/* Latency measurement, zero length message */
		if (ti.force)
			alltoall_f(ti.nodes, ti.numiter, icnt, 0);
		else
			alltoall(ti.nodes, ti.numiter, icnt, 0);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		/* Take only 1 / (ti.nodes - 1) for one round */
		for (i = 0; i < ti.numiter; i++)
			s3[i] = s2[i] / ((ti.nodes - 1.) * icnt);

		if (node == 0) {
			/* Get statistics for the entire all-to-all exchange */
			getstats(s2, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf("%6d %7d%7d%7d%7d%7d", icnt,
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));

			/*
			 * Get statistics for one out of the n rounds of
			 * exchanges
			 */
			getstats(s3, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf(" %7d%7d%7d%7d%7d\n",
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));
		}
	}

	/* Bandwidth header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      **********************************************\n");
		printf("                      *   B A N D W I D T H   S T A T I S T I C S  *\n");
		printf("                      **********************************************\n");
		printf("\n");
		printf(" MSGLEN  <--------- T I M E  (ms) -------->  <-- B A N D W I D T H   (MB/S) -->\n");
		printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
		printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ===== ======\n");
	}
	if (ti.force)
		alltoall_f(ti.nodes, ti.numiter, 1, 0);	/* Get code in cache */
	else
		alltoall(ti.nodes, ti.numiter, 1, 0);	/* Get code in cache */

	/* Loop over different message lengths */
	for (ilen = 0; ilen < ti.numlens; ilen++) {
		if (ti.force)
			alltoall_f(ti.nodes, ti.numiter, ti.msgcnt,
				   ti.msglen[ilen]);
		else
			alltoall(ti.nodes, ti.numiter, ti.msgcnt,
				 ti.msglen[ilen]);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Timing Statistics */
			getstats(s2, ti.numiter, &si);
			printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
			       ti.msglen[ilen],
			       si.max * 1000, si.min * 1000, si.mean * 1000,
			       si.median * 1000, si.stddev * 1000);

			/* Bandwidth Statistics */
			for (i = 0; i < ti.numiter; i++) {
				bytes_t = (double) ti.msgcnt * ti.msglen[ilen] *
					ti.nodes * (ti.nodes - 1);

				/*
				 * s2[] contains times before, and bandwidths
				 * after
				 */
				/* the assignment   */
				s2[i] = bytes_t / s2[i] / 1e6;
			}
			getstats(s2, ti.numiter, &si);
			printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
			     si.max, si.min, si.mean, si.median, si.stddev);
		}
	}
}

/****** bisection

  DESCRIPTION: 
	In a rectangular mesh, exchange messages between: first, the left 
	half and the right one ; then, between the top half and the bottom 
	one. (partners are determined based on 'mirror' symmetry).

*/

void
bisection(n, passes, msgcnt, msglen, h, w, dir)
	int             n;	/* Total Number of nodes */
	int             passes;	/* Number of passes over and back */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message (bytes) */
	int             h;	/* rectangular mesh height */
	int             w;	/* rectangular mesh width */
	int             dir;	/* direction for communication: H or V */

{
	int             i, ipass, imsg;
	int             numwords;
	void            getstats();
	double          zero = 0.0;
	int             mypartner;
	int             x, y, x1, y1;

	/* Clean arrays for timing data		 */
	for (i = 0; i < MAXITER; i++) {
		s1[i] = 0;
		s2[i] = 0;
		work[i] = 0;
	}

	x = node % w;
	y = node / w;
	if (dir == H) {

		/*
		 * compute x1 for the symmetric with respect to the
		 * 'vertical' axis
		 */
		x1 = w - 1 - x;
		mypartner = x1 + y * w;
	} else {		/* if dir == V */

		/*
		 * compute y1 for the symmetric with respect to the
		 * 'horizontal' axis
		 */
		y1 = h - 1 - y;
		mypartner = x + y1 * w;
	}

	/* printf("Node %d Partner %d\n", node, mypartner); */

	gsync();

	if (node != mypartner) {/* do not send messages to yourself	 */
		/* Loop on passes */
		for (ipass = 0; ipass < passes; ipass++) {
			/* Clear the recv buffer */
			numwords = msglen / 8 * msgcnt;
			dcopy(numwords, &zero, 0, bufr, 1);

			/* no extra operations in the timed part */
			if (dir == H) {

				s1[ipass] = dclock();
				/* Loop on msgcnt */
				for (imsg = 0; imsg < msgcnt; imsg++) {
					/* Start the handoff */
					/* if you are on the 'left' side	 */
					if (x < (float) (w - 1) / 2.)
						CSEND(BISECTION_MSG + imsg,
						      bufs + imsg * msglen,
						      msglen, mypartner, NODE_PTYPE);

					/*
					 * Node 'mypartner' on the 'right'
					 * side	:
					 */
					/* Receive handoff, then pass back 	  */
					else if (x > (float) (w - 1) / 2.) {
						CRECV(BISECTION_MSG + imsg, bufr + 
							imsg * msglen, msglen);
						CSEND(BISECTION_MSG + imsg, bufr 
							+ imsg * msglen,
						      msglen, mypartner, NODE_PTYPE);
					}
				}
				/* Left Side Node: Wait to get handoff back */
				if (x < (float) (w - 1) / 2.) {
					for (imsg = 0; imsg < msgcnt; imsg++) {
						CRECV(BISECTION_MSG + imsg, bufr + 
							imsg * msglen, msglen);
					}
				}
				s2[ipass] = dclock() - s1[ipass];

			} else {/* if dir == V */

				s1[ipass] = dclock();
				/* Loop on msgcnt */
				for (imsg = 0; imsg < msgcnt; imsg++) {
					/* Start the handoff */
					/* if you are on the 'upper' side       */
					if (y < (float) (h - 1) / 2.)
						CSEND(BISECTION_MSG + imsg,
						      bufs + imsg * msglen,
						      msglen, mypartner, NODE_PTYPE);

					/*
					 * Node 'mypartner' on the 'right'
					 * side:
					 */
					/* Receive handoff, then pass back        */
					else if (y > (float) (h - 1) / 2.) {
						CRECV(BISECTION_MSG + imsg, bufr + 
							imsg * msglen, msglen);
						CSEND(BISECTION_MSG + imsg, bufr + 
							imsg * msglen, msglen,
						      	mypartner, NODE_PTYPE);
					}
				}
				/* Top Half Node: Wait to get handoff back */
				if (y < (float) (h - 1) / 2.) {
					for (imsg = 0; imsg < msgcnt; imsg++) {
						CRECV(BISECTION_MSG + imsg, bufr +
						      imsg * msglen, msglen);
					}
				}
				s2[ipass] = dclock() - s1[ipass];

			}

			/* Check the correctness of the received message */
			if (compare(numwords, bufs, bufr)) {
				if (dir == H && x < (float) (w - 1) / 2.) {
					fprintf(stderr, "COMTEST - NODE "
						" %d : *** BISECTION "
						"(HORIZONTAL COMM.) "
					      "TEST ERROR ***\n", mynode());
					return;
				}
			}
			if (compare(numwords, bufs, bufr)) {
				if (dir == V && y < (float) (h - 1) / 2.) {
					fprintf(stderr, "COMTEST - NODE %d "
						": *** BISECTION "
						"(VERTICAL COMM.) "
					      "TEST ERROR ***\n", mynode());
					return;
				}
			}
		}
	}
}


/****** bisection_f

  DESCRIPTION: 
	In a rectangular mesh, exchange force-type messages between: first, 
	the left half and the right one; then, between the top half and the 
	bottom one. (partners are determined based on 'mirror' symmetry).

*/


void
bisection_f(n, passes, msgcnt, msglen, h, w, dir)
	int             n;	/* Total Number of nodes */
	int             passes;	/* Number of passes over and back */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message (bytes) */
	int             h;	/* rectangular mesh height */
	int             w;	/* rectangular mesh width */
	int             dir;	/* direction for communication: H or V */

{
	int             i, ipass, imsg;
	int             numwords;
	void            getstats();
	int             rid[MAXCNT];	/* Recv msg ID's */
	int             tmp;
	double          zero = 0.0;
	int             mypartner;
	int             x, y, x1, y1;

	/* Clean arrays for timing data         */
	for (i = 0; i < MAXITER; i++) {
		s1[i] = 0;
		s2[i] = 0;
		work[i] = 0;
	}

	x = node % w;
	y = node / w;

	if (dir == H) {

		/*
		 * compute x1 for the symmetric with respect to the
		 * 'vertical' axis
		 */
		x1 = w - 1 - x;
		mypartner = x1 + y * w;
	} else {		/* if dir == V */

		/*
		 * compute y1 for the symmetric with respect to the
		 * 'horizontal' axis
		 */
		y1 = h - 1 - y;
		mypartner = x + y1 * w;
	}

	/* printf("Node %d Partner %d\n", node, mypartner); */

	gsync();

	if (node != mypartner) {
		/* Loop on passes */
		for (ipass = 0; ipass < passes; ipass++) {
			/* Clear the recv buffer */
			numwords = msglen / 8 * msgcnt;
			dcopy(numwords, &zero, 0, bufr, 1);

			/* no extra operations in the timed part */
			if (dir == H) {

				s1[ipass] = dclock();
				/* Loop on msgcnt and post recvs */
				for (imsg = 0; imsg < msgcnt; imsg++) {
					rid[imsg] = IRECV(BISECTION_F_MSG + imsg,
					      bufr + imsg * msglen, msglen);
				}

				/*
				 * Let my partner know he can force send to
				 * me
				 */
				CSEND(NUMITER_MSG + ipass, &tmp, 0, mypartner,
				      NODE_PTYPE);
				/* Wait for OK from my partner			 */
				CRECV(NUMITER_MSG + ipass, &tmp, 0);

				/* Loop on msgcnt and send messages		 */
				/* if you are on the 'left' side		 */
				if (x < (float) (w - 1) / 2.) {
					for (imsg = 0; imsg < msgcnt; imsg++) {
						CSEND(BISECTION_F_MSG + imsg,
						      bufs + imsg * msglen,
						      msglen, mypartner, NODE_PTYPE);
					}

				} else if (x > (float) (w - 1) / 2.) {
					for (imsg = 0; imsg < msgcnt; imsg++) {
						msgwait(rid[imsg]);
						CSEND(BISECTION_F_MSG + imsg,
						      bufr + imsg * msglen,
						      msglen, mypartner, NODE_PTYPE);
					}
				}
				/* Left Side Node: Wait to get handoff back */
				if (x < (float) (w - 1) / 2.) {
					for (imsg = 0; imsg < msgcnt; imsg++) {
						msgwait(rid[imsg]);
					}
				}
				s2[ipass] = dclock() - s1[ipass];

			} else {/* if dir == V */

				s1[ipass] = dclock();
				/* Loop on msgcnt and post recvs */
				for (imsg = 0; imsg < msgcnt; imsg++) {
					rid[imsg] = IRECV(BISECTION_F_MSG + imsg,
					      bufr + imsg * msglen, msglen);
				}

				/*
				 * Let my partner know he can force send to
				 * me
				 */
				CSEND(NUMITER_MSG + ipass, &tmp, 0, mypartner,
				      NODE_PTYPE);
				/* Wait for OK from my partner                  */
				CRECV(NUMITER_MSG + ipass, &tmp, 0);

				/* Loop on msgcnt and send messages             */
				/* if you are on the 'upper' side               */
				if (y < (float) (h - 1) / 2.) {
					for (imsg = 0; imsg < msgcnt; imsg++) {
						CSEND(BISECTION_F_MSG + imsg,
						      bufs + imsg * msglen,
						      msglen, mypartner, NODE_PTYPE);
					}

				} else if (y > (float) (h - 1) / 2.) {
					for (imsg = 0; imsg < msgcnt; imsg++) {
						msgwait(rid[imsg]);
						CSEND(BISECTION_F_MSG + imsg,
						      bufr + imsg * msglen,
						      msglen, mypartner, NODE_PTYPE);
					}
				}
				/* Top Side Node: Wait to get handoff back */
				if (y < (float) (h - 1) / 2.) {
					for (imsg = 0; imsg < msgcnt; imsg++) {
						msgwait(rid[imsg]);
					}
				}
				s2[ipass] = dclock() - s1[ipass];

			}

                        /* Check the correctness of the received message */
                        if (compare(numwords, bufs, bufr)) {
                                if (dir == H && x < (float) (w - 1) / 2.) {
					fprintf(stderr, "COMTEST - NODE %d :"
						" *** BISECTION (FORCE, "
						"HORIZONTAL COMM.) "
					      "TEST ERROR ***\n", mynode());
					return;
				}
			}
			if (compare(numwords, bufs, bufr)) {
                                if (dir == V && y < (float) (h - 1) / 2.) {
					fprintf(stderr, "COMTEST - NODE %d "
						": *** BISECTION (FORCE, "
						"VERTICAL COMM.) "
						"ERROR ***\n", mynode());
					return;
				}
			}
		}
	}
}


/****** bisection_drv

  DESCRIPTION:


*/

bisection_drv(dir)
	int             dir;
{
	int             imsg;
	double          start, delta;
	double          bytes_t;
	int             icnt;
	int             jcnt;	/* DEBUG */
	int             ilen;
	int             i;

	/* Fill send buffer */
	start = 0;
	delta = 7.7777;
	dramp(BUFSIZE, start, delta, bufs);

	/* Header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   B I S E C T I O N\n\n");
		if (dir == H)
			printf("      T I M I N G   S U M M A R Y  -  'H O R I Z O N T A L'\n");
		else
			printf("        T I M I N G   S U M M A R Y  -  'V E R T I C A L'\n");
		printf("\n");
		printf("   Latency Description:\n");
		printf("      The time for one message in a group of zero length messages\n");
		if (dir == H) {
			printf("      to be exchanged in a rectangular mesh of width w between\n");
			printf("      the 'left' half of the mesh, and the 'right' one\n");
		} else {	/* if dir == V */
			printf("      to be exchanged in a rectangular mesh of width w and height h\n");
			printf("      between the 'upper' half and the 'lower' one\n");
		}

		printf("\n");
		printf("   Bandwidth Description:\n");
		printf("      This is aggregate bandwidth.  All data transfers are\n");
		printf("      included and consist of 2*msglen*msgcnt bytes.      \n");
		printf("      Note, times are per iteration.\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      ***************************************\n");
		printf("                      *         L  A  T  E  N  C  Y         *\n");
		printf("                      ***************************************\n");
		printf("\n");
		printf("\n");
		printf(" <-- Bisection Latency (usecs) -->\n");
		printf("MSGCNT   Max    Min    Mean  Median Stddev\n");
		printf("======   =====  ===== ====== ====== ======\n");
	}
	if (ti.force)
		bisection_f(ti.nodes, ti.numiter, 1, 0, ti.height, ti.width, dir);	/* get bisection code in
											 * cache */
	else
		bisection(ti.nodes, ti.numiter, 1, 0, ti.height, ti.width, dir);	/* get bisection code in
											 * cache */

	/* Loop over message counts */
	for (jcnt = 1; jcnt <= 32 * MAXCNT; jcnt = jcnt * 2) {
		icnt = jcnt / 32;
		if (icnt == 0)
			icnt = 1;

		/* Latency measurement, zero length message */
		if (ti.force)
			bisection_f(ti.nodes, ti.numiter, icnt, 0, ti.height, 
				ti.width, dir);
		else
			bisection(ti.nodes, ti.numiter, icnt, 0, ti.height, 
				ti.width, dir);

		/* Take one one-way trip, for one message of length 0 */
		for (i = 0; i < ti.numiter; i++)
			s2[i] = s2[i] / (2. * icnt);

		/* node 0 gathers maximae from all the nodes 	 */
		/* in the array s2[] ; for H, the 'right' half  */
		/* contains zeroes ; for V, the 'lower' one	 */
		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Get statistics */
			getstats(s2, ti.numiter, &si);

			/* Print Statistics */
			if (jcnt > 16)	/* DEBUG */
				printf("%7d%7d%7d%7d%7d%7d\n", icnt,
				 (int) (si.max * 1e6), (int) (si.min * 1e6),
				       (int) (si.mean * 1e6), (int) (si.median * 1e6),
				       (int) (si.stddev * 1e6));
		}
	}

	if (ti.force)
		bisection_f(ti.nodes, ti.numiter, ti.msgcnt,
			    0, ti.height, ti.width, dir);	/* bring code into cache */
	else
		bisection(ti.nodes, ti.numiter, ti.msgcnt,
			  0, ti.height, ti.width, dir);	/* bring code into cache */

	for (ilen = 0; ilen < ti.numlens; ilen++) {

		if (ti.force)
			bisection_f(ti.nodes, ti.numiter, ti.msgcnt,
				 ti.msglen[ilen], ti.height, ti.width, dir);
		else
			bisection(ti.nodes, ti.numiter, ti.msgcnt,
				  ti.msglen[ilen], ti.height, ti.width, dir);

		/* save s2[] in each node	 */
		for (i = 0; i < MAXITER; i++)
			S3[ilen][i] = s2[i];

		gdhigh(s2, ti.numiter, work);	/* Use max. */

	}

	/* Bandwidth header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      ***************************************\n");
		printf("                      *       B  I  S  E  C  T  I  O  N     *\n");
		printf("                      ***************************************\n");
		printf("\n");
		printf(" MSGLEN  <--------- T I M E  (ms) -------->  <--- B A N D W I D T H (MB/S) --->\n");
		printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
		printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ====== ======\n");
	}
	for (ilen = 0; ilen < ti.numlens; ilen++) {
		gdhigh(S3[ilen], ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Timing Statistics */
			getstats(S3[ilen], ti.numiter, &si);
			printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
			       ti.msglen[ilen],
			       si.max * 1000, si.min * 1000, si.mean * 1000,
			       si.median * 1000, si.stddev * 1000);

			/* Bandwidth Statistics */
			for (i = 0; i < ti.numiter; i++) {
				if (dir == H) {
					bytes_t = (double) ti.msgcnt *
						ti.msglen[ilen] * 2 * 
						ti.height * (ti.width / 2);
				} else {	/* if dir == V */
					bytes_t = (double) ti.msgcnt *
						ti.msglen[ilen] * 2 *
						ti.width * (ti.height / 2);
				}

				/*
				 * S3[ilen][] contains times prior and
				 * bandwidths after
				 */
				/* this assignment                                */
				S3[ilen][i] = bytes_t / S3[ilen][i] / 1e6;
			}
			getstats(S3[ilen], ti.numiter, &si);
			printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
			     si.max, si.min, si.mean, si.median, si.stddev);
		}
	}
}




/****** corner

  DESCRIPTION: 
	Corner-to-corner test: in a rectangular mesh, exchange messages 
	between: first, the top-left node (node 0) and the top-right corner; 
	then, between the top-left node (node 0) and the bottom-left one.

*/

void
corner(passes, msgcnt, msglen, h, w, dir)
        int             passes; /* Number of passes over and back */
        int             msgcnt; /* Number of messages to send on each pass */
        int             msglen; /* Length of each message (bytes) */
        int             h;      /* rectangular mesh height */
        int             w;      /* rectangular mesh width */
        int             dir;    /* direction for communication: H or V */

{
        int             i, ipass, imsg;
        int             numwords;
        void            getstats();
        double          zero = 0.0;
        int             mypartner;

        /* Clean arrays for timing data          */
        for (i = 0; i < MAXITER; i++) {
                s1[i] = 0;
                s2[i] = 0;
                work[i] = 0;
        }

        if (dir == H) {
                mypartner =  w - 1;
        } else {                /* if dir == V */
                mypartner = (h - 1) * w;
        }

        /* printf("Node %d Partner %d\n", node, mypartner); */

        gsync();

        if ((node == 0 || node == mypartner) && mypartner != 0) {
		/* do not send messages to yourself      */
                /* Loop on passes */
                for (ipass = 0; ipass < passes; ipass++) {
                        /* Clear the recv buffer */
                        numwords = msglen / 8 * msgcnt;
                        dcopy(numwords, &zero, 0, bufr, 1);

                        /* no extra operations in the timed part */

                        s1[ipass] = dclock();
                        /* Loop on msgcnt */
                        for (imsg = 0; imsg < msgcnt; imsg++) {
                                /* Start the handoff */
					if(node == 0)
                                         CSEND(CORNER_MSG + imsg,
                                              bufs + imsg * msglen,
                                              msglen, mypartner, NODE_PTYPE);

                                /* Receive handoff, then pass back        */
                                else if (node == mypartner) {
                                        CRECV(CORNER_MSG + imsg, bufr +
                                                imsg * msglen, msglen);
                                        CSEND(CORNER_MSG + imsg, bufr
                                                + imsg * msglen,
                                              msglen, 0, NODE_PTYPE);
                                }
                        }
                        /* Node 0: Wait to get handoff back */
                        if (node == 0) {
                                for (imsg = 0; imsg < msgcnt; imsg++) {
                                        CRECV(CORNER_MSG + imsg, bufr +
                                                imsg * msglen, msglen);
                                }
                        }
                        s2[ipass] = dclock() - s1[ipass];

                        /* Check the correctness of the received message */
                        if (dir == H &&
                                (node == 0 || node == mypartner) && mypartner != 0) {
                                if (compare(numwords, bufs, bufr)) {
                                        fprintf(stderr, "COMTEST - NODE "
                                                " %d : *** CORNER-TO-CORNER "
                                                "(HORIZONTAL COMM.) "
                                              "TEST ERROR ***\n", mynode());
                                        return;
                                }
                        }
                        if (dir == V &&
                                (node == 0 || node == mypartner) && mypartner != 0) {
                                if (compare(numwords, bufs, bufr)) {
                                        fprintf(stderr, "COMTEST - NODE %d "
                                                ": *** CORNER-TO-CORNER "
                                                "(VERTICAL COMM.) "
                                              "TEST ERROR ***\n", mynode());
                                        return;
                                }
                        }
                }
        }
}


/****** corner_f

  DESCRIPTION: 
	Corner-to-corner test: in a rectangular mesh, exchange force-type 
	messages between: first, the top-left node (node 0) and the top-right 
	corner; then, between the top-left node (node 0) and the bottom-left 
	one.

*/


void
corner_f(passes, msgcnt, msglen, h, w, dir)
        int             passes; /* Number of passes over and back */
        int             msgcnt; /* Number of messages to send on each pass */
        int             msglen; /* Length of each message (bytes) */
        int             h;      /* rectangular mesh height */
        int             w;      /* rectangular mesh width */
        int             dir;    /* direction for communication: H or V */

{
        int             i, ipass, imsg;
        int             numwords;
        void            getstats();
        int             rid[MAXCNT];    /* Recv msg ID's */
        int             tmp;
        double          zero = 0.0;
        int             mypartner;

        /* Clean arrays for timing data         */
        for (i = 0; i < MAXITER; i++) {
                s1[i] = 0;
                s2[i] = 0;
                work[i] = 0;
        }

        if (dir == H) {
                mypartner =  w - 1;
        } else {                /* if dir == V */
                mypartner = (h - 1) * w;
        }

        /* printf("Node %d Partner %d\n", node, mypartner); */

        gsync();

	if ((node == 0 || node == mypartner) && mypartner != 0) {
                /* Loop on passes */
                for (ipass = 0; ipass < passes; ipass++) {
                        /* Clear the recv buffer */
                        numwords = msglen / 8 * msgcnt;
                        dcopy(numwords, &zero, 0, bufr, 1);

                        s1[ipass] = dclock();
                        /* Loop on msgcnt and post recvs */
                        for (imsg = 0; imsg < msgcnt; imsg++) {
                                rid[imsg] = IRECV(CORNER_F_MSG + imsg,
                                      bufr + imsg * msglen, msglen);
                        }

                        /*
                         * Let my partner know he can force send to
                         * me
                         */
			if (node == 0)
                        	CSEND(NUMITER_MSG + ipass, &tmp, 0, mypartner,
					NODE_PTYPE);
			else
				CSEND(NUMITER_MSG + ipass, &tmp, 0, 0, NODE_PTYPE);
	
                        /* Wait for OK from the partner                  */
                        CRECV(NUMITER_MSG + ipass, &tmp, 0);

                        /* Loop on msgcnt and send messages              */
                        /* if you are node 0                 */
                        if (node == 0) {
                                for (imsg = 0; imsg < msgcnt; imsg++) {
                                        CSEND(CORNER_F_MSG + imsg,
                                              bufs + imsg * msglen,
                                              msglen, mypartner, NODE_PTYPE);
                                }

                        } else if (node == mypartner) {
                                for (imsg = 0; imsg < msgcnt; imsg++) {
                                        msgwait(rid[imsg]);
                                        CSEND(CORNER_F_MSG + imsg,
                                              bufr + imsg * msglen,
                                              msglen, 0, NODE_PTYPE);
                                }
                        }
                        /* Node 0: Wait to get handoff back */
                        if (node == 0) {
                                for (imsg = 0; imsg < msgcnt; imsg++) {
                                        msgwait(rid[imsg]);
                                }
                        }
                        s2[ipass] = dclock() - s1[ipass];

                        /* Check the correctness of the received message */
                        if (dir == H &&
                                (node == 0 || node == mypartner) && mypartner != 0) {
                                if (compare(numwords, bufs, bufr)) {
                                        fprintf(stderr, "COMTEST - NODE "
                                                " %d : *** CORNER-TO-CORNER "
                                                "(HORIZONTAL COMM.) "
                                              "TEST ERROR ***\n", mynode());
                                        return;
                                }
                        }
                        if (dir == V &&
                                (node == 0 || node == mypartner) && mypartner != 0) {
                                if (compare(numwords, bufs, bufr)) {
                                        fprintf(stderr, "COMTEST - NODE %d "
                                                ": *** CORNER-TO-CORNER "
                                                "(VERTICAL COMM.) "
                                              "TEST ERROR ***\n", mynode());
                                        return;
                                }
                        }
                }
        }
}


/****** corner_drv

  DESCRIPTION:


*/

corner_drv(dir)
        int             dir;
{
        int             imsg;
        double          start, delta;
        double          bytes_t;
        int             icnt;
        int             jcnt;   /* DEBUG */
        int             ilen;
        int             i;

        /* Fill send buffer */
        start = 0;
        delta = 7.7777;
        dramp(BUFSIZE, start, delta, bufs);

        /* Header */
        if (node == 0) {
                printf("\n");
                printf("\n");
                printf("\n");
                printf("-----------------------------------------------------------\n");
                printf("\n");
                printf("   C O R N E R   T O   C O R N E R\n\n");
                if (dir == H)
                        printf("      T I M I N G   S U M M A R Y  -  'H O R I Z O N T A L'\n");
                else
                        printf("        T I M I N G   S U M M A R Y  -  'V E R T I C A L'\n");
                printf("\n");
                printf("   Latency Description:\n");
                printf("      The time for one message in a group of zero length messages\n");
                if (dir == H) {
                        printf("      to be exchanged between node 0 and node w - 1 in a rectangular\n");
                        printf("      mesh of width w\n");
                } else {        /* if dir == V */
                        printf("      to be exchanged between node 0 and node (h - 1) * w in a rectangular\n");
                        printf("      mesh of width w and height h\n");
                }

                printf("\n");
                printf("   Bandwidth Description:\n");
                printf("      This is aggregate bandwidth.  All data transfers are\n");
                printf("      included and consist of 2*msglen*msgcnt bytes.      \n");
                printf("      Note, times are per iteration.\n");
                printf("\n");
                printf("-----------------------------------------------------------\n");
                printf("\n");
                printf("\n");
        }
        if (node == 0) {
                printf("\n");
                printf("\n");
                printf("                      ***************************************\n");
                printf("                      *         L  A  T  E  N  C  Y         *\n");
                printf("                      ***************************************\n");
                printf("\n");
                printf("\n");
                printf("<-- Corner to Corner Latency (usecs) ->\n");
                printf("MSGCNT   Max    Min    Mean  Median Stddev\n");
                printf("======  ====== ====== ====== ====== ======\n");
        }
        if (ti.force)
                corner_f(ti.numiter, 1, 0, ti.height, ti.width, dir);      
			/* get corner-to-corner code in cache */
        else
                corner(ti.numiter, 1, 0, ti.height, ti.width, dir);        
			/* get corner-to-corner code in cache */

        /* Loop over message counts */
        for (jcnt = 1; jcnt <= 32 * MAXCNT; jcnt = jcnt * 2) {
                icnt = jcnt / 32;
                if (icnt == 0)
                        icnt = 1;

                /* Latency measurement, zero length message */
                if (ti.force)
                        corner_f(ti.numiter, icnt, 0, ti.height,
                                ti.width, dir);
                else
                        corner(ti.numiter, icnt, 0, ti.height,
                                ti.width, dir);

                /* Take one one-way trip, for one message of length 0 */
                for (i = 0; i < ti.numiter; i++)
                        s2[i] = s2[i] / (2. * icnt);

                if (node == 0) {

                        /* Get statistics */
                        getstats(s2, ti.numiter, &si);

                        /* Print Statistics */
                        if (jcnt > 16)  /* DEBUG */
                                printf("%6d %7d%7d%7d%7d%7d\n", icnt,
                                 (int) (si.max * 1e6), (int) (si.min * 1e6),
                                       (int) (si.mean * 1e6), (int) (si.median * 1e6),
                                       (int) (si.stddev * 1e6));
                }
        }

        /* Bandwidth header */
        if (node == 0) {
                printf("\n");
                printf("\n");
                printf("                      ***************************************\n");
                printf("                      *    C O R N E R  T O  C O R N E R    *\n");
                printf("                      ***************************************\n");
                printf("\n");
                printf(" MSGLEN  <--------- T I M E  (ms) -------->  <--- B A N D W I D T H (MB/S) --->\n");
                printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
                printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ====== ======\n");
        }
        if (ti.force)
                corner_f(ti.numiter, ti.msgcnt,
                            0, ti.height, ti.width, dir);       /* bring code into cache */
        else
                corner(ti.numiter, ti.msgcnt,
                          0, ti.height, ti.width, dir); /* bring code into cache */

        for (ilen = 0; ilen < ti.numlens; ilen++) {

                if (ti.force)
                        corner_f(ti.numiter, ti.msgcnt,
                                 ti.msglen[ilen], ti.height, ti.width, dir);
                else
                        corner(ti.numiter, ti.msgcnt,
                                  ti.msglen[ilen], ti.height, ti.width, dir);

                /* save s2[] in each node        */
                for (i = 0; i < MAXITER; i++)
                        S3[ilen][i] = s2[i];

                gdhigh(s2, ti.numiter, work);   /* Use max. */

                if (node == 0) {
                        /* Timing Statistics */
                        getstats(s2, ti.numiter, &si);
                        printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
                               ti.msglen[ilen],
                               si.max * 1000, si.min * 1000, si.mean * 1000,
                               si.median * 1000, si.stddev * 1000);

                        /* Bandwidth Statistics */
                        for (i = 0; i < ti.numiter; i++) {
                                bytes_t = (double) ti.msgcnt * ti.msglen[ilen] * 2;

                                /*
                                 * s2[] contains times prior and bandwidths
                                 * after
                                 */
                                /* this assignment                                */
                                s2[i] = bytes_t / s2[i] / 1e6;
                        }
                        getstats(s2, ti.numiter, &si);
                        printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
                             si.max, si.min, si.mean, si.median, si.stddev);
                }
        }

}





/****** order

  DESCRIPTION:
        Send messages of lengths 256 kbytes, 0 bytes, 8 bytes (in this order), 
	from several nodes, to one receiving node. All the nodes are, in turn, 
	the receiving node. The receiving node checks that the messages 
	originating from the same source, arrive in their departure order.

	NOTE: two supplementary order tests, currently de-activated, are 
	present in this section of code (following the test described above).

*/

int             arrival1[MAXNODES][MAXCNT * ORDER_FACT1];
int             arrival2[MAXNODES][MAXCNT * MAXLENS];
int             counter[MAXNODES];

void
order(n, passes, msgcnt, numlens)
	int             n;	/* Total number of nodes */
	int             passes;	/* Number of passes */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             numlens;/* number of different message lengths */
{
	int             passes1;
	int             i, ipass, imsg, round;
	int             order_cnt;
	int             from;
	int             ilen;
	int             icnt;
	long            length_loop_error;
	long            pass_loop_error;
	long            round_loop_error;
	long            temp;
	int             k, k1;	/* number of sending nodes, for n > 32 */

	/* Header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   M E S S A G E  A R R I V A L  O R D E R  T E S T\n");
		printf("\n");
		printf("   Description:\n\n");
		printf("      Every node in turn receives messages of lengths 256 \n"
);
                printf("      kbytes, 0 bytes, and 8 bytes (in this order) , from \n"
);
                printf("      a number of other nodes. The receiving node checks \n"
);
                printf("      that the arrival order is identical with the        \n"
);
                printf("      departure order for each of the source nodes.      \n");

/*

		THIS COMMENT IS FOR THE TWO TEST THAT ARE SKIPPED BY MEANS OF
		THE RETURN STATEMENT

		printf("      Part one : for all the message lengths (the number  \n");
		printf("      of message lengths is ti.numlens) and for all the   \n");
		printf("      passes (ti.numiter times), each node in turn        \n");
		printf("      receives from all the other nodes a number of       \n");
		printf("      messages equal to ti.msgcnt * ORDER_FACT1. All the \n");
		printf("      messages are of the same length for ti.numiter	  \n");
		printf("      messages are of the same length for ti.numiter      \n");
		printf("      passes. After receiving ti.msgcnt * ORDER_FACT1	  \n");
		printf("      messages from all the other nodes, the receiving	  \n");
		printf("      node checks the correctness of their arrival, for   \n");
		printf("      each source node in turn.			\n\n");
		printf("      Part two : for all the passes (ti.numiter times),   \n");
		printf("      each node in turn receives from all the other nodes \n");
		printf("      ti.msgcnt * ORDER_FACT2 times, ti.numlens messages  \n");
		printf("      of different lengths, specified in ti.msglen[].    \n");
		printf("      The ti.numlens messages are sent alternatively in   \n");
		printf("      the decreasing and then in the increasing order of  \n");
		printf("      their lengths.       				  \n");
*/

		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
	if (node == 0)
		printf("\n\nCHECKING ARRIVAL ORDER FOR MESSAGES OF DIFFERENT "
		       "LENGTHS\n\n\n");

	/*
	 * if(n < 32) { k = n - 1; } else { k = n / (n / 16) / (n / 16); if
	 * (k < 4) k = 4; }
	 */

	/* always run with at least '-mbf 6000000 -mex 5000000' */
	if (n < 6)
		k = n - 1;
	else
		k = 5;

	/* DEBUG */
	/* fprintf(stderr, "DEBUG: Number of sending nodes = %d\n", k); */

	/* Loop on passes */
	/* for (ipass = 0; ipass < passes ; ipass++) { */
	/* Only one pass, to reduce execution time 	 */
	for (ipass = 0; ipass < 1; ipass++) {
		gsync();
		for (round = 0; round < n; round++) {
			if (node == 0)
				printf("Checking arrival order for messages received by "
				       "node %d\n", round);
			round_loop_error = 0;
			if (round < k)
				k1 = k + 1;
			else
				k1 = k;
			/* clear the arrays */
			for (i = 0; i < n; i++) {

				/*
				 * send only ti.order_numlens messages from
				 * each node,
				 */

				/*
				 * each pass and each round (except for the
				 * receiving
				 */
				/* node							 */
				for (order_cnt = 0; order_cnt < ti.order_numlens;
				     order_cnt++) {
					arrival2[i][order_cnt] = -1;
				}
				counter[i] = 0;
			}

			/*
			 * all nodes with node < k (possibly also k, if round
			 * < k) ,
			 */
			/* except 'round', send							 */

			/*
			 * ti.order_numlens messages to node 'round', which
			 * receives them and
			 */
			/* checks the correctness of the arrival order				 */
			if (node < k1 && node != round) {
				/* Loop on msgcnt */

				/*
				 * for (icnt = 0 ; icnt < ORDER_FACT2 *
				 * msgcnt ; icnt++) {
				 */
				for (icnt = 0; icnt < 1; icnt++) {
					for (ilen = 0; ilen < ti.order_numlens; 
						ilen++) {

						/*
						 * INJECTED ERROR if(round ==
						 * 3) CSEND(ORDER_MSG + icnt *
						 * ti.order_numlens + ilen -
						 * 1, bufs,
						 * ti.order_msglen[ilen],
						 * round, NODE_PTYPE); else
						 */
						CSEND(ORDER_MSG + icnt * 
							ti.order_numlens +
						      	ilen, bufs,
						ti.order_msglen[ilen], round,
						      	NODE_PTYPE);
					}
				}
			} else if (node == round) {
				/* node 'round' receives */

				/*
				 * for (icnt = 0 ; icnt < ORDER_FACT2 *
				 * msgcnt ; icnt++) {
				 */
				for (icnt = 0; icnt < 1; icnt++) {
					for (ilen = 0; ilen < ti.order_numlens; 
						ilen++) {

						/*
						 * receive from all the other
						 * nodes
						 */
						for (i = 0; i < k; i++) {
							CRECV(-1, bufr, MAXMESSGLEN);
							from = (int) (infonode());
							arrival2[from][counter[from]++] =
								(int) (infotype());
						}
					}
				}

				/*
				 * check the order correctness of the
				 * received messages
				 */
				for (i = 0; i < k1; i++) {
					if (i != round) {	/* skip myself */
						for (order_cnt = 0; order_cnt <

						/*
						 * ORDER_FACT2 * msgcnt *
						 * ti.order_numlens ;
						 */
						     ti.order_numlens;
						     order_cnt++) {
							if ((arrival2[i][order_cnt] -
								ORDER_MSG)
							    	!= order_cnt) {
								fprintf(stderr,
								"COMTEST - NODE %d :"
								"  MESSAGE ORDER "
								"ERROR - message %d "
								"from node %d has "
								"arrived"
								" on position %d\n",
								mynode(),
								arrival2[i][order_cnt]
								- ORDER_MSG, i, 
								order_cnt);
								round_loop_error = 1;
							}
						}
					}
				}
			}
			gihigh(&round_loop_error, 1, &temp);
			if (node == 0 && !round_loop_error)
				printf("No arrival order error for messages"
				       " received by node %d\n", round);

		}
	}


	return;

	/* goto label; */

	/* YET ANOTHER ORDER TEST - CURRENTLY DEACTIVATED */

	if (node == 0)
		printf("\nPART I : CHECKING ARRIVAL ORDER FOR MESSAGES OF THE SAME "
		       "LENGTH\n\n\n");

	/*
	 * reduce the number of passes by the factor used to increase the
	 * message
	 */
	/* count							 */
	passes1 = passes / ORDER_FACT1;
	if (passes1 == 0)
		passes1 = 1;

	/* Loop over different message lengths */
	for (ilen = 0; ilen < numlens; ilen++) {
		length_loop_error = 0;
		if (node == 0)
			printf("Checking arrival order for messages of length %d bytes "
			       "...\n", ti.msglen[ilen]);
		/* Loop on passes */
		for (ipass = 0; ipass < passes1; ipass++) {
			for (round = 0; round < n; round++) {
				/* clear the arrays */
				for (i = 0; i < n; i++) {
					for (order_cnt = 0 ; 
						order_cnt < ORDER_FACT1 * msgcnt;
					     	order_cnt++) {
						arrival1[i][order_cnt] = -1;
					}
					counter[i] = 0;
				}
				gsync();

				/*
				 * all nodes, except 'round', send
				 * ORDER_FACT1 * msgcnt
				 */

				/*
				 * messages to node 'round', which receives
				 * them and
				 */

				/*
				 * checks the correctness of the arrival
				 * order
				 */
				if (node != round) {
					/* Loop on msgcnt */
					for (order_cnt = 0 ; 
						order_cnt < ORDER_FACT1 *
					     	msgcnt; order_cnt++) {

						/*
						 * INJECTED ERROR if(ipass ==
						 * 1 && round == 3 &&
						 * order_cnt == 4)
						 * CSEND(ORDER_MSG +
						 * order_cnt - 1, bufs,
						 * ti.msglen[ilen], round,
						 * NODE_PTYPE); else
						 */
						CSEND(ORDER_MSG + order_cnt, bufs, 
							ti.msglen[ilen],
						      	round, NODE_PTYPE);
					}
				} else {	/* node 'round' receives */
					for (order_cnt = 0; 
						order_cnt < ORDER_FACT1 *
					     	msgcnt; order_cnt++) {

						/*
						 * receive from all the other
						 * nodes
						 */
						for (i = 0; i < n - 1; i++) {
							CRECV(-1, bufr, 
								ti.msglen[ilen]);
							from = (int) (infonode());
							arrival1[from][counter[from]++] =
								(int) (infotype());
						}
					}

					/*
					 * check the order correctness of the
					 * received messages
					 */
					for (i = 0; i < n; i++) {
						if (node != i) {	/* skip myself */
							for (order_cnt = 0 ; 
							  order_cnt <
							  ORDER_FACT1 * msgcnt;
						     	  order_cnt++) {
							  if ((arrival1[i][order_cnt] -
						          ORDER_MSG) != order_cnt) {
							  fprintf(stderr, "COMTEST "
							  "- NODE %d : MESSAGE"
							  " ORDER ERROR - "
							  "message %d from node "
							  "%d has arrived on "
							  "position %d\n",
							  mynode(),
							  arrival1[i][order_cnt]
							  - ORDER_MSG, i,
							  order_cnt);
							  length_loop_error = 1;
								}
							}
						}
					}
				}
			}
		}
		gihigh(&length_loop_error, 1, &temp);
		if (node == 0 && !length_loop_error)
			printf("No arrival order error for messages of "
				"length %d bytes\n",
			       	ti.msglen[ilen]);
	}

label:

	/* YET ANOTHER ORDER TEST - CURRENTLY DEACTIVATED */

	if (node == 0)
		printf("\n\nPART II : CHECKING ARRIVAL ORDER FOR MESSAGES "
			"OF DIFFERENT "
		       "LENGTHS\n\n\n");

	/* Loop on passes */
	for (ipass = 0; ipass < passes; ipass++) {
		pass_loop_error = 0;
		if (node == 0)
			printf("Checking arrival order for messages of different "
			       "lengths in pass %d ...\n", ipass);

		gsync();
		for (round = 0; round < n; round++) {
			/* clear the arrays */
			for (i = 0; i < n; i++) {
				for (order_cnt = 0; order_cnt < 
					ORDER_FACT2 * msgcnt * numlens; order_cnt++){
					arrival2[i][order_cnt] = -1;
				}
				counter[i] = 0;
			}

			/*
			 * all nodes, except 'round', send ORDER_FACT2 *
			 * msgcnt * numlens
			 */
			/* messages to node	*/

			/*
			 * 'round', which receives them and checks the
			 * correctness of the
			 */
			/* arrival order	*/
			if (node != round) {

				/*
				 * COULD REVERSE THE NEXT TWO LOOPS ...IT
				 * WOULD BE LIKE 1ST PART
				 */
				/* Loop on msgcnt */
				for (icnt = 0; icnt < ORDER_FACT2 * msgcnt; icnt++) {
					for (ilen = 0; ilen < numlens; ilen++) {

						/*
						 * INJECTED ERROR if(ipass ==
						 * 2 && round == 3 && ilen ==
						 * 4) CSEND(ORDER_MSG + icnt *
						 * numlens + ilen - 1, bufs,
						 * ti.msglen[numlens - 1 -
						 * ilen], round, NODE_PTYPE);
						 * else
						 */
						if (order_cnt % 2 == 0)
							CSEND(ORDER_MSG + icnt * 
								numlens +
							      	ilen, bufs,
							      	ti.msglen[numlens - 
								1 - ilen], round, 
								NODE_PTYPE);
						else
							CSEND(ORDER_MSG + icnt * 
								numlens +
							      	ilen, bufs,
							      	ti.msglen[ilen], 
								round,
							      	NODE_PTYPE);
					}
				}
			} else {
				/* node 'round' receives */
				for (icnt = 0; icnt < ORDER_FACT2 * msgcnt; icnt++) {
					for (ilen = 0; ilen < numlens; ilen++) {

						/*
						 * receive from all the other
						 * nodes
						 */
						for (i = 0; i < n - 1; i++) {
							CRECV(-1, bufr, MAXMESSGLEN);
							from = (int) (infonode());
							arrival2[from][counter[from]++] =
								(int) (infotype());
						}
					}
				}

				/*
				 * check the order correctness of the
				 * received messages
				 */
				for (i = 0; i < n; i++) {
					if (node == round && node != i) {
						/* skip myself */
						for (order_cnt = 0;
						    order_cnt < ORDER_FACT2 * 
						    msgcnt * numlens;
						    order_cnt++) {
						    if ((arrival2[i][order_cnt] - 
						    	ORDER_MSG) != order_cnt) {
							  fprintf(stderr,
							  "COMTEST - NODE %d : "
							  "MESSAGE ORDER "
							  "ERROR - message %d "
							  "from node %d has arrived"
							  " on position %d\n", 
							  mynode(),
							  arrival2[i][order_cnt]
							  - ORDER_MSG, i, order_cnt);
							pass_loop_error = 1;
							}
						}
					}
				}
			}
		}
		gihigh(&pass_loop_error, 1, &temp);
		if (node == 0 && !pass_loop_error)
			printf("No arrival order error for messages in pass %d\n", ipass);
	}
}


void
order_f(n, passes, msgcnt, numlens)
	int             n;	/* Total number of nodes */
	int             passes;	/* Number of passes */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             numlens;/* number of different message lengths */
{
	/* Header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   T H E  'M E S S A G E  A R R I V A L  O R D E R'  T E S T\n");
		printf("    I S  N O T  A V A I L B L E  F O R  F O R C E  T Y P E\n");
		printf("                   M E S S A G E S\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
}


order_drv()
{
	if (ti.force)
		order_f(ti.nodes, ti.numiter, ti.msgcnt, ti.numlens);
	else
		order(ti.nodes, ti.numiter, ti.msgcnt, ti.numlens);
}

/****** random1

  DESCRIPTION:
        Send n messages from n random sources to n random destinations.
        Asynchronous receives are posted, to avoid deadlock situations which 
	can occur when the msglen is large and both nodes send at the same 
	time.

*/

void
random1(n, passes, msgcnt, msglen)
	int             n;	/* Total number of nodes */
	int             passes;	/* Number of passes */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message */
{
	int             i, ipass, imsg;
	int             rid[MAXNODES];	/* Recv msg ID's */
	int             src[MAXNODES];
	int             dst[MAXNODES];
	int             intsize;
	int             rand_pairs;
	int		rand_ctr = 0;
	int             x, y;
	int             mid;
	int             numwords;
	void            getstats();
	double          zero = 0.0;


	intsize = sizeof(int);

	/* milliseconds change from call to call     */
	seed = (unsigned int) (1000 * dclock());

	/* node 0 establishes the n random message sources and destinations */
	/* that exchange in all 'n' messages of length 'msglen', 'msgcnt'   */
	/* times, everything repeated 'passes' times                        */
	if (node == 0) {
	        srand(seed);
		for (i = 0; i < n; i++) {
			src[i] = rand() % n;
			dst[i] = rand() % n;
			/* make sure the dst is different from the src */
			rand_ctr = 0;
			while (dst[i] == src[i]) {
				dst[i] = rand() % n;
				if (++rand_ctr > 1000) {
					dst[i] = dst[i] - 1;
					break;
				}
			}
		}

		/* send the two tables to all the other nodes   */
		CSEND(SRC_TBL_MSG, src, intsize * n, -1, NODE_PTYPE);
		CSEND(DST_TBL_MSG, dst, intsize * n, -1, NODE_PTYPE);

#ifdef DBG
		printf("src : %d %d %d %d\n", src[0], src[1], src[2], src[3]);
		printf("dst : %d %d %d %d\n", dst[0], dst[1], dst[2], dst[3]);
#endif

	} else {		/* nodes other than 0  */
		CRECV(SRC_TBL_MSG, src, intsize * n);
		CRECV(DST_TBL_MSG, dst, intsize * n);
	}

	/* Clear s1[] and s2[]		 */
	for (i = 0; i < MAXITER; i++) {
		s1[i] = 0;
		s2[i] = 0;
	}

	/* Loop on passes */

	gsync();
	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);
		s1[ipass] = dclock();
		/* Loop on msgcnt */
		for (imsg = 0; imsg < msgcnt; imsg++) {

			/*
			 * Start the handoff, by sending all the messages (if
			 * any)
			 */

			/*
			 * Avoid possible deadlock for large messages, with
			 * IRECV
			 */
			for (i = 0; i < n; i++) {
				if (node == dst[i])
					rid[i] = IRECV(RAND_MSG + i + n * imsg,
					      bufr + imsg * msglen, msglen);
				if (node == src[i])
					CSEND(RAND_MSG + i + n * imsg,
					      bufs + imsg * msglen,
					      msglen, dst[i], NODE_PTYPE);
			}
			for (i = 0; i < n; i++) {
				if (node == dst[i])
					msgwait(rid[i]);
			}
		}		/* msgcnt loop */
		s2[ipass] = dclock() - s1[ipass];
		/* make sure messages from different passes do not get mixed */
		gsync();
		/* Check the correctness of the received message */
		for (i = 0; i < n; i++) {
			if (node == dst[i]) {
				if (compare(numwords, bufs, bufr)) {
					fprintf(stderr, "COMTEST - NODE %d : *** "
						"RANDOM 1 "
					      	"TEST ERROR ***\n", mynode());
					return;
				}
			}
		}
	}			/* ipass loop */
}

/****** random2

  DESCRIPTION:
        Send n messages from n random sources to n random destinations.
        Asynchronous receives are posted, to avoid deadlock situations which 
	can occur when the msglen is large and both nodes send at the same 
	time. The sources are restricted to the lower half of the partition, 
	and the destinations, to its upper half.

*/

void
random2(n, passes, msgcnt, msglen)
	int             n;	/* Total number of nodes */
	int             passes;	/* Number of passes */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message */
{
	int             i, ipass, imsg;
	int             rid[MAXNODES];	/* Recv msg ID's */
	int             src[MAXNODES];
	int             dst[MAXNODES];
	int             intsize;
	int             rand_pairs;
	int             x, y;
	int             mid;
	int             numwords;
	void            getstats();
	double          zero = 0.0;


	intsize = sizeof(int);

	/* milliseconds change from call to call     */
	seed = (unsigned int) (1000 * dclock());

	/*
	 * node 0 establishes the n random message sources in the lower half
	 * of the
	 */

	/*
	 * partition, and the n random message destinations in the upper half
	 * of
	 */
	/* the partition                                                            */
	if (node == 0) {
	        srand(seed);
		for (i = 0; i < n; i++) {
			src[i] = rand() % (n / 2);
			dst[i] = n / 2 + (rand() %(n / 2));
		}

		/* send the two tables to all the other nodes   */
		CSEND(SRC_TBL_MSG, src, intsize * n, -1, NODE_PTYPE);
		CSEND(DST_TBL_MSG, dst, intsize * n, -1, NODE_PTYPE);

#ifdef DBG
		printf("src : %d %d %d %d\n", src[0], src[1], src[2], src[3]);
		printf("dst : %d %d %d %d\n", dst[0], dst[1], dst[2], dst[3]);
#endif

	} else {		/* nodes other than 0  */
		CRECV(SRC_TBL_MSG, src, intsize * n);
		CRECV(DST_TBL_MSG, dst, intsize * n);
	}

	/* Clear s1[] and s2[]          */
	for (i = 0; i < MAXITER; i++) {
		s1[i] = 0;
		s2[i] = 0;
	}

	/* Loop on passes */

	gsync();
	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);
		s1[ipass] = dclock();
		/* Loop on msgcnt */
		for (imsg = 0; imsg < msgcnt; imsg++) {

			/*
			 * Start the handoff, by sending all the messages (if
			 * any)
			 */
			for (i = 0; i < n; i++) {
				if (node == src[i])
					CSEND(RAND_MSG + i + n * imsg,
					      bufs + imsg * msglen,
					      msglen, dst[i], NODE_PTYPE);
				if (node == dst[i])
					CRECV(RAND_MSG + i + n * imsg,
					      bufr + imsg * msglen, msglen);
			}
		}		/* msgcnt loop */
		s2[ipass] = dclock() - s1[ipass];
		/* make sure messages from different passes do not get mixed */
		gsync();
		/* Check the correctness of the received message */
		for (i = 0; i < n; i++) {
			if (node == dst[i]) {
				if (compare(numwords, bufs, bufr)) {
					fprintf(stderr, "COMTEST - NODE %d : *** RANDOM 2 "
					      "TEST ERROR ***\n", mynode());
					return;
				}
			}
		}
	}			/* ipass loop */
}


/****** random3

  DESCRIPTION:
        Exchange a random number of messages of random lengths, between a 
	random number of node pairs, chosen randomly, everything repeated a 
	random number of times (iterations).

*/


void
random3(n, ipass, numlens)
	int             n;	/* Total number of nodes 	 */
	int             ipass;	/* Number of passes      	 */
	int             numlens;/* Number of message lengths	 */
{
	int             i, j, imsg;
	int             rid[MAXNODES];	/* Recv msg ID's */
	int             src[MAXNODES];
	int             dst[MAXNODES];
	int             intsize;
	int             rand_pairs;
	int             x, y;
	int             mid;
	int             numwords;
	void            getstats();
	double          zero = 0.0;
	int             rand_msglen;

	intsize = sizeof(int);

	/* milliseconds change from call to call     */
	seed = (unsigned int) (1000 * dclock());

	/*
	 * Load the communication system with a random number of messages of
	 * random
	 */
	/* lengths, exchanged between a random number of node pairs, chosen    */

	/*
	 * randomly, everything repeated a random number of times
	 * (iterations)
	 */
	/* Node 0 determines the parameters of the communication	    */
	if (node == 0) {
		/* prepare the 'more random' test               */
		/* initialize the array pair[] with -1          */
		for (i = 0; i < MAXNODES; i++)
			pair[i] = -1;

		/* determine the random number of pairs         */
	        srand(seed);
		rand_pairs = rand() % (n / 2);

#ifdef DEBUG
		printf("DEBUG : rand_pairs = %d\n", rand_pairs);
#endif

		/* determine the pairs                          */
		for (i = 0; i < rand_pairs; i++) {
			/* find an unpaired PE x        */
			x = rand() % n;
			while (pair[x] != -1) {
				x++;
				x = x % n;
			}
			/* find an unpaired PE y        */
			y = rand() % n;
			while (pair[y] != -1) {
				y++;
				y = y % n;
			}
			/* pair them                    */
			pair[x] = y;
			pair[y] = x;
			/* set a message length for this pair   */
			rand_msglen_ind[x] = rand() % numlens;
			rand_msglen_ind[y] = rand_msglen_ind[x];
			/* set a msgcnt for this pair           */
			/* this is a bit off the pattern, but as the default */
			/* is msgcnt = 1 , this would mean to always have    */
			/* (in the default case) rand_msgcnt[x] = 1	     */
			rand_msgcnt[x] = 1 + (rand() % (MAXCNT - 1));	
				/* at least 1 */
			rand_msgcnt[y] = rand_msgcnt[x];
		}

#ifdef DEBUG
		for (i = 0; i < n; i++) {
			printf("DEBUG : p %d l %d c %d\n", pair[i],
			       rand_msglen_ind[i], rand_msgcnt[i]);
		}
#endif

		/* transmit all these to the other nodes        */
		CSEND(TBL_MSG, &rand_pairs, intsize, -1, NODE_PTYPE);
		CSEND(TBL_MSG + 1, pair, intsize * n, -1, NODE_PTYPE);
		CSEND(TBL_MSG + 2, rand_msglen_ind, intsize * n, -1, NODE_PTYPE);
		CSEND(TBL_MSG + 3, rand_msgcnt, intsize * n, -1, NODE_PTYPE);

	} else {		/* nodes other than 0  */
		/* receive info for the 'more random' part      */
		CRECV(TBL_MSG, &rand_pairs, intsize);
		CRECV(TBL_MSG + 1, pair, intsize * n);
		CRECV(TBL_MSG + 2, rand_msglen_ind, intsize * n);
		CRECV(TBL_MSG + 3, rand_msgcnt, intsize * n);
	}

	/* in each node, compute the bytes sent per iteration (pass) */
	rand_msglen = ti.msglen[rand_msglen_ind[node]];
        /* do not exceed 2 Mbytes               */
        while(rand_msglen * rand_msgcnt[node] > 2097152)
                rand_msglen = rand_msglen / 2;
	if (rand_msglen == 4)
                rand_msglen = 2; /* the 4 byte length is not used */

	extra_bytes[rand_msglen_ind[node]][ipass] +=
		rand_msgcnt[node] * rand_msglen;

	gsync();
	/* Do not loop through the message lengths anymore, as	   */
	/* they are randomly chosen for each pair of communicating */
	/* nodes, in each pass					   */
	/* Avoid possible deadlock for large messages, with IRECV  */
	if (pair[node] != -1) {	/* then I have a pair	 */
		/* Clear the recv buffer */
		/* Use the maximum possible values	 */
		numwords = 2097152 / 8;
		dcopy(numwords, &zero, 0, bufr, 1);
		S1[rand_msglen_ind[node]][ipass] = dclock();
		/* Loop on msgcnt */
		for (imsg = 0; imsg < rand_msgcnt[node]; imsg++) {
			mid = IRECV(RAND1_MSG + pair[node] + n * imsg,
				    bufr + imsg * rand_msglen,
				    rand_msglen);
			CSEND(RAND1_MSG + node + n * imsg,
			      bufs + imsg * rand_msglen,
			      rand_msglen, pair[node], NODE_PTYPE);
			msgwait(mid);
		}		/* msgcnt loop */
		S2[rand_msglen_ind[node]][ipass] += (dclock() -
					  S1[rand_msglen_ind[node]][ipass]);
		/* Check the correctness of the received message */
		numwords = rand_msglen / 8 * rand_msgcnt[node];
		if (compare(numwords, bufs, bufr)) {
			fprintf(stderr, "COMTEST - NODE %d : *** RANDOM 3 "
				"TEST ERROR ***\n", mynode());
			return;
		}
	}
}

/****** random1_f

  DESCRIPTION:
        Send n force type messages from n random sources to n random 
	destinations.

*/

void
random1_f(n, passes, msgcnt, msglen)
	int             n;	/* Total number of nodes */
	int             passes;	/* Number of passes */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message */
{
	int             i, ipass, imsg;
	int             tmp;
	int             src[MAXNODES];
	int             dst[MAXNODES];
	int             rid[MAXNODES];	/* Recv msg ID's */
	int             intsize;
	int             rand_pairs;
	int		rand_ctr = 0;
	int             x, y;
	int             mid;
	int             numwords;
	void            getstats();
	double          zero = 0.0;

	intsize = sizeof(int);

	/* milliseconds change from call to call     */
	seed = (unsigned int) (1000 * dclock());

	/* node 0 establishes the n random message sources and destinations */
	/* that exchange in all 'n' messages of length 'msglen', 'msgcnt'   */
	/* times, everything repeated 'passes' times                        */

	if (node == 0) {
	        srand(seed);
		for (i = 0; i < n; i++) {
			src[i] = rand() % n;
			dst[i] = rand() % n;
			/* make sure the dst is different from the src */
			rand_ctr = 0;
			while (dst[i] == src[i]) {
				dst[i] = rand() % n;
				if (++rand_ctr > 1000) {
					dst[i] = dst[i] - 1;
					break;
				}
			}
		}

		/* send the two tables to all the other nodes   */
		CSEND(SRC_TBL_MSG, src, intsize * n, -1, NODE_PTYPE);
		CSEND(DST_TBL_MSG, dst, intsize * n, -1, NODE_PTYPE);
	} else {		/* nodes other than 0  */
		CRECV(SRC_TBL_MSG, src, intsize * n);
		CRECV(DST_TBL_MSG, dst, intsize * n);
	}

	/* Clear s1[] and s2[]          */
	for (i = 0; i < MAXITER; i++) {
		s1[i] = 0;
		s2[i] = 0;
	}

	gsync();
	/* Loop on passes */
	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);
		s1[ipass] = dclock();
		/* Loop on msgcnt */
		for (imsg = 0; imsg < msgcnt; imsg++) {
			/* Have to cope with possibly being a multiple  */
			/* source and/or a multiple destination         */
			/* (using confirmation messages, the sources    */
			/* and the destination move in lockstep at the  */
			/* msgcnt loop iteration level)                 */

			/* loop over the destinations, post the necessary       */
			/* irecv-s, and send signals that you are ready         */
			for (i = 0; i < n; i++) {
				if (node == dst[i]) {
					rid[i] = IRECV(RAND_F_MSG +
						       i + n * imsg,
					      bufr + imsg * msglen, msglen);
					CSEND(CONF_MSG + i + n * imsg, &tmp, 0,
					      src[i], NODE_PTYPE);
				}
			}

			/* loop over the sources, receive all the necessary     */
			/* signals from the destination nodes, and send the     */
			/* force type messages (if any)                         */
			for (i = 0; i < n; i++) {
				if (node == src[i]) {
					CRECV(CONF_MSG + i + n * imsg,
					      &tmp, 0);
					CSEND(RAND_F_MSG + i + n * imsg,
					      bufs + imsg * msglen,
					      msglen, dst[i], NODE_PTYPE);
				}
			}

			/* loop over the destinations, and wait for the force   */
			/* type messages (if any)                               */
			for (i = 0; i < n; i++) {
				if (node == dst[i])
					msgwait(rid[i]);
			}

		}		/* msgcnt loop */
		s2[ipass] = dclock() - s1[ipass];
		/* make sure messages from different passes do not get mixed */
		gsync();
		/* Check the correctness of the received message */
		for (i = 0; i < n; i++) {
			if (node == dst[i]) {
				if (compare(numwords, bufs, bufr)) {
					fprintf(stderr, "COMTEST - NODE %d : "
						"*** RANDOM 1 (FORCE)  TEST "
						"ERROR ***\n", mynode());
					return;
				}
			}
		}
	}			/* ipass loop */
}

/****** random2_f

  DESCRIPTION:
        Send n force type messages from n random sources to n random 
	destinations. The sources are confined to the lower half of the 
	partition, and the destinations to its upper half.

*/

void
random2_f(n, passes, msgcnt, msglen)
	int             n;	/* Total number of nodes */
	int             passes;	/* Number of passes */
	int             msgcnt;	/* Number of messages to send on each pass */
	int             msglen;	/* Length of each message */
{
	int             i, ipass, imsg;
	int             tmp;
	int             src[MAXNODES];
	int             dst[MAXNODES];
	int             rid[MAXNODES];	/* Recv msg ID's */
	int             intsize;
	int             rand_pairs;
	int             x, y;
	int             mid;
	int             numwords;
	void            getstats();
	double          zero = 0.0;

	intsize = sizeof(int);

	/* milliseconds change from call to call     */
	seed = (unsigned int) (1000 * dclock());

	/*
	 * node 0 establishes the n random message sources in the lower half
	 * of the
	 */

	/*
	 * partition, and the n random message destinations in the upper half
	 * of
	 */
	/* the partition                                                            */

	if (node == 0) {
	        srand(seed);
		for (i = 0; i < n; i++) {
			src[i] = rand() % (n / 2);
			dst[i] = n / 2 + (rand() % (n / 2));
		}

		/* send the two tables to all the other nodes   */
		CSEND(SRC_TBL_MSG, src, intsize * n, -1, NODE_PTYPE);
		CSEND(DST_TBL_MSG, dst, intsize * n, -1, NODE_PTYPE);

#ifdef DBG
		printf("src : %d %d %d %d\n", src[0], src[1], src[2], src[3]);
		printf("dst : %d %d %d %d\n", dst[0], dst[1], dst[2], dst[3]);
#endif

	} else {		/* nodes other than 0  */
		CRECV(SRC_TBL_MSG, src, intsize * n);
		CRECV(DST_TBL_MSG, dst, intsize * n);
	}

	/* Clear s1[] and s2[]          */
	for (i = 0; i < MAXITER; i++) {
		s1[i] = 0;
		s2[i] = 0;
	}

	gsync();
	/* Loop on passes */
	for (ipass = 0; ipass < passes; ipass++) {
		/* Clear the recv buffer */
		numwords = msglen / 8 * msgcnt;
		dcopy(numwords, &zero, 0, bufr, 1);
		s1[ipass] = dclock();
		/* Loop on msgcnt */
		for (imsg = 0; imsg < msgcnt; imsg++) {
			/* Have to cope with possibly of being multiple */
			/* source or a multiple destination             */
			/* (using confirmation messages, the sources    */
			/* and the destination move in lockstep at the  */
			/* msgcnt loop iteration level)                 */

			/* loop over the destinations, post the necessary       */
			/* irecv-s, and send signals that you are ready         */
			for (i = 0; i < n; i++) {
				if (node == dst[i]) {
					rid[i] = IRECV(RAND_F_MSG +
						       i + n * imsg,
					      bufr + imsg * msglen, msglen);
					CSEND(CONF_MSG + i + n * imsg, &tmp, 0,
					      src[i], NODE_PTYPE);
				}
			}

			/* loop over the sources, receive all the necessary     */
			/* signals from the destination nodes, and send the     */
			/* force type messages (if any)                         */
			for (i = 0; i < n; i++) {
				if (node == src[i]) {
					CRECV(CONF_MSG + i + n * imsg,
					      &tmp, 0);
					CSEND(RAND_F_MSG + i + n * imsg,
					      bufs + imsg * msglen,
					      msglen, dst[i], NODE_PTYPE);
				}
			}

			/* loop over the destinations, and wait for the force   */
			/* type messages (if any)                               */
			for (i = 0; i < n; i++) {
				if (node == dst[i])
					msgwait(rid[i]);
			}

		}		/* msgcnt loop */
		s2[ipass] = dclock() - s1[ipass];
		/* make sure messages from different passes do not get mixed */
		gsync();
		/* Check the correctness of the received message */
		for (i = 0; i < n; i++) {
			if (node == dst[i]) {
				if (compare(numwords, bufs, bufr)) {
					fprintf(stderr, "COMTEST - NODE %d : "
						"*** RANDOM 2  (FORCE) TEST ERROR ***\n", mynode());
					return;
				}
			}
		}
	}			/* ipass loop */
}


/****** random3_f

  DESCRIPTION:
        Exchange a random number of messages of random lengths, between a 
	random number of node pairs, chosen randomly, everything repeated a 
	random number of times (iterations).

*/


void
random3_f(n, ipass, numlens)
	int             n;	/* Total number of nodes        */
	int             ipass;	/* Number of passes             */
	int             numlens;/* Number of message lengths    */
{
	int             i, j, imsg;
	int             tmp;
	int             src[MAXNODES];
	int             dst[MAXNODES];
	int             rid[MAXNODES];	/* Recv msg ID's */
	int             intsize;
	int             rand_pairs;
	int             x, y;
	int             mid;
	int             numwords;
	void            getstats();
	double          zero = 0.0;
	int             rand_msglen;

	intsize = sizeof(int);

	/* milliseconds change from call to call     */
	seed = (unsigned int) (1000 * dclock());

	/*
	 * Load the communication system with a random number of messages of
	 * random
	 */
	/* lengths, exchanged between a random number of node pairs, chosen         */

	/*
	 * randomly, everything repeated a random number of times
	 * (iterations)
	 */

	if (node == 0) {
		/* prepare the 'more random' test               */
		/* initialize the array pair[] with -1          */
		for (i = 0; i < MAXNODES; i++)
			pair[i] = -1;

		/* determine the random number of pairs         */
	        srand(seed);
		rand_pairs = rand() % (n / 2);

#ifdef DEBUG
		printf("DEBUG : rand_pairs = %d\n", rand_pairs);
#endif

		/* determine the pairs                          */
		for (i = 0; i < rand_pairs; i++) {
			/* find an unpaired PE x        */
			x = rand() % n;
			while (pair[x] != -1) {
				x++;
				x = x % n;
			}
			/* find an unpaired PE y        */
			y = rand() % n;
			while (pair[y] != -1) {
				y++;
				y = y % n;
			}
			/* pair them                    */
			pair[x] = y;
			pair[y] = x;
			/* set a message length for this pair   */
			rand_msglen_ind[x] = rand() % numlens;
			rand_msglen_ind[y] = rand_msglen_ind[x];
			/* set a msgcnt for this pair           */
			/* this is a bit off the pattern, but as the default */
			/* is msgcnt = 1 , this would mean to always have    */
			/* (in the default case) rand_msgcnt[x] = 1          */
			rand_msgcnt[x] = 1 + (rand() % (MAXCNT - 1));	
				/* at least 1 */
			rand_msgcnt[y] = rand_msgcnt[x];
		}

#ifdef DEBUG
		for (i = 0; i < n; i++) {
			printf("DEBUG : p %d c %d i %d\n", pair[i],
			       rand_msgcnt[i], rand_numiter[i]);
		}
#endif

		/* transmit all these to the other nodes        */
		CSEND(TBL_MSG, &rand_pairs, intsize, -1, NODE_PTYPE);
		CSEND(TBL_MSG + 1, pair, intsize * n, -1, NODE_PTYPE);
		CSEND(TBL_MSG + 2, rand_msglen_ind, intsize * n, -1, NODE_PTYPE);
		CSEND(TBL_MSG + 3, rand_msgcnt, intsize * n, -1, NODE_PTYPE);

	} else {		/* nodes other than 0  */
		/* receive info for the 'more random' part      */
		CRECV(TBL_MSG, &rand_pairs, intsize);
		CRECV(TBL_MSG + 1, pair, intsize * n);
		CRECV(TBL_MSG + 2, rand_msglen_ind, intsize * n);
		CRECV(TBL_MSG + 3, rand_msgcnt, intsize * n);
	}

	/* in each node, compute the bytes sent per iteration (pass) */
	rand_msglen = ti.msglen[rand_msglen_ind[node]];
        /* do not exceed 2 Mbytes               */
        while(rand_msglen * rand_msgcnt[node] > 2097152)
                rand_msglen = rand_msglen / 2;
	if (rand_msglen == 4)
		rand_msglen = 2; /* the 4 byte length is not used */

	extra_bytes[rand_msglen_ind[node]][ipass] +=
		rand_msgcnt[node] * rand_msglen;

	gsync();
	/* Do not loop through the message lengths anymore, as     */
	/* they are randomly chosen for each pair of communicating */
	/* nodes, in each pass                                     */
	if (pair[node] != -1) {	/* then I have a pair    */
		/* Clear the recv buffer */
		/* Use the maximum possible values      */
		numwords = 2097152 / 8;
		dcopy(numwords, &zero, 0, bufr, 1);
		S1[rand_msglen_ind[node]][ipass] = dclock();
		/* Loop on msgcnt */
		for (imsg = 0; imsg < rand_msgcnt[node]; imsg++) {
			mid = IRECV(RAND1_F_MSG + pair[node] + imsg * n,
				    bufr + imsg * rand_msglen,
				    rand_msglen);
			CSEND(CONF_MSG + node + n * imsg, &tmp, 0,
			      pair[node], NODE_PTYPE);
			CRECV(CONF_MSG + pair[node] + n * imsg,
			      &tmp, 0);
			CSEND(RAND1_F_MSG + node + n * imsg,
			      bufs + imsg * rand_msglen,
			      rand_msglen, pair[node], NODE_PTYPE);
			msgwait(mid);
		}		/* msgcnt loop */
		S2[rand_msglen_ind[node]][ipass] += (dclock() -
					  S1[rand_msglen_ind[node]][ipass]);
		/* Check the correctness of the received message */
		numwords = rand_msglen / 8 * rand_msgcnt[node];
		if (compare(numwords, bufs, bufr)) {
			fprintf(stderr, "COMTEST - NODE %d : *** RANDOM 3 "
				"(FORCE) TEST ERROR ***\n", mynode());
			return;
		}
	}
}


/****** random_drv

  DESCRIPTION:
        This is the random driver routine.  It calls the random message
        passing timing test and prints results.

*/

random_drv()
{
	double          start, delta;
	double          abw;
	double          time;
	double          temp;
	/* no latency in this case	 */
	double          bytes_t;
	int             ilen;
	char           *charptr;
	int             do_random1, do_random2, do_random3;
	int             i;
	int             ipass;

	/* Get the types of random tests to run		 */

	do_random1 = FALSE;
	do_random2 = FALSE;
	do_random3 = FALSE;

	charptr = ti.rand_spec;
	while (*charptr) {
		switch (*charptr++) {
		case '1':
			do_random1 = TRUE;
			break;
		case '2':
			do_random2 = TRUE;
			break;
		case '3':
			do_random3 = TRUE;
		}
	}

	if (!do_random1 && !do_random2 && !do_random3) {
		if (node == 0)
			printf("\n\n\n'R A N D O M'  T E S T  :  I N V A L I D  A R G U M E N T "
			       " S P E C I F I E D  -  M U S T  B E  A N Y  O F  1 , 2 , OR  3\n\n\n");
		return;
	}
	/* Fill send buffer */
	start = 0;
	delta = 7.7777;
	dramp(BUFSIZE, start, delta, bufs);

rand1:

	if (!do_random1)
		goto rand2;	/* ugly, but saves indentation space	 */

	/* Header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   R A N D O M 1   T I M I N G   S U M M A R Y\n");
		printf("\n");
		printf("   Bandwidth Description:\n");
		printf("      This is aggregate bandwidth.  All data transfers are\n");
		printf("      included and consist of nodes*msglen*msgcnt bytes.\n");
		printf("      A number of n messages of length msglen is sent from\n");
		printf("      n random sources to n random destinations, msgcnt times\n");
		printf("      for each iteration , where n is the number of nodes.\n");
		printf("      Note, times are per iteration.\n");
		printf("\n");
		printf("\n");
                printf("      W A R N I N G : THIS IS A RANDOM TEST. IT MAY BECOME\n");
		printf("      SOMETIMES EQUIVALENT TO A FAN-IN TEST FOR ONE NODE.\n");
                printf("\n");
                printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
	/* Bandwidth header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      **********************************************\n");
		printf("                      *   B A N D W I D T H   S T A T I S T I C S  *\n");
		printf("                      **********************************************\n");
		printf("\n");
		printf(" MSGLEN  <--------- T I M E  (ms) -------->  <-- B A N D W I D T H   (MB/S) -->\n");
		printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
		printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ====== ======\n");
	}
	/* get 'random' code in cache */
	if (ti.force)
		random1_f(ti.nodes, ti.numiter, 1, 0);
	else
		random1(ti.nodes, ti.numiter, 1, 0);

	/* Loop over different message lengths */
	for (ilen = 0; ilen < ti.numlens; ilen++) {

		if (ti.force)
			random1_f(ti.nodes, ti.numiter, ti.msgcnt,
				  ti.msglen[ilen]);
		else
			random1(ti.nodes, ti.numiter, ti.msgcnt,
				ti.msglen[ilen]);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Timing Statistics */
			getstats(s2, ti.numiter, &si);
			printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
			       ti.msglen[ilen],
			       si.max * 1000, si.min * 1000, si.mean * 1000,
			       si.median * 1000, si.stddev * 1000);

			/* Bandwidth Statistics */
			for (i = 0; i < ti.numiter; i++) {
				bytes_t = (double) ti.msgcnt * ti.msglen[ilen] * 
					ti.nodes;

				/*
				 * s2[] contains times before, and bandwidths
				 * after
				 */
				/* the assignment   */

				/*
				 * s2[i] should never be 0, as in each pass
				 * some nodes
				 */
				/* are communicating		*/
				s2[i] = bytes_t / s2[i] / 1e6;
			}
			getstats(s2, ti.numiter, &si);
			printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
			     si.max, si.min, si.mean, si.median, si.stddev);
		}
	}

rand2:

	if (!do_random2)
		goto rand3;	/* ugly, but saves indentation space    */

	/* Header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   R A N D O M 2   T I M I N G   S U M M A R Y\n");
		printf("\n");
		printf("   Bandwidth Description:\n");
		printf("      This is aggregate bandwidth.  All data transfers are\n");
		printf("      included and consist of nodes*msglen*msgcnt bytes.\n");
		printf("      A number of n messages of length msglen is sent from\n");
		printf("      n random sources to n random destinations, msgcnt times\n");
		printf("      for each iteration , where n is the number of nodes.\n");
		printf("      The sources are restricted to the lower half of the \n");
		printf("      partition, and the destinations, to the upper half.\n");
		printf("      Note, times are per iteration.\n");
		printf("\n");
                printf("\n");
                printf("      W A R N I N G : THIS IS A RANDOM TEST. IT MAY BECOME\n");
                printf("      SOMETIMES EQUIVALENT TO A FAN-IN TEST FOR ONE NODE.\n");
                printf("\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
	/* Bandwidth header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      **********************************************\n");
		printf("                      *   B A N D W I D T H   S T A T I S T I C S  *\n");
		printf("                      **********************************************\n");
		printf("\n");
		printf(" MSGLEN  <--------- T I M E  (ms) -------->  <-- B A N D W I D T H   (MB/S) -->\n");
		printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
		printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ====== ======\n");
	}
	/* get 'random' code in cache */
	if (ti.force)
		random2_f(ti.nodes, ti.numiter, 1, 0);
	else
		random2(ti.nodes, ti.numiter, 1, 0);

	/* Loop over different message lengths */
	for (ilen = 0; ilen < ti.numlens; ilen++) {
		/* no check on the recv buffer contents in the random   */
		/* case - some nodes may have not been the destination  */
		/* of any message ; yet, a check could be performed for  */
		/* those which have received messages                   */

		if (ti.force)
			random2_f(ti.nodes, ti.numiter, ti.msgcnt,
				  ti.msglen[ilen]);
		else
			random2(ti.nodes, ti.numiter, ti.msgcnt,
				ti.msglen[ilen]);

		gdhigh(s2, ti.numiter, work);	/* Use max. */

		if (node == 0) {
			/* Timing Statistics */
			getstats(s2, ti.numiter, &si);
			printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
			       ti.msglen[ilen],
			       si.max * 1000, si.min * 1000, si.mean * 1000,
			       si.median * 1000, si.stddev * 1000);

			/* Bandwidth Statistics */
			for (i = 0; i < ti.numiter; i++) {
				bytes_t = (double) ti.msgcnt * ti.msglen[ilen] * 
					ti.nodes;

				/*
				 * s2[] contains times before, and bandwidths
				 * after
				 */
				/* the assignment    */

				/*
				 * s2[i] should never be 0, as in each pass
				 * some nodes
				 */
				/* are communicating   */
				s2[i] = bytes_t / s2[i] / 1e6;
			}
			getstats(s2, ti.numiter, &si);
			printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
			     si.max, si.min, si.mean, si.median, si.stddev);
		}
	}

rand3:

	if (!do_random3)
		return;

	/* Header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   R A N D O M 3   T I M I N G   S U M M A R Y\n");
		printf("\n");
		printf("   Bandwidth Description:\n");
		printf("      This is aggregate bandwidth.  All data transfers are\n");
		printf("      included and consist of a random number of messages of\n");
		printf("      random lengths, exchanged between a random number of\n");
		printf("      node pairs, chosen randomly, everything repeated a\n");
		printf("      random number of times (iterations)\n");
		printf("      Note, times are per iteration.\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
	/* Bandwidth header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("                      **********************************************\n");
		printf("                      *   B A N D W I D T H   S T A T I S T I C S  *\n");
		printf("                      **********************************************\n");
		printf("\n");
		printf(" MSGLEN  <--------- T I M E  (ms) -------->  <-- B A N D W I D T H   (MB/S) -->\n");
		printf("(bytes)   Max    Min    Mean  Median Stddev   Max    Min    Mean  Median Stddev\n");
		printf("=======  ====== ====== ====== ====== ======  ====== ====== ====== ====== ======\n");
	}
	/* loop ti.numlens times, to compensate the absence of this */
	/* loop in random3() and random3_f()                        */
	for (ilen = 0; ilen < ti.numlens; ilen++) {
		/* Loop over the passes				 */
		for (ipass = 0; ipass < ti.numiter; ipass++) {
			if (ti.force)
				random3_f(ti.nodes, ipass, ti.numlens);
			else
				random3(ti.nodes, ipass, ti.numlens);
		}
	}

	gdhigh(S2, MAXLENS * MAXITER, Work);	/* Take max time for each pas
						 * and message length */
	gisum(extra_bytes, (long) (MAXLENS * MAXITER), Extra);	
		/* Sum the bytes sent for each and each message length */

	if (node == 0) {
		for (ilen = 0; ilen < ti.numlens; ilen++) {
			/* Timing Statistics */
			/* Use the times saved in S2[ilen][]	 */
			getstats((double *) (S2[ilen]), ti.numiter, &si);
			printf("%7d%8.2f%7.2f%7.2f%7.2f%7.2f",
			       ti.msglen[ilen],
			       si.max * 1000, si.min * 1000, si.mean * 1000,
			       si.median * 1000, si.stddev * 1000);

			/* Bandwidth Statistics */
			for (i = 0; i < ti.numiter; i++) {
				/* S2[ilen][] contains times before, and   */
				/* bandwidths after the assignment         */
				if (S2[ilen][i] != 0)
					S2[ilen][i] =
						(double) (extra_bytes[ilen][i]) /
						S2[ilen][i] / 1e6;
			}
			getstats((double *) (S2[ilen]), ti.numiter, &si);
			printf("%8.2f%7.2f%7.2f%7.2f%7.2f\n",
			     si.max, si.min, si.mean, si.median, si.stddev);
		}
	}
}


/****** latency_drv

  DESCRIPTION:
	This is the latency driver routine. It measures the overhead of
	individual message passing primitives, as well as the alpha parameter 
	in the following transfer time equation:

		Transfer Time = alpha + beta*msglen

	where alpha represents the overhead of message passing for sending a
	zero byte message (this includes software overhead and bus handshake
	overhead), beta represents the reciprocal of the message passing 
	bandwidth, and msglen is the length of a message.

	The alpha parameter is obtained by timing a round trip zero byte 
	message between node 0 and (nodes-1), and dividing by 2.

*/


latency_drv()
{
	int             msgid[MAXITER + 1];
	int             imsg;
	void            getstats(), prtstats();
	double          t1, t2;
	int             latpi;	/* Latency per iter, the time for msgcnt 0
				 * length messages. Time is in usec/msg. */
	int             i;

	/* Header */
	if (node == 0) {
		printf("\n");
		printf("\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   L A T E N C Y   T I M I N G   S U M M A R Y\n");
		printf("\n");
		printf("   Latency Description:\n");
		printf("      This is the latency associated with individual\n");
		printf("      message passing routines, as well as alpha in the\n");
		printf("      following equation:\n");
		printf("\n");
		printf("	     Transfer Time = alpha + beta*msglen\n");
		printf("\n");
		printf("      All messages are sent from node 0 to (nodes-1).\n");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("\n");
	}
	/* S E N D   L A T E N C Y  */

	gsync();
	if (node == 0) {

		CSEND(CACHE_MSG, bufs, 0, nodesm1, NODE_PTYPE);	/* get code in cache */

		/* Unforced Synchronous */

		t1 = dclock();
		for (i = 0; i < ti.numiter; i++)
			CSEND(SYNC_MSG, bufs, 0, nodesm1, NODE_PTYPE);
		t2 = dclock();
		latpi = (int) ((t2 - t1) / ti.numiter * 1e6);
		printf("Unforced CSEND Latency (microseconds): %d\n", latpi);

		/* Forced Synchronous */

		t1 = dclock();
		for (i = 0; i < ti.numiter; i++)
			CSEND(FORCETYP + SYNC_MSG, bufs, 0, nodesm1, NODE_PTYPE);
		t2 = dclock();
		latpi = (int) ((t2 - t1) / ti.numiter * 1e6);
		printf("Forced   CSEND Latency (microseconds): %d\n", latpi);

		/* Unforced Asynchronous */

		/* get code in cache */
		msgid[0] = ISEND(CACHE_MSG, bufs, 0, nodesm1, 0);

		msgwait(msgid[0]);

		t1 = dclock();
		for (i = 0; i < ti.numiter; i++)
			msgid[i] = ISEND(ASYNC_MSG, bufs, 0, nodesm1, 0);
		t2 = dclock();
		for (i = 0; i < ti.numiter; i++)
			msgwait(msgid[i]);
		latpi = (int) ((t2 - t1) / ti.numiter * 1e6);
		printf("Unforced ISEND Latency (microseconds): %d\n", latpi);

		/* Forced Asynchronous */

		t1 = dclock();
		for (i = 0; i < ti.numiter; i++)
			msgid[i] = ISEND(FORCETYP + ASYNC_MSG, bufs, 0, nodesm1, 0);
		t2 = dclock();
		for (i = 0; i < ti.numiter; i++)
			msgwait(msgid[i]);
		latpi = (int) ((t2 - t1) / ti.numiter * 1e6);
		printf("Forced   ISEND Latency (microseconds): %d\n", latpi);
	}

	/*
	 * Clean up (receive the unforced sends).  Don't worry about forced
	 * sends since they get thrown away.
	 */

	else if (node == nodesm1) {

		CRECV(CACHE_MSG, bufs, 0);	/* get code in cache */

		/* Unforced Synchronous */

		for (i = 0; i < ti.numiter; i++)
			CRECV(SYNC_MSG, bufs, 0);

		/* Unforced Asynchronous */

		CRECV(CACHE_MSG, bufs, 0);	/* get code in cache */

		for (i = 0; i < ti.numiter; i++)
			CRECV(ASYNC_MSG, bufs, 0);
	}
	/* M E A S U R E   A L P H A  */

	if (node == 0) {

		/* Get code in cache and synchronize */
		CSEND(CACHE_MSG, bufs, 0, nodesm1, NODE_PTYPE);
		CRECV(CACHE_MSG, bufs, 0);

		for (i = 0; i < ti.numiter; i++) {
			s1[i] = dclock();
			CSEND(ALPHA_MSG + i, bufs, 0, nodesm1, NODE_PTYPE);
			CRECV(ALPHA_MSG + i, bufr, 0);
			s2[i] = (dclock() - s1[i]) / 2.0;
		}

		/* Get statistics for alpha */
		getstats(s2, ti.numiter, &si);

		/* Print Statistics for Alpha */
		prtstats(&si);
	} else if (node == nodesm1) {
		/* Get code in cache and synchronize */
		CRECV(CACHE_MSG, bufs, 0);
		CSEND(CACHE_MSG, bufs, 0, 0, NODE_PTYPE);

		for (i = 0; i < ti.numiter; i++) {
			CRECV(ALPHA_MSG + i, bufr, 0);
			CSEND(ALPHA_MSG + i, bufs, 0, 0, NODE_PTYPE);
		}
	}
}


/* SUPPORT FUNCTIONS	 */

/****** is_power_of_2

  DESCRIPTION:
        Returns 1 if the integer argument is a power of 2, and 0 otherwise.

*/

int
is_power_of_2(n)
	int             n;
{
	if (n <= 0) {
		fprintf(stderr, "COMTEST - NODE %d - IS_POWER_OF_2 ERROR : n = %d\n",
			mynode(), n);
		exit(1);
	}
	while (n > 1) {
		if ((n / 2) * 2 != n)
			return (FALSE);
		n = n / 2;
	}

	return (TRUE);
}


/****** init

  DESCRIPTION:
	Initialize all variables.

*/

init()
{

	extern void     resume();

	/* Get memory for send and recv bufs */
	bufs = (char *) malloc(BUFSIZE * 8 * 2 + 32);
	if (bufs != 0) {
		/* Round to next 32 byte boundary */
		bufs = bufs + (32 - (int) bufs % 32);
	} else {
		fprintf(stderr, "COMTEST - NODE %d : *** INIT (MALLOC) ERROR *** - "
			"Not enough memory!\n", mynode());
		exit(0);
	}

	bufr = bufs + BUFSIZE * 8;

	node = mynode();
	nodes = numnodes();
	nodesm1 = nodes - 1;
	ti.nodes = nodes;

#ifdef PARAGON
	/* Use SIGCONT to resume processing */
	signal(SIGCONT, resume);
#endif

}

/****** resume

  DESCRIPTION:
        Empty function. Called when a stopped process receives a
	SIGCONT signal

*/

static void
resume(sig)
	int             sig;
{
}

/****** dramp

  DESCRIPTION:
	Fill a vector with a ramp.

*/

dramp(n, start, delta, x)
	int             n;	/* Number of words to init */
	double          start;	/* Initial value */
	double          delta;	/* Delta value */
	double          x[];	/* Array to init */
{
	int             i;

	for (i = 0; i < n; i++)
		x[i] = start + i * delta;
}


/****** compare

  DESCRIPTION:
	Compare two arrays of doubles and return number of diffs.

*/

compare(n, a, b)
	int             n;
	double         *a, *b;
{
	int             i;
	int             numdiffs = 0;

	for (i = 0; i < n; i++)
		if (b[i] != a[i])
			numdiffs++;
	return numdiffs;
}


/****** printheader

  DESCRIPTION:
	Print header common to all tests.

*/

printheader(s)
	char            s[];
{
	printf("-------------------------------------------------------------\n");
	printf("%s\n", s);
	printf("-------------------------------------------------------------\n");
	printf("\n");
	printf("\n");
}

/****** do_test_type

  DESCRIPTION:
	This routine takes a single character input string and
	searches the type_spec string for it.  It returns
	a position number of the found test type as follows:
		pos = 0 (not found)
		pos > 0 (found)

*/

do_test_type(t)
	char           *t;
{
	int             typlen;
	int             pos = 0;
	int             i;

	typlen = strlen(ti.type_spec);

	for (i = 0; i < typlen; i++) {
		if (*t == ti.type_spec[i]) {
			pos = i + 1;
			break;
		}
	}
	return pos;
}

/****** dcopy

  DESCRIPTION:
	Starting with position 0, up to the len-th elements,
	copies every other ia-th element from a[] into every
	other ib-th position of b[]. The arrays a[] and b[]
	contain doubles.
*/

dcopy(len, a, ia, b, ib)
	int             len, ia, ib;
	double          a[], b[];
{
	int             i;
	for (i = 0; i < len; i++)
		b[i * ib] = a[i * ia];
}

/****** Set Defaults

  FUNCTION:
    set_defaults

*/

set_defaults()
{
	int             i;

	ti.numiter = 25;
	ti.msgcnt = 1;

	ti.msglen[0] = 0;
	ti.msglen[1] = 8;
	ti.msglen[2] = 16;
	ti.msglen[3] = 32;
	ti.msglen[4] = 64;
	ti.msglen[5] = 128;
	ti.msglen[6] = 256;
	ti.msglen[7] = 512;
	ti.msglen[8] = 1024;
	ti.msglen[9] = 2048;
	ti.msglen[10] = 4096;
	ti.msglen[11] = 8192;
	ti.msglen[12] = 16384;
	ti.msglen[13] = 32768;
	ti.msglen[14] = 65536;
	ti.msglen[15] = 131072;
	ti.msglen[16] = 262144;
	ti.msglen[17] = 524288;
	ti.msglen[18] = 1048576;
	ti.msglen[19] = 2097152;

	ti.force = FALSE;
	ti.help = FALSE;
	ti.interactive = FALSE;
	ti.numlens = 13;
	ti.underscore = FALSE;
	strcpy(ti.part_name, ".compute");
	ti.nodes = nodes;
	strcpy(ti.type_spec, "perbfal");
	ti.random = FALSE;
	ti.order = FALSE;

	ti.order_numlens = 3;

	ti.order_msglen[0] = 262144;
	ti.order_msglen[1] = 0;
	ti.order_msglen[2] = 8;
}

/****** parse_cmd

  FUNCTION NAME:
    parse_cmd

  PURPOSE:
    Parse command line and set any parameters for :

    comtest [-f] [-h] [-i] [-u] [-c msgcnt] [-m numlens] [-n numiter] \
            [-t type_spec] [-b hxw] [-k hxw] [-r {1|2|3}] [-o] [-pn part_name]

*/

parse_cmd(argc, argv)
	int             argc;
	char           *argv[];
{
	char            c;
	char            c2;
	int             i;
	char            sep;


	/* Parse cmdline and init ti any flags */
	optind = 1;
	while ((c = getopt(argc, argv, "fhiuor:c:m:n:t:b:k:")) != EOF) {
		switch (c) {
		case 'c':
			ti.msgcnt = atoi(optarg);
			if (ti.msgcnt < 1) {
				if (node == 0)
					fprintf(stderr, "COMTEST : *** "
					"MESSAGE COUNT HAS TO BE > 0 ***\n");
				exit(1);
			}
			break;

		case 'f':
			ti.force = TRUE;
			break;

		case 'h':
			ti.help = TRUE;
			break;

		case 'i':
			ti.interactive = TRUE;
			break;

		case 'm':
			ti.numlens = atoi(optarg);
			if (ti.numlens > MAXLENS || ti.numlens < 1) {
				if (node == 0)
					fprintf(stderr, "COMTEST - NODE %d : "
						"ERROR - *** EXCEEDED LIMITS :"
						" MAXIMUM NUMBER OF MESSAGE "
						"LENGTHS =  %d ***\n",
						mynode(), MAXLENS);
				exit(1);
			}
			break;

		case 'n':
			ti.numiter = atoi(optarg);
			if (ti.numiter > MAXITER || ti.numiter < 1) {
				if (node == 0)
					fprintf(stderr, "COMTEST - NODE %d : ERROR "
						"- *** EXCEEDED LIMITS :"
						" MAXIMUM ITERATIONS COUNT = %d"
						" ***\n", mynode(), MAXITER);
				exit(1);
			}
			break;

		case 't':
			strcpy(ti.type_spec, optarg);
			/* Check for invalid test type */
			for (i = 0; i < strlen(ti.type_spec); i++) {
				c2 = ti.type_spec[i];
				if (c2 != 'b' &&
				    c2 != 'e' &&
				    c2 != 'r' &&
				    c2 != 'p' &&
				    c2 != 'l' &&
				    c2 != 'a' &&
				    c2 != 'f' &&
				    c2 != 'n') {
					if (node == 0)
						fprintf(stderr, "COMTEST - NODE %d"
							" : *** ERROR *** "
							"- Invalid test type "
							"specification ***\n", 
							mynode());
					exit(1);
				}
			}
			break;

		case 'u':
			ti.underscore = TRUE;
			break;

		case 'o':
			ti.order = TRUE;
			break;

		case 'r':
			ti.random = TRUE;
			strcpy(ti.rand_spec, optarg);
			break;
		case 'b':
			ti.bisection = TRUE;
			strcpy(ti.bisect_spec, optarg);	/* just for print_hdr() */
			sscanf(optarg, "%d%c%d", &ti.height, &sep, &ti.width);
			if (ti.height < 1 || ti.height > MAXHEIGHT ||
			    ti.width < 1 || ti.width > MAXWIDTH ||
			    (sep != 'x' && sep != 'X') ||
			    (ti.height * ti.width != numnodes())) {
				if (node == 0) {
					fprintf(stderr, "COMTEST - NODE %d : ERROR -"
						" *** INCORRECT "
						"RECTANGULAR MESH SPECIFICATION\n", 
						mynode());
					exit(1);
				}
			}

			/*
			 * fprintf(stderr," DEBUG : h = %d w = %d\n",
			 * ti.height, ti.width);
			 */
			break;
                case 'k':
                        ti.corner = TRUE;
                        strcpy(ti.corner_spec, optarg); /* just for print_hdr() */
                        sscanf(optarg, "%d%c%d", &ti.height, &sep, &ti.width);
                        if (ti.height < 1 || ti.height > MAXHEIGHT ||
                            ti.width < 1 || ti.width > MAXWIDTH ||
                            (sep != 'x' && sep != 'X') ||
                            (ti.height * ti.width != numnodes())) {
                                if (node == 0) {
                                        fprintf(stderr, "COMTEST - NODE %d : ERROR -"
                                                " *** INCORRECT "
                                                "RECTANGULAR MESH SPECIFICATION\n",
                                                mynode());
                                        exit(1);
                                }
                        }

                        /*
                         * fprintf(stderr," DEBUG : h = %d w = %d\n",
                         * ti.height, ti.width);
                         */
                        break;
		}
	}

	/* Check for any remaining args */
	if (optind < argc)
		strcpy(ti.part_name, argv[optind++]);
	/* check whether ti.numlens and ti.msgcnt form an acceptable	 */
	/* combination. The messages below is dependent on the actual 	 */
	/* message lengths, ti.msglen[i].				 */
	if (ti.numlens >= 1 && ti.numlens <= 17 && ti.msgcnt > MAXCNT) {
		if (node == 0)
			fprintf(stderr, "COMTEST - NODE %d : ERROR - EXCEEDED"
				" LIMITS ON MSGCNT : "
				"FOR MESSAGE LENGTHS UP TO 256 KBYTES, "
				"msgcnt <= %d\n", mynode(), MAXCNT);
		exit(1);
	}
	if (ti.numlens == 18 && ti.msgcnt > MAXCNT / 2) {
		if (node == 0)
			fprintf(stderr, "COMTEST - NODE %d : ERROR - "
				"EXCEEDED LIMITS ON MSGCNT : "
				"FOR A MESSAGE LENGTH OF 512 KBYTES, "
				"msgcnt <=  %d\n", mynode(), MAXCNT / 2);
		exit(1);
	}
	if (ti.numlens == 19 && ti.msgcnt > MAXCNT / 4) {
		if (node == 0)
			fprintf(stderr, "COMTEST - NODE %d : ERROR - "
				"EXCEEDED LIMITS ON MSGCNT : "
				"FOR A MESSAGE LENGTH OF 1 MBYTE, "
				"msgcnt <=  %d\n", mynode(), MAXCNT / 4);
		exit(1);
	}
	if (ti.numlens == 20 && ti.msgcnt > MAXCNT / 8) {
                if (node == 0)
                        fprintf(stderr, "COMTEST - NODE %d : ERROR - "
                                "EXCEEDED LIMITS ON MSGCNT : "
                                "FOR A MESSAGE LENGTH OF 2 MBYTES, "
                                "msgcnt <=  %d\n", mynode(), MAXCNT / 8);
                exit(1);
        }
}


/******

  FUNCTION NAME: print_hdr

  PURPOSE: prints header
*/

print_hdr(c)
	char            c;
{
	if (node == 0 && !ti.help &&
	    ((c == 'a' && *ti.type_spec != 'n') ||
	     c == 'b' || c == 'k' || c == 'r' || c == 'o')) {
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
		printf("   C O M T E S T   P A R A M E T E R S\n");
		printf("\n");
		printf("      Number of Nodes: %d\n", ti.nodes);

		switch (c) {
		case 'a':
			printf("      Test Type Specification: %s\n", ti.type_spec);
			break;
		case 'b':
			printf("      Test Type Specification: b %s\n", ti.bisect_spec);
			break;
		case 'k':
                        printf("      Test Type Specification: b %s\n", ti.corner_spec);
		case 'r':
			printf("      Test Type Specification: r %s\n", ti.rand_spec);
			break;
		case 'o':
			printf("      Test Type Specification: o\n");
			break;	/* we may add some more */
		}

		printf("      Number of Iters: %d\n", ti.numiter);
		printf("      Message Type: %s\n", ti.force ? "Forced" : "Unforced");
		printf("      Send/Recv Calls: %s\n", ti.underscore ?
		       "No Error Checking" : "Error Checking");
		printf("\n");
		printf("-----------------------------------------------------------\n");
		printf("\n");
	}
}

/******

  FUNCTION NAME: dcompare

  PURPOSE: compares two doubles
*/

dcompare(a, b)
	double         *a, *b;
{
	double          val;

	val = *a - *b;
	return (val == 0.0 ? 0 : (val > 0.0 ? 1 : -1));
}

/******

  FUNCTION NAME: getstats

  PURPOSE: Computes communication statistics
*/

void
getstats(x, n, s)
	double         *x;
	int             n;
	struct STAT_INFO *s;
{

	double          total, totalsq;
	int             i;

	/* Compute min, max, mean, std deviation */

	s->min = 1e9;
	s->max = 0.0;
	total = 0.0;
	totalsq = 0.0;

	for (i = 0; i < n; i++) {
		total = total + x[i];
		totalsq = totalsq + x[i] * x[i];
		if (x[i] < s->min)
			s->min = x[i];
		if (x[i] > s->max)
			s->max = x[i];
	}

	s->mean = total / (double) n;
	s->stddev = sqrt(totalsq / (double) n - s->mean * s->mean);

	/* Compute median.  First sort x. */
	qsort(x, n, 8, dcompare);
	s->median = x[n / 2 - 1];
}

/******

  FUNCTION NAME: prtstats

  PURPOSE: Prints communication statistics
*/

void
prtstats(s)
	struct STAT_INFO *s;
{
	printf("\n");
	printf("Stats for Standard Latency, alpha (microseconds):\n");
	printf("\n");
	printf("Maximum = %d\n", (int) (s->max * 1e6));
	printf("Minimum = %d\n", (int) (s->min * 1e6));
	printf("Mean    = %d\n", (int) (s->mean * 1e6));
	printf("Median  = %d\n", (int) (s->median * 1e6));
	printf("Std_dev = %d\n", (int) (s->stddev * 1e6));
}
