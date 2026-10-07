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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
/*	%W%	*/
/*	%Q%	*/
/*	%G%	*/
/*	5.2 SID nan.h	1.3	*/
/* Handling of Not_a_Number's (only in IEEE floating-point standard) */

#define KILLFPE()	(void) kill(getpid(), 8)

union __NaN {
	double d;
	float f;
	long i[2];
	struct {
		unsigned dflow:32;
		unsigned dfhi:20;
		unsigned de:11;
		unsigned :1;
	} ds;
	struct {
		unsigned ff:23;
		unsigned fe:8;
		unsigned :1;
	} fs;
};
#define NaN(X)	((((union __NaN *)&X)->ds.de == 0x7ff))
#define NaD(X)	((((union __NaN *)&X)->ds.de == 0x7ff) || \
		((((union __NaN *)&X)->ds.de == 0) && \
		((((union __NaN *)&X)->i[0] != 0) || \
		((((union __NaN *)&X)->i[1]&0x7fffffff) != 0))))
#define NaF(X)	((((union __NaN *)&X)->fs.fe == 0xff) || \
		((((union __NaN *)&X)->fs.fe == 0) && \
		((((union __NaN *)&X)->i[0]&0x7fffffff) != 0)))
#define KILLNaN(X)	if (NaN(X)) KILLFPE()
