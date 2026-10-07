C
C***********************************************
      SUBROUTINE WHERE( mode)
C***********************************************
C
C  Prints Subroutine names in the active linkage chain for debugging.
C
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
      parameter( insert= 2 )
      COMMON /SPACES/ ion,j5,k2,k3,MULTI,laps,Loop,m,kr,LP,n13h,ibuf,nx,
     1 L,npass,nfail,n,n1,n2,n13,n213,n813,n14,n16,n416,n21,nt1,nt2,
     2 last,idebug,mpy,Loops2,mucho,mpylim, intbuf(16)
C
      CHARACTER  name*8, ISTACK*8
      COMMON /DEBUG/     ISTACK(20)
      COMMON /ORDER/ inseq, match, NSTACK(20), isave, iret
C
      made= MOD( mode,10)
                       name= 'internal'
      IF( made.EQ.1 )  name= ' ENTRY  '
      IF( made.EQ.2 )  name= ' RETURN '
      io= ABS( ion)
      IF( io.LE.0 .OR. io.GT.10 ) io=6
C
      IF( mode.EQ.12 ) THEN
           WRITE(  *,112)  ISTACK(20), ISTACK(1)
           WRITE( io,112)  ISTACK(20), ISTACK(1)
  112      FORMAT(2X,'WHERE: SEQ.ERROR.  RETURN ',A  ,'.NE. CALL ',A  )
      ENDIF
C
CPFM  IF( mode.EQ.20 ) THEN
CPFM       WRITE( io,9)
CPFM9      FORMAT(2X,'WHERE: INIPFM FAILED.' )
CPFM  ENDIF
      WRITE(  *,110)  name, ISTACK(1)
      WRITE( io,110)  name, ISTACK(1)
  110 FORMAT(/,' WHERE:  ERROR detected at ',A  ,' point in: ',A  )
C
      IF( made.EQ.1 .OR. made.EQ.2 )  THEN
C                    Pushdown stack of subroutine names and call nrs.
          DO  1  k = 12,insert+1,-1
          NSTACK(k)= NSTACK(k-insert)
          ISTACK(k)= ISTACK(k-insert)
    1     continue
C
          NSTACK(1)= inseq
          ISTACK(1)= 'WATCH   '
          NSTACK(2)= inseq
          ISTACK(2)= 'TRACE   '
          IF( made.EQ.2 )  ISTACK(2)= 'TRACK   '
      ENDIF
      WRITE(  *,111)
      WRITE(  *,114)
      WRITE(  *,113)
      WRITE(  *,114)
      WRITE(  *,118) ( ISTACK(k), NSTACK(k), k= 1,12 )
C
      WRITE( io,111)
      WRITE( io,114)
      WRITE( io,113)
      WRITE( io,114)
      WRITE( io,118) ( ISTACK(k), NSTACK(k), k= 1,12 )
  111 FORMAT(/,' ACTIVE SUBROUTINE LINKAGE CHAIN:')
  114 FORMAT('          ----           -----------')
  113 FORMAT('          name           call number')
  118 FORMAT(10X,A  ,4X,I8)
C
      DO 222 k= 1,200
      WRITE( io,221)
  221 FORMAT(/,' ********* TERMINAL ERROR; FLUSH I/O BUFFER **********')
  222 continue
      PAUSE
      STOP
c     RETURN
      END
