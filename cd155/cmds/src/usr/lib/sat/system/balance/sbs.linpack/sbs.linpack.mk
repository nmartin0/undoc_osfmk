#
# 100 x 100 Linpack Benchmark used as part of the System Balance Sat
#

AT=@

FC=if77
FFLAGS=-O4 -Mvect=smallvect -Minline=daxpy -Knoieee ${SMP_READY} ${VERBOSE}
AR=ar860
SMP_READY=-Mconcur

SRCS=	linpack.f
OBJS=	linpack.o
TARGET=	linpack.a

all:	${TARGET}

${TARGET}:	${OBJS}
	${AT}rm -f ${TARGET}
	${AT}${AR} cr ${TARGET} ${OBJS}

linpack.o:	linpack.f
	${AT}${FC} -c ${FFLAGS} linpack.f

clean:
	${AT}ftn_path=`which ${FC}` ;		\
	ar_path=`which ${AR}` ;			\
	if [ -x $$ftn_path -a -x $$ar_path ] ;	\
        then					\
          remove=TRUE ;				\
        else					\
          remove=FALSE ;			\
        fi ;					\
        for file in ${SRCS} ;			\
        do					\
          if [ ! -r $$file ] ;			\
          then					\
            remove=FALSE ;			\
          fi ;					\
        done ;					\
        if [ $$remove = TRUE ] ;		\
        then					\
          rm -f ${OBJS} ${TARGET} ;		\
        fi
