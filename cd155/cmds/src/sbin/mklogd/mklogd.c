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
 * mklogd: Micro-Kernel logger daemon
 *
 * Designed to run ONLY from the boot node to collect remote MK's (Micro-kernel)
 * printf()'s. Idea is to catch those nasty surprises when kernels die with ECC
 * panic()'s or SCSI devices (e.g., tape) complain when the device does not
 * have a media present. The offending kernel must sent a message and hence
 * must not be at interrupt level.
 *
 * Just before 'syslogd' starts 'mklogd' is started due to the synchronizing
 * nature of fifos. 'mklogd' will send remote kernel output to a fifo which
 * 'syslogd' thinks is the standard kernel logging device '/dev/klog'. 'mklogd'
 * allocates a Mach IPC port and set this port in all kernels as a norma special
 * port. A side-effect of setting the norma special port 'console logger' is a
 * kernel thread will be created to drain the MK console log buffer. Kernel
 * printf(), assert and panic() output are buffered and then drained by building
 * and sending Mach messages to the norma special 'console logging port'.
 * 'mklogd' receives these messages from all kernels. 'mklogd' will then write
 * these messages to the fifo '/dev/klog' which 'syslogd' is listening on.
 * 'syslogd' then reads fifo data and writes it to the kernel log file
 * 'var/adm/syslog/kern.log'.
 *
 * HISTORY
 * $Log: mklogd.c,v $
 * Revision 1.2  1995/03/24  23:52:22  stans
 *  Set the boot-node console logging 'special port' first so when it is exported
 *  the correct DIPC UID will be set; otherwise the DIPC subsystem will assign
 *  the incorrect UID.
 *
 *  Reviewer:lenb
 *  Risk:low
 *  Benefit or PTS #:12734
 *  Testing:
 *         Installed & booted kernel on systems: 'a5', XPS150 and testjig.
 *         WW10 sats
 *         Developer tests:
 *                 Verified console logging works for kernel printf()'s at
 *                 interrupt and non-interrupt levels.
 *
 * Revision 1.1  1995/02/06  17:35:41  stans
 * Initial revision
 *
 */

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/mode.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>

#include <mach.h>
#include <mach/boolean.h>
#include <mach/norma_special_ports.h>
#include <mach/time_value.h>
#include <mach/mach_host.h>

#include <i860paragon/conslog.h>

#define	DEBUG 0
#define dprintf if ( Debug ) printf
#define Vprintf if ( Verbose ) printf

#define BITS_PER_INT 32	/* node bitvector operations */

/*
 * Until norma special conslog items get defined in mach/norma_special_ports.h
 */
#ifndef	NORMA_CONSOLE_LOG_PORT
#define NORMA_CONSOLE_LOG_PORT  (2 + MAX_SPECIAL_KERNEL_ID)

#define norma_get_conslog_port(host, node, port)        \
        (norma_get_special_port((host), (node), NORMA_CONSOLE_LOG_PORT, (port)))

#define norma_set_conslog_port(host, port)      \
        (norma_set_special_port((host), NORMA_CONSOLE_LOG_PORT, (port)))
#endif	/* NORMA_CONSOLE_LOG_PORT */

typedef	long node_num_t;
typedef long node_status_t[128];	/* 4096 node bitvector */

node_status_t	alive_nodes;
char		*pgmname;
char		*howto =" useage: mklogd {-d {node(s)}}";
char		*syslogd_fifo_name = "/dev/klog";
char		*PidFile = "/var/run/mklogd.pid";
char		*sysConsole = "/dev/console";
node_num_t	boot_node, myNode;
kern_return_t	kr;
task_t		mytask;
host_t		my_host_port, remote_host_port;
host_priv_t	my_host_priv_port;

mach_msg_timeout_t      send_timeout=2000;      /* in milliseconds */

/*
 * bit flags for mklogd operation.
 */
unsigned	mklogd_flags;

#define	MKL_DO_NOT_DUMP_1ST_MSG	(1<<0)

