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
 * @OSF_FREE_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: ms.c,v $
 * Revision 1.4  1995/03/29  21:53:14  scotth
 *  Reviewer: carolr
 *  Risk: low
 *  Benefit or PTS #: 12816
 *  Testing: developer testing
 *  Module(s): cmds_libs/src/usr/mach/bin/ms/ms.c
 *
 * Revision 1.3  1994/11/19  03:01:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/10/18  21:13:50  stans
 *    Fixed usage string.
 *
 * Revision 1.1  1993/05/04  02:09:02  stans
 * Initial revision
 *
 * Revision 1.3  1992/06/15  15:26:02  devrcs
 * 	Source changes for NORMA_MK9...
 * 	[1992/06/11  08:05:38  david]
 *
 * Revision 2.6  91/03/27  17:27:27  mrt
 * 	Changed mach.h include
 * 
 * Revision 2.5  91/03/19  12:31:58  mrt
 * 	Changed to new copyright
 * 
 * Revision 2.4  91/03/10  17:44:13  rpd
 * 	Added average entry size to the IPC statistics.
 * 	[91/03/10            rpd]
 * 
 * Revision 2.3  91/03/10  13:41:23  rpd
 * 	Added -v.
 * 	[91/01/14            rpd]
 * 
 * Revision 2.2  90/09/12  16:32:48  rpd
 * 	Use thread_get_assignment, task_get_assignment,
 * 	and processor_get_assignment.
 * 	[90/08/30            rpd]
 * 
 * 	Added table/tree summaries to list_ipc.
 * 	[90/08/28            rpd]
 * 
 * 	Created.
 * 	[90/06/18            rpd]
 * 
 * $EndLog$
 */

#include <stdio.h>
#include <strings.h>
#include <mach.h>
#include <mach_error.h>
#include <servers/netname.h>
#include <servers/machid.h>
#include <servers/machid_debug.h>
#include <mach/norma_special_ports.h>

extern char *malloc();

#define streql(a, b)	(strcmp((a), (b)) == 0)

static mach_port_t server;
static mach_port_t auth;
int my_node;


static int all_things();

static int list_tasks(), list_threads(), list_processor_sets();
static int list_processors(), list_hosts();

static int list_tt(), list_ipc(), list_task(), list_thread();
static int list_processor(), list_processor_set(), list_host();

static unsigned int zone_count();
static void ipc_sizes(), old_ipc_sizes();

struct ipc_info {
    mach_port_rights_t total;
    mach_port_rights_t send;
    mach_port_rights_t receive;
    mach_port_rights_t sendreceive;
    mach_port_rights_t sendonce;
    mach_port_rights_t portset;
    mach_port_rights_t deadname;
    mach_port_rights_t compat;
    mach_port_rights_t marequest;
    mach_port_rights_t table;
    mach_port_rights_t splay;
    unsigned int dntotal;
    unsigned int dnused;
};

struct ipc_sizes {
    vm_size_t space;
    vm_size_t port;
    vm_size_t pset;
    vm_size_t entry;
    vm_size_t tentry;
    vm_size_t marequest;
    vm_size_t dnrequest;
};

struct old_ipc_sizes {
    vm_size_t task;
    vm_size_t port;
    vm_size_t pset;
    vm_size_t translation;
};

static struct ipc_info ipc_total;

static void
usage()
{
    quit(1, "usage: ms [-host machine] [-v] [-t|-th|-h|-ps|-p|-tt|-ipc] ids...\n");
}

static boolean_t Verbose = FALSE;
int node_count = 0, *node_number;

