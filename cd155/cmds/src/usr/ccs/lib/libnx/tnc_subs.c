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
 
#ifdef TNC
#define OSF1_ADFS 1

#include	<stdio.h>
#include	<sys/types.h>
#include	<errno.h>
#include	<syslog.h>
#include	<unistd.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<net/if.h>
#include	<netinet/in.h>
#include	<netinet/in_var.h>
#include	<net/if_types.h>
#include 	<sys/ioctl.h>
#include	<cthreads.h>

/*
 * Name:
 *	tnc_clone_daemon()
 *
 * Function:
 *	Start a copy of the daemon process who calls us on
 *	every configured network server node in the system
 *	that is currently up.
 *
 * Inputs:
 *	None
 *
 * Outputs:
 *	See returns
 *
 * Returns:
 *	0	we're a child process
 *	1	we're the parent and everything worked
 *	-1	something failed. 
 */
int
tnc_clone_daemon()
{
	int	*nodes;
	int	*errnos;
	pid_t	*pids;
	int	numnodes;
	int	i,x;
	int	rc;
	pid_t	mypid;
	node_t  mynode;

	numnodes = 0;
	rc = 0;
	
	/*
	 * We want the rforkmulti() call to be relative
	 * to the entire machine, not just the partition
	 * that we happen to be running in.
	 */
	if (join_root_part() < 0) {
		syslog(LOG_ERR, "tnc_clone_daemon: join_root_part() failed");
		return(-1);
	}

	/* Get a list of all the network server nodes */
	if ( (rc = get_network_nodes(&nodes, &numnodes)) < 0 )  {
		syslog(LOG_ERR, "tnc_clone_daemon: get_network_nodes failed");
		return(-1);
	}
#ifdef DEBUG
	printf("tnc_clone_daemon: back from get_network_nodes\n");
#endif

	/* Allocate space for errnos and pids arrays */
	errnos = (int *)malloc(numnodes * sizeof(int));
	pids = (pid_t*)malloc(numnodes * sizeof(pid_t));
	if (pids == NULL || errnos == NULL) {
		syslog(LOG_ERR, "tnc_clone_daemon: malloc failed");
		return(-1);
	}

	/* Be fruitful and multiply */
	rc = rforkmulti(&numnodes, nodes, errnos, pids);

	/* We're a child */
	if (rc == 0) {
#ifdef DEBUG
		mypid = getpid();
		mynode = mypid >> 16;
	  	printf("tnc_clone_daemon: this kid is ok. pid %d node %d\n", 
			mypid, mynode);
#endif
		return(0);
	}

	/* We're the parent */
	else {
		free(nodes);
		free(errnos);
		free(pids);	  
		/* There were problems */
		if (rc < 0) {
			syslog(LOG_ERR, 
 		           "tnc_clone_daemon: not able to rfork all children");
			return(-1);
		}
		/* Everything is just fine */
		else {
#ifdef DEBUG
			printf("tnc_clone_daemon: parent aok\n");
#endif
			return(1);
		}
	}		
}
	
/*
 * Name:
 *	get_network_nodes
 *
 * Function:
 *	Return an array of int that lists all the
 *	configured network server nodes in the system
 *	that are up.
 *
 * Inputs:
 *	nodes		ptr to array of int
 *	cnt		ptr to int
 *
 * Outputs:
 *	nodes		filled in with list of network server nodes
 *	cnt		set to number of node #s returned in nodes array
 *
 * Returns:
 *	Returns 0 if successful, else -1
 */
#define BSIZE		4096

