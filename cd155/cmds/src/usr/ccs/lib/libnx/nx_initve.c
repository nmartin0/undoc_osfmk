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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_initve.c,v 1.72 1995/03/21 21:05:18 lenb Exp $
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/version.h>
#include <mcmsg/mcmsg_appl.h>
#include <mcmsg/mcmsg_xmsg.h>
#include <nx.h>
#include "allocsys.h"
#include <nx/allocator.h>
#include <nx/bitmap.h>
#include <nx/defines.h>
#include <nx/mkpart.h>
#include <nx/alloc_types.h>

long _nx_initve_attr(char* pname, int* argc, char** argv, va_list ap);

#define PKTHDR			sizeof(xmsg_t)
#define XPKT			(applinfo.pkt_size + PKTHDR)
#define ROUNDUP(a,b)            ((((a) + (b) - 1)/(b)) * (b))
#define ROUNDDOWN(a,b)          (((a)/(b)) * (b))


#define AVAILABLE_MEMORY	((8192 * 4096) + (10 * XPKT))

/* This macro flags where we are assuming Sunmos.  In case there are other
   guest operating systems, wherever this macro appears there may
   have to be decisions made */
#define IS_SUNMOS	((is_guest) && (guest_os == NX_SUNMOS))

/* defaults for message passing packet size */
#define		SMALLPKT_BYTES		1728
#define		BIGPKT_BYTES		8192

unsigned long _mcmsg_pktdflt = BIGPKT_BYTES;
unsigned long _mcmsg_pktmax  = BIGPKT_BYTES;

#define		PKTDFLT		(_mcmsg_pktdflt)
#define		PKTMAX		(_mcmsg_pktmax)

#define		PKTMIN 		(sizeof(xmsg_t))
#define		XPKTMIN		(PKTMIN + PKTHDR)


/* defaults for message passing wired buffer area */
#define         MBFDFLT         ((1024 * 1024) + (PROVIDE_THRESHOLD))
#define         MBFMAX          (AVAILABLE_MEMORY)
#define         MBFMIN          ((4 * XPKTMIN * ( nnodes + 1)) + (10 * XPKTMIN))
 
/* defaults for message passing exported memory */
#define         MEXDFLT         (applinfo.memory_buffer - (PROVIDE_THRESHOLD))
#define         MEXMAX          (applinfo.memory_buffer - (PROVIDE_THRESHOLD))
#define         MEXMIN          (2 * (nnodes + 1) * MEAMIN)
 
/* defaults for message passing memory each */
#define         MEADFLT         (MIN((10 * XPKT),MEAMAX))
#define         MEAMAX          (MIN(((1024*1024)-31),((applinfo.memory_export / 2) / (nnodes + 1))))
#define         MEAMIN          (2 * XPKT)
 
/* defaults for message passing send threshold */
#define         STHDFLT         (applinfo.memory_each / 2)
#define         STHMAX          (applinfo.memory_each -1)
#define         STHMIN          (0) /* no minimum */

/* defaults for message passing send count */
#define         SCTDFLT         (applinfo.memory_each / 2)
#define         SCTMAX          (applinfo.memory_each)
#define         SCTMIN          (applinfo.pkt_size)

/* defaults for message passing give threshold */
#define         GTHDFLT         (applinfo.pkt_size)
#define         GTHMAX          (applinfo.memory_each / 2)
#define         GTHMIN          (applinfo.pkt_size)

#define         PLKDFLT         0

/* useful globals for debugging */
int	_mcmsg_noc;
int _mcmsg_mcpon;	/* boolean */
int _mcmsg_nica;	/* boolean */
int _mcmsg_bigpkt;	/* boolean */
int _mcmsg_asmp;	/* boolean */

/*
 * Child process that is forked if calling process is not a process group
 * leader.
 */
static pid_t		proxy_pid;
void ignore_sigmigrate();

static int		in_foreground;	/* Was sheperd in the foregound */ 