main(argc, argv)
    int argc;
    char *argv[];
{
    char *hostname = "";
    enum {
	Default, TT, Task, Thread,
	ProcSet, Proc, Host, IPC,
    } info = Default;
    mach_id_t *ids;
    unsigned int numids;
    kern_return_t kr;
    int i;
    mach_port_t host_priv;

    for (i = 1; i < argc; i++)
	if (streql(argv[i], "-host") && (i < argc-1))
	    hostname = argv[++i];
        else if  (streql(argv[i], "-n") && (i < argc-1)) {
	    node_number = (int *) malloc ((argc-i) * sizeof (int));
	    while (i+1 < argc && argv[i+1][0] != '-') {
		    node_number[node_count++] = atoi(argv[++i]);
	    }
        }
	else if (streql(argv[i], "-v"))
	    Verbose = TRUE;
	else if (streql(argv[i], "-ps") && (info == Default))
	    info = ProcSet;
	else if (streql(argv[i], "-p") && (info == Default))
	    info = Proc;
	else if (streql(argv[i], "-h") && (info == Default))
	    info = Host;
	else if (streql(argv[i], "-ipc") && (info == Default))
	    info = IPC;
	else if (streql(argv[i], "-th") && (info == Default))
	    info = Thread;
	else if (streql(argv[i], "-t") && (info == Default))
	    info = Task;
	else if (streql(argv[i], "-tt") && (info == Default))
	    info = TT;
	else if (streql(argv[i], "--")) {
	    i++;
	    break;
	} else if (argv[i][0] == '-')
	    usage();
	else
	    break;

    argv += i;
    argc -= i;

    numids = argc;
    if (numids > 0) {
	ids = (mach_id_t *) malloc(numids * sizeof *ids);

	for (i = 0; i < argc; i++)
	    ids[i] = atoi(argv[i]);
    }

    kr = netname_look_up(name_server_port, hostname, "MachID", &server);
    if (kr != KERN_SUCCESS)
	quit(1, "ms: netname_lookup_up(MachID): %s\n",
	     mach_error_string(kr));

    auth = mach_host_priv_self();
    if (auth == MACH_PORT_NULL)
	auth = mach_task_self();

#if 0
    kr = norma_node_self(auth, &my_node);
    if (kr != KERN_SUCCESS)
	quit(1, "ms: norma_node_self: %s\n",
	     mach_error_string(kr));
    else
      printf("Running on node %x\n", my_node);

    kr = norma_get_host_priv_port(auth, my_node, &host_priv);
    if (kr != KERN_SUCCESS)
	quit(1, "ms: norma_get_host_priv_port: %s\n",
	     mach_error_string(kr));
    auth = host_priv;
#endif

    switch (info) {
      case IPC: {
	int count;
	unsigned int kernel_ports, ports, spaces, translations;
	struct ipc_sizes sizes;
	struct old_ipc_sizes old_sizes;
	mhost_priv_t host_priv;
	vm_size_t new_size, old_size;

	if (Verbose) {
	    if (numids == 0) {
		mhost_t host;

		kr = machid_host_ports(server, auth, &host, &host_priv);
		if (kr != KERN_SUCCESS)
		    quit(1, "ms: machid_host_ports: %s\n",
			 mach_error_string(kr));
	    } else if (numids == 1) {
		mach_type_t type;

		kr = machid_mach_type(server, auth, ids[0], &type);
		if (kr != KERN_SUCCESS)
		    quit(1, "ms: machid_mach_type: %s\n",
			 mach_error_string(kr));

		if (type != MACH_TYPE_HOST_PRIV)
		    usage();
		host_priv = ids[0];
	    } else
		usage();

	    numids = 1;
	    ids = &host_priv;
	}

	printf(" MID   PID Size  Tot  Tbl Tree Send Recv  S/R Snd1 PSet Dead Comp Command\n");
	bzero((char *) &ipc_total, sizeof ipc_total);

	if (numids == 0)
	    count = all_things(list_tasks, list_ipc);
	else
	    count = list_tasks(list_ipc, ids, numids);

	if (Verbose) {
	    printf("\n           Size  Tot  Tbl Tree Send Recv  S/R Snd1 PSet Dead Comp\n");
	    printf("total      %2.1f %4u %4u %4u %4u %4u %4u %4u %4u %4u %4u\n",
		   ((16 * ipc_total.table + 32 * ipc_total.splay) /
		    (double) ipc_total.total),
		   ipc_total.total, ipc_total.table, ipc_total.splay,
		   ipc_total.send, ipc_total.receive, ipc_total.sendreceive,
		   ipc_total.sendonce, ipc_total.portset,
		   ipc_total.deadname, ipc_total.compat);
	    printf("\n");

	    ipc_sizes(host_priv, &sizes);
	    old_ipc_sizes(host_priv, &old_sizes);

	    /* approximate kernel ports */

	    ports = zone_count(host_priv, "ipc ports");
	    kernel_ports = ports - (ipc_total.receive + ipc_total.sendreceive);

	    /* approximate dead-name requests for kernel ports */

	    ipc_total.dnused += ipc_total.compat;
	    ipc_total.dntotal += 2 * ipc_total.compat;

	    /* approximate spaces */

	    spaces = zone_count(host_priv, "ipc spaces");

	    /* approximate translations */

	    translations = ipc_total.total + kernel_ports;

	    new_size = (sizes.space * spaces +
			sizes.port * ports +
			sizes.pset * ipc_total.portset +
			sizes.entry * ipc_total.table +
			sizes.tentry * ipc_total.splay +
			sizes.marequest * ipc_total.marequest +
			sizes.dnrequest * ipc_total.dntotal);
	    old_size = (old_sizes.task * count +
			old_sizes.port * ports +
			old_sizes.pset * ipc_total.portset +
			old_sizes.translation * translations);

	    printf("%4.1f%% table entries used productively\n",
		   (100.0 * (ipc_total.total - ipc_total.splay) /
		    (double) ipc_total.table));
	    printf("%4.1f%% dead-name request slots used productively\n",
		   100.0 * ipc_total.dnused / (double) ipc_total.dntotal);
	    printf("%4.1f%% of old data structures' space consumption\n",
		   100.0 * new_size / (double) old_size);
	    printf("\n");

	    printf("%6d bytes total space consumption\n", new_size);
	    printf("%6d bytes for spaces (%d bytes each)\n",
		   sizes.space * spaces, sizes.space);
	    printf("%6d bytes for ports (%d bytes each)\n",
		   sizes.port * ports, sizes.port);
	    printf("%6d bytes for port sets (%d bytes each)\n",
		   sizes.pset * ipc_total.portset, sizes.pset);
	    printf("%6d bytes for tables (%d bytes per entry)\n",
		   sizes.entry * ipc_total.table, sizes.entry);
	    printf("%6d bytes for splay trees (%d bytes per entry)\n",
		   sizes.tentry * ipc_total.splay, sizes.tentry);
	    printf("%6d bytes for msg-accepted requests (%d bytes each)\n",
		   sizes.marequest * ipc_total.marequest, sizes.marequest);
	    printf("%6d bytes for dead-name requests (%d bytes per entry)\n",
		   sizes.dnrequest * ipc_total.dntotal, sizes.dnrequest);
	    printf("\n");

	    printf("%6d bytes w/ old data structures\n", old_size);
	    printf("%6d bytes for tasks (%d bytes each)\n",
		   old_sizes.task * count, old_sizes.task);
	    printf("%6d bytes for ports (%d bytes each)\n",
		   old_sizes.port * ports, old_sizes.port);
	    printf("%6d bytes for port sets (%d bytes each)\n",
		   old_sizes.pset * ipc_total.portset, old_sizes.pset);
	    printf("%6d bytes for translations (%d bytes each)\n",
		   old_sizes.translation * translations,
		   old_sizes.translation);
	}
	break;
      }

      case Default:
      case TT:
	printf(" MID   PID  Node  VMem   RSS  MID STAT %%CPU Pri      User    System Command\n");

	if (numids == 0)
	    (void) all_things(list_tasks, list_tt);
	else
	    (void) list_tasks(list_tt, ids, numids);
	break;

      case Task:
	printf(" MID   PID Node Thds Sus Pri  VMem   RSS PSet      User    System Command\n");

	if (numids == 0)
	    (void) all_things(list_tasks, list_task);
	else
	    (void) list_tasks(list_task, ids, numids);
	break;

      case Thread:
	printf(" MID Node STAT %%CPU  Sleep Cur Pri Max Dep Policy PSet      User    System\n");

	if (numids == 0)
	    (void) all_things(list_threads, list_thread);
	else
	    (void) list_threads(list_thread, ids, numids);
	break;

      case ProcSet:
	printf(" MID Name Node Procs Tasks Threads LoadAve MachFactor MaxPri Policies\n");

	if (numids == 0)
	    (void) all_things(list_processor_sets, list_processor_set);
	else
	    (void) list_processor_sets(list_processor_set, ids, numids);
	break;

      case Proc:
	if (numids == 0)
	    (void) all_things(list_processors, list_processor);
	else
	    (void) list_processors(list_processor, ids, numids);
	break;

      case Host:
	if (numids == 0)
	    (void) all_things(list_hosts, list_host);
	else
	    (void) list_hosts(list_host, ids, numids);
	break;
    }

    exit(0);
}

