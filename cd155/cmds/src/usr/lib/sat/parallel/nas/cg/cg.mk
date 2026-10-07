#
# Makefile for CG
#

AT=@

FC=if77
FFLAGS=-Knoieee -O3 ${SMP_READY} ${VERBOSE}
CC=icc
CFLAGS=-Knoieee -Mvect -O3 ${SMP_READY} ${VERBOSE}
LDFLAGS=-lkmath -nx ${SMP_READY} ${VERBOSE}
SMP_READY=-Mconcur

SRCS=	aitken.f                icnvrt.f                pmatvec1.f           \
	cg1.f                   init_matrix3.f          pmatvec_row_dsumx1.f \
	cmprss_ddoti.f          main.f                  randlc.f             \
	col_dcolx_bucket.f      makea.f                 sprnvc.f             \
	ddoti.f                 matvec.f		diagst.f             \
	matvec1.f		col_dcolx1.c            initgrid.c           \
	row_dsumx_matvec.c

OBJS=	main.o			aitken.o		init_matrix3.o 	   \
	cg1.o 			initgrid.o 		row_dsumx_matvec.o \
	col_dcolx1.o 		pmatvec1.o 		makea.o 	   \
	randlc.o 		matvec.o 		ddoti.o 	   \
	diagst.o 		sprnvc.o 		cmprss_ddoti.o 	   \
	icnvrt.o 		pmatvec_row_dsumx1.o	matvec1.o 

EXEC=	cg


all:	${OBJS} 
	${AT}${FC} ${FFLAGS} -o ${EXEC} ${OBJS} ${LDFLAGS}

row_dsumx_matvec.o:	row_dsumx_matvec.c matvec.f matvec1.f
	${AT}${CC} -c ${CFLAGS} -Minline=matvec,matvec1 row_dsumx_matvec.c

.f.o:
	${AT}${FC} -c ${FFLAGS} $<

.c.o:
	${AT}${CC} -c ${CFLAGS} $<

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
