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
 * $Log: db_expr.c,v $
 * Revision 1.6  1994/11/18  20:29:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/01/29  00:29:14  lenb
 *  Reviewer: mp3sw
 *  Risk: low
 *  Benefit or PTS #:
 * ddb no longer requires a leading 0x on hex numbers
 * thanks sean
 *
 * Revision 1.4  1993/06/30  22:20:32  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:18:37  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:18:14  dleslie
 * First R1_0 release
 *
 * Revision 2.5  91/10/09  15:59:46  af
 * 	 Revision 2.4.3.1  91/10/05  13:06:04  jeffreyh
 * 	 	Added relational expression etc. to support condition expression.
 * 	 	Supported modifier after indirect expression to specify size,
 * 	 	  sign extention and non current task space indirection.
 * 	 	Changed error messages to print more information.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.4.3.1  91/10/05  13:06:04  jeffreyh
 * 	Added relational expression etc. to support condition expression.
 * 	Supported modifier after indirect expression to specify size,
 * 	  sign extention and non current task space indirection.
 * 	Changed error messages to print more information.
 * 	[91/08/29            tak]
 * 
 * Revision 2.4  91/05/14  15:33:45  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:06:25  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:17:46  mrt]
 * 
 * Revision 2.2  90/08/27  21:50:57  dbg
 * 	Use '..' instead of '$$' for db_prev.
 * 	Use '+' for db_next.
 * 	[90/08/22            dbg]
 * 
 * 	Allow repeated unary operators.
 * 	[90/08/20            dbg]
 * 
 * 	Reflected back rename of db_symbol_value->db_value_of_name
 * 	[90/08/20            af]
 * 	Reduce lint.
 * 	[90/08/07            dbg]
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */
#include <mach/boolean.h>
#include <machine/db_machdep.h>
#include <ddb/db_lex.h>
#include <ddb/db_access.h>
#include <ddb/db_command.h>
#include <kern/task.h>

/*
 * is_hex_number(char *s, db_expr_t *v)
 * called only when db_radix is 16, detects if the identifier "*s"
 * could really be a hex number.
 * return value: TRUE or FALSE
 * side effect: if TRUE, sets *v to appropriate number value 
 */
static boolean_t
is_hex_number(char *s, db_expr_t *v)
{
	int		value;
	char		c;
	int		r;
	int		digit;

	value = 0;

	for (c = *s; c; c = *++s) {
		if (c >= '0' && c <= '9') {
			digit = c - '0';
		} else if (c >= 'a' && c <= 'f') {
			digit = c - 'a' + 10;
		} else if (c >= 'A' && c <= 'F') {
			digit = c - 'A' + 10;
		} else {
			return (FALSE);
		}	
		value = value * 16 + digit;
	}
	*v = (db_expr_t)value;
	return (TRUE);
}

boolean_t
db_term(valuep)
	db_expr_t *valuep;
{
	int	t;

	switch(t = db_read_token()) {
	case tIDENT:
		{
			int	is_number = FALSE;
			int	is_symbol;

			if (db_radix == 16)
	    			is_number = is_hex_number(db_tok_string, valuep);

			is_symbol = db_value_of_name(db_tok_string, valuep);

			if (is_number && is_symbol) {
				/* ambiguous */
				db_printf(
					"'%s' ambiguous:  assuming symbol."
					"  Type '0x%s' for number.\n",
					db_tok_string,
					db_tok_string
				);
			} else if (!is_number && !is_symbol) {
				db_printf("Symbol \"%s\" not found\n",
					db_tok_string);
				db_error(0);
				/*NOTREACHED*/
			} 
			return (TRUE);
		}
	case tNUMBER:
	    *valuep = (db_expr_t)db_tok_number;
	    return (TRUE);
	case tDOT:
	    *valuep = (db_expr_t)db_dot;
	    return (TRUE);
	case tDOTDOT:
	    *valuep = (db_expr_t)db_prev;
	    return (TRUE);
	case tPLUS:
	    *valuep = (db_expr_t) db_next;
	    return (TRUE);
	case tQUOTE:
	    *valuep = (db_expr_t)db_last_addr;
	    return (TRUE);
	case tDOLLAR:
	    if (!db_get_variable(valuep))
		return (FALSE);
	    return (TRUE);
	case tLPAREN:
	    if (!db_expression(valuep)) {
		db_error("Unmached ()s\n");
		/*NOTREACHED*/
	    }
	    t = db_read_token();
	    if (t != tRPAREN) {
		db_printf("')' expected at \"%s...\"\n", db_tok_string);
		db_error(0);
		/*NOTREACHED*/
	    }
	    return (TRUE);
	default:
	    db_unread_token(t);
	    return (FALSE);
	}
}

