# 
# 2D FFT
#

AT=@

FC=if77
FFLAGS=-O2 -W0,-quad -Knoieee ${SMP_READY} ${VERBOSE}
LDFLAGS=-Knoieee -lkmath -nx ${SMP_READY} ${VERBOSE}
SMP_READY=-Mconcur

SHELL=/bin/sh

SRCS=	bfft.f		dyn_mem.h	global23.f	driver2d.f \
	fft2d.f		local12.f 	/usr/ccs/lib/libkmath.a	   \
	/usr/ccs/lib/libkmath_r.a

OBJS=	driver2d.o	fft2d.o		bfft.o		local12.o  \
	global23.o

EXEC=	fft2d

.f.o:
	${AT}${FC} -c ${FFLAGS}  $<

all:	${EXEC}

${EXEC}:	${OBJS}
	${AT}${FC} -o ${EXEC} ${OBJS} ${LDFLAGS}

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
