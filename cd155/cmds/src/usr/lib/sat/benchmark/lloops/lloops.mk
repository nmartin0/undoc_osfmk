#
# FORTRAN Livermore LOOPS
#

AT=@

FC=if77
FFLAGS=-O4 -Mvect -Knoieee -Mnostride0 ${SMP_READY} ${VERBOSE}
LDFLAGS=-lnx -lm ${SMP_READY} ${VERBOSE}
SMP_READY=-Mconcur
SHELL=/bin/sh

SRCS=	block.f		calibr.f	indata.f	index.f \
	initio.f	iqran0.f	iqranf.f	kernel.f \
	main.f		page.f		relerr.f	report.f \
	result.f	second.f	secovt.f	sensit.f \
	seqdig.f	signel.f	simd.f		sizes.f \
	sordid.f	space.f		spedup.f	stats.f \
	statw.f		stops.f		sumo.f		supply.f \
	tally.f		tdigit.f	test.f		tests.f \
	tick.f		tile.f		trace.f		track.f \
	trap.f		trial.f		valid.f		values.f \
	verify.f	watch.f		where.f

OBJS=	block.o		calibr.o	indata.o	index.o	\
	initio.o	iqran0.o	iqranf.o	kernel.o \
	main.o		page.o		relerr.o	report.o \
	result.o	second.o	secovt.o	sensit.o \
	seqdig.o	signel.o	simd.o		sizes.o \
	sordid.o	space.o		spedup.o	stats.o \
	statw.o		stops.o		sumo.o		supply.o \
	tally.o		tdigit.o	test.o		tests.o	\
	tick.o		tile.o		trace.o		track.o \
	trap.o		trial.o		valid.o		values.o \
	verify.o	watch.o		where.o

EXEC=	lloops2

.f.o:
	${AT}${FC} ${FFLAGS} -c $<

all:	${EXEC}

${EXEC}:	${OBJS}
	${AT}${FC} ${FFLAGS} -o ${EXEC} ${OBJS} ${LDFLAGS}

clean:
	${AT}path=`which ${FC}` ;	\
	if [ -x $$path ] ;		\
        then				\
          remove=TRUE ;			\
        else				\
          remove=FALSE ;		\
        fi ;				\
        for file in ${SRCS} ;		\
        do				\
          if [ ! -r $$file ] ;		\
          then				\
            remove=FALSE ;		\
          fi ;				\
        done ;				\
        if [ $$remove = TRUE ] ;	\
        then				\
          rm -f ${OBJS} ${EXEC} ;	\
        fi
