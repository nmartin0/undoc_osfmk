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
 *		Copyright (c) Locus Computing, 1991-92
 * 		This is UNPUBLISHED source code that is
 * 		the property of Locus Computing, containing
 *		proprietary secrets of LCC.  Any disclosure
 *		is strictly prohibited.  Locus makes no warantee,
 *		explicit or implicit, on the functionality of this code.
 */
/*
 * HISTORY
 * $Log: onnode.c,v $
 * Revision 1.5  1994/11/19  03:07:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/11/18  20:53:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/22  17:27:13  bolsen
 *  Reviewer: Jerry Toman and Stefan Tritscher
 *  Risk: low
 *  Benefit or PTS #: 7217 - fast -K doesn't work
 *  Testing: LCC basic Load_leveld/Onnode tests
 *  Module(s): user/etc/load_level/onnode.c
 *
 * Revision 1.2  1993/11/19  18:23:32  bolsen
 *  Reviewer: Mike Barnett and Stefan Tritscher
 *  Risk: low
 *  Benefit or PTS #: 5390 - incorrect size for shared memory segment
 *  Testing: LCC onnode and load_leveld tests
 *  Module(s): user/etc/load_level/load_leveld.c
 * 	    user/etc/load_level/onnode.c
 *
 * Revision 1.1.1.3  1993/07/01  21:17:36  cfj
 * Adding new code from vendor
 *
 * Revision 3.1  93/08/03  10:50:10  bolsen
 * Fixed the following bug:
 * [Bug 304/5390]  Use structure size for shared memory segment OR zero.
 * 
 * Revision 3.0  93/04/23  16:08:12  bolsen
 * [SPE 0007] reference common header file for shared memory structures,
 * 	variables and NEW macros (ie. common to load_leveld and onnode
 * 	(fast & fastnode)).
 * 
 * Revision 3.14  93/03/17  11:47:29  jpaul
 * Fix some logic with the kflag
 * 
 * Revision 3.13  93/01/20  14:02:01  yazz
 * Corrected computation of proj_id parameter used in ftok() call, so
 * that node numbers of the form X*128 - 1 (like 127, 255) will work.
 * 
 * Revision 3.12  92/12/29  14:32:32  mbarnett
 * Modified the onnode.c code to use the same keys which are used by the 
 * load leveler daemon when the daemon creates the shared memory region.  This
 * change allows fast/fastnode to be able to access this same shared memory
 * region.  The keys now generated use key files which are stored in
 * a private directory("/etc/load_level/shm_keys" by default).  The -K option 
 * now overrides this default.
 * 
 * Revision 3.11  92/12/02  14:33:27  mbarnett
 * Modified onnode code(and hence fast and fastnode) to generate and use the
 * same unique shared memory ipc keys that are used by the load leveler daemon.
 * This code change fixes bug #0120.
 * 
 * Revision 3.10  92/11/24  15:30:05  jpaul
 * Fix for bug #98 -- users other than root can't access 
 * shared memory: add SHM_RDONLY to shmat shared memory
 * attach call.
 * 
 * Revision 3.9  92/07/22  16:56:06  jpaul
 * Added code to handle shared memory for fast and fastnode.
 * 
 * Revision 3.8  92/06/15  16:22:42  jpaul
 * Fix bug where fast always returned 0 as the fastest node.
 * Use large initialization value to avoid checks for 
 * negative loads.
 * 
 * Revision 3.7  92/05/29  17:04:27  jpaul
 * Added support for shared memory for the load leveller.
 * Added undocumented option '-K' to choose another key
 * string for ftok.  Cleaned up comments.
 * 
 * Revision 3.6  92/05/22  13:44:19  jpaul
 * Fixed logic to flush stdout buffer in non-tty case.
 * Fixed problems exposed by VSX testing and VSTNC testing.
 * 
 * Revision 3.4  92/05/01  15:37:15  yazz
 * Re-groove to make both i386 and i860 work.
 * Also defined $(SOURCECWD) which points to the current source
 * directory -- handy for -I and other options.  (Bob Yazz)
 *
 * Revision 3.3  92/04/21  13:44:11  klh
 * Changed code to allow multiple -v options.
 *
 * Revision 3.2  92/04/16  17:00:46  jpaul
 * Fixed argv[0] pointer problem with onnode call.
 *
 * Revision 3.1  92/04/16  15:41:32  jpaul
 * Fixed pointer bug in onnode() with command pointer initialization.
 *
 */

