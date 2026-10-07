#
# PFS SAT test makefile
#

AT=@

CC=icc
CFLAGS=-nx ${SMP_READY} ${VERBOSE}
LDFLAGS=-nx ${SMP_READY} ${VERBOSE}
SMP_READY=-Mconcur

SRCS=	pfs.c numrun.c
OBJS1=	numrun.o
OBJS2=	pfs.o
EXEC1=	numrun
EXEC2=	pfs

all:	${EXEC1} ${EXEC2}

pfs.o:		pfs.c
	${AT}${CC} -c ${CFLAGS} pfs.c

numrun.o:	numrun.c
	${AT}${CC} -c ${CFLAGS} numrun.c

${EXEC1}:	${OBJS1}
	${AT}${CC} -o ${EXEC1} ${OBJS1} ${LDFLAGS}

${EXEC2}:	${OBJS2}
	${AT}${CC} -o ${EXEC2} ${OBJS2} ${LDFLAGS}

clean:
	${AT}path=`which ${CC}` ;			\
	if [ -x $$path ] ;				\
        then						\
          remove=TRUE ;					\
        else						\
          remove=FALSE ;				\
        fi ;						\
        for file in ${SRCS} ;				\
        do						\
          if [ ! -r $$file ] ;				\
          then						\
            remove=FALSE ;				\
          fi ;						\
        done ;						\
        if [ $$remove = TRUE ] ;			\
        then						\
          rm -f ${OBJS1} ${OBJS2} ${EXEC1} ${EXEC2} ;	\
        fi