#define MKL_FLAG_SET(y) ((mklogd_flags) & (y))
#define MKL_FLAG_CLEAR(y) (!((mklogd_flags) & (y)))

#if DEBUG
cons_msg	send_msg;
char		*aString = "I think, therefore we have some problems\n";
#endif

boolean_t	Verbose=FALSE,
		Debug=FALSE,
		exe_bootNode_only=TRUE;

extern int	errno;

/*
 * Iterate over the nodes which are online, calling the supplied function.
 *
 * inputs:
 *	local_host_priv_port	my local host privledged port
 *	statusbits		bitvector of nodes which are online.
 *	funct			a function pointer to call for each online node.
 *	aPort			a Mach port argument pass to funct.
 * outputs:
 *	a 'kern_return_t'
 */

do_nodes( local_host_priv_port, statusbits, funct, aPort )
        host_priv_t     local_host_priv_port;
        node_status_t   statusbits;
        kern_return_t   (*funct)();
	mach_port_t	aPort;
{
        register unsigned	kr, j, sb, mask;

        for(kr=0; kr < sizeof(node_status_t)/sizeof(unsigned); kr++) {
                sb = statusbits[kr];
                for(j=0,mask=1; j < 32; j++) {
                        if ( sb & mask ) {
                                register node_num_t target_node;

                                target_node = j+(kr*32);

                                funct(local_host_priv_port,target_node,aPort);
                        }
                        mask = mask << 1;
                }
        }
}

/*
 * set the specified port as the norma 'special' port for MK (Micro-Kernel)
 * logging.
 *
 * inputs:
 *	local_host_priv_port	my host 'priv' port.
 *	target_node		node where we will set the norma special port.
 *	logPort			MK log port.
 *
 * outputs:
 *	kern_return_t code.
 */

set_mk_log_port(local_host_priv_port,  target_node, logPort)
	host_priv_t	local_host_priv_port;
	node_num_t	target_node;
	mach_port_t	logPort;
{
	kern_return_t	kr;
	host_priv_t	remote_host_priv_port;

	dprintf("set_mk_log_port( node %d )\n",target_node);
	/*
	 * acquire the 'priviledged' host port for the specified node.
	 */
	kr = norma_get_host_priv_port(	local_host_priv_port,
					target_node,
					&remote_host_priv_port);
	if ( kr != KERN_SUCCESS ) {
		mach_error("  norma_get_host_priv_port()",kr);
		return kr;
	}

	/*
	 * set the norma 'special' MK log port.
	 */
	kr = norma_set_conslog_port( remote_host_priv_port, logPort );
	if ( kr != KERN_SUCCESS ) {
		mach_error("norma_set_conslog_port()",kr);
	}

	mach_port_deallocate(mach_task_self(), remote_host_priv_port);

	return kr;
}

/*
 * receive a message
 */
mach_msg_return_t
recv_log_message(
	mach_port_t		port,
	mach_msg_header_t	*rmsg,
	mach_msg_size_t		size)
{
	mach_msg_return_t	kr;

        bzero(rmsg, size);

        rmsg->msgh_size = size;
        rmsg->msgh_local_port = port;

	kr = mach_msg(	rmsg,
			MACH_RCV_MSG + MACH_RCV_INTERRUPT,
			0,
			size,
			port,
			MACH_MSG_TIMEOUT_NONE,
			MACH_PORT_NULL);
	return	kr;
}

/*
 * format output so syslogd can digest it. Seems syslogd forgets all chars in
 * a string after an embedded '\n' or '\r'. Break up the input string into
 * multiple syslog() writes.
 *
 * inputs:
 *	node	my node number.
 *	s	inputs string pointer
 * outputs:
 *	none.
 */