#include <dirent.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include "load_level_com.h"

#ifndef MAXNAMLEN
#define MAXNAMLEN 255
#endif

/*
 *	onnode.c : (also includes fast and fastnode.)
 *
 *	Purpose:  Allow cross-node proces tranceperency and
 * 		  access to remote processing.
 *
 *	N.B. This source file produces onnode, fast, and fastnode. -- jp
 *
 *		onnode:	 Execute command on a specified node.
 *		fast:	 Execute command on the fastest (least loaded) node.
 *		fastnode:Return the fastest node
 *
 *		Use rexecvp() call after skipping first two
 *		entries in argv list and utilizing the PATH
 *		environment variable to find the target file, if the
 * 		complete filename is not given.  (i.e. '/' not
 * 		first character of filename).
 *
 *
 *
 *	onnode:
 *		Execute command on a remote node.
 *	syntax : onnode nodenum command [args...]
 *	  	nodenum is an integer >= 0.
 * 	  	command is a valid executable command, along with optional
 * 	  	arguments.
 *
 *
 *	fast :
 *		Execute command on the fastest node in the cluster.
 *	syntax : fast [-v] command [args...]
 *
 *
 *	fastnode :
 *
 *		Report the fastest node in the cluster.
 *	syntax : fastnode
 */

extern int node_self();
extern char *getenv();


/*
#define DEBUG=1
*/

#ifdef DEBUG
#define DBG(a) printf a /* for debugging, obviously */
#else
#define DBG(a)
#endif
#define PREPOSTEROUS_LOAD 999.99

char myname[MAXNAMLEN];

/* pointer to the string containing the pathname to a directory in which
   the key files for shared memory are created */
extern char *key_dir = { KEY_DIR };

int
main(int argc,
     char *argv[],
     char *envp[])
{
	char *command;

	/*
	 * Are we doing fast or onnode?  Obtain the
	 * command first.
	 */
	command = (char *) strrchr(argv[0], '/');	/* find last slash */
	if (command == NULL)
		command = argv[0];
	else
		++command;
	strncpy(myname, command, MAXNAMLEN);		/* save for others */
	strcat(myname, "\0");
	DBG(("argv[0] is %s\n", argv[0]));
	DBG(("command is %s\n", command));

	if (strcmp(command, "onnode") == 0) {
		DBG(("doing onnode\n"));
		do_onnode(argc, argv, envp);
	}
	else {
		DBG(("doing fast/{node}\n"));
		do_fast(argc, argv, envp, command);
	}
	exit(-1);  /* NOTREACHED  */
}

/*
 * this module checks the argument count and calls the
 * onnode() routine.  If there is an argument problem,
 * or the onnode() call fails, an error message is printed
 * depending on the returncode, rc
 */
int
do_onnode(int argc,
     	char *argv[],
     	char *envp[])
{

	char *p;
	int rc;
	int node;

	if (argc < 3) {  /* not enough args  */
		usage_onnode();
		exit(-1);
	}

	node = strtol((argv[1]), &p, 0);
	DBG(("node number is %d and my name is %s.\n", node, argv[0]));
	/* check for failure from strtol */
	if ((p == argv[1]) || (*p != '\0')) { 	/* failure cases  */
		fprintf(stderr,"%s: %s is not a valid node number.\n",
			  myname, argv[1]);
		exit(-1);
	}
	/* Sufficient parameters for onnode() invocation.  Call with
	 * modified argv and argc, skipping the first two argv params
	 * ('onnode' and nodenum).  The former is not needed, the latter
	 * already saved in the nodenum variable. (and verified above)
	 * Eliminate the first 2 argv[] elements, they are
	 * the nodenum gleaned above and the 'onnode' string
	 * itself.  Not needed.
	 */
	rc = onnode((argc - 2), &argv[2], envp, node);
	/* Should never be reached.  */
	exit(-1);
}



