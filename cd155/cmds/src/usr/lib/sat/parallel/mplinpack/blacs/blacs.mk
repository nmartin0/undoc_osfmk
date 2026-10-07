#
# MPP Linpack, blacs
#

AT=@

FC=if77
FFLAGS=-O3 ${SMP_READY} ${VERBOSE}
CC=icc
CFLAGS=${FFLAGS}
AR=ar860
SMP_READY=-Mconcur

SRCS=	blacsinit.c     dgerv2d.f       global.h        typegen.c  \
	xgesd.c		dgamax2d.f      dgesd2d.f       igebc2d.f  \
	xgebc.c         xgesds.c	dgamaxc.c       dgsum2d.f  \
	igerv2d.f       xgebcs.c	dgebc2d.f       dgsumc.c   \
	igesd2d.f       xgerv.c		dgebcs.f        global.c   \
	plamch2.c       xgervs.c

OBJS=	blacsinit.o	plamch2.o	dgebc2d.o	dgesd2d.o  \
	dgerv2d.o	igebc2d.o	igesd2d.o	igerv2d.o  \
        xgebc.o		xgerv.o		xgesd.o		dgebcs.o   \
        xgebcs.o	xgervs.o	xgesds.o	typegen.o  \
        dgsum2d.o	dgamax2d.o	dgsumc.o	dgamaxc.o

TARGET=	blacs.a

all:	${TARGET}

${TARGET}: ${OBJS}
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