#define sec_to_minutes(t)       ((t) / 60)
#define sec_to_seconds(t)       ((t) % 60)
#define usec_to_100ths(t)       ((t) / 10000)

static void
digits(buffer, number)
    char *buffer;
    double number;
{
    if (number < 10.0)
	sprintf(buffer, "%4.2f", number);
    else if (number < 100.0)
	sprintf(buffer, "%4.1f", number);
    else
	sprintf(buffer, "%4.0f", number);
}

static void
mem_to_string(buffer, size)
    char *buffer;
    vm_size_t size;
{
    char digitsbuf[100];

    if (size > 1024*1024*1024) {
	digits(digitsbuf, size/(1024.0*1024.0*1024.0));
	sprintf(buffer, "%sG", digitsbuf);
    } else if (size > 1024*1024) {
	digits(digitsbuf, size/(1024.0*1024.0));
	sprintf(buffer, "%sM", digitsbuf);
    } else
	sprintf(buffer, "%dK", size/1024);
}

static void
seconds_to_string(buffer, seconds)
    char *buffer;
    int seconds;
{
    /* print the duration in six characters */

    if (seconds < 1000*60)
	sprintf(buffer, "%3d:%02d", seconds/60, seconds%60);
    else if (seconds < 10000*60*60)
	sprintf(buffer, "%4dhr", seconds/(60*60));
    else
	sprintf(buffer, "%3dday", seconds/(60*60*24));
}

static void
thstate_to_string(buffer, state, flags, suspend)
    char *buffer;
    int state, flags, suspend;
{
    char *state_str, *flag_str;

    /* print the thread state, preferably in 2-3 characters */

    switch (state) {
      case TH_STATE_RUNNING:
	state_str = "R";
	break;
      case TH_STATE_STOPPED:
	state_str = "S";
	break;
      case TH_STATE_WAITING:
	state_str = "W";
	break;
      case TH_STATE_UNINTERRUPTIBLE:
	state_str = "U";
	break;
      case TH_STATE_HALTED:
	state_str = "H";
	break;
      default:
	state_str = "?";
	break;
    }

    switch (flags) {
      case 0:
	flag_str = " ";
	break;
      case TH_FLAGS_SWAPPED:
	flag_str = "S";
	break;
      case TH_FLAGS_IDLE:
	flag_str = "I";
	break;
      default:
	flag_str = "?";
	break;
    }

    if (suspend == 0)
	sprintf(buffer, "%s%s", state_str, flag_str);
    else
	sprintf(buffer, "%s%s%d", state_str, flag_str, suspend);
}

static void
depress_to_string(buffer, depressed, priority)
    char *buffer;
    boolean_t depressed;
    int priority;
{
    /* print depressed priority info in two characters */

    if (depressed)
	sprintf(buffer, "%2d", priority);
    else
	sprintf(buffer, "no");
}

static void
policy_to_string(buffer, policy, data)
    char *buffer;
    int policy;
    int data;
{
    /* print the scheduling policy in six characters */

    switch (policy) {
      case POLICY_TIMESHARE:
	sprintf(buffer, "tshare");
	break;

      case POLICY_FIXEDPRI:
	sprintf(buffer, "fixpri");
	break;

      default:
	sprintf(buffer, "???");
	break;
    }
}

static void
pid_to_string(buffer, pid)
    char *buffer;
    int pid;
{
    /* print a PID in five characters */

    if (pid == 0)
	sprintf(buffer, " none");
    else
	sprintf(buffer, "%5d", pid);
}

static int
list_processor_set(pset)
    mprocessor_set_t pset;
{
    mprocessor_set_name_t name;
    mhost_t host;
    processor_set_basic_info_data_t basic;
    processor_set_sched_info_data_t sched;
    kern_return_t kr;
    int node;

    kr = machid_mach_lookup(server, auth, pset,
			    MACH_TYPE_PROCESSOR_SET_NAME, &name);
    if (kr != KERN_SUCCESS)
	quit(1, "ms: machid_mach_lookup: %s\n", mach_error_string(kr));

    kr = machid_processor_set_basic_info(server, auth, pset,
					 &host, &basic);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_processor_set_sched_info(server, auth, pset,
					 &host, &sched);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_mach_node(server, auth, pset, &node);
    if (kr != KERN_SUCCESS)
	return 0;

    printf("%4u %4u %4d %4d  %4d  %6d  %6.2f   %7.2f   %4d  ",
	   pset, name, node,
	   basic.processor_count, basic.task_count, basic.thread_count,
	   basic.load_average / (double) LOAD_SCALE,
	   basic.mach_factor / (double) LOAD_SCALE,
	   sched.max_priority);

    if (sched.policies & POLICY_TIMESHARE)
	printf(" tshare");
    if (sched.policies & POLICY_FIXEDPRI)
	printf(" fixpri");
    printf("\n");

    return 1;
}

static int
list_processor(processor)
    mprocessor_t processor;
{
    mhost_t host;
    mprocessor_set_name_t pset;
    processor_basic_info_data_t basic;
    char *cpu_name, *cpu_subname;
    kern_return_t kr;
    int node;

    kr = machid_processor_basic_info(server, auth, processor,
				     &host, &basic);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_processor_get_assignment(server, auth, processor, &pset);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_mach_node(server, auth, processor, &node);
    if (kr != KERN_SUCCESS)
	return 0;

    slot_name(basic.cpu_type, basic.cpu_subtype, &cpu_name, &cpu_subname);

    printf("Processor %u: node %d host %u, pset %u, %s %s, %s running, slot %d, %s master\n",
	   processor, node, host, pset, cpu_name, cpu_subname,
	   basic.running ? "is" : "not", basic.slot_num,
	   basic.is_master ? "is" : "not");
    return 1;
}

