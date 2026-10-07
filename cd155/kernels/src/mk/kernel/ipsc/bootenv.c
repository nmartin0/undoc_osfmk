/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
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
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: bootenv.c,v $
 * Revision 1.14  1994/11/18  20:51:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/08/31  21:24:39  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.10.8.2  1994/08/10  20:34:09  flb
 * 	Reviewer: Self
 * 	Risk: low
 * 	Benefit or PTS #: added missing include file
 * 	Testing: Inspection
 * 	Module(s): ipsc/bootenv.c
 *
 * Revision 1.10.8.1  1994/08/10  16:20:31  flb
 *  Reviewer: terry
 *  Risk: low
 *  Benefit or PTS #:9976
 *  Testing: System Boot with large MAGIC.MASTER
 *  Module(s): ipsc/bootenv.c, mach/host_info.h, i860/ctrap.c, kern/machine.c
 *
 * Revision 1.10  1994/03/06  22:24:38  sean
 *  Reviewer:  steved
 *  Risk:  low
 *
 * Checking in these files which represent Steved's fixes for problems
 * in the following following areas:
 *
 * -fscan
 * -vdp clock skew
 * -bootmagic for processor mode
 *
 * ./ddb/db_command.c
 * ./ddb/db_print.c
 * ./i860/hardclock.c
 * ./i860paragon/mp.h
 * ./i860paragon/fscan.c
 * ./i860paragon/fscan.h
 * ./i860paragon/model_dep.c
 * ./i860paragon/mp_start.s
 * ./i860paragon/rpm.c
 * ./i860paragon/mp.c
 * ./i860paragon/interrupt.c
 * ./intel/pmap.c
 * ./ipsc/bootenv.c
 * ./kern/sched_prim.c
 * ./kern/mach_clock.c
 * ./vm/vm_pageout.c
 *
 * Revision 1.9  1994/02/16  19:08:46  stans
 *  Commented limitations in getbootstr().
 *  Added getbootstring() for writable bootmagic substrings.
 *
 *  Reviewer:stans
 *  Risk: low
 *  Benefit or PTS #: documented interfaces.
 *  Testing: developer
 *
 * Revision 1.8  1994/01/29  00:31:03  lenb
 *  Reviewer: mp3sw
 *  Risk: low
 *  Benefit or PTS #:
 * delete #ifdef MP3, lint.
 *
 * Revision 1.7  1994/01/19  17:17:20  steved
 * Added a new function called bootenvstr which works like bootenvint but with
 * strings.
 *
 *  Reviewer: Len Brown, Sean Griffen
 *  Risk: Low
 *  Benefit or PTS #: provide more flexibility in the bootmagic routines.
 *  Testing: SAT's (byte,benchmark), AIM3, crashme, pslaves
 *  Module(s): bootenv.c
 *
 * Revision 1.6  1993/10/13  22:48:37  terry
 * increased bootmagic buffer size to 8192 bytes. it was 4096 bytes.
 *
 * Revision 1.5  1993/06/30  22:43:29  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/04/27  20:34:15  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.3  1993/04/27  00:18:47  dleslie
 * Patch release of April 23
 *
 * Revision 2.5  1993/04/16  16:47:25  SSD
 * move node_in_list from ../i860/loose_ends.c
 *
 * Revision 2.4  1993/04/16  05:23:27  SSD
 * third code drop for page flow control fixes.
 *
 * Revision 2.3  1993/02/10  22:16:29  stans
 * SSD version T8
 *
 * Revision 2.2.2.2  92/09/15  17:21:10  jeffreyh
 * 	Intel changes.
 * 	[92/09/15  15:21:36  jeffreyh]
 * 
 * Revision 2.2.2.1  92/01/09  18:44:06  jsb
 * 	Enabled PASS_BOOT_MAGIC.
 * 	[92/01/08  16:38:08  jsb]
 * 
 * Revision 2.2  91/12/10  16:32:26  jsb
 * 	New files from Intel
 * 	[91/12/10  16:12:46  jsb]
 * 
 */

/*
 * andyp
 */

#if	PARAGON860
#include	<mach/host_info.h>
#define MAXENVSIZE	KERNEL_BOOTMAGIC_MAX	/* big systems have lots of bootmagic */
#else	PARAGON860
#define MAXENVSIZE	1024
#endif	PARAGON860