int
get_network_nodes(nodes, cnt)
int	**nodes;
int	*cnt;
{

	int			rc;
	int			i,x;
	int			s;
	int			nifr;
	int			bsize;
	int			node, num;
	int			already;
	int			*retnodes, *temp;
	struct ifreq		*ifr;
	struct ifconf		ifc;

	rc = num = 0;
	bsize = BSIZE;

	/* create a socket */
	if( (s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		syslog(LOG_ERR, "get_network_nodes: could not create socket");
		return(-1);
	}
again:
	/* malloc space to hold the returned data from GIFCONF call */
	if ( (ifr = (struct ifreq *)malloc(bsize)) == NULL ) {
		syslog(LOG_ERR, 
		       "get_network_nodes: malloc of ifreq blocks failed");
		return(-1);
	}
	bzero((char *)ifr, bsize);

	/* set up ifconf struct for SIOCGIFCONF */
	ifc.ifc_len = bsize;
	ifc.ifc_buf = (char *)ifr;

	/* make the call */
	if( (rc = (ioctl(s, SIOCGIFCONF, (char *)&ifc))) < 0 ) {
		syslog(LOG_ERR, "get_network_nodes: SIOCGIFCONF ioctl failed");
		close(s);
		return(rc);
	}

	/*
	 * ifc_len = # of bytes that we used. If we used ALL
	 * the bytes we allocated, we might have missed some
	 * data. Inc bsize and try again ...
	 */
	if (ifc.ifc_len == bsize) {
		bsize += BSIZE;
		free(ifr);
		goto again;
	}
	nifr = (ifc.ifc_len) / sizeof(struct ifreq);

#ifdef DEBUG
	printf("get_network_nodes: back from GIFCONF call. nifr %d\n", nifr);
#endif

	/*
	 * We're done with the socket so close it
	 */
	close(s);

	/*
	 * nifr >= the # of node number that we will be
	 * returning to the user.
	 */
	if ( (retnodes = (int *)malloc(nifr * sizeof(int))) == NULL ) {
		syslog(LOG_ERR, "get_network_nodes: 2nd malloc failed");
		return(-1);
	}
	else {
		*nodes = retnodes;
#ifdef DEBUG
		printf("get_network_nodes: retnodes 0x%x nodes 0x%x\n", 
			retnodes, *nodes);
#endif
	}		

	for (i = 0, ifr = ifc.ifc_req; i < nifr; i++, ifr++) {

		/*
		 * Now extract the node number
		 */
		if ( (node = get_node_number(ifr->ifr_name)) < 0) {
			syslog(LOG_ERR, "get_network_nodes: bad name");
			return(-1);
		}
		else {
			/*
			 * We don't want duplicates for the node
			 * numbers, so eliminate them here.
			 */
			for (x = 0, temp = *nodes, already = FALSE;
				x < num; x++, temp++) {
				if (*temp == node) {
					already = TRUE;
					break;
				}
			}
			if (!already) {	  
				*retnodes = node;
				num++;
				retnodes++;
			}
		}
	}

	*cnt = num;
#ifdef DEBUG
	printf("get_network_nodes: number of nodes returned = %d\n", *cnt);
	for (retnodes = *nodes, i = 0; i < *cnt; i++, retnodes++)
		printf("get_network_nodes: retnodes[%d] = %d\n", 
			i, *retnodes);	  
#endif
	free(ifr);
	return(0);
}


/*
 * Name:
 *	get_node_number
 *
 * Function:
 *	Take a device name as returned by a GIFCONF ioctl()
 *	call and extract the number of the node that supports
 *	the device. The device names are in the form
 *		<node number>interfacename
 *
 * Inputs:
 *	name -	Pointer to a device name	 
 *
 * Outputs:
 *	If successful, returns the node number, else
 *	returns -1
 */
int
get_node_number(devname)
char	*devname;
{
	char	*cp;
	int	i;

	cp = devname;

#ifdef DEBUG
	printf("get_node_number: devname %s\n", devname);
#endif
	/*
	 * Interface name starts off with a "<"
	 */
	if (*cp != '<') {
		syslog(LOG_ERR, "get_node_number: invalid interface name %s",
		       devname);
		return (-1);
	}
	else
		cp++;

	/*
	 * Next comes the node number followed by a ">"
	 */
	i = atoi(cp);
#ifdef DEBUG
	printf("get_node_number returns %d\n", i);
#endif
	return(i);
}
		
#endif /* TNC */