static int
list_host(host)
    mhost_priv_t host;
{
    host_basic_info_data_t basic;
    host_sched_info_data_t sched;
    host_load_info_data_t load;
    mprocessor_set_t pset;
    mprocessor_set_name_t pset_name;
    kernel_version_t version;
    char *cpu_name, *cpu_subname, buffer[100];
    kern_return_t kr;
    int node;

    kr = machid_mach_node(server, auth, host, &node);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_host_basic_info(server, auth, host, &basic);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_host_sched_info(server, auth, host, &sched);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_host_load_info(server, auth, host, &load);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_processor_set_default(server, auth, host, &pset_name);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_host_kernel_version(server, auth, host, version);
    if (kr != KERN_SUCCESS)
	return 0;

    /* this might be an unprivileged host port */
    kr = machid_host_processor_set_priv(server, auth, host,
					pset_name, &pset);
    if (kr != KERN_SUCCESS)
	pset = 0;

    mem_to_string(buffer, basic.memory_size);
    slot_name(basic.cpu_type, basic.cpu_subtype, &cpu_name, &cpu_subname);

    printf("Node %d Host %d, default proc. set %d, default proc. set name %d\n",
	   node, host, pset, pset_name);
    printf("%s %s, %d of %d cpus available, %s memory\n",
	   cpu_name, cpu_subname,
	   basic.avail_cpus, basic.max_cpus, buffer);
    printf("%dus minimum timeout, %dus minimum quantum\n",
	   sched.min_timeout, sched.min_quantum);
    printf("Load averages: %5.2f %5.2f %5.2f\n",
	   load.avenrun[0] / (double) LOAD_SCALE,
	   load.avenrun[1] / (double) LOAD_SCALE,
	   load.avenrun[2] / (double) LOAD_SCALE);
    printf("Mach factors:  %5.2f %5.2f %5.2f\n",
	   load.mach_factor[0] / (double) LOAD_SCALE,
	   load.mach_factor[1] / (double) LOAD_SCALE,
	   load.mach_factor[2] / (double) LOAD_SCALE);
    printf("%.*s", sizeof version, version);
    return 1;
}

static int
list_tt(task)
    mtask_t task;
{
    mthread_t *threads;
    unsigned int threadCnt, i;
    task_basic_info_data_t basic;
    unix_pid_t pid;
    char unix_command[1024];
    unsigned int commlen;
    char virtual[100], resident[100], pidbuf[100];
    boolean_t didthread;
    kern_return_t kr;
    int node;

    kr = machid_task_basic_info(server, auth, task, &basic);
    if (kr != KERN_SUCCESS)
	return 0;

    commlen = sizeof unix_command;
    kr = machid_task_unix_info(server, auth, task, &pid,
			       unix_command, &commlen);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_task_threads(server, auth, task, &threads, &threadCnt);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_mach_node(server, auth, task, &node);
    if (kr != KERN_SUCCESS)
	return 0;

    pid_to_string(pidbuf, pid);

    mem_to_string(virtual, basic.virtual_size);
    mem_to_string(resident, basic.resident_size);

    printf("%4u %5s %4d  %5s %5s",
	   task, pidbuf, node, virtual, resident);

    didthread = FALSE;
    for (i = 0; i < threadCnt; i++) {
	mthread_t thread = threads[i];
	thread_basic_info_data_t basic;
	char thstate[100];
	kern_return_t kr;

	kr = machid_thread_basic_info(server, auth, thread, &basic);
	if (kr != KERN_SUCCESS)
	    continue;

	thstate_to_string(thstate, basic.run_state,
			  basic.flags, basic.suspend_count);

	if (didthread)
	    printf("\n                            ");
	printf(" %4u  %-3s %4.1f %3d %3d:%.2d.%.2d %3d:%.2d.%.2d",
	       thread, thstate,
	       100.0 * (basic.cpu_usage / (double)TH_USAGE_SCALE),
	       basic.cur_priority,
	       sec_to_minutes(basic.user_time.seconds),
	       sec_to_seconds(basic.user_time.seconds),
	       usec_to_100ths(basic.user_time.microseconds),
	       sec_to_minutes(basic.system_time.seconds),
	       sec_to_seconds(basic.system_time.seconds),
	       usec_to_100ths(basic.system_time.microseconds));
	if (!didthread)
	    printf(" %.*s", commlen, unix_command);
	didthread = TRUE;
    }
    if (!didthread)
	printf("                                        %.*s",
	       commlen, unix_command);
    printf("\n");

    kr = vm_deallocate(mach_task_self(), (vm_offset_t) threads,
		       (vm_size_t) (threadCnt * sizeof *threads));
    if (kr != KERN_SUCCESS)
	quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));

    return 1;
}

static kern_return_t
task_ipc_info(task, infop)
    mtask_t task;
    struct ipc_info *infop;
{
    struct ipc_info info;
    mach_port_t *names;
    mach_port_type_t *types;
    ipc_info_space_t iis;
    ipc_info_name_t *table;
    ipc_info_tree_name_t *tree;
    unsigned int namesCnt = 0, typesCnt = 0, tableCnt = 0, treeCnt = 0, i;
    kern_return_t kr;

    bzero((char *) &info, sizeof info);

    kr = machid_port_names(server, auth, task,
			   &names, &namesCnt, &types, &typesCnt);
    if (kr != KERN_SUCCESS)
	return kr;

