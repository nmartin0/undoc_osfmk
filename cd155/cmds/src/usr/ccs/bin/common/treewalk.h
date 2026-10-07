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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/* @(#)treewalk.h	1.3  com/cmd/prog/common,3.1,9013 9/15/89 19:40:58 */
/*
 * COMPONENT_NAME: (CMDPROG) treewalk.h
 *
 * FUNCTIONS: fwalk, tprint, walkf                                           
 *
 * ORIGINS: 27 03 09 32 00 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Changes for ANSI C were developed by HCR Corporation for IBM
 * Corporation under terms of a work made for hire contract.
 */

/* -------------------- fwalk -------------------- */

fwalk( t, f, down ) register NODE *t; int (*f)(); {

	int down1, down2;

	more:
	down1 = down2 = 0;

	(*f)( t, down, &down1, &down2 );

	switch( optype( t->in.op ) ){

	case BITYPE:
		fwalk( t->in.left, f, down1 );
		t = t->in.right;
		down = down2;
		goto more;

	case UTYPE:
		t = t->in.left;
		down = down1;
		goto more;

		}
	}

/* -------------------- walkf -------------------- */

walkf( t, f ) register NODE *t;  int (*f)(); {
	register opty;

	opty = optype(t->in.op);

	if( opty == UTYPE || opty == BITYPE) walkf( t->in.left, f );
	if( opty == BITYPE ) walkf( t->in.right, f );
	(*f)( t );
	}


/* -------------------- tprint -------------------- */
#ifndef PASS_TWO
# ifndef BUG4
tprint( t )
TPTR t;
{ /* output a nice description of the type of t */

	/* The ordering of this array corresponds to that in m_ind/manifest.h */
	static char * tnames[NBTYPES] = {
		"null",
		"ellipsis",
		"farg",
		"moety",
		"signed",
		"undef",
		"void",
		"char",
		"schar",
		"short",
		"int",
		"long",
		"float",
		"double",
		"ldouble",
		"strty",
		"unionty",
		"enumty",
		"uchar",
		"ushort",
		"unsigned",
		"ulong"
		};
	PPTR p;
	TWORD bt;

	for( ;; t = DECREF(t) ){

		if( ISCONST(t) ) printf( "const " );
		if( ISVOLATILE(t) ) printf( "volatile " );

		if( ISPTR(t) ) printf( "PTR " );
 		else if( ISFTN(t) ){
 			printf( "FTN (" );
			if( ( p = t->ftn_parm ) != PNIL ){
				for( ;; ){
					tprint( p->type );
					if( ( p = p->next ) == PNIL ) break;
					printf( ", " );
				}
 			}
			printf( ") " );
 		}
		else if( ISARY(t) ) printf( "ARY[%.0d] ", t->ary_size );
		else {
			if( ISTSIGNED(t) ) printf( "<signed> " );
			if( HASCONST(t) ) printf( "<HASCONST> " );
			if( HASVOLATILE(t) ) printf( "<HASVOLATILE> " );
			printf( tnames[bt = TOPTYPE(t)] );
			if( t->typ_size != bt ) printf( "(0%o)", t->typ_size );
			return;
		}
	}
}
# endif /* BUG4 */
# endif /* PASS_TWO */
