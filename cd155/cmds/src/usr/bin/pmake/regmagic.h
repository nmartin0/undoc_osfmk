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
 
/***************************************************************************
 *
 *    Copyright (c) 1992 Intel Corporation
 *
 *    This file is part of pmake, an extension of GNU Make.
 *    Pmake is distributed by the Intel Corporation under the
 *    terms of the GNU General Public License.
 *
 *    Title:
 *        $Id: regmagic.h,v 1.2 1994/11/19 01:34:02 mtm Exp $
 *
 *    Description:
 *        Regular expression definitions.
 *
 *    History: 
 * $Log: regmagic.h,v $
 * Revision 1.2  1994/11/19  01:34:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/04  22:05:02  raya
 * Initial checkin of pmake sources to the commands and libraries
 * tree.
 *
 *
 **************************************************************************/
/*
 * The first byte of the regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.
 */
#define	MAGIC	0234