syslog_it( int node, char *s, int size, int fifo )
{
	register char	*cp, *lp;
	char		buf[CONS_DATA_MAX_SIZE];

	for(lp=cp=s; ; cp++ ) {
		if ( *cp == '\r' ) {	/* toss <cr> */
			lp = cp+1;
			continue;
		}
		if ( *cp == '\0' ) {	/* EOS (End-Of-String) */
			/* if no chars then skip output */
			if ( strlen(lp) == 0 )
				return;
			/* build a syslogd string prefixed with a syslogd
			 * priority mask for non-boot_nodes plus the node
			 * number.
			 */
			if ( node == boot_node )
				sprintf(buf,"%d] %s\n",node,lp);
			else {
				sprintf(buf,"<%d>%d] %s\n",(LOG_KERN|LOG_CRIT),
								node, lp);
			}
			size = strlen(buf);
			Vprintf("%s",buf);
			if ( fifo >= 0 ) { /* are we talking to syslog? */
				/* write to fifo syslogd is listening on */
				if ( write(fifo, buf, size) != size ) {
					perror("mklogd: fifo write() failed");
					fifo = -1;	/* switch to syslog() */
					fprintf(stderr,
						"%s continuing with syslog()\n",
						pgmname);
					syslog(LOG_ALERT,"%s",buf);
				}
			}
			else	/* use standard syslog daemon */
				syslog(LOG_ALERT,"%s",buf);
			return;
		}

		/*
		 * output current string up to <newline> as syslogd will drop
		 * all chars after the embedded <newline>.
		 */
		if ( *cp == '\n' ) {
			*cp = '\0';	/* make a sub-string and new copy */
			/*
			 * build a syslog message. Boot node console messages
			 * have already been output to the console while
			 * non-boot_node messages have yet to be output to
			 * the boot_node console. 'syslogd' looks for '<pri>'
			 * string to set the syslogd priority. Prefix this
			 * priority to each non-boot_node string.
			 */
			if ( node == boot_node )
				sprintf(buf,"%d] %s\n", node, lp);
			else {
				sprintf(buf,"<%d>%d] %s\n",(LOG_KERN|LOG_CRIT),
								node, lp);
			}
			size = strlen(buf);
			Vprintf("%s",buf);
			if ( fifo >= 0 ) {
				if ( write(fifo, buf, size) != size ) {
					perror("mklogd: fifo write() failed");
					fifo = -1;	/* switch to syslog() */
					fprintf(stderr,
						"%s continuing with syslog()\n",
						pgmname);
					syslog(LOG_ALERT,"%s",buf);
				}
			}
			else
				syslog(LOG_ALERT,"%s",buf);
			lp = (cp+1);	/* advance past <newline> */
		}
	}
}

/*
 * Set the node_number'th bit in the specified bitvector. 
 *
 * inputs:
 *	node	node number
 *	sb	node number array of bits.
 * outputs:
 *	none.
 * side effects:
 *	bit set in bitvector.
 */

mark_node_online(node,sb)
        int     node;
        node_status_t   sb;
{
        sb[(node/BITS_PER_INT)] |= (1 << (node % BITS_PER_INT));
}

mark_node_offline(node,sb)
        int     node;
        node_status_t   sb;
{
        sb[(node/BITS_PER_INT)] &= ~(1 << (node % BITS_PER_INT));
}

/*
 * is this node talking to us?
 */

boolean_t
is_node_talking( int nodeNum, node_status_t talking_nodes )
{
	register unsigned       bits, rc;
	register unsigned       *status;

	bits = (1 << (nodeNum % BITS_PER_INT));
	status = (unsigned *)&talking_nodes[ ( nodeNum / BITS_PER_INT) ];
	return (boolean_t)(*status & bits);	/* return nodeNum bit */
}

/*
 * mark node as talking
 */

set_node_talking( int nodeNum, node_status_t talking_nodes )
{
	register unsigned       bits, rc;
	register unsigned       *status;

	bits = (1 << (nodeNum % BITS_PER_INT));
	status = (unsigned *)&talking_nodes[ ( nodeNum / BITS_PER_INT) ];
	*status |= bits;	/* set the nodeNum bit */
}


