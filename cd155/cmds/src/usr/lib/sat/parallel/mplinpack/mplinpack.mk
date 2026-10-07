#
# makefile for MPP Linpack
#

AT=@

FC=if77
FFLAGS=-O3 -Knoieee ${SMP_READY} ${VERBOSE}
LDFLAGS=-Knoieee plapack/plapack.a blacs/blacs.a lapack/lapack.a -lkmath -nx ${SMP_READY} ${VERBOSE}
SMP_READY=-Mconcur

CC=icc
CFLAGS=${FFLAGS}
                
SRCS=	mainc.c		mainf0.f	matrix.f	vector.f \
	mainf.f		mainf1.f	rand.c			 \
	/usr/ccs/lib/libkmath.a		/usr/ccs/lib/libkmath_r.a

OBJS= 	mainf0.o	mainc.o		matrix.o	vector.o \
	rand.o 

EXEC=	mplinpack

SUBDIRS=	blacs		lapack		plapack

all:	subdirs ${EXEC}

subdirs:
	${AT}for dir in ${SUBDIRS} ;	\
	do				\
		cd $$dir ;		\
		${MAKE} all AT=$(AT) ;	\
		cd .. ;			\
	done

${EXEC}:	${OBJS} makefile
	${AT}${FC} -o ${EXEC} ${OBJS} ${LDFLAGS} 

.f.o:
	${AT}${FC} -c ${FFLAGS} $<

.c.o:
	${AT}${CC} -c ${CFLAGS} $<

clean:
	${AT}c_path=`which ${CC}` ;			\
	ftn_path=`which ${FC}` ;			\
	if [ -x $$c_path -a -x $$ftn_path ] ;		\
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
	  for dir in ${SUBDIRS} ;			\
	  do						\
	    cd $$dir ;					\
	    ${MAKE} clean AT=$(AT) ;			\
	    cd .. ;					\
	  done ;					\
          rm -f ${OBJS} ${EXEC} ;			\
        fi
