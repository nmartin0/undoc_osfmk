#
# Level 2 BLAS
#

AT=@

FC=if77
FFLAGS=-c ${SMP_READY} ${VERBOSE}
LDFLAGS=-lkmath -Mquad -Knoieee -lnx ${SMP_READY} ${VERBOSE}
SMP_READY=-Mconcur


SRCS=	sblas2.f	dblas2.f	cblas2.f	zblas2.f  \
	cblas2.in	dblas2.in	sblas2.in	zblas2.in \
	/usr/ccs/lib/libkmath.a		/usr/ccs/lib/libkmath_r.a

OBJS=	sblas2.o	dblas2.o	cblas2.o	zblas2.o

EXECS=	sblas2.x	dblas2.x	cblas2.x	zblas2.x

SBLAS2=	sblas2.o

DBLAS2=	dblas2.o

CBLAS2=	cblas2.o

ZBLAS2=	zblas2.o

all:	single double complex complex16

single:		sblas2.x
double:		dblas2.x
complex:	cblas2.x
complex16:	zblas2.x

.f.o:
	${AT}${FC} ${FFLAGS} $<

sblas2.x: ${SBLAS2}
	${AT}${FC} ${SBLAS2} -o sblas2.x ${LDFLAGS}

dblas2.x: ${DBLAS2}
	${AT}${FC} ${DBLAS2} -o dblas2.x ${LDFLAGS}

cblas2.x: ${CBLAS2}
	${AT}${FC} ${CBLAS2} -o cblas2.x ${LDFLAGS}

zblas2.x: ${ZBLAS2}
	${AT}${FC} ${ZBLAS2} -o zblas2.x ${LDFLAGS}

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
          rm -f ${OBJS} ${EXECS} ;	\
        fi
