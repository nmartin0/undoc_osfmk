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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <strings.h>
#include <errno.h>
#include <nx/h.h>
#include <nx/defines.h>
#include <nx/nodeparser.h>

/***************************** autoinit ***********************
 *
 *      Calling Sequence:
 *		autoinit(argc, argv, envp);
 *
 *      Description:
 *		autoinit is called by the special crt0 that gets linked
 *		in when the user uses -NX while linking a parallel
 *		application. autoinit() parses the arguments and calls 
 *		nx_initve() and other library calls to make the user application
 *		parallel
 *
 *      Parameters:
 *		argc:		argument count
 *		argv:		arguments
 *		envp:		environment
 *
 *      Returns:
 *               status
 *
 * $Id: autoinit.c,v 1.36 1995/03/07 18:31:56 shane Exp $
 *
 * HISTORY
 * $Log: autoinit.c,v $
 * Revision 1.36  1995/03/07  18:31:56  shane
 *  Reviewer: Scott Hahn
 *  Risk: Low
 *  Benefit or PTS #:10640
 *  Testing: Tested using both with and without -Mconcur
 *  Module(s): autoinit.c
 *
 * Revision 1.35  1994/12/16  23:44:56  sdh
 * Added code to  check the return of nx_loadve and if some child
 * did not start up correctly, it prints out a message to that
 * affect.
 *
 *  Reviewer: hobbes
 *  Risk: Low/medium
 *  Benefit or PTS #: 11164, 11955, 11663
 *  Testing:
 * 	EATS: controlc, rmcall, rmcmd, sched
 * 	manual tests
 *  Module(s):
 * 	cmds_libs/src/usr/ccs/lib/libnx/nx_loadve.c
 *         cmds_libs/src/usr/ccs/lib/libnx/autoinit.c
 *         cmds_libs/src/usr/ccs/lib/common_nx_c/nx_setup.c
 *         cmds_libs/src/usr/include/nx/h.h
 *
 * Revision 1.34  1994/11/19  02:29:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.33  1993/11/18  19:24:51  dleslie
 *  Reviewer: shala
 *  Risk: low
 *  Benefit or PTS #: get nx and mcmsg headers out of export tree, not obj
 * 	tree.  This allows users to build without having an obj tree fully
 * 	populated with headers
 *  Testing: built libnx
 *  Module(s):
 *     Makefile _gcol.c _gcolx.c _gops.c _gsync.c _load.c allocUser.c
 *     allocsys.c allocsys_.c autoinit.c bitmap.c bitmap2.c create.c
 *     nodeparser.c nx_initve.c nx_load.c nx_load_.c nx_loadve.c
 *     nx_loadve_.c nx_lock.c nx_part_ops.c nx_part_ops_.c nx_port.c
 *     nx_pri.c nxlib.c parsepart.c parsesched.c partlock.c
 *     partprint.c partutils.c rkassert.c rklib.c rkmem.c utils.c
 *     writepart.c
 *
 *
 * VS:    writepart.c
 *
 * Revision 1.32  1993/10/27  01:41:44  carbajal
 * Removed -sz parsing from autoinit. It is done by nx_initve()
 *
 * Revision 1.31  1993/07/01  17:39:34  carbajal
 * Made perror()'s more descriptive
 *
 * Revision 1.30  1993/02/19  19:38:46  carbajal
 * Autoinit no longer picks out -pri from argv. This is left
 * to nx_initve.
 * Nx_initve passed priority to _nx_init().
 *
 * Revision 1.29  1993/01/25  22:20:25  carbajal
 * in -on specification make sure that the node list
 * is valid. Moved nodeparser code from here to nodeparser.c.
 * Added support for descending node range specifications.
 *
 * Revision 1.28  1993/01/18  20:01:04  carbajal
 * Always call nx_pri()
 *
 * Revision 1.27  1993/01/05  22:30:48  shala
 * Support nx_pri .
 *
 * Revision 1.26  1992/12/18  02:28:25  carbajal
 * Use NX_DFLT_SIZE
 * rewrote nodeparser routine
 * implemented -on 0..n. PTS #2712
 *
 * Revision 1.25  1992/11/30  21:43:50  carbajal
 * Need to increment index after catching a ";" on command line.
 * This caused a no such file error message when using the \;
 * option.
 *
 * Revision 1.24  1992/11/18  02:38:36  cameron
 * Made programs following \; use their own name as argv[0] rather than the
 * first program on the command line.
 *
 * Revision 1.23  1992/11/07  22:57:17  carbajal
 * Corrected args to nx_loadve, &pid_array was being passed
 * we really want pid_array. This fixes bug #3442.
 *
 * Revision 1.22  1992/07/14  14:20:23  shala
 * Remove extra printf
 *
 * Revision 1.21  92/07/14  12:57:18  cameron
 * Cahnged to properly null terminate the argvs of both the forked and
 * execed processes.
 * 
 * Revision 1.20  92/06/16  14:11:30  nandy
 * Fixed incorrect bad node spec.
 * 
 * Revision 1.19  92/06/11  14:54:14  shala
 * Removed nx_setup routine
 * 
 * Revision 1.18  92/06/10  12:39:13  shala
 * Used SPECIAL 
 * 
 * Revision 1.17  92/06/09  19:26:30  stans
 * created nx_setup() routine which is called by crt0.c when the function
 * pointer nx_setup_routine has a non-zero value. NOrmally crt0.c sets
 * nx_setup_routine == o ala BSS declaration. When autoinit.o is linked ala
 * "-nx" cmds then nx_setup_routine is set to "nx_setup()". When the pgm is
 * run "nx_setup()" is called and everybody is happy including vanilla UNIX
 * pgms.
 * 
 *
 */

