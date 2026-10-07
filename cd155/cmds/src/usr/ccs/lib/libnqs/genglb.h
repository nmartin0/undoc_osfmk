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
 
/**********************************************************************
***********************************************************************
**                                                                   **
**  NOTICE:                                                          **
**                                                                   **
**  Significant portions of UniTree Release 1 software were          **
**  developed using private DISCOS funding, however, part of this    **
**  program resulted from work developed under a United States       **
**  Government contract and the following legend applies to the      **
**  government developed portions of the code.                       **
**                                                                   **
**  The Government retains for itself and others acting on its       **
**  behalf a paid-up, non-exclusive, irrevocable worldwide license   **
**  in such copyrighted data to reproduce, prepare derivative works, **
**  distribute copies to the public, and perform publicly and        **
**  display publicly, by or on behalf of the Government.             **
**                                                                   **
**  Neither the Government, nor any of its employees, makes any      **
**  warranty, expressed or implied, or assumes any legal liability   **
**  or responsibilty for the accuracy, completeness, or usefulness   **
**  of any information, apparatus, product, or process disclosed,    **
**  or represents that its use would not infringe privately owned    **
**  rights.                                                          **
**                                                                   **
***********************************************************************
**********************************************************************/

/**********************************************************************
**                                                                   **
**        (C) Copyright 1990, 1991  General Atomics                  **
**                    All Rights Reserved.                           **
**                                                                   **
**********************************************************************/

#if !defined(lint)
/* static char	sccs_id[] = "@(#)genglb.h	161.1 (UNITREE) 7/24/91"; */
#endif

/*****************************************************************************/
/*****************************************************************************/
/**									    **/
/**		     (C) Copyright 1986	the Regents of the		    **/
/**		University of California.  All Rights Reserved.		    **/
/**									    **/
/**		This work was produced under the sponsorship of		    **/
/**		 the U.S. Department of	Energy.	 The Government		    **/
/**			retains	certain	rights therein.			    **/
/**									    **/
/**				   Written by				    **/
/**				 Storage Group				    **/
/**			   Network Systems Division,			    **/
/**			    Computations Department,			    **/
/**		    Lawrence Livermore National	Laboratory.		    **/
/**									    **/
/**	 Contact Sam Coleman (415) 422-4323 for	additional information.	    **/
/**									    **/
/*****************************************************************************/
/*****************************************************************************/

/*
 *	This file contains generic global variables commonly used by servers.
 */

#include <stdio.h>

#define	NOERROR		0

#define	NoError		0

#define	TRUE		1
#define	FALSE		0

#define	YES		1
#define	NO		0

#define	DELAYSENDBIT	0

#define	ZEROWINDOWSIZE	0

#define	ANYADDRESS	(NetAddress *) NULL
#define	ANYSTREAM	(NetAddress *) NULL
#define	NULLPTR		(char *) NULL

#define	ONEMINUTE	60
#define	ONEDAY		(60 * 60 * 24)
#define	ONEWEEK		(7 * ONEDAY)

#if !defined( O_SYNC )
#define O_SYNC O_FSYNC
#endif

typedef	unsigned char	byte;

/*
 *	These routines are implemented in com/src/panic.c.  Access to the
 *	Panic routine is through the PANIC macro. Call it when you're at wit's
 *	end. The LogMessage routine accepts a string and prints it and the time
 *	to the DebugFile.
 */

void	LogMessage();
void	Panic();
void    Panic_2();

/*
 *	Handy defines for reducing lint	errors.
 */

#define	VOID	(void)

#define	FPRINTF	VOID fprintf
#define PANIC   Panic(module_name,__LINE__,Error)
#define PANIC_2(errmsg)   Panic_2(module_name,__LINE__,Error,errmsg)
#define	PRINTF	VOID printf
#define	SPRINTF	VOID sprintf

/*
 *	Defines	to cover up the	differences among UNIX implementations.
 */

/*
#if defined(sun) || defined(ultrix)
char *		sprintf();
#else
int		sprintf();
#endif
*/

#ifdef uts
void		exit();
#define	EXIT	exit
#else
#define	EXIT	VOID exit
#endif

#if defined(uts) || defined(__CRAY__)
char *	memcpy();
#define	MCOPY(d,s,n)	VOID memcpy(d,s,n)
#else
#ifdef ultrix
void	bcopy();
#endif
#define	MCOPY(d,s,n)	VOID bcopy(s,d,n)
#endif

#ifdef ultrix

#define	BigEndWord(ans,wordptr)	{				\
	char	*aptr,*cptr;					\
	int	i;						\
								\
	aptr = (char *)	&(ans);					\
	cptr = ((char *) (wordptr)) + sizeof(int64) - 1;	\
	for ( i	= 0; i < sizeof(int64);	i++ )			\
	   *aptr++ = *cptr--; }

#else

#define	BigEndWord(ans,wordptr)	{			\
	*((int64 *)&(ans)) =	*((int64 *) (wordptr));	}

#endif
