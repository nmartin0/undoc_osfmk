#
# NAS Kernels
#

AT=@

FC=if77
FFLAGS=-O4 -Mvect -Mnostride0 -Knoieee -Mr8 -Mr8intrinsics ${SMP_READY} ${VERBOSE}
LDFLAGS=-lnx ${SMP_READY} ${VERBOSE}
SMP_READY=-Mconcur

SRCS=	nasker.f
OBJS=	nasker.o
EXEC=	nasker

all:	${EXEC}

nasker.o:	nasker.f
	${AT}${FC} -c ${FFLAGS} nasker.f

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
