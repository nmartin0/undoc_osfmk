#
# Level 3 BLAS
#

AT=@

FC=if77
FFLAGS=-c ${SMP_READY} ${VERBOSE}
LDFLAGS=-lkmath -Mquad -Knoieee -lnx ${SMP_READY} ${VERBOSE}
SMP_READY=-Mconcur

SRCS=	sblas3.f	dblas3.f	cblas3.f	zblas3.f  \
	sblas3.in	dblas3.in	cblas3.in	zblas3.in \
	/usr/ccs/lib/libkmath.a		/usr/ccs/lib/libkmath_r.a

OBJS=	sblas3.o	dblas3.o	cblas3.o	zblas3.o

EXECS=	sblas3.x	dblas3.x	cblas3.x	zblas3.x

SBLAS3=	sblas3.o

DBLAS3=	dblas3.o

CBLAS3=	cblas3.o

ZBLAS3=	zblas3.o

all:	single double complex complex16

single:		sblas3.x
double:		dblas3.x
complex:	cblas3.x
complex16:	zblas3.x

.f.o:
	${AT}${FC} ${FFLAGS} $<

sblas3.x: ${SBLAS3}
	${AT}${FC} ${SBLAS3} -o sblas3.x ${LDFLAGS}

dblas3.x: ${DBLAS3}
	${AT}${FC} ${DBLAS3} -o dblas3.x ${LDFLAGS}

cblas3.x: ${CBLAS3}
	${AT}${FC} ${CBLAS3} -o cblas3.x ${LDFLAGS}

zblas3.x: ${ZBLAS3}
	${AT}${FC} ${ZBLAS3} -o zblas3.x ${LDFLAGS}

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
