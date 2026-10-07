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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_initve_.c,v 1.10 1994/11/19 02:31:43 mtm Exp $
 *
 */

#include <stdarg.h>
#include <nx.h>
#include <errno.h>

static char newname[256];
static char newacct[256];
extern int __argc_save;
extern char **__argv_save;

#define MAX_ARGS	40

#if 0

int
_nx_initve_(name, size, anchor_node, rows, cols,
		account, argc, argv, nlen, alen, clen, vlen) 
char	*name;		/* Partition name */
long	*size;		/* Partition Size */
long    *anchor_node;   /* Anchor node */
long    *rows;          /* number of rows */
long    *cols;          /* number of columns */
char	*account;	/* Account name */
int	*argc;		/* Argument length */
char	*argv[];	/* Arguments */
long	nlen;
long	alen;
long	clen;
long	vlen;
{
	int status;
	long rect[2];

	rect[0] = *rows;
	rect[1] = *cols;

	_copyname(name, newname, nlen, sizeof(newname));
	_copyname(account, newacct, alen, sizeof(newacct));

	/* If the user says to not process the arguments on the command line, 
	 * then just pass nx_initve() a bunch of NULL pointers
	*/
	if (*argc == 0){
 		if ((status = nx_initve_attr(newname, argc, argv,
			NX_ATTR_SZ, *size,
			NX_ATTR_ANCHOR, *anchor_node, 
			NX_ATTR_RECT, rect,
			NX_ATTR_ACCT, newacct,
			NX_ATTR_END)) == -1) {
        	}
	}
	else
		/* Otherwise pass in the saved command line arguments */
 		if ((status = nx_initve_attr(newname,
			&__argc_save, __argv_save,
			NX_ATTR_SZ, *size,
			NX_ATTR_ANCHOR, *anchor_node, 
			NX_ATTR_RECT, rect,
			NX_ATTR_ACCT, newacct,
			NX_ATTR_END)) == -1) {
        	}

	return status;
}

#endif

int
nx_initve_rect_(name, anchor_node, rows, cols, 
		account, argc, argv,nlen,alen,clen,vlen)
char    *name;         /* Partition name */
long    *anchor_node;    /* anchor node for rectangle*/
long    *rows;           /* Number of rows for rectangle */
long    *cols;           /* Number of cols for rectangls */
char    *account;       /* Account name */
int     *argc;          /* Argument length */
char    *argv[];        /* Arguments */
long	nlen;
long	alen;
long	clen;
long	vlen;
{
	int status;
	long rect[2];

	rect[0] = *rows;
	rect[1] = *cols;

	_copyname(name, newname, nlen, sizeof(newname));
	_copyname(account, newacct, alen, sizeof(newacct));

	/* If the user says to not process the arguments on the command line, 
	 * then just pass nx_initve() a bunch of NULL pointers
	*/
	if (*argc == 0){
 		if ((status = nx_initve_attr(newname, argc, argv,
			NX_ATTR_ANCHOR, *anchor_node, 
			NX_ATTR_RECT, rect,
			NX_ATTR_ACCT, newacct,
			NX_ATTR_END)) == -1) {
        	}
	}
	else
		/* Otherwise pass in the saved command line arguments */
 		if ((status = nx_initve_attr(newname,
			&__argc_save, __argv_save,
			NX_ATTR_ANCHOR, *anchor_node, 
			NX_ATTR_RECT, rect,
			NX_ATTR_ACCT, newacct,
			NX_ATTR_END)) == -1) {
        	}

	return status;
}

int
nx_initve_(name, size, account, 
	argc, argv, nlen, alen, clen, vlen) 
char	*name;		/* Partition name */
long	*size;		/* Partition Size */
char	*account;	/* Account name */
int	*argc;		/* Argument length */
char	*argv[];	/* Arguments */
long	nlen;
long	alen;
long	clen;
{
	int status;

	if (*size == -1) {
		errno = EPBADNODE;
		return -1;
	}
	
	_copyname(name, newname, nlen, sizeof(newname));
	_copyname(account, newacct, alen, sizeof(newacct));

	/* If the user says to not process the arguments on the command line, 
	 * then just pass nx_initve() a bunch of NULL pointers
	*/
	if (*argc == 0){
 		if ((status = nx_initve_attr(newname, argc, argv,
			NX_ATTR_SZ, *size,
			NX_ATTR_ACCT, newacct,
			NX_ATTR_END)) == -1) {
        	}
	}
	else
		/* Otherwise pass in the saved command line arguments */
 		if ((status = nx_initve_attr(newname,
			&__argc_save, __argv_save,
			NX_ATTR_SZ, *size,
			NX_ATTR_ACCT, newacct,
			NX_ATTR_END)) == -1) {
        	}

	return status;
}



int
nx_initve_attr_(char* name, int* argc, char** argv, ...)
{
	va_list ap;
	long nlen;
	long *arg;
	int status;
	long nArgs[MAX_ARGS];

        va_start(ap, argv);

        for (;;) {
            arg = va_arg(ap, long*);
            if (*arg == NX_ATTR_END)
                break;
            arg = va_arg(ap, long*);
        }
        nlen = va_arg(ap, long);
	_copyname(name, newname, nlen, sizeof(newname));

	va_start(ap, argv);

	if (_copyargs(ap, 1, &nArgs, MAX_ARGS) < 0) {
		errno = EINVAL;
		return -1;
	}

#if MAX_ARGS == 40
	if (*argc == 0)
	    status = nx_initve_attr(newname, argc, argv,
		nArgs[0], nArgs[1], nArgs[2], nArgs[3], nArgs[4],
		nArgs[5], nArgs[6], nArgs[7], nArgs[8], nArgs[9],
		nArgs[10], nArgs[11], nArgs[12], nArgs[13], nArgs[14],
		nArgs[15], nArgs[16], nArgs[17], nArgs[18], nArgs[19],
		nArgs[20], nArgs[21], nArgs[22], nArgs[23], nArgs[24],
		nArgs[25], nArgs[26], nArgs[27], nArgs[28], nArgs[29],
		nArgs[30], nArgs[31], nArgs[32], nArgs[33], nArgs[34],
		nArgs[35], nArgs[36], nArgs[37], nArgs[38], nArgs[39]);
	else
	    /* Otherwise pass in the saved command line arguments */
	    status = nx_initve_attr(newname,
		&__argc_save, __argv_save,
		nArgs[0], nArgs[1], nArgs[2], nArgs[3], nArgs[4],
		nArgs[5], nArgs[6], nArgs[7], nArgs[8], nArgs[9],
		nArgs[10], nArgs[11], nArgs[12], nArgs[13], nArgs[14],
		nArgs[15], nArgs[16], nArgs[17], nArgs[18], nArgs[19],
		nArgs[20], nArgs[21], nArgs[22], nArgs[23], nArgs[24],
		nArgs[25], nArgs[26], nArgs[27], nArgs[28], nArgs[29],
		nArgs[30], nArgs[31], nArgs[32], nArgs[33], nArgs[34],
		nArgs[35], nArgs[36], nArgs[37], nArgs[38], nArgs[39]);
#else
        fprintf(stderr, "In file %s, MAX_ARGS != 40\n", __FILE__);
        exit(1);
#endif


        return status;

}
	

