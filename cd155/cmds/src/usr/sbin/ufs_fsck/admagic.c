/*
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */
/*
 * Exported OSF/1 AD boot magic interfaces:
 *
 * get_magic(name)
 *	Retrieve the string associated with the specified boot magic variable
 *	name. if "BOOT_NOBOOT=maybe" the get_magic("BOOT_NOBOOT") would return
 *	the string "maybe".
 *
 * atoi_term(numeric,term)
 *	convert an ASCII string (decimal radix) to an integer and return a
 *	pointer to character which terminated the numeric scan.
 *
 * node_in_list(node,list)
 *	Is this node in the node-list string?
 *
 * meshnode_is_up(node)
 *	Verify the specified node is somewhat valid by searching the bootmagic
 *	'BOOT_NODE_LIST'. The server currently does NOT update this list,
 *	therefore it is ONLY a partially correct list in that nodes which are
 *	valid may have not booted correctly. "rfork()" catches these invalid
 *	nodes but all other filesystem operations will hang the system or
 *	timeout. We really need a server syscall which does the same validation
 *	as "rfork()". 
 *
 * getServerNode(deviceName)
 *	Retrieve the somewhat valid (node is in BOOT_NODE_LIST, rfork catches
 *	the bad nodes) node number of where the UNIX server resides which
 *	provides higher level device-service (like UNIX file-system semantics)
 *	for the specified device. DO not confuse device service with
 *	micro-kernel driver support; device service in this context implies
 *	where the higher level UX server processing is handled (see DEV_TAB).
 *
 * getbootint(bootmagic-string,default-value)
 *	return the integer value assocaiated with the specified boot magic
 *	string or the default value in the case of no string match.
 *
 * HISTORY:
 *
 * $Log: admagic.c,v $
 * Revision 1.10.2.1  1995/06/11  18:43:21  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.10  1995/03/08  00:32:05  stans
 *  meshnode_is_up() routine modified to get the bitvector status of all nodes
 *  instead of a MK syscall per node.
 *
 *  Reviewer:shane
 *  Risk:medium
 *  Benefit or PTS #:12106
 *  Testing:WW08 sats
 *
 * Revision 1.6.6.1  1994/08/10  18:29:50  flb
 * Reviewer: shane
 * Risk: low
 * Benefit or PTS #:9976
 * Testing: System Boot with large MAGIC.MASTER
 * Module(s):getmagic/kernops.c,bootmesh/kernops.c,bootmesh/bootmesh.h,cbs/kernops.c,mkdevtab/kernops.c,mkdevtab/bootmagic.h,inetd/getbootenv.c,load_level/src/osf1_dep.c,ufs_fsck/admagic.c
 *
 * Revision 1.6  1993/10/06  20:40:59  stans
 *    Fixed rtn: mesh_node_is_up() to use new 'norma_node_is_valid()' call.
 *    Replaced the erroneous approach to node verification of just matching
 *    a node in the BOOT_NODE_LIST.
 *
 * Revision 1.5  1993/09/09  22:28:03  stans
 *    Added #ifndef OSF1_ADFS then #define OSF1_ADFS 1 so sys/stat.h can find
 *    dst_node field in devstat structure.
 *
 * Revision 1.4  1993/09/08  21:44:22  shala
 * Numerous comment enhancements throughout.
 *
 * getServerNode() - use norma_node_self() Mach syscall instead of
 * TNC's node_self().
 *
 * getServerNode() correctly returns success when bootmagic 'DEV_TAB'
 * has an entry for a node. Missing "{}" on an if error clause.
 *
 * get_magic() returns a copy of the bootmagic string instead of
 * a pointer to the real bootmagic string. Previously, all
 * bootmagic strings after the first found string were lost to
 * subsequent calls to get_magic().
 *
 * Fixed bug #6506 by stans.
 *
 * Revision 1.3.6.1  1993/09/08  21:37:41  shala
 * Numerous comment enhancements throughout.
 *
 * getServerNode() - use norma_node_self() Mach syscall instead of
 * TNC's node_self().
 *
 * getServerNode() correctly returns success when bootmagic 'DEV_TAB'
 * has an entry for a node. Missing "{}" on an if error clause.
 *
 * get_magic() returns a copy of the bootmagic string instead of
 * a pointer to the real bootmagic string. Previously, all
 * bootmagic strings after the first found string were lost to
 * subsequent calls to get_magic().
 *
 * Fixed bug #6506 by stans.
 *
 */