    for (i = 0; i < typesCnt; i++) {
	info.total++;

	if (types[i] & MACH_PORT_TYPE_COMPAT)
	    info.compat++;

	if (types[i] & MACH_PORT_TYPE_MAREQUEST)
	    info.marequest++;

	switch (types[i] & MACH_PORT_TYPE_ALL_RIGHTS) {
	  case MACH_PORT_TYPE_SEND:
	    info.send++;
	    break;
	  case MACH_PORT_TYPE_RECEIVE:
	    info.receive++;
	    break;
	  case MACH_PORT_TYPE_SEND_RECEIVE:
	    info.sendreceive++;
	    break;
	  case MACH_PORT_TYPE_SEND_ONCE:
	    info.sendonce++;
	    break;
	  case MACH_PORT_TYPE_PORT_SET:
	    info.portset++;
	    break;
	  case MACH_PORT_TYPE_DEAD_NAME:
	    info.deadname++;
	    break;
	}

	if (Verbose && (types[i] & MACH_PORT_TYPE_RECEIVE))
	    (void) machid_port_dnrequest_info(server, auth, task, names[i],
					      &info.dntotal, &info.dnused);
    }

    kr = vm_deallocate(mach_task_self(), (vm_offset_t) names,
		       (vm_size_t) (namesCnt * sizeof *names));
    if (kr != KERN_SUCCESS)
	quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));

    kr = vm_deallocate(mach_task_self(), (vm_offset_t) types,
		       (vm_size_t) (typesCnt * sizeof *types));
    if (kr != KERN_SUCCESS)
	quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));

    kr = machid_port_space_info(server, auth, task, &iis,
				&table, &tableCnt, &tree, &treeCnt);

    if (kr == KERN_SUCCESS) {
	kr = vm_deallocate(mach_task_self(), (vm_offset_t) table,
			   (vm_size_t) (tableCnt * sizeof *table));
	if (kr != KERN_SUCCESS)
	    quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));

	kr = vm_deallocate(mach_task_self(), (vm_offset_t) tree,
			   (vm_size_t) (treeCnt * sizeof *tree));
	if (kr != KERN_SUCCESS)
	    quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));

	info.table = iis.iis_table_size;
	info.splay = iis.iis_tree_size;
    }

    *infop = info;
    return KERN_SUCCESS;
}

static int
list_ipc(task)
    mtask_t task;
{
    unix_pid_t pid;
    char unix_command[1024], pidbuf[100];
    unsigned int commlen;
    struct ipc_info info;
    kern_return_t kr;

    kr = task_ipc_info(task, &info);
    if (kr != KERN_SUCCESS)
	return 0;

    commlen = sizeof unix_command;
    kr = machid_task_unix_info(server, auth, task, &pid,
			       unix_command, &commlen);
    if (kr != KERN_SUCCESS)
	return 0;

    pid_to_string(pidbuf, pid);

    printf("%4u %5.5s %2.1f %4u %4u %4u %4u %4u %4u %4u %4u %4u %4u %.*s\n",
	   task, pidbuf,
	   (16 * info.table + 32 * info.splay) / (double) info.total,
	   info.total, info.table, info.splay,
	   info.send, info.receive, info.sendreceive,
	   info.sendonce, info.portset, info.deadname, info.compat,
	   commlen, unix_command);

    ipc_total.total += info.total;
    ipc_total.send += info.send;
    ipc_total.receive += info.receive;
    ipc_total.sendreceive += info.sendreceive;
    ipc_total.sendonce += info.sendonce;
    ipc_total.portset += info.portset;
    ipc_total.deadname += info.deadname;
    ipc_total.compat += info.compat;
    ipc_total.marequest += info.marequest;
    ipc_total.table += info.table;
    ipc_total.splay += info.splay;
    ipc_total.dntotal += info.dntotal;
    ipc_total.dnused += info.dnused;
    return 1;
}

static int
list_task(task)
    mtask_t task;
{
    mprocessor_set_name_t pset;
    mthread_t *threads;
    unsigned int threadCnt;
    task_basic_info_data_t info;
    task_thread_times_info_data_t times;
    unix_pid_t pid;
    char unix_command[1024];
    unsigned int commlen;
    char virtual[100], resident[100], pidbuf[100];
    kern_return_t kr;
    int node;

    kr = machid_task_threads(server, auth, task, &threads, &threadCnt);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = vm_deallocate(mach_task_self(), (vm_offset_t) threads,
		       (vm_size_t) (threadCnt * sizeof *threads));
    if (kr != KERN_SUCCESS)
	quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));

    kr = machid_task_basic_info(server, auth, task, &info);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_mach_node(server, auth, task, &node);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_task_thread_times_info(server, auth, task, &times);
    if (kr != KERN_SUCCESS)
	return 0;

    commlen = sizeof unix_command;
    kr = machid_task_unix_info(server, auth, task, &pid,
			       unix_command, &commlen);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_task_get_assignment(server, auth, task, &pset);
    if (kr != KERN_SUCCESS)
	return 0;

    pid_to_string(pidbuf, pid);

    /* add time for live threads to the accumulated time
       for dead threads */

    time_value_add(&info.system_time, &times.system_time);
    time_value_add(&info.user_time, &times.user_time);

    mem_to_string(virtual, info.virtual_size);
    mem_to_string(resident, info.resident_size);

    printf("%4u %5s %4d %4d %3d %3d %5.5s %5.5s %4u %3d:%.2d.%.2d %3d:%.2d.%.2d %.*s\n",
	   task, pidbuf, node, threadCnt,
	   info.suspend_count, info.base_priority,
	   virtual, resident, pset,
	   sec_to_minutes(info.user_time.seconds),
	   sec_to_seconds(info.user_time.seconds),
	   usec_to_100ths(info.user_time.microseconds),
	   sec_to_minutes(info.system_time.seconds),
	   sec_to_seconds(info.system_time.seconds),
	   usec_to_100ths(info.system_time.microseconds),
	   commlen, unix_command);
    return 1;
}

