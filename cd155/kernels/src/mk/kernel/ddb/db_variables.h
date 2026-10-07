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
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * HISTORY
 * $Log: db_variables.h,v $
 * Revision 1.6  1994/11/18  20:30:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/07/12  19:17:31  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.4.8.1  1994/02/15  03:12:52  stans
 * show args work
 *
 * Revision 1.4  1993/06/30  22:21:03  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:19:13  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:18:58  dleslie
 * First R1_0 release
 *
 * Revision 2.5  91/10/09  16:04:17  af
 * 	 Revision 2.4.3.1  91/10/05  13:08:42  jeffreyh
 * 	 	Added suffix related field to db_variable structure.
 * 	 	Added macro definitions of db_{read,write}_variable.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.4.3.1  91/10/05  13:08:42  jeffreyh
 * 	Added suffix related field to db_variable structure.
 * 	Added macro definitions of db_{read,write}_variable.
 * 	[91/08/29            tak]
 * 
 * Revision 2.4  91/05/14  15:37:12  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:07:23  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:19:54  mrt]
 * 
 * Revision 2.2  90/08/27  21:53:40  dbg
 * 	Modularized typedef name.  Documented the calling sequence of
 * 	the (optional) access function of a variable.  Now the valuep
 * 	field can be made opaque, eg be an offset that fcn() resolves.
 * 	[90/08/20            af]
 * 
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 */
/*
 * 	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */

#ifndef	_DB_VARIABLES_H_
#define	_DB_VARIABLES_H_

#include <kern/thread.h>

/*
 * Debugger variables.
 */
struct db_variable {
	char	*name;		/* Name of variable */
	int	*valuep;	/* value of variable */
				/* function to call when reading/writing */
	int	(*fcn)(/* db_variable, db_expr_t, int, db_var_aux_param_t */);
	short	min_level;	/* number of minimum suffix levels */
	short	max_level;	/* number of maximum suffix levels */
	short	low;		/* low value of level 1 suffix */
	short	high;		/* high value of level 1 suffix */
#define DB_VAR_GET	0
#define DB_VAR_SET	1
};

typedef struct db_variable	*db_variable_t;

#define	DB_VAR_NULL	(db_variable_t)0

#define	FCN_NULL	((int (*)())0)

#define DB_VAR_LEVEL	3	/* maximum number of suffix level */

#define db_read_variable(vp, valuep)	\
	db_read_write_variable(vp, valuep, DB_VAR_GET, 0)
#define db_write_variable(vp, valuep)	\
	db_read_write_variable(vp, valuep, DB_VAR_SET, 0)

/*
 * auxiliary parameters passed to a variable handler
 */
struct db_var_aux_param {
	char		*modif;			/* option strings */
	short		level;			/* number of levels */
	short		suffix[DB_VAR_LEVEL];	/* suffix */
	thread_t	thread;			/* target task */
};

typedef struct db_var_aux_param	*db_var_aux_param_t;
	

extern struct db_variable	db_vars[];	/* debugger variables */
extern struct db_variable	*db_evars;
extern struct db_variable	db_regs[];	/* machine registers */
extern struct db_variable	*db_eregs;

#endif	/* _DB_VARIABLES_H_ */