#ifndef	OSF1_ADFS
#define	OSF1_ADFS 1	/* for sys/stat.h, dst_node */
#endif


#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
#include <mach/port.h>
#include <mach/error.h>
#include <mach/message.h>
#include <mach/norma_special_ports.h>

extern int	errno;

#ifndef	DEBUG
#define	DEBUG 0
#endif

#define MAX_BOOT_MAGIC_SIZE (1024*16)


#define	BOOTMAGIC_TRAP

#ifdef	BOOTMAGIC_TRAP	/* use BOOTMAGIC_TRAP in place of mach rpc */

int
bootmagic_trap(buffer)
    char    *buffer;
{
    asm("trap r0,r25,r0");
}

/*
 * Request bootmagic from microkernel
 */
get_boot_magic(bootmagic)
	char	*bootmagic;
{
    int 	boot_magic_len;

    boot_magic_len = bootmagic_trap(bootmagic);

    if (boot_magic_len > 0)
	return (KERN_SUCCESS);
    else
	return (KERN_FAILURE);
}


#else	/* use mach rpc in place of BOOTMAGIC_TRAP */

/*
 * Request bootmagic from microkernel
 */
get_boot_magic(bootmagic)
        char    *bootmagic;
{
	mach_port_t privileged_host_port;

	privileged_host_port = task_by_pid(-1);
	return host_get_boot_info(privileged_host_port,bootmagic);
}

#endif	/* use mach rpc in place of BOOTMAGIC_TRAP */

/*
 * get_magic(name) - Retrieve the string associated with the specified boot
 * magic variable name. if "BOOT_NOBOOT=maybe" then get_magic("BOOT_NOBOOT")
 * would return the string "maybe".
 */

char *
get_magic(name)
	char	*name;
{
	kern_return_t	kr;
	register char	*cp, *space, *newstring;
	static char	*magic=(char *)0;
	extern char	*malloc();

	if ( magic == (char *)0 ) {
		/* allocate a buffer for the magic strings */
		if ( (magic = malloc(MAX_BOOT_MAGIC_SIZE)) == (char *)0 ) {
			perror("get_magic() malloc() failed");
			return (char *)0;
		}

		/*
		 * ask MK for magic strings, substrings are newline delimited.
		 */
		if ( (kr=get_boot_magic(magic)) != KERN_SUCCESS ) {
			mach_error("host_get_boot_info() failed",kr);
			return (char *)0;
		}
	}

	/* find the specified sub-string */
	if ( (cp=strstr(magic,name)) == (char *)0 ) {
		return cp;
	}

	/*
	 * boot magic is really ONE large string with substrings being
	 * newline delimited. Temporarily force null-byte termination for
	 * the substring of interest.
	 */
	if ( (space=strchr(cp,'\n')) ) {
		*space = '\0';
	}
	/* skip over the '=', assumes magic as 'NAME=' */
	cp += strlen(name)+1;

	/*
	 * duplicate the sub-string of interest, then replace the newline
	 * with the original <newline>.
	 */
	newstring = strdup(cp);
	if ( space )
		*space = '\n';	/* undo the damage */
#if DEBUG
	printf("get_magic(%s) returns '%s'\n",name,newstring);
#endif
	/* return the copy */
	return newstring;
}


