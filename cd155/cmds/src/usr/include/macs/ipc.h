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
 * File:        ipc.h
 *
 * Abstract:
 *****************************************************************************/
#ifdef __STDC__
#   define  __(args) args
#else
#   define  __(args) ()
#   define const
#endif

extern int	c_open __((void));
extern int	c_close __((void));
extern int	cpuctl __(());

