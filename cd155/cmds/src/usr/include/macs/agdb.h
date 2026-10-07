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
 
/*****************************************************************************
 *              Copyright (c) 1992 San Diego Supercomputer Center.
 *              All rights reserved.  The SDSC software License Agreement
 *              specifies the terms and conditions for redistribution.
 *
 * File:        agdb.h
 *
 * Abstract:
 *****************************************************************************/
#ifdef __STDC__
#   define  __(args) args
#else
#   define  __(args) ()
#   define const
#endif

/* agdb.c */
extern int	delagid __((int));
extern int	deluagid __((int, int));
extern int 	getuidfromagid __((int *, int));
extern int 	countagids __((void));
extern int 	getagidfromuid __((int [], int));
extern int 	addagid __((char *, int, char *));
extern int 	adduagid __((int, int));