char	**bootenv = 0;
char	*node_in_list(int node, char *list);

extern char	*strchr(char *, char);
extern int	atoi();

/*
 *	Less space, less speed
 */
static int isspace(c)
char	c;
{
	return (((c) == ' ') || ((c) == '\t') || ((c) == '\n'));
}


/*
 * Save the Environment!
 *
 *	The pointer passed to us points to a single
 *	string with embedded newlines. eg:
 *
 *		char *bnv = "BOOT_MY_NODE=002\nBOOT_INIT_NODE=3\n...";
 *
 *	On Paragon nodes, it is possible for the environment to have
 *	spaces and/or tabs between var/value pairs.
 */
char **envcopy(bnv)
char *bnv;
{
	static char saveenv[MAXENVSIZE];
	char	**env, *src, *dst, c;
	int	i, len, cnt, nl, pspc;

	if (!bnv) {
		env = (char **) saveenv;
		env[0] = 0;
		bootenv = (char **) env;
		return bootenv;
	}

	/*
	 * count the length of all strings and the number of strings;
	 * don't count the whitespace between strings as characters
	 * for the copy but as a single line separator.
	 */
	len = nl = 0;
	src = bnv;
	while ((c = *src++)) {
		if (isspace(c))
			continue;
		len++;
		while ((c = *src++)) {
			if (isspace(c))
				break;
			len++;
		}
		nl++;
		if (c == 0)
			break;
	}

	/*
	 * sanity check
	 */
	pspc = (nl + 2) * sizeof(char *);
	if ((len + pspc) >= MAXENVSIZE) {
		/*
		 *	can't panic here -- don't always
		 *	have a console yet.  returning
		 *	null bootmagic is probably the
		 *	ethical thing to do.
		 */
		/*panic("envcopy: bootenv is too big");*/
		env = (char **) saveenv;
		env[0] = 0;
		bootenv = (char **) env;
		return bootenv;
	}


	/*
	 * copy the strings, substituting null for the
	 * first whitespace character; skipping additional
	 * whitespace as we go.
	 */
	env = (char **) saveenv;
	src = bnv;
	dst = &saveenv[pspc];
	cnt = 0;
	for (i = 0; i < nl; i++) {
		env[i] = dst;
		*dst = 0;
		while ((c = *src++) && isspace(c))
			;
		*dst++ = c;
		cnt++;
		while ((c = *src++) && (!isspace(c))) {
			*dst++ = c;
			cnt++;
		}
		*dst++ = 0;
	}
	env[i] = 0;

	if (cnt != len)
		panic("envcopy: cnt != len");

	bootenv = (char **) saveenv;
	return bootenv;
}


/*
 * funny string compare
 */
static char *upto(a, b, t)
char *a, *b, t;
{
	char	c1, c2;

	for (;;) {
		c1 = *a++;
		c2 = *b++;
		if ((c1 == t) && (c2 == 0))
			return a;
		if (c1 != c2)
			return (char *) 0;
		if ((c1 == 0) || (c2 == 0))
			return (char *) 0;
	}
	/*NOTREACHED*/
}


/*
 * similar to getenv()
 */
char *getbootenv(var)
char *var;
{
	char	*val, **env;

	for (env = bootenv; *env; ++env) {
		if ((val = upto(*env, var, '=')))
			return val;
	}

	return (char *) 0;
}

/*
 * For integers or booleans
 */

int getbootint(var, dflt)
	char	*var;
	int	dflt;
{
	char		*s, *cp;

	if ((s = getbootenv(var)) == 0) {
		return dflt;
	}
	/*
	 * Node list? "<2,3,4..5,22>33:<1,6,9>0"
	 */
	if ( (cp=strchr(s,'<')) ) {
		/* is my node number in the list? */
		if (
			(cp = node_in_list(node_self(),s)) == (char *)0
			|| cp == (char *)1
		) {
			/* not in a bracketed list, default */
			return dflt;
		}
		/*
		 * Find trailing '>' assoicated with my node number list.
		 * May be multiple node lists.
		 */
		if ( (cp=strchr(cp,'>')) )
			return atoi( ++cp );	/* return numeric value  */
		else
			return dflt;	/* default */
	}
	return atoi(s);	/* return numeric value  */
}

