#
# Communication
#
# DEFINES:
#	DBG = -DDBG {for extra debug printout}
#	-DPARAGON {for OSF operating system}

AT=@

CC=icc
CFLAGS=${DBG} -DPARAGON -O3 -Mnodepchk -Knoieee ${SMP_READY} ${VERBOSE}
LDFLAGS=-lm ${SMP_READY} -nx ${VERBOSE}
SMP_READY=-Mconcur

SRCS=	comtest.c	comtest.h
OBJS=	comtest.o
EXEC=	comtest

all:	${EXEC}

${EXEC}:	${OBJS}
	${AT}${CC} -o ${EXEC} ${OBJS} ${LDFLAGS}

comtest.o:	${SRCS}
	${AT}${CC} -c ${CFLAGS} comtest.c

clean:
	${AT}path=`which ${CC}` ;	\
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