char _autoinit_id[]="$Id: autoinit.c,v 1.36 1995/03/07 18:31:56 shane Exp $";

int
autoinit( argc, argv, envp ) 
int	*argc;
char	*argv[];
char	*envp[];
{

	register	j,k,i;

	int	first;			/* This flag is set if we are parsing
					   aguments before the first ; */ 
	node_t	*node_array;		/* array of logical nodes */
	node_t  *tmp_nodes;
	long	nodecnt;		/* # of elements in the node_array */
	pid_t	*pid_array;		/* array of pids */
	int	index;			/* index to the argv */
	long	size;			/* argument to -sz */
	char	*name;			/* partition name */
	int	ptype;			/* process type */
	char	*nodedescr;		/* argument to -on */
	long	numnodes;		/* # of nodes returned by nx_initve */
	char	*tmpptr;		/* Temorary array */
	short	onflg, ptflg, pnflg, szflg;	/* option flags */
	char	partenv[] = "NX_DFLT_PART";	/* Enviroment variable for
						   partition name */
	int	oldargc;		/* Number of arguments passed in */
	int	status;			/* stauts from the nx_ calls */
        void    perror_special(const char *); /* Special perror */



	size = 0;

	/*
	 * Allocate space for the new argument vector
	 */

	oldargc = *argc;
 	
	first = 1;
	index = 0;

	/*
	 * Look at each argument in argv
	 */
	while(index < oldargc) {
		*argc = 0;
		nodecnt = 0;
		onflg = 0;
		ptflg = 0;
		pnflg = 0;
		szflg = 0;
		for( index ; index < oldargc; index++) {

			 if( strcmp(argv[index],"-on" ) == 0 )  {
				onflg = 1;
				nodedescr = argv[++index];
			} else if( strcmp(argv[index],"-pn" ) == 0 )  {
				pnflg = 1;
				name = argv[++index];
			} else if( strcmp(argv[index],"-pt" ) == 0 )  {
				if( isnumber(argv[++index]) == 0 ) {
					errno = EINVAL;
					exit(1);
				}
				ptflg = 1;
				ptype = atoi(argv[index]);
			} else if( strcmp(argv[index],";" ) == 0 )  {
				index++;
				break;
			} else {
				argv[*argc] = argv[index];
				(*argc)++;
			}
		}

		/*
		 * Null terminate the argument vector for programs
		 * which do not look at argc.
		 */
		argv[*argc] = (char *) 0;

		/*
	 	 * First executable specification gets special treatment
	 	 */ 
		if( first) {

			/* 
			 * If partition name was not specified by the user,
			 * get it from the environment. If variable is not set
			 * set the partition name to .compute  
			 */

			if( !pnflg) {
				name = (char *) malloc ( NAMELEN );
				if( name == (char *) 0) {
					errno = ENOMEM;
					perror_special("autoinit");
					exit(1);
				}
				tmpptr = (char *) getenv(partenv);
				if( tmpptr == NULL) {
					strcpy(name, ".compute");
				} else {
					name = tmpptr;
				}
			}

			if (((numnodes = nx_initve(name, size, NULL, argc,
					 argv)) < 0 )) {
				perror_special("autoinit nx_initve");
				exit(1);
			}

		}

		node_array = (node_t *) malloc( sizeof(node_t) * numnodes);
		tmp_nodes  = (node_t *) malloc( sizeof(node_t) * numnodes);
		pid_array = (pid_t *) malloc( sizeof(pid_t) * numnodes);
	
		/*
	 	 * Set up the node array and get the node count
	 	 */

		if ( onflg) {
			nodecnt = nodeparser(nodedescr,tmp_nodes);
			/* validate the node list */
			for (i = 0; i < nodecnt; i++)
				if (tmp_nodes[i] >= numnodes){
					errno = EPBADNODE;
					perror_special("autoinit nodeparser");
					exit(1);
				}
			nodecnt = update_node_list(numnodes,nodecnt,tmp_nodes,node_array);
		} else {
			for( i = 0; i < numnodes; i++ ) 
				node_array[i] = i;
			nodecnt = numnodes;
		}

		if( !ptflg) {
			ptype = 0;
		}

		/*
		 * For the first executable, call nx_nfork(), else call
		 * nx_loadve().
		 */
		if (first) {
			status = nx_nfork( node_array, nodecnt, ptype, 
						pid_array ); 
			if( status == -1) {
				perror_special("autoinit nx_nfork");
				exit(1);
			} else if (status == 0) { 
				return(0);
			}
			first = 0;
		} else {
			status = nx_loadve( node_array, nodecnt, ptype, 
					pid_array, argv[0], argv, envp);
			if (status < nodecnt) { 
			    /* not all children started */
			    for (i = 0; i < nodecnt; i++) {
				if (pid_array[i] == 0)
				    fprintf(stderr,
					    "autoinit: application failed to start on node %d\n", node_array[i]);
			    }
			}
		}

	}

	if( nx_waitall() == -1 ) {
		perror_special("autoinit nx_waitall");
		exit(1);
	}
	exit(0);
}

