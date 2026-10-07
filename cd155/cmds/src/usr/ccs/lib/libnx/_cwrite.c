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
 *      Copyright 1992  Intel Corporation.
 *
 *
 * CFS compatibility stub for synchronous write operation.
 *
 * HISTORY:
 * $Log: _cwrite.c,v $
 * Revision 1.2  1994/11/19  02:28:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/03/20  23:47:28  brad
 * Initial revision
 *
 */

int
_cwrite(fildes, buffer, nbytes)
int		fildes;		/* file descriptor */
char		*buffer;	/* pointer to the source buffer */
unsigned int	nbytes;		/* number of bytes to write */
{
	return(write(fildes, buffer, nbytes));
}
