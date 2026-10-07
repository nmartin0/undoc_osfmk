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
 *              Copyright (c) 1990 San Diego Supercomputer Center.
 *              All rights reserved.  The SDSC software License Agreement
 *              specifies the terms and conditions for redistribution.
 *
 * File:        macerr.h
 *
 * Abstract:
 *****************************************************************************/

#ifdef __STDC__
#   define  __(args) args
#else
#   define  __(args) ()
#   define const
#endif


#define ER_NONE			0
#define ER_BADAGID		1
#define ER_BADLOCK		2
#define ER_BADREC		3
#define ER_BADUID		4
#define ER_BADUNLOCK		5
#define ER_FAILOPEN		6
#define ER_FAILOPENC		7
#define ER_FAILOPENC_TMP	8
#define ER_FAILOPEN_TMP		9

#define ER_AGIDEXISTS		11
#define ER_ANAMEEXISTS		12


extern int	macerrno;

extern void	macerr __((void));
extern void	macerr_report __((const char *));
extern void	macerr_fatal __((const char *));
