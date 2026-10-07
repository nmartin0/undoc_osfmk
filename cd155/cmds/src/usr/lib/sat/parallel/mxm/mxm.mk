#
# Matrix Vector Multiply
#

AT=@

FC=if77
FFLAGS=-O3 -c -Knoieee ${SMP_READY} ${VERBOSE}
LDFLAGS=-nx -Knoieee -lkmath ${SMP_READY} ${VERBOSE}
SMP_READY=-Mconcur

SRCS=	mxm.f		sec.f		param.h			  \
	/usr/ccs/lib/libkmath.a		/usr/ccs/lib/libkmath_r.a
OBJS=	mxm.o		sec.o
EXEC=	mxm

all:	${EXEC}

mxm.o:		mxm.f param.h
	${AT}${FC} ${FFLAGS} -c mxm.f

sec.o:		sec.f
	${AT}${FC} ${FFLAGS} -c sec.f

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
