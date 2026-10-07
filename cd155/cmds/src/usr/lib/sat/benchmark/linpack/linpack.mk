#
# 100 x 100 Linpack Benchmark
#

AT=@

FC=if77         
FFLAGS=-O4 -Mvect=smallvect -Minline=daxpy -Knoieee ${SMP_READY} ${VERBOSE}
LDFLAGS=-lnx ${SMP_READY} ${VERBOSE}
SMP_READY=-Mconcur

SRCS=	linpack.f
OBJS=	linpack.o
EXEC=	linpack

all:	${EXEC}

linpack.o:	linpack.f
	${AT}${FC} ${FFLAGS} -c linpack.f

${EXEC}:	${OBJS}
	${AT}${FC} -o ${EXEC} ${OBJS} ${LDFLAGS}

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
