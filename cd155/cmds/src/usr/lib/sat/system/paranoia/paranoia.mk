# 
# Paranoia
#

AT=@

FC=if77
FFLAGS=-O4 -Mvect ${SMP_READY} ${VERBOSE}
LDFLAGS=-lm ${SMP_READY} ${VERBOSE}
SMP_READY=-Mconcur

SRCS=	dpara.f		spara.f
OBJS=	dpara.o		spara.o
EXEC=	dpara		spara

all:	${EXEC}

dpara:		dpara.o
	${AT}${FC} -o dpara dpara.o ${LDFLAGS}

spara:		spara.o
	${AT}${FC} -o spara spara.o ${LDFLAGS}

.f.o:
	${AT}${FC} -c ${FFLAGS} $<

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