int
update_node_list(numnodes,nodecnt,tmp_nodes,node_array)
int	numnodes,nodecnt;
node_t	*tmp_nodes,*node_array;
{
	register	i,j,k;

	/* cruise through the node array looking for
	 * -100 which indicates we need to overwrite
	 * it with the last node.
	 * A -200 indicates that we saw something like 0..n
	 * so we need to fill in a range
	 * A -300 indicates we save n..0
	*/
	i = 0;
	j = 0;

	while (i < nodecnt){
		if (tmp_nodes[i] == -100){
			node_array[j++] = numnodes-1;
			i++;
		}
		else
		if (tmp_nodes[i] == -200){
			/* Count up */
			for(k = tmp_nodes[i-1]+1; k < numnodes; k++)
				node_array[j++] = k;
			i++;
		}
		else
		if (tmp_nodes[i] == -300){
			/* count down from n to whatever */
			for(k = numnodes-1; k >= tmp_nodes[i+1]; k--)
				node_array[j++] = k;
			/* skip over the -300 and the next element which marked our range */
			i+= 2;
		}
		else
			node_array[j++] = tmp_nodes[i++];
	}
	return(j);
}
extern int sys_nerr, write();
extern char *sys_errlist[];

/*
 * NAME:        perror_special
 *
 * FUNCTION:    perror_special writes a message to the standard error output
 *              describing the last error encountered by a system
 *              call or the last error encountered by a subroutine call
 *              that set 'errno'.
 *
 * RETURN VALUE DESCRIPTION:    none
 */

void
perror_special(const char *s)
{
        int n = 0;
        char *c;        /* pointer to string to be printed      */

        /*
         * we assume here that 80 is big enough to hold our sprintf()
         * down below.  if not, ints are extremely large!
         */

        char buf[80];

        if(errno >= 0 && errno < sys_nerr)
            c = sys_errlist[errno];
        else
            (void) sprintf(c = &buf[0], "Error %d occurred, unknown.", errno);

        if(s)
        {
            n = strlen(s);
        }
        if(n)
        {
            (void) write(2, s, (unsigned) n);
            (void) write(2, ": ", 2);
        }
        (void) write(2, c, (unsigned) strlen(c));
        (void) write(2, "\n", 1);
}