static int
list_thread(thread)
    mthread_t thread;
{
    mprocessor_set_name_t pset;
    thread_basic_info_data_t basic;
    thread_sched_info_data_t sched;
    char thsleep[100], thstate[100], thdepress[100], thpolicy[100];
    kern_return_t kr;
    int node;

    kr = machid_thread_basic_info(server, auth, thread, &basic);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_thread_sched_info(server, auth, thread, &sched);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_thread_get_assignment(server, auth, thread, &pset);
    if (kr != KERN_SUCCESS)
	return 0;

    kr = machid_mach_node(server, auth, thread, &node);
    if (kr != KERN_SUCCESS)
	return 0;

    seconds_to_string(thsleep, basic.sleep_time);
    thstate_to_string(thstate, basic.run_state,
		      basic.flags, basic.suspend_count);
    depress_to_string(thdepress, sched.depressed, sched.depress_priority);
    policy_to_string(thpolicy, sched.policy, sched.data);

    printf("%4u %4d  %-3s %4.1f %6s %3d %3d %3d %3s %6s %4u %3d:%.2d.%.2d %3d:%.2d.%.2d\n",
	   thread, node, thstate,
	   100.0 * (basic.cpu_usage / (double)TH_USAGE_SCALE),
	   thsleep,
	   sched.cur_priority, sched.base_priority,
	   sched.max_priority, thdepress, thpolicy, pset,
	   sec_to_minutes(basic.user_time.seconds),
	   sec_to_seconds(basic.user_time.seconds),
	   usec_to_100ths(basic.user_time.microseconds),
	   sec_to_minutes(basic.system_time.seconds),
	   sec_to_seconds(basic.system_time.seconds),
	   usec_to_100ths(basic.system_time.microseconds));
    return 1;
}

static int
list_threads(list_thread, ids, count)
    int (*list_thread)();
    mach_id_t *ids;
    unsigned int count;
{
    int total = 0;
    unsigned int i, j;
    kern_return_t kr;

    for (i = 0; i < count; i++) {
	mach_id_t id = ids[i];
	mach_type_t type;
	mthread_t *threads;
	unsigned int threadCnt;

	kr = machid_mach_type(server, auth, id, &type);
	if (kr != KERN_SUCCESS)
	    quit(1, "ms: machid_mach_type: %s\n", mach_error_string(kr));

	switch (type) {
	  case MACH_TYPE_NONE:
	    continue;

	  default:
	    kr = machid_mach_lookup(server, auth, id,
				    MACH_TYPE_THREAD, &id);
	    if (kr != KERN_SUCCESS)
		quit(1, "ms: machid_mach_lookup: %s\n", mach_error_string(kr));

	    if (id == 0)
		goto badtype;
	    /* fall-through */

	  case MACH_TYPE_THREAD:
	    total += (*list_thread)(id);
	    continue;

	  case MACH_TYPE_TASK:
	    kr = machid_task_threads(server, auth, id,
				     &threads, &threadCnt);
	    break;

	  case MACH_TYPE_PROCESSOR_SET_NAME:
	    kr = machid_mach_lookup(server, auth, id,
				    MACH_TYPE_PROCESSOR_SET, &id);
	    if (kr != KERN_SUCCESS)
		quit(1, "ms: machid_mach_lookup: %s\n", mach_error_string(kr));

	    if (id == 0)
		goto badtype;
	    /* fall-through */

	  case MACH_TYPE_PROCESSOR_SET:
	    kr = machid_processor_set_threads(server, auth, id,
					      &threads, &threadCnt);
	    break;

	  case MACH_TYPE_HOST:
	    kr = machid_mach_lookup(server, auth, id,
				    MACH_TYPE_HOST_PRIV, &id);
	    if (kr != KERN_SUCCESS)
		quit(1, "ms: machid_mach_lookup: %s\n", mach_error_string(kr));

	    if (id == 0)
		goto badtype;
	    /* fall-through */

	  case MACH_TYPE_HOST_PRIV:
	    kr = machid_host_threads(server,auth, id,
				     &threads, &threadCnt);
	    break;

	  badtype:
	    fprintf(stderr, "ms: %u has type %s\n",
		    ids[i], mach_type_string(type));
	    continue;
	}
	if (kr != KERN_SUCCESS)
	    continue;

	for (j = 0; j < threadCnt; j++)
	    total += (*list_thread)(threads[j]);

	kr = vm_deallocate(mach_task_self(), (vm_offset_t) threads,
			   (vm_size_t) (threadCnt * sizeof *threads));
	if (kr != KERN_SUCCESS)
	    quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));
    }

    return total;
}

static int
list_tasks(list_task, ids, count)
    int (*list_task)();
    mach_id_t *ids;
    unsigned int count;
{
    int total = 0;
    unsigned int i, j;
    kern_return_t kr;

    for (i = 0; i < count; i++) {
	mach_id_t id = ids[i];
	mach_type_t type;
	mtask_t *tasks;
	unsigned int taskCnt;

	kr = machid_mach_type(server, auth, id, &type);
	if (kr != KERN_SUCCESS)
	    quit(1, "ms: machid_mach_type: %s\n", mach_error_string(kr));

	switch (type) {
	  case MACH_TYPE_NONE:
	    continue;

	  default:
	    kr = machid_mach_lookup(server, auth, id,
				    MACH_TYPE_TASK, &id);
	    if (kr != KERN_SUCCESS)
		quit(1, "ms: machid_mach_lookup: %s\n", mach_error_string(kr));

	    if (id == 0)
		goto badtype;
	    /* fall-through */

	  case MACH_TYPE_TASK:
	    total += (*list_task)(id);
	    continue;

	  case MACH_TYPE_PROCESSOR_SET_NAME:
	    kr = machid_mach_lookup(server, auth, id,
				    MACH_TYPE_PROCESSOR_SET, &id);
	    if (kr != KERN_SUCCESS)
		quit(1, "ms: machid_mach_lookup: %s\n", mach_error_string(kr));

	    if (id == 0)
		goto badtype;
	    /* fall-through */

	  case MACH_TYPE_PROCESSOR_SET:
	    kr = machid_processor_set_tasks(server, auth, id,
					    &tasks, &taskCnt);
	    break;

	  case MACH_TYPE_HOST:
	    kr = machid_mach_lookup(server, auth, id,
				    MACH_TYPE_HOST_PRIV, &id);
	    if (kr != KERN_SUCCESS)
		quit(1, "ms: machid_mach_lookup: %s\n", mach_error_string(kr));

	    if (id == 0)
		goto badtype;
	    /* fall-through */

	  case MACH_TYPE_HOST_PRIV:
	    kr = machid_host_tasks(server, auth, id,
				   &tasks, &taskCnt);
	    break;

	  badtype:
	    fprintf(stderr, "ms: %u has type %s\n",
		    ids[i], mach_type_string(type));
	    continue;
	}
	if (kr != KERN_SUCCESS)
	    continue;

	for (j = 0; j < taskCnt; j++)
	    total += (*list_task)(tasks[j]);

	kr = vm_deallocate(mach_task_self(), (vm_offset_t) tasks,
			   (vm_size_t) (taskCnt * sizeof *tasks));
	if (kr != KERN_SUCCESS)
	    quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));
    }

    return total;
}