/*
 * convert an ASCII string (decimal radix) to an integer
 * inputs:
 *	p	string pointer.
 *	t	char **, return a pointer to the char which terminated the
 *		numeric string.
 * returns:
 *	integer value of the numeric string.
 * side effect:
 *	pointer to terminating char.
 */

atoi_term(p,t)
  register char *p;	/* IN */
  register char **t;	/* OUT */
{
        register int n;
        register int f;

        n = 0;
        f = 0;
        for(;;p++) {
                switch(*p) {
                case ' ':
                case '\t':
                        continue;
                case '-':
                        f++;
                case '+':
                        p++;
                }
                break;
        }
        while(*p >= '0' && *p <= '9')
                n = n*10 + *p++ - '0';

        /* return pointer to terminating character */
        if ( t )
                *t = p;

        return(f? -n: n);
}

/*
 * Is my node specified in this BOOT MAGIC node list?
 *
 *	example bootmagic strings:
 *		<0,3,10,15,22>1
 *		<0,3>0:<1>1
 *		0,2,4..31,33
 *
 * inputs:
 *	node	a node number
 *	list	BOOT MAGIC node list string ptr.
 *
 * outputs:
 *	!= 0	node is in the list:
 *		If value == 1 then node was NOT in a bracketed list
 *		otherwise value is a (char *) pointing at the char which
 *		terminated the node number atoi() scan. Example:
 *		alist="<25,27,29>4" node_in_list(27,alist); returns a ptr
 *		to the comma "," following 27. Idea here is to allow further
 *		processing to possibly pickup the real value of the bootmagic
 *		string "4".
 *
 *	 0 == NOT in the list.
 */

char *node_in_list( int node, char *list )
{
	register int	seen_brackets=0;
	int		num,num2;
	extern char	*strchr();
	extern int	atoi_term();

	/* make sure we have a valid string. */
	if ( list == (char *)0 ) {
		return( (char *)0 );
	}
	if ( *list == '\0' ) {
		return( (char *)0 );
	}

	/* scan the string looking for a match on my node number */
	while ( *list ) {
		/* skip over '<' if a bracketed list */
		if ( *list == '<' ) {
			seen_brackets++;
			list++;
		}

		/* in the list? */
		if ( (num=atoi_term(list,&list)) == node ) {
			return ( (seen_brackets ? list : (char *)1) );
		}

		/* EOS ? */
		if ( *list == '\0' )
			continue;

		/* flush white space */
		while( *list == ' ' || *list == '\t' ) list++;

		/* *list was the numeric terminator, range spec?
		 * ranges are assumed to be ascending order!!
		 */
		if ( *list == '.' && *(list+1) == '.' ) {
			list += 2;
			num2=atoi_term(list,&list);
			if ( num <= node && node <= num2 ) {
				return ( (seen_brackets ? list : (char *)1) );
			}
		}
		/* EOS ? */
		if ( *list == '\0' )
			continue;

		/* End of <nodes> spec ? */
		if ( *list == '>' ) {
			seen_brackets--;
			/* eat the value for the bracketed list */
			num2=atoi_term( (++list), &list );
			/* EOS ? */
			if ( *list == '\0' )
				continue;
		}

		list++;	/* next char after terminator */

		/* flush white space */
		while( *list == ' ' || *list == '\t' ) list++;
	}

	/* NOT in the list */
	return (char *)0;
}

/*
 * Verify the specified node is has booted by querying the MK.
 *
 * inputs:
 *	node	numeric node number.
 *
 * outputs: (boolean)
 *	TRUE == valid node which has booted correctly.
 *	FALSE == don't even think about it.
 */

#define BITS_PER_INT 32

/*
 * node_status defs which are not exported from the kernel in
 * 'norma/node_status.h'.
 */
typedef unsigned node_status_t[128];        /* 4096 node bitvector */

