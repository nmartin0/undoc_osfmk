#
# Slalom
#

AT=@

FC=if77
FFLAGS=-c -Knoieee ${VERBOSE} ${SMP_READY}
FFLAGS2=-O2 ${FFLAGS}
FFLAGS3=-O3 ${FFLAGS}
LDFLAGS=-nx -Knoieee -lkmath ${VERBOSE} ${SMP_READY}
SMP_READY=-Mconcur

SRCS=	dvdiv.f         meter.f         setup2.f        storer.f  \
	dvmul.f         reader.f        setup3.f        transf.f  \
	fixedt.f        region.f        solver.f        verify.f  \
	fixedt_tst.f    setup1.f        solver_pri.f    when.f    \
	dyn_mem.h       isc.h           msgtype.h	dinner.s  \
	/usr/ccs/lib/libkmath.a		/usr/ccs/lib/libkmath_r.a

OBJS=	fixedt.o	meter.o		reader.o	setup1.o  \
	setup2.o	setup3.o	solver.o	storer.o  \
	when.o		region.o	verify.o	dinner.o  \
	dvdiv.o		dvmul.o

EXEC=	slal

all:	${EXEC}

dinner.o:	dinner.s
	${AT}${FC} ${FFLAGS2} dinner.s

fixedt.o:	fixedt.f
	${AT}${FC} ${FFLAGS2} fixedt.f

meter.o:	meter.f
	${AT}${FC} ${FFLAGS2} meter.f

reader.o:	reader.f
	${AT}${FC} ${FFLAGS2} reader.f

setup1.o:	setup1.f
	${AT}${FC} ${FFLAGS2} setup1.f

setup2.o:	setup2.f
	${AT}${FC} ${FFLAGS2} setup2.f

setup3.o:	setup3.f
	${AT}${FC} ${FFLAGS2} setup3.f

solver.o:	solver.f
	${AT}${FC} ${FFLAGS2} solver.f

when.o:		when.f
	${AT}${FC} ${FFLAGS2} when.f

verify.o:	verify.f
	${AT}${FC} ${FFLAGS2} verify.f

storer.o:	storer.f
	${AT}${FC} ${FFLAGS2} storer.f

region.o:	region.f
	${AT}${FC} ${FFLAGS2} region.f

dvdiv.o:	dvdiv.f
	${AT}${FC} ${FFLAGS3} dvdiv.f

dvmul.o:	dvmul.f
	${AT}${FC} ${FFLAGS3} dvmul.f

${EXEC}:	${OBJS}
	${AT}${FC} -o ${EXEC} ${OBJS} ${LDFLAGS}

clean:
	${AT}path=`which ${FC}` ;	\
	if [ -x $$path ] ; 		\
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