int
db_size_option(modif, u_option, t_option)
	char *modif;
	boolean_t *u_option;
	boolean_t *t_option;
{
	register  char *p;
	int	  size = sizeof(int);

	*u_option = FALSE;
	*t_option = FALSE;
	for (p = modif; *p; p++) {
	    switch(*p) {
	    case 'b':
		size = sizeof(char);
		break;
	    case 'h':
		size = sizeof(short);
		break;
	    case 'l':
		size = sizeof(long);
		break;
	    case 'u':
		*u_option = TRUE;
		break;
	    case 't':
		*t_option = TRUE;
		break;
	    }
	}
	return(size);
}

boolean_t
db_unary(valuep)
	db_expr_t *valuep;
{
	register  char *p;
	int	  t;
	int	  size;
	boolean_t u_opt, t_opt;
	task_t	  task;
	extern	  task_t db_default_task;

	t = db_read_token();
	if (t == tMINUS) {
	    if (!db_unary(valuep)) {
		db_error("Expression syntax error after '-'\n");
		/*NOTREACHED*/
	    }
	    *valuep = -*valuep;
	    return (TRUE);
	}
	if (t == tSTAR) {
	    /* indirection */
	    if (!db_unary(valuep)) {
		db_error("Expression syntax error after '*'\n");
		/*NOTREACHED*/
	    }
	    task = TASK_NULL;
	    size = sizeof(int);
	    u_opt = FALSE;
	    t = db_read_token();
	    if (t == tIDENT && db_tok_string[0] == ':') {
		size = db_size_option(&db_tok_string[1], &u_opt, &t_opt);
		if (t_opt)
		    task = db_default_task;
	    } else
		db_unread_token(t);
	    *valuep = db_get_task_value((db_addr_t)*valuep, size, !u_opt, task);
	    return (TRUE);
	}
	if (t == tEXCL) {
	    if (!db_unary(valuep)) {
		db_error("Expression syntax error after '!'\n");
		/*NOTREACHED*/
	    }
	    *valuep = (!(*valuep));
	    return (TRUE);
	}
	db_unread_token(t);
	return (db_term(valuep));
}

boolean_t
db_mult_expr(valuep)
	db_expr_t *valuep;
{
	db_expr_t	lhs, rhs;
	int		t;
	char		c;

	if (!db_unary(&lhs))
	    return (FALSE);

	t = db_read_token();
	while (t == tSTAR || t == tSLASH || t == tPCT || t == tHASH
		|| t == tBIT_AND) {
	    c = db_tok_string[0];
	    if (!db_term(&rhs)) {
		db_printf("Expression syntax error after '%c'\n", c);
		db_error(0);
		/*NOTREACHED*/
	    }
	    switch(t) {
	    case tSTAR:
		lhs *= rhs;
		break;
	    case tBIT_AND:
		lhs &= rhs;
		break;
	    default:
		if (rhs == 0) {
		    db_error("Divide by 0\n");
		    /*NOTREACHED*/
		}
		if (t == tSLASH)
		    lhs /= rhs;
		else if (t == tPCT)
		    lhs %= rhs;
		else
		    lhs = ((lhs+rhs-1)/rhs)*rhs;
	    }
	    t = db_read_token();
	}
	db_unread_token(t);
	*valuep = lhs;
	return (TRUE);
}