/*
 * Receive Mach messages from all Micro-Kernels running Mach. The messages
 * contain the output from MK printf()'s; nice items to see. Here we retrieve
 * the messages and deliver them to syslogd for safe keeping.
 *
 * inputs:
 *	logPort		Mach port which micro-kernels will send printf() data.
 *	syslogd_fifo	file-descriptor for the syslogd fifo. 'syslogd' thinks
 *			 it has open the kernel device '/dev/klog' and is
 *			 reading local kernel printf() strings.
 * outputs:
 *	none.
 */

mk_console_logger(	mach_port_t	logPort,
			int		syslogd_fifo,
			node_status_t	alive_nodes )
{
	mach_msg_return_t	mm;
	cons_msg_t		rmsg;
	char			*cp;
	int			errMax = 5;
	node_status_t		talking_nodes;
	extern char		*malloc();

	bzero( talking_nodes, sizeof(node_status_t) );

	if ( (cp=malloc(sizeof(cons_msg)+16)) == (char *)0 ) {
		fprintf(stderr,"%s: recv buf malloc() failed.\n",pgmname);
		exit( errno );
	}

	rmsg = (cons_msg_t)((unsigned)(cp+15) & ~15);	/* cache line align */

	for(;;) {
		mm = recv_log_message(	logPort,
					(mach_msg_header_t *)rmsg,
					(mach_msg_size_t)sizeof(cons_msg) );

		if ( mm != MACH_MSG_SUCCESS ) {
			/* process the error */
			mach_error("mach_msg(receive)",mm);
			if ( --errMax <= 0 )
				exit( 1 );
			continue;
		}

		if ( MKL_FLAG_CLEAR( MKL_DO_NOT_DUMP_1ST_MSG ) ) {
			if ( !is_node_talking(rmsg->nodeNum, talking_nodes) ) {
				set_node_talking(rmsg->nodeNum, talking_nodes);
				continue;
			}
		}

		/*
		 * process the message
		 */
		{
			mach_msg_type_t	*mt;

			mt = (mach_msg_type_t *)&rmsg->data_desc;

			/* terminate the string: EOS */
			rmsg->body[ mt->msgt_number ] = '\0';
			syslog_it( rmsg->nodeNum,
					rmsg->body,
					mt->msgt_number,
					syslogd_fifo );
		}
	}
}

#if DEBUG

build_msg(mach_port_t	dest_port,
	cons_msg_t	smsg,
	char		*string)
{
	kern_return_t		kr;
	mach_msg_type_t		*mt, *mt_start;
	int			string_size;

	/*
	 * build a message
	 */
        bzero(smsg, sizeof(cons_msg));

	smsg->hdr.msgh_remote_port = dest_port;
	smsg->hdr.msgh_local_port = MACH_PORT_NULL;

        smsg->hdr.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND,0);

        mt_start = mt = (mach_msg_type_t *)&smsg->node_desc;

	string_size = strlen(string)+1;

	/*
	 * inline data: node & message
	 */
	mt->msgt_inline = 1;
	mt->msgt_name = MACH_MSG_TYPE_INTEGER_32;
	mt->msgt_size = 32;	/* bits/datum */
	mt->msgt_number = 1;	/* # of ints */
	norma_node_self( mach_task_self(), &smsg->nodeNum );
	mt += 2;	/* skip over descriptor & node num to mesg desc */

	mt->msgt_inline = 1;
	mt->msgt_name = MACH_MSG_TYPE_STRING_C;
	mt->msgt_size = 8;	/* bits/datum */
	mt->msgt_number = string_size;	/* # of chars */
	mt++;		/* skip over descriptor, to data */

	/* copy data to inline */
	bcopy( string, mt, string_size );

	/* skip over data to next descriptor */
	mt += (string_size/sizeof(mach_msg_type_t)) + 1;

        /*
         * set correct size in Mach msg header
         *
         * sizeof( Mesg header + body)
         */
        smsg->hdr.msgh_size = sizeof(mach_msg_header_t) +
			((mt - mt_start) * sizeof(mach_msg_type_t));

printf("build_msg: dst 0x%x Mach msg size %d strlen %d desc-sz %d hdr-sz %d\nmsg '%s'\n",dest_port,
	smsg->hdr.msgh_size,string_size,sizeof(mach_msg_type_t),
	sizeof(mach_msg_header_t),string);
}

