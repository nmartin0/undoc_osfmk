#
# Makefile for APPBT
#

AT=@

FC=if77

SRCS=	appbt.f           dirichlet.f       l2norm.f          xyTxz.f       \
	btadi.f           erhs.f            pintgr.f          xyTxz2.f      \
	btridxy1_kuck.f   error.f           rhs.f             xyTxzH.f      \
	btridxy2_kuck.f   exact.f           rhs2.f            xyTxzH_wait.f \
	btridzy1_kuck.f   jacx.f            rob.f             xyTxz_wait.f  \
	btridzy2_kuck.f   jacx2.f           setbviv.f         xzTyz.f       \
	btridzy3_kuck.f   jacy.f            updx.f            xzTyzH.f      \
	btverify.f        jacy2.f           updy2.f           xzTyzH_wait.f \
	compinv.f         jacz.f            updz.f            xzTyz_wait.f  \
	compnode.f        jacz2.f           updz2.f           yzTxy.f

EXEC=	bt64 bt128

all:	${EXEC}

bt64:
	${AT}cp appbt.include appbt.incl ;	\
	${MAKE} -f makebt ;			\
	mv bt bt64 ;				\
	rm *.o appbt.incl

bt128:
	${AT}sed 's/node_row = 1/node_row = 2/' appbt.include > appbt.incl ; \
	${MAKE} -f makebt ;						\
	mv bt bt128 ;							\
	rm *.o appbt.incl

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
	  rm -f ${EXEC} ;		\
	fi