boolean_t
meshnode_is_up( unsigned nodeNum )
{
	static node_status_t	alive_nodes;
	static boolean_t	first_time=TRUE;
	kern_return_t		kr;
	boolean_t		UP;
        register unsigned       node_mask;
        register unsigned       *status;

	if ( first_time ) {
		bzero(alive_nodes,sizeof(alive_nodes));
		/*
		 * retrieve the bitvector of alive nodes from the MK.
		 */
		kr = norma_node_get_status( mach_host_self(), alive_nodes );
		if ( kr != KERN_SUCCESS ) {
			mach_error("norma_node_get_status()",kr);
			return FALSE;
		}
		first_time = FALSE;
	}

	/* convert nodeNum to bit-position-in-word mask */ 
        node_mask = (1 << (nodeNum % BITS_PER_INT));

	/* convert nodeNum into longword index */
        status = (unsigned *)&alive_nodes[ ( nodeNum / BITS_PER_INT) ];

	/* node UP(True) or DOWN(False) ? */
        return ((*status & node_mask) ? TRUE : FALSE);
}

/*
 * Retrieve the node number of where the UNIX server resides which provides
 * higher level device-service (like UNIX file-system semantics) for the
 * specified device. DO not confuse device service with micro-kernel driver
 * support; device service in this context implies where the higher level UX
 * server processing is handled (see DEV_TAB).
 *
 * inputs:
 *	device name
 *
 * outputs:
 *	If the node is NOT up then return (-1) and set errno to the bad
 *	node number.
 *	Otherwise, return the node number where device supporting UX server
 *	lives.
 */

getServerNode(name)
	char	*name;
{
	register char	*cp;
	int		node;
	char		*dev_tab;
        struct devstat	dvs;
	kern_return_t	kr;
	extern char	*node_in_list(), *strchr();
	extern int	atoi();

 	/* default to the local node */
	kr = norma_node_self(mach_task_self(), &node);
	if ( kr != KERN_SUCCESS ) {
		mach_error("norma_node_self()",kr);
		errno = node;
		return(-1);
	}

	/* retrieve service node number from the device inode */
        if ( devstat(name, &dvs) < 0 ) {
		perror("devstat()");	/* failed, stay local */
		return node;
	}

	/* how does one differentiate between 0 and a device which
	 * was mknod()'ed instead of rmknod()'ed.
	 */
	if (dvs.dst_node < 0) {
		return node;
	}

	node = dvs.dst_node;

	/*
	 * Verify the node in question has booted and is available to receive
	 * an rfork().
	 */
	if ( meshnode_is_up(node) == FALSE ) {
		errno = node;
		return(-1);	/* failure */
	}

	/*
	 * Check to see if there is a DEV_TAB mapping.
	 */
	if ((dev_tab = get_magic("DEV_TAB")) == (char *)0 ) {
		return	node;
	}

	/*
	 * Find my node in the DEV_TAB list? "<22>22:<6>9"
	 */
	if ( (cp=strchr(dev_tab,'<')) ) {
		/* is my node number in the list? */
		if ( (cp=node_in_list(node,dev_tab)) != (char *)0 ) {
			/*
			 * Find trailing '>' assoicated with my node number.
			 * May be multiple node lists.
			 */
			if ( (cp=strchr(cp,'>')) ) {
				node = atoi( ++cp );
				if ( meshnode_is_up( node ) == FALSE ) {
					errno = node;
					node = -1;	/* failure */
				}
			}
		}
	}
	return node;	/* return a valid/working node # */
}

/*
 * return the integer assocaiated with the specified boot magic string.
 *
 * inputs:
 *	name	string pointer to boot magic variable name
 *	default	value to return in the default case.
 *
 * outputs:
 *	decimal integer value from the bootmagic string or the default value
 *	in the case there was no match on the bootmagic string.
 */

getbootint( name, dflt )
	char	*name;
	int	dflt;
{
	char	*magicString;

	if ( (magicString=get_magic(name)) == (char *)0 )
		return dflt;

	return atoi(magicString);
}