/*
 * do_fast() performs either the fast or fastnode command.
 */
int
do_fast(int argc,
     char *argv[],
     char *envp[],
     char *command)
{

	int rc;
	int nodenum;
	int vflag = 0;
	int kflag = 0;
	int errflag = 0;
	int argcoffset = 0; 	/* offsets into argc and argv for calls */
	int argvoffset = 0;
	char c;
	extern int optind;
	extern int opterr;  /* disable getopt error messages */
	extern char *optarg;

	opterr = 0;
	while ((c = getopt(argc, argv, "vK:")) != EOF) {
		DBG(("getopt got %c\n", c));
		switch (c) {
			case 'v': {
				vflag++;
				argcoffset++; /* get past flag */
				argvoffset++;
				break;
			}
			case 'K': { 
				DBG(("undocumented 'K' flag used...\n"));
				/* undocumented: use specified key for shmem. */
				kflag++;
				key_dir = optarg;
				DBG(("key dir='%s'\n", key_dir));
				argcoffset += 2; /* get past flag and value */
				argvoffset += 2;
				break;
			}
			default: {
				errflag++;
				break;
			}
		}
	}

	/*
	 * Check to see if we are doing fastnode or fast.
	 */
	if (strcmp(myname, "fastnode") == 0)
		if ((errflag) || ((argc > 2) && (!kflag))) {
			usage_fastnode();
			exit(-1); 
		}
		else {
			printf("%d\n", fast_node());
			return;
		}

	/* 
	 * fall through, we must be doing fast. 
	 * Check for its error case.  
	 */
	if (errflag || (argc - optind < 1)) { /* need at least one to execute */
		usage_fast();
		exit(-1);
	}

	DBG(("do_fast, calling fast_node...argv=%s, argc=%d\n", *argv, argc));
	nodenum = fast_node();  /* get the fastest node from the ll shmem. */
	DBG(("fast node returned %d\n", nodenum));

	/*
	 * Use the onnode subroutine to perform the operation
	 * after printing out the node, if verbose is enabled.
	 */
	if (vflag) {
		printf("fast node %d\n", nodenum);
	}
	fflush(stdout);  /* buffer flush required in non-tty case.  */

	/* get past calling command */
	argcoffset++; 
	argvoffset++;

	DBG(("argcoffset=%d, argvoffset=%d\n", argcoffset, argvoffset));
	DBG(("argc-argcoffset=%d\n", argc-argcoffset));
	
	rc = onnode((argc - argcoffset), &argv[argvoffset], envp, nodenum);
	/* Should never be reached.  */
	DBG(("onnode call failed, rc=%d\n", rc));
	exit(-1);
}

/*
 * fast_node uses the string 'key_dir' to attach to the 
 * proper shared memory with the load leveller.  An undocumented
 * -K option utilizes a different key_dir.
 */
