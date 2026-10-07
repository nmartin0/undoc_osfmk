#
# System Balance SAT
#

AT=@

CC=icc
CFLAGS=-O -nx ${VERBOSE}
LDFLAGS=-nx -Lsbs.linpack -llinpack -lf ${SMP_READY} ${VERBOSE}
SMP_READY=-Mconcur	

SUBDIRS=	sbs.linpack
SRCS=		balance.c
OBJS=		balance.o
EXEC=		balance

all:	subdirs ${EXEC}

balance.o:	balance.c
	${AT}${CC} -c ${CFLAGS} balance.c

${EXEC}:	${OBJS} subdirs
	${AT}${CC} -o ${EXEC} ${OBJS} ${LDFLAGS}

subdirs:
	${AT}for dir in ${SUBDIRS} ; \
        do                           \
                cd $$dir ;           \
                ${MAKE} all ;        \
                cd .. ;              \
        done

clean:
	${AT}for dir in ${SUBDIRS} ; \
	do                           \
		cd $$dir ;           \
		${MAKE} clean ;      \
                cd .. ;              \
	done
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