long
nx_initve(pname, size, account, argc, argv) 
char	*pname;		/* Partition name */
long	size;		/* Partition Size */
char	*account;	/* Account name */
int	*argc;		/* Argument length */
char	**argv;		/* Arguments */
{
	if (size == -1) {
		errno = EPBADNODE;	/* compat w/ pre-*_attr universe */
		return -1;
	}

	return nx_initve_attr(pname, argc, argv,
				NX_ATTR_SZ,   size,
				NX_ATTR_ACCT, account,
				NX_ATTR_END);
}

long
nx_initve_rect(pname, anchor_node, rows, cols, account, argc, argv) 
char	*pname;		/* Partition name */
long	anchor_node;	/* anchor node for rectangle*/
long	rows;		/* Number of rows for rectangle */
long	cols;		/* Number of cols for rectangls */
char	*account;	/* Account name */
int	*argc;		/* Argument length */
char	**argv;		/* Arguments */
{
	long rect[2];

	rect[0] = rows;
	rect[1] = cols;
	return nx_initve_attr(pname, argc, argv,
				NX_ATTR_ANCHOR, anchor_node,
				NX_ATTR_RECT, rect,
				NX_ATTR_ACCT, account,
				NX_ATTR_END);
}


long
nx_initve_attr(char* pname, int* argc, char** argv, ...)
/*	pname;		Partition name */
/*	argc;		Argument length */
/*	argv;		Arguments */
{
	va_list ap;

	va_start(ap, argv);
	return _nx_initve_attr(pname, argc, argv, ap);
}