int
fast_node()
{

	int 	memloop; 		/* loop thru shmem  */
	int	detach;	 		/* detach from shmem correctly? */
	int	new_node, fast_node = 0;/* new node, fast node this iteration */
	int 	shmid;			/* shared mem id */
	double	new_load, fast_load; 	/* new load, fast load this iteration */
	struct 	load_info *load_i_ptr;

	/* get shmem key */
	GET_KEY_FILE(key_file, key_dir);
	GET_KEY(key, key_file);
	if (key == (key_t)-1) {  /* can't get at shared mem */
		fprintf(stderr, 
			"Load information not available; using local node.\n");
		DBG(("fast_node(), key returned -1.\n"));
		return(node_self());
	}
	DBG(("Key=%d\n", key));

	/* get the shmid and attach to the shared memory segment */
	shmid = shmget(key, 0, 0);
	if (shmid == -1) { /* catastrophic error, got the key, can't attach */
		fprintf(stderr, 
			"Load information not available; using local node.\n");
		DBG(("fast_node(), got key, unable to attatch to shmem.\n"));
		return(node_self());
	}
	
	load_i_ptr = (struct load_info*)shmat(shmid, 0, SHM_RDONLY);

	/*
	 * If the load leveller shared memory is inaccessable,
	 * warn user and use local node.
	 */
	if (load_i_ptr == (struct load_info*)-1) { /* attach failed */
		fprintf(stderr, 
			"Load information not available; using local node.\n");
		DBG(("fast_node(), got key, unable to cast to load_info struct.\n"));
		return(node_self());
	}

	/* 
	 * Here, we are attached to the shared mem segment.  
	 * Loop through it backwards  and get the least loaded node 
	 * (which will be at the front of the list, therefore the 
	 * most up-to-date entry).
	 */
	DBG(("num elements: %d\n", load_i_ptr->num_elements));
	fast_load = PREPOSTEROUS_LOAD; /* init so normal load will replace */
	for (memloop = load_i_ptr->num_elements - 1; memloop >= 0; memloop--) { 
		/* zero based  */
		new_node = load_i_ptr->load_vector[memloop].node;
		new_load = load_i_ptr->load_vector[memloop].lm;

		DBG(("read new node= %d, lm=%lf\n", new_node, new_load));
		DBG(("fastnode=%d, fastload= %lf\n", fast_node, fast_load));
		DBG(("fast_load:%lf; new_load:%lf\n", fast_load, new_load));
	
		/* 
		 * Since the inital load values are negative, look 
		 * to see that it is a true load instead of the dummy
		 * negative initialzation value before using the node.
		 */
		if ((new_load < fast_load) && (new_load >= 0.0)) {
				fast_node = new_node;
				fast_load = new_load;
				DBG(("found new fast node %d %g\n",
					fast_node, fast_load));
		}  
	DBG(("\n"));
	} 
	DBG(("Detatching...\n"));
	detach = shmdt((char *)load_i_ptr);
	if (detach == -1)
		DBG(("Shared memory detach failed, errno%d\n", errno));
	DBG(("Detatched...\n"));
	return(fast_node);
} /* end fast_node */

/*
 * 	onnode() :
 *		execute command on a specified node.  get info (nodenum and
 * 		command) from the argv[] array above, which are discarded and
 *		passed to the call as paramaters.
 */
int
onnode(	int argc,	/* arg count */
	char *argv[],	/* this argv is offset by 2, argv[0] is	*/
	char *envp[],	/* really argv[2] from the onnode invocation.	*/
	int nodenum)	/* see above for discussion.			*/

			/* argv[0] has the command name.  Any additional
			 * entries in the array are arguments.
			 */
{
	int rc;
	char *command;

	command = argv[0];

	DBG(("before rexecvp.  command='%s' argv=%s  nodenum=%d\n",
		command, *argv, nodenum));
 	rc = rexecvp(command, argv, nodenum);
	/* The rexecvp failed.  Tell user why, using errno.
	 * This should not be reached.
	 */
	switch (errno) {
		case ENOENT:
			fprintf(stderr, "%s: %s not found.\n",
				myname, command);
			break;
		case EACCES:
			fprintf(stderr, "%s: can't execute %s.\n",
				myname, command);
			break;
		case EINVAL:
			fprintf(stderr, "%s: %d is not a valid node number.\n",
				myname,  nodenum);
			break;
		case ENOEXEC:
			fprintf(stderr, "%s: is not valid executable file.\n",
				myname);
		default:
			fprintf(stderr, "%s: can't execute %s, errno=%d.\n",
				myname, command, errno);
			break;
	}  /*  end switch  */
	DBG(("returncode from rexecvp:%d\n", rc));
	return(rc);
} /* end onnode  */

/*  usage messages  */

/*
 * 	usage_onnode():
 * 		print usage message and return.
 */
int
usage_onnode()
{
	fprintf(stderr, "usage: onnode node command [ arg ] ...\n");
	return;
}

/*
 * 	usage_fast():
 * 		print usage message and return.
 */
int
usage_fast()
{
	fprintf(stderr, "usage: fast [ -v ] command [ arg ] ...\n");
	return;
}

/*
 * 	usage_fastnode():
 * 		print usage message and return.
 */
int
usage_fastnode()
{
	fprintf(stderr, "usage: fastnode\n");
	return;
}
