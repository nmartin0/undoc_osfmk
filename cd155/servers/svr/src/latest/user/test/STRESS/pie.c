/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *	Copyright (c) Locus Computing, 1991-92
 * 	This is UNPUBLISHED source code that is
 * 	the property of Locus Computing, containing
 *	proprietary secrets of LCC.  Any disclosure
 *	is strictly prohibited.  Locus makes no warantee,
 *	explicit or implicit, on the functionality of this code.
 */
/*
 * HISTORY
 * $Log: pie.c,v $
 * Revision 1.3  1994/11/18  20:57:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/07/14  18:54:26  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1  1992/12/02  21:34:01  dleslie
 * New bug drop from Locus, December 2, 1992
 *
 * Revision 1.1.1.2  1993/07/01  21:22:07  cfj
 * Adding new code from vendor
 *
 * Revision 3.0  92/11/23  11:44:33  chrisp
 * First slice.
 * 
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#define RAND_MAX (double) 0x7fffffff
#define FALSE		0
#define TRUE		1
#define SIGMIGRATE	32

extern void exit(int);

extern int errno;

int	aborted;
int	this_pid;
int	ignore_sigmigrate = FALSE;
char	*our_name;

void timestamp()
{
	time_t		time_secs;
	char		timestring[20];

	time(&time_secs);
	strftime(timestring, sizeof(timestring), "%T", localtime(&time_secs));
	printf("[%d] %s %s: ", this_pid, timestring, our_name);
}


void sigalarm_trap(int sig, int code)
{
	aborted = TRUE;
}

void sigfpe_trap(int sig, int code)
{
	printf("****** sigfpe trap code %d\n", code);
	exit(1);
}

void sigmigrate_trap(int sig, int code)
{
	if (ignore_sigmigrate)
		return;
/*
	migrate(code);
*/
}

main(int argc, char *argv[])
{
	extern char *optarg;
	extern int optind;
	char	ch;
	int	random_seed;
	int	timeout = 0;
	int	error = 0;
	int	use_monte_carlo_method = TRUE;
	unsigned int imax = 0xffffffff;
	unsigned int i = 0;
	struct	sigvec sigmig = {
                	(void(*))sigmigrate_trap, 0, 0};
	struct	sigvec sigalarm = {
                	(void(*))sigalarm_trap, 0, 0};
	struct	sigvec sigfpe = {
                	(void(*))sigfpe_trap, 0, 0};
	struct	sigvec sigint = {
                	(void(*))sigalarm_trap, 0, 0};
	struct itimerval timer;

	our_name = argv[0];
	this_pid = getpid();
	random_seed = 1;

	while ((ch = getopt(argc, argv, "hi:r:t:vV")) != EOF)
		switch (ch) {
		case 'h':
			use_monte_carlo_method = FALSE;
			break;
		case 'i':
			imax = atoi(optarg);
			break;
		case 'r':
			random_seed = atoi(optarg);
			break;
		case 't':
			timeout = atoi(optarg);
			break;
		case 'V':
			ignore_sigmigrate = TRUE;
		case 'v':
			if (sigvec(SIGMIGRATE, &sigmig, NULL)) {
				printf("sigvec gives errno=%d\n", errno);
				exit(1);
			}
			break;
		case '?':
		default:
			error++;
		}
	srand(random_seed);
	if (error) {
		printf("usage: pie [-i count | -t time]\n"
		       "           [-t secs] clock time to compute\n"
		       "           [-h] compute harmonic series\n"
		       "           [-v] sets a SIGMIGRATE signal handler\n"
		       "           [-V] ignore SIGMIGRATE signal\n"
		       "           [-r seed] set random seed\n"
		);
		exit(1);
	}


	if (sigvec(SIGFPE, &sigfpe, NULL)) {
		printf("sigvec for SIGFPE gives errno=%d\n", errno);
		exit(1);
	}
	if (sigvec(SIGALRM, &sigalarm, NULL)) {
		printf("sigvec() for SIGALRM gives errno=%d\n", errno);
		exit(1);
	}
	if (sigvec(SIGINT, &sigint, NULL)) {
		printf("sigvec for SIGINT gives errno=%d\n", errno);
		exit(1);
	}

	if (timeout != 0) {
		timer.it_interval.tv_sec = 0;
		timer.it_interval.tv_usec = 0;
		timer.it_value.tv_sec = timeout;
		timer.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &timer, 0);
	}
	
	if (use_monte_carlo_method)  {
		double hits = 0.0;
		double misses = 0.0;
		double x, y;
		while (!aborted && i < imax) {
			i++;
			x = (double) random() / RAND_MAX;
			y = (double) random() / RAND_MAX;
			if ((x*x + y*y) <= 1.0 )
				hits += 1.0;
			else
				misses += 1.0;
		}
		printf("%f after %u shots, (%f %f)\n",
			4.0 * hits / (hits + misses), i, hits, misses);
	} else {
		long int j = 2 * imax - 1;
		double sum = 0.0;
		while (!aborted && i < imax) {
			i++;
			if (i % 2 == 0)
				sum -= (1.0 / (double) j);
			else
				sum += (1.0 / (double) j);
			j -= 2;
		}
		printf("%f after %u terms\n", 4.0 * sum, i);
	}
	exit(0);
}
