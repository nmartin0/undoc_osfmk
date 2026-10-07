##############################################################################
#  The BYTE UNIX Benchmarks - Release 3
#          Module: Makefile   SID: 3.9 5/15/91 19:30:15
#
##############################################################################
# Bug reports, patches, comments, suggestions should be sent to:
#
#	Ben Smith or Tom Yager at BYTE Magazine
#	bensmith@bytepb.byte.com    tyager@bytepb.byte.com
#
##############################################################################
#  Modification Log: 7/28/89 cleaned out workload files
#                    4/17/90 added routines for installing from shar mess
#                    7/23/90 added compile for dhrystone version 2.1
#                          {this is not part of Run file. still use old}
#                          removed HZ from everything but dhry.
#                          HZ is read from the environment, if not
#                          there, you must define it in this file
#                    10/30/90 moved new dhrystone into standard set
#                          new pgms {dhry included} run for a specified
#                          time rather than specified number of loops
#                    4/5/91 cleaned out files not needed for
#                          release 3 -- added release 3 files -ben
##############################################################################

AT=@

CC=icc
CFLAGS=-DTIME -O4 -Mvect ${SMP_READY} ${VERBOSE} 
#SMP_READY=-Mconcur #programs in this makefile don't benefit from this option
SHELL=/bin/sh

# local directories

PROGDIR=	./pgms
SRCDIR=		./src
TESTDIR=	./src

SCRIPTS=	cleanup.sh	fc.awk		fs.awk		multi.sh \
		tst.sh		index.sh	index.base	index.awk \
		loops.awk	loopm.awk	report.sh	report.awk

SOURCES=	arith.c		big.c		context1.c	execl.c \
		fstime.c	looper.c	timeit.c	dhry_1.c \
		dhry_2.c	dhry.h

TESTS=		sort.src	dc.dat

SATBINS=	${PROGDIR}/double	${PROGDIR}/fstime \
		${PROGDIR}/context1	${PROGDIR}/execl \
		${PROGDIR}/dhry2	${PROGDIR}/looper

REQD=	${SATBINS}		${PROGDIR}/cleanup.sh	${PROGDIR}/fs.awk \
	${PROGDIR}/multi.sh	${PROGDIR}/tst.sh	${PROGDIR}/index.sh \
	${PROGDIR}/index.base	${PROGDIR}/index.awk	${PROGDIR}/loops.awk \
	${PROGDIR}/loopm.awk	${PROGDIR}/report.sh	${PROGDIR}/report.awk \
	${TESTDIR}/sort.src	${TESTDIR}/cctest.c	${TESTDIR}/dc.dat

# ######################### the SAT ALL ############################
all:	${SATBINS}

# Individual programs

${PROGDIR}/double:	${SRCDIR}/arith.c
	${AT}${CC} ${CFLAGS} -o ${PROGDIR}/double -Ddatum=double ${SRCDIR}/arith.c

${PROGDIR}/fstime:	${SRCDIR}/fstime.c
	${AT}${CC} ${CFLAGS} -Dawk=1 -o ${PROGDIR}/fstime ${SRCDIR}/fstime.c 

${PROGDIR}/context1:	${SRCDIR}/context1.c
	${AT}${CC} ${CFLAGS} -o ${PROGDIR}/context1 ${SRCDIR}/context1.c

${PROGDIR}/execl:	${SRCDIR}/execl.c ${SRCDIR}/big.c
	${AT}${CC} ${CFLAGS} -o ${PROGDIR}/execl ${SRCDIR}/execl.c

${PROGDIR}/dhry2:	${SRCDIR}/dhry_1.c ${SRCDIR}/dhry_2.c ${SRCDIR}/dhry.h
	${AT}cd ${PROGDIR}; \
	${CC} ${CFLAGS} -DHZ=${HZ} -c ../${SRCDIR}/dhry_1.c ; \
	${CC} ${CFLAGS} -DHZ=${HZ} -c ../${SRCDIR}/dhry_2.c ; \
	${CC} ${CFLAGS} -o dhry2 dhry_1.o dhry_2.o ; \
	rm -f dhry_1.o dhry_2.o ; \
	cd ..

${PROGDIR}/looper:	${SRCDIR}/looper.c
	${AT}${CC} ${CFLAGS} -o ${PROGDIR}/looper ${SRCDIR}/looper.c

# Clean out binaries

clean:
	${AT}path=`which ${CC}` ;	\
	if [ -x $$path ] ;		\
        then				\
          remove=TRUE ;			\
        else				\
          remove=FALSE ;		\
        fi ;				\
        for file in ${SOURCES} ;	\
        do				\
          if [ ! -r ${SRCDIR}/$$file ] ;\
          then				\
            remove=FALSE ;		\
          fi ;				\
        done ;				\
        if [ $$remove = TRUE ] ;	\
        then				\
          rm -f ${SATBINS} ;		\
        fi
