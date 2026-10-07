#
# embas4
#

AT=@

FC=if77
FFLAGS=-O4 -Mvect -Knoieee ${VERBOSE} ${SMP_READY}
LDFLAGS=-nx -Knoieee ${VERBOSE} ${SMP_READY}
SMP_READY=-Mconcur

SRCS=	embar2.28.f	randlc4.f	vranlc4.f	dyn_mem.h
OBJS=	embar2.28.o	randlc4.o	vranlc4.o
EXEC=	embas4

all:	${EXEC}

.f.o:
	${AT}${FC} -c ${FFLAGS} $<

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