long
_nx_initve_attr(char* pname, int* argc, char** argv, va_list ap)
/*	pname;		Partition name */
/*	argc;		Argument length */
/*	argv;		Arguments */
{
	char		*name; 
	char		*partname;
	ino_t		inode; 
	APPLINFO_T	applinfo;	/* Pointer to applinfo structure */
	int		index;		/* index to argv */
	int		newargc;	/* new argument count */	
	int		numnodes;	/* Number of nodes returned by nx_initve*/
	int		pgrp;
	char		*tmpptr;
	char		*szptr;

	char		partenv[] = "NX_DFLT_PART"; 

	short		pktflg, mbfflg, sthflg, mexflg, plkflg, gthflg,
			meaflg, sctflg, pnflg, nocflg; /* Option Flag */
	int 		mem_each, nnodes; /* adjust defaults */
	int		priority;
	int i;
	int n,temp1,temp2;
	nx_part_info_t	part;
	NX_INIT_T	nx_info;
	nx_nodes_t 	node_parse_list;
	long		size;
	long		parse_for_map();
	int		relaxed;
	char*		selector_buf;
	int		selector_buflen;
	int		selector_cnt;
	int		is_guest = 0;	/* Is this a guest O. S.? */
	int		guest_os ;	/* Which guest O.S. is it ? */

	/*
	 * Set packet size dependent upon kernel info.
	 */
	{
		int		kerninfo;

		kerninfo = mcmsg_nx_op(1, 0, 0, 0, 0, 0, 0, 0, 0);
		if (kerninfo < 0) {
			/* error */
			errno = EINTERNAL;
			return -1;
		}
		_mcmsg_mcpon  = ((kerninfo & 1) != 0);
		_mcmsg_nica   = ((kerninfo & 2) != 0);
		_mcmsg_bigpkt = ((kerninfo & 4) != 0);
		_mcmsg_asmp   = ((kerninfo & 8) != 0);
	
		if (!_mcmsg_bigpkt) {
			_mcmsg_pktdflt = SMALLPKT_BYTES;
			_mcmsg_pktmax  = SMALLPKT_BYTES;
		}
	}

	if (argc == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (begin_parse_validation(ap) == -1)
		return -1;


	selector_cnt = parse_for_selectors(&selector_buf,
	  &selector_buflen, ap);
	if (selector_cnt == -1)
		return -1;

	/* Is this a guest O. S. ? */
	is_guest = parse_for_attribute(NX_ATTR_GUEST, &guest_os, ap);
	if (is_guest){
		switch (guest_os) {
			case NX_SUNMOS:
				break;
			default:
				errno = EQPARAM;
				return -1;

		}
	}

	size = parse_for_map(&node_parse_list, &relaxed, ap);
	if (size == -1)
		return -1;

	if ((size > 3 && node_parse_list) ||
	  (node_parse_list && node_parse_list[0] >= 0)) {
		errno = EINVAL;		/* NX_ATTR_MAP not legal */
		return -1;
	}

	/* Make sure everything is set to 0 */
	bzero(&nx_info,sizeof(nx_info));

	if (node_parse_list) {
		nx_info.rows = -node_parse_list[0];
		if (size > 1)
			nx_info.cols = -node_parse_list[1];
		else
			nx_info.cols = 0;
		if (size > 2)
			nx_info.start_node = -node_parse_list[2];
		else
			nx_info.start_node = -1;
	} else {
		nx_info.cols = 0;
		nx_info.rows = 0;
		nx_info.start_node = -1;
	}

	if (size < 0){
		errno = EPBADNODE;
		return -1;
	}

	if (pname != NULL && pname[0] != '\0')
		name = strdup(pname);
	else
		name = NULL;

#ifdef DEBUG
printf("_nx_initve entered with partname %s size %d rows %d cols %d \n",name,size,nx_info.rows,nx_info.cols);
printf("sizeof(applinfo) %d\n",sizeof(applinfo));
fflush(stdout);
#endif

	ignore_sigmigrate();

	/*
	 * Check to see if this process is a process group leader
	 */
	if (IS_SUNMOS) {
		/* The allocator knows we're done when the process group leader
		   terminates.  For a Guest OS, this process makes itself the
		   leader, it does not fork another process.
		   In case of Sunmos, the only thing that ever calls nx_initve
		   is yod, and there is no reason to fork another yod.
		 */
		setpgid(0, getpid());

	} 
	else if (getpid() != getpgrp()) {	/* not a guest O. S. */
		/*
		 * Not process group leader, check to see if we are the
		 * foreground process group.
		 */
		if (tcgetpgrp(0) == getpgrp()) {
			in_foreground = 1;
		}
		else {
			in_foreground = 0;
		}

		/*
		 * Fork off a parent process to "shepherd" the
		 * application.
		 */
		proxy_pid = fork();
		if (proxy_pid == -1) {
			return -1;
		} else if (proxy_pid != 0) {
			/* parent */
			shepherd(); /* doesn't return */
		} /* else child */
	
		/*
		 * The shepherd process is going to change this process'
		 * process group and then if the shepherd is in the foreground
		 * it will call tcsetpgrp() to put this process in the
		 * foreground. We need to wait for these to happen.
		 */
		if (in_foreground) {
			/*
			 * Wait til we are in the foreground.
			 */
			while (tcgetpgrp(0) != getpgrp());
		} else {
			/*
			 * Wait til shepherd makes us process group leader.
			 */
			while (getpid() != getpgrp());
		}
	}


	/*
	 * Initialize option flags
	 */
	pktflg = 0;
	mbfflg = 0;
	sthflg = 0;
	plkflg = 0;
	meaflg = 0;
	mexflg = 0;
	gthflg = 0;
	sctflg = 0;
	pnflg = 0;
	nocflg = 0;
	priority = DFLT_PRI;

	/*
	 * Set up applinfo 
	 */

	if (parse_for_applinfo(&applinfo, &priority, ap) < 0)
		return -1;

	/* Parse command arguments if not Sunmos.  For Sunmos, yod does
	   its own node allocation.  No command line arguments to yod
	   can be relevant to nx_initve.  This could change if yod 
	   used the allocator to allocate nodes for it. */
	newargc = 0;
	if ((*argc > 0) && (!IS_SUNMOS)) {
	    for(index = 0; index < *argc; index++) {
		
		if( strcmp(argv[index], "-pkt") == 0 ) {
		    if((++index >= *argc) || (isnumber(argv[index]) == 0)) {
			errno = EAINVALPKT;
			return(-1);
		    }
		    applinfo.pkt_size = atol(argv[index]);
		    pktflg = 1;
		} else if( strcmp(argv[index], "-mbf") == 0) {
		    if((++index >= *argc) || (isnumber(argv[index]) == 0)) {
			errno = EAINVALMBF;
			return(-1);
		    }
		    applinfo.memory_buffer = atol(argv[index]);
		    mbfflg = 1;
		} else if( strcmp(argv[index], "-mex") == 0) {
		    if((++index >= *argc) || (isnumber(argv[index]) == 0)) {
			errno = EAINVALMEX;
			return(-1);
		    }
		    applinfo.memory_export = atol(argv[index]);
		    mexflg = 1;
		} else if( strcmp(argv[index], "-mea") == 0) {
		    if((++index >= *argc) || (isnumber(argv[index]) == 0)) {
			errno = EAINVALMEA;
			return(-1);
		    }
		    applinfo.memory_each = atol(argv[index]);
		    meaflg = 1;
		} else if( strcmp(argv[index], "-sth") == 0) {
		    if((++index >= *argc) || (isnumber(argv[index]) == 0)) {
			errno = EAINVALSTH;
			return(-1);
		    }
		    applinfo.send_threshold = atol(argv[index]);
		    sthflg = 1;
		} else if( strcmp(argv[index], "-sct") == 0) {
		    if((++index >= *argc) || (isnumber(argv[index]) == 0)) {
			errno = EAINVALSCT;
			return(-1);
		    }
		    applinfo.send_count = atol(argv[index]);
		    sctflg = 1;
		} else if( strcmp(argv[index], "-gth") == 0) {
		    if((++index >= *argc) || (isnumber(argv[index]) == 0)) {
			errno = EAINVALGTH;
			return(-1);
		    }
		    applinfo.give_threshold = atol(argv[index]);
		    gthflg = 1;
		} else if( strcmp(argv[index], "-plk") == 0) {
		    applinfo.process_lock = 1;
		    plkflg = 1;
		} else if( strcmp(argv[index], "-nplk") == 0) {
		    applinfo.process_lock = 0;
		    plkflg = 1;
		} else if( strcmp(argv[index], "-pn") == 0) {
		    if (++index >= *argc) {
			errno = EINVAL;
			return(-1);
		    }
		    name = strdup(argv[index]);
		    pnflg = 1;
		} else if( strcmp(argv[index], "-sz") == 0) {
		    if (index+1 >= *argc) {
			/* no size specified */
			errno = EPBADNODE;
			return(-1);
		    }
		    /* see if we are specifying a rectangle */
		    if ( (strpbrk(argv[index+1],"x") != NULL) || 
			(strpbrk(argv[index+1],"X") != NULL)){ 
			n = parse_sdesc(argv[++index],
					(LP_MAP_T*) &node_parse_list);
			if (n == 2){
			    nx_info.rows = -node_parse_list[0];
			    nx_info.cols = -node_parse_list[1];
			    size = nx_info.rows * nx_info.cols;
			} else {
			    errno = EPBADNODE;
			    return(-1);
			}
		    } else
			if( isnumber(argv[index+1]) == 0 ) {
			    errno = EPBADNODE;
			    return(-1);
			} else {
			    size = atoi(argv[++index]);
    			if (size <= 0) {
				  errno = EPBADNODE;
				  return(-1);
			    }
                /* clear the rows/cols, as we have a cmd line spec'd size */
                nx_info.rows = nx_info.cols = 0;
			}
		} else if( strcmp(argv[index], "-nd") == 0) {
		    /* see if we are specifying a rectangle */
		    if ( ((strpbrk(argv[index+1],"x") != NULL) || 
			  (strpbrk(argv[index+1],"X") != NULL) &&
			  (strpbrk(argv[index+1],":") != NULL) )){ 
			n = parse_ndesc(argv[++index],
					(LP_MAP_T*) &node_parse_list);
			if (n == 3){
			    nx_info.rows = -node_parse_list[0];
			    nx_info.cols = -node_parse_list[1];
			    nx_info.start_node = -node_parse_list[2];
			    size = nx_info.rows * nx_info.cols;
			} else {
			    errno = EPBADNODE;
			    return(-1);
			}
		    }
		} else if( strcmp(argv[index], "-pri") == 0) {
		    if((++index >= *argc) || (isnumber(argv[index]) == 0)) {
			errno = EPINVALPRI;
			return(-1);
		    }
		    priority = atoi(argv[index]);
		} else if( strcmp(argv[index], "-noc") == 0) {
		    if((++index >= *argc) || (isnumber(argv[index]) == 0)) {
			errno = EPBADNODE;
			return(-1);
		    }
		    nnodes = atoi(argv[index]);
		    if (nnodes < 1) {
			errno = EINVAL;
			return -1;
		    }
		    nocflg = 1;
		} else if ( strcmp(argv[index], "-rlx") == 0) {
		    relaxed = 1;
		} else if ( strcmp(argv[index], "-nt") == 0) {
		    char* ntarg = argv[++index];
		    char* s;
		    if (index >= *argc) {
			errno = EINVAL;
			return -1;
		    }
		    selector_cnt =
			NX_analyse_selector_string(ntarg, &selector_buflen);
		    if (selector_cnt == -1)
			return -1;
		    if (selector_buf)
			FREE((void*) selector_buf);
		    selector_buf =
			(char*) MALLOC(selector_buflen+1);
		    s = selector_buf;
		    NX_parse_selector_string(ntarg, &s);
		} else {
		    argv[newargc] = argv[index];
		    newargc++;
		}
	    }
	}

	/* If no partition name was specified on the command line and
	 * the passed in name is null, the see if there is an NX_DFLT_PART
	 * defined
	*/
	if( (!pnflg) && (name == NULL) ) {
		name = (char *) MALLOC ( NAMELEN );
		if( name == (char *) 0) {
			errno = ENOMEM;
			perror("");
			exit(1);
		}
		tmpptr = (char *) getenv(partenv);
		if( tmpptr == NULL) {
			strcpy(name, ".compute");
		} else {
			name = tmpptr;
		}
	}

        /* Get MACS account to use */
        nx_info.macs_account = nx_getacctid();

	/* if no size was specified then check for NX_DFLT_SIZE in
	 * the environment
	*/
	if ( size == 0) {
		szptr = (char *) getenv(NX_DFLT_SIZE);
		if (szptr != NULL) 
			if (isnumber(szptr))
				size = atoi(szptr);
	}

	/* We must adjust the default memory sizes relative to the number
	   of nodes before proceeding with the rest of the applinfo stuff
	 */

	if (nocflg == 0 &&
	  !parse_for_attribute(NX_ATTR_NOC, NULL, ap)) {
		if (size == 0) {	/* use partition size */
		    if ( nx_get_partition_attributes(name,&part) < 0){
			return(-1);
		    }
		    nnodes = part.nodes;
		} else {
		    nnodes = size;
		}
	}

	applinfo.noc = nnodes;
	_mcmsg_noc = nnodes;

	if( verify_name(name) < 0 ) {
		return(-1);
	}

	dottoslash(name);

   
	/*
	 * If the name is not a full pathname, add '/etc/nx/compute'   
	 */

	if ((partname = (char *) create_partname(name)) == NULL) {
		return(-1);
	}

	/*
	 * Get inode number of the partition
	 */

	if(( inode = get_inode(partname)) <= 0 ) {
		errno = EPINVALPART;
		return(-1);
	}

	/* If we are passing any arguments to nx_initve then go ahead
	 * and modify argv
	*/
	if ((*argc > 0) && (!IS_SUNMOS)){
		*argc = newargc;
		if (argv != NULL) {
			argv[newargc] = (char *) 0;
		}
	}

	if(!plkflg && !parse_for_attribute(NX_ATTR_PLK, NULL, ap)) {
		applinfo.process_lock = PLKDFLT;
	}

	if(!pktflg && !parse_for_attribute(NX_ATTR_PKT, NULL, ap)) {
		applinfo.pkt_size = PKTDFLT;
	} else {
		applinfo.pkt_size = ROUNDUP(applinfo.pkt_size,sizeof(xmsg_t));
		if( check_range(applinfo.pkt_size, PKTMIN,
					PKTMAX) < 0 ) {
			errno = EAINVALPKT;
			return(-1);
		}
	}

	while(1) {
	
		if(!mbfflg &&
		  !parse_for_attribute(NX_ATTR_MBF, NULL, ap)) {
			applinfo.memory_buffer = MBFDFLT;
		} else {
			/* make memory_buffer even multiple of pkt_size */
			applinfo.memory_buffer =
			    ROUNDUP((applinfo.memory_buffer + PROVIDE_THRESHOLD)
									,XPKT);
			if(check_range( applinfo.memory_buffer, MBFMIN,
                                        MBFMAX) < 0 ) {
                                errno = EAINVALMBF;
                                return(-1);
			}
		}
	
		if(!mexflg &&
		  !parse_for_attribute(NX_ATTR_MEX, NULL, ap)) {
			/*
		 	* This must follow assignment of applinfo.memory_buffer
		 	* because MEXDFLT macro refers applinfo.memory_buffer.
		 	*/
			applinfo.memory_export = MEXDFLT;
			if(applinfo.memory_export < 0) {
				errno = EAINVALMBF;
				return(-1);
			}
		} else {
			/*
		 	* We need some minimal amount of memory_export. 
		 	*/
			if( check_range( applinfo.memory_export, MEXMIN,
					MEXMAX) < 0 ) {
				errno = EAINVALMEX;
				return(-1);
	
			}
		}
		if(!meaflg &&
		  !parse_for_attribute(NX_ATTR_MEA, NULL, ap)) {
			applinfo.memory_each = MEADFLT; 
		}
		applinfo.memory_each = ROUNDDOWN(applinfo.memory_each,
		                                 sizeof(xmsg_t));
	
		/* check to see if we are done with the computations */
		if( (applinfo.memory_each / 2) - PKTHDR >= applinfo.pkt_size) {
			break;
		}
		/* still don't have it right so try another iteration */
		if(pktflg ||
		  parse_for_attribute(NX_ATTR_PKT, NULL, ap)) {
			errno = EAINVALPKT;
			return(-1);
		}
		temp1 = applinfo.pkt_size;
		temp2 = ROUNDDOWN((applinfo.memory_each / 2 - PKTHDR),
		                   sizeof(xmsg_t));
		applinfo.pkt_size = MIN(temp1,temp2);
		if(applinfo.pkt_size < PKTMIN) {
			if (meaflg) {
				errno = EAINVALMEA;			
			} else {
				errno = EAINVALPKT;
			}
			return(-1);
		}
		if(applinfo.pkt_size == temp1) {
			errno = EAINVALPKT;
			return(-1);
		}
		if( check_range( applinfo.memory_each, MEAMIN,
				MEAMAX) < 0 ) {
			errno = EAINVALMEA;			
			return(-1);
		}
	} /* end while */

	if(!sthflg && !parse_for_attribute(NX_ATTR_STH, NULL, ap)) {
		applinfo.send_threshold =  STHDFLT;
	}
	applinfo.send_threshold = 
		ROUNDDOWN(applinfo.send_threshold,applinfo.pkt_size);
	if( check_range( applinfo.send_threshold,
			STHMIN, STHMAX) < 0 ) {
		errno = EAINVALSTH;			
		return(-1);
	}

	if(!sctflg && !parse_for_attribute(NX_ATTR_SCT, NULL, ap)) {
		applinfo.send_count = SCTDFLT;
	}
	applinfo.send_count = ROUNDDOWN(applinfo.send_count, applinfo.pkt_size);
	if( check_range(applinfo.send_count, 
			SCTMIN, SCTMAX) < 0 ) {
			errno = EAINVALSCT;
			return(-1);
	}

	if(!gthflg && !parse_for_attribute(NX_ATTR_GTH, NULL, ap)) {
		applinfo.give_threshold = GTHDFLT;
	}
	applinfo.give_threshold = 
		ROUNDDOWN(applinfo.give_threshold, applinfo.pkt_size);
	if( check_range( applinfo.give_threshold, GTHMIN,
			GTHMAX) < 0 ) {
		errno = EAINVALGTH;			
		return(-1);
	}

#ifdef DEBUG
printf("nx_initve partition name %s\n",name);
fflush(stdout);
#endif DEBUG

	applinfo.app = getpid();
	pgrp = getpgrp( applinfo.app );
/*
	if ( applinfo.app != pgrp ) {
		errno = EANOTPGL;
		return(-1);
	}
*/
	if (IS_SUNMOS)
	{
	/* The following parameters should not be relevant to a guest
	 *  O. S.  Use small or zero values to avoid unnecessary wired
	 *  pages.  Some of these have to be non-zero in order to keep
   	 *  the rest of the NX initialization code happy.
	 */
#ifndef SMALL_NONZERO
#define	SMALL_NONZERO	BIGPKT_BYTES	/* Arbitrary, size of 1 page */
#endif
		applinfo.process_lock = 0;
		applinfo.pkt_size = PKTDFLT;
		/* memory_buffer must be non-zero or won't work */
		applinfo.memory_buffer = SMALL_NONZERO;
		applinfo.memory_export = 0;
		applinfo.memory_each = 0;
		applinfo.send_threshold =  0;
		applinfo.send_count = 0;
		applinfo.give_threshold = 0;
	}

#ifdef DEBUG
printf("applinfo.app %ld \n",applinfo.app);
printf("applinfo.plock %ld \n",applinfo.process_lock);
printf("applinfo.pkt_size %ld \n",applinfo.pkt_size);
printf("applinfo.mwmory_buffer %ld \n",applinfo.memory_buffer);
printf("applinfo.memory_export %ld \n",applinfo.memory_export);
printf("applinfo.memory_each %ld \n",applinfo.memory_each);
printf("applinfo.send_threshold %ld \n",applinfo.send_threshold);
printf("applinfo.send_count %ld \n",applinfo.send_count);
printf("applinfo.give_threshold %ld \n",applinfo.give_threshold);
printf("applinfo.noc %ld \n",applinfo.noc);
printf("applinfo.rows %ld \n",applinfo.rows);
printf("applinfo.columns %ld \n",applinfo.columns);
fflush(stdout);
#endif

	nx_info.inode = inode;
	nx_info.size  = size;
	nx_info.priority = priority;
	nx_info.plk	= applinfo.process_lock;
	nx_info.relaxed = relaxed;

#ifdef DEBUG
printf("nx_info.inode: %d\n", nx_info.inode);                /* Inode of partition */
printf("nx_info.size: %d\n", nx_info.size);                   /* Number of nodes  */
printf("nx_info.priority: %d\n", nx_info.priority);           /* Priority of job */
printf("nx_info.rows: %d\n", nx_info.rows);                   /* Number of rows  */
printf("nx_info.cols: %d\n", nx_info.cols);                   /* Number of columns  */
printf("nx_info.start_node: %d\n", nx_info.start_node);       /* Starting node  */
printf("nx_info.plk: %d\n", nx_info.plk);                     /* -1 if plk spec'd on cmd line, 
                                                               *  0 otherwise  */
printf("nx_info.relaxed: %d\n", nx_info.relaxed);             /* Relaxed allocation requested  */
printf("nx_info.macs_account: %d\n", nx_info.macs_account);   /* MACS acnt to use for rqst  */
fflush(stdout);
#endif  /* DEBUG */

/*
 * Keep check_args_used happy
 */

	(void) parse_for_attribute(NX_ATTR_ACCT, NULL, ap);

	if (check_args_used() == -1)
		return -1;

	if(( numnodes =  _nx_init(&nx_info,&applinfo,NX_APP_VERSION,
	  selector_cnt,selector_buf,selector_buflen)) < 0 ) {

#ifdef	DEBUG
printf("-1 returned from _nx_init()\n");
#endif
		return(-1);
	}

	if (!IS_SUNMOS) {
		if(_init_mcmsg_intf() == -1) {
			return(-1);
		}
	} else {
	    /* For a Guest O. S., call setptype in order
	       to initialize the Post Page, even though we will not be doing
	       NX message passing.  PUMA message passing requires a Post
	       Page, and that's the only reason we call setptype.
	     */
		if (_setptype(getpid()) == -1) {
			return(-1);
		}
	}
	
#ifdef	DEBUG
printf("%d returned from _nx_init()\n", numnodes);
#endif
	  return(numnodes);
 
}

/*
 * catch_sigint(sig, code, scp, addr)
 *
 * Catch SIGINT,SIGTERM,SIGQUIT,SIGHUP and send it to the process group 
 * we are shepherding.
 * The parent will terminate when wait() returns.
 */
static void
catch_signal(sig, code, scp, addr)
int			sig;	/* parameters are ignored */
int			code;
struct sigcontext	*scp;
char			*addr;
{
	kill(-proxy_pid, sig);
}


/*
 * shepherd()		Watch over our flock of 1.
 *
 * Make the proxy process of the application a process group leader so
 * that we can signal the application as one unit.
 */
static int
shepherd()
{
	pid_t			save_fg_process;
	struct sigaction	new_sigaction;
	struct sigaction	old_sigaction;
	int			status;
	
	/*
	 * Catch SIGINT,SIGHUP,SIGTERM,SIGQUIT so that we can kill the 
	 * child process as well as
	 * this process when a Ctrl/c is done.
	 */
	new_sigaction.sa_handler = catch_signal;
	new_sigaction.sa_flags = SA_RESTART;
	sigemptyset(&new_sigaction.sa_mask);
	sigaction(SIGINT, &new_sigaction, &old_sigaction);

	new_sigaction.sa_handler = catch_signal;
	new_sigaction.sa_flags = SA_RESTART;
	sigemptyset(&new_sigaction.sa_mask);
	sigaction(SIGHUP, &new_sigaction, &old_sigaction);

	new_sigaction.sa_handler = catch_signal;
	new_sigaction.sa_flags = SA_RESTART;
	sigemptyset(&new_sigaction.sa_mask);
	sigaction(SIGTERM, &new_sigaction, &old_sigaction);

	new_sigaction.sa_handler = catch_signal;
	new_sigaction.sa_flags = SA_RESTART;
	sigemptyset(&new_sigaction.sa_mask);
	sigaction(SIGQUIT, &new_sigaction, &old_sigaction);

	/*
	 * Make proxy process a process group leader.
	 */
	setpgid(proxy_pid, proxy_pid);

	if (in_foreground) {
		/*
		 * We are in the foreground. Change the foreground process
		 * of the controlling terminal to that of the proxy process.
		 */
		save_fg_process = tcgetpgrp(0);
		tcsetpgrp(0, proxy_pid);
	}
        /*
         * Wait for proxy (application) to end.
         */
        while(waitpid(proxy_pid, &status, 0) != proxy_pid);
	/* If the proxy died because of a signal */
        if ( WIFSIGNALED(status) ) {
		/* see what signal it was */
                if (WTERMSIG(status) == SIGINT)
			/* Pass on sigint to the process group */
                	kill(-(getpgrp()), WTERMSIG(status));
        }

	/*
	 * If we were the foreground process then we must change the
	 * foreground process of the controlling terminal back to
	 * this process' pgroup. 
	 */
	if (in_foreground) {
		/*
		 * Calling tcsetpgrp() as a background process (which we
		 * now are) will cause a SIGTTOU, so we must ignore that
		 * signal for the duration of the call.
		 */
		new_sigaction.sa_handler = SIG_IGN;
		new_sigaction.sa_flags = SA_RESTART;
		sigemptyset(&new_sigaction.sa_mask);
		sigaction(SIGTTOU, &new_sigaction, &old_sigaction);
		tcsetpgrp(0, save_fg_process);
		sigaction(SIGTTOU, &old_sigaction, (struct sigaction *) 0);
	}

	/* pass on proxy exit status */
	if (WIFEXITED(status))
		exit(WEXITSTATUS(status));
	else
		exit(0);
}

void
ignore_sigmigrate()
{
	struct sigaction	new_sigaction;
	struct sigaction	old_sigaction;
	/*
	 * Ignore SIGMIGRATE 
	 */
	new_sigaction.sa_handler = SIG_IGN;
	sigaction(SIGMIGRATE, &new_sigaction, &old_sigaction);
}

