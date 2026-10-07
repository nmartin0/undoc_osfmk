#
# MPP Linpack, lapack
#

AT=@

FC=if77
FFLAGS=-O3 -Mvect ${SMP_READY} ${VERBOSE}
CC=icc
CFLAGS=${FFLAGS}
AR=ar860
SMP_READY=-Mconcur

SRCS=	dlacpy.f
OBJS=	dlacpy.o
TARGET=	lapack.a

all:	${TARGET}

${TARGET}:	${OBJS}
	${AT}rm -f ${TARGET} ; ${AR} cr ${TARGET} ${OBJS}

.f.o:
	${AT}${FC} -c ${FFLAGS} $<

.c.o:
	${AT}${CC} -c ${CFLAGS} $<

clean:
	${AT}c_path=`which ${CC}` ;				\
	ftn_path=`which ${FC}` ;				\
	ar_path=`which ${AR}` ;					\
	if [ -x $$c_path -a -x $$ftn_path -a -x $$ar_path ] ;	\
        then							\
          remove=TRUE ;						\
        else							\
          remove=FALSE ;					\
        fi ;							\
        for file in ${SRCS} ;					\
        do							\
          if [ ! -r $$file ] ;					\
          then							\
            remove=FALSE ;					\
          fi ;							\
        done ;							\
        if [ $$remove = TRUE ] ;				\
        then							\
          rm -f ${OBJS} ${TARGET} ;				\
        fi