static int
list_processors(list_processor, ids, count)
    int (*list_processor)();
    mach_id_t *ids;
    unsigned int count;
{
    int total = 0;
    unsigned int i, j;
    kern_return_t kr;

    for (i = 0; i < count; i++) {
	mach_id_t id = ids[i];
	mach_type_t type;
	mprocessor_t *processors;
	unsigned int processorCnt;

	kr = machid_mach_type(server, auth, id, &type);
	if (kr != KERN_SUCCESS)
	    quit(1, "ms: machid_mach_type: %s\n", mach_error_string(kr));

	switch (type) {
	  case MACH_TYPE_NONE:
	    continue;

	  default:
	    kr = machid_mach_lookup(server, auth, id,
				    MACH_TYPE_PROCESSOR, &id);
	    if (kr != KERN_SUCCESS)
		quit(1, "ms: machid_mach_lookup: %s\n", mach_error_string(kr));

	    if (id == 0)
		goto badtype;
	    /* fall-through */

	  case MACH_TYPE_PROCESSOR:
	    total += (*list_processor)(id);
	    continue;

	  case MACH_TYPE_HOST:
	    kr = machid_mach_lookup(server, auth, id,
				    MACH_TYPE_HOST_PRIV, &id);
	    if (kr != KERN_SUCCESS)
		quit(1, "ms: machid_mach_lookup: %s\n", mach_error_string(kr));

	    if (id == 0)
		goto badtype;
	    /* fall-through */

	  case MACH_TYPE_HOST_PRIV:
	    kr = machid_host_processors(server,auth, id,
					&processors, &processorCnt);
	    break;

	  badtype:
	    fprintf(stderr, "ms: %u has type %s\n",
		    ids[i], mach_type_string(type));
	    continue;
	}
	if (kr != KERN_SUCCESS)
	    continue;

	for (j = 0; j < processorCnt; j++)
	    total += (*list_processor)(processors[j]);

	kr = vm_deallocate(mach_task_self(), (vm_offset_t) processors,
			   (vm_size_t) (processorCnt * sizeof *processors));
	if (kr != KERN_SUCCESS)
	    quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));
    }

    return total;
}

static int
list_processor_sets(list_processor_set, ids, count)
    int (*list_processor_set)();
    mach_id_t *ids;
    unsigned int count;
{
    int total = 0;
    unsigned int i, j;
    kern_return_t kr;

    for (i = 0; i < count; i++) {
	mach_id_t id = ids[i];
	mach_type_t type;
	mprocessor_set_t *psets;
	unsigned int psetCnt;

	kr = machid_mach_type(server, auth, id, &type);
	if (kr != KERN_SUCCESS)
	    quit(1, "ms: machid_mach_type: %s\n", mach_error_string(kr));

	switch (type) {
	  case MACH_TYPE_NONE:
	    continue;

	  default:
	    kr = machid_mach_lookup(server, auth, id,
				    MACH_TYPE_PROCESSOR_SET, &id);
	    if (kr != KERN_SUCCESS)
		quit(1, "ms: machid_mach_lookup: %s\n", mach_error_string(kr));

	    if (id == 0)
		goto badtype;
	    /* fall-through */

	  case MACH_TYPE_PROCESSOR_SET:
	    total += (*list_processor_set)(id);
	    continue;

	  case MACH_TYPE_HOST:
	    kr = machid_mach_lookup(server, auth, id,
				    MACH_TYPE_HOST_PRIV, &id);
	    if (kr != KERN_SUCCESS)
		quit(1, "ms: machid_mach_lookup: %s\n", mach_error_string(kr));

	    if (id == 0)
		goto badtype;
	    /* fall-through */

	  case MACH_TYPE_HOST_PRIV:
	    kr = machid_host_processor_sets(server,auth, id,
					    &psets, &psetCnt);
	    break;

	  badtype:
	    fprintf(stderr, "ms: %u has type %s\n",
		    ids[i], mach_type_string(type));
	    continue;
	}
	if (kr != KERN_SUCCESS)
	    continue;

	for (j = 0; j < psetCnt; j++)
	    total += (*list_processor_set)(psets[j]);

	kr = vm_deallocate(mach_task_self(), (vm_offset_t) psets,
			   (vm_size_t) (psetCnt * sizeof *psets));
	if (kr != KERN_SUCCESS)
	    quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));
    }

    return total;
}

static int
list_hosts(list_host, ids, count)
    int (*list_host)();
    mach_id_t *ids;
    unsigned int count;
{
    int total = 0;
    unsigned int i, j;
    kern_return_t kr;

    for (i = 0; i < count; i++) {
	mach_id_t id = ids[i];
	mach_type_t type;
	mhost_t *hosts;
	unsigned int hostCnt;

	kr = machid_mach_type(server, auth, id, &type);
	if (kr != KERN_SUCCESS)
	    quit(1, "ms: machid_mach_type: %s\n", mach_error_string(kr));

	switch (type) {
	  case MACH_TYPE_NONE:
	    continue;

	  default:
	    kr = machid_mach_lookup(server, auth, id,
				    MACH_TYPE_HOST_PRIV, &id);
	    if (kr != KERN_SUCCESS)
		quit(1, "ms: machid_mach_lookup: %s\n", mach_error_string(kr));

	    if (id == 0)
		goto badtype;
	    /* fall-through */

	  case MACH_TYPE_HOST_PRIV:
	  case MACH_TYPE_HOST:
	    total += (*list_host)(id);
	    continue;

	  badtype:
	    fprintf(stderr, "ms: %u has type %s\n",
		    ids[i], mach_type_string(type));
	    continue;
	}
#if	0
	if (kr != KERN_SUCCESS)
	    continue;

	for (j = 0; j < hostCnt; j++)
	    total += (*list_host)(hosts[j]);

	kr = vm_deallocate(mach_task_self(), (vm_offset_t) hosts,
			   (vm_size_t) (hostCnt * sizeof *hosts));
	if (kr != KERN_SUCCESS)
	    quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));