boolean_t
db_add_expr(valuep)
	db_expr_t *valuep;
{
	db_expr_t	lhs, rhs;
	int		t;
	char		c;

	if (!db_mult_expr(&lhs))
	    return (FALSE);

	t = db_read_token();
	while (t == tPLUS || t == tMINUS || t == tBIT_OR) {
	    c = db_tok_string[0];
	    if (!db_mult_expr(&rhs)) {
		db_printf("Expression syntax error after '%c'\n", c);
		db_error(0);
		/*NOTREACHED*/
	    }
	    if (t == tPLUS)
		lhs += rhs;
	    else if (t == tMINUS)
		lhs -= rhs;
	    else
		lhs |= rhs;
	    t = db_read_token();
	}
	db_unread_token(t);
	*valuep = lhs;
	return (TRUE);
}

boolean_t
db_shift_expr(valuep)
	db_expr_t *valuep;
{
	db_expr_t	lhs, rhs;
	int		t;

	if (!db_add_expr(&lhs))
	    return (FALSE);

	t = db_read_token();
	while (t == tSHIFT_L || t == tSHIFT_R) {
	    if (!db_add_expr(&rhs)) {
		db_printf("Expression syntax error after \"%s\"\n",
			(t == tSHIFT_L)? "<<": ">>");
		db_error(0);
		/*NOTREACHED*/
	    }
	    if (rhs < 0) {
		db_error("Negative shift amount\n");
		/*NOTREACHED*/
	    }
	    if (t == tSHIFT_L)
		lhs <<= rhs;
	    else {
		/* Shift right is unsigned */
		lhs = (unsigned) lhs >> rhs;
	    }
	    t = db_read_token();
	}
	db_unread_token(t);
	*valuep = lhs;
	return (TRUE);
}

boolean_t
db_logical_relation_expr(valuep)
	db_expr_t *valuep;
{
	db_expr_t	lhs, rhs;
	int		t;
	char		op[3];

	if (!db_shift_expr(&lhs))
	    return(FALSE);

	t = db_read_token();
	while (t == tLOG_EQ || t == tLOG_NOT_EQ
		|| t == tGREATER || t == tGREATER_EQ
		|| t == tLESS || t == tLESS_EQ) {
	    op[0] = db_tok_string[0];
	    op[1] = db_tok_string[1];
	    op[2] = 0;
	    if (!db_shift_expr(&rhs)) {
		db_printf("Expression syntax error after \"%s\"\n", op);
		db_error(0);
		/*NOTREACHED*/
	    }
	    switch(t) {
	    case tLOG_EQ:
		lhs = (lhs == rhs);
		break;
	    case tLOG_NOT_EQ:
		lhs = (lhs != rhs);
		break;
	    case tGREATER:
		lhs = (lhs > rhs);
		break;
	    case tGREATER_EQ:
		lhs = (lhs >= rhs);
		break;
	    case tLESS:
		lhs = (lhs < rhs);
		break;
	    case tLESS_EQ:
		lhs = (lhs <= rhs);
		break;
	    }
	    t = db_read_token();
	}
	db_unread_token(t);
	*valuep = lhs;
	return (TRUE);
}

boolean_t
db_logical_and_expr(valuep)
	db_expr_t *valuep;
{
	db_expr_t	lhs, rhs;
	int		t;

	if (!db_logical_relation_expr(&lhs))
	    return(FALSE);

	t = db_read_token();
	while (t == tLOG_AND) {
	    if (!db_logical_relation_expr(&rhs)) {
		db_error("Expression syntax error after \"&&\"\n");
		/*NOTREACHED*/
	    }
	    lhs = (lhs && rhs);
	}
	db_unread_token(t);
	*valuep = lhs;
	return (TRUE);
}

boolean_t
db_logical_or_expr(valuep)
	db_expr_t *valuep;
{
	db_expr_t	lhs, rhs;
	int		t;

	if (!db_logical_and_expr(&lhs))
	    return(FALSE);

	t = db_read_token();
	while (t == tLOG_OR) {
	    if (!db_logical_and_expr(&rhs)) {
		db_error("Expression syntax error after \"||\"\n");
		/*NOTREACHED*/
	    }
	    lhs = (lhs || rhs);
	}
	db_unread_token(t);
	*valuep = lhs;
	return (TRUE);
}

int
db_expression(valuep)
	db_expr_t *valuep;
{
	return (db_logical_or_expr(valuep));
}
