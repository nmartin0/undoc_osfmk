#
# MPP Linpack, plapack
#

AT=@

FC=if77
FFLAGS=-O3 ${SMP_READY} ${VERBOSE}
CC=icc
CFLAGS=${FFLAGS}
AR=ar860
SMP_READY=-Mconcur

SRCS =	dzerovec.f      pdgemv.f        pdltsv.f        pdmnrm1.f    \
	pdtrsv.f	mypart.f        pdgeres.f       pdlubr.f     \
	pdrhsswap.f     pdutsv.f	pdasum.f        pdlaswp.f    \
	pdlur.f         pdscal.f        pdvredistr.f

OBJS = 	pdlubr.o	pdlur.o		pdtrsv.o	pdutsv.o     \
	pdltsv.o	pdgemv.o	pdgeres.o	pdasum.o     \
	pdscal.o	pdmnrm1.o	pdlaswp.o	pdrhsswap.o  \
       	mypart.o	pdvredistr.o	dzerovec.o

TARGET =	plapack.a

all:	${TARGET}

${TARGET}:	${OBJS} makefile
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
