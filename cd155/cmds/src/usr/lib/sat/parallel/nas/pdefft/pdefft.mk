#
# pdefft
#

AT=@

SHELL=/bin/sh

FC=if77
FFLAGS=-O3 -W0,-quad -Knoieee ${VERBOSE} ${SMP_READY}
LDFLAGS=-nx -Knoieee -lkmath ${VERBOSE} ${SMP_READY}
SMP_READY=-Mconcur

SRCS=	bfft.f		fft3d.f		global23.f	local23_sm.f    \
	vranlc.f	dyn_mem.h	fftpde.f	local23.f       \
	randlc.f

OBJS=	fftpde.o	fft3d.o		bfft.o		local23.o       \
	local23_sm.o	global23.o	vranlc.o	randlc.o

EXEC=	fftpde

.f.o:
	${AT}${FC} -c ${FFLAGS} $<

all:	${EXEC}

fftpde.o:	fftpde.f dyn_mem.h
	${AT}${FC} -c ${FFLAGS} fftpde.f

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
