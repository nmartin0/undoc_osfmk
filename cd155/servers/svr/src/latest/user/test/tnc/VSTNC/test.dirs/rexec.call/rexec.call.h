/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*******************************************************************************

Module:		rexec.call.h

Author:		Rex Black

Purpose:	This header defines the globals, constants and structures
		for the rexec test.


Modification History:

Date		Reason
--------	------

02/24/92	Began coding.


See Also:	

~rex/tstone/fvplans	This directory holds all the FV plans, including
		rexec.doc, which one should read before running or modifying
		this program.

*******************************************************************************/


/* Include files */
#include <ctype.h>        /* For operating on characters */
#include <stdio.h>        /* The usual stuff */
#include <errno.h>        /* To check the return */
#include "../common/vstnc.h"

/*	Constants	*/

#define NODE_ZERO	0  /* The system may have a valid node zero */
#define NODE_NEG_ONE   -1  /* The system may not have a valid negative node */	
#define NODE_NEG_OTH -999  /* The system may not have a valid negative node */
#define WIERD_NUM -696969  /* This unusual number used to initialize node so
			      that, if the program doesn't set it, the user
			      will notice. */

#define WAIT_REM_PROC  60  /* Wait one minute for the remote proc to start */

#define REXEC_ERR_CODE -1  /* rexec() should return this if it fails */

#define OK              0  /* Exit without error */
#define ENOTEST         1  /* Return this if the argument is invalid */
#define EINTERR       999  /* Return this if an internal error occurs */

/* Remote and local program paths */
#define REMOTE_PROG	"./rexec.call.remote"

/* The logical constants */
#define FALSE           0
#define TRUE            1

/*      Structures and Types */

/* The basic true/false type */
typedef short bool;


/*	Globals		*/

/* Internal error flag */

bool int_err = FALSE;

/* This gives the argv to pass to the remote program via rexec. */
char *rem_arg[] = {               
	"rexec.call.remote",	/* Note that the remote argument array      */
	"test case number",	/* provides verification data to the remote */
	"remote node number",	/* program that it needs to ensure that it  */
	"unused argument",	/* should be running in the first place and */
	"unused argument",	/* that it is executing on the right node.  */
	(char *)0
};

/* 
 * Macros for nodes used in test cases
 */
#define	SELF			0
#define	ONE_VALID_NODE		1
#define	ONE_INVALID_NODE	2