/*
 * return a pointer to a bootmagic string.
 *
 * WARNING: string is NOT writable as it will hose subsequent bootmagic
 *	    searches.
 *
 * Possible bug when the bootmagic string is expressed as multiple ranges,
 * what's returned is NOT a proper substring. If the bootmagic string is
 * "<2,3,4..5,22>str1:<1,6,9>str2" and one wants "str1" what you get back is
 * "str1:<1,6,9>str2".
 *
 * inputs:
 *	var	bootmagic string name
 *	dflt	default string ptr.
 *
 * outputs:
 *
 */

char *getbootstr(var, dflt)
        char    *var;
        char    *dflt;
{
        char	*s, *cp;

        if ((s = getbootenv(var)) == 0) {
                return dflt;
        }
        /*
         * Node list? "<2,3,4..5,22>str1:<1,6,9>str2"
         */
        if ( (cp=strchr(s,'<')) ) {
                /* is my node number in the list? */
                if (
			(cp = node_in_list(node_self(),s)) == (char *)0
			|| cp == (char *)1
		) {
			/* not in the list, default */
                        return dflt;
		}
                /*
                 * Find trailing '>' associated with my node number list.
                 * May be multiple node lists.
                 */
                if ( (cp=strchr(cp,'>')) )
                        return (++cp);  /* Return string */
                else
                        return dflt;    /* default */
        }
        return(s);      /* Return single value */
}


/*
 * For bootmagic strings which need a writable copy of the bootmagic
 * (sub)string.
 *
 * inputs:
 *	var		bootmagic name string
 *	dflt		default value string.
 *	result		result buffer address
 *	result_max	result buffer byte size.
 *
 * outputs:
 *	address of the result buffer containing the desired substring.
 */

char *getbootstring(var,dflt,result,result_max)
	char	*var;
	char	*dflt;
	char	*result;
	int	result_max;
{
	register char	*s, *cp;
	char		*start_addr;
	extern char	*node_in_list(), *strchr();
	extern int	atoi();

	strncpy(result,dflt,result_max);	/* setup default case */

	if ((s = getbootenv(var)) == 0) {
		return result;
	}
	/*
	 * Node list? "<2,3,4..5,22>string:<1,6,9>string"
	 */
	if ( (cp=strchr(s,'<')) ) {
		/* is my node number in the list? */
		if (
			(cp = node_in_list(node_self(),s)) == (char *)0
			|| cp == (char *)1
		) {
			/* not in a bracketed list, default */
			return result;
		}
		/*
		 * Find trailing '>' assoicated with my node number list.
		 * May be multiple node lists.
		 */
		if ( (cp=strchr(cp,'>')) )
			s = ++cp;	/* skip '>' */
		else
			return result;	/* default */
	}
	/*
	 * at this juncture 's' points to the start of the sub-string
	 * of interest. We check-for/remove the ':' sub-string terminator.
	 * Make sure the string is terminated correctly with a <null> byte.
	 * 'result_max' - 1 is the MAX size of the copied string; reserve
	 * space for the null byte.
	 *
	 * MK_PAGE_TO=<4>rz0e:<4,5,6..7>rz0f:<27..36>rz0b
	 */

	start_addr = result;

	while( ((*s != ':') && (--result_max) && (*result = *s)) ) {
		result++; s++;
	}
	*result = '\0';

	return	start_addr;
}

#define	PASS_BOOT_MAGIC	1

/*
 * Collapse the boot environment into one big string.
 */
char *ipsc_boot_environ()
{
	static char saveenv[MAXENVSIZE];
	char	*s, **ep = bootenv;
	int	len;

#if	PASS_BOOT_MAGIC
	s = saveenv;
	s[0] = 0;
	while (*ep) {
		len = strlen(*ep);
		strcpy(s, *ep);
		s[len] = '\n';
		s += (len + 1);
		s[0] = 0;
		ep++;
	}

	return saveenv;
#else	/* PASS_BOOT_MAGIC */
	return (char *) 0;
#endif	/* PASS_BOOT_MAGIC */
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
			list = strchr(list, ':');
			if (list == (char*)0) {
				return (0);
			}
		}

		list++;	/* next char after terminator */

		/* flush white space */
		while( *list == ' ' || *list == '\t' ) list++;
	}

	/* NOT in the list */
	return (char *)0;
}
