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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991  Intel Corporation.
 * 
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/assert.h,v 1.3 1994/11/19 02:44:37 mtm Exp $
 *
 */

/*
 * assert.h
 *
 * Assert tool
 */
#define ASSERT 1
#define POSTMORTEM 0

#if ASSERT
#define assert(x) if (!(x)) assertfail(__LINE__, __FILE__);

static
assertfail(line, file)
	int	line;
	char	*file;
{

	printf("Pid %d assertion failed %s line %d\n", getpid(), file, line);
#if POSTMORTEM
	postmortem();
#endif POSTMORTEM
	exit(1);
}

#else ASSERT
#define assert(x)
#endif ASSERT