kern_return_t
send_message( cons_msg_t msg )
{
	kern_return_t		kr;
	unsigned		sendsize;
	mach_msg_option_t	options;

	options = MACH_SEND_MSG+MACH_SEND_INTERRUPT;

	sendsize = msg->hdr.msgh_size;

        if ( Verbose ) {
                printf("send size %d(%d), body size %d\n",
			msg->hdr.msgh_size,
			sendsize,
                        (msg->hdr.msgh_size - sizeof(mach_msg_header_t)));
                fflush(stdout);
        }

	kr = mach_msg( (mach_msg_header_t *)msg, options, sendsize, 0,
			MACH_PORT_NULL, send_timeout, MACH_PORT_NULL);

	if ( kr != MACH_MSG_SUCCESS ) {
		mach_error("mach_msg(SEND)",kr);
		exit( 1 );
	}

	return	kr;
}

#endif	/* DEBUG */

main(argc, argv)
	int	argc;
	char	*argv[];
{
	node_num_t	target;
	mach_port_t	logPort;
	int		syslogd_fifo = -1;

	/* remember who I am */
	if ( pgmname = rindex(argv[0], '/'))
		pgmname++;
	else
		pgmname = argv[0];

	argc--; argv++;	/* skip over pgm name */
	while ( (argc > 0) && (argv[0][0] == '-')) {
		switch( argv[0][1] ) {
		  case 'l':
			exe_bootNode_only = FALSE;
			break;
		  case 'h':
			fprintf(stderr,"%s: %s\n",pgmname, howto);
			exit(0);
			break;
		  case 'v':
			Verbose = TRUE;
			break;
		  case 'd':
			Debug = TRUE;
			break;
		  default:
			fprintf(stderr,"%s: bad switch %s, ignored.\n",
					pgmname,argv[0]);
			break;
		}
		argv++,argc--;
	}

	/*
	 * Did we tell the micro-kernel NOT to do console logging?
	 */
	if ( getbootint("CONSLOG",TRUE) == FALSE ) {
		/*
		 * skip console logging. Make sure the fifo '/dev/klog' is
		 * removed as syslogd will hang due to the rendevous nature
		 * of fifos.
		 */
		unlink(syslogd_fifo_name);
		fprintf(stderr,"%s: no remote MK console logging, exit\n",
			pgmname);
		exit( 0 );
	}

	/*
	 * if not debugging then become a daemon process. reopen /dev/console.
	 */
        if ( Debug == FALSE ) {
		int	fd;

		daemon(0,0);    /* start our own session */
		if ( (fd = open(sysConsole, O_WRONLY)) < 0) {
			perror("open(/dev/console)");
		}
		/* reactivate stdout & stderr as /dev/console */
		if ( fd != 1 )
			dup2(fd, 1);
		if ( fd != 2 )
			dup2(fd, 2);
	}

	/*
	 * get any bootmagic flags
	 */
	mklogd_flags = (unsigned)getbootint("MKLOGD_FLAGS",0);

	/*
	 * determine 'MY' task & host-priv ports.
	 */
	mytask = mach_task_self();
	my_host_priv_port = task_by_pid( -1 );
	my_host_port = mach_host_self();

	/*
	 * determine current node number (where we are currently executing).
	 */
	kr = norma_node_self(mytask, &myNode );
	if ( kr != KERN_SUCCESS ) {
		mach_error("norma_node_self()",kr);
		exit( EINVAL );
	}

	/*
	 * find our boot node assuming the root filesystem device is always
	 * on the boot node. We must execute from the boot node as that's where
	 * '/dev/console' is attached.
	 */
	if ( (boot_node = getbootint("ROOT_DEVICE_NODE", -1)) == -1 ) {
		fprintf(stderr,"%s: can't find ROOT_DEVICE_NODE?\n",pgmname);
		exit( EINVAL );
	}

	if ( exe_bootNode_only && (boot_node != myNode) ) {
		fprintf(stderr,"%s: must run on boot node\n",pgmname);
		exit( EINVAL );
	}

	/*
	 * open a connection to syslogd a.k.a. /var/adm/syslog/daemon.log
	 */
	if ( openlog("mklogd",LOG_CONS+LOG_NOWAIT,LOG_DAEMON) < 0 ) {
		perror("openlog()");
		exit( errno );
	}

	/*
	 * allocate a Mach IPC port for MK console logging.
	 */
	kr = mach_port_allocate( mytask, MACH_PORT_RIGHT_RECEIVE, &logPort );

	if ( kr != KERN_SUCCESS ) {
		mach_error("mach_port_allocate()",kr);
		exit( 1 );
	}

	/*
	 * insert a send right - required for the set special port calls.
	 */
	kr = mach_port_insert_right(mytask, logPort, logPort,
						MACH_MSG_TYPE_MAKE_SEND);
	if ( kr != KERN_SUCCESS ) {
		mach_error("mach_port_insert_right()",kr);
		exit( 1 );
	}

	{
		int	x, y, numNodes;

		/*
		 * set a reasonable port queue limit based on the total
		 * number of nodes.
		 */

		x = getbootint("BOOT_MESH_X",45);
		y = getbootint("BOOT_MESH_Y",45);
		if ( (numNodes = x * y) >= MACH_PORT_QLIMIT_MAX )
			numNodes = MACH_PORT_QLIMIT_MAX;
		kr = mach_port_set_qlimit( mytask, logPort, numNodes);
		if ( kr != KERN_SUCCESS ) {
			mach_error("mach_port_set_qlimit()", kr);
			exit(1);
		}
	}

        /* tuck my process id away so '/sbin/init.d/syslogd' shell script can
	 * find my pid to kill me.
	 */
	{
		FILE *fp;

		fp = fopen(PidFile, "w");
		if (fp != NULL) {
			fprintf(fp, "%d\n", getpid());
			(void) fclose(fp);
		}
		else {
			char	buf[128];

			sprintf(buf,"%s: unable to open pid file '%s'",
				pgmname,PidFile);
			perror(buf);
		}
	}

	/*
	 * open the fifo which syslogd will be listening on for kernel
	 * printf()'s
	 * Warning: we hang in the open until the syslogd actually opens the
	 * fifo by virture of the rendevou nature of fifos.
	 */
        if ( (syslogd_fifo=open(syslogd_fifo_name,O_WRONLY,0)) < 0 ) {
                perror("fifo open() failed, continuing with syslog() calls.");
                syslogd_fifo = -1;
        }

	/*
	 * build/retrieve a bitVector list of nodes to operate on
	 */
	if ( argc == 0 ) { /* do ALL online Mach nodes */
		/* get the bitvector list of nodes where Mach is running.  */
       		kr = norma_node_get_status(my_host_port,alive_nodes);
       		if ( kr != KERN_SUCCESS ) {
               		mach_error("norma_node_get_status()",kr);
               		exit( EINVAL );
       		}
       	}
	else {
		/* Debug:
		 * retrieve node numbers from the command line.
		 */
		bzero( alive_nodes, sizeof(alive_nodes) );
		for(; argc > 0; argc--,argv++) {
			target = atoi(argv[0]);
			mark_node_online( target, alive_nodes );
		}
	}

	/*
	 * Must do our boot-node first so the correct DIPC UID is set.
	 * If we do NOT, then the exported UID will not be the special id.
	 */
	set_mk_log_port( my_host_priv_port, boot_node, logPort );
	mark_node_offline( boot_node, alive_nodes );	/* port already set */

	/*
	 * set the norma special MK logging port for all MK's in the list.
	 */
	do_nodes( my_host_priv_port, alive_nodes, set_mk_log_port, logPort );

#if 0	/* DEBUG */
        build_msg( logPort, (cons_msg_t)&send_msg, aString );
        send_message( (cons_msg_t)&send_msg );
#endif
	/*
	 * drain messages from the MK console logger port
	 */
	mk_console_logger( logPort, syslogd_fifo, alive_nodes );
}