#endif	0
    }

    return total;
}

int
all_things(list_all, list_thing)
    int (*list_all)();
    int (*list_thing)();
{
    mhost_t host;
    mhost_priv_t host_priv;
    kern_return_t kr;
    int i;

    if (node_count)
	    for (i = 0; i < node_count; i++) {
		    kr = machid_node_host_ports(server, auth, node_number[i],
						&host, &host_priv);
		    if (kr != KERN_SUCCESS)
			    quit(1, "ms: machid_host_ports: %s\n",
				 mach_error_string(kr));
		    
		    (*list_all)(list_thing, &host_priv, 1);
	    }
    else {
	    kr = machid_host_ports(server, auth, &host, &host_priv);
	    if (kr != KERN_SUCCESS)
		    quit(1, "ms: machid_host_ports: %s\n",
			 mach_error_string(kr));
	    
	    return (*list_all)(list_thing, &host_priv, 1);
    }

}

#if 0
static int
all_things(list_all, list_thing)
    int (*list_all)();
    int (*list_thing)();
{
    mhost_t host;
    mhost_priv_t host_priv;
    kern_return_t kr;

    kr = machid_host_ports(server, auth, &host, &host_priv);
    if (kr != KERN_SUCCESS)
	quit(1, "ms: machid_host_ports: %s\n",
	     mach_error_string(kr));

    return (*list_all)(list_thing, &host_priv, 1);
}
#endif

static unsigned int
zone_count(host_priv, name)
    mhost_priv_t host_priv;
    char *name;
{
    zone_name_t *names;
    zone_info_t *info;
    unsigned int namesCnt, infoCnt, i;
    unsigned int count = 0;
    kern_return_t kr;

    kr = machid_host_zone_info(server, auth, host_priv,
			       &names, &namesCnt, &info, &infoCnt);
    if ((kr != KERN_SUCCESS) || (namesCnt != infoCnt))
	quit(1, "ms: machid_host_zone_info: %s\n", mach_error_string(kr));

    for (i = 0; i < namesCnt; i++)
	if (streql(names[i].zn_name, name))
	    count = info[i].zi_count;

    kr = vm_deallocate(mach_task_self(), (vm_offset_t) names,
		       (vm_size_t) (namesCnt * sizeof *names));
    if (kr != KERN_SUCCESS)
	quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));

    kr = vm_deallocate(mach_task_self(), (vm_offset_t) info,
		       (vm_size_t) (infoCnt * sizeof *info));
    if (kr != KERN_SUCCESS)
	quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));

    return count;
}

static void
ipc_sizes(host_priv, sizes)
    mhost_priv_t host_priv;
    struct ipc_sizes *sizes;
{
    zone_name_t *names;
    zone_info_t *info;
    unsigned int namesCnt, infoCnt, i;
    kern_return_t kr;

    bzero((char *) sizes, sizeof *sizes);

    kr = machid_host_zone_info(server, auth, host_priv,
			       &names, &namesCnt, &info, &infoCnt);
    if ((kr != KERN_SUCCESS) || (namesCnt != infoCnt))
	quit(1, "ms: machid_host_zone_info: %s\n", mach_error_string(kr));

    for (i = 0; i < namesCnt; i++)
	if (streql(names[i].zn_name, "ipc spaces"))
	    sizes->space = info[i].zi_elem_size;
	else if (streql(names[i].zn_name, "ipc tree entries"))
	    sizes->tentry = info[i].zi_elem_size;
	else if (streql(names[i].zn_name, "ipc ports"))
	    sizes->port = info[i].zi_elem_size;
	else if (streql(names[i].zn_name, "ipc port sets"))
	    sizes->pset = info[i].zi_elem_size;
	else if (streql(names[i].zn_name, "ipc msg-accepted requests"))
	    sizes->marequest = info[i].zi_elem_size;

    kr = vm_deallocate(mach_task_self(), (vm_offset_t) names,
		       (vm_size_t) (namesCnt * sizeof *names));
    if (kr != KERN_SUCCESS)
	quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));

    kr = vm_deallocate(mach_task_self(), (vm_offset_t) info,
		       (vm_size_t) (infoCnt * sizeof *info));
    if (kr != KERN_SUCCESS)
	quit(1, "ms: vm_deallocate: %s\n", mach_error_string(kr));

    if (sizes->space == 0)
	sizes->space = 60;
    if (sizes->port == 0)
	sizes->port = 64;
    if (sizes->pset == 0)
	sizes->pset = 20;
    if (sizes->entry == 0)
	sizes->entry = 16;
    if (sizes->tentry == 0)
	sizes->tentry = 32;
    if (sizes->marequest == 0)
	sizes->marequest = 16;
    if (sizes->dnrequest == 0)
	sizes->dnrequest = 8;
}

static void
old_ipc_sizes(host_priv, old_sizes)
    mhost_priv_t host_priv;
    struct old_ipc_sizes *old_sizes;
{
    struct host_basic_info info;
    kern_return_t kr;

    kr = machid_host_basic_info(server, auth, host_priv, &info);
    if (kr != KERN_SUCCESS)
	quit(1, "ms: machid_host_basic_info: %s\n", mach_error_string(kr));

    /* standard configuration Mach 2.5 sizes */

    if (info.max_cpus > 1) {
	old_sizes->task = 80;
	old_sizes->port = 96;
	old_sizes->pset = 64;
	old_sizes->translation = 52;
    } else {
	old_sizes->task = 76;
	old_sizes->port = 88;
	old_sizes->pset = 56;
	old_sizes->translation = 52;
    }
}
